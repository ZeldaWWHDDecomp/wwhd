// Puppet Ganon, phase three: GameCube names with the verified WWHD layout.
// HD: model packets are 0xB0 bytes, line materials are enlarged, and each body part is 0x19D8.
#pragma once
#include "bindings.h"

struct bgn3_ke_s {
    /* 0x00000 */ cXyz m00[5];
    /* 0x0003C */ cXyz m3C[5];
    /* 0x00078 */ cXyz m78;
};
WWHD_OFFSET(bgn3_ke_s, m00, 0x0);
WWHD_OFFSET(bgn3_ke_s, m3C, 0x3C);
WWHD_OFFSET(bgn3_ke_s, m78, 0x78);
WWHD_SIZE(bgn3_ke_s, 0x84);

struct Bgn3LineMat0 {
    /* 0x00000 */ u8 _00000[0x130];
    /* 0x00130 */ be<u32> __vtbl;
    /* 0x00134 */ u8 _00134[0x10];
    /* 0x00144 */ be<u32> mpLines;
};
WWHD_OFFSET(Bgn3LineMat0, __vtbl, 0x130);
WWHD_OFFSET(Bgn3LineMat0, mpLines, 0x144);
WWHD_SIZE(Bgn3LineMat0, 0x148);

struct Bgn3LineMat1 {
    /* 0x00000 */ u8 _00000[0x130];
    /* 0x00130 */ be<u32> __vtbl;
    /* 0x00134 */ u8 _00134[0x50];
    /* 0x00184 */ be<u32> mpLines;
};
WWHD_OFFSET(Bgn3LineMat1, __vtbl, 0x130);
WWHD_OFFSET(Bgn3LineMat1, mpLines, 0x184);
WWHD_SIZE(Bgn3LineMat1, 0x188);

struct part_s3 {
    /* 0x00000 */ gptr<J3DModel> mpPart3Model;
    /* 0x00004 */ u8 m0004[0xB0];
    /* 0x000B4 */ dKy_tevstr_c mPart3TevStr;
    /* 0x0027C */ be<s16> m00C8;
    /* 0x0027E */ u8 _0027E[0x2];
    /* 0x00280 */ be<f32> m00CC;
    /* 0x00284 */ be<s16> mPart3ArrowHitFlashTimer;
    /* 0x00286 */ u8 _00286[0x2];
    /* 0x00288 */ cXyz m00D4;
    /* 0x00294 */ be<s16> m00E0;
    /* 0x00296 */ be<s16> m00E2;
    /* 0x00298 */ u8 _00298[0x4];
    /* 0x0029C */ cXyz m00E8;
    /* 0x002A8 */ be<f32> m00F4;
    /* 0x002AC */ dCcD_Sph mPart3Sph;
    /* 0x003D8 */ u8 _003D8[0xC];
    /* 0x003E4 */ gptr<JPABaseEmitter> mpPart3ArrowHitEmitter1;
    /* 0x003E8 */ gptr<JPABaseEmitter> mpPart3ArrowHitEmitter2;
    /* 0x003EC */ be<s16> mPart3ArrowHitEffectTimer;
    /* 0x003EE */ u8 _003EE[0x2];
    /* 0x003F0 */ bgn3_ke_s mPart3Hairs[40];
    /* 0x01890 */ Bgn3LineMat0 mPart3LineMat;
};
WWHD_OFFSET(part_s3, mpPart3Model, 0x0);
WWHD_OFFSET(part_s3, m0004, 0x4);
WWHD_OFFSET(part_s3, mPart3TevStr, 0xB4);
WWHD_OFFSET(part_s3, m00C8, 0x27C);
WWHD_OFFSET(part_s3, m00CC, 0x280);
WWHD_OFFSET(part_s3, mPart3ArrowHitFlashTimer, 0x284);
WWHD_OFFSET(part_s3, m00D4, 0x288);
WWHD_OFFSET(part_s3, m00E0, 0x294);
WWHD_OFFSET(part_s3, m00E2, 0x296);
WWHD_OFFSET(part_s3, m00E8, 0x29C);
WWHD_OFFSET(part_s3, m00F4, 0x2A8);
WWHD_OFFSET(part_s3, mPart3Sph, 0x2AC);
WWHD_OFFSET(part_s3, mpPart3ArrowHitEmitter1, 0x3E4);
WWHD_OFFSET(part_s3, mpPart3ArrowHitEmitter2, 0x3E8);
WWHD_OFFSET(part_s3, mPart3ArrowHitEffectTimer, 0x3EC);
WWHD_OFFSET(part_s3, mPart3Hairs, 0x3F0);
WWHD_OFFSET(part_s3, mPart3LineMat, 0x1890);
WWHD_SIZE(part_s3, 0x19D8);

