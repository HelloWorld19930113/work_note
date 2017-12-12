# data_transformer 

>这是ssd中核心的数据处理模块。  
>`AnnotatedDatum`和`Datum`的数据有什么区别？   

## 重要的类定义:    
1.1 Datum定义   
```proto
message Datum {
  optional int32 channels = 1;
  optional int32 height = 2;
  optional int32 width = 3;
  // the actual image data, in bytes
  optional bytes data = 4;
  optional int32 label = 5;
  // Optionally, the datum could also hold float data.
  repeated float float_data = 6;
  // If true data contains an encoded image that need to be decoded
  optional bool encoded = 7 [default = false];
}
```
1.2 DataTransformer定义    
```cpp
protected:
  // 图像变换的参数信息  
  TransformationParameter param_;

  shared_ptr<Caffe::RNG> rng_;
  Phase phase_;
  Blob<Dtype> data_mean_;
  vector<Dtype> mean_values_;      // 训练数据的均值(B-G-R)  
```


```cpp
// 处理后的数据通过transformed_data返回。  
template<typename Dtype>
void DataTransformer<Dtype>::Transform(const Datum& datum,
                                       Dtype* transformed_data) 
{
  NormalizedBBox crop_bbox;
  bool do_mirror;
  Transform(datum, transformed_data, &crop_bbox, &do_mirror);
}
```

## 3. 各种Transform()  
3.1 
```cpp
/**
 *
 *  datum: 原始的输入数据;
 *	transformed_data:转换后返回的数据;
 *  NormalizedBBox:归一化之后的BBOX, TODO?
 *  crop_bbox: 裁剪框。归一化处理过的;
 *	do_mirror: 镜像操作； 
*/
template<typename Dtype>
void DataTransformer<Dtype>::Transform(const Datum& datum,
                                       Dtype* transformed_data,
                                       NormalizedBBox* crop_bbox,
                                       bool* do_mirror)
{
	(1) 获取datum的参数信息(channels、height、width和has_uint8等)，并检测有效性;
	(2) 获取图像变换的参数信息(crop_size、scale、mirror和has_mean_file等)，并检测有效性;
	(3) 随机`crop`的策略。在(0~(datum_height - crop_size + 1))之间选取一个随机偏移，
	    这样可以保证裁取`crop_size`大小的框时不会越界，并对其坐标值进行`normalized`归一化处理。  
	(4) 数据处理，数据大小为`crop_size×crop_size`。  
}
```
随机`crop`的策略——只在`TRAIN`阶段。   
```cpp
h_off = Rand(datum_height - crop_size + 1);
w_off = Rand(datum_width - crop_size + 1);

// Return the normalized crop bbox.
crop_bbox->set_xmin(Dtype(w_off) / datum_width);
crop_bbox->set_ymin(Dtype(h_off) / datum_height);
crop_bbox->set_xmax(Dtype(w_off + width) / datum_width);
crop_bbox->set_ymax(Dtype(h_off + height) / datum_height);
```
**Note**
随机裁剪的决定权在图像`Transformation Parameter`的`crop_size`参数，如果这个参数为0，则不进行`crop`。   
3.2 对`datum`和`Annotation Group`都进行`Transform`。  
```cpp
/**
 *  
 *  datum: 原始的输入数据;
 *	transformed_blob:转换后返回的数据;
 *  transformed_anno_group_all:归一化之后的BBOX, TODO?
 *	do_mirror: 镜像操作； 
*/
void DataTransformer<Dtype>::Transform(
    const AnnotatedDatum& anno_datum, Blob<Dtype>* transformed_blob,
    RepeatedPtrField<AnnotationGroup>* transformed_anno_group_all,
    bool* do_mirror) 
{
	(1) Transform datum.  TODO: datum是不是标签数据信息？
  	(2) Transform annotation.
}
```
这个`Transform`函数是`ssd`中特色化的东西。    
3.3 `Transform Annotation`需要随着图片变换而变换。  
```cpp
void DataTransformer<Dtype>::TransformAnnotation(
    const AnnotatedDatum& anno_datum, const bool do_resize,
    const NormalizedBBox& crop_bbox, const bool do_mirror,
    RepeatedPtrField<AnnotationGroup>* transformed_anno_group_all) 
{

	TODO。。。？
}
```

## 4. 图像裁剪和扩展   
4.1 `CropImage()`   
```cpp
void DataTransformer<Dtype>::CropImage(const Datum& datum,
                                       const NormalizedBBox& bbox,
                                       Datum* crop_datum) 
{
	(1) 如果 `datum` 是经过编码的, 需要解码后再裁剪 `cv::image`.   
	(2) 获取 `bbox` 的维度, `clipped_bbox` 和 `scaled_bbox`。
	    实际`crop`时使用的是`scaled_bbox`，即`(xmin,ymin),(xmax, ymax)`；   
	(3) `crop buffer`的大小为`crop_datum_size = datum_channels * height * width`；  
	(4) 开始拷贝像素值；   
}                                      
```
4.2 
```cpp
void DataTransformer<Dtype>::CropImage(const AnnotatedDatum& anno_datum,
                                       const NormalizedBBox& bbox,
                                       AnnotatedDatum* cropped_anno_datum) 
{
	(1) 裁剪`datum`；  
	(2) 根据`crop_bbox`对`annotation`进行`Transform`；  
}
```
4.3 `Annotation Transform`函数  
```cpp
/**
* @brief Transform the annotation according to the transformation applied
* to the datum.
*
* @param anno_datum
*    AnnotatedDatum 包含要转换的 data 和 annotation.
* @param do_resize
*    If true, 在 crop 之前对 annotation 执行 resize 操作.
* @param crop_bbox
*    在 anno_datum.datum() 进行裁剪的区域
* @param do_mirror
*    If true, 指示源数据是否经过镜像.
* @param transformed_anno_group_all
*   保存所有变换后的 AnnotationGroup.
*/
void TransformAnnotation(
  const AnnotatedDatum& anno_datum, const bool do_resize,
  const NormalizedBBox& crop_bbox, const bool do_mirror,
  RepeatedPtrField<AnnotationGroup>* transformed_anno_group_all);
```
4.4 `ClipBBox`函数    
```cpp
// 裁剪`bbox`到`[0, 1]`范围内.
void ClipBBox(const NormalizedBBox& bbox, NormalizedBBox* clip_bbox);

// 裁剪`bbox`到`[0, 0; width, height]`.
void ClipBBox(const NormalizedBBox& bbox, const float height, const float width,
              NormalizedBBox* clip_bbox);
```
另外的`ScaleBBox`、`LocateBBox`、`ProjectBBox`和其他的一些`box`也是类似的实现方式。    
4.5 `ExpandImage`函数   
```cpp
void DataTransformer<Dtype>::ExpandImage(const Datum& datum,
                                         const float expand_ratio,
                                         NormalizedBBox* expand_bbox,
                                         Datum* expand_datum) 
{
	...
	// 对图片大小以`expand_ratio`进行缩放。
	int height = static_cast<int>(datum_height * expand_ratio);
	int width = static_cast<int>(datum_width * expand_ratio);
	
	// 缩放/膨胀之后图像的数据的大小 
	const int expand_datum_size = datum_channels * height * width;
	const std::string& datum_buffer = datum.data();
	std::string buffer(expand_datum_size, ' ');

	// 随后对expand区域的像素值进行拷贝
	buffer[expand_datum_index] = datum_buffer[datum_index];
}
```
其中，`expand_ratio`是对图像的`height`和`width`进行缩放的。  
4.6 `DistortImage`  
只是对编码过的`datum`有效。   





