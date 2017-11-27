# 图像变换

## flip翻转
cv::flip
```cpp
void flip(InputArray src, OutputArray dst, int flipCode)  
```
参数：
`src`，输入矩阵
`dst`，翻转后矩阵，类型与src一致
`flipCode`，翻转模式，flipCode==0垂直翻转（沿X轴翻转），flipCode>0水平翻转（沿Y轴翻转），flipCode<0水平垂直翻转（先沿X轴翻转，再沿Y轴翻转，等价于旋转180°）



