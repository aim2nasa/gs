#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	cv::VideoCapture cap(-1);

	if (!cap.isOpened()) {
		cout << "capture device not opened" << endl;
		return -1;
	}

	cv::namedWindow("capture", 1);
	for (;;)
	{
		cv::Mat frame;
		cap >> frame;
		cv::imshow("capture", frame);
		if (cv::waitKey(30) >= 0) break;
	}
	cout << "videoCapture end" << endl;
	return 0;
}