/* gnd_class (Ganondorf, final boss), WWHD layout. 
 *
 * The GameCube header of this unit is almost empty (all functions are "Nonmatching" stubs), so
 * the layout is measured from the WWHD code: constructor 0215788C allocates 0x1950 bytes.
 * Members whose meaning is not known yet are addressed by offset (GF/GP below). */
#pragma once
#include "bindings.h"

/* ---- shared classes, local layouts / bindings (SHARED-CANDIDATE) ---- */
/* GHS array helpers */
static inline void gnd_construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void gnd_destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 f7, u32 f8) { gabi::call(0x028F0164, p, n, size, dtor, f7, f8); }
/* dCcD_Sph constructor / destructor (out of line) */
static inline void gnd_Sph_ct(void* s) { gabi::call(0x025166F0, s); }
static inline void gnd_Sph_dt(void* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }
/* mDoExt_3DlineMat0_c (HD 0x148): constructor 025E9960, destructor 025E99E0, init 025E9B80,
 * update 025EA548; the virtual getMaterialID at vtable (+0x130) slot +0x14 */
static inline void mDoExt_3DlineMat0_ct(void* p) { gabi::call(0x025E9960, p); }
static inline void mDoExt_3DlineMat0_dt(void* p, s32 flags) { gabi::call(0x025E99E0, p, flags); }
static inline BOOL mDoExt_3DlineMat0_init(void* l, u16 numLines, u16 numSegs, BOOL hasSize) {
    return gabi::call<BOOL>(0x025E9B80, l, numLines, numSegs, hasSize);
}
static inline void mDoExt_3DlineMat0_update(void* l, u16 segs, f32 size, const GXColor* color, u16 space, dKy_tevstr_c* tev) {
    gabi::call(0x025EA548, l, segs, size, color, space, tev);
}
/* dComIfGd_set3DlineMat: the play's line packets (play+0x5FB4, 0x9C each) by material id */
static inline void gnd_set3DlineMat(void* l) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(l) + 0x130) + 0x14), l);
    gabi::call(0x025EDD04, pkt + id * 0x9C, l);
}
/* mDoExt_brkAnm (HD 0x78) / btkAnm (0x74) / btpAnm (0x74): constructors and init */
static inline void* gnd_brkAnm_ct(void* p) { return gabi::call<void*>(0x025E80D0, p); }
static inline void* gnd_btkAnm_ct(void* p) { return gabi::call<void*>(0x025E7C6C, p); }
static inline void* gnd_btpAnm_ct(void* p) { return gabi::call<void*>(0x025E7820, p); }
static inline BOOL gnd_brkAnm_init(void* a, J3DModelData* d, void* res, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, res, play, mode, rate, start, end, modify, entry);
}
static inline BOOL gnd_btkAnm_init(void* a, J3DModelData* d, void* res, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E7CE0, a, d, res, play, mode, rate, start, end, modify, entry);
}
static inline BOOL gnd_btpAnm_init(void* a, J3DModelData* d, void* res, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E789C, a, d, res, play, mode, rate, start, end, modify, entry);
}
/* 025A9084 dPa_rippleEcallBack::dPa_rippleEcallBack */
static inline void gnd_rippleEcallBack_ct(void* p) { gabi::call(0x025A9084, p); }
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb): fopAcM_monsSeStart, HD inline with
 * actor and eyePos null checks */
static inline void gnd_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
/* mDoGph_gInf_c blur: mBlureFlag (0x101F4825), mBlureRate (0x101F4826); onBlure 025F064C */
static inline void mDoGph_onBlure() { gabi::call(0x025F064C); }
static inline void mDoGph_offBlure() { gabi::store<u8>(0x101F4825, 0); }
static inline void mDoGph_setBlureRate(u8 r) { gabi::store<u8>(0x101F4826, r); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* gnd_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
/* JPABaseEmitter::becomeInvalidEmitter (inline): mMaxFrame (+0x5C) = -1, flags (+0x254) |= 1 */
static inline void gnd_becomeInvalidEmitter(u32 e) {
    u32 flags = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, flags | 1);
}
/* 025445B8 dEvent_manager_c::startCheckOld(const char*) (play+0x52C4) */
static inline BOOL gnd_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
static inline void gnd_XrotS(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) -> the hit actor (as in d_a_kb.h) */
struct CcAtInfo_gnd {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_gnd, 0x1C);
static inline fopAc_ac_c* gnd_cc_at_check(fopAc_ac_c* a, CcAtInfo_gnd* info) { return gabi::call<fopAc_ac_c*>(0x025192A8, a, info); }
/* 02518CC8 def_se_set(fopAc_ac_c*, cCcD_Obj*, u32) */
static inline void gnd_def_se_set(fopAc_ac_c* a, void* obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }
/* 02516178 dCcD_GObjInf::GetAtHitObj */
static inline void* gnd_GetAtHitObj(void* inf) { return gabi::call<void*>(0x02516178, inf); }
/* 0255F554 dKy_SordFlush_set(cXyz, int) (the position by address) */
static inline void gnd_SordFlush_set(cXyz* pos, s32 type) { gabi::call(0x0255F554, pos, type); }
/* daPy_py_c virtual at C++ vtable (+0xB4) slot +0x3C (probably checkPlayerGuard) */
static inline BOOL daPy_vfunc3C(fopAc_ac_c* player) { return gabi::call_ptr<BOOL>(gabi::load<u32>(player->__vtbl + 0x3C), player); }
/* 025E18EC mDoAud_bgmStart(u32) */
static inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }

