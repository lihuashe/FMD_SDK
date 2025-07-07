/**
*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file     io_gpio.c
* @brief    This file provides demo code of gpio input mode.
            Get key value.
* @details
* @author   yuan
* @date     2018-12-07
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "FT61FC3x_GPIO_INIT.H"
#include "seg.h"
#include <stdlib.h>

#define PIN_OUT_DISABLE     0
#define PIN_OUT_HIGH 		1
#define PIN_OUT_LOW 		2
#define BLINK_SEG(index)    index - 1  //����ű�����ź�

#define SEG_PIN_MAX 7
#define SEG_PIN_1 0
#define SEG_PIN_2 1
#define SEG_PIN_3 2
#define SEG_PIN_4 3
#define SEG_PIN_5 4
#define SEG_PIN_6 5
#define SEG_PIN_7 6

#define U32_BYTE0(a) ((a) & 0xFF)
#define U32_BYTE1(a) (((a) >> 8) & 0xFF)
#define U32_BYTE2(a) (((a) >> 16) & 0xFF)
#define U32_BYTE3(a) (((a) >> 24) & 0xFF)


u8 led_seg_en_seg_arr[42]={0};

u8 pin_arr[7]={LED_SEF_1,LED_SEF_2,LED_SEF_3,LED_SEF_4,LED_SEF_5,LED_SEF_6,LED_SEF_7};
u8 seg_output_low_arr[SEG_PIN_MAX]={PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE};
u8 led_seg_blink_arr[SEG_PIN_MAX]={PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE,PIN_OUT_DISABLE};




/*	��������о���
	F1,G1,C1,D1,D3,K1,
 	A1,F2,G2,C2,K3,K4,
 	B1,A2,F3,G3,E3,J1,
    E1,B2,00,A3,D4,00,
 	K2,E2,B3,C3,F4,DP,
 	J2,D2,J3,E4,C4,A4,
 	K6,K7,J4,K8,G4,B4*/

const u8 led_seg_number_seg_arr[19][7]=
{
	{1,1,1,1,1,1,0},//0
	{0,1,1,0,0,0,0},//1
	{1,1,0,1,1,0,1},//2
	{1,1,1,1,0,0,1},//3
	{0,1,1,0,0,1,1},//4
	{1,0,1,1,0,1,1},//5
	{1,0,1,1,1,1,1},//6
	{1,1,1,0,0,0,0},//7
	{1,1,1,1,1,1,1},//8
	{1,1,1,1,0,1,1},//9
	{1,1,1,0,1,1,1},//A
	{0,0,1,1,1,1,1},//b
	{1,0,0,1,1,1,0},//C
	{0,1,1,1,1,0,1},//d
	{1,0,0,1,1,1,1},//E
	{1,0,0,0,1,1,1},//F
	{0,1,1,0,1,1,1},//H
	{0,1,1,1,1,1,0},//V
	{0,0,0,0,0,0,1},//-
};


/**   ����ܶ����ʼ��
 * \brief    board_seg_init
 * \param[in]   ��
 * \return      ��
 */
void board_seg_init(void)
{

    for(int i = 0; i < 7;i++)
    {
		GPIO_INIT(pin_arr[i], Out, NoPullUp);
    }
}

/**
  * @brief  Initialize GPIO peripheral.
  * @param  No parameter.
  * @return void
  */

void led_seg_pin_set_mode(u8 pin_index,u8 pin_mode)
{
	if(pin_mode == PIN_OUT_DISABLE)
	{
		GPIO_INIT(pin_index, In, NoPullUp);
	}
	else if(pin_mode == PIN_OUT_HIGH)
	{
		GPIO_INIT(pin_index, Out, NoPullUp);
		Pin_SetHighLow_Level(pin_index, 1);
	}
	else if(pin_mode == PIN_OUT_LOW)
	{
		GPIO_INIT(pin_index, Out, NoPullUp);
		Pin_SetHighLow_Level(pin_index, 0);
	}
}


/**  ��������
 * \brief    seg_power_arr_write
 *
 * \param[in]   *write_seg_arr   д�������
* \param[in]		power            ������ϵĵ������Ʋ���     �����������SEG_POWER_0 SEG_POWER_1 SEG_POWER_2 SEG_POWER_3 SEG_POWER_NO_DISPLAY
 * \param[in]
 * \return      ��
 */
