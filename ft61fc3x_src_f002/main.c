//Project: ft61fc3x_src_f002.prj
// Device: FT61FC3X
// Memory: Flash 3KX14b, EEPROM 128X8b, SRAM 256X8b
// Author: 
//Company: 
//Version:
//   Date: 
//===========================================================
//                  FT61FC35  SOP20
//                 ----------------
//  GND -------------|1(GND)   (VDD)20|------------VDD     
// ISP_DAT ----------|2(PC1)   (PA0)19|------------[A] battery
// ISP_CLK  ---------|3(PC0)   (PA1)18|------------[O] POWER_ON
//  D7[0]------------|4(PB7)   (PA2)17|-----------	[I] ON/OFF
//  D6[O]------------|5(PB6)   (PA3)16|------------[I] POWER_KEY
//  D5[O]------------|6(PB5)   (PA4)15|----------- [I] VOL-
//  D4[O]------------|7(PB4)   (PA5)14|------------[I] VOL+
//  D3[O]------------|8(PB3)   (PA6)13|------------SCL
//  D2[O]------------|9(PB2)   (PA7)12|------------SDA
//  D1[O]------------|10(PB1)  (PB0)11|------------[O]U_EN
//                 ----------------   
//===========================================================

#include <syscfg.h>
#include "SD1516/sd1516.h"
#include "KT0646M/microphone.h"

#define VOL_UP		PA5
#define VOL_DOWN	PA4
#define POWER_KEY	PA3
#define POWER_ON	PA1

//===========================================================
//Variable definition
//===========================================================

//===========================================================
//Function name：interrupt ISR
//parameters：无
//returned value：无
//===========================================================
void interrupt ISR(void)
{
	if(TMR1IF)                     
	{
		TMR1IF = 0;                 //清零中断标志位
		timer1_isr(); 				//进入中断
	} 	
}

void delya_2us(uint8_t us)
{
	unsigned char a;
	for(a = 0; a < us; a++)
	{
		NOP();
	}
}

void delay_ms(uint16_t ms)
{
	for (uint16_t i = 0; i < ms; i++) {
		for(uint8_t j = 0; j < 250; j++) {
			NOP();
		}
	}
}


void io_init(void)
{
	//PA部分IO口初始化
	OSCCON = 0B01110001;	//IRCF=111=16MHz/4T=4MHz,0.25us		
	OPTION = 0B00001000;	 		
	INTCON = 0;  			//暂禁止所有中断
	PORTA  = 0B00000001;		
	TRISA  = 0B00111000;	//配置按键初始化，io口初始化
    WPUA = 0B00111000;

	ADCON1 = 0B10100101; 	//右对齐，转换时钟SysClk/32，负参考电压GND，正参考电压VDD
  	//Bit7:ADFM A/D转换结果格式 
	//1 = 右对齐。 
	//0 = 左对齐。
  
  	//Bit6~Bit4:ADCS ADC转换时钟源
    //TSEL=2T
	//000 = SysClk/2
	//001 = SysClk/8
	//010 = SysClk/32
	//011 = SysClk
	//100 = SysClk/4
	//101 = SysClk/16
	//110 = SysClk/64
	//111 = LIRC
	
	//Bit3~Bit2:ADNREF ADC负参考电压
	//00 = Int Vref（内部参考电压）
	//01 = GND
	//10 = Int Vref + Ext Cap（内部参考电压 + 外部电容）
	//11 = Ext Vref（外部参考电压）

  	//Bit1~Bit0:ADPREF ADC正参考电压
	//00 = Int Vref（内部参考电压）
	//01 = VDD
	//10 = Int Vref + Ext Cap（内部参考电压 + 外部电容）
	//11 = Ext Vref（外部参考电压）
                             
	ADCON0 = 0B00000000; 	
  	//Bit6~Bit3:CHS ADC模拟通道选择位
	//0000 = AN0
	//0001 = AN1
	//0010 = AN2
	//0011 = AN3
	//0100 = AN4
	//0101 = AN5
	//0110 = AN6
	//0111 = AN7
	//1xxx = Int Vref（内部参考电压）

    //Bit2:ADEX	 ADC触发条件
	//0 = 由置位指令GO/DONE（软件触发）
	//1 = 由PA4或PWM置位G0/DONE（硬件触发）
		
    //Bit1:G0/DONE ADC转换启动和状态位
    //0 = A/D转换完成/未进行转换
	//1 = A/D转换正在进行或硬件触发延时正在计数

    //Bit0:ADON 使能ADC  
    //0 = ADC被禁止且不消耗工作电流
	//1 = ADC被使能
              

	delya_2us(200);	
    delya_2us(250);           //必须要延时450us
    			   			 			
    ADCON2 = 0B01000000; 	//选择内部正参考电压2V，无外部触发源
    //Bit7~Bit6:ADINTREF ADC内部参考电压
	//00 = 0.5V
	//01 = 2.0V
	//10 = 3.0V
	//11 = 未连接
		
	//Bit5~Bit4:ETGTYP 外部触发沿（当ADEX=1时适用）
	//00 = PWM 或 ADC_ETR脚的下降沿
	//01 = PWM 或 ADC_ETR脚的上升沿
		
	//Bit3:ADDLY.8/LEBPR9 ADC外部触发延时计数器或LEB计数器的第8位
		
	//Bit2~Bit0:ETGSEL 外部触发源（当ADEX=1时适用）
	//选择PWM源时需要配置TIMER为PWM输出模式并使能输出。
	//000 = P1A0
	//001 = P1A0N
	//010 = P1B
	//011 = P1C
	//100 = P1D
	//101 = ADC_ETR
	//11x = 无


    ADCON3 = 0B00000000; 
    //Bit7:ADFBEN ADC比较结果响应故障刹车使能
	//0 = 禁止
	//1 = 使能
		
	//Bit6:ADCMPOP ADC比较器输出极性
	//0 = ADC结果的高八位大于或等于ADCMPH[7:0]
	//1 = ADC结果的高八位小于ADCMPH[7:0]

	//Bit5:ADCMPEN ADC结果比较使能位
	//0 = 关闭
	//1 = 使能
		
	//Bit3:LEBADT 前沿消隐周期结束后，ADC触发使能
	//1 = 触发ADC转换
	//0 = 不触发ADC转换

    ADCMPH = 0B00000000;	//ADC比较阈值,仅8位，用于ADC结果高8位比较

    ADON=1;                 //使能ADC
}

