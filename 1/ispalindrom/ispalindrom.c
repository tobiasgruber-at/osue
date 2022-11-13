/**
 * The main module.
 * @brief Entry point of the program.
 * @details The program reads from stdin or other files and evaluates for each line, if it was a palindrom.<br>
 * The response will be printed to stdout and can additionally be written to a specified output file.
 * @file ispalindrom.c
 * @author Tobias Gruber, 11912367
 * @date 12.10.2022
 */

#include "strfun.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>

/**
 * @brief Options of the program.
 * @details Contains information about each accepted option of the program and their arguments.
 */
struct Options {
    int ignore_casing; /**< Whether letter spacing is ignored. */
    int ignore_whitespaces; /**< Whether whitespaces are ignored. */
    int output_to_file; /**< Whether the output should also be written to a file. */
    char *output_path; /**< Output file path. */
    FILE *output_fp; /**< Output file stream. */
};

static char *prog_name; /**< The program's name. */

/**
 * @brief Prints the usage of the program to stderr and exists with an error.
 * @details Exits the program with EXIT_FAILURE.<br>
 * Error handling of fprintf is not covered as the program has to exit anyway.<br>
 * Used global variables: prog_name
 */
static void usage(void) {
    fprintf(stderr, "Usage: %s [-s] [-i] [-o outfile] [file...]\n", prog_name);
    exit(EXIT_FAILURE);
}

/**
 * @brief Evaluates the program options.
 * @details Exits the program with EXIT_FAILURE on invalid input.
 * @param argc Counter of the argument.
 * @param argv Vector of the arguments.
 * @param options Pointer to options that is updated.
 */
static void evaluate_options(int argc, char *argv[], struct Options *options) {
    int option; /**< Currently evaluated option. */
    while((option = getopt(argc, argv, "iso:")) > 0) {
        switch(option) {
            case 'i': {
                ++options->ignore_casing;
                break;
            }
            case 's': {
                ++options->ignore_whitespaces;
                break;
            }
            case 'o': {
                ++options->output_to_file;
                options->output_path = optarg;
                break;
            }
            case '?':
            default: {
                usage();
            }
        }
    }
    if (options->ignore_whitespaces > 1 || options->ignore_casing > 1 || options->output_to_file > 1) {
        usage();
    }
}

/**
 * @brief Checks if a string is a palindrom.
 * @details Compares the leftmost and the rightmost characters and brings them with each step closer.<br>
 * Continue until every character was compared or unequal characters were found.
 * @param src String to be checked.
 * @return 1 if src is a palindrom, 0 otherwise.
 */
static int is_palindrom(char src[]) {
    for (int i = 0; i < strlen(src) / 2 + 1; i++) {
        if (src[i] != src[strlen(src) - 1 - i]) return 0;
    }
    return 1;
}

/**
 * @brief Performs a palindrom check for every line of a file.
 * @description Checks for each line if it is a palindrom.<br>
 * Based on the options whitespaces or letter casing is not taking into account, and the result is optionally
 * written to an output file.
 * @param fp Pointer to the read file.
 * @param options Program options.
 * @return 0 on success, -1 on error.
 */
static int evaluate_file(FILE *fp, struct Options *options) {
    char *line = NULL; /**< String of the current line. */
    size_t len = 0; /**< Length of the current line. */
    while ((getline(&line, &len, fp)) > 0) {
        remove_newline(line);
        if (strlen(line) <= 0) continue;
        char evaluated[strlen(line) + 1]; /**< Manipulated line (based on options) that will be evaluated. */
        strcpy(evaluated, line);
        if (options->ignore_whitespaces) {
            char temp[strlen(evaluated) + 1];
            strcpy(temp, evaluated);
            trim(evaluated, temp);
        }
        if (options->ignore_casing) to_lower(evaluated);
        char palindromSuffix[] = " is a palindrom\n"; /**< Line suffix, printed if palindrom. */
        char noPalindromSuffix[] = " is not a palindrom\n"; /**< Line suffix, printed if no palindrom. */
        char line_res[strlen(line) + strlen(noPalindromSuffix) + 1]; /**< Printed response for this line. */
        strcpy(line_res, line);
        strcat(line_res, is_palindrom(evaluated) ? palindromSuffix : noPalindromSuffix);
        if (options->output_to_file) {
            if (fprintf(options->output_fp, "%s", line_res) < 0) {
                fprintf(stderr, "[%s] ERROR: fprintf failed for file '%s': %s\n", prog_name, options->output_path, strerror(errno));
                free(line);
                return -1;
            }
        } else {
            printf("%s", line_res);
        }
    }
    free(line);
    return 0;
}

/**
 * @brief Entry point of the program.
 * @details Structures the procedure of the program based on given options and arguments.<br>
 * It reads from stdin or other files and evaluates for each line, if it was a palindrom.<br>
 * Based on the options, it respects casing and whitespaces for the input or not.<br>
 * The response will be printed to stdout and is optionally written to a specified output file.<br>
 * Exits the program with EXIT_FAILURE in case of errors.<br>
 * Used global variables: prog_name
 *
 * @param argc Argument counter.
 * @param argv Argument vector.
 * @return Exit status.
 * */
int main(int argc, char *argv[]) {
    prog_name = argv[0];
    struct Options options = {0, 0, 0, NULL, NULL}; /**< Program options. */
    evaluate_options(argc, argv, &options);
    if (options.output_to_file) {
        options.output_fp = fopen(options.output_path, "w");
        if (options.output_fp == NULL) {
            fprintf(stderr, "[%s] ERROR: fopen failed for file '%s': %s\n", prog_name, options.output_path, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    int has_input_files = optind < argc;
    if (has_input_files) {
        for(; optind < argc; optind++){
            char *file_path = argv[optind]; /**< Path to the input file. */
            FILE *input_fp = fopen(file_path, "r");
            if (input_fp == NULL) {
                fprintf(stderr, "[%s] ERROR: fopen failed for file '%s': %s\n", prog_name, file_path, strerror(errno));
                if (options.output_to_file) fclose(options.output_fp);
                exit(EXIT_FAILURE);
            }
            if (evaluate_file(input_fp, &options) == -1) {
                fclose(input_fp);
                if (options.output_to_file) fclose(options.output_fp);
                exit(EXIT_FAILURE);
            }
            fclose(input_fp);
        }
    } else {
        if (evaluate_file(stdin, &options) == -1) {
            if (options.output_to_file) fclose(options.output_fp);
            exit(EXIT_FAILURE);
        }
    }
    if (options.output_to_file) fclose(options.output_fp);
    return EXIT_SUCCESS;
}
