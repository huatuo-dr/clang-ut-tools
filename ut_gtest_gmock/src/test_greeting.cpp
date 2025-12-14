/**
 * @file test_greeting.cpp
 * @brief GoogleTest unit tests for greeting module
 *
 * Demonstrates:
 * - String assertions (EXPECT_STREQ, EXPECT_STRNE)
 * - Pointer assertions (EXPECT_NE with nullptr)
 * - SetUpTestSuite / TearDownTestSuite (class-level setup)
 * - Custom test output
 */

#include <gtest/gtest.h>
#include <cstring>

// C header needs extern "C"
extern "C" {
#include "greeting.h"
}

/* ========== Basic String Tests ========== */

TEST(SayHelloTest, WithName) {
    const char *result = say_hello("Alice");
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "Hello, Alice!");
}

TEST(SayHelloTest, WithDifferentNames) {
    EXPECT_STREQ(say_hello("Bob"), "Hello, Bob!");
    EXPECT_STREQ(say_hello("Charlie"), "Hello, Charlie!");
    EXPECT_STREQ(say_hello("World"), "Hello, World!");
}

TEST(SayHelloTest, WithEmptyString) {
    const char *result = say_hello("");
    ASSERT_NE(result, nullptr);
    // Our implementation returns "Hello, stranger!" for empty string
    EXPECT_STREQ(result, "Hello, stranger!");
}

TEST(SayHelloTest, WithNull) {
    const char *result = say_hello(nullptr);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "Hello, stranger!");
}

/* ========== say_goodbye Tests ========== */

TEST(SayGoodbyeTest, WithName) {
    const char *result = say_goodbye("Alice");
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "Goodbye, Alice!");
}

TEST(SayGoodbyeTest, WithEmptyString) {
    const char *result = say_goodbye("");
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "Goodbye, stranger!");
}

TEST(SayGoodbyeTest, WithNull) {
    const char *result = say_goodbye(nullptr);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "Goodbye, stranger!");
}

/* ========== Test Fixture with Suite Setup ========== */

class GreetingFixtureTest : public ::testing::Test {
protected:
    static int test_counter;
    char *buffer;

    // Called once before all tests in this fixture
    static void SetUpTestSuite() {
        test_counter = 0;
        std::cout << "[Suite Setup] Greeting tests starting" << std::endl;
    }

    // Called once after all tests in this fixture
    static void TearDownTestSuite() {
        std::cout << "[Suite Teardown] Total tests run: " << test_counter << std::endl;
    }

    // Called before each test
    void SetUp() override {
        buffer = new char[256];
        test_counter++;
    }

    // Called after each test
    void TearDown() override {
        delete[] buffer;
        buffer = nullptr;
    }
};

int GreetingFixtureTest::test_counter = 0;

TEST_F(GreetingFixtureTest, BufferAllocated) {
    ASSERT_NE(buffer, nullptr);
    strcpy(buffer, say_hello("Test"));
    EXPECT_STREQ(buffer, "Hello, Test!");
}

TEST_F(GreetingFixtureTest, CounterIncremented) {
    // Counter should be at least 1 (this test)
    EXPECT_GE(test_counter, 1);
}

TEST_F(GreetingFixtureTest, CopyGreeting) {
    const char *greeting = say_goodbye("Fixture");
    strcpy(buffer, greeting);
    EXPECT_STREQ(buffer, "Goodbye, Fixture!");
}

/* ========== Edge Case Tests ========== */

TEST(GreetingEdgeCaseTest, LongName) {
    const char *long_name = "ThisIsAVeryLongNameThatMightCauseIssues";
    const char *result = say_hello(long_name);
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(strstr(result, long_name) != nullptr);
}

TEST(GreetingEdgeCaseTest, SpecialCharacters) {
    const char *special = "Test@123!";
    const char *result = say_hello(special);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "Hello, Test@123!!");
}

TEST(GreetingEdgeCaseTest, ReturnNotNull) {
    // All greeting functions should never return null
    EXPECT_NE(say_hello("test"), nullptr);
    EXPECT_NE(say_hello(""), nullptr);
    EXPECT_NE(say_hello(nullptr), nullptr);
    EXPECT_NE(say_goodbye("test"), nullptr);
    EXPECT_NE(say_goodbye(""), nullptr);
    EXPECT_NE(say_goodbye(nullptr), nullptr);
}

/* ========== String Assertion Demo ========== */

TEST(StringAssertionDemo, DifferentStringAssertions) {
    const char *hello = say_hello("Demo");

    // C-string comparisons
    EXPECT_STREQ(hello, "Hello, Demo!");           // Equal
    EXPECT_STRNE(hello, "Goodbye, Demo!");         // Not equal

    // Case-insensitive (not built-in, but showing the pattern)
    EXPECT_TRUE(strstr(hello, "Demo") != nullptr); // Contains substring

    // String length check
    EXPECT_GT(strlen(hello), 0u);                  // Not empty
}

/* ========== Main function ========== */

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
