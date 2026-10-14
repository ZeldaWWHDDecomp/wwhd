/** HD rupee HUD screen and its particle anchor; canonical parts base retained. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"
struct hdRupyEmitter_l {
    u8 mPrefix[0x5C];
    be<s32> mLifetime;
    u8 mToPosition[0x22C - 0x60];
    be<f32> mX, mY, mZ;
    u8 mToFlags[0x254 - 0x238];
    be<u32> mFlags;
    u8 mToLayer[0x262 - 0x258];
    be<u8> mLayer;
};
WWHD_OFFSET(hdRupyEmitter_l, mX, 0x22C);
WWHD_OFFSET(hdRupyEmitter_l, mFlags, 0x254);
WWHD_OFFSET(hdRupyEmitter_l, mLayer, 0x262);
struct hdRupyScreen_c : NoControlPart_l {
    gptr<void> mpNumberPanes[2];
    gptr<hdRupyEmitter_l> mpEmitter;
    gptr<void> mpAnchorPane;
    be<u16> mRupees;
    be<u8> mWallet;
    u8 mPadding;
    BOOL create(u32,u32,u32);
    BOOL loadLayout(void*,u32,u32,u32,void*);
    static void initializeStatics();
    static hdRupyScreen_c* construct(hdRupyScreen_c*);
    static void destroy(hdRupyScreen_c*, u32);
    void resetInAnimation();
    void inEnter();
    void displayExecute();
    void waitExecute();
    void hideEmitter();
    void showEmitter();
    void startOutAnimation();
    void updateEmitter();
    void inExecute();
    void outExecute();
    void endEmitter();
    void endExecute();
    void updateNumber();
};
WWHD_OFFSET(hdRupyScreen_c, mpNumberPanes, 0x50);
WWHD_OFFSET(hdRupyScreen_c, mpEmitter, 0x58);
WWHD_OFFSET(hdRupyScreen_c, mpAnchorPane, 0x5C);
WWHD_OFFSET(hdRupyScreen_c, mRupees, 0x60);
WWHD_OFFSET(hdRupyScreen_c, mWallet, 0x62);
WWHD_SIZE(hdRupyScreen_c, 0x64);
