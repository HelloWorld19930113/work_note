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
 * @brief A wrapper around SyncedMemory holders serving as the basic
 *        computational unit through which Layer%s, Net%s, and Solver%s
 *        interact.
 *
 * TODO(dox): 和SyncedMemory的函数接口配合工作.
 */
template <typename Dtype>
class Blob {
 public:
  Blob()
       : data_(), diff_(), count_(0), capacity_(0) {}

  /// @brief 过时的; use <code>Blob(const vector<int>& shape)</code>.
  explicit Blob(const int num, const int channels, const int height,
      const int width);
  explicit Blob(const vector<int>& shape);

  /// @brief 过时的; use <code>Reshape(const vector<int>& shape)</code>.
  void Reshape(const int num, const int channels, const int height,
      const int width);
  /**
   * @brief 改变  blob 的维度, 如果需要的话会重新分配内存。
   *  
   * 何时调用这个函数: 1. 在创建一个初始化内存分配区时
   *                2. 在调整top blob的维度时
   * Note:
   * (1) 当reshaping一个输入blob和立即调用Net::Backward()时将会出错。在前向传播时， 
   * Net::Forward 和 Net::Reshape 都不会被调用。
   * (2) 当改变blob的大小时, 如果现有的内存不足将会重新分配，多余的内存将会被释放。 
   */
  void Reshape(const vector<int>& shape);
  void Reshape(const BlobShape& shape);
  void ReshapeLike(const Blob& other);

  /**
   * 将 shape 格式化为 “%d %d %d %d (%d)”  
   */
  inline string shape_string() const {
    ostringstream stream;
    for (int i = 0; i < shape_.size(); ++i) {
      stream << shape_[i] << " ";
    }
    stream << "(" << count_ << ")";
    return stream.str();
  }

  /**
   * 返回某一维的大小或是blob的维度大小数组。 
   */
  inline const vector<int>& shape() const { return shape_; }
  /**
   * @brief Returns the dimension of the index-th axis (or the negative index-th
   *        axis from the end, if index is negative).
   *
   * @param index the axis index, which may be negative as it will be
   *        "canonicalized" using CanonicalAxisIndex.
   *        Dies on out of range index.
   */
  inline int shape(int index) const {
    return shape_[CanonicalAxisIndex(index)];
  }
  inline int num_axes() const { return shape_.size(); }
  inline int count() const { return count_; }

  /**
   * @brief 计算一个slice的体积。以维度为区间进行slice; 也就是在 slice 区间内的维度大小的
   *        乘积.
   *
   * @param start_axis slice 区间内的起始维度.
   *
   * @param end_axis slice 区间内的结束维度.
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
   * @brief 计算指定维度到最后维度的slice的体积。
   *
   * @param start_axis slice 区间内的起始维度.
   */
  inline int count(int start_axis) const {
    return count(start_axis, num_axes());
  }

  /**
   * @brief 返回 '标准' axis,
   *        允许负值索引 (e.g., -1 得到的最后一个维度).
   *
   * @param axis_index the axis index.
   *        If 0 <= index < num_axes(), return index.
   *        If -num_axes <= index <= -1, return (num_axes() - (-index)),
   *        e.g., the last axis index (num_axes() - 1) if index == -1,
   *        the second to last if index == -2, etc.
   *        Dies on out of range index.
   */
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

  /// @brief 过时的 legacy shape accessor num: use shape(0) instead.
  inline int num() const { return LegacyShape(0); }
  /// @brief 过时的 legacy shape accessor channels: use shape(1) instead.
  inline int channels() const { return LegacyShape(1); }
  /// @brief 过时的 legacy shape accessor height: use shape(2) instead.
  inline int height() const { return LegacyShape(2); }
  /// @brief 过时的 legacy shape accessor width: use shape(3) instead.
  inline int width() const { return LegacyShape(3); }
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

  /**
   * @brief 根据4个维度的值计算偏移量。 
   *   
   * @param n batch 的偏移.
   * @param c 通道的偏移.
   * @param h height 的偏移.
   * @param w width 的偏移.
   */
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
   * @brief 从一个源地址拷贝数据到data_或diff_。 
   *
   * @param source 拷贝的数据源
   * @param copy_diff  如果为 false, 拷贝的是 data_; 
   *                   如果为 true, 拷贝的是 diff_
   * @param reshape    如果为 false, 说明数据是预先进行过 reshape 的
   *                   如果为 true, 必要的话进行 reshape
   */
  void CopyFrom(const Blob<Dtype>& source, bool copy_diff = false,
      bool reshape = false);

