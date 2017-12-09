#include <fstream>
#include <iostream>

#include <cv.h>
#include <highgui.h>
#include <classification.h>

using namespace cv;
using namespace std;

#define HAS_VIDEO


struct DetectObjectInfo{
	float xmin, ymin, xmax, ymax;
	float score;
	int label;

	const Rect rect() const{
		return Rect(xmin, ymin, xmax - xmin, ymax - ymin);
	}
};

float IoU(const Rect& a, const Rect& b){
	float xmax = max(a.x, b.x);
	float ymax = max(a.y, b.y);
	float xmin = min(a.x + a.width, b.x + b.width);
	float ymin = min(a.y + a.height, b.y + b.height);
	float uw = (xmin - xmax + 1 > 0) ? (xmin - xmax + 1) : 0;
	float uh = (ymin - ymax + 1 > 0) ? (ymin - ymax + 1) : 0;
	float iou = uw * uh;
	return iou / min(a.area(), b.area());
}

vector<string> loadLabels(const char* labelsFile){
	ifstream fin(labelsFile);
	vector<string> out;

	string  s;
	while (fin >> s)
		out.push_back(s);

	return out;
}


vector<DetectObjectInfo> toDetInfo(BlobData* fr, int imWidth, int imHeight){
	const float Scorethreshold = 0.2;

	vector<DetectObjectInfo> out;
	float* data = fr->list;
	for (int i = 0; i < fr->count; i += 7, data += 7){
		DetectObjectInfo obj;

		//if invalid det
		if (data[0] == -1 || data[2] < Scorethreshold)
			continue;

		obj.label = data[1];
		obj.score = data[2];
		obj.xmin = data[3] * imWidth;
		obj.ymin = data[4] * imHeight;
		obj.xmax = data[5] * imWidth;
		obj.ymax = data[6] * imHeight;
		out.push_back(obj);
	}
	return out;
}

Scalar getColor(int label){
	static vector<Scalar> colors;
	if (colors.size() == 0){
#if 0
		for (float r = 127.5; r <= 256 + 127.5; r += 127.5){
			for (float g = 256; g >= 0; g -= 127.5){
				for (float b = 0; b <= 256; b += 127.5)
					colors.push_back(Scalar(b, g, r > 256 ? r - 256 : r));
			}
		}
#endif
		colors.push_back(Scalar(0, 255, 0));
		colors.push_back(Scalar(0, 0, 255));
		colors.push_back(Scalar(255, 0, 0));
		colors.push_back(Scalar(0, 255, 255));
		colors.push_back(Scalar(255, 0, 255));
		colors.push_back(Scalar(128, 0, 255));
		colors.push_back(Scalar(128, 255, 255));
		colors.push_back(Scalar(255, 128, 255));
		colors.push_back(Scalar(128, 255, 128));
	}
	return colors[label % colors.size()];
}

bool fileExists(const char* file){
	FILE* f = fopen(file, "rb");
	if (f == 0) return false;
	fclose(f);
	return true;
}


float cosdis(const float* a, const float* b, int len){

	float x = 0;
	float y = 0;
	float z = 0;

	for (int i = 0; i < len; ++i){
		x += a[i] * a[i];
		y += b[i] * b[i];
		z += a[i] * b[i];
	}

	float t = (sqrt(x) * sqrt(y));
	if (t == 0) return 0;
	return z / t;
}

float cosdis(const uchar* a, const uchar* b, int len){

	long x = 0;
	long y = 0;
	long z = 0;
	
	for (int i = 0; i < len; ++i){
		x += a[i] * a[i];
		y += b[i] * b[i];
		z += a[i] * b[i];
	}
	float t = 0.0;

	if (x > 0 && y > 0)
		t = (sqrt(x) * sqrt(y));
	if (t == 0 || z <= 0 || z >= t ) return 0;
	return z / t;
}

