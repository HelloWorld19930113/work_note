# blob

## 1. 带注释的头文件
[blob.hpp](blob.hpp)

## 2. proto 定义
```proto
// 指定一个 Blob 的 shape (dimensions).
message BlobShape {
  repeated int64 dim = 1 [packed = true];
}

message BlobProto {
  optional BlobShape shape = 7;
  repeated float data = 5 [packed = true];
  repeated float diff = 6 [packed = true];
  repeated double double_data = 8 [packed = true];
  repeated double double_diff = 9 [packed = true];

  // 4D dimensions -- deprecated.  Use "shape" instead.
  optional int32 num = 1 [default = 0];
  optional int32 channels = 2 [default = 0];
  optional int32 height = 3 [default = 0];
  optional int32 width = 4 [default = 0];
}
```

## 3. 代码理解   
2.1 `Blob`的成员变量   
```
protected:
  // data_ 指针，data主要是前向传播的时候用的;指针类型是shared_ptr，属于boost库的一个智能指针  
  shared_ptr<SyncedMemory> data_;  
  // diff_主要用来存储偏差，update data
  shared_ptr<SyncedMemory> diff_;
  shared_ptr<SyncedMemory> shape_data_;
  // shape_存储Blob的形状  
  vector<int> shape_;
  // count_表示Blob中的元素个数，也就是 n*n*h*w 中的 n  
  int count_;
  // capacity表示当前的元素个数，因为Blob可能会reshape  
  int capacity_;
```
2.2 void Blob<Dtype>::Update()   
```cpp
template <typename Dtype>
void Blob<Dtype>::Update() {
  // We will perform update based on where the data is located.
  switch (data_->head()) {
  case SyncedMemory::HEAD_AT_CPU:
    // perform computation on CPU
    caffe_axpy<Dtype>(count_, Dtype(-1),
        static_cast<const Dtype*>(diff_->cpu_data()),
        static_cast<Dtype*>(data_->mutable_cpu_data()));
    break;
  case SyncedMemory::HEAD_AT_GPU:
  case SyncedMemory::SYNCED:
#ifndef CPU_ONLY
    // perform computation on GPU
    caffe_gpu_axpy<Dtype>(count_, Dtype(-1),
        static_cast<const Dtype*>(diff_->gpu_data()),
        static_cast<Dtype*>(data_->mutable_gpu_data()));
#else
    NO_GPU;
#endif
    break;
  default:
    LOG(FATAL) << "Syncedmem not initialized.";
  }
}
```
更新的核心是`caffe_axpy()`函数， 该函数进行的计算是`Y=alpha*X+Y`，从上边的调用来看`alpha = -1`, 因此，更新是 `data_ - diff_`， 更新得到的值保存在 Y 中，即 data_ 中。   
**Note**   
对于 unsigned int 和 int 由于该函数主要是在 Net 中被调用，只有 Blob<float> 和 Blob<double> 型式， 因此没有定义 unsigned int 和 int 。   

## 3. 不懂    
```cpp
void ShareData(const Blob& other);
void ShareDiff(const Blob& other);
```