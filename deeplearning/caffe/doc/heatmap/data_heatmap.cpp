// Copyright 2015 Tomas Pfisterimg  
#include <fstream>  // NOLINT(readability/streams)  
#include <iostream>  // NOLINT(readability/streams)  
#include <string>  
#include <utility>  
#include <vector>  

#include "caffe/layers/data_layer.hpp"  
#include "caffe/layer.hpp"  
#include "caffe/util/io.hpp"  
#include "caffe/util/math_functions.hpp"  
#include "caffe/util/rng.hpp"  

#include <stdint.h> 
#include <cmath>  

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/highgui/highgui_c.h>  
#include <opencv2/imgproc/imgproc.hpp>  

#include "caffe/layers/data_heatmap.hpp"  
#include "caffe/util/benchmark.hpp"  
#include <unistd.h>  

namespace caffe  
{  
template <typename Dtype>  
DataHeatmapLayer<Dtype>::~DataHeatmapLayer<Dtype>() {  
    this->StopInternalThread();  
}  
  
// 读取参数文件中的一些数据什么的，然后初始化  
template<typename Dtype>
void DataHeatmapLayer<Dtype>::DataLayerSetUp(const vector<Blob<Dtype>*>& bottom,  
        const vector<Blob<Dtype>*>& top) {  
    HeatmapDataParameter heatmap_data_param = this->layer_param_.heatmap_data_param();  
  
    // Shortcuts  
    // 类标索引字符串（也就是关节类型？）  
    const std::string labelindsStr = heatmap_data_param.labelinds();  
    // batchsize  
    const int batchsize = heatmap_data_param.batchsize();  
    // heatmap的宽度  
    const int label_width = heatmap_data_param.label_width();  
    // heatmap的高度  
    const int label_height = heatmap_data_param.label_height();  
    // crop的大小  
    const int size = heatmap_data_param.cropsize();  
    // crop之后再次进行resize之后的大小  
    const int outsize = heatmap_data_param.outsize();  
    //  label的batchsize  
    const int label_batchsize = batchsize;  
    // 每个cluster都要进行采样  
    sample_per_cluster_ = heatmap_data_param.sample_per_cluster();  
    // 存放图像文件的根路径  
    root_img_dir_ = heatmap_data_param.root_img_dir();  
  
  
    // initialise rng seed  
    const unsigned int rng_seed = caffe_rng_rand();  
    srand(rng_seed);  
  
    // get label inds to be used for training  
    // 载入类标索引  
    std::istringstream labelss(labelindsStr);  
    LOG(INFO) << "using joint inds:";  
    while (labelss)  
    {  
        std::string s;  
        if (!std::getline(labelss, s, ',')) break;  
        labelinds_.push_back(atof(s.c_str()));  
        LOG(INFO) << atof(s.c_str());  
    }  
  
    // load GT  
    // shuffle file  
    // 载入ground truth文件，即关节坐标文件  
    std::string gt_path = heatmap_data_param.source();  
    LOG(INFO) << "Loading annotation from " << gt_path;  
  
    std::ifstream infile(gt_path.c_str());  
    string img_name, labels, cropInfos, clusterClassStr;  
    if (!sample_per_cluster_)// 是否根据你指定的类别随机取图像  
    {  
        // sequential sampling  
        // 文件名，关节位置坐标，crop的位置，是否均匀地在clusters上采样  
        while (infile >> img_name >> labels >> cropInfos >> clusterClassStr)  
        {  
            // read comma-separated list of regression labels  
            // 读取关节位置坐标  
            std::vector <float> label;  
            std::istringstream ss(labels);  
            int labelCounter = 1;  
            while (ss)  
            {  
                // 读取一个数字  
                std::string s;  
                if (!std::getline(ss, s, ',')) break;  
                // 是否是类标索引中的值  
                // 如果labelinds为空或者为不为空在其中找到  
                if (labelinds_.empty() || std::find(labelinds_.begin(), labelinds_.end(), labelCounter) != labelinds_.end())  
                {  
                    label.push_back(atof(s.c_str()));  
                }  
                labelCounter++;// 个数  
            }  
  
            // read cropping info  
            // 读取crop的信息  
            std::vector <float> cropInfo;  
            std::istringstream ss2(cropInfos);  
            while (ss2)  
            {  
                std::string s;  
                if (!std::getline(ss2, s, ',')) break;  
                cropInfo.push_back(atof(s.c_str()));  
            }  
  
            int clusterClass = atoi(clusterClassStr.c_str());  
            // 图像路径，关节坐标，crop信息、类别  
            img_label_list_.push_back(std::make_pair(img_name, std::make_pair(label, std::make_pair(cropInfo, clusterClass))));  
        }  
  
        // initialise image counter to 0  
        cur_img_ = 0;  
    }  
    else  
    {  
        // uniform sampling w.r.t. classes  
        // 根据类别均匀采样  
        // 也就是说图像有若干个类别，然后每个类别下有若干个图像  
        // 随机取其中一个图像  
        while (infile >> img_name >> labels >> cropInfos >> clusterClassStr)  
        {  
            // 获得你指定的类别  
            // 如果你制定为0  
            int clusterClass = atoi(clusterClassStr.c_str());  
        // 那么  
            if (clusterClass + 1 > img_list_.size())  
            {  
                // expand the array  
                img_list_.resize(clusterClass + 1);  
            }  
  
            // read comma-separated list of regression labels  
            // 读取关节的坐标位置到label这个vector  
            std::vector <float> label;  
            std::istringstream ss(labels);  
            int labelCounter = 1;  
            while (ss)  
            {  
                std::string s;  
                if (!std::getline(ss, s, ',')) break;  
                if (labelinds_.empty() || std::find(labelinds_.begin(), labelinds_.end(), labelCounter) != labelinds_.end())  
                {  
                    label.push_back(atof(s.c_str()));  
                }  
                labelCounter++;  
            }  
  
            // read cropping info  
            // 读取crop信息到cropinfo这个vector  
            std::vector <float> cropInfo;  
            std::istringstream ss2(cropInfos);  
            while (ss2)  
            {  
                std::string s;  
                if (!std::getline(ss2, s, ',')) break;  
                cropInfo.push_back(atof(s.c_str()));  
            }  
        // 每个clusterClass下都是一个vector，用于装各种图像  
            img_list_[clusterClass].push_back(std::make_pair(img_name, std::make_pair(label, std::make_pair(cropInfo, clusterClass))));  
        }// while结尾  
        
      // 图像的类别个数  
        const int num_classes = img_list_.size();  
  
        // init image sampling  
        cur_class_ = 0;  
        // cur_class_img_中存放的是某个类别中随机取到的图像的索引值  
        cur_class_img_.resize(num_classes);  
  
        // init image indices for each class  
        for (int idx_class = 0; idx_class < num_classes; idx_class++)  
        {  
            // 是否需要根据类别随机取某个类别中的一个图像  
            if (sample_per_cluster_)  
            {  
                // img_list_[idx_class].size()是该idx_class这个类中图像的个数  
                // 产生从0-该类中图像个数之间的一个随机数  
                cur_class_img_[idx_class] = rand() % img_list_[idx_class].size();  
                // 图像类别个数  
                LOG(INFO) << idx_class << " size: " << img_list_[idx_class].size();  
            }  
            else  
            {  
                cur_class_img_[idx_class] = 0;  
            }  
        }  
    }  
  
    if (!heatmap_data_param.has_meanfile())// 是否有meanfile  
    {  
        // if no mean, assume input images are RGB (3 channels)  
        this->datum_channels_ = 3;  
        sub_mean_ = false;  
    } else {  
        // Implementation of per-video mean removal  
     // 下面整个一段代码是将每个视频mean文件读取到Mat结构  
       
       
        sub_mean_ = true;  
        // 从参数文件中获取mean文件的路径  
        string mean_path = heatmap_data_param.meanfile();  
  
        LOG(INFO) << "Loading mean file from " << mean_path;  
        BlobProto blob_proto, blob_proto2;  
        Blob<Dtype> data_mean;  
        // 读取到blob,然后blob数据转换到data_mean  
        ReadProtoFromBinaryFile(mean_path.c_str(), &blob_proto);  
        data_mean.FromProto(blob_proto);  
        LOG(INFO) << "mean file loaded";  
  
        // read config  
        this->datum_channels_ = data_mean.channels();  
        // mean值的数目,有多少个视频，就有多少个mean啊  
        num_means_ = data_mean.num();  
        LOG(INFO) << "num_means: " << num_means_;  
  
        // copy the per-video mean images to an array of OpenCV structures  
        const Dtype* mean_buf = data_mean.cpu_data();  
  
        // extract means from beginning of proto file  
        // mean文件中的图像的高度  
        const int mean_height = data_mean.height();  
        // mean文件中图像的宽度  
        const int mean_width = data_mean.width();  
        // 高度数组  
        int mean_heights[num_means_];  
        // 宽度数组  
        int mean_widths[num_means_];  
  
        // offset in memory to mean images  
        //  在mean图像中的偏移量  
        const int meanOffset = 2 * (num_means_);  
        for (int n = 0; n < num_means_; n++)  
        {  
            mean_heights[n] = mean_buf[2 * n];  
            mean_widths[n] = mean_buf[2 * n + 1];  
        }  
  
        // save means as OpenCV-compatible files  
        // 将从protobin文件读取的blob存放到Mat中  
        // 获得mean_image容器，这其中包含了若干个视频的mean值  
        // 下面是分配内存  
        for (int n = 0; n < num_means_; n++)  
        {  
            cv::Mat mean_img_tmp_;  
            mean_img_tmp_.create(mean_heights[n], mean_widths[n], CV_32FC3);  
            mean_img_.push_back(mean_img_tmp_);  
            LOG(INFO) << "per-video mean file array created: " << n << ": " << mean_heights[n] << "x" << mean_widths[n] << " (" << size << ")";  
        }  
  
        LOG(INFO) << "mean: " << mean_height << "x" << mean_width << " (" << size << ")";  
    // 下面是实际的赋值  
        for (int n = 0; n < num_means_; n++)  
        {  
            for (int i = 0; i < mean_heights[n]; i++)  
            {  
                for (int j = 0; j < mean_widths[n]; j++)  
                {  
                    for (int c = 0; c < this->datum_channels_; c++)  
                    {  
                        mean_img_[n].at<cv::Vec3f>(i, j)[c] = mean_buf[meanOffset + ((n * this->datum_channels_ + c) * mean_height + i) * mean_width + j]; //[c * mean_height * mean_width + i * mean_width + j];  
                    }  
                }  
            }  
        }  
  
        LOG(INFO) << "mean file converted to OpenCV structures";  
    }  
  
  
    // init data  
    // 改变数据形状  
    this->transformed_data_.Reshape(batchsize, this->datum_channels_, outsize, outsize);  
    top[0]->Reshape(batchsize, this->datum_channels_, outsize, outsize);  
    for (int i = 0; i < this->prefetch_.size(); ++i)  
        this->prefetch_[i]->data_.Reshape(batchsize, this->datum_channels_, outsize, outsize);  
    this->datum_size_ = this->datum_channels_ * outsize * outsize;  
  
    // init label  
    int label_num_channels;  
    if (!sample_per_cluster_)// 如果不按照类别进行均匀采样  
        label_num_channels = img_label_list_[0].second.first.size();// 获取关节坐标的数字的个数（注意是数字的个数，并不是坐标的个数，要除以2才能是坐标的个数哈）  
    else// 如果按照类别均匀采样  
        label_num_channels = img_list_[0][0].second.first.size();// 第0类的第0个图像的关节数字的个数  
    label_num_channels /= 2;// 获得关节个数  
      
    // 将输出设置为对应的大小  
    // top[0]是batchsize个图像数据  
    // top[1]是batchsize个heatmap（一个heatmap有关节个数个channel）  
    // label的batchsize，关节个数作为channel，关节的heatmap的高、关节heatmap的宽度  
    top[1]->Reshape(label_batchsize, label_num_channels, label_height, label_width);  
    for (int i = 0; i < this->prefetch_.size(); ++i)  
        this->prefetch_[i]->label_.Reshape(label_batchsize, label_num_channels, label_height, label_width);  
  
    LOG(INFO) << "output data size: " << top[0]->num() << "," << top[0]->channels() << "," << top[0]->height() << "," << top[0]->width();  
    LOG(INFO) << "output label size: " << top[1]->num() << "," << top[1]->channels() << "," << top[1]->height() << "," << top[1]->width();  
    LOG(INFO) << "number of label channels: " << label_num_channels;  
    LOG(INFO) << "datum channels: " << this->datum_channels_;  
  
}  
// 根据初始化之后的信息读取实际的文件数据，以及关节的位置，并将关节位置转换为类标  
template<typename Dtype>  
void DataHeatmapLayer<Dtype>::load_batch(Batch<Dtype>* batch) {  
  
    CPUTimer batch_timer;  
    batch_timer.Start();  
    CHECK(batch->data_.count());  
    HeatmapDataParameter heatmap_data_param = this->layer_param_.heatmap_data_param();  
  
    // Pointers to blobs' float data  
    // 指向数据和类标的指针  
    Dtype* top_data = batch->data_.mutable_cpu_data();  
    Dtype* top_label = batch->label_.mutable_cpu_data();  
  
    cv::Mat img, img_res, img_annotation_vis, img_mean_vis, img_vis, img_res_vis, mean_img_this, seg, segTmp;  
  
    // Shortcuts to params  
    // 是否显示读取的图像啥的，用户调试  
    const bool visualise = this->layer_param_.visualise();  
    // 是否对图像进行缩放  
    const Dtype scale = heatmap_data_param.scale();  
    // 每次读多少个图像  
    const int batchsize = heatmap_data_param.batchsize();  
    // heatmap的高度  
    const int label_height = heatmap_data_param.label_height();  
    // heatmap的宽度  
    const int label_width = heatmap_data_param.label_width();  
    // 需要旋转多少度  
    const float angle_max = heatmap_data_param.angle_max();  
    // 是否不要翻转第一个图  
    const bool dont_flip_first = heatmap_data_param.dont_flip_first();  
    // 是否翻转关节的坐标  
    const bool flip_joint_labels = heatmap_data_param.flip_joint_labels();  
    // 关节的坐标数值需要乘以这个multfact  
    const int multfact = heatmap_data_param.multfact();  
    // 图像是否需要分割  
    const bool segmentation = heatmap_data_param.segmentation();  
    // 切割的图像的块的带下  
    const int size = heatmap_data_param.cropsize();  
    // 切割之后的图像块需要缩放到outsize大小  
    const int outsize = heatmap_data_param.outsize();  
    const int num_aug = 1;  
    // 缩放因子  
    const float resizeFact = (float)outsize / (float)size;  
    // 是不是需要随机切图像块  
    const bool random_crop = heatmap_data_param.random_crop();  
  
    // Shortcuts to global vars  
    const bool sub_mean = this->sub_mean_;  
    const int channels = this->datum_channels_;  
  
    // What coordinates should we flip when mirroring images?  
    // For pose estimation with joints assumes i=0,1 are for head, and i=2,3 left wrist, i=4,5 right wrist etc  
    //     in which case dont_flip_first should be set to true.  
    int flip_start_ind;  
    if (dont_flip_first) flip_start_ind = 2;  
    else flip_start_ind = 0;  
  
    if (visualise)  
    {  
        cv::namedWindow("original image", cv::WINDOW_AUTOSIZE);  
        cv::namedWindow("cropped image", cv::WINDOW_AUTOSIZE);  
        cv::namedWindow("interim resize image", cv::WINDOW_AUTOSIZE);  
        cv::namedWindow("resulting image", cv::WINDOW_AUTOSIZE);  
    }  
  
    // collect "batchsize" images  
    std::vector<float> cur_label, cur_cropinfo;  
    std::string img_name;  
    int cur_class;  
  
    // loop over non-augmented images  
    // 获取batchsize个图像，然后进行预处理  
    for (int idx_img = 0; idx_img < batchsize; idx_img++)  
    {  
        // get image name and class  
        // 获取文件名、label、cropinfo、类标  
        this->GetCurImg(img_name, cur_label, cur_cropinfo, cur_class);  
  
        // get number of channels for image label  
        // 获取关节的数值的个数（并不是关节个数哈，关节个数乘以2就是该数）  
        int label_num_channels = cur_label.size();  
       
     // 将根路径和文件名称拼接并读取数据到img  
        std::string img_path = this->root_img_dir_ + img_name;  
        DLOG(INFO) << "img: " << img_path;  
        img = cv::imread(img_path, CV_LOAD_IMAGE_COLOR);  
  
        // show image  
        // 显示读取的图像  
        if (visualise)  
        {  
            img_annotation_vis = img.clone();  
            this->VisualiseAnnotations(img_annotation_vis, label_num_channels, cur_label, multfact);  
            cv::imshow("original image", img_annotation_vis);  
        }  
  
        // use if seg exists  
        // 是否对图像分割  
        // 分割的模板存放在segs目录  
        // 读取分割模板到seg  
        if (segmentation)  
        {  
            std::string seg_path = this->root_img_dir_ + "segs/" + img_name;  
            std::ifstream ifile(seg_path.c_str());  
  
            // Skip this file if segmentation doesn't exist  
            if (!ifile.good())  
            {  
                LOG(INFO) << "file " << seg_path << " does not exist!";  
                idx_img--;  
                this->AdvanceCurImg();  
                continue;  
            }  
            ifile.close();  
            seg = cv::imread(seg_path, CV_LOAD_IMAGE_GRAYSCALE);  
        }  
  
        int width = img.cols;  
        int height = img.rows;  
        // size是crop的大小  
        // 如果crop的大小太大x_border会变成负数，下面会进行pad  
        int x_border = width - size;  
        int y_border = height - size;  
       
       
     // 将读取的图像转换为RGB  
        // convert from BGR to RGB  
        cv::cvtColor(img, img, CV_BGR2RGB);  
  
        // to float  
        // 转换数据类型到float  
        img.convertTo(img, CV_32FC3);  
  
        if (segmentation)  
        {  
            segTmp = cv::Mat::zeros(img.rows, img.cols, CV_32FC3);  
            int threshold = 40;// 阈值  
            // 获取分割模板  
            seg = (seg > threshold);  
            // 对图像进行分割  
            segTmp.copyTo(img, seg);  
        }  
  
        if (visualise)  
            img_vis = img.clone();  
  
        // subtract per-video mean if used  
        // 减去每个视频的均值  
        int meanInd = 0;  
        if (sub_mean)  
        {  
            // 由此可以看到每个视频的命名规则，就是目录的名字嘛，而且还是数字  
            // 比如0,1,2,3,4  
            // 假设路径是images/1/xxx.jpg  
            // 那么获取的平均值索引就是1，然后再到mean_img_中得到对应的均值图像  
            std::string delimiter = "/";  
            std::string img_name_subdirImg = img_name.substr(img_name.find(delimiter) + 1, img_name.length());  
            std::string meanIndStr = img_name_subdirImg.substr(0, img_name_subdirImg.find(delimiter));  
            meanInd = atoi(meanIndStr.c_str()) - 1;  
  
            // subtract the cropped mean  
            mean_img_this = this->mean_img_[meanInd].clone();  
  
            DLOG(INFO) << "Image size: " << width << "x" << height;  
            DLOG(INFO) << "Crop info: " << cur_cropinfo[0] << " " <<  cur_cropinfo[1] << " " << cur_cropinfo[2] << " " << cur_cropinfo[3] << " " << cur_cropinfo[4];  
            DLOG(INFO) << "Crop info after: " << cur_cropinfo[0] << " " <<  cur_cropinfo[1] << " " << cur_cropinfo[2] << " " << cur_cropinfo[3] << " " << cur_cropinfo[4];  
            DLOG(INFO) << "Mean image size: " << mean_img_this.cols << "x" << mean_img_this.rows;  
            DLOG(INFO) << "Cropping: " << cur_cropinfo[0] - 1 << " " << cur_cropinfo[2] - 1 << " " << width << " " << height;  
  
            // crop and resize mean image  
            // 对mean文件进行切割并且调整其大小为图像大小  
            // cur_cropinfo中的数据分别为x_left,x_right,y_left,y_right  
            // 而Rect则是x,y,w,h，所以需要转换  
            cv::Rect crop(cur_cropinfo[0] - 1, cur_cropinfo[2] - 1, cur_cropinfo[1] - cur_cropinfo[0], cur_cropinfo[3] - cur_cropinfo[2]);  
            mean_img_this = mean_img_this(crop);// 这样就crop了  
            cv::resize(mean_img_this, mean_img_this, img.size());  
  
            DLOG(INFO) << "Cropped mean image.";  
          
        // 原图像减去crop之后并放大成与原图像一样大小的平均值图像  
        // 这是什么原理？？？？？  
            img -= mean_img_this;  
  
            DLOG(INFO) << "Subtracted mean image.";  
  
            if (visualise)  
            {  
                img_vis -= mean_img_this;  
                img_mean_vis = mean_img_this.clone() / 255;  
                cv::cvtColor(img_mean_vis, img_mean_vis, CV_RGB2BGR);  
                cv::imshow("mean image", img_mean_vis);  
            }  
        }  
  
        // pad images that aren't wide enough  
        // 如果crop大小大于图像大小则padding，图像得右侧padding  
        if (x_border < 0)  
        {  
            DLOG(INFO) << "padding " << img_path << " -- not wide enough.";  
            // 函数原型如下  
          // void copyMakeBorder( const Mat& src, Mat& dst,  
          // int top, int bottom, int left, int right,  
          // int borderType, const Scalar& value=Scalar() );  
            cv::copyMakeBorder(img, img, 0, 0, 0, -x_border, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));  
            width = img.cols;  
            x_border = width - size;  
  
            // add border offset to joints  
            // 因为pad过图像的右侧了所以需要调整关节的x坐标  
            for (int i = 0; i < label_num_channels; i += 2)// 注意这里是i+=2哦！  
                cur_label[i] = cur_label[i] + x_border;  
  
            DLOG(INFO) << "new width: " << width << "   x_border: " << x_border;  
            if (visualise)// 显示经过padding的图像  
            {  
                img_vis = img.clone();  
                cv::copyMakeBorder(img_vis, img_vis, 0, 0, 0, -x_border, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));  
            }  
        }  
  
