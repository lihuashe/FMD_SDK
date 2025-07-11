#include "syscfg.h"
#include <stdint.h>

#define  IIC_SCL	PA6   
#define  IIC_SDA	PA7

#define  SDA_OUT    TRISA7 =0
#define  SDA_IN	    TRISA7 =1

/*-------------------------------------------------
 *  函数名称：DelayUs
 *  功能：    短延时函数 
 *  输入：    Time 延时时间长度 延时时长Time*2us
 * 	输出：    无 
 -------------------------------------------------*/
// void DelayUs(unsigned char Time)
// {
// 	unsigned char a;
// 	for(a=0;a<Time;a++)
// 	{
// 		NOP();
// 	}
// } 
extern void delya_2us(uint8_t us);
#define DelayUs	delya_2us

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
		NOP();
    }					 
  
    return receive;
}