#coding=utf-8
import os
import cv2


root = 'splitVideo\\'
jpgPath = 'jpg'
index = 0

def splitVideo(name):
	print name
	# cv2.imread(name)
	cap = cv2.VideoCapture(name)
	#获得码率及尺寸  
	# fps = cap.get(cv2.cv.CV_CAP_PROP_FPS)  
	# size = (int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH)),
 #        int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT)))  
  
	#指定写视频的格式, I420-avi, MJPG-mp4  
	# videoWriter = cv2.VideoWriter('oto_other.mp4', cv2.cv.CV_FOURCC('M', 'J', 'P', 'G'), fps, size)  
  
	#读帧  
	success,frame = cap.read()
	print cap.isOpened(),success,frame
  
	while success:
		index += 1
		print frame
		cv2.imshow("video", frame) 
		cv2.waitKey(33) 
		# cv2.waitKey(1000/int(fps)) 
		cv2.imwrite(jpgPath + str(index) + '.jpg', frame)

		success, frame = cap.read() 


# cap = cv2.VideoCapture('vtest.avi')

# while(cap.isOpened()):
#     ret, frame = cap.read()

#     gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

#     cv2.imshow('frame',gray)
#     if cv2.waitKey(1) & 0xFF == ord('q'):
#         break

# cap.release()
# cv2.destroyAllWindows()




















# Python: cv2.VideoCapture() → <VideoCapture object>
# Python: cv2.VideoCapture(filename) → <VideoCapture object>
# Python: cv2.VideoCapture(device) → <VideoCapture object>

# Python: cv2.imwrite(filename, img[, params]) → retval


if __name__ == '__main__':
	if not os.path.exists(jpgPath):
		os.makedirs(jpgPath)

	cap = cv2.VideoCapture('1.avi')

	success,frame = cap.read()
	print cap.isOpened(), success, frame
  
	while success:
		index += 1
		print frame
		cv2.imshow("video", frame) 
		cv2.waitKey(33) 
		# cv2.waitKey(1000/int(fps)) 
		cv2.imwrite(jpgPath + str(index) + '.jpg', frame)

		success, frame = cap.read() 



	# videos = os.listdir(root)
	# print videos
	# # os.chdir(root)
	# for video in videos:
	# 	splitVideo(video)