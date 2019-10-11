//
// Created by bian on 2019/10/8.
//
#include <android/log.h>
#include <mp3_encoder.h>
#include "include/com_flyscale_mp3encoder_Mp3Encoder.h"

#define LOG_TAG "Mp3Encorder"
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,FORMAT,##__VA_ARGS__);

Mp3Encoder *encoder;

JNIEXPORT jint JNICALL Java_com_flyscale_mp3encoder_Mp3Encoder_init
        (JNIEnv *env, jobject jclazz, jstring pcmFileParam, jint channels,
         jint bitRate, jint sampleRate, jstring mp3FileParam) {
    int ret = -1;
    const char *pcmPath = env->GetStringUTFChars(pcmFileParam, NULL);
    const char *mp3Path = env->GetStringUTFChars(mp3FileParam, NULL);
    encoder = new Mp3Encoder();
    ret = encoder->Init(pcmPath, mp3Path, sampleRate, channels, bitRate);
    env->ReleaseStringUTFChars(mp3FileParam, mp3Path);
    env->ReleaseStringUTFChars(pcmFileParam, pcmPath);
    return ret;
}

JNIEXPORT void JNICALL Java_com_flyscale_mp3encoder_Mp3Encoder_encode(JNIEnv *env, jobject jclazz) {
    LOGI("encoder encode");
    encoder->Encode();
}

JNIEXPORT void JNICALL Java_com_flyscale_mp3encoder_Mp3Encoder_destroy
        (JNIEnv *env, jobject jclazz) {
    encoder->Destory();
}