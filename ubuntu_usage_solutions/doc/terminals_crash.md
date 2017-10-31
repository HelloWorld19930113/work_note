# `ubuntu` 终端闪退

首先看一下`/tmp `目录的权限情况        
```bash
$ sudo ls -al / | grep tmp
```
得到结果若是如下的话，就好解决了        
```bash
drwxrwxr-x 7 root root 4096 10月 23 16:17 tmp
```
只需要一个命令          
```bash
$ sudo  chmod 1777 /tmp
```
or        
```bash
$ sudo chmod +t /tmp
```