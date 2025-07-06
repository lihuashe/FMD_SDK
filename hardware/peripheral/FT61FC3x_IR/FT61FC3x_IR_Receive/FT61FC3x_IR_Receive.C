// Project: FT61FC3x_IR_Receive.prj
// Device:  FT61FC3x
// Memory:  PROM=3Kx14, SRAM=256, EEPROM=128                             
// Description: 演示程序中,IR红外是采用6122协议，起始信号是9ms低电平，到4.5ms高电平，再到低8位
//           	用户识别码，到高8位的用户识别码，8位数据码，8位数据码的反码。RXIO（PA4）每次收到
//          	遥控器发过来的数据后，数据是合法（两对补码，不对内容判断）的话，LED(PA2)开关状
//		    	态就改变一次。
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
//  NC------------|2(PC1)   (PA0)19|-----------AN0
//  NC------------|3(PC0)   (PA1)18|------------NC
//  NC------------|4(PB7)   (PA2)17|-----------LED
//  NC------------|5(PB6)   (PA3)16|------------NC
//  NC------------|6(PB5)   (PA4)15|---------IRRIO
//  NC------------|7(PB4)   (PA5)14|------------NC
//  NC------------|8(PB3)   (PA6)13|------------NC
//  NC------------|9(PB2)   (PA7)12|------------NC
//  NC------------|10(PB1)  (PB0)11|------------NC
//                 ----------------   
// 
//*********************************************************
#include	"SYSCFG.h"
//********************宏定义******************************
#define  uchar     unsigned char 

#define  IRRIO		PA0  	      //IR的接收脚
#define  LED		PA2		      //LED指示灯的IO

uchar IRbitNum = 0;		          //用于记录接收到第几位数据了
uchar IRbitTime = 0;		      //用于计时一位的时间长短
volatile uchar IRDataTimer[4];	  //存出来的4个数据
uchar bitdata=0x01;			      //用于按位或的位数据
uchar ReceiveFinish = 0;	      //用于记录接收完成
uchar ReadAPin = 0;			      //用于读取IO口状态，电平变化中断标志清除
volatile uchar rdata1,rdata2;
/*-------------------------------------------------
 *  函数名: POWER_INITIAL 
 *	功能：  MCU初始化函数
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void POWER_INITIAL(void)
{
	OSCCON = 0B01110001;	//IRCF=111=16MHz/4T=4MHz，0.25us
	INTCON = 0;  			//暂禁止所有中断
	PORTA  = 0B00000000;		
	TRISA  = 0B00000000;	//PA输入输出 0-输出 1-输入
							//PA2->输出
						
	PORTC  = 0B00000000; 	
	TRISC  = 0B00000000;	//PC输入输出 0-输出 1-输入  
								
	WPUA   = 0B00000000;    //PA端口上拉控制 1-开上拉 0-关上拉
	WPUC   = 0B00000000;    //PC端口上拉控制 1-开上拉 0-关上拉
                            
	OPTION = 0B00001000;	//Bit3=1 WDT，Bit[2:0]=000=WDT RATE 1:1
}
/*----------------------------------------------------
 *	函数名：TIMER0_INITIAL
 *	功能：  初始化设置定时器
 *	设置TMR0定时时长560us=(1/16000000)*4*16*140(16M-4T-PSA 1:16- TMR0=255溢出)	                      
 ----------------------------------------------------*/
void TIMER0_INITIAL (void)  
{
	OPTION = 0B00000011;    //Bit3=0 TMR0，Bit[2:0]=011=TMR0 RATE 1:16  
	TMR0 = 118; 
    T0IF = 0;				//清零T0软件中断
}
/*-------------------------------------------------
 *  函数名: PA_Level_Change_INITIAL
 *	功能：  PA端口(PA2)电平变化中断初始化
 *  输入：  无
 *  输出：  无
--------------------------------------------------*/
void PA_Level_Change_INITIAL(void)
{
	TRISA0 =1; 				//PA0 INPUT
	ReadAPin = PORTA;		//清PA电平变化中断
	PAIF =0;   				//清PA_INT中断标志位
    IOCA0 =1;  				//使能PA0电平变化中断
	PAIE =1;   				//使能PA_INT中断
}
/*-------------------------------------------------
 *  函数名：interrupt ISR
 *	功能：  中断处理，包括定时器0中断和外部中断
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void interrupt ISR(void)	
{ 
   
  //************定时器0的中断处理**********************
	if(T0IE && T0IF)		
	{
		TMR0 = 118;         //注意:对TMR0重新赋值TMR0在两个周期内不变化
		 
		T0IF = 0;
        IRbitTime++;
        if(IRbitTime > 50)
        {
        	T0IE = 0;
            IRbitTime = 0;
        }
	} 
    
    //*************PA电平变化中断**********************
	if(PAIE && PAIF)		
    {
		ReadAPin = PORTA; 	//读取PORTA数据清PAIF标志
		PAIF = 0; 
        if(IRRIO == 0)
        {
        	T0IE = 1;
        	if(IRbitTime > 21)
            {
            	IRDataTimer[0] = 0;
                IRDataTimer[1] = 0;
                IRDataTimer[2] = 0;
                IRDataTimer[3] = 0;
                IRbitNum = 0;
                bitdata = 0x00;
            }
            else if(IRbitTime > 3)
            {
            	IRDataTimer[IRbitNum-1] |= bitdata;
            }
            IRbitTime = 0;
            bitdata<<=1;
            if(bitdata == 0)
            {
            	bitdata = 0x01;
                IRbitNum++;
            }
            if(IRbitNum > 4)
            {
            	IRbitNum = 0;
                T0IE = 0;  
                ReceiveFinish = 1;		
            }
        }
	}
} 
/*-------------------------------------------------
 *  函数名： main
 *	功能：  主函数
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
