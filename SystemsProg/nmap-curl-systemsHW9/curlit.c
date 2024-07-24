/* curlit.c: Simple HTTP client*/

#include "socket.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <netdb.h>

/* Constants */

#define HOST_DELIMITER  "://"
#define PATH_DELIMITER  '/'
#define PORT_DELIMITER  ':'
#define BILLION         (1000000000.0)
#define MEGABYTES       (1<<20)

/* Macros */

#define streq(a, b) (strcmp(a, b) == 0)

/* Structures */

typedef struct {
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
    char path[PATH_MAX];
} URL;

/* Functions */

/**
 * Display usage message and exit.
 * @param   status      Exit status.
 **/
void    usage(int status) {
    fprintf(stderr, "Usage: curlit [-h] URL\n");
    exit(status);
}

/**
 * Parse URL string into URL structure.
 * @param   s       URL string
 * @param   url     Pointer to URL structure
 **/
void    parse_url(const char *s, URL *url) {
    // Copy data to local buffer
    char buffer[BUFSIZ];
    strcpy(buffer, s);
    
    // Skip scheme to host
    char* host = strstr(buffer, HOST_DELIMITER);
    if (!host){
        host = buffer;
    }else{
        host += strlen(HOST_DELIMITER);
    }

    // Split host:port from path
    char* path = strchr(host, PATH_DELIMITER);
    if(!path){
        path = "";
    } else {
        *path = 0;
        path++;
    }


    // Split host and port 
    char* port = strchr(host, PORT_DELIMITER);
    if(!port){
        port = "80";
    } else {
        *port = 0;
        port++;
    }

    // Copy components to URL
    strcpy(url->host, host);
    strcpy(url->port, port);
    strcpy(url->path, path);
}

/**
 * Fetch contents of URL and print to standard out.
 *
 * Print elapsed time and bandwidth to standard error.
 * @param   s       URL string
 * @param   url     Pointer to URL structure
 * @return  true if client is able to read all of the content (or if the
 * content length is unset), otherwise false
 **/
bool    fetch_url(URL *url) {
    // Grab start time
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    bool rv = true;

    // Connect to remote host and port
    FILE *client_file = socket_dial(url->host, url->port);
    if (!client_file) {
        //printf("dial failed");    // test
        return false;
    }

    // Send request to server
    fprintf(client_file, "GET /%s HTTP/1.0\r\n", url->path);
    fprintf(client_file, "Host: %s\r\n", url->host);
    fprintf(client_file, "\r\n");

    // Read status response from server
    char buffer[BUFSIZ];
    fgets(buffer, BUFSIZ, client_file);
    // printf("%s\n", buffer);               // test
    if (!strstr(buffer, "200 OK")){
        // printf("bad return val\n");      // test
        rv = false;
    }

    // Read response headers from server
    size_t content_length = 0;
    while (fgets(buffer, BUFSIZ, client_file) && strlen(buffer) > 2){
        sscanf(buffer, "Content-Length: %lu", &content_length);
    }

    //printf("%lu\n", content_length);      // test

    // Read response body from server
    size_t nread;
    size_t total_bytes_written = 0;

    while((nread = fread(buffer, 1, BUFSIZ, client_file)) > 0) {
        fwrite(buffer, 1, nread, stdout);
        total_bytes_written += nread;
    }
    
    //printf("%lu\n", total_bytes_written);    // test

    if (content_length && (total_bytes_written != content_length)){ 
        //printf("lengths dont match\n");   // test
        rv = false;
    }

    fclose(client_file);

    // Grab end time
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Output metrics
    double elapsed_time = \
                          (end_time.tv_sec - start_time.tv_sec) + \
                          ((end_time.tv_nsec - start_time.tv_nsec) / BILLION);

    fprintf(stderr, "Elapsed Time: %0.2lf s\n", elapsed_time);

    double bandwidth = ((float)total_bytes_written / MEGABYTES) / elapsed_time;

    fprintf(stderr, "Bandwidth:    %0.2lf MB/s\n", bandwidth);

    return rv;
}

/* Main Execution */

int     main(int argc, char *argv[]) {
    // Parse command line options
    if (argc==1) usage(1);

    if (argc>2) usage(1);

    if(streq(argv[1], "-h")){
        usage(0);
    } else if (strchr(argv[1], '-')){
        usage(1);
    }

    char* s = argv[1];

    URL * url = malloc(sizeof(URL));

    // Parse URL
    parse_url(s, url);

    // Fetch URL
    if (!fetch_url(url)){
        free(url);
        return EXIT_FAILURE;
    }else{
        free(url);
        return EXIT_SUCCESS;
    }
}   

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
