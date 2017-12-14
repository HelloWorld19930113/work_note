#ifndef CAFFE_SYNCEDMEM_HPP_
#define CAFFE_SYNCEDMEM_HPP_

#include <cstdlib>

#ifdef USE_MKL
  #include "mkl.h"
#endif

#include "caffe/common.hpp"

namespace caffe {

/** 
 * @brief 如果 CUDA 可用并且处于 GPU 模式下， cudaMallocHost() 函数分配的 host memory 将会被 pinned。
 * 这将会避免 DMA 传输时的动态 pin 。在多 GPU 的并行训练中性能提升显著。   
 *
 */
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
  SyncedMemory();
  explicit SyncedMemory(size_t size);
  ~SyncedMemory();
  const void* cpu_data();
  void set_cpu_data(void* data);
  const void* gpu_data();
  void set_gpu_data(void* data);
  void* mutable_cpu_data();
  void* mutable_gpu_data();
  enum SyncedHead { UNINITIALIZED, HEAD_AT_CPU, HEAD_AT_GPU, SYNCED };
  SyncedHead head() { return head_; }
  size_t size() { return size_; }

#ifndef CPU_ONLY
  void async_gpu_push(const cudaStream_t& stream);
#endif

 private:
  void check_device();

  /* 
  * \brief 把数据放到cpu上 
  * 1.数据未初始化，则在cpu申请内存（申请为0）。此时状态为HEAD_AT_CPU 
  * 2.数据本来在gpu，则从gpu拷贝内存到cpu。此时状态为SYNCED 
  * 3.数据本来在cpu，不做处理 
  * 4.数据在cpu和gpu都有，不做处理 
  */  
  void to_cpu();
  /* 
   * \brief 把数据放到gpu上 
   * 1.数据未初始化，在gpu申请内存（申请为0）。此时状态为HEAD_AT_GPU 
   * 2.数据在cpu，从cpu拷贝到gpu。此时状态为 SYNCED  
   * 3.数据在gpu，不做操作。 
   * 4.数据在cpu和gpu都有，不做操作。 
  */  
  void to_gpu();
  void* cpu_ptr_;            // 指向 cpu 数据的指针  
  void* gpu_ptr_;            // 指向 gpu 数据的指针  
  size_t size_;              // 数据的大小
  SyncedHead head_;          // 数据存放的位置，枚举值之一  
  bool own_cpu_data_;        // cpu_ptr_ 是否指向有效的 cpu 数据空间
  bool cpu_malloc_use_cuda_; // 是否使用 cuda 的 malloc
  bool own_gpu_data_;        // gpu_ptr_ 是否指向有效的 gpu 数据空间
  int device_;

  DISABLE_COPY_AND_ASSIGN(SyncedMemory);
};  // class SyncedMemory

}  // namespace caffe

#endif  // CAFFE_SYNCEDMEM_HPP_
