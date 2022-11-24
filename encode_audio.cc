extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
#include <iostream>

void encode(AVCodecContext *aCodecCtx, AVFrame *frame, AVPacket *pkt, AVFormatContext *ctx, int64_t *pts, int64_t *dts) {
  int ret = avcodec_send_frame(aCodecCtx, frame);
  while(ret >= 0) {
    ret = avcodec_receive_packet(aCodecCtx, pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      break;
    }
    else if(ret < 0) {
      exit(1);
    }
    pkt->pts = *pts;
    pkt->dts = *dts;
    *pts += aCodecCtx->frame_size;
    *dts += 1;
    av_interleaved_write_frame(ctx, pkt);
    av_packet_unref(pkt);
  }
}

int main(int argc, char** argv) {
    AVFormatContext *ctx;
    avformat_alloc_output_context2(&ctx, NULL, "mp3", NULL);

    const AVCodec *aCodec = avcodec_find_encoder(ctx->oformat->audio_codec);
    AVCodecContext *aCodecCtx = avcodec_alloc_context3(aCodec);
    aCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    aCodecCtx->sample_rate = 44100;
    aCodecCtx->ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    aCodecCtx->bit_rate = 64000;
    avcodec_open2(aCodecCtx, aCodec, NULL);

    AVStream *oStream = avformat_new_stream(ctx, aCodec);
    oStream->time_base = (AVRational){ 1, aCodecCtx->sample_rate };
    avcodec_parameters_from_context(oStream->codecpar, aCodecCtx);
    avcodec_open2(aCodecCtx, aCodec, NULL);
    av_dump_format(ctx, 0, "foo.mp3", 1);

    avio_open(&ctx->pb, "foo.mp3", AVIO_FLAG_WRITE);

    AVPacket *pkt = av_packet_alloc();

    AVFrame *frame = av_frame_alloc();
    frame->nb_samples = aCodecCtx->frame_size;
    std::cout << aCodecCtx->frame_size << std::endl;
    frame->format = aCodecCtx->sample_fmt;
    frame->ch_layout = aCodecCtx->ch_layout;

    av_frame_get_buffer(frame, 0);
    float t = 0;
    float t2 = 0;
    float tincr = 2 * M_PI * 440.0 / aCodecCtx->sample_rate;
    float tincr2 = 2 * M_PI * 540.0 / aCodecCtx->sample_rate;
    float *samplesL;
    float *samplesR;
    int ret = avformat_write_header(ctx, NULL);
    if(ret < 0) {
      exit(1);
    }
    int64_t pts = 0;
    int64_t dts = 0;
    for(int i=0; i < 200; i++) {
      av_frame_make_writable(frame);
      samplesL = (float*)frame->data[0];
      samplesR = (float*)frame->data[1];
      for (int j = 0; j < aCodecCtx->frame_size; j++) {
        samplesL[j] = sin(t);
        samplesR[j] = sin(t2);
        t += tincr;
        t2 += tincr2;
      }
      encode(aCodecCtx, frame, pkt, ctx, &pts, &dts);
    }
    std::cout << "complete" << std::endl;

    encode(aCodecCtx, NULL, pkt, ctx, &pts, &dts);
    std::cout << "null" << std::endl;
    av_write_trailer(ctx);

    avio_closep(&ctx->pb);
    avformat_free_context(ctx);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    return 0;
}