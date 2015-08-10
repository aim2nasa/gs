#ifndef __COCVTASK_H__
#define __COCVTASK_H__

#include "ace/task.h"
#include <opencv2/opencv.hpp>

class COcvTask : public ACE_Task < ACE_MT_SYNCH >
{
public:
	COcvTask();
	virtual int svc(void);
	void detectAndDisplay(cv::Mat frame);

	int _width;
	int _height;
};

#endif