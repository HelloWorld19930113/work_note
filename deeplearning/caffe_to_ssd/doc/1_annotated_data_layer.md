# AnnotatedDataLayer 分析  
 这层并没有`Forward_cpu`和`Backward_cpu`函数。    
 
## 成员变量     
```cpp
DataReader<AnnotatedDatum> reader_;
bool has_anno_type_;
AnnotatedDatum_AnnotationType anno_type_;
vector<BatchSampler> batch_samplers_;
string label_map_file_;
```

## 重要变量的含义:    
```cpp
top_shape:是该层的输入数据的维度    
label_shape:标注好的Bbox框的xmin, ymin, xmax, ymax？
prefetch_:    
data_transformer_：定义在baseDataLayer中。   
shared_ptr<DataTransformer<Dtype> > data_transformer_;    
```

1. 定义在baseDataLayer中的变量   
prefetch_ ：一共有3个batch(每个batch由data_和label_ Blob组成)，定义在baseDataLayer中。   
Batch<Dtype> prefetch_[PREFETCH_COUNT];
BlockingQueue<Batch<Dtype>*> prefetch_free_;
BlockingQueue<Batch<Dtype>*> prefetch_full_;

Blob<Dtype> transformed_data_;   

## 核心成员函数
1. DataLayerSetUp()  
只是对数据的shape进行调整，包括`data`和`label`。
2. load_batch()  
该函数由`prefetch`线程调用。    

TODO:
1.扭曲操作和expand操作的结果？    
2.batch_samplers_的意义？   
3.GenerateBatchSamples()做了什么？   
使用BatchSampler在AnnotatedDatum数据上采样得到一系列的采样框，得到的采样框保存在sampled_bboxes中。   
4. 在上面得到的采样框中随机选取一个，并基于`expand_datum`进行CropImage。   
5. 开始transform数据。
6. 保存"rich" annotation到label_中。

anno_datum： 存放的是标注数据。    
distort_param： 图像扭曲方法。     
expand_param: 图像扩展方法。     
batch_samplers_: 保存采样器的参数。   
output_labels_： 是否带有输出标签。   

## data_transformer  
源文件为[data_transformer.cpp](./data_transformer.hpp)。    


以下代码进行数据增广， `Transform`的作用？   
```
this->data_transformer_->Transform(*sampled_datum,
                                           &(this->transformed_data_),
                                           &transformed_anno_vec);
```


