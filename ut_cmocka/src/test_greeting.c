/**
 * @file test_greeting.c
 * @brief Unit tests for greeting module
 *
 * Demonstrates cmocka features:
 * - Test fixtures (setup and teardown functions)
 * - Group setup and teardown
 * - String assertions (assert_string_equal)
 * - Pointer assertions (assert_non_null, assert_null)
 * - Memory allocation in tests
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmocka.h>

#include "greeting.h"

/*============================================================================
 * Test Fixtures - Setup and Teardown
 *===========================================================================*/

// Test context structure
struct test_context {
    char *test_name;
    int test_count;
};

// Group setup - called once before all tests in a group
static int group_setup(void **state) {
    struct test_context *ctx = malloc(sizeof(struct test_context));
    if (ctx == NULL) {
        return -1;
    }
    ctx->test_name = NULL;
    ctx->test_count = 0;
    *state = ctx;

    printf("  [Group Setup] Test context initialized\n");
    return 0;
}

// Group teardown - called once after all tests in a group
static int group_teardown(void **state) {
    struct test_context *ctx = (struct test_context *)*state;
    if (ctx != NULL) {
        if (ctx->test_name != NULL) {
            free(ctx->test_name);
        }
        free(ctx);
    }
    printf("  [Group Teardown] Test context cleaned up\n");
    return 0;
}

// Test setup - called before each test
static int test_setup(void **state) {
    struct test_context *ctx = (struct test_context *)*state;
    ctx->test_count++;
    return 0;
}

// Test teardown - called after each test
static int test_teardown(void **state) {
    struct test_context *ctx = (struct test_context *)*state;
    if (ctx->test_name != NULL) {
        free(ctx->test_name);
        ctx->test_name = NULL;
    }
    return 0;
}

/*============================================================================
 * String Assertion Tests - say_hello
 *===========================================================================*/

static void test_say_hello_with_name(void **state) {
    (void)state;
    const char *result = say_hello("Alice");

    // String assertions
    assert_non_null(result);
    assert_string_equal(result, "Hello, Alice!");
}

static void test_say_hello_with_different_names(void **state) {
    (void)state;

    assert_string_equal(say_hello("Bob"), "Hello, Bob!");
    assert_string_equal(say_hello("Charlie"), "Hello, Charlie!");
    assert_string_equal(say_hello("David"), "Hello, David!");
}

static void test_say_hello_with_empty_string(void **state) {
    (void)state;
    const char *result = say_hello("");

    assert_non_null(result);
    assert_string_equal(result, "Hello, stranger!");
}

static void test_say_hello_with_null(void **state) {
    (void)state;
    const char *result = say_hello(NULL);

    assert_non_null(result);
    assert_string_equal(result, "Hello, stranger!");
}

/*============================================================================
 * String Assertion Tests - say_goodbye
 *===========================================================================*/

static void test_say_goodbye_with_name(void **state) {
    (void)state;
    const char *result = say_goodbye("Alice");

    assert_non_null(result);
    assert_string_equal(result, "Goodbye, Alice!");
}

static void test_say_goodbye_with_empty_string(void **state) {
    (void)state;
    const char *result = say_goodbye("");

    assert_non_null(result);
    assert_string_equal(result, "Goodbye, stranger!");
}

static void test_say_goodbye_with_null(void **state) {
    (void)state;
    const char *result = say_goodbye(NULL);

    assert_non_null(result);
    assert_string_equal(result, "Goodbye, stranger!");
}

/*============================================================================
 * Tests with Fixtures - Using test context
 *===========================================================================*/

static void test_greeting_with_context(void **state) {
    struct test_context *ctx = (struct test_context *)*state;

    // Allocate test name in context
    ctx->test_name = strdup("TestUser");
    assert_non_null(ctx->test_name);

    const char *hello = say_hello(ctx->test_name);
    assert_string_equal(hello, "Hello, TestUser!");

    const char *goodbye = say_goodbye(ctx->test_name);
    assert_string_equal(goodbye, "Goodbye, TestUser!");
}

static void test_greeting_counter(void **state) {
    struct test_context *ctx = (struct test_context *)*state;

    // Verify test count is tracked
    assert_true(ctx->test_count > 0);

    // Test with dynamic name
    ctx->test_name = malloc(32);
    assert_non_null(ctx->test_name);
    snprintf(ctx->test_name, 32, "User%d", ctx->test_count);

    const char *result = say_hello(ctx->test_name);
    assert_non_null(result);
}

/*============================================================================
 * Edge Case Tests
 *===========================================================================*/

static void test_say_hello_long_name(void **state) {
    (void)state;

    // Test with a longer name
    const char *long_name = "ThisIsAVeryLongNameForTesting";
    const char *result = say_hello(long_name);

    assert_non_null(result);
    assert_true(strstr(result, long_name) != NULL);
}

static void test_say_hello_special_characters(void **state) {
    (void)state;

    // Test with special characters
    const char *result = say_hello("O'Brien");
    assert_non_null(result);
    assert_string_equal(result, "Hello, O'Brien!");
}

static void test_greeting_return_not_null(void **state) {
    (void)state;

    // Both functions should never return NULL
    assert_non_null(say_hello("test"));
    assert_non_null(say_hello(""));
    assert_non_null(say_hello(NULL));
    assert_non_null(say_goodbye("test"));
    assert_non_null(say_goodbye(""));
    assert_non_null(say_goodbye(NULL));
}

/*============================================================================
 * Main - Run all tests
 *===========================================================================*/

int main(void) {
    // Basic tests without fixtures
    const struct CMUnitTest hello_tests[] = {
        cmocka_unit_test(test_say_hello_with_name),
        cmocka_unit_test(test_say_hello_with_different_names),
        cmocka_unit_test(test_say_hello_with_empty_string),
        cmocka_unit_test(test_say_hello_with_null),
    };

    const struct CMUnitTest goodbye_tests[] = {
        cmocka_unit_test(test_say_goodbye_with_name),
        cmocka_unit_test(test_say_goodbye_with_empty_string),
        cmocka_unit_test(test_say_goodbye_with_null),
    };

    // Tests with fixtures (setup and teardown)
    const struct CMUnitTest fixture_tests[] = {
        cmocka_unit_test_setup_teardown(test_greeting_with_context, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_greeting_counter, test_setup, test_teardown),
    };

    // Edge case tests
    const struct CMUnitTest edge_case_tests[] = {
        cmocka_unit_test(test_say_hello_long_name),
        cmocka_unit_test(test_say_hello_special_characters),
        cmocka_unit_test(test_greeting_return_not_null),
    };

    int result = 0;

    printf("\n========== GREETING MODULE UNIT TESTS ==========\n\n");

    // Run basic tests
    result += cmocka_run_group_tests_name("say_hello tests", hello_tests, NULL, NULL);
    result += cmocka_run_group_tests_name("say_goodbye tests", goodbye_tests, NULL, NULL);

    // Run tests with group fixtures
    result += cmocka_run_group_tests_name("fixture tests", fixture_tests, group_setup, group_teardown);

    // Run edge case tests
    result += cmocka_run_group_tests_name("edge case tests", edge_case_tests, NULL, NULL);

    return result;
}