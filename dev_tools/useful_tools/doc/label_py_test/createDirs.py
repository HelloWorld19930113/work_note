# -*- coding = UTF-8-*-
'''
 功能： 创建图片目录，用于存放分类图片
'''
import os
import sys
reload(sys)
sys.setdefaultencoding('gbk')

files = os.listdir('complete/')
os.chdir('jpgs/')
for f in files:
    os.makedirs(f)


