/* bgn2_class (Puppet Ganon, phase 2: the spider), WWHD layout.
 *
 * GameCube -> WWHD: fopEn_enemy_c +0x11C; the three mDoExt_J3DModelPacketS grew from 0x14 to 0xB0
 * (constructor 02080404), so the members after them are +0x2F0; mDoExt_3DlineMat1_c grew from
 * 0x3C to 0x188 (+0x14C more), and HD adds a fourth model packet (0xB0) after m2EC4, so the
 * emitter pointers and the background check are +0x4EC. Size 0x35C8 (GameCube 0x30DC).
 * Measured from daBgn2_Create (inline constructors), the deleting destructor 0208FAE8,
 * useHeapInit and daBgn2_Execute. */
#pragma once
#include "bindings.h"

/* mDoExt_3DlineMat1_c (HD 0x188): vtable at +0x130, line array pointer at +0x184
 * (line 0: +0 positions, +4 sizes) [as in d_a_bmdhand.h] */
struct mDoExt_3DlineMat1_l {
    /* 0x000 */ u8 _000[0x130];
    /* 0x130 */ be<u32> __vtbl;
    /* 0x134 */ u8 _134[0x184 - 0x134];
    /* 0x184 */ be<u32> mpLines;
};
WWHD_SIZE(mDoExt_3DlineMat1_l, 0x188);

struct bgn2_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpHeadMorf;
    /* 0x3D4 */ u8 m02B8[0xB0];                 /* mDoExt_J3DModelPacketS (HD 0xB0) */
    /* 0x484 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x488 */ u8 m02D0[0xB0];                 /* mDoExt_J3DModelPacketS */
    /* 0x538 */ u8 m02E4[0xB0];                 /* mDoExt_J3DModelPacketS */
    /* 0x5E8 */ gptr<J3DModel> mpJyakutenModel[3];
    /* 0x5F4 */ gptr<mDoExt_brkAnm> mJyakutenCBrkAnm;
    /* 0x5F8 */ gptr<mDoExt_brkAnm> mJyakutenBBrkAnm;
    /* 0x5FC */ u8 _5FC[4];
    /* 0x600 */ be<s16> m0310;                  /* frame counter */
    /* 0x602 */ be<s16> m0312;                  /* action */
    /* 0x604 */ be<s16> m0314;                  /* action step */
    /* 0x606 */ u8 _606[2];
    /* 0x608 */ cXyz m0318;                     /* target (player position) */
    /* 0x614 */ u8 _614[0xC];
    /* 0x620 */ be<s16> m0330[5];               /* timers */
    /* 0x62A */ be<s16> m033A;                  /* damage cooldown */
    /* 0x62C */ be<s16> m033C;                  /* guard SE cooldown */
    /* 0x62E */ u8 _62E[2];
    /* 0x630 */ cXyz m0340;                     /* movement target */
    /* 0x63C */ cXyz m034C;                     /* previous movement target */
    /* 0x648 */ be<s16> m0358;                  /* head shake */
    /* 0x64A */ u8 _64A[6];
    /* 0x650 */ dCcD_Stts mStts;
    /* 0x68C */ dCcD_Sph m039C;                 /* head */
    /* 0x7B8 */ dCcD_Sph m04C8[2];              /* body */
    /* 0xA10 */ dCcD_Sph m0720[30];             /* legs */
    /* 0x2D38 */ dCcD_Sph m2A48;                /* weak point (light arrows) */
    /* 0x2E64 */ cXyz m2B74;
    /* 0x2E70 */ cXyz m2B80[2];
    /* 0x2E88 */ cXyz m2B98[30];
    /* 0x2FF0 */ be<s16> mArrowHitEffectTimer[32];
    /* 0x3030 */ u8 m2D40[0x20];
    /* 0x3050 */ be<s16> m2D60;
    /* 0x3052 */ u8 _3052[2];
    /* 0x3054 */ be<f32> m2D64;
    /* 0x3058 */ be<s16> mArrowHitFlashTimer;
    /* 0x305A */ be<s16> m2D6A;
    /* 0x305C */ gptr<JPABaseEmitter> mpArrowHitEmitter1[32];
    /* 0x30DC */ gptr<JPABaseEmitter> mpArrowHitEmitter2[32];
    /* 0x315C */ cXyz m2E6C;                    /* weak point position */
    /* 0x3168 */ be<s8> m2E78;                  /* body colliders attack */
    /* 0x3169 */ be<s8> m2E79;                  /* leg colliders attack */
    /* 0x316A */ u8 _316A[2];
    /* 0x316C */ be<f32> m2E7C;
    /* 0x3170 */ be<s16> m2E80;                 /* spin speed */
    /* 0x3172 */ be<s16> m2E82;                 /* spin speed target */
    /* 0x3174 */ u8 _3174[4];
    /* 0x3178 */ mDoExt_3DlineMat1_l mRedRopeMat;
    /* 0x3300 */ be<f32> m2EC4;                 /* rope sway */
    /* 0x3304 */ u8 m3304[0xB0];                /* HD: a fourth mDoExt_J3DModelPacketS */
    /* 0x33B4 */ gptr<JPABaseEmitter> m2EC8[2];
    /* 0x33BC */ be<s8> m2ED0;                  /* Keese spawns left */
    /* 0x33BD */ u8 _33BD;
    /* 0x33BE */ be<s16> m2ED2;                 /* Keese spawn cooldown */
    /* 0x33C0 */ dBgS_AcchCir mAcchCir;
    /* 0x3400 */ dBgS_ObjAcch mAcch;
    /* 0x35C4 */ be<u8> m30D8;                  /* owns the HIO child */
    /* 0x35C5 */ u8 _35C5[3];
};
WWHD_OFFSET(bgn2_class, mpHeadMorf, 0x3D0);
WWHD_OFFSET(bgn2_class, mpBodyMorf, 0x484);
WWHD_OFFSET(bgn2_class, mpJyakutenModel, 0x5E8);
WWHD_OFFSET(bgn2_class, m0310, 0x600);
WWHD_OFFSET(bgn2_class, m0330, 0x620);
WWHD_OFFSET(bgn2_class, m0340, 0x630);
WWHD_OFFSET(bgn2_class, mStts, 0x650);
WWHD_OFFSET(bgn2_class, m039C, 0x68C);
WWHD_OFFSET(bgn2_class, m0720, 0xA10);
WWHD_OFFSET(bgn2_class, m2A48, 0x2D38);
WWHD_OFFSET(bgn2_class, m2B98, 0x2E88);
WWHD_OFFSET(bgn2_class, mArrowHitEffectTimer, 0x2FF0);
WWHD_OFFSET(bgn2_class, m2D6A, 0x305A);
WWHD_OFFSET(bgn2_class, m2E6C, 0x315C);
WWHD_OFFSET(bgn2_class, mRedRopeMat, 0x3178);
WWHD_OFFSET(bgn2_class, m2EC4, 0x3300);
WWHD_OFFSET(bgn2_class, m2EC8, 0x33B4);
WWHD_OFFSET(bgn2_class, mAcchCir, 0x33C0);
WWHD_OFFSET(bgn2_class, mAcch, 0x3400);
WWHD_OFFSET(bgn2_class, m30D8, 0x35C4);
WWHD_SIZE(bgn2_class, 0x35C8);

