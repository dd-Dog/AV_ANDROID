//
// Created by bian on 2019/10/22.
//

#include <android/log.h>
#include <accompany_decoder_controller.h>
#include "include/com_flyscale_mp3encoder_Mp3Decoder.h"

#define LOG_TAG "Mp3Decoder"

AccompanyDecoderController *decoderController;

extern "C" JNIEXPORT jint JNICALL Java_com_flyscale_mp3encoder_Mp3Decoder_init
        (JNIEnv *env, jobject clazz, jstring mp3File, jstring pcmFile) {
    const char* pcmPath = env->GetStringUTFChars(pcmFile, NULL);
    const char* mp3Path = env->GetStringUTFChars(mp3File, NULL);
    decoderController = new AccompanyDecoderController();
    decoderController->Init(mp3Path, pcmPath);
    env->ReleaseStringUTFChars(mp3File, mp3Path);
    env->ReleaseStringUTFChars(pcmFile, pcmPath);
    return 0;
}

extern "C" JNIEXPORT void JNICALL Java_com_flyscale_mp3encoder_Mp3Decoder_decode
        (JNIEnv *env, jobject clazz) {
    LOGI("enter Java_com_phuket_tour_decoder_Mp3Decoder_decode...");
    if(decoderController) {
        decoderController->Decode();
    }
    LOGI("leave Java_com_phuket_tour_decoder_Mp3Decoder_decode...");
}

extern "C" JNIEXPORT void JNICALL Java_com_flyscale_mp3encoder_Mp3Decoder_destroy
        (JNIEnv *env, jobject clazz) {
    if (decoderController) {
        decoderController->Destroy();
        delete decoderController;
        decoderController = NULL;
    }
}
