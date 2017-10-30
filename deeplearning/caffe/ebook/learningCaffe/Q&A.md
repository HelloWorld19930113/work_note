# 最近训练中遇到的问题及解决方式


1. cudaSuccess (2 Vs 0)
 是因为GPU内存不足，每次处理的数据个数`batch_size`太多。此时只需要将`batch_size`的值该改小一点即可。

2. 因为某一层参数的权重不匹配
 将某一层的`name: "fc10001"`名字更改为别的名字即可。然后就不会使用这层网络参数。

3. 解析`solver.prototxt`出错
 在`solver.prototxt`中的`solver_mode: GPU`参数和训练模型使用的`caffe`中的参数不匹配。

4.`train_.prototxt`和`deploy.prototxt`中的`bias term`参数不匹配
```
caffe:Check failed: target_blobs.size() == source_layer.blobs_size() (2 vs. 1) Incompatible number of blobs for layer conv1
```
 是因为其中一个设置``为false，另外一个文件中并没有设置。
>`bias_term`: 开启偏置项，默认为`true`, 开启.

5.`layer`的参数设置错误
```
F0922 09:56:39.407747 16020 net.cpp:169] Check failed: param_size <= num_param_blobs (2 vs. 1) Too many params specified for layer conv1
```
`deploy.prototxt`文件的层参数设置错误。重新修改层的参数信息即可。

6.如何将`classifier`的`mean`用`vector`来传入？






7.`caffe`编译出错

错误：
```1122	error MSB3073: 命令“copy ..\..\3rd\staticlib\openblas\x64\*.dll ..\..\Build\cpu\Debug\x64\ cop
解决方法：
```
“属性”->“生成事件”->“后期生成时间”->“在生成中使用”->选择“否”。
```

8. 使用训练好的分类模型测试
```
F0922 21:11:46.311563 14896 net.cpp:785] Cannot copy param 0 weights from layer 'fc10001'; shape mismatch.  Source param shape is 4 12544 (50176); target param shape is 11 512 (5632). To learn this layer's parameters from scratch rather than copying from a saved net, rename the layer.
*** Check failure stack trace: ***
```
这是因为`deploy.prototxt`和`train-val.prototxt`中的层参数`num_output`不一致。一般是由于拷贝文件后并没有对相应的参数进行修改。
