//Project: FT61FC35TRB.prj
// Device: FT61FC3X
// Memory: Flash 3KX14b, EEPROM 128X8b, SRAM 256X8b
// Author: 
//Company: 
//Version:
//   Date: 
//===========================================================
//===========================================================
#include	"SYSCFG.h"
#include	"FT61FC3x_GPIO_INIT.H"
#include 	"FT61FC3x_IIC.H"
#include 	"KT0646M.h"
//===========================================================
//Variable definition
//===========================================================



void hardware_init(void)
{
	GPIO_CLOCK_INIT(); //总时钟初始化
	IIC_INITIAL();
}

//===========================================================
//Function name：interrupt ISR
//parameters：无
//returned value：无
//===========================================================
void interrupt ISR(void)
{
}
//===========================================================
//Function name：main
//parameters：无
//returned value：无
//===========================================================
main()
{
//===========================================================
//除TSSOP/SOP20封装外，PA3、PA5必须配置成输出低,程序中请勿修改
	hardware_init();

	while(1)
	{
		//KT_Test_Init();
	}
}
//===========================================================
