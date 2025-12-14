/**
 * @file test_calc.c
 * @brief Unit tests for calc module
 *
 * Demonstrates cmocka features:
 * - Basic assert macros (assert_int_equal, assert_true, etc.)
 * - Test grouping with cmocka_unit_test
 * - Parameterized tests with cmocka_unit_test_prestate
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>
#include <cmocka.h>

#include "calc.h"

/*============================================================================
 * Basic Assert Tests - calc_add
 *===========================================================================*/

static void test_calc_add_positive_numbers(void **state) {
    (void)state;
    assert_int_equal(calc_add(2, 3), 5);
    assert_int_equal(calc_add(100, 200), 300);
}

static void test_calc_add_negative_numbers(void **state) {
    (void)state;
    assert_int_equal(calc_add(-2, -3), -5);
    assert_int_equal(calc_add(-100, -200), -300);
}

static void test_calc_add_mixed_numbers(void **state) {
    (void)state;
    assert_int_equal(calc_add(-5, 3), -2);
    assert_int_equal(calc_add(5, -3), 2);
}

static void test_calc_add_zero(void **state) {
    (void)state;
    assert_int_equal(calc_add(0, 0), 0);
    assert_int_equal(calc_add(5, 0), 5);
    assert_int_equal(calc_add(0, 5), 5);
}

/*============================================================================
 * Basic Assert Tests - calc_subtract
 *===========================================================================*/

static void test_calc_subtract_positive_numbers(void **state) {
    (void)state;
    assert_int_equal(calc_subtract(5, 3), 2);
    assert_int_equal(calc_subtract(100, 30), 70);
}

static void test_calc_subtract_negative_result(void **state) {
    (void)state;
    assert_int_equal(calc_subtract(3, 5), -2);
    assert_true(calc_subtract(10, 20) < 0);
}

/*============================================================================
 * Basic Assert Tests - calc_multiply
 *===========================================================================*/

static void test_calc_multiply_positive_numbers(void **state) {
    (void)state;
    assert_int_equal(calc_multiply(2, 3), 6);
    assert_int_equal(calc_multiply(10, 10), 100);
}

static void test_calc_multiply_with_zero(void **state) {
    (void)state;
    assert_int_equal(calc_multiply(0, 100), 0);
    assert_int_equal(calc_multiply(100, 0), 0);
    assert_int_equal(calc_multiply(0, 0), 0);
}

static void test_calc_multiply_negative_numbers(void **state) {
    (void)state;
    // Negative * Negative = Positive
    assert_true(calc_multiply(-3, -4) > 0);
    assert_int_equal(calc_multiply(-3, -4), 12);

    // Positive * Negative = Negative
    assert_true(calc_multiply(3, -4) < 0);
    assert_int_equal(calc_multiply(3, -4), -12);
}

/*============================================================================
 * Basic Assert Tests - calc_divide
 *===========================================================================*/

static void test_calc_divide_exact(void **state) {
    (void)state;
    assert_int_equal(calc_divide(10, 2), 5);
    assert_int_equal(calc_divide(100, 10), 10);
}

static void test_calc_divide_truncation(void **state) {
    (void)state;
    // Integer division truncates
    assert_int_equal(calc_divide(10, 3), 3);
    assert_int_equal(calc_divide(7, 2), 3);
}

static void test_calc_divide_by_zero(void **state) {
    (void)state;
    // Division by zero should return 0 (as per our implementation)
    assert_int_equal(calc_divide(10, 0), 0);
    assert_int_equal(calc_divide(0, 0), 0);
    assert_int_equal(calc_divide(-5, 0), 0);
}

static void test_calc_divide_negative_numbers(void **state) {
    (void)state;
    assert_int_equal(calc_divide(-10, 2), -5);
    assert_int_equal(calc_divide(10, -2), -5);
    assert_int_equal(calc_divide(-10, -2), 5);
}

/*============================================================================
 * Parameterized Test - Using prestate
 *===========================================================================*/

// Structure for parameterized test data
struct calc_test_data {
    int a;
    int b;
    int expected;
};

static void test_calc_add_parameterized(void **state) {
    struct calc_test_data *data = (struct calc_test_data *)*state;
    assert_int_equal(calc_add(data->a, data->b), data->expected);
}

// Test data for parameterized tests
static struct calc_test_data add_test_data[] = {
    {1, 1, 2},
    {0, 0, 0},
    {-1, 1, 0},
    {100, -100, 0},
    {INT32_MAX - 1, 1, INT32_MAX},
};

/*============================================================================
 * Main - Run all tests
 *===========================================================================*/

int main(void) {
    const struct CMUnitTest calc_add_tests[] = {
        cmocka_unit_test(test_calc_add_positive_numbers),
        cmocka_unit_test(test_calc_add_negative_numbers),
        cmocka_unit_test(test_calc_add_mixed_numbers),
        cmocka_unit_test(test_calc_add_zero),
    };

    const struct CMUnitTest calc_subtract_tests[] = {
        cmocka_unit_test(test_calc_subtract_positive_numbers),
        cmocka_unit_test(test_calc_subtract_negative_result),
    };

    const struct CMUnitTest calc_multiply_tests[] = {
        cmocka_unit_test(test_calc_multiply_positive_numbers),
        cmocka_unit_test(test_calc_multiply_with_zero),
        cmocka_unit_test(test_calc_multiply_negative_numbers),
    };

    const struct CMUnitTest calc_divide_tests[] = {
        cmocka_unit_test(test_calc_divide_exact),
        cmocka_unit_test(test_calc_divide_truncation),
        cmocka_unit_test(test_calc_divide_by_zero),
        cmocka_unit_test(test_calc_divide_negative_numbers),
    };

    // Parameterized tests using prestate
    const struct CMUnitTest calc_parameterized_tests[] = {
        cmocka_unit_test_prestate(test_calc_add_parameterized, &add_test_data[0]),
        cmocka_unit_test_prestate(test_calc_add_parameterized, &add_test_data[1]),
        cmocka_unit_test_prestate(test_calc_add_parameterized, &add_test_data[2]),
        cmocka_unit_test_prestate(test_calc_add_parameterized, &add_test_data[3]),
        cmocka_unit_test_prestate(test_calc_add_parameterized, &add_test_data[4]),
    };

    int result = 0;

    printf("\n========== CALC MODULE UNIT TESTS ==========\n\n");

    // Run test groups
    result += cmocka_run_group_tests_name("calc_add tests", calc_add_tests, NULL, NULL);
    result += cmocka_run_group_tests_name("calc_subtract tests", calc_subtract_tests, NULL, NULL);
    result += cmocka_run_group_tests_name("calc_multiply tests", calc_multiply_tests, NULL, NULL);
    result += cmocka_run_group_tests_name("calc_divide tests", calc_divide_tests, NULL, NULL);
    result += cmocka_run_group_tests_name("calc_add parameterized tests", calc_parameterized_tests, NULL, NULL);

    return result;
}