# Caffe 在Ubuntu 14.04 64bit 下的安装
  关于Ubuntu 版本的选择，建议用ubuntu 14.04这个比较稳定的版本。

>声明： 如果没有出现问题，直接过就好。如果出现问题了，可按照文章末尾提供的问题解决方案来做。

## 1. 准备材料（NVIDIA官网下载）：
`显卡驱动`：NVIDIA-Linux-x86_64-367.44.run,(可以不使用这个，直接使用sudo apt-get install nvidia-367)

`Cuda8.0`:cuda_8.0.44_linux.run
下载网址：https://developer.nvidia.com/cuda-downloads

`Cudnn`：cudnn-8.0-linux-x64-v5.1.tgz
下载网址：https://developer.nvidia.com/cudnn

## 2. 安装步骤
2.1 系统安装
系统选择ubuntu14.04，下载后ultrISO制作到U盘安装，不细说了。关闭系统更新。

2.2 安装依赖
2.2.1 安装编译工具：
```bash
$ sudo apt-get install build-essential # basic requirement
$ sudo apt-get install cmake git
$ sudo apt-get update    #update source
```
2.2.2 安装依赖项： 
```bash
$ sudo apt-get install libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev protobuf-compiler 
$ sudo apt-get install --no-install-recommends libboost-all-dev
$ sudo apt-get install libopenblas-dev liblapack-dev libatlas-base-dev
$ sudo apt-get install libgflags-dev libgoogle-glog-dev liblmdb-dev
$ sudo apt-get install python-numpy python-scipy python-matplotlib
```

2.3 禁用nouveau驱动
**这一步可忽略**
`ALT+CTRL+F1`,进命令行；
```bash
$ sudo service lightdm stop
$ sudo apt-get --purge remove nvidia-*
```
新建黑名单，禁止系统自带驱动：
```bash
$ sudo vi /etc/modprobe.d/blacklist-nouveau.conf
```
写入： 
```
blacklist nouveau
options nouveau modset=0
```
保存推出（`：wq`）
然后执行：`$ sudo update-initramfs –u   #更新内核`
执行 
`$ lspci | grep nouveau`，查看是否有内容，没有说明禁用成功；
重启：$sudo reboot
重启后，在登录界面，不要登录进桌面，直接ALT+CTRL+F1进命令行

2.4 安装`cuda8.0`
进入cuda_8.0.44_linux.run所在目录
```bash
$ cd /home/smith/Downloads
$ sudo chmod +x cuda_8.0.44_linux.run 
$ sudo ./cuda_8.0.44_linux.run
```

按`q`键退出RELU文档，按照如下选择，显卡驱动一定要选`n`，不装
```
Do you accept the previously read EULA?
accept/decline/quit: accept

Install NVIDIA Accelerated Graphics Driver for Linux-x86_64 361.62?
(y)es/(n)o/(q)uit: n
Install the CUDA 8.0 Toolkit?
(y)es/(n)o/(q)uit: y
Enter Toolkit Location
[ default is /usr/local/cuda-8.0 ]:
Do you want to install a symbolic link at /usr/local/cuda?
(y)es/(n)o/(q)uit: y
Install the CUDA 8.0 Samples?
(y)es/(n)o/(q)uit: y
Enter CUDA Samples Location
[ default is /home/zhou ]:
Installing the CUDA Toolkit in /usr/local/cuda-8.0 …
```
完成后看到
```
Driver: Not Selected
Toolkit: Installed in /usr/local/cuda-8.0
Samples: Installed in /home/zhou, but missing recommended libraries
```
最后，配置环境变量，直接放在系统配置文件`profile`里面:
```
$ sudo gedit /etc/profile
在最后面加入两行代码:
export PATH=/usr/local/cuda-8.0/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda-8.0/lib64:$LD_LIBRARY_PATH
```
保存退出.
执行：`$ sudo ldconfig`
此时，显卡驱动没装，等待下一步显卡驱动装好后检查cuda8.0是否装好。

