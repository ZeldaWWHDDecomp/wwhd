/* bgn_class (Puppet Ganon, phase 1: the marionette), WWHD layout.
 *
 * GameCube -> WWHD, measured from the inline constructor 0208064C, the deleting destructor
 * 0208ACD8, daBgn_Delete and useHeapInit:
 * - fopEn_enemy_c is followed by an HD word (the constructor stores `this` at 0x3C8), so mPhase
 *   is at 0x3CC and the members up to the first model packet are +0x120;
 * - mDoExt_J3DModelPacketS grew from 0x14 to 0xB0 (constructor 02080404) and dKy_tevstr_c from
 *   0xB0 to 0x1C8, so part_s is 0x3F0 (GameCube 0x23C);
 * - mDoExt_3DlineMat1_c grew from 0x3C to 0x188;
 * - HD inserts three more model packets after mC724 (0x14C84, 0x14D34, 0x14DE4).
 * Size 0x15768 (GameCube 0xCC94). */
#pragma once
#include "bindings.h"

/* mDoExt_J3DModelPacketS (HD 0xB0; constructor 02080404, destructor 02082DDC, vtable 0x10008DA8
 * at +0xC). HD: the model is at +0x98; +0x9C/+0xA0/+0xA4 optional colour/material sources read by
 * the HD update (0207AB2C); +0xA8 a lazily created 0xC-byte helper (0207FD38). */
struct mDoExt_J3DModelPacketS_l {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ be<u32> __vtbl;
    /* 0x10 */ u8 _10[0x90 - 0x10];
    /* 0x90 */ be<u32> m90;
    /* 0x94 */ u8 _94[4];
    /* 0x98 */ gptr<J3DModel> mpModel;
    /* 0x9C */ be<u32> m9C;
    /* 0xA0 */ be<u32> mA0;
    /* 0xA4 */ be<u32> mA4;
    /* 0xA8 */ be<u32> mpHelper;
    /* 0xAC */ be<u8> mAC;
    /* 0xAD */ be<u8> mAD;
    /* 0xAE */ be<u8> mAE;
    /* 0xAF */ be<u8> mAF;
    void setModel(J3DModel* m) { mpModel = m; }
};
WWHD_SIZE(mDoExt_J3DModelPacketS_l, 0xB0);

/* dKy_tevstr_c HD fields used here: the fog colour is GXColorS10-like (s16 r, g, b) */
struct dKy_tevstr_l {
    /* 0x000 */ u8 _000[0xA0];
    /* 0x0A0 */ be<s16> mFogColorR;
    /* 0x0A2 */ be<s16> mFogColorG;
    /* 0x0A4 */ be<s16> mFogColorB;
    /* 0x0A6 */ u8 _0A6[2];
    /* 0x0A8 */ be<f32> mFogStartZ;
    /* 0x0AC */ u8 _0AC[0x1C8 - 0xAC];
};
WWHD_SIZE(dKy_tevstr_l, 0x1C8);

struct part_s {
    /* 0x000 */ gptr<J3DModel> mpPartModel;
    /* 0x004 */ mDoExt_J3DModelPacketS_l m004;
    /* 0x0B4 */ dKy_tevstr_l mPartTevStr;
    /* 0x27C */ be<s16> m0C8;
    /* 0x27E */ u8 _27E[2];
    /* 0x280 */ be<f32> m0CC;
    /* 0x284 */ be<s16> mPartArrowHitFlashTimer;
    /* 0x286 */ be<s8> m0D2;
    /* 0x287 */ u8 _287[1];
    /* 0x288 */ cXyz m0D4;
    /* 0x294 */ csXyz m0E0;
    /* 0x29A */ u8 _29A[0x2A8 - 0x29A];
    /* 0x2A8 */ be<f32> m0F4;
    /* 0x2AC */ dCcD_Sph mPartSph;
    /* 0x3D8 */ cXyz m224;
    /* 0x3E4 */ gptr<JPABaseEmitter> mpPartArrowHitEmitter1;
    /* 0x3E8 */ gptr<JPABaseEmitter> mpPartArrowHitEmitter2;
    /* 0x3EC */ be<s16> mPartArrowHitEffectTimer;
    /* 0x3EE */ u8 _3EE[2];
};
WWHD_OFFSET(part_s, mPartTevStr, 0xB4);
WWHD_OFFSET(part_s, m0D4, 0x288);
WWHD_OFFSET(part_s, mPartSph, 0x2AC);
WWHD_SIZE(part_s, 0x3F0);

