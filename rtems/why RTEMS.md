[TOC]

# Why RTEMS?

1.RTEMS的应用

以下是RTEMS所应用的项目数据来源于RTEMS官网([RTEMS项目应用场合](https://devel.rtems.org/wiki/TBR/UserApp/RTEMSApplications))：

|应用领域|具体项目|
|-----|-----|
|一般领域|Morphable Networked Micro-Architecture (微架构柔性网络)|
||microIOC CosyEye Camera(微型视觉相机)|
|通信领域|Tech S.A.T. GmbH AFDX|
||Tactical Communications Bridge(战术链路通信桥)|
|医学领域|Frye Electronics Fonix7000 hearing aid test system(Fonix7000助听器测试系统)|
|航空航天|NASA Solar Dynamic Observatory(NASA太阳动力天文台2010.2.11)|
||NASA Magnetospheric MultiScale(NASA多尺度磁性层系统2015.3.13)|
||NASA Express Logistics Carrier (国际空间站实验服务系统)|
||Itikka Experiment, REXUS 5 Sounding Rocket (REXUS 5探索火箭2009.3.13)|
||NASA ST5 Satellite (太空科技5号微型卫星2006.5.22)|
|军事领域|Avenger Forward Air Defense System(复仇者前防空系统)|
||MITRE Centaur Robot(MITRE地面自主机器人|

从上述表格可以看出，在航空航天领域中，RTEMS的应用非常普遍。其中NASA的很多项目中都是基于RTEMS完成的。

2. RTEMS的实时性
RTEMS, 即: 实时多处理器系统(Real Time Executive for Multiprocessor Systems)，是一个开源的无版税实时嵌入操作系统RTOS。 它最早用于美国国防系统，早期的名称为实时导弹系统（Real Time Executive for Missile Systems），后来改名为实时军用系统（Real Time Executive for Military Systems），现在由OAR公司负责版本的升级与维护。目前无论是航空航天、军工，还是民用领域RTEMS都有着极为广泛的应用。

从体系结构上来看，RTEMS是微内核抢占式的实时系统。RTEMS中没有提供任何形式的内存管理或进程。它实现了一个单进程，多线程环境。它具有下面的优点：

- 优秀的实时性能
- 支持硬实时和软实时（可抢占内核）
- 支持单调周期调度
- 支持优先级高度协议
- 非常的稳定
- 运行速度快
- 提供遵循ITRON(Industrial The Real-time Operating system Nucleus)的API
- 支持多核处理器（不同于SMP，RTEMS中多个处理器是协作关系）。
- 支持多种CPU，无论是ARM， MIPS，PowerPC，i386还是DSP，AVR，Zilog，都可以找到对应的BSP。
- 高度的可配置性，内核可高度剪裁（目标系统小只有30KB；大可上百兆）1,2,3
- 占用系统资源小，在32位系统中最小的内核只有30Kb左右1,2
- 多任务，支持事件驱动，优先级调度，抢占调度（preemptive scheduling）；
- 支持优先级继承用于解决优先级反转的问题
- 任务间的同步与通信
- 提供POSIX API，Linux/UNIX下的程序可以方便移植，包含POSIX 1003.1b API，并且支持POSIX Thread
- 提供完整的BSD的TCP/IP协议栈以及FTP、WebServer、NFS等服务
- 使用面向对象思想设计，可以大大缩短开发周期
- 核心代码使用C/C++写作，可移植性好
- 支持ISO/ANSI C库，提供可重入的ANSI C库；
- 支持ISO/ANSI C++库以及STL库
- 支持精简的可重入glibc库
- 支持图形用户界面（Microwindows/Nano-X）
- 支持文件系统（FAT，IMFS等）
- 支持多种调试模式（包括GDB，DDD，串口调试，以太网调试）
- 支持32位处理器，Tiny RTEMS项目将对8位和16位处理器进行支持2
- 支持JAVA虚拟机

3. RTEMS的CPU架构支持

RTEMS发展到现在已经已经被移植到了许多目标处理器架构之上，以下处理器架构已经可以完全支持。

- ARM
- Atmel AVR
- Blackfin
- Freescale ColdFire
- Texas Instruments – C3x/C4x DSPs
- Intel – x86 architecture members 80386, Pentium, and above
- LatticeMico32
- 68k
- MIPS
- Nios II
- PowerPC
- Renesas – H8/300, M32C, M32R, SuperH
- SPARC – ERC32, LEON, V9

4.常见实时RTOS比较
加拿大航天局03年对20款成功的RTOS按测试性能排名, 表格选自 A Selection Methodology for the RTOS Market，Philip Melanson, Siamak Tafazoli
![](http://img.my.csdn.net/uploads/201012/18/0_1292643669HN01.gif)
其中，Vxworks是在航空领域应用较早的老牌商业系统。RTEMS在测试重表现的性能指标非常理想，基本上达到或者超过老牌商业系统VxWorks。