

#include "work.h"


 WrkTarget *wrk_main(WrkState *wrk_state, WrkTarget *target) {
     printf("Running wrk_main in example\n");

     WrkTarget *main = wrk_target_create("main", WRK_TARGET_TYPE_EXE);
     wrk_target_link(target, main);
     wrk_target_add_input("main.c");

     wrk_target_build(wrk_state, main);

     return NULL;
 }

