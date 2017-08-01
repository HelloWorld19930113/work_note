# RTEMS驱动框架
[TOC]
##RTEMS驱动的初始化
RTEMS提供了两种方法来实现驱动函数的初始化。
1. 在系统启动时与系统一起进行初始化；
2. 在要用到该驱动时再手动调用驱动初始化函数；

对于第一种方法应该特别说明一下，在RTEMS中预留了一个驱动初始化入口<font color=#DC143C size=3 face="黑体">CONFIGURE_APPLICATION_EXTRA_DRIVERS </font>，可以在High Level重新定义。这个入口就是：
```python
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS    NULL_DRIVER
```
对于用户实现的多个驱动，可以使用以下方式：
```python
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS    	   xxx_DRIVER1，		\
												   xxx_DRIVER2，		\
												   xxx_DRIVER3
```
RTEMS中的驱动的函数格式也是系统定义好的。如下：

```c
typedef rtems_device_driver (*rtems_device_driver_entry)(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

typedef struct {
  rtems_device_driver_entry initialization_entry;
  rtems_device_driver_entry open_entry;
  rtems_device_driver_entry close_entry;
  rtems_device_driver_entry read_entry;
  rtems_device_driver_entry write_entry;
  rtems_device_driver_entry control_entry;
} rtems_driver_address_table;
```
<font color=#DC143C size=3 face="黑体">其中，initialization_entry就是驱动的初始化函数。其他的几个分别是open_entry、close_entry、read_entry、write_entry、control_entry。 </font>
只要系统知道了驱动的初始化入口，那么系统启动的时候就会对模块进行初始化。
###初始化函数

驱动的初始化函数的主要任务是：

- 初始化设备驱动中的数据结构体
- 硬件初始化(配置使能时钟、模块reset、中断使能等)
- 将初始化函数中初始化好的设备数据结构体指针保存到设备文件描述符对应的私有数据指针中data1。


## 设备打开函数open_entry
应用层调用open()函数时会最终调用到驱动层中的open_entry()函数，open_entry()函数主要任务有：

- 注册设备的中断服务函数；

## 设备的关闭函数close_entry	
应用层调用close()函数时会最终调用到驱动层中的close_entry()函数，close_entry()函数主要任务有：

- 卸载已注册的中断服务函数；
- 释放设备文件描述符对应的私有数据指针的内存空间，并将其data1置为空NULL。

## 设备的读函数read_entry	
应用层调用read()函数的时候会调用到驱动层的read_entry()函数，实现对设备的读操作。读操作是为了获取设备中缓存下来的数据，对于设备寄存器中有FIFO的清醒，缓存数据的操作应该会分为两个阶段——设备的FIFO缓存和中断服务函数中的数据缓存。对于设备寄存器中没有FIFO的，则只有中断服务函数中的数据缓存。

## 设备的写函数wirte_entry	
应用层调用write()函数的时候会调用到驱动层的write_entry()函数，实现对设备的操作。写函数的任务是将用户层的数据发送到设备中供设备使用，用户下发的数据量很大时，为了防止程序一直在进行数据写操作，应该分两个阶段进行——先将数据保存下来，然后再在合适的时机将数据写入到设备中。

## 设备的配置函数control_entry
应用层调用ioctl()函数的时候会调用到驱动层的control_entry()函数，实现对设备属性的配置操作。一般通过命令+参数的方式在应用层调用。



## RTEMS null 驱动实例代码

```c
#include <bsp/null.h>

rtems_device_driver null_initialization(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
    	rtems_libio_rw_args_t *args = arg;
	rtems_libio_t *iop = args->iop;

	null_dev_t *dev_p = (null_dev_t *)kmalloc(sizeof(null_dev_t));

	iop->data1 = dev_p;                /* Private data pointer */
	
	hardware_init(dev_p);
    	
    	return RTEMS_SUCCESSFUL;
}

rtems_device_driver null_open (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
	rtems_libio_rw_args_t *args = arg;
	rtems_libio_t *iop = args->iop;

	null_dev_t *dev_p = (null_dev_t *)iop->data1;

	...
	
	sc = rtems_interrupt_handler_install(
		Vector,
		"NULL",
		RTEMS_INTERRUPT_SHARED,
		null_isr,
		(void *) (dev_p)
	);

	...
	
	return RTEMS_SUCCESSFUL;
}

rtems_device_driver null_close (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
 	rtems_libio_rw_args_t *args = arg;
	rtems_libio_t *iop = args->iop;

	null_dev_t *dev_p = (null_dev_t *)iop->data1;
	
	...

	sc = rtems_interrupt_handler_remove(
		Vector,
		null_isr,
		(void *) (dev_p)
	);
	
	...
	
	kfree(dev_p);
	iop->data1 = NULL;
	
	return RTEMS_SUCCESSFUL;
}

rtems_device_driver null_read (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
	rtems_status_code rc = RTEMS_SUCCESSFUL;
	rtems_libio_rw_args_t *args = arg;
	rtems_libio_t *iop = args->iop;
	char *buf = args->buffer;
	uint32_t bytes_read = 0;
	args->bytes_moved = 0;

	uint32_t count = args->count;
	if(count > 0)
	{
		null_dev_t *dev_p = (null_dev_t *)iop->data1;
		
		...
		
		bytes_read = copy_data_to_user(buf, count);
		
		...
	}
	
	args->bytes_moved += bytes_read;

	return RTEMS_SUCCESSFUL;
}

rtems_device_driver null_write (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
	rtems_status_code rc = RTEMS_SUCCESSFUL;
	rtems_libio_rw_args_t *args = arg;
	rtems_libio_t *iop = args->iop;
	char *buf = args->buffer;
	uint32_t bytes_write = 0;
	args->bytes_moved = 0;

	uint32_t count = args->count;
	if(count > 0)
	{
		null_dev_t *dev_p = (null_dev_t *)iop->data1;
		
		...
		
		bytes_write = copy_data_from_user(buf, count);
		
		...
	}
	
	args->bytes_moved += bytes_read;

	return RTEMS_SUCCESSFUL;
}

rtems_device_driver null_control (
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg)
{
	rtems_libio_ioctl_args_t *args = (rtems_libio_ioctl_args_t *)arg;
	rtems_libio_t *iop = args->iop;
	args->ioctl_return = 0;
	
	null_dev_t *dev_p = (null_dev_t *)iop->data1;
	
	switch (args->command) {
	default:
		
		...
		
		break;

	case RTEMS_IO_GET_ATTRIBUTES:
		
		...

		break;

	case RTEMS_IO_SET_ATTRIBUTES:
		
		...
		
		break;
	}

	return RTEMS_SUCCESSFUL;
}

static void null_isr(void *param)
{
	printk("Deal with the device isr request\n");
}


#define NULL_DRIVER {		\
	.initialization_entry =null_initialization,		\
	.open_entry = null_open ,	\
	.close_entry = null_close ,	\
	.read_entry = null_read ,		\
	.write_entry = null_write ,	\
	.control_entry = null_control ,	\
}
```