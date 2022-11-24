extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
#include <iostream>


int main(int argc, const char* argv[]) {
    AVFormatContext* inputFormatContext = NULL;
    AVFormatContext* outputFormatContext = NULL;

    avformat_open_input(&inputFormatContext, "./splash.mp4", NULL, NULL);
    avformat_find_stream_info(inputFormatContext, NULL);
    std::cout << inputFormatContext->nb_streams << std::endl;

    avformat_alloc_output_context2(&outputFormatContext, NULL, NULL, "bar.mp4");

    for (int i = 0; i < inputFormatContext->nb_streams; i++) {
        AVStream *inStream = inputFormatContext->streams[i];

        AVCodecParameters *inParams = inStream->codecpar;
        AVStream *outStream = avformat_new_stream(outputFormatContext, NULL);

        avcodec_parameters_copy(outStream->codecpar, inParams);
    }

    avio_open(&outputFormatContext->pb, "bar.mp4", AVIO_FLAG_WRITE);

    int ret = avformat_write_header(outputFormatContext, NULL);
    std::cout << ret << std::endl;
    AVPacket packet;
    while (1)
    {   
        ret = av_read_frame(inputFormatContext, &packet);
        if (ret < 0) {
            break;
        }
        // AVStream *inStream = inputFormatContext->streams[packet.stream_index];
        // AVStream *outStream = outputFormatContext->streams[packet.stream_index];
        // packet.pts = av_rescale_q_rnd(packet.pts, inStream->time_base, outStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        // packet.dts = av_rescale_q_rnd(packet.dts, inStream->time_base, outStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        // packet.duration = av_rescale_q(packet.duration, inStream->time_base, outStream->time_base);

        // packet.pos = -1;

        av_interleaved_write_frame(outputFormatContext, &packet);

        av_packet_unref(&packet);
    }
    av_write_trailer(outputFormatContext);

    avformat_close_input(&inputFormatContext);
    avio_closep(&outputFormatContext->pb);
    avformat_free_context(outputFormatContext);
    avformat_free_context(inputFormatContext);
    return 0;
}