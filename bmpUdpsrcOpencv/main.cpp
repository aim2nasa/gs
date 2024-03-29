#include "ace/Log_Msg.h" 
#include "CGsTask.h"
#include "COcvTask.h"

#define WIDTH	80
#define HEIGHT	60

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));
	ACE_LOG_MSG->priority_mask(LM_INFO | LM_ERROR | LM_DEBUG, ACE_Log_Msg::PROCESS);

	COcvTask ocv;
	CGsTask gs;

	CGsTask::s_consumer = &ocv;

	ocv._width = CGsTask::s_width = WIDTH;
	ocv._height = CGsTask::s_height = HEIGHT;
	ACE_DEBUG((LM_DEBUG, "width:%d height:%d\n", ocv._width,ocv._height));
	gs.activate();
	ocv.activate();

	ocv.wait();
	gs.wait();
	ACE_DEBUG((LM_INFO, "end\n"));
	ACE_RETURN(0);
}