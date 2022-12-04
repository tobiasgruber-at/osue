#include "misc.h"
#include "stdbool.h"

/** Multiplies two hex numbers. */
void multiply(long *dst, char *x, char *y);

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
 * Fills up a hexadecimal number with leading zeroes, until it's length is a power of two and at least a given
 * min-length long.
 */
int fill_zeroes(char **str, int min_len, bool pow_of_two);

int shift_left(char **str, int n);

int add(char **res, char *x, char *y);