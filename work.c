#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined(WORK_MAIN)
#include "log.h"
#endif

#include "work.h"

#define BUILD_DIR "build/"

char *gv_includes[] =
    { "/usr/include",
      "/usr/include/x86_64-linux-gnu",
      "/usr/include/lib",
      "/usr/local/lib/tcc/include",
      "/usr/local/lib",
      "/usr/local/include",
      "./dep/fs",
      "./dep/logc",
      "./dep/growable_buf",
      "./inc",
      NULL,
    };

char *gv_source[] =
    { "src/worklib.c",
      "dep/fs/fs.c",
      "dep/logc/log.c",
      NULL,
    };

char *gv_lib_paths[] =
    { "/usr/local/lib",
      "/usr/lib/x86_64-linux-gnu",
      NULL,
    };


WrkTarget *wrk_main(WrkState *wrk_state, WrkTarget *target);

#if defined(WORK_MAIN)
int main(int argc, char *argv[]) {
    WrkState wrk_state;
    wrk_state_create(&wrk_state, true);

    log_trace("creating target");
    WrkTarget *target = wrk_target_create(NULL, WRK_TARGET_TYPE_NAMESPACE);

    assert(NULL != target);

    wrk_main(&wrk_state, target);

    log_trace("My work here is done\n");
}
#endif

WrkTarget *wrk_main(WrkState *wrk_state, WrkTarget *target) {
    printf("work is doing the work to compile itself\n");

    // TODO need to ensure 'build' dir exists.
    // wrk_cmd(wrk_state, "mkdir", "build");

    /* WorkLib */
    WrkTarget *worklib = wrk_target_create(BUILD_DIR "worklib.o", WRK_TARGET_TYPE_OBJ);

    {
        int inc_index = 0;
        while (gv_includes[inc_index] != NULL) {
            wrk_target_add_include_path(worklib, gv_includes[inc_index]);
            inc_index++;
        }
    }

    {
        int src_index = 0;
        while (gv_source[src_index] != NULL) {
            wrk_target_add_input(worklib, gv_source[src_index]);
            src_index++;
        }
    }
    wrk_target_build(wrk_state, worklib);
    wrk_output_file(wrk_state, worklib);

    /* Work Executable */
    WrkTarget *work_target = wrk_target_create(BUILD_DIR "work", WRK_TARGET_TYPE_EXE);

    {
        int inc_index = 0;
        while (gv_includes[inc_index] != NULL) {
            wrk_target_add_include_path(work_target, gv_includes[inc_index]);
            inc_index++;
        }
    }

    wrk_target_add_lib(work_target, "dl");
    wrk_target_add_lib(work_target, "pthread");
    wrk_target_add_lib(work_target, "tcc");

    wrk_target_add_input(work_target, "src/main.c");
    wrk_target_add_input(work_target, "build/worklib.o");

    {
        int lib_path_index = 0;
        while (gv_lib_paths[lib_path_index] != NULL) {
            wrk_target_add_lib_path(work_target, gv_lib_paths[lib_path_index]);
            lib_path_index++;
        }
    }

    /* Build */
    printf("creating work\n");
    wrk_target_build(wrk_state, work_target);
    printf("creation done, not yet built\n");
    wrk_output_file(wrk_state, work_target);
    printf("file output\n");

    return work_target;
}

