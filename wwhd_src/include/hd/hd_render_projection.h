/* HD perspective projection with a two-axis render scale. Private decomp source. */
#pragma once
#include "bindings.h"
struct hdProjectionScale_c {
    union { be<f32> mX; be<u32> mXBits; };
    union { be<f32> mY; be<u32> mYBits; };
};
WWHD_OFFSET(hdProjectionScale_c, mX, 0);
WWHD_OFFSET(hdProjectionScale_c, mY, 4);
WWHD_SIZE(hdProjectionScale_c, 8);
struct hdRenderProjection_c {
    /* 0x00 */ be<u8> mDirty;
    /* 0x01 */ u8 _01[0x90-1];
    /* 0x90 */ be<u32> mVtable;
    /* 0x94 */ u8 _94[0xB8-0x94];
    /* 0xB8 */ hdProjectionScale_c mScale;
    /* 0xC0 */ be<f32> mFieldOfView;
    void setScale(const hdProjectionScale_c* scale);
    void makeMatrix(struct hdProjectionMatrix_c* matrix);
};
WWHD_OFFSET(hdRenderProjection_c, mDirty, 0);
WWHD_OFFSET(hdRenderProjection_c, mVtable, 0x90);
WWHD_OFFSET(hdRenderProjection_c, mScale, 0xB8);
WWHD_OFFSET(hdRenderProjection_c, mFieldOfView, 0xC0);
WWHD_SIZE(hdRenderProjection_c, 0xC4);
struct hdProjectionMatrix_c { be<f32> mElements[16]; };
WWHD_OFFSET(hdProjectionMatrix_c, mElements, 0);
WWHD_SIZE(hdProjectionMatrix_c, 0x40);
struct hdProjectionParameters_l {
    u8 _00[0x14];
    be<f32> mAspect;
};
WWHD_OFFSET(hdProjectionParameters_l, mAspect, 0x14);

struct hdRenderProjectionHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdRenderProjectionHeaderZero_c,16);
struct hdRenderProjectionHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdRenderProjectionHeaderDescriptor_c,12);
