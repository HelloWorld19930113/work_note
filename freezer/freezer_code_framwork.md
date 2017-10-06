# 冰柜商品进出识别

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






## 关键的类定义和功能

1. FreezerSide

```cpp
class FreezerSide{

private:
	list<Mat> recoderFrames;
	string name;
	vector<WPtr<CameraWorker>> cameras;
	vector<WPtr<UxFrame>> frames;
	WPtr<ObjectDetector> detector;
	vector<WPtr<MultiTracker>> multiTracker;
	vector<ClipInfo> clipInfo;
	TimerInterface* timer;
	CameraSource upcamera, downcamera;
	HANDLE threadWait;
	volatile bool isrun;
	Mat bufUpFrame, bufDownFrmae, bufMergeFrame;
	int doorIndex;
	vector<int> cindex;  // 摄像头的数目
	volatile FreezerSide** otherSide;
}

```

2. CameraWorker

```cpp
class CameraWorker{
public:
	CameraWorker(CameraSource source, UxFrame* uxFrame)
		:source(source), uxFrame(uxFrame), isRun(false), run_stage(0){
		threadlock = CreateSemaphoreA(0, 0, 1, 0);
	}

	
private:
	static void runner(void* p);

	volatile int run_stage;
	CameraSource source;
	volatile bool isRun;
	HANDLE threadlock;
	UxFrame* uxFrame;
};
```


3. MultiTracker
```cpp
class MultiTracker{
private:
	CRITICAL_SECTION cs;
	vector<Goods> allGoods;
	TimerInterface* timer;
	string description;
	int posindex;				//所在位置索引，左上0、左下1、右上2、右下3
};

4. 
```cpp
class TraceBox{

private:
	bool hasAppendToShoppintCart;
	bool goback;
	int label;
	int cindex;
	int sdline, dgline, acline;
	bool isUsed;
	bool needAppendNotify_, needGobackNotify_;
	int invalidTraceIndex;
	vector<Point2f> trace;
	vector<DetectObjectInfo> traceBBox;
	ActionType fristAction, endAction;
	bool hide;
	int familyAppendCounter[4];			//记录4个摄像头，同类产品的依赖关系
	int familyGobackCounter[4];			//记录4个摄像头，同类产品的依赖关系
};
```



## caffe相关的API

1. 创建一个分类器
```cpp
Caffe_API Classifier* __stdcall createClassifier(
		const char* prototxt_file,
		const char* caffemodel_file,
		float scale_raw = 1,
		const char* mean_file = 0,
		int num_means = 0,
		float* means = 0,
		int gpu_id = -1);
