//
// Created by bian on 2019/10/10.
//

#ifndef NDKPROJECT_MP3_ENCODER_H
#define NDKPROJECT_MP3_ENCODER_H

#endif //NDKPROJECT_MP3_ENCODER_H

#include <stdio.h>
#include "jni.h"
#include "../libmp3lame/lame.h"

class Mp3Encoder {
private:
    FILE *pcmFile;
    FILE *mp3File;
    lame_t lameClient;
public:
    Mp3Encoder();

    ~Mp3Encoder();

    int Init(const char *pcmFilePath, const char *mp3FilePath,
             int sampleRate, int channels, int bitRate);

    void Encode();

    void Destory();
};