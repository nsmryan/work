#ifndef _WRK_H_
#define _WRK_H_

//#include <stdint.h>
#include <stdbool.h>

#include "libtcc.h"


// work run    - run wrk.c files and run the result. this is the normal use
// work build  - build obj or exe, perhaps for each file, perhaps as one
//   -e --each - bulids each file into an obj, instead of linking in memory
//
// work list   - list all source files (for use with make or entr)
// word dryrun - print commands that work would run

typedef enum WRK_RESULT_ENUM {
    WRK_RESULT_OKAY     = 1,
    WRK_RESULT_NULL_PTR = 2,
    WRK_RESULT_UNEXPECTED_TYPE = 3,
    WRK_RESULT_ERROR    = 4,
} WRK_RESULT_ENUM;

// NOTE archive files have no option in tcc. may need to make separately
typedef enum WRK_TARGET_TYPE_ENUM {
    WRK_TARGET_TYPE_NAMESPACE = 1,
    WRK_TARGET_TYPE_OBJ       = 2,
    WRK_TARGET_TYPE_AR        = 3,
    WRK_TARGET_TYPE_EXE       = 4,
    WRK_TARGET_TYPE_SO        = 5,
    WRK_TARGET_TYPE_CMD       = 6,
} WRK_TARGET_TYPE_ENUM;


struct WrkTarget;

typedef struct WrkTarget {
    char *name;
    WRK_TARGET_TYPE_ENUM type;

    char *tool;
    char *output;

    char **flags;
    char **inputs;
    char **inc_paths;
    char **libs;
    char **lib_paths;
    char **vars;
    char **var_values;

    TCCState *tcc;

    struct WrkTarget *parent;
} WrkTarget;

typedef struct WrkState {
    TCCState *tcc;
    bool run;
} WrkState;

typedef WrkTarget *WrkMain(WrkState *state, WrkTarget *target);


/* Wrk State Functions */
WRK_RESULT_ENUM wrk_state_create(WrkState *state, bool run);


/* Target Functions */
WrkTarget *wrk_target_create(char *name, WRK_TARGET_TYPE_ENUM type);
WRK_TARGET_TYPE_ENUM wrk_target_destroy(WrkTarget *target);

WrkTarget *wrk_target_dup(WrkTarget *target);
void wrk_target_link(WrkTarget *parent, WrkTarget *target);
WRK_RESULT_ENUM wrk_target_merge(WrkTarget *dest, WrkTarget *src);
WrkTarget *wrk_target_collapse(WrkTarget *target);

/* Add Files to Target */
// single
void wrk_target_add_input(WrkTarget *target, char *name);
void wrk_target_add_flag(WrkTarget *target, char *name);
void wrk_target_add_include_path(WrkTarget *target, char *name);
void wrk_target_add_lib_path(WrkTarget *target, char *name);
void wrk_target_add_lib(WrkTarget *target, char *name);

// group
void wrk_target_add_inputs(WrkTarget *target, char *name[]);
void wrk_target_add_flags(WrkTarget *target, char *name[]);
void wrk_target_add_include_paths(WrkTarget *target, char *name[]);
void wrk_target_add_lib_paths(WrkTarget *target, char *name[]);
void wrk_target_add_libs(WrkTarget *target, char *name[]);

// utilities
void wrk_target_execute(WrkState *wrk_state, WrkTarget *target);

/* Building */
WRK_RESULT_ENUM wrk_target_build(WrkState *state, WrkTarget *target);
WRK_RESULT_ENUM wrk_output_exe(WrkState *wrk_state, WrkTarget *target);
WRK_RESULT_ENUM wrk_output_file(WrkState *wrk_state, WrkTarget *target);

WrkTarget *wrk_run_workfile(WrkState *state, WrkTarget *prototype, char *work_file);

#endif
