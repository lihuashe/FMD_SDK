#include "KT0646M/microphone.h"
#include "KT0646M.h"
//#include "led.h"
#include "config.h"
#include "typedef.h"
#include "msg.h"
#include <math.h>
#include "bsp_loop.h"
#include "app_config.h"
#include "iic_soft/iic_soft.h"
#include "gpio.h"
#include "wdt.h"
#include "tune/app_tune.h"
#define LOG_TAG_CONST       MIC
#define LOG_TAG             "[microphone]"
#include "debug.h"
#include "user_flash.h"
#include "tune/seg.h"

//   u32 set_freq=679970;
mic_config_t mic_config = {0};
mic_config_t current_mic_config = {0};
u16 microphone_pairing_flag =0;
u8 channel_number = 0;

microphone_setting_t microphone_setting = {0};
extern bool err_remote_pa_off;
extern  u32 Battery_Vdet;

extern void delay_10ms(u32 tick);
extern void udelay();
extern uint8_t TestMode;

u8 number_randomdata[96]={  4,5,3,1,2,5,4,1,0,3,2,0,    \
                            3,5,2,1,0,2,4,3,1,3,5,4,   \
                            5,2,3,0,4,5,2,4,3,1,3,5,   \
                            4,3,1,4,2,0,5,3,1,3,5,2,   \
                            1,3,2,4,5,2,1,0,5,0,2,4,   \
                            2,5,1,4,2,0,5,3,1,0,5,3,   \
                            0,3,1,4,2,0,5,3,1,3,5,4,   \
                            5,4,3,0,2,1,5,3,1,3,2,0 };


/*全部频率
// 用户自定义搜台频率表
const uint32_t UserRandomFreq[2][48] =
{
    {
        //Group 0  	麦克风 A
        // CHANNEL_Group_Select = 0
        748000, //1
        749000, //2
        863150, //3
        863400, //4
        863650, //5
        863900, //6
                // CHANNEL_Group_Select = 1
        803300, //7
        803550, //8
        803800, //9
        804050, //10
        804300, //11
        804400, //12
                // CHANNEL_Group_Select = 2
        925600, //13
        925900, //14
        926200, //15
        926500, //16
        926800, //17
        927100, //18
                // CHANNEL_Group_Select = 3
        806125, //19
        806375, //20
        806625, //21
        806875, //22
        807125, //23
        807375, //24
                // CHANNEL_Group_Select = 4
        915300, //25
        916300, //26
        917300, //27  //
        918300, //28
        919300, //29
        920300, //30
                // CHANNEL_Group_Select = 5
        657027, //31
        657427, //32
        657827, //33
        658227, //34
        658627, //35
        659027, //36
                // CHANNEL_Group_Select = 6
        865150, //37
        865450, //38
        865750, //39
        867050, // 40
        867350, //41
        867650, //42
                // CHANNEL_Group_Select = 7
        748300, //43
        748600, //44
        748900, //45
        749200, //46
        749500, //47
        749800, //48
    },

    {
        //Group 1		麦克风 B
        // CHANNEL_Group_Select = 0
        750000, //1
        751000, //2
        864150, //3
        864400, //4
        864650, //5
        864900, //6
                // CHANNEL_Group_Select = 1
        804550, //7
        804800, //8
        805050, //9
        805300, //10
        805550, //11
        805900, //12
                // CHANNEL_Group_Select = 2
        927600, //13
        927900, //14
        928200, //15
        928500, //16
        928800, //17
        929100, //18
                // CHANNEL_Group_Select = 3
        808375, //19
        808625, //20
        808875, //21
        809125, //22
        809375, //23
        809625, //24
                // CHANNEL_Group_Select = 4
        921300, //25
        922300, //26
        923300, //27
        924300, //28
        925300, //29
        926300, //30
                // CHANNEL_Group_Select = 5
        659427, //31
        659827, //32
        660227, //33
        660627, //34
        661027, //35
        661427, //36
                // CHANNEL_Group_Select = 6
        866150, //37
        866450, //38
        866750, //39
        868050, // 40
        868350, //41
        868650, //42
                // CHANNEL_Group_Select = 7
        750300, //43
        750600, //44
        750900, //45
        751200, //46
        751500, //47
        751800, //48
    }

};
*/

// 项目6A08的发射频率表
const uint32_t UserRandomFreq[2][48] =
{
    {
        //Group 0  	麦克风 A
       // CHANNEL_Group_Select = 5
        657027, //31
        657427, //32
        657827, //33
        658227, //34
        658627, //35
        659027, //36
    },

    {
        //Group 1		麦克风 B

        // CHANNEL_Group_Select = 5
        659427, //31
        659827, //32
        660227, //33
        660627, //34
        661027, //35
        661427, //36
    }

};

