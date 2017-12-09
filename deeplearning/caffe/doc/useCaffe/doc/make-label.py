#!/usr/bin/env python
#coding=utf-8

import os
import sys
import random

def randomList(lst):
    for i in range(0, len(lst)):
        ind = int(random.random() * (len(lst)-1))
        ind2 = int(random.random() * (len(lst) - 1))
        t = lst[ind]
        lst[ind] = lst[ind2]
        lst[ind2] = t

#usage make-label.py folder-name test-ratio[0.15] random-seed[33]
if len(sys.argv) < 2:
    print "usage make-label.py folder-name test-ratio[default=0.15] random-seed[default=33]"
    exit(0)

name = "data2" if len(sys.argv) < 2 else sys.argv[1]
testratio = 0.15 if len(sys.argv) < 3 else float(sys.argv[2])
seed = 33 if len(sys.argv) < 4 else int(sys.argv[3])
random.seed(seed)

path = name

db = []
folders = os.listdir(path)
totalFiles = 0
for i in range(0, len(folders)):
    files = os.listdir(path + "/" + folders[i])
    randomList(files)
    totalFiles += len(files)
    db.append({"name": folders[i], "files": [["none", "%s/%s/%s" % (path, folders[i], k)] for k in files]})

countTrain = int((1 - testratio) * totalFiles)
countVal = totalFiles - countTrain
print "%d classes, totalFiles:%d, trainFiles(%.1f%%):%d, valFiles(%.1f%%):%d" % \
      (len(db), totalFiles, (1 - testratio) * 100, countTrain, testratio * 100, countVal)

labelfile = open(name + "-labels.txt", "w")
for i in range(len(db)):
    item = db[i]
    labelfile.write(item["name"] + "\n")
labelfile.close()

#获取样本不受限制
getnonLimt = False
valfile = open(name + "-val.txt", "w")
while countVal > 0:
    change = 0
    for i in range(0, len(db)):
        item = db[i]
        files = item["files"]
        if(len(files) > 1 or getnonLimt):

            #对每个类别取testacc个样本，保证均匀分布
            num = min(max(1, int(len(files) * testratio)), len(files))
            if getnonLimt:
                num = len(files)

            for k in range(0, num):
                fileItem = files[k]
                if fileItem[0] == "none":
                    if countVal > 1:
                        valfile.write(fileItem[1] + (" %d\n" % i))
                    else:
                        #如果等于1，表示是最后一条数据，这时候需要保证尾部没有换行
                        valfile.write(fileItem[1] + (" %d" % i))

                    fileItem[0] = "val"
                    countVal = countVal - 1
                    change = change+1

                    #对每个类别就只取一个样本，然后重复下去
                    if countVal <= 0 or getnonLimt:
                        break

        if countVal <= 0:
            break

    if change == 0:
        #如果没有获取到val的样本，那么就去掉限制满足数量
        getnonLimt = True

valfile.close()

trainfile = open(name + "-train.txt", "w")
for i in range(0, len(db)):
    item = db[i]
    files = item["files"]
    for k in range(0, len(files)):
        fileItem = files[k]
        if fileItem[0] == "none":
            if countTrain > 1:
                trainfile.write(fileItem[1] + (" %d\n" % i))
            else:
                # 如果等于1，表示是最后一条数据，这时候需要保证尾部没有换行
                trainfile.write(fileItem[1] + (" %d" % i))
            fileItem[0] = "train"
            countTrain = countTrain-1

trainfile.close()

dbtrain = []
dbval = []
for i in range(0, len(db)):
    item = db[i]
    files = item["files"]

    numtrain = 0
    numval = 0
    for k in files:
        if k[0] == "train":
            numtrain = numtrain + 1
        elif k[0] == "val":
            numval = numval + 1
        else:
            print "error."

    dbtrain.append(numtrain)
    dbval.append(numval)

print "train files:", dbtrain
print "val files:", dbval
print "finish"