# linux platform 驱动模型分析

> 本文中使用的linux 内核版本是3.0.15。

## 一. 概述

  platform设备和驱动与linux设备模型密切相关。platform在linux设备模型中，其实就是一种虚拟总线没有对应的硬件结构。它的主要作用就是管理系统的外设资源，比如io内存,中断信号线。现在大多数处理器芯片都是soc，如s3c2440，它包括处理器内核（arm920t）和系统的外设（lcd接口，nandflash接口等）。linux在引入了platform机制之后，内核假设所有的这些外设都挂载在platform虚拟总线上,以便进行统一管理。
    
## 二. platform 总线

### 1. platform 总线初始化
  在系统中platform对应的文件`drivers/base/platform.c`，它不是作为一个模块注册到内核的，关键的注册总线的函数由系统初始化部分，对应`/init/main.c中的do_basic_setup`函数间接调用。这里可以看出platform非常重要，要在系统其他驱动加载之前注册。下面分析platform总线注册函数

```cpp
int __init platform_bus_init(void)  
{  
    int error;  
    early_platform_cleanup();  
    error = device_register(&platform_bus);       //总线也是设备，所以也要进行设备的注册  
    if (error)  
        return error;  
    error =  bus_register(&platform_bus_type);   //注册platform_bus_type总线到内核  
    if (error)  
        device_unregister(&platform_bus);  
    return error;  
}  
```
   这个函数向内核注册了一种总线。首先由`/drivers/base/init.c`中的`driver_init`函数调用，`driver_init`函数由`/init/main.c中的do_basic_setup`函数调用，`do_basic_setup`这个函数由`kernel_init`调用，所以`platform总线是在内核初始化的时候就注册进了内核`。

### 2. platform_bus_type 总线结构与设备结构

#### (1) platform总线的设备结构——struct device

```cpp
struct device platform_bus = {  
    .init_name  = "platform",  
};  
```
   platform总线也是一种设备，这里初始化一个`device结构`，设备名称`platform`， 因为没有指定父设备，所以注册后将会在`/sys/device/`下出现`platform`目录。
 
#### (2) platform总线的总线结构——struct bus_type

```cpp
struct bus_type platform_bus_type = {  
    .name       = "platform",  
    .dev_attrs  = platform_dev_attrs,   
    .match      = platform_match,  
    .uevent     = platform_uevent,  
    .pm     = &platform_dev_pm_ops,  
};  
```

  platform_dev_attrs    设备属性
  platform_match        match函数，这个函数在当属于platform的设备或者驱动注册到内核时就会调用，完成设备与驱动的匹配工作。
  platform_uevent       热插拔操作函数

## 三. platform 设备

### 1. platform_device 结构

```cpp
struct platform_device {  
    const char  * name;  
    int     id;  
    struct device   dev;  
    u32     num_resources;  
    struct resource * resource;  
    struct platform_device_id   *id_entry;  
    /* arch specific additions */  
    struct pdev_archdata    archdata;  
};  
```
#### (1) struct resource结构体
 platform_device结构体中有一个struct resource结构，是设备占用系统的资源，定义在`include/linux/ioport.h`中，如下:

```cpp
struct resource {  
    resource_size_t start;  
    resource_size_t end;  
    const char *name;  
    unsigned long flags;  
    struct resource *parent, *sibling, *child;  
}; 
```
#### (2) num_resources   
  占用系统资源的数目，一般设备都占用两种资源，io内存和中断信号线。num_resources为两种资源的总和。

### 2. platform_device_register——设备注册函数 

```cpp
int platform_device_register(struct platform_device *pdev)  
{  
    device_initialize(&pdev->dev);  
    return platform_device_add(pdev);  
}  
```

  这个函数首先初始化了platform_device的device结构，然后调用`platform_device_add()`，这个函数是注册函数的关键，下面分析`platform_device_add()`：

