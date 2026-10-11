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
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

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
    /* 0x3D4 */ u8 m02B8[0xB0]; /* mDoExt_J3DModelPacketS (HD 0xB0) */
    /* 0x484 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x488 */ u8 m02D0[0xB0]; /* mDoExt_J3DModelPacketS */
    /* 0x538 */ u8 m02E4[0xB0]; /* mDoExt_J3DModelPacketS */
    /* 0x5E8 */ gptr<J3DModel> mpJyakutenModel[3];
    /* 0x5F4 */ gptr<mDoExt_brkAnm> mJyakutenCBrkAnm;
    /* 0x5F8 */ gptr<mDoExt_brkAnm> mJyakutenBBrkAnm;
    /* 0x5FC */ u8 _5FC[4];
    /* 0x600 */ be<s16> m0310; /* frame counter */
    /* 0x602 */ be<s16> m0312; /* action */
    /* 0x604 */ be<s16> m0314; /* action step */
    /* 0x606 */ u8 _606[2];
    /* 0x608 */ cXyz m0318; /* target (player position) */
    /* 0x614 */ u8 _614[0xC];
    /* 0x620 */ be<s16> m0330[5]; /* timers */
    /* 0x62A */ be<s16> m033A;    /* damage cooldown */
    /* 0x62C */ be<s16> m033C;    /* guard SE cooldown */
    /* 0x62E */ u8 _62E[2];
    /* 0x630 */ cXyz m0340;    /* movement target */
    /* 0x63C */ cXyz m034C;    /* previous movement target */
    /* 0x648 */ be<s16> m0358; /* head shake */
    /* 0x64A */ u8 _64A[6];
    /* 0x650 */ dCcD_Stts mStts;
    /* 0x68C */ dCcD_Sph m039C;     /* head */
    /* 0x7B8 */ dCcD_Sph m04C8[2];  /* body */
    /* 0xA10 */ dCcD_Sph m0720[30]; /* legs */
    /* 0x2D38 */ dCcD_Sph m2A48;    /* weak point (light arrows) */
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
    /* 0x315C */ cXyz m2E6C;   /* weak point position */
    /* 0x3168 */ be<s8> m2E78; /* body colliders attack */
    /* 0x3169 */ be<s8> m2E79; /* leg colliders attack */
    /* 0x316A */ u8 _316A[2];
    /* 0x316C */ be<f32> m2E7C;
    /* 0x3170 */ be<s16> m2E80; /* spin speed */
    /* 0x3172 */ be<s16> m2E82; /* spin speed target */
    /* 0x3174 */ u8 _3174[4];
    /* 0x3178 */ mDoExt_3DlineMat1_l mRedRopeMat;
    /* 0x3300 */ be<f32> m2EC4;  /* rope sway */
    /* 0x3304 */ u8 m3304[0xB0]; /* HD: a fourth mDoExt_J3DModelPacketS */
    /* 0x33B4 */ gptr<JPABaseEmitter> m2EC8[2];
    /* 0x33BC */ be<s8> m2ED0; /* Keese spawns left */
    /* 0x33BD */ u8 _33BD;
    /* 0x33BE */ be<s16> m2ED2; /* Keese spawn cooldown */
    /* 0x33C0 */ dBgS_AcchCir mAcchCir;
    /* 0x3400 */ dBgS_ObjAcch mAcch;
    /* 0x35C4 */ be<u8> m30D8; /* owns the HIO child */
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

// HD: phase-one and phase-three fields used by the spider phase.
struct bgn2BgnView {
    u8 _000[0x3D5];
    be<s8> m02B5;
    u8 _3D6[0x151CC - 0x3D6];
    be<s16> mCA60;
    be<s8> mCSMode;
    u8 _151CF[0x15764 - 0x151CF];
    be<s8> mCC90;
};
WWHD_OFFSET(bgn2BgnView, m02B5, 0x3D5);
WWHD_OFFSET(bgn2BgnView, mCA60, 0x151CC);
WWHD_OFFSET(bgn2BgnView, mCSMode, 0x151CE);
WWHD_OFFSET(bgn2BgnView, mCC90, 0x15764);
WWHD_SIZE(bgn2BgnView, 0x15765);
struct bgn2Bgn3View : fopAc_ac_c {
    u8 _3AC[0x1213C - 0x3AC];
    be<f32> m10060;
};
WWHD_OFFSET(bgn2Bgn3View, m10060, 0x1213C);
WWHD_SIZE(bgn2Bgn3View, 0x12140);

