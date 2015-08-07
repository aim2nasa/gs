#include "ace/Log_Msg.h" 
#include "CGsTask.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));

	CGsTask gs;
	gs.activate();

	gs.wait();
	ACE_DEBUG((LM_INFO, "end\n"));
	ACE_RETURN(0);
}