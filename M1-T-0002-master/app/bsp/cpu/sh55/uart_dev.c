/**
 * @file uart.c
 * @author JL
 * @brief 串口UART模块C文件
 * @version 1.2
 * @date 2018-11-22
 */
/*#include "asm/clock.h"*/
/*#include "asm/uart_dev.h"*/
#include "cpu.h"
/*#include "generic/gpio.h"*/
/*#include "spinlock.h"*/

#include "config.h"
#include "clock.h"
#include "uart_dev.h"

#define LOG_TAG_CONST       UTD
#define LOG_TAG             "[uart dev]"
#include "debug.h"


#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".uart_bss")
#pragma data_seg(".uart_data")
#pragma const_seg(".uart_const")
#pragma code_seg(".uart_code")
#endif
#define UART_CLK  clk_get("uart")
#define UART_OT_CLK  clk_get("lsb")
static uart_bus_t uart0;
static uart_bus_t uart1;
/* _WEAK_ */
/* extern */
const u32 CONFIG_UART0_ENABLE = 1;
/* _WEAK_ */
/* extern */
const u32 CONFIG_UART1_ENABLE  = 1;
/* _WEAK_ */
/* extern */
const u32 CONFIG_UART0_ENABLE_TX_DMA =  0;//串口0无DMA
/* _WEAK_ */
/* extern */
const u32 CONFIG_UART1_ENABLE_TX_DMA =  1;
static u32 kfifo_get(KFIFO *kfifo, u8 *buffer, u32 len)
{
    unsigned int i;
    len = MIN(len, kfifo->buf_in - kfifo->buf_out);
    i = MIN(len, kfifo->buf_size - (kfifo->buf_out & (kfifo->buf_size - 1)));
    memcpy(buffer, kfifo->buffer + (kfifo->buf_out & (kfifo->buf_size - 1)), i);
    memcpy(buffer + i, kfifo->buffer, len - i);
    kfifo->buf_out += len;
    return len;
}
static u32 kfifo_length(KFIFO *kfifo)
{
    return kfifo->buf_in - kfifo->buf_out;
}
/**
 * @brief ut0发送一个byte
 *
 * @param a 要发送的字节
 */
static void UT0_putbyte(char a)
{
    if (JL_UT0->CON & BIT(0)) {
        JL_UT0->BUF = a;
        __asm__ volatile("csync");
        if ((JL_UT0->CON & BIT(2)) == 0) {
            while ((JL_UT0->CON & BIT(15)) == 0);
            JL_UT0->CON |= BIT(13);
        }
    }
}
/**
 * @brief ut0接收一个byte
 *
 * @param buf 字节存放地址
 * @param timeout 接收超时时间，单位1ms
 * @return 返回0：接收失败；返回1：接收成功
 */
static u8 UT0_getbyte(u8 *buf, u32 timeout)
{
    u32 _timeout, _t_sleep;
    timeout = ut_msecs_to_jiffies(timeout);
    if (JL_UT0->CON & BIT(6)) {//DMA_MODE  无DMA
        return 0;
    } else {
        JL_UT0->CON |= BIT(12);
        _timeout = timeout + ut_get_jiffies();
        _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
        while (!(JL_UT0->CON & BIT(14))) {
            if (timeout && time_before(_timeout, ut_get_jiffies())) {
                return 0;
            }
            if (time_before(_t_sleep, ut_get_jiffies())) {
                ut_sleep();
                _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
            }
        }
        *buf = JL_UT0->BUF;
        JL_UT0->CON |= BIT(12);
        __asm__ volatile("csync");  //make RX_PND_CLR taking effect
    }
    return 1;
}
/**
 * @brief ut0中断函数
 */
SET_INTERRUPT
static void UT0_isr_fun(void)
{
    u32 rx_len = 0;
    if ((JL_UT0->CON & BIT(2)) && (JL_UT0->CON & BIT(15))) {
        JL_UT0->CON |= BIT(13);
        UT_OSSemPost(&uart0.sem_tx);
        if (uart0.isr_cbfun) {
            uart0.isr_cbfun(&uart0, UT_TX);
        }
    }
    if ((JL_UT0->CON & BIT(3)) && (JL_UT0->CON & BIT(14))) {
        JL_UT0->CON |= BIT(12);           //清RX PND
        uart0.kfifo.buf_in += uart0.frame_length; //每满frame_length字节则产生一次中断
        UT_OSSemPost(&uart0.sem_rx);
        if (uart0.isr_cbfun) {
            uart0.isr_cbfun(&uart0, UT_RX);
        }
    }
}
/**
 * @brief ut0接收字符串
 *
 * @param buf 字符串存放首地址
 * @param len 预接收长度
 * @param timeout 接收超时时间，单位1ms
 * @return 返回实际接收长度
 */
