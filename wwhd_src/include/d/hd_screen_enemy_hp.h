/* HD enemy health meter, with a positioned health pane. */
#pragma once
#include "d/hd_parts_no_control.h"
#include "d/hd_parts.h"
struct hdEnemyHpPaneTable_c { u8 _00[0x5c]; gfn mFindPane; };
struct hdEnemyHpPane_c {
    u8 _00[8]; gptr<hdEnemyHpPaneTable_c> mVtable; u8 _0c[0x10];
    be<f32> mX, mY, mZ; u8 _28[0x1c]; be<u8> mFlags;
};
WWHD_OFFSET(hdEnemyHpPane_c, mX, 0x1c);
WWHD_OFFSET(hdEnemyHpPane_c, mZ, 0x24);
WWHD_OFFSET(hdEnemyHpPane_c, mFlags, 0x44);
/* a no-control part (FW-parts_msg_base) */
struct hdEnemyHp_c : NoControlPart_l {
    gptr<hdEnemyHpPane_c> mpHealthPane;
    static hdEnemyHp_c* construct(hdEnemyHp_c* object);
    static void destroy(hdEnemyHp_c* object, u32 flags);
    void waitExecute(); void displayExecute();
    u32 setup(u32 first, u32 second, u32 third);
    u32 load(void* name, u32 first, u32 second, u32 third, void* desc);
};
WWHD_SIZE(hdEnemyHp_c, 0x54);
