// Project: FT61FC3x_EEPROM.prj
// Device:  FT61FC3x
// Memory:  PROM=3Kx14, SRAM=256, EEPROM=128                             
// Description: 此演示程序为61fc3x ERROM的演示程序.
//		  		把0x55写入地址0x13,再读出该值
//
// RELEASE HISTORY
// VERSION DATE     DESCRIPTION
// 1.0	   24-1-16	初版
// 1.1	   24-1-24  更新版本号 
// 1.2     24-2-26  更新文件头描述
//
//                  FT61FC35  SOP20
//                 ----------------
//  VDD-----------|1(VDD)   (VSS)20|-----------VSS     
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
//===========================================================
#include	"SYSCFG.h"
//===========================================================
//Variable definition
//===========================================================
#define 	unchar     	unsigned char 
unchar EEReadData=0;
/*-------------------------------------------------
 *  函数名：POWER_INITIAL
 *	功能：  上电系统初始化
 *  输入：
 *  输出：  无
 --------------------------------------------------*/	
void POWER_INITIAL (void) 
{ 
	OSCCON = 0B01110001;	//IRCF=111=16MHz/2T=8MHz,0.125us
	INTCON = 0;  			//暂禁止所有中断
    
	PORTA  = 0B00000000;		
	TRISA  = 0B00000000;	//PA输入输出 0-输出 1-输入					
	PORTC  = 0B00000000; 	
	TRISC  = 0B00000000;	//PC输入输出 0-输出 1-输入  
								
	WPUA   = 0B00000000;    //PA端口上拉控制 1-开上拉 0-关上拉					
	WPUC   = 0B00000000;    //PC端口上拉控制 1-开上拉 0-关上拉
                            
	OPTION = 0B00001000;	//Bit3=1 WDT MODE
                            //Bit[2:0]=000 WDT RATE 1:1 
}
//===========================================================
//函数名称：interrupt ISR
//输入：    无
//输出：    无
//===========================================================
void interrupt ISR(void)
{
    
}
/*----------------------------------------------------
 *	函数名称：EEPROMread
 *	功能：    读EEPROM数据
 *	输入：    EEAddr 需读取数据的地址
 *  输出：    ReEEPROMread 对应地址读出的数据
 ----------------------------------------------------*/
unchar EEPROMread(unchar EEAddr)
{
	unchar ReEEPROMread;

	EEADR = EEAddr;    
	RD = 1;
    NOP();
    NOP();   
	ReEEPROMread = EEDAT;     //EEPROM的读数据 ReEEPROMread = EEDATA;
	return ReEEPROMread;
}
/*---------------------------------------------------- 
 *	函数名称：EEPROMwrite
 *	功能：    写数据到EEPROM
 *	输入：    EEAddr 需要写入数据的地址
 *	          Data 需要写入的数据
 *	输出：    无
 ----------------------------------------------------*/
void EEPROMwrite(unchar EEAddr,unchar Data)
{
	while(GIE) 					//等待GIE为0
    {
		GIE = 0;
        NOP();
        NOP();
	}
	EEADR = EEAddr; 	 	  //EEPROM的地址
	EEDAT = Data;		 	  //EEPROM的写数据  EEDATA = Data
	EEIF = 0;                 //清0中断标志位
	EECON1 |= 0x34;			  //置位WREN1，WREN2，WREN3三个变量
	WR = 1;					  //置位WR启动编程
	while(WR);      		  //等待EE写入完成
    GIE = 1;
}

//===========================================================
