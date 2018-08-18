#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "myjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")
#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == (alength) && memcmp(expect, actual, alength) == 0, expect, actual, "%s")

#define TEST_ERROR(error, json)\
    do {\
        json_value v;\
        v.type = JSON_FALSE;\
        EXPECT_EQ_INT(error, json_parse(&v, json));\
        EXPECT_EQ_INT(JSON_NULL, json_get_type(&v));\
    } while(0)

#define TEST_NUMBER(expect, json)\
    do {\
        json_value v;\
        EXPECT_EQ_INT(JSON_PARSE_OK, json_parse(&v, json));\
        EXPECT_EQ_INT(JSON_NUMBER, json_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect, json_get_number(&v));\
    } while(0)

#define TEST_STRING(expect, json)\
    do {\
        json_value v;\
        json_init(&v);\
        EXPECT_EQ_INT(JSON_PARSE_OK, json_parse(&v, json));\
        EXPECT_EQ_INT(JSON_STRING, json_get_type(&v));\
        EXPECT_EQ_STRING(expect, json_get_string(&v), json_get_string_length(&v));\
        json_free(&v);\
    } while(0)

static void test_parse_null() {
    json_value v;
    v.type = JSON_FALSE;
    EXPECT_EQ_INT(JSON_PARSE_OK, json_parse(&v, "null"));
    EXPECT_EQ_INT(JSON_NULL, json_get_type(&v));
}

static void test_parse_expect_value() {
    TEST_ERROR(JSON_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(JSON_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value() {
    TEST_ERROR(JSON_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(JSON_PARSE_INVALID_VALUE, "?");
}

static void test_parse_root_not_singular() {
    TEST_ERROR(JSON_PARSE_ROOT_NOT_SINGULAR, "null x");
}

static void test_parse_number() {
    // TODO: add special number test
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

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

static void test_parse_true() {
    json_value v;
    v.type = JSON_NULL;
    EXPECT_EQ_INT(JSON_PARSE_OK, json_parse(&v, "true"));
    EXPECT_EQ_INT(JSON_TRUE, json_get_type(&v));
}

static void test_parse_false() {
    json_value v;
    v.type = JSON_NULL;
    EXPECT_EQ_INT(JSON_PARSE_OK, json_parse(&v, "false"));
    EXPECT_EQ_INT(JSON_FALSE, json_get_type(&v));
}

static void test_parse_string() {
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
    // TODO: add escape characters
    // json_parse_string()
//    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
//    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
}

static void test_access_string() {
    json_value v;
    json_init(&v);
    json_set_string(&v, "", 0);
    EXPECT_EQ_STRING("", json_get_string(&v), json_get_string_length(&v));
    json_set_string(&v, "Hello", 5);
    EXPECT_EQ_STRING("Hello", json_get_string(&v), json_get_string_length(&v));
    json_free(&v);
}

static void test_parse() {
    test_parse_null();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();

    test_parse_true();
    test_parse_false();

    test_parse_number();

    test_access_string();
    test_parse_string();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
