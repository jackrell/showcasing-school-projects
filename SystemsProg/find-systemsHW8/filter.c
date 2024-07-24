/*filter.c: Filter functions */

#include "findit.h"

#include <stdlib.h>
#include <string.h>

#include <fnmatch.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* Filter Functions */

/**
 * Determines if file at specified path has matching file type.
 * @param   path        Path string
 * @param   options     Pointer to options structure
 * @return  true if file at specified path has matching file type specified in
 * options.
 **/
bool	filter_by_type(const char *path, Options *options) { 
    // Use lstat
    struct stat s;
    if (lstat(path, &s) < 0) return false;
    
    return ((s.st_mode & S_IFMT) == options->type);
}

/**
 * Determines if file at specified path has matching basename.
 * @param   path        Path string
 * @param   options     Pointer to options structure
 * @return  true if file at specified path has basename that matches specified
 * pattern in options.
 **/
bool	filter_by_name(const char *path, Options *options) {
    // Use basename and fnmatch
    return !fnmatch(options->name, basename((char *)path), 0);
}

/**
 * Determines if file at specified path has matching access mode.
 * @param   path        Path string
 * @param   options     Pointer to options structure
 * @return  true if file at specified path has matching access mode specified
 * in options.
 **/
bool	filter_by_mode(const char *path, Options *options) {
    // Use access
    return !access(path, options->mode);
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
