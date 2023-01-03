#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define HTTP_PREFIX "http://"

typedef struct Options {
    int pflag;
    int oflag;
    int dflag;
    char *url_path;
    char *port;
    FILE *output;
} t_opt;

/**
 * @brief Prints the usage of the program to stderr and exists with an error.
 * @details Exits the program with EXIT_FAILURE.<br>
 * Error handling of fprintf is not covered as the program has to exit anyway.<br>
 * Used global variables: prog_name
 */
static void usage(void) {
    fprintf(stderr, "Usage: %s [-p PORT] [ -o FILE | -d DIR ] URL\n", prog_name);
    exit(EXIT_FAILURE);
}

static int parse_args(t_opt *opts, int argc, char** argv) {
    int opt;
    char *output_path;
    while ((opt = getopt(argc, argv, "p:o:d:")) != -1) {
        switch (opt) {
            case 'p':
                opts->pflag++;
                opts->port = optarg;
                if (parse_int(NULL, opts->port) == -1) return t_err("parse_int");
                break;
            case 'o':
                opts->oflag++;
                output_path = optarg;
                break;
            case 'd':
                opts->dflag++;
                output_path = optarg;
                break;
            case '?':
            default:
                usage();
        }
    }
    if (
        optind > argc - 1 ||
        opts->pflag > 1 ||
        opts->oflag > 1 ||
        opts->dflag > 1 ||
        (opts->oflag == 1 && opts->dflag == 1)
    ) usage();
    char *url = argv[optind];
    if (strncmp(url, HTTP_PREFIX, strlen(HTTP_PREFIX)) != 0) return m_err("incorrect protocol");
    opts->url_path = url + strlen(HTTP_PREFIX);
    if (opts->oflag || opts->dflag) {
        if (opts->dflag) {
            char *file_name;
            file_name = strrchr(opts->url_path, '/');
            if (file_name == NULL) file_name = "";
            else file_name++;
            if (strlen(file_name) == 0) file_name = "index.html";
            strcat(output_path, file_name);
        }
        opts->output = fopen(output_path, "w");
        if (opts->output == NULL) return t_err("fopen");
    }
    return 0;
}

int main(int argc, char** argv) {
    prog_name = argv[0];
    t_opt opts = { 0, 0, 0, NULL, "80", stdout };
    if (parse_args(&opts, argc, argv) < 0) {
        e_err("parse_args");
    }
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(opts.url_path, opts.port, &hints, &ai) != 0) e_err("getaddrinfo");
    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    connect(sockfd, ai->ai_addr, ai->ai_addrlen);
    FILE *sockfile = fdopen(sockfd, "r+");
    if (sockfile == NULL) e_err("fdopen");
    if (fprintf(sockfile, "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: osue-http-client/1.0\r\nConnection: close\r\n\r\n", opts.url_path) < 0) e_err("fputs");
    fflush(sockfile);

    bool is_content;
    size_t len = 0, size = 0, linec = 0;
    char *line;
    while ((size = getline(&line, &len, sockfile)) != -1) {
        if (
            linec++ == 0 &&
            (
                !substr_at(line, "HTTP/1.1", 0) ||
                !substr_at(line, "200", 9) ||
                !substr_at(line, "OK", 13)
            )
        ) e_err("incorrect protocol");
        if (is_content) {
            fprintf(opts.output, "%s", line);
        } else if (size <= 2) is_content = true;
    }
    printf("\n");
    free(line);

}
