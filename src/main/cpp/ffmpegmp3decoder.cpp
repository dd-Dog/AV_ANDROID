//
// Created by bian on 2019/10/21.
//
extern "C" {    //CPP引用C库，如果不加可能导致undefined reference问题
#include "ffmpegmp3decoder.h"
#include <android/log.h>
#include <zconf.h>
#include "include/libavfilter/avfilter.h"
#include <libavdevice/avdevice.h>
}


int FFmpegMp3Decoder::Init(const char *fileString, int packetBufferSizeParam) {
    packetBufferSize = packetBufferSizeParam;
    int ret2 = Init(fileString);
    LOGI("return log ............");
    return ret2;
}

int FFmpegMp3Decoder::Init(const char *srcFilepath) {
    LOGI("enter FFmpegMp3Decoder::init,srcFilepath=%s", srcFilepath);
    audioBuffer = NULL;
    position = -1.0f;
    audioBufferCursor = 0;
    audioBufferSize = 0;
    swrContext = NULL;
    swsContext = NULL;
    swrBuffer = NULL;
    audioFrame = NULL;
    videoFrame = NULL;
    videoCodexCtx = NULL;
    pictureValid = false;

    swrBufferSize = 0;
    isNeedFirstFrameCorrectFlag = true;
    firstFrameCorrectionInSecs = 0.0f;

    //1.注册协议，格式与编码器
    av_register_all();
    avcodec_register_all();
    avdevice_register_all();
    avformat_network_init();
    avfilter_register_all();

    formatContext = avformat_alloc_context();

    //打开视频源文件
    int open_ret;
    if ((open_ret = avformat_open_input(&formatContext, srcFilepath, NULL, NULL)) != 0) {
        char buf[] = "";
        av_strerror(open_ret, buf, 1024);//获取错误信息
        LOGE("open input file failed!!!,path=%s,strerror=(%s)", srcFilepath, buf);
        free(buf);
        return open_ret;
    }
    LOGI("open input file success!");
//    while(true){
//        LOGI("waiting.........");
//    }
    //取出流信息
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        LOGE("can not find strem info!!!");
        return -1;
    }

    //3.寻找各个流信息，并打开对应的解码器

    ////遍历所有类型的流（音频流、视频流、字幕流）,寻找音视频流
    LOGI("stream number=%d", formatContext->nb_streams);
    videoStreamIndex = -1;
    audioStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        AVStream *stream = formatContext->streams[i];
        if (AVMEDIA_TYPE_VIDEO == stream->codec->codec_type) {
            //视频流
            videoStreamIndex = i;
        } else if (AVMEDIA_TYPE_AUDIO == stream->codec->codec_type) {
            //音频流
            audioStreamIndex = i;
        }

    }
    if (videoStreamIndex == -1) {
        LOGE("找不到视频流！");
    }
    if (audioStreamIndex == -1) {
        LOGE("找不到音频流！");
    }

    if (videoStreamIndex == -1 && audioStreamIndex == -1) {
        LOGE("No found aduio or video stream, return!");
        return -1;
    }

    if (audioStreamIndex != -1) {
        //打开音频流解码器
        //音频流编解码上下文
        //音频流
        AVStream *audioStream = formatContext->streams[audioStreamIndex];
        audioCodecCtx = formatContext->streams[audioStreamIndex]->codec;
        if (audioStream->time_base.den && audioStream->time_base.num)
            timeBase = av_q2d(audioStream->time_base);
        else if (audioStream->codec->time_base.den && audioStream->codec->time_base.num)
            timeBase = av_q2d(audioStream->codec->time_base);

        LOGI("set time base done");
        //根据编码id查找解码器
        AVCodec *acodec = avcodec_find_decoder(audioCodecCtx->codec_id);

        if (!acodec) {
            LOGE("找不到音频解码器!");
            return -1;
        }
        LOGI("find audio decoder success!");
        //打开音频解码器
        int openACodecErrCode = 0;
        if ((openACodecErrCode = avcodec_open2(audioCodecCtx, acodec, NULL)) < 0) {
            LOGE("打开音频解码器失败!, openACodecErrCode=%d", openACodecErrCode);
            return -1;
        }
        LOGI("open audio decoder success!");
    }

    if (videoStreamIndex != -1) {
        //打开视频解码器
        videoCodexCtx = formatContext->streams[videoStreamIndex]->codec;
        //根据编码id查找解码器
        AVCodec *vcodec = avcodec_find_decoder(videoCodexCtx->codec_id);
        if (!vcodec) {
            LOGE("找不到视频频解码器!");
            return -1;
        }
        //打开解码器
        int openVCodecErrCode = 0;
        if ((openVCodecErrCode = avcodec_open2(videoCodexCtx, vcodec, NULL)) < 0) {
            LOGE("打开视频解码器失败!openVCodecErrCode=%d", openVCodecErrCode);
            return -1;
        }
    }

    //4.初始化解码后数据的结构体
    //构建音频的格式转换对象以及音频解码后数据存放的对象
    if (audioCodecCtx->sample_fmt != AV_SAMPLE_FMT_S16) {
        //如果不是我们需要的格式,需要resampler
        swrContext = swr_alloc_set_opts(NULL,
                                        av_get_default_channel_layout(OUT_PUT_CHANNELS),
                                        AV_SAMPLE_FMT_S16,
                                        audioCodecCtx->sample_rate,
                                        av_get_default_channel_layout(audioCodecCtx->channels),
                                        audioCodecCtx->sample_fmt,
                                        audioCodecCtx->sample_rate,
                                        0,
                                        NULL);
        if (!swrContext || swr_init(swrContext)) {
            if (swrContext) {
                swr_free(&swrContext);
            }
        }
        audioFrame = av_frame_alloc();
    }
    LOGI("init output success!");

    if (videoStreamIndex != -1) {
        //构建视频的格式转换对象以及视频 解码后的数据存放的对象
        bool pictureValid = avpicture_alloc(&picture,
                                            AV_PIX_FMT_YUV420P,
                                            videoCodexCtx->width,
                                            videoCodexCtx->height) == 0;
        LOGI("pictureValid=%d", pictureValid);
        if (!pictureValid) {
            LOGE("picture alloc failed!");
            return -1;
        }
        swsContext = sws_getCachedContext(swsContext,
                                          videoCodexCtx->width,
                                          videoCodexCtx->height,
                                          videoCodexCtx->pix_fmt,
                                          videoCodexCtx->width,
                                          videoCodexCtx->height,
                                          AV_PIX_FMT_YUV420P,
                                          SWS_FAST_BILINEAR,
                                          NULL, NULL, NULL);
        videoFrame = av_frame_alloc();
    }
    return 0;
}

