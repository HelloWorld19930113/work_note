# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ldd/work/gitwork/c_blind_clear/cpp/opencv

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ldd/work/gitwork/c_blind_clear/cpp/opencv

# Include any dependencies generated for this target.
include CMakeFiles/testOpencv.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/testOpencv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testOpencv.dir/flags.make

CMakeFiles/testOpencv.dir/testOpencv.cpp.o: CMakeFiles/testOpencv.dir/flags.make
CMakeFiles/testOpencv.dir/testOpencv.cpp.o: testOpencv.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ldd/work/gitwork/c_blind_clear/cpp/opencv/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testOpencv.dir/testOpencv.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testOpencv.dir/testOpencv.cpp.o -c /home/ldd/work/gitwork/c_blind_clear/cpp/opencv/testOpencv.cpp

CMakeFiles/testOpencv.dir/testOpencv.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testOpencv.dir/testOpencv.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ldd/work/gitwork/c_blind_clear/cpp/opencv/testOpencv.cpp > CMakeFiles/testOpencv.dir/testOpencv.cpp.i

CMakeFiles/testOpencv.dir/testOpencv.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testOpencv.dir/testOpencv.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ldd/work/gitwork/c_blind_clear/cpp/opencv/testOpencv.cpp -o CMakeFiles/testOpencv.dir/testOpencv.cpp.s

CMakeFiles/testOpencv.dir/testOpencv.cpp.o.requires:

.PHONY : CMakeFiles/testOpencv.dir/testOpencv.cpp.o.requires

CMakeFiles/testOpencv.dir/testOpencv.cpp.o.provides: CMakeFiles/testOpencv.dir/testOpencv.cpp.o.requires
	$(MAKE) -f CMakeFiles/testOpencv.dir/build.make CMakeFiles/testOpencv.dir/testOpencv.cpp.o.provides.build
.PHONY : CMakeFiles/testOpencv.dir/testOpencv.cpp.o.provides

CMakeFiles/testOpencv.dir/testOpencv.cpp.o.provides.build: CMakeFiles/testOpencv.dir/testOpencv.cpp.o


# Object files for target testOpencv
testOpencv_OBJECTS = \
"CMakeFiles/testOpencv.dir/testOpencv.cpp.o"

# External object files for target testOpencv
testOpencv_EXTERNAL_OBJECTS =

testOpencv: CMakeFiles/testOpencv.dir/testOpencv.cpp.o
testOpencv: CMakeFiles/testOpencv.dir/build.make
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_ts.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_ocl.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_gpu.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_contrib.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_legacy.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_video.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.2.4.8
testOpencv: /usr/lib/x86_64-linux-gnu/libopencv_core.so.2.4.8
testOpencv: CMakeFiles/testOpencv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ldd/work/gitwork/c_blind_clear/cpp/opencv/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable testOpencv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testOpencv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testOpencv.dir/build: testOpencv

.PHONY : CMakeFiles/testOpencv.dir/build

CMakeFiles/testOpencv.dir/requires: CMakeFiles/testOpencv.dir/testOpencv.cpp.o.requires

.PHONY : CMakeFiles/testOpencv.dir/requires

CMakeFiles/testOpencv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testOpencv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testOpencv.dir/clean

CMakeFiles/testOpencv.dir/depend:
	cd /home/ldd/work/gitwork/c_blind_clear/cpp/opencv && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ldd/work/gitwork/c_blind_clear/cpp/opencv /home/ldd/work/gitwork/c_blind_clear/cpp/opencv /home/ldd/work/gitwork/c_blind_clear/cpp/opencv /home/ldd/work/gitwork/c_blind_clear/cpp/opencv /home/ldd/work/gitwork/c_blind_clear/cpp/opencv/CMakeFiles/testOpencv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testOpencv.dir/depend

