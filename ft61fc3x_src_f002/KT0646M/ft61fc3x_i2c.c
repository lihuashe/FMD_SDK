#include "syscfg.h"
#include <stdint.h>

#define  IIC_SCL	PA6   
#define  IIC_SDA	PA7

#define  SDA_OUT    TRISA7 =0
#define  SDA_IN	    TRISA7 =1

/*-------------------------------------------------
 *  �������ƣ�DelayUs
 *  ���ܣ�    ����ʱ���� 
 *  ���룺    Time ��ʱʱ�䳤�� ��ʱʱ��Time*2us
 * 	�����    �� 
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
		NOP();
    }					 
  
    return receive;
}