# beaglebone black linux内核编译

使用`arch/arm/boot/configs/bb.org_defconfig`文件配置`.config`：

```bash
$ make ARCH=arm CROSS_COMPILE=arm-none-eabi- bb.org_defconfig
  HOSTCC  scripts/basic/fixdep
  HOSTCC  scripts/kconfig/conf.o
  SHIPPED scripts/kconfig/zconf.tab.c
  SHIPPED scripts/kconfig/zconf.lex.c
  SHIPPED scripts/kconfig/zconf.hash.c
  HOSTCC  scripts/kconfig/zconf.tab.o
  HOSTLD  scripts/kconfig/conf
#
# configuration written to .config
#

# 正式编译
$ make ARCH=arm CROSS_COMPILE=arm-none-linux-eabi- zImage 
```

清除配置文件
```bash
$ make distclean 
```
