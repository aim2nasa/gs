#include "ace/Log_Msg.h" 
#include "CGsTask.h"
#include "COcvTask.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));

	COcvTask ocv;
	CGsTask gs;

	CGsTask::s_consumer = &ocv;

	ocv.activate();
	gs.activate();

	ocv.wait();
	gs.wait();
	ACE_DEBUG((LM_INFO, "end\n"));
	ACE_RETURN(0);
}