/* ---- this TU's statics and .data ---- */
#define BGN2_SAFESTRING_VTBL 0x10008DDC
#define BGN2_VTBL 0x10008E94 /* bgn2_class vtable (HD virtual destructor) */
#define BGN2_HIO_VTBL 0x10008E84
extern daBgn2_HIO_c& l_HIO;
extern gptr<bgn2BgnView>& bgn;
extern gptr<bgn2Bgn3View>& bgn3;
extern be<s32>& ki_all_count;
extern be<u8>& hio_set;
extern cXyz& zero;
extern dKy_tevstr_c& bg_tevstr;

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
    /* 0x58 */ be<u32> __vtbl_58; /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64; /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);
inline void bgn2_LinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
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
inline s32 dBgS_GetAttributeCode(dBgS* bgs, void* polyInfo) {
    return gabi::call<s32>(0x024EF0F4, bgs, polyInfo);
}

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
inline void mDoAud_monsSeStart(u32 se, cXyz* pos, u32 id, s32 reverb) {
    gabi::call(0x025E1A7C, se, pos, id, reverb);
}
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
inline mDoExt_brkAnm* mDoExt_brkAnm_ct(mDoExt_brkAnm* p) {
    return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p);
}
inline BOOL mDoExt_brkAnm_init(mDoExt_brkAnm* a, J3DModelData* d, J3DAnmTevRegKey* k, s32 play,
                               s32 mode, f32 speed, s16 start, s16 end, s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, speed, start, end, modify, entry);
}

/* ---- functions of the unit (natural calls between parts) ---- */
void anm_init(bgn2_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed,
              int soundFileIdx);
int gr_check(bgn2_class* i_this, cXyz* pos);
void asi_hamon_set(bgn2_class* i_this);
int checkGround(bgn2_class* i_this);
void move_se_set(bgn2_class* i_this);
int pos_move(bgn2_class* i_this);
BOOL daBgn2_Execute(bgn2_class* i_this);

// HD: private runtime adapters preserve this translation unit's inline ABI.
/* 025EBA58 mDoExt_3DlineMat1_c::init(u16 numLines, u16 numSegs, ResTIMG*, BOOL hasSize) [as in
 * d_a_bmdhand] */
static inline BOOL mDoExt_3DlineMat1_init(mDoExt_3DlineMat1_l* l, u16 lines, u16 segs, void* tex,
                                          s32 hasSize) {
    return gabi::call<BOOL>(0x025EBA58, l, lines, segs, tex, hasSize);
}
/* 0207FD38 HD: model packet setup (mDoExt_J3DModelPacketS) [as in d_a_ki] */
static inline void bgn2_packet_init(void* pkt, u32 p) { gabi::call(0x0207FD38, pkt, p); }

/* dKy_tevstr_c (HD) assignment as the HD operator= copies it: three 0x44-byte light blocks
 * (+0x00, +0xC0, +0x144: six floats, four colour bytes, four shorts, eight floats) and the words,
 * shorts and bytes at +0x84..+0xBC; the rest is not copied */
