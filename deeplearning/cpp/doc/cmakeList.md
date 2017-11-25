# `CMakeList.txt`文件编写









## 一个`CMakeList.txt`实例
```cmake
# 这是一个测试函数的程序的CMakeLists.txt，"#"后面为注释的内容，CMake的命令全部为大写

# cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 2.8)

# 指定生成的工程名为 ”cmake_test“
PROJECT(cmake_test)  

# 指定头文件目录为 “include”

INCLUDE_DIRECTORIES(  
include  
)  

# 指定源文件目录为 "src"，并将其赋值给环境变量 "DIR_SRCS"
AUX_SOURCE_DIRECTORY(src DIR_SRCS)  

# 设定环境变量 "TEST_CMAKE" 的值为环境变量 "DIR_SRCS" 的值
# 此处并没有什么特别的作用，只是用于显示如何用环境变量对环境变量进行赋值
SET(TEST_CMAKE 
${DIR_SRCS}  
) 

# 将外部链接库赋值给环境变量 "LIBRARIES" ，(当然也可以不用这个环境变量，而在后面直接使用该库名)
SET(LIBRARIES  
#libm.so  
)  

# 指定生成文件，将环境变量 "TEST_CMAKE" 目录下的所有文件编译生成 "bin" 目录下的可执行文件 "test" 
# 但是 "bin" 目录应该是存在的
ADD_EXECUTABLE(bin/test ${TEST_CMAKE})  

# 指定 “bin/test" 执行时的链接库为环境变量 "LIBRARIES" 的值 "libm.so"
TARGET_LINK_LIBRARIES(bin/test ${LIBRARIES})  
```
以下是不带注释的`CMakeList.txt`文件： 
```cmake
PROJECT(cmake_test)  

INCLUDE_DIRECTORIES(  
include  
)  

AUX_SOURCE_DIRECTORY(src DIR_SRCS)  

SET(TEST_CMAKE 
${DIR_SRCS}  
)  

SET(LIBRARIES  
#libm.so  
)  

#add executable file  
ADD_EXECUTABLE(bin/test ${TEST_CMAKE})  

#add link library  
TARGET_LINK_LIBRARIES(bin/test ${LIBRARIES})  
```    


## `cmake c++11`          
`CMakeLists.txt`中添加      
```
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
```