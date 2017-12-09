#coding=utf-8
import os
import cv2
import time
from xml.etree import ElementTree as ET  

'''
2017-09-20 12:46:04
	功能： 使用标注得到的.txt文件截取ROI商品区域，并保存为jpg文件。以便后续进行分类
	
	可能需要修改的位置：
		最后while循环中根据需要进行跳帧提取或全部提取
'''

#parameters
jpgPath = "jpg"
xmlPath = "xml"
txtPath = "txt"

#code
def filesIterator():
	
	imgs = os.listdir(jpgPath)
	imgs = [( imgs[i], os.sep.join([jpgPath, imgs[i]]) ) for i in range(len(imgs))]
	for i in range(len(imgs)-1, -1, -1):
	    if not imgs[i][1].lower().endswith("jpg") and not imgs[i][1].lower().endswith("png") and not imgs[i][1].lower().endswith("jpeg"):
	        del imgs[i]

	# xmls = os.listdir(xmlPath)
	# xmls = [(xmls[i], os.sep.join(xmlPath, xmls[i]) ) for i in range(len(xmls))]
	# for i in range(len(xmls)-1, -1, -1):
	#     if not xmls[i][1].lower().endswith("xml"):
	#         del xmls[i]

	txts = os.listdir(txtPath)
	txts = [(txts[i], os.sep.join(txtPath, txts[i]) ) for i in range(len(txts))]
	for i in range(len(txts)-1, -1, -1):
	    if not txts[i][1].lower().endswith("txt"):
	        del txts[i]

	if(len(imgs) != len(txts)):
	    print "error: imgs is not equals to txts counts."
	    return []

	return imgs

'''
载入txt文件中的ROI区域信息
	name 为 .txt 文件的路径

	说明：
	.txt 文件的格式定义：
	```
		2,1
		260,81,509,488,0,mxff
		134,91,314,580,1,mxfl
	```
	其中，第一行的第一个参数为图片中的目标对象的个数，如果这个参数为1，那么接下来就会有1行；类似的如果这个参数为2，那么接下来就会有2行
	第二行的含义为：pmin[0], pmin[1], pmax[0], pmax[1], cls, className[cls]： 第一个点的x 第一个点的y 第二个点的x  第二个点的y  类别index  类别名
'''
def loadROIFromTxt(name):
    if not os.path.exists(name):
        return []

    obs = []
    with open(name, "r") as txt:
        info = txt.readline().split(",")
        num = int(info[0])
        for i in range(num):
            info = txt.readline().split(",")
            point1 = (int(info[0]), int(info[1]))
            point2 = (int(info[2]), int(info[3]))
            objClass = int(info[4])
            objName = info[5]
            obs.append((point1, point2, objClass, objName))
    return obs


'''
截取ROI区域并保存
	如果.txt中有两种不同的目标对象，则应该将ROi保存在不同的文件夹中。
'''
def saveROITxt(filename, rois, img):
    roiSaveDir = 'roi/'
    if(not os.path.exists(roiSaveDir)):
        os.makedirs(roiSaveDir)

    for item in rois:
        point1 = item[0]
        point2 = item[1]
        pMin = (min(point1[0], point2[0]), min(point1[1], point2[1]))
        pMax = (max(point1[0], point2[0]), max(point1[1], point2[1]))
        objName = item[3]

        # 计算roi的行列起始结束值
        colStart = pMin[0]
        colEnd = pMax[0]
        rowStart = pMin[1]
        rowEnd = pMax[1]

        roiImg = img[rowStart:rowEnd, colStart:colEnd]

        # 创建图片保存目录 
        path = os.sep.join([roiSaveDir, objName])
        if(not os.path.exists(path)):
            os.makedirs(path)

        jpgName = os.sep.join([path, filename])
        cv2.imwrite(jpgName, roiImg,[int(cv2.IMWRITE_JPEG_QUALITY), 100])   # 100 is the highest quality.

'''
随机截取ROI区域并保存
	随机的对框进行放大，放大范围限制在[ , ]中。
'''
def randomSaveROITxt(filename, rois, img):
    roiSaveDir = 'roi/'
    if(not os.path.exists(roiSaveDir)):
        os.makedirs(roiSaveDir)

    for item in rois:
        point1 = item[0]
        point2 = item[1]
        pMin = (min(point1[0], point2[0]), min(point1[1], point2[1]))
        pMax = (max(point1[0], point2[0]), max(point1[1], point2[1]))
        objName = item[3]

        # 计算roi的行列起始结束值
        colStart = pMin[0]
        colEnd = pMax[0]
        rowStart = pMin[1]
        rowEnd = pMax[1]

        roiImg = img[rowStart:rowEnd, colStart:colEnd]

        # 创建图片保存目录 
        path = os.sep.join([roiSaveDir, objName])
        if(not os.path.exists(path)):
            os.makedirs(path)

        jpgName = os.sep.join([path, filename])
        cv2.imwrite(jpgName, roiImg,[int(cv2.IMWRITE_JPEG_QUALITY), 100])   # 100 is the highest quality.




def main():
	imgs = filesIterator()
	
	for i in range(len(imgs)):
    	img = cv2.imread(imgs[i][1])
	    pos = imgs[i][0].rfind(".")   # 文件扩展名的前缀
	    txtName = os.sep.join([txtPath, imgs[i][0][:pos]+'.xml.txt'])
	    
	    # 读取txt文件中的ROI数据
	    rois = loadROIFromTxt(txtName)
	    saveROITxt(imgs[i][0], rois, img)

if __name__ == '__main__':
	main()


