/* filter.unit.c: filter unit test */

#include "findit.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

/* Tests */

int test_00_filter_by_type() {
    Options o = {0};

    // Test directories
    o.type = S_IFDIR;
    assert(filter_by_type(".", &o));
    assert(filter_by_type("..", &o));
    assert(filter_by_type("/tmp", &o));
    assert(!filter_by_type("Makefile", &o));
    assert(!filter_by_type("filter.c", &o));
    assert(!filter_by_type("list.c", &o));
    assert(!filter_by_type("/root/.ssh", &o));
    assert(!filter_by_type("CHUPABLAHBLA", &o));

    // Test files
    o.type = S_IFREG;
    assert(!filter_by_type(".", &o));
    assert(!filter_by_type("..", &o));
    assert(!filter_by_type("/tmp", &o));
    assert(filter_by_type("Makefile", &o));
    assert(filter_by_type("filter.c", &o));
    assert(filter_by_type("list.c", &o));
    assert(!filter_by_type("/root/.ssh", &o));
    assert(!filter_by_type("CHUPABLAHBLA", &o));
    return EXIT_SUCCESS;
}

int test_01_filter_by_name() {
    Options o = {0};
    
    // Test no pattern
    o.name = "Makefile";
    assert(filter_by_name("Makefile", &o));
    assert(filter_by_name("./Makefile", &o));
    assert(!filter_by_name("Makefiles", &o));
    assert(!filter_by_name("makefile", &o));
    assert(!filter_by_name("./Makefile/asdf", &o));
    
    // Test pattern
    o.name = "*.c";
    assert(!filter_by_name("Makefile", &o));
    assert(!filter_by_name("./Makefile", &o));
    assert(filter_by_name("filter.c", &o));
    assert(filter_by_name("./filter.c", &o));
    assert(!filter_by_name("./filter.ch", &o));
    return EXIT_SUCCESS;
}

int test_02_filter_by_mode() {
    Options o = {0};
    
    // Test readable
    o.mode = R_OK;
    assert(filter_by_mode("Makefile", &o));
    assert(filter_by_mode("filter.unit", &o));
    assert(!filter_by_mode("/root/.ssh", &o));
    
    // Test writable
    o.mode = W_OK;
    assert(filter_by_mode("Makefile", &o));
    assert(filter_by_mode("filter.unit", &o));
    assert(!filter_by_mode("/root/.ssh", &o));

    // Test executable
    o.mode = X_OK;
    assert(!filter_by_mode("Makefile", &o));
    assert(filter_by_mode("filter.unit", &o));
    assert(!filter_by_mode("/root/.ssh", &o));
    return EXIT_SUCCESS;
}

/* Main Execution */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s NUMBER\n\n", argv[0]);
        fprintf(stderr, "Where NUMBER is right of the following:\n");
        fprintf(stderr, "    0  Test filter_by_type\n");
        fprintf(stderr, "    1  Test filter_by_name\n");
        fprintf(stderr, "    2  Test filter_by_mode\n");
        return EXIT_FAILURE;
    }

    int number = atoi(argv[1]);
    int status = EXIT_FAILURE;

    switch (number) {
        case 0:  status = test_00_filter_by_type(); break;
        case 1:  status = test_01_filter_by_name(); break;
        case 2:  status = test_02_filter_by_mode(); break;
        default: fprintf(stderr, "Unknown NUMBER: %d\n", number); break;
    }

    return status;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