        DLOG(INFO) << "Entering jitter loop.";  
  
        // loop over the jittered versions  
        // 将关节位置转换为heatmap  
        for (int idx_aug = 0; idx_aug < num_aug; idx_aug++)  
        {  
            // augmented image index in the resulting batch  
            const int idx_img_aug = idx_img * num_aug + idx_aug;  
              
            // 关节坐标，首先将从文件读取的关节坐标赋值给它  
            // 接下来因为要对图像进行crop，crop之后的图像还要resize  
            // 所以对应的关节坐标也要进行crop和缩放，经过这个处理的  
            // 关节位置就存放在了 cur_label_aug  
            std::vector<float> cur_label_aug = cur_label;  
          
        // 是否随机crop  
            if (random_crop)  
            {  
                // random sampling  
                DLOG(INFO) << "random crop sampling";  
  
                // horizontal flip  
                // 随机旋转是否需要水平翻转  
                if (rand() % 2)  
                {  
                    // flip，0表示水平  
                    // 水平翻转  
                    cv::flip(img, img, 1);  
  
                    if (visualise)  
                        cv::flip(img_vis, img_vis, 1);  
  
                    // "flip" annotation coordinates  
                    // 将图像的坐标也翻转了  
                    for (int i = 0; i < label_num_channels; i += 2)  
                        // width 是原始图像的宽度，原始图像的宽度除以multfact就是关节的图像宽度，关节图像的宽度减去关节的x坐标就是翻转过来的x坐标  
                        cur_label_aug[i] = (float)width / (float)multfact - cur_label_aug[i];  
  
                    // "flip" annotation joint numbers  
                    // assumes i=0,1 are for head, and i=2,3 left wrist, i=4,5 right wrist etc  
                    // where coordinates are (x,y)  
                    // 将索引位置也翻转了。。。  
                    if (flip_joint_labels)  
                    {  
                        float tmp_x, tmp_y;  
                        for (int i = flip_start_ind; i < label_num_channels; i += 4)  
                        {  
                            CHECK_LT(i + 3, label_num_channels);  
                            tmp_x = cur_label_aug[i];  
                            tmp_y = cur_label_aug[i + 1];  
                            cur_label_aug[i] = cur_label_aug[i + 2];  
                            cur_label_aug[i + 1] = cur_label_aug[i + 3];  
                            cur_label_aug[i + 2] = tmp_x;  
                            cur_label_aug[i + 3] = tmp_y;  
                        }  
                    }  
                }  
  
                // left-top coordinates of the crop [0;x_border] x [0;y_border]  
                // 生成左上的坐标，用于切割图像  
                int x0 = 0, y0 = 0;  
                x0 = rand() % (x_border + 1);  
                y0 = rand() % (y_border + 1);  
  
                // do crop  
                cv::Rect crop(x0, y0, size, size);  
  
                // NOTE: no full copy performed, so the original image buffer is affected by the transformations below  
                // img_crop与img公用一个内存，所以在img_crop中所作的更改对img也会有  
                cv::Mat img_crop(img, crop);  
  
                // "crop" annotations  
                // 万一关节的位置在crop的大小之外怎么办？？？疑问  
                for (int i = 0; i < label_num_channels; i += 2)  
                {  
                    cur_label_aug[i] -= (float)x0 / (float) multfact;  
                    cur_label_aug[i + 1] -= (float)y0 / (float) multfact;  
                }  
  
                // show image  
                if (visualise)  
                {  
                    DLOG(INFO) << "cropped image";  
                    cv::Mat img_vis_crop(img_vis, crop);  
                    cv::Mat img_res_vis = img_vis_crop / 255;  
                    cv::cvtColor(img_res_vis, img_res_vis, CV_RGB2BGR);  
                    this->VisualiseAnnotations(img_res_vis, label_num_channels, cur_label_aug, multfact);  
                    cv::imshow("cropped image", img_res_vis);  
                }  
  
                // rotations  
                // 旋转图像到一个均匀分布的角度  
                float angle = Uniform(-angle_max, angle_max);  
                cv::Mat M = this->RotateImage(img_crop, angle);  
  
                // also flip & rotate labels  
                // 遍历所有关节坐标  
                for (int i = 0; i < label_num_channels; i += 2)  
                {  
                    // convert to image space  
                    // 将关节坐标转换到图像中的坐标  
                    float x = cur_label_aug[i] * (float) multfact;  
                    float y = cur_label_aug[i + 1] * (float) multfact;  
  
                    // rotate  
                    // ？为啥  
                    cur_label_aug[i] = M.at<double>(0, 0) * x + M.at<double>(0, 1) * y + M.at<double>(0, 2);  
                    cur_label_aug[i + 1] = M.at<double>(1, 0) * x + M.at<double>(1, 1) * y + M.at<double>(1, 2);  
  
                    // convert back to joint space  
                    // 转换回关节空间  
                    cur_label_aug[i] /= (float) multfact;  
                    cur_label_aug[i + 1] /= (float) multfact;  
                }  
  
                img_res = img_crop;  
            } else {// 中心crop(就是图像的中心crop啊)  
                // determinsitic sampling  
                DLOG(INFO) << "deterministic crop sampling (centre)";  
  
                // centre crop  
                const int y0 = y_border / 2;  
                const int x0 = x_border / 2;  
  
                DLOG(INFO) << "cropping image from " << x0 << "x" << y0;  
  
                // do crop  
                cv::Rect crop(x0, y0, size, size);  
                cv::Mat img_crop(img, crop);  
  
                DLOG(INFO) << "cropping annotations.";  
  
                // "crop" annotations  
                // 长见识了，关节的annotation也是需要crop的  
                for (int i = 0; i < label_num_channels; i += 2)  
                {  
                    // 除以multfact转换到关节坐标，然后再减去  
                    // 不过我有疑问，万一crop之后的图像没有关节咋办  
                    // 这样真的好吗  
                    cur_label_aug[i] -= (float)x0 / (float) multfact;  
                    cur_label_aug[i + 1] -= (float)y0 / (float) multfact;  
                }  
  
                if (visualise)  
                {  
                    cv::Mat img_vis_crop(img_vis, crop);  
                    cv::Mat img_res_vis = img_vis_crop.clone() / 255;  
                    cv::cvtColor(img_res_vis, img_res_vis, CV_RGB2BGR);  
                    this->VisualiseAnnotations(img_res_vis, label_num_channels, cur_label_aug, multfact);  
                    cv::imshow("cropped image", img_res_vis);  
                }  
                img_res = img_crop;  
            }// end of else  
  
            // show image  
            if (visualise)  
            {  
                cv::Mat img_res_vis = img_res / 255;  
                cv::cvtColor(img_res_vis, img_res_vis, CV_RGB2BGR);  
                this->VisualiseAnnotations(img_res_vis, label_num_channels, cur_label_aug, multfact);  
                cv::imshow("interim resize image", img_res_vis);  
            }  
  
            DLOG(INFO) << "Resizing output image.";  
  
            // resize to output image size  
            // 将crop之后的图像弄到给定的大小  
            cv::Size s(outsize, outsize);  
            cv::resize(img_res, img_res, s);  
  
            // "resize" annotations  
            // resize 标注的关节  
            // 将图像进行缩放了，那么关节的坐标也要缩放  
            for (int i = 0; i < label_num_channels; i++)  
                cur_label_aug[i] *= resizeFact;  
  
            // show image  
            if (visualise)  
            {  
                cv::Mat img_res_vis = img_res / 255;  
                cv::cvtColor(img_res_vis, img_res_vis, CV_RGB2BGR);  
                this->VisualiseAnnotations(img_res_vis, label_num_channels, cur_label_aug, multfact);  
                cv::imshow("resulting image", img_res_vis);  
            }  
  
            // show image  
            if (visualise && sub_mean)  
            {  
                cv::Mat img_res_meansub_vis = img_res / 255;  
                cv::cvtColor(img_res_meansub_vis, img_res_meansub_vis, CV_RGB2BGR);  
                cv::imshow("mean-removed image", img_res_meansub_vis);  
            }  
  
            // multiply by scale  
            // 去均值、crop、缩放之后的像素值乘以该scale得到最终的图像的  
            if (scale != 1.0)  
                img_res *= scale;  
  
            // resulting image dims  
            const int channel_size = outsize * outsize;  
            const int img_size = channel_size * channels;  
  
            // store image data  
            // 将处理好的图像存放到top_data  
            DLOG(INFO) << "storing image";  
            for (int c = 0; c < channels; c++)  
            {  
                for (int i = 0; i < outsize; i++)  
                {  
                    for (int j = 0; j < outsize; j++)  
                    {  
                        top_data[idx_img_aug * img_size + c * channel_size + i * outsize + j] = img_res.at<cv::Vec3f>(i, j)[c];  
                    }  
                }  
            }  
  
            // store label as gaussian  
            // 将关节转换为高斯图像  
            DLOG(INFO) << "storing labels";  
            const int label_channel_size = label_height * label_width;  
            const int label_img_size = label_channel_size * label_num_channels / 2;  
            cv::Mat dataMatrix = cv::Mat::zeros(label_height, label_width, CV_32FC1);  
            float label_resize_fact = (float) label_height / (float) outsize;  
            float sigma = 1.5;  
  
            for (int idx_ch = 0; idx_ch < label_num_channels / 2; idx_ch++)  
            {  
                // 将经过缩放的关节转换到图像空间的坐标(也就是乘以multfact)，再将缩小之后的图像空间坐标转换到缩小之前的图像空间坐标(也就是乘以label_resize_fact)  
                float x = label_resize_fact * cur_label_aug[2 * idx_ch] * multfact;  
                float y = label_resize_fact * cur_label_aug[2 * idx_ch + 1] * multfact;  
                for (int i = 0; i < label_height; i++)  
                {  
                    for (int j = 0; j < label_width; j++)  
                    {  
                        // 计算索引  
                        int label_idx = idx_img_aug * label_img_size + idx_ch * label_channel_size + i * label_height + j;  
                        float gaussian = ( 1 / ( sigma * sqrt(2 * M_PI) ) ) * exp( -0.5 * ( pow(i - y, 2.0) + pow(j - x, 2.0) ) * pow(1 / sigma, 2.0) );  
                        gaussian = 4 * gaussian;  
                          
                        // 存入到top_label  
                        top_label[label_idx] = gaussian;  
  
                        if (idx_ch == 0)  
                            dataMatrix.at<float>((int)j, (int)i) = gaussian;  
                    }  
                }  
            }  
  
        } // jittered versions loop  
  
