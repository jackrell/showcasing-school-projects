/* timeit.c: Run command with a time limit */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

/* Macros */

#define	streq(a, b) (strcmp(a, b) == 0)
#define strchomp(s) (s)[strlen(s) - 1] = 0
#define debug(M, ...) \
    if (Verbose) { \
        fprintf(stderr, "%s:%d:%s: " M, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    }

#define BILLION 1000000000.0

/* Globals */

int  Timeout  = 10;
bool Verbose  = false;
int  ChildPid = 0;

/* Functions */

/**
 * Display usage message and exit.
 * @param   status      Exit status.
 **/
void	usage(int status) {
    fprintf(stderr, "Usage: timeit [options] command...\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -t SECONDS  Timeout duration before killing command (default is %d)\n", Timeout);
    fprintf(stderr, "    -v          Display verbose debugging output\n");
    exit(status);
}

/**
 * Parse command line options.
 * @param   argc        Number of command line arguments.
 * @param   argv        Array of command line argument strings.
 * @return  Array of strings representing command to execute (must be freed).
 **/
char ** parse_options(int argc, char **argv) {
    // Iterate through command line arguments to determine Timeout and
    // Verbose flags
    
    int i = 1;
    while (i<argc){
        if (streq(argv[i], "-h")){
            usage(0);
        } else if (streq(argv[i], "-v")){
            Verbose = true;
        } else if (streq(argv[i], "-t")){
            i++;
            Timeout = atoi(argv[i]);
        }else{
            break;
        }
        i++;
    }

    debug("Timeout = %d\n", Timeout);
    debug("Verbose = %d\n", Verbose);

    // Copy remaining arguments into new array of strings
    if (i == argc) usage(1);

    char ** command = calloc(argc - i + 1, sizeof(char*));

    int n = 0;
    while(i < argc) {
        command[n] = argv[i];
        n++;
        i++;
    }

    if (Verbose) {
        // Print out new array of strings (to stderr)
        debug("Command =");
        int j = 0;
        while(command[n]){
            fprintf(stderr, " %s", command[j]);
            j++;
        }
        fprintf(stderr, "\n");

    }

    return command;
}

/**
 * Handle signal.
 * @param   signum      Signal number.
 **/
void    handle_signal(int signum) {
    // Kill child process
    debug("Killing child %d...\n", ChildPid);
    kill(ChildPid, 9);
}

/* Main Execution */

int	main(int argc, char *argv[]) {
    // Parse command line options
    char** command = parse_options(argc, argv);

    // Register alarm handler and save start time
    debug("Registering handlers...\n");
    int status = EXIT_SUCCESS;
    signal(SIGALRM, handle_signal);

    debug("Grabbing start time...\n");
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Fork child process:
    pid_t pid = fork();
    
    //  1. Child executes command parsed from command line
    debug("Executing child...\n");

    alarm(Timeout); // set alarm
    if (pid < 0) { // parent failure
        fprintf(stderr, "Unable to fork: %s\n", strerror(errno));
        status = EXIT_FAILURE;
        goto cleanup;
    } else if (pid == 0) { // child
        if (execvp(command[0], command) < 0) {
            status = EXIT_FAILURE;
            goto cleanup;
        }
    } else { // parent
        ChildPid = pid;
        wait(&status);
    }

    //  2. Parent sets alarm based on Timeout and waits for child
    debug("Sleeping for %d seconds...\n", Timeout);
    debug("Waiting for child %d...\n", ChildPid);

    // Print out child's exit status or termination signal
    debug("Child exit status: %d\n", status);

    // Print elapsed time
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double elapsed_time = \
                          (end_time.tv_sec - start_time.tv_sec) + \
                          (end_time.tv_nsec - start_time.tv_nsec) / BILLION;

    debug("Grabbing end time...\n");
    printf("Time Elapsed: %0.1lf\n", elapsed_time);

    status = WIFEXITED(status) ? WEXITSTATUS(status) : WTERMSIG(status);

    // Cleanup
    cleanup:
    free(command);
    return status;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
