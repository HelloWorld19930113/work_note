#在rtems shell中挂载emmc和sd卡到文件系统中
在rtems shell中挂载emmc和sd卡到文件系统中

```
# mount -t dosfs /dev/emmc0 /emmc_mount_p
# mount -t dosfs /dev/sd0 /sd_mount_p
```