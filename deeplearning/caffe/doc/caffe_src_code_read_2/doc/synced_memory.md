# 内存同步操作   

最重要的就是内存分配和 caffe 的底层数据的切换（cpu模式和gpu模式），需要用到内存同步模块。这个类的代码比较少，但是作用是非常明显的。文件对应着 syncedmem.hpp 和 syncedmem.cpp。   
首先是两个全局的内联函数。如果机器是支持 GPU 的并且安装了 cuda ，通过 cudaMallocHost 分配的 host memory 将会被 pinned ， pinned 的意思就是内存不会被 paged out ， 我们知道内存里面是由页作为基本的管理单元。分配的内存可以常驻在内存空间中对效率是有提升的，因为这样的空间不会被别的进程所抢占。同样，如果内存越大，能被分配的 Pinned 内存自然也越大。还有一点是，对于单一的 GPU 而言提升并不会太显著，但是对于多个 GPU 的并行而言可以显著提高稳定性。这里是两个封装过的函数，内部通过 cuda 来分配和释放内存。   

> Intel Math Kernel Library，简称 MKL 。

## 1. 带注释的头文件
[synced_memory.hpp](synced_memory.hpp)

## 2. 代码理解   
2.1 `SyncedMemory`的成员变量    
```cpp
private:
  void check_device();

  void to_cpu();
  void to_gpu();
  void* cpu_ptr_;
  void* gpu_ptr_;
  size_t size_;
  SyncedHead head_;
  bool own_cpu_data_;
  bool cpu_malloc_use_cuda_;
  bool own_gpu_data_;
  int device_;
```

`cudaError_t CUDARTAPI cudaGetDevice(int *device);`函数返回当前正在使用的 GPU 设备。     


