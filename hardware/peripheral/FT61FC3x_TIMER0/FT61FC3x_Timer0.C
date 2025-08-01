// Project: FT61FC3x_Timer0.prj
// Device:  FT61FC3x
// Memory:  PROM=3Kx14, SRAM=256, EEPROM=128                             
// Description: DemoPortOut输出60Hz占空比50%的波形-Timer0实现
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
//  NC------------|6(PB5)   (PA4)15|---DemoPortOut
//  NC------------|7(PB4)   (PA5)14|------------NC
//  NC------------|8(PB3)   (PA6)13|------------NC
//  NC------------|9(PB2)   (PA7)12|------------NC
//  NC------------|10(PB1)  (PB0)11|------------NC
//                 ----------------   
// 
//==========================================================
#include "SYSCFG.h"
//==========================================================
#define  unchar         unsigned char 
#define  DemoPortOut	PA4   
/*-------------------------------------------------
 *  函数名：interrupt ISR
 *	功能：  中断处理
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void interrupt ISR(void)
{  
	if(T0IE && T0IF)                  //8.16ms翻转一次≈60Hz
	{
		T0IF = 0;
		DemoPortOut = ~DemoPortOut;   //翻转电平
	} 
} 
/*-------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{
	 
	OSCCON = 0B01110001;	//IRCF=111=16MHz/2T=8MHz，0.125us
    OPTION = 0B00001000;    //Bit3=1 WDT，Bit[2:0]=000=WDT RATE 1:1
	INTCON = 0;  			//暂禁止所有中断
	PORTA  = 0B00000000;		
	TRISA  = 0B00000000;	//PA输入输出 0-输出 1-输入
							//PA4-OUT
	PORTC  = 0B00000000; 	
	TRISC  = 0B00000000;	//PC输入输出 0-输出 1-输入  
								
	WPUA   = 0B00000000;    //PA端口上拉控制 1-开上拉 0-关上拉						
	WPUC   = 0B00000000;    //PC端口上拉控制 1-开上拉 0-关上拉							                  
}
/*----------------------------------------------------
 *	函数名：TIMER0_INITIAL
 *	功能：  初始化设置定时器
 *	设置TMR0定时时长=(1/16000000)*2*256*255=8.16ms	                      
 ----------------------------------------------------*/
void TIMER0_INITIAL (void)  
{
	OPTION = 0B00000111;	//Bit3=0 Timer0，Bit[2:0]=111=Timer0 RATE 1:256
	T0IF = 0;				//清空Timer0软件中断
    TMR0 = 0;
}
