/**
 * @file test_multi_calc.c
 * @brief Unity + fff mock tests for multi-calc module
 *
 * Demonstrates fff (Fake Function Framework) for mocking functions.
 * Uses --wrap linker option to intercept calc_xxx function calls.
 */

#include <stdio.h>
#include "unity.h"
#include "fff.h"
#include "multi-calc.h"
#include "calc.h"

/* Define fff globals (required once per test file) */
DEFINE_FFF_GLOBALS;

/* Declare fake functions for calc module */
FAKE_VALUE_FUNC(int, calc_add, int, int);
FAKE_VALUE_FUNC(int, calc_subtract, int, int);
FAKE_VALUE_FUNC(int, calc_multiply, int, int);
FAKE_VALUE_FUNC(int, calc_divide, int, int);

/* Wrapped functions that delegate to fff fakes */
int __wrap_calc_add(int a, int b) {
    return calc_add_fake.custom_fake ?
           calc_add_fake.custom_fake(a, b) :
           calc_add(a, b);
}

int __wrap_calc_subtract(int a, int b) {
    return calc_subtract_fake.custom_fake ?
           calc_subtract_fake.custom_fake(a, b) :
           calc_subtract(a, b);
}

int __wrap_calc_multiply(int a, int b) {
    return calc_multiply_fake.custom_fake ?
           calc_multiply_fake.custom_fake(a, b) :
           calc_multiply(a, b);
}

int __wrap_calc_divide(int a, int b) {
    return calc_divide_fake.custom_fake ?
           calc_divide_fake.custom_fake(a, b) :
           calc_divide(a, b);
}

/* Helper to reset all fakes */
static void reset_all_fakes(void) {
    RESET_FAKE(calc_add);
    RESET_FAKE(calc_subtract);
    RESET_FAKE(calc_multiply);
    RESET_FAKE(calc_divide);
    FFF_RESET_HISTORY();
}

void setUp(void) {
    reset_all_fakes();
}

void tearDown(void) {
    /* Nothing to clean up */
}

/* ========== Basic mock tests (return value) ========== */

static void test_expression_with_mocked_return_values(void) {
    /* Set return values for each fake function */
    calc_add_fake.return_val = 5;       /* calc_add(2, 3) -> 5 */
    calc_subtract_fake.return_val = 6;  /* calc_subtract(10, 4) -> 6 */
    calc_multiply_fake.return_val = 30; /* calc_multiply(5, 6) -> 30 */

    int result = multi_calc_expression(2, 3, 10, 4);

    TEST_ASSERT_EQUAL_INT(30, result);

    /* Verify functions were called */
    TEST_ASSERT_EQUAL_INT(1, calc_add_fake.call_count);
    TEST_ASSERT_EQUAL_INT(1, calc_subtract_fake.call_count);
    TEST_ASSERT_EQUAL_INT(1, calc_multiply_fake.call_count);
}

static void test_average_with_mocked_return_values(void) {
    /* For multi_calc_average: (a+b+c)/3 */
    /* calc_add is called twice, calc_divide once */
    calc_add_fake.return_val = 10;    /* Both add calls return 10 */
    calc_divide_fake.return_val = 3;  /* Final divide returns 3 */

    int result = multi_calc_average(1, 2, 3);

    TEST_ASSERT_EQUAL_INT(3, result);
    TEST_ASSERT_EQUAL_INT(2, calc_add_fake.call_count);
    TEST_ASSERT_EQUAL_INT(1, calc_divide_fake.call_count);
}

/* ========== Argument capture tests ========== */

static void test_expression_captures_arguments(void) {
    calc_add_fake.return_val = 5;
    calc_subtract_fake.return_val = 6;
    calc_multiply_fake.return_val = 30;

    multi_calc_expression(2, 3, 10, 4);

    /* Verify calc_add was called with (2, 3) */
    TEST_ASSERT_EQUAL_INT(2, calc_add_fake.arg0_val);
    TEST_ASSERT_EQUAL_INT(3, calc_add_fake.arg1_val);

    /* Verify calc_subtract was called with (10, 4) */
    TEST_ASSERT_EQUAL_INT(10, calc_subtract_fake.arg0_val);
    TEST_ASSERT_EQUAL_INT(4, calc_subtract_fake.arg1_val);

    /* Verify calc_multiply was called with (5, 6) - the return values */
    TEST_ASSERT_EQUAL_INT(5, calc_multiply_fake.arg0_val);
    TEST_ASSERT_EQUAL_INT(6, calc_multiply_fake.arg1_val);
}

