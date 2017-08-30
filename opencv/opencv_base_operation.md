# 操作像素
 这里使用的都是一些基本的方法。

## 基本操作

1. 读取图片到一个矩阵中；
```cpp
Mat imread(const string& filename, int flags=1)
...
cv::Mat image = cv::imread("boldt.jpg", 0);
```
imread()返回的是保存图片的像素矩阵`Mat`。其中第二个参数为0表示返回的是灰度图，>1返回的是3-通道图片。
2. 在一个图像框中显示图片，可以对这个图形框命名；
```cpp
cv::imshow("Image", image);
```
3. 将像素矩阵保存为图片，保存的图片名和格式可以指定；
```cpp
cv::imwrite("salted.bmp", image);
```
4. 等待用户输入;
```cpp
cv::waitKey(5000);
```
5. cv::Mat常用属性
```cpp
cv::Mat image；
image.cols;
image.rows;
image.channels();
image.at<uchar>(j, i); // 单通道像素点；
image.at<cv::Vec3b>(j, i)[0] // 三通道像素点；
```

## 高级操作
Mat.data表示的是矩阵的首地址；

1. 融合两张图片
```cpp
// Calculates the weighted sum of two arrays
void addWeighted(InputArray src1, double alpha, InputArray src2, double beta, double gamma, OutputArray dst, int dtype=-1)
// dst = src1*alpha + src2*beta + gamma;
```
2. 图像的分离和合并
`split()`将图像中的3个通道分离为单通道矩阵，并保存在一个`vector`中；这样就可以直接操作图像的某一个通道了，一般选用vector<Mat>().at(n)访问对应的通道数据。
将3个单通道矩阵合并为一张图片；
```cpp
void split(const Mat& m, vector<Mat>& mv );
void merge(const vector<Mat>& mv, OutputArray dst );
...
cv::split(image1, planes);
cv::merge(planes, result)；
```


3. 操作图像的某一个固定区域——ROI
```cpp
Rect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height);
void copyTo( OutputArray m, InputArray mask );
...
cv::Mat imageROI = image(cv::Rect(385, 270, logo.cols, logo.rows));  // 获取图片中的某一区域
cv::addWeighted(imageROI, 1.0, logo, 0.3, 0., imageROI); // 对该区域进行处理
logo.copyTo(imageROI, mask);  // 将logo图像矩阵拷贝到imageROI；
...
imageROI = channels.at(1);  // 获取第二通道(G通道)对应的图像数据；
```
imageROI返回值是指向原图的ROI区域的指针，因此改变这个值相应的会体现在原图中。
Rect_()是模板类，参数为矩形框左上角的坐标和矩形框的长和宽。

4. 比较几种遍历图像方法的效率
获取当前系统的运行时间：
`cv::getTickCount()`可以获取系统当前`tick`的个数；
`cv::getTickFrequency()`可以获取一个`tick`的频率，即一秒包含几个`tick`；
因此计算程序运行时间(单位为ms)的代码为：
```cpp
double exec_time = (double)getTickCount();
// do something ...
exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
```

5. 获取像素的前后左右行
 限制像素值在[0,255]之间，`cv::saturate_cast<uchar>()`;
 将图像看成是一个宽度x高度的矩阵，那么可以有以下3种方式来获取当前行：
 <1>Mat.ptr()，





## Mat方法
1. size()
返回图像的尺寸，长和宽。
2. create()
根据图像属性创建Mat
```cpp
void create(int rows, int cols, int type);   // 
void create(Size size, int type);
void create(int ndims, const int* sizes, int type);
```