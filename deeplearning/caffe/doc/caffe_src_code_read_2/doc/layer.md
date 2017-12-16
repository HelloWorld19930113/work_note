# Layer 层  

![](../pictures/caffe_layers.png)

> 一个层可以有多个输出.   

## 2. special  
2.1 AutoTopBlobs() 函数   
```cpp
virtual inline bool AutoTopBlobs() const { return false; }
```
自动创建匿名的 top blobs， 按照参数 ExactNumTopBlobs() 或 MinTopBlobs() 指定的数量创建。   
2.2 SetUp()函数   
```
void SetUp(const vector<Blob<Dtype>*>& bottom,
  const vector<Blob<Dtype>*>& top) 
{
	1. 检查输入输出 blob 个数是否满足要求，每个层能处理的输入输出数据不一样 
    2. 调用 LayerSetUp 函数初始化特定的层，每个 Layer 子类需重写这个函数完成定制的初始化 
    3. 调用 Reshape 函数为 top blob 分配合适大小的存储空间 
    4. 为每个 top blob 的非零 loss 权重设置 loss 权重乘子
}
```


## 3. proto 定义
3.1 LayerParameter 定义   
```proto
message LayerParameter {
  optional string name = 1;   // layer 的 name
  optional string type = 2;   // layer 的 type
  repeated string bottom = 3; // 每个 bottom blob 的 name
  repeated string top = 4;    // 每个 top blob 的 name

  optional Phase phase = 10;  // TRAIN 或 TEST

  // 分配给每个 top blob 的权重值的数量 
  // 每个 layer 中都会分配一个默认值，通常给每个 top blob 分配的是 0 或 1
  repeated float loss_weight = 5;   

  // 指定训练参数(全局学习常数的乘子，name和其他权重值分享的参数)
  repeated ParamSpec param = 6; 

  repeated BlobProto blobs = 7;  // 该层所有的 blobs ,其中包含数值参数

  // 指示每个 bottom 数据是否需要进行反向运算。 如果未指定， Caffe 会自动推测是够需要
  // 进行反向运算来计算参数的梯度。 如果设置为 true ，则强制反向计算。 如果设置为 false ，
  // 则不进行反向运算。
  //
  // propagate_down_.size() 要么为 0 ， 要么等于 bottoms_ 的大小   
  repeated bool propagate_down = 11;

  // 根据当前的 NetState 控制 [是否 OR 何时] 将一个 layer 包含到一个 net 中。你可以指
  // 定一个非零值表示的规则来 [include OR exclude]。如果没指定任何规则，那么这个 layer 
  // 默认是被包含的。如果当前的 NetState 满足任何一个特定的规则，那么该 layer 就会被包含。  
  repeated NetStateRule include = 8;
  repeated NetStateRule exclude = 9;

  optional TransformationParameter transform_param = 100; // 数据预处理参数.

  optional LossParameter loss_param = 101;  // loss layers 共享的参数

  // Layer type-specific parameters.
  //
  // Note: 某些层有不止一个的计算引擎(GPUS)。这些层就会包含 Engine type 和
  // engine parameter 来选择计算方式。 默认的 engine 实在编译阶段通过 ENGINE
  // 开关来设置的。 
  optional AccuracyParameter accuracy_param = 102;
  optional ArgMaxParameter argmax_param = 103;
  optional BatchNormParameter batch_norm_param = 139;
  optional BiasParameter bias_param = 141;
  optional ConcatParameter concat_param = 104;
  optional ContrastiveLossParameter contrastive_loss_param = 105;
  optional ConvolutionParameter convolution_param = 106;
  optional CropParameter crop_param = 144;
  optional DataParameter data_param = 107;
  optional DropoutParameter dropout_param = 108;
  optional DummyDataParameter dummy_data_param = 109;
  optional EltwiseParameter eltwise_param = 110;
  optional ELUParameter elu_param = 140;
  optional EmbedParameter embed_param = 137;
  optional ExpParameter exp_param = 111;
  optional FlattenParameter flatten_param = 135;
  optional HDF5DataParameter hdf5_data_param = 112;
  optional HDF5OutputParameter hdf5_output_param = 113;
  optional HingeLossParameter hinge_loss_param = 114;
  optional ImageDataParameter image_data_param = 115;
  optional InfogainLossParameter infogain_loss_param = 116;
  optional InnerProductParameter inner_product_param = 117;
  optional InputParameter input_param = 143;
  optional LogParameter log_param = 134;
  optional LRNParameter lrn_param = 118;
  optional MemoryDataParameter memory_data_param = 119;
  optional MVNParameter mvn_param = 120;
  optional ParameterParameter parameter_param = 145;
  optional PoolingParameter pooling_param = 121;
  optional PowerParameter power_param = 122;
  optional PReLUParameter prelu_param = 131;
  optional PythonParameter python_param = 130;
  optional RecurrentParameter recurrent_param = 146;
  optional ReductionParameter reduction_param = 136;
  optional ReLUParameter relu_param = 123;
  optional ReshapeParameter reshape_param = 133;
  optional ScaleParameter scale_param = 142;
  optional SigmoidParameter sigmoid_param = 124;
  optional SoftmaxParameter softmax_param = 125;
  optional SPPParameter spp_param = 132;
  optional SliceParameter slice_param = 126;
  optional TanHParameter tanh_param = 127;
  optional ThresholdParameter threshold_param = 128;
  optional TileParameter tile_param = 138;
  optional TripletLossParameter triplet_loss_param = 147;
  optional WindowDataParameter window_data_param = 129;
}
```
3.2 ParamSpec 定义   
```proto
// 指定 training parameters 
// 这些参数有：
//		1. multipliers on global learning constants
//		2. multipliers on global weight decay
// 		3. name 
//		4. other settings used for weight sharing
message ParamSpec {
  // The names of the parameter blobs -- useful for sharing parameters among
  // layers, but never required otherwise.  To share a parameter between two
  // layers, give it a (non-empty) name.
  optional string name = 1;

  // Whether to require shared weights to have the same shape, or just the same
  // count -- defaults to STRICT if unspecified.
  optional DimCheckMode share_mode = 2;
  enum DimCheckMode {
    // STRICT (default) requires that num, channels, height, width each match.
    STRICT = 0;
    // PERMISSIVE requires only the count (num*channels*height*width) to match.
    PERMISSIVE = 1;
  }

  // The multiplier on the global learning rate for this parameter.
  optional float lr_mult = 3 [default = 1.0];

  // The multiplier on the global weight decay for this parameter.
  optional float decay_mult = 4 [default = 1.0];
}

```
3.3 NetStateRule 定义   
```proto
message NetStateRule {
  // Set phase to require the NetState have a particular phase (TRAIN or TEST)
  // to meet this rule.
  optional Phase phase = 1;

  // Set the minimum and/or maximum levels in which the layer should be used.
  // Leave undefined to meet the rule regardless of level.
  optional int32 min_level = 2;
  optional int32 max_level = 3;

  // Customizable sets of stages to include or exclude.
  // The net must have ALL of the specified stages and NONE of the specified
  // "not_stage"s to meet the rule.
  // (Use multiple NetStateRules to specify conjunctions of stages.)
  repeated string stage = 4;
  repeated string not_stage = 5;
}
```
3.4 TransformationParameter  
```proto
// Message that stores parameters used to apply transformation
// to the data layer's data
message TransformationParameter {
  // For data pre-processing, we can do simple scaling and subtracting the
  // data mean, if provided. Note that the mean subtraction is always carried
  // out before scaling.
  optional float scale = 1 [default = 1];
  // Specify if we want to randomly mirror data.
  optional bool mirror = 2 [default = false];
  // Specify if we would like to randomly crop an image.
  optional uint32 crop_size = 3 [default = 0];
  // mean_file and mean_value cannot be specified at the same time
  optional string mean_file = 4;
  // if specified can be repeated once (would subtract it from all the channels)
  // or can be repeated the same number of times as channels
  // (would subtract them from the corresponding channel)
  repeated float mean_value = 5;
  // Force the decoded image to have 3 color channels.
  optional bool force_color = 6 [default = false];
  // Force the decoded image to have 1 color channels.
  optional bool force_gray = 7 [default = false];
}

``` 
3.5 LossParameter   
```proto
// Message that stores parameters shared by loss layers
message LossParameter {
  // If specified, ignore instances with the given label.
  optional int32 ignore_label = 1;
  // How to normalize the loss for loss layers that aggregate across batches,
  // spatial dimensions, or other dimensions.  Currently only implemented in
  // SoftmaxWithLoss and SigmoidCrossEntropyLoss layers.
  enum NormalizationMode {
    // Divide by the number of examples in the batch times spatial dimensions.
    // Outputs that receive the ignore label will NOT be ignored in computing
    // the normalization factor.
    FULL = 0;
    // Divide by the total number of output locations that do not take the
    // ignore_label.  If ignore_label is not set, this behaves like FULL.
    VALID = 1;
    // Divide by the batch size.
    BATCH_SIZE = 2;
    // Do not normalize the loss.
    NONE = 3;
  }
  // For historical reasons, the default normalization for
  // SigmoidCrossEntropyLoss is BATCH_SIZE and *not* VALID.
  optional NormalizationMode normalization = 3 [default = VALID];
  // Deprecated.  Ignored if normalization is specified.  If normalization
  // is not specified, then setting this to false will be equivalent to
  // normalization = BATCH_SIZE to be consistent with previous behavior.
  optional bool normalize = 2;
}
```


## 4. 成员变量


