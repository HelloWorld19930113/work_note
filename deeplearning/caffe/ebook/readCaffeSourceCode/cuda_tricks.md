# cuda的设计技巧：
在common.hpp中用到一个宏定义CUDA_KERNEL_LOOP

```
#define CUDA_KERNEL_LOOP(i, n) \
  for (int i = blockIdx.x * blockDim.x + threadIdx.x; \
       i < (n); \
       i += blockDim.x * gridDim.x)
```
先看看`caffe`采取的线程格和线程块的维数设计，还是从`device_alternate.hpp`可以看到
CAFFE_CUDA_NUM_THREADS
CAFFE_GET_BLOCKS(constintN)
明显都是一维的。
整理一下CUDA_KERNEL_LOOP格式看看，
for(inti = blockIdx.x * blockDim.x + threadIdx.x;i< (n);i+= blockDim.x * gridDim.x)
blockDim.x* gridDim.x表示的是该线程格所有线程的数量。
n表示核函数总共要处理的元素个数。
有时候，n会大于blockDim.x* gridDim.x，因此并不能一个线程处理一个元素。
由此通过上面的方法，让一个线程串行（for循环）处理几个元素。
再来看一下这个核函数的实现。
template<typename Dtype>
__global__void mul_kernel(const int n, const Dtype* a,
constDtype* b, Dtype* y)
{
  CUDA_KERNEL_LOOP(index,n)
  {
  y[index]= a[index] * b[index];
  }
}
就是算两个向量的点积了。由于向量的维数可能大于该kernel函数线程格的总线程数量。