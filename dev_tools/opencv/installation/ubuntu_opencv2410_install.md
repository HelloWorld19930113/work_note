# ubuntu 14.04 安装 OpenCV-2.4.10
>声明：如果安装中出现问题，可以去本文最后的问题模块中寻找解决方案。
1. 先下载OpenCV的[源码](http://opencv.org/downloads.html)    
2. 解压到任意目录     
```
unzip opencv-2.4.10.zip
```
3. 进入源码目录   
```
cd opencv-2.4.10
```
4. 安装一些依赖软件    
```
sudo apt-get install build-essential cmake libgtk2.0-dev pkg-config python-dev python-numpy libavcodec-dev libavformat-dev libswscale-dev  
```
5. 开始编译    
```
cd cmake
mkdir release
cd release
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D CUDA_GENERATION=Kepler ../../  
```
安装所有的`lib`文件都会被安装到`/usr/local`目录。   
**注意**:   
这里加上`-D CUDA_GENERATION=Kepler`或者`-D CUDA_GENERATION=auto`是因为我的显卡计算能力较弱，会报如下错误：   
```
[  2%] Building NVCC (Device) object modules/core/CMakeFiles/cuda_compile.dir/__/dynamicuda/src/cuda/./cuda_compile_generated_matrix_operations.cu.o
nvcc fatal   : Unsupported gpu architecture 'compute_11'
CMake Error at cuda_compile_generated_matrix_operations.cu.o.cmake:208 (message):
  Error generating
  /home/klm/Downloads/opencv-2.4.10/cmake/release/modules/core/CMakeFiles/cuda_compile.dir/__/dynamicuda/src/cuda/./cuda_compile_generated_matrix_operations.cu.o


make[2]: *** [modules/core/CMakeFiles/cuda_compile.dir/__/dynamicuda/src/cuda/./cuda_compile_generated_matrix_operations.cu.o] Error 1
make[1]: *** [modules/core/CMakeFiles/opencv_core.dir/all] Error 2
make: *** [all] Error 2

```
6. 编译,并安装  
如果计算机中安装过`cuda8`，那么在此之前需要修改`opencv-2.4.10/modules/gpu/src/graphcuts.cpp`：     
```
#if !defined (HAVE_CUDA) || defined (CUDA_DISABLER)   
# 改为  
#if !defined (HAVE_CUDA) || defined (CUDA_DISABLER) || (CUDART_VERSION >= 8000) 
```
然后再开始编译，如果未安装过`cuda`，则可以忽略。    
```
sudo make install 
```
以下是非必要操作(可忽略)：    
- 1) 配置相关信息，使`OpenCV`动态库被共享，具体方法如下：   
在`sudo gedit /etc/ld.so.conf.d/opencv.conf`文件，文件内容如下：   
```
# opencv.conf
/usr/local/lib
```
然后使用动态库管理命令`ldconfig`，使`opencv`的相关链接库文件被系统共享，具体命令如下：   
```
sudo ldconfig -v |grep "opencv"
```
**PS**： 可能会出错，但是没有问题。     
- 2) 添加`OpenCV`的头文件位置，首先在`/ect/profile`文件中添加如下信息：    
```
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig
```
`pkg-config`维护`opencv`的相关配置文件，可以在`/usr/local/lib/pkgconfig`目录下看到`opencv.pc`文件，此文件主要记录`opencv`的动态库信息和头文件信息。    
使用`pkg-config`命令，可以列出`opencv的配置信息`，具体命令如下：    
```
# 切换路径：
cd /urs/local/lib/pkgconfig
# 执行如下命令：
pkg-config --libs opencv  #查看opencv相关配置信息
```
**注意**：在更改相关文件时，可能文件的权限首先，故需先更改相关的权限。   
7. 测试，在某个目录下建立一个`test.cpp`文件   
```cpp
#include <cv.h>
#include <highgui.h>

using namespace cv;

int main(int argc, char* argv[]) {
    Mat image;
    image = imread(”test.jpg“, 1);

    namedWindow("Display Image", CV_WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    waitKey(0);
    return 0;
}
```
8. 同目录,新建一个文件`CMakeLists.txt`,写入如下内容：     
```make
project(test)  
find_package(OpenCV REQUIRED)  
add_executable(test test)  
target_link_libraries(test ${OpenCV_LIBS})  
cmake_minimum_required(VERSION 2.8)
```
9. 编译成可执行文件     
```
cmake .
make
```
10. 找一张`jpg`图片做个测试，注意要和上面那个可执行文件放在同一目录下面,图片名是`test.jpg`。   
```
./test
```
如果能看到照片，那就表示成功了。     



## 问题总结
1. 编译`opencv2.4.10`时出现错误：`error: ‘NppiGraphcutState’ has not been declared`    
安装cuda之后再安装opencv时出现错误。因为cuda8.0较新，要编译通过需要修改源码：        
修改`opencv-2.4.10/modules/gpu/src/graphcuts.cpp`：   
```
#if !defined (HAVE_CUDA) || defined (CUDA_DISABLER)   
# 改为  
#if !defined (HAVE_CUDA) || defined (CUDA_DISABLER) || (CUDART_VERSION >= 8000) 
```
重新编译即可。   

2.error:`/usr/bin/ld: cannot find -lcufft -lnpps -lnppi -lnppc -lcudart`      
```
sudo ln -s /usr/local/lib/libnppc.so /usr/local/cuda/lib64/libnppc.so
sudo ln -s /usr/local/cuda/lib64/libnppc.so /usr/local/lib/libnppc.so
sudo ln -s /usr/local/cuda/lib64/libnpps.so /usr/local/lib/libnpps.so
sudo ln -s /usr/local/cuda/lib64/libnppi.so /usr/local/lib/libnppi.so
sudo ln -s /usr/local/cuda/lib64/libcufft.so /usr/local/lib/libcufft.so
```
