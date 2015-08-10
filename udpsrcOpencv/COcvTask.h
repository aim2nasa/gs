#ifndef __COCVTASK_H__
#define __COCVTASK_H__

#include "ace/task.h"

class COcvTask : public ACE_Task < ACE_MT_SYNCH >
{
public:
	COcvTask();
	virtual int svc(void);

	int _width;
	int _height;
};

#endif