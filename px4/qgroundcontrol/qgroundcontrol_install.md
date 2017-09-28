# QGroundControl 地面站安装

1. 缺少mavlink2协议源文件：
```bash
$ git submodule init
$ git submodule update
```
2. 缺少sdl2库
```bash
$ sudo apt-get install libsdl2-dev
```
3. 卸载内置的终端管理器：
```bash
$ sudo usermod -a -G dialout $USER
$ sudo apt-get purge modemmanager
```