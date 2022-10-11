int main(int argc, char *argv[]);

/**
 * Checks if a string is a palindrom.
 * @param line String to be checked
 * @return 1 if string is a palindrom, and 0 if not.
 */
int is_palindrom(char line[]);

/**
 * Performs a palindrom check for every line of a file.
 * @description Allocates memory for the returned pointer.
 * @param file Path to the file.
 * @param ignore_casing Whether letter casing should be ignored for the palindrom check.
 * @param ignore_whitespaces Whether whitespaces should be ignored for the palindrom check.
 * @return String of the evaluation for every line of the file.
 */
char *check_file(char *file, int ignore_casing, int ignore_whitespaces);

/**
 * Prints the usage of the program.
 * @param program_name Name of this program.
 */
void print_usage(char program_name[]);