void seg_production_test(void)
{
	u8 i;
	for(i = 0;i < 42;i ++)
	led_seg_en_seg_arr[i] = 1;
}
/**  A B ͨ��ѡ��
 * \brief    seg_power_arr_write
 *
 * \param[in]   *write_seg_arr   д�������
* \param[in]		power            ������ϵĵ������Ʋ���     �����������SEG_POWER_0 SEG_POWER_1 SEG_POWER_2 SEG_POWER_3 SEG_POWER_NO_DISPLAY
 * \param[in]
 * \return      ��
 */
void channel_select_init(void)
{
	/*if(gpio_read(GPIO_CHANNEL) == 1)
		channel_A_display(1);
		else
		channel_B_display(1);
		*/
}
/**   ��ص�����ʾ
 * \brief    seg_power_arr_write
 *
 * \param[in]   *write_seg_arr   д�������
* \param[in]		power            ������ϵĵ������Ʋ���     �����������SEG_POWER_0 SEG_POWER_1 SEG_POWER_2 SEG_POWER_3 SEG_POWER_NO_DISPLAY
 * \param[in]
 * \return      ��
 */
void seg_power_arr_write(u8 *write_seg_arr,SEG_POWERTypeDef power)
{
//	if(power != SEG_POWER_NO_DISPLAY)
//		{
	if(power == SEG_POWER_NO_DISPLAY)
	{
		write_seg_arr[SEG_POWER_BORD1_J1] = 0;
		write_seg_arr[SEG_POWER_BORD2_J2] = 0;
		write_seg_arr[SEG_POWER_BORD3_J3] = 0;
		write_seg_arr[SEG_POWER_BORD4_J4] = 0;
		write_seg_arr[SEG_POWER_GRID1_K8] = 0;
		write_seg_arr[SEG_POWER_GRID2_K7] = 0;
		write_seg_arr[SEG_POWER_GRID3_K6] = 0;
	}

	if(power == SEG_POWER_0)
	{
		write_seg_arr[SEG_POWER_BORD1_J1] = 1;
		write_seg_arr[SEG_POWER_BORD2_J2] = 1;
		write_seg_arr[SEG_POWER_BORD3_J3] = 1;
		write_seg_arr[SEG_POWER_BORD4_J4] = 1;
		write_seg_arr[SEG_POWER_GRID1_K8] = 0;
		write_seg_arr[SEG_POWER_GRID2_K7] = 0;
		write_seg_arr[SEG_POWER_GRID3_K6] = 0;
	}
	if(power == SEG_POWER_1)
	{
		write_seg_arr[SEG_POWER_BORD1_J1] = 1;
		write_seg_arr[SEG_POWER_BORD2_J2] = 1;
		write_seg_arr[SEG_POWER_BORD3_J3] = 1;
		write_seg_arr[SEG_POWER_BORD4_J4] = 1;
		write_seg_arr[SEG_POWER_GRID1_K8] = 1;
		write_seg_arr[SEG_POWER_GRID2_K7] = 0;
		write_seg_arr[SEG_POWER_GRID3_K6] = 0;

	}
	if(power == SEG_POWER_2)
	{
		write_seg_arr[SEG_POWER_BORD1_J1] = 1;
		write_seg_arr[SEG_POWER_BORD2_J2] = 1;
		write_seg_arr[SEG_POWER_BORD3_J3] = 1;
		write_seg_arr[SEG_POWER_BORD4_J4] = 1;
		write_seg_arr[SEG_POWER_GRID1_K8] = 1;
		write_seg_arr[SEG_POWER_GRID2_K7] = 1;
		write_seg_arr[SEG_POWER_GRID3_K6] = 0;
	}
	if(power == SEG_POWER_3)
	{
		write_seg_arr[SEG_POWER_BORD1_J1] = 1;
		write_seg_arr[SEG_POWER_BORD2_J2] = 1;
		write_seg_arr[SEG_POWER_BORD3_J3] = 1;
		write_seg_arr[SEG_POWER_BORD4_J4] = 1;
		write_seg_arr[SEG_POWER_GRID1_K8] = 1;
		write_seg_arr[SEG_POWER_GRID2_K7] = 1;
		write_seg_arr[SEG_POWER_GRID3_K6] = 1;
	}
}


