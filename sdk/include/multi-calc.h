#ifndef __MULTI_CALC_H__
#define __MULTI_CALC_H__

/**
 * Calculate expression: (a + b) * (c - d)
 * @param a First operand
 * @param b Second operand
 * @param c Third operand
 * @param d Fourth operand
 * @return Result of (a + b) * (c - d)
 *
 * @note This function depends on calc_add, calc_subtract, calc_multiply
 */
int multi_calc_expression(int a, int b, int c, int d);

/**
 * Calculate average of three integers
 * @param a First number
 * @param b Second number
 * @param c Third number
 * @return Average of a, b, c (integer division)
 *
 * @note This function depends on calc_add, calc_divide
 */
int multi_calc_average(int a, int b, int c);

#endif /* __MULTI_CALC_H__ */