        DLOG(INFO) << "next image";  
  
        // move to the next image  
        // Advance是进行  
        // Cur是表示当前  
        // 那么就是移动到下一个图像  
        this->AdvanceCurImg();  
  
        if (visualise)  
            cv::waitKey(0);  
  
  
    } // original image loop  
  
    batch_timer.Stop();  
    DLOG(INFO) << "Prefetch batch: " << batch_timer.MilliSeconds() << " ms.";  
}  
  
  
// 获取当前图像的路径、类标、crop信息、类别  
template<typename Dtype>  
void DataHeatmapLayer<Dtype>::GetCurImg(string& img_name, std::vector<float>& img_label, std::vector<float>& crop_info, int& img_class)  
{  
  
    if (!sample_per_cluster_)  
    {  
        img_name = img_label_list_[cur_img_].first;  
        img_label = img_label_list_[cur_img_].second.first;  
        crop_info = img_label_list_[cur_img_].second.second.first;  
        img_class = img_label_list_[cur_img_].second.second.second;  
    }  
    else  
    {  
        img_class = cur_class_;  
        // 看见没，这里用到了cur_class_img_，这个在SetUp中生成的随机数作为该类别的图像索引，该随机数的范围在[0,该类别图像的个数-1]之间。  
        img_name = img_list_[img_class][cur_class_img_[img_class]].first;  
        img_label = img_list_[img_class][cur_class_img_[img_class]].second.first;  
        crop_info = img_list_[img_class][cur_class_img_[img_class]].second.second.first;  
    }  
}  
  