/**
 * \brief    seg_channel_power_signal_fre_arr_write
 *
 * \param[in]   *write_seg_arr   д�������
 * \param[in]   channelA         ������ϵ�A�ο��Ʋ���      �����������SEG_CHANNEL_A_DISPLAY SEG_CHANNEL_A_NO_DISPLAY
 * \param[in]	channelB         ������ϵ�B�ο��Ʋ���      �����������SEG_CHANNEL_B_DISPLAY SEG_CHANNEL_B_NO_DISPLAY
 * \param[in]	power            ������ϵĵ������Ʋ���     �����������SEG_POWER_0 SEG_POWER_1 SEG_POWER_2 SEG_POWER_3 SEG_POWER_NO_DISPLAY
 * \param[in]   signal           ������ϵ��ź�ǿ�ȿ��Ʋ��� �����������SEG_SIGNAL_DISPLAY SEG_FRE_NO_DISPLAY
 * \param[in]   fre              ������ϵ�Ƶ�ʵ�λ���Ʋ��� �����������SEG_FRE_DISPLAY SEG_FRE_NO_DISPLAY
 * \param[in]
*
 * \return      ��

 */
void seg_channel_power_signal_fre_arr_write(u8 *write_seg_arr,SEG_CHANNEL_ATypeDef channelA,SEG_CHANNEL_BTypeDef channelB,SEG_POWERTypeDef power, SEG_FRETypeDef fre)
	{
		if(channelA == SEG_CHANNEL_A_DISPLAY)
			write_seg_arr[SEG_CHANNEL_A_K2] = 1;

		if(channelB == SEG_CHANNEL_B_DISPLAY)
			write_seg_arr[SEG_CHANNEL_B_K4] = 1;

		if(power != SEG_POWER_NO_DISPLAY)
		{

			if(power == SEG_POWER_0)
			{
				write_seg_arr[SEG_POWER_BORD1_J1] = 1;
				write_seg_arr[SEG_POWER_BORD2_J2] = 1;
				write_seg_arr[SEG_POWER_BORD3_J3] = 1;
				write_seg_arr[SEG_POWER_BORD4_J4] = 1;
			}
			if(power == SEG_POWER_1)
			{
				write_seg_arr[SEG_POWER_BORD1_J1] = 1;
				write_seg_arr[SEG_POWER_BORD2_J2] = 1;
				write_seg_arr[SEG_POWER_BORD3_J3] = 1;
				write_seg_arr[SEG_POWER_BORD4_J4] = 1;
				write_seg_arr[SEG_POWER_GRID1_K8] = 1;
			}
			if(power == SEG_POWER_2)
			{
				write_seg_arr[SEG_POWER_BORD1_J1] = 1;
				write_seg_arr[SEG_POWER_BORD2_J2] = 1;
				write_seg_arr[SEG_POWER_BORD3_J3] = 1;
				write_seg_arr[SEG_POWER_BORD4_J4] = 1;
				write_seg_arr[SEG_POWER_GRID1_K8] = 1;
				write_seg_arr[SEG_POWER_GRID2_K7] = 1;
			}
			if(power == SEG_POWER_3)
			{
				write_seg_arr[SEG_POWER_BORD1_J1] = 1;
				write_seg_arr[SEG_POWER_BORD2_J2] = 1;
				write_seg_arr[SEG_POWER_BORD3_J3] = 1;
				write_seg_arr[SEG_POWER_BORD4_J4] = 1;
				write_seg_arr[SEG_POWER_GRID1_K8] = 1;
				write_seg_arr[SEG_POWER_GRID2_K7] = 1;
				write_seg_arr[SEG_POWER_GRID3_K6] = 1;
			}
		}

		if(fre == SEG_FRE_DISPLAY)
			write_seg_arr[SEG_FRE_K3] = 1;

	}

void display_signal_fre(SEG_FRETypeDef fre)
{
	if(fre == SEG_FRE_DISPLAY)
		led_seg_en_seg_arr[SEG_FRE_K3] = 1;
}


