/**
 * @file test_multi_calc.cpp
 * @brief GoogleTest + mockcpp unit tests for multi-calc module
 *
 * Demonstrates mockcpp features:
 * - MOCKER() for mocking C functions (runtime API hooking)
 * - .stubs() / .expects() for setting expectations
 * - .with() for argument matching
 * - .will(returnValue()) for setting return values
 * - GlobalMockObject::verify() / reset()
 *
 * Note: mockcpp uses runtime API hooking, no --wrap linker option needed!
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

// C headers need extern "C"
extern "C" {
#include "calc.h"
#include "multi-calc.h"
}

USING_MOCKCPP_NS

/* ========== Test Fixture ========== */

class MultiCalcMockcppTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset all mocks before each test
        GlobalMockObject::reset();
    }

    void TearDown() override {
        // Verify all expectations and reset
        GlobalMockObject::verify();
        GlobalMockObject::reset();
    }
};

/* ========== Real Function Tests (No Mock) ========== */

// Test: without MOCKER, real function is called
TEST_F(MultiCalcMockcppTest, NoMockCallsRealFunction) {
    // No MOCKER() here - real calc_add will be called
    int result = calc_add(2, 3);

    // Real implementation: 2 + 3 = 5
    EXPECT_EQ(result, 5);
}

/* ========== Basic Mock Tests ========== */

// Test: mock calc_add to return a fixed value
TEST_F(MultiCalcMockcppTest, MockCalcAddReturnFixedValue) {
    // Setup mock: calc_add always returns 100
    MOCKER(calc_add)
        .stubs()
        .will(returnValue(100));

    // Call the function
    int result = calc_add(1, 2);

    // Verify result
    EXPECT_EQ(result, 100);
}

// Test: mock with argument matching
TEST_F(MultiCalcMockcppTest, MockWithArgumentMatching) {
    // Setup mock: only when calc_add(2, 3) is called, return 999
    MOCKER(calc_add)
        .stubs()
        .with(eq(2), eq(3))
        .will(returnValue(999));

    // This call matches the expectation
    int result = calc_add(2, 3);
    EXPECT_EQ(result, 999);
}

// Test: mock with any argument
TEST_F(MultiCalcMockcppTest, MockWithAnyArgument) {
    // Setup mock: calc_multiply with any arguments returns 42
    MOCKER(calc_multiply)
        .stubs()
        .with(any(), any())
        .will(returnValue(42));

    EXPECT_EQ(calc_multiply(1, 2), 42);
    EXPECT_EQ(calc_multiply(100, 200), 42);
    EXPECT_EQ(calc_multiply(0, 0), 42);
}

/* ========== Expression Mock Tests ========== */

// Test: mock all calc functions for multi_calc_expression
// Expression: (a + b) * (c - d)
TEST_F(MultiCalcMockcppTest, ExpressionWithMockedFunctions) {
    // Mock calc_add to return 5 (simulating 2 + 3 = 5)
    MOCKER(calc_add)
        .stubs()
        .will(returnValue(5));

    // Mock calc_subtract to return 6 (simulating 10 - 4 = 6)
    MOCKER(calc_subtract)
        .stubs()
        .will(returnValue(6));

    // Mock calc_multiply to return 30 (simulating 5 * 6 = 30)
    MOCKER(calc_multiply)
        .stubs()
        .will(returnValue(30));

    // Call the expression function
    int result = multi_calc_expression(2, 3, 10, 4);

    // Verify result
    EXPECT_EQ(result, 30);
}

/* ========== Average Mock Tests ========== */

// Test: mock for multi_calc_average
// Average: (a + b + c) / 3
TEST_F(MultiCalcMockcppTest, AverageWithMockedFunctions) {
    // Mock calc_add to return 15 (for both additions)
    MOCKER(calc_add)
        .stubs()
        .will(returnValue(15));

    // Mock calc_divide to return 5
    MOCKER(calc_divide)
        .stubs()
        .will(returnValue(5));

    // Call the average function
    int result = multi_calc_average(3, 5, 7);

    // Verify result
    EXPECT_EQ(result, 5);
}

/* ========== Invocation Count Tests ========== */

// Test: verify function is called exactly once
TEST_F(MultiCalcMockcppTest, ExpectsCalledOnce) {
    MOCKER(calc_add)
        .expects(once())
        .will(returnValue(10));

    // Call once
    calc_add(1, 2);

    // verify() in TearDown will check the expectation
}

// Test: verify function is called exactly N times
TEST_F(MultiCalcMockcppTest, ExpectsCalledExactlyTwice) {
    MOCKER(calc_add)
        .expects(exactly(2))
        .will(returnValue(10));

    // Call twice
    calc_add(1, 2);
    calc_add(3, 4);

    // verify() in TearDown will check the expectation
}

/* ========== Return Value Sequence ========== */

// Test: return different values on consecutive calls
TEST_F(MultiCalcMockcppTest, ReturnValueSequence) {
    MOCKER(calc_add)
        .stubs()
        .will(returnValue(10))
        .then(returnValue(20))
        .then(returnValue(30));

    EXPECT_EQ(calc_add(0, 0), 10);  // First call
    EXPECT_EQ(calc_add(0, 0), 20);  // Second call
    EXPECT_EQ(calc_add(0, 0), 30);  // Third call
    EXPECT_EQ(calc_add(0, 0), 30);  // Fourth call (repeats last)
}

/* ========== Complex Scenario ========== */

// Test: complex scenario with multiple mocks
TEST_F(MultiCalcMockcppTest, ComplexScenario) {
    // Setup different return values based on arguments
    MOCKER(calc_add)
        .stubs()
        .with(eq(1), eq(2))
        .will(returnValue(100));

    MOCKER(calc_add)
        .stubs()
        .with(eq(3), eq(4))
        .will(returnValue(200));

    MOCKER(calc_subtract)
        .stubs()
        .will(returnValue(50));

    MOCKER(calc_multiply)
        .stubs()
        .will(returnValue(1000));

    // Test with first set of arguments
    // Expression: (1 + 2) * (5 - 3) = 100 * 50 = 1000
    int result1 = multi_calc_expression(1, 2, 5, 3);
    EXPECT_EQ(result1, 1000);
}

/* ========== Main function ========== */

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
