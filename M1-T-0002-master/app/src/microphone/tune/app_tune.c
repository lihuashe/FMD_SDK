#include "app_tune.h"
#include "user_header.h"
#include "gpio.h"
#include "app_mg.h"
#include "key.h"
#include "wdt.h"

#define LOG_TAG_CONST       APP_TUNE
#define LOG_TAG             "[app_tune]"
#include "debug.h"
#include "adc_drv.h"
#include "KT0646M/microphone.h"
#include "KT0646M/KT0646M.h"

#include "user_flash.h"
//#include "tune/led.h"
#include "uart_dev.h"
#include <stdio.h>
#include "tune/seg.h"

static u8 power_flag =1;

char *devicechaver="VA11"; //{'V','A','0','1','\0'};
char *devicechbver="VB11"; //{'V','B','0','1','\0'};
char *Project_No="0002";
static u16  reg_slnc_cfgA=0;
static u16 reg_slnc_cfgA_before=0x5344;
static u16 power_off_count=0;
u8 slnc_mute_act_falg =0;
static u8 tune_init_flag;
u32 Battery_Vdet=0;
extern u16 microphone_pairing_flag;
extern u8 channel_number ;
extern u8 led_seg_en_seg_arr[42];

static u8  auto_mute_flag=0;
static u8  long_start_flag=0;
static u8  display_Refresh_flag =1;

static u8 Battery_states=0;
static u8 Battery_states_bak=9;


uint8_t TestMode = 0; // 0:正常搜台模式, 1:测试模式
static u8 tube_pin[7] = {
    IO_TUBE_NUM_1,
    IO_TUBE_NUM_2,
    IO_TUBE_NUM_3,
    IO_TUBE_NUM_4,
    IO_TUBE_NUM_5,
    IO_TUBE_NUM_6,
    IO_TUBE_NUM_7,
};

uint8_t CheckSum(uint8_t *buf, uint8_t len);

extern const u16 iokey_msg_tube_table[6][IO_KEY_MAX_NUM];
static u16 tube_msg_filter(u8 key_status, u8 key_num, u8 key_type)
{
    u16 msg = NO_MSG;
    switch (key_type) {
#if KEY_IO_EN
    case KEY_TYPE_IO:
        msg = iokey_msg_tube_table[key_status][key_num];
        break;
#endif
#if KEY_AD_EN
    // case KEY_TYPE_AD:
    //     msg = adkey_msg_tube_table[key_status][key_num];
    //     break;
#endif

    default:
        break;
    }

    return msg;
}




void io_init()
{
    gpio_set_die(IO_PORTA_11,digital_mode);
    gpio_set_direction(IO_PORTA_11,output_mode);
    gpio_set_pull_down(IO_PORTA_11,pull_down_dis);
    gpio_set_pull_up(IO_PORTA_11,pull_up_en);

    gpio_write(IO_PORTA_11,1);
    power_flag=0;
    board_seg_init();
    power_on_display();
    tune_init_flag = 1;
    process_microphone();
    if (TestMode == 1)  //测试模式下的灯测试 500ms闪 次
    {

        for (int i=0; i<3; i++)
        {
            //kt0646_white_led_on();
			//kt0646_red_led_on();
            udelay(500*1000);
          //  kt0646_white_led_off();
		//	kt0646_red_led_off();
            udelay(500*1000);
        }
    }
}

int io_deinit()
{
   for(int i = 0; i < 7;i++)
    {
        gpio_set_die(tube_pin[i],analog_mode);
        gpio_set_direction(tube_pin[i],input_mode);
    }
    tune_init_flag = 0;
    return 0;
}
extern void uart01_test();



