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

static void test_parse_null()
{
    tiny_value v;
    int ret = TINY_PARSE_OK;
    v.type = TINY_NULL;
    ret = tiny_parse(&v, "null");
    EXPECT_EQ_INT(TINY_PARSE_OK, ret);
    EXPECT_EQ_INT(TINY_NULL, tiny_get_value(&v));
}

int main(int argc, char const *argv[])
{
    test_parse_null();
    printf("result: %s, %d / %d, pass: %3.2f%%\n", main_ret == 0 ? "pass" : "fail", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
