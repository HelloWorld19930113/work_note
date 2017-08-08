# ROS安装问题

## 如何下载ROS源码？

切换到一个代码下载目录，执行下面的命令：

```bash
$ rosinstall_generator robot --rosdistro indigo --deps --wet-only --tar > indigo-robot-wet.rosinstall
```

下载代码：

```bash
$ wstool init -j8 src indigo-robot-wet.rosinstall
```

如果下载过程中被中断了，可以使用以下命令断点续传：

```bash
$wstool update -j 4 -t src
```


## 2. 安装软件源的问题；

```bash
$ sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu precise main" > /etc/apt/sources.list.d/ros-latest.list'
$ wget http://packages.ros.org/ros.key -O - | sudo apt-key add -
$ sudo apt-get update
```

## 3. `catkin`找不到

```bash
$ echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc
$ source ~/.bashrc
```


## 4. 找不到`anglesConfig.cmake`和`angles-config.cmake`

```bash
_______________________________________________________________________________
Errors     << mavros:cmake /home/zack/catkin_ws/logs/mavros/build.cmake.001.log
CMake Warning at /opt/ros/indigo/share/catkin/cmake/catkinConfig.cmake:76 (find_package):
  Could not find a package configuration file provided by "angles" with any
  of the following names:

    anglesConfig.cmake
    angles-config.cmake

  Add the installation prefix of "angles" to CMAKE_PREFIX_PATH or set
  "angles_DIR" to a directory containing one of the above files.  If "angles"
  provides a separate development package or SDK, be sure it has been
  installed.
Call Stack (most recent call first):
  CMakeLists.txt:7 (find_package)
```

RE:
```bash
The cmake error you posed indicates that you don't have the angles package installed. Have you tried installing it?

sudo apt-get install ros-<hydro/indigo>-angles
More generally, if you have a workspace where the dependencies may be missing, you can usually install them with rosdep:

rosdep install --from-paths src -i
```

## 5. `cmake error`

```bash
Errors     << mavlink:install /home/zack/catkin_ws/logs/mavlink/build.install.003.log
CMake Error at pymavlink/safe_execute_install.cmake:4 (message):
  
  execute_process(/home/zack/catkin_ws/build/mavlink/pymavlink/python_distutils_install.sh)
  returned error code 1
Call Stack (most recent call first):
  pymavlink/cmake_install.cmake:36 (include)
  cmake_install.cmake:69 (include)

make: *** [install] Error 1
```

 删除build和devl文件夹之后重新编译：
 
 ```bash
 $rm -rf build
 $rm -rf devl
 $catkin build
 ```
 