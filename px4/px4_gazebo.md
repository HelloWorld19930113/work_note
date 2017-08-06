# PX4 的 ROS 仿真环境搭建
 
## Gazebo仿真
 Gazebo是种软件仿真 ，是一个自主机器人3D仿真环境。它能够作为一个完整的机器人仿真套件或脱机用于机器人。其中Plugin是自行编译的。

## 安装过程

- 1. 安装Gazebo 6仿真器(ubuntu 15.10)
```
$ wget -O gazebo/gazebo6_install.sh http://osrf-distributions.s3.amazonaws.com/gazebo/gazebo6_install.sh;
$ cd gazebo
$ sudo sh ./gazebo6_install.sh
```
- 2. 运行
~$ gazebo
# => 安装protobuf 库
~$sudo apt-get install libprotobuf-dev libprotoc-dev protobuf-compiler libeigen3-dev
# => 编译Gazebo插件
# => 克隆gazebo plugins repository到~/src/sitl_gazebo
~$git clone https://github.com/PX4/sitl_gazebo.git
# => 在仓库的顶层建立Build文件夹
~$mkdir Build
# => 把build目录添加到gazebo plugin path，e.g.添加如下到我的.profile 文件
# Set the plugin path so Gazebo finds our model and sim
export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:$HOME/src/sitl_gazebo/Build
# Set the model path so Gazebo finds the airframes
export GAZEBO_MODEL_PATH=${GAZEBO_MODEL_PATH}:$HOME/src/sitl_gazebo/models
# Disable online model lookup since this is quite experimental and unstable
export GAZEBO_MODEL_DATABASE_URI=""
# => 还需要添加仓库的主目录
# Set path to sitl_gazebo repository
export SITL_GAZEBO_PATH=$HOME/src/sitl_gazebo
# => 使生效
~$source~/.profile
# => 进入Build目录执行make
~$cd~/src/sitl_gazebo/Build
~$cmake ..
# => 生成sdf文件
~$make sdf
# => 构建 gazebo plugins
~$make
# => 现在可以运行gazebo了
～$gazebo












ROS(indigo) 安装和使用更新版本的Gazebo，本文以7为例。

Gazebo7支持更多新的功能，如果使用下面命令安装ROS（indigo）：

```bash
$ sudo apt-get install ros-indigo-desktop-full  
```
那么配套安装的是Gazebo2，如何在ROS（indigo）中使用更新版本的Gazebo呢？
首先，需要卸载ros-indigo-desktop-full等，如下：

```bash
$ sudo apt-get remove ros-indigo-desktop-full   
```

然后，依次输入下面命令添加`ros`下载源：

```bash
sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'  
```

可以用：cat /etc/apt/sources.list.d/gazebo-stable.list ，查看，如下：

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


完成后，更新：
```bash
$ sudo apt-get update  
```

这时，在终端输入gazebo和libgazebo，并用tab键查看：

```
$ sudo apt-get install gazebo7
```

如果需要开发gazebo，需安装libgazebo*-dev，如果只是使用，安装gazebo7，如下：

```bash
$ sudo apt-get install libgazebo7-dev  
```

```bash
$ sudo apt-get install gazebo7  
```

安装完毕后，在终端输入`gazebo`，查看：

```bash
$ gazebo  
```
## 安装ROS
 但是到这里只是安装了Gazebo的新版，需要重新安装ROS。ROS Kinetic只支持Wily(15.10)和Xenial(16.04)对应内核分别为4.2和4.4，其他版本需要编译安装不支持直接deb软件源安装，

- 1. 使用下面命令添加软件下载源：

```bash
$ sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
```

- 2. 设置keys

```bash
$ sudo apt-key adv --keyserver hkp://ha.pool.sks-keyservers.NET:80 --recv-key 0xB01FA116

Executing: /tmp/tmp.vZcRo5lOC8/gpg.1.sh --keyserver
hkp://ha.pool.sks-keyservers.net:80
--recv-key
0xB01FA116
```
- 3. 安装

```bash
$ sudo apt-get update
$ sudo apt-get install ros-kinetic-desktop
```
注意不要安装full版，Gazebo2不能与更新版本的Gazebo共存，需要单独安装所需功能包即可。
然后，安装ros-gazebo接口库等，以7为例如下：

```
$ sudo apt-get install ros-indigo-gazebo7-  
ros-indigo-gazebo7-msgs         ros-indigo-gazebo7-ros-control  
ros-indigo-gazebo7-plugins      ros-indigo-gazebo7-ros-pkgs  
ros-indigo-gazebo7-ros  
```
安装完毕后，ROS（indigo）就可以和Gazebo7一起使用了。

安装功能包：
```bash
$ sudo apt-get install ros-kinetic-PACKAGE
```

例如：
```bash
$ sudo apt-get install ros-kinetic-slam-gmapping
```
查找在kinetic中可以使用的功能包：
```bash
$ apt-cache search ros-kinetic$ apt-cache search ros-kinetic
```

- 4. 初始化
```bash
$ sudo rosdep init
Wrote /etc/ros/rosdep/sources.list.d/20-default.list
Recommended: please run
rosdep update
```


~$ rosdep update
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
Add distro "jade"
Add distro "kinetic"
updated cache in /home/relaybot/.ros/rosdep/sources.cache

- 5. 环境配置

```bash
$ echo "source /opt/ros/kinetic/setup.bash" >> ~/.bashrc
$ source ~/.bashrc
```
注意当安装多个ROS发行版，使用kinetic需要用到下面命令：

```bash
$ source /opt/ros/kinetic/setup.bash
```

- 6. 安装rosinstall

```bash
$ sudo apt-get install python-rosinstall
```

- 7. 测试roscore

```bash
$ roscore
```
有兴趣也可以测试一下小海归的例子。


下面可以按照 教程 进入仿真。
~$cd~/src/Firmware
~$ make posix_sitl_default gazebo
教程到这里，基本的环境算是搭建好了，接下来应该就是各个模块的学习了。

-End-