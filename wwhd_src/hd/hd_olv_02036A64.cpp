/** HD JPEG encoding and quality retries. No GameCube counterpart.
 * Private decomp source. */
#include "bindings.h"
#include "hd/hd_olv_02036A64.h"

static f32 sQualityHalf;
enum : u32 {
    DEFAULT_QUALITY = 100,
    DEFAULT_WIDTH = 800,
    DEFAULT_HEIGHT = 450,
    MAX_SCREENSHOT_BYTES = 0x50000,
    HEAP_NAME_VTABLE = 0x10005260,
};

/* 02036A64 */
static u32 invokeJpegDelegate(hdJpegDelegate* i_this, u32 argument, u32 userData) {
    return i_this->invoke(argument, userData);
}

/* 02036AB8 */
static hdJpegEncoder* constructJpegEncoder(hdJpegEncoder* i_this) {
    if (i_this == NULL) {
        i_this = (hdJpegEncoder*)operator_new(sizeof(hdJpegEncoder));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mHeap = NULL;
    i_this->mHeight = DEFAULT_HEIGHT;
    i_this->mQuality = DEFAULT_QUALITY;
    i_this->mWidth = DEFAULT_WIDTH;
    i_this->mWorkSize = 0;
    i_this->mLastEncodedSize = 0;
    i_this->mFormat = 0;
    i_this->mWorkBuffer = NULL;
    return i_this;
}

/* 02036B1C */
static void destroyJpegEncoder(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02036B30 */
static void initializeJpegEncoder(hdJpegEncoder* i_this, void* parent, u32 size) {
    hdJpegHeapName name;
    name.mVtable = HEAP_NAME_VTABLE;
    name.mText = STR(0x10005278);
    hdJpegHeap* heap = hdJpegCreateHeap(size, &name, parent, 1, 0);
    i_this->mHeap = heap;
    if (heap == NULL) {
        return;
    }
    u32 workSize = heap->getMaxAllocatableSize(4);
    // HD: reload the heap after the virtual call before allocating the work buffer.
    hdJpegHeap* currentHeap = i_this->mHeap;
    i_this->mWorkSize = workSize;
    i_this->mWorkBuffer = hdJpegAllocateWorkBuffer(workSize, currentHeap, 4);
}

/* 02036BBC */
static void finalizeJpegEncoder(hdJpegEncoder* i_this) {
    void* buffer = i_this->mWorkBuffer;
    if (buffer != NULL) {
        hdJpegDeleteWorkBuffer(buffer);
        i_this->mWorkBuffer = NULL;
    }
    hdJpegHeap* heap = i_this->mHeap;
    i_this->mWorkSize = 0;
    if (heap != NULL) {
        heap->destroy();
        i_this->mHeap = NULL;
    }
}

/* 02036C28 */
static void setJpegSize(hdJpegEncoder* i_this, u32 width, u32 height, u32 format) {
    i_this->mHeight = height;
    i_this->mFormat = format;
    i_this->mWidth = width;
}

/* 02036C38 */
static void setJpegQuality(hdJpegEncoder* i_this, u32 quality) {
    i_this->mQuality = quality;
}

/* 02036C40 */
static u32 encodeJpeg(hdJpegEncoder* i_this, const void* source, u32 pitch, void* destination) {
    hdJpegEncodeFrame frame;
    frame.mWorkSize = 0;
    frame.mInfo.mHeight = i_this->mHeight;
    frame.mInfo.mWidth = i_this->mWidth;
    frame.mInfo.mFormat = i_this->mFormat;
    if (hdJpegGetWorkSize(&frame.mInfo, &frame.mWorkSize) != 0) {
        return FALSE;
    }
    if (i_this->mWorkSize < frame.mWorkSize) {
        return FALSE;
    }
    u32 quality = i_this->mQuality;
    frame.mOutputSize = 0;
    if (hdJpegSystemEncode(i_this->mWorkBuffer, i_this->mWorkSize, destination, &frame.mInfo,
                           quality, source, pitch, &frame.mOutputSize) != 0) {
        return FALSE;
    }
    f32 half = sQualityHalf;
    for (;;) {
        u32 size = frame.mOutputSize;
        i_this->mLastEncodedSize = size;
        if (size <= MAX_SCREENSHOT_BYTES) {
            return TRUE;
        }
        // HD: preserve signed conversion, float precision and PPC integer conversion when halving.
        quality = (u32)gabi::ftoi((f32)(f64)(s32)quality * half);
        if (hdJpegSystemEncode(i_this->mWorkBuffer, i_this->mWorkSize, destination, &frame.mInfo,
                               quality, source, pitch, &frame.mOutputSize) != 0) {
            return FALSE;
        }
    }
}

/* 02036DAC */
static void __sinit_hd_jpeg_encoder_cpp() {
    sinit_header_statics_rodata(0x102009C4, 0x1018F3FC, 0x102009D0, 0x10005290);
}

/* 02036E40 */
static void destroyJpegEncoderCompanion(void* i_this, u32 flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 02036E54 */
static void jpegEncoderCompanionEmpty() {}

/* WWHD glue (tools/verify/autoglue.py): private encoder constant and calls
 data sQualityHalf 10005284
 ext 02753004 hdJpegHeap* hdJpegCreateHeap(u32, hdJpegHeapName*, void*, u32, u32)
 ext 0273B0D4 void* hdJpegAllocateWorkBuffer(u32, hdJpegHeap*, u32)
 ext 0273AFC8 void hdJpegDeleteWorkBuffer(void*)
 ext 0284DEF8 u32 hdJpegGetWorkSize(hdJpegImageInfo*, be<u32>*)
 ext 0284BBB4 u32 hdJpegSystemEncode(void*, u32, void*, hdJpegImageInfo*, u32, const void*, u32, be<u32>*)
*/
