# Ubuntu 16.04备份和恢复
一开始想借助于第三方的备份恢复工具来完成当前的`Ubuntu 16.04`系统的备份以及后面的还原工作，不过考虑到那样做比较麻烦，索性就使用tar工具直接备份得了。这篇文章给大家详细介绍了`Ubuntu 16.04`备份和恢复的方法，有需要的朋友们可以参考借鉴，下面来一起看看吧。    

> 注意：
(1) 一定要重新备份下：`/boot`和`/etc/fstab`，然后再执行恢复命令。   
(2) 在重新启动之前，一定要将这两个这些文件更新。

## 前言
 在备份时主要需要确认有哪些目录不需要备份，网上虽然有很多备份的教程，不过还是要根据自己的情况来调整参数。   

## 1. 清理缓存、回收站等   
```bash  
# 将已经删除了的软件包的.deb安装文件从硬盘中删除掉
$ sudo apt-get autoclean   
# 类似上面的命令，但它删除包缓存中的所有包。这是个很好的做法，因为多数情况下这些包没有用了。   
$ sudo apt-get clean
```
## 2. 删除系统不再使用的孤立软件     
```bash
$ sudo apt-get autoremove
```
## 3. 使用`tar`备份
- (1) 命令及参数解释    
 v: 显示详细的压缩信息   
 c：创建tar包   
 j：使用bzip2压缩格式    
 z：使用gzip压缩格式    
 p：使用绝对路径    
 f：生成的压缩文件的路径    
- (2) 终端操作    
```bash
$ su - root
```
- (3) 使用`gzip`压缩格式（压缩略低，但是速度快）    
```bash
$ tar vzcpf /Downloads/ubuntu_`date +%Y%m%d_%H`.tar.gz --exclude=/proc --exclude=/dev --exclude=/mnt --exclude=/media --exclude=/lost+found --exclude=/cdrom --exclude=/tmp --exclude=/sys --exclude=/home/klm/.cache --exclude=/home/klm/pkgs --exclude=/home/klm/Downloads --exclude=/run  / > /Downloads/ubuntu_`date +%Y%m%d_%H`.log 2> /Downloads/ubuntu_`date +%Y%m%d_%H`.error
```
其中，`-exclude=`表示这些目录并不会被打包。这里有：`/proc，/dev，/mnt，/media，/lost+found，/cdrom，/tmp，/sys，/home/ldd/.cache，/run`。    
如果你的硬盘已经分区了`/home`，则应该对`/home`目录单独备份，或者不要备份。    
- (4) 使用`bzip2`压缩格式（压缩略高，但是速度慢）   
```bash
$ tar vjcpf /Downloads/ubuntu_`date +%Y%m%d_%H`.tar.bz2 --exclude=/proc --exclude=/dev --exclude=/mnt --exclude=/media --exclude=/lost+found --exclude=/cdrom --exclude=/tmp --exclude=/sys --exclude=/home/klm/.cache --exclude=/home/klm/pkgs --exclude=/home/klm/Downloads --exclude=/run  / > /Downloads/ubuntu_`date +%Y%m%d_%H`.log 2> /Downloads/ubuntu_`date +%Y%m%d_%H`.error
```

## 4. 备份两个重要的文件    
 备份`/boot`和`/etc/fstab`.        
```
sudo cp -R /boot /home/klm/boot
sudo cp /etc/fstab /home/klm/fstab
```

## 5. 恢复系统    
将备份文件拷贝到/目录，执行恢复命令：   
5.1 使用`gzip`格式   
```bash
$ su - root
$ tar vxzpf ubuntu*.tar.gz -C /
```
5.2 使用`bzip2`格式   
```bash
$ tar vxjpf ubuntu*.tar.bz2 -C /
```
