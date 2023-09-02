#include <iostream>
#include <gst/gst.h>

int main(int argc, char* argv[]) {

    gst_init(&argc, &argv);

    if (argc != 3) {
        std::cerr << "U must need: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }

    const char* inputFileName = argv[1];
    const char* outputFileName = argv[2];


    GstElement* pipeline = gst_pipeline_new("video-player");
    GstElement* source = gst_element_factory_make("filesrc", "file-source");
    GstElement* decode = gst_element_factory_make("decodebin", "decode");
    GstElement* sink = gst_element_factory_make("autovideosink", "video-sink");
    GstElement* audioSink = gst_element_factory_make("autoaudiosink", "audio-sink");
    GstElement* muxer = gst_element_factory_make("avmux_mp4", "muxer");
    GstElement* filesink = gst_element_factory_make("filesink", "file-sink");

    if (!pipeline || !source || !decode || !sink || !muxer || !filesink) {
        std::cerr << "failed elements creation" << std::endl;
        return 1;
    }


    g_object_set(G_OBJECT(source), "location", inputFileName, NULL);
    g_object_set(G_OBJECT(filesink), "location", outputFileName, NULL);


    gst_bin_add_many(GST_BIN(pipeline), source, decode, sink, audioSink, muxer, filesink, NULL);


    if (!gst_element_link(source, decode) || !gst_element_link(decode, sink) ||
        !gst_element_link(decode, audioSink) || !gst_element_link(sink, muxer) ||
        !gst_element_link(audioSink, muxer) || !gst_element_link(muxer, filesink)) {
        std::cerr << "Failed set links" << std::endl;
        return 1;
    }


    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed start playback" << std::endl;
        return 1;
    }


    GstBus* bus = gst_element_get_bus(pipeline);
    GstMessage* msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));


    if (msg != nullptr) {
        if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
            GError* err = nullptr;
            gchar* debugInfo = nullptr;
            gst_message_parse_error(msg, &err, &debugInfo);
            std::cerr << "Failed during playback: " << err->message << std::endl;
                                                                             g_error_free(err);
            g_free(debugInfo);
        } else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS) {
            std::cout << "Playback end." << std::endl;
        }
        gst_message_unref(msg);
    }


    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));

    return 0;
}
