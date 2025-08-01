/**
 * @file gpio.h
 * @brief
 * @author @zh-jieli.com
 * @version 1.0.0
 * @date 2018-10-11
 */
#ifndef  __GPIO_H__
#define  __GPIO_H__
#include "typedef.h"

typedef enum {
    CH0_CH0_PWM = 0,
    CH0_CH1_PWM,
    CH0_T2_PWM_OUT0,
    CH0_T2_PWM_OUT1,
    CH0_SPI0_DO,
    CH0_UT1_TX,
    CH0_IN_CH3_SEL,
    CH0_PLL24M,

    CH1_CH2_PWM = 8,
    CH1_CH3_PWM,
    CH1_T2_PWM_OUT0,
    CH1_T2_PWM_OUT1,
    CH1_UT0_TX,
    CH1_UT1_TX,
    CH1_CLKOUT,
    CH1_PLL24M,
    CH2_CH0_PWM = 16,
    CH2_CH1_PWM,
    CH2_T2_PWM_OUT0,
    CH2_T2_PWM_OUT1,
    CH2_SPI0_DO,
    CH2_UT1_TX,
    CH2_IN_CH3_SEL,
    CH2_PLL24M,
    CH3_CH2_PWM = 24,
    CH3_CH3_PWM,
    CH3_T2_PWM_OUT0,
    CH3_T2_PWM_OUT1,
    CH3_UT0_TX,
    CH3_UT1_TX,
    CH3_CLKOUT,
    CH3_PLL24M,
} OUTCH_SEL_T;

typedef enum {
    DACN_OUTCH0 = 0,
    DACN_OUTCH1 = 1,
    DACN_OUTIO = 2,
    DACP_OUTCH0 = 3,
    DACP_OUTCH1 = 4,
    DACP_OUTIO = 5,
} DACX_MODE_SEL;

#define IO_GROUP_NUM 		16

#define IO_PORTA_00 				(IO_GROUP_NUM * 0 + 0)
#define IO_PORTA_01 				(IO_GROUP_NUM * 0 + 1)
#define IO_PORTA_02 				(IO_GROUP_NUM * 0 + 2)
#define IO_PORTA_03 				(IO_GROUP_NUM * 0 + 3)
#define IO_PORTA_04 				(IO_GROUP_NUM * 0 + 4)
#define IO_PORTA_05 				(IO_GROUP_NUM * 0 + 5)
#define IO_PORTA_06 				(IO_GROUP_NUM * 0 + 6)
#define IO_PORTA_07 				(IO_GROUP_NUM * 0 + 7)
#define IO_PORTA_08 				(IO_GROUP_NUM * 0 + 8)
#define IO_PORTA_09 				(IO_GROUP_NUM * 0 + 9)
#define IO_PORTA_10 				(IO_GROUP_NUM * 0 + 10)
#define IO_PORTA_11 				(IO_GROUP_NUM * 0 + 11)
#define IO_PORTA_12 				(IO_GROUP_NUM * 0 + 12)
#define IO_PORTA_13 				(IO_GROUP_NUM * 0 + 13)
#define IO_PORTA_14 				(IO_GROUP_NUM * 0 + 14)
#define IO_PORTA_15 				(IO_GROUP_NUM * 0 + 15)

#define IO_PORTB_00 				(IO_GROUP_NUM * 1 + 0)
#define IO_PORTB_01 				(IO_GROUP_NUM * 1 + 1)
#define IO_PORTB_02 				(IO_GROUP_NUM * 1 + 2)
#define IO_PORTB_03 				(IO_GROUP_NUM * 1 + 3)
#define IO_PORTB_04 				(IO_GROUP_NUM * 1 + 4)
#define IO_PORTB_05 				(IO_GROUP_NUM * 1 + 5)
#define IO_PORTB_06 				(IO_GROUP_NUM * 1 + 6)
#define IO_PORTB_07 				(IO_GROUP_NUM * 1 + 7)
#define IO_PORTB_08 				(IO_GROUP_NUM * 1 + 8)
#define IO_PORTB_09 				(IO_GROUP_NUM * 1 + 9)
#define IO_PORTB_10 				(IO_GROUP_NUM * 1 + 10)
#define IO_PORTB_11 				(IO_GROUP_NUM * 1 + 11)

#define IO_PORTD_00 				(IO_GROUP_NUM * 2 + 0)
#define IO_PORTD_01 				(IO_GROUP_NUM * 2 + 1)
#define IO_PORTD_02 				(IO_GROUP_NUM * 2 + 2)
#define IO_PORTD_03 				(IO_GROUP_NUM * 2 + 3)
#define IO_PORTD_04 				(IO_GROUP_NUM * 2 + 4)

