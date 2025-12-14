/**
 * @file test_calc.cpp
 * @brief GoogleTest unit tests for calc module
 *
 * Demonstrates:
 * - Basic assertions (EXPECT_EQ, ASSERT_EQ, EXPECT_TRUE, etc.)
 * - Test fixtures (TEST_F)
 * - Parameterized tests (TEST_P)
 * - Death tests (EXPECT_DEATH)
 */

#include <gtest/gtest.h>

// C header needs extern "C"
extern "C" {
#include "calc.h"
}

/* ========== Basic Tests (TEST macro) ========== */

// Test calc_add with positive numbers
TEST(CalcAddTest, PositiveNumbers) {
    EXPECT_EQ(calc_add(2, 3), 5);
    EXPECT_EQ(calc_add(100, 200), 300);
}

// Test calc_add with negative numbers
TEST(CalcAddTest, NegativeNumbers) {
    EXPECT_EQ(calc_add(-2, -3), -5);
    EXPECT_EQ(calc_add(-100, -200), -300);
}

// Test calc_add with mixed numbers
TEST(CalcAddTest, MixedNumbers) {
    EXPECT_EQ(calc_add(-5, 3), -2);
    EXPECT_EQ(calc_add(5, -3), 2);
}

// Test calc_add with zero
TEST(CalcAddTest, WithZero) {
    EXPECT_EQ(calc_add(0, 0), 0);
    EXPECT_EQ(calc_add(5, 0), 5);
    EXPECT_EQ(calc_add(0, 5), 5);
}

// Test calc_subtract
TEST(CalcSubtractTest, PositiveNumbers) {
    EXPECT_EQ(calc_subtract(5, 3), 2);
    EXPECT_EQ(calc_subtract(100, 50), 50);
}

TEST(CalcSubtractTest, NegativeResult) {
    EXPECT_EQ(calc_subtract(3, 5), -2);
}

// Test calc_multiply
TEST(CalcMultiplyTest, PositiveNumbers) {
    EXPECT_EQ(calc_multiply(3, 4), 12);
    EXPECT_EQ(calc_multiply(10, 10), 100);
}

TEST(CalcMultiplyTest, WithZero) {
    EXPECT_EQ(calc_multiply(5, 0), 0);
    EXPECT_EQ(calc_multiply(0, 5), 0);
}

TEST(CalcMultiplyTest, NegativeNumbers) {
    EXPECT_EQ(calc_multiply(-3, 4), -12);
    EXPECT_EQ(calc_multiply(-3, -4), 12);
}

// Test calc_divide
TEST(CalcDivideTest, ExactDivision) {
    EXPECT_EQ(calc_divide(10, 2), 5);
    EXPECT_EQ(calc_divide(100, 10), 10);
}

TEST(CalcDivideTest, Truncation) {
    EXPECT_EQ(calc_divide(7, 3), 2);
    EXPECT_EQ(calc_divide(10, 3), 3);
}

TEST(CalcDivideTest, DivideByZero) {
    // Division by zero returns 0 in our implementation
    EXPECT_EQ(calc_divide(10, 0), 0);
}

TEST(CalcDivideTest, NegativeNumbers) {
    EXPECT_EQ(calc_divide(-10, 2), -5);
    EXPECT_EQ(calc_divide(10, -2), -5);
    EXPECT_EQ(calc_divide(-10, -2), 5);
}

/* ========== Test Fixture (TEST_F macro) ========== */

// Test fixture class
class CalcFixtureTest : public ::testing::Test {
protected:
    int operand_a;
    int operand_b;

    // Called before each test
    void SetUp() override {
        operand_a = 10;
        operand_b = 5;
    }

    // Called after each test
    void TearDown() override {
        // Cleanup if needed
    }
};

TEST_F(CalcFixtureTest, AddWithFixture) {
    EXPECT_EQ(calc_add(operand_a, operand_b), 15);
}

TEST_F(CalcFixtureTest, SubtractWithFixture) {
    EXPECT_EQ(calc_subtract(operand_a, operand_b), 5);
}

TEST_F(CalcFixtureTest, MultiplyWithFixture) {
    EXPECT_EQ(calc_multiply(operand_a, operand_b), 50);
}

TEST_F(CalcFixtureTest, DivideWithFixture) {
    EXPECT_EQ(calc_divide(operand_a, operand_b), 2);
}

/* ========== Parameterized Tests (TEST_P macro) ========== */

// Test data structure
struct CalcAddTestData {
    int a;
    int b;
    int expected;
};

// Parameterized test fixture
class CalcAddParamTest : public ::testing::TestWithParam<CalcAddTestData> {
};

TEST_P(CalcAddParamTest, AdditionWorks) {
    const CalcAddTestData& data = GetParam();
    EXPECT_EQ(calc_add(data.a, data.b), data.expected);
}

// Test data
INSTANTIATE_TEST_SUITE_P(
    AdditionTests,
    CalcAddParamTest,
    ::testing::Values(
        CalcAddTestData{1, 1, 2},
        CalcAddTestData{0, 0, 0},
        CalcAddTestData{-1, 1, 0},
        CalcAddTestData{100, 200, 300},
        CalcAddTestData{-50, -50, -100}
    )
);

/* ========== Assertion Types Demo ========== */

TEST(AssertionDemo, DifferentAssertionTypes) {
    // EXPECT_* continues on failure, ASSERT_* stops the test
    EXPECT_EQ(calc_add(1, 1), 2);      // Equal
    EXPECT_NE(calc_add(1, 1), 3);      // Not equal
    EXPECT_LT(calc_add(1, 1), 3);      // Less than
    EXPECT_LE(calc_add(1, 1), 2);      // Less than or equal
    EXPECT_GT(calc_add(1, 1), 1);      // Greater than
    EXPECT_GE(calc_add(1, 1), 2);      // Greater than or equal

    EXPECT_TRUE(calc_add(1, 1) == 2);  // Boolean true
    EXPECT_FALSE(calc_add(1, 1) == 3); // Boolean false
}

/* ========== Main function ========== */

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