struct bgn3_class : fopEn_enemy_c {
    /* 0x003C8 */ request_of_phase_process_class mPhase;
    /* 0x003D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x003D4 */ u8 m002B8[0xB0];
    /* 0x00484 */ gptr<J3DModel> m002CC;
    /* 0x00488 */ u8 m002D0[0xB0];
    /* 0x00538 */ bgn3_ke_s mHairs[40];
    /* 0x019D8 */ Bgn3LineMat0 mLineMat;
    /* 0x01B20 */ u8 m017A0[0xB0];
    /* 0x01BD0 */ gptr<J3DModel> mpJyakutenCModel;
    /* 0x01BD4 */ gptr<J3DModel> mpJyakutenBModel;
    /* 0x01BD8 */ gptr<J3DModel> mpJyakutenAModel;
    /* 0x01BDC */ gptr<mDoExt_brkAnm> mJyakutenCBrkAnm;
    /* 0x01BE0 */ gptr<mDoExt_brkAnm> mJyakutenBBrkAnm;
    /* 0x01BE4 */ u8 _01BE4[0x4];
    /* 0x01BE8 */ part_s3 mParts[10];
    /* 0x11E58 */ cXyz m0FD7C;
    /* 0x11E64 */ be<s16> m0FD88;
    /* 0x11E66 */ be<s16> m0FD8A;
    /* 0x11E68 */ be<s16> m0FD8C;
    /* 0x11E6A */ be<s16> m0FD8E;
    /* 0x11E6C */ be<s16> m0FD90;
    /* 0x11E6E */ be<s16> m0FD92;
    /* 0x11E70 */ be<s16> m0FD94;
    /* 0x11E72 */ be<s16> m0FD96;
    /* 0x11E74 */ be<f32> m0FD98;
    /* 0x11E78 */ be<s16> mArrowHitFlashTimer;
    /* 0x11E7A */ be<s16> m0FD9E;
    /* 0x11E7C */ gptr<JPABaseEmitter> mpArrowHitEmitter1;
    /* 0x11E80 */ gptr<JPABaseEmitter> mpArrowHitEmitter2;
    /* 0x11E84 */ be<s16> mArrowHitEffectTimer;
    /* 0x11E86 */ be<s16> m0FDAA[2];
    /* 0x11E8A */ be<s16> m0FDAE;
    /* 0x11E8C */ be<s16> m0FDB0;
    /* 0x11E8E */ u8 _11E8E[0x2];
    /* 0x11E90 */ be<s16> m0FDB4;
    /* 0x11E92 */ be<s16> m0FDB6;
    /* 0x11E94 */ be<f32> m0FDB8;
    /* 0x11E98 */ be<f32> m0FDBC;
    /* 0x11E9C */ dCcD_Stts mStts;
    /* 0x11ED8 */ dCcD_Sph m0FDFC;
    /* 0x12004 */ dCcD_Sph m0FF28;
    /* 0x12130 */ cXyz m10054;
    /* 0x1213C */ be<f32> m10060;
    /* 0x12140 */ Bgn3LineMat1 mRedRopeMat;
    /* 0x122C8 */ be<f32> m100A0;
    /* 0x122CC */ u8 mHDModelPacket[0xB0];
    /* 0x1237C */ u8 m100A4[4];
    /* 0x12380 */ be<s16> m100A8;
    /* 0x12382 */ u8 _12382[2];
    /* 0x12384 */ be<s32> m100AC;
    /* 0x12388 */ be<u8> m100B0;
    /* 0x12389 */ u8 _12389[0x3];
};
WWHD_OFFSET(bgn3_class, mPhase, 0x3C8);
WWHD_OFFSET(bgn3_class, mpMorf, 0x3D0);
WWHD_OFFSET(bgn3_class, m002B8, 0x3D4);
WWHD_OFFSET(bgn3_class, m002CC, 0x484);
WWHD_OFFSET(bgn3_class, m002D0, 0x488);
WWHD_OFFSET(bgn3_class, mHairs, 0x538);
WWHD_OFFSET(bgn3_class, mLineMat, 0x19D8);
WWHD_OFFSET(bgn3_class, m017A0, 0x1B20);
WWHD_OFFSET(bgn3_class, mpJyakutenCModel, 0x1BD0);
WWHD_OFFSET(bgn3_class, mpJyakutenBModel, 0x1BD4);
WWHD_OFFSET(bgn3_class, mpJyakutenAModel, 0x1BD8);
WWHD_OFFSET(bgn3_class, mJyakutenCBrkAnm, 0x1BDC);
WWHD_OFFSET(bgn3_class, mJyakutenBBrkAnm, 0x1BE0);
WWHD_OFFSET(bgn3_class, mParts, 0x1BE8);
WWHD_OFFSET(bgn3_class, m0FD7C, 0x11E58);
WWHD_OFFSET(bgn3_class, m0FD88, 0x11E64);
WWHD_OFFSET(bgn3_class, m0FD8A, 0x11E66);
WWHD_OFFSET(bgn3_class, m0FD8C, 0x11E68);
WWHD_OFFSET(bgn3_class, m0FD8E, 0x11E6A);
WWHD_OFFSET(bgn3_class, m0FD90, 0x11E6C);
WWHD_OFFSET(bgn3_class, m0FD92, 0x11E6E);
WWHD_OFFSET(bgn3_class, m0FD94, 0x11E70);
WWHD_OFFSET(bgn3_class, m0FD96, 0x11E72);
WWHD_OFFSET(bgn3_class, m0FD98, 0x11E74);
WWHD_OFFSET(bgn3_class, mArrowHitFlashTimer, 0x11E78);
WWHD_OFFSET(bgn3_class, m0FD9E, 0x11E7A);
WWHD_OFFSET(bgn3_class, mpArrowHitEmitter1, 0x11E7C);
WWHD_OFFSET(bgn3_class, mpArrowHitEmitter2, 0x11E80);
WWHD_OFFSET(bgn3_class, mArrowHitEffectTimer, 0x11E84);
WWHD_OFFSET(bgn3_class, m0FDAA, 0x11E86);
WWHD_OFFSET(bgn3_class, m0FDAE, 0x11E8A);
WWHD_OFFSET(bgn3_class, m0FDB0, 0x11E8C);
WWHD_OFFSET(bgn3_class, m0FDB4, 0x11E90);
WWHD_OFFSET(bgn3_class, m0FDB6, 0x11E92);
WWHD_OFFSET(bgn3_class, m0FDB8, 0x11E94);
WWHD_OFFSET(bgn3_class, m0FDBC, 0x11E98);
WWHD_OFFSET(bgn3_class, mStts, 0x11E9C);
WWHD_OFFSET(bgn3_class, m0FDFC, 0x11ED8);
WWHD_OFFSET(bgn3_class, m0FF28, 0x12004);
WWHD_OFFSET(bgn3_class, m10054, 0x12130);
WWHD_OFFSET(bgn3_class, m10060, 0x1213C);
WWHD_OFFSET(bgn3_class, mRedRopeMat, 0x12140);
WWHD_OFFSET(bgn3_class, m100A0, 0x122C8);
WWHD_OFFSET(bgn3_class, mHDModelPacket, 0x122CC);
WWHD_OFFSET(bgn3_class, m100A8, 0x12380);
WWHD_OFFSET(bgn3_class, m100AC, 0x12384);
WWHD_OFFSET(bgn3_class, m100B0, 0x12388);
WWHD_SIZE(bgn3_class, 0x1238C);

