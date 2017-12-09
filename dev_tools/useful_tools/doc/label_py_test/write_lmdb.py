# -*- coding:UTF-8 -*-

'''
	功能： 将图片文件生成lmdb
	<从网上找的一个示例文件>
'''

# 我们可以采用cv2来读入自己的图像数据，采用datum格式来存储数据。
# Datum is a Google Protobuf Message class used to store data 
# and optionally a label. A Datum can be thought of a as a matrix 
# with three dimensions: width, height, and channel.
import lmdb
import numpy as np
import cv2
import caffe
from caffe.proto import caffe_pb2



# basic setting
# 这个设置用来存放lmdb数据的目录
lmdb_file = 'lmdb_data'
batch_size = 32

# create the lmdb file
# map_size指的是数据库的最大容量，根据需求设置
lmdb_env = lmdb.open(lmdb_file, map_size=int(1e12))
lmdb_txn = lmdb_env.begin(write=True)

# 因为caffe中经常采用datum这种数据结构存储数据
datum = caffe_pb2.Datum()

item_id = -1
for x in range(1000):
    item_id += 1

    #prepare the data and label
    
    #data = np.ones((3,64,64), np.uint8) * (item_id%128 + 64) #CxHxW array, uint8 or float
    # pic_path设置成图像目录, 0表示读入灰度图
    data = cv2.imread(pic_path, 0)
    # label 设置图像的label就行
    label = item_id%128 + 64

    # save in datum
    datum = caffe.io.array_to_datum(data, label)
    keystr = '{:0>8d}'.format(item_id)
    lmdb_txn.put( keystr, datum.SerializeToString() )

    # write batch
    if(item_id + 1) % batch_size == 0:
        lmdb_txn.commit()
        lmdb_txn = lmdb_env.begin(write=True)
        print (item_id + 1)


map_size = X.nbytes * 10

env = lmdb.open('mylmdb', map_size=map_size)

with env.begin(write=True) as txn:
    # txn is a Transaction object
    for i in range(N):
        datum = caffe.proto.caffe_pb2.Datum()
        datum.channels = X.shape[1]
        datum.height = X.shape[2]
        datum.width = X.shape[3]
        datum.data = X[i].tobytes()  # or .tostring() if numpy < 1.9
        datum.label = int(y[i])
        str_id = '{:08}'.format(i)

        # The encode is only essential in Python 3
        txn.put(str_id.encode('ascii'), datum.SerializeToString())

# write last batch
if (item_id+1) % batch_size != 0:
    lmdb_txn.commit()
    print 'last batch'
    print (item_id + 1)