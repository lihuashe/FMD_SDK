//Project: ft61fc3x_src_f002.prj
// Device: FT61FC3X
// Memory: Flash 3KX14b, EEPROM 128X8b, SRAM 256X8b
// Author: 
//Company: 
//Version:
//   Date: 
//===========================================================
//                  FT61FC35  SOP20
//                 ----------------
//  GND -------------|1(GND)   (VDD)20|------------VDD     
// ISP_DAT ----------|2(PC1)   (PA0)19|------------[A] battery
// ISP_CLK  ---------|3(PC0)   (PA1)18|------------[O] POWER_ON
//  D7[0]------------|4(PB7)   (PA2)17|-----------	[I] ON/OFF
//  D6[O]------------|5(PB6)   (PA3)16|------------[I] POWER_KEY
//  D5[O]------------|6(PB5)   (PA4)15|----------- [I] VOL-
//  D4[O]------------|7(PB4)   (PA5)14|------------[I] VOL+
//  D3[O]------------|8(PB3)   (PA6)13|------------SCL
//  D2[O]------------|9(PB2)   (PA7)12|------------SDA
//  D1[O]------------|10(PB1)  (PB0)11|------------[O]U_EN
//                 ----------------   
//===========================================================

#include <syscfg.h>
#include "SD1516/sd1516.h"
#include "KT0646M/microphone.h"

#define VOL_UP		PA5
#define VOL_DOWN	PA4
#define POWER_KEY	PA3
#define POWER_ON	PA1

//===========================================================
//Variable definition
//===========================================================

//===========================================================
//Function name��interrupt ISR
//parameters����
//returned value����
//===========================================================
void interrupt ISR(void)
{
	if(TMR1IF)                     
	{
		TMR1IF = 0;                 //�����жϱ�־λ
		timer1_isr(); 				//�����ж�
	} 	
}

void delya_2us(uint8_t us)
{
	unsigned char a;
	for(a = 0; a < us; a++)
	{
		NOP();
	}
}

void delay_ms(uint16_t ms)
{
	for (uint16_t i = 0; i < ms; i++) {
		for(uint8_t j = 0; j < 250; j++) {
			NOP();
		}
	}
}


void io_init(void)
{
	//PA����IO�ڳ�ʼ��
	OSCCON = 0B01110001;	//IRCF=111=16MHz/4T=4MHz,0.25us		
	OPTION = 0B00001000;	 		
	INTCON = 0;  			//�ݽ�ֹ�����ж�
	PORTA  = 0B00000001;		
	TRISA  = 0B00111000;	//���ð�����ʼ����io�ڳ�ʼ��
    WPUA = 0B00111000;

	ADCON1 = 0B10100101; 	//�Ҷ��룬ת��ʱ��SysClk/32�����ο���ѹGND�����ο���ѹVDD
  	//Bit7:ADFM A/Dת�������ʽ 
	//1 = �Ҷ��롣 
	//0 = ����롣
  
  	//Bit6~Bit4:ADCS ADCת��ʱ��Դ
    //TSEL=2T
	//000 = SysClk/2
	//001 = SysClk/8
	//010 = SysClk/32
	//011 = SysClk
	//100 = SysClk/4
	//101 = SysClk/16
	//110 = SysClk/64
	//111 = LIRC
	
	//Bit3~Bit2:ADNREF ADC���ο���ѹ
	//00 = Int Vref���ڲ��ο���ѹ��
	//01 = GND
	//10 = Int Vref + Ext Cap���ڲ��ο���ѹ + �ⲿ���ݣ�
	//11 = Ext Vref���ⲿ�ο���ѹ��

  	//Bit1~Bit0:ADPREF ADC���ο���ѹ
	//00 = Int Vref���ڲ��ο���ѹ��
	//01 = VDD
	//10 = Int Vref + Ext Cap���ڲ��ο���ѹ + �ⲿ���ݣ�
	//11 = Ext Vref���ⲿ�ο���ѹ��
                             
	ADCON0 = 0B00000000; 	
  	//Bit6~Bit3:CHS ADCģ��ͨ��ѡ��λ
	//0000 = AN0
	//0001 = AN1
	//0010 = AN2
	//0011 = AN3
	//0100 = AN4
	//0101 = AN5
	//0110 = AN6
	//0111 = AN7
	//1xxx = Int Vref���ڲ��ο���ѹ��

    //Bit2:ADEX	 ADC��������
	//0 = ����λָ��GO/DONE�����������
	//1 = ��PA4��PWM��λG0/DONE��Ӳ��������
		
    //Bit1:G0/DONE ADCת��������״̬λ
    //0 = A/Dת�����/δ����ת��
	//1 = A/Dת�����ڽ��л�Ӳ��������ʱ���ڼ���

    //Bit0:ADON ʹ��ADC  
    //0 = ADC����ֹ�Ҳ����Ĺ�������
	//1 = ADC��ʹ��
              

	delya_2us(200);	
    delya_2us(250);           //����Ҫ��ʱ450us
    			   			 			
    ADCON2 = 0B01000000; 	//ѡ���ڲ����ο���ѹ2V�����ⲿ����Դ
    //Bit7~Bit6:ADINTREF ADC�ڲ��ο���ѹ
	//00 = 0.5V
	//01 = 2.0V
	//10 = 3.0V
	//11 = δ����
		
	//Bit5~Bit4:ETGTYP �ⲿ�����أ���ADEX=1ʱ���ã�
	//00 = PWM �� ADC_ETR�ŵ��½���
	//01 = PWM �� ADC_ETR�ŵ�������
		
	//Bit3:ADDLY.8/LEBPR9 ADC�ⲿ������ʱ��������LEB�������ĵ�8λ
		
	//Bit2~Bit0:ETGSEL �ⲿ����Դ����ADEX=1ʱ���ã�
	//ѡ��PWMԴʱ��Ҫ����TIMERΪPWM���ģʽ��ʹ�������
	//000 = P1A0
	//001 = P1A0N
	//010 = P1B
	//011 = P1C
	//100 = P1D
	//101 = ADC_ETR
	//11x = ��


    ADCON3 = 0B00000000; 
    //Bit7:ADFBEN ADC�ȽϽ����Ӧ����ɲ��ʹ��
	//0 = ��ֹ
	//1 = ʹ��
		
	//Bit6:ADCMPOP ADC�Ƚ����������
	//0 = ADC����ĸ߰�λ���ڻ����ADCMPH[7:0]
	//1 = ADC����ĸ߰�λС��ADCMPH[7:0]

	//Bit5:ADCMPEN ADC����Ƚ�ʹ��λ
	//0 = �ر�
	//1 = ʹ��
		
	//Bit3:LEBADT ǰ���������ڽ�����ADC����ʹ��
	//1 = ����ADCת��
	//0 = ������ADCת��

    ADCMPH = 0B00000000;	//ADC�Ƚ���ֵ,��8λ������ADC�����8λ�Ƚ�

    ADON=1;                 //ʹ��ADC
}