#define IO_MAX_NUM 					(IO_PORTD_04+1)
#define IO_PORT_MAX					(IO_PORTD_04 + 1)

#define GPIOA                       (IO_GROUP_NUM * 0)
#define GPIOB                       (IO_GROUP_NUM * 1)
#define GPIOD                       (IO_GROUP_NUM * 2)

enum {
    INPUT_CH0,
    INPUT_CH1,
    INPUT_CH2,
    INPUT_CH3,
};

enum gpio_op_mode {
    GPIO_SET = 1,
    GPIO_AND,
    GPIO_OR,
    GPIO_XOR,
};
enum gpio_direction {
    GPIO_OUT = 0,
    GPIO_IN = 1,
};
struct gpio_reg {
    volatile unsigned int out;
    volatile unsigned int in;
    volatile unsigned int dir;
    volatile unsigned int die;
    volatile unsigned int pu;
    volatile unsigned int pd;
    volatile unsigned int hd0;
    volatile unsigned int hd;
    volatile unsigned int dieh;
};

struct gpio_platform_data {
    unsigned int gpio;
};

#define DACN_IO_DIR 	BIT(2)
#define DACN_IO_PD		BIT(4)
#define DACN_IO_PU 		BIT(6)
#define DACN_IO_DIE 	BIT(9)

#define DACP_IO_DIR 	BIT(3)
#define DACP_IO_PD		BIT(5)
#define DACP_IO_PU 		BIT(7)
#define DACP_IO_DIE 	BIT(10)

#define GPIO_PLATFORM_DATA_BEGIN(data) \
	static const struct gpio_platform_data data = { \

#define GPIO_PLATFORM_DATA_END() \
	};

/*----------------------------------------------------------------------------*/
/** @brief: DACNO,DACNP作为普通IO输出，或者作为outputch输出
    @param: mode:选择DACNO,DACPO输出的方式    outchsel：如果是outputch方式，选择output源  out：如果是IO方式选择输出0还是1
    @return:void
    @author:
    @note:  void dacnp_out_mode_init(DACX_MODE_SEL mode,OUTCH_SEL_T outchsel,u8 out)
*/
/*----------------------------------------------------------------------------*/
void gpio_dacnp_out_mode_init(DACX_MODE_SEL mode, OUTCH_SEL_T outchsel, u8 out);
/*----------------------------------------------------------------------------*/
/** @brief: DACNO,DACNP作为普通IO输出，输出0还是1
    @param: mode:DACN_OUTIO或者DACP_OUTIO out：如果是IO方式选择输出0还是1
    @return:void
    @author:
    @note:  void gpio_dacnp_out_set(DACX_MODE_SEL mode,u8 out)
*/
/*----------------------------------------------------------------------------*/
void gpio_dacnp_out_set(DACX_MODE_SEL mode, u8 out);
/**
 * @brief usb_iomode
 *
 * @param enable 1，使能；0，关闭
 */
// void usb_iomode(u32 enable);
/**
 * @brief gpio_set_direction
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param dir 1，输入；0，输出
 *
 * @return
 */
int gpio_set_direction(u32 gpio, u32 dir);


/**
 * @brief gpio_direction_input
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 *
 * @return
 */
// int gpio_direction_input(u32 gpio);
/**
 * @brief gpio_direction_input
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，输出1,  0，输出0
 *
 * @return
 */
// int gpio_set_output_value(u32 gpio, u32 dir);

/**
 * @brief gpio_dir
 *
 * @param gpio [GPIOA GPIOB GPIOC GPIOD GPIOR GPIOUSB]
 * @param start [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param len  [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param dat 1，输入；0，输出
 *
 * @return
 */
// u32 gpio_dir(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);

/**
 * @brief gpio_direction_output
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，输出1；0，输出0
 *
 * @return
 */
// int gpio_direction_output(u32 gpio, int value);

/**
 * @brief gpio_out
 *
 * @param gpio [GPIOA GPIOB GPIOC GPIOD GPIOR GPIOUSB]
 * @param start [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param len  [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param dat 1，输入；0，输出
 *
 * @return
 */
// u32 gpio_out(u32 gpio, u32 start, u32 len, u32 dat);

/**
 * @brief gpio_set_pull_up
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，上拉；0，不上拉
 *
 * @return
 */
int gpio_set_pull_up(u32 gpio, int value);

/**
 * @brief gpio_set_pu
 *
 * @param gpio [GPIOA GPIOB GPIOC GPIOD GPIOR GPIOUSB]
 * @param start [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param len  [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param dat 1，上拉；0，不上拉
 *
 * @return
 */
// u32 gpio_set_pu(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);

