#include "SYSCFG.h"
#include "FT61FC3x_GPIO_INIT.H"



void GPIO_CLOCK_INIT(void)
{
    OSCCON = 0B01110001;	//IRCF=111=16MHz/4T=4MHz,0.25us
    OPTION = 0B00001000;    //Bit3=1 WDT，Bit[2:0]=000=WDT RATE 1:1				 		
	INTCON = 0;  			//暂禁止所有中断
    PORTA  = 0B00000000;
    PORTB  = 0B00000000;
    PORTC  = 0B00000000;
}


void GPIO_INIT(Gpio_Pin_t gpio_x, Gpio_Direction_t out_in, Gpio_PullUp_Down_t pull_dir) 
{
    Pin_SetDirection(gpio_x, out_in);
    Pin_SetPullUp(gpio_x, pull_dir);
}

void Pin_SetDirection(Gpio_Pin_t pin, Gpio_Direction_t dir) 
{
    if (pin <= GPIO_PA7) {
        // PORTA
        if (dir == Out) {
            TRISA &= ~(1 << (pin - GPIO_PA0));
        } else {
            TRISA |= (1 << (pin - GPIO_PA0));
        }
    } else if (pin >= GPIO_PB0 && pin <= GPIO_PB7) {
        // PORTB
        if (dir == Out) {
            TRISB &= ~(1 << (pin - GPIO_PB0));
        } else {
            TRISB |= (1 << (pin - GPIO_PB0));
        }
    } else if (pin >= GPIO_PC0 && pin <= GPIO_PC7) {
        // PORTC
        if (dir == Out) {
            TRISC &= ~(1 << (pin - GPIO_PC0));
        } else {
            TRISC |= (1 << (pin - GPIO_PC0));
        }
    }
}

void Pin_SetPullUp(Gpio_Pin_t pin, bool enable) 
{
    if (pin <= PA7) {
        // PORTA
        if (enable) {
            WPUA |= (1 << (pin - GPIO_PA0));
        } else {
            WPUA &= ~(1 << (pin - GPIO_PA0));
        }
    } else if (pin >= GPIO_PB0 && pin <= GPIO_PB7) {
        // PORTB
        if (enable) {
            WPUB |= (1 << (pin - PB0));
        } else {
            WPUB &= ~(1 << (pin - PB0));
        }
    } else if (pin >= GPIO_PC0 && pin <= GPIO_PC7) {
        // PORTC
        if (enable) {
            WPUC |= (1 << (pin - PC0));
        } else {
            WPUC &= ~(1 << (pin - PC0));
        }
    }
}

/******************* (C) COPYRIGHT 2025 Sensasy Corporation *****END OF FILE****/