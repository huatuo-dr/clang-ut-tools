/**
 * @file test_multi_calc.c
 * @brief Unit tests for multi-calc module with Mock
 *
 * Demonstrates cmocka Mock features:
 * - will_return() to set mock return values
 * - mock_type() to get mock values in wrapped functions
 * - __wrap_xxx() functions to replace real implementations
 * - __real_xxx() to call original functions when needed
 * - Hybrid testing: mixing real and mock functions in same test file
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdbool.h>
#include <cmocka.h>

#include "multi-calc.h"

/*============================================================================
 * Real Function Declarations
 *
 * When using --wrap, the linker provides __real_xxx to access original functions
 *===========================================================================*/

extern int __real_calc_add(int a, int b);
extern int __real_calc_subtract(int a, int b);
extern int __real_calc_multiply(int a, int b);
extern int __real_calc_divide(int a, int b);

/*============================================================================
 * Mock Control Flags
 *
 * These flags control whether to use mock or real implementation
 *===========================================================================*/

static bool mock_calc_add = true;
static bool mock_calc_subtract = true;
static bool mock_calc_multiply = true;
static bool mock_calc_divide = true;

/*============================================================================
 * Mock Functions - Using __wrap_ prefix
 *
 * These functions can switch between mock and real implementation
 *===========================================================================*/

/**
 * Mock implementation of calc_add
 * Can return mock value or call real function based on flag
 */
int __wrap_calc_add(int a, int b) {
    if (mock_calc_add) {
        return mock_type(int);
    } else {
        return __real_calc_add(a, b);
    }
}

/**
 * Mock implementation of calc_subtract
 */
int __wrap_calc_subtract(int a, int b) {
    if (mock_calc_subtract) {
        return mock_type(int);
    } else {
        return __real_calc_subtract(a, b);
    }
}

/**
 * Mock implementation of calc_multiply
 */
int __wrap_calc_multiply(int a, int b) {
    if (mock_calc_multiply) {
        return mock_type(int);
    } else {
        return __real_calc_multiply(a, b);
    }
}

/**
 * Mock implementation of calc_divide
 */
int __wrap_calc_divide(int a, int b) {
    if (mock_calc_divide) {
        return mock_type(int);
    } else {
        return __real_calc_divide(a, b);
    }
}

/*============================================================================
 * Helper Functions - Enable/Disable all mocks
 *===========================================================================*/

static void enable_all_mocks(void) {
    mock_calc_add = true;
    mock_calc_subtract = true;
    mock_calc_multiply = true;
    mock_calc_divide = true;
}

static void disable_all_mocks(void) {
    mock_calc_add = false;
    mock_calc_subtract = false;
    mock_calc_multiply = false;
    mock_calc_divide = false;
}

/*============================================================================
 * Test Cases for multi_calc_expression (Pure Mock)
 * Formula: (a + b) * (c - d)
 *===========================================================================*/

/**
 * Test normal calculation: (2 + 3) * (10 - 4) = 5 * 6 = 30
 * Mock sequence: calc_add -> calc_subtract -> calc_multiply
 */
static void test_expression_normal(void **state) {
    (void)state;
    enable_all_mocks();

    // Setup mock return values in call order
    will_return(__wrap_calc_add, 5);       // calc_add(2, 3) -> 5
    will_return(__wrap_calc_subtract, 6);  // calc_subtract(10, 4) -> 6
    will_return(__wrap_calc_multiply, 30); // calc_multiply(5, 6) -> 30

    // Call the function under test
    int result = multi_calc_expression(2, 3, 10, 4);

    // Verify result
    assert_int_equal(result, 30);
}

/**
 * Test when subtraction result is zero: (a + b) * 0 = 0
 */
static void test_expression_subtract_zero(void **state) {
    (void)state;
    enable_all_mocks();

    will_return(__wrap_calc_add, 10);      // any sum
    will_return(__wrap_calc_subtract, 0);  // c - d = 0
    will_return(__wrap_calc_multiply, 0);  // anything * 0 = 0

    int result = multi_calc_expression(5, 5, 7, 7);

    assert_int_equal(result, 0);
}

/**
 * Test with negative intermediate result
 */
static void test_expression_negative_subtract(void **state) {
    (void)state;
    enable_all_mocks();

    will_return(__wrap_calc_add, 3);       // 1 + 2 = 3
    will_return(__wrap_calc_subtract, -3); // 2 - 5 = -3
    will_return(__wrap_calc_multiply, -9); // 3 * (-3) = -9

    int result = multi_calc_expression(1, 2, 2, 5);

    assert_int_equal(result, -9);
}

