//
// Created by bian on 2019/10/21.
//

#ifndef NDKPROJECT_FFMPEG_TEST_H
#define NDKPROJECT_FFMPEG_TEST_H



#include <stdio.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/pixdesc.h"
};

typedef struct AudioPacket {

    static const int AUDIO_PACKET_ACTION_PLAY = 0;
    static const int AUDIO_PACKET_ACTION_PAUSE = 100;
    static const int AUDIO_PACKET_ACTION_SEEK = 101;

    short *buffer;
    int size;
    float position;
    int action;

    float extra_param1;
    float extra_param2;

    AudioPacket() {
        buffer = NULL;
        size = 0;
        position = -1;
        action = 0;
        extra_param1 = 0;
        extra_param2 = 0;
    }

    ~AudioPacket() {
//		__android_log_print(ANDROID_LOG_ERROR, "~AudioPacket", "delete AudioPacket");
        if (NULL != buffer) {
//			__android_log_print(ANDROID_LOG_ERROR, "~AudioPacket", "buffer's addr is %x", buffer);
//			__android_log_print(ANDROID_LOG_ERROR, "~AudioPacket", "delete buffer ...");
            delete[] buffer;
            buffer = NULL;
//			__android_log_print(ANDROID_LOG_ERROR, "~AudioPacket", "delete buffer success");
        }
//		__android_log_print(ANDROID_LOG_ERROR, "~AudioPacket", "delete AudioPacket success");
    }
} AudioPacket;

#ifndef INT64_MIN
#define INT64_MIN  (-9223372036854775807LL - 1)
#endif

#define OUT_PUT_CHANNELS 2

#define LOG_TAG "Mp3Encorder"
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,FORMAT,##__VA_ARGS__);


class FFmpegMp3Decoder {
private:
    FILE *srcFile;
    FILE *outFile;
    int videoStreamIndex;
    int audioStreamIndex;

    AVFormatContext *formatContext;

    //音视频流解码器
    AVCodecContext *videoCodexCtx;
    AVCodecContext *audioCodecCtx;

    int packetBufferSize;
    AVFrame *audioFrame;
    AVFrame *videoFrame;

    SwsContext *swsContext;
    SwrContext *swrContext;

    AVPacket avPacket;
    short *audioBuffer;

    float position;
    float timeBase;

    bool pictureValid;
    AVPicture picture;

    bool isNeedFirstFrameCorrectFlag;
    float firstFrameCorrectionInSecs;

    //输出音频文件的时长
    float duration;

    //读取音频流的缓冲区的标志
    int audioBufferCursor;

    int audioBufferSize;

    void *swrBuffer;
    int swrBufferSize;

    int Decode();
    int Init(const char* fileString);
public:
    FFmpegMp3Decoder();

    ~FFmpegMp3Decoder();

    //获取采样率以及比特率
    virtual int getMusicMeta(const char *fileString, int *metaData);

    int readSamples(short *samples, int size);

    int readFrame();

    void handleVideoFrame(int channels);

    virtual AudioPacket *decodePacket();

    int handleAudioFrame();

    //初始化这个decoder，即打开指定的mp3文件
    virtual int Init(const char *fileString, int packetBufferSize);

    void Destroy();

};

#endif //NDKPROJECT_FFMPEG_TEST_H