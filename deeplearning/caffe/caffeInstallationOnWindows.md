 Windows+VS2013爆详细Caffe编译安装教程
标签： 图像处理visual studio机器视觉
2016-07-05 21:26 17485人阅读 评论(44) 收藏 举报
 分类： 深度学习（11）   Caffe
版权声明：本文为博主原创文章，未经博主允许不得转载。
目录(?)[+]
1. 安装cuda
    Cuda是英伟达推出的GPU加速运算平台
    我这里安装的是cuda7.5,已经安装过的忽略,还没有安装过的这里有安装教程.windows下面安装还是非常简单的.
    点击打开链接
2. 下载cuDNN(其实是个压缩包)
    https://developer.nvidia.com/cudnn
    用于深度学习加速.
 

    中间可能有个注册的过程,自己注册一下.(因为暂时不支持V5,所以下载V4或者v3)
                                                                                     
    然后点击下载就行


3. 从Microsoft官方Github上下载Caffe的源码压缩包.
    微软官方也移植了Caffe,在windows下面的配置会比较简单一点.
    https://github.com/Microsoft/caffe

4. 安装
    1.经过上面的步骤,cuda应该已经正确的安装好了.同时也应该有了两个压缩包:一个cuDNN的压缩包和一个Caffe的源码包.(我是放在D盘Caffe文件夹下面的)
                                         
    2.解压这两个文件包,得到
         
 
    3.打开caffe-master文件夹,然后看到一个windows文件夹,然后继续打开windows文件夹,看到里面一个CommonSettings.props.example文件,复制出来一份,并改名字为CommonSettings.props
                                            
    4.用VS2013打开Caffe.sln(就到刚刚那个文件夹里面),打开效果如下
                                                 
    5.打开CommenSetting.probs.在cudapath那里把存放解压的cudnn出来的文件夹cuda的目录路径放到其中去.(比如我这里是装在D:Caffe下面)
     
    6.打开设置,
                                                     
               选择编译类型为release,x64
                                                
                 关闭视警告为错误(即设置为No)如果不设置的话在编译boost库的时候会由于文字编码的警告而报错.
                                               
               然后就在开始编译,(先编译的是ibcaffe).等啊等.....就编译成功了
           这个时候,在你刚刚解压出来的caffe-master文件夹下面就应该出现一个名叫Build的文件夹,
                                             
                  以后编译的成功和运行需要的文件都会存放在build\X64\Relase下面.
                                              
                前面已经提示是先编译的libcaffe,在solution上面右击选择属性（properties）
               
                      可以发现默认选择的是单任务，只编译libcaffe。这就是为什么之前默认先编译libcaffe。
          
       接下来，可以选择更多的任务来编译。类似地，我们接下来也可以只选择caffe来编译，同样的要注意release和X64.
     然后类似的build等待。。。
     然后也编译成功啦。。。还有一些其他的需要的时候再编译。。
                                              

               注意:之所以选择微软官方的版本,是因为在编译过程中会自动下载一些依赖库.(见下图)不用手动选择,所以非常简单方便.
                                                  
