# PX4 的 ROS 仿真环境搭建
 
## Gazebo仿真
这是另外一种 软件仿真 ，是一个自主机器人3D仿真环境。它能够作为一个完整的机器人仿真套件或脱机用于机器人。其中Plugin是自行编译的，过程如下：
安装过程 如下:
初学PX4之环境搭建

# => 安装Gazebo 6仿真器(ubuntu 15.10)
~$wget -O /tmp/gazebo6_install.sh http://osrf-distributions.s3.amazonaws.com/gazebo/gazebo6_install.sh;sudo sh /tmp/gazebo6_install.sh
# => 运行
~$gazebo
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
下面可以按照 教程 进入仿真。
~$cd~/src/Firmware
~$make posix_sitl_default gazebo
教程到这里，基本的环境算是搭建好了，接下来应该就是各个模块的学习了。
