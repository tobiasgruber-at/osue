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
    struct Options options = {0, 0, 0, NULL};
    evaluate_options(argc, argv, &options);
    int has_input_files = optind < argc;
    if (has_input_files) {
        char *output = NULL;
        for(; optind < argc; optind++){
            char *file_path = argv[optind];
            FILE *fp = fopen(file_path, "r");
            if (fp == NULL) {
                exit(EXIT_FAILURE);
            }
            if (evaluate_file(fp, 1, &output, &options) == -1) {
                if (output != NULL) {
                    free(output);
                }
                fclose(fp);
                exit(EXIT_FAILURE);
            }
            fclose(fp);
        }
        if (options.output_to_file) {
            if (append_to_file(options.output_file, output) == -1) {
                if (output != NULL) {
                    free(output);
                }
                exit(EXIT_FAILURE);
            }
        } else {
            printf("%s", output);
        }
    } else {
        if (evaluate_file(stdin, 0, NULL, &options) == -1) {
            exit(EXIT_FAILURE);
        }
    }
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
                options->output_file = optarg;
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

int evaluate_file(FILE *fp, int output_type, char **dst_p, struct Options *options) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) > 0) {
        remove_newline(line);
        char evaluated[strlen(line) + 1];
        strcpy(evaluated, line);
        if (strlen(evaluated) <= 0) {
            continue;
        }
        if (options->ignore_whitespaces) {
            char temp[strlen(evaluated) + 1];
            strcpy(temp, evaluated);
            trim(evaluated, temp);
        }
        if (options->ignore_casing) {
            to_lower(evaluated);
        }
        char palindromSuffix[] = " is a palindrom\n";
        char noPalindromSuffix[] = " is not a palindrom\n";
        char line_res[strlen(line) + strlen(noPalindromSuffix) + 1];
        strcpy(line_res, line);
        strcat(line_res, is_palindrom(evaluated) ? palindromSuffix : noPalindromSuffix);
        if (output_type == 0) {
            printf("%s", line_res);
            if (options->output_file != NULL) {
                if (append_to_file(options->output_file, line_res) == -1) {
                    free(line);
                    return -1;
                }
            }
        } else if (output_type == 1) {
            size_t dst_size = sizeof(char) * ((*dst_p == NULL ? 0 : strlen(*dst_p)) + strlen(line_res) + 1);
            char *tmp = (char *) realloc(*dst_p, dst_size);
            if (tmp == NULL) {
                free(line);
                fprintf(stderr, "[%s] ERROR: malloc failed: %s\n", prog_name, strerror(errno));
                return -1;
            }
            *dst_p = tmp;
            strcat(*dst_p, line_res);
        }
    }
    free(line);
    return 0;
}

int is_palindrom(char src[]) {
    for (int i = 0; i < strlen(src) / 2 + 1; i++) {
        if (src[i] != src[strlen(src) - 1 - i]) {
            return 0;
        }
    }
    return 1;
}

int append_to_file(char* file_path, char *src) {
    FILE *fp = fopen(file_path, "a");
    if (fp == NULL) {
        fprintf(stderr, "[%s] ERROR: fopen failed for file '%s': %s\n", prog_name, file_path, strerror(errno));
        return -1;
    }
    if (fprintf(fp, "%s", src) == EOF) {
        fprintf(stderr, "[%s] ERROR: fputs failed for file '%s': %s\n", prog_name, file_path, strerror(errno));
        return -1;
    }
    fclose(fp);
    return 0;
}

void usage(void) {
    fprintf(stderr, "Usage: %s [-s] [-i] [-o outfile] [file...]\n", prog_name);
    exit(EXIT_FAILURE);
}
