#include "ispalindrom.h"
#include "strfun.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    int option, ignore_casing, ignore_whitespaces, output_to_file;
    ignore_casing = ignore_whitespaces = output_to_file = 0;
    char *output_file;
    while((option = getopt(argc, argv, "sio:")) > 0) {
        switch(option) {
            case 's': {
                if (ignore_whitespaces) {
                    print_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                ignore_whitespaces = 1;
                //printf("ignore whitespaces\n");
                break;
            }
            case 'i': {
                if (ignore_casing) {
                    print_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                ignore_casing = 1;
                //printf("ignoring casing\n");
                break;
            }
            case 'o': {
                if (output_to_file) {
                    print_usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                output_to_file = 1;
                output_file = optarg;
                //printf("outputting to file: %s\n", output_file);
                break;
            }
            default: {
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }
    int remaining_arguments = argc - optind;
    if (remaining_arguments > 0) {
        for(; optind < argc; optind++){
            char *file_to_read = argv[optind];
            char *output = NULL;
            if (check_file(&output, file_to_read, ignore_casing, ignore_whitespaces) == -1) {
                exit(EXIT_FAILURE);
            }
            if (output_to_file) {
                // write to output file
            } else {
                printf("%s", output);
            }
            free(output);
        }
    } else {
        //sscanf("%s", )
    }
    return EXIT_SUCCESS;
}

int check_file(char **dst_p, char *file, int ignore_casing, int ignore_whitespaces) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        return -1;
    }
    char *line = NULL;
    size_t len = 0, dst_size = 0;
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
        dst_size += sizeof(char) * (strlen(line_res) + 1);
        char *tmp = (char *) ((*dst_p == NULL) ? malloc(dst_size) : realloc(*dst_p, dst_size));
        if (tmp == NULL) {
            free(line);
            if (*dst_p != NULL) {
                free(*dst_p);
            }
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

void print_usage(char program_name[]) {
    printf("Usage: %s [-s] [-i] [-o outfile] [file...]\n", program_name);
}
