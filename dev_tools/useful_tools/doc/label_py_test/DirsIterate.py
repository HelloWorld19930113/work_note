# -*- coding: utf-8 -*-
'''
 功能：中文目录的迭代方法
 
'''
import os
import cv2
import numpy as np

import sys
reload(sys)  
sys.setdefaultencoding('gbk')
# print sys.getdefaultencoding()

def readImage():
	root='C:/Users/lenovo/Desktop/te/新建文件夹/402731.jpg'
	fileName = root.encode('gbk')
	if not os.path.exists(fileName):
		print 'file not exists'

	image = cv2.imread(fileName)
	if image is None:
		print 'error: Reading image error'
		exit(1)

	cv2.imshow('show',image)
	cv2.waitKey()


def readImages(dir):
	# 列出更目录下的20个文件夹
	if not os.path.exists(dir):
		print ("error: No such directory exists: %s." %(dir))
		return 0
	# os.chdir(dir)

	imgsAll = []
	print "Reading images...."
	for d in os.listdir(dir):
		if os.path.isdir(d):
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
				imgsAll.append([image, d])

	return imgsAll


if __name__ == '__main__':
	imgsAll = readImages('./')
	for i in imgsAll:
		print i[1]