float average(vector<float> &sm)
{
	float sum = 0;
	int valid = 0;

	if (!sm.empty())
	{
		for (int i = 0; i < sm.size(); i++){
			if (sm[i] - 0.7 > 0){
				valid++;
				sum += sm[i];
			}
		}
	}

	if (valid == 0) return 0;
	
	return sum / valid;
}

int main(int argc, char * argv)
{
	char caffeModel[256] = "E:/vs_workspace/generalTemplate/VGG_VOC0712_SSD_300x300_iter_80000.caffemodel";
	char deployPrototxt[256] = "E:/vs_workspace/generalTemplate/deploy.prototxt";
	char labels[256] = "E:/vs_workspace/generalTemplate/labels.txt";

	vector<string> labelMap = loadLabels(labels);
	float means[] = { 104.0f, 117.0f, 123.0f };
	Classifier classifier(deployPrototxt, caffeModel, 1, 0, 3, means, 0);
	//float means[] = { 123, 117, 104 };
	Classifier cls("deploy_resnet101-v2.prototxt", "resnet-101_iter_5000.caffemodel", 1, 0, 3, means, 0);

	vector<Mat> imgSet;
	Mat img;
	string preName = "00010";
	char name[256];
	for (int i = 2; i < 39; i++){
		sprintf(name, "templet/%s%02d.jpg", preName.c_str(), i);
		img = imread(name);
		if (img.data)
			imgSet.push_back(img);
		if (imgSet.size() <= 0)
			return -1;
	}
#if 1
#ifdef HAS_VIDEO
	VideoCapture cap("jiHuo.avi");
#else
	//HK_Camera hkCamera;

	//hkCamera.open();
	//frame = hkCamera.getImg();
	VideoCapture cap(0);
#endif

	WPtr<BlobData> feature;
	vector<DetectObjectInfo> objs;

	// 处理视频流数据
	cv::Mat frame;

	cv::Mat detectROI;
	cap >> frame; 
	vector<float> sm;
	while (1)
	{
		if (frame.empty()){
			continue;
		}

		cap >> frame;

		feature = classifier.extfeature(frame, "detection_out");
		objs = toDetInfo(feature, frame.cols, frame.rows);
		// First frame, give the groundtruth to the tracker
		for (size_t i = 0; i < objs.size(); i++)
		{
			auto obj = objs[i];
			//cout << obj.label << endl;  // 因为第一个label是背景，第二个label是饮料；
			rectangle(frame, obj.rect(), getColor(obj.label), 2);

			auto box = obj.rect();
			box = box & Rect(0, 0, frame.cols, frame.rows);

			if (box.width > 0 && box.height > 0){
				// 在冰柜的检测区域内
				if (box.x > 200 && box.x < 400){
					frame(box).copyTo(detectROI);

					int len = 0;
					for (int i = 0; i < imgSet.size(); i++){
						resize(detectROI, detectROI, imgSet[i].size(), 0);
						len = imgSet[i].cols*imgSet[i].rows * imgSet[i].channels();

						if (imgSet[i].size() == detectROI.size())
							sm.push_back(cosdis(detectROI.data, imgSet[i].data, len));
					}
					cout << "cosdis=" << average(sm) << endl;

					imshow("roi", detectROI);
				}
			}
			//Mat im1 = imread("492846.jpg");

			//WPtr<BlobData> feature1 = cls.extfeature(im1, "pool5");
			//WPtr<BlobData> feature2 = cls.extfeature(im2, "pool5");
			//WPtr<SoftmaxResult> feature1 = cls.predictSoftmax(dectectROI, 1);

			//float sm = cosdis(dectectROI.data, imgSet[2].data, imgSet[2].data);

			//paDrawString(frame, labelMap[obj.label].c_str(), Point(obj.xmin, obj.ymin - 20), getColor(obj.label), 20, true);
		}
		line(frame, Point(200, 0), Point(200, frame.rows), Scalar(255, 0, 255), 2);
		line(frame, Point(400, 0), Point(400, frame.rows), Scalar(255, 0, 255), 2);
		imshow("Image", frame);
		waitKey(1);
	}
#endif
	return 0;
}




