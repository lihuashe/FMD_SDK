

#include "config.h"
#include "common.h"
#include "boot.h"
#include "tick_timer_driver.h"
#include "audio.h"
#include "msg.h"
#include "dac.h"
#include "dac_api.h"
#include "audio_adc.h"
#include "vfs.h"
#include "src_api.h"
#include "device.h"
#include "ioctl_cmds.h"
#include "vm.h"
#include "nor_fs/nor_fs.h"
#include "key.h"
#include "init.h"
#include "adc_drv.h"
#include "clock.h"
#include "app_power_mg.h"
#include "efuse.h"
#include "my_malloc.h"
#include "user_flash.h"
#include "errno-base.h"
#include "KT0646M/microphone.h"

#define LOG_TAG_CONST       FLASH
#define LOG_TAG             "[flash]"
#include "debug.h"


extern const unsigned char music_data[57300];

static struct vfs_attr userf_attr;
int user_flash_info_init(void)
{
    u32 err;
    void *pvfs = 0;
    void *pvfile = 0;
    void *device = 0;
    u32 capacity = 0;

    err = vfs_mount(&pvfs, (void *)NULL, (void *) NULL);
    ASSERT(!err, "fii vfs mount : 0x%x\n", err)
    err = vfs_openbypath(pvfs, &pvfile, "/app_area_head/USERF");
    ASSERT(!err, "fii vfs openbypath : 0x%x\n", err)
    err = vfs_ioctl(pvfile, FS_IOCTL_FILE_ATTR, (int)&userf_attr);
    ASSERT(!err, "fii vfs ioctl : 0x%x\n", err)
    log_debug("file size : 0x%x\nfile sclust : 0x%x\n", userf_attr.fsize, userf_attr.sclust);
    log_debug("boot info 0x%x\n", boot_info.flash_size);
    vfs_file_close(&pvfile);
    vfs_fs_close(&pvfs);

    boot_info.vm.vm_saddr = userf_attr.sclust;
    boot_info.vm.vm_size = userf_attr.fsize;

    device = dev_open(__SFC_NANE, 0);
    dev_ioctl(device, IOCTL_GET_CAPACITY, (u32)&capacity);
    dev_ioctl(device, IOCTL_SET_VM_INFO, (u32)&boot_info);
    dev_close(device);

    return 0;
}


struct vfs_attr *get_userf_attr_p(void)
{
    return &userf_attr;
}

void user_flash_write_test(){
    u32 err;
    void *pvfs = 0;
    void *pvfile = 0;
    void *device = 0;
    u32 capacity = 0;
    static u8 cnt=0xff;
    u8 i;
    cnt--;
     u8 wrtie_buff[10]={0,1,2,3,4,5,6,7,8,9};
    //u8 wrtie_buff[10];
   // my_memset(wrtie_buff,cnt,10);
    log_debug("cnt:%d  write\n",cnt);
    put_buf(wrtie_buff,10);

    log_debug("cnt:%d  ",cnt);

    i = sfc_erase(SECTOR_ERASER, userf_attr.sclust);
    // sfc_erase_zone( userf_attr.sclust,10);
    log_debug("erase_ret:%d  ",i);
    sfc_write(wrtie_buff,userf_attr.sclust,10);
}


void user_flash_read_test(){
    u32 err;
    void *pvfs = 0;
    void *pvfile = 0;
    void *device = 0;
    u32 capacity = 0;
    u8 read_buf[10];

    sfc_read(read_buf,userf_attr.sclust,10);
    put_buf(read_buf,10);
}

void user_flash_read_info(user_area_t *user_info){
    u32 err;
    u8 read_buf[10];
    my_memset(user_info,'0',10);
    sfc_read(read_buf,userf_attr.sclust,10);
    user_info->ch = read_buf[0];
    user_info->flag=read_buf[1];
    user_info->device_id_reg12=read_buf[2] << 8 | read_buf[3];
    user_info->device_id_reg13=read_buf[4] << 8 | read_buf[5];
    user_info->device_id_reg14=read_buf[6] << 8 | read_buf[7];
    user_info->device_id_reg27=read_buf[8] << 8 | read_buf[9];
    put_buf(read_buf,10);
}

