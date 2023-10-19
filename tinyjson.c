#include <stdio.h>
#include <assert.h>

#include "tinyjson.h"

#define is_whitspace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')

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

static int tiny_parse_value(tiny_context *c, tiny_value *v)
{
    assert(!is_whitspace(*c->json));
    assert(NULL != v);
    switch (c->json[0])
    {
    case 'n':
        return tiny_parse_null(c, v);
    case '\0':
        return TINY_PARSE_EXPECT_VALUE;
    default:
        return TINY_PARSE_INVALID_VALUE;
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

tiny_type tiny_get_value(const tiny_value *v)
{
    assert(NULL != v);
    return v->type;
}