关于`卸载cuda`：
```bash
$ cd /usr/local/cuda-8.0/bin
$ sudo ./uninstall_cuda_8.0.pl
```
这里可能会出现链接问题
```
/usr/lib/nvidia-375/libEGL.so.1 is not a symbolic link
/usr/lib32/nvidia-375/libEGL.so.1 is not a symbolic link
/usr/local/cuda-8.0/targets/x86_64-linux/lib/libcudnn.so.5 is not a symbolic link
```
这里是软链接被拷贝为源文件了，需要重建软链接：
```
sudo ln -s /usr/lib/nvidia-375/libEGL.so.375.66 /usr/lib/nvidia-375/libEGL.so.1
sudo ln -s /usr/lib32/nvidia-375/libEGL.so.375.66 /usr/lib32/nvidia-375/libEGL.so.1
sudo ln -sf libcudnn.so.5.1.10 libcudnn.so.5
sudo ln -sf libcudnn.so.5 libcudnn.so
```

2.5 显卡驱动安装
进入显卡驱动目录
```bash
$ sudo apt-get install nvidia-367
或者(推荐用第一种)：
$ cd /home/smith/Downloads
$ sudo su
$ sudo ./NVIDIA-Linux-x86_64-367.44.run
```
一路按照提示选择安装，具体不记得了，主要有接受协议，在系统内核注册，用新路径注册，更新`X-server`，安装完成后会自动回到命令行，重启电脑。

2.6 检查之前的安装
此时在home目录下会出现文件夹`NVIDIA_CUDA-8.0_Samples`,打开终端，进入该目录:
```bash
$ sudo make -j8  #编译samples，我电脑8线程，全开编译
```
等待2分钟左右，编译完成，执行下条指令：
```bash
$ sudo ./1_Utilities/deviceQuery/deviceQuery
```
出现如下信息，cuda8.0安装成功（忘记截图了，下面信息是gtx670装cuda6.5的）
```bash
./deviceQuery Starting...  

 CUDA Device Query (Runtime API) version (CUDART static linking)

Detected 1 CUDA Capable device(s)

Device 0: "GeForce GTX 1080 Ti"
  CUDA Driver Version / Runtime Version          8.0 / 8.0
  CUDA Capability Major/Minor version number:    6.1
  Total amount of global memory:                 11164 MBytes (11706630144 bytes)
  (28) Multiprocessors, (128) CUDA Cores/MP:     3584 CUDA Cores
  GPU Max Clock rate:                            1582 MHz (1.58 GHz)
  Memory Clock rate:                             5505 Mhz
  Memory Bus Width:                              352-bit
  L2 Cache Size:                                 2883584 bytes
  Maximum Texture Dimension Size (x,y,z)         1D=(131072), 2D=(131072, 65536), 3D=(16384, 16384, 16384)
  Maximum Layered 1D Texture Size, (num) layers  1D=(32768), 2048 layers
  Maximum Layered 2D Texture Size, (num) layers  2D=(32768, 32768), 2048 layers
  Total amount of constant memory:               65536 bytes
  Total amount of shared memory per block:       49152 bytes
  Total number of registers available per block: 65536
  Warp size:                                     32
  Maximum number of threads per multiprocessor:  2048
  Maximum number of threads per block:           1024
  Max dimension size of a thread block (x,y,z): (1024, 1024, 64)
  Max dimension size of a grid size    (x,y,z): (2147483647, 65535, 65535)
  Maximum memory pitch:                          2147483647 bytes
  Texture alignment:                             512 bytes
  Concurrent copy and kernel execution:          Yes with 2 copy engine(s)
  Run time limit on kernels:                     Yes
  Integrated GPU sharing Host Memory:            No
  Support host page-locked memory mapping:       Yes
  Alignment requirement for Surfaces:            Yes
  Device has ECC support:                        Disabled
  Device supports Unified Addressing (UVA):      Yes
  Device PCI Domain ID / Bus ID / location ID:   0 / 1 / 0
  Compute Mode:
     < Default (multiple host threads can use ::cudaSetDevice() with device simultaneously) >

deviceQuery, CUDA Driver = CUDART, CUDA Driver Version = 8.0, CUDA Runtime Version = 8.0, NumDevs = 1, Device0 = GeForce GTX 1080 Ti
Result = PASS
``` 
可以看到，最后出现了PASS，安装`cuda`完成。

