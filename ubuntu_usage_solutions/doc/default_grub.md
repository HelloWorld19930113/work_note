# 修改系统默认启动 `ubuntu`或`windows`

打开`ubuntu`系统以后，我们打开超级终端，输入以下命令:         

```bash
$ sudo gedit /etc/default/grub
```

显示如下         
```
# If you change this file, run 'update-grub' afterwards to update
# /boot/grub/grub.cfg.
# For full documentation of the options in this file, see:
#   info -f grub -n 'Simple configuration'

GRUB_DEFAULT=0
#GRUB_HIDDEN_TIMEOUT=0
GRUB_HIDDEN_TIMEOUT_QUIET=true
GRUB_TIMEOUT=10
GRUB_DISTRIBUTOR=`lsb_release -i -s 2> /dev/null || echo Debian`
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"
GRUB_CMDLINE_LINUX="locale=zh_CN"
```
GRUB_DEFAULT代表的就是启动项的顺序，从数字0开始，依次代表如下启动项（这是在我的电脑上，不同的ubuntu版本和windows系统可能会有一些不同）：            

`GRUB_DEFAULT`代表的就是启动项的顺序，从数字`0`开始，依次代表如下启动项（这是在我的电脑上，不同的ubuntu版本和windows系统可能会有一些不同）：         
```
Ubuntu
Advanced options for Ubuntu
Memory test (memtest86+)
Memory test (memtest86+, serial console 115200)
Windows 7 (loader) (on /dev/sda1)
```
windows排第四位（注意，顺序是从0开始计的），所以，把GRUB_DEFAULT的值修改为`4`，然后别忘了运行命令：           

```bash
$ sudo update-grub
```
好了，重启电脑，默认启动的系统就换到`windows`了。       
