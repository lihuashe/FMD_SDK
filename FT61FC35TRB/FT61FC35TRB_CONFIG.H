/**
**********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     key_driver.h
* @brief    按键驱动
* @details  本文件提供了按键的驱动接口，包括初始化、数据读取、配置等功能
* @author   lihuashe
* @date     2025-7-20
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _FT61FC35TRB_CONFIG_H_
#define _FT61FC35TRB_CONFIG_H_

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

#define APP_XIHAO_FCTORY_LOG_LEVEL								0
#define APP_AUDIO_LOG_LEVEL										3
#define APP_MODULE_LOG_LEVEL                                    3
#endif

#define CONFIG_INTERNAL_DCDC_ENABLE				1
#define MODE_DEEP_SLEEP_NO_RESET_MCU			1
#define REMOTE_LED_ENABLE						1
#define REMOTE_IR_ENABLE						1
#define REMOTE_BATT_ENABLE						1
#define MTU_REQUEST_ENABLE						1
#define FACTORY_SIGNAL_ENABLE					1
#define IDENTITY_ADDRS_RESOLV_EN				1
#define XIHAO_FUN_ENABLE						1
#define APP_SEC_NVDS_SUPPORT 					1
#define SLEEP_FLASH_OFF_EN						1
#define REMOTE_64M_ENABLE						0
#define COMB_KEY_SEND_CODE_EN					1	//组合键发码
#define AUTO_PAIR_ENABLE						1

//功能模块使能
#define BACKLIGHT_ENABLE						1	//led_module
#define KT0646M_ENABLE					        1	//kt_module



#define VERSION_STR		"1.0.0" //1:主；1:次;  0:release;1~9:debug
/*============================================================================*
 *                              Enumerations
 *============================================================================*/

/*============================================================================*
 *                              Data Structures
 *============================================================================*/


/*============================================================================*
 *                              Function Prototypes
 *============================================================================*/



#ifdef __cplusplus
}
#endif

#endif /* _DA228EC_DRIVER_H_ */

/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/