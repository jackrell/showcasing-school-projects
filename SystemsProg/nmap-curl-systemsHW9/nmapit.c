/* nmapit.c: Simple network port scanner */

#include "socket.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <signal.h>

#ifndef GNU_SOURCE
typedef void (*sighandler_t)(int);
#endif

/* Functions */

/**
 * Display usage message and exit.
 * @param   status      Exit status
 **/
void    usage(int status) {
    fprintf(stderr, "Usage: nmapit [-p START-END] HOST\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -p START-END    Specifies the range of port numbers to scan\n");
    exit(status);
}

/**
 * Handle alarm signal.
 * @param   signum      Signal number
 **/
void sigalrm_handler(int signum) {
    alarm(0);
}

/**
 * Parse port range string into start and end port integers.
 * @param   range       Port range string (ie. START-END)
 * @param   start       Pointer to starting port integer
 * @param   end         Pointer to ending port integer
 * @return  true if parsing both start and end were successful, otherwise false
 **/
bool parse_ports(char *range, int *start, int *end) {
    // Parse starting port
    char *tok;

    tok = strtok(range, "-");
    //printf("%s\n", tok);
    if (tok == NULL || *tok == '\0'){
        return false;
    }

    *start = atoi(tok);

    tok = strtok(NULL, "-");
    //printf("%s\n", tok);
    if (tok == NULL || *tok == '\0'){
        //printf("returning false\n");
        return false;
    }

    *end = atoi(tok);
    
    return true;
}

/**
 * Scan ports at specified host from starting and ending port numbers
 * (inclusive).
 * @param   host        Host to scan
 * @param   start       Starting port number
 * @param   end         Ending port number
 * @return  true if any port is found, otherwise false
 **/
bool scan_ports(const char* host, int start, int end) {
    // Register signal handler for alarm
    struct sigaction action = {.sa_handler = sigalrm_handler};
    if (sigaction(SIGALRM, &action, NULL) < 0){
        return false;
    }
    // For each port, set alarm, attempt to dial host and port
    bool rv = false;
    char port_s[10];
    for (int port = start; port <= end; port++){
        sprintf(port_s, "%d", port);
        alarm(1);
        if (socket_dial(host, port_s)){
            alarm(0);
            rv = true;
            printf("%d\n", port);
        } 
    }

    return rv;
}

/* Main Execution */

int main(int argc, char *argv[]) {
    // TODO: Parse command-line arguments
    if (argc == 1) usage(1);

    int i = 1;

    char range[BUFSIZ];
    char host[BUFSIZ];

    strcpy(range, "1-1023");

    while (i<argc){
        if (strcmp(argv[i], "-h") == 0){
            usage(0);
        } else if (strcmp(argv[i], "-p") == 0){
            i++;
            strcpy(range, argv[i]);
            i++;
        } else {
            strcpy(host, argv[i]);
            i++;
        }
    }
    
    int start = 0;
    int end = 0;

    if (!parse_ports(range, &start, &end)) usage(1);

    // Scan ports
    if (scan_ports(host, start, end)){
        return EXIT_SUCCESS;
    }else{
        return EXIT_FAILURE;
    }
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
