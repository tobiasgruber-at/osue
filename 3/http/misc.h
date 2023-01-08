/**
 * Miscellaneous module definitions.
 * @brief Covers miscellaneous operations.
 * @details Includes operations for handling errors and parsing.
 * @file misc.h
 * @author Tobias Gruber, 11912367
 * @date 03.01.2023
 **/

#include <stdbool.h>

char *prog_name; /**> The programs name. */

/**
 * @brief Logs an error.
 * @details Prints an error message to stderr.<br>
 * Used global variables: prog_name
 * @param msg Error message.
 * @return -1.
 */
int m_err(char *msg);

/**
 * @brief Traces an error.
 * @details Prints to stderr that a function was failing.<br>
 * Used global variables: prog_name
 * @param fun_name Name of the failed function.
 * @return -1.
 */
int t_err(char *fun_name);

/**
 * @brief Prints an error and exits.
 * @details Prints the error with a trace to stderr and exits with EXIT_FAILURE.<br>
 * Used global variables: prog_name
 * @param fun_name Name of the failed function.
 */
void e_err(char *fun_name);

/**
 * @brief Parses an integer from a decimal number.
 * @details Validates if the string is a positive integer.
 * @param dst Pointer to be updated with the parsed integer. If NULL, it just returns whether it's an integer.
 * @param src String to be parsed of the hexadecimal number.
 * @return 0 on success and if valid, -1 on error or if invalid.
 */
int parse_int(int *dst, char *src);