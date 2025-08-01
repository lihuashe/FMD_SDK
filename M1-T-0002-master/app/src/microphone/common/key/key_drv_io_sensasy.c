#include "key_drv_io_sensasy.h"

#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "debug.h"


#if KEY_IO_EN

/*----------------------------------------------------------------------------*/
/**@brief   按键去抖函数，输出稳定键值
   @param   key：键值
   @return  稳定按键
   @note    u8 key_filter(u8 key)
*/
/*----------------------------------------------------------------------------*/
static u8 key_filter(u8 key)
{
    static u8 used_key = NO_KEY;
    static u8 old_key;
    static u8 key_counter;

    if (old_key != key) {
        key_counter = 0;
        old_key = key;
    } else {
        key_counter++;
        if (key_counter == KEY_BASE_CNT) {
            used_key = key;
        }
    }
    return used_key;
}

/*----------------------------------------------------------------------------*/
/**@brief   io按键初始化
   @param   void
   @param   void
   @return  void
   @note    void io_key_init(void)
*/
/*----------------------------------------------------------------------------*/
static void io_key_init(void)
{
    KEY1_INIT();
  //  KEY2_INIT();
}

/*----------------------------------------------------------------------------*/
/**@brief   获取IO按键电平值
   @param   void
   @param   void
   @return  key_num:io按键号
   @note    u8 get_iokey_value(void)
*/
/*----------------------------------------------------------------------------*/
static u8 get_iokey_value(void)
{
    //key_puts("get_iokey_value\n");
    u8 key_num = NO_KEY;

    if (IS_KEY1_DOWN())
    {
        key_num = 1;
    }


    return key_filter(key_num);
}

const key_interface_t key_io_info = {
    .key_type = KEY_TYPE_IO,
    .key_init = io_key_init,
    .key_get_value = get_iokey_value,
};

#endif/*KEY_IO_EN*/
