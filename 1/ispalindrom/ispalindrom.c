#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>

void printUsage(void) {
    printf("Usage: ");
}

int main(int argc, char *argv[]) {
    int option, ignoreCasing, ignoreWhitespaces;
    ignoreCasing = ignoreWhitespaces = 0;
    // TODO: why is outputFile a string?
    char *outputFile;
    while((option = getopt(argc, argv, "sio:")) != -1) {
        switch(option) {
            case 's': {
                ignoreWhitespaces = 1;
                printf("ignore whitespaces\n");
                break;
            }
            case 'i': {
                ignoreCasing = 1;
                printf("ignoring casing\n");
                break;
            }
            case 'o': {
                outputFile = optarg;
                printf("outputting to file: %s\n", outputFile);
                break;
            }
            default: {
                
            }
        }
    }
    //char filesToRead[argc - optind][] = {};
    int i = 0;
    for(; optind < argc; optind++){     
        //filesToRead[i++] = argv[optind];
        printf("extra arguments: %s\n", argv[optind]); 
    }
}