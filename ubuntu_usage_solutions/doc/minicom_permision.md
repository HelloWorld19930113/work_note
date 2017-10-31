# 如何让普通用户 访问`/dev/ttyUSB0`

```bash
$ sudo apt remove modemmanager
$ sudo usermod -a -G dialout $USER
```
