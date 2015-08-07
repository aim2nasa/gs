#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <stdlib.h>

GstFlowReturn new_preroll(GstAppSink *appsink, gpointer data) {
	g_print("Got preroll!\n");
	return GST_FLOW_OK;
}

GstFlowReturn new_sample(GstAppSink *appsink, gpointer data) {
	g_print(">");
	return GST_FLOW_OK;
}

static gboolean my_bus_callback(GstBus *bus, GstMessage *message, gpointer data) {
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

int main(int argc, char *argv[])
{
	GError *error = NULL;

	gst_init(&argc, &argv);

	gchar *descr = g_strdup(
		"udpsrc port = 52156 ! "
		"gdpdepay ! "
		"rtph264depay ! "
		"avdec_h264 ! "
		"videoconvert ! "
		//"autovideosink sync = false"
		"appsink name=sink sync = true"
		);

	//gchar *descr = g_strdup(
	//	"tcpclientsrc host = 192.168.219.124 port=5000 ! "
	//	"gdpdepay ! "
	//	"rtph264depay ! "
	//	"avdec_h264 ! "
	//	"videoconvert ! "
	//	"appsink name=sink sync = true"
	//	);

	//gchar *descr = g_strdup(
	//	"videotestsrc  pattern=ball ! "
	//	"video/x-raw,format=RGB ! "
	//	"videoconvert ! "
	//	"appsink name=sink sync=true"
	//	);

	GstElement *pipeline = gst_parse_launch(descr, &error);

	if (error != NULL) {
		g_print("could not construct pipeline: %s\n", error->message);
		g_error_free(error);
		exit(-1);
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

	//namedWindow("edges", 1);
	while (1) {
		g_main_iteration(false);
	}

	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));

	return 0;
}