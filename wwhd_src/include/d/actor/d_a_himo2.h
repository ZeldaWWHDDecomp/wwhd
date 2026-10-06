/* himo2_class (Grappling Hook rope), WWHD layout. 
 *
 * GameCube -> WWHD: fopAc_ac_c +0x11C up to m1F30. mDoExt_3DlineMat1_c grew from 0x3C to 0x188
 * (constructor 025EB82C), so m1F6C..m1FD8 shift and dCcD_Stts m2014 is at 0x2514. HD keeps only
 * 100 entries of m218C (GameCube 200). After mpHookModel HD inserts four mDoExt_J3DModelPacketS
 * (0xB0 each, constructor 02080404), so the members from m24B4 on are +0x630. Size 0x2D68.
 * Offsets measured from daHimo2_Create, CallbackCreateHeap, s_a_d_sub and himo2_bg_check. */
#pragma once
#include "bindings.h"

struct himo2_s {
    /* 0x00 */ u8 m00[0x0C];
    /* 0x0C */ be<f32> m0C;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ be<s16> m1C;
    /* 0x1E */ be<s16> m1E;
    /* 0x20 */ u8 m20[4];
};
WWHD_SIZE(himo2_s, 0x24);

/* mDoExt_3DlineMat1_c (HD 0x188), opaque */
struct mDoExt_3DlineMat1_c_l {
    u8 _[0x188];
};

struct himo2_class : fopAc_ac_c {
    /* 0x03AC */ u8 _3AC[0x3B8 - 0x3AC];
    /* 0x03B8 */ be<s16> m029C;
    /* 0x03BA */ be<s16> m029E;
    /* 0x03BC */ be<s16> m02A0;
    /* 0x03BE */ be<s16> m02A2;
    /* 0x03C0 */ be<s16> m02A4;
    /* 0x03C2 */ u8 _3C2[0x3D0 - 0x3C2];
    /* 0x03D0 */ cXyz m02B4;
    /* 0x03DC */ u8 _3DC[0x3E8 - 0x3DC];
    /* 0x03E8 */ be<s32> m02CC;
    /* 0x03EC */ u8 _3EC[0x3F4 - 0x3EC];
    /* 0x03F4 */ be<u32> m02D8;
    /* 0x03F8 */ be<s32> m02DC;
    /* 0x03FC */ be<s8> m02E0;
    /* 0x03FD */ u8 _3FD[3];
    /* 0x0400 */ be<f32> m02E4;
    /* 0x0404 */ u8 _404[4];
    /* 0x0408 */ cXyz m02EC[2];
    /* 0x0420 */ u8 _420[4];
    /* 0x0424 */ be<s16> m0308;
    /* 0x0426 */ u8 _426[0x42C - 0x426];
    /* 0x042C */ himo2_s m0310[100];
    /* 0x123C */ himo2_s m1120[100];
    /* 0x204C */ mDoExt_3DlineMat1_c_l m1F30;
    /* 0x21D4 */ be<s32> m1F6C;
    /* 0x21D8 */ be<f32> m1F70[5];
    /* 0x21EC */ cXyz m1F84;
    /* 0x21F8 */ be<s16> m1F90;
    /* 0x21FA */ be<s16> m1F92;
    /* 0x21FC */ be<s16> m1F94;
    /* 0x21FE */ u8 _21FE[2];
    /* 0x2200 */ mDoExt_3DlineMat1_c_l m1F98;
    /* 0x2388 */ be<f32> m1FD4;
    /* 0x238C */ mDoExt_3DlineMat1_c_l m1FD8;
    /* 0x2514 */ dCcD_Stts m2014;
    /* 0x2550 */ dCcD_Sph m2050;
    /* 0x267C */ gptr<fopAc_ac_c> m217C;
    /* 0x2680 */ be<u32> m2180;
    /* 0x2684 */ be<f32> m2184;
    /* 0x2688 */ be<f32> m2188;
    /* 0x268C */ gptr<fopAc_ac_c> m218C[100];  /* HD: 100 (GameCube 200) */
    /* 0x281C */ be<u8> m24AC;
    /* 0x281D */ u8 _281D[3];
    /* 0x2820 */ gptr<J3DModel> mpHookModel;
    /* 0x2824 */ u8 mPackets[4][0xB0];          /* HD: mDoExt_J3DModelPacketS x4 */
    /* 0x2AE4 */ be<f32> m24B4;
    /* 0x2AE8 */ be<f32> m24B8;
    /* 0x2AEC */ be<s32> m24BC;
    /* 0x2AF0 */ be<s32> m24C0;
    /* 0x2AF4 */ be<f32> m24C4;
    /* 0x2AF8 */ be<s16> m24C8;
    /* 0x2AFA */ be<s16> m24CA;
    /* 0x2AFC */ cXyz m24CC;
    /* 0x2B08 */ be<s8> m24D8;
    /* 0x2B09 */ be<s8> m24D9;
    /* 0x2B0A */ u8 _2B0A[2];
    /* 0x2B0C */ cXyz m24DC;
    /* 0x2B18 */ cXyz m24E8;
    /* 0x2B24 */ be<f32> m24F4;
    /* 0x2B28 */ be<f32> m24F8;
    /* 0x2B2C */ be<f32> m24FC;
    /* 0x2B30 */ be<f32> m2500;
    /* 0x2B34 */ cXyz m2504;
    /* 0x2B40 */ be<s16> m2510;
    /* 0x2B42 */ be<s16> m2512;
    /* 0x2B44 */ be<f32> m2514;
    /* 0x2B48 */ be<f32> m2518;
    /* 0x2B4C */ be<s8> m251C;
    /* 0x2B4D */ u8 _2B4D[3];
    /* 0x2B50 */ be<f32> m2520;
    /* 0x2B54 */ cXyz m2524;
    /* 0x2B60 */ be<f32> m2530;
    /* 0x2B64 */ dBgS_AcchCir m2534;
    /* 0x2BA4 */ dBgS_ObjAcch m2574;
};
WWHD_OFFSET(himo2_class, m02CC, 0x3E8);
WWHD_OFFSET(himo2_class, m0310, 0x42C);
WWHD_OFFSET(himo2_class, m1F30, 0x204C);
WWHD_OFFSET(himo2_class, m1F98, 0x2200);
WWHD_OFFSET(himo2_class, m1FD8, 0x238C);
WWHD_OFFSET(himo2_class, m2014, 0x2514);
WWHD_OFFSET(himo2_class, m2050, 0x2550);
WWHD_OFFSET(himo2_class, m218C, 0x268C);
WWHD_OFFSET(himo2_class, m24AC, 0x281C);
WWHD_OFFSET(himo2_class, mpHookModel, 0x2820);
WWHD_OFFSET(himo2_class, m24B4, 0x2AE4);
WWHD_OFFSET(himo2_class, m2500, 0x2B30);
WWHD_OFFSET(himo2_class, m2534, 0x2B64);
WWHD_OFFSET(himo2_class, m2574, 0x2BA4);
WWHD_SIZE(himo2_class, 0x2D68);

