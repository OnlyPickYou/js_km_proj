#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../common/rf_frame.h"

#include "../common/device_power.h"

#include "../../proj/drivers/keyboard.h"

#include "kb_emi.h"
#include "kb_rf.h"
#include "kb_custom.h"
#include "kb_info.h"
#include "kb_batt.h"
#include "kb_pm.h"
#include "kb_ota.h"
#include "kb_led.h"
#include "kb_test.h"


kb_lock_status_t kb_lock_status = {
		0,
		0,

		0,
		0,
};

void kb_platform_init(void);
extern  void kb_keyScan_init(void);

kb_status_t  kb_status;

extern kb_data_t	kb_event;

void  user_init(void)
{
	//memset(&kb_status,0,sizeof(kb_status));
	kb_status.no_ack = 1;
	//kb_keyScan_init();
	kb_custom_init();
	kb_batt_det_init();
	kb_info_load();
	kb_rf_init();

	int status = kb_status.host_keyboard_status | (kb_status.kb_mode == STATE_POWERON ? KB_NUMLOCK_STATUS_POWERON : 0);
	kb_scan_key (status, 1);
	kb_status.host_keyboard_status = KB_NUMLOCK_STATUS_INVALID;

#if(HYX_ONE_2_THREE_DEVICE )

	for(u8 i=0; i<MAX_DONGLE_NUM; i++){
		if(paired_info.bind_id[i] != 0xffffffff){
			paired_info.cnt++;
		}
	}

	if(paired_info.cnt > 0){

		pkt_km.num = paired_info.num;

		kb_status.mode_link = LINK_PIPE_CODE_OK;
		kb_status.dongle_id = paired_info.bind_id[0];

    	kb_rf_pkt = (u8*)&pkt_km;
    	rf_set_tx_pipe (PIPE_KEYBOARD);
    	kb_status.kb_mode = STATE_NORMAL;
    	rf_set_access_code1 (kb_status.dongle_id);
    	rf_set_power_level_index (kb_cust_tx_power);

	}
	else{//poweron or link ERR deepback
    	rf_set_access_code1 ( U32_MAX );
    	rf_set_tx_pipe (PIPE_PARING);
    	rf_set_power_level_index (kb_cust_tx_power_paring);
	}
#endif

	kb_platform_init();
	kb_pm_init();

	//gpio_gp26_init;

}


void kb_platform_init(void)
{
	kb_device_led_init(kb_status.led_gpio_lvd, kb_status.led_level_lvd, 5);

	if(kb_status.led_gpio_scr){
		gpio_set_input_en( kb_status.led_gpio_scr, 0 );
	    gpio_set_output_en( kb_status.led_gpio_scr, 1 );
	    gpio_write(kb_status.led_gpio_scr,0^kb_status.led_level_scr);
	}
	if(kb_status.led_gpio_cap){
		gpio_set_input_en( kb_status.led_gpio_cap, 0 );
	    gpio_set_output_en( kb_status.led_gpio_cap, 1 );
	    gpio_write(kb_status.led_gpio_cap,0^kb_status.led_level_cap);
	}
	if(kb_status.led_gpio_num){
		gpio_set_input_en( kb_status.led_gpio_num, 0 );
	    gpio_set_output_en( kb_status.led_gpio_num, 1 );
	    gpio_write(kb_status.led_gpio_num,0^kb_status.led_level_num);
	}

	if(!gpio_read(GPIO_GP31)){
		SET_SINGLE_BATT();
	}
	analog_write (0x2a, analog_read(0x2a)&0x3f );  //disable GP31 1M pullup


	//emi paring±ØÐëÔÚSTATE_POWERON?
	if(kb_status.kb_mode == STATE_POWERON){
		if( (kb_event.keycode[0] == VK_ESC) && (kb_event.cnt == 1)){
			kb_status.kb_mode  = STATE_EMI;
		}
		else{
			kb_device_led_setup( kb_led_cfg[KB_LED_POWER_ON] );
			kb_status.kb_mode = STATE_SYNCING;
			memset(&kb_event,0,sizeof(kb_event));  //clear the numlock key when power on
		}
	}
#if(HYX_ONE_2_THREE_DEVICE)
	else if(kb_status.kb_mode == STATE_NORMAL){

		if( kb_pairing_mode_detect() ){

			pkt_pairing.num = (paired_info.cnt >= MAX_DONGLE_NUM) ? 0 : paired_info.cnt;

			kb_rf_pkt = (u8*)&pkt_pairing;

			pkt_pairing.flow = PKT_FLOW_PARING;
			kb_status.mode_link = 0;
			kb_device_led_setup(kb_led_cfg[KB_LED_MANUAL_PAIRING]);	//8Hz,fast blink
			kb_status.kb_mode  = STATE_PAIRING;
			rf_set_access_code1 ( U32_MAX );
			rf_set_tx_pipe (PIPE_PARING);

			rf_set_power_level_index (kb_cust_tx_power_paring);
		}

	}
#endif
}

