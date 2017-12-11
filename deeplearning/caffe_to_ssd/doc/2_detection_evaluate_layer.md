# DetectionEvaluateLayer 分析   

>一般层的组成：
param

## protobuf定义
```proto
message DetectionEvaluateParameter {
  // 实际预测的类别数(Required!)
  optional uint32 num_classes = 1;

  // 背景类的 `Label id`. 必须进行sanity检测
  // Needed for sanity check so that
  // background class is neither in the ground truth nor the detections.
  optional uint32 background_label_id = 2 [default = 0];
  // Threshold for deciding true/false positive.
  optional float overlap_threshold = 3 [default = 0.5];
  // If true, also consider difficult ground truth for evaluation.
  optional bool evaluate_difficult_gt = 4 [default = true];
  // A file which contains a list of names and sizes with same order
  // of the input DB. The file is in the following format:
  //    name height width
  //    ...
  // If provided, we will scale the prediction and ground truth NormalizedBBox
  // for evaluation.
  optional string name_size_file = 5;
  // The resize parameter used in converting NormalizedBBox to original image.
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
bool use_normalized_bbox_;

bool has_resize_;
ResizeParameter resize_param_;
```
## 2. 核心成员函数
2.1 LayerSetUp()     











