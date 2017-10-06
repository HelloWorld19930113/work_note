# -*- coding: utf-8 -*-  
  
#=================  
#File: PyOpenCV.py  
#Author: Wendy  
#Date: 2013-12-03  
#=================  
  
#eclipse, python2.7, opencv 2.4.11
  
import cv2  
  
#获得视频的格式  
video = cv2.VideoCapture('2.avi')  
  
#获得码率及尺寸  
fps = video.get(cv2.cv.CV_CAP_PROP_FPS)  
size = (int(video.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH)),   
        int(video.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT)))  
  
#指定写视频的格式, I420-avi, MJPG-mp4  
videoWriter = cv2.VideoWriter('oto_other.mp4', cv2.cv.CV_FOURCC('M', 'J', 'P', 'G'), fps, size)  
  
#读帧  
success, frame = video.read()  
print success , frame, fps
  
while success :  
    cv2.imshow("Oto Video", frame) #显示  
    cv2.waitKey(1000/int(fps)) #延迟  
    videoWriter.write(frame) #写视频帧  
    success, frame = video.read() #获取下一帧  
