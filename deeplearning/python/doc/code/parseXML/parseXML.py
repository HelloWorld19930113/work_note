# encoding=utf-8
import os
import cv2
from xml.etree import ElementTree as ET  


def print_node(node):  
    # 打印结点基本信息 
    print "=============================================="  
    print "node.attrib:%s" % node.attrib  
    if node.attrib.has_key("xmin") > 0 :  
        print "node.attrib['age']:%s" % node.attrib['age']  
    print "node.tag:%s" % node.tag  
    print "node.text:%s" % node.text  

def loadROIFromXML(path):
    # per=ET.parse('testXml.xml')  
    # path = path + '.xml'
    print path
    per=ET.parse(path) 

    roi = []
    colStart = 0
    colEnd = 0
    rowStart = 0
    rowEnd = 0 

    node_find = per.findall('./object/name')  
    for node in node_find:  
        print "node.text:%s" % node.text  

    node_find = per.findall('./object/bndbox')  
    # node_find = root.getElementsByTagName('bndbox')  
    print type(node_find), '-' ,len(node_find) 
    for node in node_find:  
        for child in node.getchildren():  # 2 通过 getchildren
            print child.text
    
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

            roi=[[colStart, colEnd],[rowStart,rowEnd]]
            # print 'sex:',oneper.get('sex')  
        print roi 
    return roi

  
def saveRoiWithXML(name, rois, img):
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


if __name__ == '__main__':
    loadROIFromXML('test.xml')