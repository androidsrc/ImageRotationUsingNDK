package com.aman.ndktesting;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.widget.ImageView;

import com.jni.bitmap_operations.JniBitmapHolder;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    private static String TAG="MainActivity";

    JniBitmapHolder bitmapHolder = new JniBitmapHolder();
    ImageView imageView;
    Bitmap bitmap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        imageView = (ImageView) findViewById(R.id.imageView);

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig= Bitmap.Config.RGB_565;
        bitmap = BitmapFactory.decodeFile("/sdcard/guru.jpg", options);
        bitmapHolder.storeBitmap(bitmap);
        bitmapHolder.rotateBitmapCw90();
        Bitmap bitmap1=bitmapHolder.getBitmapAndFree(bitmap);

        imageView.setImageBitmap(bitmap1);
        saveFile(bitmap1, 80);

    }


    public static String saveFile(Bitmap bmp, int quality) {
        FileOutputStream out = null;

        File dir= new File("/sdcard/sample");
        if(!dir.exists()){
            try{
                dir.mkdir();
            }catch (Exception e){
                e.printStackTrace();
            }
            Log.d(TAG,"Folder cointribe created!!");
        }

        String filename = String.format("/sdcard/cointribe/%d_%d.jpg", quality, System.currentTimeMillis());
        try {
            out = new FileOutputStream(filename);
            bmp.compress(Bitmap.CompressFormat.JPEG, quality, out); // bmp is your Bitmap instance
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return filename;
    }

}
