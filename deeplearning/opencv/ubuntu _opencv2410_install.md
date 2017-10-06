# ubuntu 14.04 安装 OpenCV -2.4.13
1. 先下载OpenCV的源码   http://opencv.org/downloads.html
2. 解压到任意目录
    unzip opencv-2.4.10.zip
3.进入源码目录
    cd opencv-2.4.10
4. 事先安装一些软件
    sudo apt-get install build-essential cmake libgtk2.0-dev pkg-config python-dev python-numpy libavcodec-dev libavformat-dev libswscale-dev  

5.  进入cmake
    cd cmake
6. cmake编译生成Makefile，
    cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..  
    安装所有的lib文件都会被安装到/usr/local目录

7. 编译,并安装
    sudo make install 
make 
sudo make install 

以下是非必要操作(可忽略)：
```bash
sudo sh -c 'echo"/usr/local/lib">/etc/ld.so.conf.d/opencv.conf' 
sudo ldconfig 
PS： 出错，但是没有问题
```

8. 测试，在某个目录下建立一个test.cpp文件

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

9. 同目录,新建一个文件`CMakeLists.txt`,写入如下内容
```make
project(test)  
find_package(OpenCV REQUIRED)  
add_executable(test test)  
target_link_libraries(test ${OpenCV_LIBS})  
cmake_minimum_required(VERSION 2.8)
```
10. 编译成可执行文件

cmake .
make

11.  随便弄个jpg图片做个测试，注意要和上面那个可执行文件放在同一目录下面,图片名是test.jpg。

12.    ./test

    如果能看到照片，那就表示成功了。
