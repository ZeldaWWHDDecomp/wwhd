/* HD render target dimensions; no GameCube equivalent. Private decomp source. */
#pragma once
#include "bindings.h"
struct hdRenderSize_c {
    /* 0x00 */ be<f32> mWidth;
    /* 0x04 */ be<f32> mHeight;
};
WWHD_OFFSET(hdRenderSize_c, mHeight, 4);
WWHD_SIZE(hdRenderSize_c, 8);

struct hdRenderResolutionHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdRenderResolutionHeaderZero_c,16);
struct hdRenderResolutionHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdRenderResolutionHeaderDescriptor_c,12);
