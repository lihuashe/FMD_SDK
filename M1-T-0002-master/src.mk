#########################################
# 改文件包含了需要添加到项目中的源码
#########################################

# 需要添加的bsp的源码
bsp_source_files = \
	app/bsp/common/decoder/decoder_api.c \
	app/bsp/common/decoder/decoder_msg_tab.c \
	app/bsp/common/decoder/decoder_point.c \
	app/bsp/common/decoder/eq.c \
	app/bsp/common/decoder/list/a_api.c \
	app/bsp/common/decoder/list/f1a_api.c \
	app/bsp/common/decoder/list/f1x_parsing.c \
	app/bsp/common/decoder/list/midi_api.c \
	app/bsp/common/decoder/list/midi_ctrl_api.c \
	app/bsp/common/decoder/list/mio_phy.c \
	app/bsp/common/decoder/mio_api.c \
	app/bsp/common/decoder/sine_play.c \
	app/bsp/common/fs/nor_fs/nor_fs_resource.c \
	app/bsp/common/fs/sydf/sydf_resource.c \
	app/bsp/common/fs/vfs.c \
	app/bsp/common/fs/vfs_resource.c \
	app/bsp/common/iic_soft/iic_soft.c \
	app/bsp/common/msg/msg.c \
	app/bsp/common/power_manage/app_power_mg.c \
	app/bsp/common/reserved_area/reserved_area.c \
	app/bsp/common/sound_effect_list/speed_api.c \
	app/bsp/common/spi_soft/spi_soft.c \
	app/bsp/common/uart_update/uart_update.c \
	app/bsp/common/vm/vm_api.c \
	app/bsp/common/user_flash/user_flash.c \
	app/bsp/cpu/sh55/adc_drv.c \
	app/bsp/cpu/sh55/clock.c \
	app/bsp/cpu/sh55/cpu.c \
	app/bsp/cpu/sh55/dac_cpu.c \
	app/bsp/cpu/sh55/irflt.c \
	app/bsp/cpu/sh55/pl_cnt.c \
	app/bsp/cpu/sh55/port_init.c \
	app/bsp/cpu/sh55/port_wkup.c \
	app/bsp/cpu/sh55/power_api.c \
	app/bsp/cpu/sh55/uart.c \
	app/bsp/cpu/sh55/uart_dev.c \
	app/bsp/lib/audio.c \
	app/bsp/lib/common.c \
	app/bsp/lib/dac_api.c \
	app/bsp/lib/my_malloc.c \
	app/bsp/lib/tab_read.c \
	app/bsp/modules/soft_isr/soft_isr.c \
	app/bsp/start/sh54_sh55/bsp_loop.c \
	app/bsp/start/sh54_sh55/device_list.c \
	app/bsp/start/sh54_sh55/init.c \
	app/bsp/start/sh54_sh55/main.c 

# 需要添加的bsp引用的头文件路径
bsp_includes = \
	-Iapp/bsp/lib \
	-Iapp/bsp/start/sh54_sh55 \
	-Iapp/bsp/cpu/sh55 \
	-Iapp/bsp/cpu/sh55/spi \
	-Iapp/bsp/cpu/sh55/wdt \
	-Iapp/bsp/cpu/sh55/audio \
	-Iapp/bsp/common \
	-Iapp/bsp/common/fs \
	-Iapp/bsp/common/iic_soft \
	-Iapp/bsp/common/eeprom \
	-Iapp/bsp/common/msg \
	-Iapp/bsp/common/file_operate \
	-Iapp/bsp/common/api_mg \
	-Iapp/bsp/common/key \
	-Iapp/bsp/common/power_manage \
	-Iapp/bsp/common/norflash \
	-Iapp/bsp/common/spi_soft \
	-Iapp/bsp/common/reserved_area \
	-Iapp/bsp/modules \
	-Iapp/bsp/modules/timer \
	-Iapp/bsp/common/uart_update \
	-Iapp/bsp/common/user_flash \

#项目路径
project_path = app/src/microphone/
post_build_path = app/post_build/sh55/microphone
# 项目路径下的源码默认会全部添加到编译系统
project_source_files = $(shell find $(project_path) -type f -name '*.c')

# 项目路径下的头文件会全部添加到编译系统
project_includes = $(addprefix -I,$(dir $(shell find $(project_path) -type f -name '*.h')))

