# -*- coding:UTF-8 -*-
'''
	功能：用于生成caffe训练使用的train-lmdb数据库文件。具体处理如下：将3帧连续的数据进行拼接后扩展为1*9channels，然后作为一个datum保存到lmdb中。
	
	注意：
		1. 这里只有生成训练阶段的数据集，也就是说在之后的solver.prototxt和train.protxt中关于TEST的参数和层都需要去掉。
		2. datum的label不能存储字符串，只能是数字(long)，因此可以使用类别目录中添加数字后缀。
'''

# LMDB from Python
# You will need the Python package lmdb as well as Caffe’s 
# python package (make pycaffe in Caffe). LMDB provides key-value 
# storage, where each <key, value> pair will be a sample in our dataset. 
# The key will simply be a string version of an ID value, and the value
# will be a serialized version of the Datum class in Caffe (which are built
# using protobuf).

import numpy as np
# sys.path.insert(0, 'E:/win-caffe-master/python')
# sys.path.insert(0, 'E:/win-caffe-master/Build/x64/Release/pycaffe')
import caffe  
import lmdb  
from caffe.proto import caffe_pb2 
import random
import os
import cv2
import sys
reload(sys)  
sys.setdefaultencoding('gbk')

def readImages(dir):
	# 列出更目录下的20个文件夹
	if not os.path.exists(dir):
		print ("error: No such directory exists: %s." %(dir))
		return 0
	# os.chdir(dir)

	imgsAll = []
	print "Reading images...."
	for d in os.listdir(dir):
		# 加上前面的路径
		d = dir + d
		if os.path.isdir(d):
			if d == 'mylmdb':
				os.removedirs('mylmdb'.encode('gbk'))
			pre = d.split('_')
			if len(pre) == 2:
				label = pre[1]
			
			imgs = os.listdir(d)
			# 读取图片
			for i in range(0, len(imgs), 3):
			    image1 = cv2.imread((d+'/'+imgs[i]).encode('gbk'))
			    image2 = cv2.imread((d+'/'+imgs[i+1]).encode('gbk'))
			    image3 = cv2.imread((d+'/'+imgs[i+2]).encode('gbk'))

			    image1 = cv2.resize(image1,(224, 224), interpolation=cv2.INTER_CUBIC)
			    image2 = cv2.resize(image2,(224, 224), interpolation=cv2.INTER_CUBIC)
			    image3 = cv2.resize(image3,(224, 224), interpolation=cv2.INTER_CUBIC)

			    # cv2.imshow('show1',image3)
			    # cv2.waitKey()
			    image = np.concatenate((image1, image2, image3), axis=2)
			    imgsAll.append([image, label])

	return imgsAll

'''
创建lmdb函数
	imgs就是我们要保存的数据
	空数据可以这样定义：X = np.zeros((N, 3, 32, 32), dtype=np.uint8)
'''
def createLmdb(imgs):
	# 打乱顺序
	random.shuffle(imgs)
	N = len(imgs)

	# map_size指的是数据库的最大容量，一般设置为理论值的10倍。当然设置的很大也没有什么坏处。
	# map_size = imgs[0][0].shape[0]*imgs[0][0].shape[1]*imgs[0][0].shape[2]*len(imgs)*10
	map_size=int(1e10)

	env = lmdb.open('mylmdb', map_size=map_size)

	with env.begin(write=True) as txn:
		# txn is a Transaction object
		for i in range(N):
			datum = caffe.proto.caffe_pb2.Datum()
			datum.channels = imgs[i][0].shape[2]
			datum.height = imgs[i][0].shape[0]
			datum.width = imgs[i][0].shape[1]
			datum.data = imgs[i][0].tobytes()  # or .tostring() if numpy < 1.9
			datum.label = int(imgs[i][1])
			str_id = '{:08}'.format(i)
			# print datum.channels,datum.height,datum.width

			# The encode is only essential in Python 3
			txn.put(str_id.encode('ascii'), datum.SerializeToString())
	print 'Create Lmdb Done...'
	print '######################################'

if __name__ == '__main__':
	imgs = readImages('drinks/')
	print len(imgs)
	createLmdb(imgs)


# # You can also open up and inspect an existing LMDB database from Python:

# import numpy as np
# import lmdb
# import caffe

# env = lmdb.open('mylmdb', readonly=True)
# with env.begin() as txn:
#     raw_datum = txn.get(b'00000000')

# datum = caffe.proto.caffe_pb2.Datum()
# datum.ParseFromString(raw_datum)

# flat_x = np.fromstring(datum.data, dtype=np.uint8)
# x = flat_x.reshape(datum.channels, datum.height, datum.width)
# y = datum.label
# Iterating <key, value> pairs is also easy:

# with env.begin() as txn:
#     cursor = txn.cursor()
#     for key, value in cursor:
#         print(key, value)