//===========================================================
//Function name��main
//parameters����
//returned value����
//===========================================================
static uint8_t key_down = 0;
static uint8_t key_hold = 0;
static uint8_t key_read = 0;

static uint16_t power_key_ticks = 0;
main()
{
	delay_ms(5);
	uint8_t i,j;

	/** ����Ĭ��IO״̬ */
	TRISB0 = 0;	// KT0646.EN, ���
	PB0	= 1;	// KT0646.EN, Ĭ�ϲ�ʹ��
	
    OPTION = 0B00001000;
	PORTA  = 0B00000000;		
	TRISA  = 0B00111000;	//���ð�����ʼ����io�ڳ�ʼ��
    WPUA = 0B00111000;


	/** �������: ���� >0.5s,���� */
	i = 0;
	do {
	delay_ms(10);
	i++;
	} while(POWER_KEY == 0 && i < 50);
	if (i < 50) {
	while(1);	// ����ʱ��<0.5s, ������;
	}
	/** MCU����������� */
	POWER_ON = 1;	// �����ɹ�,����ߵ�ƽ���ֵ�Դ����;

	/** microphon��ʼ�� */
	// microphone_init();

	/** ��ʾ��ʼ�� */
	sd1516_init();
	disp.digits[0] = 'c';
	disp.digits[1] = 'h';
	disp.digits[2] = ' ';
	disp.digits[3] = g_config.channel + 0x30;
	disp.status.cha = 1;
	disp.status.mhz = 1;
	disp.status.signal = 1;
	disp.status.level = 3;
	sd1516_update_display();
	/** microphon��ʼ�� */
	microphone_init();

	/** ���������쳣 */
	while(1)
	{
		/** ����ɨ�� */
		key_read = 0;
		key_read = (POWER_KEY << 2) | (VOL_UP << 1) | (VOL_DOWN);
		// ��ⰴ�����£��͵�ƽ��Ч���½��أ�
		key_down = (~key_hold) & key_read; 
		key_hold = key_read;
		/** ������ */
		// ����-
		if (key_down & 0x01) {
			disp.digits[2] = '1';
			sd1516_update_display();
			// microphone_vol_down();
		}
		// ����+
		if (key_down & 0x02) {
			disp.digits[2] = '2';
			sd1516_update_display();
			// microphone_vol_up();
		}
		// power
		if (key_down & 0x04) {
			disp.digits[2] = '4';
			power_key_ticks = 0;
		}
		if ((key_hold & 0x04) == 0) {
			power_key_ticks++;
			// ��Դ������2S
			if (power_key_ticks > 40) {
				/** �ػ����� */
				// // 1. BPSK���͹ػ�ָ��,������ն�pop��,ͬʱ��ʾOFF
				// microphone_bpsk_poweroff();
				// // 2. ���浱ǰ״̬
				// microphone_save_config();
				// // 3. ��ʾoff,�ȴ�500ms
				sd1516_clear();
				sd1516_draw_digit(1, '0');
				sd1516_draw_digit(2, '0');
				sd1516_draw_digit(3, 'f');
				delay_ms(500);
				// 4. KT0646�ػ�
				// KT_WirelessMicTx_PowerDownProcedure();
				// 5. MCU�ͷŵ�ԴHOLD
				POWER_ON = 0;
				// 6. �ȴ��û��ͷŵ�Դ����,�����ϵ�
				while(POWER_KEY == 0)
				{
					;
				}
				// 7. ��Ӧ�õ�����
				while(1);
			}
		}

		delay_ms(50);
	}
}
//===========================================================