```cpp
int platform_device_add(struct platform_device *pdev)  
{  
    int i, ret = 0;  
  
    if (!pdev)  
        return -EINVAL;  
  
    if (!pdev->dev.parent)  
        pdev->dev.parent = &platform_bus;  
        //可以看出，platform设备的父设备一般都是platform_bus，所以注册后的platform设备都出现在/sys/devices/platform下  
    pdev->dev.bus = &platform_bus_type;   //挂到platform总线上  

    if (pdev->id != -1)  
        dev_set_name(&pdev->dev, "%s.%d", pdev->name,  pdev->id);  
    else  
        dev_set_name(&pdev->dev, "%s", pdev->name);  
        //设置设备名字，这个名字与/sys/devices/platform_bus下的名字对应  

    for (i = 0; i < pdev->num_resources; i++) { //下面操作设备所占用的系统资源  
        struct resource *p, *r = &pdev->resource[i];  
  
        if (r->name == NULL)  
            r->name = dev_name(&pdev->dev);  
  
        p = r->parent;  
        if (!p) {  
            if (resource_type(r) == IORESOURCE_MEM)  
                p = &iomem_resource;  
            else if (resource_type(r) == IORESOURCE_IO)  
                p = &ioport_resource;  
        }  
  
        if (p && insert_resource(p, r)) {  
            printk(KERN_ERR  
                   "%s: failed to claim resource %d\n",  
                   dev_name(&pdev->dev), i);  
            ret = -EBUSY;  
            goto failed;  
        }  
    }  
       //上面主要是遍历设备所占用的资源，找到对应的父资源，如果没有定义，那么根据资源的类型，分别赋值为`iomem_resource`和`ioport_resource`，然后调用`insert_resource()`插入资源。  
       
       //使用父资源这样的系统的目的是形成一个树形的数据结构，便于系统的管理 

    pr_debug("Registering platform device '%s'. Parent at %s\n",  
         dev_name(&pdev->dev), dev_name(pdev->dev.parent));  
  
    ret = device_add(&pdev->dev);     // 注册到设备模型中  
    if (ret == 0)  
        return ret;  
failed:  
    while (--i >= 0) {  
        struct resource *r = &pdev->resource[i];  
        unsigned long type = resource_type(r);  
        if (type == IORESOURCE_MEM || type == IORESOURCE_IO)  
            release_resource(r);  
    }  
    return ret;  
}  
```
 在函数的最后调用了 `device_add`函数注册设备模型，这个函数调用完毕之后，`platform设备`就已经存在于内核中了，它就会在内核中默默等待与属于它的`platform驱动`相匹配。设备是和设备树有关的，设备树在系统初始化阶段就已经被解析过了；而platform驱动可能在系统启动之后的某个时刻才会加载。因此，在platform机制中，一般是platform设备在等待platform驱动。

### 3. mini2440内核注册platform设备过程
  因为一种soc确定之后，其外设模块就已经确定了，所以注册`platform设备`就由板级初始化代码来完成，在`mini2440`中是`mach-mini2440.c`的`mini2440_machine_init函数`中调用` platform_add_devices(mini2440_devices, ARRAY_SIZE(mini2440_devices))`来完成注册。这个函数完成mini2440的所有platform设备的注册：

```cpp
static void __init mini2440_init(void)
{
    ... 
    platform_add_devices(mini2440_devices, ARRAY_SIZE(mini2440_devices));
    ...
}
```

(1) `platform_add_devices函数`是`platform_device_register`的简单封装，它向内核注册一组`platform设备`。关于该函数之前已经进行过详细介绍。
(2) mini2440_devices是一个platform_device指针数组，定义如下：

```cpp
static struct platform_device *mini2440_devices[] __initdata = {  
    &s3c_device_usb,  
    &s3c_device_rtc,  
    &s3c_device_lcd,  
    &s3c_device_wdt,  
    &s3c_device_i2c0,  
    &s3c_device_iis,  
    &mini2440_device_eth,  
    &s3c24xx_uda134x,  
    &s3c_device_nand,  
    &s3c_device_sdi,  
    &s3c_device_usbgadget,  
};  
```
  这个就是mini2440的所有外设资源了，每个外设的具体定义在`/arch/arm/plat-s3c24xx/devs.c`，下面以`s3c_device_lcd`为例说明，其他的类似。`s3c_device_lcd`在devs.c中它定义为：

