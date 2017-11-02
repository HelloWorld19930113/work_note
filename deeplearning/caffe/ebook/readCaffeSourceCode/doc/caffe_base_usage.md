# `caffe`中的约定用法

1. `LOG`和`LOG_IF`
`caffe`使用了`glog`，在`caffe`的`solver`中输出都是用的`LOG`和`LOG_IF`。      
1.1 `LOG_IF`是条件输出：
```cpp
LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
```
`,`后面是条件，这里是满足大于10就输出后面的字符串
1.2 `LOG`就是一般输出：
```cpp
LOG(INFO) << "Hello World";
LOG(ERROR) << "Hello World";
```
`INFO`表示提示信息，`ERROR`表示输出错误信息，都是输出后面的`Hello World`；`ERROR`输出应该是报错的红色：

这是`solver.cpp`中的一段代码：
```
LOG(INFO) << "Solving " << net_->name();
```
这就是输出某个参数的格式
```
LOG_IF(INFO, Caffe::root_solver()) << "Iteration " << iter_
          << ", loss = " << smoothed_loss_;
```
关于glog的详细介绍，请参考[这里](http://www.cnblogs.com/tianyajuanke/archive/2013/02/22/2921850.html)