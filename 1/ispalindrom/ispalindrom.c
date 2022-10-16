/**
 * @file ispalindrom.c
 * @author Tobias Gruber
 * @date 12.10.2022
 * @brief Implementation of the main module.
 */

#include "ispalindrom.h"
#include "strfun.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>

char *prog_name;

int main(int argc, char *argv[]) {
    prog_name = argv[0];
    struct Options options = {0, 0, 0, NULL, NULL};
    evaluate_options(argc, argv, &options);
    if (options.output_to_file) {
        options.output_fp = fopen(options.output_path, "w");
        if (options.output_fp == NULL) {
            fprintf(stderr, "[%s] ERROR: fopen failed for file '%s': %s\n", prog_name, options.output_path, strerror(errno));
            return -1;
        }
    }
    int has_input_files = optind < argc;
    if (has_input_files) {
        for(; optind < argc; optind++){
            char *file_path = argv[optind];
            FILE *input_fp = fopen(file_path, "r");
            if (input_fp == NULL) {
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

void evaluate_options(int argc, char *argv[], struct Options *options) {
    int option;
    while((option = getopt(argc, argv, "sio:")) > 0) {
        switch(option) {
            case 's': {
                ++options->ignore_whitespaces;
                break;
            }
            case 'i': {
                ++options->ignore_casing;
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

int evaluate_file(FILE *fp, struct Options *options) {
    char *line = NULL;
    size_t len = 0;
    while ((getline(&line, &len, fp)) > 0) {
        remove_newline(line);
        if (strlen(line) <= 0) continue;
        char evaluated[strlen(line) + 1];
        strcpy(evaluated, line);
        if (options->ignore_whitespaces) {
            char temp[strlen(evaluated) + 1];
            strcpy(temp, evaluated);
            trim(evaluated, temp);
        }
        if (options->ignore_casing) to_lower(evaluated);
        char palindromSuffix[] = " is a palindrom\n";
        char noPalindromSuffix[] = " is not a palindrom\n";
        char line_res[strlen(line) + strlen(noPalindromSuffix) + 1];
        strcpy(line_res, line);
        strcat(line_res, is_palindrom(evaluated) ? palindromSuffix : noPalindromSuffix);
        printf("%s", line_res);
        if (options->output_to_file) {
            if (fprintf(options->output_fp, "%s", line_res) < 0) {
                fprintf(stderr, "[%s] ERROR: fputs failed for file '%s': %s\n", prog_name, options->output_path, strerror(errno));
                free(line);
                return -1;
            }
        }
    }
    free(line);
    return 0;
}

int is_palindrom(char src[]) {
    for (int i = 0; i < strlen(src) / 2 + 1; i++) {
        if (src[i] != src[strlen(src) - 1 - i]) return 0;
    }
    return 1;
}

void usage(void) {
    fprintf(stderr, "Usage: %s [-s] [-i] [-o outfile] [file...]\n", prog_name);
    exit(EXIT_FAILURE);
}
