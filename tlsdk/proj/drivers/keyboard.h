
#pragma once

#include "../drivers/usbkeycode.h"

#define KB_SWITCH_NUM		8
#define KB_RETURN_KEY_MAX	6

#define	KB_STATUS_LOCK						BIT(4)
#define	KB_NUMLOCK_STATUS_INVALID			BIT(7)
#define	KB_NUMLOCK_STATUS_POWERON			BIT(15)

typedef struct{
	u8 cnt;
	u8 ctrl_key;
	u8 keycode[KB_RETURN_KEY_MAX]; //6
	//u8 padding[2];	//  for  32 bit padding,  if KB_RETURN_KEY_MAX change,  this should be changed
}kb_data_t;

typedef struct {
	kb_data_t *last_send_data;
	u32 last_send_time;
	u8 resent_no_ack_cnt;
	u8 key_pressed;
	u8 frame_no;
	u8 ack_frame_no;
}rf_kb_ctrl_t;

typedef struct{
	u8 val_org;             //  转码前
	u8 val_chg;             //  转码后
	u8 this_pressed;        //  本次scan是否按下
	u8 last_pressed;        //  本次scan之前已经被按过，尚未发出
	u32 longpress_th;       //  长按门限,ms
	u32 press_start_time;   //  上次按下时间
	u8 longpressed;         //  已经达到长按时间
	u8 processed;           //  已经处理了本次长按动作，不再重复处理
}kb_longpress_ctrl_t;

typedef struct{
	u8 len;
	u8 data[3];

}kb_switch_key_t;

extern kb_switch_key_t *kb_switch_key[8];


enum{
    KB_NOT_LONGPRESS,
	KB_IS_LONGPRESS,
    KB_KEY_RELEASE,
};

static inline int kb_is_key_valid(kb_data_t *p)
{
	return (p->cnt || p->ctrl_key);
}

static inline void kb_set_key_invalid(kb_data_t *p)
{
	p->cnt = p->ctrl_key = 0;
}

extern rf_kb_ctrl_t rf_kb_ctrl;

#if(MCU_CORE_TYPE == MCU_CORE_8368 || MCU_CORE_TYPE == MCU_CORE_8266)
extern u16 scan_pins[];
extern u16 drive_pins[];
#else
extern u32 scan_pins[];
extern u32 drive_pins[];
#endif
//int kb_is_data_same(kb_data_t *a, kb_data_t *b);
//int kb_check_longpress(kb_longpress_ctrl_t *ctrl, int i, int *key_idx);
//void kb_init(void);
u32 kb_scan_key (int numlock_status, int read_key);
