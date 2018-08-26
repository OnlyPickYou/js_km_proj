#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../common/rf_frame.h"
#include "device_info.h"
#include "mouse.h"
#include "mouse_custom.h"

#define			PM_REG_START		0x19
#define			PM_REG_END			0x1f

#ifndef PM_POWERON_DETECTION_ENABLE
#define PM_POWERON_DETECTION_ENABLE 0
#endif


/*
 *  Base on from power on or deep sleep back
 *  Load customizable information from the 3.3V Analog register
 *  or Load from OTP
 *
 */
void device_info_load(mouse_status_t *mouse_status)
{

	u8 mode   = analog_read(PM_REG_START);
	u8 sensor = analog_read(PM_REG_START+1);
	paired_info.num    = analog_read(PM_REG_START+2);

    mouse_status->mouse_mode = mode ? STATE_NORMAL : STATE_POWERON;

//   Need get poweron, cpi, etc back first
    if ( mouse_status->mouse_mode != STATE_POWERON ){
    	mouse_status->cpi = sensor & INFO_SENSOR_CPI_CTRL;
        mouse_status->mouse_sensor = sensor & INFO_SENSOR_STATUS_CTRL;

    }

}

#if DEVICE_INFO_STORE
/*
 * Save the information need from the deep sleep back
 *
 */
void device_info_save(mouse_status_t *mouse_status, u32 sleep_save)
{
	u8 mode = mouse_status->mouse_mode;
    u8 sensor = (mouse_status->mouse_sensor & INFO_SENSOR_STATUS_CTRL) | (mouse_status->cpi & INFO_SENSOR_CPI_CTRL) ;

    analog_write (PM_REG_START, mode );
    analog_write ( (PM_REG_START+1), sensor );
    analog_write ( (PM_REG_START + 2), paired_info.num);

}
#endif