static u32 UT0_read_buf(u8 *buf, u32 len, u32 timeout)
{
    u32 i;
    u32 _timeout, _t_sleep;
    if (len == 0) {
        return 0;
    }
    timeout = ut_msecs_to_jiffies(timeout);
    if (JL_UT0->CON & BIT(6)) {   //DMA_MODE  无DMA

        return 0;
    } else {
        _timeout = timeout + ut_get_jiffies();
        _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
        for (i = 0; i < len; i++) {
            while (!(JL_UT0->CON & BIT(14))) {
                if (timeout && time_before(_timeout, ut_get_jiffies())) {
                    return i;
                }
                if (time_before(_t_sleep, ut_get_jiffies())) {
                    ut_sleep();
                    _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
                }
            }
            *(buf + i) = JL_UT0->BUF;
            JL_UT0->CON |= BIT(12);
            __asm__ volatile("csync");  //make RX_PND_CLR taking effect
        }
    }
    return len;
}
/**
 * @brief ut0发送字符串
 *
 * @param buf 字符串首地址
 * @param len 发送的字符串长度
 */
static void UT0_write_buf(const u8 *buf, u32 len)
{
    u32 i;
    if (len == 0) {
        return;
    }
    if (CONFIG_UART0_ENABLE_TX_DMA) {
        UT0_putbyte('n');
        UT0_putbyte('o');
        UT0_putbyte('-');
        UT0_putbyte('D');
        UT0_putbyte('M');
        UT0_putbyte('A');
    } else {
        for (i = 0; i < len; i ++) {
            UT0_putbyte(*(buf + i));
        }
    }
}
/**
 * @brief ut0配置波特率
 *
 * @param baud 波特率值
 */
static void UT0_set_baud(u32 baud)
{
    uart_clk_sel();//JL_CLK->CON0 &= ~(0b11 << 8);//pll48M
    JL_UT0->CON &= ~BIT(0);
    JL_UT0->CON |= BIT(13) | BIT(12);
    JL_UT0->BAUD = UART_CLK / baud / 4 - 1;
    JL_UT0->BAUD = UART_CLK / baud / 4 - 1;
    JL_UT0->CON |= BIT(13) | BIT(12)  | BIT(0);
    JL_UT0->BUF = ' ';
}
/**
 * @brief ut0使能
 */
static void UT0_open(u32 baud, u32 is_9bit, void *cbuf, u32 cbuf_size, u32 rx_cnt, u32 ot)
{
    JL_UT0->CON = BIT(13) | BIT(12) ;
    UT_OSSemCreate(&uart0.sem_rx, 0);
    UT_OSSemCreate(&uart0.sem_tx, 0);
    request_irq(IRQ_UART0_IDX, IRQ_UART0_IP, UT0_isr_fun, 0);
    if (cbuf) {
        uart0.kfifo.buffer = cbuf;
        uart0.kfifo.buf_size = cbuf_size;
        uart0.kfifo.buf_in = 0;
        uart0.kfifo.buf_out = 0;
        uart0.frame_length = rx_cnt;
        uart0.rx_timeout = ot;
        //无DMA
    }
    if (is_9bit) {
        JL_UT0->CON |= BIT(1);
    } else {
        JL_UT0->CON &= ~BIT(1);
    }
    UT0_set_baud(baud);
        JL_UT0->CON |= BIT(3);
}
/**
 * @brief ut0关闭，注销
 */
static void UT0_close(void)
{
    UT_OSSemClose(&uart0.sem_rx);
    UT_OSSemClose(&uart0.sem_tx);
    irq_disable(IRQ_UART0_IDX);
    JL_UT0->CON = BIT(13) | BIT(12);//清中断，未关外设
}
/**
 * @brief ut1发送一个byte
 *
 * @param a 要发送的字节
 */
