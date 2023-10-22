#include "test.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

int main(int argc, char const *argv[])
{
    printf("result: %s, %d / %d, pass: %3.2f%%\n", main_ret == 0 ? "pass" : "fail", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
