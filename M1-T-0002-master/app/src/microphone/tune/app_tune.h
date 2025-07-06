#include "msg.h"

#define I2CDef_KT065xM_CHA 1
#define I2CDef_KT065xM_CHB 2

void PowerOFF_io_init();
void PowerON_io_init();
int app_tune(void *param);
void KT_SeekFrq_Task();
void Rx_Response(uint8_t cmd);
