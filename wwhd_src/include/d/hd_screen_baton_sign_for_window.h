/* HD message-window baton sign. Private binary reconstruction. */
#pragma once
#include "d/hd_parts.h"
struct hdBatonSignForWindow_c : hdParts_c {
    be<u32> mSignFrame;
    static void destroy(hdBatonSignForWindow_c* self, s32 flags);
    static hdBatonSignForWindow_c* construct(hdBatonSignForWindow_c* self);
    void startAnimation(u32 animation);
    void inEnter();
    u32 animationDone(u32 animation);
    void inExecute();
    void outEnter();
    void outExecute();
    void setAnimationFrame(u32 animation, u32 frame);
    u32 initialize(u32 first, u32 second, void* descriptor);
    void finalize();
    void executeState();
    void setSignFrame(u32 frame);
    void resetSignFrame();
    void requestIn();
    void requestOut();
    u32 createLayout(void* resource, u32 first, u32 second, u32 third, void* descriptor);
};
WWHD_OFFSET(hdBatonSignForWindow_c, mSignFrame, 0x50);
WWHD_SIZE(hdBatonSignForWindow_c, 0x54);
