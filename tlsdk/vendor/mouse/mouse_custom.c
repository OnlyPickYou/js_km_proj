#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../common/rf_frame.h"
#include "../common/emi.h"

#include "device_info.h"
#include "mouse.h"
#include "mouse_rf.h"
#include "mouse_button.h"
#include "mouse_wheel.h"
#include "mouse_twheel.h"
#include "mouse_sensor.h"
#include "mouse_batt.h"
#include "mouse_rf.h"
#include "mouse_custom.h"

custom_cfg_t   *p_custom_cfg;

const u32 m_hw_def_dft[] = {
    M_HW_BTN_LEFT,//GP10
    M_HW_BTN_RIGHT,//GP8
    M_HW_BTN_MIDL,//GP9
    M_HW_BTN_BB,//GP5
    M_HW_BTN_FB,//GP4
    M_HW_BTN_CPI,//SWS
    M_HW_LED_CTL,//DM    

    M_HW_GPIO_LEVEL_LEFT | (M_HW_GPIO_LEVEL_RIGHT<<8) | (M_HW_GPIO_LEVEL_MIDL<<16) | (M_HW_GPIO_LEVEL_BB<<24),
    M_HW_GPIO_LEVEL_FB   | (M_HW_GPIO_LEVEL_CPI<<8)   | (M_HW_GPIO_LEVEL_LED<<16)  | (M_HW_VBAT_CHN<<24),

    M_HW_CFG_1_DIR_0,//DM
    M_HW_CFG_1_DIR_1,//MSDI
    M_HW_CFG_2_DIR_0,//DM
    M_HW_CFG_2_DIR_1,//MSDI
    

    M_HW_WHEEL_Z0,//GP0
    M_HW_WHEEL_Z1,//GP7
    M_HW_SNS_DATA, //GP2   
    M_HW_SNS_CLK,//GP3
    M_HW_SNS_MOT_PIN,//GP1
};


#define SENSOR_IDX_CLOCK_3  0
#define SENSOR_IDX_CLOCK_6  1
#define SENSOR_IDX_CLOCK_9  2
#define SENSOR_IDX_CLOCK_12 3


const u8 custom_tbl_dir[4] = {
	SENSOR_DIRECTION_CLOCK_3,
	SENSOR_DIRECTION_CLOCK_6,
	SENSOR_DIRECTION_CLOCK_9,
	SENSOR_DIRECTION_CLOCK_12
};

const u8 custom_dir_idx_re_def[4] = {
    CUST_DIR_IDX_RE_0,
    CUST_DIR_IDX_RE_1,
    CUST_DIR_IDX_RE_2,
    CUST_DIR_IDX_RE_3 
};

custom_cpi_cfg_t mouse_cpi = {
    1,  //default cpi_idx
    3,  //segment: 3
    0,  //cpi_800
    2,  //cpi_1200
    3,  //cpi_1600
    1,  //optional
};

custom_btn_ui_t mouse_btn_ui = {
    ( FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT ),                       //pairing ui
    ( FLAG_BUTTON_LEFT | FLAG_BUTTON_RIGHT | FLAG_BUTTON_MIDDLE ),  //emi ui
    ( FLAG_BUTTON_MIDDLE | FLAG_BUTTON_RIGHT ),                       //cpi-2-btn ui, should not be the same with pairing ui
    U8_MAX,                                                         //cpi-2-btn time
};


led_cfg_t mouse_led_cfg[] = {
    32,     1,      1,      0x40,    //power-on, 2s on
    2,      2,      255,    0x40,    //pairing manual, 4Hz
    0,      8,      3,      0x80,    //pairing end
    4,      4,      3,      0,       //battery low  2Hz
    8,      8,      3,      0,       //cpi, 1Hz
    0,      8,      3,      0,       //rsvd, 3Hz
};




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

#if (1)
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



#if 1//MOUSE_CUSTOM_FULL_FUNCTION    
void mouse_custom_re_get( u8 *p_dst, u8 *p_src_0, u8 *p_src_1, u32 len ){
    int i;
    for( i = 0; i < len; i++ ){
        *p_dst = (*p_src_1 == U8_MAX) ? *p_src_0 : *p_src_1;
        *p_src_0++;
        *p_src_1++;
        *p_dst++;
    }
}

void mouse_custom_re_get_4( u32 *p_dst, u32 *p_src_0, u32 *p_src_1, u32 len ){
    int i;
    for( i = 0; i < len; i++ ){
        *p_dst = (*p_src_1 == U32_MAX) ? *p_src_0 : *p_src_1;
        *p_src_0++;
        *p_src_1++;
        *p_dst++;
    }
}
#else
static inline void mouse_custom_re_get( u8 *p_dst, u8 *p_src_0, u8 *p_src_1, u32 len ) {}
static inline void mouse_custom_re_get_4( u32 *p_dst, u32 *p_src_0, u32 *p_src_1, u32 len ) {}
#endif

