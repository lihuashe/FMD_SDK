// Project: FT61FC3x_IIC.prj
// Device:  FT61FC3x
// Memory:  PROM=3Kx14, SRAM=256, EEPROM=128                             
// Description: ����ʾ����λFT61FC3x_IIC����ʾ����.
//		    	�ó����0x55д��(24C02)0x12��ַ,���0x12��ַ��ֵ���ж��Ƿ�д��ɹ� 
//
// RELEASE HISTORY
// VERSION DATE     DESCRIPTION
// 1.0	   24-1-16	����
// 1.1	   24-1-24  ���°汾�� 
// 1.2     24-2-26  �����ļ�ͷ����
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
#include "FT61FC3x_IIC.H"
//*********************************************************
#define  unchar     unsigned char 

#define  IIC_SCL	PB2  
#define  IIC_SDA	PA4

#define  SDA_OUT    TRISA4 =0
#define  SDA_IN	    TRISA4 =1

volatile unchar	IICReadData;
 /*-------------------------------------------------
 *  ��������IIC_INITIAL
 *	���ܣ�  �ϵ�ϵͳ��ʼ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void IIC_INITIAL(void) 
{	
	OSCCON = 0B01110001;	//IRCF=111=16MHz/4T=4MHz,0.25us
    //OPTION = 0B00001000;    //Bit3=1 WDT，Bit[2:0]=000=WDT RATE 1:1				 		
	INTCON = 0;  			//暂禁止所有中断
	PORTA  = 0B00000000;		
	TRISA  = 0B00000000;	//PA输入输出 0-输出 1-输入
							//PA4->输出
	PORTB  = 0B00000000;		
	TRISB  = 0B00000000;	//PA输入输出 0-输出 1-输入
							//PA4->输出
	PORTC  = 0B00000000; 	
	TRISC  = 0B00000000;	//PC输入输出 0-输出 1-输入  
								
	WPUA   = 0B00000000;    //PA端口上拉控制 1-开上拉 0-关上拉	
	WPUB   = 0B00000000;    //PA端口上拉控制 1-开上拉 0-关上拉							
	WPUC   = 0B00000000;    //PC端口上拉控制 1-开上拉 0-关上拉

}

/*----------------------------------------------------
 *	函数名：DelayMs
 *	功能：  短延时函数--16M-2T--大概快1%左右.
 *	输入：  Time延时时间长度 延时时长Time ms
 *	返回：  无 
 ----------------------------------------------------*/
void DelayMs(unsigned short Time)
{
	unsigned short a,b;
	for(a=0;a<Time;a++)
	{
		for(b=0;b<5;b++)
		{
			DelayUs(197); 	//快1%
		}
	}
}

/*-------------------------------------------------
 *  微秒级延时
 *  功能：   延时函数
 *  输入：   Time 延时时间（单位：~2us）
 *  输出：   无 
 -------------------------------------------------*/
void DelayUs(unsigned char Time)
{
    for(unsigned char a = 0; a < Time; a++)
    {
        NOP(); // 使用内联汇编确保NOP指令
    }
} 

/*-------------------------------------------------
 *  IIC起始信号
 *  功能： 发送IIC起始信号
 *  输入： 无
 *  输出： 无
 --------------------------------------------------*/
void IIC_Start(void)
{
    SDA_OUT;              // SDA设置为输出
    IIC_SDA = 1;	  	  
    IIC_SCL = 1;
    DelayUs(2);
    IIC_SDA = 0;          // START: SCL高电平时SDA从高变低
    DelayUs(2);
    IIC_SCL = 0;          // 钳住总线，准备发送数据
    DelayUs(2);
}	  

/*-------------------------------------------------
 *  IIC停止信号
 *  功能： 发送IIC停止信号
 *  输入： 无
 *  输出： 无
 --------------------------------------------------*/
void IIC_Stop(void)
{
    SDA_OUT;              // SDA设置为输出
    IIC_SCL = 0;
    IIC_SDA = 0;          
    DelayUs(2);
    IIC_SCL = 1; 
    DelayUs(2);
    IIC_SDA = 1;          // STOP: SCL高电平时SDA从低变高
    DelayUs(2);							   	
}

/*-------------------------------------------------
 *  等待ACK响应
 *  功能： 等待从机ACK响应
 *  输入： 无
 *  输出： 1-ACK失败，0-ACK成功
 --------------------------------------------------*/