struct move_s {
    /* 0x000 */ cXyz mHimo[60];
    /* 0x2D0 */ be<u8> m2D0;
    /* 0x2D1 */ u8 _2D1[3];
    /* 0x2D4 */ cXyz m2D4;
    /* 0x2E0 */ csXyz m2E0;
    /* 0x2E6 */ u8 _2E6[2];
    /* 0x2E8 */ be<f32> m2E8;
    /* 0x2EC */ be<f32> m2EC;
    /* 0x2F0 */ u8 _2F0[4];
    /* 0x2F4 */ be<f32> m2F4;
    /* 0x2F8 */ be<s16> m2F8;
    /* 0x2FA */ be<s16> m2FA;
    /* 0x2FC */ be<s16> m2FC;
    /* 0x2FE */ be<s16> m2FE;
    /* 0x300 */ be<s16> m300;
    /* 0x302 */ u8 _302[2];
    /* 0x304 */ be<f32> m304;
    /* 0x308 */ be<s8> m308;
    /* 0x309 */ u8 _309[3];
};
WWHD_SIZE(move_s, 0x30C);

/* mDoExt_3DlineMat1_c (HD 0x188): line array pointer at +0x184 (line i: +0 positions, +4 sizes,
 * 0x?? per line) [as in d_a_bgn2.h] */
struct mDoExt_3DlineMat1_bgn {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines;
};
WWHD_SIZE(mDoExt_3DlineMat1_bgn, 0x188);

