#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#define HTTP_PREFIX "http://"
#define DEFAULT_FILE "index.html"

typedef struct Options {
    int pflag;
    int oflag;
    int dflag;
    char *server_host;
    char *server_path;
    char *server_args;
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
    if (strncmp(url_with_protocol, HTTP_PREFIX, strlen(HTTP_PREFIX)) != 0) {
        errno = EINVAL;
        return m_err("Invalid protocol");
    }
    opts->server_host = url_with_protocol + strlen(HTTP_PREFIX);
    if (
        strlen(opts->server_host) == 0 ||
        substr_at(opts->server_host, "/", 0) ||
        substr_at(opts->server_host, "?", 0)
    ) {
        errno = EINVAL;
        return m_err("Invalid hostname");
    }
    int len_before = strlen(opts->server_host);
    opts->server_host = strtok(opts->server_host, "/");
    if (len_before == strlen(opts->server_host)) {
        opts->server_host = strtok(opts->server_host, "?");
    } else {
        opts->server_path = strtok(NULL, "?");
    }
    opts->server_args = strtok(NULL, "");
    return 0;
}

static int open_out_fp(t_opt *opts, char *output) {
    if (opts->oflag || opts->dflag) {
        if (opts->dflag) {
            char *file_name = DEFAULT_FILE;
            if (opts->server_path != NULL) {
                file_name = strrchr(opts->server_path, '/');
                if (file_name == NULL) file_name = opts->server_path;
                else if (strlen(++file_name) == 0) file_name = DEFAULT_FILE;
            }
            char *temp = output;
            output = (char *) malloc(sizeof(char) * (strlen(output) + strlen(file_name) + 2));
            if (output == NULL) return t_err("realloc");
            strcpy(output, temp);
            strcat(output, "/");
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
                int port;
                if (parse_int(&port, opts->server_port) == -1) return t_err("parse_int");
                if (port > 65535) {
                    errno = EINVAL;
                    return m_err("Port too high");
                }
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
            default: usage();
        }
    }
    if (
        optind != argc - 1 ||
        opts->pflag > 1 ||
        opts->oflag > 1 ||
        opts->dflag > 1 ||
        (opts->oflag == 1 && opts->dflag == 1)
    ) usage();
    if (parse_url_details(opts, argv) == -1) return t_err("parse_url_details");
    if (open_out_fp(opts, output_path) == -1) return t_err("open_out_fp");
    return 0;
}

/**
 * aia
 * @param sockfile
 * @param opts
 * @return
 */
static int connect_server(FILE **sockfile, t_opt *opts) {
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(opts->server_host, opts->server_port, &hints, &ai) != 0) {
        errno = EINVAL;
        return t_err("getaddrinfo");
    }
    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) == -1) {
        freeaddrinfo(ai);
        return t_err("connect");
    };
    freeaddrinfo(ai);
    *sockfile = fdopen(sockfd, "r+");
    if (*sockfile == NULL) return t_err("fdopen");
    return 0;
}

static int send_request(FILE *sockfile, t_opt *opts) {
    char resource[(opts->server_path ? strlen(opts->server_path) : 0) + (opts->server_args ? strlen(opts->server_args) : 0) + 3];
    strcpy(resource, "/");
    if (opts->server_path) strcat(resource, opts->server_path);
    if (opts->server_args) {
        strcat(resource, "?");
        strcat(resource, opts->server_args);
    }
    if (fprintf(
        sockfile,
        "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: tuwien-osue-http/1.0\r\nConnection: close\r\n\r\n",
        resource,
        opts->server_host
    ) < 0) return t_err("fprintf");
    fflush(sockfile);
    return 0;
}

/**
 *
 * @param sockfile
 * @param opts
 * @return Exit status (negative)
 */
static int print_response(FILE *sockfile, t_opt opts) {
    bool is_content = false;
    size_t len = 0, linec = 0;
    char *line = NULL;
    while (getline(&line, &len, sockfile) != -1) {
        if (linec++ == 0) {
            char *protocol = strtok(line, " ");
            char *status = strtok(NULL, " ");
            char *status_text = strtok(NULL, "");
            if (
                protocol == NULL || status == NULL || status_text == NULL ||
                strcmp(protocol, "HTTP/1.1") != 0 ||
                parse_int(NULL, status) == -1
            ) {
                free(line);
                printf("Protocol error!\n");
                return -2;
            }
            if (strcmp(status, "200") != 0) {
                printf("%s %s", status, status_text);
                free(line);
                return -3;
            }
        }
        else if (is_content) fprintf(opts.output, "%s", line);
        else if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) is_content = true;
    }
    free(line);
    if (linec == 0) {
        printf("Protocol error!");
        return -2;
    }
    return 0;
}

int main(int argc, char** argv) {
    prog_name = argv[0];
    t_opt opts = { 0, 0, 0, NULL, NULL, NULL, "80", stdout };
    if (parse_args(&opts, argc, argv) < 0) e_err("parse_args");
    FILE *sockfile = NULL;
    if (connect_server(&sockfile, &opts) == -1) {
        if (opts.output != stdout) fclose(opts.output);
        e_err("connect_server");
    }
    if (send_request(sockfile, &opts) == -1) {
        fclose(sockfile);
        if (opts.output != stdout) fclose(opts.output);
        e_err("send_request");
    }
    int res = print_response(sockfile, opts);
    if (res < 0) {
        fclose(sockfile);
        if (opts.output != stdout) fclose(opts.output);
        exit(abs(res));
    }
    fclose(sockfile);
    if (opts.output != stdout) fclose(opts.output);
}
