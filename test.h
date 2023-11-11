#ifndef __TINY_JSON_TEST_H__
#define __TINY_JSON_TEST_H__

#include <stdio.h>
#include <string.h>
#include "tinyjson.h"

#define EXPECT_EQ_BASE(equality, expect, actual, format)                                                          \
    do                                                                                                            \
    {                                                                                                             \
        test_count++;                                                                                             \
        if (equality)                                                                                             \
            test_pass++;                                                                                          \
        else                                                                                                      \
        {                                                                                                         \
            fprintf(stderr, "%s:%d expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual); \
            main_ret = 1;                                                                                         \
        }                                                                                                         \
    } while (0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE(((expect) == (actual)), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")
#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == (alength) && memcmp(expect, actual, alength) == 0, expect, actual, "%s")

#define TEST_NUMBER(expect, json)                           \
    do                                                      \
    {                                                       \
        tiny_value v;                                       \
        EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, json)); \
        EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(&v));      \
        EXPECT_EQ_DOUBLE(expect, tiny_get_number(&v));      \
    } while (0)

#define TEST_ERROR(expect, json)                     \
    do                                               \
    {                                                \
        tiny_value v;                                \
        EXPECT_EQ_INT(expect, tiny_parse(&v, json)); \
        EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v)); \
    } while (0)

#define TEST_STRING(expect, json)                                                  \
    do                                                                             \
    {                                                                              \
        tiny_value v;                                                              \
        tiny_init(&v);                                                             \
        EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, json));                        \
        EXPECT_EQ_INT(TINY_STRING, tiny_get_type(&v));                             \
        EXPECT_EQ_STRING(expect, tiny_get_string(&v), tiny_get_string_length(&v)); \
        tiny_free(&v);                                                             \
    } while (0)

#endif /* __TINY_JSON_TEST_H__ */