static inline void tev_blk_copy(u32 d, u32 s) {
    for (u32 o = 0x00; o < 0x18; o += 4)
        gabi::store<f32>(d + o, gabi::load<f32>(s + o));
    for (u32 o = 0x18; o < 0x1C; o++)
        gabi::store<u8>(d + o, gabi::load<u8>(s + o));
    for (u32 o = 0x1C; o < 0x24; o += 2)
        gabi::store<u16>(d + o, gabi::load<u16>(s + o));
    for (u32 o = 0x24; o < 0x44; o += 4)
        gabi::store<f32>(d + o, gabi::load<f32>(s + o));
}
static inline void tevstr_assign(u32 d, u32 s) {
    tev_blk_copy(d, s);
    for (u32 o = 0x84; o < 0x90; o += 4)
        gabi::store<u32>(d + o, gabi::load<u32>(s + o));
    for (u32 o = 0x90; o < 0x98; o += 2)
        gabi::store<u16>(d + o, gabi::load<u16>(s + o));
    for (u32 o = 0x98; o < 0xA0; o += 4)
        gabi::store<u32>(d + o, gabi::load<u32>(s + o));
    for (u32 o = 0xA0; o < 0xA8; o += 2)
        gabi::store<u16>(d + o, gabi::load<u16>(s + o));
    for (u32 o = 0xA8; o < 0xB4; o += 4)
        gabi::store<f32>(d + o, gabi::load<f32>(s + o));
    for (u32 o = 0xB4; o < 0xBD; o++)
        gabi::store<u8>(d + o, gabi::load<u8>(s + o));
    tev_blk_copy(d + 0xC0, s + 0xC0);
    tev_blk_copy(d + 0x144, s + 0x144);
}

/* the other parts of Puppet Ganon (statics bgn, bgn3) */

/* fopAc_ac_c::attention_info (HD +0x388): distances[] +0, position +8, flags +0x14 */

/* dVibration_c::StartShock(REG0_S(2) + add, -0x21, cXyz(0, 1, 0)): the play object is fetched
 * before the debug register is read */
static inline void bgn2_StartShock(s32 add) {
    dVibration_c* vib = dComIfGp_getVibration();
    gabi::Local<cXyz> up;
    up->x = 0.0f;
    up->y = 1.0f;
    up->z = 0.0f;
    s32 strength = REG0_S(2) + add;
    gabi::call(0x025CB374, vib, strength, -0x21, up.get());
}

/* CcAtInfo (0x1C) */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_l, 0x1C);
/* 02518DB0 at_power_check(CcAtInfo*), 02518CC8 def_se_set(actor, hitObj, mtrl) */
static inline void at_power_check(CcAtInfo_l* info) { gabi::call(0x02518DB0, info); }
static inline void def_se_set(fopAc_ac_c* a, u32 obj, u32 mtrl) {
    gabi::call(0x02518CC8, a, obj, mtrl);
}
/* 0255F554 dKy_SordFlush_set(cXyz pos (by value: pointer to a copy), int) */
static inline void dKy_SordFlush_set(cXyz* pos, s32 p) { gabi::call(0x0255F554, pos, p); }

// HD: the angle and hit record share one guest stack frame (0x24 bytes).
struct frame_l {
    csXyz local_78;
    u8 _06[2];
    CcAtInfo_l atInfo;
};
WWHD_SIZE(frame_l, 0x24);

