/** HD ProfileTask and its emitted companions; private decomp source. */
#pragma once
#include "wwhd.h"
#include "hd/hd_sys_02035444.h"

// HD: fixed safe-string storage exposes a buffer and its character capacity.
struct hdProfileSafeBuffer {
    gptr<void> mBuffer;
    be<u32> mVtable;
    be<u32> mCapacity;
    void terminateByte() {
        // Preserve 32-bit wrapping, including zero-capacity buffers.
        u32 end = gabi::ea(mBuffer.get()) + (u32)mCapacity - 1;
        *gabi::at<be<u8>>(end) = 0;
    }
    void terminateWide() {
        u32 end = gabi::ea(mBuffer.get()) + (u32)mCapacity * 2 - 2;
        *gabi::at<be<u16>>(end) = 0;
    }
};
WWHD_OFFSET(hdProfileSafeBuffer, mCapacity, 8);

struct hdProfileDelegateVirtualEntry {
    be<u32> mAdjustment;
    gfn mFunction;
};
WWHD_SIZE(hdProfileDelegateVirtualEntry, 8);
struct hdProfileDelegate {
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
            auto* vtablePointer = gabi::at<gptr<hdProfileDelegateVirtualEntry>>(vtableAddress);
            hdProfileDelegateVirtualEntry* entries = *vtablePointer;
            function = entries[index].mFunction.get();
        }
        return gabi::call_ptr<u32>(function, target, argument, userData);
    }
};
WWHD_OFFSET(hdProfileDelegate, mObject, 4);
WWHD_OFFSET(hdProfileDelegate, mThisDelta, 8);
WWHD_OFFSET(hdProfileDelegate, mVirtualIndex, 0xA);
WWHD_OFFSET(hdProfileDelegate, mFunction, 0xC);
WWHD_SIZE(hdProfileDelegate, 0x10);


struct hdProfileInstanceGuard {
    be<u8> mOwnsInstance;
};
struct hdProfileTask {
    u8 _padding00[0x20];
    be<u32> mTaskVtable;
    u8 _padding24[0x4C];
    be<u32> field_0x70; // HD: a second vtable is installed here; base type is unconfirmed.
    u8 _padding74[0x54];
    hdProfileInstanceGuard mInstanceGuard;
    u8 _paddingC9[3];
};
WWHD_OFFSET(hdProfileTask, mTaskVtable, 0x20);
WWHD_OFFSET(hdProfileTask, field_0x70, 0x70);
WWHD_OFFSET(hdProfileTask, mInstanceGuard, 0xC8);
WWHD_SIZE(hdProfileTask, 0xCC);
struct hdProfileDownloadedDataArray {
    u8 mStorage[50 * 0xC208];
};
using hdProfileElementDestructor = void (*)(void*, u32);
void hdProfileDestroyArray(void*, u32, u32, hdProfileElementDestructor, u32, u32);
void hdProfileDestroyDownloadedData(void*, u32);
void hdProfileConstructTaskBase(hdProfileTask*, hdTaskConstructArg*, const char*);
void hdProfileDestroyTaskBase(hdProfileTask*, u32);
void hdProfileCalcTaskBase(hdProfileTask*);
