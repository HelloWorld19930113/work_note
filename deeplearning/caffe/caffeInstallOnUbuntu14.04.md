Caffe在Ubuntu 14.04 64bit 下的安装


最近因为各种原因，装过不少次Caffe，安装过程很多坑，为节省新手的时间，特此总结整个安装流程。
关于Ubuntu 版本的选择，建议用14.04这个比较稳定的版本，但是千万不要用麒麟版！！！比原版体验要差很多！！！
Caffe的安装过程，基本采纳 这篇文章 然后稍作改动，跳过大坑。
Caffe + Ubuntu 14.04 64bit + CUDA 6.5 配置说明  http://www.linuxidc.com/Linux/2015-04/116444.htm

1. 安装开发依赖包

sudo apt-get install build-essential
sudo apt-get install vim cmake git
sudo apt-get install libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libboost-all-dev libhdf5-serial-dev libgflags-dev libgoogle-glog-dev liblmdb-dev protobuf-compiler
2. 安装CUDA

一般电脑都有双显卡：Intel 的集成显卡 + Nvidia 的独立显卡。要想两个显卡同时运行，需要关闭 lightdm 服务。

2.1 到 这里 下载安装包，选Linux x86 下的 Ubuntu 14.04， Local Package Installer，下载下来的文件为

　　cuda-repo-ubuntu1404-7-0-local_7.0-28_amd64.deb

2.2 在BIOS设置里选择用Intel显卡来显示或作为主要显示设备

2.3 进入Ubuntu， 按 ctrl+alt+F1 ，登入自己的账号，然后输入以下命令

sudo service lightdm stop
2.4 安装 CUDA，cd 到安装包目录，输入以下命令：

sudo dpkg -i cuda-repo-ubuntu1404-7-0-local_7.0-28_amd64.deb
sudo apt-get update
sudo apt-get install cuda 
安装完后重启电脑。

3. 安装cuDNN

3.1 到这里注册下载，貌似注册验证要花一两天的样子，嫌麻烦的可以直接到Linux公社资源站下载

资源包下载地址：

------------------------------------------分割线------------------------------------------

FTP地址：ftp://ftp1.linuxidc.com

用户名：ftp1.linuxidc.com

密码：www.linuxidc.com

在 2015年LinuxIDC.com\7月\Caffe在Ubuntu 14.04 64bit 下的安装

下载方法见 http://www.linuxidc.com/Linux/2013-10/91140.htm

------------------------------------------分割线------------------------------------------

3.2 完后到下载目录，执行以下命令安装

tar -zxvf cudnn-6.5-linux-x64-v2.tgz
cd cudnn-6.5-linux-x64-v2
sudo cp lib* /usr/local/cuda/lib64/
sudo cp cudnn.h /usr/local/cuda/include/
 再更新下软连接

cd /usr/local/cuda/lib64/
sudo rm -rf libcudnn.so libcudnn.so.6.5
sudo ln -s libcudnn.so.6.5.48 libcudnn.so.6.5
sudo ln -s libcudnn.so.6.5 libcudnn.so
3.3 设置环境变量

gedit /etc/profile
在打开的文件尾部加上

PATH=/usr/local/cuda/bin:$PATH
export PATH
保存后执行以下命令使之生效

source /etc/profile
同时创建以下文件

sudo vim /etc/ld.so.conf.d/cuda.conf
内容是

/usr/local/cuda/lib64
保存后，使之生效

sudo ldconfig
4. 安装CUDA Sample 及 ATLAS

4.1 Build sample

cd /usr/local/cuda/samples
sudo make all -j8
我电脑是八核的，所以make 时候用-j8参数，大家根据情况更改，整个过程有点长，十分钟左右。

4.2 查看驱动是否安装成功

cd bin/x86_64/linux/release
./deviceQuery
出现以下信息则成功

./deviceQuery Starting...

 CUDA Device Query (Runtime API) version (CUDART static linking)

