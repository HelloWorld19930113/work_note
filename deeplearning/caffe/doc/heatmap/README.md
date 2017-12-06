# 在`caffe`中添加`heatmap`层    

>参考博客：   
[1. `caffe`添加`PrecisionRecallLosslayer`层](http://blog.csdn.net/langb2014/article/details/50489305#reply)    
[2. `caffe`添加`HeatmapData`层](http://blog.csdn.net/langb2014/article/details/50985804)   
第二篇博客以[github-caffe heatmap](https://github.com/tpfister/caffe-heatmap)中所实现的`data_heatma.cpp`、`data_heatmap.hpp`、`euclidean_loss_heatmap_layer.hpp`和`euclidean_loss_heatmap_layer.cpp`为例介绍如何写自己的层。  

## 1. 准备工作
1. 先下载训练和测试的图片数据。    
2. 下载预训练模型(optional)。    

**Note** these files require multfact=282 in both training and testing data layers.       

## 2. 开始训练    
2.1 准备输入文件        
下载得到的训练数据中包含`两个txt文件`和两个图片文件夹`train`和`test`。   
```
Create two label files, one for training and another for testing, in the format:
train/FILE.jpg 123,144,165,123,66,22 372.296,720,1,480,0.53333 0

This is a space-delimited file where

the first arg is the path to your image
the second arg is a comma-delimited list of (x,y) coordinates you wish to regress (the coordinates in the train/FILE.jpg image space)
the third arg is a comma-delimited list of crops & scaling factors of the input image (in order x_left,x_right,y_left,y_right,scaling_fact). Note: These crop & scaling factors are only used to crop the mean image. You can set these to 0 if you aren't using a mean image (for mean subtraction).
the fourth arg is a coordinate 'cluster' (from which you have the option to evenly sample images in training). You can set this to 0.
Example pre-cropped images and label files for FLIC are provided above.
```
2.2 修改`models/heatmap-flic-fusion/train_val.txt`中的两个路径值。    
```
source: "/home/lsn/data/flic_heatmap/train_shuffle.txt"
root_img_dir: "/home/lsn/data/flic_heatmap/"   

source: "/home/lsn/data/flic_heatmap/test_shuffle.txt"
root_img_dir: "/home/lsn/data/flic_heatmap/"
```
2.3 开始训练。     
```
$ sh train_heatmap.sh heatmap-flic-fusion 1 
```
2.4 Supported augmentations    
```
Random crop, resize, mirror and rotation
Heatmap params

visualise: show visualisations for crops, rotations etc (recommended for testing)
source: label file
root_img_dir: directory with images (recommend you store images on ramdisk)
meanfile: proto file containing the mean image(s) to be subtracted (optional)
cropsize: size of random crop (randomly cropped from the original image)
outsize: size that crops are resized to
multfact: label coordinates in the ground truth text file are multiplied by this (default 1)
sample_per_cluster: sample evenly across clusters
random_crop: do random crop (if false, do center crop)
label_height/width: width of regressed heatmap (must match net config)
segmentation: segment images on the fly (assumes images are in a segs/ directory)
angle_max: max rotation angle for training augmentation
Pose estimation-specific parameters

flip_joint_labels: when horizontally flipping images for augmentation, if this is set to true the code also swaps left<->right labels (this is important e.g. for observer-centric pose estimation). This assumes that the left,right joint labelsare listed consecutively (e.g. wrist_left,wrist_right,elbow_left,elbow_right)
dont_flip_first: This option allows you to turn off label mirroring for the first label. E.g. for labels head,wrist_right,wrist_left,elbow_right,elbow_left,shoulder_right,shoulder_left, the first joint is head and should not be swapped with wrist_right.
```
**Notes**
Ensure that the cropsize is set so that the crop normally covers most of the positions in the image that you wish to regress. E.g. for FLIC we prepared 256x256 cropped input images (cropped as a bounding box around the provided torso point) and used these as input images.    

## Paper   
Please cite our ICCV'15 paper in your publications if this code helps your research:    
```
@InProceedings{Pfister15a,
author       = "Pfister, T. and Charles, J. and Zisserman, A.",
title        = "Flowing ConvNets for Human Pose Estimation in Videos",
booktitle    = "IEEE International Conference on Computer Vision",
year         = "2015",
}
```
