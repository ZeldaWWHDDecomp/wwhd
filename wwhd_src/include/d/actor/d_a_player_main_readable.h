#pragma once
#include "d/actor/d_a_player_main.h"
#include "gc_api/d/d_com_inf_game.h"
using daPy_actorKeep_c = daPy_actorKeep_l;
// HD: views of the opaque animation and item-heap blocks in the shared Link layout.
struct PlayerAnmHeap { be<u16> mIdx; u8 padding[0xE]; };
WWHD_SIZE(PlayerAnmHeap, 0x10);
struct PlayerItemBlock {
    u8 padding[0x4438 - 0xD90];
    be<u32> mpItemHeaps[2];
};
WWHD_OFFSET(PlayerItemBlock, mpItemHeaps, 0x36A8);
struct PlayerAnmData {
    be<u16> mUnderBckIdx, mUpperBckIdx;
    be<u8> mLeftHandIdx, mRightHandIdx;
    be<u16> mTexAnmIdx;
};
WWHD_SIZE(PlayerAnmData, 8);
enum {
    dItemNo_EMPTY_BOTTLE_e = 0x50, dItemNo_UNK_BOTTLE_60_e = 0x60,
    dItemNo_PICTO_BOX_e = 0x23, dItemNo_DELUXE_PICTO_BOX_e = 0x26,
    dItemNo_BOW_e = 0x27, dItemNo_MAGIC_ARROW_e = 0x35, dItemNo_LIGHT_ARROW_e = 0x36,
    dItemBtn_X_e = 0, dItemBtn_Y_e = 1,
    dRes_INDEX_LKANM_BCK_TAKEL_e = 0x105, dRes_INDEX_LKANM_BCK_TAKER_e = 0x106,
    dRes_INDEX_LKANM_BCK_REST_e = 0xD7,
    dRes_INDEX_LKANM_BCK_ATNG_e = 0x16, dRes_INDEX_LKANM_BCK_ATNGHAM_e = 0x1B,
    dRes_INDEX_LKANM_BTP_TMABAA_e = 0x22D, dRes_INDEX_LKANM_BTP_TMABAJ_e = 0x238,
    UPPER_MOVE2_e = 2, UNDER_MOVE0_e = 0,
    DIR_FORWARD = 0, DIR_BACKWARD = 1, DIR_LEFT = 2, DIR_RIGHT = 3,
    daPyProc_CROUCH_DEFENSE_e = 0xC, daPyProc_CROUCH_DEFENSE_SLIP_e = 0xD,
    daPyProc_AUTO_JUMP_e = 0x24, daPyProc_GUARD_SLIP_e = 0x6D,
    dCcG_At_Spl_UNK8 = 8,
    daPyStts0_SUBJECT_e = 0x2000, JA_SE_SUBJ_VIEW_IN = 0x8FA,
};
// HD: preserve the full sound return register. These calls cross independently verified ranges.
u32 Player_seStart(u32, cXyz*, u32, s32);
void Player_systemSound(u32);
BOOL Player_checkItemEquipAnime(daPy_lk_c*);
J3DModel* Player_setAnimeHeap(daPy_lk_c*, u32);
void Player_endDamageEmitter(daPy_lk_c*);
BOOL Player_checkNextMode(daPy_lk_c*, s32);
