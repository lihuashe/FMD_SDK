#ifndef _FT61FC3x_GPIO_INIT_H_
#define _FT61FC3x_GPIO_INIT_H_

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
#define  GPIO_PORT(x)   GPIO_##x

/*============================================================================*
 *                              Register Definitions
 *============================================================================*/


/*============================================================================*
 *                              Enumerations
 *============================================================================*/
typedef enum {
    In,
    Out
} Gpio_Direction_t;

typedef enum {
    PullUp,
    NoPullUp
} Gpio_PullUp_Down_t;

typedef enum {
    GPIO_PA0, GPIO_PA1, GPIO_PA2, GPIO_PA3, GPIO_PA4, GPIO_PA5, GPIO_PA6, GPIO_PA7,
    GPIO_PB0, GPIO_PB1, GPIO_PB2, GPIO_PB3, GPIO_PB4, GPIO_PB5, GPIO_PB6, GPIO_PB7,
    GPIO_PC0, GPIO_PC1, GPIO_PC2, GPIO_PC3, GPIO_PC4, GPIO_PC5, GPIO_PC6, GPIO_PC7,
}Gpio_Pin_t;
/*============================================================================*
 *                              Data Structures
 *============================================================================*/


/*============================================================================*
 *                              Function Prototypes
 *============================================================================*/

void GPIO_CLOCK_INIT(void);

void GPIO_INIT(Gpio_Pin_t gpio_x, Gpio_Direction_t out_in, Gpio_PullUp_Down_t pull_dir);

#ifdef __cplusplus
}
#endif

#endif /* _DA228EC_DRIVER_H_ */

/******************* (C) COPYRIGHT 2024 Sensasy Corporation *****END OF FILE****/