/**
 * Miscellaneous module definitions.
 * @brief Covers miscellaneous operations.
 * @details Includes operations from handling errors to parsing integers.
 * @file misc.h
 * @author Tobias Gruber, 11912367
 * @date 30.10.2022
 **/

char *prog_name; /**> The programs name. */

/**
 * @brief Logs an error.
 * @details Prints an error message to stderr.<br>
 * Used global variables: prog_name
 * @param msg
 * @return -1
 */
int m_err(char *msg);

/**
 * @brief Traces an error.
 * @details Prints to stderr that a function was failing.<br>
 * Used global variables: prog_name
 * @param fun_name Name of the failed function.
 * @return -1
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
 * @brief Removes the newline at the end of a string.
 * @details Replaces the newline character at the end of src with a null terminator.
 * @param src String that should be manipulated.
 */
void remove_newline(char src[]);

/**
 * @brief Parses an integer from a string.
 * @details Validates if the string is a positive integer.
 * @param dst Pointer to be updated with the parsed integer.
 * @param src String to be parsed.
 * @return 0 on success, -1 on error.
 */
int parse_int(int *dst, char *src, int base);

int parse_c_int(int *dst, char src, int base);

/** Gets the max of two numbers. */
int max(int a, int b);

void free_arr(char *x[], int len);