/**
 * Test with all zeros
 */
static void test_expression_all_zeros(void **state) {
    (void)state;
    enable_all_mocks();

    will_return(__wrap_calc_add, 0);
    will_return(__wrap_calc_subtract, 0);
    will_return(__wrap_calc_multiply, 0);

    int result = multi_calc_expression(0, 0, 0, 0);

    assert_int_equal(result, 0);
}

/*============================================================================
 * Test Cases for multi_calc_average (Pure Mock)
 * Formula: (a + b + c) / 3
 *===========================================================================*/

/**
 * Test normal average: (10 + 20 + 30) / 3 = 60 / 3 = 20
 * Mock sequence: calc_add -> calc_add -> calc_divide
 */
static void test_average_normal(void **state) {
    (void)state;
    enable_all_mocks();

    will_return(__wrap_calc_add, 30);    // 10 + 20 = 30
    will_return(__wrap_calc_add, 60);    // 30 + 30 = 60
    will_return(__wrap_calc_divide, 20); // 60 / 3 = 20

    int result = multi_calc_average(10, 20, 30);

    assert_int_equal(result, 20);
}

/**
 * Test average with truncation: (1 + 1 + 1) / 3 = 3 / 3 = 1
 */
static void test_average_truncation(void **state) {
    (void)state;
    enable_all_mocks();

    will_return(__wrap_calc_add, 2);     // 1 + 1 = 2
    will_return(__wrap_calc_add, 3);     // 2 + 1 = 3
    will_return(__wrap_calc_divide, 1);  // 3 / 3 = 1

    int result = multi_calc_average(1, 1, 1);

    assert_int_equal(result, 1);
}

/**
 * Test average with all zeros
 */
static void test_average_all_zeros(void **state) {
    (void)state;
    enable_all_mocks();

    will_return(__wrap_calc_add, 0);     // 0 + 0 = 0
    will_return(__wrap_calc_add, 0);     // 0 + 0 = 0
    will_return(__wrap_calc_divide, 0);  // 0 / 3 = 0

    int result = multi_calc_average(0, 0, 0);

    assert_int_equal(result, 0);
}

/**
 * Test: What if calc_add returns unexpected value?
 * This demonstrates how mock can simulate abnormal behavior
 */
static void test_average_mock_abnormal(void **state) {
    (void)state;
    enable_all_mocks();

    // Mock calc_add to return unexpected values
    will_return(__wrap_calc_add, 999);   // Unexpected!
    will_return(__wrap_calc_add, 1000);  // Unexpected!
    will_return(__wrap_calc_divide, 333);

    // The result is based on mock values, not real calculation
    int result = multi_calc_average(1, 2, 3);

    // Verify the function uses the values from calc_* correctly
    assert_int_equal(result, 333);
}

/**
 * Test: negative numbers
 */
static void test_average_negative(void **state) {
    (void)state;
    enable_all_mocks();

    will_return(__wrap_calc_add, -10);    // -5 + (-5) = -10
    will_return(__wrap_calc_add, -15);    // -10 + (-5) = -15
    will_return(__wrap_calc_divide, -5);  // -15 / 3 = -5

    int result = multi_calc_average(-5, -5, -5);

    assert_int_equal(result, -5);
}

/*============================================================================
 * Hybrid Test Cases - Mix real and mock functions
 *
 * Demonstrates how to use __real_xxx to call original functions
 *===========================================================================*/

/**
 * Test using ALL REAL functions (no mock)
 * This verifies the actual implementation works correctly
 */
static void test_expression_real_all(void **state) {
    (void)state;
    disable_all_mocks();  // Use real functions

    // (2 + 3) * (10 - 4) = 5 * 6 = 30
    int result = multi_calc_expression(2, 3, 10, 4);
    assert_int_equal(result, 30);

    // (5 + 5) * (8 - 3) = 10 * 5 = 50
    result = multi_calc_expression(5, 5, 8, 3);
    assert_int_equal(result, 50);

    // (0 + 0) * (0 - 0) = 0 * 0 = 0
    result = multi_calc_expression(0, 0, 0, 0);
    assert_int_equal(result, 0);
}

/**
 * Test using ALL REAL functions for average
 */