static void UT1_putbyte(char a)
{
    if (JL_UT1->CON0 & BIT(0)) {
        JL_UT1->BUF = a;
        __asm__ volatile("csync");
        if ((JL_UT1->CON0 & BIT(2)) == 0) {
            while ((JL_UT1->CON0 & BIT(15)) == 0);
            JL_UT1->CON0 |= BIT(13);
        }
    }
}
/**
 * @brief ut1接收一个byte
 *
 * @param buf 字节存放地址
 * @param timeout 接收超时时间，单位1ms
10ms¥；返回1：接收成功
 */
static u8 UT1_getbyte(u8 *buf, u32 timeout)
{
    u32 _timeout, _t_sleep;
    timeout = ut_msecs_to_jiffies(timeout);
    if (JL_UT1->CON0 & BIT(6)) {
        if (!kfifo_length(&uart1.kfifo)) {
            UT_OSSemPend(&uart1.sem_rx, timeout);
        }
        UT_OSSemSet(&uart1.sem_rx, 0);
        return kfifo_get(&uart1.kfifo, buf, 1);
    } else {
        _timeout = timeout + ut_get_jiffies();
        _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
        while (!(JL_UT1->CON0 & BIT(14))) {
            if (timeout && time_before(_timeout, ut_get_jiffies())) {
                return 0;
            }
            if (time_before(_t_sleep, ut_get_jiffies())) {
                ut_sleep();
                _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
            }
        }
        *buf = JL_UT1->BUF;
        JL_UT1->CON0 |= BIT(12);
        __asm__ volatile("csync");  //make RX_PND_CLR taking effect
    }
    return 1;
}
/**
 * @brief ut1中断函数
 */
SET_INTERRUPT
static void UT1_isr_fun(void)
{
    u32 rx_len = 0;
    if ((JL_UT1->CON0 & BIT(2)) && (JL_UT1->CON0 & BIT(15))) {
        JL_UT1->CON0 |= BIT(13);
        UT_OSSemPost(&uart1.sem_tx);
        if (uart1.isr_cbfun) {
            uart1.isr_cbfun(&uart1, UT_TX);
        }
    }
    if ((JL_UT1->CON0 & BIT(3)) && (JL_UT1->CON0 & BIT(14))) {
        JL_UT1->CON0 |= BIT(12);           //清RX PND
        uart1.kfifo.buf_in += uart1.frame_length; //每满32字节则产生一次中断
        UT_OSSemPost(&uart1.sem_rx);
        if (uart1.isr_cbfun) {
            uart1.isr_cbfun(&uart1, UT_RX);
        }
    }
    if ((JL_UT1->CON0 & BIT(5)) && (JL_UT1->CON0 & BIT(11))) {
        //OTCNT PND
        JL_UT1->CON0 |= BIT(7);                     //DMA模式1：写收到的数目到UTx_HRXCNT寄存器
        JL_UT1->CON0 |= BIT(10);                    //清OTCNT PND
        JL_UT1->CON0 |= BIT(12);                    //清RX PND(这里的顺序不能改变，这里要清一次)
        rx_len = JL_UT1->HRXCNT;             //读当前串口接收数据的个数
        if (rx_len) {
            uart1.kfifo.buf_in += rx_len;
            /* log_info("%s() %d\n", __func__, __LINE__); */
            UT_OSSemPost(&uart1.sem_rx);
            if (uart1.isr_cbfun) {
                uart1.isr_cbfun(&uart1, UT_RX_OT);
            }
        }
    }
}
/**
 * @brief ut1接收字符串
 *
 * @param buf 字符串存放首地址
 * @param len 预接收长度
 * @param timeout 接收超时时间，单位1ms
 * @return 返回实际接收的长度
 */
