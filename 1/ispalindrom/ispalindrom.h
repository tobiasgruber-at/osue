int main(int argc, char *argv[]);

/**
 * Checks if a string is a palindrom.
 * @param line String to be checked
 * @return 1 if string is a palindrom, and 0 if not.
 */
int is_palindrom(char line[]);

/**
 * Reads and checks every line of a file for palindroms.
 * @param file Path to the file.
 * @return Evaluation for every line of the file.
 */
char *check_file(char *file);

/**
 * Prints the usage of the program.
 * @param program_name Name of this program.
 */
void print_usage(char program_name[]);