/**  �����������ʾ����
 * \brief    seg_number_arr_write
 *
 * \param[in]   *write_seg_arr  д�������
 * \param[in]   seg_number_1    ������ϵĵ�һ�����ֿ��Ʋ���     �����������0 1 2 3 4 5 6 7 8 9 A B C D E F SEG_NUMBER_NO_DISPLAY
 * \param[in]	seg_number_2    ������ϵĵڶ������ֿ��Ʋ���     �����������0 1 2 3 4 5 6 7 8 9 A B C D E F SEG_NUMBER_NO_DISPLAY
 * \param[in]	seg_number_3    ������ϵĵ��������ֿ��Ʋ���     �����������0 1 2 3 4 5 6 7 8 9 A B C D E F SEG_NUMBER_NO_DISPLAY
 * \param[in]   seg_point       ������ϵ�С������Ʋ���         �����������SEG_DP_DISPLAY SEG_DP_NO_DISPLAY
 * \param[in]   seg_number_4    ������ϵĵ��ĸ����ֿ��Ʋ���     �����������0 1 2 3 4 5 6 7 8 9 A B C D E F SEG_NUMBER_NO_DISPLAY
 * \param[in]
*
 * \return      ��

 */
void seg_number_arr_write(u8 *write_seg_arr,SEG_NUMBERTypeDef seg_number_1,SEG_NUMBERTypeDef seg_number_2,SEG_NUMBERTypeDef seg_number_3,SEG_DPTypeDef seg_dp,SEG_NUMBERTypeDef seg_number_4)  //���������֡��ʾ
{
	if(seg_number_1 != SEG_NUMBER_NO_DISPLAY)
		{
			write_seg_arr[SEG_A1] = led_seg_number_seg_arr[seg_number_1][0];
			write_seg_arr[SEG_B1] = led_seg_number_seg_arr[seg_number_1][1];
			write_seg_arr[SEG_C1] = led_seg_number_seg_arr[seg_number_1][2];
			write_seg_arr[SEG_D1] = led_seg_number_seg_arr[seg_number_1][3];
			write_seg_arr[SEG_E1] = led_seg_number_seg_arr[seg_number_1][4];
			write_seg_arr[SEG_F1] = led_seg_number_seg_arr[seg_number_1][5];
			write_seg_arr[SEG_G1] = led_seg_number_seg_arr[seg_number_1][6];
		}
		else
		{
			write_seg_arr[SEG_A1] = 0;
			write_seg_arr[SEG_B1] = 0;
			write_seg_arr[SEG_C1] = 0;
			write_seg_arr[SEG_D1] = 0;
			write_seg_arr[SEG_E1] = 0;
			write_seg_arr[SEG_F1] = 0;
			write_seg_arr[SEG_G1] = 0;
		}
	if(seg_number_2 != SEG_NUMBER_NO_DISPLAY)
			{
				write_seg_arr[SEG_A2] = led_seg_number_seg_arr[seg_number_2][0];
				write_seg_arr[SEG_B2] = led_seg_number_seg_arr[seg_number_2][1];
				write_seg_arr[SEG_C2] = led_seg_number_seg_arr[seg_number_2][2];
				write_seg_arr[SEG_D2] = led_seg_number_seg_arr[seg_number_2][3];
				write_seg_arr[SEG_E2] = led_seg_number_seg_arr[seg_number_2][4];
				write_seg_arr[SEG_F2] = led_seg_number_seg_arr[seg_number_2][5];
				write_seg_arr[SEG_G2] = led_seg_number_seg_arr[seg_number_2][6];
			}
		else
		{
			write_seg_arr[SEG_A2] = 0;
			write_seg_arr[SEG_B2] = 0;
			write_seg_arr[SEG_C2] = 0;
			write_seg_arr[SEG_D2] = 0;
			write_seg_arr[SEG_E2] = 0;
			write_seg_arr[SEG_F2] = 0;
			write_seg_arr[SEG_G2] = 0;
		}
	if(seg_number_3 != SEG_NUMBER_NO_DISPLAY)
			{
				write_seg_arr[SEG_A3] = led_seg_number_seg_arr[seg_number_3][0];
				write_seg_arr[SEG_B3] = led_seg_number_seg_arr[seg_number_3][1];
				write_seg_arr[SEG_C3] = led_seg_number_seg_arr[seg_number_3][2];
				write_seg_arr[SEG_D3] = led_seg_number_seg_arr[seg_number_3][3];
				write_seg_arr[SEG_E3] = led_seg_number_seg_arr[seg_number_3][4];
				write_seg_arr[SEG_F3] = led_seg_number_seg_arr[seg_number_3][5];
				write_seg_arr[SEG_G3] = led_seg_number_seg_arr[seg_number_3][6];
			}
					else
		{
			write_seg_arr[SEG_A3] = 0;
			write_seg_arr[SEG_B3] = 0;
			write_seg_arr[SEG_C3] = 0;
			write_seg_arr[SEG_D3] = 0;
			write_seg_arr[SEG_E3] = 0;
			write_seg_arr[SEG_F3] = 0;
			write_seg_arr[SEG_G3] = 0;
		}
	if(seg_number_4 != SEG_NUMBER_NO_DISPLAY)
			{
				write_seg_arr[SEG_A4] = led_seg_number_seg_arr[seg_number_4][0];
				write_seg_arr[SEG_B4] = led_seg_number_seg_arr[seg_number_4][1];
				write_seg_arr[SEG_C4] = led_seg_number_seg_arr[seg_number_4][2];
				write_seg_arr[SEG_D4] = led_seg_number_seg_arr[seg_number_4][3];
				write_seg_arr[SEG_E4] = led_seg_number_seg_arr[seg_number_4][4];
				write_seg_arr[SEG_F4] = led_seg_number_seg_arr[seg_number_4][5];
				write_seg_arr[SEG_G4] = led_seg_number_seg_arr[seg_number_4][6];
			}
		else
		{
			write_seg_arr[SEG_A4] = 0;
			write_seg_arr[SEG_B4] = 0;
			write_seg_arr[SEG_C4] = 0;
			write_seg_arr[SEG_D4] = 0;
			write_seg_arr[SEG_E4] = 0;
			write_seg_arr[SEG_F4] = 0;
			write_seg_arr[SEG_G4] = 0;
		}

			if(seg_dp == SEG_DP_DISPLAY)
				write_seg_arr[SEG_DP] = 1;

}
/**  �ϵ���ʾ
 * \brief    power_on_display
 * \param[in]   ��
 * \return      ��
 */
