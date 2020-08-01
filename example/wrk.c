#include "stdio.h"
#include "work.h"


 WrkTarget *wrk_main(WrkState *wrk_state, WrkTarget *target) {
     printf("Running wrk_main in example\n");

     printf("creating target\n");
     WrkTarget *main_target = wrk_target_create("main", WRK_TARGET_TYPE_EXE);

     printf("linking target\n");
     wrk_target_link(target, main_target);

     printf("adding input to target\n");
     // TODO this should not require the 'example' path
     wrk_target_add_input(main_target, "example/main.c");

     wrk_target_add_include_path(main_target, "/usr/include");
     wrk_target_add_include_path(main_target, "/usr/include/x86_64-linux-gnu");
     wrk_target_add_include_path(main_target, "/usr/local/lib/tcc/include");

     printf("adding flag to target\n");
     wrk_target_add_flag(main_target, "-g");

     printf("building main\n");
     wrk_target_build(wrk_state, main_target);

     printf("wrk.c finished\n");

     return NULL;
 }

