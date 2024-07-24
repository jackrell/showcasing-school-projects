/* moveit.c: Interactive Move Command */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

/* Macros */

#define streq(a, b) (strcmp(a, b) == 0)
#define strchomp(s) (s)[strlen(s) - 1] = 0

/* Functions */

/**
 * Display usage message and exit.
 * @param   status      Exit status.
 **/
void    usage(int status) {
    fprintf(stderr, "Usage: moveit files...\n");
    exit(status);
}

/**
 * Save list of file paths to temporary file.
 * @param   files       Array of path strings.
 * @param   n           Number of path strings.
 * @return  Newly allocated path to temporary file (must be freed).
 **/
char *  save_files(char **files, size_t n) {
    // Create temporary file
    char tpath[] = "moveit.XXXXXX";
    int tfd = mkstemp(tpath);
    // Open temporary file for writing
    FILE* f = fdopen(tfd, "w");
    // Write paths to temporary file
    for (int i = 0; i<n; i++){
        fprintf(f, "%s\n", files[i]);
    }

    fclose(f);

    return strdup(tpath);
}

/**
 * Run $EDITOR on specified path.
 * @param   path        Path to file to edit.
 * @return  Whether or not the $EDITOR process terminatesd successfully.
 **/
bool    edit_files(const char *path) {
    // Get EDITOR from environment (default to vim if not found)
    char* editor = getenv("EDITOR");
    if (editor == NULL) editor = "vim";


    // Fork process
    //  1. Child: execute editor on path
    //  2. Parent: wait for child
    int rc = fork();
    int status = 0;

    if (rc < 0) { // failed
        fprintf(stderr, "Fork failed: %s\n", strerror(errno));
        return EXIT_FAILURE;

    }else if (rc == 0){ // child
        if (execlp(editor, editor, path, (char *)NULL) < 0) {
            exit(EXIT_FAILURE);
        }
    }else{ // parent
        wait(&status);
    }

    // Return exit status of child process
    
    return (WEXITSTATUS(status) == 0);
}

/**
 * Rename files as specified in contents of path.
 * @param   files       Array of old path names.
 * @param   n           Number of old path names.
 * @param   path        Path to file with new names.
 * @return  Whether or not all rename operations were successful.
 **/
bool    move_files(char **files, size_t n, const char *path) {
    //Open temporary file at path for reading
    FILE* tf = fopen(path, "r");
    if(!tf) return false;
    bool status = true;

    // Rename each file in array according to new name in temporary file
    char buffer[BUFSIZ];
    int i = 0;
    while(fgets(buffer, BUFSIZ, tf) && (i < n)){
        strchomp(buffer);
        if(!streq(buffer, files[i])){
            if (rename(files[i], buffer)< 0){
                status = false;
            }
        } 
        i++;
    }

    fclose(tf);

    return status;
}

/* Main Execution */

int     main(int argc, char *argv[]) {
    int estatus = EXIT_SUCCESS;
    
    // Parse command line options
    if (argc == 1){
        usage(1);
    }
    if (argc == 2 && streq(argv[1], "-h")){
        usage(0);
    }

    int n = argc-1;

    char **files = malloc(n * sizeof(char*));

    for (int i = 0; i < n; i++){
        files[i] = malloc((strlen(argv[i+1]) + 1) * sizeof(char));
        strcpy(files[i], argv[i+1]);
    }

    //Save files
    char *path = save_files(files, n);
    // Edit files
    if(!edit_files(path)){
        estatus = EXIT_FAILURE;
        goto cleanup;
    }
    // Move files
    if(!move_files(files, n, path)){
        estatus = EXIT_FAILURE;
        goto cleanup;
    }
    // Cleanup temporary file
    cleanup:
    unlink(path);
    free(path);
    for (int i = 0; i < n; i++) free(files[i]);
    free(files);
    return estatus;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