const u8 EQ_param_default[15] = {

    EQ_GAIN_Neg8dB, // EQ_25H
    EQ_GAIN_Neg7dB, // EQ_40H
    EQ_GAIN_Neg6dB, // EQ_63H
    EQ_GAIN_Neg3dB, // EQ_100H
    EQ_GAIN_0dB, // EQ_160H
    EQ_GAIN_0dB, // EQ_250H
    EQ_GAIN_0dB, // EQ_400H
    EQ_GAIN_0dB, // EQ_630H
    EQ_GAIN_0dB, // EQ_1KH
    EQ_GAIN_0dB, // EQ_1K6
    EQ_GAIN_0dB, // EQ_2K5
    EQ_GAIN_0dB, // EQ_4KH
    EQ_GAIN_0dB, // EQ_6K3
    EQ_GAIN_0dB, // EQ_10K
    EQ_GAIN_0dB, // EQ_16K
};

/**
 * @brief 无线麦克风功能初始化
 *
 * @return int
 */
int microphone_init()
{
	log_info("KT0646M init\r\n");
	// default value
	if (mic_config.flash_key != MIC_CONFIG_FLAS_KEY)
	{
		mic_config.freq =  607250;// // 607.25
		//mic_config.PA_gain = 35;
		mic_config.PA_gain = 20;
		//log_info("mic_config.freq=%d\r\n",mic_config.freq);

        mic_config.PILOT_EN = 1;
		mic_config.PILOT_DATA[0]=  0xA700;
		mic_config.PILOT_DATA[1] = 0xf0e4;
		mic_config.PILOT_DATA[2] = 0x4161;
		mic_config.PILOT_DATA[3] = 0x4d66;

	}

    mic_config.EQ_EN = 0;
    for (int i = 0; i < 15; i++) {
        mic_config.EQ[i] = EQ_GAIN_0dB;
    }
//    memcpy(mic_config.EQ,EQ_param_default,sizeof(EQ_param_default));

    int i;
    for (i = 0; i < 3; i++) {
        // KT0616M.CHIPEN
        gpio_set_die(GPIO_MICROPHONE_CHIPEN,digital_mode);
        gpio_set_direction(GPIO_MICROPHONE_CHIPEN,output_mode);
        // hard reset
        gpio_write(GPIO_MICROPHONE_CHIPEN, 0);
        udelay(200*1000);
        gpio_write(GPIO_MICROPHONE_CHIPEN, 1);
        udelay(300*1000);

        // 总线测试
        u16 ID_KT0646M = KT_WirelessMicTx_PreInit();
        if (ID_KT0646M != 0) {
            log_info("Successed Read KT0646M ID = 0x%04x\r\n", ID_KT0646M);
        } else {
            log_info("Failed Read KT0646M ID\r\n");
            return 0;
        }
        // 芯片配置
        if (KT_WirelessMicTx_Init() == 0) {
            log_info("KT0646M init Failed\r\n");
        }
        else {
             log_info("KT0646M init Successed\r\n");
        }

        // 启动FM功能
        if (KT_MicTX_Init() == false) {
            log_info("FM started, freq=%dKHz, PA gain=%d\r\n, retry = %d", mic_config.freq,mic_config.PA_gain, i);
        } else {
            break;
        }
    }

    if (i == 3) {
        return 0;   // 芯片初始化失败
    } else {
        return 1;
    }
}



