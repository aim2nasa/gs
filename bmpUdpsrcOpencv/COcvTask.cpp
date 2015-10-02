#include "COcvTask.h"
#include "opencv2/opencv.hpp"

#undef BMP_DUMP
#define ZOOM	4

COcvTask::COcvTask()
:_width(0), _height(0)
{

}

int COcvTask::svc(void)
{
	ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc start\n"));

#if BMP_DUMP
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

		ACE_ASSERT(_width > 0 && _height > 0);
		size_t bmpHdrSize = message->size() - (_width*_height * 3 /*RGB*/);
		cv::Mat frame(cv::Size(_width, _height), CV_8UC3, message->rd_ptr() + bmpHdrSize, cv::Mat::AUTO_STEP);
		cv::flip(frame, frame, 0);

#if BMP_DUMP
		char name[256];
		sprintf_s(name, "%d.bmp", n);
		FILE *fp = fopen(name, "wb");
		fwrite(message->rd_ptr(), 1, message->size(), fp);
		fclose(fp);
		n++;
#endif

		cv::resize(frame, frame, cv::Size(_width * ZOOM, _height * ZOOM));
		cv::imshow("capture", frame);
		cv::waitKey(1);

		ACE_DEBUG((LM_DEBUG, "."));
		message->release();
	}
	ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc end\n"));
	return 0;
}