/* HD HeartAll_00 screen; canonical parts base and two20-element pane arrays. */
#pragma once
#include "d/hd_parts_no_control.h"
#include "d/hd_parts_heart.h"
#include "d/hd_screen_command_a.h"
struct hdHudHeartScreen_c : NoControlPart_l {
    gptr<hdPartsPane_c> mpHeartPanes[20];
    gptr<hdPartsPane_c> mpLoopPanes[20];
    be<u32> mRefillTimer;
    be<u32> mShownLife;
    be<u32> mShownContainers;
    be<u32> mSelectedHeart;
    be<u8> mHidden;
    be<u8> mSpecialMode;
    be<u8> mFreezeValues;
    u8 mTail;
    static void destroy(hdHudHeartScreen_c*, u32);
    static hdHudHeartScreen_c* construct(hdHudHeartScreen_c*);
    void blankEnter();
    void blankMove();
    u32 heartValue(u32);
    void updateValues();
    void inEnter();
    void inMove();
    void outEnter();
    void outMove();
    void hideEnter();
    void hideMove();
    void specialMove();
    void destroyContents();
    void execute();
    void drawChildren();
    void reset();
    BOOL init(void*,u32,void*);
    BOOL createHeart(void*);
    BOOL loadLayout(void*,u32,u32,u32,void*);
    hdHudHeart* legacyView() { return reinterpret_cast<hdHudHeart*>(this); }
    hdHeartPart_c* heart(u32 index) { return reinterpret_cast<hdHeartPart_c*>(child(index)); }
};
WWHD_OFFSET(hdHudHeartScreen_c, mpHeartPanes, 0x50);
WWHD_OFFSET(hdHudHeartScreen_c, mpLoopPanes, 0xA0);
WWHD_OFFSET(hdHudHeartScreen_c, mRefillTimer, 0xF0);
WWHD_OFFSET(hdHudHeartScreen_c, mHidden, 0x100);
WWHD_SIZE(hdHudHeartScreen_c, 0x104);
struct hdHeartModeView_l {
    u8 mPrefix[0x3C];
    be<u16> mMode;
    u8 mToLife[5];
    be<u8> mLife;
    be<u8> mContainers;
    u8 mToVisible[0xE];
    be<u8> mVisible;
};
WWHD_OFFSET(hdHeartModeView_l, mLife, 0x43);
WWHD_OFFSET(hdHeartModeView_l, mVisible, 0x53);
