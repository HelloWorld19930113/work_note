# encoding=utf-8
# 导入必要的软件包
import argparse
import datetime
import imutils
import time
import cv2
import math
import numpy as np


x = 4.5;
y = 9.0;

images = [[1,0,0], [0,1,0]]

def mean(img):
	row = 0
	col = 0
	sum = 0.0

	for i in img:
		for j in range(len(i)):
			sum += i[j]
		col = len(i)
		row+=1

	sum /= (row*col)
	# print sum

def absSqrt(frameA, frameB):
	frame=[]





if __name__ == '__main__':
    # time.sleep(100)

    frame_a = cv2.imread("1.jpg", cv2.CV_LOAD_IMAGE_COLOR)
    frame_b = cv2.imread("2.jpg", cv2.CV_LOAD_IMAGE_COLOR)

    print type(frame_a)

    height = frame_a.shape[0]
    width = frame_a.shape[1]
    print height/3, width/3

    shrinkImgA = np.zeros((height/3,width/3,3), np.double)
    shrinkImgB = np.zeros((height/3,width/3,3), np.double)
    roi3x3_a = np.zeros((3,3,3), np.double)
    roi3x3_b = np.zeros((3,3,3), np.double)

    offset = 3

    i = 0
    j = 0
    idx = 0
    while i < height:
        while j < width:
            roi3x3_a = frame_a[i:i+offset,j:j+offset]
            roi3x3_b = frame_b[i:i+offset,j:j+offset]
            j+=offset
        i+=offset
        shrinkImgA[idx] = mean(roi3x3_a) 
        shrinkImgB[idx] = mean(roi3x3_b) 
        idx+=1



    print math.sqrt(x)
    print math.sqrt(y)



