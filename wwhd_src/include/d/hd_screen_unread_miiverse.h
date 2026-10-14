/** HD unread Miiverse indicator and its bounded decimal counter. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"
struct hdUnreadMiiverse_c : NoControlPart_l {
    gptr<void> mpNumberPane;
    be<u32> mUnreadCount;
    BOOL create(u32,u32,u32);
    void findNumberPane();
    BOOL loadLayout(void*,u32,u32,u32,void*);
    static void initializeStatics();
    static hdUnreadMiiverse_c* construct(hdUnreadMiiverse_c*);
    static void destroy(hdUnreadMiiverse_c*, u32);
    void resetAnimation();
    BOOL canDisplay();
    void waitExecute();
    void updateText();
    void inEnter();
    void inExecute();
    void displayExecute();
    void outEnter();
    void outExecute();
    void resetState();
};
WWHD_OFFSET(hdUnreadMiiverse_c, mpNumberPane, 0x50);
WWHD_OFFSET(hdUnreadMiiverse_c, mUnreadCount, 0x54);
WWHD_SIZE(hdUnreadMiiverse_c, 0x58);