/**
 * @brief gpio_set_pull_down
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，下拉；0，不下拉
 *
 * @return
 */
int gpio_set_pull_down(u32 gpio, int value);

/**
 * @brief gpio_set_pd
 *
 * @param gpio [GPIOA GPIOB GPIOC GPIOD GPIOR GPIOUSB]
 * @param start [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param len  [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param dat 1，下拉；0，不下拉
 *
 * @return
 */
// u32 gpio_set_pd(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);

/**
 * @brief gpio_set_hd0
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，增强输出；0，不增强输出
 *
 * @return
 */
u32 gpio_set_hd0(u32 gpio, u32 value);

/**
 * @brief gpio_set_hd
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，增强输出；0，不增强输出
 *
 * @return
 */
int gpio_set_hd(u32 gpio, int value);

/**
 * @brief gpio_set_die
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，IO普通输入；0，IO模拟输入
 *
 * @return
 */
int gpio_set_die(u32 gpio, int value);

/**
 * @brief gpio_set_dieh
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，IO普通输入；0，IO模拟输入
 *
 * @return
 */
u32 gpio_set_dieh(u32 gpio, u32 value);

/**
 * @brief gpio_die
 *
 * @param gpio [GPIOA GPIOB GPIOC GPIOD GPIOR GPIOUSB]
 * @param start [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param len  [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param dat 1，IO普通输入；0，IO模拟输入
 *
 * @return
 */
// u32 gpio_die(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);

/**
 * @brief gpio_dieh
 *
 * @param gpio [GPIOA GPIOB GPIOC GPIOD GPIOR GPIOUSB]
 * @param start [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param len  [0-15]，GPIOR[0-3]，GPIOUSB[0-1]
 * @param dat 1，IO普通输入；0，IO模拟输入
 *
 * @return
 */
// u32 gpio_dieh(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op);

/**
 * @brief gpio_set_output_channle
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param clk 参考枚举CHx_UTx_TX，如CH0_UT0_TX
 *
 * @return
 */
u32 gpio_output_channle(u32 gpio, u32 clk);

/**
 * @brief gpio_read
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 *
 * @return
 */
int gpio_read(u32 gpio);

/**
 * @brief gpio_in
 *
 * @param gpio [GPIOA GPIOB GPIOC GPIOD GPIOR GPIOUSB]
 *
 * @return
 */
// u32 gpio_in(u32 gpio);
/**
 * @brief gpio_write
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 * @param value 1，输出1；0，输出0
 *
 * @return
 */
u32 gpio_write(u32 gpio, u32 value);

/**
 * @brief gpio_wakeup0 use IN_CHNL0_SEL
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 *
 * @return
 */
u32 gpio_wakeup0(u32 gpio); //输入通道2：UART0输入；或wakeup_src[0]

/**
 * @brief gpio_wakeup1 use IN_CHNL0_SEL
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 *             与irflt共用
 * @return
 */
u32 gpio_wakeup1(u32 gpio);//输入通道1：irflt输入；或wakeup_src[1]

/**
 * @brief gpio_irflt_in use IN_CHNL1_SEL
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 *             y与wakeup1共用
 * @return
 */
u32 gpio_irflt_in(u32 gpio);//输入通道1：irflt输入；或wakeup_src[1]

/**
 * @brief gpio_cap_mux use IN_CHNL2_SEL
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 *
 * @return
 */
u32 gpio_cap_mux(u32 gpio);//输入通道0：tmr012 cap_in；或UART1输入

/**
 * @brief gpio_clock_in use IN_CHNL3_SEL
 *
 * @param gpio 参考宏IO_PORTx_xx，如IO_PORTA_00
 *
 * @return
 */
u32 gpio_clock_in(u32 gpio);//输入通道3：tmr2 cin输入

/**
 * @brief 字符串转引脚号：“PA05” --> IO_PORTA_05
 *
 * @param p : "PA01"，"PB06"，"USBDP"
 *
 * @return 对应的引脚号
 */
u32 get_gpio(const char *p);


//gpio操作关中断临界保护控制变量
extern const char GPIO_CRITICAL_OPT;

//cgh:io配置的宏
#define input_mode       1
#define output_mode      0

#define digital_mode     1
#define analog_mode      0

#define pull_up_en       1
#define pull_down_en     1
#define pull_up_dis      0
#define pull_down_dis    0
// u32 gpio_close_uart0();
// u32 gpio_close_uart1();
// void gpio_uart_rx_input(u8 rx_pin,u8 ut_num,u8 ut_num1);
// u8 gpio_set_uart1(u32 ut_ch);
// u8 gpio_set_uart0(u32 ut_ch);
#endif  /*GPIO_H*/

