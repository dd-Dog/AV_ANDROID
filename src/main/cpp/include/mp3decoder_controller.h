//
// Created by bian on 2019/10/22.
//

#ifndef NDKPROJECT_MP3DECODER_CONTROLLER_H
#define NDKPROJECT_MP3DECODER_CONTROLLER_H


#include <stdio.h>
#include "ffmpegmp3decoder.h"
#define CHANNEL_PER_FRAME	2
#define BITS_PER_CHANNEL		16
#define BITS_PER_BYTE		8

class AccompanyDecoderController {
protected:
    FILE* pcmFile;

    /** 伴奏的解码器 **/
    FFmpegMp3Decoder* accompanyDecoder;

    /** 伴奏和原唱的采样频率与解码伴奏和原唱的每个packet的大小 **/
    int accompanySampleRate;
    int accompanyPacketBufferSize;
public:
    AccompanyDecoderController();
    ~AccompanyDecoderController();

    /** 初始两个decoder,并且根据上一步算出的采样率，计算出伴奏和原唱的bufferSize **/
    int Init(const char* accompanyPath, const char* pcmFilePath);
    /** 解码操作 **/
    void Decode();
    /** 销毁这个controller **/
    void Destroy();
};
#endif //NDKPROJECT_MP3DECODER_CONTROLLER_H


