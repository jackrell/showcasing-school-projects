/* list.unit.c: linked list unit test */

#include "findit.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

/* Macros */

#define streq(a, b) (strcmp(a, b) == 0)

/* Functions */

bool filter_by_length(const char *path, Options *options) {
    return strlen(path) > options->type;
}

/* Tests */

int test_00_node_create() {
    Data d = {.string="Your Light"};

    // Test: String data
    Node *n0 = node_create(d, NULL);
    assert(n0);
    assert(streq(n0->data.string, d.string));

    // Test: String data (duplicated), next
    Node *n1 = node_create((Data)strdup(d.string), n0);
    assert(n1);
    assert(n1->next == n0);
    assert(streq(n1->data.string, d.string));

    // Test: Function data
    Node *n2 = node_create((Data)filter_by_length, n1);
    assert(n2);
    assert(n2->next == n1);
    assert(n2->data.function == filter_by_length);

    free(n0);
    free(n1->data.string);
    free(n1);
    free(n2);

    return EXIT_SUCCESS;
}

int test_01_node_delete() {
    Data d0 = {.string="Your Light"};
    Data d1 = {.string="My darkness"};
    Data d2 = {.string="Big Moon"};

    // Test: String data
    Node *n0 = node_create(d0, NULL);
    assert(n0);
    assert(streq(n0->data.string, d0.string));
    node_delete(n0, false, false);

    // Test: String data (duplicated)
    Node *n1 = node_create((Data)strdup(d1.string), NULL);
    assert(n1);
    assert(streq(n1->data.string, d1.string));
    node_delete(n1, true, false);

    // Test: Function data
    Node *n2 = node_create((Data)strdup(d0.string),
    	       node_create((Data)strdup(d1.string),
    	       node_create((Data)strdup(d2.string), NULL)));
    assert(streq(n2->data.string, d0.string));
    assert(streq(n2->next->data.string, d1.string));
    assert(streq(n2->next->next->data.string, d2.string));
    node_delete(n2, true, true);

    return EXIT_SUCCESS;
}

int test_02_list_append() {
    Data d[] = {
	{"I wonder what the chance is you wanted to"},
	{"A thousand vacant stares won't make it true"},
	{"Make it true"},
    };
    List l = {NULL, NULL};

    // Test: Append to empty
    list_append(&l, d[0]);
    assert(l.head && l.tail);
    assert(l.head == l.tail);
    assert(streq(l.head->data.string, d[0].string));
    assert(streq(l.tail->data.string, d[0].string));

    // Test: Append to non-empty
    list_append(&l, d[1]);
    assert(l.head && l.tail);
    assert(l.head->next == l.tail);
    assert(streq(l.head->data.string, d[0].string));
    assert(streq(l.tail->data.string, d[1].string));

    list_append(&l, d[2]);
    assert(l.head && l.tail);
    assert(l.head->next->next == l.tail);
    assert(streq(l.head->data.string, d[0].string));
    assert(streq(l.head->next->data.string, d[1].string));
    assert(streq(l.tail->data.string, d[2].string));

    node_delete(l.head, false, true);
    return EXIT_SUCCESS;
}

int test_03_list_filter() {
    Data d[] = {
	{"Don't, don't, don't, don't blame another night on the moon"},
	{"Sometimes faith just sings to a different tune"},
	{"Why do you have to take it out so hard on yourself (I don't wanna lose myself, lose myself"},
	{"We were promised the world, so was everyone else (I don't wanna lose myself, lose myself)"},
	{NULL},
    };
    List l = {NULL};
    Options o = {0};

    for (Data *p = d; p->string; p++) {
    	list_append(&l, *p);
    }

    // Test: filter middle
    o.type = strlen(d[1].string);
    list_filter(&l, filter_by_length, &o, false);
    assert(streq(l.head->data.string, d[0].string));
    assert(streq(l.head->next->data.string, d[2].string));
    assert(streq(l.head->next->next->data.string, d[3].string));
    assert(l.tail == l.head->next->next);
    node_delete(l.head, false, true);

    // Test: filter all
    l.head = NULL;
    l.tail = NULL;
    o.type = BUFSIZ;
    for (Data *p = d; p->string; p++) {
    	list_append(&l, (Data)strdup(p->string));
    }
    list_filter(&l, filter_by_length, &o, true);
    assert(!l.head && !l.tail);
    node_delete(l.head, true, true);

    return EXIT_SUCCESS;
}

int test_04_list_output() {
    Data d[] = {
        {"I have been holding my breath"},
        {"For too many nights in a row"},
        {"And somewhere on coastlines unknown to me"},
        {"You paint your dreams"},
        {"With reds and blues and greens"},
        {"Yea you're painting daffodils by the sea"},
        {"Without me"},
	{NULL},
    };

    List l = {NULL};

    for (Data *p = d; p->string; p++) {
    	list_append(&l, *p);
    }

    char tmp_path[BUFSIZ] = "/tmp/list.unit.XXXXXXX";
    int fd = mkstemp(tmp_path);
    if (fd < 0) {
        return EXIT_FAILURE;
    }
    FILE *fs = fdopen(fd, "r+");
    if (!fs) {
        return EXIT_FAILURE;
    }
    unlink(tmp_path);

    list_output(&l, fs);
    rewind(fs);

    char buffer[BUFSIZ];
    Node *curr = l.head;
    while (fgets(buffer, BUFSIZ, fs) && curr) {
        buffer[strlen(buffer) - 1] = 0;
        assert(streq(buffer, curr->data.string));
        curr = curr->next;
    }
    assert(!curr);

    node_delete(l.head, false, true);
    return EXIT_SUCCESS;
}

/* Main Execution */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s NUMBER\n\n", argv[0]);
        fprintf(stderr, "Where NUMBER is right of the following:\n");
        fprintf(stderr, "    0  Test node_create\n");
        fprintf(stderr, "    1  Test node_delete\n");
        fprintf(stderr, "    2  Test list_append\n");
        fprintf(stderr, "    3  Test list_filter\n");
        fprintf(stderr, "    4  Test list_output\n");
        return EXIT_FAILURE;
    }

    int number = atoi(argv[1]);
    int status = EXIT_FAILURE;

    switch (number) {
        case 0:  status = test_00_node_create(); break;
        case 1:  status = test_01_node_delete(); break;
        case 2:  status = test_02_list_append(); break;
        case 3:  status = test_03_list_filter(); break;
        case 4:  status = test_04_list_output(); break;
        default: fprintf(stderr, "Unknown NUMBER: %d\n", number); break;
    }

    return status;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
