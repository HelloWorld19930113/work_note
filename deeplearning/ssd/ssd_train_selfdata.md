# SSD框架训练自己的数据集

SSD demo中详细介绍了如何在VOC数据集上使用SSD进行物体检测的训练和验证。本文介绍如何使用SSD实现对自己数据集的训练和验证过程，内容包括：
1 数据集的标注
2 数据集的转换
3 使用SSD如何训练
4 使用SSD如何测试

## 1 数据集的标注　
 数据的标注使用`label标注`工具。
该工具生成的标签格式是:
```xml
object_number
className x1min y1min x1max y1max
classname x2min y2min x2max y2max
```
##2 数据集的转换
 caffe训练使用LMDB格式的数据，ssd框架中提供了voc数据格式转换成LMDB格式的脚本。所以实践中先将`label标注`的数据转换成voc数据格式，然后再转换成LMDB格式。

2.1 voc数据格式
一共有4个文件夹，分别是：`Annotations、ImageSets/Main、JPEGImages、label`
(1)Annotations中保存的是`xml`格式的`label信息`,文件内容如下：
```
<?xml version="1.0" ?>
<annotation>
	<size>
	 	<width>
	 		800
	 	</width>
	 	<height>
	 		600
	 	</height>
	 </size>
	<object>
		<name>bld004</name>
			<bndbox>
	            <xmin>498</xmin>
	            <ymin>350</ymin>
	            <xmax>586</xmax>
	            <ymax>410</ymax>
			</bndbox>
	</object>
</annotation>
```
(2)`ImageSet`目录下的`Main`目录里存放的是用于表示训练的图片集和测试的图片集
 训练完成后需要测试训练后的效果。
(3)`JPEGImages`目录下存放所有图片集
(4)`label`目录下保存的是`label标注工具`标注好的bounding box坐标文件，
该目录下的文件就是待转换的label标签文件。
>图片标注工具标注后可以输出`.txt`文件或(和)`.xml`文件。如果输出的是`.txt`文件，那么接下来还需要将此`.txt`文件转换为`.xml`格式的文件。

2.3 `VOC`数据转换成`LMDB`数据
 SSD提供了VOC数据到LMDB数据的转换脚本文件：`data/VOC0712/create_list.sh` 和 `./data/VOC0712/create_data.sh`，这两个脚本是完全针对VOC0712目录下的数据进行的转换。因此，实现中为了不破坏`VOC0712`目录下的数据内容，针对我们自己的数据集拷贝这两个文件，修改这两个脚本，将脚本中涉及到`VOC0712`的信息替换成我们自己的目录信息。
在处理我们的数据集时，将`VOC0712`替换成`indoor`。
具体的步骤如下：
(1) 在 `$HOME/data/VOCdevkit`目录下创建`indoor`目录，该目录中存放自己转换完成的`VOC数据集`；
(2) `$CAFFE_ROOT/examples`目录下创建`indoor目录`；
(3) `$CAFFE_ROOT/data`目录下创建`indoor`目录，同时将data/VOC0712下的create_list.sh,create_data.sh,labelmap_voc.prototxt
这三个文件copy到indoor目录下，分别重命名为create_list_indoor.sh,create_data_indoor.sh, labelmap_indoor.prototxt
(4)对上面新生成的两个create文件进行修改，主要修改是将VOC0712相关的信息替换成`indoor`。修改后的这两个文件分别为：


