#ifndef __KEY_DRV_IO_H__
#define __KEY_DRV_IO_H__

#include "gpio.h"
#include "key.h"
#include "user_header.h"

#define IS_KEY1_DOWN()    	(!(JL_PORTA->IN & KEY1_BIT))
#define IS_KEY2_DOWN()    	(!(JL_PORTB->IN & KEY2_BIT))

#define KEY1_INIT()        do{\
							/**key0 init*/\
							JL_PORTA->PU |= KEY1_BIT,\
							JL_PORTA->PD &= ~KEY1_BIT,\
							JL_PORTA->DIE |= KEY1_BIT,\
							JL_PORTA->DIR |= KEY1_BIT;\
					    	}while(0)

#define KEY2_INIT()        do{\
							/**key1 init*/\
							JL_PORTB->PU |= KEY2_BIT,\
							JL_PORTB->PD &= ~KEY2_BIT,\
							JL_PORTB->DIE |= KEY2_BIT,\
							JL_PORTB->DIR |= KEY2_BIT;\
					    	}while(0)

extern const key_interface_t key_io_info;

#endif/*__KEY_DRV_IO_H__*/
