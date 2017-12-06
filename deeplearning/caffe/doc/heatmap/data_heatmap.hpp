// Copyright 2014 Tomas Pfister  
  
#ifndef CAFFE_HEATMAP_HPP_  
#define CAFFE_HEATMAP_HPP_  
  
#include "caffe/layer.hpp"  
#include <vector>  
#include <boost/timer/timer.hpp>  
#include <opencv2/core/core.hpp>  
  
#include "caffe/common.hpp"  
#include "caffe/data_transformer.hpp"  
#include "caffe/filler.hpp"  
#include "caffe/internal_thread.hpp"  
#include "caffe/proto/caffe.pb.h"  
  
namespace caffe  
{  
  
// 继承自PrefetchingDataLayer  
template<typename Dtype>  
class DataHeatmapLayer: public BasePrefetchingDataLayer<Dtype>  
{  
  
public:  
  
    explicit DataHeatmapLayer(const LayerParameter& param)  
        : BasePrefetchingDataLayer<Dtype>(param) {}  
    virtual ~DataHeatmapLayer();  
    virtual void DataLayerSetUp(const vector<Blob<Dtype>*>& bottom,  
                                const vector<Blob<Dtype>*>& top);  
  
    virtual inline const char* type() const { return "DataHeatmap"; }  
  
    virtual inline int ExactNumBottomBlobs() const { return 0; }  
    virtual inline int ExactNumTopBlobs() const { return 2; }  
  
  
protected:  
    // 虚函数，就是实际读取一批数据到Batch中  
    virtual void load_batch(Batch<Dtype>* batch);  
    // 以下都是自己定义的要使用的函数，都在load_batch中被调用了  
      
    // Filename of current image  
    inline void GetCurImg(string& img_name, std::vector<float>& img_class, std::vector<float>& crop_info, int& cur_class);  
  
    inline void AdvanceCurImg();  
  
    // Visualise point annotations  
    inline void VisualiseAnnotations(cv::Mat img_annotation_vis, int numChannels, std::vector<float>& cur_label, int width);  
  
    // Random number generator  
    inline float Uniform(const float min, const float max);  
  
    // Rotate image for augmentation  
    inline cv::Mat RotateImage(cv::Mat src, float rotation_angle);  
  
    // Global vars  
    shared_ptr<Caffe::RNG> rng_data_;  
    shared_ptr<Caffe::RNG> prefetch_rng_;  
    vector<std::pair<std::string, int> > lines_;  
    int lines_id_;      
    int datum_channels_;  
    int datum_height_;  
    int datum_width_;  
    int datum_size_;  
    int num_means_;  
    int cur_class_;  
    vector<int> labelinds_;  
    // 图像均值的vector容器，其中存放的是每个视频的均值  
    vector<cv::Mat> mean_img_;  
    // 是否需要减去每个视频的均值  
    bool sub_mean_;  // true if the mean should be subtracted  
    // 是否对在每个类进行均匀采样  
    bool sample_per_cluster_; // sample separately per cluster?  
    string root_img_dir_;  
    // 如果开启sample_per_cluster_则该vector中放的就是在该类别中随机采样的图像的索引  
    // 举个例子，如果类别1的图像的个数是10个，那么就随机生成[0,9]之间的一个数作为采样的图像的索引  
    // 从类别1中将该图像取出进行处理，就是sample_per_cluster_=true的含义  
    // 这个数组实际上就是从类别到该类别的随机的一个图像编号的映射  
    vector<float> cur_class_img_; // current class index  
      
    // 当前图像的索引，处理的时候用  
    int cur_img_; // current image index  
      
    // 图像索引（图像的编号从0开始）到类别的映射  
    vector<int> img_idx_map_; // current image indices for each class  
  
    // array of lists: one list of image names per class  
    // 这么一长串这么吓人  
    // 分解开来看，要访问的时候  
    // 最外层首先要提供索引，因为第一个类型是vector  
    // 第二层还是vector，所以还是需要索引才能访问  
    //  第三层是pair，访问第一个可以用first，第二个用second  
    // 如果第三层是first，则第四层直接就是string的值了  
    // 如果第三层是second，则第四层就是pair，那么可以用first或者用second  
    // 如果第四层是first，那么第五层就可以用索引访问  
    // 如果第四层是second，那么第五层就直接是int值  
    vector< vector< pair<string, pair<vector<float>, pair<vector<float>, int> > > > > img_list_;  
  
    // vector of (image, label) pairs  
    // 外层是vector，所以用索引  
    // 第二层是pair，所以用first或者second  
    // 第三层是pair，所以继续用first或者second  
    // 第四层是vector或者pair，如果第三层的是first，那么第四层就可以用索引访问  
    // 如果第三层是second，那么第四层就直接得到值了  
    vector< pair<string, pair<vector<float>, pair<vector<float>, int> > > > img_label_list_;      
};  
  
}
#endif /* CAFFE_HEATMAP_HPP_ */