#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "libtcc.h"
#include "buf.h"
#include "log.h"

#include "work.h"


WrkTarget *wrk_target_create(char *name, WRK_TARGET_TYPE_ENUM type) {
    WrkTarget *target = (WrkTarget*)calloc(sizeof(WrkTarget), 1);

    target->name = name;
    target->type = type;

    return target;
}

void wrk_error_func(void *wrk_state_ptr, const char *msg) {
    printf("tcc error '%s'\n", msg);
}

WRK_RESULT_ENUM wrk_state_create(WrkState *state, bool run) {
    TCCState *tcc = tcc_new();
    if (tcc == NULL) {
        log_error("%s failed to create TCCState", __FUNCTION__);
        return WRK_RESULT_NULL_PTR;
    }

    tcc_set_error_func(tcc, (void*)state, wrk_error_func);

    memset(state, 0, sizeof(*state));

    state->run = run;
    state->tcc = tcc;

    return WRK_RESULT_OKAY;
}

void wrk_target_link(WrkTarget *dst, WrkTarget *src) {
    src->parent = dst;
}

void wrk_target_add_input(WrkTarget *target, char *name) {
    buf_push(target->inputs, name);
}

void wrk_target_add_inputs(WrkTarget *target, char *name[]) {
    uint32_t index = 0;
    while (name[index] != NULL) {
        wrk_target_add_input(target, name[index]);
        index++;
    }
}


void wrk_target_add_flag(WrkTarget *target, char *name) {
    buf_push(target->flags, name);
}

void wrk_target_add_flags(WrkTarget *target, char *name[]) {
    uint32_t index = 0;
    while (name[index] != NULL) {
        wrk_target_add_flag(target, name[index]);
        index++;
    }
}

void wrk_target_add_include_path(WrkTarget *target, char *name) {
    buf_push(target->inc_paths, name);
}

void wrk_target_add_include_paths(WrkTarget *target, char *name[]) {
    uint32_t index = 0;
    while (name[index] != NULL) {
        wrk_target_add_include_path(target, name[index]);
        index++;
    }
}

void wrk_target_add_lib_path(WrkTarget *target, char *name) {
    buf_push(target->lib_paths, name);
}

void wrk_target_add_lib_paths(WrkTarget *target, char *name[]) {
    uint32_t index = 0;
    while (name[index] != NULL) {
        wrk_target_add_lib_path(target, name[index]);
        index++;
    }
}

void wrk_target_add_lib(WrkTarget *target, char *name) {
    buf_push(target->libs, name);
}

void wrk_target_add_libs(WrkTarget *target, char *name[]) {
    uint32_t index = 0;
    while (name[index] != NULL) {
        wrk_target_add_lib(target, name[index]);
        index++;
    }
}

WRK_RESULT_ENUM wrk_target_build(WrkState *wrk_state, WrkTarget *target) {
    int ret = 0;

    if (target->tcc == NULL) {
        target->tcc = tcc_new();
    }

    if (target->tcc == NULL) {
        log_error("TCC context creation failure");
    }

    tcc_set_error_func(target->tcc, NULL, wrk_error_func);

    log_trace("adding flags");
    for (uint32_t flag_index = 0; flag_index < buf_size(target->flags); flag_index++) {
        log_trace("\tflag '%s'", target->flags[flag_index]);
        tcc_set_options(target->tcc, target->flags[flag_index]);
        if (ret != 0) {
            log_error("tcc error %d", ret);
            return WRK_RESULT_ERROR;
        }
    }

    log_trace("adding include paths");
    for (uint32_t inc_index = 0; inc_index < buf_size(target->inc_paths); inc_index++) {
        log_trace("\tinclude '%s'", target->inc_paths[inc_index]);
        ret = tcc_add_include_path(target->tcc, target->inc_paths[inc_index]);
        if (ret != 0) {
            log_error("tcc error %d", ret);
            return WRK_RESULT_ERROR;
        }
    }

    log_trace("adding symbols");
    for (uint32_t var_index = 0; var_index < buf_size(target->vars); var_index++) {
        log_trace("\tsymbol '%s'", target->vars[var_index]);
        tcc_define_symbol(target->tcc, target->vars[var_index], target->var_values[var_index]);
    }

    log_trace("adding library paths");
    for (uint32_t lib_path_index = 0; lib_path_index < buf_size(target->lib_paths); lib_path_index++) {
        log_trace("\tlib path '%s'", target->lib_paths[lib_path_index]);
        ret = tcc_add_library_path(target->tcc, target->lib_paths[lib_path_index]);
        if (ret != 0) {
            log_error("tcc error %d", ret);
            return WRK_RESULT_ERROR;
        }
    }

    log_trace("adding files");
    for (uint32_t input_index = 0; input_index < buf_size(target->inputs); input_index++) {
        log_trace("\tfile '%s'", target->inputs[input_index]);
        ret = tcc_add_file(target->tcc, target->inputs[input_index]);
        if (ret != 0) {
            log_error("tcc error %d", ret);
            return WRK_RESULT_ERROR;
        }
    }

    log_trace("adding libraries");
    for (uint32_t lib_index = 0; lib_index < buf_size(target->libs); lib_index++) {
        log_trace("\tlib '%s'", target->libs[lib_index]);
        // NOTE tcc_add_file vs tcc_add_library
        ret = tcc_add_library(target->tcc, target->libs[lib_index]);
        //ret = tcc_add_file(target->tcc, target->libs[lib_index]);
        if (ret != 0) {
            log_error("tcc error %d", ret);
            return WRK_RESULT_ERROR;
        }
    }

    return WRK_RESULT_OKAY;
}

