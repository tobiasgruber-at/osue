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
    while((option = getopt(argc, argv, "sio:")) != -1) {
        switch(option) {
            case 's': {
                ignore_whitespaces = 1;
                //printf("ignore whitespaces\n");
                break;
            }
            case 'i': {
                ignore_casing = 1;
                //printf("ignoring casing\n");
                break;
            }
            case 'o': {
                output_to_file = 1;
                output_file = optarg;
                //printf("outputting to file: %s\n", output_file);
                break;
            }
            default: {}
        }
    }
    int remaining_arguments = argc - optind;
    if (remaining_arguments <= 0) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (optind < argc) {
        for(; optind < argc; optind++){
            char *file_to_read = argv[optind];
            char *output;
            output = check_file(file_to_read);
            if (output_to_file) {
                // write to output file
            } else {
                printf("%s", output);
            }
        }
    } else {
        //sscanf("%s", )
    }
    return EXIT_SUCCESS;
}

int is_palindrom(char line[]) {
    for (int i = 0; i < strlen(line) / 2 + 1; i++) {
        if (line[i] != line[strlen(line) - 1 - i]) {
            return 0;
        }
    }
    return 1;
}

char *check_file(char *file) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    char *line = NULL;
    char *output = (char*) malloc(sizeof(char));
    if (output == NULL) {
        free(output);
        exit(EXIT_FAILURE);
    }
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        char *res = remove_newline(line);
        if (strlen(res) <= 0) {
            continue;
        }
        if (is_palindrom(res)) {
            strcat(res, " is a palindrom\n");
        } else {
            strcat(res, " is not a palindrom\n");
        }
        output = (char *) realloc(output, (strlen(output) + strlen(res) + 1) * sizeof(*output));
        if (output == NULL) {
            free(output);
            exit(EXIT_FAILURE);
        }
        //printf("2: %s\n", output);
        strcat(output, res);
    }
    fclose(fp);
    return output;
}

void print_usage(char program_name[]) {
    printf("Usage: %s [-s] [-i] [-o outfile] [file...]", program_name);
}