void power_on_display(void)
{
   // log_info("into power_on_display");
	led_seg_en_seg_arr[SEG_F2] = 1;
	led_seg_en_seg_arr[SEG_C2] = 1;
	led_seg_en_seg_arr[SEG_A2] = 1;
	led_seg_en_seg_arr[SEG_F3] = 1;
	led_seg_en_seg_arr[SEG_A3] = 1;
	led_seg_en_seg_arr[SEG_E3] = 1;
	led_seg_en_seg_arr[SEG_B2] = 1;
	led_seg_en_seg_arr[SEG_E2] = 1;
	led_seg_en_seg_arr[SEG_B3] = 1;
	led_seg_en_seg_arr[SEG_C3] = 1;
	led_seg_en_seg_arr[SEG_D2] = 1;
}

void ota_dynamic_display(u8 seg)
{
	switch (seg)
	{
	case 0:
		led_seg_en_seg_arr[SEG_A4] = 0;
		led_seg_en_seg_arr[SEG_B4] = 0;
		led_seg_en_seg_arr[SEG_C4] = 0;
		led_seg_en_seg_arr[SEG_D4] = 0;
		led_seg_en_seg_arr[SEG_E4] = 0;
		led_seg_en_seg_arr[SEG_F4] = 0;
		led_seg_en_seg_arr[SEG_G4] = 0;
		break;
	case 1:
		led_seg_en_seg_arr[SEG_F4] = 1;
		break;
	case 2:
		led_seg_en_seg_arr[SEG_A4] = 1;
		break;
	case 3:
		led_seg_en_seg_arr[SEG_B4] = 1;
		break;
	case 4:
		led_seg_en_seg_arr[SEG_C4] = 1;
		break;
	case 5:
		led_seg_en_seg_arr[SEG_D4] = 1;
		break;
	case 6:
		led_seg_en_seg_arr[SEG_E4] = 1;
		break;

	default:
		break;
	}
}

/**  OTA������ʾ
 * \brief    ota_display
 * \param[in]   ��
 * \return      ��
 */
