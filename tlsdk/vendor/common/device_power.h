/*
 * mouse_power.h
 *
 *  Created on: Feb 12, 2014
 *      Author: xuzhen
 */

#ifndef DEVICE_POWER_H_
#define DEVICE_POWER_H_

#include "user_config.h"

#ifndef DEVICE_DEEPSLEEP_EN
#define DEVICE_DEEPSLEEP_EN	1
#endif

#define MCU_SIM_SUSPEND     0   //define 1: debug mode, no suspend or deep-sleep

#define M_SUSPEND_0 		0
#define M_SUSPEND_8MS 		1
#define M_SUSPEND_100MS 	2
#define M_SUSPEND_SNSR_SLP	0x08
#define M_SUSPEND_MCU_SLP	0x10
#define M_SUSPEND_DEEP_SLP	0x18

#define SLEEP_MODE_WAIT_DEEP    		3

typedef struct{ 
    u32  wakeup_src;
    u32  wakeup_time;

    u16  cnt_8ms;               //count increase if no mouse action within 8ms
    u16  thresh_8ms;    

    u16  cnt_100ms;             //count increase if no mouse action for longer suspend, // may still keep RF link, and after reach some limit may need goto deep sleep mode
    u16  thresh_100ms;

    u8   mode;   
    u8   mcu_sleep_en;      
    u8   sensor_sleep; 
    u8   sensor_sleep_en;

    u8   quick_sleep;    
    u8   device_busy;     
} sleep_cfg_t;

extern sleep_cfg_t device_sleep;
void device_sleep_mode_machine(sleep_cfg_t *s_cfg);
void kb_sleep_mode_machine(sleep_cfg_t *s_cfg);

/*
 *   based on power saving mode, need setup the wake up pin, 32K timer etc
 *
 */
extern int device_sync;
static inline void device_sleep_wakeup( sleep_cfg_t *s_cfg ){
	if ( MCU_SIM_SUSPEND || s_cfg->mode == M_SUSPEND_0 ){
        cpu_suspend_wakeup_sim( 8000 );
	}
	else{
		int wakeup_status = cpu_sleep_wakeup_rc (M_SUSPEND_MCU_SLP & s_cfg->mode, s_cfg->wakeup_src, s_cfg->wakeup_time);   // 8ms wakeup
		if(!(wakeup_status & 2)){
            device_sync = 0;	//ll_channel_alternate_mode ();
		}
    }
}

#endif /* MOUSE_POWER_H_ */
