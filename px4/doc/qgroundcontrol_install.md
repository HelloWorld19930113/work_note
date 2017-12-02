# QGroundControl 地面站安装

> 声明：如果配置过程中遇到问题，可以去本文最末尾寻找解决方案，如果没有再去网上查找。   







## 安装中遇到的问题
1. 缺少mavlink2协议源文件       
```bash
$ git submodule init
$ git submodule update
```
2. 缺少sdl2库   
```bash
$ sudo apt-get install libsdl2-dev
```
3. 卸载内置的终端管理器    
```bash
$ sudo usermod -a -G dialout $USER
$ sudo apt-get purge modemmanager
```