//-----------------------------------------------------------------------------
//函 数 名：KT_MicTX_Init
//功能描述：芯片初始化
//函数说明：
//全局变量：Memery_Frequency; Load_Frequency;
//输    入：
//返    回：无
//设 计 者：KANG Hekai                时间：2012-08-01
//修 改 者：Zhou Dongfeng            时间：2015-12-28
//版    本：V1.0
//-----------------------------------------------------------------------------
bool KT_MicTX_Init(void)
{
   // log_info("into KT_MicTX_Init mic_config.freq =[657027KHZ]\r\n");
   // if (KT_WirelessMicTx_Tune(mic_config.freq) == false) {
    if (KT_WirelessMicTx_Tune(607250) == false) {
        return false;
    }
    else{
       log_info("33333333 KT_MicTX_Init mic_config.freq =[607250KHZ]\r\n");
    }

    KT_WirelessMicTx_EQGAIN(EQ_25H, mic_config.EQ[0]);
    KT_WirelessMicTx_EQGAIN(EQ_40H, mic_config.EQ[1]);
    KT_WirelessMicTx_EQGAIN(EQ_63H, mic_config.EQ[2]);

    KT_WirelessMicTx_EQGAIN(EQ_100H,mic_config.EQ[3]);
    KT_WirelessMicTx_EQGAIN(EQ_160H,mic_config.EQ[4]);
    KT_WirelessMicTx_EQGAIN(EQ_250H,mic_config.EQ[5]);

    KT_WirelessMicTx_EQGAIN(EQ_400H,mic_config.EQ[6]);
    KT_WirelessMicTx_EQGAIN(EQ_630H,mic_config.EQ[7]);
    KT_WirelessMicTx_EQGAIN(EQ_1KH, mic_config.EQ[8]);

    KT_WirelessMicTx_EQGAIN(EQ_1K6, mic_config.EQ[9]);
    KT_WirelessMicTx_EQGAIN(EQ_2K5, mic_config.EQ[10]);
    KT_WirelessMicTx_EQGAIN(EQ_4KH, mic_config.EQ[11]);

    KT_WirelessMicTx_EQGAIN(EQ_6K3, mic_config.EQ[12]);
    KT_WirelessMicTx_EQGAIN(EQ_10K, mic_config.EQ[13]);
    KT_WirelessMicTx_EQGAIN(EQ_16K, mic_config.EQ[14]);

    //log_print("KT_WirelessMicTx_EQSW mic_config.EQ_EN=[%d]\r\n",mic_config.EQ_EN);
    KT_WirelessMicTx_EQSW(mic_config.EQ_EN);
    KT_WirelessMicTx_ECHO(ECHO_DISABLE,ECHO_RATIO_0,ECHO_DELAY_22ms);
    udelay(200*1000);

	// KT_WirelessMicTx_PAGain(10);	// 差分10dBm，单端7dBm
	KT_WirelessMicTx_PAGain(mic_config.PA_gain); 	// 差分18dBm，单端15dBm
    //KT_WirelessMicTx_PASW(PA_ON);
//	log_print("KT_WirelessMicTx_EQSW mic_config.EQ_EN=[%d]\r\n",mic_config.EQ_EN);
	KT_Bus_Write(0x12, mic_config.PILOT_DATA[0]);
	KT_Bus_Write(0x13, mic_config.PILOT_DATA[1]);
	KT_Bus_Write(0x14, mic_config.PILOT_DATA[2]);
	KT_Bus_Write(0x27, mic_config.PILOT_DATA[3]);
    // KT_WirelessMicTx_Pilot_Fdev(PILOT_FDEV_5K);
  //  log_print("KT_WirelessMicTx_Pilot mic_config.PILOT_EN=[%d]\r\n",mic_config.PILOT_EN);
    KT_WirelessMicTx_Pilot(mic_config.PILOT_EN);
    //log_print("KT_WirelessMicTx_PAGain end  999999\r\n");
    return TRUE;
}


int microphone_set_freq()
{
    int result = 0;

    u16 pilot_config = KT_Bus_Read(0x1F);

    KT_WirelessMicTx_MuteSel(TRUE);
    KT_WirelessMicTx_Pilot(FALSE);
    KT_WirelessMicTx_PAGain(0);
	KT_WirelessMicTx_PASW(PA_OFF);
    udelay(10*1000);

    // for (int i = 0; i < 3; i++) {
    //     result = KT_WirelessMicTx_Tune(mic_config.freq);
    //     if (result) break;
    // }
    // printf("set new freq = %dKHz, result = %d\n",mic_config.freq, result);

	// KT_WirelessMicTx_PAGain(mic_config.PA_gain);
	// KT_WirelessMicTx_PASW(PA_ON);

	// KT_Bus_Write(0x12, mic_config.PILOT_DATA[0]);
	// KT_Bus_Write(0x13, mic_config.PILOT_DATA[1]);
	// KT_Bus_Write(0x14, mic_config.PILOT_DATA[2]);
	// KT_Bus_Write(0x27, mic_config.PILOT_DATA[3]);
	// KT_WirelessMicTx_Pilot(TRUE);
    // KT_Bus_Write(0x1F, pilot_config);


    // pilot_config = KT_Bus_Read(0x1e);          //增加这么一段对DSP进行复位
    // KT_Bus_Write(0x1e,pilot_config|0x0010);    //保证每次的辅助通道的数据能正常发送
    // Delay_ms(5);
    // pilot_config = KT_Bus_Read(0x1e);
    // KT_Bus_Write(0x1e, pilot_config&~0x0010);

    // KT_WirelessMicTx_MuteSel(FALSE);

    int retry = 3;
    do {
        // 芯片配置
        KT_WirelessMicTx_Init();
        // 启动FM功能
        if (KT_MicTX_Init() == true) {
            KT_WirelessMicTx_PAGain(mic_config.PA_gain);
            KT_WirelessMicTx_PASW(PA_ON);

            KT_Bus_Write(0x12, mic_config.PILOT_DATA[0]);
            KT_Bus_Write(0x13, mic_config.PILOT_DATA[1]);
            KT_Bus_Write(0x14, mic_config.PILOT_DATA[2]);
            KT_Bus_Write(0x27, mic_config.PILOT_DATA[3]);
            KT_WirelessMicTx_Pilot(TRUE);
            KT_Bus_Write(0x1F, pilot_config);


            pilot_config = KT_Bus_Read(0x1e);          //增加这么一段对DSP进行复位
            KT_Bus_Write(0x1e,pilot_config|0x0010);    //保证每次的辅助通道的数据能正常发送
            udelay(5*1000);
            pilot_config = KT_Bus_Read(0x1e);
            KT_Bus_Write(0x1e, pilot_config&~0x0010);

            KT_WirelessMicTx_MuteSel(FALSE);
            log_info("set new freq = %dKHz, result = %d\n",mic_config.freq, result);

            return 1;
        }
        // 复位芯片
        microphone_enable(false);
        udelay(100*1000);
        microphone_enable(true);
        udelay(300*1000);
    }while(retry--);

    log_info("microphone_set_freq failed!");
    return 0;
}

