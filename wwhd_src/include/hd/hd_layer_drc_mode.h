/* HD layer that changes UI rendering for DRC mode. Private decomp source. */
#pragma once
#include "bindings.h"
struct hdDrcLayer_c {
    u8 _00[0xC];
    be<u32> mVtable;
    be<u32> mDrawVtable;
    u8 _14[0x880-0x14];
    void initialize();
};
WWHD_OFFSET(hdDrcLayer_c, mVtable, 0xC);
WWHD_OFFSET(hdDrcLayer_c, mDrawVtable, 0x10);
WWHD_SIZE(hdDrcLayer_c, 0x880);
struct hdDrcRenderStep_c {
    u8 _00[0xC];
    be<u32> mVtable;
    SafeString mName;
    be<u32> mTarget;
    be<u32> mFlags;
    be<u32> mDelegateVtable;
    gptr<hdDrcLayer_c> mpLayer;
    be<u32> mDelegateAdjustment;
    be<u32> mDrawFunction;
    u8 _30[0x10];
};
WWHD_OFFSET(hdDrcRenderStep_c, mName, 0x10);
WWHD_OFFSET(hdDrcRenderStep_c, mTarget, 0x18);
WWHD_OFFSET(hdDrcRenderStep_c, mFlags, 0x1C);
WWHD_OFFSET(hdDrcRenderStep_c, mDelegateVtable, 0x20);
WWHD_OFFSET(hdDrcRenderStep_c, mpLayer, 0x24);
WWHD_OFFSET(hdDrcRenderStep_c, mDelegateAdjustment, 0x28);
WWHD_OFFSET(hdDrcRenderStep_c, mDrawFunction, 0x2C);
WWHD_SIZE(hdDrcRenderStep_c, 0x40);

struct hdLayerDrcModeHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdLayerDrcModeHeaderZero_c,16);
struct hdLayerDrcModeHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdLayerDrcModeHeaderDescriptor_c,12);
