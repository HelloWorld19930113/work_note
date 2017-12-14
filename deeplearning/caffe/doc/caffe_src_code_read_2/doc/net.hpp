#ifndef CAFFE_NET_HPP_
#define CAFFE_NET_HPP_

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/layer.hpp"
#include "caffe/proto/caffe.pb.h"

namespace caffe {

/**
 * @brief Connects Layer%s together into a directed acyclic graph (DAG)
 *        specified by a NetParameter.
 *
 * TODO(dox): more thorough description.
 */
template <typename Dtype>
class Net {
 public:
  /** 
   * @brief 调用 Init 函数初始化网络 
   * @param string& param_file 
   */  
  explicit Net(const NetParameter& param);
  explicit Net(const string& param_file, Phase phase,
      const int level = 0, const vector<string>* stages = NULL);
  virtual ~Net() {}

  /** 
   * brief Initialize a network with a NetParameter.
   * 输入：NetParameter& in_param 
   * 步骤： 
   * <1> 调用InsertSplits()函数从 in_param 读入新网络到 param 
   * <2> 构建所有的层并设置其连接关系
          定义name_，blob_name_to_idx，available_blobs，num_layers 
   * <3> 
   * <4> 对每一个输入层的blob： 
   *     产生一块和当前blob一样大的空间 e.g. input_dim=[12 55 66 39 20 24 48 64]表示第一个blob的四个维数为 12 55 66 39，第二个为 20 24 48 64 接着blob_pointer指向这块空间 
   *     blob_pointer压到blobs_中 vector<shared_ptr<Blob<Dtype>>> blobs_ 
   *     blob_name压到blob_names_中 vector<string> blob_names_ 
   *     param.force_backward()压到blob_need_backward_中vector<bool> blob_need_backward_ 
   *     i 压到 net_input_blob_indices_中 net_input_blob_indices_ -> vector 
   *     blob_pointer.get() 压到 net_input_blobs_中 
   *     注意与blobs_的区别 
   *     vector<shared_ptr<Blob<Dtype>>> blobs_ 
   *     vector<Blob<Dtype>*> net_input_blobs_ 
   *     shared_ptr类型的参数调用.get()则得到Blob*类型 
   *     map<string, int> blob_name_to_idx 
   *     初始化为输入层的每个blob的名字 set<string> available_blobs 
   *     计算所需内存 memory_used += blob_pointer->count() 
   *  
   * <5> 用网络的层数 layer_.size() 去初始化 bottom_vecs_, bottom_id_vecs_, 
   *     bottom_need_backward_， param_id_vecs_, top_vecs_ 和 top_id_vecs_ 
   * <6> 对第i层（很大的一个for循环）： 
   *     param.layers(i)返回的是关于第当前层的参数： 
   *     layer_param = param.layers(i) 
   *     把当前层的参数转换为shared_ptr<Layer<Dtype>>，并压入到layers_中 
   *     把当前层的名字压入到layer_names_：vector<string> layer_names_ 
   *     判断当前层是否需要反馈 need_backward = param.force_backward() 
   *  
   *     下面开始产生当前层：分为处理bottom的blob和top的blob两个步骤 
   *     对第j个bottom的blob： 
   *         layer_param.bottom_size()存的是当前层的输入blob数量 
   *         layer_param.bottom(j)存的是第j个输入blob的名字 
   *         读取当前blob的id，其中blob_name_to_idx在输入层初始化过了 
   *         blob_name_to_idx[blob_name] = i 
   *         输出当前blob的名字 
   *         存入第j个输入blob的指针bottom_vecs_[i].push_back(blobs_[blob_id].get()) 
   *         存入第j个输入blob的id bottom_id_vecs_[i].push_back(blob_id) 
   *         更新need_backward 
   *         从available_blobs中删除第j个blob的名字 
   *  
   *     对第j个top的blob： 
   *         layer_param.top_size()存的是当前层的输出blob数量 
   *         layer_param.top(j)存的是第j个输出blob的名字 
   *         判断是否进行同址计算 
   *         输出当前blob的名字 
   *         定义一块新的blob空间，用blob_pointer指向这块空间 
   *         把这个指针存入到blobs_中 
   *         把blob_name、force_backward、idx存入对应的容器中 
   *         向available_blobs插入当前blob的名字 
   *         top_vecs_[i]对于第i层，插入当前blob的指针 
   *         top_id_vecs_[i]对于第i层，插入当前blob的id 
   *     输出当前层位于top的blob的信息 
   *     计算所需内存 
   *     判断当前层i是否需要backward 
   *  
   * <7> 所有名字在available_blobs中的blob为当前层的输出blob，存入net_output_blobs_中 
   * <8> 建立每个blob的name和index的对应关系map：blob_names_index_ 
   * <9> 建立每个层的name和index的对应关系map：layer_names_index_ 
   * <10> 调用GetLearningRateAndWeightDecay函数 
   * 
   */  
  void Init(const NetParameter& param);