WRK_RESULT_ENUM wrk_output_file(WrkState *wrk_state, WrkTarget *target) {
    assert(NULL != wrk_state);
    assert(NULL != target);

    log_trace("setting output type");

    int tcc_type = 0;
    char *output_type_name = NULL;

    // not used: TCC_OUTPUT_MEMORY, TCC_OUTPUT_PREPROCESS
    switch (target->type) {
        case WRK_TARGET_TYPE_SO:
            tcc_type = TCC_OUTPUT_DLL;
            output_type_name = "shared object";
            break;

        case WRK_TARGET_TYPE_OBJ:
            tcc_type = TCC_OUTPUT_OBJ;
            output_type_name = "object file";
            break;

        case WRK_TARGET_TYPE_EXE:
            tcc_type = TCC_OUTPUT_EXE;
            output_type_name = "executable";
            break;

        case WRK_TARGET_TYPE_NAMESPACE:
            output_type_name = "namespace";
            break;

        default:
            output_type_name = "UnexpectedType";
            tcc_type = 0;
    }

    assert(NULL != output_type_name);

    log_trace("tcc_type = '%s' (%d)", output_type_name, tcc_type);
    if (tcc_type == 0) {
        log_error("Unexpected target type when outputting file. Type = %d", target->type);
        return WRK_RESULT_UNEXPECTED_TYPE;
    }

    log_trace("setting output to %d", tcc_type);
    int ret = tcc_set_output_type(target->tcc, tcc_type);
    if (ret != 0) {
        log_error("%s failed to set output type (%d)", __FUNCTION__, ret);
        return WRK_RESULT_ERROR;
    }

    log_trace("outputting file %s", target->name);
    ret = tcc_output_file(target->tcc, target->name);
    if (ret != 0) {
        log_error("%s failed to output file (%d)", __FUNCTION__, ret);
        return WRK_RESULT_ERROR;
    }

    log_trace("finished output of %s with type %s", target->name, output_type_name);

    return WRK_RESULT_OKAY;
}

