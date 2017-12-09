
#include <string>
#include <iostream>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using std::string;
using namespace std;
using namespace cv;

uchar roi[3];

uchar* mean(Mat& image)
{
	int sum[3] = {0, 0, 0 };
	int nl = image.rows;
	int nc = image.cols;
	int ch = image.channels();
	//遍历图像的每个像素  
	for (int j = 0; j<nl; ++j)
	{
		//uchar *data = image.ptr<uchar>(j);
		for (int i = 0; i< nc; ++i)
		{
			for (int k = 0; k < ch; k++)
			{
				sum[k] += image.at<Vec3b>(j, i)[k];
				roi[k] = (uchar)(sum[k] / nc / nl);
			}
		}
	}

	return roi;
}



 Mat absSqrt(Mat &imgA, Mat& imgB)
{
	int nl = imgA.rows;
	int nc = imgA.cols;
	int ch = imgA.channels();

	Mat retMat(imgA.rows, imgA.cols, CV_8U, cv::Scalar(100));

	for (int j = 0; j < nl; j++)
	{
		for (int i = 0; i < nc; i++)
		{
			retMat.at<uchar>(j, i) = 
				(uchar)sqrt(pow(imgA.at<Vec3b>(j, i)[0] - imgB.at<Vec3b>(j, i)[0], 2) 
					+ pow(imgA.at<Vec3b>(j, i)[1] - imgB.at<Vec3b>(j, i)[1], 2) 
					+ pow(imgA.at<Vec3b>(j, i)[2] - imgB.at<Vec3b>(j, i)[2], 2));
		}
	}

	return retMat;
}

 int main()
{

	Mat frame_a = imread("1.jpg");
	Mat frame_b = imread("2.jpg");
	if (!frame_b.data)
	{
		std::cout << "read image failue!" << std::endl;
		return -1;
	}
	
	// cout << frame_a.size() << endl;

	int height = frame_a.size().height;
	int width = frame_a.size().width;
	
	//Mat thresholdMat(height / 3, width / 3, CV_8U, cv::Scalar(100));

	Mat shrinkImgA(height / 3, width / 3, CV_8UC3, cv::Scalar(100));
	Mat shrinkImgB(height / 3, width / 3, CV_8UC3, cv::Scalar(100));
	
	//cout << height / 3 << " - " << width / 3 << endl;

	Mat roi3x3A(3, 3, CV_8UC3, cv::Scalar(100));
	Mat roi3x3B(3, 3, CV_8UC3, cv::Scalar(100));

	int offset = 3;

	int nl = frame_a.rows;
	int nc = frame_a.cols;
	
	
	//遍历图像的每个像素  
	int clo = 0, row = 0;
	for (int j = 0; j < (nl/3)*3; j += offset)
	{
		// 拷贝每列的元素
		for (int i = 0; i < (nc/3)*3; i += offset)
		{
			for (int k = 0; k < 3; k++)
			{	
				for (int nRoi = 0; nRoi < 3; nRoi++)
				{
					//cout << k << " - " << nRoi << " - " << j + k << " - " << i + nRoi << endl;
					roi3x3A.at<Vec3b>(k, nRoi)[0] = frame_a.at<Vec3b>(j+k, i+nRoi)[0]; // B/G/R
					roi3x3A.at<Vec3b>(k, nRoi)[1] = frame_a.at<Vec3b>(j+k, i+nRoi)[1]; // B/G/R
					roi3x3A.at<Vec3b>(k, nRoi)[2] = frame_a.at<Vec3b>(j+k, i+nRoi)[2]; // B/G/R

					roi3x3B.at<Vec3b>(k, nRoi)[0] = frame_b.at<Vec3b>(j + k, i + nRoi)[0]; // B/G/R
					roi3x3B.at<Vec3b>(k, nRoi)[1] = frame_b.at<Vec3b>(j + k, i + nRoi)[1]; // B/G/R
					roi3x3B.at<Vec3b>(k, nRoi)[2] = frame_b.at<Vec3b>(j + k, i + nRoi)[2]; // B/G/R
				}
			}
			//cout << j/3 << " - "<< i/3 << endl;
			
			shrinkImgA.at<Vec3b>(j/3, i/3)[0] = mean(roi3x3A)[0];
			shrinkImgA.at<Vec3b>(j/3, i/3)[1] = mean(roi3x3A)[1];
			shrinkImgA.at<Vec3b>(j/3, i/3)[2] = mean(roi3x3A)[2];

			shrinkImgB.at<Vec3b>(j / 3, i / 3)[0] = mean(roi3x3B)[0];
			shrinkImgB.at<Vec3b>(j / 3, i / 3)[1] = mean(roi3x3B)[1];
			shrinkImgB.at<Vec3b>(j / 3, i / 3)[2] = mean(roi3x3B)[2];

		}
	}
	Mat ret = absSqrt(shrinkImgA, shrinkImgB);
	Mat thresholdMat;
	threshold(ret, thresholdMat, 100, 255, 3);
	
	namedWindow("img");
	imshow("img", thresholdMat);
	waitKey();
}

