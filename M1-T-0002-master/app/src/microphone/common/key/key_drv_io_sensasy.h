#ifndef __KEY_DRV_IO_ZL_H__
#define __KEY_DRV_IO_ZL_H__

#include "gpio.h"
#include "key.h"
#include "user_header.h"

#define IS_KEY1_DOWN()    	(!(JL_PORTA->IN & KEY1_BIT))

#define KEY1_INIT()        do{\
							/**key0 init*/\
							JL_PORTA->PU |= KEY1_BIT,\
							JL_PORTA->PD &= ~KEY1_BIT,\
							JL_PORTA->DIE |= KEY1_BIT,\
							JL_PORTA->DIR |= KEY1_BIT;\
					    	}while(0)



extern const key_interface_t key_io_info;

#endif/*__KEY_DRV_IO_ZL_H__*/
