# 普通用户(非root用户) 访问 /dev/ttyUSB0 on 14.04

```bash
$ sudo apt remove modemmanager

$ sudo usermod -a -G dialout $USER
```