void seg_disply_link_freq_battery()
{
    link_on_display();
    display_signal_fre(1);
    if (CHANNELNUMBER == 0){
        channel_A_display(1);
        channel_B_display(0);
    }
    else {
        channel_A_display(0);
        channel_B_display(1);
    }
        Battery_Vdet = (my_adc_get_volgate(ADC_CH_PB5)*250)/100;
    //  log_debug("Battery_Vdet =[%umv] power_flag=[%d]",Battery_Vdet,power_flag);
    KT0646_setchangeBattery();
    //Battery_Vdet = 3000;
    if (Battery_Vdet >= 2600)
    {
        Battery_states =4;
        if ((microphone_pairing_flag == 1) && (power_flag ==1)  && (auto_mute_flag==0))
        {
            seg_power_arr_write(led_seg_en_seg_arr,4);
        }

    }
    else if (Battery_Vdet >= 2400)
    {
        Battery_states =3;
        if ((microphone_pairing_flag == 1) && (power_flag ==1) && (auto_mute_flag==0))
        {
            seg_power_arr_write(led_seg_en_seg_arr,3);
        }

    }
    else if (Battery_Vdet >=2200)
    {
        Battery_states =2;
        if ((microphone_pairing_flag == 1) && (power_flag ==1)  && (auto_mute_flag==0))
        {
            seg_power_arr_write(led_seg_en_seg_arr,2);
        }

    }
    else if (Battery_Vdet >=2000 )
    {
        Battery_states =1;
        if ((microphone_pairing_flag == 1) && (power_flag ==1) && (auto_mute_flag==0))
        {
            seg_power_arr_write(led_seg_en_seg_arr,1);
        }
    }
    else
    {
        log_info("vdet =[%umv]",Battery_Vdet);
        Battery_states =0;
        microphone_AUX_Disconnect();
        seg_power_arr_write(led_seg_en_seg_arr,0);
        udelay(500*1000);
        seg_power_arr_write(led_seg_en_seg_arr,1);
        udelay(500*1000);
        seg_power_arr_write(led_seg_en_seg_arr,0);
        udelay(500*1000);
        seg_power_arr_write(led_seg_en_seg_arr,1);
        udelay(500*1000);
        seg_power_arr_write(led_seg_en_seg_arr,0);
        udelay(500*1000);
        seg_power_arr_write(led_seg_en_seg_arr,1);
        udelay(10*1000);
        gpio_write(IO_PORTA_11,0);
        extern void sys_softoff();
        sys_softoff();

    }
    Battery_states =5;
    display_Refresh_flag=1;
}

int app_tune_init(void *param)
{
    log_info("app tune init!\r\n");
    key_table_sel((void *)tube_msg_filter);

    gpio_set_die(IO_PORTB_05, analog_mode);
    gpio_set_direction(IO_PORTB_05, input_mode);
    gpio_set_pull_down(IO_PORTB_05, pull_down_dis);
    gpio_set_pull_up(IO_PORTB_05, pull_down_dis);

    my_adc_init();
   // Battery_Vdet = (my_adc_get_volgate(ADC_CH_PB5)*250)/100;
    io_init();
    power_flag=1;
    uart01_test();//和PC的通讯串口初始化
    key_table_sel((void *)tube_msg_filter);
    KT_WirelessMicTx_Mic_Sens(MIC_SENS_GAIN_4);
    log_info("tube init ok ");
    return 0;
}


static int app_tune_close()
{
    io_deinit();
    return 0;
}

void tune_poll()
{
   led_seg_display_timer_callback();
}