/* himo2HIO_c (HD: vtable after the members), l_himo2HIO at 0x1046469C */
struct himo2HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<s16> m08;
    /* 0x06 */ be<s16> m0A;
    /* 0x08 */ be<s16> m0C;
    /* 0x0A */ be<s16> m0E;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<f32> m1C;
    /* 0x1C */ be<f32> m20;
    /* 0x20 */ be<u32> __vtbl;
};
WWHD_SIZE(himo2HIO_c, 0x24);
#define l_himo2HIO (*gabi::at<himo2HIO_c>(0x1046469C))
#define rope_scale (*gabi::at<be<f32>>(0x10464674))
#define himo2_btd (*gabi::at<be<u32>>(0x10464678)) /* found with b_a_sub (0216DE2C) */
#define himo2_dr (*gabi::at<be<u32>>(0x1046467C))  /* found with dr_a_sub (0216DDDC) */
/* the HD debug registers (REG0_F / REG0_S) that this TU still reads */
#define HIMO2_REG(off) (0x1047B608 + (off))

#define HIMO2_VTBL 0x10010EF4
#define HIMO2_SAFESTRING_VTBL 0x10010DEC

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025EB82C mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c (HD, out of line) */
static inline void mDoExt_3DlineMat1_c_ct(void* p) { gabi::call(0x025EB82C, p); }
/* 025EBA58 mDoExt_3DlineMat1_c::init(s16 numLines, s16 numSegments, ResTIMG*, int) */
static inline BOOL mDoExt_3DlineMat1_c_init(void* p, s32 n, s32 seg, void* img, s32 x) { return gabi::call<BOOL>(0x025EBA58, p, n, seg, img, x); }
/* 02080404 mDoExt_J3DModelPacketS::mDoExt_J3DModelPacketS (HD 0xB0) */
static inline void mDoExt_J3DModelPacketS_ct(void* p) { gabi::call(0x02080404, p); }
static inline u32 dComIfGp_getParticle_ea() { return gabi::load<u32>(dComIfGp_ea() + 0x5AB0); }
