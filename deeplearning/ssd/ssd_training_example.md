# `ssd`训练样本的流程

Author: Wish

1.下载并编译ssd caffe
```
git clone https://github.com/weiliu89/caffe.git
cd caffe
git checkout ssd
cp Makefile.config.example Makefile.config
gedit Makefile.config
```
这里我们使用gpu和cudnn，那么修改`USE_CUDNN := 1`
我们要使用python接口，这里修改`WITH_PYTHON_LAYER := 1`
然后执行下面的命令：
```
make -j16
make py
make test -j16
```

2.准备图片文件，在`imgs(images)`里面有已经标定好的图片文件，注意是`.jpg`文件哟

我们通过`python lab.py`能够实现对imgs目录下的图片文件做画框操作，然后保存`xml`文件
下面列出`xml`的主要几个元素，只要满足这几个参数就可以了
（图片被我归一化到300*300，实际上并不需要完全一样，可以允许不一样的尺寸）
```xml
<annotation>
    <size>
        <width>300</width>
        <height>300</height>
    </size>
    <object>
        <name>st</name>     # 注意这里的name是st
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
```
Annotations         这个放标注信息文件`xml`的，画框等标记位置信息
ImageSets           这个放数据集信息相关的，定义训练和测试等
    -- Main         这里是`ImageSets`的子目录，文件在这里面
JPEGImages          这里放图片文件的
```
4.拷贝图片
(1)把`xml`全部复制到`Annotations`里面;
(2)把`.jpg`复制到`JPEGImages`;
(3)然后通过一段代码（就是`make-list-file.py`），产生的`trainval.txt`，`test.txt`复制到`ImageSets/Main`目录下;
其中`make-list-file.py`的内容如下：
```python
import os
import random

trainacc = 0.9

fs = os.listdir("JPEGImages")
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
```
5.进入到`caffe/data`，在当前目录下复制一个`VOC0712`为`xwk_st`（这个名字必须跟上面的`xwk_st`一样）

我们打开里边的`labelmap_voc.prototxt`
修改类别`1`为`st`(这个名字必须跟lab.py里面指定的名字相匹配)
```
item {
  name: "st"
  label: 1
  display_name: "st"
}
```
6.打开`create_list.sh，修改第3行的root_dir指向我们创建的目录上一级（即xwk_st所在目录）
```
root_dir=/home/eric/trainData/
# 修改第13行
for name in VOC2007 VOC2012  修改为
for name in xwk_st
```
我们打开终端，切换到`caffe`工程所在目录。然后执行创建列表
```
$ cd /home/eric/work/gitwork/ssd/caffe
$ ./data/xwk_st/create_list.sh
```
可以看到`xwk_st`目录下多了几个文件

7.打开`create_data.sh`，
```
# 修改第7第8行
data_root_dir="$HOME/data/VOCdevkit"
dataset_name="VOC0712"
# 为
data_root_dir="/hope/userdata/dl/5.ssd/ssd-jc/doc/data"
dataset_name="xwk_st"
```
然后在终端执行（caffe目录下跟第6步一样）：
```
$ ./data/xwk_st/create_data.sh
```
哐当，会报错，提示`caffe.proto`:
我们去`caffe/scripts/create_annoset.py`找到第6行，加入一个路径
```
sys.path.insert(0, "/home/eric/work/gitwork/ssd/caffe/python")
from caffe.proto import caffe_pb2
from google.protobuf import text_format
```
再来一遍
```
$ ./data/xwk_st/create_data.sh
```
提示：Processed 256 files.完成了

8.我们到`caffe/examples/ssd`里面，复制一个`ssd_pascal.py`，命名`ssd_pascal_xwk_st.py`并打开:
在头部，我们插入路径
```
from __future__ import print_function
import sys
sys.path.insert(0, "/home/eric/work/gitwork/ssd/caffe/python")
import caffe
# 然后修改大约再85行
train_data = "examples/VOC0712/VOC0712_trainval_lmdb"
# 为:
train_data = "/home/eric/data/xwk_st/lmdb/xwk_st_trainval_lmdb"
# 然后修改：
test_data = "examples/VOC0712/VOC0712_test_lmdb"
# 为
test_data = "/home/eric/data/xwk_st/lmdb/xwk_st_test_lmdb"
# 修改大于335行
gpus = "0,1,2,3"，修改为gpus = "0"
# 修改batch_size为16,在340行左右，当然太大了异常的话自己调整小
batch_size = 16
accum_batch_size = 16
# 修改num_test_image = 29，在362行左右，这个是我们test.txt里面记录的文件个数
num_test_image = 29
# 修改243行左右：(可以使用内部的models/)
# Directory which stores the model .prototxt file.
save_dir = "/home/eric/data/models"
# Directory which stores the snapshot of models.
snapshot_dir = "/home/eric/data/snapshots"
# Directory which stores the job script and log file.
job_dir = "/home/eric/data/jobs"

# 目录没有需要自己建立的
# 修改264行左右：
# Stores the test image names and sizes. Created by data/VOC0712/create_list.sh
name_size_file = "/home/eric/data/xwk_st/test_name_size.txt"
# The pretrained model. We use the Fully convolutional reduced (atrous) VGGNet.
pretrain_model = "/home/eric/data/VGG_ILSVRC_16_layers_fc_reduced.caffemodel"
# Stores LabelMapItem.
label_map_file = "data/xwk_st/labelmap_voc.prototxt"
# 这个预训练模型下载地址是：http://cs.unc.edu/~wliu/projects/ParseNet/VGG_ILSVRC_16_layers_fc_reduced.caffemodel
```
修改完之后保存退出。

9.终端在caffe目录下，执行：
```
$ python examples/ssd/ssd_pascal_xwk_st.py
```
就开始训练啦

10.调用`ssd`模型做识别

目录下的`callssd.py`就是一个简单的演示例子

**注意**
当你自己训练模型完毕后，从`models`里面复制出来的`deploy.prototxt`文件底部设置了一个指向一个`txt文件`的路径。这个是没有必要的，可以删掉这个参数，可以参照一下目录下面我放的`deploy.prototxt`这个文件。



## 问题及解决方案：
1. 将某个目录下的所有`.jpg`文件复制到某个文件夹
```bash
$ find -name "*.xml" -exec cp {} xxxx/ \;
```
-exec 代表继续执行一个命令；
{} 代表 刚才查找出的结果；
最后一定要加上“\”

2. 将某个目录下的所有`.xml`文件中的某个字符串`str1`替换为另外字符串`str2`
```bash
$ find -name '*.xml' |xargs perl -pi -e 's|xxx|***|g'
```

3. `Argument list too long`
```bash
$ find xml/ -name "*.xml" -exec cp {} Annotations/ \;
$ find jpg/ -name "*.jpg" -exec cp {} JPEGImages/ \;
```

person
motorbike
train
sofa
horse
tvmonitor
sheep
pottedplant
