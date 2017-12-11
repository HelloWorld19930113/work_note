#coding=utf-8
'''
功能： 使用标注得到的保存在.xml中的BBOX提取出ROI区域
      然后在原jpg上标出ROI框并保存
注意： 
      ROI 区域的切片规则是：[row_s:row_e，column_s:column_e]
'''

import os
import cv2
import time
from xml.etree import ElementTree as ET  

# 保存.jpg和.xml文件的目录
# test
# jpgPath = "jpg"
# xmlPath = "xml"

# Standard directory
jpgPath = "JPEGImages"
xmlPath = "Annotations"


colors = [(0, 255, 0), (255, 0, 0)]

#code
imgs = os.listdir(jpgPath)
imgs = [(imgs[i], jpgPath + "/" + imgs[i]) for i in range(len(imgs))]
for i in range(len(imgs)-1, -1, -1):
    if not imgs[i][1].lower().endswith("jpg") and not imgs[i][1].lower().endswith("png") and not imgs[i][1].lower().endswith("jpeg"):
        del imgs[i]

xmls = os.listdir(xmlPath)
xmls = [(xmls[i], xmlPath + "/" + xmls[i]) for i in range(len(xmls))]
for i in range(len(xmls)-1, -1, -1):
    if not xmls[i][1].lower().endswith("xml"):
        del xmls[i]

if(len(imgs) != len(xmls)):
    print "error: imgs is not equals to xmls counts."
    exit(1)

# for i in range(len(imgs)-1, -1, -1):
#     print i,
# while 1:
#     time.sleep(10)


show = []
dpoint = []
epoint = []


def loadROI(path):
    # per=ET.parse('testXml.xml')  
    # path = path + '.xml'
    print path
    per=ET.parse(path) 

    roi = []
    rois = []
    colStart = 0
    colEnd = 0
    rowStart = 0
    rowEnd = 0 
    class_name = []

    name_node = per.getiterator("name") 
    for name in name_node:
        print "node.text:%s" % name.text   
        class_name.append(name.text)

    lst_node = per.getiterator("bndbox")  
    for oneper in lst_node:  #找出person节点  
        for child in oneper.getchildren(): #找出person节点的子节点  
            
            if child.tag == 'xmin':
                colStart = int(child.text)
            elif child.tag == 'ymin':
                colEnd = int(child.text)
            elif child.tag == 'xmax':
                rowStart = int(child.text)
            elif child.tag == 'ymax':
                rowEnd = int(child.text)

            roi=[[colStart, colEnd],[rowStart,rowEnd]] # 此时只考虑了一个框
        
        rois.append(roi)
        # print roi[0][0],roi[0][1],roi[1][0],roi[1][1], type(roi)
    print len(rois), len(class_name)
    if len(rois) == len(class_name):
        return rois, class_name

# 保存为ROI图片        
def saveROI(name, class_name, rois, img):
    index = 0

    if(not os.path.exists('roi')):
        os.makedirs('roi')

    if len(rois):

        for roi in rois:
            class_path = 'roi/'+ class_name[index]
            print class_path
            if(not os.path.exists(class_path)):
                os.makedirs(class_path)
            
            full_name = 'roi/' + class_name[index] + '/' + name 

            d = roi[0]
            e = roi[1]
            pmin = (min(d[0], e[0]), min(d[1], e[1]))
            pmax = (max(d[0], e[0]), max(d[1], e[1]))

            colStart = pmin[0]
            colEnd = pmax[0]
            rowStart = pmin[1]
            rowEnd = pmax[1]

            # roiImg = np.zeros((height,width,3), np.uint8)
            # roiImg = (colStart, rowStart), (colEnd, rowEnd)
            # roiImg = ( rowStart:rowEnd, colStart:colEnd)
            cv2.imwrite(full_name, img[rowStart:rowEnd, colStart:colEnd], [int(cv2.IMWRITE_JPEG_QUALITY), 100])   # 100 is the highest quality.
            # cv2.imwrite(full_name, img[colStart:colEnd, rowStart:rowEnd], [int(cv2.IMWRITE_JPEG_QUALITY), 100])   # 100 is the highest quality.

            index = index + 1


# 在图片上将ROI区域框选出来  
def saveRect(name, class_name, rois, img):
    index = 0
    class_path = 'roi/'+ class_name[index]

    if(not os.path.exists('roi')):
        os.makedirs('roi')
    if(not os.path.exists(class_path)):
        os.makedirs(class_path)

    if len(rois):
        for roi in rois:
            full_name = 'roi/' + class_name[index] + '/' + name 

            d = roi[0]
            e = roi[1]
            pmin = (min(d[0], e[0]), min(d[1], e[1]))
            pmax = (max(d[0], e[0]), max(d[1], e[1]))

            colStart = pmin[0]
            colEnd = pmax[0]
            rowStart = pmin[1]
            rowEnd = pmax[1]

            # roiImg = np.zeros((height,width,3), np.uint8)
            # roiImg = (colStart, rowStart), (colEnd, rowEnd)
            # roiImg = ( rowStart:rowEnd, colStart:colEnd)
            cv2.rectangle(img, (colStart, rowStart), (colEnd, rowEnd), (255,0,0), 2)
            cv2.imwrite(full_name, img,[int(cv2.IMWRITE_JPEG_QUALITY), 100])   # 100 is the highest quality.

            index = index + 1


if len(imgs) == 0:  
    print "empty imgs dir."
    exit(1)


# Debug
# for i in range(len(imgs)-1, -1, -1):
#     print i
# while 1:
#     time.sleep(10)


while i < len(imgs):

    show = cv2.imread(imgs[i][1])
    pos = imgs[i][0].rfind(".")   # 文件扩展名的前缀
    rois, class_name = loadROI("%s/%s.xml" %(xmlPath, imgs[i][0][:pos]))
    # currentClass, objs = loadObjs("%s/%s.xml.txt" %(path, imgs[i][0][:pos]))
    # print rois
    saveROI("%s.jpg" %(imgs[i][0][:pos]), class_name, rois, show)
    #saveRect("%s.jpg" %(imgs[i][0][:pos]), class_name, rois, show)
    i = i+1