/* daBgn2_HIO_c, HD: the vtable pointer is the last word (0x30); size 0x34 */
struct daBgn2_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<u8> m06;
    /* 0x03 */ u8 _03;
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<s16> m18;
    /* 0x16 */ be<s16> m1A;
    /* 0x18 */ be<s16> m1C;
    /* 0x1A */ be<s16> m1E;
    /* 0x1C */ be<s16> m20;
    /* 0x1E */ be<s16> m22;
    /* 0x20 */ be<s16> m24;
    /* 0x22 */ be<s16> m26;
    /* 0x24 */ be<s16> m28;
    /* 0x26 */ be<s16> m2A;
    /* 0x28 */ be<s16> m2C;
    /* 0x2A */ be<s16> m2E;
    /* 0x2C */ be<s16> m30;
    /* 0x2E */ u8 _2E[2];
    /* 0x30 */ be<u32> __vtbl;
};
WWHD_SIZE(daBgn2_HIO_c, 0x34);

/* the other parts of Puppet Ganon, as far as phase 2 touches them (HD offsets) */
inline s8 bgn_m02B5(void* bgn) { return gabi::load<s8>(gabi::ea(bgn) + 0x3D5); }       /* GameCube 0x2B5 */
inline u32 bgn_mCA60(void* bgn) { return gabi::ea(bgn) + 0x151CC; }                       /* s16, GameCube 0xCA60 */
inline u32 bgn_mCSMode(void* bgn) { return gabi::ea(bgn) + 0x151CE; }                     /* s8, GameCube 0xCA62 */
inline u32 bgn_mCC90(void* bgn) { return gabi::ea(bgn) + 0x15764; }                       /* s8, GameCube 0xCC90 */
inline u32 bgn3_m10060(void* bgn3) { return gabi::ea(bgn3) + 0x1213C; }                  /* f32, GameCube 0x10060 */

