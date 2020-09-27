/*
 * stack_checker.c
 *
 *      Author: john
 */

#include "cfi_bunny.h"
#include "instrumentation_bunny.h"

void *taint_bunny;
void *cfi_bunny;
void *fd_input_ctx;

static void event_exit() {

    ub_ev_fd_input_destroy_ctx(fd_input_ctx);

    cfib_exit(cfi_bunny);
    tb_raw_exit(taint_bunny);
    ib_exit();
    drmgr_exit();
}

static void got_read(ub_fd_input_data_t *input_data) {

    ub_input_buf_t *input_buf = (ub_input_buf_t *) ub_list_get_ith_value(
            input_data->buf_list, 0);

    if (input_data->type == UB_FD_FILE) {

        if (strstr(input_data->source_info.file_info.base_name, ".so") == NULL) {

            tb_taint_mem_block(taint_bunny, (uintptr_t) input_buf->buf,
                    input_buf->size, (void *) 1);
        }
    } else {

        tb_taint_mem_block(taint_bunny, (uintptr_t) input_buf->buf,
                input_buf->size, (void *) 1);

    }
}

static void cfi_handle_detection_func(cfib_attack_type_t type,
        ub_opnd_access_t *corrupt_slot, void *user_data) {

    DR_ASSERT(false);
}

DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[]) {


    drmgr_init();
    ib_init(NULL);

    tb_raw_taint_policy_t taint_policy;
    tb_raw_initialise_bitwise_taint_policy(&taint_policy);

    tb_options_t te_options;
    te_options.enable_inline = false;
    te_options.enable_taint_off = false;
    te_options.enable_fp = true;


    taint_bunny = tb_simd_init(id, IB_OPT_BARE, &te_options, NULL,
            &taint_policy);

    cfi_bunny = cfib_init(taint_bunny, cfi_handle_detection_func,
            CFIB_CALL_ATTACK | CFIB_RET_ATTACK | CFIB_JMP_ATTACK,
            IB_OPT_PC_INFO, NULL);

    cfib_register_hooks(cfi_bunny);
    tb_register_hooks(taint_bunny);

    fd_input_ctx =
            ub_ev_fd_input_init_ctx(got_read, true,
                    UB_EV_FD_INPUT_OPT_LOCAL_SOCKET
                            | UB_EV_FD_INPUT_OPT_SOURCE_INFO
                            | UB_EV_FD_INPUT_OPT_REMOTE_SOCKET
                            | UB_EV_FD_INPUT_OPT_FILE, id);

    dr_register_exit_event(event_exit);

}
