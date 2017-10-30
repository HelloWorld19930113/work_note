# caffe源码阅读——基本概念


1. Blobs, Layers, and Nets：Caffe模型解析
 caffe使用blobs结构来存储。交换和处理网络中正向和反向迭代时的数据和导数信息。blob时caffe的标准数组结构，它提供了一个统一的内存接口。Layer是caffe模型和计算的基本单元，Net是一系列layers和其连接的集合。
 blob详细描述了信息时如何在layer和net中存储和交互的。
 
 solver(求解方法)单独配置，以解耦模型的建立与优化过程。

 1.1 blob 的存储与交换
 blob是caffe中处理和传递实际数据的数据封装包，并且在GPU和CPU之间具有同步处理能力。从数学意义上说，blob是按C风格连续存储的N维数组。
 caffe基于blobs存储和交换数据。为了便于优化，
 


2. `proto`数据结构（类）的标准化操作
```
void CopyFrom();	//在ByteString中定义实现ByteString和字节数组/字符串互相转换函数
void MergeFrom();	//用于合并
void Clear();
bool IsInitialized() const;
int ByteSize() const;

bool MergePartialFromCodedStream();

// 解码时可以调用C++接口ParseFromArray, 编码时可以先调用C++接口ByteSize预先获得编码后的数据大小，让后动态分配内存后调用SerializeToArray进行编码即可。

void SerializeWithCachedSizes() const;//序列化打包
SerializeWithCachedSizesToArray() const;
int GetCachedSize()//打包后出来的大小
void SharedCtor();
void SharedDtor();
void SetCachedSize() const;
```
