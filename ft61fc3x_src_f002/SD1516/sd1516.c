#include "sd1516.h"
#include "stdint.h"

#define DISPLAY_REFRESH_RATE   58
#define DISPLAY_REFRESH_TIMER_FREQ    DISPLAY_REFRESH_RATE * 7


/**
 * @brief  7-segment display digit and character patterns.
 *
 * This constant array defines the segment patterns for displaying digits (0-9),
 * some letters (A-F, b, C, d, E, F), and a blank space on a 7-segment display.
 * Each element corresponds to a value to be displayed, with bits representing
 * which segments (A-G) should be illuminated.
 *
 * Segment bit definitions (example):
 *   SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G
 *
 * Index mapping:
 *   [0]  = '0'
 *   [1]  = '1'
 *   [2]  = '2'
 *   [3]  = '3'
 *   [4]  = '4'
 *   [5]  = '5'
 *   [6]  = '6'
 *   [7]  = '7'
 *   [8]  = '8'
 *   [9]  = '9'
 *   [10] = 'A'
 *   [11] = 'b'
 *   [12] = 'C'
 *   [13] = 'd'
 *   [14] = 'E'
 *   [15] = 'F'
 *   [16] = ' ' (blank, all segments off)
 *
 * Each pattern is a bitwise OR of the SEG_* macros, indicating which segments
 * should be turned on for the corresponding character.
 */
static const uint8_t digit_patterns[19] = {
    // ���� 0: A,B,C,D,E,F��ʾ, G����ʾ
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
    // ���� 1: B,C��ʾ
    SEG_B | SEG_C,
    // ���� 2: A,B,D,E,G��ʾ
    SEG_A | SEG_B | SEG_D | SEG_E | SEG_G,
    // ���� 3: A,B,C,D,G��ʾ
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,
    // ���� 4: B,C,F,G��ʾ
    SEG_B | SEG_C | SEG_F | SEG_G,
    // ���� 5: A,C,D,F,G��ʾ
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
    // ���� 6: A,C,D,E,F,G��ʾ
    SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    // ���� 7: A,B,C��ʾ
    SEG_A | SEG_B | SEG_C,
    // ���� 8: ȫ����ʾ
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    // ���� 9: A,B,C,D,F,G��ʾ
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,
    // ��ĸ A: A,B,C,E,F,G��ʾ
    SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
    // ��ĸ b: C,D,E,F,G��ʾ
    SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    // ��ĸ C: A,D,E,F��ʾ
    SEG_A | SEG_D | SEG_E | SEG_F,
    // ��ĸ d: B,C,D,E,G��ʾ
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,
    // ��ĸ E: A,D,E,F,G��ʾ
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    // ��ĸ F: A,E,F,G��ʾ
    SEG_A | SEG_E | SEG_F | SEG_G,
    // ��ĸ G: A,E,F,G��ʾ
    SEG_A | SEG_C | SEG_D | SEG_E | SEG_F,
    // ��ĸ H: A,E,F,G��ʾ
    SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,

    // �ո�: ȫ������ʾ
    0x00
};

/**
 * @brief Lookup table for 7-segment display encoding of digits.
 *
 * This 2D array defines the segment patterns for displaying digits on a 7-segment display.
 * Each row corresponds to a digit, and each column represents the state (on/off) of a segment.
 *
 * @note The array is organized as [digit][segment], where:
 *       - digit: index of the digit (0-3)
 *       - segment: index of the segment (0-6)
 *
 * @see The actual segment mapping depends on the hardware wiring.
 */
static const uint8_t digit_segments[4][7] = {
    {DISP_DIGITAL_1A, DISP_DIGITAL_1B, DISP_DIGITAL_1C, DISP_DIGITAL_1D, DISP_DIGITAL_1E, DISP_DIGITAL_1F, DISP_DIGITAL_1G},
    {DISP_DIGITAL_2A, DISP_DIGITAL_2B, DISP_DIGITAL_2C, DISP_DIGITAL_2D, DISP_DIGITAL_2E, DISP_DIGITAL_2F, DISP_DIGITAL_2G},
    {DISP_DIGITAL_3A, DISP_DIGITAL_3B, DISP_DIGITAL_3C, DISP_DIGITAL_3D, DISP_DIGITAL_3E, DISP_DIGITAL_3F, DISP_DIGITAL_3G},
    {DISP_DIGITAL_4A, DISP_DIGITAL_4B, DISP_DIGITAL_4C, DISP_DIGITAL_4D, DISP_DIGITAL_4E, DISP_DIGITAL_4F, DISP_DIGITAL_4G}
};


