/*
 * kb_custom.c
 *
 *  Created on: 2015-1-21
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/rf_drv.h"
#include "../common/rf_frame.h"

#include "kb_custom.h"
#include "kb_rf.h"

paired_info_t paired_info = {

		0xffffffff,
		0xffffffff,
		0xffffffff,
		0,
		0,
		0,
		0,


};

void (*device_program) (int adr, unsigned char id);
void (*device_program_on) (void);
void (*device_program_off) (void);

void otp_device_program_on (void);
void otp_device_program_off (void);
void otp_device_program(int , unsigned char);

void flash_device_program_on (void);
void flash_device_program_off (void);
void flash_device_program(int , unsigned char);

#if (DONGLE_ID_IN_FW || 1)
/****************************************************************************************************************
     set_device_id_in_firmware
     load paired device id into flash and ram
 ***************************************************************************************************************/
int set_device_id_in_firmware (int type, u32 id, paired_info_t *paired_info)
{
	int ret = 1;

	if(paired_info->bind_id[type] == id)
	{
		//do nothing
	}
	else if (paired_info->index < DEVICE_ID_PARING_MAX)
	{
		u32 adr = (DEVICE_PARING_INFO_ADDR + 6 * paired_info->index);


		device_program_on ();
		device_program (adr, type);
		device_program (adr+2, id);
		device_program (adr+3, id>>8);
		device_program (adr+4, id>>16);
		device_program (adr+5, id>>24);
		device_program_off ();



		u8 temp = *(volatile u8*)(adr);
		temp = *(volatile u8*)(adr+1024);
		temp = *(volatile u8*)(adr+2*1024);

		u8 type_read =  *(volatile u8*)(adr);
		u16 id_read_low = *(volatile u16*)(adr+2);
		u16 id_read_high = *(volatile u16*)(adr+4);

		if( type_read != type || ((id_read_high<<16) | id_read_low) != id ){
			return 0;
		}


		paired_info->bind_id[type & 3] = id;
		paired_info->index ++;
	}
	else {
		ret = 0;
	}
	return ret;
}


/****************************************************************************************************************
     device_program_on
     device_program
     device_program_off
 ***************************************************************************************************************/

//#if (OTP_PROGRAM)

//////////////////////////////////////////////////////
//copy from  dut otp code
//////////////////////////////////////////////////////
void otp_device_program_on (void){
	/* open vpp 6.75V */
	unsigned char irqst = read_reg8(0x800643);
	write_reg8(0x800643,0);

	write_reg8(0x800071, 0x13);;    // set DCDC 6.75 clk to 60M
	sleep_us(100);
	analog_write(0x85, 0x14);
	sleep_us(1000);   //wait at least 1000us
	analog_write(0x85, 0x54);
	sleep_us(100);

	write_reg8(0x800643,irqst);
}

void otp_device_program_off(void)
{
	analog_write(0x85, 0x0c);
	write_reg8(0x800071, 0x03);
}

#if 0
_attribute_ram_code_
#endif
void otp_device_program(int addr, unsigned char value){
	unsigned char irqst = read_reg8(0x800643);
	write_reg8(0x800643,0);

	write_reg8(0x800012, 0x7e);
	write_reg16(0x800010, addr);
	write_reg8(0x80001a, 0x02);
	write_reg8(0x800013, value);
	WaitUs (100);
	write_reg8(0x800013, value);
	write_reg8(0x80001a, 0x0);
	WaitUs (20);

	write_reg8(0x800643,irqst);
}

//#else

void flash_device_program_on (void){}
void flash_device_program_off(void){}
//flash program to emulate OTP
#if 1
_attribute_ram_code_
#endif
void flash_device_program (int adr, unsigned char id)
{
	unsigned char irqst = read_reg8(0x800643);
	write_reg8(0x800643,0);
	write_reg8 (0x80000d, 0x00);
	write_reg8 (0x80000c, 0x06);	//write enable
	while (read_reg8(0x80000d));
	write_reg8 (0x80000d, 0x01);

	WaitUs (1);

	write_reg8 (0x80000d, 0x00);
	write_reg8 (0x80000c, 0x02);	//write command
	while (read_reg8(0x80000d));

	write_reg8 (0x80000c, adr>>16);
	while (read_reg8(0x80000d));

	write_reg8 (0x80000c, adr>>8);
	while (read_reg8(0x80000d));

	write_reg8 (0x80000c, adr);
	while (read_reg8(0x80000d));

	write_reg8 (0x80000c, id);
	while (read_reg8(0x80000d));

	write_reg8 (0x80000d, 0x01);
#if 0
	write_reg8 (0x80000d, 0x00);
	write_reg8 (0x80000c, 0x05);	//read status
	while (read_reg8(0x80000d));
	int i;
	for (i=0; i<1000000; i++)
	{
		write_reg8 (0x80000c, 0x00);	//launch 8-cycle to trigger read
		while (read_reg8(0x80000d));
		if ( !(read_reg8(0x80000c) & 1) )
			       break;
	}
	write_reg8 (0x80000d, 0x01);
#else
	WaitUs (1000);
#endif
	write_reg8(0x800643,irqst);
}
//#endif
#endif


kb_custom_cfg_t *p_kb_custom_cfg;

/******************************************************************
 * function:
 * normal_addr -> 0x3c00  fn_addr -> 0x3c90
 * numlock_addr -> 0x3d20 fn_numlock_addr -> 0x3db0
 *
 * if you need to fix the cust addr then
 * normal_addr -> 0x3900  fn_addr -> 0x3990
 * numlock_addr -> 0x3920 fn_numlock_addr -> 0x39b0
 *
*****************************************************************/


