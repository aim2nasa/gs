#ifndef __CGSTASK_H__
#define __CGSTASK_H__

#include "ace/task.h"
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

class CGsTask : public ACE_Task < ACE_MT_SYNCH >
{
public:
	CGsTask();
	virtual int svc(void);

	static GstFlowReturn new_preroll(GstAppSink *appsink, gpointer data);
	static GstFlowReturn new_sample(GstAppSink *appsink, gpointer data);
	static gboolean my_bus_callback(GstBus *bus, GstMessage *message, gpointer data);
};

#endif