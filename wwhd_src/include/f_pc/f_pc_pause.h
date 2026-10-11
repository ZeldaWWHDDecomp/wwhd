/* Process pause helpers, GameCube names with the verified WWHD node prefix.
 * HD: base has a vtable at B4, moving subtype/layer four bytes. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_base.h"
struct pause_process_node : base_process_class {
    /* 0xB8 */ be<u32> mSubType;
    /* 0xBC */ gptr<void> mpNodeMtd;
    /* 0xC0 */ u8 mLayer[0x2C];
};
WWHD_OFFSET(pause_process_node, mSubType, 0xB8);
WWHD_OFFSET(pause_process_node, mLayer, 0xC0);
WWHD_SIZE(pause_process_node, 0xEC);
using pause_callback = s32 (*)(void*, u32);
/* fpcBs_Is_JustOfType: f_pc/f_pc_base.h */
void fpcLyIt_OnlyHere(void* layer, pause_callback callback, u32 mask);
