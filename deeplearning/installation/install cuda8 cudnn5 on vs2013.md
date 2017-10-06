CUDA 8.0 + VS2013 + win7 x64开发环境搭建 
2017-06-29 17:30 512人阅读 评论(0) 收藏 举报 
 分类： 
机器学习（5）  
本文参考了网络上其它文档，具体不再一个个致谢，前辈们都是强大的指明灯！根据自己的具体情况进行了修改或勘误，血泪之作，仅供菜鸟借鉴使用，注意，菜鸟，高手就不要瞎BB了。
（明明很简单的事情，但是作为新手，这玩意居然折腾了伦家好几天，有些不开心）

安装VS2013。
伦家的VS2013为中文版。
（实验室那个盗版的VS2013不知道哪里缺文件，并不能用；VS2015的社区版本不支持cross，也就算了，装了VS2015的专业版，不知道哪里出问题，居然连界面都进不去，有点愤怒的feel…）

安装cuda_8.0。?
官网下载直接next安装，需要记住安装的路径，不讲。以前的版本分sdk、doc、toolkit三个安装部分，新的变成了Samples、doc、Development，其实木有太大区别，Samples=sdk。
（然而作为菜鸟的我，第一次安装的时候，根据网上历史版本的教程居然不知道哪个是哪个，需要注意的是，如果安装后需要卸载一定要卸载干净，注册表要删了，不然下次一定提示你library目录安装不了）

设置环境变量：
安装完毕后，在计算机上点右键，打开属性->高级系统设置->环境变量，可以看到系统中多了CUDA_PATH和CUDA_PATH_V8_0两个环境变量，接下来，还要在系统中添加以下几个环境变量：
　　CUDA_SDK_PATH = C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0
　　CUDA_LIB_PATH = %CUDA_PATH%\lib\x64
　　CUDA_BIN_PATH = %CUDA_PATH%\bin
　　CUDA_SDK_BIN_PATH = %CUDA_SDK_PATH%\bin\win64
CUDA_SDK_LIB_PATH = %CUDA_SDK_PATH%\common\lib\x64
伦家的安装路径是默认的，所以添加的路径分别是下面这样的：
CUDA_PATH
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0
CUDA_PATH_V8_0
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0
CUDA_SDK_PATH
C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0
CUDA_LIB_PATH
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64
CUDA_BIN_PATH
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin
CUDA_SDK_BIN_PATH
C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\bin\win64
CUDA_SDK_LIB_PATH
C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\common\lib\x64
然后，在系统变量 PATH 的末尾添加：
　;%CUDA_LIB_PATH%;%CUDA_BIN_PATH%;%CUDA_SDK_LIB_PATH%;%CUDA_SDK_BIN_PATH%;
；C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64；C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin；C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\common\lib\x64；C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\bin\win64；
重新启动计算机。
至此，cuda的安装就搞定了。

监测cuda安装成功与否:
这个步骤用到两个东西，都是cuda为我们准备好的: deviceQuery.exe 和 bandwithTest.exe
首先启动cmd DOS命令窗口
默认进来的是c:\users\Admistrator>路径，输入 cd .. 两次，来到c:目录下
输入dir 找到安装的cuda文件夹

直接执行bandwidthTest.exe
该文件的路径：C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\extras\demo_suite

再执行deviceQuery.exe
该文件的路径：C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\extras\demo_suite


Rsult=PASS及说明，都通过了。如果Rsult=Fail 那不好意思，重新安装吧。

配置VS 2013
　　5.1 启动VS2013
5.2 新建一个win32的控制台工程，空的。

伦家的保存路径：c:\users\lvfeiya\documents\visual studio 2013\Projects

5.3 右键源文件文件夹->新建项->选择cuda c/c++- Header>新建一个以.cu结尾的文件

保存路径：
c:\Users\lvfeiya\documents\visual studio 2013\Projects\sevenCuda\sevenCuda\
　　5.4 右键sevenCuda-》生成依赖项-》生成自定义-》选择cuda生成

　　5.5 右键test.cu-》属性-》选择cuda c/c++编译器

5.6 右键工程-》属性-》链接器-》常规-》附加库目录-》添加目录 $(CUDA_PATH_V5_5)\lib\$(Platform);
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib(Platform)
$(CUDA_PATH_V8_0)\lib\$(Platform);
（伦家也不知道究竟是上面的还是下面的格式，，，反正伦家添加的是下面的）

　　5.7 在链接器-》输入中添加 cudart.lib

　　5.8 在工具-》选项-》文本编辑器-》文件扩展名-》添加cu \cuh两个文件扩展名
这个设置是让VS2010编辑.cu文件时，把.cu文件里的C/C++语法高亮。在VS2013的菜单 依次点击：Tools->Options…->Text Editor->File Extension，添加.cu和.cuh，使之采用C++编辑器。

至此，编译环境的相关搭建就完成了。