static u32 UT1_read_buf(u8 *buf, u32 len, u32 timeout)
{
    u32 i;
    u32 _timeout, _t_sleep;
    if (len == 0) {
        return 0;
    }
    timeout = ut_msecs_to_jiffies(timeout);
    if (JL_UT1->CON0 & BIT(6)) {
        if (!kfifo_length(&uart1.kfifo)) {
            UT_OSSemPend(&uart1.sem_rx, timeout);
        }
        UT_OSSemSet(&uart1.sem_rx, 0);
        return kfifo_get(&uart1.kfifo, buf, len);
    } else {
        _timeout = timeout + ut_get_jiffies();
        _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
        for (i = 0; i < len; i++) {
            while (!(JL_UT1->CON0 & BIT(14))) {
                if (timeout && time_before(_timeout, ut_get_jiffies())) {
                    return i;
                }
                if (time_before(_t_sleep, ut_get_jiffies())) {
                    ut_sleep();
                    _t_sleep = ut_msecs_to_jiffies(10) + ut_get_jiffies();
                }
            }
            *(buf + i) = JL_UT1->BUF;
            JL_UT1->CON0 |= BIT(12);
            __asm__ volatile("csync");  //make RX_PND_CLR taking effect
        }
    }
    return len;
}
/**
 * @brief ut1发送字符串
 *
 * @param buf 字符串首地址
 * @param len 发送的字符串长度
 * @param timeout 发送超时时间，单位10ms
 */
static void UT1_write_buf(const u8 *buf, u32 len)
{
    u32 i;
    if (len == 0) {
        return;
    }
    if (CONFIG_UART1_ENABLE_TX_DMA) {
        UT_OSSemSet(&uart1.sem_tx, 0);
        JL_UT1->CON0 |= BIT(13);
        JL_UT1->CON0 |= BIT(2);
        JL_UT1->TXADR = (u32)buf;
        JL_UT1->TXCNT = len;
        UT_OSSemPend(&uart1.sem_tx, 0);
        JL_UT1->CON0 &= ~BIT(2);
    } else {
        for (i = 0; i < len; i ++) {
            UT1_putbyte(*(buf + i));
        }
    }
}
/**
 * @brief ut1配置波特率
 *
 * @param baud 波特率值
 */
static void UT1_set_baud(u32 baud)
{
    uart_clk_sel();//JL_CLK->CON0 &= ~(0b11 << 8);//pll48M
    JL_UT1->CON0 &= ~(BIT(1) | BIT(0));
    JL_UT1->CON0 |= BIT(13) | BIT(12) | BIT(10);
    JL_UT1->BAUD = UART_CLK / baud / 4 - 1;
    if (JL_UT1->CON0 & BIT(5)) { //使能了OT中断
        if (uart1.rx_timeout > 10) {
            JL_UT1->OTCNT = (uart1.rx_timeout / 10) * (UART_OT_CLK / 10) / 10;
        } else {
            JL_UT1->OTCNT = uart1.rx_timeout * UART_OT_CLK / 1000;
        }
    }
    JL_UT1->CON0 |= BIT(13) | BIT(12) | BIT(10) | BIT(1) | BIT(0);
}
/**
 * @brief ut1使能
 */
static void UT1_open(u32 baud, u32 is_9bit, void *cbuf, u32 cbuf_size, u32 rx_cnt, u32 ot)
{
    JL_UT1->CON0 = BIT(13) | BIT(12) | BIT(10);
    UT_OSSemCreate(&uart1.sem_rx, 0);
    UT_OSSemCreate(&uart1.sem_tx, 0);
    request_irq(IRQ_UART1_IDX, IRQ_UART1_IP, UT1_isr_fun, 0);
    if (cbuf) {
        uart1.kfifo.buffer = cbuf;
        uart1.kfifo.buf_size = cbuf_size;
        uart1.kfifo.buf_in = 0;
        uart1.kfifo.buf_out = 0;
        uart1.frame_length = rx_cnt;
        uart1.rx_timeout = ot;
        JL_UT1->RXSADR = (u32)uart1.kfifo.buffer;
        JL_UT1->RXEADR = (u32)(uart1.kfifo.buffer + uart1.kfifo.buf_size);
        JL_UT1->RXCNT = uart1.frame_length;
        JL_UT1->CON0 |= BIT(7) | BIT(6) | BIT(5) | BIT(3);
    }
    if (is_9bit) {
        JL_UT1->CON2 |= BIT(0);
    } else {
        JL_UT1->CON2 &= ~BIT(0);
    }
    UT1_set_baud(baud);
}
/**
 * @brief ut1关闭，注销
 */
static void UT1_close(void)
{
    UT_OSSemClose(&uart1.sem_rx);
    UT_OSSemClose(&uart1.sem_tx);
    irq_disable(IRQ_UART1_IDX);
    JL_UT1->CON0 = BIT(13) | BIT(12) | BIT(10);
}


