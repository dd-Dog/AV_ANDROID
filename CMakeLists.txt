
#指定使用的cmake最低版本号
cmake_minimum_required(VERSION 3.4.1)

#指定工程名，非必须
#project(mp3encoder)
project(mp3decoder)

#指定头文件路径
include_directories(src/main/cpp/include/)

set(SRC_FILES
        src/main/cpp/Mp3Decoder.cpp
        src/main/cpp/ffmpegmp3decoder.cpp
        src/main/cpp/mp3decoder_controller.cpp

        #######################libmp3lame测试源码################################
        #        src/main/cpp/mp3_encoder.cpp
        #        src/main/cpp/libmp3lame/bitstream.c
        #        src/main/cpp/libmp3lame/encoder.c
        #        src/main/cpp/libmp3lame/fft.c
        #        src/main/cpp/libmp3lame/gain_analysis.c
        #        src/main/cpp/libmp3lame/id3tag.c
        #        src/main/cpp/libmp3lame/lame.c
        #        src/main/cpp/libmp3lame/mpglib_interface.c
        #        src/main/cpp/libmp3lame/newmdct.c
        #        src/main/cpp/libmp3lame/presets.c
        #        src/main/cpp/libmp3lame/psymodel.c
        #        src/main/cpp/libmp3lame/quantize_pvt.c
        #        src/main/cpp/libmp3lame/quantize.c
        #        src/main/cpp/libmp3lame/reservoir.c
        #        src/main/cpp/libmp3lame/set_get.c
        #        src/main/cpp/libmp3lame/tables.c
        #        src/main/cpp/libmp3lame/takehiro.c
        #        src/main/cpp/libmp3lame/util.c
        #        src/main/cpp/libmp3lame/vbrquantize.c
        #        src/main/cpp/libmp3lame/VbrTag.c
        #        src/main/cpp/libmp3lame/version.c
        )
#编译为共享库
add_library(
        audiodecoder

        # Sets the library as a shared library.
        SHARED

        #源文件路径
        ${SRC_FILES})

#add_library(avformat STATIC IMPORTED)
#add_library(avcodec STATIC IMPORTED)
#add_library(avdevice STATIC IMPORTED)
#add_library(avfilter STATIC IMPORTED)
#add_library(avutil STATIC IMPORTED)
#add_library(swresample STATIC IMPORTED)
#add_library(swscale STATIC IMPORTED)
#add_library(fdk-aac STATIC IMPORTED)
#add_library(x264 STATIC IMPORTED)
#add_library(mp3lame STATIC IMPORTED)

add_library(avformat-57 SHARED IMPORTED)
add_library(avcodec-57 SHARED IMPORTED)
add_library(avdevice-57 SHARED IMPORTED)
add_library(avfilter-6 SHARED IMPORTED)
add_library(avutil-55 SHARED IMPORTED)
add_library(swresample-2 SHARED IMPORTED)
add_library(swscale-4 SHARED IMPORTED)

set(STATIC_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/armeabi-v7a/)

set_target_properties(avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavcodec-57.so)
set_target_properties(avdevice-57
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavdevice-57.so)
set_target_properties(avfilter-6
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavfilter-6.so)
set_target_properties(avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavformat-57.so)
set_target_properties(avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavutil-55.so)
set_target_properties(swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libswresample-2.so)
set_target_properties(swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libswscale-4.so)
#set_target_properties(fdk-aac
#        PROPERTIES IMPORTED_LOCATION
#        ${STATIC_LIBS_DIR}/libfdk-aac.a)
#set_target_properties(x264
#        PROPERTIES IMPORTED_LOCATION
#        ${STATIC_LIBS_DIR}/libx264.a)
#set_target_properties(mp3lame
#        PROPERTIES IMPORTED_LOCATION
#        ${STATIC_LIBS_DIR}/libmp3lame.a)

target_link_libraries(audiodecoder z) #链接系统libz库
target_link_libraries(audiodecoder swresample-2)
target_link_libraries(audiodecoder avdevice-57)
target_link_libraries(audiodecoder avformat-57)
target_link_libraries(audiodecoder avfilter-6)
target_link_libraries(audiodecoder avcodec-57)
target_link_libraries(audiodecoder avutil-55)
target_link_libraries(audiodecoder swscale-4)
#target_link_libraries(audiodecoder fdk-aac)
#target_link_libraries(audiodecoder x264)
#target_link_libraries(audiodecoder mp3lame)

#在默认路径下查找log库，并保存在变量log-lib中
find_library(log-lib log)

# 链接库
target_link_libraries( # Specifies the target library.
        audiodecoder
        # 将Log-lib变量代表的库，链接到audioencoder库
        ${log-lib})
