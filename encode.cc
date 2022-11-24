extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
  #include <libavutil/opt.h>
  #include <libavutil/imgutils.h>
}
#include <iostream>

void encode(AVCodecContext *aCodecCtx, AVFrame *frame, AVPacket *pkt, AVFormatContext *ctx) {
  int ret = avcodec_send_frame(aCodecCtx, frame);
  while(ret >= 0) {
    ret = avcodec_receive_packet(aCodecCtx, pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      break;
    }
    else if(ret < 0) {
      exit(1);
    }
    pkt->stream_index = 0;
    av_packet_rescale_ts(pkt, aCodecCtx->time_base, ctx->streams[0]->time_base);
    av_interleaved_write_frame(ctx, pkt);
    av_packet_unref(pkt);
  }
}

void encode_v(AVCodecContext *vCodecCtx, AVFrame *frame, AVPacket *pkt, AVFormatContext *ctx) {
  int ret = avcodec_send_frame(vCodecCtx, frame);
  while(ret >= 0) {
    ret = avcodec_receive_packet(vCodecCtx, pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      return;
    }
    else if(ret < 0) {
      exit(1);
    }
    pkt->stream_index = 1;
    av_packet_rescale_ts(pkt, vCodecCtx->time_base, ctx->streams[1]->time_base);
    av_interleaved_write_frame(ctx, pkt);
    av_packet_unref(pkt);
  }
}

int main(int argc, char** argv) {
    std::cout << "Start" << std::endl;
    AVFormatContext *ctx = NULL;

    avformat_alloc_output_context2(&ctx, NULL, "mp4", NULL);
    
    const AVCodec *aCodec = avcodec_find_encoder(ctx->oformat->audio_codec);
    AVCodecContext *aCodecCtx = avcodec_alloc_context3(aCodec);
    aCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    aCodecCtx->sample_rate = 44100;
    aCodecCtx->ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    aCodecCtx->bit_rate = 256000;
    avcodec_open2(aCodecCtx, aCodec, NULL);
    const AVSampleFormat *p = aCodec->sample_fmts;
    while(*p != AV_SAMPLE_FMT_NONE) {
      std::cout << *p << std::endl;
      p++;
    }

    const AVCodec *vCodec = avcodec_find_encoder(ctx->oformat->video_codec);
    AVCodecContext *vCodecCtx = avcodec_alloc_context3(vCodec);
    vCodecCtx->bit_rate = 64000;
    vCodecCtx->width = 352;
    vCodecCtx->height = 288;
    vCodecCtx->time_base = (AVRational){1, 25};
    vCodecCtx->framerate = (AVRational){25, 1};
    vCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    vCodecCtx->gop_size = 10;
    vCodecCtx->max_b_frames = 1;

    avcodec_open2(vCodecCtx, vCodec, NULL);

    AVStream *oStream = avformat_new_stream(ctx, aCodec);
    oStream->time_base = (AVRational){ 1, aCodecCtx->sample_rate };
    avcodec_parameters_from_context(oStream->codecpar, aCodecCtx);
    
    AVStream *vStream = avformat_new_stream(ctx, vCodec);
    avcodec_parameters_from_context(vStream->codecpar, vCodecCtx);
    vStream->time_base = vCodecCtx->time_base;

    av_dump_format(ctx, 0, "foo.mp4", 1);

    avio_open(&ctx->pb, "foo.mp4", AVIO_FLAG_WRITE);

    AVPacket *pkt = av_packet_alloc();
    AVPacket *vPkt = av_packet_alloc();

    AVFrame *frame = av_frame_alloc();
    frame->nb_samples = aCodecCtx->frame_size;
    frame->format = aCodecCtx->sample_fmt;
    frame->ch_layout = aCodecCtx->ch_layout;

    AVFrame *vFrame = av_frame_alloc();
    vFrame->format = vCodecCtx->pix_fmt;
    vFrame->width = vCodecCtx->width;
    vFrame->height = vCodecCtx->height;
  
    av_frame_get_buffer(frame, 0);
    av_frame_get_buffer(vFrame, 0);
    int ret = 0;
    ret = avformat_write_header(ctx, NULL);
  
    float t = 0;
    float t2 = 0;
    float tincr = 2 * M_PI * 440.0 / aCodecCtx->sample_rate;
    float tincr2 = 2 * M_PI * 740.0 / aCodecCtx->sample_rate;
    float *samplesL;
    float *samplesR;
    if(ret < 0) {
      exit(1);
    }
    int64_t pts = 0;
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
      frame->pts = pts;
      pts += frame->nb_samples;
      encode(aCodecCtx, frame, pkt, ctx);
    }
    encode(aCodecCtx, NULL, pkt, ctx);

    for(int64_t i = 0; i < 250; i++) {
      av_frame_make_writable(vFrame);
      for(int y = 0; y < vCodecCtx->height; y++) {
        for(int x = 0; x < vCodecCtx->width; x++) {
          vFrame->data[0][y * vFrame->linesize[0] + x] = x + y + i * 3;
        }
      }
      for(int y = 0; y < vCodecCtx->height/2; y++) {
        for(int x = 0; x < vCodecCtx->width/2; x++) {
          vFrame->data[1][y * vFrame->linesize[1] + x] = 128 + y + i * 2;
          vFrame->data[2][y * vFrame->linesize[2] + x] = 64 + x + i * 5;
        }
      }
      vFrame->pts = i;
      encode_v(vCodecCtx, vFrame, vPkt, ctx);
    }
    encode_v(vCodecCtx, NULL, vPkt, ctx);

    av_write_trailer(ctx);

    avio_closep(&ctx->pb);
    avformat_free_context(ctx);

    avcodec_free_context(&aCodecCtx);
    avcodec_free_context(&vCodecCtx);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    av_frame_free(&vFrame);
    av_packet_free(&vPkt);
    return 0;
}