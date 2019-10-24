//
// Created by bian on 2019/10/22.
//

#include <mp3decoder_controller.h>
#include <android/log.h>
#include "include/com_flyscale_mp3encoder_Mp3Decoder.h"
#include "ffmpegmp3decoder.h"

AccompanyDecoderController *decoderController;

extern "C" JNIEXPORT jint JNICALL Java_com_flyscale_mp3encoder_Mp3Decoder_init
        (JNIEnv *env, jobject clazz, jstring mp3File, jstring pcmFile) {
    const char *mp3FileUTF = env->GetStringUTFChars(mp3File, NULL);
    const char *pcmFileUTF = env->GetStringUTFChars(pcmFile, NULL);
    LOGI("Mp3Decoder_init");
    decoderController = new AccompanyDecoderController();
    int ret = decoderController->Init(mp3FileUTF, pcmFileUTF);
    LOGI("Mp3Decoder_init,ret=%d", ret);
    env->ReleaseStringUTFChars(mp3File, mp3FileUTF);
    env->ReleaseStringUTFChars(pcmFile, pcmFileUTF);
    return ret;

}

extern "C" JNIEXPORT void JNICALL Java_com_flyscale_mp3encoder_Mp3Decoder_decode
        (JNIEnv *env, jobject clazz) {
    LOGI("enter Java_com_phuket_tour_decoder_Mp3Decoder_decode...");
    if (decoderController) {
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
