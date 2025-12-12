#include <stdio.h>
#include <stdlib.h>
#include "calc.h"
#include "greeting.h"
#include "multi-calc.h"

void test_calculator() {
    printf("\n=== Calculator Test ===\n");

    int a = 10, b = 3;
    printf("Testing with a = %d, b = %d\n", a, b);

    printf("calc_add(%d, %d) = %d\n", a, b, calc_add(a, b));
    printf("calc_subtract(%d, %d) = %d\n", a, b, calc_subtract(a, b));
    printf("calc_multiply(%d, %d) = %d\n", a, b, calc_multiply(a, b));
    printf("calc_divide(%d, %d) = %d\n", a, b, calc_divide(a, b));

    // Test division by zero
    printf("\nTesting division by zero:\n");
    printf("calc_divide(%d, 0) = %d (should return 0)\n", a, calc_divide(a, 0));
}

void test_greeting() {
    printf("\n=== Greeting Test ===\n");

    // Test with normal names
    printf("%s\n", say_hello("Alice"));
    printf("%s\n", say_goodbye("Alice"));

    printf("%s\n", say_hello("Bob"));
    printf("%s\n", say_goodbye("Bob"));

    // Test with empty string and NULL
    printf("\nTesting with empty/NULL names:\n");
    printf("%s\n", say_hello(""));
    printf("%s\n", say_goodbye(""));
    printf("%s\n", say_hello(NULL));
    printf("%s\n", say_goodbye(NULL));
}

void test_multi_calculator() {
    printf("\n=== Multi-Calculator Test ===\n");

    // Test multi_calc_expression: (a + b) * (c - d)
    int a = 2, b = 3, c = 10, d = 4;
    printf("Testing expression (a + b) * (c - d):\n");
    printf("  a = %d, b = %d, c = %d, d = %d\n", a, b, c, d);
    printf("  (%d + %d) * (%d - %d) = %d\n", a, b, c, d, multi_calc_expression(a, b, c, d));

    // Another example
    a = 5; b = 5; c = 8; d = 3;
    printf("  a = %d, b = %d, c = %d, d = %d\n", a, b, c, d);
    printf("  (%d + %d) * (%d - %d) = %d\n", a, b, c, d, multi_calc_expression(a, b, c, d));

    // Test multi_calc_average: (a + b + c) / 3
    printf("\nTesting average (a + b + c) / 3:\n");
    int x = 10, y = 20, z = 30;
    printf("  average(%d, %d, %d) = %d\n", x, y, z, multi_calc_average(x, y, z));

    x = 7; y = 8; z = 9;
    printf("  average(%d, %d, %d) = %d\n", x, y, z, multi_calc_average(x, y, z));

    // Edge case: result with truncation
    x = 1; y = 1; z = 1;
    printf("  average(%d, %d, %d) = %d\n", x, y, z, multi_calc_average(x, y, z));
}

int main(int argc, char *argv[]) {
    printf("====================================\n");
    printf("   CMocka Project - Application\n");
    printf("====================================\n");

    // If arguments provided, use them for greeting
    if (argc > 1) {
        printf("\nCustom greeting for: %s\n", argv[1]);
        printf("%s\n", say_hello(argv[1]));
        printf("%s\n", say_goodbye(argv[1]));
    }

    // Run tests
    test_calculator();
    test_multi_calculator();
    test_greeting();

    printf("\n====================================\n");
    printf("   Application finished successfully\n");
    printf("====================================\n");

    return 0;
}