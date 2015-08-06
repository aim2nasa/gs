#include <gst/gst.h>

using namespace std;

//gst-launch-1.0.exe -v tcpclientsrc host=192.168.219.124 port=5000 ! gdpdepay ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink sync=false
int main(int argc, char *argv[])
{
	GstElement *pipeline, *source, *sink, *convert, *gdpay, *rtphdpay, *avdec;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	/* Initialize GStreamer */
	gst_init(&argc, &argv);

	/* Create the elements */
	source = gst_element_factory_make("tcpclientsrc", "source");
	gdpay = gst_element_factory_make("gdpdepay", "gdpay");
	rtphdpay = gst_element_factory_make("rtph264depay", "rtphdpay");
	avdec = gst_element_factory_make("avdec_h264", "avdec");
	convert = gst_element_factory_make("videoconvert", "convert");
	sink = gst_element_factory_make("autovideosink", "sink");

	/* Create the empty pipeline */
	pipeline = gst_pipeline_new("test-pipeline");

	if (!pipeline || !source || !sink || !convert || !gdpay || !rtphdpay || !avdec) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Build the pipeline */
	gst_bin_add_many(GST_BIN(pipeline), source, gdpay, rtphdpay, avdec, convert, sink, NULL);
	if (gst_element_link(source, gdpay) != TRUE) {
		g_printerr("source -> gdpay Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	if (gst_element_link(gdpay,rtphdpay) != TRUE) {
		g_printerr("gdpay -> rtphdpay Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	if (gst_element_link(rtphdpay, avdec) != TRUE) {
		g_printerr("rtphdpay -> avdec Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	if (gst_element_link(avdec,convert) != TRUE) {
		g_printerr("avdec -> convert Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	if (gst_element_link(convert, sink) != TRUE) {
		g_printerr("convert -> sink Elements could not be linked.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* Modify the source's properties */
	g_object_set(source, "host", "192.168.219.124", NULL);
	g_object_set(source, "port", 5000, NULL);
	g_object_set(sink, "sync", false, NULL);

	/* Start playing */
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* Wait until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

	/* Parse message */
	if (msg != NULL) {
		GError *err;
		gchar *debug_info;

		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			gst_message_parse_error(msg, &err, &debug_info);
			g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
			g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
			g_clear_error(&err);
			g_free(debug_info);
			break;
		case GST_MESSAGE_EOS:
			g_print("End-Of-Stream reached.\n");
			break;
		default:
			/* We should not reach here because we only asked for ERRORs and EOS */
			g_printerr("Unexpected message received.\n");
			break;
		}
		gst_message_unref(msg);
	}

	/* Free resources */
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return 0;
}