/* bwds_class (Molgera larva), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c is +0x11C; the members up to m0320 stay +0x11C. HD dropped
 * m0326 (the knockback pitch): m0324 is the knockback yaw, m0328 follows it, so the members from
 * m0328 on are +0x11A and from m032C on +0x118. Size 0x19E4 (constructor 02108B2C).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"

/* dPa_smokeEcallBack (HD 0x20): vtable at +0, colour (GXColor) at +0x16 */
struct dPa_smokeEcallBack_bw {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x16 - 0x04];
    /* 0x16 */ be<u8> mColor[4];
    /* 0x1A */ u8 _1A[0x20 - 0x1A];
};

struct bwds_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class m02AC;
    /* 0x3D0 */ be<u8> m02B4;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ gptr<J3DModel> mp02BC[0xE];
    /* 0x410 */ be<s16> m02F4;
    /* 0x412 */ be<s16> m02F6;  /* action */
    /* 0x414 */ be<s16> m02F8;  /* mode */
    /* 0x416 */ be<s8> m02FA;
    /* 0x417 */ be<s8> m02FB;
    /* 0x418 */ cXyz m02FC;
    /* 0x424 */ u8 m0308[4];
    /* 0x428 */ be<f32> m030C;
    /* 0x42C */ be<f32> m0310;
    /* 0x430 */ be<s16> m0314[5];
    /* 0x43A */ be<s16> m031E;
    /* 0x43C */ be<f32> m0320;  /* knockback speed */
    /* 0x440 */ be<s16> m0324;  /* knockback yaw (HD: no pitch m0326) */
    /* 0x442 */ be<s16> m0328;
    /* 0x444 */ cXyz m032C[0xF];
    /* 0x4F8 */ csXyz m03E0[0xF];
    /* 0x552 */ u8 _552[2];
    /* 0x554 */ cXyz m043C[0xF];
    /* 0x608 */ be<s32> m04F0;
    /* 0x60C */ be<s8> m04F4;
    /* 0x60D */ be<s8> m04F5;
    /* 0x60E */ u8 _60E[0x614 - 0x60E];
    /* 0x614 */ be<s16> m04FC;
    /* 0x616 */ u8 _616[2];
    /* 0x618 */ be<s16> m0500;
    /* 0x61A */ be<s16> m0502;
    /* 0x61C */ be<f32> m0504;
    /* 0x620 */ dCcD_Stts m0508;
    /* 0x65C */ dCcD_Sph m0544;
    /* 0x788 */ dCcD_Sph m0670[0xF];
    /* 0x191C */ cXyz m1804;
    /* 0x1928 */ cXyz m1810;
    /* 0x1934 */ be<s8> m181C;
    /* 0x1935 */ be<s8> m181D;
    /* 0x1936 */ be<s8> m181E;
    /* 0x1937 */ u8 _1937;
    /* 0x1938 */ dPa_smokeEcallBack_bw m1820[3];
    /* 0x1998 */ dPa_followEcallBack m1880;
    /* 0x19AC */ be<s8> m1894[2];
    /* 0x19AE */ u8 _19AE[2];
    /* 0x19B0 */ cXyz m1898[2];
    /* 0x19C8 */ gptr<mDoExt_McaMorf> mp18B0[2];
    /* 0x19D0 */ gptr<mDoExt_btkAnm> mp18B8[2];
    /* 0x19D8 */ gptr<mDoExt_brkAnm> mp18C0[2];
    /* 0x19E0 */ be<u8> m18C8;
    /* 0x19E1 */ u8 _19E1[3];
};
WWHD_OFFSET(bwds_class, mpMorf, 0x3D4);
WWHD_OFFSET(bwds_class, m02FC, 0x418);
WWHD_OFFSET(bwds_class, m0328, 0x442);
WWHD_OFFSET(bwds_class, m043C, 0x554);
WWHD_OFFSET(bwds_class, m0508, 0x620);
WWHD_OFFSET(bwds_class, m0670, 0x788);
WWHD_OFFSET(bwds_class, m1804, 0x191C);
WWHD_OFFSET(bwds_class, m1820, 0x1938);
WWHD_OFFSET(bwds_class, m1894, 0x19AC);
WWHD_OFFSET(bwds_class, m18C8, 0x19E0);
WWHD_SIZE(bwds_class, 0x19E4);

/* l_HIO (0x10462C44): daBwds_HIO_c, HD layout {mNo, m005, m008.., vtable last} */
struct daBwds_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m005;
    /* 0x02 */ u8 _02[2];
    /* 0x04 */ be<f32> m008;
    /* 0x08 */ be<f32> m00C;
    /* 0x0C */ be<f32> m010;
    /* 0x10 */ be<s16> m014;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> m018;
    /* 0x18 */ be<f32> m01C;
    /* 0x1C */ be<u32> __vtbl;
};
WWHD_SIZE(daBwds_HIO_c, 0x20);
#ifndef BWDS_STORAGE_OWNER
// Autoglue exports the owner source's statics as references to guest storage.
extern daBwds_HIO_c& l_HIO;
extern be<u8>& hio_set;
extern GXColor& eff_col;
#endif

#define BWDS_VTBL 0x1000C2A4      /* bwds_class vtable (HD virtual destructor) */
#define BWDS_HIO_VTBL 0x1000C294  /* daBwds_HIO_c vtable */
#define SAFESTRING_VTBL 0x1000C22C

