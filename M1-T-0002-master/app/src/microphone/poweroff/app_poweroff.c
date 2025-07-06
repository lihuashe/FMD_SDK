#include "config.h"
#include "msg.h"
#include "app_mg.h"
#include "poweroff/app_poweroff.h"
#include "asm/power_interface.h"
//#include "music/app_music.h"
//#include "music_api.h"
#include "user_header.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "debug.h"
//static music_play_obj *music_obj = NULL;
static u8 power_pin[1] = {
    IO_POWER_ONOFF,
};

static u16 poweroff_msg_filter(u8 key_status, u8 key_num, u8 key_type)
{
    u16 msg = NO_MSG;
    switch (key_type) {
#if KEY_IO_EN
    case KEY_TYPE_IO:
        msg = iokey_msg_poweroff_table[key_status][key_num];
        break;
#endif
#if KEY_AD_EN
    case KEY_TYPE_AD:
        msg = adkey_msg_poweroff_table[key_status][key_num];
        break;
#endif

    default:
        break;
    }

    return msg;
}

static int app_poweroff_init(void *param)
{
    if (param == NULL) {
        return -1;
    }



    return 0;
}


static int app_poweroff_close(void)
{
    log_info("app_poweroff_close \n");

    return 0;
}

int app_poweroff(void *param)
{
    int msg[2];

    log_info("app_poweroff \n");
    if (app_poweroff_init(param) != 0) {
        log_info("app_poweroff init error !!! \n");
        app_switch_next();
        return 0;
    }


    while (1) {
        get_msg(2, &msg[0]);
        bsp_loop();

        if (common_msg_deal(msg) != (-1)) {
            continue;
        }

        switch (msg[0]) {

       
       case MSG_POWER_OFF:
            log_info("MSG_POWER_OFF :  \n" );
            gpio_write(IO_PORTA_11,0);
            extern void sys_softoff();
            sys_softoff();

            break;
        case MSG_POWER_ON:
            gpio_write(IO_PORTA_11,1);
            log_info("MSG_POWER_ON :  \n" );
            break;
        case MSG_NEXT_MODE:
            app_switch_next();
            break;
        case MSG_APP_SWITCH_ACTIVE:
            if (app_switch_en()) {
                goto _app_switch;
            }
            break;

        default:
            break;
        }
    }

_app_switch:
    app_poweroff_close();
    return 0;
}


