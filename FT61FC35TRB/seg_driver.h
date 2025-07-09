
#ifndef __SEG_DRIVER_H
#define __SEG_DRIVER_H


/* Includes ------------------------------------------------------------------*/

#include	"FT61FC3x_GPIO_INIT.H"
#include 	"FT61FC35TRB_CONFIG.H"


#define LED_SEF_1       GPIO_PB1
#define LED_SEF_2       GPIO_PB2
#define LED_SEF_3       GPIO_PB3
#define LED_SEF_4       GPIO_PB4
#define LED_SEF_5       GPIO_PB5
#define LED_SEF_6       GPIO_PB6
#define LED_SEF_7       GPIO_PB7

//��һ��
#define SEG_F1 0
#define SEG_G1 1
#define SEG_C1 2
#define SEG_D1 3
#define SEG_D3 4
#define SEG_SIGNAL_K1 5
//�ڶ���
#define SEG_A1 6
#define SEG_F2 7
#define SEG_G2 8
#define SEG_C2 9
#define SEG_FRE_K3 10
#define SEG_CHANNEL_B_K4 11
//������
#define SEG_B1 12
#define SEG_A2 13
#define SEG_F3 14
#define SEG_G3 15
#define SEG_E3 16
#define SEG_POWER_BORD1_J1 17
//������
#define SEG_E1 18
#define SEG_B2 19
#define SEG_A3 21
#define SEG_D4 22
//������
#define SEG_CHANNEL_A_K2 24
#define SEG_E2 25
#define SEG_B3 26
#define SEG_C3 27
#define SEG_F4 28
#define SEG_DP 29
//������
#define SEG_POWER_BORD2_J2 30
#define SEG_D2 31
#define SEG_POWER_BORD3_J3 32
#define SEG_E4 33
#define SEG_C4 34
#define SEG_A4 35
//������
#define SEG_POWER_GRID3_K6 36
#define SEG_POWER_GRID2_K7 37
#define SEG_POWER_BORD4_J4 38
#define SEG_POWER_GRID1_K8 39
#define SEG_G4 40
#define SEG_B4 41

/**
 * @brief   �����������ʾ�ַ�
 */
typedef enum{
	SEG_DISPLAY_NUMBER0 = 0,
	SEG_DISPLAY_NUMBER1 = 1,
	SEG_DISPLAY_NUMBER2 = 2,
	SEG_DISPLAY_NUMBER3 = 3,
	SEG_DISPLAY_NUMBER4 = 4,
	SEG_DISPLAY_NUMBER5 = 5,
	SEG_DISPLAY_NUMBER6 = 6,
	SEG_DISPLAY_NUMBER7 = 7,
	SEG_DISPLAY_NUMBER8 = 8,
	SEG_DISPLAY_NUMBER9 = 9,
	SEG_DISPLAY_NUMBERA = 10,
	SEG_DISPLAY_NUMBERB = 11,
	SEG_DISPLAY_NUMBERC = 12,
	SEG_DISPLAY_NUMBERD = 13,
	SEG_DISPLAY_NUMBERE = 14,
	SEG_DISPLAY_NUMBERF = 15,
	SEG_DISPLAY_NUMBERH  = 16,
	SEG_DISPLAY_NUMBERV  = 17,
	SEG_DISPLAY_CHARMUTE = 18,
	SEG_NUMBER_NO_DISPLAY = 19,

}SEG_NUMBERTypeDef;
/**
 * @brief   ����С�����������ʾ����
 */
typedef enum{

	SEG_DP_NO_DISPLAY	= 0,
	SEG_DP_DISPLAY	= 1,
}SEG_DPTypeDef;
/**
 * @brief   ͨ��A�������ʾ����
 */
typedef enum{
	SEG_CHANNEL_A_DISPLAY	= 1,
	SEG_CHANNEL_A_NO_DISPLAY	= 2,
}SEG_CHANNEL_ATypeDef;
/**
 * @brief   ͨ��B�������ʾ����
 */
typedef enum{
	SEG_CHANNEL_B_DISPLAY	= 1,
	SEG_CHANNEL_B_NO_DISPLAY	= 2,

}SEG_CHANNEL_BTypeDef;
/**
 * @brief   ��ص����������ʾ����
 */
typedef enum{
	SEG_POWER_NO_DISPLAY = 0,
	SEG_POWER_0	= 1,
	SEG_POWER_1	= 2,
	SEG_POWER_2 = 3,
	SEG_POWER_3 = 4,
}SEG_POWERTypeDef;
/**
 * @brief   �ź�ǿ���������ʾ����
 */
typedef enum{
	SEG_SIGNAL_DISPLAY	= 1,
	SEG_SIGNAL_NO_DISPLAY	= 2,
}SEG_SIGNALTypeDef;
/**
 * @brief   Ƶ���������ʾ����
 */
typedef enum{
	SEG_FRE_DISPLAY	= 1,
	SEG_FRE_NO_DISPLAY	= 2,
}SEG_FRETypeDef;


void board_seg_init(void);
void led_seg_display_timer_callback(void);  //������ַ���ʾ
void led_seg_pin_set_mode(u8 pin_index,u8 pin_mode);

void display_signal_fre(SEG_FRETypeDef fre);

void seg_production_test(void);

void display_1_grid(void);
void display_2_grid(void);
void display_3_grid(void);
void display_4_grid(void);
void err_display(void);
void ota_display(void);
void ota_dynamic_display(u8 seg);

void number_none_display(void);
void enter_pair_statu_display(void);
void channel_select_init(void);
void power_on_display(void);
void power_off_display(void);
void device_ERR_display(void);
void link_off_display(void);
void link_on_display(void);
void channel_A_display(u8 state);
void channel_B_display(u8 state);
void seg_display_none(void);
void seg_reset_uid_display(void);

void device_version_display(u8 version);
void device_version1_display(char* ver);
void device_projectno_display(char* projectno);
void device_mute_display();

void device_ch_display(u8 ch);
void device_ch_displayF002(u8 ch);
void seg_power_arr_write(u8 *write_seg_arr,SEG_POWERTypeDef power);

void seg_number_arr_write(u8 *write_seg_arr,SEG_NUMBERTypeDef seg_number_1,SEG_NUMBERTypeDef seg_number_2,SEG_NUMBERTypeDef seg_number_3,SEG_DPTypeDef seg_DP,SEG_NUMBERTypeDef seg_number_4);  //���������֡��ʾ
void device_frequency_display(u32 freq);
void seg_channel_power_signal_fre_arr_write(u8 *write_seg_arr,SEG_CHANNEL_ATypeDef channelA,SEG_CHANNEL_BTypeDef channelB,SEG_POWERTypeDef power, SEG_FRETypeDef fre);


#endif