//===========================================================
//Function name：main
//parameters：无
//returned value：无
//===========================================================
static uint8_t key_down = 0;
static uint8_t key_hold = 0;
static uint8_t key_read = 0;

static uint16_t power_key_ticks = 0;
main()
{
	delay_ms(5);
	uint8_t i,j;

	/** 配置默认IO状态 */
	TRISB0 = 0;	// KT0646.EN, 输出
	PB0	= 1;	// KT0646.EN, 默认不使能
	
    OPTION = 0B00001000;
	PORTA  = 0B00000000;		
	TRISA  = 0B00111000;	//配置按键初始化，io口初始化
    WPUA = 0B00111000;


	/** 开机检测: 按键 >0.5s,开机 */
	i = 0;
	do {
	delay_ms(10);
	i++;
	} while(POWER_KEY == 0 && i < 50);
	if (i < 50) {
	while(1);	// 按键时间<0.5s, 不开机;
	}
	/** MCU输出开机保持 */
	POWER_ON = 1;	// 开机成功,输出高电平保持电源开启;

	/** microphon初始化 */
	// microphone_init();

	/** 显示初始化 */
	sd1516_init();
	disp.digits[0] = 'c';
	disp.digits[1] = 'h';
	disp.digits[2] = ' ';
	disp.digits[3] = g_config.channel + 0x30;
	disp.status.cha = 1;
	disp.status.mhz = 1;
	disp.status.signal = 1;
	disp.status.level = 3;
	sd1516_update_display();
	/** microphon初始化 */
	microphone_init();

	/** 处理按键和异常 */
	while(1)
	{
		/** 按键扫描 */
		key_read = 0;
		key_read = (POWER_KEY << 2) | (VOL_UP << 1) | (VOL_DOWN);
		// 检测按键按下（低电平有效的下降沿）
		key_down = (~key_hold) & key_read; 
		key_hold = key_read;
		/** 处理按键 */
		// 音量-
		if (key_down & 0x01) {
			disp.digits[2] = '1';
			sd1516_update_display();
			// microphone_vol_down();
		}
		// 音量+
		if (key_down & 0x02) {
			disp.digits[2] = '2';
			sd1516_update_display();
			// microphone_vol_up();
		}
		// power
		if (key_down & 0x04) {
			disp.digits[2] = '4';
			power_key_ticks = 0;
		}
		if ((key_hold & 0x04) == 0) {
			power_key_ticks++;
			// 电源键长按2S
			if (power_key_ticks > 40) {
				/** 关机流程 */
				// // 1. BPSK发送关机指令,避免接收端pop音,同时显示OFF
				// microphone_bpsk_poweroff();
				// // 2. 保存当前状态
				// microphone_save_config();
				// // 3. 显示off,等待500ms
				sd1516_clear();
				sd1516_draw_digit(1, '0');
				sd1516_draw_digit(2, '0');
				sd1516_draw_digit(3, 'f');
				delay_ms(500);
				// 4. KT0646关机
				// KT_WirelessMicTx_PowerDownProcedure();
				// 5. MCU释放电源HOLD
				POWER_ON = 0;
				// 6. 等待用户释放电源按键,整机断电
				while(POWER_KEY == 0)
				{
					;
				}
				// 7. 不应该到这里
				while(1);
			}
		}

		delay_ms(50);
	}
}
//===========================================================
