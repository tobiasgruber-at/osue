/**
 * @file ispalindrom.h
 * @author Tobias Gruber
 * @date 12.10.2022
 * @brief The main module.
 * @details The program reads files and performs a palindrom check on each line.
 */

#include <stdio.h>

struct Options {
    int ignore_casing;
    int ignore_whitespaces;
    int output_to_file;
    char *output_path;
    FILE *output_fp;
};

// TODO: add docs
int main(int argc, char *argv[]);

/**
 * Evaluates the program options.
 * @param argc Counter of the argument.
 * @param argv Vector of the arguments.
 * @param options Pointer to options that is updated.
 */
void evaluate_options(int argc, char *argv[], struct Options *options);

/**
 * Performs a palindrom check for every line of a file.
 * @description Probably allocates memory for dst argument.
 * @param fp Pointer to the file.
 * @param dst_p Pointer to the string that should be updated with an evaluation report of each line iff immediate
 * output is 1.
 * @param options Program options.
 * @return 0 if execution was fine, and -1 if there was an error.
 */
int evaluate_file(FILE *fp, struct Options *options);

/**
 * Checks if a string is a palindrom.
 * @param src String to be checked
 * @return 1 if string is a palindrom, and 0 if not.
 */
int is_palindrom(char src[]);

/**
 * Appends a given string to a file.
 * @param file_path Path to the file.
 * @param src String to write to the file.
 * @return 0 if execution was fine, and -1 if there was an error.
 */
int append_to_file(char* file_path, char *src);

/** Prints the usage of the program and exists with an error. */
void usage(void);
