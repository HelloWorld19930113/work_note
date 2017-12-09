#!/usr/bin/python
# -*- coding: UTF-8 -*-

from xml.dom.minidom import parse
import xml.dom.minidom

# 使用minidom解析器打开 XML 文档
DOMTree = xml.dom.minidom.parse("test.xml")
collection = DOMTree.documentElement
if collection.hasAttribute("person"):
   print "Root element : %s" % collection.getAttribute("person")

# 在集合中获取所有电影
movies = collection.getElementsByTagName("person")

# 打印每部电影的详细信息
for movie in movies:
   print "*****Movie*****"
   if movie.hasAttribute("age"):
      print "age: %s" % movie.getAttribute("age")
   name = movie.getElementsByTagName('name')[0]
   print "name: %s" % name.childNodes[0].data
   # format = movie.getElementsByTagName('format')[0]
   # print "Format: %s" % format.childNodes[0].data
   # rating = movie.getElementsByTagName('rating')[0]
   # print "Rating: %s" % rating.childNodes[0].data
   # description = movie.getElementsByTagName('description')[0]
   # print "Description: %s" % description.childNodes[0].data