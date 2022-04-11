#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	cv::VideoCapture cap("D:\\TestContents\\movie\\Aladdin.2019.1080p.HDRip.x264.6CH-MkvCage.com.mkv");

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
	cout << "end of VideoFileCapture" << endl;
	return 0;
}