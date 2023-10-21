#include <stdio.h>
#include "tinyjson.h"
static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

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

#define TEST_NUMBER(expect, actual)                           \
    do                                                        \
    {                                                         \
        tiny_value v;                                         \
        EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, actual)); \
        EXPECT_EQ_INT(TINY_NUMBER, tiny_get_type(&v));        \
        EXPECT_EQ_DOUBLE(expect, tiny_get_number(&v));        \
    } while (0)

static void test_parse_null()
{
    tiny_value v;

    v.type = TINY_FALSE; /* 初值应该与TINY_NULL不同 */
    EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, "null"));
    EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));
}

static void test_parse_true()
{
    tiny_value v;

    v.type = TINY_NULL;
    EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, "true"));
    EXPECT_EQ_INT(TINY_TRUE, tiny_get_type(&v));
}

static void test_parse_false()
{
    tiny_value v;

    v.type = TINY_NULL;
    EXPECT_EQ_INT(TINY_PARSE_OK, tiny_parse(&v, "false"));
    EXPECT_EQ_INT(TINY_FALSE, tiny_get_type(&v));
}

static void test_expect_value()
{
    tiny_value v;

    v.type = TINY_NULL;
    EXPECT_EQ_INT(TINY_PARSE_EXPECT_VALUE, tiny_parse(&v, ""));
    EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));

    v.type = TINY_NULL;
    EXPECT_EQ_INT(TINY_PARSE_EXPECT_VALUE, tiny_parse(&v, " "));
    EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));
}

static void test_parse_invalid_value()
{
    tiny_value v;

    v.type = TINY_NULL;
    EXPECT_EQ_INT(TINY_PARSE_INVALID_VALUE, tiny_parse(&v, "nul"));
    EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));

    v.type = TINY_NULL;
    EXPECT_EQ_INT(TINY_PARSE_INVALID_VALUE, tiny_parse(&v, "v"));
    EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));
}

static void test_parse_root_not_singular()
{
    tiny_value v;

    v.type = TINY_NULL;
    EXPECT_EQ_INT(TINY_PARSE_ROOT_NOT_SINGULAR, tiny_parse(&v, "null x"));
    EXPECT_EQ_INT(TINY_NULL, tiny_get_type(&v));
}

static void test_parse_number()
{
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}

static void test_parse()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number();

    test_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main(int argc, char const *argv[])
{
    test_parse();
    printf("result: %s, %d / %d, pass: %3.2f%%\n", main_ret == 0 ? "pass" : "fail", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