display_context_t disp;

static uint8_t scan_row_index = 0;
static uint8_t matrix_buffer[7];

void sd1516_init(void)
{
    // ϵͳʱ��ѡ��Ϊ�ڲ�����16MHz,��Ƶ��Ϊ1:1
    OSCCON = 0b01110001;
    
    /* ������ʾˢ�¶�ʱ��Timer1 */

    // ϵͳʱ��ѡ��Ϊ�ڲ�����16MHz,��Ƶ��Ϊ1:1
    OSCCON = 0b01110001;

    // HIRC = 16M,Ԥ��Ƶϵ��=16
    uint16_t period = (16000000 / 16 + (DISPLAY_REFRESH_TIMER_FREQ - 1)) / DISPLAY_REFRESH_TIMER_FREQ;

    /** 1. ����Timer1 + Timer2ʱ��Դ,Ԥ��Ƶϵ��=4
     * T1CON0
     *   7:5    - N/A
     *   4      - T1CKPSA,Ԥ��Ƶ������λ��1 = Timer1, 0 = Timer2
     *   3      - T1CKRUN,SLEEPģʽ��T1CK�������У�1 = Yes(ʱ��Դ��ָ��ʱ��), 0 = No
     *   2      - T1ON, Timer1ģ�飺1 = ʹ��, 0 = �ر�
     *   1:0    - Timer1ʱ��Դ��01 = HIRC
    */
    T1CON0 = 0b00010001; // Timer1ʱ��Դ = HIRC
    T2CKPS0 = 1;
    T2CKPS1 = 1;

    /** 2. ����Timer2���� (12bit)
     * PR1L
     *   7:0    - Timer1���ڼĴ�����8λ
     * PR1H
     *   7:4    - N/A
     *   3:0    - Timer1���ڼĴ�����4λ
    */
    PR1H = (uint8_t)(period >> 8 & 0x000F);
    PR1L = (uint8_t)(period & 0x00FF);

    /** 3. ����Timer1������
     * TMR1L
     *   7:0    - Timer1��������8λ
     * TMR1H
     *   7:4    - N/A
     *   3:0    - Timer1��������4λ
     */
    TMR1L = 0;
    TMR1H = 0;

    /** 4. ʹ��Timer1�ж�
     * PIE1
     *   3      - TMR1IE, Timer1��PR1ƥ���ж�
     */
    TMR1IF = 0;
    TMR1IE = 1;
    T1ON = 1;

    /* ����LED����˿�PB */
    TRISB = 0xFF;  // PB����Ϊ����
    PORTB = 0x00;  // PB���0

    for (int i = 0; i < 7; i++)
    {
        matrix_buffer[0] = 0;
    }
    scan_row_index = 0;

    TRISB = 0x00; // PB�������

    PEIE = 1;  // ʹ�������ж�
    GIE = 1;  // ʹ���ж�
}


void timer1_isr(void)
{
    register uint8_t t = matrix_buffer[scan_row_index];
    // PB0 ���ֲ���
    t = t << 1; 
    t |= PB0;
    // ���row[i];  
    PORTB = t;
    TRISB = ((~t) | PB0) & ~(1 << (scan_row_index + 1));  // PB����Ϊ����
    // next row
    scan_row_index++;
    scan_row_index = (scan_row_index > 6) ? 0 : scan_row_index;
}

void sd1516_set_dot(uint8_t pos)
{
    uint8_t r = (uint8_t)(pos >> 4 & 0x0F);
    uint8_t c = (uint8_t)(pos & 0x0F);
    matrix_buffer[r] |= 1 << c;
}

