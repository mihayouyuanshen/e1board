#include "common_g.h"
#include "frm_dispatcher_g.h"
#include "util_logger_g.h"
#include <stddef.h>

void TEST_repeatadd(void)
{
	FRM_DispAddController("usr", NULL);
	LOG_debug("first is successful");
	FRM_DispAddController("usr", NULL);
	LOG_debug("second is error?");
}