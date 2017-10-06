OpenPose:一个实时多人关键点检测和多线程C++库

Check all the release notes.
We are offering internships at Carnegie Mellon University as OpenPose programmer (need to live in or be willing to move to Pittsburgh).

## 介绍

`OpenPose` is a library for real-time multi-person keypoint detection and multi-threading written in C++ using OpenCV and Caffe*, authored by Gines Hidalgo, Zhe Cao, Tomas Simon, Shih-En Wei, Hanbyul Joo and Yaser Sheikh.

* It uses Caffe, but the code is ready to be ported to other frameworks (Tensorflow, Torch, etc.). If you implement any of those, feel free to make a pull request!

OpenPose represents the first real-time system to jointly detect human body, hand and facial keypoints (in total 130 keypoints) on single images. In addition, the system computational performance on body keypoint estimation `is invariant to` the number of detected people in the image.

OpenPose is `freely available for free non-commercial use`, and may be redistributed under these conditions. Please, see the license for further details. Interested in a commercial license? Check this link. For commercial queries, contact Yaser Sheikh.

另外，`OpenPose`在`CMU全景工作室`之外是不能用的。

## 库中包含的主要功能:

多人的`15或18`个关键点的身体姿态估计和渲染。运行时间和图片中的人数无关。

多人的`2x21`个关键点的手部估计和渲染。注意：在这个初始版本中，运行时间和图片中的人数呈线性关系。

Multi-person 2x21-keypoint hand estimation and rendering. Note: In this initial version, running time linearly depends on the number of people on the image. Coming soon (in around 1-5 days)!

Multi-person 70-keypoint face estimation and rendering. Note: In this initial version, running time linearly depends on the number of people on the image.

Flexible and easy-to-configure multi-threading module.

Image, video, and webcam reader.

Able to save and load the results in various formats (JSON, XML, PNG, JPG, ...).

Small display and GUI for simple result visualization.

All the functionality is wrapped into a simple-to-use OpenPose Wrapper class.

The pose estimation work is based on the C++ code from the ECCV 2016 demo, "Realtime Multiperson Pose Estimation", Zhe Cao, Tomas Simon, Shih-En Wei, Yaser Sheikh. The full project repo includes Matlab and Python version, as well as training code.

Operating Systems

Ubuntu 14 and 16.
Windows 8 and 10.
OpenPose has also been used on Windows 7, Mac, CentOS, and Nvidia Jetson (TK1 and TX1) embedded systems. However, we do not officially support them at the moment.
Results

Body + Hands + Face Estimation



Body Estimation



Body + Face Estimation



Body + Hands



Contents

Installation, Reinstallation and Uninstallation
Custom Caffe
Quick Start
Demo
OpenPose Wrapper
OpenPose Library
Output
Speed Up Openpose And Benchmark
Send Us Your Feedback!
Citation
Other Contributors
Installation, Reinstallation and Uninstallation

You can find the installation, reinstallation and uninstallation steps on: doc/installation.md.

Custom Caffe

We only modified some Caffe compilation flags and minor details. You can use your own Caffe distribution, these are the files we added and modified:

Added files: install_caffe.sh; as well as Makefile.config.Ubuntu14.example, Makefile.config.Ubuntu16.example, Makefile.config.Ubuntu14_cuda_7.example and Makefile.config.Ubuntu16_cuda_7.example (extracted from Makefile.config.example). Basically, you must enable cuDNN.
Edited file: Makefile. Search for "# OpenPose: " to find the edited code. We basically added the C++11 flag to avoid issues in some old computers.
Optional - deleted Caffe file: Makefile.config.example.
In order to link it to OpenPose:
Run make all && make distribute in your Caffe version.
Open the OpenPose Makefile config file: ./Makefile.config.UbuntuX.example (where X depends on your OS and CUDA version).
Modify the Caffe folder directory variable (CAFFE_DIR) to your custom Caffe distribute folder location in the previous OpenPose Makefile config file.
Quick Start

Most users cases should not need to dive deep into the library, they might just be able to use the Demo or the simple OpenPose Wrapper. So you can most probably skip the library details in OpenPose Library.

Demo

Your case if you just want to process a folder of images or video or webcam and display or save the pose results.

Forget about the OpenPose library details and just read the doc/demo_overview.md 1-page section.

OpenPose Wrapper

Your case if you want to read a specific format of image source and/or add a specific post-processing function and/or implement your own display/saving.

(Almost) forget about the library, just take a look to the Wrapper tutorial on examples/tutorial_wrapper/.

Note: you should not need to modify the OpenPose source code nor examples. In this way, you are able to directly upgrade OpenPose anytime in the future without changing your code. You might create your custom code on examples/user_code/ and compile it by using make all in the OpenPose folder.

OpenPose Library

Your case if you want to change internal functions and/or extend its functionality. First, take a look at the Demo and OpenPose Wrapper. Second, read the 2 following subsections: OpenPose Overview and Extending Functionality.

OpenPose Overview: Learn the basics about the library source code in doc/library_overview.md.

Extending Functionality: Learn how to extend the library in doc/library_extend_functionality.md.

Adding An Extra Module: Learn how to add an extra module in doc/library_add_new_module.md.

Doxygen Documentation Autogeneration

You can generate the documentation by running the following command. The documentation will be generated in doc/doxygen/html/index.html. You can simply open it with double-click (your default browser should automatically display it).

cd doc/
doxygen doc_autogeneration.doxygen
Output

Check the output (format, keypoint index ordering, etc.) in doc/output.md.

Speed Up OpenPose and Benchmark

Check the OpenPose Benchmark and some hints to speed up OpenPose on doc/installation.md#faq.

Send Us Your Feedback!

Our library is open source for research purposes, and we want to continuously improve it! So please, let us know if...

... you find any bug (in functionality or speed).

... you added some functionality to some class or some new Worker subclass which we might potentially incorporate.

... you know how to speed up or improve any part of the library.

... you have a request about possible functionality.

... etc.

Just comment on GibHub or make a pull request and we will answer as soon as possible! Send us an email if you use the library to make a cool demo or YouTube video!

Citation

Please cite these papers in your publications if it helps your research (the face keypoint detector was trained using the same procedure described in [Simon et al. 2017]):

@inproceedings{cao2017realtime,
  author = {Zhe Cao and Tomas Simon and Shih-En Wei and Yaser Sheikh},
  booktitle = {CVPR},
  title = {Realtime Multi-Person 2D Pose Estimation using Part Affinity Fields},
  year = {2017}
  }

@inproceedings{simon2017hand,
  author = {Tomas Simon and Hanbyul Joo and Iain Matthews and Yaser Sheikh},
  booktitle = {CVPR},
  title = {Hand Keypoint Detection in Single Images using Multiview Bootstrapping},
  year = {2017}
  }

@inproceedings{wei2016cpm,
  author = {Shih-En Wei and Varun Ramakrishna and Takeo Kanade and Yaser Sheikh},
  booktitle = {CVPR},
  title = {Convolutional pose machines},
  year = {2016}
  }
Other Contributors

We would like to thank all the people who helped OpenPose in any way. The main contributors are listed in doc/contributors.md.