CUDA 8.0 + VS2013 + win7 x64��������� 
2017-06-29 17:30 512���Ķ� ����(0) �ղ� �ٱ� 
 ���ࣺ 
����ѧϰ��5��  
���Ĳο��������������ĵ������岻��һ������л��ǰ���Ƕ���ǿ���ָ���ƣ������Լ��ľ�������������޸Ļ���Ѫ��֮��������������ʹ�ã�ע�⣬���񣬸��־Ͳ�ҪϹBB�ˡ�
�������ܼ򵥵����飬������Ϊ���֣��������Ȼ�������׼Һü��죬��Щ�����ģ�

��װVS2013��
�׼ҵ�VS2013Ϊ���İ档
��ʵ�����Ǹ������VS2013��֪������ȱ�ļ����������ã�VS2015�������汾��֧��cross��Ҳ�����ˣ�װ��VS2015��רҵ�棬��֪����������⣬��Ȼ�����涼����ȥ���е��ŭ��feel����

��װcuda_8.0��?
��������ֱ��next��װ����Ҫ��ס��װ��·������������ǰ�İ汾��sdk��doc��toolkit������װ���֣��µı����Samples��doc��Development����ʵľ��̫������Samples=sdk��
��Ȼ����Ϊ������ң���һ�ΰ�װ��ʱ�򣬸���������ʷ�汾�Ľ̳̾�Ȼ��֪���ĸ����ĸ�����Ҫע����ǣ������װ����Ҫж��һ��Ҫж�ظɾ���ע���Ҫɾ�ˣ���Ȼ�´�һ����ʾ��libraryĿ¼��װ���ˣ�

���û���������
��װ��Ϻ��ڼ�����ϵ��Ҽ���������->�߼�ϵͳ����->�������������Կ���ϵͳ�ж���CUDA_PATH��CUDA_PATH_V8_0������������������������Ҫ��ϵͳ��������¼�������������
����CUDA_SDK_PATH = C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0
����CUDA_LIB_PATH = %CUDA_PATH%\lib\x64
����CUDA_BIN_PATH = %CUDA_PATH%\bin
����CUDA_SDK_BIN_PATH = %CUDA_SDK_PATH%\bin\win64
CUDA_SDK_LIB_PATH = %CUDA_SDK_PATH%\common\lib\x64
�׼ҵİ�װ·����Ĭ�ϵģ�������ӵ�·���ֱ������������ģ�
CUDA_PATH
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0
CUDA_PATH_V8_0
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0
CUDA_SDK_PATH
C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0
CUDA_LIB_PATH
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64
CUDA_BIN_PATH
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin
CUDA_SDK_BIN_PATH
C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\bin\win64
CUDA_SDK_LIB_PATH
C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\common\lib\x64
Ȼ����ϵͳ���� PATH ��ĩβ��ӣ�
��;%CUDA_LIB_PATH%;%CUDA_BIN_PATH%;%CUDA_SDK_LIB_PATH%;%CUDA_SDK_BIN_PATH%;
��C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib\x64��C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin��C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\common\lib\x64��C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\bin\win64��
���������������
���ˣ�cuda�İ�װ�͸㶨�ˡ�

���cuda��װ�ɹ����:
��������õ���������������cudaΪ����׼���õ�: deviceQuery.exe �� bandwithTest.exe
��������cmd DOS�����
Ĭ�Ͻ�������c:\users\Admistrator>·�������� cd .. ���Σ�����c:Ŀ¼��
����dir �ҵ���װ��cuda�ļ���

ֱ��ִ��bandwidthTest.exe
���ļ���·����C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\extras\demo_suite

��ִ��deviceQuery.exe
���ļ���·����C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\extras\demo_suite


Rsult=PASS��˵������ͨ���ˡ����Rsult=Fail �ǲ�����˼�����°�װ�ɡ�

����VS 2013
����5.1 ����VS2013
5.2 �½�һ��win32�Ŀ���̨���̣��յġ�

�׼ҵı���·����c:\users\lvfeiya\documents\visual studio 2013\Projects

5.3 �Ҽ�Դ�ļ��ļ���->�½���->ѡ��cuda c/c++- Header>�½�һ����.cu��β���ļ�

����·����
c:\Users\lvfeiya\documents\visual studio 2013\Projects\sevenCuda\sevenCuda\
����5.4 �Ҽ�sevenCuda-������������-�������Զ���-��ѡ��cuda����

����5.5 �Ҽ�test.cu-������-��ѡ��cuda c/c++������

5.6 �Ҽ�����-������-��������-������-�����ӿ�Ŀ¼-�����Ŀ¼ $(CUDA_PATH_V5_5)\lib\$(Platform);
C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\lib(Platform)
$(CUDA_PATH_V8_0)\lib\$(Platform);
���׼�Ҳ��֪������������Ļ�������ĸ�ʽ�����������׼���ӵ�������ģ�

����5.7 ��������-����������� cudart.lib

����5.8 �ڹ���-��ѡ��-���ı��༭��-���ļ���չ��-�����cu \cuh�����ļ���չ��
�����������VS2010�༭.cu�ļ�ʱ����.cu�ļ����C/C++�﷨��������VS2013�Ĳ˵� ���ε����Tools->Options��->Text Editor->File Extension�����.cu��.cuh��ʹ֮����C++�༭����

���ˣ����뻷������ش������ˡ�

CUDA����������
```cpp
#include< stdio.h> 
#include "cuda_runtime.h" 
#include "device_launch_parameters.h" 
bool InitCUDA()
{
	int count;
	cudaGetDeviceCount(&count);
	if (count == 0)
	{
		fprintf(stderr, "There is no device.\n");
		return false;
	}
	int i;
	for (i = 0; i < count; i++)
	{
		cudaDeviceProp prop;
		if (cudaGetDeviceProperties(&prop, i) == cudaSuccess)
		{
			if (prop.major >= 1)
			{
				break;
			}
		}
	}
	if (i == count)
	{
		fprintf(stderr, "There is no device supporting CUDA 1.x.\n");
		return false;
	}
	cudaSetDevice(i);
	return true;
}

int main()
{
	if (!InitCUDA())
	{
		return 0;
	}
	printf("HelloWorld, CUDA has been initialized.\n");

	return 0;
}
```

## ��װcudnn
��װcudnn�Ƚϼ򵥣��򵥵�˵�����Ǹ��Ƽ����ļ������ļ���ͷ�ļ�����cudnn��ͷ�ļ����Ƶ�cuda��װ·����include·���£���cudnn�Ŀ��ļ����Ƶ�cuda��װ·����lib64·���¡�
#��ѹ�ļ�
tar -zxvf cudnn-6.5-linux-x64-v2.tgz
#�л�·��
cd cudnn-6.5-linux-x64-v2
#����lib�ļ���cuda��װ·���µ�lib64/
sudo cp lib* /usr/local/cuda/lib64/
#����ͷ�ļ�
sudo cp cudnn.h /usr/local/cuda/include/

#����������
cd /usr/local/cuda/lib64/ 
sudo rm -rf libcudnn.so libcudnn.so.6.5 
sudo ln -s libcudnn.so.6.5.48 libcudnn.so.6.5 
sudo ln -s libcudnn.so.6.5 libcudnn.so 
��ĿǰΪֹ��cudnn�Ѿ���װ���ˡ����ǣ��Ƿ�װ�ɹ����أ�����ͨ�������cudnn sample���ԡ�
#����cudnn-sample-v2
tar �Czxvf cudnn-sample-v2.tgz
cd cudnn-sample-v2
make
./mnistCUDNN
#�ĳ������гɹ���˵��cudnn��װ�ɹ���

��ʱ���ܳ��ִ���./mnist CUDNN: error while loading shared libraries: libcudart.so.6.5: cannot ope
��������ο� https://groups.google.com/forum/#!topic/caffe-users/dcZrE3-60mc























��װAnaconda
Anaconda������Theano����ĸ���Python�⣬��������װpython������ȷ�ǳ����㣬��װ��ʱ��ѡ��Anaconda�ӵ����������С�Anaconda��λ��Ҫ�����cuda�汾һ����ѡ��Ķ���64 bit��
��װĿ¼���õ�E:\Anapy2��
��װ����һ�£���cmd�����У�

װ���Ժ���ż���һ����
��֤BLAS�Ƿ�װ�ɹ�
����numpy������BLAS�ģ����BLASû�а�װ�ɹ�����Ȼnumpy����԰�װ�������޷�ʹ��BLAS�ļ��١���֤numpy�Ƿ�ɹ�����BLAS���룺
python
import numpy
id(numpy.dot) == id(numpy.core.multiarray.dot)
False
���ΪFalse��ʾ�ɹ�ʹ��BLAS���٣������Ture���ʾ�õ���python��ʵ�֣�û�м��١� ״��������Ļ���֮��theano����һЩ���ӻᱬ����cannot find -lf77blas-lcblas-latlas�Ĵ���.�򵥴�����װ
���׼������Ȼ��ôŪ����false��~~(>_<)~~���̳��ﶼ��ƭ�˵ģ��׼����ˣ���װ��Ч��
Ҳ���Կ���MKL���շѵģ���OpenBLAS��
��������������һЩ��װ����Ĳ��ͺ���������
http://ijiaer.com/python-with-mkl-hpc/��mkl��
https://www.kaggle.com/c/otto-group-product-classification-challenge/forums/t/13973/a-few-tips-to-install-theano-on-windows-64-bits/93135��OpenBLAS��

��װMingW
ò��Anaconda-2.֮ǰ�İ汾��������MinGW�����ģ�֮��Ͳ���MinGW�ˡ����鿴C:\Anaconda������MinGWĿ¼��
��װ������
��CMD��ע����windows������ʾ���������ǽ��뵽python�����£���
����conda install mingw libpython��Ȼ��س�������ְ�װ���ȣ��Ե�Ƭ�̼��ɰ�װ��ϡ���ʱ����C:\Anaconda\MinGwĿ¼�ˡ� ������һ��Ҫ�ã����ٲ��û����һ����false��
����windows������������g++ -v���鿴�Ƿ�װ�ɹ���

��������
1). �༭�û������е�path�������ں��׷��C:\Anaconda;C:\Anaconda\Scripts; ��Ҫ©���ֺţ��˴���Ҫ�����Լ��İ�װĿ¼��д��
�׼ҵ�·����E:\Anapy2; E:\Anapy2\Scripts
2). �����ϵͳ�������������path�м��� C:\Anaconda\MinGW\bin;C:\Anaconda\MinGW\x86_64-w64-mingw32\lib���ɡ�
E:\Anapy2\MinGW\bin; E:\Anapy2\MinGW\x86_64-w64-mingw32\lib
��ϵͳ�������½�����PYTHONPATH������ֵΪC:\Anaconda\Lib\site-packages\theano;
PYTHONPATH
E:\Anapy2\Lib\site-packages\theano;
�˴�����ָ����װ��theano��Ŀ¼���ģ�theano���ں��氲װ��������װ��Ĭ��·�����������
3). ��cmd���ῴ����������и�·��C:\Users\FYB>����homeĿ¼���ڴ�Ŀ¼���½� .theanorc.txt �ļ�����Ϊtheano�������ļ���ע�������еĵ�һ����.��������Ѿ����ڣ���ֱ���޸ĸ��ļ����������������ݣ�
��νcmd��homeĿ¼����cmdʱ����>ǰ���Ĭ��·����

[global]
openmp=False
[blas]
ldflags =
[gcc]
cxxflags = -ID:\Anaconda\MinGW\include
cxxflags = -IE:\Anapy2\MinGW
һ��Ҫ���㰲װ��Anaconda��·����һ����ҪŪ�������Ҳ���MinGw��
�������ԣ�
����theano�Ƿ�װ�ɹ�
���Է���1 ��
import theano
print theano.config.blas.ldflags

û�г���(û�з���ֵ)��˵���Ѿ����óɹ���
��ʵ������import theano��������Ѿ�л��л���ˡ�
���Է���2
�������ָ����ԣ�����ʱ��������������ʾ����warnings���������ϻ������еĻ���˵��theanoû���⣬������ʾ��������Щ������û��װ�ã���
import theano
theano.test()
���У�

����ʱʾû��nose-parameterized���ģ�飬��װ������
pip install nose-parameterized
ע�⣺����2������cpu�����У����������theano��device = gpu�������2�Ͳ��������ˡ�

