# ubuntu 16.04 安装ssd教程

Author:Wish

1.下载并编译ssd caffe

git clone https://github.com/weiliu89/caffe.git
cd caffe
git checkout ssd
cp Makefile.config.example Makefile.config
gedit Makefile.config

这里我们使用gpu和cudnn，那么修改USE_CUDNN := 1
我们要使用python接口，这里修改WITH_PYTHON_LAYER := 1
make -j16
make py
make test -j16


2. 准备图片文件，在imgs(images)里面有已经标定好的图片文件，注意是jpg文件哟

我们通过python lab.py能够实现对imgs目录下的图片文件做画框操作，然后保存xml文件
下面列出xml的主要几个元素，只要满足这几个参数就可以了
（图片被我归一化到300*300，实际上并不需要完全一样，可以允许不一样的尺寸）
```xml
<annotation>
    <size>
        <width>300</width>
        <height>300</height>
    </size>
    <object>
        <name>st</name>
        <bndbox>
            <xmin>54</xmin>
            <ymin>121</ymin>
            <xmax>256</xmax>
            <ymax>283</ymax>
        </bndbox>
    </object>
</annotation>
```
3.新建一个目录叫`xwk_st`，里面有文件夹

Annotations         这个放标注信息文件`xml`的，画框等标记位置信息
ImageSets           这个放数据集信息相关的，定义训练和测试等
    -- Main         这里是`ImageSets`的子目录，文件在这里面
JPEGImages          这里放图片文件的

4.我们把xml全部复制到Annotations里面，把jpg复制到JPEGImages

然后通过一段代码（就是make-list-file.py），产生的trainval.txt，test.txt复制到ImageSets/Main里边
import os
import random

trainacc = 0.9
fs = os.listdir("xwk_st/JPEGImages")
ntrain = int(trainacc * len(fs))
nval = len(fs) - ntrain
random.shuffle(fs)

with open("trainval.txt", "wb") as tf:
    for i in range(ntrain):
        p = fs[i].rfind(".")
        tf.write(fs[i][:p] + "\n")

pos = ntrain
with open("test.txt", "wb") as tf:
    for i in range(nval):
        p = fs[i+pos].rfind(".")
        tf.write(fs[i+pos][:p] + "\n")

5.进入到caffe/data，我们复制一个VOC0712，并命名xwk_st（这个名字必须跟上面的xwk_st一样）

我们打开里边的labelmap_voc.prototxt
修改类别1为st(这个名字必须跟lab.py里面指定的名字相匹配)
item {
  name: "st"
  label: 1
  display_name: "st"
}

6.打开create_list.sh，修改第3行的root_dir指向我们创建的目录上一级（即xwk_st所在目录）

root_dir=/hope/userdata/dl/5.ssd/ssd-jc/doc/data

修改第13行
for name in VOC2007 VOC2012  修改为
for name in xwk_st

我们打开终端，cd到caffe目录
cd /hope/userdata/dl/5.ssd/ssd-jc/caffe

然后执行创建列表
./data/xwk_st/create_list.sh

可以看到xwk_st目录下多了几个文件

7.打开create_data.sh，修改第7第8行

data_root_dir="$HOME/data/VOCdevkit"
dataset_name="VOC0712"
为
data_root_dir="/hope/userdata/dl/5.ssd/ssd-jc/doc/data"
dataset_name="xwk_st"

然后在终端执行（caffe目录下跟第6步一样）：
./data/xwk_st/create_data.sh

哐当，会报错，提示caffe.proto
我们去caffe/scripts/create_annoset.py
找到第6行，加入一个路径
sys.path.insert(0, "/hope/userdata/dl/5.ssd/ssd-jc/caffe/python")
from caffe.proto import caffe_pb2
from google.protobuf import text_format

再来一遍
./data/xwk_st/create_data.sh

提示：Processed 256 files.完成了

8.我们到caffe/examples/ssd里面，复制一个ssd_pascal.py，命名ssd_pascal_xwk_st.py并打开他

在头部，我们插入路径
from __future__ import print_function
import sys
sys.path.insert(0, "/hope/userdata/dl/5.ssd/ssd-jc/caffe/python")
import caffe


然后修改大约再85行
train_data = "examples/VOC0712/VOC0712_trainval_lmdb"
修改为以下，我们自己产生的东西：
train_data = "/hope/userdata/dl/5.ssd/ssd-jc/doc/data/xwk_st/lmdb/xwk_st_trainval_lmdb"

然后是：
test_data = "examples/VOC0712/VOC0712_test_lmdb"
修改为
test_data = "/hope/userdata/dl/5.ssd/ssd-jc/doc/data/xwk_st/lmdb/xwk_st_test_lmdb"

修改大于335行
gpus = "0,1,2,3"，修改为gpus = "0"

修改batch_size为16,在340行左右，当然太大了异常的话自己调整小
batch_size = 16
accum_batch_size = 16

修改num_test_image = 29，在362行左右，这个是我们test.txt里面记录的文件个数

修改243行左右：
# Directory which stores the model .prototxt file.
save_dir = "/hope/userdata/dl/5.ssd/ssd-jc/doc/data/models"
# Directory which stores the snapshot of models.
snapshot_dir = "/hope/userdata/dl/5.ssd/ssd-jc/doc/data/snapshots"
# Directory which stores the job script and log file.
job_dir = "/hope/userdata/dl/5.ssd/ssd-jc/doc/data/jobs"

目录没有需要自己建立的

修改264行左右：
# Stores the test image names and sizes. Created by data/VOC0712/create_list.sh
name_size_file = "data/xwk_st/test_name_size.txt"
# The pretrained model. We use the Fully convolutional reduced (atrous) VGGNet.
pretrain_model = "/hope/userdata/dl/5.ssd/ssd-jc/doc/data/VGG_ILSVRC_16_layers_fc_reduced.caffemodel"
# Stores LabelMapItem.
label_map_file = "data/xwk_st/labelmap_voc.prototxt"
这个预训练模型下载地址是：http://cs.unc.edu/~wliu/projects/ParseNet/VGG_ILSVRC_16_layers_fc_reduced.caffemodel


9.终端在caffe目录下，执行：

python examples/ssd/ssd_pascal_xwk_st.py

就开始训练啦

10.调用ssd模型做识别

目录下的callssd.py就是一个简单的演示例子
不过需要注意的是，当你自己训练出来的模型，从models里面复制出来的deploy.prototxt文件地步包括了一个路径指向一个txt文件，这个是没有必要的，可以删掉这个参数，可以参照一下目录下面我放的deploy.prototxt这个文件


Wish 2017年4月2日 02:28:11
点击我有彩蛋