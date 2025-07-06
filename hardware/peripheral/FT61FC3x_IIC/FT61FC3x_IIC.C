// Project: FT61FC3x_IIC.prj
// Device:  FT61FC3x
// Memory:  PROM=3Kx14, SRAM=256, EEPROM=128                             
// Description: 此演示程序位FT61FC3x_IIC的演示程序.
//		    	该程序把0x55写入(24C02)0x12地址,后读0x12地址的值，判断是否写入成功 
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
//  NC------------|4(PB7)   (PA2)17|-----------SDA
//  NC------------|5(PB6)   (PA3)16|------------NC
//  NC------------|6(PB5)   (PA4)15|-----------SCL
//  NC------------|7(PB4)   (PA5)14|------------NC
//  NC------------|8(PB3)   (PA6)13|------------NC
//  NC------------|9(PB2)   (PA7)12|------------NC
//  NC------------|10(PB1)  (PB0)11|------------NC
//                 ----------------   
// 
//*********************************************************
#include "SYSCFG.h"
#include "FT61FC3x_GPIO_INIT.H"
//*********************************************************
#define  unchar     unsigned char 

#define  IIC_SCL	PA4   
#define  IIC_SDA	PA2

#define  SDA_OUT    TRISA2 =0
#define  SDA_IN	    TRISA2 =1

volatile unchar	IICReadData;
 /*-------------------------------------------------
 *  函数名：IIC_INITIAL
 *	功能：  上电系统初始化
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void IIC_INITIAL(void) 
{				
	GPIO_INIT(GPIO_PA2, Out, NoPullUp);
	GPIO_INIT(GPIO_PA4, Out, NoPullUp);
}
/*-------------------------------------------------
 *  函数名称：DelayUs
 *  功能：    短延时函数 
 *  输入：    Time 延时时间长度 延时时长Time*2us
 * 	输出：    无 
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
 *  函数名：IIC_Start
 *	功能：  产生IIC起始信号
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void IIC_Start(void)
{
	SDA_OUT;              //SDA线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	DelayUs(2);
 	IIC_SDA=0;            //START:when CLK is high,DATA change form high to low 
	DelayUs(2);
	IIC_SCL=0;            //钳住I2C总线，准备发送或接收数据 
    DelayUs(2);
}	  
/*-------------------------------------------------
 *  函数名：IIC_Stop
 *	功能：  产生IIC停止信号
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
 void IIC_Stop(void)
{
	SDA_OUT;              //SDA线输出
	IIC_SCL=0;
	IIC_SDA=0;            //STOP:when CLK is high DATA change form low to high
 	DelayUs(2);
	IIC_SCL=1; 
    DelayUs(2);
	IIC_SDA=1;            //发送I2C总线结束信号
	DelayUs(2);							   	
}
/*-------------------------------------------------
 *  函数名：IIC_Wait_Ack
 *	功能：  等待应答信号到来
 *  输入：  无
 *  输出：  返回值：1，接收应答失败
 *                  0，接收应答成功
 --------------------------------------------------*/
unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;      
	SDA_IN;               //SDA设置为输入  
	IIC_SDA=1;
	DelayUs(1);	   
	IIC_SCL=1;
	DelayUs(1);	 
	while(IIC_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250) //等待超时
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;            //时钟输出0 	   
	return 0;  
} 
/*-------------------------------------------------
 *  函数名：IIC_Ack
 *	功能：  产生ACK应答
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT;              //SDA线输出
	IIC_SDA=0;
	DelayUs(1);	
	IIC_SCL=1;
	DelayUs(1);	
	IIC_SCL=0;
}
/*-------------------------------------------------
 *  函数名：IIC_NAck
 *	功能：  不产生ACK应答
 *  输入：  无
 *  输出：  无
 --------------------------------------------------*/	    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT;              //SDA线输出
	IIC_SDA=1;
	DelayUs(1);	
	IIC_SCL=1;
	DelayUs(1);	
	IIC_SCL=0;
}					 				     
/*-------------------------------------------------
 *  函数名：IIC_Send_Byte
 *	功能：  IIC发送一个字节
 *  输入：  写入要发送的一个字节数据txd
 *  输出：  无
 --------------------------------------------------*/		  
void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT;	          //SDA线输出   
    IIC_SCL=0;            //拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		if(txd&0x80)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		DelayUs(1);				  
		IIC_SCL=1;
		DelayUs(1);	
		IIC_SCL=0;	
		DelayUs(1);
    }	 
} 	    
/*-------------------------------------------------
 *  函数名：IIC_Read_Byte
 *	功能：  IIC读一个字节
 *  输入：  无
 *  输出：  读出存储器里面的数据并返回receive
 --------------------------------------------------*/
 unsigned char IIC_Read_Byte(void)
{
	unsigned char i,receive=0;
	SDA_IN;               //SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        DelayUs(1);	
     	IIC_SCL=1;
        receive<<=1;
        if(IIC_SDA)receive++;   
		DelayUs(1);	
    }					 
    IIC_NAck();           //发送nACK
  
    return receive;
}
/*-------------------------------------------------
 *  函数名：IIC_READ
 *	功能：  IIC读出制定位置的数据
 *  输入：  address
 *  输出：  读出address存储器里面的数据iicdata
 --------------------------------------------------*/
 unsigned char IIC_READ(unsigned char address)
	{
	unsigned char iicdata = 0;
	IIC_READ_Begin:
		IIC_Start();
		IIC_Send_Byte(0xa0);
		if(IIC_Wait_Ack())goto IIC_READ_Begin;
		IIC_Send_Byte(address);				//填要读的数据地址
		if(IIC_Wait_Ack())goto IIC_READ_Begin; 
		IIC_Start();
		IIC_Send_Byte(0xa1);
		if(IIC_Wait_Ack())goto IIC_READ_Begin;
		iicdata=IIC_Read_Byte();
		IIC_Stop();		
		return iicdata;
	}
 /*-------------------------------------------------
 *  函数名：IIC_WRITE
 *	功能：  IIC把数据data写入制定的位置address
 *  输入：  address，data
 *  输出：  无
 --------------------------------------------------*/
void IIC_WRITE(unsigned char address,unsigned char data)
	{
	IIC_WRITE_Begin:
		IIC_Start();
		IIC_Send_Byte(0xa0);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Send_Byte(address);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Send_Byte(data);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Stop();	
	}