```cpp
struct platform_device s3c_device_lcd = {  
    .name         = "s3c2410-lcd",  
    .id       = -1,  
    .num_resources    = ARRAY_SIZE(s3c_lcd_resource),  
    .resource     = s3c_lcd_resource,  
    .dev              = {  
        .dma_mask       = &s3c_device_lcd_dmamask,  
        .coherent_dma_mask  = 0xffffffffUL  
    }  
}; 
```

 可以看出，它占用的资源是 `s3c_lcd_resource`，定义如下：

```cpp
static struct resource s3c_lcd_resource[] = {  
    [0] = {  
        .start = S3C24XX_PA_LCD,  
        .end   = S3C24XX_PA_LCD + S3C24XX_SZ_LCD - 1,  
        .flags = IORESOURCE_MEM,  
    },  
    [1] = {  
        .start = IRQ_LCD,  
        .end   = IRQ_LCD,  
        .flags = IORESOURCE_IRQ,  
    }  
};  
```

   这是一个数组，有两个元素，说明lcd占用了系统两个资源，一个资源类型是`IORESOURCE_MEM`代表`io内存`，起使地址`S3C24XX_PA_LCD`，这个是`LCDCON1`寄存器的地址。另外一个资源是中断信号线。

 
## 四. platform设备驱动
  如果要将所写的驱动程序注册成`platform驱动`，那么所做的工作就是初始化一个`platform_driver`，然后调用`platform_driver_register`进行注册。

### 1. 基本数据结构——platform_driver

```cpp
struct platform_driver {  
    int (*probe)(struct platform_device *);  
    int (*remove)(struct platform_device *);  
    void (*shutdown)(struct platform_device *);  
    int (*suspend)(struct platform_device *, pm_message_t state);  
    int (*resume)(struct platform_device *);  
    struct device_driver driver;  
    struct platform_device_id *id_table;  
};  
```
    这是platform驱动基本的数据结构，在驱动程序中我们要做的就是声明一个这样的结构并初始化。下面是lcd驱动程序对它的初始化：


```cpp
static struct platform_driver s3c2412fb_driver = {  
    .probe      = s3c2412fb_probe,  
    .remove     = s3c2410fb_remove,  
    .suspend    = s3c2410fb_suspend,  
    .resume     = s3c2410fb_resume,  
    .driver     = {  
        .name   = "s3c2412-lcd",  
        .owner  = THIS_MODULE,  
    },  
};  
```
 上面几个函数是我们要实现的，它将赋值给`device_driver`中的相关成员(稍后就可以看到)，`probe函数`是用来查询特定设备是否真正存在的函数。当设备从系统删除的时候调用`remove函数`。

### 2. platform 驱动注册函数——platform_driver_register

```cpp
int platform_driver_register(struct platform_driver *drv,
                struct module *owner)
{
    drv->driver.owner = owner;
    drv->driver.bus = &platform_bus_type;
    drv->driver.probe = platform_drv_probe;
    drv->driver.remove = platform_drv_remove;
    drv->driver.shutdown = platform_drv_shutdown;

    return driver_register(&drv->driver);
} 
```

  这个函数首先使驱动属于`platform_bus_type总线`，将`platform_driver`结构中的定义的`probe，remove,shutdown`赋值给`device_driver`结构中的相应成员，以供`linux设备模型核心`调用，然后调用`driver_regster`将设备驱动注册到`linux设备模型核心`中。`driver_regster`会调用`bus_add_driver`将一个驱动添加到 `paltform总线` 上, `bus_add_driver`会调用`driver_attach`会将驱动绑定到匹配符合条件的设备上，下面是它的内部实现：

```cpp
int bus_for_each_dev(struct bus_type *bus, struct device *start,
             void *data, int (*fn)(struct device *, void *))
{
    struct klist_iter i;
    struct device *dev;
    int error = 0;

    if (!bus)
        return -EINVAL;

    klist_iter_init_node(&bus->p->klist_devices, &i,
                 (start ? &start->p->knode_bus : NULL));   // 获取保存在bus中设备链表
    while ((dev = next_device(&i)) && !error)     //调用__driver_attach()迭代匹配设备和驱动
        error = fn(dev, data);
    klist_iter_exit(&i);
    return error;
}
```
  可以看到，这个函数的核心是`error = fn(dev, data);`，而其中的`fn()`就是`__driver_attach()`。其具体定义如下：