  /**
   * @brief Run Forward and return the result.
   *
   */
  const vector<Blob<Dtype>*>& Forward(Dtype* loss = NULL);
  /// @brief DEPRECATED; use Forward() instead.
  const vector<Blob<Dtype>*>& ForwardPrefilled(Dtype* loss = NULL) {
    LOG_EVERY_N(WARNING, 1000) << "DEPRECATED: ForwardPrefilled() "
        << "will be removed in a future version. Use Forward().";
    return Forward(loss);
  }

  /**
   * The From and To variants of Forward and Backward operate on the
   * (topological) ordering by which the net is specified. For general DAG
   * networks, note that (1) computing from one layer to another might entail
   * extra computation on unrelated branches, and (2) computation starting in
   * the middle may be incorrect if all of the layers of a fan-in are not
   * included.
   */
  Dtype ForwardFromTo(int start, int end);
  Dtype ForwardFrom(int start);
  Dtype ForwardTo(int end);
  /// @brief DEPRECATED; set input blobs then use Forward() instead.
  const vector<Blob<Dtype>*>& Forward(const vector<Blob<Dtype>* > & bottom,
      Dtype* loss = NULL);

  /**
   * @brief Zeroes out the diffs of all net parameters.
   *        Should be run before Backward.
   */
  void ClearParamDiffs();

  /**
   * The network backward should take no input and output, since it solely
   * computes the gradient w.r.t the parameters, and the data has already been
   * provided during the forward pass.
   */
  void Backward();
  void BackwardFromTo(int start, int end);
  void BackwardFrom(int start);
  void BackwardTo(int end);

  /**
   * @brief Reshape all layers from bottom to top.
   *
   * This is useful to propagate changes to layer sizes without running
   * a forward pass, e.g. to compute output feature size.
   */
  void Reshape();

  Dtype ForwardBackward() {
    Dtype loss;
    Forward(&loss);
    Backward();
    return loss;
  }

  /// @brief Updates the network weights based on the diff values computed.
  void Update();
  /**
   * @brief Shares weight data of owner blobs with shared blobs.
   *
   * Note: this is called by Net::Init, and thus should normally not be
   * called manually.
   */
  void ShareWeights();

  /**
   * @brief For an already initialized net, implicitly copies (i.e., using no
   *        additional memory) the pre-trained layers from another Net.
   */
  void ShareTrainedLayersWith(const Net* other);
  // For an already initialized net, CopyTrainedLayersFrom() copies the already
  // trained layers from another net parameter instance.
  /**
   * @brief For an already initialized net, copies the pre-trained layers from
   *        another Net.
   */
  void CopyTrainedLayersFrom(const NetParameter& param);
  void CopyTrainedLayersFrom(const string trained_filename);
  void CopyTrainedLayersFromBinaryProto(const string trained_filename);
  void CopyTrainedLayersFromHDF5(const string trained_filename);
  /// @brief Writes the net to a proto.
  void ToProto(NetParameter* param, bool write_diff = false) const;
  /// @brief Writes the net to an HDF5 file.
  void ToHDF5(const string& filename, bool write_diff = false) const;

