#  ubuntu 14.04 x64 PX4编译环境搭建   
 
> 平台：`ubuntu 14.04 x64`     
> 声明：如果配置过程中遇到问题，可以去本文最末尾寻找解决方案，如果没有再去网上查找。 


## 警告   
不要使用“sudo”解决权限问题。它将创造更多的权限问题在这个过程中,需要系统重新安装修复它们：     
`$ sudo usermod -a -G dialout $USER`  

## 背景
官方开源的`Pixhawk`固件自`2015年10月6号`起，删除了`makefile`文件夹，按照在工具链中的控制台命令行编译，再按照原来的编译步骤去编译时就会出现问题了，因为固件已经由`make`编译系统转到了`cmake`编译系统了。    
1. `make`编译系统       
```
# 先将master分支clone到本地
git clone https://github.com/PX4/Firmware.git

# 然后再更新本地仓库，再编译
git submodule init
git submodule update

# 在工具链中的console中编译
make distclean
make archives
make px4fmu-v2_default
```

通过`git`版本控制来切换分支得到想要的版本。在`git`中，`tag`就是一个只读的`branch`，一般为每一个可发布的里程碑版本打一个`tag`。如在方式一中的`tag`，`v1.0.1、v1.0.0rc12、v1.0.0rc11`等。比如想要得到`v1.0.1`这个分支的代码，可以这样做：    
```
# 在本地的git bash中切换分支：git checkout tag_name
git checkout v1.0.1
```
2. `cmake`编译系统   
本文将详细介绍`Cmake`的编译过程。    
2.1 安装开发环境工具链    
执行以下4条命令安装`cmake`编译器、`git`远程下载、`qtcreator`库文件等和仿真工具：    
```
$ sudo add-apt-repository ppa:george-edison55/cmake-3.x-y
$ sudo apt-get update
$ sudo apt-get install python-argparse git-core wget zip python-empy qtcreator cmake build-essential genromfs -y
$ sudo apt-get install ant protobuf-compiler libeigen3-dev libopencv-dev openjdk-8-jdk openjdk-8-jre clang-3.5 lldb-3.5-y
```
`Ubuntu`附带了一个串行调制解调器，这严重干涉任何机器人相关使用`串行端口(或USB串行)`。它可以无副作用的被卸载。执行以下命令删除串口模式管理器：    
```
$ sudo apt-get remove modemmanager
```
执行以下命令更新依赖包：   
```
$ sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded -y
$ sudo apt-get update
$ sudo apt-get install python-serial openocd \
	flex bison libncurses5-dev autoconf texinfo build-essential \
	libftdi-dev libtool zlib1g-dev \
	python-empy gcc-arm-none-eabi -y
```
2.2 执行以下命令下载`PX4`源码    
```
$ mkdir -p ~/src
$ cd ~/src
$ git clone https://github.com/PX4/Firmware.git
$ cd Firmware
$ git submodule update --init --recursive
```
2.3 编译源码    
``` 
$ cd Firmware
$ make px4fmu-v2_default
```
编译成功后打印信息如下：   
```
[100%]Linking CXX executable firmware_nuttx
[100%]Built target firmware_nuttx
Scanning dependencies of target build_firmware_px4fmu-v2
[100%]Generating nuttx-px4fmu-v2-default.px4
[100%]Built target build_firmware_px4fmu-v2
```
3. 下载固件至`PIXHAWK`    
```
$ make px4fmu-v2_default upload
```
下载完成后的打印信息如下：
```
Erase:[====================]100.0%
Program:[====================]100.0%
Verify:[====================]100.0%
Rebooting.
[100%]Built target upload
```
## 可能遇到的问题    
在编译过程中一定会遇到不少问题，现将我在安装中遇到的问题及其解决方案一一列举如下：    
1. `cmake`版本无效
```
$ make px4fmu-v2-default

	Makefile:44: Not a valid CMake version or CMake not installed.
	Makefile:45: On Ubuntu 16.04, install or upgrade via:
	Makefile:46:
	Makefile:47: 3rd party PPA:
	Makefile:48: sudo add-apt-repository ppa:george-edison55/cmake-3.x -y
	Makefile:49: sudo apt-get update
	Makefile:50: sudo apt-get install cmake
	Makefile:51:
	Makefile:52: Official website:
	Makefile:53: wget https://cmake.org/files/v3.4/cmake-3.4.3-Linux-x86_64.sh
	Makefile:54: chmod +x cmake-3.4.3-Linux-x86_64.sh
	Makefile:55: sudo mkdir /opt/cmake-3.4.3
	Makefile:56: sudo ./cmake-3.4.3-Linux-x86_64.sh --prefix=/opt/cmake-3.4.3 --exclude-subdir
	Makefile:57: export PATH=/opt/cmake-3.4.3/bin:$PATH
	Makefile:58:
	Makefile:59: *** Fatal。 停止。
```
根据提示可以确定，问题是由于 cmake的版本有不匹配。根据提示安装`cmake-3.x`版本。    
```
$ sudo apt-get install cmake3
```
这是因为在`Makefile`中会调用一个`shell`脚本*`check_cmake.sh`*来检查`cmake`的版本。脚本内容如下：  
```bash
#!/bin/bash
cmake_ver=`cmake --version`

if [[ $cmake_ver == *" 2.8"* ]] || [[ $cmake_ver == *" 2.9"* ]] || [[ $cmake_ver == *" 3.0"* ]] || [[ $cmake_ver == *" 3.1"* ]]   then
  exit 1;
fi
```
根据脚本内容可知，`2.8/2.9/3.0/3.1`的版本为无效版本。    
2. 缺少 `genromfs `工具
```
$ make px4fmu-v2_default
	-- Nuttx build for px4fmu-v2 on m4 hardware, using nsh with ROMFS on px4fmu_common
	CMake Error at cmake/toolchains/Toolchain-arm-none-eabi.cmake:62 (message):
	  could not find genromfs
	
	CMake Error: Error required internal CMake variable not set, cmake may be not be built correctly.
	make: *** [px4fmu-v2_default] 错误 1
```
是由于`genromfs`安装包未安装，执行下面的命令安装`genromfs`：   
```bash
$ sudo apt-get install genromfs
```
3. 缺少 `ninja-build`工具   
```
$ make px4fmu-v2_default
	log: ninja version 0.1.3 initializing
	die: error: must run as root
	make: *** [px4fmu-v2_default] 错误 1

```
是由于`ninja`安装包未安装，这里应该安装`ninja-build`，而不是`ninja`。执行下面的命令即可：    
```
$ sudo apt-get remove --purge ninja(如果之前安装过ninja,则需要执行这条命令)
...
$ sudo apt-get install ninja-build
```
4. 缺少 `python packages`   
```
$ make px4fmu-v2_default
	python import error:  No module named em
	Required python packages not installed.

	On a Debian/Ubuntu system please run:
	  sudo apt-get install python-empy
	  sudo pip install catkin_pkg

	On MacOS please run:
	  sudo pip install empy catkin_pkg

	On Windows please run:
	  easy_install empy catkin_pkg

	ninja: build stopped: subcommand failed.
	make: *** [px4fmu-v2_default] 错误 1
```
根据提示安装缺少的`python包`：   
```
$ sudo apt-get install python-empy
$ sudo pip install catkin_pkg
```
5. `gcc 版本无效`
```
$ make px4fmu-v2_default
	In file included from ../src/include/../platforms/px4_nodehandle.h:42:0,
		         from ../src/include/px4.h:46,
		         from ../src/drivers/camera_trigger/interfaces/src/pwm.cpp:1:
	../src/include/../platforms/px4_subscriber.h:42:22: fatal error: functional: No such file or directory
	 #include <functional>
		              ^
	compilation terminated.
	[173/488] Building CXX object src/driv...Files/drivers__px4io.dir/px4io.cpp.obj
	ninja: build stopped: subcommand failed.
	make: *** [px4fmu-v2_default] 错误 1

```
由于`arm-none-eabi-gcc`版本问题，需要重新安装`arm-none-eabi`，系统是已经默认安装的是最新版`4.9`，这里需要换成`4.8`版本，去`GNU官方地址`[arm-none-eabi-gcc4.8](https://launchpad.net/gcc-arm-embedded/+download) 下载`gcc-arm-none-eabi-4_8-2014q3-20140805-linux.tar.bz2`，然后进入下载文件夹，进行如下操作：   
```
# 卸载原来的gcc-arm-none-eabi
$ sudo apt-get remove --purge gcc-arm-none-eabi

# 安装下载好的gcc-arm-none-eabi   
$ tar xjvf gcc-arm-none-eabi-4_8-2014q3-20140805-linux.tar.bz2
$ sudo mv gcc-arm-none-eabi-4_8-2014q3 /opt
$ exportline="export PATH=/opt/gcc-arm-none-eabi-4_8-2014q3/bin:$PATH"
```
如果是`ubuntu 64位`系统，`arm-none-eabi`是直接下载人家`编译好的32位`的话，还需要一个东东：
`sudo apt-get install lsb-core`。重新安装的`arm-none-eabi-gcc`版本是4.8.4，即使是4.8.2的版本在`ubuntu 14.04 64位`上也是不可用的。   
6. `编译得到的镜像文件过大`     
```
$ make px4fmu-v2_default
	/home/zack/opt/gcc-arm-none-eabi-4_8-2014q3/bin/../lib/gcc/arm-none-eabi/4.8.4/../../../../arm-none-eabi/bin/ld: src/firmware/nuttx/firmware_nuttx section `.data' will not fit in region `flash'
	/home/zack/opt/gcc-arm-none-eabi-4_8-2014q3/bin/../lib/gcc/arm-none-eabi/4.8.4/../../../../arm-none-eabi/bin/ld: region `flash' overflowed by 3576 bytes
	collect2: error: ld returned 1 exit status
	ninja: build stopped: subcommand failed.
	make: *** [px4fmu-v2_default] 错误 1
```
编译完的镜像文件过大，需要将一部分不需要的模块注释，不将其编译到镜像文件中。可参考以下文件来修改*`Firmware/cmake/configs/nuttx_px4fmu-v2_default.cmake`*文件：    
[Firmware/cmake/configs/nuttx_px4fmu-v2_default.cmake](https://github.com/PX4/Firmware/blob/master/cmake/configs/nuttx_px4fmu-v2_default.cmake)   
**注**：一般只要注释掉`modules/logger`即可。