# `Ubuntu14.04`安装`TensorFlow1.0.1`
系统环境：Ｕbuntu14.04 64bit
Python版本：Python 2.7.8
TensorFlow版：TensorFlow 1.0.1

## 安装步骤   
1. 检查Python和pip的版本   
由于本系统之前已经安装了Python，这里只需要确认一下版本号。      
```
$ python -V
Python 2.7.8
```
```
$ pip -V
pip 9.0.1 from /usr/local/lib/python2.7/dist-packages (python 2.7)
```
２. 安装`virtualenv`   
```
$ sudo pip install virtualenv
Successfully installed virtualenv-15.1.0
```
3. 创建虚拟环境   
```
$ virtualenv --system-site-packages ~/tf101py27
New python executable in /home/tsiangleo/tf101py27/bin/python
Installing setuptools, pip, wheel...done.
```
上述命令在`~/tf101py27`目录下创建了一个虚拟环境。    
４. 激活（进入）虚拟环境   
```
$ source ~/tf101py27/bin/activate
(tf101py27) ~$
```
此时系统进入了虚拟环境，后续就在该虚拟环境下安装软件，和正常的安装过程一样。  
5. 安装`TensorFlow1.0.1`   
```
# Ubuntu/Linux 64-bit, CPU only, Python 2.7
(tf101py27) ~$ pip install --upgrade https://storage.googleapis.com/tensorflow/linux/cpu/tensorflow-1.0.1-cp27-none-linux_x86_64.whl
```
上面的命令可能会报错:`SSLError: [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed (_ssl.c:581)`.可先将`whl`包下载下来，然后进行安装，如下所示：      
```
(tf101py27)  ~$ wget https://storage.googleapis.com/tensorflow/linux/cpu/tensorflow-1.0.1-cp27-none-linux_x86_64.whl
(tf101py27)  ~$ pip install --upgrade tensorflow-1.0.1-cp27-none-linux_x86_64.whl
```
这个过程要持续一段时间，系统会下载相关的依赖包，比如`numpy`等。        
安装成功后输出：`Successfully installed numpy-1.12.1 pbr-3.0.0 protobuf-3.2.0 tensorflow-1.0.1`     
６. 验证`TensorFlow`是否安装成功         
```
(tf101py27)  ~/tf101py27$ python
Python 2.7.8 (default, Jun 18 2015, 18:54:19) 
[GCC 4.9.1] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> import tensorflow as tf
>>> hello = tf.constant('Hello, TensorFlow!')
>>> sess = tf.Session()
>>> print(sess.run(hello))
```
７. 退出虚拟环境    
```
(tf101py27)  ~/tf101py27$ deactivate
```