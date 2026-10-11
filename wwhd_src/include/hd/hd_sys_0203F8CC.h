/** HD SystemTask startup views; private decomp source. */
#pragma once
#include "hd/hd_sys_02035444.h"
struct hdSystemStartupTask;
struct hdSystemTypeTable {
    u8 _opaque00[0xC]; gfn mCheckType;
    u32 check(void* object, hdTaskRuntimeTypeInfo* type) const { return gabi::call_ptr<u32>(mCheckType.get(), object, type); }
};
struct hdSystemCreatedTask { u8 _opaque00[0x70]; gptr<hdSystemTypeTable> mVtable; };
struct hdSystemViewportTable {
    u8 _opaque00[0x3C]; gfn mSetViewport;
    u32 setViewport(void* object) const { return gabi::call_ptr<u32>(mSetViewport.get(), object, 2); }
};
struct hdSystemViewportObject { u8 _opaque00[0x24]; gptr<hdSystemViewportTable> mVtable; };
struct hdSystemStartupTask {
    u8 _opaque00[0x20]; be<u32> mTaskVtable;
    u8 _opaque24[0x38]; gptr<void> mTaskManager;
    u8 _opaque60[0x10]; be<u32> mRuntimeVtable;
    u8 _opaque74[0x54]; be<u8> mOwnsInstance; u8 _paddingC9[3];
};
WWHD_SIZE(hdSystemStartupTask, 0xCC);
WWHD_OFFSET(hdSystemStartupTask, mTaskManager, 0x5C);
WWHD_OFFSET(hdSystemStartupTask, mOwnsInstance, 0xC8);
struct hdSystemDestroyableA { u8 _opaque00[0x1C]; be<u32> mVtable; };
struct hdSystemDestroyableB { u8 _opaque00[0x20]; be<u32> mVtable; };
struct hdSystemInstanceGuard { be<u8> mOwnsInstance; };
struct hdSystemFixedString {
    gptr<be<u8>> mBuffer; be<u32> mVtable; be<u32> mCapacity;
    void terminate() {
        // HD: capacity arithmetic wraps, including the zero-capacity case.
        *gabi::at<be<u8>>(gabi::ea(mBuffer.get()) + (u32)mCapacity - 1) = 0;
    }
};
struct hdSystemHeapName { be<u32> mTextAddress; be<u32> mVtable; };
struct hdSystemRootHeapHolder { u8 _opaque00[0x10]; gptr<void> mHeap; };
struct hdSystemChildTaskId { be<u32> mKind; be<u32> mFactory; };
struct hdSystemChildHeapOption { be<u32> mSize; u8 _opaque04[0xC]; be<u32> mTailWord; };
WWHD_SIZE(hdSystemChildHeapOption, 0x14);
struct hdSystemChildTaskArgument {
    u8 _opaque00[0xC]; hdSystemChildHeapOption mHeapOptions[5];
    u8 _opaque70[0xC]; be<u32> mCreateCallback;
    void copyFrom(const hdSystemChildTaskArgument& source) {
        // HD: the original copies 32 guest words, including opaque argument bytes.
        be<u32>* destinationWords = (be<u32>*)this;
        const be<u32>* sourceWords = (const be<u32>*)&source;
        for (u32 i = 0; i < 32; ++i) destinationWords[i] = sourceWords[i];
    }
    u32 currentHeapIndex() const {
        // HD: the creation index overlays the fourth heap option at +0x58.
        return mHeapOptions[3].mTailWord;
    }
};
WWHD_SIZE(hdSystemChildTaskArgument, 0x80);
struct hdSystemViewport { be<u32> mWords[6]; };
void hdSystemDestroyFirstBase(hdSystemDestroyableA*, u32);
void hdSystemDestroySecondBase(hdSystemDestroyableB*, u32);
void* hdSystemAllocate(u32, void*, s32);
void* hdSystemConstructSmallObject(void*);
void* hdSystemConstructDelegateTask(void*, hdTaskConstructArg*);
void* hdSystemConstructLargeTask(void*, hdTaskConstructArg*);
void* hdSystemConstructProfileTask(void*, hdTaskConstructArg*);
void hdSystemDestroyTask(hdSystemStartupTask*, u32);
void hdSystemConstructTaskBase(hdSystemStartupTask*, hdTaskConstructArg*, u32);
void hdSystemInitializeFontHeap(void*);
void hdSystemInitializeFontManager(void*);
void hdSystemInitializeMessageHeap(void*);
void hdSystemInitializeMessageManager(void*);
void* hdSystemGetRootHeap();
void* hdSystemCreateExpandedHeap(u32, hdSystemHeapName*, void*, u32, u32);
void hdSystemInitializeMessageGroupHeap(void*);
void hdSystemAssignMessageGroupHeap(void*, void*);
void hdSystemInitializeBackgroundHeap(void*);
void hdSystemAssignBackgroundHeap(void*, void*);
void hdSystemInitializeMain();
void hdSystemPrepareBase(hdSystemStartupTask*);
void hdSystemConstructChildArgument(hdSystemChildTaskArgument*, hdSystemChildTaskId*);
hdSystemCreatedTask* hdSystemCreateChildTask(void*, hdSystemChildTaskArgument*);
void hdSystemConstructViewport(hdSystemViewport*);
void hdSystemSetViewportRectangle(hdSystemViewport*, f32, f32, f32, f32);
hdSystemViewportObject* hdSystemGetViewportObject(hdSystemStartupTask*);
