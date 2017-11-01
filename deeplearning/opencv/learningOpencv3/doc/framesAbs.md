# 图像局部与分割
 主要是作背景和前景的建模。

This cleanup usually takes the form of image-processing operations (cv::erode(), cv::dilate(), and cv::floodFill(), mostly) that eliminate stray patches of pixels.    

The first thing we need to do is to instantiate a line iterator object. We do this with
the `cv::LineIterator` constructor:     
```cpp
cv::LineIterator::LineIterator(
	const cv::Mat& image, // 要迭代的图像
	cv::Point pt1, // 线段的起点
	cv::Point pt2, // 线段的终点
	// 连通性,4(直线可以沿着右、左、上、下方向) 或 8(再增加对角线方向)
	int connectivity = 8,
	int left_to_right = 0 // 1='固定方向(从左到右)地迭代'
);
```

We could then use the following code to detect the magnitude (absolute value) of foreground differences in frameForeground:        
```cpp
cv::absdiff(
	frameTime1, // First input array
	frameTime2, // Second input array
	frameForeground // Result array
);
```

Because pixel values always exhibit noise and fluctuations, we should ignore (set to 0) small differences (say, less than 15), and mark the rest as big differences (set to 255):            
```cpp
cv::threshold(
	frameForeground, // Input image
	frameForeground, // Result image
	15, // Threshold value
	255, // Max value for upward operations
	cv::THRESH_BINARY // Threshold type to use
);
```

We need to clean up small noise areas as discussed earlier; we might do this with `cv::erode()` or by using connected components. For color images, we could use the same code for each color channel and then combine the channels with the `cv::max()` function.     

## 平均背景法
The averaging method makes use of four OpenCV routines:        
1.`cv::Mat::operator +=()`, to accumulate images over time;     
2.`cv::absdiff()`, to accumulate frame-to-frame image differences over time;       
3.`cv::inRange()`, to segment the image (once a back‐ground model has been learned) into foreground and background regions;        
4.`cv::max()`, to compile segmentations from different color channels into a single mask image.            
