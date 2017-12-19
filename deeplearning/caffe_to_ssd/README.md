# 从 caffe 到 ssd  
主要分析`ssd`基于`caffe`做了哪些工作。    


## 新添加的层   
[1. protobuf 使用方法](./doc/protobuf_compile.md)  
[2. datum 和 blob 的区别](./doc/datum_blob.md)  
[3. annotated_data_layer](./doc/1_annotated_data_layer.md)  
[4. detection_evaluate_layer](./doc/2_detection_evaluate_layer.md)  
[5. data_transformer](./doc/data_transformer.md)  


## common 概念   

`LayerSetUp()`实现该层特定的一些参数初始化操作.
`Reshape()`是根据`bottom`的 `shape` 来 reshape top blob 的 shape. 
一定要分清 `_param 和 (bottom 和 top)` 各自的类型. bottom 和 top 的类型为 Blob.
.prototxt 中定义的 bottom blob 和 top blob 的个数是确定好的.  因为这在对应层的 .cpp 实现文件中会确定使用.    