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
 * @brief Multiplies two hex numbers.
 * @details x and y must not exceed a size of 8 bytes (size of an integer).<br>
 * Transforms res to have a length that is a power of two.
 * Allocates necessary memory for <strong>res</strong>.
 * @param dst Pointer to be updated with the product as a string.
 * @param x First hex number string.
 * @param y Second hex number string.
 * @return 0 on success, -1 on error.
 */
int multiply(char **dst, char *x, char *y);

/**
 * @brief Checks if a string is a valid hexadecimal number.
 * @details Checks if every character is hexadecimal.
 * @param str String to be validated.
 * @return 0 if valid, -1 if invalid.
 */
int is_hex(char *str);

/**
 * Copies half of a string to another string.<br>
 * <strong>dst</strong> must have enough allocated memory.
 * @param dst String to be updated.
 * @param src String to be copied.
 * @param half 0 if the first half should be copied, 1 for the second half.
 * @param half_length Half length of the source string.
 */
void half_str(char *dst, char *src, int half, int half_length);

/**
 * Fills up a hexadecimal number with leading zeroes.
 * @details Allocates necessary memory for <strong>str</strong>.
 * @param str Pointer to be updated with the resulting string.
 * @param min_len Length of the new string. It might become even longer if pow_of_two is true.
 * @param pow_of_two Whether the new string should have a length that is a power of two.
 * @return 0 on success, -1 on error.
 */
int fill_zeroes(char **str, int min_len, bool pow_of_two);

/**
 * Shifts a hex number n digits to the left.
 * @details Frees existing and allocates new memory for <strong>str</strong>.
 * @param str Pointer to be updated with the resulting string.
 * @param n Digits to be shifted.
 * @return 0 on success, -1 on error.
 */
int shift_left(char **str, int n);

/**
 * Calculates the sum of two hex numbers.
 * @details Transforms both numbers to have the same length and iteratively sums up every digit
 * of both from right to left.<br>
 * Allocates necessary memory for <strong>res</strong>.
 * @param res Pointer to be updated with the resulting string.
 * @param x Pointer to the first hexadecimal string.
 * @param y Pointer to the second hexadecimal string.
 * @return 0 on success, -1 on error.
 */
int add(char **res, char **x, char **y);