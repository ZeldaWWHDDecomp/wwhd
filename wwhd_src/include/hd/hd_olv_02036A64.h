/** HD JPEG encoder and GHS delegate views; private decomp source. */
#pragma once
#include "wwhd.h"

struct hdJpegDelegateVirtualEntry {
    be<u32> mAdjustment;
    gfn mFunction;
};
WWHD_SIZE(hdJpegDelegateVirtualEntry, 8);
struct hdJpegDelegate {
    be<u32> mVtable;
    gptr<void> mObject;
    be<s16> mThisDelta;
    be<s16> mVirtualIndex;
    union {
        gfn mFunction;
        struct {
            be<s16> mFunctionHigh;
            be<s16> mVtableOffset;
        } mVirtual;
    };
    u32 invoke(u32 argument, u32 userData) {
        void* object = mObject;
        if (object == NULL) {
            return gabi::ea(this);
        }
        s32 index = mVirtualIndex;
        if (index == 0) {
            return gabi::ea(this);
        }
        // HD: GHS this and vtable adjustments are signed offsets with 32-bit guest wrapping.
        u32 targetAddress = gabi::ea(object) + (u32)(s32)mThisDelta;
        void* target = gabi::at<void>(targetAddress);
        u32 function;
        if (index < 0) {
            function = mFunction.get();
        } else {
            u32 vtableAddress = targetAddress + (u32)(s32)mVirtual.mVtableOffset;
            auto* vtablePointer = gabi::at<gptr<hdJpegDelegateVirtualEntry>>(vtableAddress);
            hdJpegDelegateVirtualEntry* entries = *vtablePointer;
            function = entries[index].mFunction.get();
        }
        return gabi::call_ptr<u32>(function, target, argument, userData);
    }
};
WWHD_OFFSET(hdJpegDelegate, mObject, 4);
WWHD_OFFSET(hdJpegDelegate, mThisDelta, 8);
WWHD_OFFSET(hdJpegDelegate, mVirtualIndex, 0xA);
WWHD_OFFSET(hdJpegDelegate, mFunction, 0xC);
WWHD_SIZE(hdJpegDelegate, 0x10);

struct hdJpegHeapVtable {
    u8 _padding00[0x24];
    gfn mDestroy;
    u8 _padding28[0x54];
    gfn mGetMaxAllocatableSize;
};
WWHD_OFFSET(hdJpegHeapVtable, mGetMaxAllocatableSize, 0x7C);
struct hdJpegHeap {
    u8 _padding00[0xC];
    gptr<hdJpegHeapVtable> mVtable;
    u32 getMaxAllocatableSize(u32 alignment) {
        return gabi::call_ptr<u32>(mVtable->mGetMaxAllocatableSize.get(), this, alignment);
    }
    void destroy() { gabi::call_ptr<u32>(mVtable->mDestroy.get(), this); }
};
WWHD_OFFSET(hdJpegHeap, mVtable, 0xC);
struct hdJpegHeapName {
    gptr<const char> mText;
    be<u32> mVtable;
};
WWHD_SIZE(hdJpegHeapName, 8);
struct hdJpegEncoder {
    gptr<hdJpegHeap> mHeap;
    gptr<void> mWorkBuffer;
    be<u32> mWorkSize;
    be<u32> mQuality;
    be<u32> mWidth;
    be<u32> mHeight;
    be<u32> mFormat;
    be<u32> mLastEncodedSize;
};
WWHD_OFFSET(hdJpegEncoder, mQuality, 0xC);
WWHD_OFFSET(hdJpegEncoder, mLastEncodedSize, 0x1C);
WWHD_SIZE(hdJpegEncoder, 0x20);
struct hdJpegImageInfo {
    be<u32> mWidth;
    be<u32> mHeight;
    be<u32> mFormat;
};
WWHD_SIZE(hdJpegImageInfo, 12);
struct hdJpegEncodeFrame {
    be<u32> mOutputSize;
    hdJpegImageInfo mInfo;
    be<u32> mWorkSize;
};
WWHD_OFFSET(hdJpegEncodeFrame, mWorkSize, 0x10);
WWHD_SIZE(hdJpegEncodeFrame, 0x14);
hdJpegHeap* hdJpegCreateHeap(u32 size, hdJpegHeapName* name, void* parent, u32 direction, u32 locking);
void* hdJpegAllocateWorkBuffer(u32 size, hdJpegHeap* heap, u32 alignment);
void hdJpegDeleteWorkBuffer(void* buffer);
u32 hdJpegGetWorkSize(hdJpegImageInfo* info, be<u32>* workSize);
u32 hdJpegSystemEncode(void* work, u32 workSize, void* destination, hdJpegImageInfo* info,
                       u32 quality, const void* source, u32 pitch, be<u32>* outputSize);