static void test_average_captures_arguments(void) {
    calc_add_fake.return_val = 6;
    calc_divide_fake.return_val = 2;

    multi_calc_average(1, 2, 3);

    /* Verify calc_divide was called with (sum, 3) */
    TEST_ASSERT_EQUAL_INT(6, calc_divide_fake.arg0_val);
    TEST_ASSERT_EQUAL_INT(3, calc_divide_fake.arg1_val);
}

/* ========== Return value sequence tests ========== */

static int add_return_values[] = {3, 6};  /* First call returns 3, second returns 6 */

static void test_average_with_return_sequence(void) {
    /* Set up return value sequence for calc_add */
    SET_RETURN_SEQ(calc_add, add_return_values, 2);
    calc_divide_fake.return_val = 3;

    int result = multi_calc_average(1, 2, 3);

    /* First calc_add(1, 2) returns 3 */
    /* Second calc_add(3, 3) returns 6 */
    /* calc_divide(6, 3) returns 3 */
    TEST_ASSERT_EQUAL_INT(3, result);
    TEST_ASSERT_EQUAL_INT(2, calc_add_fake.call_count);
}

/* ========== Custom fake function tests ========== */

static int custom_add(int a, int b) {
    /* Custom implementation: always return 100 */
    (void)a;
    (void)b;
    return 100;
}

static int custom_multiply_double(int a, int b) {
    /* Custom implementation: return double the product */
    return (a * b) * 2;
}

static void test_expression_with_custom_fake(void) {
    /* Use custom fake for calc_add */
    calc_add_fake.custom_fake = custom_add;
    calc_subtract_fake.return_val = 50;
    calc_multiply_fake.custom_fake = custom_multiply_double;

    int result = multi_calc_expression(2, 3, 10, 4);

    /* calc_add returns 100, calc_subtract returns 50 */
    /* calc_multiply_double(100, 50) = 5000 * 2 = 10000 */
    TEST_ASSERT_EQUAL_INT(10000, result);
}

/* ========== Call history tests ========== */

static void test_call_history_order(void) {
    calc_add_fake.return_val = 5;
    calc_subtract_fake.return_val = 6;
    calc_multiply_fake.return_val = 30;

    multi_calc_expression(2, 3, 10, 4);

    /* Verify call order using fff_globals.call_history */
    TEST_ASSERT_EQUAL_PTR((void *)calc_add, fff.call_history[0]);
    TEST_ASSERT_EQUAL_PTR((void *)calc_subtract, fff.call_history[1]);
    TEST_ASSERT_EQUAL_PTR((void *)calc_multiply, fff.call_history[2]);
}

/* ========== Argument history tests ========== */

static void test_argument_history(void) {
    calc_add_fake.return_val = 10;
    calc_divide_fake.return_val = 3;

    multi_calc_average(1, 2, 3);

    /* First call to calc_add: (1, 2) */
    TEST_ASSERT_EQUAL_INT(1, calc_add_fake.arg0_history[0]);
    TEST_ASSERT_EQUAL_INT(2, calc_add_fake.arg1_history[0]);

    /* Second call to calc_add: (10, 3) where 10 is return from first add */
    TEST_ASSERT_EQUAL_INT(10, calc_add_fake.arg0_history[1]);
    TEST_ASSERT_EQUAL_INT(3, calc_add_fake.arg1_history[1]);
}

/* ========== fff feature demo ========== */

static void test_fff_features_demo(void) {
    /* Reset specific fake */
    RESET_FAKE(calc_add);

    /* Set return value */
    calc_add_fake.return_val = 42;

    /* Call the fake directly */
    int result = calc_add(10, 20);

    /* Assertions on fake */
    TEST_ASSERT_EQUAL_INT(42, result);
    TEST_ASSERT_EQUAL_INT(1, calc_add_fake.call_count);
    TEST_ASSERT_EQUAL_INT(10, calc_add_fake.arg0_val);
    TEST_ASSERT_EQUAL_INT(20, calc_add_fake.arg1_val);

    /* Reset and verify */
    RESET_FAKE(calc_add);
    TEST_ASSERT_EQUAL_INT(0, calc_add_fake.call_count);
}

/* ========== Main ========== */

int main(void) {
    UNITY_BEGIN();

    /* Basic mock tests */
    RUN_TEST(test_expression_with_mocked_return_values);
    RUN_TEST(test_average_with_mocked_return_values);

    /* Argument capture tests */
    RUN_TEST(test_expression_captures_arguments);
    RUN_TEST(test_average_captures_arguments);

    /* Return value sequence tests */
    RUN_TEST(test_average_with_return_sequence);

    /* Custom fake function tests */
    RUN_TEST(test_expression_with_custom_fake);

    /* Call history tests */
    RUN_TEST(test_call_history_order);

    /* Argument history tests */
    RUN_TEST(test_argument_history);

    /* fff feature demo */
    RUN_TEST(test_fff_features_demo);

    return UNITY_END();
}
