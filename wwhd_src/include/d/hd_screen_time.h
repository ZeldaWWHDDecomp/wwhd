/** HD elapsed-time HUD; bottom/top placement selects the corresponding animation. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"
struct hdTimeScreen_c : NoControlPart_l {
    gptr<void> mpTimePane;
    be<u32> mAnimation, mPlacement;
    be<u8> mVisible;
    u8 mPadding[3];
    BOOL create(u32,u32,u32);
    BOOL loadLayout(void*,u32,u32,u32,void*);
    static void initializeStatics();
    static hdTimeScreen_c* construct(hdTimeScreen_c*);
    static void destroy(hdTimeScreen_c*, u32);
    void resetAnimation();
    BOOL canDisplay();
    void waitExecute();
    void inEnter();
    void updateText();
    void inExecute();
    void displayExecute();
    void outEnter();
    void outExecute();
    void outExit();
    void calc();
    void draw(void*);
    void resetState();
    void setBottom();
    void setTop();
};
WWHD_OFFSET(hdTimeScreen_c, mpTimePane, 0x50);
WWHD_OFFSET(hdTimeScreen_c, mAnimation, 0x54);
WWHD_OFFSET(hdTimeScreen_c, mPlacement, 0x58);
WWHD_OFFSET(hdTimeScreen_c, mVisible, 0x5C);
WWHD_SIZE(hdTimeScreen_c, 0x60);
