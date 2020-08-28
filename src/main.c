#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "work.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Please provide a file to compile!\n");
        exit(0);
    }

    char *wrk_file = argv[1];

    log_trace("wrk_file is %s\n", wrk_file);

    WrkState wrk_state;
    wrk_state_create(&wrk_state, true);

    log_trace("creating target");
    WrkTarget *prototype = wrk_target_from_env();

    WrkTarget *target = wrk_run_workfile(&wrk_state, prototype, wrk_file);
    printf("finished running workfile\n");

    assert(NULL != target);

    log_trace("My work here is done\n");
}

