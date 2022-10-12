int main(int argc, char *argv[]);

/**
 * Performs a palindrom check for every line of a file.
 * @description Allocates memory for dst argument.
 * @param dst Pointer to the string that should be updated with an evaluation report of each line.
 * @param file Path to the file.
 * @param ignore_casing Whether letter casing should be ignored for the palindrom check.
 * @param ignore_whitespaces Whether whitespaces should be ignored for the palindrom check.
 * @return 0 if execution was fine, and -1 if there was an error.
 */
int check_file(char **dst_p, char *file, int ignore_casing, int ignore_whitespaces);

/**
 * Checks if a string is a palindrom.
 * @param src String to be checked
 * @return 1 if string is a palindrom, and 0 if not.
 */
int is_palindrom(char src[]);

/**
 * Prints the usage of the program.
 * @param program_name Name of this program.
 */
void print_usage(char program_name[]);