int microphone_save_config()
{
   // flash_erase_sector(MIC_CONFIG_STORE_ADDRESS);
    mic_config.flash_key = MIC_CONFIG_FLAS_KEY;
  //  flash_write_page(MIC_CONFIG_STORE_ADDRESS, sizeof(mic_config),(u8*)&mic_config);

    return 1;
}

//Bpsk 设备主动断开连接 发送
void microphone_AUX_Disconnect()
{
u16 device_id_reg13 =0;
    u16 ret =0;
    ret = KT_Bus_Read(0x1f);
    log_info("read  aux_addr2 reg1f=[0x%04x]  \r\n",ret);
    ret = KT_Bus_Read(0x1f);
    log_info("read  aux_addr2 reg1f=[0x%04x]  \r\n",ret);
    log_info("read  aux_addr2 reg2H =[0x%02x  0x02x ]  \r\n",U16_HI(ret),U16_LO(ret));
    device_id_reg13 = (0xe2 << 8) | (U16_LO(ret));

    //KT_Bus_Write(0x12, device_id_reg12);
     //KT_Bus_Write(0x14, device_id_reg14);
    KT_Bus_Write(0x13, device_id_reg13);  //88固定死的  后一个字节 为通道 16

    KT_Bus_Write(0x20,(AUX_ADDRB << 8) | AUX_ADDRA);
    KT_Bus_Write(0x21,(AUX_ADDRD << 8) | AUX_ADDRC);

    ret = KT_Bus_Read(0x1f);
    KT_Bus_Write(0x1f,(ret&0x80f0)|(AUXDATA_EN << 14)|(AUX_REG_NUM << 12) |
                     (AUX_CARRY_NUM << 9)|BPSK_NEW_MODE);
}


//Bpsk 设备主动静音时 发送
void microphone_AUX_mute()
{
    u16 device_id_reg13 =0;
    u16 ret =0;
    ret = KT_Bus_Read(0x1f);
    log_debug("read  aux_addr1f reg1f=[0x%04x]  \r\n",ret);
    ret = KT_Bus_Read(0x13);
    log_debug("read  aux_addr13 reg1f=[0x%04x]  \r\n",ret);
   // log_debug("read  aux_addr13 reg2H =[0x%02x  0x02x ]  \r\n",U16_HI(ret),U16_LO(ret));
    device_id_reg13 = (0xe3 << 8) | (U16_LO(ret));

    KT_Bus_Write(0x13, device_id_reg13);  //0xe2固定死的 为断开连接  后一个字节 为通道 16

    ret = KT_Bus_Read(0x27);
    ret = KT_Bus_Read(0x13);
    log_debug("write  aux_addr13 reg14=[0x%04x]  \r\n",ret);

    KT_Bus_Write(0x20,(AUX_ADDRB << 8) | AUX_ADDRA);
    KT_Bus_Write(0x21,(AUX_ADDRD << 8) | AUX_ADDRC);

    ret = KT_Bus_Read(0x1f);
    KT_Bus_Write(0x1f,(ret&0x80f0)|(AUXDATA_EN << 14)|(AUX_REG_NUM << 12) |
                     (AUX_CARRY_NUM << 9)|BPSK_NEW_MODE);
}