struct daBgn3_HIO_c {
    /* 0x00000 */ be<s8> mNo;
    /* 0x00001 */ be<u8> m05;
    /* 0x00002 */ be<u8> m06;
    /* 0x00003 */ u8 _00003[0x1];
    /* 0x00004 */ be<f32> m08;
    /* 0x00008 */ be<s16> m0C;
    /* 0x0000A */ be<s16> m0E;
    /* 0x0000C */ be<f32> m10;
    /* 0x00010 */ be<s16> m14;
    /* 0x00012 */ be<s16> m16;
    /* 0x00014 */ be<f32> m18;
    /* 0x00018 */ be<s16> m1C;
    /* 0x0001A */ be<s16> m1E;
    /* 0x0001C */ be<s16> m20;
    /* 0x0001E */ be<s16> m22;
    /* 0x00020 */ be<s16> m24;
    /* 0x00022 */ be<s16> m26;
    /* 0x00024 */ be<s16> m28;
    /* 0x00026 */ be<s16> m2A;
    /* 0x00028 */ be<s16> m2C;
    /* 0x0002A */ be<s16> m2E;
    /* 0x0002C */ be<s16> m30;
    /* 0x0002E */ u8 _0002E[0x2];
    /* 0x00030 */ be<f32> m34;
    /* 0x00034 */ be<f32> m38;
    /* 0x00038 */ be<s16> m3C;
    /* 0x0003A */ be<u8> m3E;
    /* 0x0003B */ u8 _0003B[0x1];
    /* 0x0003C */ be<f32> m40;
    /* 0x00040 */ be<f32> m44;
    /* 0x00044 */ be<f32> m48;
    /* 0x00048 */ be<f32> m4C;
    /* 0x0004C */ be<s16> m50;
    /* 0x0004E */ be<s16> m52;
    /* 0x00050 */ be<s16> m54;
    /* 0x00052 */ u8 _00052[0x2];
    /* 0x00054 */ be<u32> __vtbl;
};
WWHD_OFFSET(daBgn3_HIO_c, mNo, 0x0);
WWHD_OFFSET(daBgn3_HIO_c, m05, 0x1);
WWHD_OFFSET(daBgn3_HIO_c, m06, 0x2);
WWHD_OFFSET(daBgn3_HIO_c, m08, 0x4);
WWHD_OFFSET(daBgn3_HIO_c, m0C, 0x8);
WWHD_OFFSET(daBgn3_HIO_c, m0E, 0xA);
WWHD_OFFSET(daBgn3_HIO_c, m10, 0xC);
WWHD_OFFSET(daBgn3_HIO_c, m14, 0x10);
WWHD_OFFSET(daBgn3_HIO_c, m16, 0x12);
WWHD_OFFSET(daBgn3_HIO_c, m18, 0x14);
WWHD_OFFSET(daBgn3_HIO_c, m1C, 0x18);
WWHD_OFFSET(daBgn3_HIO_c, m1E, 0x1A);
WWHD_OFFSET(daBgn3_HIO_c, m20, 0x1C);
WWHD_OFFSET(daBgn3_HIO_c, m22, 0x1E);
WWHD_OFFSET(daBgn3_HIO_c, m24, 0x20);
WWHD_OFFSET(daBgn3_HIO_c, m26, 0x22);
WWHD_OFFSET(daBgn3_HIO_c, m28, 0x24);
WWHD_OFFSET(daBgn3_HIO_c, m2A, 0x26);
WWHD_OFFSET(daBgn3_HIO_c, m2C, 0x28);
WWHD_OFFSET(daBgn3_HIO_c, m2E, 0x2A);
WWHD_OFFSET(daBgn3_HIO_c, m30, 0x2C);
WWHD_OFFSET(daBgn3_HIO_c, m34, 0x30);
WWHD_OFFSET(daBgn3_HIO_c, m38, 0x34);
WWHD_OFFSET(daBgn3_HIO_c, m3C, 0x38);
WWHD_OFFSET(daBgn3_HIO_c, m3E, 0x3A);
WWHD_OFFSET(daBgn3_HIO_c, m40, 0x3C);
WWHD_OFFSET(daBgn3_HIO_c, m44, 0x40);
WWHD_OFFSET(daBgn3_HIO_c, m48, 0x44);
WWHD_OFFSET(daBgn3_HIO_c, m4C, 0x48);
WWHD_OFFSET(daBgn3_HIO_c, m50, 0x4C);
WWHD_OFFSET(daBgn3_HIO_c, m52, 0x4E);
WWHD_OFFSET(daBgn3_HIO_c, m54, 0x50);
WWHD_OFFSET(daBgn3_HIO_c, __vtbl, 0x54);
WWHD_SIZE(daBgn3_HIO_c, 0x58);