CUDA测试用例：
```cpp
#include< stdio.h> 
#include "cuda_runtime.h" 
#include "device_launch_parameters.h" 
bool InitCUDA()
{
	int count;
	cudaGetDeviceCount(&count);
	if (count == 0)
	{
		fprintf(stderr, "There is no device.\n");
		return false;
	}
	int i;
	for (i = 0; i < count; i++)
	{
		cudaDeviceProp prop;
		if (cudaGetDeviceProperties(&prop, i) == cudaSuccess)
		{
			if (prop.major >= 1)
			{
				break;
			}
		}
	}
	if (i == count)
	{
		fprintf(stderr, "There is no device supporting CUDA 1.x.\n");
		return false;
	}
	cudaSetDevice(i);
	return true;
}

int main()
{
	if (!InitCUDA())
	{
		return 0;
	}
	printf("HelloWorld, CUDA has been initialized.\n");

	return 0;
}
```

## 安装cudnn
安装cudnn比较简单，简单地说，就是复制几个文件：库文件和头文件。将cudnn的头文件复制到cuda安装路径的include路径下，将cudnn的库文件复制到cuda安装路径的lib64路径下。
#解压文件
tar -zxvf cudnn-6.5-linux-x64-v2.tgz
#切换路径
cd cudnn-6.5-linux-x64-v2
#复制lib文件到cuda安装路径下的lib64/
sudo cp lib* /usr/local/cuda/lib64/
#复制头文件
sudo cp cudnn.h /usr/local/cuda/include/

#更新软连接
cd /usr/local/cuda/lib64/ 
sudo rm -rf libcudnn.so libcudnn.so.6.5 
sudo ln -s libcudnn.so.6.5.48 libcudnn.so.6.5 
sudo ln -s libcudnn.so.6.5 libcudnn.so 
到目前为止，cudnn已经安装完了。但是，是否安装成功了呢，还得通过下面的cudnn sample测试。
#运行cudnn-sample-v2
tar –zxvf cudnn-sample-v2.tgz
cd cudnn-sample-v2
make
./mnistCUDNN
#改程序运行成功，说明cudnn安装成功。

此时可能出现错误：./mnist CUDNN: error while loading shared libraries: libcudart.so.6.5: cannot ope
解决方法参考 https://groups.google.com/forum/#!topic/caffe-users/dcZrE3-60mc























安装Anaconda
Anaconda包含了Theano所需的各种Python库，用它来安装python环境的确非常方便，安装的时候勾选把Anaconda加到环境变量中。Anaconda的位数要跟你的cuda版本一样，选择的都是64 bit。
安装目录我用的E:\Anapy2。
安装完检查一下，打开cmd命令行：

装完以后别着急下一步：
验证BLAS是否安装成功
由于numpy是依赖BLAS的，如果BLAS没有安装成功，虽然numpy亦可以安装，但是无法使用BLAS的加速。验证numpy是否成功依赖BLAS编译：
python
import numpy
id(numpy.dot) == id(numpy.core.multiarray.dot)
False
结果为False表示成功使用BLAS加速，如果是Ture则表示用的是python的实现，没有加速。 状况不解决的话，之后theano库跑一些例子会爆类似cannot find -lf77blas-lcblas-latlas的错误.简单处理：重装
（伦家这个居然怎么弄都是false，~~(>_<)~~，教程里都是骗人的，伦家试了，重装无效）
也可以考虑MKL（收费的）和OpenBLAS。
这里给出两个库的一些安装方面的博客和讨论帖：
http://ijiaer.com/python-with-mkl-hpc/（mkl）
https://www.kaggle.com/c/otto-group-product-classification-challenge/forums/t/13973/a-few-tips-to-install-theano-on-windows-64-bits/93135（OpenBLAS）

安装MingW
貌似Anaconda-2.之前的版本是内置了MinGW环境的，之后就不带MinGW了。（查看C:\Anaconda下有无MinGW目录）
安装方法：
打开CMD（注意是windows命令提示符，并不是进入到python环境下）；
输入conda install mingw libpython，然后回车，会出现安装进度，稍等片刻即可安装完毕。此时就有C:\Anaconda\MinGw目录了。 （网速一定要好，网速不好会出现一串的false）
可在windows命令行中输入g++ -v来查看是否安装成功：

环境配置
1). 编辑用户变量中的path变量，在后边追加C:\Anaconda;C:\Anaconda\Scripts; 不要漏掉分号，此处需要根据自己的安装目录填写。
伦家的路径：E:\Anapy2; E:\Anapy2\Scripts
2). 在你的系统环境变量里面的path中加入 C:\Anaconda\MinGW\bin;C:\Anaconda\MinGW\x86_64-w64-mingw32\lib即可。
E:\Anapy2\MinGW\bin; E:\Anapy2\MinGW\x86_64-w64-mingw32\lib
在系统变量中新建变量PYTHONPATH，变量值为C:\Anaconda\Lib\site-packages\theano;
PYTHONPATH
E:\Anapy2\Lib\site-packages\theano;
此处就是指明安装的theano的目录是哪，theano会在后面安装，正常安装的默认路径就是这个。
3). 打开cmd，会看到窗口里边有个路径C:\Users\FYB>，即home目录，在此目录下新建 .theanorc.txt 文件（作为theano的配置文件，注意名字中的第一个“.”，如果已经存在，则直接修改该文件），设置如下内容：
所谓cmd的home目录：打开cmd时，在>前面的默认路径：

