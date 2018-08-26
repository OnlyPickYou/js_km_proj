/*
 * kb_info.c
 *
 *  Created on: 2015-1-21
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/rf_drv.h"
#include "../common/rf_frame.h"

#include "kb_rf.h"
#include "kb_info.h"
#include "kb_custom.h"


//u8 deep_count;
#if(!HYX_ONE_2_THREE_DEVICE)
extern int kb_is_lock_pressed;
#endif
void kb_info_load(void)
{
#if 1
	kb_status.mode_link = analog_read(PM_REG_MODE_LINK);
	paired_info.num = analog_read(PM_REG_CHECK);
#endif
}

void kb_info_save(void)
{
#if 1


	analog_write(PM_REG_MODE_LINK,kb_status.mode_link&LINK_PIPE_CODE_OK);
	analog_write(PM_REG_CHECK,paired_info.num);


#endif
}
