#pragma once
#include "d/hd_message_parts.h"
struct CopyString_l { gptr<char> mpText; gptr<gfn> mpVTable; be<s32> mCapacity; be<u8> mBytes[256]; };
WWHD_OFFSET(CopyString_l,mBytes,0xC);
WWHD_SIZE(CopyString_l,0x10C);
struct CopyResource_l { u8 mReserved[0xC]; be<u32> mFlags; u8 mReserved10[0x46]; be<u16> mCount; u8 mReserved58[0xC]; be<u32> mArrayOffset; u8 mReserved68[0x18]; be<u32> mNameOffset; };
WWHD_OFFSET(CopyResource_l,mNameOffset,0x80);
struct CopyOwner_l;
struct CopyContext_l { u8 mReserved[0x18]; gptr<CopyOwner_l> mpOwner; };
struct CopyOwner_l { u8 mReserved[0xC]; gptr<CopyResource_l> mpResource; u8 mReserved10[0x20]; gptr<gfn> mpTypeVTable; };
union CopyDimension_l { be<u32> mBits; be<f32> mValue; };
WWHD_SIZE(CopyDimension_l,4);
struct CopyTextPane_l {
 u8 mReserved[8]; gptr<gfn> mpVTable; u8 mReservedC[0x74]; be<u8> mName[0x2C]; be<u32> mColor;
 u8 mReservedB0[8]; be<u32> mFontWidth; be<u32> mFontHeight; u8 mReservedC0[0x44];
 CopyDimension_l mOriginalWidth; CopyDimension_l mOriginalHeight; be<f32> mWidthScale; be<f32> mHeightScale;
 gptr<void> mpFont; gptr<void> mpText; be<u32> mSavedColor; be<u8> mScalable; u8 mReserved121[3];
 gptr<CopyOwner_l> mpOwner; CopyString_l mCopyName; gptr<void> mpArray; be<u16> mArrayCount; u8 mReserved23A[2];
 u8 mMessage[0x924]; be<f32> mScaleWidth; be<f32> mScaleHeight;
 static CopyTextPane_l* construct(CopyTextPane_l*,void*,u32,u32);
 static CopyTextPane_l* copyConstruct(CopyTextPane_l*,CopyTextPane_l*);
 void setCopyName(CopyResource_l*);
 void initialize(CopyResource_l*,CopyResource_l*,CopyContext_l*);
 void copyFromResource();
 void applyScale();
};
WWHD_OFFSET(CopyTextPane_l,mOriginalWidth,0x104);
WWHD_OFFSET(CopyTextPane_l,mpOwner,0x124);
WWHD_OFFSET(CopyTextPane_l,mCopyName,0x128);
WWHD_OFFSET(CopyTextPane_l,mpArray,0x234);
WWHD_OFFSET(CopyTextPane_l,mMessage,0x23C);
WWHD_OFFSET(CopyTextPane_l,mScaleWidth,0xB60);
WWHD_SIZE(CopyTextPane_l,0xB68);
struct CopyAttribute_l { be<u32> mKey; be<u32> mValueOffset; };
struct CopyFont_l { u8 mReserved[4]; gptr<gfn> mpVTable; };
struct CopyPaneTypeView_l { u8 mReserved[8]; gptr<gfn> mpTypeVTable; };