// HD process IDs corresponding to the GameCube searches in this translation unit.
enum { fpcNm_BGN_e = 0xF3, fpcNm_KS_e = 0xCD, fpcNm_ARROW_e = 0x1D8 };
// HD: shared pause byte written by damage reactions.
inline be<u8>& bgn3PauseTimer() { return *gabi::at<be<u8>>(0x101EACB7); }

template <class T> inline be<u32>& bgn3Word(T& field) {
    return *gabi::at<be<u32>>(gabi::ea(&field));
}

// HD: the inline line-check frame includes the pass-check subobjects (0x6C bytes).
struct Bgn3LinChk {
    be<u32> mpPolyPassChk;
    be<u32> mpGrpPassChk;
    u8 _08[8];
    be<u32> __vtbl_10;
    u8 mPolyInfo[0xC];
    be<u32> __vtbl_20;
    u8 _24[0xC];
    cXyz mCross;
    u8 _3C[0x1C];
    be<u32> __vtbl_58;
    be<u8> mPass[8];
    be<u32> __vtbl_64;
    be<u32> mGrp;
};
WWHD_SIZE(Bgn3LinChk, 0x6C);
WWHD_OFFSET(Bgn3LinChk, mCross, 0x30);
WWHD_OFFSET(Bgn3LinChk, mPass, 0x5C);
// HD: this inline attack-info frame is 0x14 bytes, not the general CcAtInfo layout.
struct Bgn3AtInfo {
    be<u32> mpObj;
    be<u32> mpActor;
    u8 _08[2];
    be<u8> mResultingAttackType;
    u8 _0B[9];
};
WWHD_SIZE(Bgn3AtInfo, 0x14);
WWHD_OFFSET(Bgn3AtInfo, mResultingAttackType, 0xA);
inline void cMtx_XrotS(Mtx34* matrix, s16 angle) { gabi::call(0x025F18EC, matrix, angle); }

