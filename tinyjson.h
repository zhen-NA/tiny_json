#ifndef __TINYJSON_H_
#define __TINYJSON_H_

typedef enum
{
    TINY_NULL,
    TINY_FALSE,
    TINY_TRUE,
    TINY_NUMBER,
    TINY_STRING,
    TINY_BUTT
} tiny_type;

typedef struct
{
    union
    {
        double n;
        struct
        {
            char *s;
            size_t len;
        } s;

    } u;
    tiny_type type;
} tiny_value;

enum
{
    TINY_PARSE_OK = 0,
    TINY_PARSE_EXPECT_VALUE,
    TINY_PARSE_INVALID_VALUE,
    TINY_PARSE_ROOT_NOT_SINGULAR,
    TINY_PARSE_NUMBER_TOO_BIG,
    TINY_PARSE_MISS_QUOTATION_MARK,
    TINY_PARSE_BUTT
};

#define tiny_init(v)           \
    do                         \
    {                          \
        (v)->type = TINY_NULL; \
    } while (0)

int tiny_parse(tiny_value *v, const char *json);

tiny_type tiny_get_type(const tiny_value *v);

void tiny_free(tiny_value *v);

void tiny_set_boolean(tiny_value *v, int b);
int tiny_get_boolean(tiny_value *v);

void tiny_set_number(tiny_value *v, double n);
double tiny_get_number(const tiny_value *v);

void tiny_set_string(tiny_value *v, const char *s, size_t len);
char *tiny_get_string(tiny_value *v);
size_t tiny_get_string_length(tiny_value *v);


#endif /* __TINYJSON_H_ */