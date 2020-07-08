#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    WrkTarget *prototype = wrk_target_create(NULL, WRK_TARGET_TYPE_NAMESPACE);

    WrkTarget *target = wrk_run(&wrk_state, prototype, wrk_file);

    log_trace("Work done!\n");
}

