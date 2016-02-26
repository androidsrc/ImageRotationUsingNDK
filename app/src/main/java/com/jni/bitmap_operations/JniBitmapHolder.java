package com.jni.bitmap_operations;

import java.nio.ByteBuffer;

import android.graphics.Bitmap;
import android.util.Log;

public class JniBitmapHolder {
    static String TAG="JniBitmapHolder";
    ByteBuffer _handler = null;

    static {

        try {
            System.loadLibrary("JniBitmapOperationsLibrary");
        } catch (UnsatisfiedLinkError ule) {
            Log.e(TAG, "WARNING: Could not load native library: " + ule.getMessage());
        }

    }

    public enum ScaleMethod {
        NearestNeighbour, BilinearInterpolation
    }

    private native ByteBuffer jniStoreBitmapData(Bitmap bitmap);

    private native Bitmap jniGetBitmapFromStoredBitmapData(ByteBuffer handler);

    private native void jniFreeBitmapData(ByteBuffer handler);

    private native void jniRotateBitmapCcw90(ByteBuffer handler);

    private native void jniRotateBitmapCw90(ByteBuffer handler);

    private native void jniRotateBitmap180(ByteBuffer handler);

    public JniBitmapHolder() {
    }

    public JniBitmapHolder(final Bitmap bitmap) {
        storeBitmap(bitmap);
    }

    public void storeBitmap(final Bitmap bitmap) {
        if (_handler != null)
            freeBitmap();
        _handler = jniStoreBitmapData(bitmap);
    }

    public void rotateBitmapCcw90() {
        if (_handler == null)
            return;
        jniRotateBitmapCcw90(_handler);
    }

    public void rotateBitmapCw90() {
        if (_handler == null)
            return;
        jniRotateBitmapCw90(_handler);
    }

    public void rotateBitmap180() {
        if (_handler == null)
            return;
        jniRotateBitmap180(_handler);
    }

    public Bitmap getBitmap() {
        if (_handler == null)
            return null;
        return jniGetBitmapFromStoredBitmapData(_handler);
    }

    public Bitmap getBitmapAndFree(Bitmap bitmap) {
        bitmap.recycle();
        bitmap = getBitmap();
        freeBitmap();
        return bitmap;
    }

    public void freeBitmap() {
        if (_handler == null)
            return;
        jniFreeBitmapData(_handler);
        _handler = null;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        if (_handler == null)
            return;
        Log.w(TAG, "JNI bitmap wasn't freed nicely.please remember to free the bitmap as soon as you can");
        freeBitmap();
    }
}
