/* bwds_class (Molgera larva), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c is +0x11C; the members up to m0320 stay +0x11C. HD dropped
 * m0326 (the knockback pitch): m0324 is the knockback yaw, m0328 follows it, so the members from
 * m0328 on are +0x11A and from m032C on +0x118. Size 0x19E4 (constructor 02108B2C).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"

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
#define l_HIO (*gabi::at<daBwds_HIO_c>(0x10462C44))
#define hio_set (*gabi::at<be<u8>>(0x101B3874))
#define eff_col 0x10462C34u /* GXColor */

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
/* virtual remove() (vtable +0x44) of a particle callback (as in d_a_kb.h) */
static inline void bw_vremove(void* cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb); }
/* attention_info.flags (+0x39C) */
static inline be<u32>& attn_flags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* bw_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
/* 02518CC8 def_se_set(fopAc_ac_c*, cCcD_Obj*, u32) */
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) (as in d_a_kb.h) */
struct CcAtInfo_bw {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ u8 _08[0x14 - 0x08];
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_bw, 0x1C);
static inline void cc_at_check(fopAc_ac_c* a, CcAtInfo_bw* info) { gabi::call(0x025192A8, a, info); }
/* dComIfGp_particle_setToon (HD): dPa_control_c::set with group 2, setupInfo = roomNo (as in d_a_mo2.h) */
static inline JPABaseEmitter* dComIfGp_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                        void* cb, s8 roomNo) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, roomNo, nullptr, nullptr, nullptr);
}
/* dPa_smokeEcallBack::setColor (inline): 4-byte copy */
static inline void smoke_setColor(dPa_smokeEcallBack_bw* cb, u32 col) {
    u8 c[4];
    for (int k = 0; k < 4; k++) c[k] = gabi::load<u8>(col + k);
    for (int k = 0; k < 4; k++) cb->mColor[k] = c[k];
}
static inline dPa_smokeEcallBack* smoke(dPa_smokeEcallBack_bw* cb) { return (dPa_smokeEcallBack*)cb; }
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt BW_GNDCHK_VT = {0x1000C254, 0x1000C264, 0x1000C284, 0x1000C274};
static inline void bw_GndChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x20, 0x1000C264);
    gabi::store<u32>(b + 0x40, 0x1000C284);
    gabi::store<u32>(b + 0x4C, 0x1000C244);
    gabi::call(0x02008DAC, c, 0); /* cBgS_Chk::~cBgS_Chk */
}
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0 (the matcher calls it init), init 025E8154 (as in d_a_oq.h) */
static inline void* mDoExt_brkAnm_ct(void* p) { return gabi::call<void*>(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, mode, rate, start, end, modify, entry);
}
/* mDoExt_btkAnm (HD 0x74): constructor 025E7C6C (the matcher calls it init) */
static inline void* mDoExt_btkAnm_ct(void* p) { return gabi::call<void*>(0x025E7C6C, p); }
/* GHS array helpers (as in d_a_kb.h; __destroy_arr also takes r8) */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 f7, u32 f8) { gabi::call(0x028F0164, p, n, size, dtor, f7, f8); }
/* 02515AE8 dCcD_Sph::~dCcD_Sph, 025166F0 dCcD_Sph::dCcD_Sph */
static inline void dCcD_Sph_dt(void* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }
static inline void dCcD_Sph_ct(void* s) { gabi::call(0x025166F0, s); }
