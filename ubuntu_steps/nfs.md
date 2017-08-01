## 搭建iTop4412使用的 NFS 环境

## 1. 配置宿主机
将 nfs 挂载点中提前拷贝好rootfs；然后编辑`/etc/exports`文件，添加下面一行：

```
/home/ldd/source/nfs/rootfs  *(rw,sync,no_subtree_check,no_root_squash)
```

## 2. 安装 nfs-kernel-server

```bash
$ sudo apt-get install nfs-kernel-server

$ sudo service nfs-kernel-server restart 

$ sudo gedit /etc/exports
/home/ldd/source/nfs/rootfs  *(rw,sync,no_subtree_check,no_root_squash)


$ showmount -e
Export list for ldd-ubuntu:
/home/ldd/source/nfs/rootfs *

$ sudo mount -t nfs 192.168.1.103:/home/ldd/source/nfs/rootfs tmp
$ ls tmp/
bin  etc          icon  linuxrc  proc  sample.bmp  sys  usr
dev  hello_world  lib   mnt      root  sbin        tmp  var

```