struct bgn_class : fopEn_enemy_c {
    /* 0x3C8 */ be<u32> m3C8;                       /* HD: `this` (stored by the constructor) */
    /* 0x3CC */ request_of_phase_process_class mPhase;
    /* 0x3D4 */ be<u8> m02B4;
    /* 0x3D5 */ be<s8> m02B5;
    /* 0x3D6 */ u8 _3D6[2];
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3DC */ gptr<J3DModel> mpChestModel;
    /* 0x3E0 */ mDoExt_J3DModelPacketS_l m02C0;
    /* 0x490 */ gptr<JPABaseEmitter> mpArrowHitEmitter1;
    /* 0x494 */ gptr<JPABaseEmitter> mpArrowHitEmitter2;
    /* 0x498 */ be<s16> mArrowHitEffectTimer;
    /* 0x49A */ u8 _49A[2];
    /* 0x49C */ gptr<J3DModel> mpJyakutenCModel;
    /* 0x4A0 */ gptr<J3DModel> mpJyakutenBModel;
    /* 0x4A4 */ gptr<J3DModel> mpJyakutenAModel;
    /* 0x4A8 */ gptr<mDoExt_brkAnm> mJyakutenCBrkAnm;
    /* 0x4AC */ gptr<mDoExt_brkAnm> mJyakutenBBrkAnm;
    /* 0x4B0 */ u8 _4B0[4];
    /* 0x4B4 */ be<s16> m02F8;
    /* 0x4B6 */ u8 _4B6[2];
    /* 0x4B8 */ be<f32> m02FC;
    /* 0x4BC */ be<s16> mArrowHitFlashTimer;
    /* 0x4BE */ be<s16> m0302;
    /* 0x4C0 */ be<s16> m0304;
    /* 0x4C2 */ u8 _4C2[2];
    /* 0x4C4 */ cXyz m0308;
    /* 0x4D0 */ part_s mHeadParts[2];
    /* 0xCB0 */ part_s mPelvisParts[2];
    /* 0x1490 */ part_s mLeftArmParts[21];
    /* 0x6740 */ part_s mRightArmParts[21];
    /* 0xB9F0 */ part_s mLeftLegParts[4];
    /* 0xC9B0 */ part_s mRightLegParts[4];
    /* 0xD970 */ part_s mTailParts[21];
    /* 0x12C20 */ move_s mAAA8[8];
    /* 0x14480 */ cXyz mC308;
    /* 0x1448C */ csXyz mC314;
    /* 0x14492 */ be<s16> mC31A;
    /* 0x14494 */ u8 _14494[8];
    /* 0x1449C */ be<f32> mC324[2];
    /* 0x144A4 */ be<f32> mC32C[2];
    /* 0x144AC */ be<f32> mC334;
    /* 0x144B0 */ be<f32> mC338;
    /* 0x144B4 */ cXyz mC33C[8];
    /* 0x14514 */ mDoExt_3DlineMat1_bgn mBlueRopeMat;
    /* 0x1469C */ mDoExt_3DlineMat1_bgn mRedRopeMat;
    /* 0x14824 */ mDoExt_3DlineMat1_bgn mDefeatCSRopeMat;
    /* 0x149AC */ cXyz mC450[60];
    /* 0x14C7C */ be<s8> mC720;
    /* 0x14C7D */ u8 _14C7D[3];
    /* 0x14C80 */ be<f32> mC724;
    /* 0x14C84 */ mDoExt_J3DModelPacketS_l mHdPacket[3]; /* HD */
    /* 0x14E94 */ cXyz mC728;
    /* 0x14EA0 */ cXyz mC734;
    /* 0x14EAC */ u8 _14EAC[4];
    /* 0x14EB0 */ be<s16> mC744;
    /* 0x14EB2 */ be<s16> mC746;
    /* 0x14EB4 */ be<s16> mC748;
    /* 0x14EB6 */ be<s16> mC74A;
    /* 0x14EB8 */ be<s16> mC74C;
    /* 0x14EBA */ be<s16> mC74E;
    /* 0x14EBC */ be<s16> mC750;
    /* 0x14EBE */ be<s16> mC752;
    /* 0x14EC0 */ be<s16> mC754;
    /* 0x14EC2 */ u8 _14EC2[2];
    /* 0x14EC4 */ cXyz mC758;
    /* 0x14ED0 */ be<s16> mC764;
    /* 0x14ED2 */ u8 _14ED2[6];
    /* 0x14ED8 */ be<f32> mC76C;
    /* 0x14EDC */ be<s16> mC770;
    /* 0x14EDE */ u8 _14EDE[2];
    /* 0x14EE0 */ be<f32> mC774;
    /* 0x14EE4 */ be<s8> mC778;
    /* 0x14EE5 */ be<s8> mC779;
    /* 0x14EE6 */ u8 _14EE6[2];
    /* 0x14EE8 */ cXyz mC77C;
    /* 0x14EF4 */ cXyz mC788;
    /* 0x14F00 */ cXyz mC794;
    /* 0x14F0C */ cXyz mC7A0;
    /* 0x14F18 */ be<s16> mC7AC[5];
    /* 0x14F22 */ be<s16> mC7B6;
    /* 0x14F24 */ be<s16> mC7B8;
    /* 0x14F26 */ u8 _14F26[2];
    /* 0x14F28 */ be<f32> mC7BC;
    /* 0x14F2C */ dCcD_Stts mStts;
    /* 0x14F68 */ dCcD_Sph mC7FC;
    /* 0x15094 */ dCcD_Sph mCoreSph;
    /* 0x151C0 */ cXyz mCA54;
    /* 0x151CC */ be<s16> mCA60;
    /* 0x151CE */ be<s8> mCSMode;
    /* 0x151CF */ u8 _151CF[1];
    /* 0x151D0 */ be<s16> mKSubCount;
    /* 0x151D2 */ u8 _151D2[2];
    /* 0x151D4 */ cXyz mCSCamEye;
    /* 0x151E0 */ cXyz mCA74;
    /* 0x151EC */ cXyz mCSCamCenter;
    /* 0x151F8 */ u8 _151F8[0xC];
    /* 0x15204 */ be<f32> mCA98;
    /* 0x15208 */ be<f32> mCA9C;
    /* 0x1520C */ be<f32> mCSFovY;
    /* 0x15210 */ gptr<J3DModel> mpWater0Model;
    /* 0x15214 */ gptr<J3DModel> mpWater1Model;
    /* 0x15218 */ dKy_tevstr_l mWaterTevStr;
    /* 0x153E0 */ gptr<J3DModel> mpRoomReflectionModel;
    /* 0x153E4 */ mDoExt_J3DModelPacketS_l mCB60;
    /* 0x15494 */ dKy_tevstr_l mRoomTevStr;
    /* 0x1565C */ mDoExt_J3DModelPacketS_l mCC24;
    /* 0x1570C */ be<s8> mCC38;
    /* 0x1570D */ u8 _1570D[3];
    /* 0x15710 */ u8 mPunchSmokeCb[2][0x20];      /* dPa_smokeEcallBack (HD 0x20) */
    /* 0x15750 */ be<s8> mKeeseSpawnNum;
    /* 0x15751 */ u8 _15751[3];
    /* 0x15754 */ be<f32> mCC80;
    /* 0x15758 */ be<f32> mCC84;
    /* 0x1575C */ be<f32> mCC88;
    /* 0x15760 */ gptr<JPABaseEmitter> mCC8C;
    /* 0x15764 */ be<s8> mCC90;
    /* 0x15765 */ be<u8> mCC91;
    /* 0x15766 */ u8 _15766[2];
};
WWHD_OFFSET(bgn_class, mPhase, 0x3CC);
WWHD_OFFSET(bgn_class, m0308, 0x4C4);
WWHD_OFFSET(bgn_class, mHeadParts, 0x4D0);
WWHD_OFFSET(bgn_class, mPelvisParts, 0xCB0);
WWHD_OFFSET(bgn_class, mLeftArmParts, 0x1490);
WWHD_OFFSET(bgn_class, mRightArmParts, 0x6740);
WWHD_OFFSET(bgn_class, mLeftLegParts, 0xB9F0);
WWHD_OFFSET(bgn_class, mRightLegParts, 0xC9B0);
WWHD_OFFSET(bgn_class, mTailParts, 0xD970);
WWHD_OFFSET(bgn_class, mAAA8, 0x12C20);
WWHD_OFFSET(bgn_class, mC308, 0x14480);
WWHD_OFFSET(bgn_class, mBlueRopeMat, 0x14514);
WWHD_OFFSET(bgn_class, mC450, 0x149AC);
WWHD_OFFSET(bgn_class, mHdPacket, 0x14C84);
WWHD_OFFSET(bgn_class, mC728, 0x14E94);
WWHD_OFFSET(bgn_class, mC77C, 0x14EE8);
WWHD_OFFSET(bgn_class, mStts, 0x14F2C);
WWHD_OFFSET(bgn_class, mCoreSph, 0x15094);
WWHD_OFFSET(bgn_class, mCA54, 0x151C0);
WWHD_OFFSET(bgn_class, mCA60, 0x151CC);
WWHD_OFFSET(bgn_class, mWaterTevStr, 0x15218);
WWHD_OFFSET(bgn_class, mCB60, 0x153E4);
WWHD_OFFSET(bgn_class, mCC24, 0x1565C);
WWHD_OFFSET(bgn_class, mPunchSmokeCb, 0x15710);
WWHD_OFFSET(bgn_class, mCC80, 0x15754);
WWHD_OFFSET(bgn_class, mCC91, 0x15765);
WWHD_SIZE(bgn_class, 0x15768);

