#include "typedef.h"
#include "key.h"
#include "msg.h"

#if KEY_IO_EN
#define IOKEY_TUBE_SHORT_UP \
							/*00*/		NO_MSG,\
							/*01*/		MSG_KEY_SHORT_UP,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_TUBE_LONG \
							/*00*/		NO_MSG,\
							/*01*/		MSG_KEY_LONG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_TUBE_HOLD \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_TUBE_LONG_UP \
							/*00*/		NO_MSG,\
							/*01*/		MSG_KEY_LONG_UP,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_TUBE_DOUBLE_KICK \
							/*00*/		NO_MSG,\
							/*01*/		NO_MSG,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_TUBE_SHORT \
							/*00*/		NO_MSG,\
							/*01*/		MSG_KEY_SHORT,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_TUBE_LONG_START \
							/*00*/		NO_MSG,\
							/*01*/		MSG_KEY_LONG_START,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

#define IOKEY_TUBE_LONG1_UP \
							/*00*/		NO_MSG,\
							/*01*/		MSG_KEY_LONG1_UP,\
							/*02*/		NO_MSG,\
							/*03*/		NO_MSG,\
							/*04*/		NO_MSG,\
							/*05*/		NO_MSG,\
							/*06*/		NO_MSG,\
							/*07*/		NO_MSG,\
							/*08*/		NO_MSG,\
							/*09*/		NO_MSG,\

const u16 iokey_msg_tube_table[][IO_KEY_MAX_NUM] = {
    /*短按*/		{IOKEY_TUBE_SHORT},
    /*短按抬起*/	{IOKEY_TUBE_SHORT_UP},
    /*长按*/		{IOKEY_TUBE_LONG},
    /*连按*/		{IOKEY_TUBE_HOLD},
    /*长按抬起*/	{IOKEY_TUBE_LONG_UP},
    /*双击*/		{IOKEY_TUBE_DOUBLE_KICK},
    /*长按计时开始*/{IOKEY_TUBE_LONG_START},
	/*长按3*/		{IOKEY_TUBE_LONG1_UP},
};
#endif