int FFmpegMp3Decoder::Decode() {
    while (true) {
        AudioPacket *accompanyPacket = decodePacket();
        if (-1 == accompanyPacket->size) {
            break;
        }
        LOGI("write AudioPacket to file");
        fwrite(accompanyPacket->buffer, sizeof(short), accompanyPacket->size, outFile);
    }
}

AudioPacket *FFmpegMp3Decoder::decodePacket() {
    LOGI("FFmpegMp3Decoder::decodePacket packetBufferSize is %d", packetBufferSize);
    short *samples = new short[packetBufferSize];
//	LOGI("accompanyPacket buffer's addr is %x", samples);
    //读取采样数据，保存到samples
    int stereoSampleSize = readSamples(samples, packetBufferSize);

    //将读出来的采样数据封装为AudioPacket
    AudioPacket *samplePacket = new AudioPacket();
    if (stereoSampleSize > 0) {
        //构造成一个packet
        samplePacket->buffer = samples;
        samplePacket->size = stereoSampleSize;
        /** 这里由于每一个packet的大小不一样有可能是200ms 但是这样子position就有可能不准确了 **/
        samplePacket->position = position;
    } else {
        samplePacket->size = -1;
    }
    return samplePacket;
}

/** 读取采样数据
 * @param samples 数据存储的缓冲区
 * @param size 要读取的字节数
 * @return
 */
int FFmpegMp3Decoder::readSamples(short *samples, int size) {
    LOGI("readSamples,size=%d", size);
    int sampleSize = size;
    while (size > 0) {
        if (audioBufferCursor < audioBufferSize) {
            //剩余可读取的大小
            int audioBufferDataSize = audioBufferSize - audioBufferCursor;
            //取要读取的大小和剩余大小 取较小的值为本次读取的大小
//            int copySize = MIN(size, audioBufferDataSize);
            int copySize = size < audioBufferDataSize ? size : audioBufferDataSize;
            LOGI("copySize=%d", copySize);
            //从audioBuffer拷贝到samples，字节数为copysize*2
            memcpy(samples + (sampleSize - size), audioBuffer + audioBufferCursor, copySize * 2);
            size -= copySize;
            //更新已拷贝数据下标
            audioBufferCursor += copySize;
        } else {
            if (readFrame() < 0) {
                break;
            }
        }
//		LOGI("size is %d", size);
    }
    int fillSize = sampleSize - size;
    if (fillSize == 0) {
        return -1;
    }
    return fillSize;
}


