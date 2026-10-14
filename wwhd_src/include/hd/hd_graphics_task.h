/** HD graphics task and its library-owned render buffers. */
#pragma once
#include "SSystem/SComponent/c_m2d_g_box.h"
#include "hd/hd_layer_2d.h"
#include "hd/hd_render_resources.h"
struct hdGraphicsTask_c {
    u8 mBaseStart[0x20];
    be<u32> mInterfaceVtable;
    u8 mBeforeVtable[0x4C];
    be<u32> mVtable;
    u8 mBaseTail[0xFC];
    be<u32> mCallbackVtable;
    be<u8> mRegistered;
    u8 mPadding[3];
    be<u32> mFlags;
    be<u32> mDisplayMode;
    cXy mScreenSize;
    cXy mRenderSize;
    u8 mRenderBuffers[2][0xA70];
    be<u8> mAdaptive;
    be<u8> mHistoryTiming;
    u8 mPadding2[2];
    be<f32> mHighThreshold;
    be<f32> mLowThreshold;
    cXy mRequestedSize;
    be<u32> mMode;
    be<u8> mPresent;
    be<u8> mScaled;
    be<u8> mFlag;
    u8 mPadding3;
    be<f32> mValues[4];
    be<u32> mCooldown;
    be<u32> mCooldownReset;
    SafeString mName;
    void registerTask();
    void configureBuffers();
    void updateResolution();
    void beginFrame();
    void destroy(u32 flags);
    void installRenderCallback();
    void updateScreenSize();
    void enableScene(u32 enabled);
    void enableDRC(u32 enabled);
    void finish();
    void renderCallback(hdLayerRenderInfo_c* info);
};
WWHD_OFFSET(hdGraphicsTask_c, mRegistered, 0x174);
WWHD_OFFSET(hdGraphicsTask_c, mFlags, 0x178);
WWHD_OFFSET(hdGraphicsTask_c, mRenderBuffers, 0x190);
WWHD_OFFSET(hdGraphicsTask_c, mAdaptive, 0x1670);
WWHD_OFFSET(hdGraphicsTask_c, mName, 0x16A4);
WWHD_SIZE(hdGraphicsTask_c, 0x16AC);
struct hdGraphicsDisplayOwner_l {
    u8 mReserved[0x360];
    gptr<void> mpDisplays[2];
    u8 mBeforeCallback[8];
    be<u32> mRenderCallback;
};
struct hdGraphicsMode_l {
    u8 mReserved[0x1D0];
    be<u32> mDisplayMode;
};

struct hdGraphicsDisposer_l {
    u8 mReserved[12];
    be<u32> mVtable;
};
struct hdGraphicsService_c {
    be<u32> mVtable;
    u8 mLibraryState[0x6C];
    hdGraphicsDisposer_l mDisposer;
    static hdGraphicsService_c* create(void* heap);
};
WWHD_SIZE(hdGraphicsService_c, 0x80);

struct hdGraphicsRendererFlags_l {
    u8 mReserved[0x1070];
    be<u32> mFormat;
    u8 mBeforeFlags[4];
    be<u16> mFlags;
};
struct hdGraphicsTypeVtable_l {
    u8 mReserved[12];
    be<u32> mIsKindOf;
};
struct hdGraphicsDisplayType_l {
    u8 mReserved[8];
    gptr<hdGraphicsTypeVtable_l> mpVtable;
};

struct hdGraphicsTaskBase_l {
    u8 mStart[0x20];
    be<u32> mInterfaceVtable;
    u8 mBefore70[0x4C];
    be<u32> mVtable;
    struct Listener {
        u8 mLibraryState[0x1C];
        hdGraphicsDisposer_l mDisposer;
        u8 mState[0x24];
        be<u32> mVtable;
    } mListeners[3];
    be<u32> mCallbackVtable;
};
WWHD_SIZE(hdGraphicsTaskBase_l, 0x174);
struct hdGraphicsReferenceBlock_l {
    u8 mReserved[8];
    be<u32> mReferences;
};
struct hdGraphicsReference_c {
    be<u32> mVtable;
    hdGraphicsDisposer_l mDisposer;
    u8 mAllocator[12];
    gptr<hdGraphicsReferenceBlock_l> mpBlock;
    u8 mReserved[0x18];
    be<u8> mOwnsReference;
    void destroy(u32 flags);
};
WWHD_OFFSET(hdGraphicsReference_c, mpBlock, 0x20);
WWHD_OFFSET(hdGraphicsReference_c, mOwnsReference, 0x3C);