Detected 1 CUDA Capable device(s)

Device 0: "GeForce GTX 670"
  CUDA Driver Version / Runtime Version          6.5 / 6.5
  CUDA Capability Major/Minor version number:    3.0
  Total amount of global memory:                 4095 MBytes (4294246400 bytes)
  ( 7) Multiprocessors, (192) CUDA Cores/MP:     1344 CUDA Cores
  GPU Clock rate:                                1098 MHz (1.10 GHz)
  Memory Clock rate:                             3105 Mhz
  Memory Bus Width:                              256-bit
  L2 Cache Size:                                 524288 bytes
  Maximum Texture Dimension Size (x,y,z)         1D=(65536), 2D=(65536, 65536), 3D=(4096, 4096, 4096)
  Maximum Layered 1D Texture Size, (num) layers  1D=(16384), 2048 layers
  Maximum Layered 2D Texture Size, (num) layers  2D=(16384, 16384), 2048 layers
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
  Concurrent copy and kernel execution:          Yes with 1 copy engine(s)
  Run time limit on kernels:                     Yes
  Integrated GPU sharing Host Memory:            No
  Support host page-locked memory mapping:       Yes
  Alignment requirement for Surfaces:            Yes
  Device has ECC support:                        Disabled
  Device supports Unified Addressing (UVA):      Yes
  Device PCI Bus ID / PCI location ID:           1 / 0
  Compute Mode:
     < Default (multiple host threads can use ::cudaSetDevice() with device simultaneously) >

deviceQuery, CUDA Driver = CUDART, CUDA Driver Version = 6.5, CUDA Runtime Version = 6.5, NumDevs = 1, Device0 = GeForce GTX 670
Result = PASS
4.3 安装ATLAS

ATLAS是做线性代数运算的，还有俩可以选：一个是Intel 的 MKL，这个要收费，还有一个是OpenBLAS，这个比较麻烦；但是运行效率ATLAS < OpenBLAS < MKL

我就用ATLAS咯：

sudo apt-get install libatlas-base-dev 
5. 安装Caffe需要的Python包

网上介绍用现有的anaconda，我反正不建议，因为路径设置麻烦，很容易出错，而且自己安装很简单也挺快的。

首先需要安装pip

sudo apt-get install python-pip
再下载caffe，我把caffe放在用户目录下

cd
git clone https://github.com/BVLC/caffe.git
再转到caffe的python目录，安装scipy

cd caffe/python
sudo apt-get install python-numpy python-scipy python-matplotlib ipython ipython-notebook python-pandas python-sympy python-nose
最后安装requirement里面的包，需要root权限

sudo su
for req in $(cat requirements.txt); do pip install $req; done
如果提示报错，一般是缺少必须的包引起的，直接根据提示 pip install <package-name>就行了。

安装完后退出root权限

exit 
6. 编译caffe

首先修改配置文件，回到caffe目录

cd ~/caffe
cp Makefile.config.example Makefile.config
gedit Makefile.config
这里仅需修改两处：

i) 使用cuDNN

# USE_CUDNN := 1 
这里去掉#，取消注释为

USE_CUDNN := 1 
ii) 修改python包目录，这句话

PYTHON_INCLUDE := /usr/include/python2.7 \
　　/usr/lib/python2.7/dist-packages/numpy/core/include
改为

PYTHON_INCLUDE := /usr/include/python2.7 \
　　/usr/local/lib/python2.7/dist-packages/numpy/core/include
因为新安装的python包目录在这里： /usr/local/lib/python2.7/dist-packages/

接下来就好办了，直接`make`
```
  make all -j4
  make test
  make runtest
  make pycaffe
```
这时候cd 到caffe 下的 python 目录，试试caffe 的 python wrapper安装好没有：
```
python
>>> import caffe
```
如果不报错，那就说明安装好了。

## 问题 ubuntu 16.04 中会遇到的问题
1. fatal error: hdf5.h: 没有那个文件或目录”解决方法
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