static u32 uart_is_idle(u32 ut_num)
{
    switch (ut_num) {
    case 0 :
        return !(JL_UT0->CON & BIT(0));
    case 1 :
        return !(JL_UT1->CON0 & BIT(0));
    default :
        break;
    }
    return -1;
}
/**ut硬件集成引脚名称字符串，上下对应为一对*/
/* static char *ut_tx_pin[] = {"PA04", "PA00", "reserved", "reserved", "DM", "A9", "DM", "A9"}; */
/* static char *ut_rx_pin[] = {"PA05", "PA01", "reserved", "reserved", "DP", "A9",  " ",  " "}; */
static u8 ut_tx_pin[] = {
    IO_PORTA_06, IO_PORTA_00, IO_PORTA_06, IO_PORTA_00, //UT0_TX
    IO_PORTA_04, IO_PORTB_09, IO_PORTA_04, IO_PORTB_09,  //UT1_TX
};
#define ICH0SEL 40//////////////////////////////////////////////////////
#define ICH2SEL 42//////////////////////////////////////////////////////
static u8 ut_rx_pin[] = {
    IO_PORTA_07, IO_PORTA_01, ICH2SEL, ICH2SEL, //UT0_RX
    IO_PORTA_05, IO_PORTB_09, ICH0SEL, ICH0SEL,  //UT1_RX
};

u8 gpio_uart_rx_input(u8 rx_pin, u8 ut_num, u8 tx_pin);
u8 gpio_set_uart1(u32 ut_ch);
u8 gpio_set_uart0(u32 ut_ch);
/**
 * @brief ut模块初始化函数，供外部调用
 *
 * @param arg 传入uart_argment型结构体指针
 * @return 返回uart_bus_t型结构体指针
 */
__attribute__((noinline))
const uart_bus_t *uart_dev_open(const struct uart_platform_data_t *arg)
{
    u8 i;
    u8 ut_num;
    u8 CHx_UTx_TX;
    uart_bus_t *ut = NULL;
    u32 ut_ch = -1;
    u8 gpio_input_channle_flag = 0;
    for (i = 0; i < 8; i++) {
        if ((arg->tx_pin == ut_tx_pin[i]) || ((arg->tx_pin == (u8) - 1) && (arg->rx_pin == ut_rx_pin[i]))) {
            ut_num = i / 4;
            if (uart_is_idle(ut_num)) {
                ut_ch = i % 4;
                if ((arg->rx_pin != ut_rx_pin[i]) && (arg->rx_pin != (u8) - 1)) {
                    gpio_input_channle_flag = 1;
                }
                break;
            }
        }
    }
    if (ut_ch == -1) {
        if (uart_is_idle(0)) {
            ut_num = 0;
            if (arg->tx_pin < IO_PORTB_00) {
                CHx_UTx_TX = CH1_UT0_TX;
            } else {
                CHx_UTx_TX = CH3_UT0_TX;
            }
        } else if (uart_is_idle(1)) {
            ut_num = 1;
            if (arg->tx_pin < IO_PORTB_00) {
                CHx_UTx_TX = CH0_UT1_TX;    //or :CH1_UT1_TX
            } else {
                CHx_UTx_TX = CH2_UT1_TX;    //or :CH3_UT1_TX
            }
        } else {
            return NULL;
        }
    }
    if (arg->rx_cbuf) {
        if ((arg->rx_cbuf_size == 0) || (arg->rx_cbuf_size & (arg->rx_cbuf_size - 1))) {//1248...不会
            return NULL;
        }
    }
    if (CONFIG_UART0_ENABLE && ut_num == 0) {
        gpio_set_uart0(ut_ch);
        uart0.argv = arg->argv;
        uart0.isr_cbfun = arg->isr_cbfun;
        uart0.putbyte = UT0_putbyte;
        uart0.getbyte = UT0_getbyte;
        uart0.read = UT0_read_buf;
        uart0.write = UT0_write_buf;
        uart0.set_baud = UT0_set_baud;
        UT0_open(arg->baud, arg->is_9bit,
                 arg->rx_cbuf, arg->rx_cbuf_size,
                 arg->frame_length, arg->rx_timeout);
        ut = &uart0;
    } else if (CONFIG_UART1_ENABLE && ut_num == 1) {
        gpio_set_uart1(ut_ch);
        uart1.argv = arg->argv;
        uart1.isr_cbfun = arg->isr_cbfun;
        uart1.putbyte = UT1_putbyte;
        uart1.getbyte = UT1_getbyte;
        uart1.read    = UT1_read_buf;
        uart1.write   = UT1_write_buf;
        uart1.set_baud = UT1_set_baud;
        UT1_open(arg->baud, arg->is_9bit,
                 arg->rx_cbuf, arg->rx_cbuf_size,
                 arg->frame_length, arg->rx_timeout);
        ut = &uart1;
    } else {
        return NULL;
    }

    if (ut_ch == -1) {
        if (arg->rx_pin != (u8) - 1) {
            gpio_uart_rx_input(arg->rx_pin, ut_num, arg->tx_pin);//io选择--->设置输入通道
        }
        if (arg->tx_pin != (u8) - 1) {
            gpio_output_channle(arg->tx_pin, CHx_UTx_TX);//io选择--->设置输出通道
        }
    }
    if (gpio_input_channle_flag) {//io选择--->设置输入通道
        gpio_uart_rx_input(arg->rx_pin, ut_num, arg->tx_pin);
    }
    return ut;
}
u32 gpio_close_uart0();
u32 gpio_close_uart1();
u32 uart_dev_close(uart_bus_t *ut)
{
    UT_OSSemClose(&ut->sem_rx);
    UT_OSSemClose(&ut->sem_tx);
    if (&uart0 == ut) {
        UT0_close();
        return gpio_close_uart0();
    } else if (&uart1 == ut) {
        UT1_close();
        return gpio_close_uart1();
    }
    return 0;
}
u32 gpio_close_uart0()
{
    SFR(JL_IOMC->IOMC0,  4,  1, 0);//JL_IOMC->IOMC0 &=  ~BIT(4);
    SFR(JL_IOMC->IOMC0,  5, 2, 0);
    return 1;
}
u32 gpio_close_uart1()
{
    JL_IOMC->IOMC0 &=  ~BIT(7);
    SFR(JL_IOMC->IOMC0,  8, 2, 0);//USB
    return 1;
}

