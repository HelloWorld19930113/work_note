# DetectionEvaluateLayer 分析   

>一般层的组成：
param：   
top：   
bottom：   
>弄清楚每一层的top和bottom是什么？    


## protobuf定义
```proto
message DetectionEvaluateParameter {
  // 实际预测的类别数(Required!)，默认值为-1
  optional uint32 num_classes = 1;

  // 背景类的 `Label id`. 为保证背景类既不在’ground truth‘中
  // 也不在检测对象中，必须进行完整性检查
  optional uint32 background_label_id = 2 [default = 0];

  // 判断是 true/false positive 的阈值.
  optional float overlap_threshold = 3 [default = 0.5];
  
  // 如果为真，那么也会考虑’evaluation‘时的 difficult ground truth.
  optional bool evaluate_difficult_gt = 4 [default = true];

  // 该文件中包含一系列的‘names’和'sizes',其顺序和输入数据库‘DB’中存储顺序相同
  // 格式如下：
  //    name height width
  // 如果文件存在，那么就会使用缩放后的‘prediction’和‘ground truth NormalizedBBox’
  // 来做‘evaluation’
  optional string name_size_file = 5;

  // 将‘NormalizedBBox'转换为原始图片使用的‘resize’参数.
  optional ResizeParameter resize_param = 6;
}
```

## 1. detection_evaluate_layer.hpp 中定义的变量    
```cpp
int num_classes_;	
int background_label_id_;
float overlap_threshold_;
bool evaluate_difficult_gt_;
vector<pair<int, int> > sizes_;
int count_;

// 如果’name_size_file‘中存在有效数据，则不使用’normalized bbox‘来做最后的估计
bool use_normalized_bbox_;   

bool has_resize_;          // 是否提供了’resize参数‘
ResizeParameter resize_param_;  //保存‘resize参数’
```
## 2. 核心成员函数
2.1 LayerSetUp()函数  
获取层中的参数信息，并根据这些参数初始化层的成员变量。  
2.2 Reshape()函数  
bottom[0], n-c-w(1-1-7)   
bottom[1], n-c-w(1-1-8)   
num() 和 channels() 是 1.
TODO: 计算top层的shape。    
```cpp
// num() and channels() are 1.
vector<int> top_shape(2, 1);
int num_pos_classes = background_label_id_ == -1 ?
    num_classes_ : num_classes_ - 1;
int num_valid_det = 0;
const Dtype* det_data = bottom[0]->cpu_data();
for (int i = 0; i < bottom[0]->height(); ++i) {
  if (det_data[1] != -1) {
    ++num_valid_det;
  }
  det_data += 7;
}
top_shape.push_back(num_pos_classes + num_valid_det);
// Each row is a 5 dimension vector, which stores
// [image_id, label, confidence, true_pos, false_pos]
top_shape.push_back(5);
top[0]->Reshape(top_shape);
```
2.3 Forward_cpu()函数
bottom中包含两部分数据：
- det_data：检测数据， TODO？    
- gt_data：真值数据；    
获取所有的检测结果，格式为`map<int, LabelBBox>`；   
获取所有的`ground truth`(including difficult ones)，格式为`map<int, LabelBBox>`；   
给每一个`label`都加入`ground truth`数值；   
插入`detection`和 `evaluate`的`status`；   