/* daBgn_HIO_c (HD: GameCube offsets - 4, the vtable pointer is the last word 0x178); size 0x17C */
struct daBgn_HIO_c {
    /* 0x000 */ be<s16> m004;
    /* 0x002 */ be<s8> mNo;
    /* 0x003 */ u8 _003;
    /* 0x004 */ be<f32> m008;
    /* 0x008 */ be<u8> m00C;
    /* 0x009 */ be<u8> m00D;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ be<f32> m010;
    /* 0x010 */ be<s16> m014;
    /* 0x012 */ be<s16> m016;
    /* 0x014 */ be<s16> m018;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ be<f32> m01C;
    /* 0x01C */ be<f32> m020;
    /* 0x020 */ be<u8> m024, m025, m026, m027, m028, m029, m02A, m02B, m02C, m02D, m02E, m02F, m030;
    /* 0x02D */ u8 _02D[3];
    /* 0x030 */ be<f32> m034;
    /* 0x034 */ be<f32> m038;
    /* 0x038 */ be<s16> m03C, m03E, m040, m042, m044;
    /* 0x042 */ u8 _042[2];
    /* 0x044 */ be<f32> m048, m04C, m050, m054, m058, m05C, m060, m064, m068, m06C, m070, m074, m078, m07C, m080,
        m084, m088, m08C, m090, m094, m098, m09C, m0A0, m0A4;
    /* 0x0A4 */ be<s16> m0A8, m0AA, m0AC, m0AE, m0B0, m0B2, m0B4, m0B6, m0B8, m0BA, m0BC, m0BE, m0C0, m0C2, m0C4,
        m0C6, m0C8, m0CA, m0CC, m0CE, m0D0, m0D2;
    /* 0x0D0 */ be<f32> m0D4;
    /* 0x0D4 */ be<s16> m0D8, m0DA, m0DC, m0DE, m0E0, m0E2, m0E4, m0E6, m0E8, m0EA, m0EC, m0EE, m0F0, m0F2;
    /* 0x0F0 */ be<f32> m0F4, m0F8, m0FC, m100, m104, m108, m10C, m110, m114, m118, m11C, m120, m124, m128, m12C,
        m130, m134, m138, m13C, m140, m144, m148, m14C, m150, m154, m158, m15C, m160, m164, m168, m16C, m170;
    /* 0x170 */ be<s16> mKeeseNum3HP, mKeeseNum2HP, mKeeseNum1HP, mKeeseNumMax;
    /* 0x178 */ be<u32> __vtbl;
};
WWHD_OFFSET(daBgn_HIO_c, m0D4, 0xD0);
WWHD_OFFSET(daBgn_HIO_c, m0F4, 0xF0);
WWHD_OFFSET(daBgn_HIO_c, mKeeseNum3HP, 0x170);
WWHD_SIZE(daBgn_HIO_c, 0x17C);

