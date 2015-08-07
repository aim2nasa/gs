#include "COcvTask.h"
#include <opencv2/opencv.hpp>

COcvTask::COcvTask()
{

}

int COcvTask::svc(void)
{
	ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc start\n"));
	ACE_Message_Block *message;
	for (;;){
		if (this->getq(message) == -1) ACE_ERROR_RETURN((LM_ERROR, "%p\n", "getq"), -1);

		if (message->msg_type() == ACE_Message_Block::MB_HANGUP) {
			message->release();
			ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc MB_HANGUP received\n"));
			break;
		}

		cv::Mat frame(cv::Size(1920, 1080), CV_8SC1, message->rd_ptr(), cv::Mat::AUTO_STEP);

		ACE_DEBUG((LM_DEBUG, "."));
		message->release();
	}
	ACE_DEBUG((LM_DEBUG, "(%t) COcvTask::svc end\n"));
	return 0;
}