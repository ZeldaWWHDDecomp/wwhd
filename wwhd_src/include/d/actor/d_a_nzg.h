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
    /* 0x3D4 */ be<u8> m2B8;
    /* 0x3D5 */ be<u8> m2B9;
    /* 0x3D6 */ be<u8> m2BA;
    /* 0x3D7 */ be<u8> m2BB;
    /* 0x3D8 */ u8 m2BC[2];
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0;
    /* 0x3DE */ be<s16> m2C2[5];
    /* 0x3E8 */ be<f32> m2CC;
    /* 0x3EC */ be<f32> m2D0;
    /* 0x3F0 */ be<u32> m2D4;
    /* 0x3F4 */ gptr<dPath> mpPath;
    /* 0x3F8 */ u8 m2DC[0x405 - 0x3F8];
    /* 0x405 */ be<u8> m2E9;
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
