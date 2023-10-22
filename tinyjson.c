#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

#include "tinyjson.h"

#define is_whitspace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')
#define IS_DIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define IS_DIGIT_1TO9(ch) ((ch) >= '1' && (ch) <= '9')

typedef struct
{
    const char *json;
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
    assert('n' == *c->json);

    if (c->json[1] != 'u' || c->json[2] != 'l' || c->json[3] != 'l')
        return TINY_PARSE_INVALID_VALUE;

    c->json += 4; /* sizeof(null) */
    v->type = TINY_NULL;

    return TINY_PARSE_OK;
}

static int tiny_parse_true(tiny_context *c, tiny_value *v)
{
    assert('t' == c->json[0]);

    if (c->json[1] != 'r' || c->json[2] != 'u' || c->json[3] != 'e')
        return TINY_PARSE_INVALID_VALUE;

    c->json += 4; /* sizeof(true) */
    v->type = TINY_TRUE;
    return TINY_PARSE_OK;
}

static int tiny_parse_false(tiny_context *c, tiny_value *v)
{
    assert('f' == c->json[0]);

    if (c->json[1] != 'a' || c->json[2] != 'l' || c->json[3] != 's' || c->json[4] != 'e')
        return TINY_PARSE_INVALID_VALUE;

    c->json += 5; /* sizeof(false) */
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
    v->number = strtod(c->json, NULL);
    if (errno == ERANGE && (v->number == HUGE_VAL || v->number == -HUGE_VAL))
        return TINY_PARSE_NUMBER_TOO_BIG;

    v->type = TINY_NUMBER;
    c->json += n;
    return TINY_PARSE_OK;
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
    }
}

int tiny_parse(tiny_value *v, const char *json)
{
    tiny_context c;
    int ret;
    assert(NULL != v);

    /* 应保持值合法 */
    v->type = TINY_NULL;
    /** TODO： 接口设计角度，是否需要做这个检查？
     *  不做，脏值可能会影响结果
     *  是否应该作为调用者的要求？
     */
    if (NULL == json)
        return TINY_PARSE_INVALID_VALUE;
    c.json = json;
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
    return v->number;
}
