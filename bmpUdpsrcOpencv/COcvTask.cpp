#include "COcvTask.h"
#include "opencv2/opencv.hpp"

COcvTask::COcvTask()
:_width(0), _height(0)
{

}

int COcvTask::svc(void)
{
	ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc start\n"));

	cv::VideoCapture cap(-1);
	if (!cap.isOpened()) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "device not opened"), -1);

#if 0
	int n = 0;
#endif

	cv::namedWindow("capture", 1);
	ACE_Message_Block *message;
	for (;;){
		if (this->getq(message) == -1) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "getq"), -1);

		if (message->msg_type() == ACE_Message_Block::MB_HANGUP) {
			message->release();
			ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc MB_HANGUP received\n"));
			break;
		}

		cv::Mat frame(cv::Size(_width, _height), CV_8UC3, message->rd_ptr(), cv::Mat::AUTO_STEP);

#if 0
		char name[256];
		sprintf_s(name, "%d.bmp", n);
		FILE *fp = fopen(name, "wb");
		fwrite(message->rd_ptr(), 1, message->size(), fp);
		fclose(fp);
		n++;
#endif

		cv::Mat frame1;
		cv::resize(frame, frame1, cv::Size(_width * 5, _height * 5));
		cv::imshow("capture", frame1);
		cv::waitKey(1);

		ACE_DEBUG((LM_DEBUG, "."));
		message->release();
	}
	ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc end\n"));
	return 0;
}