/**
 * @file test_multi_calc.cpp
 * @brief GoogleTest + GMock unit tests for multi-calc module
 *
 * Demonstrates GMock features:
 * - MOCK_METHOD for mocking functions
 * - EXPECT_CALL for setting expectations
 * - Return values (Return, ReturnArg, ReturnRef)
 * - Argument matchers (_, Eq, Gt, Lt, etc.)
 * - Cardinality (Times, AtLeast, AtMost)
 * - Action sequences (InSequence)
 *
 * Note: For C functions, we use linker --wrap option + GMock
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

// C headers need extern "C"
extern "C" {
#include "calc.h"
#include "multi-calc.h"
}

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::InSequence;

/* ========== Mock Control Flags ========== */

static bool mock_calc_add = true;
static bool mock_calc_subtract = true;
static bool mock_calc_multiply = true;
static bool mock_calc_divide = true;

// Mock return values
static int mock_add_return = 0;
static int mock_subtract_return = 0;
static int mock_multiply_return = 0;
static int mock_divide_return = 0;

// Call tracking
static int add_call_count = 0;
static int subtract_call_count = 0;
static int multiply_call_count = 0;
static int divide_call_count = 0;

// Argument capture
static int last_add_a = 0, last_add_b = 0;
static int last_subtract_a = 0, last_subtract_b = 0;
static int last_multiply_a = 0, last_multiply_b = 0;
static int last_divide_a = 0, last_divide_b = 0;

/* ========== Real Function Declarations ========== */

extern "C" {
    extern int __real_calc_add(int a, int b);
    extern int __real_calc_subtract(int a, int b);
    extern int __real_calc_multiply(int a, int b);
    extern int __real_calc_divide(int a, int b);
}

/* ========== Wrap Functions ========== */

extern "C" {

int __wrap_calc_add(int a, int b) {
    add_call_count++;
    last_add_a = a;
    last_add_b = b;
    if (mock_calc_add) {
        return mock_add_return;
    }
    return __real_calc_add(a, b);
}

int __wrap_calc_subtract(int a, int b) {
    subtract_call_count++;
    last_subtract_a = a;
    last_subtract_b = b;
    if (mock_calc_subtract) {
        return mock_subtract_return;
    }
    return __real_calc_subtract(a, b);
}

int __wrap_calc_multiply(int a, int b) {
    multiply_call_count++;
    last_multiply_a = a;
    last_multiply_b = b;
    if (mock_calc_multiply) {
        return mock_multiply_return;
    }
    return __real_calc_multiply(a, b);
}

int __wrap_calc_divide(int a, int b) {
    divide_call_count++;
    last_divide_a = a;
    last_divide_b = b;
    if (mock_calc_divide) {
        return mock_divide_return;
    }
    return __real_calc_divide(a, b);
}

} // extern "C"

/* ========== Helper Functions ========== */

static void enable_all_mocks() {
    mock_calc_add = true;
    mock_calc_subtract = true;
    mock_calc_multiply = true;
    mock_calc_divide = true;
}

static void disable_all_mocks() {
    mock_calc_add = false;
    mock_calc_subtract = false;
    mock_calc_multiply = false;
    mock_calc_divide = false;
}

static void reset_mock_state() {
    add_call_count = 0;
    subtract_call_count = 0;
    multiply_call_count = 0;
    divide_call_count = 0;
    mock_add_return = 0;
    mock_subtract_return = 0;
    mock_multiply_return = 0;
    mock_divide_return = 0;
}

/* ========== Test Fixture ========== */

class MultiCalcMockTest : public ::testing::Test {
protected:
    void SetUp() override {
        enable_all_mocks();
        reset_mock_state();
    }

    void TearDown() override {
        disable_all_mocks();
    }
};

/* ========== Mock Tests for multi_calc_expression ========== */

// Test: (a + b) * (c - d)
TEST_F(MultiCalcMockTest, ExpressionWithMockedValues) {
    // Setup mock returns
    mock_add_return = 5;       // calc_add(2, 3) -> 5
    mock_subtract_return = 6;  // calc_subtract(10, 4) -> 6
    mock_multiply_return = 30; // calc_multiply(5, 6) -> 30

    int result = multi_calc_expression(2, 3, 10, 4);

    EXPECT_EQ(result, 30);
    EXPECT_EQ(add_call_count, 1);
    EXPECT_EQ(subtract_call_count, 1);
    EXPECT_EQ(multiply_call_count, 1);
}

