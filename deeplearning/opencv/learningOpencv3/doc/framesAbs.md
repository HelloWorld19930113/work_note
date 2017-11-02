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

计算平均像素值，然后使用这个来确定高低阈值。动态的计算就可以判断前景和背景。
这种方法只能用于：
1.背景场景中不包含运动的部分
2.要求场景中的光线保持不变

## 累积均值、方差和协方差
The accumulate function has the following prototype:    
```
void accumulate(
	cv::InputArray src, // Input, 1 or 3 channels, U8 or F32
	cv::InputOutputArray dst, // Result image, F32 or F64
	cv::InputArray mask = cv::noArray() // Use src pixel if mask pixel != 0
);
```

OpenCV provides us with a function that does what we need all in a single convenient package—cv::accumulateSquare():       
```
void accumulateSquare(
	cv::InputArray src, // Input, 1 or 3 channels, U8 or F32
	cv::InputOutputArray dst, // Result image, F32 or F64
	cv::InputArray mask = cv::noArray() // Use src pixel if mask pixel != 0
);
```

In practice, the best way to do this is simply to compute the variances using the code
we already developed, and to compute the three new objects (the off-diagonal elements
of Σx,y) separately. Looking at the formula for the covariance, we see that
cv::accumulateSquare() will not quite work here, as we need to accumulate the (xi ·
yi) terms (i.e., the product of two different channel values from a particular pixel in
each image). The function that does this for us in OpenCV is cv::accumulateProd
uct():
```
void accumulateProduct(
	cv::InputArray src1, // Input, 1 or 3 channels, U8 or F32
	cv::InputArray src2, // Input, 1 or 3 channels, U8 or F32
	cv::InputOutputArray dst, // Result image, F32 or F64
	cv::InputArray mask = cv::noArray() // Use src pixel if mask pixel != 0
);
```
This function works exactly like cv::accumulateSquare(), except that rather than
squaring the individual elements of src, it multiplies the corresponding elements of
src1 and src2. What it does not do (unfortunately) is allow us to pluck individual
channels out of those incoming arrays. In the case of multichannel arrays in src1 and
src2, the computed result is done on a per-channel basis.

OpenCV provides a function for computing Mahalanobis distances:
double cv::Mahalanobis( // Return distance as F64
cv::InputArray vec1, // First vector (1-dimensional, length n)
cv::InputArray vec2, // Second vector (1-dimensional, length n)
cv::InputArray icovar // Inverse covariance matrix, n-by-n
);
The cv::Mahalanobis() function expects vector objects for vec1 and vec2 of dimension
d, and a d × d matrix for the inverse covariance icovar. (The inverse covariance
is used because inverting this matrix is costly, and in most cases you have many vectors
you would like to compare with the same covariance—so the assumption is that
you will invert it once and pass the inverse covariance to cv::Mahalanobis() many
times for each such inversion.)

