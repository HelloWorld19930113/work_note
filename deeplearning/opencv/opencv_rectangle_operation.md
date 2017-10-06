# OpenCV 中的矩形框操作


cvRentangle和cv::rectangle函数原型对比：

```cpp
C:       void cvRectangle(CvArr* img, CvPoint pt1, CvPoint pt2, CvScalar color, int thickness=1, int line_type=8, int shift=0 )

C++: void rectangle(Mat& img, Point pt1, Point pt2,const Scalar& color, int thickness=1, int lineType=8, int shift=0)
C++: void rectangle(Mat& img, Rect rec, const Scalar& color, int thickness=1, int lineType=8, int shift=0 )
```

使用示例：

1. 已知矩形的两个顶点
cv::rectangle(matImage,cvPoint(20,200),cvPoint(200,300),Scalar(255,0,0),1,1,0);  
2. 已知一个矩形的变量——Rect
//Rect(int a,int b,int c,int d)a,b为矩形的左上角坐标,c,d为矩形的长和宽  
cv::rectangle(matImage,Rect(100,300,20,200),Scalar(0,0,255),1,1,0); 

line
int width = img.width();
Mat