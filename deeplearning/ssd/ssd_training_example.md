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
(图片被我归一化到300*300，实际上并不需要完全一样，可以允许不一样的尺寸)
3.在服务器中的`home`目录中有一个`data`目录，这个目录中存放的都是原始的标注数据。我们在这个目录下新建一个`xwk`目录，并新建以下三个文件夹，其中`ImageSets`中包含一个子目录`Main`：
```
Annotations         # 这个放标注信息文件xml的，画框等标记位置信息
ImageSets/Main      # 数据集相关信息保存在Main子目录，定义训练和测试等
JPEGImages          # 这里放图片文件的
```
4.拷贝训练所需的数据文件
(1)把`.xml`全部复制到`Annotations`里面;
(2)把`.jpg`复制到`JPEGImages`;
(3)然后运行`python make-list-file.py`命令，会产生`trainval.txt`和`test.txt`两个文本文件，将这两个文件复制到`ImageSets/Main`目录下;
5.进入到`{CAFFE_ROOT}/caffe/data`，执行下面的命令：
```bash
cp VOC0712/ xwk/ -R
```
这样就在该目录下按照`VOC0712`复制得到一个`xwk`(这个名字必须跟上面的`xwk`一样)。
接下来打开`xwk`目录下的`labelmap_voc.prototxt`，并修改类别`1`为`st`(这个名字必须跟`lab.py`里面指定的名字相匹配，这个名字最终会被保存在xml文件的<name></name>域中)
```
item {
  name: "st"
  label: 1
  display_name: "st"
}
```
6.打开`create_list.sh`，作如下修改：
```
# 修改第3行的`root_dir`指向我们保存标注数据的目录（即新建`xwk`目录时所在目录）
root_dir=$HOME/data
# 修改第13行
for name in VOC2007 VOC2012  
# 修改为
for name in xwk
```
在终端中执行`./create_list.sh`创建列表
```
$ ./create_list.sh
```
可以看到`xwk`目录下多了几个文件。
7.打开`create_data.sh`，作如下修改：
```
# 修改第7第8行
data_root_dir="$HOME/data/VOCdevkit"
dataset_name="VOC0712"
# 为
data_root_dir="$HOME/data"
dataset_name="xwk"
```
然后在终端执行（caffe目录下跟第6步一样）：
```
$ ./create_data.sh
```
哐当，会报错，提示`caffe.proto`:
我们去`caffe/scripts/create_annoset.py`找到第6行，加入一个路径(这个路径是`caffe`工程中的`python`目录,可能根据不同的工程目录稍微进行微调):
```
sys.path.insert(0, "$HOME/work/gitwork/caffe/python")
from caffe.proto import caffe_pb2
from google.protobuf import text_format
```
再来一遍：
```
$ ./create_data.sh
```
提示：Processed 256 files.完成了
8.我们到`caffe/examples/ssd`里面，复制`ssd_pascal.py`为`ssd_pascal_xwk.py`，并打开:
同样的，在文件头部我们插入路径
```
from __future__ import print_function
import sys
sys.path.insert(0, "/home/eric/work/gitwork/ssd/caffe/python")
import caffe
# 然后修改大约再85行
train_data = "examples/VOC0712/VOC0712_trainval_lmdb"
# 为:
train_data = "/home/eric/data/xwk/lmdb/xwk_trainval_lmdb"
# 然后修改：
test_data = "examples/VOC0712/VOC0712_test_lmdb"
# 为
test_data = "/home/eric/data/xwk/lmdb/xwk_test_lmdb"
# 修改大于335行，设置使用的gpu编号
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
name_size_file = "/home/eric/data/xwk/test_name_size.txt"
# The pretrained model. We use the Fully convolutional reduced (atrous) VGGNet.
pretrain_model = "/home/eric/data/VGG_ILSVRC_16_layers_fc_reduced.caffemodel"
# Stores LabelMapItem.
label_map_file = "data/xwk/labelmap_voc.prototxt"
# 这个预训练模型下载地址是：http://cs.unc.edu/~wliu/projects/ParseNet/VGG_ILSVRC_16_layers_fc_reduced.caffemodel
```
修改完之后保存退出。
9.终端在`caffe`目录下，执行：
```
$ sudo python examples/ssd/ssd_pascal_xwk.py
```
就开始训练啦
10.调用`ssd`模型做识别
目录下的`callssd.py`就是一个简单的演示例子
**注意**
当你自己训练模型完毕后，从`models`里面复制出来的`deploy.prototxt`文件底部设置了一个指向一个`txt文件`的路径。这个是没有必要的，可以删掉这个参数，可以参照一下目录下面我放的`deploy.prototxt`这个文件。
## 问题及解决方案：
2. 将某个目录下的所有`.xml`文件中的某个字符串`str1`替换为另外字符串`str2`
```bash
$ find -name '*.xml' |xargs perl -pi -e 's|<name>st</name>|<name>xwk</name>|g'
```

3. `Argument list too long`
```bash
$ find xml/ -name "*.xml" -exec cp {} Annotations/ \;
$ find jpg/ -name "*.jpg" -exec cp {} JPEGImages/ \;
```
4. log
```bash
nohup python examples/ssd/ssd_xwk.py > xwk.log 2>&1 &
```

5. 关闭test 
```
#'test_iter': [test_iter],
#'test_interval': 10000,
...
#'test_initialization': False,
...
#test_net=[test_net_file],
```






person
motorbike
train
sofa
horse
tvmonitor
sheep
pottedplant