void sd1516_clr_dot(uint8_t pos)
{
    uint8_t r = (uint8_t)(pos >> 4 & 0x0F);
    uint8_t c = (uint8_t)(pos & 0x0F);
    matrix_buffer[r] &= ~(1 << c);    
}

// ���ַ�ת��Ϊ����
uint8_t char_to_index(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';  // ����0-9
    } else if (c >= 'A' && c <= 'H') {
        return c - 'A' + 10;  // ��ĸA-F
    } else if (c >= 'a' && c <= 'h') {
        return c - 'a' + 10;  // Сд��ĸa-f
    } else if (c == ' ') {
        return sizeof(digit_patterns)-1;  // �ո�ȫ�������
    } else {
        return 0;  // Ĭ����ʾ0
    }
}

// ��ʾ��������/��ĸ��ָ��λ��
void sd1516_draw_digit(uint8_t digit_pos, char c)
{
    uint8_t index = char_to_index(c);
    uint8_t pattern = digit_patterns[index];  // ��ȡ�ַ��Ķ�ģʽ
    
    if (digit_pos > 3) return;  // ���λ����Ч��
    
    // �����λ�õ����ж�
    for (uint8_t i = 0; i < 7; i++) {
        sd1516_clr_dot(digit_segments[digit_pos][i]);
    }
    
    // ����λģʽ��ʾ��Ӧ�Ķ�
    for (uint8_t i = 0; i < 7; i++) {
        if (pattern & (1 << i)) {  // ����Ӧλ�Ƿ�Ϊ1
            sd1516_set_dot(digit_segments[digit_pos][i]);
        }
    }
}

void sd1516_update_display(void)
{
    uint8_t i,m,pos;
    uint8_t pattern,mask;

    for(i = 0; i < 7; i++) {
        matrix_buffer[i] = 0;
    }

    /* ����������ʾ */   
    for (pos = 0; pos < 4; pos++) {
        m = char_to_index(disp.digits[pos]);
        pattern = digit_patterns[m];
        // ����λģʽ��ʾ��Ӧ�Ķ�
        mask = 0x01;
        for (i = 0; i < 7; i++) {
            // ����Ӧλ�Ƿ�Ϊ1
            if (pattern & mask) {  
                sd1516_set_dot(digit_segments[pos][i]);
            }
            mask = mask << 1;
        }
    }

    /* ����״̬ */
    if (disp.status.cha) {
        sd1516_set_dot(DISP_CHA_K2);
    }
    if (disp.status.chb) {
        sd1516_set_dot(DISP_CHB_K4);
    }
    if (disp.status.mhz) {
        sd1516_set_dot(DISP_MHZ_K3);
    }
    if (disp.status.signal) {
        sd1516_set_dot(DISP_SIGNAL_K1);
    }
    
    /* ��ʾ���� */
    sd1516_set_dot(DISP_BAT_J1);
    sd1516_set_dot(DISP_BAT_J2);
    sd1516_set_dot(DISP_BAT_J3);
    sd1516_set_dot(DISP_BAT_J4);
    if (disp.status.level > 0) { 
        sd1516_set_dot(DISP_L1_K8);
    }
    if (disp.status.level > 1) {
        sd1516_set_dot(DISP_L2_K7);
    }
    if (disp.status.level > 2) {
        sd1516_set_dot(DISP_L3_K6);
    }
}

// ֱ��ͨ��λģʽ�����ַ���ʾ
void sd1516_draw_custom_pattern(uint8_t digit_pos, uint8_t pattern)
{
    if (digit_pos > 3) return;
    
    // �����λ�õ����ж�
    for (uint8_t i = 0; i < 7; i++) {
        sd1516_clr_dot(digit_segments[digit_pos][i]);
    }
    
    // ����λģʽ��ʾ��Ӧ�Ķ�
    for (uint8_t i = 0; i < 7; i++) {
        if (pattern & (1 << i)) {
            sd1516_set_dot(digit_segments[digit_pos][i]);
        }
    }
}

void sd1516_clear()
{
    for(char i = 0; i < 7; i++) {
        matrix_buffer[i] = 0;
    }    
}