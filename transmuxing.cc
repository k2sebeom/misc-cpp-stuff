extern "C" {
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
#include <iostream>


int main(int argc, const char* argv[]) {
    AVFormatContext *inputFormat = avformat_alloc_context();
    avformat_open_input(&inputFormat, "./foo.mp4", NULL, NULL);
    avformat_find_stream_info(inputFormat, NULL);
    

    avformat_free_context(inputFormat);
    return 0;
}