ʹ��GPU
�����theano����ֻ��������ϰ벿�֣����ʱ�򻹲��ܽ���gpu���١����ʹ��GPU����Ҫ�������²��裺
Theano�ļ�����(GPU)�����༭Theano�������ļ�.theanorc.txt , ����������ݣ�
[global]
openmp = False
device = gpu
floatX = float32
allow_input_downcast = True
base_compiledir = path_to_a_directory+without_such_characters
[blas]
ldflags =
[gcc]
cxxflags=-IE:\Anapy2\MinGW
[nvcc]
fastmath = True
flags = -LE:\Anapy2\libs
compiler_bindir = H:\VS2013\VC\bin
[lib]
cnmem=.75
��Python�����С�import theano.sandbox.cuda��. ��������һ��Cuda�ļ�, Ӧ��û�д��������
�����Ƿ�ʹ��GPU
���Է���1��
import theano

���������ֺ���˵���Ѿ����ú��ˣ�
�����CNMeM is disabled��������Ϊ��û����Theano�������ļ�.theanorc.txt�������
[lib]
cnmem=.75
�����cuDNN�����ã�����Ҫȥ��������cuDNN����Ҫ��ע�ᣬ�����Ժ�ѽ�ѹ����������ļ��ֱ���ӵ�C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0������Ӧ��bin��lib��include�ļ��������ֵ�����û�����֮ǰ��û�г������������bug�;���ģ�����Ժ�ͳ��־����ˣ������޽⣬ǰ���Ƕ�˵�����Ӱ�����У�Ȼ�󱦱�ÿ�ο�����Щ���о�����༫�ˣ�
����Ŀ��GPUָ�������ѡ��
-arch: ָ��nvcc����Ŀ��GPU���ͺţ�����Ϊ��real�� GPU��Ҳ����Ϊ��virtual�� PTX�ܹ������ѡ��ָ����nvcc���뵽PTX�׶ε�Ŀ��ܹ�����-codeѡ��ָ����nvcc���׶����ɵ�����ʱ�����Ŀ��ܹ����ֽ׶�֧�ֵļܹ������У�virtual �ܹ� compute_10, compute_11, compute_12, compute_13��ʵ����Щ��ܹ���real GPU sm_10, sm_11, sm_12, sm_13.
���Է���2��
����Spyder�������Theano GPU���ٲ��԰������벢����
 #!/usr/bin/env python
 # -- coding: utf-8 --

 ������
 ���ܣ������Ƿ�ʹ��GPU
 ʱ�䣺2016��6��10�� 11:20:10
 ������

 from theano import function, config, shared, sandbox
 import theano.tensor as T
 import numpy
 import time

 vlen = 10*30*768 # 10 x cores x threads per core
 iters = 1000

 rng = numpy.random.RandomState(22)
 x = shared(numpy.asarray(rng.rand(vlen), config.floatX))
 f = function([], T.exp(x))
 print(f.maker.fgraph.toposort())
 t0 = time.time()
 for i in range(iters):
 r = f()
 t1 = time.time()
 print(��Looping %d times took�� % iters, t1 - t0, ��seconds��)
 print(��Result is��, r)

 if numpy.any([isinstance(x.op, T.Elemwise) for x in f.maker.fgraph.toposort()]):
 print(��Used the cpu��)
 else:
 print(��Used the gpu��)
���������ͼ��ʾ�����н����������һ����Used the gpu����Ϣ���������Theano GPU���ٲ��Գɹ���

��ľ�з����������ڴ�ܸߣ��ر�spyder���ٴӵ������½ǵĳ�������򿪣���Ȼ���ز���������kernal failed���ǽ���ȥ���棬��֪����ʲô�۶��ӣ�����cmd������ж����װ��Ȼ����ľ�����ã����ǿ��Դ�Anaconda�İ�װ�ļ����ҵ�spyder.exe�ļ���·���ǣ�E:\Anapy2\Scripts�������Ϳ�ݷ�ʽ�������ٴ򿪣��㶨���׼ҹ�������Ϊǰ����װ�˺ü���Anaconda������ע���ɶ�ӵ�ľ��ɾ����ȫ��
����ʣ��������Ӱ��ʹ�õ�bug:
1�� ����ʹ��blas���٣�
2�� Python���е�ʱ�����ʾDEBUG: nvcc STDOUT nvcc warning.
Bingo ������