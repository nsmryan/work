
#include "stdio.h"
#include "stdlib.h"

#include "libtcc.h"


int main(int argc, char *argv[]) {
    printf("Hello tcc!\n");

    int ret = 0;

    TCCState *tcc = NULL;

    tcc = tcc_new();
    if (tcc == NULL) {
        printf("Could not create tcc state!\n");
        exit(0);
    }

    ret = tcc_add_file(tcc, "./ex.c");
    if (ret != 0) {
        printf("Add file failed (%d)\n", ret);
        exit(0);
    }

    tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);
    tcc_relocate(tcc, TCC_RELOCATE_AUTO);

    int (*func)(int) = tcc_get_symbol(tcc, "func");
    if (func == NULL) {
        printf("Could not find 'func'!\n");
        exit(0);
    }

    ret = func(1);
    printf("func(1) = %d\n", ret);

    tcc_delete(tcc);


    tcc = tcc_new();
    if (tcc == NULL) {
        printf("Could not create tcc state!\n");
        exit(0);
    }

    ret = tcc_add_file(tcc, "./ex2.c");
    if (ret != 0) {
        printf("Add file failed (%d)\n", ret);
        exit(0);
    }

    tcc_set_output_type(tcc, TCC_OUTPUT_EXE);
    tcc_relocate(tcc, TCC_RELOCATE_AUTO);

    int (*func2)(int) = tcc_get_symbol(tcc, "func");
    if (func2 == NULL) {
        printf("Could not find 'func2'!\n");
        exit(0);
    }

    ret = func2(1);
    printf("func2(1) = %d\n", ret);

    tcc_delete(tcc);
}