```cpp
static int __driver_attach(struct device *dev, void *data)
{
    struct device_driver *drv = data;

    if (!driver_match_device(drv, dev))
        return 0;

    if (dev->parent)    /* Needed for USB */
        device_lock(dev->parent);
    device_lock(dev);
    if (!dev->driver)
        driver_probe_device(drv, dev);   //尝试执行驱动的探测函数
    device_unlock(dev);
    if (dev->parent)
        device_unlock(dev->parent);

    return 0;
}
```
 `driver_probe_device`函数会调用`really_probe`函数，在`really_probe`函数中

```cpp
static int really_probe(struct device *dev, struct device_driver *drv)
{
    int ret = 0;

    dev->driver = drv;
    if (driver_sysfs_add(dev)) {   // 将 platform 设备节点添加到/sys/devices/platform目录下
        printk(KERN_ERR "%s: driver_sysfs_add(%s) failed\n",
            __func__, dev_name(dev));
        goto probe_failed;
    }

    if (dev->bus->probe) {
        ret = dev->bus->probe(dev);    // 在这里就会执行 platform 驱动的probe函数。
        if (ret)
            goto probe_failed;
    } else if (drv->probe) {
        ret = drv->probe(dev);     // platform bus 的 platform 驱动的 probe函数是相同的。
        if (ret)
            goto probe_failed;
    }

    driver_bound(dev);        // 将设备驱动添加到驱动的设备列表中
    ret = 1;

    pr_debug("bus: '%s': %s: bound device %s to driver %s\n",
         drv->bus->name, __func__, dev_name(dev), drv->name);
    goto done;

probe_failed:
    devres_release_all(dev);
    driver_sysfs_remove(dev);
    dev->driver = NULL;

    if (ret != -ENODEV && ret != -ENXIO) {
        /* driver matched but the probe failed */
        printk(KERN_WARNING
               "%s: probe of %s failed with error %d\n",
               drv->name, dev_name(dev), ret);
    }

    /* Ignore errors returned by ->probe so that the next driver can try its luck.*/
    ret = 0;
done:
    atomic_dec(&probe_count);
    wake_up(&probe_waitqueue);
    return ret;
}
```
 到此，`platform_driver_register`函数就完成它的全部使命了。

## 五. 各环节的整合——真的是这样的吗？
  前面提到`mini2440`板级初始化程序将它所有的`platform设备`注册到了`linux设备模型核心`中，在`/sys/devices/platform目录`中都有相应的目录表示。`platform驱动`则是由各个驱动程序模块分别注册到系统中的。但是他们是如何联系起来的呢，这就跟linux设备模型核心有关系了。在`ldd3中的linux设备模型的各环节的整合`中有详细的论述。这里简要说明一下`platform实现`的方法。每当注册一个`platform驱动`的时候就会调用`driver_register`，这个函数的调用会遍历设备驱动所属总线上的所有设备，并对每个设备调用`总线的match函数`。`platform驱动`是属于`platform_bus_type总线`，所以调用`platform_match函数`。这个函数实现如下：

```cpp
static int platform_match(struct device *dev, struct device_driver *drv)  
{  
    struct platform_device *pdev = to_platform_device(dev);  
    struct platform_driver *pdrv = to_platform_driver(drv);  
  
    /* match against the id table first */  
    if (pdrv->id_table)  
        return platform_match_id(pdrv->id_table, pdev) != NULL;  

    /* fall-back to driver name match */  
    return (strcmp(pdev->name, drv->name) == 0);  
}  
```
  这个函数`将device结构转换为platform_devcie结构`，`将device_driver结构转换为platform_driver结构`，前面这两步的实现就是通过调用`container_of()`。然后调用`platform_match_id`对设备与驱动相关信息进行比较。如果没有比较成功会返回`NULL`,以便进行下一个设备的比较，如果比较成功就会返回`对应的id 结构体指针`，并且将device结构中的driver指针指向这个驱动。然后调用`device_driver中的probe函数`, 在`lcd驱动`中就是`s3c2412fb_probe`。 这个函数是我们要编写的函数。这个函数检测驱动的状态，并且测试能否真正驱动设备，并且做一些初始化工作。