enum {
    fpcNm_BGN_e = 0xF3,
    fpcNm_BGN3_e = 0xF5,
    dBgS_Attr_WATER_e = 0x13,
    TEV_TYPE_BG2 = 3,
    ID_AK_SN_O_KGTCOMMONHAMON00 = 0x8407,
    JA_SE_CM_BGN_MECHA_ROTATE = 0x705A,
    JA_SE_CM_BGN_MECHA_ROPE = 0x705B,
    fpcNm_KI_e = 0xD7,
    ID_AK_SN_O_KGTT2JUMPHANDSPLASH00 = 0x840F,
    ID_AK_ST_O_KGTT2JUMPHANDSMOKE00 = 0xA410,
    ID_AK_SN_KGTT2CHESTSPLASH00 = 0x841B,
    ID_AK_SN_KGTT2STOMACHSPLASH00 = 0x841C,
    JA_SE_CM_BGN_T_FALL_WATER = 0x597A,
    JA_SE_CM_BGN_T_FALL = 0x597B,
    JA_SE_CM_BGN_T_FALL_WIND = 0x597D,
    ID_AK_SN_KGTT2CHESTPOTA00 = 0x8419,
    ID_AK_SN_KGTT2STOMACHPOTA00 = 0x841A,
    JA_SE_CM_BGN_T_JUMP_UP = 0x597C,
    JA_SE_CV_BGN_HIT_2 = 0x496F,
    JA_SE_CM_BGN_T_SLIDE = 0x597E,
    JA_SE_CM_BGN_T_TO_M_1 = 0x597F,
    ID_AK_JN_SIBOUBAKUEN = 0x13,
    ID_AK_JN_SIBOUFLASH = 0x16,
    JA_SE_CM_BGN_METAM_EXPLODE = 0x5980,
    JA_SE_CM_BGN_T_ROUND = 0x705C,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_JN_OK = 0xD,
    JA_SE_LK_ARROW_HIT = 0x2879,
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_CM_BGN_M_BRK_ORB = 0x5983,
    ID_AK_SN_KGTBREAKWEAKPOINT00 = 0x8457,
    ID_AK_SN_KGTBREAKWEAKPOINT01 = 0x8458,
    JA_SE_CV_BGN_HIT_1 = 0x496E,
    ID_AK_SN_KGTHITWEAKPOINT00 = 0x8459,
    ID_AK_JN_CCTHUNDER00 = 0x3ED,
    ID_AK_JN_CCTHUNDER01 = 0x3EE,
};

enum {
    dRes_INDEX_BGN_BCK_BGN_HEAD1_e = 0x6,
    dRes_INDEX_BGN_BCK_WAIT1_e = 0xF,
    dRes_INDEX_BGN_BDL_BGN_HEAD1_e = 0x15,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENA2_e = 0x17,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENB2_e = 0x1A,
    dRes_INDEX_BGN_BDL_BGN_JYAKUTENC2_e = 0x1D,
    dRes_INDEX_BGN_BDL_BGN_KUMO1_e = 0x1F,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENB2_e = 0x26,
    dRes_INDEX_BGN_BRK_BGN_JYAKUTENC2_e = 0x29,
    dRes_INDEX_BGN_BTI_NOT_CUT1_e = 0x2E,
};

enum {
    dRes_INDEX_BGN_BCK_DAMAGE1_e = 0x8,
    dRes_INDEX_BGN_BCK_DERU1_e = 0x9,
    dRes_INDEX_BGN_BCK_JUMP1_e = 0xA,
    dRes_INDEX_BGN_BCK_MODORU1_e = 0xB,
    dRes_INDEX_BGN_BCK_RAKKA1_e = 0xC,
    dRes_INDEX_BGN_BCK_RAKKA2_e = 0xD,
    dRes_INDEX_BGN_BCK_SETTI1_e = 0xE,
    dRes_INDEX_BGN_BCK_WAIT2_e = 0x10,
};

enum {
    BGN_KUMO1_JNT_J_BGN2_KARADA1_e = 2,
    BGN_KUMO1_JNT_ATAMA_e = 3,
    BGN_KUMO1_JNT_J_BGN2_ASHI_LB1_e = 4,
    BGN_KUMO1_JNT_J_BGN2_KARADA2_e = 34,
    BGN_KUMO1_JNT_JYAKUTEN_e = 35,
};
extern be<s32> (&body_d)[2];
extern be<f32> (&body_scale)[2];
extern be<f32> (&asi_scale)[5];
extern be<s16> (&fl_check_d)[32];

// HD: constructors and deleting destructors used by the compiler-generated actor code.

