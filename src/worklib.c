#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

WRK_RESULT_ENUM wrk_state_create(WrkState *state, bool run) {
    TCCState *tcc = tcc_new();
    if (tcc == NULL) {
        log_error("%s failed to create TCCState", __FUNCTION__);
        return WRK_RESULT_NULL_PTR;
    }

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

WRK_RESULT_ENUM wrk_target_build(WrkState *wrk_state, WrkTarget *target) {
    int ret = 0;
    for (uint32_t inc_index = 0; inc_index < buf_size(target->inc_paths); inc_index++) {
        ret = tcc_add_include_path(wrk_state->tcc, target->inc_paths[inc_index]);
        if (ret != 0) {
            return WRK_RESULT_ERROR;
        }
    }

    for (uint32_t var_index = 0; var_index < buf_size(target->vars); var_index++) {
        tcc_define_symbol(wrk_state->tcc, target->vars[var_index], target->var_values[var_index]);
    }

    for (uint32_t lib_path_index = 0; lib_path_index < buf_size(target->lib_paths); lib_path_index++) {
        ret = tcc_add_library_path(wrk_state->tcc, target->lib_paths[lib_path_index]);
        if (ret != 0) {
            return WRK_RESULT_ERROR;
        }
    }

    for (uint32_t lib_index = 0; lib_index < buf_size(target->libs); lib_index++) {
        ret = tcc_add_library(wrk_state->tcc, target->libs[lib_index]);
        if (ret != 0) {
            return WRK_RESULT_ERROR;
        }
    }

    for (uint32_t input_index = 0; input_index < buf_size(target->inputs); input_index++) {
        ret = tcc_add_file(wrk_state->tcc, target->inputs[input_index]);
        if (ret != 0) {
            return WRK_RESULT_ERROR;
        }
    }

    for (uint32_t flag_index = 0; flag_index < buf_size(target->flags); flag_index++) {
        // TODO do we need to concat options first, or can we feed them one-by-one like this?
        ret = tcc_set_options(wrk_state->tcc, target->flags[flag_index]);
        if (ret != 0) {
            return WRK_RESULT_ERROR;
        }
    }

    ret = tcc_add_file(wrk_state->tcc, target->name);
    if (ret != 0) {
        return WRK_RESULT_ERROR;
    }

    return WRK_RESULT_OKAY;;
}

WrkTarget *wrk_run(WrkState *wrk_state, WrkTarget *prototype, char *work_file) {
    log_trace("wrk_run for '%s'", work_file);

    TCCState *tcc = tcc_new();
    if (tcc == NULL) {
        log_error("%s failed to create TCCState", __FUNCTION__);
        return NULL;
    }

    int ret = 0;

    ret = tcc_add_library_path(tcc, ".");
    if (ret != 0) {
        log_error("%s failed to add library path", __FUNCTION__);
        return NULL;
    }

    ret = tcc_add_include_path(tcc, "inc");
    if (ret != 0) {
        log_error("%s failed to add include path", __FUNCTION__);
        return NULL;
    }

    // TODO add file or add library?
    ret = tcc_add_file(tcc, "libwork.so");
    if (ret != 0) {
        log_error("%s failed to add library (%d)", __FUNCTION__, ret);
        return NULL;
    }

    ret = tcc_add_file(tcc, work_file);
    if (ret != 0) {
        log_error("%s failed to add file %s", __FUNCTION__, work_file);
        return NULL;
    }

    ret = tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);
    if (ret != 0) {
        log_error("%s failed to set output type");
        return NULL;
    }

    ret = tcc_relocate(tcc, TCC_RELOCATE_AUTO);
    if (ret != 0) {
        log_error("%s failed to relocate");
        return NULL;
    }

    WrkTarget *result_target = NULL;

    WrkMain *wrk_main = tcc_get_symbol(tcc, "wrk_main");
    if (wrk_main == NULL) {
        log_error("%s failed to relocate");
        return NULL;
    }

    log_trace("wrk_run running wrk_main");
    result_target = wrk_main(wrk_state, prototype);

    return result_target;
}

