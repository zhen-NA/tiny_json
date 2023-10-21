#ifndef __TINYJSON_H_
#define __TINYJSON_H_

typedef enum
{
    TINY_NULL,
    TINY_FALSE,
    TINY_TRUE,
    TINY_NUMBER,
    TINY_BUTT
} tiny_type;

typedef struct
{
    tiny_type type;
    double number;
} tiny_value;

enum
{
    TINY_PARSE_OK = 0,
    TINY_PARSE_EXPECT_VALUE,
    TINY_PARSE_INVALID_VALUE,
    TINY_PARSE_ROOT_NOT_SINGULAR,
    TINY_PARSE_BUTT
};

int tiny_parse(tiny_value *v, const char *json);

tiny_type tiny_get_type(const tiny_value *v);

double tiny_get_number(const tiny_value *v);

#endif /* __TINYJSON_H_ */