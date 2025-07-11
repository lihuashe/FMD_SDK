#ifndef  __MICROPHONE_H__
#define  __MICROPHONE_H__

#include "KT0646M.h"
#include <stdint.h>

#define MIC_CONFIG_STORE_ADDRESS	0x79000
#define MIC_CONFIG_FLAS_KEY			0x55AA0103

#define GPIO_MICROPHONE_CHIPEN		IO_PORTA_10
#define PD4_FUNC					AS_GPIO

#define  FREQMAXNUMBER     150      
#define  FREQSTARTCH       1      
#ifndef M_TYPE
#define M_TYPE 0
#endif

#if M_TYPE == 0
	#define  CHANNELNUMBER     0  
#elif M_TYPE == 1
	#define  CHANNELNUMBER     1        
#else
	#define  CHANNELNUMBER     0        
#endif

#define U16_HI(a)    (((a) >> 8) & 0xFF)
#define U16_LO(a)    ((a) & 0xFF)

typedef struct
{
	uint8_t 	flash_key1;
	uint8_t 	flash_key2;
	// PILOT
	uint16_t 	id[2];
	uint8_t  	channel;
	uint8_t		volume;
}microphone_config_t;

extern  microphone_config_t g_config;

int microphone_init();

void microphone_save_config();

void microphone_load_config();

void microphone_vol_up();

void microphone_vol_down();

void microphone_bpsk_poweroff();

void microphone_bpsk_mute();
#endif
