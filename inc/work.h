#ifndef _WRK_H_
#define _WRK_H_

#include <stdint.h>

#include "tcclib.h"


// work run    - run wrk.c files and run the result. this is the normal use
// work build  - build obj or exe, perhaps for each file, perhaps as one
//   -e --each - bulids each file into an obj, instead of linking in memory
//
// work list   - list all source files (for use with make or entr)
// word dryrun - print commands that work would run


typedef enum WRK_RESULT_ENUM {
    WRK_RESULT_OKAY     = 1,
    WRK_RESULT_NULL_PTR = 2,
} WRK_RESULT_ENUM;

typedef enum WRK_TARGET_TYPE_ENUM {
    WRK_TARGET_TYPE_NAMESPACE = 1,
    WRK_TARGET_TYPE_SO        = 2,
    WRK_TARGET_TYPE_AR        = 3,
    WRK_TARGET_TYPE_EXE       = 4,
} WRK_TARGET_TYPE_ENUM;


typedef struct WrkTarget {
    char *name;
    WRK_TARGET_TYPE_ENUM type;

    char *tool;

    char **flags;
    char **inputs;
    char **incs;
    char **inc_paths;
    char **libs;
    char **lib_paths;
    char **vars;

    WrkTarget *parent;
} WrkTarget;

typedef struct WrkState {
    TCCState *tcc;
    bool run;
} WrkState;


/* Wrk State Functions */
WRK_RESULT_ENUM wrk_state_create(WkrState *state, bool run);


/* Target Functions */
WrkTarget *wrk_target_create(char *name, WRK_TARGET_TYPE_ENUM type);
WRK_TARGET_TYPE_ENUM wrk_target_destroy(WrkTarget *target);

WrkTarget *wrk_target_dup(WrkTarget *target);
WRK_RESULT_ENUM wrk_target_link(WrkTarget *parent, WrkTarget *target);
WRK_RESULT_ENUM wrk_target_merge(WrkTarget *dest, WrkTarget *src);
WrkTarget *wrk_target_collapse(WrkTarget *target);

char *wrk_target_command(WrkTarget *target);


/* Building */
WRK_TARGET_TYPE_ENUM wrk_target_build(WrkState *state, WrkTarget *target);

WrkTarget *wrk_run(WrkState *state, WrkTarget *prototype, char *work_file);

#endif
