#include <jni.h>
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <cstring>
#include <unistd.h>

#define  LOG_TAG    "Applog"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C"
{
//store
JNIEXPORT jobject JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniStoreBitmapData(
        JNIEnv *env, jobject obj, jobject bitmap);
//get
JNIEXPORT jobject JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniGetBitmapFromStoredBitmapData(
        JNIEnv *env, jobject obj, jobject handle);
//free
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniFreeBitmapData(
        JNIEnv *env, jobject obj, jobject handle);
//rotate 90 degrees CCW
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniRotateBitmapCcw90(
        JNIEnv *env, jobject obj, jobject handle);
//rotate 90 degrees CW
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniRotateBitmapCw90(
        JNIEnv *env, jobject obj, jobject handle);
//rotate 180 degrees
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniRotateBitmap180(
        JNIEnv *env, jobject obj, jobject handle);

class JniBitmap {
public:
    uint16_t *_storedBitmapPixels;
    AndroidBitmapInfo _bitmapInfo;

    JniBitmap() {
        _storedBitmapPixels = NULL;
    }
};

/**rotates the inner bitmap data by 90 degrees counter clock wise*/ //
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniRotateBitmapCcw90(
        JNIEnv *env, jobject obj, jobject handle) {
    JniBitmap *jniBitmap = (JniBitmap *) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return;
    uint16_t *previousData = jniBitmap->_storedBitmapPixels;
    uint32_t newWidth = jniBitmap->_bitmapInfo.height;
    uint32_t newHeight = jniBitmap->_bitmapInfo.width;
    jniBitmap->_bitmapInfo.width = newWidth;
    jniBitmap->_bitmapInfo.height = newHeight;
    uint16_t *newBitmapPixels = new uint16_t[newWidth * newHeight];
    int whereToGet = 0;
    // XY. ... ... ..X
    // ...>Y..>...>..Y
    // ... X.. .YX ...
    for (int x = 0; x < newWidth; ++x)
        for (int y = newHeight - 1; y >= 0; --y) {
            //take from each row (up to bottom), from left to right
            uint32_t pixel = previousData[whereToGet++];
            newBitmapPixels[newWidth * y + x] = pixel;
        }
    delete[] previousData;
    jniBitmap->_storedBitmapPixels = newBitmapPixels;
}

/**rotates the inner bitmap data by 90 degrees clock wise*/ //
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniRotateBitmapCw90(
        JNIEnv *env, jobject obj, jobject handle) {
    JniBitmap *jniBitmap = (JniBitmap *) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return;
    uint16_t *previousData = jniBitmap->_storedBitmapPixels;
    uint32_t newWidth = jniBitmap->_bitmapInfo.height;
    uint32_t newHeight = jniBitmap->_bitmapInfo.width;
    jniBitmap->_bitmapInfo.width = newWidth;
    jniBitmap->_bitmapInfo.height = newHeight;
    uint16_t *newBitmapPixels = new uint16_t[newWidth * newHeight];
    int whereToGet = 0;
    // XY. ..X ... ...
    // ...>..Y>...>Y..
    // ... ... .YX X..
    jniBitmap->_storedBitmapPixels = newBitmapPixels;
    for (int x = newWidth - 1; x >= 0; --x)
        for (int y = 0; y < newHeight; ++y) {
            //take from each row (up to bottom), from left to right
            uint32_t pixel = previousData[whereToGet++];
            newBitmapPixels[newWidth * y + x] = pixel;
        }
    delete[] previousData;
}

/**rotates the inner bitmap data by 180 degrees (*/ //
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniRotateBitmap180(
        JNIEnv *env, jobject obj, jobject handle) {
    JniBitmap *jniBitmap = (JniBitmap *) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return;
    uint16_t *pixels = jniBitmap->_storedBitmapPixels;
    uint16_t *pixels2 = jniBitmap->_storedBitmapPixels;
    uint32_t width = jniBitmap->_bitmapInfo.width;
    uint32_t height = jniBitmap->_bitmapInfo.height;
    //no need to create a totally new bitmap - it's the exact same size as the original
    // 1234 fedc
    // 5678>ba09
    // 90ab>8765
    // cdef 4321
    int whereToGet = 0;
    for (int y = height - 1; y >= height / 2; --y)
        for (int x = width - 1; x >= 0; --x) {
            //take from each row (up to bottom), from left to right
            uint32_t tempPixel = pixels2[width * y + x];
            pixels2[width * y + x] = pixels[whereToGet];
            pixels[whereToGet] = tempPixel;
            ++whereToGet;
        }
    //if the height isn't even, flip the middle row :
    if (height % 2 == 1) {
        int y = height / 2;
        whereToGet = width * y;
        int lastXToHandle = width % 2 == 0 ? (width / 2) : (width / 2) - 1;
        for (int x = width - 1; x >= lastXToHandle; --x) {
            uint32_t tempPixel = pixels2[width * y + x];
            pixels2[width * y + x] = pixels[whereToGet];
            pixels[whereToGet] = tempPixel;
            ++whereToGet;
        }
    }
}

/**free bitmap*/ //
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniFreeBitmapData(
        JNIEnv *env, jobject obj, jobject handle) {
    JniBitmap *jniBitmap = (JniBitmap *) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return;
    delete[] jniBitmap->_storedBitmapPixels;
    jniBitmap->_storedBitmapPixels = NULL;
    delete jniBitmap;
}

/**restore java bitmap (from JNI data)*/ //
JNIEXPORT jobject JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniGetBitmapFromStoredBitmapData(
        JNIEnv *env, jobject obj, jobject handle) {
    JniBitmap *jniBitmap = (JniBitmap *) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL) {
        LOGD("no bitmap data was stored. returning null...");
        return NULL;
    }
    //
    //creating a new bitmap to put the pixels into it - using Bitmap Bitmap.createBitmap (int width, int height, Bitmap.Config config) :
    //
    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls,
                                                            "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("RGB_565");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(
            bitmapConfigClass, "valueOf",
            "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       valueOfBitmapConfigFunction, configName);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls,
                                                    createBitmapFunction,
                                                    jniBitmap->_bitmapInfo.width,
                                                    jniBitmap->_bitmapInfo.height, bitmapConfig);
    //
    // putting the pixels into the new bitmap:
    //
    int ret;
    void *bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return NULL;
    }
    uint16_t *newBitmapPixels = (uint16_t *) bitmapPixels;
    int pixelsCount = jniBitmap->_bitmapInfo.height
                      * jniBitmap->_bitmapInfo.width;
    memcpy(newBitmapPixels, jniBitmap->_storedBitmapPixels,
           sizeof(uint16_t) * pixelsCount);
    AndroidBitmap_unlockPixels(env, newBitmap);
    //LOGD("returning the new bitmap");
    return newBitmap;
}