u8 gpio_uart_rx_input(u8 rx_pin, u8 ut_num, u8 tx_pin)
{
    u8 sel = rx_pin;
    if (rx_pin > IO_PORT_MAX) {
        return 0;
    }
    //选择端口uart0
    if (ut_num == 0) {
        if (tx_pin == ut_tx_pin[0]) {
            SFR(JL_IOMC->IOMC0,  5, 2, 2);
        } else if (tx_pin == ut_tx_pin[1]) {
            SFR(JL_IOMC->IOMC0,  5, 2, 3);
        } else {
            SFR(JL_IOMC->IOMC0,  4,  1, 0);
            SFR(JL_IOMC->IOMC0,  5, 2, 3);
        }
    }
    //选择端口uart1
    else if (ut_num == 1) {
        if (tx_pin == ut_tx_pin[4]) {
            SFR(JL_IOMC->IOMC0,  8, 2, 2);
        } else if (tx_pin == ut_tx_pin[5]) {
            SFR(JL_IOMC->IOMC0,  8, 2, 3);
        } else {
            SFR(JL_IOMC->IOMC0,  7,  1, 0);
            SFR(JL_IOMC->IOMC0,  8, 2, 3);
        }
    } else {
        return 0;
    }
//配置输入通道
    if (rx_pin > 30) {
        sel = rx_pin - 32 + 28;
    }
    if (ut_num == 0) {
        SFR(JL_IOMC->IOMC2, 12, 6, sel);
    } else if (ut_num == 1) {
        SFR(JL_IOMC->IOMC2, 0, 6, sel);
    } else {
        return 0;
    }

    gpio_set_die(rx_pin, 1);
    gpio_set_direction(rx_pin, 1);//in  /*JL_PORTA->DIR |= BIT(5);*/
    gpio_set_pull_up(rx_pin, 1);
    gpio_set_pull_down(rx_pin, 0);
    return 1;
}
u8 gpio_set_uart1(u32 ut_ch)  //ch=-1；0；1；2；3
{
    if (ut_ch == -1) {
        return 0;
    }
    JL_IOMC->IOMC0 |=  BIT(7);
    SFR(JL_IOMC->IOMC0,  8, 2, ut_ch);//USB
    log_info("gpio_set_uart1:%d ",ut_ch);
    if (ut_ch == 0) {
        gpio_set_direction(IO_PORTA_05, 1);//in  /*JL_PORTA->DIR |= BIT(5);*/
        gpio_set_direction(IO_PORTA_04, 0);//out /*JL_PORTA->DIR &= ~BIT(4);*/
        gpio_set_pull_up(IO_PORTA_04, 1);
        gpio_set_pull_up(IO_PORTA_05, 1);
        gpio_set_die(IO_PORTA_05, 1);
        return 1;
    } else if (ut_ch == 1) {
        gpio_set_direction(IO_PORTB_09, 1);//in  /*JL_PORTA->DIR |= BIT(5);*/
        gpio_set_direction(IO_PORTB_09, 0);//out /*JL_PORTA->DIR &= ~BIT(4);*/
        gpio_set_pull_up(IO_PORTB_09, 1);
        gpio_set_pull_up(IO_PORTB_09, 1);
        return 1;
    } else if (ut_ch == 2) {
        return 0;
    } else {
        return 0;
    }
}
u8 gpio_set_uart0(u32 ut_ch)  //ch=-1；0；1
{
    if (ut_ch == -1) {
        return 0;
    }
    SFR(JL_IOMC->IOMC0,  4,  1, 1);// JL_IOMC->IOMC0 &= ~BIT(5);
    SFR(JL_IOMC->IOMC0,  5, 2, ut_ch);//JL_IOMC->IOMC0 |=  BIT(4);
    log_info("gpio_set_uart0:%d ",ut_ch);
    if (ut_ch == 0) {
        gpio_set_direction(IO_PORTA_06, 0);//JL_PORTA->DIR &= ~BIT(4);
        gpio_set_direction(IO_PORTA_07, 1);
        return 1;
    } else if (ut_ch == 1) {
        gpio_set_direction(IO_PORTA_04, 0);//JL_PORTA->DIR &= ~BIT(4);
        gpio_set_direction(IO_PORTA_05, 1);
        return 1;
    } else if (ut_ch == 2) {
        return 0;
    } else {
        return 0;
    }
}

