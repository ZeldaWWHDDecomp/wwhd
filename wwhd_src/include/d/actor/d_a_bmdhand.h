/* bmdhand_class (Kalle Demos ceiling tentacle), WWHD layout. 
 *
 * GameCube members +0x11C up to mLineMat; mDoExt_3DlineMat1_c is 0x188 in HD (GameCube 0x3C),
 * so the colliders and m824 move by a further +0x14C. */
#pragma once
#include "bindings.h"

struct hand_s {
    /* 0x00 */ cXyz m00;
    /* 0x0C */ cXyz m0C;
    /* 0x18 */ be<f32> m18;
};
WWHD_SIZE(hand_s, 0x1C);

/* mDoExt_3DlineMat1_c (HD 0x188): vtable at +0x130, line array pointer at +0x184
 * (line 0: +0 positions, +4 sizes) */
struct mDoExt_3DlineMat1_l {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines;
};
WWHD_SIZE(mDoExt_3DlineMat1_l, 0x188);

struct bmdhand_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ be<s16> m2B8;
    /* 0x3D6 */ be<s16> m2BA;
    /* 0x3D8 */ be<s16> m2BC;
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0[4];
    /* 0x3E4 */ be<s16> m2C8;
    /* 0x3E6 */ be<s16> m2CA;
    /* 0x3E8 */ cXyz m2CC;
    /* 0x3F4 */ cXyz m2D8;
    /* 0x400 */ cXyz m2E4;
    /* 0x40C */ cXyz m2F0;
    /* 0x418 */ be<f32> m2FC;
    /* 0x41C */ be<s16> m300;
    /* 0x41E */ be<s16> m302;
    /* 0x420 */ be<s8> m304;
    /* 0x421 */ u8 _421[3];
    /* 0x424 */ be<f32> m308;
    /* 0x428 */ be<f32> m30C;
    /* 0x42C */ be<f32> m310;
    /* 0x430 */ be<f32> m314;
    /* 0x434 */ be<f32> m318;
    /* 0x438 */ be<f32> m31C;
    /* 0x43C */ be<f32> m320;
    /* 0x440 */ hand_s m324[20];
    /* 0x670 */ mDoExt_3DlineMat1_l mLineMat;
    /* 0x7F8 */ dCcD_Stts mStts;
    /* 0x834 */ dCcD_Sph m5CC;
    /* 0x960 */ dCcD_Sph m6F8;
    /* 0xA8C */ be<u8> m824;
    /* 0xA8D */ u8 _A8D[3];
};
WWHD_OFFSET(bmdhand_class, m324, 0x440);
WWHD_OFFSET(bmdhand_class, mLineMat, 0x670);
WWHD_OFFSET(bmdhand_class, mStts, 0x7F8);
WWHD_OFFSET(bmdhand_class, m6F8, 0x960);
WWHD_OFFSET(bmdhand_class, m824, 0xA8C);

/* bmd_class (Kalle Demos), the fields the tentacles use (HD offsets) */
struct bmd_l {
    /* 0x000 */ u8 _000[0x328];
    /* 0x328 */ csXyz shape_angle;          /* fopAc_ac_c::shape_angle */
    /* 0x32E */ u8 _32E[0x3D0 - 0x32E];
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3D4 */ u8 _3D4[0x424 - 0x3D4];
    /* 0x424 */ be<s16> m302;               /* GameCube 0x302 */
    /* 0x426 */ u8 _426[4];
    /* 0x42A */ be<s16> m308_0;             /* GameCube m308[0] */
    /* 0x42C */ u8 _42C[0x434 - 0x42C];
    /* 0x434 */ be<s16> m312;
    /* 0x436 */ u8 _436[0x448 - 0x436];
    /* 0x448 */ be<f32> m328;
    /* 0x44C */ u8 _44C[0x451 - 0x44C];
    /* 0x451 */ be<s8> m331;
    /* 0x452 */ be<s8> m332;
};
WWHD_OFFSET(bmd_l, m332, 0x452);

struct daBmdhand_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<s16> m0C;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<u32> __vtbl;
};
WWHD_SIZE(daBmdhand_HIO_c, 0x10);
