# encoding=utf-8
# 
# XML文件的解析方法：
# 1.SAX (simple API for XML )
#    python 标准库包含SAX解析器，SAX用事件驱动模型，通过在解析XML的过程中触发一个个的事件并调用用户定义的回调函数来处理XML文件。
# 2.DOM(Document Object Model)
#    将XML数据在内存中解析成一个树，通过对树的操作来操作XML。
# 3.ElementTree(元素树)
#    ElementTree就像一个轻量级的DOM，具有方便友好的API。代码可用性好，速度快，消耗内存少。
#    注：因DOM需要将XML数据映射到内存中的树，一是比较慢，二是比较耗内存，而SAX流式读取XML文件，比较快，占用内存少，但需要用户实现回调函数（handler）。
#   
# 因此这里使用ElementTree(元素树)方法解析；
# 

'''
XML实例文件testXml.xml内容如下：
<annotation><size><width>800</width><height>600</height></size>
<object>
    <name>bld034</name>
    <bndbox>
        <xmin>264</xmin>
        <ymin>45</ymin>
        <xmax>398</xmax>
        <ymax>108</ymax>
    </bndbox>
</object>
</annotation>
'''

# 操作流程
'''
1.加载xml文件
    加载XML文件共有2种方法,一是加载指定字符串，二是加载指定文件
    root = ElementTree.parse(r"D:/test.xml")  
    root = ElementTree.fromstring("D:/test.xml")  
2.获取element的方法
  a) getiterator()
  b) getchildren()
  c) find方法
  d) findall方法
3. 注意主标签的特殊性；
'''

from xml.etree import ElementTree as ET  
import time


def print_node(node):  
    # 打印结点基本信息 
    print "=============================================="  
    print "node.attrib:%s" % node.attrib  
    if node.attrib.has_key("xmin") > 0 :  
        print "node.attrib['age']:%s" % node.attrib['age']  
    print "node.tag:%s" % node.tag  
    print "node.text:%s" % node.text  

def read_xml(text):  
    '''''读xml文件'''  
    # 加载XML文件（2种方法,一是加载指定字符串，二是加载指定文件）      
    # root = ElementTree.parse(r"D:/test.xml")  
    root = ET.fromstring(text)  
      
    # 获取element的方法  
    # 1 通过getiterator   
    lst_node = root.getiterator("bndbox")  
    for node in lst_node:  
        for child in node.getchildren():  # 2 通过 getchildren
            print_node(child)  
    # 3 .find方法
    node_find = root.findall('./object/bndbox')  
    # node_find = root.getElementsByTagName('bndbox')  
    print type(node_find), '-' ,len(node_find) 
    for node in node_find:  
        for child in node.getchildren():  # 2 通过 getchildren
            print_node(child)  
    
    while 1:
        time.sleep(1);
    # 4. findall方法  
    node_findall = root.findall('annotation')[1]  
    print_node(node_findall)  

def parseROI(path):
    # per=ET.parse('testXml.xml')  
    per=ET.parse(path) 

    roi = ()
    colStart = 0
    colEnd = 0
    rowStart = 0
    rowEnd = 0 

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

            roi=(colStart, colEnd),(rowStart,rowEnd)
            # print 'sex:',oneper.get('sex')  
        # print roi 
        print roi[0][0],roi[0][1],roi[1][0],roi[1][1]
      


if __name__ == '__main__':  
    parseROI('testXml.xml')

    print
    print '#################### Test ################\n'  
    read_xml(open("testXml.xml").read())  