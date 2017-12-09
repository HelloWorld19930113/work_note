#coding=utf-8
'''
功能： 使用标注得到的保存在.xml中的BBOX提取出ROI区域
      然后在原jpg上标出ROI框并保存
'''

import os
import cv2
import time
from xml.etree import ElementTree as ET  

# 保存.jpg和.xml文件的目录
jpgPath = "jpg"
xmlPath = "xml"


wndName = "ROI"
className = ["sh002", "demo"]
colors = [(0, 255, 0), (255, 0, 0)]

#code
currentClass = 0

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
rois = []

def loadROI(path):
    # per=ET.parse('testXml.xml')  
    # path = path + '.xml'
    print path
    per=ET.parse(path) 

    roi = []
    colStart = 0
    colEnd = 0
    rowStart = 0
    rowEnd = 0 
    class_name = ""

    name_node = per.getiterator("name") 
    for name in name_node:
        print "node.text:%s" % name.text   
        class_name = name.text

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
        print roi 
        # print roi[0][0],roi[0][1],roi[1][0],roi[1][1], type(roi)
    return roi, class_name
      
def saveROI(name, rois, img):
    if(not os.path.exists('roi')):
        os.makedirs('roi')

    with open(name, "w") as jpg:
        # for item in rois:
            # print rois,item[0], item[1]
            # print rois
        d = rois[0]
        e = rois[1]
        pmin = (min(d[0], e[0]), min(d[1], e[1]))
        pmax = (max(d[0], e[0]), max(d[1], e[1]))

        colStart = pmin[0]
        colEnd = pmax[0]
        rowStart = pmin[1]
        rowEnd = pmax[1]

        # roiImg = np.zeros((height,width,3), np.uint8)
        roiImg = img[rowStart:rowEnd, colStart:colEnd]
        cv2.imwrite(name, roiImg,[int(cv2.IMWRITE_JPEG_QUALITY), 100])   # 100 is the highest quality.

     
def saveRect(name, class_name, rois, img):
    if(not os.path.exists('rect')):
        os.makedirs('rect')
        class_path = 'rect/' + class_name
        if(not os.path.exists(class_path)):
            os.makedirs(class_path)

    full_name = 'rect/' + class_name + '/' + name 
    with open(full_name, "w") as jpg:
        # for item in rois:
            # print rois,item[0], item[1]
            # print rois
        d = rois[0]
        e = rois[1]
        pmin = (min(d[0], e[0]), min(d[1], e[1]))
        pmax = (max(d[0], e[0]), max(d[1], e[1]))

        colStart = pmin[0]
        colEnd = pmax[0]
        rowStart = pmin[1]
        rowEnd = pmax[1]

        # roiImg = np.zeros((height,width,3), np.uint8)
        cv2.rectangle(img, (colStart, rowStart), (colEnd, rowEnd), (255,0,0), 2)
        cv2.imwrite(name, img,[int(cv2.IMWRITE_JPEG_QUALITY), 100])   # 100 is the highest quality.


if len(imgs) == 0:
    print "empty imgs dir."
    exit(1)


# Debug
# for i in range(len(imgs)-1, -1, -1):
#     print i
# while 1:
#     time.sleep(10)


while True:
    show = cv2.imread(imgs[i][1])
    pos = imgs[i][0].rfind(".")   # 文件扩展名的前缀
    rois, class_name = loadROI("%s/%s.xml" %(xmlPath, imgs[i][0][:pos]))
    # currentClass, objs = loadObjs("%s/%s.xml.txt" %(path, imgs[i][0][:pos]))
    # print rois
    #saveROI("%s/%s.jpg" %("roi", imgs[i][0][:pos]), rois, show)
    saveRect("%s.jpg" %(imgs[i][0][:pos]), class_name, rois, show)
    i = i+1
