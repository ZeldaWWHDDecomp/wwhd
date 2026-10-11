/* d_a_player (GameCube names): daPy_py_c in d/actor/d_a_player.h. */
#pragma once
#include "d/actor/d_a_player.h"

// HD: Link actor pointer (verified caller 024B2828) [v d_a_tag_ret]
inline daPy_py_c* daPy_getPlayerLinkActorClass() {
    return gabi::at<daPy_py_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYERPTR));
}
// HD: virtual vtable +0x8C (verified caller 024B2828) [v d_a_tag_ret]
inline void daPy_py_c_onDekuSpReturnFlg(daPy_py_c* player, u32 vtable, u8 linkId) {
    gabi::call_ptr(gabi::load<u32>(vtable + 0x8C), player, linkId);
}

// HD: virtual slots used by boss cutscenes [v d_a_bmd].
struct daPy_BossVirtualTable {
    u8 mPrefix[0xE4];
    be<u32> mVoiceStart;
    u8 mUnusedSlots[0x12C - 0xE8];
    be<u32> mSetThrowDamage;
};
WWHD_OFFSET(daPy_BossVirtualTable, mVoiceStart, 0xE4);
WWHD_OFFSET(daPy_BossVirtualTable, mSetThrowDamage, 0x12C);
WWHD_SIZE(daPy_BossVirtualTable, 0x130);
inline void daPy_py_c_voiceStart(daPy_py_c* player, u32 sound) {
    auto* table = gabi::at<daPy_BossVirtualTable>(player->__vtbl);
    gabi::call_ptr(table->mVoiceStart, player, sound);
}
inline void daPy_py_c_setThrowDamage(daPy_py_c* player, cXyz* pos, s16 angle, f32 forward, f32 vertical, s32 mode) {
    auto* table = gabi::at<daPy_BossVirtualTable>(player->__vtbl);
    gabi::call_ptr(table->mSetThrowDamage, player, pos, angle, forward, vertical, mode);
}