void kb_map_addr_cust(void)
{
	extern u8 *	kb_p_map[4];
	extern u8 *kb_map_shift;

	extern kb_switch_key_t *kb_switch_key[8];
	u16 adr_offset = 0;
	if(KB_MAP_EXIST(0x3900)){
		adr_offset = 0x300;
	}
	if(KB_MAP_EXIST(KB_CUS_MAP_ADDR - adr_offset)){
		kb_p_map[0] = KB_CUS_MAP_ADDR - adr_offset;  //normal
		u8 fn_customed = KB_MAP_EXIST(KB_CUS_MAP_ADDR - adr_offset + 0x90);
		u8 num_customed = KB_MAP_EXIST(KB_CUS_MAP_ADDR - adr_offset + 0x120);
		kb_p_map[1] = fn_customed ? (KB_CUS_MAP_ADDR - adr_offset + 0x90) : kb_p_map[0];   //fn
		kb_p_map[2] = num_customed ? (KB_CUS_MAP_ADDR - adr_offset + 0x120) : kb_p_map[0];   //num
		kb_p_map[3] = KB_MAP_EXIST(KB_CUS_MAP_ADDR - adr_offset + 0x1b0) ? \
				(KB_CUS_MAP_ADDR - adr_offset + 0x1b0) : \
				(fn_customed ? kb_p_map[1] :  (num_customed ? kb_p_map[2] : kb_p_map[0]) ); //fn+num
	}

	kb_map_shift = KB_SWITCH_SHIFT_ADDR;

	for(int i=0; i<8; i++){
		kb_switch_key[i] =  (volatile  u8*)(KB_SWITCH_KEY_ADDR + i * 4);
	}

	if( *(volatile u32 *)(KB_SWITCH_SHIFT_ADDR) == 0xdeadbeef){
		kb_map_shift = KB_SWITCH_SHIFT_ADDR - 0x300;
	}
}
void kb_custom_init(void)
{
	//3f00  3f20  3f40
	for(int i=0;i<2;i++){
		p_kb_custom_cfg = (kb_custom_cfg_t *) (DEVICE_ID_ADDRESS+(i*0x20));
		if(p_kb_custom_cfg->cap != 0){
			break;
		}
	}

    if(p_kb_custom_cfg->pipe_pairing != U16_MAX){
		rf_set_access_code0 (rf_access_code_16to32(p_kb_custom_cfg->pipe_pairing));
	}

	if (p_kb_custom_cfg->did != U32_MAX) {
		pkt_pairing.did = p_kb_custom_cfg->did;
	}
#if (KEYBOARD_PIPE1_DATA_WITH_DID)
	pkt_km.did = pkt_pairing.did;
#endif

	analog_write(0x81,p_kb_custom_cfg->cap == U8_MAX ? 0xd8 : p_kb_custom_cfg->cap);

	kb_status.cust_tx_power = (p_kb_custom_cfg->tx_power == 0xff) ? RF_POWER_8dBm : p_kb_custom_cfg->tx_power;
	kb_status.tx_power = RF_POWER_2dBm;

	kb_status.led_gpio_lvd = (p_kb_custom_cfg->gpio_lvd == U16_MAX) ? GPIO_GP25 : p_kb_custom_cfg->gpio_lvd;

	kb_status.led_gpio_scr = (p_kb_custom_cfg->gpio_scr == U16_MAX) ? 0 : p_kb_custom_cfg->gpio_scr;
	kb_status.led_gpio_cap = (p_kb_custom_cfg->gpio_cap == U16_MAX) ? 0 : p_kb_custom_cfg->gpio_cap;
	kb_status.led_gpio_num = (p_kb_custom_cfg->gpio_num == U16_MAX) ? 0 : p_kb_custom_cfg->gpio_num;

	kb_status.led_level_lvd = (p_kb_custom_cfg->level_lvd == U8_MAX) ? 1 : 0; //0xff:high_valid,level = 1

	kb_status.led_level_scr = (p_kb_custom_cfg->level_scr == U8_MAX) ? 0 : 1; //0xff:high valid,level = 0
	kb_status.led_level_cap = (p_kb_custom_cfg->level_cap == U8_MAX) ? 0 : 1; //0xff:high valid,level = 0
	kb_status.led_level_num = (p_kb_custom_cfg->level_num == U8_MAX) ? 0 : 1; //0xff:high valid,level = 0

#if (CAVY_HAMSTER_REMINGTON_COMPATIBLE_EN)
	work_with_cavy_chn = (p_kb_custom_cfg->chn_type != U8_MAX) ? 1 : 0;
#endif

	kb_map_addr_cust();
	if(p_kb_custom_cfg->memory_type != U8_MAX){
		device_program = flash_device_program;
		device_program_on = flash_device_program_on;
		device_program_off = flash_device_program_off;
	}
	else{
		device_program = otp_device_program;
		device_program_on = otp_device_program_on;
		device_program_off = otp_device_program_off;
	}
	u16 *p = (u16 *) DEVICE_PARING_INFO_ADDR;

	for (paired_info.index = 0; paired_info.index < DEVICE_ID_PARING_MAX; paired_info.index++) {
		if (*p == 0xffff)
			break;
		paired_info.bind_id[p[0] & 3] = p[1] + (p[2] << 16);
		p += 3;
	}

}

