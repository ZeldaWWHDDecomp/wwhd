/** HD render resource and shader names. Library resource-file dictionaries remain opaque. */
#pragma once
#include "hd/hd_render_resources.h"
struct hdFixedShaderName_l {
    gptr<be<u8>> mpText;
    be<u32> mVtable;
    be<u32> mCapacity;
    be<u8> mBuffer[8];
};
WWHD_SIZE(hdFixedShaderName_l, 0x14);
struct hdFixedWideShaderName_l {
    gptr<be<u16>> mpText;
    be<u32> mVtable;
    be<u32> mCapacity;
    be<u16> mBuffer[9];
    u8 mPadding[2];
};
WWHD_SIZE(hdFixedWideShaderName_l, 0x20);
struct hdRenderShaderNames_c {
    u8 mReserved[12];
    hdFixedShaderName_l mGroupName;
    u8 mPadding[8];
    hdFixedWideShaderName_l mProgramName;
};
WWHD_OFFSET(hdRenderShaderNames_c, mGroupName, 0x0C);
WWHD_OFFSET(hdRenderShaderNames_c, mProgramName, 0x28);
WWHD_SIZE(hdRenderShaderNames_c, 0x48);