int FFmpegMp3Decoder::readFrame() {
    LOGI("readFrame")
    //读取流内容并且解码
    int gotFrame = 0;
    while (true) {
        //从文件中读取一个AVPacket
        if (av_read_frame(formatContext, &avPacket)) {
            LOGI("End of file.");
            break;
        }

        int packetStreamIndex = avPacket.stream_index;
        if (packetStreamIndex == videoStreamIndex) {
            //视频流
            int len = avcodec_decode_video2(videoCodexCtx, videoFrame, &gotFrame, &avPacket);
            if (len < 0) {
                break;
            }
            if (gotFrame) {
                //处理解码后的裸数据
//                handleVideoFrame(channels);
                break;
            }
        } else if (packetStreamIndex == audioStreamIndex) {
            //音频流
            int len = avcodec_decode_audio4(audioCodecCtx, audioFrame, &gotFrame, &avPacket);
            if (len < 0) {
                LOGI("decode audio error, skip packet");
                continue;
            }
            if (gotFrame) {
                //处理解码后的裸数据
                if (handleAudioFrame() < 0) {
                    return -1;
                };
                break;
            }
        }
    }
    return 0;
}

int FFmpegMp3Decoder::handleAudioFrame() {
    LOGI("handleAudioFrame")
    void *audioData;
    int numFrames;
    int numChannels = OUT_PUT_CHANNELS;
    const int ratio = 2;
    if (swrContext) {
        int bufSize = av_samples_get_buffer_size(NULL, numChannels,
                                                 (int) (audioFrame->nb_samples * ratio),
                                                 AV_SAMPLE_FMT_S16, 1);
        if (!swrBuffer || swrBufferSize < bufSize) {
            swrBufferSize = bufSize;
            swrBuffer = realloc(swrBuffer, swrBufferSize);
        }
        Byte *outbuf[2] = {(Byte *) swrBuffer, NULL};
        numFrames = swr_convert(swrContext, outbuf,
                                audioFrame->nb_samples * ratio,
                                (const uint8_t **) (audioFrame->data),
                                audioFrame->nb_samples);

        if (numFrames < 0) {
            LOGI("fail resample audio");
            return -1;
        }
        audioData = swrBuffer;
    } else {
        if (audioCodecCtx->sample_fmt != AV_SAMPLE_FMT_S16) {
            LOGI("bucheck, audio format is invalid");
            return -1;
        }
        audioData = audioFrame->data[0];
        numFrames = audioFrame->nb_samples;
    }
    if (isNeedFirstFrameCorrectFlag && position >= 0) {
        float expectedPosition = position + duration;
        float actualPosition = av_frame_get_best_effort_timestamp(audioFrame) * timeBase;
        firstFrameCorrectionInSecs = actualPosition - expectedPosition;
        isNeedFirstFrameCorrectFlag = false;
    }
    duration = av_frame_get_pkt_duration(audioFrame) * timeBase;
    position =
            av_frame_get_best_effort_timestamp(audioFrame) * timeBase - firstFrameCorrectionInSecs;
    audioBufferSize = numFrames * numChannels;
//					LOGI(" \n duration is %.6f position is %.6f audioBufferSize is %d\n", duration, position, audioBufferSize);
    audioBuffer = (short *) audioData;
    audioBufferCursor = 0;
    return 0;
}

void FFmpegMp3Decoder::handleVideoFrame(int channels) {

}

int FFmpegMp3Decoder::getMusicMeta(const char *fileString, int *metaData) {
    LOGI("getMusicMeta");
    int init_ret = Init(fileString);
    if (init_ret < 0) {
        LOGE("getMusicMeta init failed!!!");
        return init_ret;
    }
    int sampleRate = audioCodecCtx->sample_rate;
    LOGI("sampleRate is %d", sampleRate);
    int bitRate = audioCodecCtx->bit_rate;
    LOGI("bitRate is %d", bitRate);
    Destroy();
    metaData[0] = sampleRate;
    metaData[1] = bitRate;
    return 0;
}

void FFmpegMp3Decoder::Destroy() {
    //关闭音频资源
    if (NULL != swrBuffer) {
        free(swrBuffer);
        swrBuffer = NULL;
        swrBufferSize = 0;
    }
    if (NULL != swrContext) {
        swr_free(&swrContext);
        swrContext = NULL;
    }
    if (NULL != audioFrame) {
        av_free(audioFrame);
        audioFrame = NULL;
    }
    if (NULL != audioCodecCtx) {
        avcodec_close(audioCodecCtx);
        audioCodecCtx = NULL;
    }

    //关闭视频资源
    if (swsContext != NULL) {
        sws_freeContext(swsContext);
        swsContext = NULL;
    }
    if (pictureValid) {
        avpicture_free(&picture);
        pictureValid = false;
    }
    if (videoFrame != NULL) {
        av_free(videoFrame);
        videoFrame = NULL;
    }
    if (videoCodexCtx != NULL) {
        avcodec_close(videoCodexCtx);
        videoCodexCtx = NULL;
    }

    //关闭连接资源
    if (NULL != formatContext) {
        LOGI("leave LiveReceiver::destory");
        avformat_close_input(&formatContext);
        formatContext = NULL;
    }
}

FFmpegMp3Decoder::FFmpegMp3Decoder() {

}

FFmpegMp3Decoder::~FFmpegMp3Decoder() {

}