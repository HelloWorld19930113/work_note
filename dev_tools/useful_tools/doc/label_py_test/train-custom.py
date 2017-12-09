# -*- coding: UTF-8 -*-
'''
	功能： 自定义caffe训练
		1. 将图像数据保存到List中；
		2. 将连续的3帧图像数据合并为1*9channels
		3. 将这些数据打乱后输入到网络的data blobs中开始训练
'''
from __future__ import division
import sys 
caffe_root = '/home/lsn/Software/caffe/'
sys.path.insert(0, caffe_root + 'python')
import caffe
import random
from random import shuffle
import numpy as np
import os
import cv2
import time
reload(sys) 
sys.setdefaultencoding('utf8')

jpgPath = 'entire'
txtPath = 'txt'
# solver_path =  'E:/data/entry_wangs/solver.prototxt'
solver_path =  'solver.prototxt'
# caffe.set_mode_gpu()
# caffe.set_device(1)
# solver = caffe.SGDSolver('solver7.prototxt', ) 
#solver.restore('./Models/RNwithNeg1/FJSC_RN_iter_'+str(i)+'.solverstate')

# 载入截取的图片区域：Rect(x, y, width, height)
def loadROIFromTxt(name):
	if not os.path.exists(name):
		print 'error: No such file(s) exist: %s.' %name
		return []

	with open(name, "r") as txt:
		info = txt.readline().split(",")
		x = int(info[1])
		y = int(info[0])
		w = int(info[3])
		h = int(info[2])
		if x == 0 and y == 0 and w == 0 and h == 0:
			return [0, 300, 200, 200]
	return [x, y, w, h]

def lightConvert(img, light = True):
	beta = int(random.uniform(0,1)*150)

	M = np.ones(img.shape,dtype="uint8")*beta
	if light:
		return cv2.add(img,M) 
	else:
		return cv2.subtract(img,M)

'''
补全实现
1. 读取原图尺寸
2. 读取矩形框的大小
3. 将正方形区域平移(平移量：)
4. 将矩形框按照max(w,h)补全为正方形
5. 将此正方形的区域和原图区域进行比较，越界的像素置为255
6. 截取中心的正方形区域(300x300)
  - 获取中心点
  - 确定300x300的框
  - 截取ROI
'''
def ROIcompletion(img, rect):
	if not len(rect) == 4 :
		# print 'error: No such file(s) exist: %s.' %imgName
		return 0

	# print imgName
	# img = cv2.imread(imgName)
	if img is None:
		return img

	shift = 0
	direct = 0
	light = True
	baseLength = 30
	shift = int(baseLength * random.uniform(0, 1))
	if shift > 15:
		direct = 0
		shift = -shift
		light = False
	else:
		direct = 1

	roi = [1,2,3,4]
	for i in range(len(rect)):
		roi[i] = rect[i]

	# 平移(up-down-left-right)   
	# print roi
	if roi[0] < shift:
		if direct == 0:
			roi[0] += shift
	if roi[1] > shift:
		if direct == 1:
			roi[1] += shift
	# print roi

	# 补全为正方形（以左边为准） 
	roi[2] = int(roi[2]/2)*2
	roi[3] = int(roi[3]/2)*2

	if roi[2] > img.shape[1]:
		roi[2] = img.shape[1] 
	if roi[3] > img.shape[1]:
		roi[3] = img.shape[1]

	sideLength = max(roi[2], roi[3])   # 正方形边长
	if sideLength > img.shape[1]:
		sideLength = img.shape[1]
	minLength = min(roi[2], roi[3])

	centPt = (roi[0]+roi[2], roi[1]+roi[3]) 
	square = [centPt[0]-sideLength ,roi[1]-sideLength, sideLength,sideLength]
	centPtSquare = (int(sideLength/2), int(sideLength/2))

	# 将平移后越界的像素置为255
	imgROI = np.ones((sideLength,sideLength, 3), dtype="uint8")*255
	
	# 平移后的图
	xStart = int(centPtSquare[0]-roi[2]/2)
	xEnd = int(centPtSquare[0]+roi[2]/2)
	yStart = int(centPtSquare[1]-roi[3]/2)
	yEnd = int(centPtSquare[1]+roi[3]/2)

	oldROI = img[roi[0]:roi[0] + roi[2], roi[1]:roi[1] + roi[3]]
	xEnd = xStart+oldROI.shape[0]
	yEnd = yStart+oldROI.shape[1]
	imgROI[xStart:xEnd, yStart:yEnd] = oldROI
   
	# resize    
	imgROI = cv2.resize(imgROI,(224, 224), interpolation=cv2.INTER_CUBIC)

	return lightConvert(imgROI, light)

