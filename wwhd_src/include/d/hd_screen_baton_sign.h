/* HD BatonSign_00 screen. Private binary reconstruction. */
#pragma once
#include "d/hd_parts_in_out_loop.h"
struct hdBatonSign_c : hdPartsInOutLoop_c {
    be<s32> mSignFrame;
    be<s32> mShownSignFrame;
    be<u8> mSignRequested;
    u8 mPadding[3];
    static void destroy(hdBatonSign_c* self, s32 flags);
    static hdBatonSign_c* construct(hdBatonSign_c* self);
    void waitExecute();
    void inEnter();
    void inExecute();
    void displayExecute();
    void outEnter();
    void outExecute();
    u32 initialize(u32 first, u32 second, hdPartsDesc_c* descriptor);
    void finish();
    u32 createLayout(void* resource, u32 first, u32 second, u32 third, void* descriptor);
};
WWHD_OFFSET(hdBatonSign_c, mSignFrame, 0x5C);
WWHD_OFFSET(hdBatonSign_c, mShownSignFrame, 0x60);
WWHD_OFFSET(hdBatonSign_c, mSignRequested, 0x64);
WWHD_SIZE(hdBatonSign_c, 0x68);