int main(int argc, char * argv)
{
	char caffeModel[256] = "E:/vs_workspace/generalTemplate/VGG_VOC0712_SSD_300x300_iter_80000.caffemodel";
	char deployPrototxt[256] = "E:/vs_workspace/generalTemplate/deploy.prototxt";
	char labels[256] = "E:/vs_workspace/generalTemplate/labels.txt";

	vector<string> labelMap = loadLabels(labels);
	float means[] = { 104.0f, 117.0f, 123.0f };
	Classifier classifier(deployPrototxt, caffeModel, 1, 0, 3, means, 0);
	//float means[] = { 123, 117, 104 };
	Classifier cls("deploy_resnet101-v2.prototxt", "resnet-101_iter_5000.caffemodel", 1, 0, 3, means, 0);

	vector<Mat> imgSet;
	Mat img;
	string preName = "00010";
	char name[256];
	for (int i = 2; i < 39; i++){
		sprintf(name, "templet/%s%02d.jpg", preName.c_str(), i);
		img = imread(name);
		if (img.data)
			imgSet.push_back(img);
		if (imgSet.size() <= 0)
			return -1;
	}
#if 1
#ifdef HAS_VIDEO
	VideoCapture cap("jiHuo.avi");
#else
	//HK_Camera hkCamera;

	//hkCamera.open();
	//frame = hkCamera.getImg();
	VideoCapture cap(0);
#endif

	WPtr<BlobData> feature;
	vector<DetectObjectInfo> objs;

	// 处理视频流数据
	cv::Mat frame;

	cv::Mat detectROI;
	cap >> frame; 
	vector<float> sm;
	while (1)
	{
		if (frame.empty()){
			continue;
		}

		cap >> frame;

		feature = classifier.extfeature(frame, "detection_out");
		objs = toDetInfo(feature, frame.cols, frame.rows);
		// First frame, give the groundtruth to the tracker
		for (size_t i = 0; i < objs.size(); i++)
		{
			auto obj = objs[i];
			//cout << obj.label << endl;  // 因为第一个label是背景，第二个label是饮料；
			rectangle(frame, obj.rect(), getColor(obj.label), 2);

			auto box = obj.rect();
			box = box & Rect(0, 0, frame.cols, frame.rows);

			if (box.width > 0 && box.height > 0){
				// 在冰柜的检测区域内
				if (box.x > 200 && box.x < 400){
					frame(box).copyTo(detectROI);

					int len = 0;
					for (int i = 0; i < imgSet.size(); i++){
						resize(detectROI, detectROI, imgSet[i].size(), 0);
						len = imgSet[i].cols*imgSet[i].rows * imgSet[i].channels();

						if (imgSet[i].size() == detectROI.size())
							sm.push_back(cosdis(detectROI.data, imgSet[i].data, len));
					}
					cout << "cosdis=" << average(sm) << endl;

					imshow("roi", detectROI);
				}
					
				
			}
			//Mat im1 = imread("492846.jpg");

			//WPtr<BlobData> feature1 = cls.extfeature(im1, "pool5");
			//WPtr<BlobData> feature2 = cls.extfeature(im2, "pool5");
			//WPtr<SoftmaxResult> feature1 = cls.predictSoftmax(dectectROI, 1);

			//float sm = cosdis(dectectROI.data, imgSet[2].data, imgSet[2].data);

			//paDrawString(frame, labelMap[obj.label].c_str(), Point(obj.xmin, obj.ymin - 20), getColor(obj.label), 20, true);
		}
		line(frame, Point(200, 0), Point(200, frame.rows), Scalar(255, 0, 255), 2);
		line(frame, Point(400, 0), Point(400, frame.rows), Scalar(255, 0, 255), 2);
		imshow("Image", frame);
		waitKey(1);
	}
#endif
	return 0;
}



