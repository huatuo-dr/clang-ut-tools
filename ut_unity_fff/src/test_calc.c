/**
 * @file test_calc.c
 * @brief Unity unit tests for calc module
 *
 * Demonstrates Unity basic assertions and test organization.
 */

#include "unity.h"
#include "calc.h"

/* Unity requires setUp and tearDown functions */
void setUp(void) {
    /* Called before each test */
}

void tearDown(void) {
    /* Called after each test */
}

/* ========== calc_add tests ========== */

static void test_calc_add_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(5, calc_add(2, 3));
    TEST_ASSERT_EQUAL_INT(100, calc_add(50, 50));
}

static void test_calc_add_negative_numbers(void) {
    TEST_ASSERT_EQUAL_INT(-5, calc_add(-2, -3));
    TEST_ASSERT_EQUAL_INT(-100, calc_add(-50, -50));
}

static void test_calc_add_mixed_numbers(void) {
    TEST_ASSERT_EQUAL_INT(1, calc_add(-2, 3));
    TEST_ASSERT_EQUAL_INT(-1, calc_add(2, -3));
}

static void test_calc_add_zero(void) {
    TEST_ASSERT_EQUAL_INT(5, calc_add(5, 0));
    TEST_ASSERT_EQUAL_INT(5, calc_add(0, 5));
    TEST_ASSERT_EQUAL_INT(0, calc_add(0, 0));
}

/* ========== calc_subtract tests ========== */

static void test_calc_subtract_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(2, calc_subtract(5, 3));
    TEST_ASSERT_EQUAL_INT(0, calc_subtract(5, 5));
}

static void test_calc_subtract_negative_result(void) {
    TEST_ASSERT_EQUAL_INT(-2, calc_subtract(3, 5));
}

/* ========== calc_multiply tests ========== */

static void test_calc_multiply_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(6, calc_multiply(2, 3));
    TEST_ASSERT_EQUAL_INT(100, calc_multiply(10, 10));
}

static void test_calc_multiply_with_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, calc_multiply(5, 0));
    TEST_ASSERT_EQUAL_INT(0, calc_multiply(0, 5));
}

static void test_calc_multiply_negative_numbers(void) {
    TEST_ASSERT_EQUAL_INT(-6, calc_multiply(-2, 3));
    TEST_ASSERT_EQUAL_INT(6, calc_multiply(-2, -3));
}

/* ========== calc_divide tests ========== */

static void test_calc_divide_exact(void) {
    TEST_ASSERT_EQUAL_INT(2, calc_divide(6, 3));
    TEST_ASSERT_EQUAL_INT(5, calc_divide(10, 2));
}

static void test_calc_divide_truncation(void) {
    TEST_ASSERT_EQUAL_INT(2, calc_divide(7, 3));
    TEST_ASSERT_EQUAL_INT(3, calc_divide(10, 3));
}

static void test_calc_divide_by_zero(void) {
    /* Division by zero should return 0 (as defined in calc.c) */
    TEST_ASSERT_EQUAL_INT(0, calc_divide(5, 0));
    TEST_ASSERT_EQUAL_INT(0, calc_divide(0, 0));
}

static void test_calc_divide_negative_numbers(void) {
    TEST_ASSERT_EQUAL_INT(-2, calc_divide(-6, 3));
    TEST_ASSERT_EQUAL_INT(2, calc_divide(-6, -3));
}

/* ========== Unity assertion demo ========== */

static void test_unity_assertions_demo(void) {
    /* Integer assertions */
    TEST_ASSERT_EQUAL_INT(42, 42);
    TEST_ASSERT_NOT_EQUAL(42, 43);

    /* Boolean assertions */
    TEST_ASSERT_TRUE(1 == 1);
    TEST_ASSERT_FALSE(1 == 0);

    /* Pointer assertions */
    int value = 10;
    int *ptr = &value;
    TEST_ASSERT_NOT_NULL(ptr);

    int *null_ptr = NULL;
    TEST_ASSERT_NULL(null_ptr);

    /* Range assertions */
    TEST_ASSERT_INT_WITHIN(5, 100, 102);  /* 102 is within 5 of 100 */

    /* Comparison assertions */
    TEST_ASSERT_GREATER_THAN(5, 10);
    TEST_ASSERT_LESS_THAN(10, 5);
    TEST_ASSERT_GREATER_OR_EQUAL(5, 5);
    TEST_ASSERT_LESS_OR_EQUAL(5, 5);
}

/* ========== Main ========== */

int main(void) {
    UNITY_BEGIN();

    /* calc_add tests */
    RUN_TEST(test_calc_add_positive_numbers);
    RUN_TEST(test_calc_add_negative_numbers);
    RUN_TEST(test_calc_add_mixed_numbers);
    RUN_TEST(test_calc_add_zero);

    /* calc_subtract tests */
    RUN_TEST(test_calc_subtract_positive_numbers);
    RUN_TEST(test_calc_subtract_negative_result);

    /* calc_multiply tests */
    RUN_TEST(test_calc_multiply_positive_numbers);
    RUN_TEST(test_calc_multiply_with_zero);
    RUN_TEST(test_calc_multiply_negative_numbers);

    /* calc_divide tests */
    RUN_TEST(test_calc_divide_exact);
    RUN_TEST(test_calc_divide_truncation);
    RUN_TEST(test_calc_divide_by_zero);
    RUN_TEST(test_calc_divide_negative_numbers);

    /* Unity assertions demo */
    RUN_TEST(test_unity_assertions_demo);

    return UNITY_END();
}
