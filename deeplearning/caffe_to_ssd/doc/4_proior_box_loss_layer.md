# PriorBoxLayer   



## 1. PriorBoxParameter proto
```proto
// Message that store parameters used by PriorBoxLayer
message PriorBoxParameter {
  // 编码/解码 类型.
  enum CodeType {
    CORNER = 1;
    CENTER_SIZE = 2;
    CORNER_SIZE = 3;
  }
  // box 的最小尺寸 (单位为像素值). Required!
  repeated float min_size = 1;
  // box 的最大尺寸 (单位为像素值). Required!
  repeated float max_size = 2;

  // 多种不同的长宽比 (aspect ratios). 重复的长宽比会被忽略 .
  // 如果未指定, 使用 默认的长宽比 1 .
  repeated float aspect_ratio = 3;

  // 如果为 true, 将会 翻转每一个 aspect ratio.
  // 例如, 对于 aspect ratio "r",  也会生成 "1.0/r" 的 aspect ratio.
  optional bool flip = 4 [default = true];
  
  // 如果为 true, 将会将 prior 的值限定在 [0, 1] 内.  
  optional bool clip = 5 [default = false];
  // Variance for adjusting the prior bboxes.
  repeated float variance = 6;

  // By default, we calculate img_height, img_width, step_x, step_y based on
  // bottom[0] (feat) and bottom[1] (img). Unless these values are explicitely
  // provided.
  // Explicitly provide the img_size.
  optional uint32 img_size = 7;
  // Either img_size or img_h/img_w should be specified; not both.
  optional uint32 img_h = 8;
  optional uint32 img_w = 9;

  // Explicitly provide the step size.
  optional float step = 10;
  // Either step or step_h/step_w should be specified; not both.
  optional float step_h = 11;
  optional float step_w = 12;

  // Offset to the top left corner of each cell.
  optional float offset = 13 [default = 0.5];
}
```

## 3. TODO 不懂   
1. num_priors_ (即 len(aspect_ratio))   
```cpp
// num_priors_ = 6;这里很重要，不然就只有5个，和论文中的6个就不相符了
num_priors_ += 1;   
```
2. prior box 中心点的确定  
```cpp
// 这里和 Faster RCNN 一样，就是把 feature map 上的点映射回原图   
// 这里加上 offset_( = 0.5) 也是为了四舍五入，和 faster rcnn python 代码类似  
float center_x = (w + offset_) * step_w;   
float center_y = (h + offset_) * step_h;
```
3. min_size 和 max_size 的作用?     
min_size 和 max_size 是指 prior box 的最小尺寸和最大尺寸.而且这两个尺寸是必须被指定的. 他们两个分别确定了 first prior box(aspect_ratio = 1, size = min_size) 和 second prior box(aspect_ratio = 1, size = sqrt(min_size * max_size)). 其余的 prior box 由 aspect ratio 确定(box_width = min_size_ * sqrt(ar), box_height = min_size_ / sqrt(ar)).   
到这里，所有的prior_box选取完成，共6个比例，每一层都会设置一个min_size.    
4. 
// 设置 variance 值.  
// 解答： https://github.com/weiliu89/caffe/issues/75  
// 除以variance是对预测box和真实box的误差进行放大，从而增加loss，增大梯度，加快收敛。  
// 另外，top_data += top[0]->offset(0, 1); 已经使指针指向新的地址，所以 variance 不会覆盖前面的结果。  
```
top_data += top[0]->offset(0, 1); // 这里我猜是指向了下一个chanel
if (variance_.size() == 1) {
	caffe_set<Dtype>(dim, Dtype(variance_[0]), top_data);
}
```