#include "msg.h"
#define UART_TR_BUF_LEN     64
#define data_num        5//传输帧数
u8 uart_user_init = 0;
u8 read_buf_save[512] = {0};
u8 read_buf[UART_TR_BUF_LEN] = {0};

const uart_bus_t *ut = NULL;
u8 write_buf[64] = "123456789012345678901234567890123456789012345678901234567890ABC ";//64byte

u8 read_data[64] = {0}, rx_cnt = 0;
u32 i = 0, time_out = 0;

static void uart0_isr_hook(void *arg, u32 status){
    // log_info("----uart0_isr_func----\n");
}

static void uart1_isr_hook(void *arg, u32 status){
    // if (status == UT_RX) {
        
    //     memcpy(&read_buf_save,read_buf,UART_TR_BUF_LEN);
    //     xprintf("read_buf_save1: %s\n",read_buf_save);       //测试时打开
    //     // log_info("\n");
    //     // memset(&read_buf_save,0,sizeof(read_buf_save));
    //     // xprintf("read_buf_save0: %s\n",read_buf_save);
    //     // log_info("\n");
    //     log_info("----UT_RX----\n");
    //     JL_UT1->CON0 |= BIT(12);                    //清RX PND(这里的顺序不能改变，这里要清一次)
    // }else if (status == UT_RX_OT ){
    //     // log_info("----UT_RX_OT----\n");
    //     JL_UT1->CON0 |= BIT(12);                    //清RX PND(这里的顺序不能改变，这里要清一次)
    //     // put_buf(read_buf,64);
    //     // xprintf("read_buf: %s\n",read_buf);

    // }else if(status == UT_TX){
    //     log_info("----UT_tx----\n");
    // }else{
    //     log_info("----uart1_isr_func:%d----\n",status);

    // }

}

//#include "user/user_include.h"

