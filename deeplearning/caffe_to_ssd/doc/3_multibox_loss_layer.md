# MultiBoxLossLayer   

[这篇博客](http://blog.csdn.net/u011956147/article/details/73033170)主要写 multibox_loss_layer， 这也是 SSD 比较关键内容，主要包括内建了两个 layer 进行 loss 回归，还包括比如 FindMatches， MineHardExamples， EncodeLocPrediction && EncodeConfPrediction 等都是比较重要的函数（其中有一部分在 bbox_util 中，后面会介绍）.   
**主要完成的工作**   
- decode the predictions.
- perform matching between priors/predictions and ground truth.
- use matched boxes and confidences to compute loss.

## 1. proto 定义   
```proto
// MultiBoxLossLayer 中用到的参数信息  
message MultiBoxLossParameter {
  // Localization loss type.
  enum LocLossType {
    L2 = 0;
    SMOOTH_L1 = 1;
  }
  optional LocLossType loc_loss_type = 1 [default = SMOOTH_L1];
  // Confidence loss type.
  enum ConfLossType {
    SOFTMAX = 0;
    LOGISTIC = 1;
  }
  optional ConfLossType conf_loss_type = 2 [default = SOFTMAX];
  // Weight for localization loss.
  optional float loc_weight = 3 [default = 1.0];
  // Number of classes to be predicted. Required!
  optional uint32 num_classes = 4;
  // If true, bounding box are shared among different classes.
  optional bool share_location = 5 [default = true];
  // Matching method during training.
  enum MatchType {
    BIPARTITE = 0;
    PER_PREDICTION = 1;
  }
  optional MatchType match_type = 6 [default = PER_PREDICTION];
  // If match_type is PER_PREDICTION, use overlap_threshold to
  // determine the extra matching bboxes.
  optional float overlap_threshold = 7 [default = 0.5];
  // Use prior for matching.
  optional bool use_prior_for_matching = 8 [default = true];
  // Background label id.
  optional uint32 background_label_id = 9 [default = 0];
  // If true, also consider difficult ground truth.
  optional bool use_difficult_gt = 10 [default = true];
  // If true, perform negative mining.
  // DEPRECATED: use mining_type instead.
  optional bool do_neg_mining = 11;
  // The negative/positive ratio.
  optional float neg_pos_ratio = 12 [default = 3.0];
  // The negative overlap upperbound for the unmatched predictions.
  optional float neg_overlap = 13 [default = 0.5];
  // Type of coding method for bbox.
  optional PriorBoxParameter.CodeType code_type = 14 [default = CORNER];
  // If true, encode the variance of prior box in the loc loss target instead of
  // in bbox.
  optional bool encode_variance_in_target = 16 [default = false];
  // If true, map all object classes to agnostic class. It is useful for learning
  // objectness detector.
  optional bool map_object_to_agnostic = 17 [default = false];
  // If true, ignore cross boundary bbox during matching.
  // Cross boundary bbox is a bbox who is outside of the image region.
  optional bool ignore_cross_boundary_bbox = 18 [default = false];
  // If true, only backpropagate on corners which are inside of the image
  // region when encode_type is CORNER or CORNER_SIZE.
  optional bool bp_inside = 19 [default = false];
  // Mining type during training.
  //   NONE : use all negatives.
  //   MAX_NEGATIVE : select negatives based on the score.
  //   HARD_EXAMPLE : select hard examples based on "Training Region-based Object Detectors with Online Hard Example Mining", Shrivastava et.al.
  enum MiningType {
    NONE = 0;
    MAX_NEGATIVE = 1;
    HARD_EXAMPLE = 2;
  }
  optional MiningType mining_type = 20 [default = MAX_NEGATIVE];
  // Parameters used for non maximum suppression durig hard example mining.
  optional NonMaximumSuppressionParameter nms_param = 21;
  optional int32 sample_size = 22 [default = 64];
  optional bool use_prior_for_nms = 23 [default = false];
}
```

## 3. 成员函数   
3.1 LayerSetUp()    
在这个函数里面还分别新建了两个layer用于loc回归和conf loss的计算.   
```cpp
// 所有的 loss layer 都会在 LayerSetup() 函数中首先调用下面这个函数  
// 然后再去设置其他相关的成员变量    
LossLayer<Dtype>::LayerSetUp(bottom, top);
```
TODO: `add_loss_weight(val)`会将 loss_weight_ 容器中的所有元素设置为 `val`.   
3.2 Reshape()   
```cpp
// 所有的 loss layer 都会在 Reshape() 函数中首先调用下面这个函数  
// 然后再去设置其他相关的成员变量    
LossLayer<Dtype>::Reshape(bottom, top);
```