void user_flash_write_info(user_area_t *user_info)
{
    u8 i=0;
    // u8 wrtie_buff[10]={0,1,2,3,4,5,6,7,8,9};
    u8 wrtie_buff[10];
    my_memset(wrtie_buff,'0',10);
    //put_buf(wrtie_buff,10);
    wrtie_buff[0]=user_info->ch;
    wrtie_buff[1]=user_info->flag;
    wrtie_buff[2]=U16_HI(user_info->device_id_reg12);
    wrtie_buff[3]=U16_LO(user_info->device_id_reg12);
    wrtie_buff[4]=U16_HI(user_info->device_id_reg13);
    wrtie_buff[5]=U16_LO(user_info->device_id_reg13);
    wrtie_buff[6]=U16_HI(user_info->device_id_reg14);
    wrtie_buff[7]=U16_LO(user_info->device_id_reg14);
    wrtie_buff[8]=U16_HI(user_info->device_id_reg27);
    wrtie_buff[9]=U16_LO(user_info->device_id_reg27);
    i = sfc_erase(SECTOR_ERASER, userf_attr.sclust);
    // sfc_erase_zone( userf_attr.sclust,10);
    log_debug("erase_ret:%d  ",i);
    sfc_write(wrtie_buff,userf_attr.sclust,10);

}

void user_test(){
    log_debug("into user_flash_read_test \n");
    user_flash_read_test();
    delay_10ms(10);
    log_debug("into user_flash_write_test \n");
    user_flash_write_test();
    delay_10ms(10);
    user_flash_read_test();
     return;
}

#define WRITE_LEN          1024
// int user_file_copy_flash(u32 flash_addr,u8 *file,u32 file_len){
int user_file_copy_flash(u32 file_len){
    static bool erase_flag = 0;
    u8 buf_temp[1024];

    u32 err;
    static u32 file_seek=0;
    while(file_seek < file_len){
        log_debug("file_seek:%d  ",file_seek);
        if(erase_flag){
            erase_flag = 0;
            if(sfc_erase(SECTOR_ERASER, userf_attr.sclust+file_seek)){

            }else{
                log_debug("erase_err  ");
                return -1;
            }
            putchar('E');
        }
        if((file_seek+WRITE_LEN) < file_len){
            my_memcpy(buf_temp,music_data+file_seek,WRITE_LEN);
            err = sfc_write(buf_temp,userf_attr.sclust+file_seek,WRITE_LEN);

            log_debug("err = :%d ",err);
            file_seek += WRITE_LEN;
        }else{
            my_memcpy(buf_temp,music_data+file_seek,file_len-file_seek);
            err = sfc_write(buf_temp,userf_attr.sclust+file_seek,file_len-file_seek);
            log_debug("err = :%d ",err);
            file_seek = file_len;
        }
        if(file_seek%(4096) < 1024){
            erase_flag = 1;
        }
        log_debug("file_seek:%d  ",file_seek);
        wdt_clear();
    }
    log_debug("FLASH_WRITE_OVER");

    return 0;
}

int user_file_read_flash(u8 * buff,u32 addr ,u16 len){
    log_debug("addr:%d ---len:%d ",addr,len);
    sfc_read(buff,userf_attr.sclust+addr,len);
    return 0;
}

extern void chip_reset();
void copy_test(void){
//    int err;
//     err = user_file_copy_flash(57300);
}

uint8_t SaveDataToFlash(mic_config_t *pmic_configdata)        // buf????256byte??
{
	uint16_t i;
	u32 err;
//	uint8_t *p = (uint8_t *)pmic_configdata;
//	uint32_t Sum = 0;
	if (pmic_configdata->flash_key == MIC_CONFIG_FLAS_KEY)
    {
        i = sfc_erase(SECTOR_ERASER, userf_attr.sclust);
        //  log_debug("erase_ret:%d  ",i);
        err = sfc_write((uint8_t *)pmic_configdata,userf_attr.sclust, sizeof(mic_config_t));

        log_debug("Save flash Data [%d] \n",err);
    }
    return TRUE;
}


uint8_t ReadDataFromFlash(void)         //
{
	uint32_t i,j;
	uint32_t Title, Sum = 0;
    uint8_t *p = NULL;
    memset(&mic_config,0,sizeof(mic_config_t));
 //   log_debug("mic_config_t len =:[%d] ",sizeof(mic_config_t));
    sfc_read((uint8_t *)&mic_config,userf_attr.sclust,sizeof(mic_config_t));
    if(mic_config.flash_key == MIC_CONFIG_FLAS_KEY)
    {

        return 1;          // ture
    }
    return 0;
}


void eraseToFlash()
{
    bool b_erase;
    b_erase = sfc_erase(SECTOR_ERASER, userf_attr.sclust);
    if (b_erase)
    {
         log_debug("erase success !! ");
    }else
    {
        log_debug("erase fail!!  ");
}
    }