enum {
    dRes_INDEX_BWDS_BCK_KOBOSS_CLOSE_e = 4,
    dRes_INDEX_BWDS_BCK_KOBOSS_PAKUPAKU_e = 5,
    dRes_INDEX_BWDS_BDL_KOBOSS_HEAD_e = 9,

};
enum {
    JA_SE_LK_HS_SPIKE = 0x286F,
    JA_SE_CV_BWD_C_HS_DAMAGE = 0x48E3,
    JA_SE_CV_BWD_C_ATTACK = 0x48E4,
    JA_SE_CV_BWD_C_DAMAGE = 0x48E5,
    JA_SE_CV_BWD_C_DIE = 0x48E6,
    JA_SE_CM_BWD_C_MOVE_SAND = 0x61E4,
    JA_SE_CM_BWD_C_JUMP_OUT = 0x69E2,
    JA_SE_CM_BWD_C_IN_SAND = 0x69E3,
};
enum { ACTION_UG_MOVE = 0, ACTION_HOOK_ON = 1, ACTION_HOOK_CHANCE = 2, ACTION_FAIL = 5 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* debug registers (HD keeps them): REG_F(child, i) / REG_S(child, i) from bindings.h */
#define REG0_F(i) REG_F(0, i)
#define REG0_S(i) REG_S(0, i)
#define REG10_F(i) REG_F(10, i)
#define REG10_S(i) REG_S(10, i)
#define REG18_F(i) REG_F(18, i)
// HD: callback removal and the joint-matrix dirty flag use virtual/private views.
struct BwdsCallbackVtable { u8 padding[0x44]; be<u32> remove; };
struct BwdsCallback { gptr<BwdsCallbackVtable> vtable; };
struct BwdsJointMatrices { u8 padding0[4]; be<u16> mFlags; u8 padding6[10]; gptr<Mtx34> mMatrices; };
struct BwdsModel { u8 padding[0x2C]; gptr<BwdsJointMatrices> mJointMatrices; };
struct BwdsPlayerVtable { u8 padding[0x10C]; be<u32> setHookshotCarryOffset; };
WWHD_OFFSET(BwdsCallbackVtable, remove, 0x44);
WWHD_OFFSET(BwdsModel, mJointMatrices, 0x2C);
WWHD_OFFSET(BwdsJointMatrices, mMatrices, 0x10);
WWHD_OFFSET(BwdsPlayerVtable, setHookshotCarryOffset, 0x10C);
static inline void bw_vremove(void* cb) {
    gabi::call_ptr(reinterpret_cast<BwdsCallback*>(cb)->vtable->remove, cb);
}
static inline Mtx34* bw_getAnmMtx(J3DModel* model, s32 joint) {
    BwdsJointMatrices* matrices = reinterpret_cast<BwdsModel*>(model)->mJointMatrices;
    matrices->mFlags = matrices->mFlags | 0x10;
    return matrices->mMatrices.get() + joint;
}
/* 02518CC8 def_se_set(fopAc_ac_c*, cCcD_Obj*, u32) */
void bw_def_se_set(fopAc_ac_c*, void*, u32);
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) (as in d_a_kb.h) */
struct CcAtInfo_bw {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ u8 _08[0x14 - 0x08];
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_bw, 0x1C);
void bw_cc_at_check(fopAc_ac_c*, CcAtInfo_bw*);
/* bw_particle_setToon (HD): dPa_control_c::set with group 2, setupInfo = roomNo (as in d_a_mo2.h) */
static inline JPABaseEmitter* bw_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                        void* cb, s8 roomNo) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, roomNo, nullptr, nullptr, nullptr);
}
/* dPa_smokeEcallBack::setColor (inline): 4-byte copy */
static inline void smoke_setColor(dPa_smokeEcallBack_bw* cb, const GXColor* col) {
    u8 r = col->r, g = col->g, b = col->b, a = col->a;
    cb->mColor[0] = r; cb->mColor[1] = g; cb->mColor[2] = b; cb->mColor[3] = a;
}
static inline dPa_smokeEcallBack* smoke(dPa_smokeEcallBack_bw* cb) { return (dPa_smokeEcallBack*)cb; }
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt BW_GNDCHK_VT = {0x1000C254, 0x1000C264, 0x1000C284, 0x1000C274};
void bw_checkDtor(void*, s32);
static inline void bw_GndChk_dt(dBgS_GndChk* check) {
    static_cast<cBgS_PolyInfo*>(check)->__vtbl = 0x1000C264;
    static_cast<dBgS_PolyPassChk*>(check)->__vtbl = 0x1000C284;
    static_cast<dBgS_GrpPassChk*>(check)->__vtbl = 0x1000C244;
    bw_checkDtor(check, 0);
}
// HD: constructor entries return the original pointer (including NULL).
void* bw_brkCtor(void*);
void* bw_btkCtor(void*);
void bw_constructArray(void*, u32, u32, u32);
void bw_destroyArray(void*, u32, u32, u32, u32, u32);
void bw_sphereCtor(void*);
void bw_sphereDtor(void*, s32);
void* bw_smokeCtor(void*, u8);
void bw_actorDtor(fopAc_ac_c*, s32);
void bw_anm_init(bwds_class*, int, f32, u8, f32, int);
void bw_seStart(fopAc_ac_c*, u32, u32);

// HD: external array element callbacks.
static constexpr u32 BWDS_SPHERE_CTOR = 0x025166F0;
static constexpr u32 BWDS_SPHERE_DTOR = 0x02515AE8;

static inline BwdsPlayerVtable* bw_playerVtable(fopAc_ac_c* player) {
    return gabi::at<BwdsPlayerVtable>(player->__vtbl);
}
