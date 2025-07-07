#ifndef  __MICROPHONE_H__
#define  __MICROPHONE_H__

#include "KT0646M.h"

#define MIC_CONFIG_STORE_ADDRESS	0x79000
#define MIC_CONFIG_FLAS_KEY			0x55AA0103

#define GPIO_MICROPHONE_CHIPEN		IO_PORTA_10
#define PD4_FUNC					AS_GPIO

#define  FREQMAXNUMBER     6        //项目最大频段数量
#define  FREQSTARTCH       31       //项目起始频段在全部发射频段的位置


#ifndef M_TYPE
#define M_TYPE 0
#endif

#if M_TYPE == 0
	#define  CHANNELNUMBER     0        //0 通道A   1通道B
#elif M_TYPE == 1
	#define  CHANNELNUMBER     1        //0 通道A   1通道B
#else
	#define  CHANNELNUMBER     0        //0 通道A   1通道B
#endif

#define U16_HI(a)    (((a) >> 8) & 0xFF)
#define U16_LO(a)    ((a) & 0xFF)

typedef struct
{
	u32	flash_key;
	s32 freq;

	u8 PA_gain;
	// EQ设置
	u8 EQ_EN;	// EQ使能
	u8 EQ[15];	// EQ调节值
	// PILOT
	u8 PILOT_EN;
	u16 PILOT_DATA[4];
	u8 channel;
	u8 PA_state;
	u8 ch_power_count;
}mic_config_t;

typedef struct
{
	u8 cmd;
	u8 len;
	u8 cmd_data[64];
}microphone_setting_t;

typedef struct
{
	u16 channel;
	u32 freq;
}microphone_status_t;

extern  mic_config_t mic_config;

extern u32 tick_microphone_new_cmd;
extern microphone_setting_t microphone_setting;



int microphone_init();

bool KT_MicTX_Init(void);

int microphone_set_freq();

void process_microphone();

int microphone_set_PA0();

void microphone_enable(bool enable);
int microphone_pairing(s32 Freq,u8 ch);
void microphone_AUX_Disconnect();
void microphone_AUX_mute();
void microphone_AUX_unmute();
void KT_mic_config_t_print(mic_config_t mic_config);
void KT0646_setchangeBattery();
void KT0646_setchangekey(u8 key);

#endif
