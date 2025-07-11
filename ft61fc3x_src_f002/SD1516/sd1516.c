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
    // 数字 0: A,B,C,D,E,F显示, G不显示
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
    // 数字 1: B,C显示
    SEG_B | SEG_C,
    // 数字 2: A,B,D,E,G显示
    SEG_A | SEG_B | SEG_D | SEG_E | SEG_G,
    // 数字 3: A,B,C,D,G显示
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,
    // 数字 4: B,C,F,G显示
    SEG_B | SEG_C | SEG_F | SEG_G,
    // 数字 5: A,C,D,F,G显示
    SEG_A | SEG_C | SEG_D | SEG_F | SEG_G,
    // 数字 6: A,C,D,E,F,G显示
    SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    // 数字 7: A,B,C显示
    SEG_A | SEG_B | SEG_C,
    // 数字 8: 全部显示
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    // 数字 9: A,B,C,D,F,G显示
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,
    // 字母 A: A,B,C,E,F,G显示
    SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,
    // 字母 b: C,D,E,F,G显示
    SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    // 字母 C: A,D,E,F显示
    SEG_A | SEG_D | SEG_E | SEG_F,
    // 字母 d: B,C,D,E,G显示
    SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,
    // 字母 E: A,D,E,F,G显示
    SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,
    // 字母 F: A,E,F,G显示
    SEG_A | SEG_E | SEG_F | SEG_G,
    // 字母 G: A,E,F,G显示
    SEG_A | SEG_C | SEG_D | SEG_E | SEG_F,
    // 字母 H: A,E,F,G显示
    SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,

    // 空格: 全部不显示
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
    // 系统时钟选择为内部振荡器16MHz,分频比为1:1
    OSCCON = 0b01110001;
    
    /* 配置显示刷新定时器Timer1 */

    // 系统时钟选择为内部振荡器16MHz,分频比为1:1
    OSCCON = 0b01110001;

    // HIRC = 16M,预分频系数=16
    uint16_t period = (16000000 / 16 + (DISPLAY_REFRESH_TIMER_FREQ - 1)) / DISPLAY_REFRESH_TIMER_FREQ;

    /** 1. 配置Timer1 + Timer2时钟源,预分频系数=4
     * T1CON0
     *   7:5    - N/A
     *   4      - T1CKPSA,预分频器分配位：1 = Timer1, 0 = Timer2
     *   3      - T1CKRUN,SLEEP模式下T1CK保持运行：1 = Yes(时钟源非指令时钟), 0 = No
     *   2      - T1ON, Timer1模块：1 = 使能, 0 = 关闭
     *   1:0    - Timer1时钟源：01 = HIRC
    */
    T1CON0 = 0b00010001; // Timer1时钟源 = HIRC
    T2CKPS0 = 1;
    T2CKPS1 = 1;

    /** 2. 配置Timer2周期 (12bit)
     * PR1L
     *   7:0    - Timer1周期寄存器低8位
     * PR1H
     *   7:4    - N/A
     *   3:0    - Timer1周期寄存器高4位
    */
    PR1H = (uint8_t)(period >> 8 & 0x000F);
    PR1L = (uint8_t)(period & 0x00FF);

    /** 3. 重置Timer1计数器
     * TMR1L
     *   7:0    - Timer1计数器低8位
     * TMR1H
     *   7:4    - N/A
     *   3:0    - Timer1计数器高4位
     */
    TMR1L = 0;
    TMR1H = 0;

    /** 4. 使能Timer1中断
     * PIE1
     *   3      - TMR1IE, Timer1与PR1匹配中断
     */
    TMR1IF = 0;
    TMR1IE = 1;
    T1ON = 1;

    /* 配置LED输出端口PB */
    TRISB = 0xFF;  // PB配置为输入
    PORTB = 0x00;  // PB输出0

    for (int i = 0; i < 7; i++)
    {
        matrix_buffer[0] = 0;
    }
    scan_row_index = 0;

    TRISB = 0x00; // PB配置输出

    PEIE = 1;  // 使能外设中断
    GIE = 1;  // 使能中断
}


void timer1_isr(void)
{
    register uint8_t t = matrix_buffer[scan_row_index];
    // PB0 保持不变
    t = t << 1; 
    t |= PB0;
    // 输出row[i];  
    PORTB = t;
    TRISB = ((~t) | PB0) & ~(1 << (scan_row_index + 1));  // PB配置为输入
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

// 将字符转换为索引
uint8_t char_to_index(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';  // 数字0-9
    } else if (c >= 'A' && c <= 'H') {
        return c - 'A' + 10;  // 字母A-F
    } else if (c >= 'a' && c <= 'h') {
        return c - 'a' + 10;  // 小写字母a-f
    } else if (c == ' ') {
        return sizeof(digit_patterns)-1;  // 空格（全部清除）
    } else {
        return 0;  // 默认显示0
    }
}

// 显示单个数字/字母到指定位置
void sd1516_draw_digit(uint8_t digit_pos, char c)
{
    uint8_t index = char_to_index(c);
    uint8_t pattern = digit_patterns[index];  // 获取字符的段模式
    
    if (digit_pos > 3) return;  // 检查位置有效性
    
    // 清除该位置的所有段
    for (uint8_t i = 0; i < 7; i++) {
        sd1516_clr_dot(digit_segments[digit_pos][i]);
    }
    
    // 根据位模式显示相应的段
    for (uint8_t i = 0; i < 7; i++) {
        if (pattern & (1 << i)) {  // 检查对应位是否为1
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

    /* 更新数字显示 */   
    for (pos = 0; pos < 4; pos++) {
        m = char_to_index(disp.digits[pos]);
        pattern = digit_patterns[m];
        // 根据位模式显示相应的段
        mask = 0x01;
        for (i = 0; i < 7; i++) {
            // 检查对应位是否为1
            if (pattern & mask) {  
                sd1516_set_dot(digit_segments[pos][i]);
            }
            mask = mask << 1;
        }
    }

    /* 更新状态 */
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
    
    /* 显示电量 */
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

// 直接通过位模式设置字符显示
void sd1516_draw_custom_pattern(uint8_t digit_pos, uint8_t pattern)
{
    if (digit_pos > 3) return;
    
    // 清除该位置的所有段
    for (uint8_t i = 0; i < 7; i++) {
        sd1516_clr_dot(digit_segments[digit_pos][i]);
    }
    
    // 根据位模式显示相应的段
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