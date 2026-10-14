/** Companion lifecycle for the merged swim HUD; canonical parts base and owned timer state. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"
struct hdSwimTimeHud_c : NoControlPart_l {
    gptr<void> mpPanes[6];
    be<f32> mPaneY[6];
    be<f32> mDistance, mRatio, mBlinkRange;
    be<u32> mBlinkFrame, mBlinkPeriod, mSoundCooldown, mFlashDelay, mInAnimation;
    be<u8> mFlashing[6], mAvailable, mBlinking;
    BOOL create(u32,u32,u32);
    BOOL loadLayout(void*,u32,u32,u32,void*);
    static void initializeStatics();
    static hdSwimTimeHud_c* construct(hdSwimTimeHud_c*);
    void finishedExecute();
    void resetState();
    static void destroy(hdSwimTimeHud_c*, u32);
    void resetAnimations();
    BOOL canDisplay();
    void waitExecute();
    void inEnter();
    void inExecute();
    void displayEnter();
    void outEnter();
    void outExecute();
};
WWHD_OFFSET(hdSwimTimeHud_c, mpPanes, 0x50);
WWHD_OFFSET(hdSwimTimeHud_c, mRatio, 0x84);
WWHD_OFFSET(hdSwimTimeHud_c, mInAnimation, 0x9C);
WWHD_OFFSET(hdSwimTimeHud_c, mFlashing, 0xA0);
WWHD_OFFSET(hdSwimTimeHud_c, mAvailable, 0xA6);
WWHD_SIZE(hdSwimTimeHud_c, 0xA8);