还可以查看`nvcc`版本
```bash
$ nvcc –version
nvcc: NVIDIA (R) Cuda compiler driver
Copyright (c) 2005-2016 NVIDIA Corporation
Built on Sun_Sep__4_22:14:01_CDT_2016
Cuda compilation tools, release 8.0, V8.0.44
```

显示显卡信息:
```
$ nvidia–smi
```
显示信息如下：
```bash
Wed Sep  6 15:02:50 2017       
+-----------------------------------------------------------------------------+
| NVIDIA-SMI 375.66                 Driver Version: 375.66                    |
|-------------------------------+----------------------+----------------------+
| GPU  Name        Persistence-M| Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp  Perf  Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |
|===============================+======================+======================|
|   0  GeForce GTX 108...  Off  | 0000:01:00.0      On |                  N/A |
| 61%   74C    P2   207W / 250W |  10064MiB / 11164MiB |     48%      Default |
+-------------------------------+----------------------+----------------------+
                                                                               
+-----------------------------------------------------------------------------+
| Processes:                                                       GPU Memory |
|  GPU       PID  Type  Process name                               Usage      |
|=============================================================================|
|    0      1251    G   /usr/lib/xorg/Xorg                             218MiB |
+-----------------------------------------------------------------------------+
```

2.7 Atlas安装
 ATLAS是做线性代数运算的，还有俩可以选：一个是Intel 的 `MKL`，这个要收费，还有一个是`OpenBLAS`，这个比较麻烦；但是运行效率`ATLAS < OpenBLAS < MKL`  
```bash
$ sudo apt-get install libatlas-base-dev
```
实际上这步在之前安装依赖项时已经安装过了。

2.8 `cuDNN`安装
```bash
$ tar -zxvf cudnn-8.0-linux-x64-v5.1.tgz  
$ cd cuda 
$ sudo cp lib64/lib* /usr/local/cuda/lib64/  
$ sudo cp include/cudnn.h /usr/local/cuda/include/
```
更新软连接：
```bash
$ cd /usr/local/cuda/lib64/
$ sudo chmod +r libcudnn.so.5.1.5 
$ sudo ln -sf libcudnn.so.5.1.5 libcudnn.so.5
$ sudo ln -sf libcudnn.so.5 libcudnn.so
```
更新设置：
```bash
$ sudo ldconfig
```

## 3. 安装`Caffe`需要的`Python`包     
  网上介绍用现有的`anaconda`，我反正不建议，因为路径设置麻烦，很容易出错，而且自己安装很简单也挺快的。   

5.1 首先需要安装`pip`   
```
sudo apt-get install python-pip
```
5.2 再下载`caffe`   
```
git clone https://github.com/BVLC/caffe.git
```
5.3 转到`caffe/python`目录，安装`scipy`   
```
cd caffe/python
sudo apt-get install python-numpy python-scipy python-matplotlib ipython ipython-notebook python-pandas python-sympy python-nose
```
最后安装`requirement`里面的包(注意这里需要`root`权限)    
```
sudo su
for req in $(cat requirements.txt); do pip install $req; done
```
如果提示报错，一般是缺少必须的包引起的，直接根据提示 `pip install <package-name>`就行了。   
安装完后退出`root`权限！！！   