  /// @brief returns the network name.
  inline const string& name() const { return name_; }
  /// @brief returns the layer names
  inline const vector<string>& layer_names() const { return layer_names_; }
  /// @brief returns the blob names
  inline const vector<string>& blob_names() const { return blob_names_; }
  /// @brief returns the blobs
  inline const vector<shared_ptr<Blob<Dtype> > >& blobs() const {
    return blobs_;
  }
  /// @brief returns the layers
  inline const vector<shared_ptr<Layer<Dtype> > >& layers() const {
    return layers_;
  }
  /// @brief returns the phase: TRAIN or TEST
  inline Phase phase() const { return phase_; }
  /**
   * @brief returns the bottom vecs for each layer -- usually you won't
   *        need this unless you do per-layer checks such as gradients.
   */
  inline const vector<vector<Blob<Dtype>*> >& bottom_vecs() const {
    return bottom_vecs_;
  }
  /**
   * @brief returns the top vecs for each layer -- usually you won't
   *        need this unless you do per-layer checks such as gradients.
   */
  inline const vector<vector<Blob<Dtype>*> >& top_vecs() const {
    return top_vecs_;
  }
  /// @brief returns the ids of the top blobs of layer i
  inline const vector<int> & top_ids(int i) const {
    CHECK_GE(i, 0) << "Invalid layer id";
    CHECK_LT(i, top_id_vecs_.size()) << "Invalid layer id";
    return top_id_vecs_[i];
  }
  /// @brief returns the ids of the bottom blobs of layer i
  inline const vector<int> & bottom_ids(int i) const {
    CHECK_GE(i, 0) << "Invalid layer id";
    CHECK_LT(i, bottom_id_vecs_.size()) << "Invalid layer id";
    return bottom_id_vecs_[i];
  }
  inline const vector<vector<bool> >& bottom_need_backward() const {
    return bottom_need_backward_;
  }
  inline const vector<Dtype>& blob_loss_weights() const {
    return blob_loss_weights_;
  }
  inline const vector<bool>& layer_need_backward() const {
    return layer_need_backward_;
  }
  /// @brief returns the parameters
  inline const vector<shared_ptr<Blob<Dtype> > >& params() const {
    return params_;
  }
  inline const vector<Blob<Dtype>*>& learnable_params() const {
    return learnable_params_;
  }
  /// @brief returns the learnable parameter learning rate multipliers
  inline const vector<float>& params_lr() const { return params_lr_; }
  inline const vector<bool>& has_params_lr() const { return has_params_lr_; }
  /// @brief returns the learnable parameter decay multipliers
  inline const vector<float>& params_weight_decay() const {
    return params_weight_decay_;
  }
  inline const vector<bool>& has_params_decay() const {
    return has_params_decay_;
  }
  const map<string, int>& param_names_index() const {
    return param_names_index_;
  }
  inline const vector<int>& param_owners() const { return param_owners_; }
  inline const vector<string>& param_display_names() const {
    return param_display_names_;
  }
  /// @brief Input and output blob numbers
  inline int num_inputs() const { return net_input_blobs_.size(); }
  inline int num_outputs() const { return net_output_blobs_.size(); }
  inline const vector<Blob<Dtype>*>& input_blobs() const {
    return net_input_blobs_;
  }
  inline const vector<Blob<Dtype>*>& output_blobs() const {
    return net_output_blobs_;
  }
  inline const vector<int>& input_blob_indices() const {
    return net_input_blob_indices_;
  }
  inline const vector<int>& output_blob_indices() const {
    return net_output_blob_indices_;
  }
  bool has_blob(const string& blob_name) const;
  const shared_ptr<Blob<Dtype> > blob_by_name(const string& blob_name) const;
  bool has_layer(const string& layer_name) const;
  const shared_ptr<Layer<Dtype> > layer_by_name(const string& layer_name) const;

  void set_debug_info(const bool value) { debug_info_ = value; }

  // Helpers for Init.
  /**
   * @brief Remove layers that the user specified should be excluded given the current
   *        phase, level, and stage.
   */
  static void FilterNet(const NetParameter& param,
      NetParameter* param_filtered);
  /// @brief return whether NetState state meets NetStateRule rule
  static bool StateMeetsRule(const NetState& state, const NetStateRule& rule,
      const string& layer_name);

  // Invoked at specific points during an iteration
  class Callback {
   protected:
    virtual void run(int layer) = 0;