unsigned char IIC_Wait_Ack(void)
{
    unsigned char ucErrTime = 0;      
    SDA_IN;               // SDA设置为输入  
    IIC_SDA = 1;          // 释放SDA线
    DelayUs(1);	   
    IIC_SCL = 1;          // 拉高SCL
    DelayUs(1);	 
    
    // 检测SDA是否为低电平（ACK信号）
    while(IIC_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250) // 超时等待
        {
            IIC_Stop();
            return 1;      // ACK失败
        }
    }
    IIC_SCL = 0;          // SCL拉低
    return 0;             // ACK成功
} 

/*-------------------------------------------------
 *  发送ACK响应
 *  功能： 主机发送ACK信号
 *  输入： 无
 *  输出： 无
 --------------------------------------------------*/
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT;              // SDA设置为输出
    IIC_SDA = 0;          // SDA低电平表示ACK
    DelayUs(1);	
    IIC_SCL = 1;
    DelayUs(1);	
    IIC_SCL = 0;
}

/*-------------------------------------------------
 *  发送NACK响应
 *  功能： 主机发送NACK信号
 *  输入： 无
 *  输出： 无
 --------------------------------------------------*/	    
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT;              // SDA设置为输出
    IIC_SDA = 1;          // SDA高电平表示NACK
    DelayUs(1);	
    IIC_SCL = 1;
    DelayUs(1);	
    IIC_SCL = 0;
}					 				     

/*-------------------------------------------------
 *  发送一个字节
 *  功能： IIC发送一个字节
 *  输入： txd - 要发送的字节
 *  输出： 无
 --------------------------------------------------*/		  
void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
    SDA_OUT;	          // SDA设置为输出   
    IIC_SCL = 0;          // 拉低SCL开始数据传输
    
    for(t = 0; t < 8; t++)
    {              
        // 从高位开始发送
        if(txd & 0x80) {
            IIC_SDA = 1;
        } else {
            IIC_SDA = 0;
        }
        txd <<= 1;        // 准备发送下一位
        
        DelayUs(1);				  
        IIC_SCL = 1;      // 拉高SCL，从机读取数据
        DelayUs(1);	
        IIC_SCL = 0;      // 拉低SCL，准备发送下一位
        DelayUs(1);
    }	 
} 	    

/*-------------------------------------------------
 *  读取一个字节（发送NACK）
 *  功能： IIC读取一个字节（读取后发送NACK）
 *  输入： 无
 *  输出： 读取的字节
 --------------------------------------------------*/
unsigned char IIC_Read_Byte(void)
{
    unsigned char i, receive = 0;
    SDA_IN;               // SDA设置为输入
    
    for(i = 0; i < 8; i++)
    {
        IIC_SCL = 0; 
        DelayUs(1);	
        IIC_SCL = 1;      // 拉高SCL，主机读取数据
        
        receive <<= 1;    // 左移准备接收新位
        if(IIC_SDA) {
            receive |= 0x01;
        }
        DelayUs(1);	
    }					 
    IIC_NAck();           // 发送NACK
    return receive;
}

/*-------------------------------------------------
 *  读取一个字节（发送ACK）
 *  功能： IIC读取一个字节（读取后发送ACK）
 *  输入： 无
 *  输出： 读取的字节
 --------------------------------------------------*/
unsigned char IIC_Read_Byte_Ack(void)
{
    unsigned char i, receive = 0;
    SDA_IN;               // SDA设置为输入
    
    for(i = 0; i < 8; i++)
    {
        IIC_SCL = 0; 
        DelayUs(1);	
        IIC_SCL = 1;      // 拉高SCL，主机读取数据
        
        receive <<= 1;    // 左移准备接收新位
        if(IIC_SDA) {
            receive |= 0x01;
        }
        DelayUs(1);	
    }					 
    IIC_Ack();            // 发送ACK
    return receive;
}

/*-------------------------------------------------
 *  IIC读取单个字节
 *  功能： 从指定设备地址和寄存器地址读取一个字节
 *  输入： i2c_address - 设备地址
 *         reg - 寄存器地址
 *  输出： 读取的字节
 --------------------------------------------------*/