```
参数含义:





## 程序退出 
 按键为`q`的时候就会退出程序。



## Q&A
1. MultiTracker 中的 allGoods 是如何被初始化的？


2. 分类器
利用`caffe`模型和`deploy.prototxt`文件来创建一个分类器。

3. 商品是被拿出来还是放回去的，应该如何判断？
角度和区域是如何共同决定动作是从Right-Left，还是Left-Right的？
轨迹长度是如何计算的？
allGoods的size()是在什么时候发生变化的,也就是什么时候push_back()或者insert()的?

4. 视频中人为划定的动态区、D区和G区位置
----------------------------------------------
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|      货架区      |   识别区  |  顾客站立区 |
|      (S区)       |  (动态区) |     (G区)   |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
|                  |           |             |
----------------------------------------------

5. 某一个商品的label是如何获取到的?
 分类器检测到的商品信息中包含有label；通过这个label作为匹配的ID。
6. 4个摄像头同类产品的依赖关系如何记录？
```cpp
int familyAppendCounter[4];
int familyGobackCounter[4];
```
如果有两个摄像头的目标被合并了，就会使`familyAppendCounter[i]++`；


## 商品识别中的核心操作
>约定：
从左到右轨迹为取走商品；
从右到左轨迹为放回商品；

1. doActionRec() - do action recognization
 首先，进行第一层筛选：
  (1) 商品的轨迹有效；判断标准?：如果存在超过2帧以上认为是有效的产品/ 轨迹长度大于35，而且不是小轨迹，就认为有效
  (2) 商品的动作无效；判断标准：起始动作和结束动作至少有一个是无效效动作；
 然后，识别用户对此商品的操作(拿走或是放回去)：
   在正式处理之前会有一部分预处理工作：
  (1)  




时间限制，timer相关
 TIME_TYPE nframe = this->timer->now(); // 获取当前相机处理过的图片张数，再用这个数乘以每秒处理图片的频率之后就可以得到当前的时间。
 TIME_TYPE lastTraceTime()函数： 
  返回检测到商品的最后一张图片的处理时间。
 TIME_TYPE createTime() 函数：
  和上一次函数类似，返回检测到商品的第一张图片的处理时间。
  因此，根据这两个时间，可以判断当前处理的图片对应的商品是哪个index。

 // 要求满足大于10个点而且轨迹长度大于100，才做识别，如果不满足就下次消失的时候识别吧
 大于10个点的意思是：处理过十帧数据了；轨迹长度的计算方法是：
 for (int i = 0; i < trace.size() - 1; ++i)
			tlen += L2Dis(trace[i], trace[i + 1]);

if (!(allGoods[k].traceCount() > 10 && allGoods[k].traceLength() > 100))
	continue;



 如果超时的话，就从`allGoods`中移除这个商品。


2. doWorkByLoop() - 核心检测函数
> cindex是{2,3}，一共5个摄像头；但是在目标检测过程中只有4个；即一侧门上安装两个，一共两扇门；

(1) 检测目标(多个)；
(2) matchAndTrackObjects()函数对检测出来的目标，与allGoods[i]相匹配，如果匹配上就加入`addTrace`；如果allGoods[i]中不存在这个商品，就建立新的商品对象；
 for():
 <1> 要处理的数据是检测出的一系列图像帧中的`DetectObjectInfo`目标信息；
 <2> used标志位的作用？
 <3> 如果匹配到allGoods中的一个商品，那么直接合并？到底合并什么东东？
 <4> 
 <5> 将这个匹配到的运动添加到轨迹中；最后会把allGoods中的对象清空(意义何在？是因为购物车已经添加过的原因吗？)。
(3) doActionRec() - do action recognization
 首先，进行第一层筛选：
  (1) 商品的轨迹有效；判断标准?：如果存在超过2帧以上认为是有效的产品/ 轨迹长度大于35，而且不是小轨迹，就认为有效
  (2) 商品的动作无效；判断标准：起始动作和结束动作至少有一个是无效效动作；
 然后，识别用户对此商品的操作(拿走或是放回去)：
   如果是拿走货物，就会将货物加入购物车；





## Log 日志系统





## `SSD resnet`目标检测系统
1. 创建一个分类器
```cpp
dnn = createClassifier(prototxt, caffemodel, 1, 0, 3, means, 0);
```

2. 检测一个目标
```cpp
virtual vector<DetectObjectInfo> detectObject(const Mat& frame, float threshold)
{
	WPtr<BlobData> fr = dnn->extfeature(frame, "detection_out");
	
	return nms(toDetInfo(fr, frame.cols, frame.rows, threshold, now()), 0.2);
	}
```
(1) 使用dnn分类器提取特征；
(2) 调用nms()函数识别商品；
 其中toDetInfo()函数是将分类器提取出的特征值转换为`DetectObjectInfo`检测对象信息。
 实现流程：
  对`检测出的特征`按照score从大到小排序；
  非极大值抑制(基于IOU);

 问题：
  这里分类器检测出来的信息为什么是包含7个数据的？
   是不是在训练的时候指定的网络输出？

>非极大值抑制(NMS)
 非极大值抑制顾名思义就是抑制不是极大值的元素，搜索局部的极大值。这个局部代表的是一个邻域，邻域有两个参数可变，一是邻域的维数，二是邻域的大小。这里不讨论通用的NMS算法，而是用于在目标检测中用于提取分数最高的窗口的。例如在行人检测中，滑动窗口经提取特征，经分类器分类识别后，每个窗口都会得到一个分数。但是滑动窗口会导致很多窗口与其他窗口存在包含或者大部分交叉的情况。这时就需要用到NMS来选取那些邻域里分数最高（是行人的概率最大），并且抑制那些分数低的窗口。
> 重叠面积(IOU)

2. 

## 一些设计思路注释

//原则
//如果跟踪时，目标距离小于一定阈值，而且两个目标的帧数小于一定值时（表明不是长跟踪目标），此时，无论类别是什么，都合并为当前目标（记录目标的频次，避免长跟踪的错误识别问题，类别为频次最高的那个）

//新原则
//1.检测框a和现有候选商品g比较，若能找到一个g'，满足最后显示的时间差、满足最后显示的位置差在一定范围时。认为a是g'的下一个位置
//2.对于每个商品g'，如果商品在D区消失，则消失时间超过一个阈值t，认为其永久消失，此时判断该商品的行为是In or Out。如果商品在G区，该区内的检测结果无论是什么，都将认为该商品是活的

//合并策略：
//D 动态区合并策略：如果时间差短、位接置近即合并他，此时的类别需要根据频次计算的
//G 区合并策略：此时不改变商品类别，如果有检测到同类的产品则更新该产品的位置，如果检测不到则以其他检测到的位置算在一起

//对区域的判断，是指其最后出现的位置在哪个地方
const float thresholdMinTimeOfUnion = 15;		//合并时间值，该时间以内合并他
const float thresholdMinDisOfUnion = 160;	//200
const float thresholdMinDisOfDiffrentLabelUnion = 70;	//label不一致的时候合并要求的距离




