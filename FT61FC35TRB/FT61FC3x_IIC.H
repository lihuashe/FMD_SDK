#ifndef _FT61FC3x_IIC_H_
#define _FT61FC3x_IIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>

/*============================================================================*
 *                              Definitions
 *============================================================================*/


/*============================================================================*
 *                              Register Definitions
 *============================================================================*/


/*============================================================================*
 *                              Enumerations
 *============================================================================*/


/*============================================================================*
 *                              Data Structures
 *============================================================================*/


/*============================================================================*
 *                              Function Prototypes
 *============================================================================*/
void IIC_INITIAL(void);

void DelayUs(unsigned char Time);

unsigned char IIC_READ(unsigned char i2c_address, unsigned char reg);

void IIC_WRITE(unsigned char i2c_address, unsigned char reg,unsigned char data);

void IIC_READ_MULTI(unsigned char i2c_address, unsigned char reg, unsigned char *pData, unsigned int length);

void IIC_WRITE_MULTI(unsigned char i2c_address, unsigned char reg, unsigned char *pData, unsigned int length);

#ifdef __cplusplus
}
#endif

#endif /* _DA228EC_DRIVER_H_ */

/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/