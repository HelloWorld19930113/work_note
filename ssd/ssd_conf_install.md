# ubuntu 14.04 安装 ssd 以及出现的问题

>声明： 如果没有出现问题，直接过就好。如果出现问题了，再按照以下解决方案来做。

## 第一部分，准备材料（NVIDIA官网下载）：
`显卡驱动`：NVIDIA-Linux-x86_64-367.44.run,(可以不使用这个，直接使用sudo apt-get install nvidia-367)

`Cuda8.0`:cuda_8.0.44_linux.run
下载网址：https://developer.nvidia.com/cuda-downloads

`Cudnn`：cudnn-8.0-linux-x64-v5.1.tgz
下载网址：https://developer.nvidia.com/cudnn

## 第二部分，安装步骤
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
blacklist nouveau
options nouveau modset=0
保存推出（：wq）
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

还可以查看nvcc版本
```bash
$ nvcc –version
nvcc: NVIDIA (R) Cuda compiler driver
Copyright (c) 2005-2016 NVIDIA Corporation
Built on Sun_Sep__4_22:14:01_CDT_2016
Cuda compilation tools, release 8.0, V8.0.44
```

显示显卡信息:
$ nvidia–smi
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

2.9 拉取caffe源码
```bash
$ git clone https://github.com/BVLC/caffe.git
```

2.10 安装python的pip和easy_install，方便安装软件包（超慢的下载。。。）
```
$ sudo wget --no-check-certificate https://bootstrap.pypa.io/ez_setup.py 
$ sudo python ez_setup.py --insecure
$ wget https://bootstrap.pypa.io/get-pip.py
$ sudo python get-pip.py
```

2.11 安装python依赖(路径根据自己的目录可能要调一下)
```bash
$ cd caffe/python
$ sudo su
$ for req in $(cat requirements.txt); do pip install $req; done
```
这步安装也有点慢，别急，等会儿，先去干点别的 ^_^

2.12 编辑caffe所需的Makefile文件，配置
```bash
$ cd caffe 
$ cp Makefile.config.example Makefile.config 
$ sudo gedit Makefile.config 
```
$Makefile.config里面有依赖库的路径，及各种编译配置,主要作以下修改：
```bash
取消USE_CUDNN := 1的注释；
开启GPU，USE_LMDB := 1；
```
配置运行环境，调用`CUDA`库，在`/etc/ld.so.conf.d目录`新建`caffe.conf`,
```bash
$ sudo gedit /etc/ld.so.conf.d/caffe.conf
```
添加：`/usr/local/cuda/lib64`
保存退出，执行:
```bash
$ sudo ldconfig
```

2.13 编译caffe、pycaffe
进入caffe根目录，
$sudo make -j4
  测试一下结果，
$sudo make test -j4
$sudo make runtest -j4
(runtest中个别没通过没关系，不影响使用)
$sudo make pycaffe -j4
$sudo make distribute
第三部分，拿cifar10测试下效果
$cd /home/smith/caffe
$sudo sh data/cifar10/get_cifar10.sh  （脚本下载速度太慢，找个迅雷下载拷进来，再照脚本解压）
# sudo sh examples/cifar10/create_cifar10.sh
# sudo sh examples/cifar10/train_quick.sh
下面，网络开始初始化、训练了，loss会开始下降，很快的就会出现优化完成。


PS:
1、尝试了安装opencv3.0.0，可惜失败了，有博客说是cuda8.0版本太新，不支持了，后面有时间再搞了。
2、









## 安装中可能出现的问题 

1. 运行make之后出现如下错误：
/usr/include/boost/property_tree/detail/json_parser_read.hpp:257:264: error: ‘type name’ declared as function returning an array 
escape 
^ 
/usr/include/boost/property_tree/detail/json_parser_read.hpp:257:264: error: ‘type name’ declared as function returning an array 
make: * [.build_release/cuda/src/caffe/layers/detection_output_layer.o] Error 1 
make: * Waiting for unfinished jobs….

办法： 
修改json_parser_read.hpp：打开文件夹Document，选中computer，在搜索json_parser_read.hpp，找到该文件的路径之后用如下命令打开

sudo gedit /usr/include/boost/property_tree/detail/json_parser_read.hpp
将257行开始的escape代码段注释掉即可，如下：

/*escape
                    =   chset_p(detail::widen<Ch>("\"\\/bfnrt").c_str())
                            [typename Context::a_escape(self.c)]
                    |   'u' >> uint_parser<unsigned long, 16, 4, 4>()
                            [typename Context::a_unicode(self.c)]
                    ;*/



2. 在make py的时候，遇到了这个错误:

/usr/bin/ld: cannot find -lhdf5_hl
/usr/bin/ld: cannot find -lhdf5
collect2: error: ld returned 1 exit status

这说明连接器找不到　hdf5_hl和hdf5这两个库，没法进行链接。 
我的解决方案是更改makefile:在ｍakefile中作如下更改：

#LIBRARIES += glog gflags protobuf boost_system boost_filesystem m hdf5_hl hdf5
LIBRARIES += glog gflags protobuf boost_system boost_filesystem m hdf5_serial_hl hdf5_serial

把第一行注释，然后改成第二行的内容就可以了。

3. Check failed: error == cudaSuccess (10 vs. 0)  invalid device ordinal
 在330行附近，将`gpus = "0,1,2,3,4"`改为`gpus = "0"`;

```
# Solver parameters.
# Defining which GPUs to use.
gpus = "0"
gpulist = gpus.split(",")
num_gpus = len(gpulist)
```

4. 编译cuda-examples时，/usr/bin/ld: cannot find -lglut
```bash
sudo apt-get install freeglut3 freeglut3-dev
```

5. 运行python 脚本的时候，import caffe出错
添加:
```bash
import sys
sys.path.append(“/home/smith/caffe/python”)

or:
$ export PYTHONPATH=$PYTHONPATH:/home/smith/caffe/python
```