## 6. 编译`caffe`  
6.1 首先修改配置文件，回到caffe目录   
```
cd ~/caffe
cp Makefile.config.example Makefile.config
# 修改配置文件
gedit Makefile.config
# 这里仅需修改4处：
# 1) 使用cuDNN:这里去掉#，取消注释为
USE_CUDNN := 1 
# 2) 修改python包目录，这句话
PYTHON_INCLUDE := /usr/include/python2.7 \
　　/usr/lib/python2.7/dist-packages/numpy/core/include
# 改为
PYTHON_INCLUDE := /usr/include/python2.7 \
　　/usr/local/lib/python2.7/dist-packages/numpy/core/include
3) 开启GPU
4) USE_LMDB := 1；
```
  因为新安装的`python`包目录在这里： `/usr/local/lib/python2.7/dist-packages/`。   

6.2 配置运行环境，调用`CUDA`库，在`/etc/ld.so.conf.d目录`新建`caffe.conf`         
```bash
$ sudo gedit /etc/ld.so.conf.d/caffe.conf
```
添加：`/usr/local/cuda/lib64`
保存退出，执行:
```bash
$ sudo ldconfig
```

  接下来就好办了，直接`make`   
```
  make all -j4
  make test
  make runtest
  make pycaffe
```
这时候cd 到caffe 下的 python 目录，试试`caffe`的 `python wrapper`安装好没有：   
```
python
>>> import caffe
```
如果不报错，那就说明安装好了。   


## 安装中出现的问题汇总

1. 运行make之后出现如下错误：
```
/usr/include/boost/property_tree/detail/json_parser_read.hpp:257:264: error: ‘type name’ declared as function returning an array 
escape 
^ 
/usr/include/boost/property_tree/detail/json_parser_read.hpp:257:264: error: ‘type name’ declared as function returning an array 
make: * [.build_release/cuda/src/caffe/layers/detection_output_layer.o] Error 1 
make: * Waiting for unfinished jobs….
```
办法： 
修改`json_parser_read.hpp`：打开文件夹`Document`，选中`computer`，在搜索json_parser_read.hpp，找到该文件的路径之后用如下命令打开

```
sudo gedit /usr/include/boost/property_tree/detail/json_parser_read.hpp
```
将257行开始的`escape`代码段注释掉即可，如下：
```
/*escape
=   chset_p(detail::widen<Ch>("\"\\/bfnrt").c_str())
      [typename Context::a_escape(self.c)]
|   'u' >> uint_parser<unsigned long, 16, 4, 4>()
      [typename Context::a_unicode(self.c)]
;*/

```

2. 在`make py`的时候，遇到了这个错误(ubuntu 16.04 中的问题，在ubuntu14.04中应该不会遇到):
```
fatal error: hdf5.h: 没有那个文件或目录
```
这说明连接器找不到`hdf5_hl`和`hdf5`这两个库，没法进行链接。 
我的解决方案是:
Step 1   
在Makefile.config文件的第85行，添加/usr/include/hdf5/serial/ 到 INCLUDE_DIRS，也就是把下面第一行代码改为第二行代码。   
```
INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include
INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include /usr/include/hdf5/serial/
```
Step 2   
在Makefile文件的第173行，把 hdf5_hl 和hdf5修改为hdf5_serial_hl 和 hdf5_serial，也就是把下面第一行代码改为第二行代码。   
```
LIBRARIES += glog gflags protobuf boost_system boost_filesystem m hdf5_hl hdf5
LIBRARIES += glog gflags protobuf boost_system boost_filesystem m hdf5_serial_hl hdf5_serial
```

3. 训练过程中遇到:`Check failed: error == cudaSuccess (10 vs. 0)  invalid device ordinal`
 是因为GPU个数的原因，枚举设备时候出错。在`ssd_pascal_xxx.py`330行附近，将`gpus = "0,1,2,3,4"`改为`gpus = "0"`;
```
# Solver parameters.
# Defining which GPUs to use.
gpus = "0"
gpulist = gpus.split(",")
num_gpus = len(gpulist)
```

4. 缺少库文件 
编译`cuda-examples`时，
`/usr/bin/ld: cannot find -lglut`

