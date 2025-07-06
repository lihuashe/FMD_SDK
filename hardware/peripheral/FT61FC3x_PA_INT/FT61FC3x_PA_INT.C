// Project: FT61FC3x_PA_INT.prj
// Device:  FT61FC3x
// Memory:  PROM=3Kx14, SRAM=256, EEPROM=128                             
// Description: 程序中DemoPortOut(PA4)输出100帧50Hz的占空比为50%的方波后，
//           	MCU进入睡眠，等待中断的发生；
//		    	当每次PA2电平变化中断触发后，重复以上流程;
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
//  NC------------|4(PB7)   (PA2)17|----DemoPortIn
//  NC------------|5(PB6)   (PA3)16|------------NC
//  NC------------|6(PB5)   (PA4)15|---DemoPortOut
//  NC------------|7(PB4)   (PA5)14|------------NC
//  NC------------|8(PB3)   (PA6)13|------------NC
//  NC------------|9(PB2)   (PA7)12|------------NC
//  NC------------|10(PB1)  (PB0)11|------------NC
//                 ----------------   
// 
//=========================================================
#include "SYSCFG.h"
//=========================================================
#define  unchar     unsigned char 

#define  DemoPortOut	PA4
#define  DemoPortIn 	PA2  
 
unchar FCount;
unchar ReadAPin;
/*-------------------------------------------------
 *  函数名：interrupt ISR
 *	功能：  中断处理函数
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void interrupt ISR(void)
{ 
	 if(PAIE && PAIF)
    {
		ReadAPin = PORTA; 	//读取PORTA数据清PAIF标志
		PAIF = 0;  			//清PAIF标志位
		PAIE = 0;  			//暂先禁止PA0中断
		IOCA2 =0;  			//禁止PA0电平变化中断
		 
    }
} 
/*----------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 ----------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
	OSCCON = 0B01110001;	//IRCF=111=16MHz/2T=8MHz,0.125us			 		
	INTCON = 0;  			//暂禁止所有中断
	PORTA  = 0B00000000;		
	TRISA  = 0B00000100;	//PA输入输出 0-输出 1-输入
							//PA2-IN  PA4-OUT			
	PORTC  = 0B00000000; 	
	TRISC  = 0B00000000;	//PC输入输出 0-输出 1-输入  
								
	WPUA   = 0B00000100;    //PA端口上拉控制 1-开上拉 0-关上拉
							//开PA2上拉
	WPUC   = 0B00000000;    //PC端口上拉控制 1-开上拉 0-关上
                            
	OPTION = 0B00001000;	//Bit3=1 WDT，Bit[2:0]=000 WDT RATE=1:1					 		
}
/*----------------------------------------------------
 *	函数名：  DelayUs
 *	功能：    短延时函数
 *	输入：    Time 延时时间长度 延时时长Time us
 *	返回：    无 
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
 *	函数名：  DelayMs
 *	功能：    短延时函数--16M-2T--大概快1%左右
 *	输入：    Time 延时时间长度 延时时长Time ms
 *	返回：    无 
 ----------------------------------------------------*/
void DelayMs(unsigned char Time)
{
	unsigned char a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
		 	DelayUs(197); 	 //快1%
		}
	}
}
/*-------------------------------------------------
 *  函数名: PA2_Level_Change_INITIAL
 *	功能：  PA端口(PA2)电平变化中断初始化
 *  输入：  无
 *  输出：  无
--------------------------------------------------*/
void PA2_INITIAL(void)
{
 
	TRISA2 =1; 			     //设置 PA2 输入
	ReadAPin = PORTA;	     //清PA电平变化中断
	PAIF =0;   			     //清PA INT中断标志位
    IOCA2 =1;  			     //使能PA2电平变化中断
	PAIE =1;   			     //使能PA INT中断
}
/*-------------------------------------------------
 *  函数名: main 
 *	功能：  主函数
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void main()
{
	POWER_INITIAL();		 //系统初始化
 
	while(1)
	{
		for(FCount=0;FCount<100;FCount++)	
		{                    //输出100次波形
			DemoPortOut = 1; 				
			DelayMs(10);  	 //10ms 
			DemoPortOut = 0;
			DelayMs(10); 
		}
		PA2_INITIAL();		 //初始化外部中断
		GIE = 1;			 //开总中断
       
		SLEEP(); 			 //睡眠
	}
}