_attribute_ram_code_ void irq_handler(void)
{
	u16  src_rf = reg_rf_irq_status;
	if(src_rf & FLD_RF_IRQ_RX){
		irq_device_rx();
	}

	if(src_rf & FLD_RF_IRQ_TX){
		//irq_device_tx();
		reg_rf_irq_status = FLD_RF_IRQ_TX;
	}
}

extern int kb_is_lock_pressed;



u32 key_scaned;
u32 kb_switch_dongle_num()
{

#if(!HYX_ONE_2_THREE_DEVICE)
	//static u8 kb_lock_cnt = 0;
	kb_lock_status.cur_kb_cnt = kb_event.cnt;
	kb_lock_status.cur_keycode = kb_event.keycode[0];

	if(kb_lock_status.cur_kb_cnt == 1 && kb_lock_status.cur_keycode == VK_LOCK && !kb_lock_status.last_kb_cnt){
		//kb_lock_cnt++;
		kb_is_lock_pressed = !kb_is_lock_pressed? KB_STATUS_LOCK : 0; //lock <-> unlock

		 //lock -> unlock
		if(!kb_is_lock_pressed && kb_status.host_keyboard_status == KB_NUMLOCK_STATUS_INVALID){
			kb_event.cnt = 0;  //send 0 to dongle, get ack to have numlcom status
		}
	}

	if(kb_is_lock_pressed){
		key_scaned = 0;
	}

	kb_lock_status.last_kb_cnt = kb_event.cnt;
	kb_lock_status.last_keycode = kb_event.keycode[0];
#else
	if(kb_event.cnt == 1 && kb_event.keycode[0] == VK_F1 ){

		paired_info.num = 0 | (paired_info.num << 4) | BIT(7);			//prev | next

		pkt_km.num = paired_info.num;
		paired_info.flag = KB_SWITCH_DONGLE_FLAG;
		memset(&kb_event, 0, sizeof(kb_data_t));

	}
	else if(kb_event.cnt == 1 && kb_event.keycode[0] == VK_F2){

		paired_info.num = 1 | (paired_info.num << 4) | BIT(7) ;

		pkt_km.num = paired_info.num;
		paired_info.flag = KB_SWITCH_DONGLE_FLAG;
		memset(&kb_event, 0, sizeof(kb_data_t));

	}
	else if(kb_event.cnt == 1 && kb_event.keycode[0] == VK_F3){

		paired_info.num = 2 | (paired_info.num << 4) | BIT(7);

		pkt_km.num = paired_info.num;
		paired_info.flag = KB_SWITCH_DONGLE_FLAG;
		memset(&kb_event, 0, sizeof(kb_data_t));
	}

	return 1;
#endif
}
void main_loop(void)
{
	cpu_rc_tracking_en(RC_TRACKING_32M_ENABLE);
	batt_det_count++;
#if(HYX_ONE_2_THREE_DEVICE)
	key_scaned = kb_scan_key (kb_status.host_keyboard_status, !km_dat_sending);
#else
	key_scaned = kb_scan_key (kb_status.host_keyboard_status | kb_is_lock_pressed, !km_dat_sending);
#endif

	if(key_scaned){
		kb_switch_dongle_num();
	}

	if(kb_status.kb_mode == STATE_EMI){
		kb_emi_process();
	}
	else{
		if(kb_status.kb_mode <= STATE_PAIRING){
			kb_paring_and_syncing_proc();
		}

		kb_rf_proc(key_scaned);
		kb_device_led_process();
		kb_batt_det_process();
		kb_pm_proc();
	}
	cpu_rc_tracking_disable;
	kb_status.loop_cnt++;

}



