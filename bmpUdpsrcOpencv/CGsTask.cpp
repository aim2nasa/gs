#include "CGsTask.h"

ACE_Task<ACE_MT_SYNCH>* CGsTask::s_consumer = NULL;
int CGsTask::s_width = 0;
int CGsTask::s_height = 0;
std::vector<char> CGsTask::s_buffer;
char CGsTask::s_bmpHeader[] = { "BM" };

CGsTask::CGsTask()
{
	gst_init(NULL, NULL);
}

int CGsTask::svc(void)
{
	gchar *descr = g_strdup(
		"udpsrc port = 52157 ! "
		"appsink name=sink sync = true"
		);

	GError *error = NULL;
	GstElement *pipeline = gst_parse_launch(descr, &error);

	if (error != NULL) {
		g_error_free(error);
		return -1;
	}

	/* get sink */
	GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

	gst_app_sink_set_emit_signals((GstAppSink*)sink, true);
	gst_app_sink_set_drop((GstAppSink*)sink, true);
	gst_app_sink_set_max_buffers((GstAppSink*)sink, 1);
	GstAppSinkCallbacks callbacks = { NULL, new_preroll, new_sample };
	gst_app_sink_set_callbacks(GST_APP_SINK(sink), &callbacks, NULL, NULL);

	GstBus *bus;
	guint bus_watch_id;
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	bus_watch_id = gst_bus_add_watch(bus, my_bus_callback, NULL);
	gst_object_unref(bus);

	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

	while (1) {
		g_main_iteration(false);
	}

	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));
	return 0;
}

GstFlowReturn CGsTask::new_preroll(GstAppSink *appsink, gpointer data)
{
	ACE_DEBUG((LM_DEBUG, "preroll\n"));
	return GST_FLOW_OK;
}

GstFlowReturn CGsTask::new_sample(GstAppSink *appsink, gpointer data)
{
	ACE_DEBUG((LM_DEBUG, ">"));
	GstSample *sample = gst_app_sink_pull_sample(appsink);
	GstBuffer *buffer = gst_sample_get_buffer(sample);

	GstMapInfo map;
	gst_buffer_map(buffer, &map, GST_MAP_READ);

	s_buffer.insert(s_buffer.end(), map.data, map.data + map.size);
	if (s_width > 0 && s_height > 0) {
		const int nBmpSize = s_width*s_height * 3/*RGB*/ + 54 /*Header size*/;
		if (s_buffer.size() > nBmpSize) {
			if(fetchFrame(s_buffer,s_bmpHeader,nBmpSize)>0)
				ACE_DEBUG((LM_DEBUG, "+"));
			else
				ACE_DEBUG((LM_DEBUG, "~"));
		}
	}

	gst_buffer_unmap(buffer, &map);
	gst_sample_unref(sample);
	return GST_FLOW_OK;
}

gboolean CGsTask::my_bus_callback(GstBus *bus, GstMessage *message, gpointer data)
{
	g_print("Got %s message\n", GST_MESSAGE_TYPE_NAME(message));
	switch (GST_MESSAGE_TYPE(message)) {
	case GST_MESSAGE_ERROR: {
		GError *err;
		gchar *debug;

		gst_message_parse_error(message, &err, &debug);
		g_print("Error: %s\n", err->message);
		g_error_free(err);
		g_free(debug);
		break;
	}
	case GST_MESSAGE_EOS:
		/* end-of-stream */
		break;
	default:
		/* unhandled message */
		break;
	}
	/* we want to be notified again the next time there is a message
	* on the bus, so returning TRUE (FALSE means we want to stop watching
	* for messages on the bus and our callback should not be called again)
	*/
	return TRUE;
}

int CGsTask::fetchFrame(std::vector<char>& buffer, char bmpHeader[], int nBmpSize)
{
	memcpy(bmpHeader + 2, &nBmpSize, sizeof(int));

	ACE_DEBUG((LM_CRITICAL, "buffer:%d,bmpSize:%d", buffer.size(), nBmpSize));

	int nCount = 0, nDelCount=0;
	while (buffer.size() > nBmpSize) {
		if (findHeader(buffer, bmpHeader)) {
			ACE_Message_Block *pBlock = new ACE_Message_Block(nBmpSize);
			pBlock->copy(&buffer[0], nBmpSize);
			s_consumer->putq(pBlock);
			buffer.erase(buffer.begin(), buffer.begin() + nBmpSize);
			nCount++;
		}
		else{
			buffer.erase(buffer.begin());
			nDelCount++;
		}
	}
	ACE_DEBUG((LM_CRITICAL, ",Buf:%d, Que:%d Erased:%d \n", buffer.size(), nCount, nDelCount));
	return nCount;
}

bool CGsTask::findHeader(std::vector<char>& buffer,char header[])
{
	for (int i = 0; i < sizeof(header); i++)
		if (buffer[i] != header[i])
			return false;

	return true;
}