void ota_display(void)
{
	led_seg_en_seg_arr[SEG_A1] = 1;
	led_seg_en_seg_arr[SEG_B1] = 1;
	led_seg_en_seg_arr[SEG_C1] = 1;
	led_seg_en_seg_arr[SEG_D1] = 1;
	led_seg_en_seg_arr[SEG_E1] = 1;
	led_seg_en_seg_arr[SEG_F1] = 1;

	led_seg_en_seg_arr[SEG_A2] = 1;
	led_seg_en_seg_arr[SEG_E2] = 1;
	led_seg_en_seg_arr[SEG_F2] = 1;

	led_seg_en_seg_arr[SEG_A3] = 1;
	led_seg_en_seg_arr[SEG_B3] = 1;
	led_seg_en_seg_arr[SEG_C3] = 1;
	led_seg_en_seg_arr[SEG_E3] = 1;
	led_seg_en_seg_arr[SEG_F3] = 1;
	led_seg_en_seg_arr[SEG_G3] = 1;
}

/**  ���ʧ��
 * \brief    err_display
 * \param[in]   ��
 * \return      ��
 */
void err_display(void)
{
	seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERF,SEG_DISPLAY_NUMBERF,SEG_DISPLAY_NUMBERF,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBERF);
}
/**   2.4G���ӳɹ�
 * \brief    link_ok_display
 * \param[in]   ��
 * \return      ��
 */
void link_on_display(void)
{
	led_seg_en_seg_arr[SEG_SIGNAL_K1] = 1;
}
/**   2.4G����ʧ��
 * \brief    link_ok_display
 * \param[in]   ��
 * \return      ��
 */
void link_off_display(void)
{
	led_seg_en_seg_arr[SEG_SIGNAL_K1] = 0;
}

/**   ͨ��A��ʾ
 * \brief    channel_A_display
 * \param[in]   ��
 * \return      ��
 */
void channel_A_display(u8 state)
{
	led_seg_en_seg_arr[SEG_CHANNEL_A_K2] = state;
}
/**   ���״̬��ʾ ----
 * \brief    channel_A_display
 * \param[in]   ��
 * \return      ��
 */
void display_1_grid(void)
{
	led_seg_en_seg_arr[SEG_G1] = 1;
}
void display_2_grid(void)
{
	led_seg_en_seg_arr[SEG_G2] = 1;
}
void display_3_grid(void)
{
	led_seg_en_seg_arr[SEG_G3] = 1;
}
void display_4_grid(void)
{
	led_seg_en_seg_arr[SEG_G4] = 1;
}
void enter_pair_statu_display(void)
{
	led_seg_en_seg_arr[SEG_G1] = 1;
	led_seg_en_seg_arr[SEG_G2] = 1;
	led_seg_en_seg_arr[SEG_G3] = 1;
	led_seg_en_seg_arr[SEG_G4] = 1;
}

/**   ͨ��A��ʾ
 * \brief    channel_A_display
 * \param[in]   ��
 * \return      ��
 */
void channel_B_display(u8 state)
{
	led_seg_en_seg_arr[SEG_CHANNEL_B_K4] = state;
}

/**   �ϵ���ʾ
 * \brief    power_off_display
 * \param[in]   ��
 * \return      ��
 */
void power_off_display(void)
{
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
	seg_number_arr_write(led_seg_en_seg_arr,SEG_NUMBER_NO_DISPLAY,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBERF,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBERF);
}

void device_ERR_display(void)
{
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
	seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERE,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_NUMBER_NO_DISPLAY);
}

void device_frequency_display(u32 freq)
{
    char * disply=NULL;
//    log_info("displayfreq1=[%08x],displayfreq1=[%d]",freq,freq);
    char strfreq[5]={0};
    disply=strfreq;
    memset(strfreq,0,sizeof(strfreq));
   sprintf(disply,"%d",freq);
	u8 disply0 = strfreq[0] - 0x30;
	u8 disply1 = strfreq[1] - 0x30;
	u8 disply2 = strfreq[2] - 0x30;
	u8 disply3 = strfreq[3] - 0x30;
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
	seg_number_arr_write(led_seg_en_seg_arr,disply0,disply1,disply2,SEG_DP_DISPLAY,disply3);
}

