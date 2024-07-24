/* findit.c: Search for files in a directory hierarchy */

#include "findit.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

/* Macros */

#define	streq(a, b) (strcmp(a, b) == 0)

/* Functions */

/**
 * Print usage message and exit with status
 * @param   status      Exit status
 **/
void usage(int status) {
    fprintf(stderr, "Usage: findit PATH [OPTIONS]\n\n");
    fprintf(stderr, "Options:\n\n");
    fprintf(stderr, "   -type [f|d]	File is of type f for regular file or d for directory\n");
    fprintf(stderr, "   -name pattern	Name of file matches shell pattern\n");
    fprintf(stderr, "   -executable	File is executable or directory is searchable by user\n");
    fprintf(stderr, "   -readable	File is readable by user\n");
    fprintf(stderr, "   -writable	File is writable by user\n");
    exit(status);
}

/**
 * Recursively walk specified directory, adding all file system entities to
 * specified files list.
 * @param   root        Directory to walk
 * @param   files       List of files found
 **/
void	find_files(const char *root, List *files) {
    // Add root to files
    list_append(files, (Data)strdup(root));

    // Walk directory
    //  - Skip current and parent directory entries
    //  - Form full path to entry
    //  - Recursively walk directories or add entry to files list
    DIR *d = opendir(root);
    if (!d) return;

    for (struct dirent *e = readdir(d); e; e = readdir(d)) {
        if (streq(e->d_name, ".") || streq(e->d_name, "..")) {
            continue;
        }

        char path[BUFSIZ];
        sprintf(path, "%s/%s", root, e->d_name);
    
        /*
        struct stat s;
        if (stat(path, &s) < 0) {
            continue;
        }
        */

        if (e->d_type == DT_DIR){
            find_files(path, files);
        } else {
            list_append(files, (Data)strdup(path));
        }
    }

    closedir(d);
}

/**
 * Iteratively filter list of files with each filter in list of filters.
 * @param   files       List of files
 * @param   filters     List of filters
 * @param   options     Pointer to options structure
 **/
void	filter_files(List *files, List *filters, Options *options) {
    // Apply each filter to list of files
    for (Node *curr = filters->head; curr; curr = curr->next){
        list_filter(files, curr->data.function, options, true);
    }

}

/* Main Execution */

int main(int argc, char *argv[]) {
    // TODO: Parse command line arguments */
    List files = {0};
    List filters = {0};
    Options options = {0};
    char root[BUFSIZ];
    //char name[BUFSIZ];

    if (argc < 2) usage(EXIT_FAILURE);

    strncpy(root, argv[1], BUFSIZ);

    int i = 2;

    while (i < argc){
        if (streq(argv[i], "-type")){
            i++;
            if (streq(argv[i], "d")){
                options.type = S_IFDIR;
            } else {
                options.type = S_IFREG;
            }
            list_append(&filters, (Data)filter_by_type);
        
        } else if (streq(argv[i], "-name")){
            i++;
            options.name =argv[i];
            list_append(&filters, (Data)filter_by_name);
       
        } else if (streq(argv[i], "-executable")){
            options.mode = X_OK;
            list_append(&filters, (Data)filter_by_mode);
        } else if (streq(argv[i], "-writable")){
            options.mode = W_OK;
            list_append(&filters, (Data)filter_by_mode);
        } else if (streq(argv[i], "-readable")){
            options.mode = R_OK;
            list_append(&filters, (Data)filter_by_mode);
        
        }else{
            usage(EXIT_FAILURE);
        }

        i++;
    }

    // TODO: Find files, filter files, print files
    find_files(root, &files);
    
    filter_files(&files, &filters, &options);

    list_output(&files, stdout);

    node_delete(files.head, true, true);
    node_delete(filters.head, false, true);
    
    return EXIT_SUCCESS;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