/* ---- this TU's statics ---- */
inline daBgn_HIO_c& l_HIO() { return *gabi::at<daBgn_HIO_c>(0x10461CDC); }
inline be<u32>& bgn_g() { return *gabi::at<be<u32>>(0x10461A34); }       /* static bgn_class* bgn */
inline be<s32>& ki_all_count() { return *gabi::at<be<s32>>(0x10461A48); }
inline be<u8>& hio_set() { return *gabi::at<be<u8>>(0x10190F40); }
inline be<u32>& bgn2_g() { return *gabi::at<be<u32>>(0x10461A38); }      /* static bgn2_class* bgn2 */
inline be<u32>& bgn3_g() { return *gabi::at<be<u32>>(0x10461A3C); }      /* static bgn3_class* bgn3 */
inline be<s32>& BGN_HAND_MAX() { return *gabi::at<be<s32>>(0x10461A40); }
inline be<s32>& BGN_TAIL_MAX() { return *gabi::at<be<s32>>(0x10461A44); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
WWHD_OPAQUE(J3DDrawBuffer);
WWHD_OPAQUE(J3DPacket);
/* j3dSys (HD 0x104B45C0): view matrix at +0x38 */
inline Mtx34* j3dSys_viewMtx() { return gabi::at<Mtx34>(0x104B45F8); }
/* 028E9108 PSMTXConcat(a, b, ab) */
inline void PSMTXConcat(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
/* dComIfGd_getOpaListSky(): play+0x5D4C (HD) */
inline J3DDrawBuffer* dComIfGd_getOpaListSky() { return gabi::at<J3DDrawBuffer>(gabi::load<u32>(dComIfGp_ea() + 0x5D4C)); }
/* 027F0E04 J3DDrawBuffer::entryImm(J3DPacket*, u16) */
inline s32 J3DDrawBuffer_entryImm(J3DDrawBuffer* b, void* pkt, u16 idx) { return gabi::call<s32>(0x027F0E04, b, pkt, idx); }
/* dPa_smokeEcallBack::remove(): virtual (vtable at +0, slot +0x44) */
inline void smoke_remove(void* cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb); }

/* cM_rad2s 02019510 (cM_fsin inline = cM_ssin(cM_rad2s(x))) [as in d_a_bgn2] */
inline s16 bgn_cM_rad2s(f32 x) { return gabi::call<s16>(0x02019510, x); }
/* 025F18EC mDoMtx_XrotS */
inline void cMtx_XrotS(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
/* 028E8DAC PSVECSubtract(a, b, a-b) */
inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }
/* 024EF0F4 dBgS::GetAttributeCode(cBgS_PolyInfo&) */
inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* polyInfo) { return gabi::call<s32>(0x024EF0F4, bgs, polyInfo); }
/* f32 -> u32 (GHS: values not below 2^31, and NaN, are converted with the top bit added back) [as in d_a_bgn2] */
inline u32 f2u(f32 f) {
    if (f < 2147483648.0f)
        return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}