//cfg_init	to£ºval_c   =        0    |   1    |   2    |      3
//--------------------------------------------------------------
//	0		cust_addr_0 =        8    |   a    |   4    |      6
//	1		cust_addr_1 =        7    |   9    |   3    |      5
//	2		cust_addr_2 =        6    |   8    |   2    |      4
//	3		cust_addr_3 =        5    |   7    |   1    |      3
//--------------------------------------------------------------
u8 custom_cfg_re_define( u8 cfg, u8* p_cfg_re_def ){
	u32 val_c = ( *(p_cfg_re_def + cfg) == U8_MAX ) ? cfg : \
		( ( ( cfg + *(p_cfg_re_def+cfg) ) >> 1 ) & 3 );
	return val_c;
}

//Pin-1 Pin-0  index    dir
//  1      1       0    clk_3
//  1      0       1    clk_6
//  0      1       2    clk_9
//  0      0       3    clk_12
u32 mouse_custom_cfg_r ( u32 *dir_r ){
    u32 dir_idx = 0;
	//Pin_1/Pin_0 internal pull up, and get input level

#if(0)
	gpio_setup_up_down_resistor( dir_r[0], 1 );
	gpio_setup_up_down_resistor( dir_r[1], 1 );
#else
    gpio_write(dir_r[0], 1);
    gpio_write(dir_r[1], 1);
#endif

	WaitUs(10);   //can change to mcu stall later, if needs
	//should disable output_en ?
	//gpio_set_output_en(dir_r[0], 0);
	//gpio_set_output_en(dir_r[1], 0);
	gpio_set_input_en(dir_r[0], 1);
	gpio_set_input_en(dir_r[1], 1);
	if( gpio_read(dir_r[0]) )
		dir_idx |= BIT(0);
	if( gpio_read(dir_r[1]) )
		dir_idx |= BIT(1);
	//Pin_1/Pin_0 recovery from direction detection setting
#if(0)
	gpio_setup_up_down_resistor( dir_r[0], PM_PIN_UP_DOWN_FLOAT );
	gpio_setup_up_down_resistor( dir_r[1], PM_PIN_UP_DOWN_FLOAT );
#else
	gpio_write(dir_r[0], 0);
	gpio_write(dir_r[1], 0);
#endif

	return  (3 - dir_idx);
}

u8 mouse_custom_sensor_dir_init (mouse_hw_t *pHW){
    u32 idx_dir;
	idx_dir = mouse_custom_cfg_r(pHW->cfg_1_r);
    if ( p_custom_cfg->sns_dir == U8_MAX ){		//default is "hardware config sensor direction"
        idx_dir = mouse_custom_cfg_r(pHW->cfg_1_r);
        idx_dir = custom_cfg_re_define( idx_dir, custom_dir_idx_re_def );
    }
    else{
        idx_dir = p_custom_cfg->sns_dir;		//can custom "soft config default dpi"
    }
    idx_dir = custom_cfg_re_define( idx_dir, p_custom_cfg->sns_dir_idx_re);
	return custom_tbl_dir[idx_dir&3];
}

extern rf_packet_pairing_t	pkt_pairing;

void mouse_custom_init ( mouse_status_t *pStatus ){

	for(int i = 0; i<2; i++){
		p_custom_cfg = (custom_cfg_t *)(DEVICE_ID_ADDRESS - i*0x100);
		if(p_custom_cfg->cap != 0){
			break;
		}
	}

    if ( p_custom_cfg->cap != U8_MAX )
        cap_internal_adjust( p_custom_cfg->cap );

	mouse_custom_re_get_4( pStatus->hw_define, m_hw_def_dft, &p_custom_cfg->cust_ms_hw, sizeof(mouse_hw_t)>>2 );
	pStatus->wheel_dir = p_custom_cfg->wheel_dir == U8_MAX ? 1 : -1;
    
    pStatus->sensor_dir = mouse_custom_sensor_dir_init ( pStatus->hw_define );

#if (CAVY_HAMSTER_REMINGTON_COMPATIBLE_EN)
    work_with_cavy_mouse = (p_custom_cfg->chn_type != U8_MAX);
#endif

    mouse_custom_re_get( &mouse_cpi, &mouse_cpi, &p_custom_cfg->sns_cpi, sizeof(custom_cpi_cfg_t) );

    pStatus->cpi = mouse_cpi.sns_cpi_dft;	//default is "soft config default dpi"


    mouse_custom_re_get( &mouse_btn_ui, &mouse_btn_ui, &p_custom_cfg->btn_ui, sizeof(custom_btn_ui_t) );
    mouse_custom_re_get( mouse_led_cfg, mouse_led_cfg, p_custom_cfg->led_cfg, sizeof(mouse_led_cfg) );	
    pkt_pairing.did = (p_custom_cfg->did == U32_MAX) ? pkt_pairing.did : p_custom_cfg->did;   //device-id init


#if (MOUSE_PIPE1_DATA_WITH_DID)
    pkt_km.did = pkt_pairing.did;
#endif

	u16 vendor_id = p_custom_cfg->vid;
    if(vendor_id != U16_MAX){
		rf_set_access_code0 (rf_access_code_16to32(vendor_id));
	}
    pStatus->high_end = (p_custom_cfg->high_end == U8_MAX) ? U8_MAX : p_custom_cfg->high_end;

    if(p_custom_cfg->memory_type != U8_MAX){
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

