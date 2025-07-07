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
#define  IIC_SDA	PB5

#define  SDA_OUT    TRISB5 =0
#define  SDA_IN	    TRISB5 =1

volatile unchar	IICReadData;
 /*-------------------------------------------------
 *  ��������IIC_INITIAL
 *	���ܣ�  �ϵ�ϵͳ��ʼ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void IIC_INITIAL(void) 
{				
	GPIO_INIT(GPIO_PA2, Out, NoPullUp);
	GPIO_INIT(GPIO_PA4, Out, NoPullUp);
}
/*-------------------------------------------------
 *  �������ƣ�DelayUs
 *  ���ܣ�    ����ʱ���� 
 *  ���룺    Time ��ʱʱ�䳤�� ��ʱʱ��Time*2us
 * 	�����    �� 
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
 *  ��������IIC_Start
 *	���ܣ�  ����IIC��ʼ�ź�
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void IIC_Start(void)
{
	SDA_OUT;              //SDA�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	DelayUs(2);
 	IIC_SDA=0;            //START:when CLK is high,DATA change form high to low 
	DelayUs(2);
	IIC_SCL=0;            //ǯסI2C���ߣ�׼�����ͻ�������� 
    DelayUs(2);
}	  
/*-------------------------------------------------
 *  ��������IIC_Stop
 *	���ܣ�  ����IICֹͣ�ź�
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
 void IIC_Stop(void)
{
	SDA_OUT;              //SDA�����
	IIC_SCL=0;
	IIC_SDA=0;            //STOP:when CLK is high DATA change form low to high
 	DelayUs(2);
	IIC_SCL=1; 
    DelayUs(2);
	IIC_SDA=1;            //����I2C���߽����ź�
	DelayUs(2);							   	
}
/*-------------------------------------------------
 *  ��������IIC_Wait_Ack
 *	���ܣ�  �ȴ�Ӧ���źŵ���
 *  ���룺  ��
 *  �����  ����ֵ��1������Ӧ��ʧ��
 *                  0������Ӧ��ɹ�
 --------------------------------------------------*/
unsigned char IIC_Wait_Ack(void)
{
	unsigned char ucErrTime=0;      
	SDA_IN;               //SDA����Ϊ����  
	IIC_SDA=1;
	DelayUs(1);	   
	IIC_SCL=1;
	DelayUs(1);	 
	while(IIC_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250) //�ȴ���ʱ
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;            //ʱ�����0 	   
	return 0;  
} 
/*-------------------------------------------------
 *  ��������IIC_Ack
 *	���ܣ�  ����ACKӦ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT;              //SDA�����
	IIC_SDA=0;
	DelayUs(1);	
	IIC_SCL=1;
	DelayUs(1);	
	IIC_SCL=0;
}
/*-------------------------------------------------
 *  ��������IIC_NAck
 *	���ܣ�  ������ACKӦ��
 *  ���룺  ��
 *  �����  ��
 --------------------------------------------------*/	    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT;              //SDA�����
	IIC_SDA=1;
	DelayUs(1);	
	IIC_SCL=1;
	DelayUs(1);	
	IIC_SCL=0;
}					 				     
/*-------------------------------------------------
 *  ��������IIC_Send_Byte
 *	���ܣ�  IIC����һ���ֽ�
 *  ���룺  д��Ҫ���͵�һ���ֽ�����txd
 *  �����  ��
 --------------------------------------------------*/		  
