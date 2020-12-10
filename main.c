#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h> // for AV_PIX_FMT_RGB24 and AVPixelFormat
#define SAMPLEAVIFILE "sample_1920x1080.avi"
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  printf("%d * %d\n" , width, height);
  
  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  //
/* size_t fwrite((1)const void *ptr, (2)size_t size, (3)size_t nmemb, */
                     /* (4)FILE *stream); */
/* The function fwrite() writes nmemb items of data, each size bytes long, to the stream pointed to by stream, obtaining  them  from  the */
/*        location given by ptr. */
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}



int main(int argc, char *argv[]) {
    AVFormatContext *pFormatCtx = NULL;
    AVCodecContext *pCodecCtx= NULL;
    AVCodec *pCodec = NULL;
    AVDictionary **ptrToDictList = NULL;
    AVFrame * pFrame = NULL;
    AVPacket pPacket;
    struct SwsContext * swsCtx;
    char * filename;
    int receivedFrame;
    int vidStreamIdx = 0;
    unsigned int nBytes;  
    uint8_t * buffer = NULL;

    av_register_all();
    if (argv[1] == NULL){
        filename = malloc(strlen(SAMPLEAVIFILE) * sizeof(char));
        printf("%s\n", "using the default sample file");
        strcpy(filename, SAMPLEAVIFILE);
    }
    else{
        filename = malloc(strlen(argv[1]) * sizeof(char));
        strcpy(filename, argv[1]);
    }
    printf("using %s\n", filename);

    avformat_open_input(&pFormatCtx, filename, NULL, NULL);
    // we opened the file now we should be able to
    // manipulate the audio and video of it
    for (int i = 0; i < pFormatCtx->nb_streams; i++){
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            vidStreamIdx = i;
            break;
        }
        printf("%d\n" , vidStreamIdx);
    }
    pCodecCtx = pFormatCtx->streams[vidStreamIdx]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    avcodec_open2(pCodecCtx, pCodec, ptrToDictList);
    pFrame = av_frame_alloc();
    buffer = av_malloc(nBytes * sizeof(uint8_t));

    int i = 0;

    while(av_read_frame(pFormatCtx, &pPacket) >= 0){
        // keep reading into a frame
        if (pPacket.stream_index == vidStreamIdx){
            avcodec_decode_video2(pCodecCtx, pFrame, &receivedFrame, &pPacket);
            if(receivedFrame) {
                receivedFrame = 0;
                // Save the frame to disk
                if(++i<=5){
                    SaveFrame(pFrame, pCodecCtx->width, pCodecCtx->height, i);
                }
            }
        }
    }
    return 0;
}


