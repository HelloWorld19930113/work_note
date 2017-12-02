# `ROS(indigo)`安装`gazebo`

>声明：本文以`gazebo7`为例。

## 安装步骤  
1. 卸载系统预装的`Gazebo`   
`Gazebo7`支持更多新的功能，如果使用下面命令安装`ROS（indigo）`：    
```bash
$ sudo apt-get install ros-indigo-desktop-full  
```
那么配套安装的是`Gazebo2`，如何在`ROS（indigo）`中使用更新版本的`Gazebo`呢？    
首先，需要卸载`ros-indigo-desktop-full`等，如下：    
```bash
$ sudo apt-get remove ros-indigo-desktop-full   
```
2. 添加`ros`下载源    
```bash
sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'  
```
可以用：`cat /etc/apt/sources.list.d/gazebo-stable.list`查看是否添加成功，如下：    
```bash
deb http://packages.osrfoundation.org/gazebo/ubuntu-stable xenial main
```
添加秘钥：    
```bash
$ wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -  

--2016-11-24 12:39:26--  http://packages.osrfoundation.org/gazebo.key  
Resolving packages.osrfoundation.org (packages.osrfoundation.org)... 54.193.183.180  
Connecting to packages.osrfoundation.org (packages.osrfoundation.org)|54.193.183.180|:80... [sudo] password for relaybot: connected.  
HTTP request sent, awaiting response... 200 OK  
Length: 1772 (1.7K) [application/pgp-keys]  
Saving to: ‘STDOUT’  
  
100%[======================================>] 1,772       --.-K/s   in 0s        
  
2016-11-24 12:39:26 (7.08 MB/s) - written to stdout [1772/1772]  
  
OK  
```
完成后，更新软件源：      
```bash
$ sudo apt-get update  
```
这时，在终端输入`gazebo`和`libgazebo`，并用`tab键`查看：    
```
$ sudo apt-get install gazebo7
```
3. 安装`gazebo7`       
如果需要开发`gazebo`，需安装`libgazebo*-dev`，如果只是使用则只需安装`gazebo7`，如下：   
```bash
$ sudo apt-get install libgazebo7-dev  
$ sudo apt-get install gazebo7  
```
安装完毕后，在终端输入`gazebo`，查看：     
```bash
$ gazebo  
```
4. 安装`ROS`
到这里只是安装了`Gazebo`的新版，需要重新安装`ROS`。对应内核分别为`4.2`和`4.4`，其他版本需要编译安装`不支持直接deb软件源`安装，
4.1 使用下面命令添加软件下载源    
```bash
$ sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
```
4.2 设置`keys`    
```bash
$ sudo apt-key adv --keyserver hkp://ha.pool.sks-keyservers.NET:80 --recv-key 0xB01FA116

Executing: /tmp/tmp.vZcRo5lOC8/gpg.1.sh --keyserver
hkp://ha.pool.sks-keyservers.net:80
--recv-key
0xB01FA116
```
4.3 安装`ros`   
```bash
$ sudo apt-get update
$ sudo apt-get install ros-indigo-desktop
```
注意不要安装`full`版，`Gazebo2`不能与更新版本的Gazebo`共存`，需要单独安装所需功能包。    
然后，安装`ros-gazebo`接口库等，以`gazebo7`为例。如下：    
```
$ sudo apt-get install ros-indigo-gazebo7-  
ros-indigo-gazebo7-msgs         ros-indigo-gazebo7-ros-control  
ros-indigo-gazebo7-plugins      ros-indigo-gazebo7-ros-pkgs  
ros-indigo-gazebo7-ros  
```
安装完毕后，`ROS（indigo）`就可以和`Gazebo7`一起使用了。     
安装功能包：    
```bash
$ sudo apt-get install ros-indigo-PACKAGE
```
例如：    
```bash
$ sudo apt-get install ros-indigo-slam-gmapping
```
查找在`kinetic`中可以使用的功能包：    
```bash
$ apt-cache search ros-indigo$ apt-cache search ros-indigo
```
4.4 初始化    
```bash
$ sudo rosdep init
Wrote /etc/ros/rosdep/sources.list.d/20-default.list
Recommended: please run
$ rosdep update
reading in sources list data from /etc/ros/rosdep/sources.list.d
Hit https://raw.githubusercontent.com/ros/rosdistro/master/rosdep/osx-homebrew.yaml
Hit https://raw.githubusercontent.com/ros/rosdistro/master/rosdep/base.yaml
Hit https://raw.githubusercontent.com/ros/rosdistro/master/rosdep/Python.yaml
Hit https://raw.githubusercontent.com/ros/rosdistro/master/rosdep/ruby.yaml
Hit https://raw.githubusercontent.com/ros/rosdistro/master/releases/fuerte.yaml
Query rosdistro index https://raw.githubusercontent.com/ros/rosdistro/master/index.yaml
Add distro "groovy"
Add distro "hydro"
Add distro "indigo"
updated cache in /home/relaybot/.ros/rosdep/sources.cache
```
4.5 环境配置    
```bash
$ echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc
$ source ~/.bashrc
```
注意当安装`多个ROS`发行版，使用`kinetic`需要用到下面命令：     
```bash
$ source /opt/ros/indigo/setup.bash
```
4.6 安装`rosinstall`     
```bash
$ sudo apt-get install python-rosinstall
```
4.7 测试`roscore`    
```bash
$ roscore
```
有兴趣也可以测试一下小海归的例子。    
5. `PX4-Gazebo`配置    
安装`protobuf`库，它用作`Gazebo`的接口。    
```bash
$ sudo apt-get install libprotobuf-dev libprotoc-dev protobuf-compiler libeigen3-dev 
```
在仿真文件夹目录顶层创建一个`Build`文件夹     
```bash
$ cd ~/src/Firmware/Tools/sitl_gazebo
$ mkdir Build
```
添加路径    
```bash
# 设置插件的路径以便 Gazebo 能找到模型文件
export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:$HOME/src/Firmware/Tools/sitl_gazebo/Build
# 设置模型路径以便 Gazebo 能找到机型
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:$HOME/src/Firmware/Tools/sitl_gazebo/models
# 禁用在线模型查找
export GAZEBO_MODEL_DATABASE_URI=""
# 设置 sitl_gazebo 文件夹的路径
export SITL_GAZEBO_PATH=$HOME/src/Firmware/Tools/sitl_gazebo
```
切换到构建目录并从中调用`CMake`     
```bash
$ cd Build
$ cmake ..
```
构建`gazebo`插件    
```bash
$ make
```
万事俱备，只欠东风。现在就可以运行`Gazebo`仿真了，以最基本的`iris`仿真为例     
```bash
$ cd~/src/Firmware
$ make posix_sitl_default gazebo
```
教程到这里，基本的环境算是搭建好了，接下来应该就是各个模块的学习了。     
