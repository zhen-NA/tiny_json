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
    assert(NULL != v);
    c.json = json;
    v->type = TINY_BUTT;
    tiny_parse_whitspace(&c);
    return tiny_parse_value(&c, v);
}

tiny_type tiny_get_value(const tiny_value *v)
{
    assert(NULL != v);
    return v->type;
}