[global]
openmp=False
[blas]
ldflags =
[gcc]
cxxflags = -ID:\Anaconda\MinGW\include
cxxflags = -IE:\Anapy2\MinGW
一定要是你安装的Anaconda的路径，一定不要弄错，否则找不到MinGw。
重启电脑！
测试theano是否安装成功
测试方法1 ：
import theano
print theano.config.blas.ldflags

没有出错(没有返回值)则说明已经配置成功。
其实单单是import theano不报错就已经谢天谢地了。
测试方法2
用下面的指令测试（测试时会有其他错误提示或是warnings，但基本上还能运行的话则说明theano没问题，错误提示可能是有些东西还没安装好）：
import theano
theano.test()
运行：

测试时示没有nose-parameterized这个模块，安装方法：
pip install nose-parameterized
注意：测试2必须在cpu下运行，如果配置了theano的device = gpu，则测试2就不能运行了。

使用GPU
上面的theano配置只是完成了上半部分，这个时候还不能进行gpu加速。如果使用GPU则需要继续以下步骤：
Theano文件配置(GPU)——编辑Theano的配置文件.theanorc.txt , 添加如下内容：
[global]
openmp = False
device = gpu
floatX = float32
allow_input_downcast = True
base_compiledir = path_to_a_directory+without_such_characters
[blas]
ldflags =
[gcc]
cxxflags=-IE:\Anapy2\MinGW
[nvcc]
fastmath = True
flags = -LE:\Anapy2\libs
compiler_bindir = H:\VS2013\VC\bin
[lib]
cnmem=.75
在Python中运行”import theano.sandbox.cuda”. 将会编译第一个Cuda文件, 应当没有错误产生。
测试是否使用GPU
测试方法1：
import theano

讲道理，出现红框就说明已经配置好了！
如果是CNMeM is disabled，就是因为你没有在Theano的配置文件.theanorc.txt里面添加
[lib]
cnmem=.75
如果就cuDNN不能用，就需要去官网下载cuDNN，需要先注册，下载以后把解压后的三个子文件分别添加到C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0下面相应的bin、lib、include文件里。（很奇怪的是在没有添加之前是没有出现篮框里面的bug和警告的，添加以后就出现警告了，而且无解，前辈们都说这个不影响运行，然后宝宝每次看到这些都感觉心里苦极了）
编译目标GPU指令集的命令选项
-arch: 指定nvcc编译目标GPU的型号，可以为“real” GPU，也可以为“virtual” PTX架构。这个选项指定了nvcc编译到PTX阶段的目标架构，而-code选项指定了nvcc最后阶段生成的运行时代码的目标架构。现阶段支持的架构类型有：virtual 架构 compute_10, compute_11, compute_12, compute_13和实现这些虚架构的real GPU sm_10, sm_11, sm_12, sm_13.
测试方法2：
启动Spyder，下面的Theano GPU加速测试案例代码并运行
 #!/usr/bin/env python
 # -- coding: utf-8 --

 “””
 功能：测试是否使用GPU
 时间：2016年6月10日 11:20:10
 “””

 from theano import function, config, shared, sandbox
 import theano.tensor as T
 import numpy
 import time

 vlen = 10*30*768 # 10 x cores x threads per core
 iters = 1000

 rng = numpy.random.RandomState(22)
 x = shared(numpy.asarray(rng.rand(vlen), config.floatX))
 f = function([], T.exp(x))
 print(f.maker.fgraph.toposort())
 t0 = time.time()
 for i in range(iters):
 r = f()
 t1 = time.time()
 print(‘Looping %d times took’ % iters, t1 - t0, ‘seconds’)
 print(‘Result is’, r)

 if numpy.any([isinstance(x.op, T.Elemwise) for x in f.maker.fgraph.toposort()]):
 print(‘Used the cpu’)
 else:
 print(‘Used the gpu’)
如果出现下图所示的运行结果，其中有一条“Used the gpu”信息输出，表明Theano GPU加速测试成功。

有木有发现我这里内存很高，关闭spyder，再从电脑左下角的程序那里打开，竟然加载不动，不是kernal failed就是进不去界面，不知道是什么幺蛾子，果断cmd命令行卸载重装，然而并木有鸟用！但是可以从Anaconda的安装文件里找到spyder.exe文件（路径是：E:\Anapy2\Scripts），发送快捷方式到桌面再打开，搞定！伦家估计是因为前面重装了好几次Anaconda，可能注册表啥子的木有删除完全。
终于剩下两个不影响使用的bug:
1， 不能使用blas加速；
2， Python运行的时候会提示DEBUG: nvcc STDOUT nvcc warning.
Bingo ！！！