# 总的 Makefile，用于调用目录下各个子工程对应的 Makefile
# 注意： Linux 下编译方式：
# 1. 从 http://pkgman.jieliapp.com/doc/all 处找到下载链接
# 2. 下载后，解压到 /opt/jieli 目录下，保证
#   /opt/jieli/common/bin/clang 存在（注意目录层次）
# 3. 确认 ulimit -n 的结果足够大（建议大于8096），否则链接可能会因为打开文件太多而失败
#   可以通过 ulimit -n 8096 来设置一个较大的值
# 支持的目标
# make ad15n_app_mg
# make ad14n_simple
# make ad14n_mcu
# make ad14n_app_mg
# make ad15n_mcu
# make ad15n_simple
.PHONY: all clean microphone

all: microphone
	@echo +ALL DONE

clean: clean_microphone
	@echo +CLEAN DONE

# ad15n_app_mg:
# 	@$(MAKE) -C . -f Makefile.ad15n_app_mg

# clean_ad15n_app_mg:
# 	@$(MAKE) -C . -f Makefile.ad15n_app_mg clean

microphone:
	@$(MAKE) -C . -f Makefile.microphone

clean_microphone:
	@$(MAKE) -C . -f Makefile.microphone clean
