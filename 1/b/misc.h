char *prog_name; /**> The programs name. */

/**
 * @brief Traces an error.
 * @details Prints the failure of the function with additional information.
 * @param fun_name Name of the failed function.
 * @return -1
 */
int t_err(char *fun_name);

/**
 * @brief Prints error and exits.
 * @details Prints the error with a trace and exits with error code 1.
 * @param fun_name Name of the failed function.
 */
void e_err(char *fun_name);
