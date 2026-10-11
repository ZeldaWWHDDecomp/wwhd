/** HD UI visitor, animation entry and layout views; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdUiVisitorTable { u8 _opaque00[0x1C]; gfn mVisit; };
struct hdUiVisitor {
    be<u32> field_0x0; // HD: constructor clears this bookkeeping word; role unconfirmed.
    gptr<hdUiVisitorTable> mVtable;
    u32 visit(void* object) { return gabi::call_ptr<u32>(mVtable->mVisit.get(), this, object); }
};
struct hdUiVisitorList { be<u32> mCount; u8 _opaque04[4]; gptr<gptr<void>> mElements; };
struct hdUiStringTable { u8 _opaque00[0x14]; gfn mAssureTermination; };
struct hdUiSafeString {
    gptr<be<u8>> mText; gptr<hdUiStringTable> mVtable;
    void assureTermination() { gabi::call_ptr(mVtable->mAssureTermination.get(), this); }
};
struct hdUiFixedString {
    gptr<be<u8>> mText; gptr<hdUiStringTable> mVtable; be<s32> mCapacity; be<u8> mBuffer[32];
    void terminate() {
        // HD: preserve the wrapped address even for zero/negative capacity.
        *gabi::at<be<u8>>(gabi::ea(mText.get()) + (u32)(s32)mCapacity - 1) = 0;
    }
};
WWHD_SIZE(hdUiFixedString, 0x2C);
struct hdUiAnimationResource { u8 _opaque00[0xA]; be<u8> mLoopMode; };
struct hdUiFrameTable {
    u8 _opaque00[0xA4]; gfn mPlay; u8 _opaqueA8[0xC]; gfn mPlayAlternate;
};
struct hdUiFrameController {
    u8 _opaque00[8]; gptr<hdUiAnimationResource> mResource;
    u8 _opaque0C[8]; gptr<hdUiFrameTable> mVtable;
    u8 _opaque18[0xC]; be<f32> mFrame; u8 _opaque28[4]; be<u32> mFlags;
    u32 play() {
        u32 target = mVtable->mPlay.get();
        u32 loop = mResource->mLoopMode == 1 ? 1 : 0;
        return gabi::call_ptr<u32>(target, this, loop);
    }
    u32 playAlternate() {
        u32 target = mVtable->mPlayAlternate.get();
        u32 loop = mResource->mLoopMode == 1 ? 1 : 0;
        return gabi::call_ptr<u32>(target, this, loop);
    }
};
struct hdUiAnimationEntry {
    gptr<void> mOwner; be<f32> mRate; hdUiFixedString mName;
    gptr<hdUiFrameController> mController; be<u32> mVtable;
    hdUiFixedString* nameForConstruction() {
        // HD: this == -8 makes the embedded constructor address wrap to NULL.
        return gabi::at<hdUiFixedString>(gabi::ea(this) + offsetof(hdUiAnimationEntry, mName));
    }
};
WWHD_SIZE(hdUiAnimationEntry, 0x3C);
WWHD_OFFSET(hdUiAnimationEntry, mController, 0x34);
struct hdUiPartsPane { u8 _opaque00[8]; be<u32> mVtable; u8 _opaque0C[0xA4]; };
WWHD_SIZE(hdUiPartsPane, 0xB0);
struct hdUiWindowPane { u8 _opaque00[8]; be<u32> mVtable; u8 _opaque0C[0xCC]; };
WWHD_SIZE(hdUiWindowPane, 0xD8);
struct hdUiAdaptorTargetTable { u8 _opaque00[0x34]; gfn mForwardFirst; u8 _opaque38[4]; gfn mForwardSecond; };
struct hdUiAdaptorTarget { u8 _opaque00[0xC]; gptr<hdUiAdaptorTargetTable> mVtable; };
struct hdUiAdaptor { be<u32> mVtable; gptr<hdUiAdaptorTarget> mTarget; };
struct hdUiAdaptorType { gptr<hdUiAdaptorType> mParent; };
void hdUiEntryCopyBytes(void*, const void*, u32, u32);
void hdUiEntryConstructPartsBase(hdUiPartsPane*, u32, u32, u32);
void hdUiEntryDestroyPartsBase(hdUiPartsPane*, u32);
void hdUiEntryConstructWindowBase(hdUiWindowPane*, u32, u32, u32);
void hdUiEntryDestroyWindowBase(hdUiWindowPane*, u32);
inline be<u8>* hdUiEntryAdvanceByte(be<u8>* text, u32 count) {
    // HD: string pointer arithmetic wraps in the 32-bit guest address space.
    return gabi::at<be<u8>>(gabi::ea(text) + count);
}
inline gptr<void>* hdUiEntryAdvanceElement(gptr<void>* element) {
    // HD: visitor iteration retains the original wrapped element address.
    return gabi::at<gptr<void>>(gabi::ea(element) + sizeof(gptr<void>));
}
