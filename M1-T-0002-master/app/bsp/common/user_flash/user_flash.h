#ifndef _USER_FLASH_H
#define _USER_FLASH_H

#include "KT0646M/microphone.h"

#define U16_HI(a)    (((a) >> 8) & 0xFF)
#define U16_LO(a)    ((a) & 0xFF)

#define U32_BYTE0(a) ((a) & 0xFF)
#define U32_BYTE1(a) (((a) >> 8) & 0xFF)
#define U32_BYTE2(a) (((a) >> 16) & 0xFF)
#define U32_BYTE3(a) (((a) >> 24) & 0xFF)
typedef struct user_area_header {
    u8 ch;                // ��ǰ�豸ͨ��
    u8 flag;              //�Ƿ��Ѿ�д���кű�־  0xFF �ͱ�ʾ�Ѿ�д���� �Ժ�Ͳ�����д��
    u16 device_id_reg12;  //�Ĵ���12
    u16 device_id_reg13;  //�Ĵ���13
    u16 device_id_reg14;  //�Ĵ���14
    u16 device_id_reg27;  //�Ĵ���27
    //u8 user_SerialNumber_id[8];  //���кŵ�����
} user_area_t;

user_area_t user_area_info ;

extern  mic_config_t mic_config;
int user_flash_info_init(void);
void user_test();
void copy_test(void);
int user_file_read_flash(u8 * buff,u32 addr ,u16 len);


void user_flash_read_info(user_area_t *user_info);

void user_flash_write_info(user_area_t *user_info);
uint8_t ReadDataFromFlash(void);
uint8_t SaveDataToFlash(mic_config_t *pmic_configdata);
void eraseToFlash();
#endif
