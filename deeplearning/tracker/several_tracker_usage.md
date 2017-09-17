# 几种不同`tracker`算法的源码使用说明

1.TLD
环境：MATLAB2012a vs2013 opencv2.4.10 编译
**作者：qykshr**
出现如下错误是因为编译方法不对，﻿﻿
```
bb_overlap.cpp(36) : error C3861: “min”:  找不到标识符 
bb_overlap.cpp(36) : error C3861: “max”:  找不到标识符 
bb_overlap.cpp(37) : error C3861: “min”:  找不到标识符 
bb_overlap.cpp(37) : error C3861: “max”:  找不到标识符
```
解决方法如下：
注：新版本的opencv已不用编译，下载之后解压即可。

原作者的OpenTLD[下载地址](https://github.com/zk00006/OpenTLD.git)

OpenTLD编译方法如下：
```
(1)、在`matlab命令行`中，执行`mex -setup`，完成编译器配置(`需要安装 Microsoft Software Development Kit (SDK) 7.1`);
(2)、修改`TLD`代码包中的`compile.m`文件，将其中的`Include`和`libpath`换成你电脑中相应的`opencv相应路径`;
(3)、将TLD代码包中`mex文件夹`下 `lk.cpp, fern.cpp, bb_overlap.cpp `文件中的如下代码注释掉，即在每一行前加上`//`;
```cpp
#ifdef _CHAR16T
#define CHAR16_T
#endif
```
(4)、修改`TLD`代码包中`mex文件夹`下`bb_overlap.cpp`文件，在头部加上`#include "minmax.h"`;
(5)、在matlab中执行complie.m文件：
```
>> compile
# 成功提示信息
PC
Compilation finished.
```
如果还是有问题，那么可以尝试修改TLD中的compile.m，将原来的`mex`改为`mex -g`。
```
if ispc
    disp('PC');
    include = ' -Id:\opencv\build\include\opencv\ -Id:\opencv\build\include\';
    libpath = 'd:\opencv\build\x64\vc12\lib\';
    files = dir([libpath '*.lib']);
    
    lib = [];
    for i = 1:length(files),
        lib = [lib ' ' libpath files(i).name];
    end
    
    eval(['mex lk.cpp -O -g' include lib]);
    mex -O -c -g tld.cpp
    mex -O -g fern.cpp tld.obj
    mex -O -g linkagemex.cpp
    mex -O -g bb_overlap.cpp
    mex -O -g warp.cpp
    mex -O -g distance.cpp
end
```6). 运行
<1> 运行本地图片文件，直接执行run_TLD.m文件：

<2> 运行本地摄像头：修改`run_TLD.m`文件的第20行为：
```
opt.source = struct('camera',1,'input','_input/','bb0',[]);
```
然后选择要追踪的初始框，之后双击框内即可开始追踪。


2.TLD
环境：C++版本 vs2013 opencv2.4.10 编译




