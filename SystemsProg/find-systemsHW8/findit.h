/* findit.h: Search for files in a directory hierarchy */

#pragma once

#include <stdbool.h>
#include <stdio.h>

/* Options Structure */

typedef struct {
    int   type;         // File type (-type)
    char *name;         // File name pattern (-name)
    int   mode;         // Access modes (-executable, -readable, -writable)
} Options;

/* Filter Functions */

typedef bool (*Filter)(const char *path, Options *options);

bool	filter_by_type(const char *path, Options *options);
bool	filter_by_name(const char *path, Options *options);
bool	filter_by_mode(const char *path, Options *options);

/* Data Union */

typedef union {
    char *  string;     // String data
    Filter  function;   // Filter function
} Data;

/* Node Structure */

typedef struct Node Node;
struct Node {
    Data    data;       // Data value
    Node   *next;       // Pointer to next Node
};

Node *  node_create(Data data, Node *next);
void    node_delete(Node *n, bool release, bool recursive);

/* List Structure */

typedef struct {
    Node   *head;       // Pointer to first Node
    Node   *tail;       // Pointer to last Node
} List;

void    list_append(List *l, Data data);
void    list_filter(List *l, Filter filter, Options *options, bool release);
void    list_output(List *l, FILE *stream);

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
