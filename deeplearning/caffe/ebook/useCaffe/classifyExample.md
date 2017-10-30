# caffe 训练一个自己的分类器

1.下载附件里面的数据

2.下载附件里面把图片目录标签处理的文件`make-label.py`

3.运行 `make-label.py`
`cmd`中切换到`make-label.py`的存放目录后运行:
```
$ python make-label.py test
```
这时候就会生产这个三个文件`xxx-train.txt,xxx-val.txt,xxx-labels.txt`

4.找个训练网络这里用的alexNet 

5.新建一个convert-imageset.bat文件
```
H:\program\caffe\caffe-master\Build\x64\Release\convert_imageset ./ test-val.txt val-lmdb --resize_width=200 --resize_height=200
H:\program\caffe\caffe-master\Build\x64\Release\convert_imageset ./ test-train.txt train-lmdb --resize_width=200 --resize_height=200
H:\program\caffe\caffe-master\Build\x64\Release\compute_image_mean train-lmdb mean.binaryproto
```

第六步.修改网络参数
```
solver.prototxt
net: "train_val.prototxt"
test_iter: 3
test_interval:100
base_lr: 0.001
lr_policy: "step"
gamma: 0.1
stepsize: 100000
display: 100
max_iter: 45000
momentum: 0.9
weight_decay: 0.0005
snapshot: 10000
snapshot_prefix: "models/caffe_alexnet_train"
solver_mode: GPU
```
这里的`test_iter`: 3是通过我们测试数据集大小`24 / 8`(网络`train_val.prototxt`的训练部分的`batch_size = 8`)计算出来的
`solver_mode` 如果你不支持`GPU`请改成`CPU`
新建一个`models`文件夹，就是`snapshot_prefix`指定的存放模型位置
 
```
train_val.prototxt
name: "AlexNet"
layer {
  name: "data"
  type: "Data"
  top: "data"
  top: "label"
  include {
    phase: TRAIN
  }
  transform_param {
    mirror: true
    crop_size: 227
    mean_file: "mean.binaryproto"
  }
  data_param {
    source: "train-lmdb"
    batch_size: 8
    backend: LMDB
  }
}
layer {
  name: "data"
  type: "Data"
  top: "data"
  top: "label"
  include {
    phase: TEST
  }
  transform_param {
    mirror: false
    crop_size: 227
    mean_file: "mean.binaryproto"
  }
  data_param {
    source: "val-lmdb"
    batch_size: 8
    backend: LMDB
  }
}

....中间省略，看最后
layer {
  name: "fc8"
  type: "InnerProduct"
  bottom: "fc7"
  top: "fc8"
  param {
    lr_mult: 1
    decay_mult: 1
  }
  param {
    lr_mult: 2
    decay_mult: 0
  }
  inner_product_param {
    num_output: 2
    weight_filler {
      type: "gaussian"
      std: 0.01
    }
    bias_filler {
      type: "constant"
      value: 0
    }
  }
}
```
把`fc8`层的`num_output`修改为我们的类别数，只有两类

第七步新建一个`train.bat`
```
H:\program\caffe\caffe-master\Build\x64\Release\caffe train --solver=solver.prototxt
```

然后运行`train.bat`就开始训练了就是这么`so easy`。

