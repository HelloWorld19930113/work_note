# Linux SD卡驱动

>@file linux_sd_driver.md
      主要分析的文件是/drivers/mmc/omap_host.c

最初的理由：通过测试`RTEMS`的
`SD卡驱动`发现，并不是总线速度的问题。看来还是在驱动这里存在问题。

几点说明

- 凡是//TODO的地方，都是自己的理解，还未来得及查阅资料确证。
- 一些仍然需要弄明白的问题：
    1. kset，kobj在内核中的地位是什么，它们之前的关系又是什么？
    2. 在驱动加载之前，dev设备应该已经注册到系统中了；那么dev是什么时候注册的？
       mmc_blk_probe()函数中做了很多工作；

# sd卡的driver和device之间的一丝联系
//TODO 从linux2.6之后，设备信息改用了一种特殊机制来传递给内核：`device tree`设备树机制；
因此，设备的信息是以设备树的形式定义的。具体定义如下：

```cpp
@file:
    arch/arm/boot/dts/am33xx.dtsi
    device-tree node 定义了MMCHS0的device硬件设备信息：

    mmc1: mmc@48060000 {
        compatible = "ti,omap4-hsmmc";
        ti,hwmods = "mmc1";
        ti,dual-volt;
        ti,needs-special-reset;
        ti,needs-special-hs-handling;
        dmas = <&edma_xbar 24 0 0
            &edma_xbar 25 0 0>;
        dma-names = "tx", "rx";
        interrupts = <64>;
        interrupt-parent = <&intc>;
        reg = <0x48060000 0x1000>;
        status = "disabled";
    };

@file：
    &mmc1 {
        status = "okay";
        bus-width = <0x4>;
        pinctrl-names = "default";
        pinctrl-0 = <&mmc1_pins>;
        cd-gpios = <&gpio0 6 GPIO_ACTIVE_LOW>;
    };
```
    
而驱动中则是通过 struct of_device_id结构体或者结构体数组来定义驱动；使用结构体数组当然可以定义多个驱动信息，这是因为实际的驱动和设备之前的关系是，一个驱动有时是可以支持多个设备的。

```cpp
@file：
    driver/mmc/host/omap_hsmmc.c
    定义了driver驱动中的匹配信息；
    static const struct of_device_id omap_mmc_of_match[] = {
    ...
        {
            .compatible = "ti,omap4-hsmmc",
            .data = &omap4_mmc_of_data,
        },
    ...
    };
```
  注意到两者的信息中都有一个`共同特征——compatible`没错，将来的设备和驱动的匹配就靠它了。

# SD卡驱动程序的框架

SD卡驱动程序由3部分组成，core-host-card。缺一不可。
//TODO
host中实现了总线功能；
card中实现了卡的功能；
core中实现了platform总线的功能；
那么我们最关心的问题来了，SD卡驱动是怎么运作起来的？这里先给出一个总体的调用流程，接下来在详细分析。 

这里是整个驱动的入口：
```cpp
//file:drivers/mmc/card/block.c
static int __init mmc_blk_init(void)
{
    res = register_blkdev(MMC_BLOCK_MAJOR, "mmc");  //只是注册一个设备号而已

    res = mmc_register_driver(&mmc_driver);  //核心功能都在这个函数中
}
```

1. `mmc_register_driver()`函数
```cpp
int mmc_register_driver(struct mmc_driver *drv)
{
	drv->drv.bus = &mmc_bus_type;
	return driver_register(&drv->drv);
}
```
`mmc_register_driver()`函数执行两个任务:
(1). 将mmc_driver的drv成员中的总线赋值为mmc_bus_type;
(2). 调用driver_register()函数将mmc_driver驱动注册到总线上;

2. `driver_register()`函数
  driver_register()函数将驱动注册到总线中；可是这个函数只有一个drv参数，那到底注册到哪个总线上？只有两种可能性：
- 注册的目的总线在drv参数中被提前设置好了；
- 这条总线是系统维护的一条全局总线；
在上面的mmc_register_driver()函数中可以看到，mmc_driver的drv成员中的总线被赋值为mmc_bus_type，因此，在调用driver_register()函数之前，这条总线就被保存在了drv中。
 
 那 `driver_register(struct device_driver *drv)`函数到底做了什么？
    这部分的详细分析在`what_does_driver_register_do.md`文件中有详细分析！这里仅介绍两个重要的工作：
    将驱动注册到总线上；    klist_add_tail(&priv->knode_bus, &bus->p->klist_drivers);
    调用`bus->probe()`和`drv->probe()`函数；
    至此，驱动和总线的绑定完成。 


##先分析card
>@file: card/block.c

```cpp
static int __init mmc_blk_init(void)
{
    ...
    res = register_blkdev(MMC_BLOCK_MAJOR, "mmc");

    res = mmc_register_driver(&mmc_driver);
    ...
}
```
这是card层的驱动入口函数。主要注册了两个模块，一个是blkdev，一个是mmc_driver。

register_blkdev()只是向内核中注册了一个主设备号。
mmc_register_driver()中调用了函数driver_register(),这个函数是platform 机制中的核心函数。将驱动挂在platform总线上。






##分析core.c

```cpp
static int __init mmc_init(void)
{
	...
	ret = mmc_register_bus();
	if (ret)
		goto destroy_workqueue;

	ret = mmc_register_host_class();
	if (ret)
		goto unregister_bus;

	ret = sdio_register_bus();
	if (ret)
		goto unregister_host_class;
	...
}
```
 这里做了3件事：
 1. 注册mmc_bus_type;
 2. 注册mmc_host_class；
 3. 注册sdio_bus_type；
 
 这里主要是实现platform机制的bus层。最终的mmc_bus_type和mmc_device都会通过这个总线层来match。因此，它是核心core层。
 



##分析host

### host_probe函数；
    of_match_device()函数确定设备是否支持；//TODO

关键寄存器配置：
    CAPA：VS30 | VS18;
    HCTL: MMC_CAP_SD_HIGHSPEED | MMC_CAP_4_BIT_DATA



- 获取设备的匹配数据(包含.compatible域和寄存器offset信息，也叫platform data)；
- 从设备树文件中读取设备的resource资源信息；
- 从设备树文件中读取设备的irq中断信息；
- 将资源ioremap到内核空间中，因此内核中维护两个base:一个是映射后的，一个是未映射的物理地址；
- 获取设备树中定义的host controller_flags和driver中指定的host controller_flags;
- 初始化host capabilities：
    mmc->caps |= MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED |
             MMC_CAP_WAIT_WHILE_BUSY | MMC_CAP_ERASE;
    其中有high_speed,wait while busy, erase;
    其他的还有一些：8 bit data line，nonremovable;
- 设置host的pinctrl state，也就是：
    sdr104
    ddr50
    sdr50
    sdr25
    sdr12
    ddr_1_8v
    hs
    hs200_1_8v
- 请求一条中断线，并注册中断服务函数 omap_hsmmc_irq()；


alloc_host()函数中最重要的是
```cpp
struct mmc_host *mmc_alloc_host(int extra, struct device *dev)
{
	...
	INIT_DELAYED_WORK(&host->detect, mmc_rescan);
	...
}
```
 将扫描卡的函数设置为work_queue，这样在卡状态无法获取时，认为卡被移走，然后使用定时器检测卡是否插入，一旦卡插入之后，执行初始化操作；
 那么假设我们现在是第一次运行驱动程序，那么



mmc_add_host(mmc)->mmc_start_host(mmc)->mmc_gpiod_request_cd_irq(mmc)