WrkTarget *wrk_run_workfile(WrkState *wrk_state, WrkTarget *prototype, char *work_file) {
    log_trace("wrk_run for '%s'", work_file);

    int ret = 0;

    ret = tcc_add_library_path(wrk_state->tcc, "/usr/lib/x86_64-linux-gnu");
    if (ret != 0) {
        log_error("%s failed to add library path", __FUNCTION__);
        return NULL;
    }

    ret = tcc_add_library_path(wrk_state->tcc, "/usr/local/lib");
    if (ret != 0) {
        log_error("%s failed to add library path", __FUNCTION__);
        return NULL;
    }

    log_trace("adding include paths");
    ret = tcc_add_include_path(wrk_state->tcc, "inc");
    if (ret != 0) {
        log_error("%s failed to add include path", __FUNCTION__);
        return NULL;
    }

    ret = tcc_add_include_path(wrk_state->tcc, "/usr/include");
    if (ret != 0) {
        log_error("%s failed to add include path", __FUNCTION__);
        return NULL;
    }

    ret = tcc_add_include_path(wrk_state->tcc, "/usr/local/lib/tcc/include");
    if (ret != 0) {
        log_error("%s failed to add include path", __FUNCTION__);
        return NULL;
    }

    ret = tcc_add_include_path(wrk_state->tcc, "/usr/local/include");
    if (ret != 0) {
        log_error("%s failed to add include path", __FUNCTION__);
        return NULL;
    }

    // TODO this is not portable
    ret = tcc_add_include_path(wrk_state->tcc, "/usr/include/x86_64-linux-gnu");
    if (ret != 0) {
        log_error("%s failed to add include path", __FUNCTION__);
        return NULL;
    }

    // TODO should create a libworklib.so and just link this, or .a
    log_trace("adding worklib.o");
    ret = tcc_add_file(wrk_state->tcc, "worklib.o");
    if (ret != 0) {
        log_error("%s failed to add library (%d)", __FUNCTION__, ret);
        return NULL;
    }

    log_trace("adding log.o");
    ret = tcc_add_file(wrk_state->tcc, "log.o");
    if (ret != 0) {
        log_error("%s failed to add library (%d)", __FUNCTION__, ret);
        return NULL;
    }

    log_trace("adding libtcc.a");
    ret = tcc_add_library(wrk_state->tcc, "tcc");
    if (ret != 0) {
        log_error("%s failed to add library (%d)", __FUNCTION__, ret);
        return NULL;
    }

    log_trace("adding '%s'", work_file);
    ret = tcc_add_file(wrk_state->tcc, work_file);
    if (ret != 0) {
        log_error("%s failed to add file %s", __FUNCTION__, work_file);
        return NULL;
    }

    log_trace("adding '%s'", "-g");
    tcc_set_options(wrk_state->tcc, "-g");
    if (ret != 0) {
        log_error("%s failed to add file %s", __FUNCTION__, work_file);
        return NULL;
    }

    log_trace("setting output type");
    ret = tcc_set_output_type(wrk_state->tcc, TCC_OUTPUT_MEMORY);
    if (ret != 0) {
        log_error("%s failed to set output type");
        return NULL;
    }

    log_trace("relocating");
    ret = tcc_relocate(wrk_state->tcc, TCC_RELOCATE_AUTO);
    if (ret != 0) {
        log_error("%s failed to relocate");
        return NULL;
    }

    WrkTarget *result_target = NULL;

    WrkMain *wrk_main = tcc_get_symbol(wrk_state->tcc, "wrk_main");
    if (wrk_main == NULL) {
        log_error("%s failed to get symbol wrk_main");
        return NULL;
    }

    log_trace("wrk_run running wrk_main");
    result_target = wrk_main(wrk_state, prototype);

    return result_target;
}

void wrk_target_execute(WrkState *wrk_state, WrkTarget *target) {
    uint32_t cmd_len = 0;

    assert(NULL != target->tool);
    cmd_len = strlen(target->tool);

    // get string length
    for (uint32_t index = 0; index < buf_size(target->flags); index++) {
        cmd_len += strlen(target->flags[index]) + strlen(" ");
    }

    for (uint32_t index = 0; index < buf_size(target->inc_paths); index++) {
        cmd_len += strlen(target->inc_paths[index]) + strlen(" -I");
    }

    for (uint32_t index = 0; index < buf_size(target->inputs); index++) {
        cmd_len += strlen( target->inputs[index]) + strlen(" ");
    }

    for (uint32_t index = 0; index < buf_size(target->lib_paths); index++) {
        cmd_len += strlen( target->lib_paths[index]) + strlen("-L ");
    }

    for (uint32_t index = 0; index < buf_size(target->libs); index++) {
        cmd_len += strlen(target->libs[index]) + strlen("-l ");
    }

    for (uint32_t index = 0; index < buf_size(target->vars); index++) {
        cmd_len += strlen(target->vars[index]) + strlen("-D");
    }

    for (uint32_t index = 0; index < buf_size(target->var_values); index++) {
        cmd_len += strlen(target->var_values[index]) + strlen("= ");
    }

    // build command string
    // NOTE quadratic complexity string concat
    char *cmd = (char*)calloc(cmd_len, 1);

    strcat(cmd, target->tool);
    strcat(cmd, " ");

    for (uint32_t index = 0; index < buf_size(target->flags); index++) {
        strcat(cmd, target->flags[index]);
        strcat(cmd, " ");
    }

    for (uint32_t index = 0; index < buf_size(target->inc_paths); index++) {
        strcat(cmd, "-I");
        strcat(cmd, target->inc_paths[index]);
        strcat(cmd, " ");
    }

    for (uint32_t index = 0; index < buf_size(target->inputs); index++) {
        strcat(cmd, target->inputs[index]);
        strcat(cmd, " ");
    }

    for (uint32_t index = 0; index < buf_size(target->lib_paths); index++) {
        strcat(cmd, "-L");
        strcat(cmd, target->lib_paths[index]);
        strcat(cmd, " ");
    }

    for (uint32_t index = 0; index < buf_size(target->libs); index++) {
        strcat(cmd, "-l");
        strcat(cmd, target->libs[index]);
        strcat(cmd, " ");
    }

    for (uint32_t index = 0; index < buf_size(target->vars); index++) {
        strcat(cmd, "-D");
        strcat(cmd, target->vars[index]);
        strcat(cmd, "=");
        strcat(cmd, target->var_values[index]);
        strcat(cmd, " ");
    }

    log_trace("%s", cmd);
    system(cmd);

    if (NULL != cmd) {
        free(cmd);
    }
}
