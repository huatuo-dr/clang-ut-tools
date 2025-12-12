#include "calc.h"

int calc_add(int a, int b) {
    return a + b;
}

int calc_subtract(int a, int b) {
    return a - b;
}

int calc_multiply(int a, int b) {
    return a * b;
}

int calc_divide(int a, int b) {
    // Handle division by zero
    if (b == 0) {
        return 0;
    }
    return a / b;
}