//Bpsk 设备主动非静音时 发送
void microphone_AUX_unmute()
{
    u16 device_id_reg13 =0;
    u16 ret =0;
    ret = KT_Bus_Read(0x1f);
    log_debug("read  aux_addr1f reg1f=[0x%04x]  \r\n",ret);
    ret = KT_Bus_Read(0x13);
    log_debug("read  aux_addr13 reg1f=[0x%04x]  \r\n",ret);
  //  log_debug("read  aux_addr13 reg2H =[0x%02x  0x02x ]  \r\n",U16_HI(ret),U16_LO(ret));
    device_id_reg13 = (0xe4 << 8) | (U16_LO(ret));

    KT_Bus_Write(0x13, device_id_reg13);  //0xe2固定死的 为断开连接  后一个字节 为通道 16

    ret = KT_Bus_Read(0x27);
    ret = KT_Bus_Read(0x13);
    log_debug("write  aux_addr14 reg14=[0x%04x]  \r\n",ret);

    KT_Bus_Write(0x20,(AUX_ADDRB << 8) | AUX_ADDRA);
    KT_Bus_Write(0x21,(AUX_ADDRD << 8) | AUX_ADDRC);

    ret = KT_Bus_Read(0x1f);
    KT_Bus_Write(0x1f,(ret&0x80f0)|(AUXDATA_EN << 14)|(AUX_REG_NUM << 12) |
                     (AUX_CARRY_NUM << 9)|BPSK_NEW_MODE);
}


int microphone_pairing(s32 Freq,u8 ch)
{
    u16 pilot_config = KT_Bus_Read(0x1F);

    KT_WirelessMicTx_MuteSel(TRUE);
    KT_WirelessMicTx_Pilot(FALSE);
    KT_WirelessMicTx_PAGain(0);
	KT_WirelessMicTx_PASW(PA_OFF);
    // Delay_ms(10);

    // for (int i = 0; i < 3; i++) {
    //     result = KT_WirelessMicTx_Tune(620000);     // 固定配对频率
    //     if (result) break;
    // }

    int retry = 3;
    do {
        // 芯片配置
       // KT_WirelessMicTx_Init();
        // 启动FM功能
        if (KT_WirelessMicTx_Tune(Freq) == true) {
            KT_WirelessMicTx_PAGain(mic_config.PA_gain);
           // KT_WirelessMicTx_PASW(PA_ON);

            u16 p0 = ((u16)0xA9 << 8) + ch-1;
            KT_Bus_Write(0x12, p0);
            KT_Bus_Write(0x13, 0xf0e4);
            KT_Bus_Write(0x14, 0x4161);
            KT_Bus_Write(0x27, 0x4d66);
            KT_WirelessMicTx_Pilot(TRUE);
            KT_Bus_Write(0x1F, pilot_config);

            pilot_config = KT_Bus_Read(0x1e);          //增加这么一段对DSP进行复位
            KT_Bus_Write(0x1e, pilot_config|0x0010);    //保证每次的辅助通道的数据能正常发送
            udelay(5*1000);
            pilot_config = KT_Bus_Read(0x1e);
            KT_Bus_Write(0x1e, pilot_config&~0x0010);
            //KT_WirelessMicTx_MuteSel(FALSE);

            return 1;
        }
        // 复位芯片
        microphone_enable(false);
        udelay(100*1000);
        microphone_enable(true);
        udelay(300*1000);
    }while(retry--);
    log_info("microphone_pairing end");
    return 0;
}

 static bool is_microphone_hw_init = FALSE;
 bool is_need_save_config = false;
 bool is_need_pa_stop = false;
 bool is_microphone_enabled = false;