/**store java bitmap as JNI data*/ //
JNIEXPORT jobject JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniStoreBitmapData(
        JNIEnv *env, jobject obj, jobject bitmap) {
    AndroidBitmapInfo bitmapInfo;
    uint16_t *storedBitmapPixels = NULL;
    //LOGD("reading bitmap info...");
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }
    //LOGD("width:%d height:%d stride:%d", bitmapInfo.width, bitmapInfo.height, bitmapInfo.stride);
    if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGBA_8888!");
        return NULL;
    }
    //
    //read pixels of bitmap into native memory :
    //
    //LOGD("reading bitmap pixels...");
    void *bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return NULL;
    }
    uint16_t *src = (uint16_t *) bitmapPixels;
    storedBitmapPixels = new uint16_t[bitmapInfo.height * bitmapInfo.width];
    int pixelsCount = bitmapInfo.height * bitmapInfo.width;
    memcpy(storedBitmapPixels, src, sizeof(uint16_t) * pixelsCount);
    AndroidBitmap_unlockPixels(env, bitmap);
    JniBitmap *jniBitmap = new JniBitmap();
    jniBitmap->_bitmapInfo = bitmapInfo;
    jniBitmap->_storedBitmapPixels = storedBitmapPixels;
    return env->NewDirectByteBuffer(jniBitmap, 0);
}
}

