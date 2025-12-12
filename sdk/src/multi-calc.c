#include "multi-calc.h"
#include "calc.h"

int multi_calc_expression(int a, int b, int c, int d) {
    // Calculate (a + b) * (c - d)
    int sum = calc_add(a, b);           // a + b
    int diff = calc_subtract(c, d);     // c - d
    int result = calc_multiply(sum, diff);  // (a + b) * (c - d)
    return result;
}

int multi_calc_average(int a, int b, int c) {
    // Calculate (a + b + c) / 3
    int sum1 = calc_add(a, b);          // a + b
    int sum2 = calc_add(sum1, c);       // (a + b) + c
    int result = calc_divide(sum2, 3);  // (a + b + c) / 3
    return result;
}