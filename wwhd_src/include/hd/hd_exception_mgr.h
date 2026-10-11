/* HD exception console, delegates and dump manager views. Private reconstruction. */
#pragma once
#include "wwhd.h"
struct HdExcType {
    gptr<void> mParent;
};
struct HdExcDelegate {
    gptr<gfn> mVTable;
    gptr<void> mObject;
    be<s16> mThisAdjustment;
    be<s16> mIndex;
    union {
        gptr<gfn> mFunction;
        struct {
            be<u16> mReserved;
            be<s16> mVTableOffset;
        };
    };
};
struct HdExcVTablePointer {
    gptr<gfn> mVTable;
};
struct HdExcController {
    u8 mPad[0x10];
    gptr<gfn> mVTable;
};
struct HdExcOffsetNode {
    gptr<void> mPrevious;
    gptr<struct HdExcOffsetNode> mNext;
    be<u32> mReserved;
    be<u32> mOffset;
};
struct HdExcControllerOwner {
    u8 mPad[0xCC];
    HdExcOffsetNode mHead;
};
struct HdExcRefObject {
    u8 mPad[8];
    be<u32> mReferenceCount;
};
struct HdExcHeap {
    u8 mPad[0xC];
    gptr<gfn> mVTable;
    be<u32> mHandle;
    be<u32> mReserved14;
    gptr<struct HdExcHeap> mSelf;
    gptr<HdExcRefObject> mRefObject;
    be<u32> mReserved20;
    u8 mPad24[0x14];
    be<u8> mActive;
    u8 mPad39[3];
};
struct HdExcMutex {
    u8 mOpaque[0x3C];
};
struct HdExcEntry {
    gptr<struct HdExcEntry> mNext;
    gptr<gfn> mVTable;
};
struct HdExcManager {
    gptr<HdExcEntry> mHead;
    gptr<gfn> mDsiCallback;
    gptr<gfn> mIsiCallback;
    gptr<gfn> mProgramCallback;
    HdExcHeap mHeap;
    HdExcMutex mMutex;
};
struct HdExcCallbacks {
    u8 mPad0[4];
    gptr<gfn> mDsiCallback;
    gptr<gfn> mIsiCallback;
    gptr<gfn> mProgramCallback;
    u8 mRegistration[0x20];
    gptr<HdExcDelegate> mDelegate;
    HdExcDelegate mDumpDelegate;
};
struct HdExcVaList {
    union {
        be<u32> mIndices;
        struct {
            be<u8> mGprIndex;
            be<u8> mFprIndex;
            be<u16> mReserved;
        };
    };
    gptr<void> mOverflow;
    gptr<void> mRegisterSave;
};
struct HdExcVaFrame {
    be<u32> mBackchain;
    be<u32> mReserved4;
    HdExcVaList mList;
    be<u32> mReserved14;
    be<u32> mGprs[8];
    be<f64> mFprs[8];
};
struct HdExcPanicFrame {
    HdExcVaFrame mArgs;
    HdExcVaList mCopy;
    be<u32> mSavedR29;
    be<u32> mSavedR30;
    be<u32> mSavedR31;
};
struct HdExcStatics {
    be<u32> mWords[4];
};
struct HdExcRange {
    be<f32> mLower;
    be<f32> mUpper;
    be<u8> mSigned;
    be<u8> mUnsigned;
    u8 mPad[2];
};
struct HdExcRegistration {
    be<u32> mWords[3];
};
enum HdExcKind { HD_EXC_DSI = 2, HD_EXC_ISI = 3, HD_EXC_PROGRAM = 6 };
WWHD_SIZE(HdExcManager, 0x88);
WWHD_SIZE(HdExcHeap, 0x3C);
WWHD_SIZE(HdExcDelegate, 0x10);
WWHD_SIZE(HdExcVaList, 12);
WWHD_SIZE(HdExcVaFrame, 0x78);
WWHD_SIZE(HdExcPanicFrame, 0x90);
WWHD_OFFSET(HdExcManager, mMutex, 0x4C);
WWHD_OFFSET(HdExcVaFrame, mGprs, 0x18);
WWHD_OFFSET(HdExcVaFrame, mFprs, 0x38);
WWHD_OFFSET(HdExcPanicFrame, mCopy, 0x78);
WWHD_OFFSET(HdExcCallbacks, mDumpDelegate, 0x34);
