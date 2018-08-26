

#include "../config/user_config.h"
#include "../mcu/config.h"

//#if (1)
#if(__TL_LIB_8266__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8266))

#include "../common/types.h"
#include "../common/compatibility.h"
#include "../common/bit.h"
#include "../common/utility.h"
#include "../common/static_assert.h"
#include "../mcu/compiler.h"
#include "../mcu/register.h"
#include "../mcu/anareg.h"
#include "../mcu/analog.h"

#include "../mcu/gpio.h"


/************
 *
 * gpio:         indicate the pin
 * up_down:      1 need pull up, 0 need pull down
 *
 *     BIT(7.6)   BIT(5.4)   BIT(3.2)   BIT(1.0)
mask_not 0x3f       0xcf	  0xf3       0xfc

 0a		 PA1         PA0
 0b		 PA5         PA4      PA3        PA2		0
 0c		 PB1         PB0      PA7        PA6
 0d		 PB5         PB4      PB3        PB2		1
 0e		 PC1         PC0      PB7        PB6
 0f		 PC5         PC4      PC3        PC2		2
 10		 PD1         PD0      PC7        PC6
 11		 PD5         PD4      PD3        PD2		3
 12		 PE1         PE0      PD7        PD6
 13		 PE5         PE4      PE3        PE2		4
 14		 PF1         PF0      PE7        PE6
 */
void gpio_setup_up_down_resistor(u32 gpio, u32 up_down)
{
	u8 r_val;

	if( up_down == PM_PIN_UP_DOWN_FLOAT ){
		r_val = 0;
	}
	else if(up_down == PM_PIN_PULLUP_1M){
		r_val = PM_PIN_PULLUP_1M;
	}
	else if(up_down == PM_PIN_PULLUP_10K){
		r_val = PM_PIN_PULLUP_10K;
	}
	else{
		r_val = PM_PIN_PULLDOWN_100K;
	}


	u8 pin = gpio & 0xff;

	u8 base_ana_reg = 0x0b + ((gpio>>8)<<1);
	if(pin & 0x03){
		base_ana_reg -= 1;
	}
	else if(pin & 0xc0){
		base_ana_reg += 1;
	}
	else{
		return;
	}

	u8 mask_not = 0xfc;   //default for  PX2  PX6
	u8 shift_num = 0;

	if(pin & 0x88){  //PX3  PX7
		mask_not = 0xf3;
		shift_num = 2;
	}
	else if(pin & 0x11){  //PX0  PX4
		mask_not = 0xcf;
		shift_num = 4;
	}
	else if(pin & 0x22){ //PX1  PX5
		mask_not = 0x3f;
		shift_num = 6;
	}

	analog_write(base_ana_reg, ( analog_read(base_ana_reg) & mask_not ) | (r_val<<shift_num) );
}

#endif