/* dComIfGp_particle_setToon: dPa_control_c::set with group 2 */
inline JPABaseEmitter* dComIfGp_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                 void* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, setup, nullptr, nullptr, nullptr);
}
/* bg_tevstr (static dKy_tevstr_c, 0x10461E58): colour C0 (s16 r, g, b at +0x90), K0 (u8 at +0x98) */
inline u32 bg_tevstr() { return 0x10461E58; }
/* dBgS_LinChk on the stack (HD layout as in d_a_bgn2; this TU's vtables) */
struct dBgS_LinChk_bgn {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x30 - 0x24];
    /* 0x30 */ cXyz mCross;
    /* 0x3C */ u8 _3C[0x58 - 0x3C];
    /* 0x58 */ be<u32> __vtbl_58;     /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;     /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_bgn, 0x6C);

/* dCcD_GObjInf::OnAtSetBit / OffAtSetBit (inline): bit 0 of the At SPrm word */
inline void OnAtSetBit(dCcD_GObjInf* o) { o->mObjAt.mSPrm |= 1u; }
inline void OffAtSetBit(dCcD_GObjInf* o) { o->mObjAt.mSPrm &= ~1u; }
/* dComIfGd_setListSky (HD): the sky opa/xlu lists (play+0x5D4C/0x5D50) */
inline void dComIfGd_setListSky() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D4C));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D50));
}
/* HD J3DMaterial: material i of a model's data (+0xAC) is *(*(data+0x10) + 4i); its pixel-engine block
 * is self-relative at material+0x20 (0: none) */
inline u32 bgn_material0(J3DModel* model) {
    u32 md = gabi::load<u32>(gabi::ea(model) + 0xAC);
    return gabi::load<u32>(gabi::load<u32>(md + 0x10));
}
inline u32 bgn_material_pe(u32 mat) {
    s32 off = gabi::load<s32>(mat + 0x20);
    return off != 0 ? mat + 0x20 + off : 0;
}

/* 025CB374 dVibration_c::StartShock(int, int, cXyz) (cXyz by value) */
inline BOOL StartShock(dVibration_c* vib, s32 strength, s32 flags, cXyz* pos) { return gabi::call<BOOL>(0x025CB374, vib, strength, flags, pos); }
/* 025E18EC mDoAud_bgmStart(id) */
inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
/* HD J3D: a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices);
 * getAnmMtx marks them dirty [as in d_a_bgn2] */
inline Mtx34* model_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
/* 028249B0 JPASetRMtxTVecfromMtx(mtx, rot, trans): JPABaseEmitter::setGlobalRTMatrix (HD inline) */
inline void JPABaseEmitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    u32 b = gabi::ea(e);
    gabi::call(0x028249B0, m, gabi::at<u8>(b + 0x1F0), gabi::at<u8>(b + 0x22C));
}
/* JPABaseEmitter::becomeInvalidEmitter (HD inline): mMaxFrame (+0x5C) = -1, flags (+0x254) |= 1 */
inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 b = gabi::ea(e);
    u32 f = gabi::load<u32>(b + 0x254);
    gabi::store<s32>(b + 0x5C, -1);
    gabi::store<u32>(b + 0x254, f | 1);
}
/* JPABaseEmitter::setGlobalScale (HD inline): +0x220 and +0x238 [as in d_a_bgn2] */
inline void JPABaseEmitter_setGlobalScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 b = gabi::ea(e);
    gabi::store<f32>(b + 0x220, x);
    gabi::store<f32>(b + 0x224, y);
    gabi::store<f32>(b + 0x228, z);
    gabi::store<f32>(b + 0x238, x);
    gabi::store<f32>(b + 0x23C, y);
    gabi::store<f32>(b + 0x240, z);
}
/* JPABaseEmitter::setGlobalTranslation (HD inline): +0x22C; y negated for version (+0x262) >= 7 */
inline void JPABaseEmitter_setGlobalTranslation(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 b = gabi::ea(e);
    if (gabi::load<u8>(b + 0x262) >= 7)
        y = -y;
    gabi::store<f32>(b + 0x22C, x);
    gabi::store<f32>(b + 0x230, y);
    gabi::store<f32>(b + 0x234, z);
}

#define BGN_SAFESTRING_VTBL 0x10008A1C /* this TU's sead::SafeString vtable */
#define BGN_ARC STR(0x10008C4C)         /* "Bgn" (useHeapInit) */
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0, init 025E8154 [as in d_a_bgn2] */
inline mDoExt_brkAnm* mDoExt_brkAnm_ct(mDoExt_brkAnm* p) { return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p); }
inline BOOL mDoExt_brkAnm_init(mDoExt_brkAnm* a, J3DModelData* d, void* k, s32 play, s32 mode, f32 speed, s16 start, s16 end,
                               s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, speed, start, end, modify, entry);
}
/* 025EBA58 mDoExt_3DlineMat1_c::init(u16 numLines, u16 numSegments, ResTIMG*, int) */
inline bool mDoExt_3DlineMat1_init(void* m, u16 lines, u16 segs, void* img, s32 p) { return gabi::call<bool>(0x025EBA58, m, lines, segs, img, p); }