  // 获取某个偏移处的 data_ 值
  inline Dtype data_at(const int n, const int c, const int h,
      const int w) const {
    return cpu_data()[offset(n, c, h, w)];
  }
  // 获取某个偏移处的 diff_ 值
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

  inline const shared_ptr<SyncedMemory>& data() const {
    CHECK(data_);
    return data_;
  }

  inline const shared_ptr<SyncedMemory>& diff() const {
    CHECK(diff_);
    return diff_;
  }

  /**
   * 这里有 data_ 和 diff_ 两类数据，而这个 diff_ 就是我们所熟知的偏差; data_主要存储
   * 前向传递的数据，而 diff_ 存储的是反向传播中的梯度  
   *
   */
  const Dtype* cpu_data() const;
  void set_cpu_data(Dtype* data);
  const int* gpu_shape() const;
  const Dtype* gpu_data() const;
  void set_gpu_data(Dtype* data);
  const Dtype* cpu_diff() const;
  const Dtype* gpu_diff() const;

  // mutable 是可读写访问 
  Dtype* mutable_cpu_data();
  Dtype* mutable_gpu_data();
  Dtype* mutable_cpu_diff();
  Dtype* mutable_gpu_diff();

  // 更新 data_ 的数据,减去 diff_ 的数据，即：合并 data 和 diff。 
  void Update();
  /** 
   * 从proto中恢复一个blob对象的 data_ 和 diff_ 。  
   */  
  void FromProto(const BlobProto& proto, bool reshape = true);
  void ToProto(BlobProto* proto, bool write_diff = false) const;

  /* 
   * @brief 计算 data_ 的 L1 范数(数据的绝对值之和) 
   * 说明：其中用到了 math_function.hpp 中的函数 caffe_cpu_asum() 和 caffe_gpu_asum，
   *      实现的功能是对向量 X 求其每个元素绝对值的和，不同的是 X 分别在 cpu 和 gpu 中。 
   */  
  Dtype asum_data() const;
  Dtype asum_diff() const;
  
  /* 
   * @brief 计算 data_ 的 L2 范数(数据的平方和)  
   * 说明：用到了 math_function.hpp 中的 caffe_cpu_dot(), caffe_cpu_strided_dot(),
   *      caffe_gpu_dot(), caffe_gpu_strided_dot()。具体就是就向量 X 的平方和。 
   */  
  Dtype sumsq_data() const;
  Dtype sumsq_diff() const;

  /* 
   * @brief 正规化 data_ 。 
   * 说明：  用到 math_function.hpp 中的 caffe_scal() 和 caffe_gpu_scal() 函数，
   *        就是对向量 X 乘上一个 constant 因子。 
   */ 
  void scale_data(Dtype scale_factor);
  void scale_diff(Dtype scale_factor);

  /**
   * @brief 将 data_ 指针指向 SyncedMemory other 中保存着 data_ 的数据空间。这样在
   *        Forward pass 时 Layer%s 间执行简单拷贝的话会很方便。 
   *
   * 这个函数会释放保存着 data_ 的 SyncedMemory。当使用 `=` 赋值时， 
   * shared_ptr 会调用其析构函数。
   */
  void ShareData(const Blob& other);
  void ShareDiff(const Blob& other);

  bool ShapeEquals(const BlobProto& other);

 protected:
  // data_ 指针，data主要是正向传播的时候用的;指针类型是shared_ptr，属于boost库的一个智能指针  
  shared_ptr<SyncedMemory> data_;  
  // diff_ 主要用来存储偏差，update data
  shared_ptr<SyncedMemory> diff_;
  shared_ptr<SyncedMemory> shape_data_;
  // shape_ 存储Blob的形状  
  vector<int> shape_;
  // count_ 表示 Blob 占用的内存空间大小，也就是 n*n*h*w 的乘积  
  int count_;
  // capacity_ 表示当前的元素个数，因为Blob可能会reshape  
  int capacity_;

  DISABLE_COPY_AND_ASSIGN(Blob);
};  // class Blob

}  // namespace caffe

#endif  // CAFFE_BLOB_HPP_
