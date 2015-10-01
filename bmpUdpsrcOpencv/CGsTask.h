#ifndef __CGSTASK_H__
#define __CGSTASK_H__

#include "ace/task.h"
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <ace/Auto_Event.h>
#include <vector>

class CGsTask : public ACE_Task < ACE_MT_SYNCH >
{
public:
	CGsTask();
	virtual int svc(void);

	static ACE_Task<ACE_MT_SYNCH> *s_consumer;
	static int s_width;
	static int s_height;

	static GstFlowReturn new_preroll(GstAppSink *appsink, gpointer data);
	static GstFlowReturn new_sample(GstAppSink *appsink, gpointer data);
	static gboolean my_bus_callback(GstBus *bus, GstMessage *message, gpointer data);
	static int fetchFrame(std::vector<char>& buffer, char bmpHeader[], int nBmpSize);
	static bool findHeader(std::vector<char>& buffer, char header[]);

protected:
	static std::vector<char> s_buffer;
	static char s_bmpHeader[6];
};

#endif