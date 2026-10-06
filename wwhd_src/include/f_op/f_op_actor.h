/* fopAc_ac_c: WWHD layout.
 *
 * GameCube -> WWHD: unchanged up to 0xC0; +4 from 0xC4 (the C++ vtable pointer at 0xB4 of the
 * HD classes pushes the tail of leafdraw_class); dKy_tevstr_c grew from 0xB0 to 0x1C8 (HD
 * lighting), so every field from setID on is +0x11C. Offsets from tools/decomp/layout.py and
 * the verified functions. Size 0x3AC (GameCube 0x290). */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "wwhd.h"

struct actor_place {
    /* 0x00 */ cXyz pos;
    /* 0x0C */ csXyz angle;
    /* 0x12 */ be<s8> roomNo;
    /* 0x13 */ be<u8> field_0x13;
};
WWHD_SIZE(actor_place, 0x14);

/* dKy_tevstr_c (HD: 0x1C8 bytes); only the fields used so far */
struct dKy_tevstr_c {
    /* 0x00 */ u8 _00[0xB9];
    /* 0xB9 */ be<s8> mRoomNo; /* GameCube 0xA9 */
    /* 0xBA */ u8 _BA[0x1C8 - 0xBA];
};
WWHD_SIZE(dKy_tevstr_c, 0x1C8);

struct request_of_phase_process_class {
    /* 0x0 */ be<u32> mpHandlerTable;
    /* 0x4 */ be<s32> id;
};

WWHD_OPAQUE(JKRSolidHeap);

struct fopAc_ac_c {
    /* 0x000 */ u8 _000[0xB0];
    /* 0x0B0 */ be<u32> mParameters;       /* base.base.mParameters */
    /* 0x0B4 */ be<u32> __vtbl;            /* HD: virtual destructor */
    /* 0x0B8 */ u8 _0B8[0xF4 - 0xB8];
    /* 0x0F4 */ gptr<JKRSolidHeap> heap;   /* GameCube 0x0F0 */
    /* 0x0F8 */ u8 _0F8[0x110 - 0xF8];     /* eventInfo */
    /* 0x110 */ dKy_tevstr_c tevStr;       /* GameCube 0x10C */
    /* 0x2D8 */ be<u16> setID;
    /* 0x2DA */ be<u8> group;
    /* 0x2DB */ be<u8> cullType;
    /* 0x2DC */ be<u8> demoActorID;
    /* 0x2DD */ be<s8> argument;
    /* 0x2DE */ be<u8> gbaName;
    /* 0x2DF */ u8 _2DF;
    /* 0x2E0 */ be<u32> actor_status;
    /* 0x2E4 */ be<u32> actor_condition;
    /* 0x2E8 */ be<u32> parentActorID;
    /* 0x2EC */ actor_place home;
    /* 0x300 */ actor_place old;
    /* 0x314 */ actor_place current;
    /* 0x328 */ csXyz shape_angle;
    /* 0x32E */ u8 _32E[2];
    /* 0x330 */ cXyz scale;
    /* 0x33C */ cXyz speed;
    /* 0x348 */ be<u32> cullMtx;
    /* 0x34C */ u8 cull[0x18];
    /* 0x364 */ be<f32> cullSizeFar;
    /* 0x368 */ be<u32> model;
    /* 0x36C */ be<u32> jntHit;
    /* 0x370 */ be<f32> speedF;
    /* 0x374 */ be<f32> gravity;
    /* 0x378 */ be<f32> maxFallSpeed;
    /* 0x37C */ cXyz eyePos;
    /* 0x388 */ u8 attention_info[0x18];
    /* 0x3A0 */ be<s8> max_health;
    /* 0x3A1 */ be<s8> health;
    /* 0x3A2 */ u8 _3A2[2];
    /* 0x3A4 */ be<s32> itemTableIdx;
    /* 0x3A8 */ be<u8> stealItemBitNo;
    /* 0x3A9 */ be<s8> stealItemLeft;
    /* 0x3AA */ u8 _3AA[2]; /* HD: +0x3AA construction state (0 raw, 1 constructed, 2 destroyed; fopAc_Delete calls the virtual dtor when 1) */
};
WWHD_SIZE(fopAc_ac_c, 0x3AC);
WWHD_OFFSET(fopAc_ac_c, tevStr, 0x110);
WWHD_OFFSET(fopAc_ac_c, home, 0x2EC);
WWHD_OFFSET(fopAc_ac_c, current, 0x314);
WWHD_OFFSET(fopAc_ac_c, speedF, 0x370);
WWHD_OFFSET(fopAc_ac_c, eyePos, 0x37C);

enum {
    fopAcCnd_NOEXEC_e = 0x02,
    fopAcCnd_INIT_e = 0x08,
    fopAcCnd_NODRAW_e = 0x04, /* HD: fopAc_Draw sets/clears bit 0x04 */
};

/* 025D4ED0 fopAc_ac_c::fopAc_ac_c() */
inline void fopAc_ac_c_ct(fopAc_ac_c* p) { gabi::call(0x025D4ED0, p); }

/* fopEn_enemy_c (GameCube 0x290..0x2AC), assumed +0x11C like the rest of the base: confirm on first use */
struct fopEn_enemy_c : fopAc_ac_c {
    /* 0x3AC */ be<f32> mBtHeight;
    /* 0x3B0 */ be<f32> mBtBodyR;
    /* 0x3B4 */ be<f32> mBtMaxDis;
    /* 0x3B8 */ be<f32> mBtStartFrame;
    /* 0x3BC */ be<f32> mBtEndFrame;
    /* 0x3C0 */ be<f32> mBtNowFrame;
    /* 0x3C4 */ be<u8> mBtAttackType;
    /* 0x3C5 */ u8 _3C5[3];
};
WWHD_SIZE(fopEn_enemy_c, 0x3C8);
