# 梳理caffe代码 - syncedmem

接着最重要的就是内存分配和`Caffe`的底层数据的切换（`cpu模式和gpu模式`），需要用到内存同步模块。这类个类的代码比较少，但是作用是非常明显的。文件对应着`syncedmem.hpp`和`syncedmem.cpp`。首先是两个全局的内联函数。如果机器是支持`GPU`的并且安装了`cuda`，通过`cudaMallocHost`分配的`host memory`将会被`pinned`，`pinned`的意思就是内存不会被`paged out`，我们知道内存里面是由页作为基本的管理单元。分配的内存可以常驻在内存空间中对效率是有帮助的，空间不会被别的进程所抢占。同样如果内存越大，能被分配的`Pinned`内存自然也越大。还有一点是，对于单一的`GPU`而言提升并不会太显著，但是对于多个`GPU`的并行而言可以显著提高稳定性。这里是两个封装过的函数，内部通过`cuda`来分配主机和释放内存的接口.         

以下是`syncedmem.hpp`的代码：    
```cpp
#ifndef CAFFE_SYNCEDMEM_HPP_
#define CAFFE_SYNCEDMEM_HPP_

#include <cstdlib>
#ifdef USE_MKL
  #include "mkl.h"
#endif
#include "caffe/common.hpp"
namespace caffe {
// If CUDA is available and in GPU mode, host memory will be allocated pinned,
// using cudaMallocHost. It avoids dynamic pinning for transfers (DMA).
// The improvement in performance seems negligible in the single GPU case,
// but might be more significant for parallel training. Most importantly,
// it improved stability for large models on many GPUs.
inline void CaffeMallocHost(void** ptr, size_t size, bool* use_cuda) {
#ifndef CPU_ONLY
  if (Caffe::mode() == Caffe::GPU) {
    CUDA_CHECK(cudaMallocHost(ptr, size));
    *use_cuda = true;
    return;
  }
#endif
#ifdef USE_MKL
  *ptr = mkl_malloc(size ? size:1, 64);
#else
  *ptr = malloc(size);
#endif
  *use_cuda = false;
  CHECK(*ptr) << "host allocation of size " << size << " failed";
}
inline void CaffeFreeHost(void* ptr, bool use_cuda) {
#ifndef CPU_ONLY
  if (use_cuda) {
    CUDA_CHECK(cudaFreeHost(ptr));
    return;
  }
#endif
#ifdef USE_MKL
  mkl_free(ptr);
#else
  free(ptr);
#endif
}
/**
* @brief Manages memory allocation and synchronization between the host (CPU)
*        and device (GPU).
*
* TODO(dox): more thorough description.
*/
class SyncedMemory {
public:
  // 第一个为简单初始化，第二个只是把 size （大小）设置了，并未申请内存。
  SyncedMemory();
  explicit SyncedMemory(size_t size);
  ~SyncedMemory();
  // 分别是获取cpu，gpu中数据的指针，需要说明的一点是，该过程会同步数据。
  // 有获取，就有设置 - set_cpu_data()和set_gpu_data()
  //
  // 注意：set_xxx_data()后就不再拥有该数据，即 own_cpu_data 或
  // own_gpu_data 就变为 false，因为还有data指向该数据。
  // 一般来说，只有当同步后才会为true，也即to_cpu()或者to_gpu()后。
  const void* cpu_data();
  void set_cpu_data(void* data);  // 清空CPU的数据
  const void* gpu_data();
  void set_gpu_data(void* data);
  void* mutable_cpu_data();//获取可以更改cpu数据,并改变数据的状态为HEAD_AT_CPU
  void* mutable_gpu_data();
  // SymceHead有四种状态: 无数据、数据在 cpu 、gpu 、cpu 和 gpu 中都有  
  enum SyncedHead { UNINITIALIZED, HEAD_AT_CPU, HEAD_AT_GPU, SYNCED };
  SyncedHead head() { return head_; }
  size_t size() { return size_; }
#ifndef CPU_ONLY
  void async_gpu_push(const cudaStream_t& stream);
#endif
private:
  void check_device();
  /*
   * 功能：把数据放到cpu上
   * 1.数据未初始化，则在cpu申请内存（申请为0）。此时状态为HEAD_AT_CPU
   * 2.数据本来在gpu，则从gpu拷贝内存到cpu。此时状态为SYNCED
   * 3.数据本来在cpu，不做处理
   * 4.数据在cpu和gpu都有，不做处理
  */  
  void to_cpu();
  /*
   * 功能：把数据放到gpu上
   * 1.数据未初始化，在gpu申请内存（申请为0）。此时状态为HEAD_AT_GPU
   * 2.数据在cpu，从cpu拷贝到gpu。此时状态为SYNCED
   * 3.数据在gpu，不做操作。
   * 4.数据在cpu和gpu都有，不做操作。
  */
  void to_gpu();
  void* cpu_ptr_;  //指向cpu的指针
  void* gpu_ptr_;  //指向gpu的指指针
  size_t size_;
  SyncedHead head_;  //数据存放的位置，枚举值之一
  // own_gpu_data_和own_cpu_data_这两个成员变量的作用:
  // 表示是否拥有该数据，也即在cpu或gpu中是否有其他指针指向该数据
  bool own_cpu_data_;
  bool cpu_malloc_use_cuda_;
  bool own_gpu_data_;
  int device_;
  /*
   * DISABLE_COPY_AND_ASSIGN宏禁用该类的拷贝函数和"="操作符
   * 如果想让你的类不能使用 copy 构造函数和赋值操作符，只要将该类的
   * copy 构造函数和"="操作符函数定义为 private,并且只声明不实现
  */  
  DISABLE_COPY_AND_ASSIGN(SyncedMemory);
};  // class SyncedMemory
}  // namespace caffe
#endif  // CAFFE_SYNCEDMEM_HPP_
```