```bash
sudo apt-get install freeglut3 freeglut3-dev
```

5. 运行`python`脚本的时候，import caffe出错
添加:
```bash
import sys
sys.path.append(“/home/smith/caffe/python”)

or:
$ export PYTHONPATH=$PYTHONPATH:/home/smith/caffe/python
```

6. make runtest -j4编译时GT540M的CUDA Capability是2.1,而官方的cudnn加速是不支持3.0以下的版本的Check failed: status == CUDNN_STATUS_SUCCESS (6 vs. 0)
caffe make runtest error（core dumped）Check failed: status == CUDNN_STATUS_SUCCESS (6 vs. 0)

=================================================================

简单讲就是GPU的加速性能不够，CUDNN只支持CUDA Capability 3.0以上的GPU加速

==================================================================

在实验室台式机上(I7+GeForce GTX TITAN Black With CUDA CAPABILITY 3.5)成功配置caffe并测试数据无误、训练数据也开始以后，也打算笔记本上(I5+GEFORCE GT540M--With CUDA CAPABILITY 2.1)配上caffe.

于是安装了`cuda`和`cudnn`加速，make和make test都过了，而make runtest时报错，大概是这样滴错误

```
...
[----------] 6 tests from CuDNNConvolutionLayerTest/1, where TypeParam = double
[ RUN      ] CuDNNConvolutionLayerTest/1.TestSimpleConvolutionGroupCuDNN
F1014 08:55:30.083176 23568 cudnn_conv_layer.cpp:30] Check failed: status == CUDNN_STATUS_SUCCESS (6 vs. 0)  CUDNN_STATUS_ARCH_MISMATCH
*** Check failure stack trace: ***
    @     0x2b082d0a8daa  (unknown)
    @     0x2b082d0a8ce4  (unknown)
    @     0x2b082d0a86e6  (unknown)
    @     0x2b082d0ab687  (unknown)
    @           0x739689  caffe::CuDNNConvolutionLayer<>::LayerSetUp()
    @           0x42c1f0  caffe::Layer<>::SetUp()
    @           0x49d776  caffe::CuDNNConvolutionLayerTest_TestSimpleConvolutionGroupCuDNN_Test<>::TestBody()
    @           0x68d613  testing::internal::HandleExceptionsInMethodIfSupported<>()
    @           0x6840b7  testing::Test::Run()
    @           0x68415e  testing::TestInfo::Run()
    @           0x684265  testing::TestCase::Run()
    @           0x6875a8  testing::internal::UnitTestImpl::RunAllTests()
    @           0x687837  testing::UnitTest::Run()
    @           0x41e9d0  main
    @     0x2b0831cf1ec5  (unknown)
    @           0x4261c7  (unknown)
    @              (nil)  (unknown)
make: *** [runtest] Aborted (core dumped) caffe::NetTest_TestReshape_Test<>::TestBody()
    @           0x68d613  testing::internal::HandleExceptionsInMethodIfSupported<>()
    @           0x6840b7  testing::Test::Run()
    @           0x68415e  testing::TestInfo::Run()
    @           0x684265  testing::TestCase::Run()
    @           0x6875a8  testing::internal::UnitTestImpl::RunAllTests()
    @           0x687837  testing::UnitTest::Run()
    @           0x41e9d0  main
    @     0x2b2a9f778ec5  (unknown)
    @           0x4261c7  (unknown)
    @              (nil)  (unknown)
make: *** [runtest] Aborted (core dumped)
```
在内网找了半天无果，终于在墙外找到了解决办法，不，是问题所在。 因为笔记本上的GT540M的CUDA Capability是2.1,而官方的cudnn加速是不支持3.0以下的版本的，因此只能在Makefile.config中注释掉USE_CUDNN这行，重新执行以下
```
make clean
make all -j4
make test -j4
make runtest -j4
```
最后除了make runtest中2 DISABLED TESTS之外，没有其他问题。make runtest中出现几个测试例子不过不影响使用