extern u32 user_uart_read(u8 *buf, u32 len, u32 timeout);
extern void user_uart_send_dma( u8 *buf, u32 len);
void KT0646_UserTask_100ms(void) // // 1000ms任务, 主要处理串口接收响应，打印信息等操作
{
 uint8_t buff_head[4]={0};
    uint8_t buff_data[10]={0};
    uint8_t head_len = 3;
    uint8_t Data_len = 0;
    uint8_t rev_len = 0;
    uint8_t checksum_buf[20]={0};
    uint8_t I2CDef_KT065xM_CH =0;
    memset(buff_head,0,sizeof(buff_head));
    rev_len=user_uart_read(buff_head,head_len,25);
    if (rev_len ==3)
    {
    //    log_info("buff_head[0]=[0x%02x] buff_head[1]=[0x%02x] buff_head[2]=[0x%02x]\n",buff_head[0],buff_head[1],buff_head[2]);
        if ((buff_head[0] == 0x55) &&  (buff_head[1] == 0xAA))
        {
            Data_len=buff_head[2] + 1;
            rev_len=user_uart_read(buff_data,Data_len,25);
            put_buf(buff_data,rev_len);

            memset(checksum_buf,0,sizeof(checksum_buf));
            checksum_buf[0]=buff_head[2];
            memcpy(checksum_buf+1,buff_data,Data_len);
            if (CheckSum(checksum_buf,Data_len) == buff_data[rev_len -1])
            {
                if (buff_data[0] == 0x00) //判断是否为 cmd命令
                {
                    if ((buff_data[1] == 0x04) || (buff_data[1] == 0x06) || (buff_data[1] == 0x08) ||
                        (buff_data[1] == 0x10) || (buff_data[1] == 0x0A) || (buff_data[1] == 0x11))
                    {
                        if (buff_data[2] == 0x00)
                        {
                            I2CDef_KT065xM_CH=I2CDef_KT065xM_CHA;
                        }
                        else{
                            I2CDef_KT065xM_CH=I2CDef_KT065xM_CHB;
                        }
                   }
                   else if (buff_data[1] == 0x12)     //烧录序列号
                   {
                        if  (rev_len <7)
                        {
                            log_error("Packet verification error !!!  err data !!!");
                        }
                        else{

          /*                  if (ReadDataFromFlash())        //
                            {
                                KT_mic_config_t_print(mic_config);
                                log_info("Read Flash OK\n");
                            }
                            else{
                                log_info("No Data in Flash!!!\n");
                                eraseToFlash();
                                log_error("Erase Flash OK\n");
                            }
                            */
                            mic_config.flash_key=MIC_CONFIG_FLAS_KEY;
                            mic_config.PILOT_DATA[0] = (buff_data[2]<< 8) | (buff_data[3]) ;
                            mic_config.PILOT_DATA[1] = (buff_data[4] <<8) | (buff_data[5]) ;

                            if (TestMode == 0)  //正常模式下存flash 测试模式下不存flash
                            {
                                if (SaveDataToFlash(&mic_config) )
                                {
                                    log_debug("MIC  device change batt  Save Data  Success\n");
                                }
                                else
                                {
                                    log_error("MIC  device change batt  Save Data  fail\n");
                                }
                            }
                        }
                    }
                    Rx_Response(buff_data[1]);
                }
                else
                {
                    log_error("Not CMD command  err CheckSum !!!");
                }

            }
            else
            {
                 log_error("Packet verification error !!!  err checksum =[0x%02x]!!!",buff_data[rev_len -1]);
            }
        }
    }

}

void Rx_Response(uint8_t cmd)
{
    uint8_t send_buff[30]={0};
    uint8_t send_len=0;
    switch (cmd)
    {
       case 0x12:  //收到0x00命令 CMD: RequestProtocolVersion   版本 V1.0
           memset(send_buff,0,sizeof(send_buff));
           send_buff[0]=0x55;
           send_buff[1]=0xAA;
           send_buff[2]=0x04;
           send_buff[3]=0x01;
           send_buff[4]=0x00;
           send_buff[5]=0x01;
           send_buff[6]=0x00;
           send_buff[7]=CheckSum(send_buff + 2,5);
           user_uart_send_dma(send_buff,8);  //返回 命令  STAT:RetProtocolVersion
           break;
        default:
            break;
        }
}


void KT0656_UserTask_250ms(void) // 500ms 任务，主要打印一些次要信息，处理呼吸灯等
{


}

uint8_t CheckSum(uint8_t *buf, uint8_t len)
{
    uint8_t i,sum=0;
    for(i=0 ;i< len ;i++)
    {
        sum = sum + buf[i];
    }
    return  sum;
}

