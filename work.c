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
      "/usr/local/lib/tcc/include",
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

char *gv_libs[] =
    { "dl",
      "pthread",
      "tcc",
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

    WrkTarget *rm = wrk_target_create("rm", WRK_TARGET_TYPE_CMD);
    rm->tool = "rm";
    wrk_target_add_flag(rm, "build");
    wrk_target_add_flag(rm, "-r");
    wrk_target_execute(wrk_state, rm);

    WrkTarget *mkdir = wrk_target_create("mkdir", WRK_TARGET_TYPE_CMD);
    mkdir->tool = "mkdir";
    wrk_target_add_flag(mkdir, "build");
    wrk_target_execute(wrk_state, mkdir);

    /* WorkLib */
    WrkTarget *worklib = wrk_target_create(BUILD_DIR "worklib.o", WRK_TARGET_TYPE_OBJ);

    wrk_target_add_include_paths(worklib, gv_includes);
    wrk_target_add_inputs(worklib, gv_source);

    wrk_target_build(wrk_state, worklib);
    wrk_output_file(wrk_state, worklib);

    /* Work Executable */
    WrkTarget *work_target = wrk_target_create(BUILD_DIR "work", WRK_TARGET_TYPE_EXE);

    wrk_target_add_include_paths(work_target, gv_includes);
    wrk_target_add_libs(work_target, gv_libs);
    wrk_target_add_lib_paths(work_target, gv_lib_paths);

    wrk_target_add_input(work_target, "src/main.c");
    wrk_target_add_input(work_target, "build/worklib.o");

    /* Build */
    printf("creating work\n");
    wrk_target_build(wrk_state, work_target);
    printf("creation done, not yet built\n");
    wrk_output_file(wrk_state, work_target);
    printf("file output\n");

    // testing executable targets
    work_target->tool = "tcc";
    wrk_target_add_flag(work_target, "-o build/work_exe");
    wrk_target_execute(wrk_state, work_target);

    return work_target;
}