def ROIcompletion_(img, rect, shift, direct):
	if not len(rect) == 4 or direct > 3 or shift < 0:
		return 0
	
	roi = [1,2,3,4]

	for i in range(len(rect)):
		roi[i] = rect[i]

	# 平移(up-down-left-right)   
	# print roi
	if roi[0] < shift:
		if direct == 0:
			roi[0] += shift
		elif direct == 1:
			roi[0] -= shift
	if roi[1] > shift:
		if direct == 2:
			roi[1] += shift
		elif direct == 3:
			roi[1] -= shift
	# print roi

	# 补全为正方形（以左边为准） 
	roi[2] = int(roi[2]/2)*2
	roi[3] = int(roi[3]/2)*2

	if roi[2] > img.shape[1]:
		roi[2] = img.shape[1] 
	if roi[3] > img.shape[1]:
		roi[3] = img.shape[1]

	sideLength = max(roi[2], roi[3])   # 正方形边长
	if sideLength > img.shape[1]:
		sideLength = img.shape[1]
	minLength = min(roi[2], roi[3])

	centPt = (roi[0]+roi[2], roi[1]+roi[3]) 
	square = [centPt[0]-sideLength ,roi[1]-sideLength, sideLength,sideLength]
	centPtSquare = (int(sideLength/2), int(sideLength/2))

	# 将平移后越界的像素置为255
	imgROI = np.ones((sideLength,sideLength, 3), dtype="uint8")*255
	
	# 平移后的图
	xStart = int(centPtSquare[0]-roi[2]/2)
	xEnd = int(centPtSquare[0]+roi[2]/2)
	yStart = int(centPtSquare[1]-roi[3]/2)
	yEnd = int(centPtSquare[1]+roi[3]/2)
	
	print "1", xStart, xEnd-xStart, yStart, yEnd-yStart
	print "2", roi[0],  roi[2], roi[1], roi[3]
	
	valid = img[roi[0]:roi[0]+roi[2], roi[1]:roi[1]+roi[3]]
	xEnd = xStart+valid.shape[0]
	yEnd = valid.shape[1]+yStart
	print xEnd, valid.shape[0],xStart
	print yEnd, valid.shape[1],yStart

	# valid.reshape(xEnd-xStart, yEnd-yStart)
	print valid.shape, imgROI[xStart:xEnd, yStart:yEnd].shape
	# return valid

	imgROI[xStart:xEnd, yStart:yEnd] = valid
   
	# resize    
	imgROI = cv2.resize(imgROI,(224, 224), interpolation=cv2.INTER_CUBIC)

	return imgROI

def readImages(dir):
	# 列出更目录下的20个文件夹
	if not os.path.exists(dir):
		print ("error: No such directory exists: %s." %(dir))
		return 0
	os.chdir(dir)

	imgsAll = []
	print "Reading images...."
	for d in os.listdir('./'):
		# 读取单个类别目录下的jpg文件和txt文件并保存
		imgs = os.listdir(d + "/" + jpgPath)
		txts = os.listdir(d + "/" + txtPath)
		# print len(imgs),len(txts), d
		# imgs = cv2.imread(d + '/' + jpgPath+imgs[i])
		imgs = [(cv2.imread(d + "/" + jpgPath+"/"+imgs[i]), int(d), dir + "/" + d + "/" + jpgPath + "/" + imgs[i]) for i in range(len(imgs))]
		for i in range(len(imgs)-1, -1, -1):
			if not imgs[i][2].lower().endswith("jpg") and not imgs[i][2].lower().endswith("png") and not imgs[i][2].lower().endswith("jpeg"):
				del imgs[i]
		if int(d) == 255:
			txts = [(i, i) for i in range(len(imgs))]
		else:
			txts = [(loadROIFromTxt(d + "/" + txtPath+"/"+txts[i]), dir + "/"+ d + "/" + txtPath + "/" + txts[i]) for i in range(len(txts))]
			for i in range(len(txts)-1, -1, -1):
				if not txts[i][1].lower().endswith("txt") or len(txts[i]) == 0:
					del txts[i]

		# 判断jpg的个数和txt的个数是否相等
		if not int(d) == 255 and (not len(imgs) == len(txts)):
			print "error: imgs is not equals to txts.", len(imgs),len(txts)
			exit(1)
		for i in range(len(imgs)):
			imgsAll.append([imgs[i], txts[i]])
	
	return imgsAll

