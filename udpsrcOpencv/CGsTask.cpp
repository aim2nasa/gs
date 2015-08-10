#include "CGsTask.h"

ACE_Task<ACE_MT_SYNCH>* CGsTask::s_consumer = NULL;
ACE_Auto_Event CGsTask::s_sampleEvt;
int CGsTask::s_width = 0;
int CGsTask::s_height = 0;

CGsTask::CGsTask()
{
	gst_init(NULL, NULL);
}

int CGsTask::svc(void)
{
	gchar *descr = g_strdup(
		"udpsrc port = 52156 ! "
		"gdpdepay ! "
		"rtph264depay ! "
		"avdec_h264 ! "
		"videoconvert ! "
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
	GstCaps *caps = gst_sample_get_caps(sample);
	GstStructure *s = gst_caps_get_structure(caps, 0);
	GstBuffer *buffer = gst_sample_get_buffer(sample);
	const GstStructure *info = gst_sample_get_info(sample);

	GstMapInfo map;
	gst_buffer_map(buffer, &map, GST_MAP_READ);

	gst_structure_get_int(s, "width", &CGsTask::s_width);
	gst_structure_get_int(s, "height", &CGsTask::s_height);
	//ACE_DEBUG((LM_DEBUG, "width:%d height:%d\n", &CGsTask::s_width, &CGsTask::s_height));

	//enqueue frame
	ACE_Message_Block *pBlock = new ACE_Message_Block(map.size);
	pBlock->copy(reinterpret_cast<const char*>(map.data),map.size);
	s_consumer->putq(pBlock);

	gst_buffer_unmap(buffer, &map);
	gst_sample_unref(sample);
	CGsTask::s_sampleEvt.signal();
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