inline part_s3* bgn3Part(u32 address) { return gabi::at<part_s3>(address); }
struct Bgn3SinCos {
    be<f32> mSin, mCos;
};
WWHD_SIZE(Bgn3SinCos, 8);

struct Bgn3LineRecord {
    gptr<cXyz> mpPositions;
    u8 _04[12];
};
WWHD_SIZE(Bgn3LineRecord, 0x10);
inline cXyz* bgn3LinePositions(Bgn3LineMat0* line, int index) {
    return gabi::at<Bgn3LineRecord>(line->mpLines)[index].mpPositions;
}
enum Bgn3ResourceIndex {
    dRes_INDEX_BGN_BDL_BGN_HEAD1_e = 21,
    dRes_INDEX_BGN_BCK_BGN_HEAD1_e = 6,
    dRes_INDEX_BGN_BDL_BGN_DEKU1_e = 20,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENA3_e = 24,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENB3_e = 27,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENC3_e = 30,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENB3_e = 39,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENC3_e = 42,
    dRes_INDEX_BGN_BTI_NOT_CUT1_e = 46,
};

// HD emitter fields used by the worm's arrow-hit effects.
struct Bgn3EmitterView {
    u8 _000[0x5C];
    be<s32> mMaxFrame;
    u8 _060[0x220 - 0x60];
    cXyz mGlobalScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    u8 _244[0x10];
    be<u32> mFlags;
    u8 _258[0xA];
    be<u8> mType;
    u8 _263;
};
WWHD_SIZE(Bgn3EmitterView, 0x264);
WWHD_OFFSET(Bgn3EmitterView, mGlobalScale, 0x220);
WWHD_OFFSET(Bgn3EmitterView, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Bgn3EmitterView, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Bgn3EmitterView, mFlags, 0x254);
WWHD_OFFSET(Bgn3EmitterView, mType, 0x262);

