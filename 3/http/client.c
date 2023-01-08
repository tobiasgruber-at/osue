/**
 * Client module.
 * @brief Main entry point for the http client.
 * @details Implements a client for the HTTP protocol (version 1.1) for sending a GET request.<br>
 * Establishes a TCP / IP socket connection to a server to requests a resource.<br>
 * Output is written to a specified file or stdout.
 * @file client.c
 * @author Tobias Gruber, 11912367
 * @date 25.12.2022
 **/

#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#define HTTP_PREFIX "http://" /**< Http protocol prefix for urls. */
#define DEFAULT_FILE "index.html" /**< Default file for http requests. */
#define BUF_SIZE 1024 /**< Size of buffers for socket streams. */

/**
 * @brief Program options.
 * @details The program configurations that are specified by the arguments.
 */
typedef struct Options {
    int pflag; /**< Count of passed -p flags (from the arguments). */
    int oflag; /**< Count of passed -o flags (from the arguments). */
    int dflag; /**< Count of passed -d flags (from the arguments). */
    char *server_host; /**< Host name of the server. */
    char *server_path; /**< Path to resource on the server. */
    char *server_args; /**< Request arguments passed to the server. */
    char *server_port; /**< Port name of the server. */
    FILE *output; /**< File pointer for the output. */
} t_opt;

/**
 * @brief Prints the usage of the program to stderr and exists with an error.
 * @details Exits the program with EXIT_FAILURE.<br>
 * Used global variables: prog_name
 */
static void usage(void) {
    fprintf(stderr, "Usage: %s [-p PORT] [ -o FILE | -d DIR ] URL\n", prog_name);
    exit(EXIT_FAILURE);
}

/**
 * @brief Parses details from an url.
 * @details Validates and parses important components of an url, such as protocol, host, resource path and arguments.
 * @param opts Program options.
 * @param url_with_protocol Url including the protocol prefix.
 * @return 0 on success or if valid, -1 on error or if invalid.
 */
static int parse_url_details(t_opt *opts, char *url_with_protocol) {
    if (strncmp(url_with_protocol, HTTP_PREFIX, strlen(HTTP_PREFIX)) != 0) {
        errno = EINVAL;
        return m_err("Invalid protocol");
    }
    opts->server_host = url_with_protocol + strlen(HTTP_PREFIX);
    if (
        strlen(opts->server_host) == 0 ||
        strncmp(opts->server_host, "/", 1) == 0 ||
        strncmp(opts->server_host, "?", 1) == 0
    ) {
        errno = EINVAL;
        return m_err("Invalid hostname");
    }
    int len_before = strlen(opts->server_host);
    opts->server_host = strtok(opts->server_host, "/");
    if (len_before == strlen(opts->server_host)) {
        opts->server_host = strtok(opts->server_host, "?");
        opts->server_args = strtok(NULL, "");
    } else {
        char *temp = strtok(NULL, "");
        if (temp != NULL) {
            if (strncmp(temp, "?", 1) == 0) {
                opts->server_args = strtok(temp, "?");
            } else {
                opts->server_path = strtok(temp, "?");
                opts->server_args = strtok(NULL, "");
            }
        }
    }
    return 0;
}

/**
 * @brief Opens an output stream based on passed arguments.
 * @details If specified in program options, that the output should be printed to either a directory or a file,
 * a corresponding output stream is opened.<br>
 * If the output should be printed to a directory, the file depends on the resource that is requested (falls back to
 * index.html).<br>
 * Opens the output file that must be closed later.
 * @param opts Program options.
 * @param output Path to output file.
 * @return 0 on success, -1 on error.
 */
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

/**
 * @brief Parses the program arguments.
 * @details Reads and validates program options and arguments.<br>
 * Parses informations of the passed url and opens an output stream if necessary.<br>
 * Might exit the program with EXIT_FAILURE if arguments invalid.<br>
 * Opens the output file that must be closed later.
 * @param opts Pointer to program options.
 * @param argc Argument counter.
 * @param argv Argument vector.
 * @return 0 on success, -1 on error.
 * */
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
    if (parse_url_details(opts, argv[optind]) == -1) return t_err("parse_url_details");
    if (open_out_fp(opts, output_path) == -1) return t_err("open_out_fp");
    return 0;
}

/**
 * @brief Connects the client with the http server.
 * @details Requests ip addresses of the server and connects via sockets.<br>
 * Opens the socket file sockfile, must be closed later.
 * @param sockfile Pointer that should be updated with the socket file.
 * @param opts Program options.
 * @return 0 on success, -1 on error.
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

/**
 * @brief Sends an http request.
 * @details Sends a GET request to the server. Details are specified in the program options.<br>
 * Socket stream gets flushed after request is sent.
 * @param sockfile Pointer of the socket file.
 * @param opts Program options.
 * @return 0 on success, -1 on error.
 */
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
 * @brief Validates and prints the response of the http request.
 * @details Validates if the response's protocol, status and if it is well-formed.<br>
 * If there is a response body, it is printed to the output stream.<br>
 * Header infos are parsed by getline() and the body is parsed by fread() to process any data (incl. binary).
 * @param sockfile Pointer of the socket file.
 * @param opts Program options.
 * @return 0 on success and a negative exit status on failure.
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
                fprintf(stderr, "Protocol error!\n");
                free(line);
                return -2;
            }
            if (strcmp(status, "200") != 0) {
                fprintf(stderr, "%s %s", status, status_text);
                free(line);
                return -3;
            }
        } else if (strcmp(line, "\r\n") == 0) {
            is_content = true;
            break;
        }
    }
    free(line);
    if (linec == 0) {
        fprintf(stderr, "Protocol error!");
        return -2;
    }
    if (is_content) {
        char buf[BUF_SIZE];
        while (!feof(sockfile)) {
            size_t n = fread(buf, 1, BUF_SIZE, sockfile);
            fwrite(buf, 1, n, opts.output);
        }
    }
    return 0;
}

/**
 * @brief Requests a resource from a given server.
 * @details Based on the passed arguments it: Connects to a server, requests a resource and prints it to a output
 * stream.<br>
 * The connection is established using sockets and the http protocol is being adhered to.<br>
 * Might exit with status code 1, 2 or 3 if errors occur.<br>
 * Global variables: prog_name
 * @param argc Argument counter.
 * @param argv Argument vector.
 * @return EXIT_SUCCESS on successful termination.
 */
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
    return EXIT_SUCCESS;
}