/* __construct_array / __destroy_arr (GHS runtime) */
inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 f7, u32 f8) { gabi::call(0x028F0164, p, n, size, dtor, f7, f8); }
/* mDoExt_3DlineMat1_c: constructor 025EB82C, destructor 025EB8B8 [as in d_a_bridge] */
inline void mDoExt_3DlineMat1_ct(void* l) { gabi::call(0x025EB82C, l); }
inline void mDoExt_3DlineMat1_dt(void* l, s32 flags) { gabi::call(0x025EB8B8, l, flags); }
/* dCcD_Sph: constructor 025166F0, destructor 02515AE8 */
inline void dCcD_Sph_ct(dCcD_Sph* s) { gabi::call(0x025166F0, s); }
inline void dCcD_Sph_dt(dCcD_Sph* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }
/* dKy_tevstr_c inline constructor (HD): three 0x44-byte light blocks (+0x0, +0xC0, +0x144) start as
 * copies of the template at 0x1016E414 (floats +0x0..+0x14 and +0x24..+0x40, bytes +0x18..+0x1B,
 * halves +0x1C..+0x22) */
struct tevstr_tmpl {
    f32 f0[6];
    u8 b[4];
    s16 h[4];
    f32 f1[8];
    void load() {
        u32 s = 0x1016E414;
        for (int i = 0; i < 6; i++) f0[i] = gabi::load<f32>(s + 4 * i);
        for (int i = 0; i < 4; i++) b[i] = gabi::load<u8>(s + 0x18 + i);
        for (int i = 0; i < 4; i++) h[i] = gabi::load<s16>(s + 0x1C + 2 * i);
        for (int i = 0; i < 8; i++) f1[i] = gabi::load<f32>(s + 0x24 + 4 * i);
    }
    void store(u32 t) const {
        for (u32 k : {0x0u, 0xC0u, 0x144u}) {
            for (int i = 0; i < 6; i++) gabi::store<f32>(t + k + 4 * i, f0[i]);
            for (int i = 0; i < 4; i++) gabi::store<u8>(t + k + 0x18 + i, b[i]);
            for (int i = 0; i < 4; i++) gabi::store<s16>(t + k + 0x1C + 2 * i, h[i]);
            for (int i = 0; i < 8; i++) gabi::store<f32>(t + k + 0x24 + 4 * i, f1[i]);
        }
    }
};

/* ---- functions of the unit (natural calls between parts) ---- */
void bgn_mtx_copy(Mtx34* dst, Mtx34* src);
void bgn_colorS10_to_f(be<f32>* dst, be<s16>* src);
void bgn_color_to_f(be<f32>* dst, be<u8>* src);
void mDoExt_J3DModelPacketS_update(mDoExt_J3DModelPacketS_l* p);
void mDoExt_J3DModelPacketS_setup(mDoExt_J3DModelPacketS_l* p, u32 heap);
mDoExt_J3DModelPacketS_l* mDoExt_J3DModelPacketS_ct(mDoExt_J3DModelPacketS_l* p);
void part_draw(bgn_class* i_this, part_s* param_2);
s32 ki_check(bgn_class* i_this);
void SafeString_assureTermination(SafeString* s);
void mDoExt_J3DModelPacketS_setMaterial(mDoExt_J3DModelPacketS_l* p, u32 st);
void mDoExt_J3DModelPacketS_dt(mDoExt_J3DModelPacketS_l* p, s32 flags);
s32 part_init(part_s* param_1, J3DModelData* param_2);
s32 gr_check(bgn_class* i_this, cXyz* param_2);
void move_se_set(bgn_class* i_this);
void attack_eff_set(bgn_class* i_this, cXyz* param_2, int param_3);
void part_control_0(bgn_class* i_this, int param_2, part_s* param_3, move_s* param_4, f32 param_5);
void part_control_2(bgn_class* i_this, int param_2, part_s* param_3, f32 param_4);
void water1_disp(bgn_class* i_this);
