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
            output = check_file(file_to_read, ignore_casing, ignore_whitespaces);
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

char *check_file(char *file, int ignore_casing, int ignore_whitespaces) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    char *line = NULL;
    char *output = (char*) malloc(sizeof(char));
    if (output == NULL) {
        exit(EXIT_FAILURE);
    }
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1) {
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
        char res[strlen(line) + strlen(noPalindromSuffix) + 1];
        strcpy(res, line);
        if (is_palindrom(evaluated)) {
            strcat(res, palindromSuffix);
        } else {
            strcat(res, noPalindromSuffix);
        }
        char *tmp = (char *) realloc(output, (strlen(output) + strlen(res) + 1) * sizeof(*output));
        if (tmp == NULL) {
            free(output);
            exit(EXIT_FAILURE);
        }
        output = tmp;
        //printf("2: %s\n", output);
        strcat(output, res);
    }
    fclose(fp);
    return output;
}

int is_palindrom(char line[]) {
    for (int i = 0; i < strlen(line) / 2 + 1; i++) {
        //printf("%i: %c = %c", i, line[i], line[strlen(line) - 1 - i]);
        if (line[i] != line[strlen(line) - 1 - i]) {
            return 0;
        }
    }
    return 1;
}

void print_usage(char program_name[]) {
    printf("Usage: %s [-s] [-i] [-o outfile] [file...]\n", program_name);
}