# 操作像素

## 

1. 读取图片到一个矩阵中；
cv::Mat image = cv::imread("boldt.jpg", 0);

1. 在一个图像框中显示图片，可以对这个图形框命名；
cv::imshow("Image", image);

3. 将像素矩阵保存为图片，保存的图片名和格式可以指定；
cv::imwrite("salted.bmp", image);

4. 等待用户输入;
cv::waitKey(5000);

5. cv::Mat常用属性
```cpp
cv::Mat image；
image.cols;
image.rows;
image.channels();
image.at<uchar>(j, i); // 单通道像素点；
image.at<cv::Vec3b>(j, i)[0] // 三通道像素点；


```