void IIC_Send_Byte(unsigned char txd)
{                        
    unsigned char t;   
	SDA_OUT;	          //SDA�����   
    IIC_SCL=0;            //����ʱ�ӿ�ʼ���ݴ���
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
 *  ��������IIC_Read_Byte
 *	���ܣ�  IIC��һ���ֽ�
 *  ���룺  ��
 *  �����  �����洢����������ݲ�����receive
 --------------------------------------------------*/
 unsigned char IIC_Read_Byte(void)
{
	unsigned char i,receive=0;
	SDA_IN;               //SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        DelayUs(1);	
     	IIC_SCL=1;
        receive<<=1;
        if(IIC_SDA)receive++;   
		DelayUs(1);	
    }					 
    IIC_NAck();           //����nACK
  
    return receive;
}
/*-------------------------------------------------
 *  ��������IIC_READ
 *	���ܣ�  IIC�����ƶ�λ�õ�����
 *  ���룺  address
 *  �����  ����address�洢�����������iicdata
 --------------------------------------------------*/
 unsigned char IIC_READ(unsigned char i2c_address, unsigned char reg)
{
	unsigned char iicdata = 0;
	IIC_READ_Begin:
		IIC_Start();
		IIC_Send_Byte(i2c_address);
		if(IIC_Wait_Ack())goto IIC_READ_Begin;
		IIC_Send_Byte(reg);				//��Ҫ�������ݵ�ַ
		if(IIC_Wait_Ack())goto IIC_READ_Begin; 
		IIC_Start();
		IIC_Send_Byte(i2c_address|1);
		if(IIC_Wait_Ack())goto IIC_READ_Begin;
		iicdata=IIC_Read_Byte();
		IIC_Stop();		
		return iicdata;
}
 /*-------------------------------------------------
 *  ��������IIC_WRITE
 *	���ܣ�  IIC������dataд���ƶ���λ��address
 *  ���룺  address��data
 *  �����  ��
 --------------------------------------------------*/
void IIC_WRITE(unsigned char i2c_address, unsigned char reg,unsigned char data)
	{
	IIC_WRITE_Begin:
		IIC_Start();
		IIC_Send_Byte(i2c_address);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Send_Byte(reg);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Send_Byte(data);
		if(IIC_Wait_Ack())goto IIC_WRITE_Begin;

		IIC_Stop();	
	}

/**
 * @brief IIC多字节写入
 *	@param i2c_address 起始写入地址
 * @param reg 起始写入地址
 * @param pData 数据缓冲区指针
 * @param length 写入字节数
 */
void IIC_READ_MULTI(unsigned char i2c_address, unsigned char reg, unsigned char *pData, unsigned int length)
{
    unsigned int i;

    for (;;)
    {
        IIC_Start();
        IIC_Send_Byte(i2c_address);  // 设备写地址，准备写入地址
        if (IIC_Wait_Ack() == 0)  // 0表示成功
        {
            IIC_Send_Byte(reg); // 起始地址
            if (IIC_Wait_Ack() == 0)
            {
                IIC_Start();
                IIC_Send_Byte(0xA1);  // 设备读地址
                if (IIC_Wait_Ack() == 0)
                {
                    // 读数据
                    for (i = 0; i < length; i++)
                    {
                        if (i == length - 1)
                            pData[i] = IIC_Read_Byte_NAck();  // 最后一个字节读后不应答
                        else
                            pData[i] = IIC_Read_Byte_Ack();   // 读字节后应答
                    }
                    IIC_Stop();
                    break;  // 读成功，跳出循环
                }
            }
        }
        IIC_Stop();
        // 失败则重试
    }
}





/**
 * @brief IIC多字节写入
 *	@param i2c_address 起始写入地址
 * @param reg 起始写入地址
 * @param pData 数据缓冲区指针
 * @param length 写入字节数
 */
void IIC_WRITE_MULTI(unsigned char i2c_address, unsigned char reg, unsigned char *pData, unsigned int length)
{
    unsigned int i;

    for (;;)
    {
        IIC_Start();
        IIC_Send_Byte(i2c_address);  // 设备写地址
        if (IIC_Wait_Ack() == 0)
        {
            IIC_Send_Byte(reg); // 起始地址
            if (IIC_Wait_Ack() == 0)
            {
                for (i = 0; i < length; i++)
                {
                    IIC_Send_Byte(pData[i]);
                    if (IIC_Wait_Ack() != 0)
                    {
                        IIC_Stop();
                        break;  // 这里用goto跳出for循环，后面会改成循环
                    }
                }
                IIC_Stop();
                break;  // 写成功，跳出循环
            }
        }
        IIC_Stop();
    }
}
