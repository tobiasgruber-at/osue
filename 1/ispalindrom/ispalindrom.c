#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

void print_usage(void) {
    printf("Usage: isPalindrom [-s] [-i] [-o outfile] [file...]");
}

char *remove_newline(char line[]) {
    if (line[strlen(line) - 1] == 10) {
        line[strlen(line) - 1] = '\0';
    }
    return line;
}

void trim(char res[], char line[]) {
    /*int res_i = 0;
    for (int i = 0; i < strlen(line); ++i) {
        if (line[i] != 32) {
            res[res_i] = line[i];
            res_i++;
        }
    }*/

    /*//printf("%lu\n", sizeof(char) * (strlen(*line) + 1));
    char *res_p = (char *) malloc(sizeof(char) * (strlen(*line) + 1));
    printf("1");
    char *cur_p = res_p;
    printf("2");
    for (int i = 0; i < strlen(*line); ++i) {
        if ((*line)[i] != 32) {
            *cur_p = (*line)[i];
            cur_p++;
        }
    }
    *cur_p = '\0';
    *line = res_p;
    printf("1: %s", res_p);*/
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
        exit(1);
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
            exit(1);
        }
        //printf("2: %s\n", output);
        strcat(output, res);
    }
    fclose(fp);
    return output;
}

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
        print_usage();
        exit(1);
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
}