/**
**********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     da228ec_driver.c
* @brief    DA228EC加速度传感器驱动实现
* @details  本文件实现了DA228EC加速度传感器的驱动功能，包括初始化、数据读取、配置等
* @author   lihuashe
* @date     2025-6-23
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "key_driver.h"
#include "FT61FC3x_GPIO_INIT.H"
#include "SYSCFG.h"

#define POWER_KEY           PA3
#define VOLUE_SUB_KEY       PA4
#define VOLUE_APP_KEY       PA5


void key_init(void)
{
    GPIO_INIT(GPIO_PA2, In, PullUp);
	GPIO_INIT(GPIO_PA3, In, PullUp);
    GPIO_INIT(GPIO_PA4, In, PullUp);
}

unsigned char key_scan(void)
{
    if (!POWER_KEY)
        return POWER_KEY_VALUE;

    if (!VOLUE_SUB_KEY)
        return VOLUE_SUB_KEY_VALUE;

    if (!VOLUE_SUB_KEY)
        return VOLUE_SUB_KEY_VALUE;

}




/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/