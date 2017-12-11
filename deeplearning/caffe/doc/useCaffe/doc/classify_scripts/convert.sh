#!/bin/bash

/home/lthpc/work/gitwork/caffe/build/tools/convert_imageset ./ data-val.txt val-lmdb --resize_width=224 --resize_height=224 --shuffle=true
/home/lthpc/work/gitwork/caffe/build/tools/convert_imageset ./ data-train.txt train-lmdb --resize_width=224 --resize_height=224 --shuffle=true
/home/lthpc/work/gitwork/caffe/build/tools/compute_image_mean train-lmdb 
