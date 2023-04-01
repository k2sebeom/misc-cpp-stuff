extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
#include <iostream>
#include <sndfile.hh>


int main(int argc, char* argv[]) {
    if(argc < 3) {
        return 1;
    }
    char* fname = argv[1];
    char* oname = argv[2];
    AVFormatContext *formatCtx = avformat_alloc_context();
    avformat_open_input(&formatCtx, fname, NULL, NULL);
    avformat_find_stream_info(formatCtx, NULL);

    AVStream *stream = formatCtx->streams[0];
    const AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
    AVCodecContext *codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecCtx, stream->codecpar);
    avcodec_open2(codecCtx, codec, NULL);

    av_dump_format(formatCtx, 0, fname, 0);

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    SndfileHandle file(oname, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, 2, 44100);

    while(av_read_frame(formatCtx, pkt) >= 0) {
      int ret = avcodec_send_packet(codecCtx, pkt);
      while(ret >= 0) {
        ret = avcodec_receive_frame(codecCtx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
          break;
        }
        else if(ret < 0) {
          exit(1);
        }
        float buffer[frame->nb_samples * 2];
        float *out = &buffer[0];
        if(frame->format == AV_SAMPLE_FMT_FLTP) {
          float *left = (float*)frame->data[0];
          float *right = (float*)frame->data[1];
          for(int i = 0; i < frame->nb_samples; i++) {
            *out = left[i];
            out++;
            *out = right[i];
            out++;
          }
        }
        else if(frame->format == AV_SAMPLE_FMT_FLT) {
          float *data = (float*)frame->data[0];
          for(int i = 0; i < 2 * frame->nb_samples; i++) {
            *out = *data;
            out++;
            data++;
          }
        }
        else if(frame->format == AV_SAMPLE_FMT_S16P) {
          int16_t *left = (int16_t*)frame->data[0];
          int16_t *right = (int16_t*)frame->data[1];
          for(int i = 0; i < frame->nb_samples; i++) {
            *out = left[i] / (float)32768;
            out++;
            *out = right[i] / (float)32768;
            out++;
          }
        }
        else if(frame->format == AV_SAMPLE_FMT_S16) {
          int16_t *data = (int16_t*)frame->data[0];
          for(int i = 0; i < 2 * frame->nb_samples; i++) {
            *out = *data / (float)32768;
            out++;
            data++;
          }
        }
        file.writef(buffer, frame->nb_samples);
        av_packet_unref(pkt);
      }
    }
    av_packet_free(&pkt);
    av_frame_free(&frame);
    avformat_free_context(formatCtx);
    avcodec_free_context(&codecCtx);
    return 0;
}