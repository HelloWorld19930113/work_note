# 追踪算法简介
1.KCF——Kernelized Correlation Filter
 核函数相关滤波

`OpenCV3`在`opencv_contrib`中新增了`KCF Tracker`，故想体验一下。
OpenCV3的配置见这里。
我之前给`VS`配置的是`OpenCV2.4.13`，这次更改为`OpenCV3.1.0`后，将`Project`->`属性`中的包含目录改为：D:\opencv310\opencv\build\include\opencv2和D:\opencv310\opencv\build\include\opencv后，发现报重定义的错误，怀疑没有改成功。遂直接在“属性管理器”直接更改`Microsoft.CPP.x64.user`文件，就ok了。

测试KCF的代码如下：
```cpp
#include <opencv2/opencv.hpp>    
#include <core/types.hpp>  //声明Rect2d  
#include <opencv2/tracking/tracker.hpp>   //声明Tracker类型  
  
using namespace std;  
using namespace cv;  
  
int main() {  
    // declares all required variables    
    //! [vars]    
    Rect2d roi;  
    Mat frame;  
    //! [vars]    
  
    // create a tracker object    
    Ptr<Tracker> tracker = Tracker::create("KCF");  
    //! [create]    
  
    // set input video    
    //! [setvideo]    
    std::string video = "D:\\Project\\MotionDetector\\MotionDetection\\fastMCD-master\\data\\LosAngelesCarChase05September2014_KABC.avi";  
    cv::VideoCapture cap(video);  
    //! [setvideo]    
  
    // get bounding box    
    //! [getframe]    
    cap >> frame;  
    //! [getframe]    
    //! [selectroi]选择目标roi以GUI的形式    
    roi = selectROI("tracker", frame);  
    //! [selectroi]    
  
    //quit if ROI was not selected    
    if (roi.width == 0 || roi.height == 0)  
        return 0;  
  
    // initialize the tracker    
    //! [init]    
    tracker->init(frame, roi);  
    //! [init]    
  
    // perform the tracking process    
    printf("Start the tracking process\n");  
    for (;;) {  
        // get frame from the video    
        cap >> frame;  
  
        // stop the program if no more images    
        if (frame.rows == 0 || frame.cols == 0)  
            break;  
  
        // update the tracking result    
        //! [update]    
        tracker->update(frame, roi);  
        //! [update]    
  
        //! [visualization]    
        // draw the tracked object    
        rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);  
  
        // show image with the tracked object    
        imshow("tracker", frame);  
        //! [visualization]    
        //quit on ESC button    
        if (waitKey(1) == 27)  
            break;  
    }  
  
    return 0;  
}  
```
实验效果：
1.`KCF`速度很快的，`720P`可以达到`30fps`。
2.`KCF`对部分遮挡的效果还是很好的，但处理全遮挡不行，因为毕竟不属于`Long Term Tracker`，没有重检测机制，还是要结合`TLD`比较好。