/* ---- this TU ---- */
#define GND_SAFESTRING_VTBL 0x1000FFFCu /* this TU's copy of the sead::SafeString vtable */
#define GND_VTBL 0x10010054u            /* gnd_class vtable (HD virtual destructor) */

/* daGnd_HIO_c l_HIO (0x104640BC, 0x8C bytes; vtable at +0x88): +0 mNo, +3 line-draw flag */
#define L_HIO 0x104640BCu
/* static flag: the HIO child has been created (0x101B54D8) */
#define GND_HIO_INIT 0x101B54D8u

struct gnd_class : fopEn_enemy_c {
    /* 0x3C8 */ be<u32> m03C8;                     /* this (written by the constructor) */
    /* 0x3CC */ request_of_phase_process_class mPhs;
    /* 0x3D4 */ be<u8> m03D4;                      /* param & 0xF (BGM on create) */
    /* 0x3D5 */ u8 _3D5[3];
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3DC */ gptr<mDoExt_btkAnm> mpBtk;
    /* 0x3E0 */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3E4 */ gptr<mDoExt_btpAnm> mpBtp;
    /* 0x3E8 */ u8 _3E8[0xD98 - 0x3E8];            /* see GF() */
    /* 0xD98 */ u8 mLineMat[0x148];                /* mDoExt_3DlineMat0_c (4 hair strands, 20 segments) */
    /* 0xEE0 */ dBgS_AcchCir mAcchCir;
    /* 0xF20 */ dBgS_Acch mAcch;
    /* 0x10E4 */ dCcD_Stts mStts;
    /* 0x1120 */ dCcD_Cyl mCyl;
    /* 0x1250 */ dCcD_Sph mHeadSph;
    /* 0x137C */ dCcD_Sph mChestSph;
    /* 0x14A8 */ dCcD_Sph mWeponSph[2];
    /* 0x1700 */ u8 _1700[0x1914 - 0x1700];        /* see GF() */
    /* 0x1914 */ u8 mRipple[0x14];                 /* dPa_rippleEcallBack */
    /* 0x1928 */ u8 _1928[0x194C - 0x1928];
    /* 0x194C */ be<u8> mHIOInit;
    /* 0x194D */ u8 _194D[3];
};
WWHD_OFFSET(gnd_class, mpMorf, 0x3D8);
WWHD_OFFSET(gnd_class, mLineMat, 0xD98);
WWHD_OFFSET(gnd_class, mAcch, 0xF20);
WWHD_OFFSET(gnd_class, mStts, 0x10E4);
WWHD_OFFSET(gnd_class, mCyl, 0x1120);
WWHD_OFFSET(gnd_class, mWeponSph, 0x14A8);
WWHD_OFFSET(gnd_class, mRipple, 0x1914);
WWHD_SIZE(gnd_class, 0x1950);

/* members addressed by offset: GF(type, off) is a be<type>& field, GP(off) its address */
#define GF(T, off) (*gabi::at<be<T>>(gabi::ea(i_this) + (off)))
#define GP(off) gabi::at<void>(gabi::ea(i_this) + (off))
#define GXYZ(off) gabi::at<cXyz>(gabi::ea(i_this) + (off))

/* functions of this unit (natural calls between the source files become guest calls) */
BOOL checkGround(gnd_class* i_this, f32 y);
void attack_eff_remove(gnd_class* i_this);
void anm_init(gnd_class* i_this, int anm, f32 morf, u8 mode, f32 speed, int bas);
void ke_move(gnd_class* i_this);
void body_flash(gnd_class* i_this);
void splash_set(gnd_class* i_this);
void pos_move(gnd_class* i_this, s8 noTurn);
BOOL player_view_check(gnd_class* i_this, s16 angle);
void wait_set(gnd_class* i_this);
void demowait(gnd_class* i_this);
void yawait(gnd_class* i_this);
void finish(gnd_class* i_this);
void attack_last(gnd_class* i_this);
void defence0(gnd_class* i_this);
void damage(gnd_class* i_this);
void damage_check(gnd_class* i_this);
