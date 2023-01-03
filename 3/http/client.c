#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#define HTTP_PREFIX "http://"

typedef struct Options {
    int pflag;
    int oflag;
    int dflag;
    char *server_url; /**< Host and filepath */
    char *server_host;
    char *server_port;
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

/**
 * Mallocs space for server_host
 * @param opts
 * @param argv
 * @return
 */
static int parse_url_details(t_opt *opts, char **argv) {
    char *url_with_protocol = argv[optind];
    if (strncmp(url_with_protocol, HTTP_PREFIX, strlen(HTTP_PREFIX)) != 0) return m_err("incorrect protocol");
    opts->server_url = url_with_protocol + strlen(HTTP_PREFIX);
    opts->server_host = (char *) malloc(sizeof(char) * strlen(opts->server_url) + 1);
    strcpy(opts->server_host, opts->server_url);
    if (
        (opts->server_host = strtok(opts->server_host, "/")) == NULL ||
        (opts->server_host = strtok(opts->server_host, "?")) == NULL
    ) {
        free(opts->server_host);
        return t_err("strtok");
    }
    return 0;
}

static int open_output(t_opt *opts, char *output) {
    if (opts->oflag || opts->dflag) {
        if (opts->dflag) {
            char *file_name = strrchr(opts->server_url, '/');
            if (file_name == NULL) file_name = "";
            else file_name++;
            if (strlen(file_name) == 0) file_name = "/index.html";
            char *temp = output;
            output = (char *) malloc(sizeof(char) * (strlen(output) + strlen(file_name) + 1));
            if (output == NULL) return t_err("realloc");
            strcpy(output, temp);
            strcat(output, file_name);
        }
        opts->output = fopen(output, "w");
        if (opts->dflag) free(output);
        if (opts->output == NULL) return t_err("fopen");
    }
    return 0;
}

static int parse_args(t_opt *opts, int argc, char** argv) {
    char opt, *output_path;
    while ((opt = getopt(argc, argv, "p:o:d:")) != -1) {
        switch (opt) {
            case 'p':
                opts->pflag++;
                opts->server_port = optarg;
                if (parse_int(NULL, opts->server_port) == -1) return t_err("parse_int");
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
    if (parse_url_details(opts, argv) == -1) return t_err("parse_url_details");
    if (open_output(opts, output_path) == -1) {
        free(opts->server_host);
        return t_err("open_output");
    }
    return 0;
}

int main(int argc, char** argv) {
    prog_name = argv[0];
    t_opt opts = { 0, 0, 0, NULL, NULL, "80", stdout };
    if (parse_args(&opts, argc, argv) < 0) {
        e_err("parse_args");
    }
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(opts.server_url, opts.server_port, &hints, &ai) != 0) {
        free(opts.server_host);
        errno = EINVAL;
        e_err("getaddrinfo");
    }
    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    connect(sockfd, ai->ai_addr, ai->ai_addrlen);
    FILE *sockfile = fdopen(sockfd, "r+");
    if (sockfile == NULL) {
        free(opts.server_host);
        e_err("fdopen");
    }
    if (fprintf(sockfile, "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: http-client/1.0\r\nConnection: close\r\n\r\n", opts.server_host) < 0) {
        free(opts.server_host);
        e_err("fputs");
    }
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
        ) {
            free(opts.server_host);
            e_err("incorrect protocol");
        }
        if (is_content) {
            fprintf(opts.output, "%s", line);
        } else if (size <= 2) is_content = true;
    }
    printf("\n");
    free(line);
    free(opts.server_host);
}