void device_version1_display(char* ver)
{
   // char * disply=NULL;
  //  log_info("ver =[%s]",ver);
    char strver[5]={0};
	u8 disply1 =0;
    memset(strver,0,sizeof(strver));
    memcpy(strver,ver,sizeof(strver));
 //  sprintf(disply,"%d",ver);
  //  log_info("strver =[%s] strver[1]=[%d]",strver,strver[1]);
	u8 disply0 = SEG_DISPLAY_NUMBERV ;
	if (strver[1] == 65)
	{
		disply1 = SEG_DISPLAY_NUMBERA ;
	}
	else{
		disply1 = SEG_DISPLAY_NUMBERB;
	}
	u8 disply2 = strver[2] - 0x30;
	u8 disply3 = strver[3] - 0x30;
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
	seg_number_arr_write(led_seg_en_seg_arr,disply0,disply1,disply2,SEG_DP_NO_DISPLAY,disply3);
}


void device_projectno_display(char* projectno)
{
    char strprojectno[5]={0};
    memset(strprojectno,0,sizeof(strprojectno));
    memcpy(strprojectno,projectno,sizeof(strprojectno));
    u8 disply0 =strprojectno[0] - 0x30;
    u8 disply1 =strprojectno[1] - 0x30;
	u8 disply2 = strprojectno[2] - 0x30;
	u8 disply3 = strprojectno[3] - 0x30;
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
	seg_number_arr_write(led_seg_en_seg_arr,disply0,disply1,disply2,SEG_DP_NO_DISPLAY,disply3);
}

void device_mute_display()
{
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
	seg_number_arr_write(led_seg_en_seg_arr,SEG_NUMBER_NO_DISPLAY,SEG_DISPLAY_CHARMUTE,SEG_DISPLAY_CHARMUTE,SEG_DP_NO_DISPLAY,SEG_NUMBER_NO_DISPLAY);
}

void device_version_display(u8 version)
{
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
	switch (version)
	{
	case 0:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER0);
		break;
	case 1:
		seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER1);
		break;
	case 2:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER2);
		break;
	case 3:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER3);
		break;
	case 4:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER4);
		break;
	case 5:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER5);
		break;
	case 6:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER6);
		break;
	case 7:
		seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER7);
		break;
	case 8:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER8);
		break;
	case 9:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER9);
		break;
	default:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER1);
		break;
	}

}

void device_ch_display(u8 ch)
{
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
	switch (ch)
	{
	case 0:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER0);
		break;
	case 1:
		seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER1);
		break;
	case 2:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER2);
		break;
	case 3:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER3);
		break;
	case 4:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER4);
		break;
	case 5:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER5);
		break;
	case 6:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER6);
		break;
	case 7:
		seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER7);
		break;
	case 8:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER8);
		break;
	case 9:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER9);
		break;
    	case 10:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER0);
		break;
	case 11:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER1);
		break;
	case 12:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER2);
		break;
	case 13:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER3);
		break;
	case 14:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER4);
		break;
	case 15:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER5);
		break;
	case 16:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER6);
		break;
	case 17:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER7);
		break;
	case 18:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER8);
		break;
	case 19:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER1,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER9);
		break;
	case 20:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER0);
		break;
	case 21:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER1);
		break;
	case 22:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER2);
		break;
	case 23:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER3);
		break;
	case 24:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER4);
		break;
	case 25:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER5);
		break;
	case 26:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER6);
		break;
	case 27:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER7);
		break;
	case 28:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER8);
		break;
	case 29:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER2,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER9);
		break;
	case 30:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER0);
		break;
	case 31:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER1);
		break;
	case 32:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER2);
		break;
	case 33:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER3);
		break;
	case 34:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER4);
		break;
	case 35:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER5);
		break;
	case 36:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER6);
		break;
	case 37:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER7);
		break;
	case 38:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER8);
		break;
	case 39:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER3,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER9);
		break;
	case 40:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER0);
		break;
	case 41:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER1);
		break;
	case 42:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER2);
		break;
	case 43:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER3);
		break;
	case 44:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER4);
		break;
	case 45:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER5);
		break;
	case 46:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER6);
		break;
	case 47:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER7);
		break;
	case 48:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER4,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER8);
		break;
	default:
        seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_NUMBER0,SEG_DP_NO_DISPLAY,SEG_DISPLAY_NUMBER1);
		break;
	}

}

