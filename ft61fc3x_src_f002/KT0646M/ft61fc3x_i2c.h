#ifndef __FT61FC3X_I2C_H
#define __FT61FC3X_I2C_H

#include "SYSCFG.h"
#include <stdint.h>

void DelayUs(unsigned char Time);

void IIC_Start(void);

void IIC_Stop(void);

unsigned char IIC_Wait_Ack(void);

void IIC_Ack(void);

void IIC_NAck(void);

void IIC_Send_Byte(unsigned char txd);

unsigned char IIC_Read_Byte(void);

#endif

