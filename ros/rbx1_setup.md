# ROS rbx1

使用ros系统，需要用到许多数据包，有些时候你需要使用的ROS数据包并没有Debian包的形式，这时你需要从数据源安装它。代码开发人员通常使用的有三种主流的版本控制系统：SVN，Git和Mercurial。下面介绍一下如何使用git来下载你需要的代码资源。 
首先创建并编译好你需要使用的工作空间，然后执行下面的操作： 

```bash
$ cd ~/catkin_ws/src //此处为你自己创建的工作空间 
$ git clone https://github.com/pirobot/rbx1.git //此处为你需要代码的地址 
$ cd rbx1 //根据你下载生成的文件夹来确定 
$ git checkout indigo-devel //根据你安装的ros版本来确定，我使用的是indigo版本 
$ cd ~/catkin_ws 
$ catkin_make 
$ source ~/catkin_ws/devel/setup.bash 
```

rbx1会用到以下这些功能包，因此需要安装：

```bash
sudo apt-get install ros-kinetic-turtlebot-bringup ros-kinetic-turtlebot-create-desktop ros-kinetic-openni-* ros-kinetic-openni2-* ros-kinetic-freenect-* ros-kinetic-usb-cam ros-kinetic-laser-* ros-kinetic-hokuyo-node ros-kinetic-audio-common gstreamer0.10-pocketsphinx ros-kinetic-pocketsphinx ros-kinetic-slam-gmapping ros-kinetic-joystick-drivers python-rosinstall ros-kinetic-orocos-kdl ros-kinetic-python-orocos-kdl python-setuptools ros-kinetic-dynamixel-motor-* libopencv-dev python-opencv ros-kinetic-vision-opencv ros-kinetic-depthimage-to-laserscan ros-kinetic-arbotix-* ros-kinetic-turtlebot-teleop ros-kinetic-move-base ros-kinetic-map-server ros-kinetic-fake-localization ros-kinetic-amcl git subversion mercurial
```