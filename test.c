#include "test.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

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

static void test_expect_value()
{
    TEST_ERROR(TINY_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(TINY_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value()
{
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "V");

    /* invalid number */
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(TINY_PARSE_INVALID_VALUE, "nan");
}

static void test_parse_root_not_singular()
{
    TEST_ERROR(TINY_PARSE_ROOT_NOT_SINGULAR, "null x");

    /* invalid number */
    TEST_ERROR(TINY_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TEST_ERROR(TINY_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(TINY_PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_number_too_big()
{
    TEST_ERROR(TINY_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(TINY_PARSE_NUMBER_TOO_BIG, "-1e309");
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
    test_parse_number_too_big();
}

int main(int argc, char const *argv[])
{
    test_parse();
    printf("result: %s, %d / %d, pass: %3.2f%%\n", main_ret == 0 ? "pass" : "fail", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
