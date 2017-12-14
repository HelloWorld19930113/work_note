# net(四)

net 定义网络， 整个网络中含有很多 layers， net.cpp 负责计算整个网络在训练中的 forward , backward 过程， 即计算 forward/backward 时各 layer 的 gradient 。

## 1. 官网描述  
The forward and backward passes are the essential computations of a Net.  
Let’s consider a simple logistic regression classifier.The forward pass computes the output given the input for inference.In forward `Caffe` composes the computation of each layer to compute the “function” represented by the model.This pass goes from bottom to top.    
**Forward pass**  
The data `x` is passed through an inner product layer for g(x) then through a softmax for h(g(x)) and softmax loss to give fW(x).
The backward pass computes the gradient given the loss for learning.In backward Caffe reverse-composes the gradient of each layer to compute the gradient of the whole model by `automatic differentiation`.This is back-propagation.This pass goes from top to bottom.    
**Backward pass**    
The backward pass begins with the loss and computes the gradient with respect to the output ∂(fW)/∂(h). The gradient with respect to the rest of the model is computed layer-by-layer through the chain rule. Layers with parameters, like the `INNER_PRODUCT` layer, compute the gradient with respect to their parameters ∂(fW)/∂(Wip) during the backward step.  
These computations follow immediately from defining the model: `Caffe` plans and carries out the forward and backward passes for you.  

The `Net::Forward()` and `Net::Backward()` methods carry out the respective passes while `Layer::Forward()` and `Layer::Backward()` compute each step.   
Every layer type has `forward_{cpu,gpu}()` and `backward_{cpu,gpu}()` methods to compute its steps according to the mode of computation. A layer may only implement CPU or GPU mode due to constraints or convenience.
The Solver optimizes a model by first calling forward to yield the output and loss, then calling backward to generate the gradient of the model, and then incorporating the gradient into a weight update that attempts to minimize the loss. Division of labor between the Solver, Net, and Layer keep Caffe modular and open to development.   
**Note**
每一个层都有 `forward_{cpu,gpu}()` 和 `backward_{cpu,gpu}()` 函数。Solver, Net, 和 Layer 的分工让 Caffe 变得模块化，并且容易去开发。   


## 2. 带注释的头文件   
[net.hpp](net.hpp)   

SplitLayers 是什么？   

## 3. proto 定义
```proto
message NetParameter {
  optional string name = 1; // consider giving the network a name
  // DEPRECATED. See InputParameter. The input blobs to the network.
  repeated string input = 3;
  // DEPRECATED. See InputParameter. The shape of the input blobs.
  repeated BlobShape input_shape = 8;

  // 4D input dimensions -- deprecated.  Use "input_shape" instead.
  // If specified, for each input blob there should be four
  // values specifying the num, channels, height and width of the input blob.
  // Thus, there should be a total of (4 * #input) numbers.
  repeated int32 input_dim = 4;

  // Whether the network will force every layer to carry out backward operation.
  // If set False, then whether to carry out backward is determined
  // automatically according to the net structure and learning rates.
  optional bool force_backward = 5 [default = false];
  // The current "state" of the network, including the phase, level, and stage.
  // Some layers may be included/excluded depending on this state and the states
  // specified in the layers' include and exclude fields.
  optional NetState state = 6;

  // Print debugging information about results while running Net::Forward,
  // Net::Backward, and Net::Update.
  optional bool debug_info = 7 [default = false];

  // The layers that make up the net.  Each of their configurations, including
  // connectivity and behavior, is specified as a LayerParameter.
  repeated LayerParameter layer = 100;  // ID 100 so layers are printed last.

  // DEPRECATED: use 'layer' instead.
  repeated V1LayerParameter layers = 2;
}
```   
重要的成员：     
layer_.size() 就是网络中的所有的层。   
input_.size() 输入层blob的个数;   
input_[i]     表示第i个blob的名字;   
input_dim     表示 4D 输入数据的维度。如 input_dim=[12 55 66 39 20 24 48 64] 表示第一个blob的四个维数为 12 55 66 39，第二个为 20 24 48 64。  


## 4. 成员变量   
```cpp
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

	// bottom_vecs 中保存每层的输入向量，向量中只是保存指针，不是实际的存储空间。   
	vector<vector<Blob<Dtype>*> > bottom_vecs_;
	vector<vector<int> > bottom_id_vecs_;   // 存每一层输入(bottom)的id
	vector<vector<bool> > bottom_need_backward_;
	
	// top_vecs_ 中保存每层的输入向量，同 bottom_vecs，向量中只是保存指针。   
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
```


