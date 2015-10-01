#include "ace/Log_Msg.h" 
#include "CGsTask.h"
#include "COcvTask.h"

#define WIDTH	80
#define HEIGHT	60

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));

	COcvTask ocv;
	CGsTask gs;

	CGsTask::s_consumer = &ocv;

	gs.activate();

	ocv._width = CGsTask::s_width = WIDTH;
	ocv._height = CGsTask::s_height = HEIGHT;
	ACE_DEBUG((LM_DEBUG, "from stream, width:%d height:%d\n", ocv._width,ocv._height));
	ocv.activate();

	ocv.wait();
	gs.wait();
	ACE_DEBUG((LM_INFO, "end\n"));
	ACE_RETURN(0);
}