unsigned char IIC_READ(unsigned char i2c_address, unsigned char reg)
{
    unsigned char iicdata = 0;
    
    IIC_READ_Begin:
        IIC_Start();
        IIC_Send_Byte(i2c_address);     // 发送设备地址（写模式）
        if(IIC_Wait_Ack()) goto IIC_READ_Begin;
        
        IIC_Send_Byte(reg);             // 发送寄存器地址
        if(IIC_Wait_Ack()) goto IIC_READ_Begin; 
        
        IIC_Start();
        IIC_Send_Byte(i2c_address | 1); // 发送设备地址（读模式）
        if(IIC_Wait_Ack()) goto IIC_READ_Begin;
        
        iicdata = IIC_Read_Byte();      // 读取数据（发送NACK）
        IIC_Stop();		
        return iicdata;
}

/*-------------------------------------------------
 *  IIC写入单个字节
 *  功能： 向指定设备地址和寄存器地址写入一个字节
 *  输入： i2c_address - 设备地址
 *         reg - 寄存器地址
 *         data - 要写入的数据
 *  输出： 无
 --------------------------------------------------*/
void IIC_WRITE(unsigned char i2c_address, unsigned char reg, unsigned char data)
{
    IIC_WRITE_Begin:
        IIC_Start();
        IIC_Send_Byte(i2c_address);     // 发送设备地址（写模式）
        if(IIC_Wait_Ack()) goto IIC_WRITE_Begin;

        IIC_Send_Byte(reg);             // 发送寄存器地址
        if(IIC_Wait_Ack()) goto IIC_WRITE_Begin;

        IIC_Send_Byte(data);            // 发送数据
        if(IIC_Wait_Ack()) goto IIC_WRITE_Begin;

        IIC_Stop();	
}

/*-------------------------------------------------
 *  IIC多字节读取
 *  功能： 从指定设备地址和寄存器地址读取多个字节
 *  输入： i2c_address - 设备地址
 *         reg - 寄存器起始地址
 *         pData - 数据缓冲区指针
 *         length - 要读取的字节数
 *  输出： 无
 --------------------------------------------------*/
void IIC_READ_MULTI(unsigned char i2c_address, unsigned char reg, 
                   unsigned char *pData, unsigned int length)
{
    unsigned int i;
    
    IIC_READ_MULTI_Begin:
        IIC_Start();
        IIC_Send_Byte(i2c_address);     // 发送设备地址（写模式）
        if(IIC_Wait_Ack()) {
			IIC_Stop();
            goto IIC_READ_MULTI_Begin;
        }
        
        IIC_Send_Byte(reg);             // 发送寄存器地址
        if(IIC_Wait_Ack()) {
			IIC_Stop();
            goto IIC_READ_MULTI_Begin;
        }
        
        IIC_Start();
        IIC_Send_Byte(i2c_address | 1); // 发送设备地址（读模式）
        if(IIC_Wait_Ack()) {
			IIC_Stop();
            goto IIC_READ_MULTI_Begin;
        }
        
        // 读取数据
        for(i = 0; i < length; i++) {
            if(i == length - 1) {
                // 最后一个字节，读取后发送NACK
                pData[i] = IIC_Read_Byte();
            } else {
                // 非最后一个字节，读取后发送ACK
                pData[i] = IIC_Read_Byte_Ack();
            }
        }
        
        IIC_Stop();
}

/*-------------------------------------------------
 *  IIC多字节写入
 *  功能： 向指定设备地址和寄存器地址写入多个字节
 *  输入： i2c_address - 设备地址
 *         reg - 寄存器起始地址
 *         pData - 数据缓冲区指针
 *         length - 要写入的字节数
 *  输出： 无
 --------------------------------------------------*/
void IIC_WRITE_MULTI(unsigned char i2c_address, unsigned char reg, 
                    unsigned char *pData, unsigned int length)
{
    unsigned int i;
    
    IIC_WRITE_MULTI_Begin:
        IIC_Start();
        IIC_Send_Byte(i2c_address);     // 发送设备地址（写模式）
        if(IIC_Wait_Ack()) {
			IIC_Stop();
            goto IIC_WRITE_MULTI_Begin;
        }
        
        IIC_Send_Byte(reg);             // 发送寄存器地址
        if(IIC_Wait_Ack()) {
			IIC_Stop();
            goto IIC_WRITE_MULTI_Begin;
        }
        
        // 写入数据
        for(i = 0; i < length; i++) {
            IIC_Send_Byte(pData[i]);
            if(IIC_Wait_Ack()) {
				IIC_Stop();
                goto IIC_WRITE_MULTI_Begin;
            }
        }
        
        IIC_Stop();
}