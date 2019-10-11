package com.flyscale.mp3encoder;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    static {
        System.loadLibrary("audioencoder");
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
        switch (v.getId()){
            case R.id.convert:
                new Thread(){
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
                        }else {
                            Log.e(getClass().getName(), "pcm file does not exits!");
                        }
                    }
                }.start();

                break;
        }
    }
}
