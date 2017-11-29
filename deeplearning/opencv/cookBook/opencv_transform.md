# opencv 图像变换


1. Mat roi区域赋值

Mat中的roi区域操作也很简单，如下：
取temp矩阵的roi区域，然后把src中的值赋给它 
```cpp
cv::Mat temp(src.rows+20,src.cols+34,CV_8UC1);
temp.setTo(255);

cv::Mat tempRoi(temp,cv::Rect(17,10,src.cols,src.rows));
src.copyTo(tempRoi);
```
对于这个copyTo函数，如果tempRoi与src的大小不同的话

2. 