/**
 * @file test_greeting.c
 * @brief Unity unit tests for greeting module
 *
 * Demonstrates Unity setUp/tearDown fixtures and string assertions.
 */

#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "greeting.h"

/* Test context for fixture demo */
static struct {
    char *buffer;
    int call_count;
} test_ctx;

/* Unity requires setUp and tearDown functions */
void setUp(void) {
    /* Allocate buffer for each test */
    test_ctx.buffer = (char *)malloc(256);
    test_ctx.call_count = 0;
}

void tearDown(void) {
    /* Free buffer after each test */
    if (test_ctx.buffer != NULL) {
        free(test_ctx.buffer);
        test_ctx.buffer = NULL;
    }
}

/* ========== say_hello tests ========== */

static void test_say_hello_with_name(void) {
    const char *result = say_hello("World");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Hello, World!", result);
}

static void test_say_hello_with_different_names(void) {
    TEST_ASSERT_EQUAL_STRING("Hello, Alice!", say_hello("Alice"));
    TEST_ASSERT_EQUAL_STRING("Hello, Bob!", say_hello("Bob"));
    TEST_ASSERT_EQUAL_STRING("Hello, Unity!", say_hello("Unity"));
}

static void test_say_hello_with_empty_string(void) {
    const char *result = say_hello("");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Hello, stranger!", result);
}

static void test_say_hello_with_null(void) {
    const char *result = say_hello(NULL);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Hello, stranger!", result);
}

/* ========== say_goodbye tests ========== */

static void test_say_goodbye_with_name(void) {
    const char *result = say_goodbye("World");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Goodbye, World!", result);
}

static void test_say_goodbye_with_empty_string(void) {
    const char *result = say_goodbye("");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Goodbye, stranger!", result);
}

static void test_say_goodbye_with_null(void) {
    const char *result = say_goodbye(NULL);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Goodbye, stranger!", result);
}

/* ========== Fixture demo tests ========== */

static void test_fixture_buffer_allocated(void) {
    /* Verify setUp allocated buffer */
    TEST_ASSERT_NOT_NULL(test_ctx.buffer);

    /* Use the buffer */
    strcpy(test_ctx.buffer, "Test data");
    TEST_ASSERT_EQUAL_STRING("Test data", test_ctx.buffer);
}

static void test_fixture_counter_reset(void) {
    /* Verify counter is reset for each test */
    TEST_ASSERT_EQUAL_INT(0, test_ctx.call_count);

    /* Increment counter */
    test_ctx.call_count++;
    TEST_ASSERT_EQUAL_INT(1, test_ctx.call_count);
}

static void test_fixture_counter_independent(void) {
    /* This test should also start with counter = 0 */
    TEST_ASSERT_EQUAL_INT(0, test_ctx.call_count);
}

/* ========== String assertion demo ========== */

static void test_unity_string_assertions_demo(void) {
    const char *str1 = "Hello";
    const char *str2 = "Hello";
    const char *str3 = "World";

    /* String equality */
    TEST_ASSERT_EQUAL_STRING(str1, str2);

    /* String length */
    TEST_ASSERT_EQUAL_UINT(5, strlen(str1));

    /* Partial string comparison */
    TEST_ASSERT_EQUAL_STRING_LEN("Hel", str1, 3);

    /* Memory comparison */
    TEST_ASSERT_EQUAL_MEMORY(str1, str2, 5);

    /* Using buffer from fixture */
    strcpy(test_ctx.buffer, str3);
    TEST_ASSERT_EQUAL_STRING("World", test_ctx.buffer);
}

/* ========== Edge case tests ========== */

static void test_say_hello_long_name(void) {
    const char *long_name = "VeryLongNameThatExceedsNormalLength";
    const char *result = say_hello(long_name);
    TEST_ASSERT_NOT_NULL(result);

    /* Build expected string in buffer */
    sprintf(test_ctx.buffer, "Hello, %s!", long_name);
    TEST_ASSERT_EQUAL_STRING(test_ctx.buffer, result);
}

static void test_say_hello_special_characters(void) {
    const char *result = say_hello("Test@123");
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Hello, Test@123!", result);
}

/* ========== Main ========== */

int main(void) {
    UNITY_BEGIN();

    /* say_hello tests */
    RUN_TEST(test_say_hello_with_name);
    RUN_TEST(test_say_hello_with_different_names);
    RUN_TEST(test_say_hello_with_empty_string);
    RUN_TEST(test_say_hello_with_null);

    /* say_goodbye tests */
    RUN_TEST(test_say_goodbye_with_name);
    RUN_TEST(test_say_goodbye_with_empty_string);
    RUN_TEST(test_say_goodbye_with_null);

    /* Fixture demo tests */
    RUN_TEST(test_fixture_buffer_allocated);
    RUN_TEST(test_fixture_counter_reset);
    RUN_TEST(test_fixture_counter_independent);

    /* String assertion demo */
    RUN_TEST(test_unity_string_assertions_demo);

    /* Edge case tests */
    RUN_TEST(test_say_hello_long_name);
    RUN_TEST(test_say_hello_special_characters);

    return UNITY_END();
}