/* ---- this TU's statics and .data ---- */
#define BGN2_SAFESTRING_VTBL 0x10008DDC
#define BGN2_VTBL 0x10008E94          /* bgn2_class vtable (HD virtual destructor) */
#define BGN2_HIO_VTBL 0x10008E84
inline daBgn2_HIO_c& l_HIO() { return *gabi::at<daBgn2_HIO_c>(0x10462054); }
inline be<u32>& bgn_g() { return *gabi::at<be<u32>>(0x10462020); }         /* static bgn_class* bgn */
inline be<u32>& bgn3_g() { return *gabi::at<be<u32>>(0x10462024); }        /* static bgn3_class* bgn3 */
inline be<s32>& ki_all_count() { return *gabi::at<be<s32>>(0x10462028); }
inline be<u8>& hio_set() { return *gabi::at<be<u8>>(0x10191040); }
inline cXyz* zero_l() { return gabi::at<cXyz>(0x10462048); }
inline dKy_tevstr_c* bg_tevstr() { return gabi::at<dKy_tevstr_c>(0x10462088); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dBgS_LinChk on the stack (HD layout as in d_a_mt.h; this TU's vtables) */
struct dBgS_LinChk_l {
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
WWHD_SIZE(dBgS_LinChk_l, 0x6C);
inline void bgn2_LinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x10008E44;
    c->__vtbl_64 = 0x10008E64;
    c->__vtbl_58 = 0x10008E74;
    c->__vtbl_20 = 0x10008E54;
}
inline void bgn2_LinChk_dt(dBgS_LinChk_l* c) {
    c->__vtbl_58 = 0x10008E74;
    c->__vtbl_64 = 0x10008E04;
    c->__vtbl_20 = 0x10008DF4;
    cBgS_LinChk_dt(c, 0);
}
/* 024EF0F4 dBgS::GetAttributeCode(cBgS_PolyInfo&) */
inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* polyInfo) { return gabi::call<s32>(0x024EF0F4, bgs, polyInfo); }

/* HD J3D: a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices);
 * getAnmMtx marks them dirty */
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
/* JPABaseEmitter::setGlobalScale (HD inline): +0x220 and +0x238 */
inline void JPABaseEmitter_setGlobalScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 b = gabi::ea(e);
    gabi::store<f32>(b + 0x220, x);
    gabi::store<f32>(b + 0x224, y);
    gabi::store<f32>(b + 0x228, z);
    gabi::store<f32>(b + 0x238, x);
    gabi::store<f32>(b + 0x23C, y);
    gabi::store<f32>(b + 0x240, z);
}
/* JPABaseEmitter::setGlobalTranslation (HD inline): +0x22C; y negated for version (+0x262) >= 7
 * [as in d_a_kita] */
inline void JPABaseEmitter_setGlobalTranslation(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 b = gabi::ea(e);
    if (gabi::load<u8>(b + 0x262) >= 7)
        y = -y;
    gabi::store<f32>(b + 0x22C, x);
    gabi::store<f32>(b + 0x230, y);
    gabi::store<f32>(b + 0x234, z);
}
/* HD: fopAcM_monsSeStart inline (the process id is read before the reverb call) [as in d_a_ki.h] */
inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 se, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 id = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, se, &a->eyePos, id, param, reverb); /* mDoAud_monsSeStart */
    }
}
/* 025E1A7C mDoAud_monsSeStart(se, pos, actorId, reverb) */
inline void mDoAud_monsSeStart(u32 se, cXyz* pos, u32 id, s32 reverb) { gabi::call(0x025E1A7C, se, pos, id, reverb); }
/* 025E1904 mDoAud_bgmStop(frames), 025E18EC mDoAud_bgmStart(id) */
inline void mDoAud_bgmStop_l(u32 frames) { gabi::call(0x025E1904, frames); }
inline void mDoAud_bgmStart_l(u32 id) { gabi::call(0x025E18EC, id); }
/* f32 -> u32 (GHS: values not below 2^31, and NaN, are converted with the top bit added back) */
inline u32 f2u(f32 f) {
    if (f < 2147483648.0f)
        return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}
/* cM_rad2s 02019510; cM_fsin (inline) = cM_ssin(cM_rad2s(x)) */
inline s16 cM_rad2s(f32 x) { return gabi::call<s16>(0x02019510, x); }
inline f32 cM_fsin(f32 x) { return cM_ssin((u16)cM_rad2s(x)); }
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0, init 025E8154 [as in d_a_mt.h] */
inline mDoExt_brkAnm* mDoExt_brkAnm_ct(mDoExt_brkAnm* p) { return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p); }
inline BOOL mDoExt_brkAnm_init(mDoExt_brkAnm* a, J3DModelData* d, J3DAnmTevRegKey* k, s32 play, s32 mode, f32 speed, s16 start, s16 end,
                               s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, speed, start, end, modify, entry);
}

/* ---- functions of the unit (natural calls between parts) ---- */
void anm_init(bgn2_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx);
int gr_check(bgn2_class* i_this, cXyz* pos);
void asi_hamon_set(bgn2_class* i_this);
int checkGround(bgn2_class* i_this);
void move_se_set(bgn2_class* i_this);
int pos_move(bgn2_class* i_this);
BOOL daBgn2_Execute(bgn2_class* i_this);
