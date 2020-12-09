#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h> // for AV_PIX_FMT_RGB24 and AVPixelFormat
#define SAMPLEAVIFILE "sample_960x540.avi"
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  //
/* size_t fwrite(const void *ptr, size_t size, size_t nmemb, */
                     /* FILE *stream); */
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
    if(avformat_open_input(&pFormatCtx, filename, NULL, NULL)!=0)
    {
        printf("couldnot open the file\n");
        return -1; // Couldn't open file
    }
    // we opened the file now we should be able to
    // manipulate the audio and video of it

    av_dump_format(pFormatCtx, 0, filename, 0);
    for (int i = 0; i < pFormatCtx->nb_streams; i++){
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO);
/*             // we found the video get the idx */
        {
            vidStreamIdx = i;
            break;
        }
        printf("%d\n" , vidStreamIdx);
    }
    pCodecCtx = pFormatCtx->streams[vidStreamIdx]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    /* pCodecCtx = avcodec_alloc_context3(pCodec); */
    avcodec_copy_context(pCodecCtx, pCodecCtx);
    if (pCodec == NULL){
        printf("got no codec \n");
    }
    if (avcodec_open2(pCodecCtx, pCodec, ptrToDictList) < 0)
    {
        printf("got nothing on open");
        return -1;
    }

    pFrame = av_frame_alloc();
    buffer = av_malloc(nBytes * sizeof(uint8_t));

    pCodecCtx->pix_fmt = AV_PIX_FMT_RGB24;
    /* swsCtx = sws_getContext(pCodecCtx->width, */
    /*     pCodecCtx->height, */
    /*     pCodecCtx->pix_fmt, */
    /*     pCodecCtx->width, */
    /*     pCodecCtx->height, */
    /*     AV_PIX_FMT_RGB24, */
    /*     SWS_BILINEAR, */
    /*     NULL, */
    /*     NULL, */
    /*     NULL */
    /*     ); */
    int i = 0;
    while(av_read_frame(pFormatCtx, &pPacket) >= 0){
        // keep reading into a frame
        if ( pPacket.stream_index == vidStreamIdx ){
            /* avcodec_send_packet(pCodecCtx, &pPacket); */
            avcodec_decode_video2(pCodecCtx, pFrame, &receivedFrame, &pPacket);
            if(receivedFrame) {
                // Convert the image from its native format to RGB
                /* sws_scale(swsCtx, (uint8_t const * const *)pFrame->data, */
                /*         pFrame->linesize, 0, pCodecCtx->height, */
                /*         pFrame->data, pFrame->linesize); */

                // Save the frame to disk
                if(++i<=5)
                {

                    /* nBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height); */
                    /* avpicture_fill((AVPicture *) pFrame, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height); */
                    SaveFrame(pFrame, pCodecCtx->width, pCodecCtx->height, i);
                }
            }
        }
    }
    return 0;
}


