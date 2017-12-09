


#include <process.h>
#include <cv.h>
#include <highgui.h>
#include <Windows.h>
using namespace cv;
using namespace std;


void thread(void* p){
	int index = *(int*)p;
	VideoCapture cap(index);
	Mat frame;

	if (!cap.isOpened()){
		printf("open fail %d\n", index);
		return;
	}

	//cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	//cap.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 768);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	while (!cap.read(frame));

	char buf[200];
	sprintf(buf, "camera %d", index);

	int n = 0;
	double tck = getTickCount();
	while (cap.read(frame)){
		n++;
		double t = (getTickCount() - tck) / getTickFrequency() * 1000;
		double fps = n / (t / 1000);

		imshow(buf, frame);
		waitKey(1);
		if (n % 10 == 0)
			printf("%d fps: %.2f[%.2f ms]\n", index, fps, t);
	}
}

void main(){
	int indexs[4] = { 0, 1, 2, 3 };
	for (int i = 0; i < 4; ++i)
		_beginthread(thread, 0, indexs+i);

	Sleep(10000000);
}