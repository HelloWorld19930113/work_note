# 新版本安装 `sublime_text3`

1. 下载安装包        
从[sublime官网](https://download.sublimetext.com/sublime_text_3_build_3143_x64.tar.bz2)下载安装包，由于`apt源`的问题，所以直接下载`tar包`。        
2. 复制文件到系统目录        
下载完成后将文件解压到`/opt目录`。        
```bash
$ sudo tar -xvf sublime_text_3_build_3143_x64.tar.bz2 -c /opt
$ sudo mv /opt/sublime_text_3 /opt/sublime_text
$ sudo ln -s /usr/bin/subl /opt/sublime_text
```
3. 支持搜狗输入法          
将编译好的`libsublime-imfix.so`复制到`/opt/sublime_text`目录下：          
```bash
$ sudo cp lib/libsublime-imfix.so /opt/sublime_text/
```