def funcoftrain():
	caffe.set_mode_gpu()
	caffe.set_device(0)
	solver = None
	# solver = caffe.SGDSolver(solver_path)
	solver = caffe.SGDSolver('solver1.prototxt')
	solver.net.blobs['data'].reshape(32, 9, 224, 224)
	solver.net.blobs['label'].reshape(32, 1)
	# solver.restore('/home/wangruochen/FCNforString/InvoiceRecognition_high/model/icbc_high_iter_340000.solverstate') 

	imgsAll = readImages('/home/lsn/eric/work/entry/class/')
	# print len(imgsAll)
	random.shuffle(imgsAll)
	batch_size = 32
	
	# 将图像数量padding成batch	_size的倍数
	residual = len(imgsAll)%(batch_size*3)
	loops = int(len(imgsAll)/(batch_size*3))
	# if residual > 0:
	# 	for i in range(batch_size-residual):
	# 		imgsAll.append([imgsAll[i][0],imgsAll[i][1]])

	label = 0
	# image
	image1 = np.zeros((224,224, 3), dtype="uint8")
	image2 = np.zeros((224,224, 3), dtype="uint8")
	image3 = np.zeros((224,224, 3), dtype="uint8")

	for epoch in range(20000):
		j = 0
		for j in range(0, loops, batch_size*3):
			idx = int(j/3)
			imgs = imgsAll[j:j+batch_size*3]

			# 处理一个 batch_size
			for i in range(0, len(imgs), 3):
				if imgs[i][0][1] == 255:
					image1 = cv2.resize(imgs[i][0][0],(224, 224), interpolation=cv2.INTER_CUBIC)
				elif imgs[i+1][0][1] == 255:
					image2 = cv2.resize(imgs[i+1][0][0],(224, 224), interpolation=cv2.INTER_CUBIC)
				elif imgs[i+2][0][1] == 255:
					image3 = cv2.resize(imgs[i+2][0][0],(224, 224), interpolation=cv2.INTER_CUBIC)
				else:
					rect1 = loadROIFromTxt(imgs[i][1][1])
					rect2 = loadROIFromTxt(imgs[i+1][1][1])
					rect3 = loadROIFromTxt(imgs[i+2][1][1])
					image1 = ROIcompletion(imgs[i][0][0], rect1)
					image2 = ROIcompletion(imgs[i+1][0][0], rect2)
					image3 = ROIcompletion(imgs[i+2][0][0], rect3)
				
				image = np.concatenate((image1, image2, image3), axis=2)
				label = int(imgs[i][0][1])
				# cv2.imshow('light', image)
				# cv2.waitKey(33)
				# if i%1 == 0:
				# 	time.sleep(1)
				# print solver.net.blobs['data'].data[i].shape,rect, image.shape
				image = image.transpose(2, 0, 1)
				# if image.shape[0] < 224 or image.shape[1] < 224:
				# 	continue
				solver.net.blobs['data'].data[idx] = image
				solver.net.blobs['label'].data[idx] = label  
				# print solver.net.blobs['label'].data[idx]
				# i += 1
			
			# print label
			solver.step(1)

#solver.restore('./Models/net_iter_'+str(i)+'.solverstate')
#solver.step(800000)    

# do net surgery to set the deconvolution wezights for bilinear interpolation

#print(solver.net.params.keys())

# copy base weights for fine-tuning
#solver.net.copy_from(base_weights)

# solve straight through -- a better approach is to define a solving loop to
# 1. take SGD steps
# 2. score the model by the test net `solver.test_nets[0]`
# 3. repeat until satisfied
#solver.step(80000);

if __name__ == "__main__":
	funcoftrain()
