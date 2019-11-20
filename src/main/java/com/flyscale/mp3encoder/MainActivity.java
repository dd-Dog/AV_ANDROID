package com.flyscale.mp3encoder;

import android.os.Environment;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    static {
        System.loadLibrary("audiodecoder");
    }

    private Mp3Encoder mp3Encoder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mp3Encoder = new Mp3Encoder();
        findViewById(R.id.convert).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                testFFmpegDecoder();
            }
        }).start();
    }

    /**
     * 测试FFmpeg解码器
     */
    /**
     * 原始的文件路径
     **/
    private static String mp3FilePath = "/mnt/sdcard/bianjb_test/131.mp3";
    /**
     * 解码后的PCM文件路径
     **/
    private static String pcmFilePath = "/mnt/sdcard/bianjb_test/131.pcm";

    private void testFFmpegDecoder() {
        long startTimeMills = System.currentTimeMillis();
        File srcFile = new File(mp3FilePath);
        if (!srcFile.exists()) {
            Log.e(getClass().getName(), "file nost exist!!!");
            return;
        }
        Mp3Decoder decoder = new Mp3Decoder();
        int ret = decoder.init(mp3FilePath, pcmFilePath);
        Log.d(getClass().getName(), "ret=" + ret);
        if (ret >= 0) {
            decoder.decode();
            decoder.destroy();
        } else {
            Log.i(getClass().getName(), "Decoder Initialized Failed...");
        }
        int wasteTimeMills = (int) (System.currentTimeMillis() - startTimeMills);
        Log.i(getClass().getName(), "Decode Mp3 Waste TimeMills : " + wasteTimeMills + "ms");
    }


    /**
     * 测试mp3lame编码器
     */
    private void testEncoderMp3() {
        new Thread() {
            @Override
            public void run() {
                super.run();
                String path = Environment.getExternalStorageDirectory().getPath();
                Log.d(getClass().getName(), "external path=" + path);
                File pcmFile = new File(path + "/test/beijing.pcm");
                if (pcmFile.exists()) {
                    int init = mp3Encoder.init(pcmFile.getPath(), 1, 16,
                            8000, "/sdcard/test/beijing-out.mp3");
                    if (init == -1) {
                        Log.e(getClass().getName(), "init failed!!");
                        return;
                    }
                    mp3Encoder.encode();
                    mp3Encoder.destroy();
                } else {
                    Log.e(getClass().getName(), "pcm file does not exits!");
                }
            }
        }.start();
    }
}
