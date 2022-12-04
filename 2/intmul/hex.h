/**
 * Hex module definitions.
 * @brief Covers operations on hexadecimal numbers.
 * @details Includes functions for hexadecimal arithmetic, validation and manipulation.
 * @file hex.h
 * @author Tobias Gruber, 11912367
 * @date 4.12.2022
 **/

#include "misc.h"
#include "stdbool.h"

/**
 * Multiplies two hex numbers.
 * @details x and y must not exceed a size of 8 bytes (size of an integer).
 * @param dst
 * @param x
 * @param y
 */
int multiply(char **dst, char *x, char *y);

/**
 * Checks if a string is a valid hex number.
 * @param str
 * @return
 */
int is_hex(char *str);

/**
 * Copies half of a string to another string.
 * @param dst Destination string.
 * @param src Source string.
 * @param half 0 if the first half should be copied, 1 for the second half.
 * @param half_length Half length of the source string.
 */
void half_str(char *dst, char *src, int half, int half_length);

/**
 * Fills up a hexadecimal number with leading zeroes.
 * min-length long.
 * @details Allocates memory for <strong>str</strong>.
 * @param str
 * @param min_len Length of the new string. Might get longer if pow_of_two is true.
 * @param pow_of_two Whether the new string should have a length that is a power of two.
 */
int fill_zeroes(char **str, int min_len, bool pow_of_two);

/**
 * Shifts a hex number n digits to the left.
 * @details Frees existing and allocates new memory for <strong>str</strong>.
 * @param str
 * @param n
 * @return
 */
int shift_left(char **str, int n);

/**
 * Calculates the sum of two hex numbers.
 * @param res
 * @param x
 * @param y
 * @return
 */
int add(char **res, char **x, char **y);