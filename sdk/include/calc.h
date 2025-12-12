#ifndef __CALC_H__
#define __CALC_H__

/**
 * Add two integers
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 */
int calc_add(int a, int b);

/**
 * Subtract two integers
 * @param a First operand
 * @param b Second operand
 * @return Difference of a and b (a - b)
 */
int calc_subtract(int a, int b);

/**
 * Multiply two integers
 * @param a First operand
 * @param b Second operand
 * @return Product of a and b
 */
int calc_multiply(int a, int b);

/**
 * Divide two integers
 * @param a Dividend
 * @param b Divisor (should not be zero)
 * @return Quotient of a and b (a / b)
 * @note Returns 0 if divisor is 0
 */
int calc_divide(int a, int b);

#endif /* __CALC_H__ */