void KT_mic_config_t_print(mic_config_t mic_config)
{
    log_debug("mic_config.flash_key = [0x%08x]", mic_config.flash_key);
    log_debug("mic_config.freq = [%d]", mic_config.freq);
    log_info("mic_config.PA_gain = [%d]", mic_config.PA_gain);
    log_debug("mic_config.EQ_EN = [0x%02x]", mic_config.EQ_EN);
    log_debug("mic_config.EQ[0] = [0x%02x]", mic_config.EQ[0]);
    log_debug("mic_config.EQ[1] = [0x%02x]", mic_config.EQ[1]);
    log_debug("mic_config.EQ[2] = [0x%02x]", mic_config.EQ[2]);
    log_debug("mic_config.EQ[3] = [0x%02x]", mic_config.EQ[3]);
    log_debug("mic_config.EQ[4] = [0x%02x]", mic_config.EQ[4]);
    log_debug("mic_config.EQ[5] = [0x%02x]", mic_config.EQ[5]);
    log_debug("mic_config.EQ[6] = [0x%02x]", mic_config.EQ[6]);
    log_debug("mic_config.EQ[7] = [0x%02x]", mic_config.EQ[7]);
    log_debug("mic_config.EQ[8] = [0x%02x]", mic_config.EQ[8]);
    log_debug("mic_config.EQ[9] = [0x%02x]", mic_config.EQ[9]);
    log_debug("mic_config.EQ[10] = [0x%02x]", mic_config.EQ[10]);
    log_debug("mic_config.EQ[11] = [0x%02x]", mic_config.EQ[11]);
    log_debug("mic_config.EQ[12] = [0x%02x]", mic_config.EQ[12]);
    log_debug("mic_config.EQ[13] = [0x%02x]", mic_config.EQ[13]);
    log_debug("mic_config.EQ[14] = [0x%02x]", mic_config.EQ[14]);
    log_debug("mic_config.PILOT_EN = [0x%02x]", mic_config.PILOT_EN);

    log_debug("mic_config.PILOT_DATA[0] = [0x%04x]", mic_config.PILOT_DATA[0]);
    log_debug("mic_config.PILOT_DATA[1] = [0x%04x]", mic_config.PILOT_DATA[1]);
    log_debug("mic_config.PILOT_DATA[2] = [0x%04x]", mic_config.PILOT_DATA[2]);
    log_debug("mic_config.PILOT_DATA[3] = [0x%04x]", mic_config.PILOT_DATA[3]);
    log_debug("mic_config.channel = [0x%02x]", mic_config.channel);
    log_debug("mic_config.PA_state = [0x%02x]", mic_config.PA_state);
}

void  KT0646_setchangeBattery()
{
    u16 ret;
    u16 device_id_reg27;
    u8  falshbatt =0;
    u8  battchang =0;
    device_id_reg27 = KT_Bus_Read(0x27);  //辅助信道功发送数据保留寄存器4

    falshbatt =U16_LO(device_id_reg27);
    battchang=(u8)fabs((Battery_Vdet/100)-falshbatt);
   // log_info("read  aux_reserved_reg4 reg27=[0x%04x] falshbatt=[%d] curbattery=[%d] \r\n",device_id_reg27,falshbatt,Battery_Vdet/100);
    if (battchang >=2 )
    {
        KT_WirelessMicTx_PASW(PA_ON);
        ret = KT_Bus_Read(0x1f);
        KT_Bus_Write(0x27, (U16_HI(device_id_reg27)<< 8) | (Battery_Vdet/100));
        mic_config.PILOT_DATA[3] = (U16_HI(device_id_reg27)<< 8) | (Battery_Vdet/100);
        //KT_mic_config_t_print(mic_config);
        
        KT_Bus_Write(0x20,(AUX_ADDRB << 8) | AUX_ADDRA);
        KT_Bus_Write(0x21,(AUX_ADDRD << 8) | AUX_ADDRC);

        KT_Bus_Write(0x1f,(ret&0x80f0)|(AUXDATA_EN << 14)|(AUX_REG_NUM << 12) |
                            (AUX_CARRY_NUM << 9)|BPSK_NEW_MODE);
        
    }
}

void  KT0646_setchangekey(u8 key)
{
    u16 ret;
    u16 device_id_reg27;
    u8 falshkey =0;
    device_id_reg27 = KT_Bus_Read(0x27);  //辅助信道功发送数据保留寄存器4

    falshkey = U16_HI(device_id_reg27);
    log_debug("read  aux_reserved_reg4 reg27=[0x%04x] falshkey=[%d] key=[%d] \r\n",device_id_reg27,falshkey,key);
    if (falshkey != key )
    {
        KT_WirelessMicTx_PASW(PA_ON);
        ret = KT_Bus_Read(0x1f);
        if (mic_config.flash_key == MIC_CONFIG_FLAS_KEY)
        {
            KT_Bus_Write(0x27, (key<< 8) | (U16_LO(device_id_reg27)));
            mic_config.PILOT_DATA[3] = ((key<< 8) | (U16_LO(device_id_reg27)));
         //   KT_mic_config_t_print(mic_config);
            if (TestMode == 0)  //正常模式下存flash 测试模式下不存flash
            {
                if (SaveDataToFlash(&mic_config) )
                {
                    log_info("MIC  device change key  Save Data  Success\n");
                }
                else
                {
                    log_error("MIC  device change key  Save Data  fail\n");
                }
            }
            KT_Bus_Write(0x20,(AUX_ADDRB << 8) | AUX_ADDRA);
            KT_Bus_Write(0x21,(AUX_ADDRD << 8) | AUX_ADDRC);

            KT_Bus_Write(0x1f,(ret&0x80f0)|(AUXDATA_EN << 14)|(AUX_REG_NUM << 12) |
                             (AUX_CARRY_NUM << 9)|BPSK_NEW_MODE);
        }
    }
}