inline void bgn2_packetConstruct(void* p) { gabi::call(0x02080404, p); }
inline void bgn2_packetDestruct(void* p, s32 flags) { gabi::call(0x02082DDC, p, flags); }
inline void bgn2_sphereConstruct(dCcD_Sph* p) { gabi::call(0x025166F0, p); }
inline void bgn2_sphereDestruct(dCcD_Sph* p, s32 flags) { gabi::call(0x02515AE8, p, flags); }
inline void bgn2_sphereArrayConstruct(dCcD_Sph* p, s32 count) {
    gabi::call(0x028EFFD0, p, count, 0x12C, 0x025166F0);
}
inline void bgn2_sphereArrayDestruct(dCcD_Sph* p, s32 count) {
    gabi::call(0x028F0164, p, count, 0x12C, 0x02515AE8, 0, 0);
}
inline void bgn2_lineConstruct(mDoExt_3DlineMat1_l* p) { gabi::call(0x025EB82C, p); }
inline void bgn2_lineDestruct(mDoExt_3DlineMat1_l* p, s32 flags) {
    gabi::call(0x025EB8B8, p, flags);
}
inline void bgn2_actorDestruct(bgn2_class* p) { gabi::call(0x025D50BC, p, 0); }
inline void bgn2_acchDestruct(dBgS_ObjAcch* p) {
    gabi::store<u32>(gabi::ea(p) + 0x20, 0x10008E24);
    gabi::store<u32>(gabi::ea(p) + 0x14, 0x10008E34);
    gabi::call(0x024EFD9C, p, 0);
}
inline void bgn2_acchCirDestruct(dBgS_AcchCir* p) {
    gabi::call(0x02018034, gabi::at<u8>(gabi::ea(p) + 0x14), 2);
}

struct bgn2SphereSource {
    be<u32> mFlags;
    be<u32> mAtType;
    be<u8> mAtp;
    be<u8> mAtpPadding[3]; // HD: the checked source record includes the padding bytes.
    be<u32> mAtSPrm;
    be<u32> mTgType;
    be<u32> mTgSPrm;
    be<u32> mCoSPrm;
    be<u32> mGObjAtFlags;
    be<u32> mGObjAtSPrm;
    be<u32> mGObjTgFlags;
    be<u32> mGObjTgSPrm;
    be<u32> mGObjCoSPrm;
    be<f32> mCenter[3];
    be<f32> mRadius;
};
WWHD_SIZE(bgn2SphereSource, 0x40);

// HD: rope line data is a pair of guest pointers. Accessors retain wrapped guest addresses.
struct bgn2LineView {
    gptr<cXyz> mpPositions;
    gptr<u8> mpSizes;
};
WWHD_OFFSET(bgn2LineView, mpSizes, 4);
WWHD_SIZE(bgn2LineView, 8);
inline u32 bgn2_linePositions(u32 line) {
    return gabi::load<u32>(line + offsetof(bgn2LineView, mpPositions));
}
inline u32 bgn2_lineSizes(u32 line) {
    return gabi::load<u32>(line + offsetof(bgn2LineView, mpSizes));
}
inline void bgn2_setLinePoint(u32 positions, s32 i, const cXyz* p) {
    gabi::at<cXyz>(positions + sizeof(cXyz) * i)->copy(*p);
}
inline void bgn2_setLineSize(u32 sizes, s32 i, u8 size) { gabi::store<u8>(sizes + i, size); }
// HD: phase transition clears this play-object value before stopping the background music.
struct bgn2PlayView {
    u8 _000[0x5B44];
    be<f32> mPhaseChange;
};
WWHD_OFFSET(bgn2PlayView, mPhaseChange, 0x5B44);
WWHD_SIZE(bgn2PlayView, 0x5B48);
inline void bgn2_clearPhaseChange() {
    gabi::store<f32>(dComIfGp_ea() + offsetof(bgn2PlayView, mPhaseChange), 0.0f);
}
inline void bgn2_setPauseTimer(u8 frames) { gabi::store<u8>(0x101EACB7, frames); }
inline void* bgn2_polyInfo(dBgS_LinChk_l* c) { return gabi::at<u8>(gabi::ea(c) + 0x14); }
void* bgn_s_sub(void*, void*);
void* bgn3_s_sub(void*, void*);
void* ki_c_sub(void*, void*);
