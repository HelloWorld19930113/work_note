# SD卡驱动

[TOC]

SD卡作为飞行数据的保存介质，是作为一个紧要任务来实现的。一旦SD卡可以工作，shell也就有了存在的可能。
 
 SD卡驱动实现的难点：
 - 选择总线；
 - 熟悉MMC Host 和SD卡之间的通信协议；
 - 熟悉块设备的驱动框架；
 - 块设备的初始化、读写函数；


## SD卡驱动框架
![](https://github.com/gzj2013/markdown_src/raw/master/sd_card_drv.png)

RTEMS的内核层提供了标准的块设备core。我们在实现SD卡这种块设备驱动的过程中需要使用内核提供的core层。
 
 以下是完成sd卡驱动过程中需要用的几个重要的API
1. 创建一个物理磁盘；
```cpp
rtems_status_code rtems_disk_create_phys(
  dev_t dev,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_block_device_ioctl handler,
  void *driver_data,
  const char *name
);
```
>这个函数的作用是：使用设备号@dev创建一个物理磁盘；
>关键的一个设备I/O操作参数是 @ handler。设备名通过@ name

>在具体块设备的初始化函数中检测到一个物理块设备时，会调用这个函数。设备驱动程序中需要提供@ handler的设备IO操作。


2. 实现一个读块请求
```cpp
static inline void rtems_blkdev_request_done(
  rtems_blkdev_request *req,
  rtems_status_code status
)；
```
>这个函数的作用是：发送一个传输请求完成的信号；每一个请求需调用这个函数一次。
>参数 @ req 中有个bufs成员，这个成员是scatter 或 gather缓存区数组。其中每个成员为`rtems_blkdev_sg_buffer`结构体类型，定义如下所示：
```cpp
typedef struct rtems_blkdev_sg_buffer {
  /**
   * Block index.
   */
  rtems_blkdev_bnum block;

  /**
   * Buffer length.
   */
  uint32_t length;

  /**
   * Buffer pointer.
   */
  void *buffer;

  /**
   * User pointer.
   */
  void *user;
} rtems_blkdev_sg_buffer;
```
>因此，这个结构体中包含了每个sg buffer的详细信息。

**NOTE**
总结一下sg buffer和block之间的对应关系。

3. 


###1. 如何检测系统中存在一个物理块设备？
`sd卡`的识别流程如下图所示：
![](https://github.com/gzj2013/markdown_src/raw/master/sd_card_identification.png) 
 上图中有一个重要的环节没有明确的指出来，即在系统上电到进入`idle state`之前。在设备驱动程序中需要对HSMMC控制器模块进行必要的初始化(如：1.开启模块时钟、2. Software Reset.   3. Control Line Reset   4. Set Default Capacity   5 . Set Bus Width    6. Set transmit frequency  7. Send Init Srteam)。

>这里需要注意的是：

###2.块设备的读操作




###3.块设备的写操作




##问题及BUG
###1. SD卡驱动
  在sd卡驱动实现后使用了一段时间出现了一个问题，就是突然在初始化阶段的发送CMD(命令)和接收RSP(命令对应的响应)出现了问题。导致后边的挂载文件系统失败，相当于整个数据日志功能完全丧失。

经过跟踪打印信息后发现是在发送CMD2命令的时候出现了Time_out错误返回码。之前设置的时间是1s，对于没有实际测试SD卡性能的我就以为这个时间已经算是很大的一个等待时间了。结果各种方法都试过之后发现并没什么实质性的改善作用；尝试着将等待时间变成2倍之后，突然全部命令都通过了。原来是等待命令响应的时间太短了；

（但是比较奇怪的是为什么之前跑了很长时间都没有问题，偏偏在移植了PX4和添加了硬件Float运算编译选项之后出现了问题？）

这个问题解决之后并没有顺利搞定。在最后的文件系统挂载阶段还是出现了问题。一般这里出现问题时，最大的可能性就是读取到的Sector中的数据不正确。在打印跟踪之后发现确实如此，第一个sector扇区的所有数据全为0，这是不正确的。但是到底是什么导致了数据读取错误呢？

在读取数据的代码的中间添加了一个printk();函数之后发现数据读取正确了。

原来竟然是获取数据的指针变量没有定义为volatile类型导致的。

```cpp
static int mmcsd_read_data(struct mmchs_reg *mmc_base, char *buf, uint32_t size)
{
	volatile uint32_t *output_buf = (uint32_t *)buf;
	uint32_t count;
	count = (size > MMCSD_SECTOR_SIZE) ? MMCSD_SECTOR_SIZE : size;
	count /= 4; /* readl() returns a data with uint32_t type , 32 bit*/
	while(size){
		if (mmchs_stat & BRR_MASK) {
			
			mmba->stat |= BRR_MASK;
			uint32 k;
			for (k = 0k < unt ; k++) {
				*output_bu= mmc_se->data;
				output_buf++;
			}
			size -= (count*4); }
			...
		}
	return 0;
}
```

###3.
```cpp
const rtems_filesystem_file_handlers_r msdos_file_handlers = {
	.open_h = rtems_filesystem_default_open,
	.close_h = rtems_filesystem_default_close,
	.read_h = msdos_file_read,
	.write_h = msdos_file_write,
	.ioctl_h = rtems_filesystem_default_ioctl,
	...
};
```

可以看到所有的`msdos`的文件操作函数都`msdos_file_handlers`中被指定。这个结构体会被`msdos`的初始化函数`rtems_dosfs_initialize()`赋值。然后如果在`system.h`中定义了如下宏：

```cpp
#define CONFIGURE_FILESYSTEM_DOSFS
```

那么在`confdefs.h`中就会执行如下编译指令：

```cpp
/**
* DOSFS
*/
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_DOSFS) && \
   defined(CONFIGURE_FILESYSTEM_DOSFS)
 #include <rtems/dosfs.h>
 #if !defined(CONFIGURE_MAXIMUM_DOSFS_MOUNTS)
   #define CONFIGURE_MAXIMUM_DOSFS_MOUNTS 1
 #endif
 #define CONFIGURE_FILESYSTEM_ENTRY_DOSFS \
   { RTEMS_FILESYSTEM_TYPE_DOSFS,rtems_dosfs_initialize}
 #define CONFIGURE_SEMAPHORES_FOR_DOSFS CONFIGURE_MAXIMUM_DOSFS_MOUNTS
#else
 #define CONFIGURE_SEMAPHORES_FOR_DOSFS 0
#endif
```
这样，`DOSFS`就会被添加到`rtems`系统的文件系统table列表`rtems_filesystem_table[]`中:


```cpp
const rtems_filesystem_table_t rtems_filesystem_table[] = {
#if !defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
	{ "/", IMFS_initialize_support },
#endif
...
#if defined(CONFIGURE_FILESYSTEM_DOSFS) && \
defined(CONFIGURE_FILESYSTEM_ENTRY_DOSFS)
	CONFIGURE_FILESYSTEM_ENTRY_DOSFS,
#endif
...
	CONFIGURE_FILESYSTEM_NULL
};
```
之后如果要使用该文件系统时，在`mount()`函数中会调用`rtems_filesystem_iterate()`函数，这样就会对选择的文件系统进行初始化：

```cpp
bool rtems_filesystem_iterate(
 rtems_per_filesystem_routine routine,
 void *routine_arg
)
{
	const rtems_filesystem_table_t *table_entry = &rtems_filesystem_table [0];

	while ( table_entry->type && !stop ) {
		stop = (*routine)( table_entry, routine_arg );  //这里执行的就是
		++table_entry;
	}
	...
	return stop;
}
```

在mount()的时候调用该函数；

```cpp
int rtems_dosfs_initialize(
 rtems_filesystem_mount_table_entry_t *mt_entry,
 const void                           *data
)
{
	int rc = 0;
	const rtems_dosfs_mount_options   *mount_options = data;
	rtems_dosfs_convert_control       *converter;

	if (mount_options == NULL || mount_options->converter == NULL) {
		converter = rtems_dosfs_create_default_converter();
	} else {
		converter = mount_options->converter;
	}
	
	if (converter != NULL) {
		rc = msdos_initialize_support(mt_entry,
		                             &msdos_ops,
		                             &msdos_file_handlers,
		                             &msdos_dir_handlers,
		                             converter);
	} else {
		 errno = ENOMEM;
		 rc = -1;
	}

	return rc;
}
```

在这个函数中设置`msdos`的系统调用(函数)

```cpp
static void
msdos_set_handlers(rtems_filesystem_location_info_t *loc)
{
	msdos_fs_info_t *fs_info = loc->mt_entry->fs_info;
	fat_file_fd_t   *fat_fd = loc->node_access;
	
	if (fat_fd->fat_file_type == FAT_DIRECTORY)
		loc->handlers = fs_info->directory_handlers;
	else
		loc->handlers = fs_info->file_handlers;
}
```

以下是MSDOS文件系统的初始化
```cpp


int
msdos_initialize_support(
   rtems_filesystem_mount_table_entry_t    *temp_mt_entry,
   const rtems_filesystem_operations_table *op_table,
   const rtems_filesystem_file_handlers_r  *file_handlers,
   const rtems_filesystem_file_handlers_r  *directory_handlers,
   rtems_dosfs_convert_control             *converter
   )
{
   int                rc = RC_OK;
   rtems_status_code  sc = RTEMS_SUCCESSFUL;
   msdos_fs_info_t   *fs_info = NULL;
   fat_file_fd_t     *fat_fd = NULL;
   fat_dir_pos_t      root_pos;
   uint32_t           cl_buf_size;
   fs_info = (msdos_fs_info_t *)calloc(1, sizeof(msdos_fs_info_t));
   if (!fs_info)
       rtems_set_errno_and_return_minus_one(ENOMEM);
   temp_mt_entry->fs_info = fs_info;
   fs_info->converter = converter;
   rc = fat_init_volume_info(&fs_info->fat, temp_mt_entry->dev);
   if (rc != RC_OK)
   {
       free(fs_info);
       return rc;
   }
   fs_info->file_handlers      = file_handlers;
   fs_info->directory_handlers = directory_handlers;
   /*
    * open fat-file which correspondes to  root directory
    * (so inode number 0x00000010 is always used for root directory)
    */
   fat_dir_pos_init(&root_pos);
   root_pos.sname.cln = FAT_ROOTDIR_CLUSTER_NUM;
   rc = fat_file_open(&fs_info->fat, &root_pos, &fat_fd);
   if (rc != RC_OK)
   {
       fat_shutdown_drive(&fs_info->fat);
       free(fs_info);
       return rc;
   }
   /* again: unfortunately "fat-file" is just almost fat file :( */
   fat_fd->fat_file_type = FAT_DIRECTORY;
   fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;
   fat_fd->cln = fs_info->fat.vol.rdir_cl;
   fat_fd->map.file_cln = 0;
   fat_fd->map.disk_cln = fat_fd->cln;
   /* if we have FAT12/16 */
   if ( fat_fd->cln == 0 )
   {
       fat_fd->fat_file_size = fs_info->fat.vol.rdir_size;
       cl_buf_size = (fs_info->fat.vol.bpc > fs_info->fat.vol.rdir_size) ?
                     fs_info->fat.vol.bpc                                :
                     fs_info->fat.vol.rdir_size;
   }
   else
   {
       rc = fat_file_size(&fs_info->fat, fat_fd);
       if ( rc != RC_OK )
       {
           fat_file_close(&fs_info->fat, fat_fd);
           fat_shutdown_drive(&fs_info->fat);
           free(fs_info);
           return rc;
       }
       cl_buf_size = fs_info->fat.vol.bpc;
   }
   fs_info->cl_buf = (uint8_t *)calloc(cl_buf_size, sizeof(char));
   if (fs_info->cl_buf == NULL)
   {
       fat_file_close(&fs_info->fat, fat_fd);
       fat_shutdown_drive(&fs_info->fat);
       free(fs_info);
       rtems_set_errno_and_return_minus_one(ENOMEM);
   }
   sc = rtems_semaphore_create(3,
                               1,
                               RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO,
                               0,
                               &fs_info->vol_sema);
   if (sc != RTEMS_SUCCESSFUL)
   {
       fat_file_close(&fs_info->fat, fat_fd);
       fat_shutdown_drive(&fs_info->fat);
       free(fs_info->cl_buf);
       free(fs_info);
       rtems_set_errno_and_return_minus_one( EIO );
   }
   temp_mt_entry->mt_fs_root->location.node_access = fat_fd;
   temp_mt_entry->mt_fs_root->location.handlers = directory_handlers;
   temp_mt_entry->ops = op_table;
   return rc;
}

/* rtems_blkdev_generic_write --
*     Generic block device write primitive. Implemented using block device
*     buffer management primitives.
*/
rtems_device_driver
rtems_blkdev_generic_write(
   rtems_device_major_number major RTEMS_UNUSED,
   rtems_device_minor_number minor RTEMS_UNUSED,
   void                    * arg)
{
   rtems_status_code rc = RTEMS_SUCCESSFUL;
   rtems_libio_rw_args_t *args = arg;
   rtems_libio_t *iop = args->iop;
   rtems_disk_device *dd = iop->data1;
   uint32_t block_size = dd->block_size;
   char *buf = args->buffer;
   uint32_t count = args->count;
   rtems_blkdev_bnum block = (rtems_blkdev_bnum) (args->offset / block_size);   //这是要写入块的索引
   uint32_t blkofs = (uint32_t) (args->offset % block_size);
   args->bytes_moved = 0;
   while (count > 0)
   {
       rtems_bdbuf_buffer *diskbuf;
       uint32_t            copy;
       if ((blkofs == 0) && (count >= block_size))
           rc = rtems_bdbuf_get(dd, block, &diskbuf);
       else
           rc = rtems_bdbuf_read(dd, block, &diskbuf);
       if (rc != RTEMS_SUCCESSFUL)
           break;
       copy = block_size blkofs;
       if (copy > count)
           copy = count;
       memcpy((char *)diskbuf->buffer + blkofs, buf, copy);
       args->bytes_moved += copy;
       rc = rtems_bdbuf_release_modified(diskbuf);
       if (rc != RTEMS_SUCCESSFUL)
           break;
       count -= copy;
       buf += copy;
       blkofs = 0;
       block++;
   }
   return rc;
}
```




##参考文档
[SD Specifications_Part 1_Physical Layer Simplified Specification_v4.10_2013.pdf](https://github.com/gzj2013/tech_reference/raw/master/SD%20Specifications_Part%201_Physical%20Layer%20Simplified%20Specification_v4.10_2013.pdf)