//
// Created by bian on 2019/10/22.
//

#include <android/log.h>
#include "include/mp3decoder_controller.h"

AccompanyDecoderController::AccompanyDecoderController() {
    accompanyDecoder = NULL;
    pcmFile = NULL;
}


AccompanyDecoderController::~AccompanyDecoderController() {
}

int AccompanyDecoderController::Init(const char *accompanyPath, const char *pcmFilePath) {
    LOGI("Init,accompanyPath=%s,pcmFilePath=%s", accompanyPath, pcmFilePath);
    //初始化两个decoder
    FFmpegMp3Decoder *tempDecoder = new FFmpegMp3Decoder();
    int accompanyMetaData[2];
    int meta_ret = tempDecoder->getMusicMeta(accompanyPath, accompanyMetaData);
    delete tempDecoder;
    if(meta_ret<0){
        LOGE("init getMusicMeta failed,meta_ret=%d", meta_ret);
        return meta_ret;
    }
    LOGI("music meta get success!");
    //初始化伴奏的采样率
    accompanySampleRate = accompanyMetaData[0];
    int accompanyByteCountPerSec =
            accompanySampleRate * CHANNEL_PER_FRAME * BITS_PER_CHANNEL / BITS_PER_BYTE;
    accompanyPacketBufferSize = (int) ((accompanyByteCountPerSec / 2) * 0.2);
    accompanyDecoder = new FFmpegMp3Decoder();
    int ret = accompanyDecoder->Init(accompanyPath, accompanyPacketBufferSize);
    if (ret < 0){
        LOGE("AccompanyDecoderController::Init,ret=%d", ret);
        return ret;
    }
    pcmFile = fopen(pcmFilePath, "wb+");
    return 0;
}

void AccompanyDecoderController::Decode() {
    while (true) {
        AudioPacket *accompanyPacket = accompanyDecoder->decodePacket();
        if (-1 == accompanyPacket->size) {
            break;
        }
        fwrite(accompanyPacket->buffer, sizeof(short), accompanyPacket->size, pcmFile);
    }
}

void AccompanyDecoderController::Destroy() {
    if (NULL != accompanyDecoder) {
        accompanyDecoder->Destroy();
        delete accompanyDecoder;
        accompanyDecoder = NULL;
    }
    if (NULL != pcmFile) {
        fclose(pcmFile);
        pcmFile = NULL;
    }
}