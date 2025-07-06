// Project: FT61FC3x_IO.prj
// Device:  FT61FC3x
// Memory:  PROM=3Kx14, SRAM=256, EEPROM=128                             
// Description: 当DemoPortIn悬空或者高电平时，DemoPortOut输出50Hz占空比50%的波形	    
//		    	当DemoPortIn接地时，DemoPortOut输出高电平
// 				注意：因为PA6是烧录口的CLK，当用PA6做烧录口仿真时，此程序必须断开烧录口测试
//				验证，断开后要断电VCC复位单片机。
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
//  NC------------|8(PB3)   (PA6)13|----DemoPortIn
//  NC------------|9(PB2)   (PA7)12|------------NC
//  NC------------|10(PB1)  (PB0)11|------------NC
//                 ----------------   
// 
//===========================================================
#include	"SYSCFG.h";
//===========================================================
//Variable definition
//===========================================================
#define  unchar     	unsigned char 

#define  DemoPortOut	PA4   
#define  DemoPortIn		PA6
/*-------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
	OSCCON = 0B01110001;	//IRCF=111=16MHz/2T=8MHz，0.125us
    OPTION = 0B00001000;	//Bit3=1 WDT，Bit[2:0]=000=WDT RATE 1:1
	INTCON = 0;  			//暂禁止所有中断
    
	PORTA  = 0B00000000;		
	TRISA  = 0B01000000;	//PA输入输出 0-输出 1-输入
							//PA4-OUT PA6-IN			
	PORTC  = 0B00000000; 	
	TRISC  = 0B00000000;	//PC输入输出 0-输出 1-输入  
								
	WPUA   = 0B01000000;    //PA端口上拉控制 1-开上拉 0-关上拉
							//开PA6上拉
	WPUC   = 0B00000000;    //PC端口上拉控制 1-开上拉 0-关上拉
}   
/*----------------------------------------------------
 *	函数名：DelayUs
 *	功能：  短延时函数
 *	输入：  Time延时时间长度 延时时长Time us
 *	输出：  无 
 ----------------------------------------------------*/
void DelayUs(unsigned char Time)
{
	unsigned char a;
	for(a=0;a<Time;a++)
	{
		NOP();
	}
}                  
/*----------------------------------------------------
 *	函数名：DelayMs
 *	功能：  短延时函数--16M-2T--大概快1%左右
 *	输入：  Time延时时间长度 延时时长Time ms
 *	输出：  无 
 ----------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(197); 	    //快1%
		}
	}
}
