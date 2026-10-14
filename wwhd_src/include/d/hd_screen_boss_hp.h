/* HD boss health meter and its nested boss icon. */
#pragma once
#include "d/hd_parts_no_control.h"
#include "d/hd_parts.h"
struct hdBossPartDesc_c {
    u8 _00[8];
    hdUiSafeString mResource;
    u8 _10[4];
    hdUiSafeString mClassName;
};
WWHD_OFFSET(hdBossPartDesc_c, mClassName, 0x14);
/* a no-control part (FW-parts_msg_base) */
struct hdBossHp_c : NoControlPart_l {
    static hdBossHp_c* construct(hdBossHp_c* object);
    static void destroy(hdBossHp_c* object, u32 flags);
    void waitEnter(); void waitExecute(); void inEnter(); void inExecute();
    void displayExecute(); void outEnter(); void outExecute(); void outExit();
    u32 setup(u32 first, u32 second, u32 third);
    void toWait();
    u32 load(void* name, u32 first, u32 second, u32 third, void* desc);
    u32 createBossIcon(hdBossPartDesc_c* desc);
};
WWHD_SIZE(hdBossHp_c, 0x50);
