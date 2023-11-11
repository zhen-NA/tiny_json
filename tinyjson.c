#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "tinyjson.h"

/* 支持在Makefile中自定义该值 */
#ifndef TINY_PRASE_INIT_SIZE
#define TINY_PRASE_INIT_SIZE 256
#endif

#define is_whitspace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')
#define IS_DIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define IS_DIGIT_1TO9(ch) ((ch) >= '1' && (ch) <= '9')
#define EXPECTCH(c, ch)           \
    do                            \
    {                             \
        assert(*c->json == (ch)); \
        c->json++;                \
    } while (0)
#define PUSHC(c, ch)                                        \
    do                                                      \
    {                                                       \
        *(char *)tiny_context_push(c, sizeof(char)) = (ch); \
    } while (0)

typedef struct
{
    const char *json;
    char *stack;
    size_t size, top;
} tiny_context;

static void tiny_parse_whitspace(tiny_context *c)
{
    const char *p = c->json; /* const类型，这样写 "==" 的时候，就不用因为误写成 "=" 而拧巴了 */
    while (is_whitspace(*p))
        p++;
    c->json = p;
}

static int tiny_parse_null(tiny_context *c, tiny_value *v)
{
    EXPECTCH(c, 'n');

    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return TINY_PARSE_INVALID_VALUE;

    c->json += 3; /* strlen("null") -1 */
    v->type = TINY_NULL;

    return TINY_PARSE_OK;
}

static int tiny_parse_true(tiny_context *c, tiny_value *v)
{
    EXPECTCH(c, 't');

    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return TINY_PARSE_INVALID_VALUE;

    c->json += 3; /* strlen("true") - 1 */
    v->type = TINY_TRUE;
    return TINY_PARSE_OK;
}

static int tiny_parse_false(tiny_context *c, tiny_value *v)
{
    EXPECTCH(c, 'f');

    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return TINY_PARSE_INVALID_VALUE;

    c->json += 4; /* strlen("false") - 1 */
    v->type = TINY_FALSE;
    return TINY_PARSE_OK;
}

/* 返回数字域的长度，0表示失败*/
static int tiny_parse_check_number(tiny_context *c)
{
    const char *p = c->json;
    if (*p == '-')
        p++;
    if (*p == '0')
        p++;
    else
    {
        if (!IS_DIGIT_1TO9(*p))
            return 0;
        for (p++; IS_DIGIT_1TO9(*p); p++)
            ;
    }
    if (*p == '.')
    {
        p++;
        if (!IS_DIGIT(*p))
            return 0;
        for (p++; IS_DIGIT(*p); p++)
            ;
    }
    if (*p == 'e' || *p == 'E')
    {
        p++;
        if (*p == '+' || *p == '-')
            p++;
        if (!IS_DIGIT(*p))
            return 0;
        for (p++; IS_DIGIT(*p); p++)
            ;
    }

    return p - c->json;
}

static int tiny_parse_number(tiny_context *c, tiny_value *v)
{
    /* 数字域的长度 */
    int n = tiny_parse_check_number(c);
    if (!n)
        return TINY_PARSE_INVALID_VALUE;

    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
        return TINY_PARSE_NUMBER_TOO_BIG;

    v->type = TINY_NUMBER;
    c->json += n;
    return TINY_PARSE_OK;
}

static void *tiny_context_push(tiny_context *c, size_t size)
{
    /* 为什么是void *? */
    void *ret;
    assert(size > 0);
    if (c->top + size > c->size)
    {
        if (c->size == 0)
            c->size = TINY_PRASE_INIT_SIZE;
        while (c->top + size > c->size)
            c->size += c->size >> 1; /* 扩大1.5倍*/
        c->stack = realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void *tiny_context_pop(tiny_context *c, size_t size)
{
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}

static int tiny_parse_string(tiny_context *c, tiny_value *v)
{
    const char *p;
    size_t len, head = c->top;
    EXPECTCH(c, '\"');
    p = c->json;
    while (1)
    {
        char ch = *p++;
        switch (ch)
        {
        case '\"':
            /* 后续加入对象之后，缓冲区目前的top不一定是栈底 */
            len = c->top - head;
            tiny_set_string(v, (char *)tiny_context_pop(c, len), len);
            /* c->json最后要移动到结尾 */
            c->json = p;
            return TINY_PARSE_OK;
        case '\0':
            v->type = TINY_NULL;
            return TINY_PARSE_MISS_QUOTATION_MARK;
        default:
            PUSHC(c, ch);
        }
    }
}
static int tiny_parse_value(tiny_context *c, tiny_value *v)
{
    assert(!is_whitspace(*c->json));
    assert(NULL != v);
    switch (c->json[0])
    {
    case 'n':
        return tiny_parse_null(c, v);
    case 't':
        return tiny_parse_true(c, v);
    case 'f':
        return tiny_parse_false(c, v);
    case '\0':
        return TINY_PARSE_EXPECT_VALUE;
    default:
        /* number类型判断相对复杂，默认都按照数字处理，数字类型的检查放到数字的解析函数中完成 */
        return tiny_parse_number(c, v);
    case '\"':
        return tiny_parse_string(c, v);
    }
}

int tiny_parse(tiny_value *v, const char *json)
{
    tiny_context c;
    int ret;
    assert(NULL != v);

    /* 应保持值合法 */
    v->type = TINY_NULL;
    if (NULL == json)
        return TINY_PARSE_INVALID_VALUE;

    c.json = json;
    c.stack = NULL;
    c.size = c.top = 0;

    tiny_parse_whitspace(&c);
    if ((ret = tiny_parse_value(&c, v)) == TINY_PARSE_OK)
    {
        tiny_parse_whitspace(&c);
        if ('\0' != c.json[0])
        {
            ret = TINY_PARSE_ROOT_NOT_SINGULAR;
            v->type = TINY_NULL;
        }
    }
    return ret;
}

tiny_type tiny_get_type(const tiny_value *v)
{
    assert(NULL != v);
    return v->type;
}

double tiny_get_number(const tiny_value *v)
{
    assert(v != NULL && v->type == TINY_NUMBER);
    return v->u.n;
}

char *tiny_get_string(tiny_value *v)
{
    assert(v != NULL && v->type == TINY_STRING);
    return v->u.s.s;
}

size_t tiny_get_string_length(tiny_value *v)
{
    assert(v != NULL && v->type == TINY_STRING);
    return v->u.s.len;
}

void tiny_free(tiny_value *v)
{
    assert(v != NULL);
    if (v->type == TINY_STRING)
        free(v->u.s.s);
    v->type =TINY_NULL;
    return;
}

void tiny_set_string(tiny_value *v, const char *s, size_t len)
{
    assert(v != NULL && (s != NULL || len ==0));
    tiny_free(v);
    v->u.s.s = (char *)malloc(len + 1);
    memcpy(v->u.s.s, s, len);
    v->u.s.len = len;
    v->u.s.s[len] = '\0';
    v->type = TINY_STRING;
    return;
}