    template <typename T>
    friend class Net;
  };
  const vector<Callback*>& before_forward() const { return before_forward_; }
  void add_before_forward(Callback* value) {
    before_forward_.push_back(value);
  }
  const vector<Callback*>& after_forward() const { return after_forward_; }
  void add_after_forward(Callback* value) {
    after_forward_.push_back(value);
  }
  const vector<Callback*>& before_backward() const { return before_backward_; }
  void add_before_backward(Callback* value) {
    before_backward_.push_back(value);
  }
  const vector<Callback*>& after_backward() const { return after_backward_; }
  void add_after_backward(Callback* value) {
    after_backward_.push_back(value);
  }

 protected:
  // Helpers for Init.
  /// @brief Append a new top blob to the net.
  void AppendTop(const NetParameter& param, const int layer_id,
                 const int top_id, set<string>* available_blobs,
                 map<string, int>* blob_name_to_idx);
  /// @brief Append a new bottom blob to the net.
  int AppendBottom(const NetParameter& param, const int layer_id,
                   const int bottom_id, set<string>* available_blobs,
                   map<string, int>* blob_name_to_idx);
  /// @brief Append a new parameter blob to the net.
  void AppendParam(const NetParameter& param, const int layer_id,
                   const int param_id);

  /// @brief Helper for displaying debug info in Forward.
  void ForwardDebugInfo(const int layer_id);
  /// @brief Helper for displaying debug info in Backward.
  void BackwardDebugInfo(const int layer_id);
  /// @brief Helper for displaying debug info in Update.
  void UpdateDebugInfo(const int param_id);

  /// @brief The network name
  string name_;
  /// @brief The phase: TRAIN or TEST
  Phase phase_;
  /// @brief Individual layers in the net
  vector<shared_ptr<Layer<Dtype> > > layers_;
  vector<string> layer_names_;
  map<string, int> layer_names_index_;
  vector<bool> layer_need_backward_;
  /// @brief the blobs storing intermediate results between the layer.
  vector<shared_ptr<Blob<Dtype> > > blobs_;
  vector<string> blob_names_;
  map<string, int> blob_names_index_;
  vector<bool> blob_need_backward_;
  /// bottom_vecs stores the vectors containing the input for each layer.
  /// They don't actually host the blobs (blobs_ does), so we simply store
  /// pointers.
  vector<vector<Blob<Dtype>*> > bottom_vecs_;
  vector<vector<int> > bottom_id_vecs_;
  vector<vector<bool> > bottom_need_backward_;
  /// top_vecs stores the vectors containing the output for each layer
  vector<vector<Blob<Dtype>*> > top_vecs_;
  vector<vector<int> > top_id_vecs_;
  /// Vector of weight in the loss (or objective) function of each net blob,
  /// indexed by blob_id.
  vector<Dtype> blob_loss_weights_;
  vector<vector<int> > param_id_vecs_;
  vector<int> param_owners_;
  vector<string> param_display_names_;
  vector<pair<int, int> > param_layer_indices_;
  map<string, int> param_names_index_;
  /// blob indices for the input and the output of the net
  vector<int> net_input_blob_indices_;
  vector<int> net_output_blob_indices_;
  vector<Blob<Dtype>*> net_input_blobs_;
  vector<Blob<Dtype>*> net_output_blobs_;
  /// The parameters in the network.
  vector<shared_ptr<Blob<Dtype> > > params_;
  vector<Blob<Dtype>*> learnable_params_;
  /**
   * The mapping from params_ -> learnable_params_: we have
   * learnable_param_ids_.size() == params_.size(),
   * and learnable_params_[learnable_param_ids_[i]] == params_[i].get()
   * if and only if params_[i] is an "owner"; otherwise, params_[i] is a sharer
   * and learnable_params_[learnable_param_ids_[i]] gives its owner.
   */
  vector<int> learnable_param_ids_;
  /// the learning rate multipliers for learnable_params_
  vector<float> params_lr_;
  vector<bool> has_params_lr_;
  /// the weight decay multipliers for learnable_params_
  vector<float> params_weight_decay_;
  vector<bool> has_params_decay_;
  /// The bytes of memory used by this net
  size_t memory_used_;
  /// Whether to compute and display debug info for the net.
  bool debug_info_;
  // Callbacks
  vector<Callback*> before_forward_;
  vector<Callback*> after_forward_;
  vector<Callback*> before_backward_;
  vector<Callback*> after_backward_;

DISABLE_COPY_AND_ASSIGN(Net);
};


}  // namespace caffe

#endif  // CAFFE_NET_HPP_
