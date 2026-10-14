/* HD boot/ending scene support views. Private binary reconstruction. */
#pragma once
#include "d/hd_layout_mgr.h"
#include "d/hd_input_ctrl.h"
#include "m_Do/m_Do_lib.h"

struct hdSceneStateHandle_c {
    gptr<hdStateId> mpDescriptor;
    hdStateId* descriptor();
};
struct hdSceneDrawInfo_c {
    u8 mPrefix[0xC];
    be<u32> mViewIndex;
};
struct hdSceneDrawDelegate_c {
    gptr<void> mpVtable;
    gptr<void> mpOwner;
    be<s16> mThisAdjustment;
    be<s16> mVirtualIndex;
    gptr<void> mpMethod;
};
struct hdSceneScreen_c {
    u8 mBase[0xC];
    gptr<void> mpVtable;
    hdScreenString mDisplayName;
    be<u32> mEnabled;
    be<u32> mFlags;
    hdSceneDrawDelegate_c mDrawDelegate;
    u8 mReserved[0x10];
};
struct hdSceneDisposer_c {
    u8 mBase[0xC];
    gptr<hdScreenDeleteTable> mpVtable;
};
struct hdSceneHeaderWords_c { be<u32> mWords[4]; };
struct hdSceneHeaderRange_c {
    be<f32> mLower, mUpper;
    be<u8> mSigned, mUnsigned;
    u8 mPadding[2];
};
struct hdSceneRegistration_c { u8 mStorage[0xC]; };
struct hdSceneTypeName_c { gptr<void> mpName; gptr<void> mpVtable; };
WWHD_SIZE(hdSceneStateHandle_c, 4);
WWHD_OFFSET(hdSceneStateHandle_c, mpDescriptor, 0);
WWHD_OFFSET(hdSceneDrawInfo_c, mViewIndex, 0xC);
WWHD_SIZE(hdSceneDrawInfo_c, 0x10);
WWHD_OFFSET(hdSceneDrawDelegate_c, mpOwner, 4);
WWHD_OFFSET(hdSceneDrawDelegate_c, mpMethod, 0xC);
WWHD_SIZE(hdSceneDrawDelegate_c, 0x10);
WWHD_OFFSET(hdSceneScreen_c, mpVtable, 0xC);
WWHD_OFFSET(hdSceneScreen_c, mDisplayName, 0x10);
WWHD_OFFSET(hdSceneScreen_c, mEnabled, 0x18);
WWHD_OFFSET(hdSceneScreen_c, mFlags, 0x1C);
WWHD_OFFSET(hdSceneScreen_c, mDrawDelegate, 0x20);
WWHD_SIZE(hdSceneScreen_c, 0x40);
WWHD_SIZE(hdSceneDisposer_c, 0x10);
WWHD_OFFSET(hdSceneDisposer_c, mpVtable, 0xC);
WWHD_SIZE(hdSceneHeaderWords_c, 0x10);
WWHD_SIZE(hdSceneHeaderRange_c, 0xC);
WWHD_SIZE(hdSceneRegistration_c, 0xC);
WWHD_SIZE(hdSceneTypeName_c, 8);

void hdBootSupportStaticInit();
void* hdSceneAllocate(u32 size);
void* hdSceneHeapAllocate(u32 size, void* heap, u32 alignment);
void hdSceneDelete(void* object);
void hdSceneScreenConstruct(hdSceneScreen_c*);
void hdSceneScreenDestroy(hdSceneScreen_c*, u32 flags);
void hdSceneDisposerConstruct(hdSceneDisposer_c*, void* heap, u32 flags);
void hdSceneDisposerDestroy(hdSceneDisposer_c*, u32 flags);
void hdSceneAttachScreen(mDoLibCamera*, u32 mode, hdSceneScreen_c*);
void hdSceneDrawSetup(u32, u32, u32, u32, u32, u32, u32);
void hdSceneDrawPrepare(void* renderer);
void hdSceneRegisterGlobal(hdSceneRegistration_c*);
void hdSceneConstructSigned(be<u8>*);
void hdSceneConstructUnsigned(be<u8>*);