static void test_average_real_all(void **state) {
    (void)state;
    disable_all_mocks();  // Use real functions

    // (10 + 20 + 30) / 3 = 60 / 3 = 20
    int result = multi_calc_average(10, 20, 30);
    assert_int_equal(result, 20);

    // (7 + 8 + 9) / 3 = 24 / 3 = 8
    result = multi_calc_average(7, 8, 9);
    assert_int_equal(result, 8);

    // (1 + 1 + 1) / 3 = 3 / 3 = 1
    result = multi_calc_average(1, 1, 1);
    assert_int_equal(result, 1);
}

/**
 * Test with PARTIAL mock: mock only calc_multiply
 * calc_add and calc_subtract use real implementation
 */
static void test_expression_partial_mock_multiply(void **state) {
    (void)state;

    // Enable only calc_multiply mock
    mock_calc_add = false;       // Use real
    mock_calc_subtract = false;  // Use real
    mock_calc_multiply = true;   // Use mock
    mock_calc_divide = false;    // Use real

    // calc_add(2, 3) = 5 (real)
    // calc_subtract(10, 4) = 6 (real)
    // calc_multiply(5, 6) = mock returns 999 instead of 30
    will_return(__wrap_calc_multiply, 999);

    int result = multi_calc_expression(2, 3, 10, 4);

    // Real add and subtract, but mock multiply returns 999
    assert_int_equal(result, 999);
}

/**
 * Test with PARTIAL mock: mock only calc_divide
 * calc_add uses real implementation
 */
static void test_average_partial_mock_divide(void **state) {
    (void)state;

    // Enable only calc_divide mock
    mock_calc_add = false;     // Use real
    mock_calc_subtract = false;
    mock_calc_multiply = false;
    mock_calc_divide = true;   // Use mock

    // calc_add(10, 20) = 30 (real)
    // calc_add(30, 30) = 60 (real)
    // calc_divide(60, 3) = mock returns 100 instead of 20
    will_return(__wrap_calc_divide, 100);

    int result = multi_calc_average(10, 20, 30);

    // Real additions, but mock divide returns 100
    assert_int_equal(result, 100);
}

/**
 * Compare mock vs real in same test
 * This demonstrates the difference between mock and real results
 */
static void test_compare_mock_vs_real(void **state) {
    (void)state;
    int mock_result, real_result;

    // First: test with mock
    enable_all_mocks();
    will_return(__wrap_calc_add, 100);  // Mock: return 100
    will_return(__wrap_calc_add, 200);  // Mock: return 200
    will_return(__wrap_calc_divide, 66);
    mock_result = multi_calc_average(1, 2, 3);
    assert_int_equal(mock_result, 66);  // Mock result

    // Second: test with real
    disable_all_mocks();
    real_result = multi_calc_average(1, 2, 3);
    assert_int_equal(real_result, 2);   // Real result: (1+2+3)/3 = 2

    // Verify they are different (mock vs real)
    assert_int_not_equal(mock_result, real_result);
    printf("    Mock result: %d, Real result: %d\n", mock_result, real_result);
}

/*============================================================================
 * Main - Run all tests
 *===========================================================================*/

int main(void) {
    const struct CMUnitTest expression_tests[] = {
        cmocka_unit_test(test_expression_normal),
        cmocka_unit_test(test_expression_subtract_zero),
        cmocka_unit_test(test_expression_negative_subtract),
        cmocka_unit_test(test_expression_all_zeros),
    };

    const struct CMUnitTest average_tests[] = {
        cmocka_unit_test(test_average_normal),
        cmocka_unit_test(test_average_truncation),
        cmocka_unit_test(test_average_all_zeros),
        cmocka_unit_test(test_average_mock_abnormal),
        cmocka_unit_test(test_average_negative),
    };

    const struct CMUnitTest hybrid_tests[] = {
        cmocka_unit_test(test_expression_real_all),
        cmocka_unit_test(test_average_real_all),
        cmocka_unit_test(test_expression_partial_mock_multiply),
        cmocka_unit_test(test_average_partial_mock_divide),
        cmocka_unit_test(test_compare_mock_vs_real),
    };

    int result = 0;

    printf("\n========== MULTI-CALC MODULE MOCK TESTS ==========\n\n");

    result += cmocka_run_group_tests_name("expression mock tests", expression_tests, NULL, NULL);
    result += cmocka_run_group_tests_name("average mock tests", average_tests, NULL, NULL);
    result += cmocka_run_group_tests_name("hybrid tests (real + mock)", hybrid_tests, NULL, NULL);

    return result;
}