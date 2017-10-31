# USB驱动是如何工作的？

## USB驱动中要做什么？


## USB总线




## 驱动组成部分
1. 主机控制器驱动——HCD( Host Controller Driver.)
2. hub驱动(如EHCI、musb)
3. . 总线驱动
4. 设备驱动
- 大容量存储设备(mass storage device class driver) 
- 串口设备
- 网卡设备
- bulk 设备



## 以大容量存储设备为例进行分析

 g_sUSBHCD——保存所有Host控制器的状态信息

1.
```cpp
while(1){
	 关中断；(USB中断还是全局中断？)
	 处理中断；
	 开中断；(USB中断还是全局中断？)
	 
	 如果中断为正常中断，置位state为HCD_DEV_RESET，然后执行一系列的准备工作(如获取USB控制器的速度、申请一个设备描述符、设置USB设备地址);
	 
	usleep(1000);
}
```
while()循环用来保证USB Host 控制器一直处于激活状态。

2.设备操作函数注册
 open()
 连接设备()
 断开设备()


3. 检测USB设备是否就绪



4. 