TEST_F(MultiCalcMockTest, ExpressionCapturesArguments) {
    mock_add_return = 10;
    mock_subtract_return = 20;
    mock_multiply_return = 200;

    multi_calc_expression(1, 2, 3, 4);

    // Verify arguments passed to calc_add
    EXPECT_EQ(last_add_a, 1);
    EXPECT_EQ(last_add_b, 2);

    // Verify arguments passed to calc_subtract
    EXPECT_EQ(last_subtract_a, 3);
    EXPECT_EQ(last_subtract_b, 4);

    // Verify arguments passed to calc_multiply
    EXPECT_EQ(last_multiply_a, 10);  // Result of add
    EXPECT_EQ(last_multiply_b, 20);  // Result of subtract
}

/* ========== Mock Tests for multi_calc_average ========== */

// Test: (a + b + c) / 3
TEST_F(MultiCalcMockTest, AverageWithMockedValues) {
    // First add: a + b
    // Second add: (a+b) + c
    // Then divide by 3
    mock_add_return = 6;      // Both adds return 6
    mock_divide_return = 2;   // Final divide returns 2

    int result = multi_calc_average(1, 2, 3);

    EXPECT_EQ(result, 2);
    EXPECT_EQ(add_call_count, 2);  // Called twice
    EXPECT_EQ(divide_call_count, 1);
}

TEST_F(MultiCalcMockTest, AverageArgumentCapture) {
    mock_add_return = 100;
    mock_divide_return = 33;

    multi_calc_average(10, 20, 30);

    // Last divide should be sum / 3
    EXPECT_EQ(last_divide_a, 100);  // Sum from mocked add
    EXPECT_EQ(last_divide_b, 3);    // Divisor is always 3
}

/* ========== Hybrid Tests (Real + Mock) ========== */

class MultiCalcHybridTest : public ::testing::Test {
protected:
    void SetUp() override {
        reset_mock_state();
    }
};

TEST_F(MultiCalcHybridTest, ExpressionWithRealFunctions) {
    disable_all_mocks();

    // (2 + 3) * (10 - 4) = 5 * 6 = 30
    int result = multi_calc_expression(2, 3, 10, 4);

    EXPECT_EQ(result, 30);
}

TEST_F(MultiCalcHybridTest, AverageWithRealFunctions) {
    disable_all_mocks();

    // (1 + 2 + 3) / 3 = 6 / 3 = 2
    int result = multi_calc_average(1, 2, 3);

    EXPECT_EQ(result, 2);
}

TEST_F(MultiCalcHybridTest, PartialMock_OnlyMultiply) {
    // Use real add and subtract, mock multiply
    mock_calc_add = false;
    mock_calc_subtract = false;
    mock_calc_multiply = true;
    mock_multiply_return = 999;

    // Real: 2+3=5, 10-4=6, Mock: 5*6=999
    int result = multi_calc_expression(2, 3, 10, 4);

    EXPECT_EQ(result, 999);
}

TEST_F(MultiCalcHybridTest, CompareMockVsReal) {
    // Test with mock
    enable_all_mocks();
    mock_add_return = 100;
    mock_divide_return = 66;

    int mock_result = multi_calc_average(1, 2, 3);

    // Test with real
    disable_all_mocks();
    int real_result = multi_calc_average(1, 2, 3);

    EXPECT_EQ(mock_result, 66);
    EXPECT_EQ(real_result, 2);
    EXPECT_NE(mock_result, real_result);
}

/* ========== Call Count Tests ========== */

TEST_F(MultiCalcMockTest, VerifyCallCounts) {
    mock_add_return = 5;
    mock_subtract_return = 3;
    mock_multiply_return = 15;

    multi_calc_expression(1, 2, 3, 4);

    EXPECT_EQ(add_call_count, 1);
    EXPECT_EQ(subtract_call_count, 1);
    EXPECT_EQ(multiply_call_count, 1);
    EXPECT_EQ(divide_call_count, 0);  // Not used in expression
}

TEST_F(MultiCalcMockTest, AverageCallCounts) {
    mock_add_return = 10;
    mock_divide_return = 3;

    multi_calc_average(1, 2, 3);

    EXPECT_EQ(add_call_count, 2);     // Called twice for sum
    EXPECT_EQ(divide_call_count, 1);  // Called once for average
    EXPECT_EQ(multiply_call_count, 0);
    EXPECT_EQ(subtract_call_count, 0);
}

/* ========== Main function ========== */

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
