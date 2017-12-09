# AnnotatedDataLayer 分析   





重要变量的含义:    

top_shape:是该层的输入数据的维度    
label_shape:标注好的Bbox框的xmin, ymin, xmax, ymax？
prefetch_:    
data_transformer_：定义在baseDataLayer中。   
shared_ptr<DataTransformer<Dtype> > data_transformer_;    

prefetch_ ：一共有3个batch(每个batch由data_和label_ Blob组成)，定义在baseDataLayer中。   
Batch<Dtype> prefetch_[PREFETCH_COUNT];
BlockingQueue<Batch<Dtype>*> prefetch_free_;
BlockingQueue<Batch<Dtype>*> prefetch_full_;







## DataLayerSetUp()      
只是对数据的shape进行调整，包括`data`和`label`。



## load_batch()
该函数由`prefetch`线程调用。    

TODO:
1.扭曲操作和expand操作的结果？    
2.batch_samplers_的意义？   
3.GenerateBatchSamples()做了什么？   

anno_datum： 存放的是标注数据。    
distort_param： 图像扭曲方法。     
expand_param: 图像扩展方法。     


随机选取一个采样框并在`expand_datum`上进行裁剪。   

