# ROS indigo 环境搭建
 
## 安装Gazebo7

1 设置您的计算机以接受来自packages.osrfoundation.org的软件。

```bash
$ sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
```

2 添加秘钥

```bash
$ wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
```


3 安装Gazebo7

```bash
# 首先更新源
$ sudo apt-get update
# 安装Gazebo7
$ sudo apt-get install gazebo7 
# 开发者需要安装额外的包
$ sudo apt-get install libgazebo7-dev
```

现在可以尝试启动`gazebo`，直接从命令行启动

```bash
$ gazebo
```



## 安装过程
ROS indigo不支持Wily(15.10)和Xenial(16.04)对应内核分别为4.2和4.4，其他版本需要编译安装不支持直接deb软件源安装，

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
$ sudo apt-get install ros-indigo-desktop
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
$ sudo apt-get install ros-indigo-PACKAGE
```

例如：
```bash
$ sudo apt-get install ros-indigo-slam-gmapping
```

查找在indigo中可以使用的功能包：
```bash
$ apt-cache search ros-indigo$ apt-cache search ros-indigo
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
Add distro "indigo"
updated cache in /home/relaybot/.ros/rosdep/sources.cache

- 5. 环境配置

```bash
$ echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc
$ source ~/.bashrc
```
注意当安装多个ROS发行版，使用indigo需要用到下面命令：

```bash
$ source /opt/ros/indigo/setup.bash
```

- 6. 安装rosinstall

```bash
$ sudo apt-get install python-rosinstall
```

- 7. 测试roscore

```bash
$ roscore
```
-End-