// 实际上就是移动索引  
template<typename Dtype>  
void DataHeatmapLayer<Dtype>::AdvanceCurImg()  
{  
    if (!sample_per_cluster_)  
    {  
        if (cur_img_ < img_label_list_.size() - 1)  
            cur_img_++;  
        else  
            cur_img_ = 0;  
    }  
    else  
    {  
        const int num_classes = img_list_.size();  
  
        if (cur_class_img_[cur_class_] < img_list_[cur_class_].size() - 1)  
            cur_class_img_[cur_class_]++;  
        else  
            cur_class_img_[cur_class_] = 0;  
  
        // move to the next class  
        if (cur_class_ < num_classes - 1)  
            cur_class_++;  
        else  
            cur_class_ = 0;  
    }  
  
}  
  
// 可视化关节点  
template<typename Dtype>  
void DataHeatmapLayer<Dtype>::VisualiseAnnotations(cv::Mat img_annotation_vis, int label_num_channels, std::vector<float>& img_class, int multfact)  
{  
    // colors  
    const static cv::Scalar colors[] = {  
        CV_RGB(0, 0, 255),  
        CV_RGB(0, 128, 255),  
        CV_RGB(0, 255, 255),  
        CV_RGB(0, 255, 0),  
        CV_RGB(255, 128, 0),  
        CV_RGB(255, 255, 0),  
        CV_RGB(255, 0, 0),  
        CV_RGB(255, 0, 255)  
    };  
  
    int numCoordinates = int(label_num_channels / 2);  
  
    // points  
    // 将关节点放到centers数组中  
    cv::Point centers[numCoordinates];  
    for (int i = 0; i < label_num_channels; i += 2)  
    {  
        int coordInd = int(i / 2);  
        centers[coordInd] = cv::Point(img_class[i] * multfact, img_class[i + 1] * multfact);  
        // 给关节画圈圈  
        cv::circle(img_annotation_vis, centers[coordInd], 1, colors[coordInd], 3);  
    }  
  
    // connecting lines  
    // 1,3,5是一条膀子  
    // 2,4,6是一条膀子  
    cv::line(img_annotation_vis, centers[1], centers[3], CV_RGB(0, 255, 0), 1, CV_AA);  
    cv::line(img_annotation_vis, centers[2], centers[4], CV_RGB(255, 255, 0), 1, CV_AA);  
    cv::line(img_annotation_vis, centers[3], centers[5], CV_RGB(0, 0, 255), 1, CV_AA);  
    cv::line(img_annotation_vis, centers[4], centers[6], CV_RGB(0, 255, 255), 1, CV_AA);  
}  
  
// [min,max]的均匀分布  
template <typename Dtype>  
float DataHeatmapLayer<Dtype>::Uniform(const float min, const float max) {  
    float random = ((float) rand()) / (float) RAND_MAX;  
    float diff = max - min;  
    float r = random * diff;  
    return min + r;  
}  
  
// 旋转图像  
template <typename Dtype>  
cv::Mat DataHeatmapLayer<Dtype>::RotateImage(cv::Mat src, float rotation_angle)  
{  
    cv::Mat rot_mat(2, 3, CV_32FC1);  
    cv::Point center = cv::Point(src.cols / 2, src.rows / 2);  
    double scale = 1;  
  
    // Get the rotation matrix with the specifications above  
    rot_mat = cv::getRotationMatrix2D(center, rotation_angle, scale);  
  
    // Rotate the warped image  
    cv::warpAffine(src, src, rot_mat, src.size());  
  
    return rot_mat;  
}
  
INSTANTIATE_CLASS(DataHeatmapLayer);  
REGISTER_LAYER_CLASS(DataHeatmap);  

} // namespace caffe  