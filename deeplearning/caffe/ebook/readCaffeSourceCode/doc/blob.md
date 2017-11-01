# 梳理caffe代码 - blob

贯穿整个`caffe`的就是数据`blob`，为了更好的说明一些函数的细节。最后贴出啦实现部分的代码。以下是`caffe/include/caffe/blob.hpp`：

## blob 头文件
```cpp
#ifndef CAFFE_BLOB_HPP_
#define CAFFE_BLOB_HPP_

#include <algorithm>
#include <string>
#include <vector>

#include "caffe/common.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/syncedmem.hpp"

const int kMaxBlobAxes = 32;

namespace caffe {

/**
 * @brief A wrapper around SyncedMemory holders。
 *        是Layers,Nets,和Solvers之间的基本计算单元
 *
 * TODO(dox): more thorough description.
 */

template <typename Dtype>
class Blob {
 public:
  Blob()
       : data_(), diff_(), count_(0), capacity_(0) {}

  /// @brief Deprecated; use <code>Blob(const vector<int>& shape)</code>.
  /// explicit 关键字的作用是禁止单参数构造函数的隐式转换  
  explicit Blob(const int num, const int channels, const int height,
      const int width);
  explicit Blob(const vector<int>& shape);

  /// @brief Deprecated; use <code>Reshape(const vector<int>& shape)</code>.
  /// Reshape函数将'num,channels,height,width'传递给'vector shape_'
  void Reshape(const int num, const int channels, const int height,
      const int width);
  /**
   * @brief 改变blob的维度，如果需要会产生内存分配
   *
   * This function can be called both to create an initial allocation
   * of memory, and to adjust the dimensions of a top blob during Layer::Reshape
   * or Layer::Forward. When changing the size of blob, memory will only be
   * reallocated if sufficient memory does not already exist, and excess memory
   * will never be freed.
   *
   * Note that reshaping an input blob and immediately calling Net::Backward is
   * an error; either Net::Forward or Net::Reshape need to be called to
   * propagate the new input shape to higher layers.
   */
  /// 根据'shape'来初始化'shape_'和'shape_data_'，同时为'data_'和'diff_'分配空间。
  void Reshape(const vector<int>& shape);  // 
  void Reshape(const BlobShape& shape);
  void ReshapeLike(const Blob& other);
  /// 以'string'形式获取'shape_'，用于打印'blob'的'log'
  inline string shape_string() const {
    ostringstream stream;
    for (int i = 0; i < shape_.size(); ++i) {
      stream << shape_[i] << " ";
    }
    stream << "(" << count_ << ")";
    return stream.str();
  }
  /// 获取'shape_' 
  inline const vector<int>& shape() const { return shape_; }
  /**
   * @brief 返回第'index'轴上的维度(如果'index'为负数，则从'end'端开始计数)
   *
   * @param index the axis index, which may be negative as it will be
   *        "canonicalized" using CanonicalAxisIndex.
   *        Dies on out of range index.
   */ 
  inline int shape(int index) const {
    return shape_[CanonicalAxisIndex(index)];
  }
  inline int num_axes() const { return shape_.size(); } //获取blob的维度 
  inline int count() const { return count_; }  //获取当前'data'的大小  

  /**
   * @brief 计算'blob'的一个切片的'volume'; 也就是在给定轴间范围内维度的乘积
   *
   * @param start_axis The first axis to include in the slice.
   * @param end_axis The first axis to exclude from the slice.
   */
  inline int count(int start_axis, int end_axis) const {
    CHECK_LE(start_axis, end_axis);
    CHECK_GE(start_axis, 0);
    CHECK_GE(end_axis, 0);
    CHECK_LE(start_axis, num_axes());
    CHECK_LE(end_axis, num_axes());
    int count = 1;
    for (int i = start_axis; i < end_axis; ++i) {
      count *= shape(i);
    }
    return count;
  }
  /**
   * @brief Compute the volume of a slice spanning from a particular first
   *        axis to the final axis.
   *
   * @param start_axis The first axis to include in the slice.
   */
  // 获取某一维到结束数据的大小  
  inline int count(int start_axis) const {  
    return count(start_axis, num_axes());
  }

  /**
   * @brief Returns the 'canonical' version of a (usually) user-specified axis,
   *        allowing for negative indexing (e.g., -1 for the last axis).
   *
   * @param axis_index the axis index.
   *        If 0 <= index < num_axes(), return index.
   *        If -num_axes <= index <= -1, return (num_axes() - (-index)),
   *        e.g., the last axis index (num_axes() - 1) if index == -1,
   *        the second to last if index == -2, etc.
   *        Dies on out of range index.
   */
  // 主要是对负数的索引进行正确转换
  inline int CanonicalAxisIndex(int axis_index) const {
    CHECK_GE(axis_index, -num_axes())
        << "axis " << axis_index << " out of range for " << num_axes()
        << "-D Blob with shape " << shape_string();
    CHECK_LT(axis_index, num_axes())
        << "axis " << axis_index << " out of range for " << num_axes()
        << "-D Blob with shape " << shape_string();
    if (axis_index < 0) {
      return axis_index + num_axes();
    }
    return axis_index;
  }
  /// 以下是'Blob'中的4个基本变量num,channel,height,width
  /// 可以直接通过'shape(0)','shape(1)','shape(2)','shape(3)'来访问  
  ///
  /// @brief Deprecated legacy shape accessor num: use shape(0) instead.
  inline int num() const { return LegacyShape(0); }
  /// @brief Deprecated legacy shape accessor channels: use shape(1) instead.
  inline int channels() const { return LegacyShape(1); }
  /// @brief Deprecated legacy shape accessor height: use shape(2) instead.
  inline int height() const { return LegacyShape(2); }
  /// @brief Deprecated legacy shape accessor width: use shape(3) instead.
  inline int width() const { return LegacyShape(3); }
  /// data_维数不大于4时才能使用，功能同shape()类似。
  inline int LegacyShape(int index) const {
    CHECK_LE(num_axes(), 4)
        << "Cannot use legacy accessors on Blobs with > 4 axes.";
    CHECK_LT(index, 4);
    CHECK_GE(index, -4);
    if (index >= num_axes() || index < -num_axes()) {
      // Axis is out of range, but still in [0, 3] (or [-4, -1] for reverse
      // indexing) -- this special case simulates the one-padding used to fill
      // extraneous axes of legacy blobs.
      return 1;
    }
    return shape(index);
  }

  /// offset计算的方式也支持两种方式。一种是指定'n,c,h,w'，另一种是用'vector'指定   
  /// 'n,c,h,w'对应的偏移量'offset'=((n*channels()+c)*height()+h)*width()+w  
  inline int offset(const int n, const int c = 0, const int h = 0,
      const int w = 0) const {
    CHECK_GE(n, 0);
    CHECK_LE(n, num());
    CHECK_GE(channels(), 0);
    CHECK_LE(c, channels());
    CHECK_GE(height(), 0);
    CHECK_LE(h, height());
    CHECK_GE(width(), 0);
    CHECK_LE(w, width());
    return ((n * channels() + c) * height() + h) * width() + w;
  }

  inline int offset(const vector<int>& indices) const {
    CHECK_LE(indices.size(), num_axes());
    int offset = 0;
    for (int i = 0; i < num_axes(); ++i) {
      offset *= shape(i);
      if (indices.size() > i) {
        CHECK_GE(indices[i], 0);
        CHECK_LT(indices[i], shape(i));
        offset += indices[i];
      }
    }
    return offset;
  }
  /**
   * @brief Copy from a source Blob.目的地是'gpu_data'或'cpu_data'
   *
   * @param source the Blob to copy from
   * @param copy_diff if false, copy the data; if true, copy the diff
   * @param reshape if false, require this Blob to be pre-shaped to the shape
   *        of other (and die otherwise); if true, Reshape this Blob to other's
   *        shape if necessary
   */
  void CopyFrom(const Blob<Dtype>& source, bool copy_diff = false,
      bool reshape = false);

  /**
   * 这一部分函数主要通过给定的位置访问数据，根据位置计算与数据起始地址之间的 
   * 的偏差'offset'，在通过'cpu_data*'指针获得地址 
   */  
  inline Dtype data_at(const int n, const int c, const int h,
      const int w) const {
    return cpu_data()[offset(n, c, h, w)];
  }

  inline Dtype diff_at(const int n, const int c, const int h,
      const int w) const {
    return cpu_diff()[offset(n, c, h, w)];
  }

  inline Dtype data_at(const vector<int>& index) const {
    return cpu_data()[offset(index)];
  }

  inline Dtype diff_at(const vector<int>& index) const {
    return cpu_diff()[offset(index)];
  }
  /// 获取data_ 
  inline const shared_ptr<SyncedMemory>& data() const {
    CHECK(data_);
    return data_;
  }
  /// 获取diff_  
  inline const shared_ptr<SyncedMemory>& diff() const {
    CHECK(diff_);
    return diff_;
  }

  // 这里有'data'和'diff'两类数据，而这个'diff'就是我们所熟知的偏差，前者主要存储  
  // 前向传递的数据，而后者存储的是反向传播中的梯度  
  const Dtype* cpu_data() const;
  void set_cpu_data(Dtype* data);
  const int* gpu_shape() const;
  const Dtype* gpu_data() const;
  void set_gpu_data(Dtype* data);
  const Dtype* cpu_diff() const;
  const Dtype* gpu_diff() const;
  Dtype* mutable_cpu_data();   // mutable是可读写访问  
  Dtype* mutable_gpu_data();
  Dtype* mutable_cpu_diff();
  Dtype* mutable_gpu_diff();
  void Update();  // 即 data_ - diff_;
  void FromProto(const BlobProto& proto, bool reshape = true); // 从proto中恢复一个blob对象 
  void ToProto(BlobProto* proto, bool write_diff = false) const; // 将blob序列化为proto

  /// @brief Compute the sum of absolute values (L1 norm) of the data.
  Dtype asum_data() const;
  /// @brief Compute the sum of absolute values (L1 norm) of the diff.
  Dtype asum_diff() const;
  /// @brief Compute the sum of squares (L2 norm squared) of the data.
  Dtype sumsq_data() const;
  /// @brief Compute the sum of squares (L2 norm squared) of the diff.
  Dtype sumsq_diff() const;

  /// @brief Scale the blob data by a constant factor.
  void scale_data(Dtype scale_factor);
  /// @brief Scale the blob diff by a constant factor.
  void scale_diff(Dtype scale_factor);

  /**
   * @brief Set the data_ shared_ptr to point to the SyncedMemory holding the
   *        data_ of Blob other -- useful in Layer%s which simply perform a copy
   *        in their Forward pass.
   *
   * This deallocates the SyncedMemory holding this Blob's data_, as
   * shared_ptr calls its destructor when reset with the "=" operator.
   */
  void ShareData(const Blob& other);  // 本Blob共享other的data_  
  /**
   * @brief Set the diff_ shared_ptr to point to the SyncedMemory holding the
   *        diff_ of Blob other -- useful in Layer%s which simply perform a copy
   *        in their Forward pass.
   *
   * This deallocates the SyncedMemory holding this Blob's diff_, as
   * shared_ptr calls its destructor when reset with the "=" operator.
   */
  void ShareDiff(const Blob& other); // 本Blob共享other的diff_  

  bool ShapeEquals(const BlobProto& other);

 protected:
  /// 'data_'是正向传播数据的指针，指针类型是'shared_ptr'，属于'boost'库的一个智能指针，
  /// 这一部分主要用来申请内存存储data，data主要是正向传播的时候用的 
  shared_ptr<SyncedMemory> data_;
  shared_ptr<SyncedMemory> diff_;
  shared_ptr<SyncedMemory> shape_data_;  
  vector<int> shape_; // 'shape_'存储Blob的形状  
  int count_;    // Blob中的元素个数，也就是'n*c*h*w'  
  int capacity_; // 'capacity'表示当前的元素个数，因为'Blob'可能会'reshape'  

  DISABLE_COPY_AND_ASSIGN(Blob);  //禁止拷贝和赋值运算  
};  // class Blob

}  // namespace caffe

#endif  // CAFFE_BLOB_HPP_
```