#if 1
void uart01_test()
{
    //uart0无DMA。uart1支持DMA收发。
    //常变量CONFIG_UART1_ENABLE_TX_DMA控制DMA发送开关(1:开DMA；0:关DMA)
    //当开启DMA接收时(给rx_cbuf结构体成员赋值会打开DMA接收)，个数rx_cbuf_size 必须设为2的n次方
    //流程：UART1自发自收流程，user_uart_send_test定时发送数据，采用中断方式接收数据，通讯过程中尽量不使用打印,
    struct uart_platform_data_t arg;

#if 0//uart0

    arg.tx_pin = IO_PORTA_01;
    arg.rx_pin = IO_PORTA_00;
    arg.rx_cbuf = NULL;
    arg.rx_cbuf_size = 0;
    arg.frame_length = 0;
    arg.rx_timeout = 0;
    arg.isr_cbfun = uart0_isr_hook;
    arg.argv = JL_UT0;
    arg.is_9bit = 0;
    arg.baud = 460800;
    log_info("----uart0 test!----\n");
    log_char('A');
    log_char('\n');
    if (uart_dev_close(&uart0) == 0) {
        log_error("~~~~~~~~~uart0 close fail!~~~~~~~~~~\n");
    }
    ut = uart_dev_open(&arg);

    if (ut == 0) {
        log_error("------uart0 init fail!--------\n");
    } else {
        log_char('B');
        log_char('\n');
        log_info("------uart0 init ok!--------\n");
        uart_user_init = 1;

        // 发送
        UT0_putbyte(write_buf[0]);
        ut->putbyte('\n');
        UT0_write_buf(write_buf, sizeof(write_buf));
        ut->putbyte('\n');
        ut->write(write_buf, sizeof(write_buf));
        //接收
        log_info("\nuart0 wait for time !\n");
        time_out = 100;
        rx_cnt = 2;
        while (rx_cnt) {
            if (UT0_getbyte(&read_data[0], time_out)) {
                log_info("get byte: %c\n", read_data[0]);
                rx_cnt--;
            } else {
                log_info("receive 1 byte fail!\t");
                log_info("please send 1 byte!\n");
            }
        }
        rx_cnt = 2;
        while (rx_cnt) {
            if (ut->read(read_data, sizeof(read_data) - 1, time_out)) {
                log_info("get buf: %s\n", read_data);
                for (i = 0; i < sizeof(read_data); i++) {
                    read_data[i] = 0;
                }
                rx_cnt--;
            } else {
                log_info("receive buf fail!\t");
                log_info("please send some characters!\n");
            }
        }
    }
#else//uart1

#define IO_UART1_RX0    IO_PORTA_01
#define IO_UART1_TX0    IO_PORTA_00
    arg.tx_pin = IO_UART1_TX0; //
    arg.rx_pin = IO_UART1_RX0; //
    arg.rx_cbuf = read_buf;
    arg.rx_cbuf_size = sizeof(read_buf);
    arg.frame_length = sizeof(read_buf);
    arg.rx_timeout = 200;
    arg.isr_cbfun = uart1_isr_hook; //
    arg.argv = JL_UT1;
    arg.is_9bit = 0;
    arg.baud = 115200;
    log_debug("----uart1 test!----\n");
    if (uart_dev_close(&uart1) == 0) {
        log_error("~~~~~~~~~uart1 close fail!~~~~~~~~~~\n");
    }
    ut = uart_dev_open(&arg);
    if (ut == 0) {
        log_error("------uart1 init fail!--------\n");
    } else {
        log_debug("------uart1 init ok!JL_UT0->CON:%x%x --------\n",JL_UT1->CON0+1,JL_UT1->CON0);
        uart_user_init = 1;
     //   xprintf("%s\n",read_buf_save);
        return;
    }

#endif
}

void user_uart_send_test(void)
{
    if(!uart_user_init)
    return ;

    ut->write(write_buf, sizeof(write_buf));
   // ut->read(read_data, sizeof(read_data) - 1, time_out);
}

void user_uart_send_byte(char byte)
{
    ut->putbyte(byte);
}

//timeout:接收超时时间，单位1ms
void user_uart_get_byte(u8 *buf, u32 timeout)
{
    ut->getbyte(buf, time_out);
}

void user_uart_send_dma( u8 *buf, u32 len)
{
    ut->write(buf,len);
}

u32 user_uart_read(u8 *buf, u32 len, u32 timeout)
{
    u32 re_len;
    re_len = ut->read(buf,len,timeout);
    return re_len;
}

#endif