// HD entry bindings retain the verified full-width argument and return registers.
namespace Bgn3Bindings {
inline s32 cBgS_LineCross(u32 a0, u32 a1) { return gabi::call<s32>(0x02008860, a0, a1); }
inline void cBgS_LinChk_dt(u32 a0, u32 a1) { return gabi::call<void>(0x02008B4C, a0, a1); }
inline void cBgS_LinChk_ct(u32 a0) { return gabi::call<void>(0x02008FEC, a0); }
inline void cCcD_Stts_ct(u32 a0) { return gabi::call<void>(0x0200BD2C, a0); }
inline void cCcS_Set(u32 a0, u32 a1) { return gabi::call<void>(0x0200E240, a0, a1); }
inline u32 cM_rad2s(f32 a0) { return gabi::call<u32>(0x02019510, a0); }
inline void cXyz_add(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x0201AD78, a0, a1, a2); }
inline void cXyz_subtract(u32 a0, u32 a1, u32 a2) {
    return gabi::call<void>(0x0201ADE0, a0, a1, a2);
}
inline void mDoExt_J3DModelPacketS_setMaterial(u32 a0, u32 a1) {
    return gabi::call<void>(0x0207FD38, a0, a1);
}
inline void mDoExt_J3DModelPacketS_ct(u32 a0) { return gabi::call<void>(0x02080404, a0); }
inline void mDoExt_J3DModelPacketS_dt(u32 a0, u32 a1) {
    return gabi::call<void>(0x02082DDC, a0, a1);
}
inline s32 dBgS_GetAttributeCode(u32 a0, u32 a1) { return gabi::call<s32>(0x024EF0F4, a0, a1); }
inline void dBgS_LinChk_Set(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x024F1AFC, a0, a1, a2, a3);
}
inline u32 dCcD_GetGObjInf(u32 a0) { return gabi::call<u32>(0x025157AC, a0); }
inline void dCcD_Stts_dt(u32 a0, u32 a1) { return gabi::call<void>(0x02515860, a0, a1); }
inline void dCcD_Sph_dt(u32 a0, u32 a1) { return gabi::call<void>(0x02515AE8, a0, a1); }
inline void dCcD_GStts_ct(u32 a0) { return gabi::call<void>(0x02515DA0, a0); }
inline void dCcD_Stts_Init(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x02515F14, a0, a1, a2, a3);
}
inline s32 dCcD_GObjInf_ChkTgHit(u32 a0) { return gabi::call<s32>(0x025162A4, a0); }
inline u32 dCcD_GObjInf_GetTgHitObj(u32 a0) { return gabi::call<u32>(0x02516300, a0); }
inline void dCcD_Sph_ct(u32 a0) { return gabi::call<void>(0x025166F0, a0); }
inline void dCcD_Sph_Set(u32 a0, u32 a1) { return gabi::call<void>(0x0251677C, a0, a1); }
inline void def_se_set(u32 a0, u32 a1, u32 a2) { return gabi::call<void>(0x02518CC8, a0, a1, a2); }
inline u32 at_power_check(u32 a0) { return gabi::call<u32>(0x02518DB0, a0); }
inline s32 dComIfG_resLoad(u32 a0, u32 a1) { return gabi::call<s32>(0x02520460, a0, a1); }
inline void dKy_SordFlush_set(u32 a0, u32 a1) { return gabi::call<void>(0x0255F554, a0, a1); }
inline u32 dPa_control_c_set(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8,
                             u32 a9, u32 a10, u32 a11) {
    return gabi::call<u32>(0x025A847C, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}
inline void dPa_control_c_setSimple(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6) {
    return gabi::call<void>(0x025A8D40, a0, a1, a2, a3, a4, a5, a6);
}
inline void dVibration_c_StartShock(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x025CB374, a0, a1, a2, a3);
}
inline void fopAc_ac_c_ct(u32 a0) { return gabi::call<void>(0x025D4ED0, a0); }
inline void fopAc_ac_c_dt(u32 a0, u32 a1) { return gabi::call<void>(0x025D50BC, a0, a1); }
inline u32 fopAcM_create(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {
    return gabi::call<u32>(0x025D5834, a0, a1, a2, a3, a4, a5, a6, a7);
}
inline s32 fopAcM_entrySolidHeap(u32 a0, u32 a1, u32 a2) {
    return gabi::call<s32>(0x025D63E8, a0, a1, a2);
}
inline s16 fopAcM_searchActorAngleY(u32 a0, u32 a1) { return gabi::call<s16>(0x025D6894, a0, a1); }
inline u32 fpcM_Search(u32 a0, u32 a1) { return gabi::call<u32>(0x025DE508, a0, a1); }
inline void mDoAud_bgmStop(u32 a0) { return gabi::call<void>(0x025E1904, a0); }
inline void mDoAud_seStart(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x025E1A40, a0, a1, a2, a3);
}
inline void mDoAud_monsSeStart(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x025E1A7C, a0, a1, a2, a3);
}
inline void mDoAud_monsSeStartAtActor(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    return gabi::call<void>(0x025E1AA4, a0, a1, a2, a3, a4);
}
inline u32 mDoExt_J3DModel_create(u32 a0, u32 a1, u32 a2) {
    return gabi::call<u32>(0x025E38E0, a0, a1, a2);
}
inline u32 mDoExt_McaMorf_ct(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8,
                             u32 a9, u32 a10, u32 a11, f32 a12) {
    return gabi::call<u32>(0x025E4F64, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}
inline void mDoExt_McaMorf_play(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x025E535C, a0, a1, a2, a3);
}
inline void mDoExt_McaMorf_calc(u32 a0) { return gabi::call<void>(0x025E55A0, a0); }
inline void mDoExt_baseAnm_play(u32 a0) { return gabi::call<void>(0x025E742C, a0); }
inline u32 mDoExt_brkAnm_ct(u32 a0) { return gabi::call<u32>(0x025E80D0, a0); }
inline s32 mDoExt_brkAnm_init(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7,
                              u32 a8, f32 a9) {
    return gabi::call<s32>(0x025E8154, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
inline void mDoExt_3DlineMat0_ct(u32 a0) { return gabi::call<void>(0x025E9960, a0); }
inline void mDoExt_3DlineMat0_dt(u32 a0, u32 a1) { return gabi::call<void>(0x025E99E0, a0, a1); }
inline s32 mDoExt_3DlineMat0_init(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<s32>(0x025E9B80, a0, a1, a2, a3);
}
inline void mDoExt_3DlineMat0_update(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5) {
    return gabi::call<void>(0x025EA548, a0, a1, a2, a3, a4, a5);
}
inline void mDoExt_3DlineMat1_ct(u32 a0) { return gabi::call<void>(0x025EB82C, a0); }
inline void mDoExt_3DlineMat1_dt(u32 a0, u32 a1) { return gabi::call<void>(0x025EB8B8, a0, a1); }
inline s32 mDoExt_3DlineMat1_init(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    return gabi::call<s32>(0x025EBA58, a0, a1, a2, a3, a4);
}
inline void mDoExt_3DlineMatSortPacket_setMat(u32 a0, u32 a1) {
    return gabi::call<void>(0x025EDD04, a0, a1);
}
inline u32 mDoHIO_createChild(u32 a0, u32 a1) { return gabi::call<u32>(0x025F0A10, a0, a1); }
inline u32 operator_new(u32 a0) { return gabi::call<u32>(0x0273AD10, a0); }
inline void operator_delete(u32 a0) { return gabi::call<void>(0x0273AF40, a0); }
inline void PSVECSubtract(u32 a0, u32 a1, u32 a2) {
    return gabi::call<void>(0x028E8DAC, a0, a1, a2);
}
inline void std_allocator_ct(u32 a0) { return gabi::call<void>(0x028EAB2C, a0); }
inline void std_char_traits_ct(u32 a0) { return gabi::call<void>(0x028ED6F8, a0); }
inline void construct_array(u32 a0, u32 a1, u32 a2, u32 a3) {
    return gabi::call<void>(0x028EFFD0, a0, a1, a2, a3);
}
inline void destroy_array(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    return gabi::call<void>(0x028F0164, a0, a1, a2, a3, a4, a5);
}
inline void register_global_object(u32 a0) { return gabi::call<void>(0x028F026C, a0); }
inline f32 sqrtf(f32 a0) { return gabi::call<f32>(0x028F4384, a0); }
} // namespace Bgn3Bindings

// HD: the lighting constructor copies this 0x44-byte prefix to three light records.
struct Bgn3LightInit {
    be<f32> mPositionDirection[6];
    be<u8> mColorBytes[4];
    be<s16> mColorWords[4];
    be<f32> mParameters[8];
};
WWHD_SIZE(Bgn3LightInit, 0x44);
struct Bgn3TevCopyView {
    Bgn3LightInit mLight;
    u8 _044[0x40];
    cXyz mLightPosWorld;
    be<u16> mColorC0[4];
    be<u32> mColorK0;
    be<u32> mColorK1;
    be<u16> mFogColor[4];
    be<f32> mFogValues[3];
    be<u8> mEnvironment[9];
    u8 _0BD[3];
    Bgn3LightInit mLightCopy1;
    u8 _104[0x40];
    Bgn3LightInit mLightCopy2;
    u8 _188[0x40];
};
WWHD_SIZE(Bgn3TevCopyView, 0x1C8);
WWHD_OFFSET(Bgn3TevCopyView, mLightPosWorld, 0x84);
WWHD_OFFSET(Bgn3TevCopyView, mLightCopy1, 0xC0);
WWHD_OFFSET(Bgn3TevCopyView, mLightCopy2, 0x144);

struct Bgn3BossView {
    u8 _000[0x3D5];
    be<s8> m02B5;
    u8 _3D6[0x151CE - 0x3D6];
    be<u8> mCSMode;
    u8 _151CF;
};
WWHD_SIZE(Bgn3BossView, 0x151D0);
WWHD_OFFSET(Bgn3BossView, m02B5, 0x3D5);
WWHD_OFFSET(Bgn3BossView, mCSMode, 0x151CE);
struct Bgn3HeaderStatics {
    be<f32> mNegativePi, mPi;
    be<u8> mFlag, mByte;
    u8 _0A[2];
    be<u32> mZeroWords[4];
};
WWHD_SIZE(Bgn3HeaderStatics, 0x1C);
struct Bgn3PiConstants {
    be<f32> mNegativePi, mPi;
};
WWHD_SIZE(Bgn3PiConstants, 8);
inline dComIfG_inf_c* bgn3GameInfo(u32 address) { return gabi::at<dComIfG_inf_c>(address); }

struct Bgn3TimerView {
    be<s16> mTimers[5];
};
WWHD_SIZE(Bgn3TimerView, 10);
struct Bgn3RopeLineRecord {
    gptr<cXyz> mpPositions;
    gptr<be<u8>> mpSizes;
    u8 _08[0x10];
};
WWHD_SIZE(Bgn3RopeLineRecord, 0x18);
inline u32 bgn3LineMaterialID(Bgn3LineMat0* line) {
    u32 vtable = line->__vtbl;
    return gabi::call_ptr<u32>(gabi::load<u32>(vtable + 0x14), line);
}
