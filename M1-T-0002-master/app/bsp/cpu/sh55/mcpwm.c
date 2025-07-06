#include "sfr.h"
#include "cpu.h"
#include "config.h"
#include "clock.h"
#include "mcpwm.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "debug.h"


#define pwm_frq_cnt(f)  (clk_get("lsb")/f)




// #define _pwm_frq_duty(channel,frq,duty) \
//     JL_PWM->TMR##channel##_PR = pwm_frq_cnt(frq); \
//     JL_PWM->CH##channel##_CMP = pwm_frq_cnt(frq) * duty / 100;

#define _pwm_frq_duty(channel,frq,duty) \
    JL_PWM->TMR##channel##_PR = pwm_frq_cnt(frq) / 8; \
    JL_PWM->CH##channel##_CMP = pwm_frq_cnt(frq) / 8 * duty / 100;

const u32 mcpwm_tab[4] = {
    IO_PORTA_06,
    IO_PORTA_07,
    IO_PORTA_11,
    IO_PORTA_12,
};


// #define _mcpwm_set(channel,frq,duty)                 \
//     _pwm_frq_duty(channel,frq,duty);                 \
//     /*sel pwm use mcpwmtmr0*/                       \
//     SFR(JL_PWM->PWMCON1, (1+channel*4), 2, channel);\选择用哪个定时器
//     SFR(JL_PWM->PWMCON1, (channel*4), 1, 0);		    \正向/反向
//     /* mcpwm tmr0 en */                             \
//     JL_PWM->TMR##channel##_CON = BIT(5);            \TMR寄存器清0
        // JL_PWM->TMR##channel##_CON |= BIT(1) | BIT(0);\TMR寄存器8分频
//     JL_PWM->PWMCON0 |= BIT(8+channel);              \定时器计数开启
//     /*mcpwm ch0 en  */                              \
//     JL_PWM->PWMCON0 |= BIT(channel);                \对应pwm模块开启
//      gpio_set_die(mcpwm_tab[channel], 1);           \初始化IO
//     gpio_set_direction(mcpwm_tab[channel], 0);      \初始化IO
// 	JL_IOMC->IOMC0 &= ~BIT(16+channel);                 pwm模块占用使能关闭（开启占用会从固定硬件IO输出，取消后不从固定IO输出，可映射到其他IO

#define _mcpwm_set(channel,frq,duty)                 \
    _pwm_frq_duty(channel,frq,duty);                 \
    /*sel pwm use mcpwmtmr0*/                       \
    SFR(JL_PWM->PWMCON1, (1+channel*4), 2, channel);\
    SFR(JL_PWM->PWMCON1, (channel*4), 1, 0);		\
    JL_PWM->TMR##channel##_CON = BIT(5);            \
    JL_PWM->TMR##channel##_CON |= BIT(1) | BIT(0);  \
    /* 8分频 */\
    JL_PWM->PWMCON0 |= BIT(8+channel);              \
    JL_PWM->PWMCON0 |= BIT(channel);                \
    gpio_set_die(mcpwm_tab[channel], 1);           \
    gpio_set_direction(mcpwm_tab[channel], 0);      \
	JL_IOMC->IOMC0 &= ~(BIT(16+channel));

void mcpwm_set(u8 channel, u32 fre, u32 duty)
{
    switch (channel) {
    case 0:
        _mcpwm_set(0, fre, duty);
        break;
    case 1:
        _mcpwm_set(1, fre, duty);
        break;
    case 2:
        _mcpwm_set(2, fre, duty);
        break;
    case 3:
        _mcpwm_set(3, fre, duty);
        break;
    default:
        break;
    }
    log_info("JL_PWM->PWMCON1:%x \n", JL_PWM->PWMCON1);
}

void pwm_frq_duty(u8 channel, u32 fre, u32 duty)
{
    switch (channel) {
    case 0:
        _pwm_frq_duty(0, fre, duty);
        break;
    case 1:
        _pwm_frq_duty(1, fre, duty);
        break;
    case 2:
        _pwm_frq_duty(2, fre, duty);
        break;
    case 3:
        _pwm_frq_duty(3, fre, duty);
        break;
    default:
        break;
    }
}

void test_mcpwm(void)
{
    log_info("*****test mcpwm****\n");
    mcpwm_set(0, 10000, 50); //fre 1000,50%占空比
    mcpwm_set(1, 10000, 40); //fre 1000,40%占空比
    mcpwm_set(2, 10000, 30); //fre 1000,30%占空比
    mcpwm_set(3, 10000, 20); //fre 1000,20%占空比
}



