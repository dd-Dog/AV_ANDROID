//
// Created by bian on 2019/10/10.
//

#include "stdio.h"
#include <android/log.h>
#include <mp3_encoder.h>

#define LOG_TAG "Mp3Encorder"
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,FORMAT,##__VA_ARGS__);


int Mp3Encoder::Init(const char *pcmFilePath, const char *mp3FilePath,
                     int sampleRate, int channels, int bitRate) {
    int ret = -1;
    pcmFile = fopen(pcmFilePath, "rb");//rb-以二进制读取模式打开文件
    if (pcmFile) {
        mp3File = fopen(mp3FilePath, "wb");//wb-以二进制写入模式打开文件
        if (mp3File) {
            lameClient = lame_init();
            lame_set_in_samplerate(lameClient, sampleRate);//输入采样率
            lame_set_out_samplerate(lameClient, sampleRate);//输出采样率
            lame_set_num_channels(lameClient, channels);//声道个数
            lame_set_brate(lameClient, bitRate / 1000);//比特率
            lame_init_params(lameClient);
            ret = 0;
            LOGI("set lameClient params done!")
        } else {
            LOGE("open mp3File failed! path=%s", mp3FilePath);
        }

    } else {
        LOGE("open pcmFile failed! path=%s", pcmFilePath);
    }
    return ret;
}

void Mp3Encoder::Encode() {
    int channels = lame_get_num_channels(lameClient);
    LOGI("通道个数为：%d", channels);

    int bufferSize = 1024 * 256;//缓冲区大小
    short *buffer = new short[bufferSize / channels];//读取pcmFile的缓冲区

    //双声道情况时，为左右声道分配缓冲区
    short *leftBuffer = new short[bufferSize / 4]; //左声道缓冲区
    short *rightBuffer = new short[bufferSize / 4];//右声道缓冲区

    unsigned char *mp3_buffer = new unsigned char[bufferSize];

    size_t readBufferSize = 0;
    int frameSize = sizeof(short int) * channels;//一个帧的字节数
    LOGI("frameSize=%d", frameSize);
    while ((readBufferSize = fread(buffer, frameSize, bufferSize / channels,
                                   pcmFile)) > 0) {
        if (channels == 1) {
            LOGI("readBufferSize=%d", readBufferSize);
            //对pcm数据进行编码，单声道情况，右声道为空
            size_t wroteSize = lame_encode_buffer(lameClient,
                                                  (short int *) buffer,//左声道
                                                  NULL,//右声道
                                                  (int) (readBufferSize),
                                                  mp3_buffer,//编译后的数据
                                                  bufferSize);
            LOGI("wroteSize=%d", wroteSize);
            fwrite(mp3_buffer, 1, wroteSize, mp3File);//写入编码后的数据到mp3File
        } else if (channels == 2) {
            for (int i = 0; i < readBufferSize; i++) {
                if (i % 2 == 0) {
                    leftBuffer[i / 2] = buffer[i];
                } else {
                    rightBuffer[i / 2] = buffer[i];
                }
            }
            //对pcm数据进行编码
            size_t wroteSize = lame_encode_buffer(lameClient,
                                                  (short int *) leftBuffer,//左声道
                                                  (short int *) rightBuffer,//右声道
                                                  (int) (readBufferSize / 2),
                                                  mp3_buffer,//编译后的数据
                                                  bufferSize);
            fwrite(mp3_buffer, 1, wroteSize, mp3File);//写入编码后的数据到mp3File
        }
    }
    delete[] buffer;
    delete[] leftBuffer;
    delete[] rightBuffer;
    delete[] mp3_buffer;

}

void Mp3Encoder::Destory() {
    if (pcmFile) {
        fclose(pcmFile);
    }
    if (mp3File) {
        fclose(mp3File);
        lame_close(lameClient);
    }
}

Mp3Encoder::Mp3Encoder() {

}

Mp3Encoder::~Mp3Encoder() {

}