int app_tune(void *param)
{
    int msg[2];
    u16 reg26=0;
    log_info("Enter APP TUNE  zl!\n");
    if (app_tune_init(param) != 0) {
        log_error("app_tune init error !!! \n");
        app_switch_next();
        return 0;
    }

    while(1)
    {
        get_msg(2, &msg[0]);
        bsp_loop();
        switch (msg[0])
        {
        case MSG_100MS:
            KT0646_UserTask_100ms();
            wdt_clear();
            break;
        case MSG_500MS:

            extern u8 led_seg_en_seg_arr[42];

            reg_slnc_cfgA=KT_Bus_Read(0x25);
            if (reg_slnc_cfgA_before != reg_slnc_cfgA)
            {
             //   log_info("REG:0x25 = [0x%04x]  reg_slnc_cfgA_before=[0x%04x] power_off_count=[%d] display_Refresh_flag=[%d]\r\n", reg_slnc_cfgA,reg_slnc_cfgA_before,power_off_count,display_Refresh_flag);
                reg_slnc_cfgA_before=reg_slnc_cfgA;

                if (reg_slnc_cfgA_before == 0x5344)  //5分钟检测到没有音频信号输入 触发  静音
                {
                    power_off_count=0;
                    display_Refresh_flag=1;
                    auto_mute_flag=1;
                    device_mute_display();
                    microphone_AUX_mute();
                    KT_WirelessMicTx_PAGain(0);
                    KT_WirelessMicTx_MuteSel(TRUE);
                   // log_info("MIC disenable");
                }
                else{
                    power_off_count=0;
                    display_Refresh_flag=1;
                    Battery_states =9;
                    //log_info("microphone_pairing_flag=[%d] display_Refresh_flag=[%d]\r\n",microphone_pairing_flag,display_Refresh_flag);
                    if ((microphone_pairing_flag == 1) && (power_flag ==1))
                    {
                        if (display_Refresh_flag==1)
                        {
                           // display_Refresh_flag=0;
                           // device_frequency_display(mic_config.freq/100);
						    KT_WirelessMicTx_PAGain(mic_config.PA_gain);
                            device_ch_displayF002(mic_config.channel +1 );
                            if (auto_mute_flag ==1)
                            {
                                auto_mute_flag=0;
                                microphone_AUX_unmute();
                            }
                            if (power_flag ==1)
                            {
                                link_on_display();
                                display_signal_fre(1);
                            }
                            if (channel_number == 0){
                                channel_A_display(1);
                                channel_B_display(0);
                            }
                            else {
                                channel_A_display(0);
                                channel_B_display(1);
                            }
                        }
                    }
                }
            }
            else{
                if (reg_slnc_cfgA_before == 0x5344)  //静音后开始计时
                {
                     power_off_count++;
                    //  display_Refresh_flag=1;
                }else
                {
                    power_off_count=0;
                    KT_WirelessMicTx_MuteSel(FALSE);
                    if (display_Refresh_flag==1)
                    {
                       // display_Refresh_flag=0;
                        //log_info("3333 microphone_pairing_flag=[%d] display_Refresh_flag=[%d]\r\n",microphone_pairing_flag,display_Refresh_flag);
                        if ((microphone_pairing_flag == 1) && (power_flag ==1))
                        {
                           // device_frequency_display(mic_config.freq/100);
						   KT_WirelessMicTx_PAGain(mic_config.PA_gain);
                           device_ch_displayF002(mic_config.channel +1 );
                            if (auto_mute_flag ==1)
                            {
                                auto_mute_flag=0;
                                microphone_AUX_unmute();
                            }
                            if (channel_number == 0){
                                channel_A_display(1);
                                channel_B_display(0);
                            }
                            else {
                                channel_A_display(0);
                                channel_B_display(1);
                            }
                            if (power_flag ==1)
                            {
                                link_on_display();
                                display_signal_fre(1);
                            }
                        }
                    }
                    //log_info("MIC enable");
                }
            }

                if (reg_slnc_cfgA == 0x5344)  //检测到没有音频信号输入
            {
                reg_slnc_cfgA_before=reg_slnc_cfgA;

               // log_info("REG:0x25 = [0x%04x] power_off_count=[%d] \r\n", reg_slnc_cfgA,power_off_count);
                if (power_off_count ==120*25)   //静音后 25分钟 关机
                {
                //  power_off_display();
                    microphone_AUX_Disconnect();
                    udelay(1*1000*1000);
                    microphone_enable(FALSE);
                    udelay(10*1000);
                    gpio_write(IO_PORTA_11,0);
                    extern void sys_softoff();
                    sys_softoff();
                }
            }
            Battery_Vdet = (my_adc_get_volgate(ADC_CH_PB5)*250)/100;
           // log_info("microphone_pairing_flag=[%d] Battery_Vdet =[%umv] power_flag=[%d] Battery_states_bak=[%d] Battery_states=[%d] display_Refresh_flag=[%d]",microphone_pairing_flag,Battery_Vdet,power_flag,Battery_states_bak,Battery_states ,display_Refresh_flag);
           // Battery_Vdet = 3000;
            KT0646_setchangeBattery();
            if (Battery_Vdet >= 2600)
            {
                Battery_states =4;
              //  log_info("7777777777777777  Battery_states_bak=[%d] Battery_states=[%d]\r\n",Battery_states_bak,Battery_states);
                if((Battery_states_bak != Battery_states) || (display_Refresh_flag ==1))
                {
                 //  log_info("8888888888888888\r\n");
                    Battery_states_bak =Battery_states;
                  //  display_Refresh_flag =0;
                    if ((microphone_pairing_flag == 1) && (power_flag ==1)  && (auto_mute_flag==0))
                    {
                    //    log_info("999999999999\r\n");
                        seg_power_arr_write(led_seg_en_seg_arr,4);
                    }
                }
            }
            else if (Battery_Vdet >= 2400)
            {
                Battery_states =3;
                if ((microphone_pairing_flag == 1) && (power_flag ==1) &&  (auto_mute_flag==0))
                {
                    Battery_states_bak =Battery_states;
                   // display_Refresh_flag =0;
                    if ((microphone_pairing_flag == 1) && (power_flag ==1))
                    {
                        seg_power_arr_write(led_seg_en_seg_arr,3);
                    }
                }
            }
            else if (Battery_Vdet >=2200)
            {
                Battery_states =2;
                if((Battery_states_bak !=Battery_states) || (display_Refresh_flag ==1))
                {
                    Battery_states_bak =Battery_states;
                   // display_Refresh_flag =0;
                    if ((microphone_pairing_flag == 1) && (power_flag ==1) && (auto_mute_flag==0))
                    {
                        seg_power_arr_write(led_seg_en_seg_arr,2);
                    }
                }
            }
            else if (Battery_Vdet >=2000 )
            {
                Battery_states =1;
                if((Battery_states_bak !=Battery_states) || (display_Refresh_flag ==1))
                {
                    Battery_states_bak =Battery_states;
                    //display_Refresh_flag =0;
                    if ((microphone_pairing_flag == 1) && (power_flag ==1) && (auto_mute_flag==0))
                    {
                        seg_power_arr_write(led_seg_en_seg_arr,1);
                    }
                }
            }
            else
            {
                log_info("seneasy vdet =[%umv]",Battery_Vdet);
                microphone_AUX_Disconnect();
                Battery_states =0;
                if(Battery_states_bak !=Battery_states)
                {
                    Battery_states_bak =Battery_states;
                }
                seg_power_arr_write(led_seg_en_seg_arr,0);
                udelay(500*1000);
                seg_power_arr_write(led_seg_en_seg_arr,1);
                udelay(500*1000);
                seg_power_arr_write(led_seg_en_seg_arr,0);
                udelay(500*1000);
                seg_power_arr_write(led_seg_en_seg_arr,1);
                udelay(500*1000);
                seg_power_arr_write(led_seg_en_seg_arr,0);
                udelay(500*1000);
                seg_power_arr_write(led_seg_en_seg_arr,1);
                udelay(10*1000);
                gpio_write(IO_PORTA_11,0);
                extern void sys_softoff();
                sys_softoff();

            }

            if (display_Refresh_flag == 1)
            {
                display_Refresh_flag=0;
            }

            wdt_clear();
            break;

        case MSG_KEY_SHORT:
           log_info("  key MSG_KEY_SHORT power_flag=[%d] microphone_pairing_flag=[%d] \n",power_flag,microphone_pairing_flag);
           if (power_flag ==1)
           {
                if (auto_mute_flag ==0)
               {
                    power_flag=0;
                    microphone_pairing_flag=0;
               }

            }
            break;
        case MSG_KEY_SHORT_UP:
            log_info("  key  MSG_KEY_SHORT_UP  power_flag=[%d] long_start_flag=[%d] auto_mute_flag=[%d]\n",power_flag,long_start_flag,auto_mute_flag);
            if (power_flag==0)
            {
                power_flag=1;
                microphone_pairing_flag=1;
            }
            if (auto_mute_flag ==1 )
            {
                power_off_count=0;
                auto_mute_flag=0;
                microphone_pairing_flag=1;
                display_Refresh_flag=1;
                // microphone_enable(TRUE);
                KT_Bus_Write(0x25, 0x5344 );
                reg26=KT_Bus_Read(0x26);
                KT_Bus_Write( 0x26,(reg26&0xfffb)|(0 << 2));
                KT_WirelessMicTx_PAGain(mic_config.PA_gain);
                KT_WirelessMicTx_MuteSel(FALSE);
                log_debug("MIC enable");
                microphone_AUX_unmute();
                device_ch_displayF002(mic_config.channel +1 );
                seg_disply_link_freq_battery();
                Battery_states =5;
                display_Refresh_flag=1;
            }
            break;


        case MSG_KEY_LONG1_UP:  //长按1秒
           log_info("  key  MSG_KEY_SHORT_UP  power_flag=[%d] long_start_flag=[%d]\n",power_flag,long_start_flag);

            if ((power_flag==0) ||(auto_mute_flag ==1))
            {
                if  (long_start_flag ==0)
                {
                     power_off_display();
                    udelay(1*1000*1000);
                    seg_display_none();

                }
                microphone_AUX_Disconnect();
                microphone_enable(FALSE);
                udelay(10*1000);
                gpio_write(IO_PORTA_11,0);
                udelay(10*1000);
                extern void sys_softoff();
                sys_softoff();
            }
            long_start_flag=0;
            break;


        case MSG_KEY_LONG_START:
            log_info("  key  MSG_KEY_LONG_START  power_flag=[%d] long_start_flag=[%d] \n",power_flag,long_start_flag);
            if ((power_flag==0) ||(auto_mute_flag ==1))
           {
                long_start_flag =1;
                power_flag=0;
                microphone_pairing_flag=0;
                power_off_display();
                microphone_AUX_Disconnect();
                udelay(1*1000*1000);
                seg_display_none();
                udelay(10*1000);
           }
           break;
        case MSG_KEY_LONG:   //长按10秒
         //   log_info("  key MSG_KEY_LONG power_flag=[%d] microphone_pairing_flag =[%d]\n",power_flag,microphone_pairing_flag);
            if ((microphone_pairing_flag == 0)  ||(auto_mute_flag ==1))
            {
                device_projectno_display(Project_No); //显示项目编号
                udelay(1*1000*1000);
                if (CHANNELNUMBER)
                {
                    device_version1_display(devicechbver);   //显示版本编号
                }
                else{
                    device_version1_display(devicechaver);
                }
                udelay(2*1000*1000);
                seg_display_none();
                microphone_enable(FALSE);
            }
            break;
        case MSG_KEY_LONG_UP:
          //  log_info("  key MSG_KEY_LONG_UP power_flag=[%d] microphone_pairing_flag =[%d]\n",power_flag,microphone_pairing_flag);
            if ((microphone_pairing_flag == 0) ||(auto_mute_flag ==1))
            {
                gpio_write(IO_PORTA_11,0);
                extern void sys_softoff();
                sys_softoff();
            }
            long_start_flag=0;
            break;
        case MSG_APP_SWITCH_ACTIVE:
            log_debug("MSG_APP_SWITCH_ACTIVE \n");
            if (app_switch_en()) {
                goto _app_tube_switch;
            }
            break;
        default:
            break;
        }
    }

_app_tube_switch:
    app_tune_close();
    return 0;
}
