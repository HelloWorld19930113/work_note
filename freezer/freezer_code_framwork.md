# 冰柜货物进出识别

学到的实践开发经验：
1. 日志系统的使用区分本地文件和打印信息；
2. 保证系统配置只进行一次，static；



## 初始化
1. 禁用错误输出；
2. 创建两个`SyncThread`对象；
3. 初始化临界区资源锁；
4. 初始化`Log`系统；
5. 获取系统配置参数；
6. 读取商品名和条形码；
7. 创建一个`MultiFrameShow`类型的`demoFrame`对象；
8. 创建一个TimerFrameBase对象；
9. 如果系统配置文件`xxx-config.ini`中包含冰柜的左边和右边的视频，则使用这里的视频。
10. 两个应该是比较重要的FreezerSide *指针，`g_leftptr 和 g_rightptr`；


## `doOpenDoor()`函数
  
该函数中会调用`3个run()`函数；

```cpp
demoFrame->run("演示窗口", 1);
g_left->run();
g_right->run();
```

我们依次来分析这`三个run()`函数：
1. MultiFrameShow::run()
 `run()`函数中最后会启动一个线程函数，线程的入口为`worker()`。
 `this`指代的就是当前对象的指针。`worker()`函数中主要实现的操作是：
 - 创建一个图像窗口。(设置窗口的name，窗口的大小，移动窗口到合适位置后显示窗口图片，这些函数都是opencv库函数)
2. g_left->run()
`g_left`是`FreezerSide`类型的对象。同样的也是会开启一个线程`worker()`来处理任务。该函数是一个`static`函数；
```cpp
static void worker(void* p)
{
	FreezerSide* ptr = (FreezerSide*)p;
	if (systemConfig.multiGPU){
		setupGPU(ptr->doorIndex);
	}
	else{
		setupGPU(0);
	}

	ptr->timer->setCfgThreadid(GetCurrentThreadId());
	ptr->cameras[0]->resume();
	ptr->cameras[1]->resume();

	while (ptr->isrun){
		ptr->timer->update();
		ptr->doWorkByLoop();
		ptr->timer->timeGoing();
	}

	ptr->cameras[0]->pause();
	ptr->cameras[1]->pause();
	ReleaseSemaphore(ptr->threadWait, 1, 0);
	ptr->isrun = false;
}
```
疑问：
setupGPU()函数作用是什么？
GetCurrentThreadId()函数用来获取当前线程的ID。

<1> CameraWorker::resume()函数最终调用到的是CameraWorker::runner()函数：
使用两个摄像头，一个上边，一个下边。

CameraWorker 和 UxFrame 类之间的关系？
openCapture()可以打开摄像头设备和录像文件。
VideoCapture 是opencv中的一个类。
cap.read(frame)表示读取到的图像以矩阵的形式保存在`frame`中。

<2> FreezerSide::doWorkByLoop()
 这个函数才是真正的图像处理循环函数。

先不考虑`FULL_DETECT`情形：
```
1. 获取上下两张图像合并后的图像；
 


```
3. 用户动作识别
 主要是识别用户对商品的操作——拿走/放回去。
```
MultiFrameShow 是核心

- run() 
 另开一个线程，负责图像处理。
 

```


## 循环调度线程

```cpp
{
	int tck = GetTickCount();
	while (systemRunning && GetTickCount() - tck <= 5000000)
		Sleep(10);
}
```




## 程序退出 
 按键为`q`的时候就会退出程序。