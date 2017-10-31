# 更换`ubuntu`软件下载源(163)	 
 使用下面的行直接替换原来的地址    
```
deb http://mirrors.163.com/ubuntu/ trusty main universe restricted multiverse
deb-src http://mirrors.163.com/ubuntu/ trusty main universe restricted multiverse
deb http://mirrors.163.com/ubuntu/ trusty-security universe main multiverse restricted
deb-src http://mirrors.163.com/ubuntu/ trusty-security universe main multiverse restricted
deb http://mirrors.163.com/ubuntu/ trusty-updates universe main multiverse restricted
deb http://mirrors.163.com/ubuntu/ trusty-proposed universe main multiverse restricted
deb-src http://mirrors.163.com/ubuntu/ trusty-proposed universe main multiverse restricted
deb http://mirrors.163.com/ubuntu/ trusty-backports universe main multiverse restricted
deb-src http://mirrors.163.com/ubuntu/ trusty-backports universe main multiverse restricted
deb-src http://mirrors.163.com/ubuntu/ trusty-updates universe main multiverse restricted
```
**注意**：    
  要区分`ubuntu`的版本。    