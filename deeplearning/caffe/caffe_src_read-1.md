# caffe源码阅读——基本概念


1. Blobs, Layers, and Nets：Caffe模型解析
 caffe使用blobs结构来存储。交换和处理网络中正向和反向迭代时的数据和导数信息。blob时caffe的标准数组结构，它提供了一个统一的内存接口。Layer是caffe模型和计算的基本单元，Net是一系列layers和其连接的集合。
 blob详细描述了信息时如何在layer和net中存储和交互的。
 
 solver(求解方法)单独配置，以解耦模型的建立与优化过程。

 1.1 blob 的存储与交换
 blob是caffe中处理和传递实际数据的数据封装包，并且在GPU和CPU之间具有同步处理能力。从数学意义上说，blob是按C风格连续存储的N维数组。
 caffe基于blobs存储和交换数据。为了便于优化，
 

