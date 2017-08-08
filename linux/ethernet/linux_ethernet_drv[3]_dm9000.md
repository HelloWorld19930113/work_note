@(linux device driver)
# linux网卡驱动(3)——DM9000篇

[TOC]

- dm9000的驱动框架
- 与网卡core层相关的操作和重要结构体介绍
- dm9000的寄存器操作
- 


 以dm9000网卡驱动为参考，分析Linux的网卡驱动开发流程，源文件路径：`drivers/net/ethernet/davicom/dm9000.c`

## 准备

 网卡驱动中，网络设备的管理和字符设备、块设备的管理是有差别的。在这里没有了`一切皆文件`的哲学思想，转而使用`socket`套接字。




## 驱动入口

### 1. 注册平台驱动

将驱动注册到总线上，match驱动和设备。`module_platform_driver()`函数会调用`driver_register()`函数，并最终调用驱动的`probe()`函数。

代码清单如下：

```cpp
module_platform_driver(dm9000_driver);
```

### 2. probe函数

   主要完成的任务是：探测设备获得并保存资源信息，根据这些信息申请内存和中断，最后调用register_netdev注册这个网络设备。以下是代码清单，可以分成几个部分来看：

- 1 首先定义了几个局部变量：
        struct dm9000_plat_data *pdata = pdev->dev.platform_data;
         struct board_info *db; /* Point a board information structure */
         struct net_device *ndev;

- 2 初始化一个网络设备。关键系统函数：`alloc_etherdev()`
- 3 获得资源信息并将其保存在`board_info`变量`db`中。关键系统函数：`netdev_priv()`,  `platform_get_resource()`
- 4 根据资源信息分配内存，申请中断等等, 并将申请后的资源信息也保存到db中，并且填充ndev中的参数。 关键系统函数：request_mem_region(),  ioremap()。 自定义函数:dm9000_set_io()
- 5 完成了第4步以后，回顾一下db和ndev中都有了什么：

       struct board_info *db:
                 addr_res -- 地址资源
                 data_res -- 数据资源
                 irq_res    -- 中断资源
                 addr_req -- 分配的地址内存资源
                 io_addr   -- 寄存器I/O基地址
                 data_req -- 分配的数据内存资源
                 io_data   -- 数据I/O基地址
                 dumpblk  -- IO模式
                 outblk     -- IO模式
                 inblk        -- IO模式
                 lock         -- 自旋锁（已经被初始化）
                 addr_lock -- 互斥锁（已经被初始化）

        struct net_device *ndev：
                 base_addr  -- 设备IO地址
                 irq             -- 设备IRQ号

-6 设备复位。硬件操作函数`dm9000_reset()`
- 7 读一下生产商和制造商的ID，应该是`0x9000 0A46`。 关键函数：`ior()`
- 8 读一下芯片类型。
`     ========以上步骤结束后我们可以认为已经找到了DM9000========   `
- 9 借助ether_setup()函数来部分初始化ndev。因为对以太网设备来讲，很多操作与属性是固定的，内核可以帮助完成。
- 10 手动初始化`ndev的ops`和`db的mii`部分。
- 11 如果有的话）从EEPROM中读取节点地址。这里可以看到mini2440这个板子上没有为DM9000外挂EEPROM，所以读取出来的全部是0xff。见函数dm9000_read_eeprom。 关于外挂EEPROM，可以参考datasheet上的7.EEPROM Format一节。
- 12 很显然ndev是我们在probe函数中定义的局部变量，如果我想在其他地方使用它怎么办呢？ 这就需要把它保存起来。内核提供了这个方法，使用函数platform_set_drvdata()可以将ndev保存成平台总线设备的私有数据。以后再要使用它时只需调用platform_get_drvdata()就可以了。
- 13 使用`register_netdev()`注册`ndev`。
