/* list.c: Singly Linked List */

#include "findit.h"

#include <stdlib.h>

/* Node Functions */

/**
 * Allocate a new Node structure.
 * @param   data        Data value
 * @param   next        Pointer to next Node structure
 * @return  Pointer to new Node structure (must be deleted).
 **/
Node *  node_create(Data data, Node *next) {
    // Use calloc
    Node *n = calloc(1, sizeof(Node));
    if (n){
        n->data = data;
        n->next = next;
    }
    return n;
}

/**
 * Deallocate Node structure.
 * @param   n           Pointer to Node structure
 * @param   release     Whether or not to free Data string
 * @param   recursive   Whether or not to recursively delete next Node structure
 **/
void    node_delete(Node *n, bool release, bool recursive) {
    // Handle base case, recursive case, and then release data and Node
    if (n  == NULL) return;
    
    if (recursive){
        node_delete(n->next, release, recursive);
    }
    
    if (release) free(n->data.string);
    free(n);

    return;
}

/* List Functions */

/**
 * Append data to end of specified List.
 * @param   l           Pointer to List structure
 * @param   data        Data value to append
 **/
void    list_append(List *l, Data data) {
    Node* n = l->head;

    if(!n){
        l->head = node_create(data, NULL);
        l->tail = l->head;
        return;
    }
    while(n->next){
        n = n->next;
    }
    n->next = node_create(data, NULL);
    l->tail = n->next;
}

/**
 * Filter list by applying the filter function to each Data string in List with
 * the given options:
 *
 *  - If filter function returns true, then keep current Node.
 *  - Otherwise, remove current Node from List and delete it.
 *
 * @param   l           Pointer to List structure
 * @param   filter      Filter function to apply to each Data string
 * @param   options     Pointer to Options structure to use with filter function
 * @param   release     Whether or not to release data string when deleting Node
 **/
void    list_filter(List *l, Filter filter, Options *options, bool release) {
    // Iterate through List and apply filter function to each Data string
    // to determine whether or not to keep the Node.
    Node* n = l->head;
    Node* prev = NULL;

    if (!n) return;

    while(n){
        if(filter(n->data.string, options)){
            prev = n;
            n = n->next;
        }else{
            if (n == l->head){
                l->head = n->next;
                node_delete(n, release, false);
                n = l->head;
            } else if (n == l->tail){
                l->tail = prev;
                prev->next = NULL;
                node_delete(n, release, false);
                n = NULL;
            } else {
                prev->next = n->next;
                node_delete(n, release, false);
                n = prev->next;
            }
        }
    }
    if(!l->head) l->tail = NULL;
}

/**
 * Output each Data string in List to specified stream.
 * @param   l           Pointer to List structure
 * @param   stream      File stream to output to
 **/
void    list_output(List *l, FILE *stream) {
    //  Iterate though List and output each Data string to given stream
    // (one string per line).
    for (Node* n = l->head; n; n=n->next){
        fprintf(stream, "%s\n", n->data.string);
    }
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
