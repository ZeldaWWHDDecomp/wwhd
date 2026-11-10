/* Rat group actor, GameCube field names and verified WWHD offsets. Local only. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_kankyo.h"

enum { dRes_INDEX_NZG_BDL_KANA_00_e = 3 };
enum : s16 { fpcNm_Bomb2_e = 0x127, fpcNm_ITEM_e = 0xFF };
static const u32 NZG_VTBL = 0x10024598;
static const u32 CM3DGAAB_VTBL = 0x10024588;
static inline f32 REG8_F(int i) { return REG_F(8, i); }
struct nzg_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<J3DModel> mpModel;
    /* 0x3D4 */ be<u8> mSpawnRangePrm;  /* was m2B8: daNZG_Create 0231175C: param bits 0-7; mSpawnRange = it * 10 */
    /* 0x3D5 */ be<u8> mMaxRatsPrm;  /* was m2B9: daNZG_Create 0231175C: param bits 8-15; mMaxRats = it (0 -> 1) */
    /* 0x3D6 */ be<u8> mSpawnMode;  /* was m2BA: daNZG_Create 0231175C: param bits 16-23 (> 2 -> 0); mode 0 also creates an NpcNz child; nzg_00_move (in daNZG_Execute 02311240) gives the rat param 0x100 (no player-range wait) always for 1, by 50 % for 2 */
    /* 0x3D7 */ be<u8> mBlocked;  /* was m2BB: nzg_00_move (in daNZG_Execute 02311240) sets 1 when another actor (not a rat, bomb, item or bait) touches the hole cylinder; daNZG_Execute switches on it; nzg_01_move clears it once that actor is 80 away */
    /* 0x3D8 */ u8 m2BC[2];
    /* 0x3DA */ be<s16> mRatCount;  /* was m2BE: nzg_00_move (in daNZG_Execute 02311240) adds 1 per rat created (fopAcM_createChild fpcNm_NZ_e) and creates no more once it reaches mMaxRats */
    /* 0x3DC */ be<s16> mMaxRats;  /* was m2C0: daNZG_Create 0231175C: mMaxRatsPrm or 1; nzg_00_move (in daNZG_Execute 02311240) stops creating rats at it */
    /* 0x3DE */ be<s16> mTimers[5];  /* was m2C2: daNZG_Execute 02311240 counts all 5 down; nzg_00_move (in daNZG_Execute) sets [0] to (child id & 3) * 10 + 20 after a rat and waits for it */
    /* 0x3E8 */ be<f32> mSpawnRange;  /* was m2CC: daNZG_Create 0231175C: mSpawnRangePrm * 10; nzg_00_move (in daNZG_Execute 02311240) creates rats without param 0x100 only while the player is within it * 0.5 */
    /* 0x3EC */ be<f32> mNoSpawnRange;  /* was m2D0: daNZG_Execute 02311240 sets 400 each frame; nzg_00_move (in daNZG_Execute) creates a rat only while the player is farther than it * 0.5 */
    /* 0x3F0 */ be<u32> mBlockerId;  /* was m2D4: nzg_00_move (in daNZG_Execute 02311240) stores the id of the actor on the hole; nzg_01_move (in daNZG_Execute) checks its distance and resets it to fpcM_ERROR_PROCESS_ID_e */
    /* 0x3F4 */ gptr<dPath> mpPath;
    /* 0x3F8 */ u8 m2DC[0x405 - 0x3F8];
    /* 0x405 */ be<u8> mPathNo;  /* was m2E9: daNZG_Create 0231175C: param bits 24-31; dPath_GetRoomPath(it, room) into mpPath when it is not 0xFF */
    /* 0x406 */ u8 _406[2];
    /* 0x408 */ dCcD_Stts mStts;
    /* 0x444 */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(nzg_class, mStts, 0x408);
WWHD_SIZE(nzg_class, 0x574);


u32 fopAcM_createChild(s16 name, u32 parent, u32 param, cXyz* pos, s32 roomNo,
                      csXyz* angle, cXyz* scale, s8 subtype, u32 createFunc);
u32 fopAcM_createChild(const char* name, u32 parent, u32 param, cXyz* pos, s32 roomNo,
                      csXyz* angle, cXyz* scale, u32 createFunc);
