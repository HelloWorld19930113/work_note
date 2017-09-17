其实在openpose还没有出来之前就一直关注CMU的工作，他们模型的效果很好，并且取得了较好的鲁棒性，特别是人被遮挡了一部分还是能够估计出来，我想这一点其实也说明较大的数据所取得的鲁棒性真的很好，但是计算量也很可观。openpose中包含了对人脸关键点的定位，人手的关键点的定位以及人身体的各个关节的定位。openpose是基于CVPR 2016 Convolutional Pose Machine（CPM）和CVPR2017 realtime multi-person pose estimation以及CVPR2017  Hand Keypoint Detection in Single Images using Multiview Bootstrapping这3篇paper的模型做出来的（脸的没看到有相关的论文）。

下面可以给大家粗略地讲讲这些模型：

(1) CPM的算法思想（单人姿态估计）
 CPM的模型采用的`大卷积核`来获得大的感受野，这对于推断被遮挡的关节是很有效的。网络结构如下
<img src="https://pic4.zhimg.com/v2-d98e358ee111b69abd4c1ad3d9e9594b_b.png" data-rawwidth="849" data-rawheight="504" class="origin_image zh-lightbox-thumb" width="849" data-original="https://pic4.zhimg.com/v2-d98e358ee111b69abd4c1ad3d9e9594b_r.png">

整个算法的流程是：
1）首先对图像的所有出现的人进行回归，回归各个人的关节的点
2）然后根据`center map`来去除掉对其他人的响应
3）最后通过重复地对预测出来的`heatmap`进行`refine`，得到最终的结果在/再进行`refine`的时候，需要引入中间层的`loss`，从而保证较深的网络仍然可以训练下去，不至于梯度弥散或者爆炸。这种思路很好，通过`coarse to fine`来逐渐提升回归的准确度。

(2) `realtime multi-person pose estimation`的算法思想（多人姿态估计）
  该算法其实是基于`CPM`来构造的，所以计算量也很可观这就是为什么在上述回答中有些人说那么吃显卡。
  网络的结构如下:
  <img src="https://pic3.zhimg.com/v2-ce656e3655aaf3f8a82f20b844e069e6_b.png" data-rawwidth="705" data-rawheight="449" class="origin_image zh-lightbox-thumb" width="705" data-original="https://pic3.zhimg.com/v2-ce656e3655aaf3f8a82f20b844e069e6_r.png">
  该网络的结构与`CPM`其实很类似，也是通过`CPM`的方式先将一幅图中所有人的关节点都回归出来，此外还同时回归出`part affine field(PAF)`，什么是`part affine field`？看了下面的图你就明白了。
  <img src="https://pic2.zhimg.com/v2-937c57296d66cc8a1c99f179ae3929dd_b.png" data-rawwidth="648" data-rawheight="476" class="origin_image zh-lightbox-thumb" width="648" data-original="https://pic2.zhimg.com/v2-937c57296d66cc8a1c99f179ae3929dd_r.png">
  实际上就是两个关节之间的连接所产生的`heatmap`。为什么要提`PAF`?因为本文的方法使用的是自底向上的方法即先回归出所有人的关节点，然后再对这些关节点进行划分，这样就可以把关节分配到每个人。至于怎么划分，就是通过`PAF`来进行划分，具体的划分算法这里不细讲，感兴趣的可以看论文。

(3) `Hand Keypoint Detection in Single Images using Multiview` Bootstrapping（手的姿态估计）CMU自己建了一个采集人的姿态的球（当然他们手的姿态也在里面采集），看看下面的图你就知道了
  <img src="https://pic2.zhimg.com/v2-19c6ec73687e421769de914747c21495_b.png" data-rawwidth="635" data-rawheight="358" class="origin_image zh-lightbox-thumb" width="635" data-original="https://pic2.zhimg.com/v2-19c6ec73687e421769de914747c21495_r.png">他们手的姿态的数据集就是在这个球里面采集（CMU Panoptic Dataset）这玩意有厉害，看看下面的参数，你们可以感受一下：Massively Multiview System480 VGA camera views30+ HD views10 RGB-D sensorsHardware-based syncCalibration看完之后，其实我想说，对DL来说数据真的很重要，这么鲁棒的模型，全是数据训出来的。。PS：这里还没更 hand是怎么具体做的。（4）其他的单人姿态估计方法其实除了CPM还有其他的比较好的姿态估计方法，最经典的就是ECCV 2016的stacked hourglass模型，该模型也取得了较为不错的效果。此外还有一些脑洞大开的方法：比如对关节与关节之间的关系进行建模的ECCV 2016 Chained Predictions Using Convolutional Neural Networks。这里并没有使用图模型来进行建模，而是直接用CNN来进行建模。比如使用GAN来对关节进行建模的方法，国立清华大学的这篇paper：Self Adversarial Training for Human Pose Estimation，该方法是基于hourglass和BEGAN来进行对抗训练，取得了较好的结果。（5）总结总的说来其实姿态估计的玩法还是很多样，目前的方法其实算法复杂度还是太高，距离使用还有一点点距离要走。此外，从2D到3D的姿态估计也是未来的一个趋势，2017的SIGGraph有一篇VNect: Real-time 3D Human Pose Estimation with a Single RGB Camera，这篇还是很靠谱的。不过这个方向对于研究者而言，面对的挑战也越来越大。其实姿态估计一直面临一个无法解决的问题那就是遮挡，不管怎么样，这是一个很大的挑战。我相信随着GAN的发展，遮挡问题肯定是可以被解决的。（6）很多人都在用OpenPose，如果想使用Python版本的 Openpose可以参考我的文章：PyOpenPose编译与使用 - 一亩半分地 - CSDN博客编译这玩意着实碰了不少坑，希望后来人少走弯路，这玩意对opencv的版本和protobuf的版本有要求。关于姿态估计可以看我的另外一个回答：目前姿态估计（pose estimation）有哪些主流方法？做姿态估计的同行可以加群讨论，自己一个人做研究没有人讨论也是挺苦逼的。。。【姿态估计】(qq群号650059258)