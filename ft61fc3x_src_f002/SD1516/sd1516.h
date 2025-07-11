#ifndef SD1516_H
#define SD1516_H

#include "syscfg.h"
#include "stdint.h"

/**  LED��ʾ����(r->c)
 *       P1  P2  P3  P4  P5  P6  P7
 * ----��----------------------------
 *  P1 | 0   1F  1G  1C  1D  3D  K1 
 *  P2 | 1A  0   2F  2G  2C  K3  K4
 *  P3 | 1B  2A  0   3F  3G  3E  J1
 *  P4 | 1E  2B      0   3A  4D    
 *  P5 | K2  2E  3B  3C  0   4F  DP
 *  P6 | J2  2D  J3  4E  4C  0   4A
 *  P7 | K6  K7  J4  K8  4G  4B  0
 * 
*/
#define DISP_DIGITAL_1A    0x10
#define DISP_DIGITAL_1B    0x20
#define DISP_DIGITAL_1C    0x03
#define DISP_DIGITAL_1D    0x04
#define DISP_DIGITAL_1E    0x30
#define DISP_DIGITAL_1F    0x01
#define DISP_DIGITAL_1G    0x02

#define DISP_DIGITAL_2A    0x21
#define DISP_DIGITAL_2B    0x31
#define DISP_DIGITAL_2C    0x14
#define DISP_DIGITAL_2D    0x51
#define DISP_DIGITAL_2E    0x41
#define DISP_DIGITAL_2F    0x12
#define DISP_DIGITAL_2G    0x13

#define DISP_DIGITAL_3A    0x34
#define DISP_DIGITAL_3B    0x42
#define DISP_DIGITAL_3C    0x43
#define DISP_DIGITAL_3D    0x05
#define DISP_DIGITAL_3E    0x25
#define DISP_DIGITAL_3F    0x23
#define DISP_DIGITAL_3G    0x24

#define DISP_DIGITAL_4A    0x56
#define DISP_DIGITAL_4B    0x65
#define DISP_DIGITAL_4C    0x54
#define DISP_DIGITAL_4D    0x35
#define DISP_DIGITAL_4E    0x53
#define DISP_DIGITAL_4F    0x45
#define DISP_DIGITAL_4G    0x64

#define DISP_SIGNAL_K1     0x06
#define DISP_CHA_K2        0x40
#define DISP_MHZ_K3        0x15
#define DISP_CHB_K4        0x16
#define DISP_L3_K6         0x60
#define DISP_L2_K7         0x61
#define DISP_L1_K8         0x63
#define DISP_BAT_J1        0x26
#define DISP_BAT_J2        0x50
#define DISP_BAT_J3        0x52
#define DISP_BAT_J4        0x62

#define DISP_DP            0x56

// 7������ܶ�λ���� (�����Զ�����ʾ)
#define SEG_A   0x01    // bit 0 - �϶�
#define SEG_B   0x02    // bit 1 - ���϶�  
#define SEG_C   0x04    // bit 2 - ���¶�
#define SEG_D   0x08    // bit 3 - �¶�
#define SEG_E   0x10    // bit 4 - ���¶�
#define SEG_F   0x20    // bit 5 - ���϶�
#define SEG_G   0x40    // bit 6 - �ж�


/**
 * @struct display_context
 * @brief Structure representing the context for display operations.
 *
 */
typedef struct 
{
    uint8_t digits[4];      // 4λ�������ʾ����
    struct 
    {   
        uint8_t             :2;     // reserved
        uint8_t level       :2;     // ��ص������� (0-3)
        uint8_t signal      :1;     // �ź�ָʾ��
        uint8_t cha         :1;     // CHAͨ��ָʾ
        uint8_t chb         :1;     // CHBͨ��ָʾ
        uint8_t mhz         :1;     // MHZ��λָʾ
        uint8_t dp          :1;     // С����ָʾ
    }status;                 
}display_context_t;


extern display_context_t disp;

// ��������
void sd1516_init(void);
void timer1_isr(void);

void sd1516_draw_digit(uint8_t digit_pos, char c);

void sd1516_update_display(void);

void sd1516_draw_custom_pattern(uint8_t digit_pos, uint8_t pattern);

void sd1516_clear();
// ʹ��ʾ��:
// sd1516_show_number(1234);           // ��ʾ����1234
// sd1516_draw_digital('1', '2', '3', '4');  // ��ʾ1234
// sd1516_draw_digital('A', 'b', 'C', 'd');  // ��ʾAbCd
// sd1516_draw_digital(' ', '1', '2', '3');  // ��ʾ 123����һλ�ո�
// sd1516_draw_custom_pattern(0, SEG_A|SEG_B|SEG_C); // �Զ�����ʾģʽ
// sd1516_clear_all_digits();          // ���������ʾ

#endif // SD1516_H