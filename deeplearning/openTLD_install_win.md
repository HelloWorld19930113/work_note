 Building
=========

Dependencies
------------

* OpenCV & OpenCV highgui >= 2.0
* CMake >= 2.6

Compiling
---------

Use CMake to build the project. You can use "cmake-gui", if you need a graphical user interface.

* CMake
	* Specify the source path (root path of the dictionary) and the binary path (where to build the program, out
	  of source build recommended)
	* Configure
	* Select compiler
	* Define the install path and other options
	* Generate
* Windows: go to the binary directory and build the solutions you want (You have to compile in RELEASE mode):
	* tld: build the project
	* INSTALL: install the project



# openTLD

1. error C2039: “PatchGenerator”: 不是“cv”的成员

	cv::PatchGenerator generator;
在`TLD.h`文件中添加：
#include <opencv2/legacy/legacy.hpp>