## 顺便将实现部分也贴出来         
```cpp
#include <climits>  
#include <vector>  
  
#include "caffe/blob.hpp"  
#include "caffe/common.hpp"  
#include "caffe/syncedmem.hpp"  
#include "caffe/util/math_functions.hpp"  
  
namespace caffe {  
  
template <typename Dtype>  
//该函数将num,channels,height,width传递给vector shape_   
void Blob<Dtype>::Reshape(const int num, const int channels, const int height,  
    const int width) {  
  vector<int> shape(4);  
  shape[0] = num;  
  shape[1] = channels;  
  shape[2] = height;  
  shape[3] = width;  
  Reshape(shape);  
}  
  
template <typename Dtype>  
void Blob<Dtype>::Reshape(const vector<int>& shape) {  
  CHECK_LE(shape.size(), kMaxBlobAxes);  
  count_ = 1;  
  shape_.resize(shape.size());//重新定义vector shape_ 的size  
  for (int i = 0; i < shape.size(); ++i) {  
    CHECK_GE(shape[i], 0);//确保shape 每个元素为正数  
    CHECK_LE(shape[i], INT_MAX / count_) << "blob size exceeds INT_MAX";  
    count_ *= shape[i];  
    shape_[i] = shape[i];  
  }  
  //由于count_超过了当前capacity_ 因此需要重新分配内存空间  
  if (count_ > capacity_) {  
    capacity_ = count_;  
    data_.reset(new SyncedMemory(capacity_ * sizeof(Dtype)));  
    diff_.reset(new SyncedMemory(capacity_ * sizeof(Dtype)));  
  }  
}  
  
template <typename Dtype>// BlobShape 在caffe.proto 中定义  
void Blob<Dtype>::Reshape(const BlobShape& shape) {  
  CHECK_LE(shape.dim_size(), kMaxBlobAxes);  
  vector<int> shape_vec(shape.dim_size());  
  for (int i = 0; i < shape.dim_size(); ++i) {  
    shape_vec[i] = shape.dim(i);//dim 包含num，channels，height， width  
  }  
  Reshape(shape_vec);//用protobuf传递来dim 对shape_ 进行reshape  
}  
//用已知的Blob的shape来对shape_ 进行reshape  
template <typename Dtype>  
void Blob<Dtype>::ReshapeLike(const Blob<Dtype>& other) {  
  Reshape(other.shape());  
}  
//用num，channels，height， width 初始化  
template <typename Dtype>  
Blob<Dtype>::Blob(const int num, const int channels, const int height,  
    const int width)  
  // capacity_ must be initialized before calling Reshape  
  : capacity_(0) {  
  Reshape(num, channels, height, width);  
}  
//用shape 初始化  
template <typename Dtype>  
Blob<Dtype>::Blob(const vector<int>& shape)  
  // capacity_ must be initialized before calling Reshape  
  : capacity_(0) {  
  Reshape(shape);  
}  
//返回cpu 中的数据  
template <typename Dtype>  
const Dtype* Blob<Dtype>::cpu_data() const {  
  CHECK(data_);  
  return (const Dtype*)data_->cpu_data();  
}  
// 清空cpu 数据  
template <typename Dtype>  
void Blob<Dtype>::set_cpu_data(Dtype* data) {  
  CHECK(data);  
  data_->set_cpu_data(data);  
}  
//返回gpu 中的数据  
template <typename Dtype>  
const Dtype* Blob<Dtype>::gpu_data() const {  
  CHECK(data_);  
  return (const Dtype*)data_->gpu_data();  
}  
//反向传播导数diff_ 操作函数,返回cpu 中的数据  
template <typename Dtype>  
const Dtype* Blob<Dtype>::cpu_diff() const {  
  CHECK(diff_);  
  return (const Dtype*)diff_->cpu_data();  
}  
//返回gpu 中的数据  
template <typename Dtype>  
const Dtype* Blob<Dtype>::gpu_diff() const {  
  CHECK(diff_);  
  return (const Dtype*)diff_->gpu_data();  
}  
  
template <typename Dtype>  
Dtype* Blob<Dtype>::mutable_cpu_data() {  
  CHECK(data_);  
  return static_cast<Dtype*>(data_->mutable_cpu_data());  
}  
  
template <typename Dtype>  
Dtype* Blob<Dtype>::mutable_gpu_data() {  
  CHECK(data_);  
  return static_cast<Dtype*>(data_->mutable_gpu_data());  
}  
  
template <typename Dtype>  
Dtype* Blob<Dtype>::mutable_cpu_diff() {  
  CHECK(diff_);  
  return static_cast<Dtype*>(diff_->mutable_cpu_data());  
}  
  
template <typename Dtype>  
Dtype* Blob<Dtype>::mutable_gpu_diff() {  
  CHECK(diff_);  
  return static_cast<Dtype*>(diff_->mutable_gpu_data());  
}  
//当前的blob 的data_ 指向已知blob的数据  
template <typename Dtype>  
void Blob<Dtype>::ShareData(const Blob& other) {  
  CHECK_EQ(count_, other.count());  
  data_ = other.data();  
}  
//当前的blob 的diff_ 指向已知blob的反向传播导数  
template <typename Dtype>  
void Blob<Dtype>::ShareDiff(const Blob& other) {  
  CHECK_EQ(count_, other.count());  
  diff_ = other.diff();  
}  
  
// The "update" method is used for parameter blobs in a Net, which are stored  
// as Blob<float> or Blob<double> -- hence we do not define it for  
// Blob<int> or Blob<unsigned int>.  
template <> void Blob<unsigned int>::Update() { NOT_IMPLEMENTED; }  
template <> void Blob<int>::Update() { NOT_IMPLEMENTED; }  
//Updata函数用于参数blob的更新（weight，bias 等减去对应的导数）  
template <typename Dtype>  
void Blob<Dtype>::Update() {  
  // We will perform update based on where the data is located.  
  switch (data_->head()) {  
  case SyncedMemory::HEAD_AT_CPU://数据在cpu上，则在cpu上进行计算  
    // perform computation on CPU  
    caffe_axpy<Dtype>(count_, Dtype(-1),  
        static_cast<const Dtype*>(diff_->cpu_data()),  
        static_cast<Dtype*>(data_->mutable_cpu_data()));  
    break;  
  case SyncedMemory::HEAD_AT_GPU:  
  case SyncedMemory::SYNCED:  
#ifndef CPU_ONLY//如果没有定义CPU_ONLY，且数据在gpu上，则在gpu上进行计算  
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
  
template <> unsigned int Blob<unsigned int>::asum_data() const {  
  NOT_IMPLEMENTED;  
  return 0;  
}  
  
template <> int Blob<int>::asum_data() const {  
  NOT_IMPLEMENTED;  
  return 0;  
}  
//返回data_ 中所有 element 的绝对值之和  
template <typename Dtype>  
Dtype Blob<Dtype>::asum_data() const {  
  if (!data_) { return 0; }  
  switch (data_->head()) {  
  case SyncedMemory::HEAD_AT_CPU:  
    return caffe_cpu_asum(count_, cpu_data());  
  case SyncedMemory::HEAD_AT_GPU:  
  case SyncedMemory::SYNCED:  
#ifndef CPU_ONLY  
  {  
    Dtype asum;  
    caffe_gpu_asum(count_, gpu_data(), &asum);  
    return asum;  
  }  
#else  
    NO_GPU;  
#endif  
  case SyncedMemory::UNINITIALIZED:  
    return 0;  
  default:  
    LOG(FATAL) << "Unknown SyncedMemory head state: " << data_->head();  
  }  
  return 0;  
}  
  
template <> unsigned int Blob<unsigned int>::asum_diff() const {  
  NOT_IMPLEMENTED;  
  return 0;  
}  
  
template <> int Blob<int>::asum_diff() const {  
  NOT_IMPLEMENTED;  
  return 0;  
}  
//返回diff_ 中所有 element 的绝对值之和  
template <typename Dtype>  
Dtype Blob<Dtype>::asum_diff() const {  
  if (!diff_) { return 0; }  
  switch (diff_->head()) {  
  case SyncedMemory::HEAD_AT_CPU:  
    return caffe_cpu_asum(count_, cpu_diff());  
  case SyncedMemory::HEAD_AT_GPU:  
  case SyncedMemory::SYNCED:  
#ifndef CPU_ONLY  
  {  
    Dtype asum;  
    caffe_gpu_asum(count_, gpu_diff(), &asum);  
    return asum;  
  }  
#else  
    NO_GPU;  
#endif  
  case SyncedMemory::UNINITIALIZED:  
    return 0;  
  default:  
    LOG(FATAL) << "Unknown SyncedMemory head state: " << diff_->head();  
  }  
  return 0;  
}  
  
template <> unsigned int Blob<unsigned int>::sumsq_data() const {  
  NOT_IMPLEMENTED;  
  return 0;  
}  
  
template <> int Blob<int>::sumsq_data() const {  
  NOT_IMPLEMENTED;  
  return 0;  
}  
//返回 data_ 中所有 element 的平方和  
template <typename Dtype>  
Dtype Blob<Dtype>::sumsq_data() const {  
  Dtype sumsq;  
  const Dtype* data;  
  if (!data_) { return 0; }  
  switch (data_->head()) {  
  case SyncedMemory::HEAD_AT_CPU:  
    data = cpu_data();  
    sumsq = caffe_cpu_dot(count_, data, data);  
    break;  
  case SyncedMemory::HEAD_AT_GPU:  
  case SyncedMemory::SYNCED:  
#ifndef CPU_ONLY  
    data = gpu_data();  
    caffe_gpu_dot(count_, data, data, &sumsq);  
#else  
    NO_GPU;  
#endif  
    break;  
  case SyncedMemory::UNINITIALIZED:  
    return 0;  
  default:  
    LOG(FATAL) << "Unknown SyncedMemory head state: " << data_->head();  
  }  
  return sumsq;  
}  
  
template <> unsigned int Blob<unsigned int>::sumsq_diff() const {  
  NOT_IMPLEMENTED;  
  return 0;  
}  
  
template <> int Blob<int>::sumsq_diff() const {  
  NOT_IMPLEMENTED;  
  return 0;  
}  
//返回 diff_ 中所有 element 的平方和  
template <typename Dtype>  
Dtype Blob<Dtype>::sumsq_diff() const {  
  Dtype sumsq;  
  const Dtype* diff;  
  if (!diff_) { return 0; }  
  switch (diff_->head()) {  
  case SyncedMemory::HEAD_AT_CPU:  
    diff = cpu_diff();  
    sumsq = caffe_cpu_dot(count_, diff, diff);  
    break;  
  case SyncedMemory::HEAD_AT_GPU:  
  case SyncedMemory::SYNCED:  
#ifndef CPU_ONLY  
    diff = gpu_diff();  
    caffe_gpu_dot(count_, diff, diff, &sumsq);  
    break;  
#else  
    NO_GPU;  
#endif  
  case SyncedMemory::UNINITIALIZED:  
    return 0;  
  default:  
    LOG(FATAL) << "Unknown SyncedMemory head state: " << data_->head();  
  }  
  return sumsq;  
}  
  
template <> void Blob<unsigned int>::scale_data(unsigned int scale_factor) {  
  NOT_IMPLEMENTED;  
}  
  
template <> void Blob<int>::scale_data(int scale_factor) {  
  NOT_IMPLEMENTED;  
}  
// 给data乘以scale_factor  
template <typename Dtype>  
void Blob<Dtype>::scale_data(Dtype scale_factor) {  
  Dtype* data;  
  if (!data_) { return; }  
  switch (data_->head()) {  
  case SyncedMemory::HEAD_AT_CPU:  
    data = mutable_cpu_data();  
    caffe_scal(count_, scale_factor, data);  
    return;  
  case SyncedMemory::HEAD_AT_GPU:  
  case SyncedMemory::SYNCED:  
#ifndef CPU_ONLY  
    data = mutable_gpu_data();  
    caffe_gpu_scal(count_, scale_factor, data);  
    return;  
#else  
    NO_GPU;  
#endif  
  case SyncedMemory::UNINITIALIZED:  
    return;  
  default:  
    LOG(FATAL) << "Unknown SyncedMemory head state: " << data_->head();  
  }  
}  
  
template <> void Blob<unsigned int>::scale_diff(unsigned int scale_factor) {  
  NOT_IMPLEMENTED;  
}  
  
template <> void Blob<int>::scale_diff(int scale_factor) {  
  NOT_IMPLEMENTED;  
}  
// 给diff乘以scale_factor  
template <typename Dtype>  
void Blob<Dtype>::scale_diff(Dtype scale_factor) {  
  Dtype* diff;  
  if (!diff_) { return; }  
  switch (diff_->head()) {  
  case SyncedMemory::HEAD_AT_CPU:  
    diff = mutable_cpu_diff();  
    caffe_scal(count_, scale_factor, diff);  
    return;  
  case SyncedMemory::HEAD_AT_GPU:  
  case SyncedMemory::SYNCED:  
#ifndef CPU_ONLY  
    diff = mutable_gpu_diff();  
    caffe_gpu_scal(count_, scale_factor, diff);  
    return;  
#else  
    NO_GPU;  
#endif  
  case SyncedMemory::UNINITIALIZED:  
    return;  
  default:  
    LOG(FATAL) << "Unknown SyncedMemory head state: " << diff_->head();  
  }  
}  
//BlobProto 是定义在caffe.proto 中的一个message，其字段有 data,diff,shape,num,channels,height,width  
template <typename Dtype>  
bool Blob<Dtype>::ShapeEquals(const BlobProto& other) {  
  if (other.has_num() || other.has_channels() ||  
      other.has_height() || other.has_width()) {  
    // Using deprecated 4D Blob dimensions --  
    // shape is (num, channels, height, width).  
    // Note: we do not use the normal Blob::num(), Blob::channels(), etc.  
    // methods as these index from the beginning of the blob shape, where legacy  
    // parameter blobs were indexed from the end of the blob shape (e.g., bias  
    // Blob shape (1 x 1 x 1 x N), IP layer weight Blob shape (1 x 1 x M x N)).  
    return shape_.size() <= 4 &&  
           LegacyShape(-4) == other.num() &&  
           LegacyShape(-3) == other.channels() &&  
           LegacyShape(-2) == other.height() &&  
           LegacyShape(-1) == other.width();  
  }  
  vector<int> other_shape(other.shape().dim_size());  
  for (int i = 0; i < other.shape().dim_size(); ++i) {  
    other_shape[i] = other.shape().dim(i);  
  }  
  return shape_ == other_shape;  
}//检查当前的blob和已知的 other 的 shape 是否相同，相同返回true  
  
template <typename Dtype>  
void Blob<Dtype>::CopyFrom(const Blob& source, bool copy_diff, bool reshape) {  
  if (source.count() != count_ || source.shape() != shape_) {  
    if (reshape) {  
      ReshapeLike(source);  
    } else {  
      LOG(FATAL) << "Trying to copy blobs of different sizes.";  
    }  
  }  
  switch (Caffe::mode()) {  
  case Caffe::GPU:  
    if (copy_diff) {  
      caffe_copy(count_, source.gpu_diff(),  
          static_cast<Dtype*>(diff_->mutable_gpu_data()));  
    } else {  
      caffe_copy(count_, source.gpu_data(),  
          static_cast<Dtype*>(data_->mutable_gpu_data()));  
    }  
    break;  
  case Caffe::CPU:  
    if (copy_diff) {  
      caffe_copy(count_, source.cpu_diff(),  
          static_cast<Dtype*>(diff_->mutable_cpu_data()));  
    } else {  
      caffe_copy(count_, source.cpu_data(),  
          static_cast<Dtype*>(data_->mutable_cpu_data()));  
    }  
    break;  
  default:  
    LOG(FATAL) << "Unknown caffe mode.";  
  }  
}//从source 拷贝数据,copy_diff控制是拷贝diff还是data  
  
template <typename Dtype>  
void Blob<Dtype>::FromProto(const BlobProto& proto, bool reshape) {  
  if (reshape) {  
    vector<int> shape;  
    if (proto.has_num() || proto.has_channels() ||  
        proto.has_height() || proto.has_width()) {  
      // Using deprecated 4D Blob dimensions --  
      // shape is (num, channels, height, width).  
      shape.resize(4);  
      shape[0] = proto.num();  
      shape[1] = proto.channels();  
      shape[2] = proto.height();  
      shape[3] = proto.width();  
    } else {  
      shape.resize(proto.shape().dim_size());  
      for (int i = 0; i < proto.shape().dim_size(); ++i) {  
        shape[i] = proto.shape().dim(i);  
      }  
    }  
    Reshape(shape);  
  } else {//如果不做reshape要求当前的blob的shape和proto传入的shape相同  
    CHECK(ShapeEquals(proto)) << "shape mismatch (reshape not set)";  
  }  
  // copy data  
  Dtype* data_vec = mutable_cpu_data();  
  for (int i = 0; i < count_; ++i) {  
    data_vec[i] = proto.data(i);  
  }//将proto传入的data拷贝到cpu数据  
  if (proto.diff_size() > 0) {  
    Dtype* diff_vec = mutable_cpu_diff();  
    for (int i = 0; i < count_; ++i) {  
      diff_vec[i] = proto.diff(i);  
    }//将proto传入的diff 拷贝到cpu数据  
  }  
}  
  
template <typename Dtype>  
void Blob<Dtype>::ToProto(BlobProto* proto, bool write_diff) const {  
  proto->clear_shape();  
  for (int i = 0; i < shape_.size(); ++i) {  
    proto->mutable_shape()->add_dim(shape_[i]);  
  }  
  proto->clear_data();  
  proto->clear_diff();  
  const Dtype* data_vec = cpu_data();  
  for (int i = 0; i < count_; ++i) {  
    proto->add_data(data_vec[i]);//将data写入proto  
  }  
  if (write_diff) {  
    const Dtype* diff_vec = cpu_diff();  
    for (int i = 0; i < count_; ++i) {  
      proto->add_diff(diff_vec[i]);//将diff写入proto  
    }  
  }  
}  
  
INSTANTIATE_CLASS(Blob);  
template class Blob<int>;  
template class Blob<unsigned int>;  
  
}  // namespace caffe  
```

