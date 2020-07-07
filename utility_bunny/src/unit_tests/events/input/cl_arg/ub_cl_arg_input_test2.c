/**
 *  @file ub_cl_arg_input_test2.c
 *  @brief 
 *  @author John F.X. Galea
 */

#include "dr_defines.h"
#include "dr_api.h"
#include "../../../../events/input/cl_arg/ub_event_cl_arg_input.h"
#include "../../../../readers/ub_file_reader.h"
#include <string.h>

int count = 0;

void *cl_arg_input_ctx;

void got_cl_data(ub_ev_cl_arg_data_t *data) {

	count++;

	DR_ASSERT_MSG(data->index == 0,
			"ub_cl_arg_input_option_test - 0 failed...");


	char *base = ub_fr_get_basename(data->arg);

	DR_ASSERT_MSG(strcmp(base, "ub_cl_arg_test_app") == 0,
			"ub_cl_arg_input_option_test - 1 failed...");
}

static void event_exit() {

	DR_ASSERT_MSG(count == 1,
			"ub_cl_arg_input_option_test - 2 failed...");

	ub_ev_cl_arg_input_destroy_ctx(cl_arg_input_ctx);
}

DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[]) {

	cl_arg_input_ctx = ub_ev_cl_arg_input_init_ctx(got_cl_data, true,
			UB_EV_CL_ARG_INPUT_OPT_HANDLE_0TH);

	ub_ev_cl_arg_input_parse(cl_arg_input_ctx);

	dr_register_exit_event(event_exit);
}
