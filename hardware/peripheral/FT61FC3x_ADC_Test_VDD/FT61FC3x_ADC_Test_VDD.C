// Project: FT61FC3x_ADC_Test_VDD.prj
// Device:  FT61FC3x
// Memory:  PROM=3Kx14, SRAM=256, EEPROM=128                             
// Description: 通过内部参考电压反推VDD电压
//
// RELEASE HISTORY
// VERSION DATE     DESCRIPTION
// 1.0	   24-1-16	初版
// 1.1	   24-1-24  更新版本号 
// 1.2     24-2-26  更新文件头描述
//
//                  FT61FC35  SOP20
//                 ----------------
//  GND-----------|1(GND)   (VDD)20|------------VDD     
//  NC------------|2(PC1)   (PA0)19|------------NC
//  NC------------|3(PC0)   (PA1)18|------------NC
//  NC------------|4(PB7)   (PA2)17|------------NC
//  NC------------|5(PB6)   (PA3)16|------------NC
//  NC------------|6(PB5)   (PA4)15|------------NC
//  NC------------|7(PB4)   (PA5)14|------------NC
//  NC------------|8(PB3)   (PA6)13|------------NC
//  NC------------|9(PB2)   (PA7)12|------------NC
//  NC------------|10(PB1)  (PB0)11|------------NC
//                 ----------------   
// 
#include "SYSCFG.h"
//宏定义****************************************************
#define  unchar     unsigned char 
#define  unint      unsigned int
#define  unlong     unsigned long

volatile unint  adcData;
volatile unint  VDD_Voltage;
/*-------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{
	OSCCON = 0B01110001;	//16MHz/2T = 8MHz，0.125us
	INTCON = 0B00000000;  	//暂禁止所有中断
    
	PORTA = 0B00000000;		
	TRISA = 0B00000000;		//PA输入输出 0-输出 1-输入
	PORTB = 0B00000000;		
	TRISB = 0B00000000;		//PB输入输出 0-输出 1-输入							
	PORTC = 0B00000000; 	
	TRISC = 0B00000000;		//PC输入输出 0-输出 1-输入  	
	
	WPUA = 0B00000000;     	//PA端口上拉控制 1-开上拉 0-关上拉
	WPUB = 0B00000000;     	//PB端口上拉控制 1-开上拉 0-关上拉
	WPUC = 0B00000000;     	//PC端口上拉控制 1-开上拉 0-关上拉
}
/*------------------------------------------------- 
 *	函数名称：DelayUs
 *	功能：    短延时函数 --16M-2T--大概快1%左右.
 *	输入参数：Time 延时时间长度 延时时长Time Us
 *	返回参数：无 
 -------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
}  
/*-------------------------------------------------
 *  函数名: ADC_INITIAL
 *	功能：  ADC初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void ADC_INITIAL(void) 
{
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
              

	DelayUs(200);	
    DelayUs(250);           //必须要延时450us
    			   			 			
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
/*-------------------------------------------------
 *  函数名: GET_ADC_DATA
 *	功能：  读取通道ADC值
 *  输入：  adcChannel 通道序号
 *  输出：  INT类型AD值(单次采样无滤波)
 --------------------------------------------------*/
unint GET_ADC_DATA (unchar adcChannel) 
{ 
	ADCON0 &= 0B00000111;        				   
    ADCON0 |= adcChannel<<3; 				//重新加载通道值
	DelayUs(40);                        	//廷时等待电压稳定 Tst >10us
	ADCON0 = ADCON0|0x02;             		//启动ADC 
    NOP();
    NOP();
	while(ADCON0&0x02);    					//等待ADC转换完成

    return  (unint)(ADRESH<<8|ADRESL);      //整合12位AD值
} 
