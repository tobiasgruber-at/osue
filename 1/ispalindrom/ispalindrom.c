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
    int option, ignore_casing = 0, ignore_whitespaces = 0, output_to_file = 0;
    char *output_file = NULL;
    prog_name = argv[0];
    while((option = getopt(argc, argv, "sio:")) > 0) {
        switch(option) {
            case 's': {
                ++ignore_whitespaces;
                break;
            }
            case 'i': {
                ++ignore_casing;
                break;
            }
            case 'o': {
                ++output_to_file;
                output_file = optarg;
                break;
            }
            case '?':
            default: {
                usage();
            }
        }
    }
    if (ignore_whitespaces > 1 || ignore_casing > 1 || output_to_file > 1) {
        usage();
    }
    if (optind < argc) {
        char *output = NULL;
        for(; optind < argc; optind++){
            char *file_to_read = argv[optind];
            if (check_file(&output, file_to_read, ignore_casing, ignore_whitespaces) == -1) {
                if (output != NULL) {
                    free(output);
                }
                exit(EXIT_FAILURE);
            }
        }
        if (output_to_file) {
            if (save_to_file(output_file, output) == -1) {
                if (output != NULL) {
                    free(output);
                }
                exit(EXIT_FAILURE);
            }
        } else {
            printf("%s", output);
        }
        free(output);
    } else {
        // TODO: read user input from stdin
    }
    return EXIT_SUCCESS;
}

int check_file(char **dst_p, char *file_path, int ignore_casing, int ignore_whitespaces) {
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        fprintf(stderr, "[%s] ERROR: fopen failed for file '%s': %s\n", prog_name, file_path, strerror(errno));
        return -1;
    }
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
        if (ignore_whitespaces) {
            char temp[strlen(evaluated) + 1];
            strcpy(temp, evaluated);
            trim(evaluated, temp);
        }
        if (ignore_casing) {
            to_lower(evaluated);
        }
        char palindromSuffix[] = " is a palindrom\n";
        char noPalindromSuffix[] = " is not a palindrom\n";
        char line_res[strlen(line) + strlen(noPalindromSuffix) + 1];
        strcpy(line_res, line);
        strcat(line_res, is_palindrom(evaluated) ? palindromSuffix : noPalindromSuffix);
        size_t dst_size = sizeof(char) * ((*dst_p == NULL ? 0 : strlen(*dst_p)) + strlen(line_res) + 1);
        char *tmp = (char *) realloc(*dst_p, dst_size);
        if (tmp == NULL) {
            fclose(fp);
            free(line);
            fprintf(stderr, "[%s] ERROR: malloc failed: %s\n", prog_name, strerror(errno));
            return -1;
        }
        *dst_p = tmp;
        strcat(*dst_p, line_res);
    }
    free(line);
    fclose(fp);
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

int save_to_file(char* file_path, char *src) {
    FILE *fp = fopen(file_path, "w");
    if (fp == NULL) {
        fprintf(stderr, "[%s] ERROR: fopen failed for file '%s': %s\n", prog_name, file_path, strerror(errno));
        return -1;
    }
    if (fputs(src, fp) == EOF) {
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