void device_ch_displayF002(u8 ch)
{
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
    seg_number_arr_write(led_seg_en_seg_arr,SEG_DISPLAY_NUMBERC,SEG_DISPLAY_NUMBERH,SEG_DISPLAY_CHARMUTE,SEG_DP_NO_DISPLAY,ch);
}
/**   Ϣ��
 * \brief    seg_display_none
 * \param[in]   ��
 * \return      ��
 */
void seg_display_none(void)
{
	memset(&led_seg_en_seg_arr,0,sizeof(led_seg_en_seg_arr));
}
/**   ���ֶ�ȫ��ʾ
 * \brief    seg_display_none
 * \param[in]   ��
 * \return      ��
 */
void seg_reset_uid_display(void)
{
	led_seg_en_seg_arr[SEG_A1] = 1;
	led_seg_en_seg_arr[SEG_B1] = 1;
	led_seg_en_seg_arr[SEG_C1] = 1;
	led_seg_en_seg_arr[SEG_D1] = 0;
	led_seg_en_seg_arr[SEG_E1] = 1;
	led_seg_en_seg_arr[SEG_F1] = 1;
	led_seg_en_seg_arr[SEG_G1] = 1;

	led_seg_en_seg_arr[SEG_A2] = 1;
	led_seg_en_seg_arr[SEG_B2] = 0;
	led_seg_en_seg_arr[SEG_C2] = 1;
	led_seg_en_seg_arr[SEG_D2] = 1;
	led_seg_en_seg_arr[SEG_E2] = 0;
	led_seg_en_seg_arr[SEG_F2] = 1;
	led_seg_en_seg_arr[SEG_G2] = 1;

	led_seg_en_seg_arr[SEG_A3] = 1;
	led_seg_en_seg_arr[SEG_E3] = 1;
	led_seg_en_seg_arr[SEG_F3] = 1;
}
/** �ر�����������ʾ
 * \brief    seg_display_none
 * \param[in]   ��
 * \return      ��
 */
void number_none_display(void)
{
	seg_number_arr_write(led_seg_en_seg_arr,SEG_NUMBER_NO_DISPLAY,SEG_NUMBER_NO_DISPLAY,SEG_NUMBER_NO_DISPLAY,SEG_DP_NO_DISPLAY,SEG_NUMBER_NO_DISPLAY);

}



void led_seg_loop_bilnk(u8 *type)
{
	for(u8 i=0;i<SEG_PIN_MAX;i++)led_seg_pin_set_mode(pin_arr[i],type[i]);//һ������߸������IO״̬
}


void led_seg_reset(void)
{
	for(u8 i=0;i<SEG_PIN_MAX;i++)led_seg_blink_arr[i]=PIN_OUT_DISABLE;
}


void led_seg_frame_display(u8 *en_blink_seg_arr)  //�����֡��ʾ
{
		static u8 led_seg_loop_status = 0;
		led_seg_reset();
		led_seg_loop_bilnk(led_seg_blink_arr);
		for(u8 pin_index = 0; pin_index < SEG_PIN_MAX; pin_index++)
		{
			if(pin_index > led_seg_loop_status)led_seg_blink_arr[pin_index] = en_blink_seg_arr[led_seg_loop_status*6+pin_index-1];
			else if(pin_index == led_seg_loop_status) led_seg_blink_arr[pin_index] = PIN_OUT_LOW;  	//���͵�ƽ
			else if(pin_index < led_seg_loop_status) led_seg_blink_arr[pin_index] = en_blink_seg_arr[led_seg_loop_status*6+pin_index]; 	//��ֵ
		}
		led_seg_loop_bilnk(led_seg_blink_arr);	//���������
		if(++led_seg_loop_status >= SEG_PIN_MAX )led_seg_loop_status =0;

}


void led_seg_display_timer_callback(void)  //������ַ���ʾ
{
	 led_seg_frame_display(led_seg_en_seg_arr);
}


