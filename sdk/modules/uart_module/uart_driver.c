
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "uart_driver.h"




#include "rf.h"
#include <string.h>
#include "stamachine.h"

// 硬件配置
#define TX_PIN       // 定义发送IO口
#define RX_PIN        // 定义接收IO口
#define BAUD_RATE 2400   // 波特率

// 定时器配置 (基于16MHz时钟)
#define BIT_TIME (416)   // 1位时间 (416us @ 2400bps)
#define HALF_BIT_TIME (BIT_TIME/2)

// 帧格式定义
#define START_BIT 0
#define STOP_BIT  1

// 接收状态机
typedef enum {
    RX_IDLE,
    RX_START_BIT,
    RX_DATA_BITS,
    RX_STOP_BIT
} RxState;

// 发送数据结构
typedef union {
    uint8 data;
    struct {
        uint8_t Data_bit0 : 1;
        uint8_t Data_bit1 : 1;
        uint8_t Data_bit2 : 1;
        uint8_t Data_bit3 : 1;
        uint8_t Data_bit4 : 1;
        uint8_t Data_bit5 : 1;
        uint8_t Data_bit6 : 1;
        uint8_t Data_bit7 : 1;
    };
} RF_Data_Union;

// 接收缓冲区
#define RX_BUFFER_SIZE 64
typedef struct {
    volatile uint8_t buffer[RX_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t count;
} RxBuffer;

// 全局变量
volatile RxState rxState = RX_IDLE;
volatile uint8_t rxByte = 0;
volatile uint8_t rxBitCount = 0;
volatile uint8_t rxStartTime = 0;
volatile RxBuffer rxBuffer;

// 初始化接收缓冲区
void rxBufferInit(void) {
    rxBuffer.head = 0;
    rxBuffer.tail = 0;
    rxBuffer.count = 0;
}

// 将数据放入接收缓冲区
uint8_t rxBufferPut(uint8_t data) {
    if (rxBuffer.count >= RX_BUFFER_SIZE) {
        return 0; // 缓冲区满
    }
    
    rxBuffer.buffer[rxBuffer.head] = data;
    rxBuffer.head = (rxBuffer.head + 1) % RX_BUFFER_SIZE;
    rxBuffer.count++;
    return 1;
}

// 从接收缓冲区获取数据
uint8_t rxBufferGet(uint8_t *data) {
    if (rxBuffer.count == 0) {
        return 0; // 缓冲区空
    }
    
    *data = rxBuffer.buffer[rxBuffer.tail];
    rxBuffer.tail = (rxBuffer.tail + 1) % RX_BUFFER_SIZE;
    rxBuffer.count--;
    return 1;
}

// 串口接收中断处理 (在GPIO中断中调用)
void uartRxHandler(void) {
    static uint8_t lastPinState = 1;
    uint8_t currentPinState = RX_PIN;
    
    // 检测下降沿 (起始位)
    if (lastPinState && !currentPinState && rxState == RX_IDLE) {
        rxState = RX_START_BIT;
        rxStartTime = TMR1L; // 记录起始时间
        TMR1H = 0;
        TMR1L = HALF_BIT_TIME; // 设置半位时间定时
        T1ON = 1;
    }
    
    lastPinState = currentPinState;
}

// 定时器中断处理 (用于接收时序)
void uartRxTimerHandler(void) {
    switch (rxState) {
        case RX_START_BIT:
            // 在半位时间后检查起始位
            if (!RX_PIN) {
                // 有效的起始位
                rxState = RX_DATA_BITS;
                rxBitCount = 0;
                rxByte = 0;
                TMR1H = 0;
                TMR1L = BIT_TIME; // 设置完整位时间
            } else {
                // 噪声或错误，返回空闲状态
                rxState = RX_IDLE;
                T1ON = 0;
            }
            break;
            
        case RX_DATA_BITS:
            // 读取数据位 (在位的中间采样)
            rxByte |= (RX_PIN << rxBitCount);
            rxBitCount++;
            
            if (rxBitCount >= 8) {
                rxState = RX_STOP_BIT;
                TMR1H = 0;
                TMR1L = BIT_TIME; // 设置停止位时间
            } else {
                TMR1H = 0;
                TMR1L = BIT_TIME; // 重置定时器
            }
            break;
            
        case RX_STOP_BIT:
            // 检查停止位
            if (RX_PIN) {
                // 有效的停止位
                rxBufferPut(rxByte); // 存入缓冲区
            }
            // 无论停止位是否有效，都返回空闲状态
            rxState = RX_IDLE;
            T1ON = 0;
            break;
            
        default:
            rxState = RX_IDLE;
            T1ON = 0;
            break;
    }
    TMR1IF = 0; // 清除定时器中断标志
}

// 发送一个字节
void uartSendByte(uint8_t data) {
    // 等待前一个字节发送完成
    while (RFSend.RF_Staue != IDLE_STA);
    
    // 设置发送数据
    RFSend.RF_Data.data = data;
    RFSend.Step = 0;
    RFSend.RF_Staue = BUSY_STA;
    
    // 启动定时器
    PR1H = (uint8_t)(BIT_TIME >> 8);
    PR1L = (uint8_t)BIT_TIME;
    T1ON = 1;
}

// 发送字符串
void uartSendString(const char *str) 
{
    while (*str) {
        uartSendByte(*str++);
    }
}

// 发送函数 (在定时器中断中调用)
void Tx_Send_Run(void) 
{
    switch (RFSend.Step) {
        case 0: // 发送起始位
            TX_PIN = START_BIT;
            RFSend.Step++;
            break;
            
        case 1: // 发送数据位0
            TX_PIN = RFSend.RF_Data.Data_bit0;
            RFSend.Step++;
            break;
            
        case 2: // 发送数据位1
            TX_PIN = RFSend.RF_Data.Data_bit1;
            RFSend.Step++;
            break;
            
        case 3: // 发送数据位2
            TX_PIN = RFSend.RF_Data.Data_bit2;
            RFSend.Step++;
            break;
            
        case 4: // 发送数据位3
            TX_PIN = RFSend.RF_Data.Data_bit3;
            RFSend.Step++;
            break;
            
        case 5: // 发送数据位4
            TX_PIN = RFSend.RF_Data.Data_bit4;
            RFSend.Step++;
            break;
            
        case 6: // 发送数据位5
            TX_PIN = RFSend.RF_Data.Data_bit5;
            RFSend.Step++;
            break;
            
        case 7: // 发送数据位6
            TX_PIN = RFSend.RF_Data.Data_bit6;
            RFSend.Step++;
            break;
            
        case 8: // 发送数据位7
            TX_PIN = RFSend.RF_Data.Data_bit7;
            RFSend.Step++;
            break;
            
        case 9: // 发送停止位
            TX_PIN = STOP_BIT;
            RFSend.Step++;
            break;
            
        case 10: // 发送完成
            RFSend.RF_Staue = IDLE_STA;
            T1ON = 0;
            RFSend.Step = 0;
            break;
            
        default:
            RX_PIN = 1; break;
            break;
    }
}

// 接收一个字节 (非阻塞)
uint8_t uartReceiveByte(uint8_t *data) {
    return rxBufferGet(data);
}

// 初始化串口
void uartInit(void) {
    // 配置IO
    TRIS_RF_GPIO = 0; // 发送引脚设为输出
    TRIS_RF_RX = 1;   // 接收引脚设为输入
    
    // 配置定时器
    T1CON0 = 0B00010000; // 定时器1配置
    T2CON0 = 0B00000011; // 定时器2配置 (1:16分频)
    
    // 初始化状态
    RFSend.RF_Staue = IDLE_STA;
    rxState = RX_IDLE;
    rxBufferInit();
    
    // 配置接收引脚中断
    // (需要根据具体硬件配置GPIO中断)
}

// 主定时器中断处理
void interrupt ISR(void) {
    if (TMR1IF) {
        if (RFSend.RF_Staue == BUSY_STA) {
            RF_Send_Run(); // 处理发送
        } else if (rxState != RX_IDLE) {
            uartRxTimerHandler(); // 处理接收
        }
        TMR1IF = 0;
    }
    
    // 处理GPIO中断
    // (需要实现GPIO中断处理函数来调用uartRxHandler)
}

