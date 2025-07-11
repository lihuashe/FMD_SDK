#include "microphone.h"
#include "KT0646M.h"
//#include "led.h"
#define LOG_TAG_CONST       MIC
#define LOG_TAG             "[microphone]"

#define	U_EN		PB0

microphone_config_t g_config;

const uint16_t chan_reg_value[6][2] = 
{   
    /*  chan_rega, chan_regb */
    {0x7D03, 0x8DC1},   // 657MHz
    {0x8503, 0x2E19},   // 692MHz
    {0x8503, 0x51D1},   // 695MHz
    {0x8503, 0x699F},   // 697MHz
    {0x8503, 0x816F},   // 699MHz
};

#define MICROPHONE_MAX_VOLUME   10
const uint16_t volume_table[MICROPHONE_MAX_VOLUME] = {
    MIC_SENS_GAIN_0, MIC_SENS_GAIN_1, MIC_SENS_GAIN_2, MIC_SENS_GAIN_3, MIC_SENS_GAIN_4,
    MIC_SENS_GAIN_5, MIC_SENS_GAIN_6, MIC_SENS_GAIN_7, MIC_SENS_GAIN_8, MIC_SENS_GAIN_9,
};

// undefined functions
void inline device_ERR_display()
{

}

void inline device_ch_display_ir()
{

}

#define log_print
#define log_debug	
#define log_error	

uint8_t eeprom_read(uint8_t address)
{
	uint8_t r;
	EEADR = address;    
	RD = 1;
    NOP();
    NOP();   
	r = EEDAT;     //EEPROM�Ķ����� r = EEDATA;
	return r;    
}

void eeprom_write(uint8_t address, uint8_t b)
{
	while(GIE) 					//�ȴ�GIEΪ0
    {
		GIE = 0;
        NOP();
        NOP();
	}
	EEADR = address; 	 	  //EEPROM�ĵ�ַ
	EEDAT = b;		 	      //EEPROM��д����  EEDATA = Data
	EEIF = 0;                 //��0�жϱ�־λ
	EECON1 |= 0x34;			  //��λWREN1��WREN2��WREN3��������
	WR = 1;					  //��λWR�������
	while(WR);      		  //�ȴ�EEд�����
    GIE = 1;    
}

void microphone_load_config()
{
    uint8_t * p = (uint8_t *)&g_config;
    for (uint8_t i = 0; i < sizeof(microphone_config_t); i++) {
        *p++ = eeprom_read(i);
    }
}

void microphone_save_config()
{
    uint8_t * p = (uint8_t *)&g_config;

    for (uint8_t i = 0; i < sizeof(microphone_config_t); i++) {
        eeprom_write(i, *p++);
    }
}

bool microphone_config(void)
{
    if (KT_WirelessMicTx_Tune_Direct(chan_reg_value[g_config.channel][0], chan_reg_value[g_config.channel][1]) == false) {
        return false;
    }
    else {
       log_info("microphone_config mic_config.freq =[657027KHZ]\r\n");
    }

    KT_WirelessMicTx_EQSW(false);
    KT_WirelessMicTx_ECHO(ECHO_DISABLE, ECHO_RATIO_0, ECHO_DELAY_22ms);
    KT_Delay_ms(200);

	KT_WirelessMicTx_PAGain(35); 	// ���18dBm������15dBm
    KT_WirelessMicTx_PASW(PA_ON);

	KT_Bus_Write(0x12, 0xA700);
	KT_Bus_Write(0x13, 0xF0E4);
	KT_Bus_Write(0x14, ((uint16_t)0x88 << 8) | (g_config.channel + 1));
	KT_Bus_Write(0x27, 0x0366);
    KT_WirelessMicTx_Pilot_Fdev(PILOT_FDEV_5K);    
    KT_Bus_Write(0x20, (AUX_ADDRB << 8) | AUX_ADDRA);
    KT_Bus_Write(0x21, (AUX_ADDRD << 8) | AUX_ADDRC);

    uint16_t ret = KT_Bus_Read(0x1F);
    KT_Bus_Write(0x1f,(ret & 0x80f0) | (AUXDATA_EN << 14)   | (AUX_REG_NUM << 12) |
                                       (AUX_CARRY_NUM << 9) | BPSK_NEW_MODE);
                     
    KT_WirelessMicTx_Pilot(true);
    KT_WirelessMicTx_MuteSel(false);
    return true;
}

/**
 * @brief ������˷繦�ܳ�ʼ��
 *
 * @return int
 */
int microphone_init()
{
	log_info("KT0646M init\r\n");

	// default value
    microphone_load_config();

    if (g_config.flash_key1 != 0x55 || g_config.flash_key2 != 0xAA)
    {
        g_config.channel = 0;
        g_config.id[1] = 0x0403;
        g_config.id[0] = 0x0201;
	}

    int i;
    for (i = 0; i < 3; i++) {
        // KT0616M.CHIPEN
        // hard reset
        U_EN = 0;
        KT_Delay_ms(200);
        U_EN = 1;
        KT_Delay_ms(300);

        // ���߲���
        uint16_t ID_KT0646M = KT_WirelessMicTx_PreInit();
        if (ID_KT0646M != 0) {
            log_info("Successed Read KT0646M ID = 0x%04x\r\n", ID_KT0646M);
        } else {
            log_info("Failed Read KT0646M ID\r\n");
            return 0;
        }
        // ??Ƭ��??
        if (KT_WirelessMicTx_Init() == 0) {
            log_info("KT0646M init Failed\r\n");
        }
        else {
             log_info("KT0646M init Successed\r\n");
        }

        // ??��FM����
        if (microphone_config() == false) {
            // to-do: ��˷�����ʧ��
            // log_info("FM started, freq=%dKHz, PA gain=%d\r\n, retry = %d", mic_config.freq,mic_config.PA_gain, i);
        } else {
            break;
        }
    }

    if (i == 3) {
        return 0;   // ??Ƭ��ʼ��ʧ��
    } else {
        return 1;
    }
}

void microphone_vol_up()
{
    if (g_config.volume < (MICROPHONE_MAX_VOLUME - 1)) 
    {
        g_config.volume++;
        KT_WirelessMicTx_Mic_Sens(volume_table[g_config.volume]);
    }
}

void microphone_vol_down()
{
    if (g_config.volume != 0)
    {
        g_config.volume--;
        KT_WirelessMicTx_Mic_Sens(volume_table[g_config.volume]);
    }
}

void microphone_bpsk_poweroff()
{
    KT_Bus_Write(0x14, ((uint16_t)0xe2 << 8) | (g_config.channel + 1));  //0xe2�̶����� Ϊ�Ͽ�����  ��һ??�ֽ� Ϊͨ�� 16
}

void microphone_bpsk_mute()
{
    KT_Bus_Write(0x14, ((uint16_t)0xe3 << 8) | (g_config.channel + 1));  //0xe2�̶����� Ϊ�Ͽ�����  ��һ??�ֽ� Ϊͨ�� 16
}