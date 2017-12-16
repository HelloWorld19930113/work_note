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
2. 
```cpp
// 这里和 Faster RCNN 一样，就是把 feature map 上的点映射回原图   
// 这里加上 offset_( = 0.5) 也是为了四舍五入，和 faster rcnn python 代码类似  
float center_x = (w + offset_) * step_w;   
float center_y = (h + offset_) * step_h;
```