void process_microphone()
{
    u16 ret;
    u16 device_id_reg12;
    u16 device_id_reg13;
    u16 device_id_reg14;
    u16 device_id_reg27;
    u32 number_random=0;
    u8 current_channel=0;
    extern int get_random();
   // extern get_u16_random();
    //channel_number=get_random()%2;
    channel_number = CHANNELNUMBER;  //0 通道A   1通道B
    my_memset(&mic_config,'0',sizeof(mic_config_t));
    if (TestMode == 1)  //测试模式
    {
         log_info("into test mode .....\n");
        eraseToFlash();
        mic_config.ch_power_count = get_random()%5;
    }
    //user_flash_read_info(&user_area_info);
    if (ReadDataFromFlash())        //
    {
        KT_mic_config_t_print(mic_config);
       log_info("Read Flash OK\n");
       mic_config.ch_power_count ++;
    }
    else{
        log_info("No Data in Flash!!!\n");
        eraseToFlash();
        mic_config.ch_power_count = get_random()%5;
		log_error("Erase Flash OK\n");

    }
    microphone_enable(true);
    ret=microphone_init();
    if (ret == 0)
    {
        log_info("microphone_init fail  \r\n");
        device_ERR_display();
        return ;
    }
    //number_random = ((get_random()%185 + 51) << 24) | ((get_random()%205 + 33) << 16) | ((get_random()%251 + 1) << 8) | (get_random()%252 + 3);
   // current_channel=number_random%FREQMAXNUMBER;
     if  (mic_config.ch_power_count >95)
     {
           mic_config.ch_power_count = get_random()%5;
     }
    current_channel=number_randomdata[mic_config.ch_power_count ];
    log_info("mic_config.ch_power_count11111=[%d] current_channel=[%d]\r\n",mic_config.ch_power_count,current_channel);

    if (mic_config.channel == current_channel)
    {
        if (mic_config.channel == ( FREQMAXNUMBER -1))
        {
            mic_config.channel =0;
        }
        else{
            mic_config.channel= current_channel+1;
        }
    }
    else{
        mic_config.channel= current_channel;
    }
    mic_config.freq= UserRandomFreq[channel_number][mic_config.channel];
    log_info("ble set mic  FREQMAXNUMBER=[%d] channel_number=[%d]  chanal=%d(%dKHz)\r\n" ,FREQMAXNUMBER, channel_number,mic_config.channel + FREQSTARTCH ,mic_config.freq);
    if (TestMode == 0)  //正常模式模式
    {
        microphone_pairing_flag  = microphone_pairing(mic_config.freq,mic_config.channel +FREQSTARTCH);
    }
    else{
        if (channel_number == 0) // A通道的测试模式
        {
            mic_config.freq=TEST_CHA_FREQ;
            mic_config.channel=1;
            microphone_pairing_flag=0;
            log_debug("into a ch test mode  mic_config.freq=[%d] mic_config.channel=[%d] microphone_pairing_flag=[%d] \n", mic_config.freq,mic_config.channel,microphone_pairing_flag);
            microphone_pairing_flag  = microphone_pairing(mic_config.freq,mic_config.channel );
            log_info("into a ch test mode  microphone_pairing_flag=[%d] \n", microphone_pairing_flag);
        }
        if (channel_number == 1) // B通道的测试模式
        {

            mic_config.freq=TEST_CHB_FREQ;
            mic_config.channel=1;
            microphone_pairing_flag=0;
            log_debug("into b ch test mode  mic_config.freq=[%d] mic_config.channel=[%d] microphone_pairing_flag=[%d] \n", mic_config.freq,mic_config.channel,microphone_pairing_flag);
            microphone_pairing_flag  = microphone_pairing(mic_config.freq,mic_config.channel );
            log_debug("into b ch test mode  microphone_pairing_flag=[%d] \n", microphone_pairing_flag);
        }
    }

    if (microphone_pairing_flag == 0)
    {
        log_info("microphone_pairing fail  \r\n");
        device_ERR_display();
        return ;
    }
    //device_ch_display(mic_config.channel +1 );
   // u32  displayfreq = mic_config.freq)/100;
  //  log_info("mic_config.freq =[%d] displayfreq=[%08x],displayfreq=[%d]",mic_config.freq,displayfreq,displayfreq);
   // display_signal_fre(1);
   // link_on_display();
   // device_frequency_display(mic_config.freq/100);
    device_ch_displayF002(mic_config.channel +1 );
   //link_on_display();
    if (channel_number == 0){
        channel_A_display(1);
        channel_B_display(0);
    }
    else {
        channel_A_display(0);
        channel_B_display(1);
    }


    KT_WirelessMicTx_PASW(PA_ON);
    ret = KT_Bus_Read(0x1f);
    if (mic_config.flash_key == MIC_CONFIG_FLAS_KEY)
    {
        log_info("MIC  device Not the first pairing");
        device_id_reg12 =mic_config.PILOT_DATA[0];

        if (TestMode == 0)  //正常模式
        {
            device_id_reg13 = (0x88 << 8) | (mic_config.channel + FREQSTARTCH  );
        }else{       //测试模式
            device_id_reg13 = (0x88 << 8) | (mic_config.channel + FREQSTARTCH  );
        }
	    device_id_reg14 = mic_config.PILOT_DATA[2];
        device_id_reg27 = mic_config.PILOT_DATA[3];
    }
    else {
        log_info(" MIC  device First pairing OK .....");
        device_id_reg12 = (get_random()%255 << 8) | (get_random()%255);

        if (TestMode == 0)  //正常模式
        {
            device_id_reg13 = (0x88 << 8) | (mic_config.channel + FREQSTARTCH   );
        }else{       //测试模式
            device_id_reg13 = (0x88 << 8) | (mic_config.channel  + FREQSTARTCH );
        }
	    device_id_reg14= (get_random()%255) << 8 | (get_random()%255);
        log_info("Battery_Vdet =[%umv]",Battery_Vdet);
        device_id_reg27 = (0x0 << 8) | (Battery_Vdet/100);
    }


    KT_Bus_Write(0x12, device_id_reg12);
    KT_Bus_Write(0x13, device_id_reg13); //88固定死的  后一个字节 为通道 16
    KT_Bus_Write(0x14, device_id_reg14);
    KT_Bus_Write(0x27, device_id_reg27);

    mic_config.flash_key = MIC_CONFIG_FLAS_KEY;
    mic_config.PILOT_DATA[0] = device_id_reg12;
    mic_config.PILOT_DATA[1] = device_id_reg13;
    mic_config.PILOT_DATA[2] = device_id_reg14;
    mic_config.PILOT_DATA[3] = device_id_reg27;
    KT_mic_config_t_print(mic_config);
    if (TestMode == 0)  //正常模式下存flash 测试模式下不存flash
    {
        if (SaveDataToFlash(&mic_config) )
        {
            log_info("Save Data A Success\n");
        }
        else
        {
            log_error("Save Data A fail\n");
        }

    }
    KT_Bus_Write(0x20,(AUX_ADDRB << 8) | AUX_ADDRA);
    KT_Bus_Write(0x21,(AUX_ADDRD << 8) | AUX_ADDRC);

    KT_Bus_Write(0x1f,(ret&0x80f0)|(AUXDATA_EN << 14)|(AUX_REG_NUM << 12) |
                     (AUX_CARRY_NUM << 9)|BPSK_NEW_MODE);
    KT_WirelessMicTx_MuteSel(FALSE);
    KT_WirelessMicTx_Read_Mic_Sens();
   /* u16 reg03;
    u16 reg0F;
    u16 reg11;
    u16 reg30;
    u16 reg31;
    reg03 = KT_Bus_Read(0x03);
    reg0F = KT_Bus_Read(0x0F);
    reg11 = KT_Bus_Read(0x11);
    reg30 = KT_Bus_Read(0x30);
    reg31 = KT_Bus_Read(0x31);
    log_info(" reg03=[0x%04x] reg0F =[0x%04x]  reg11 =[0x%04x] reg30 =[0x%04x] reg31 =[0x%04x]",reg03, reg0F, reg11,reg30,reg31);
    */

}

void microphone_enable(bool enable)
{
    gpio_set_die(GPIO_MICROPHONE_CHIPEN,digital_mode);
    gpio_set_direction(GPIO_MICROPHONE_CHIPEN,output_mode);
    log_print("enable=[%d]",enable);
    if (enable) {
        gpio_write(GPIO_MICROPHONE_CHIPEN, 1);
    } else {

        KT_WirelessMicTx_MuteSel(TRUE);
        KT_WirelessMicTx_Standby();
        gpio_write(GPIO_MICROPHONE_CHIPEN, 0);
    }
}
