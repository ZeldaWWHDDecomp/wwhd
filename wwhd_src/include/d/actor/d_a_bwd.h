/* bwd_class (Molgera), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c is +0x11C; the members up to m03C4 stay +0x11C. HD adds an
 * array of 20 floats after m03C4 (m03C4b, per body segment, written by end()), so m0414..m1710
 * are +0x16C. The tevstr m1714 grows by 0x118 (dKy_tevstr_c 0x1C8), so m17C4..m18FC are +0x284.
 * HD has no m18F0 (the cXyz between m18E4 and m18FC): m18FC and everything after it is +0x278.
 * Size 0x3ECC (constructor 020FDE40). Offsets from the verified functions. */
#pragma once
#include "bindings.h"

/* dPa_smokeEcallBack (HD 0x20): vtable at +0, colour (GXColor) at +0x16 */
struct dPa_smokeEcallBack_bd {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x16 - 0x04];
    /* 0x16 */ be<u8> mColor[4];
    /* 0x1A */ u8 _1A[0x20 - 0x1A];
};

struct sita_s {
    /* 0x00 */ gptr<J3DModel> m00;
    /* 0x04 */ cXyz m04;
    /* 0x10 */ u8 m10[4];
};

struct bwd_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhaseBwd;
    /* 0x03D0 */ request_of_phase_process_class mPhaseBwds;
    /* 0x03D8 */ be<u8> m02BC;
    /* 0x03D9 */ u8 _3D9[3];
    /* 0x03DC */ gptr<mDoExt_McaMorf> mpHeadMorf;
    /* 0x03E0 */ gptr<mDoExt_brkAnm> mpHeadBrkAnm;
    /* 0x03E4 */ be<f32> m02C8;
    /* 0x03E8 */ gptr<J3DModel> m02CC;
    /* 0x03EC */ u8 _3EC[4];
    /* 0x03F0 */ gptr<J3DModel> mpBodyModel[20];
    /* 0x0440 */ gptr<J3DModel> m0324[20];
    /* 0x0490 */ gptr<mDoExt_brkAnm> mpBodyMorf[20];
    /* 0x04E0 */ be<f32> m03C4[20];
    /* 0x0530 */ be<f32> m03C4b[20];   /* HD */
    /* 0x0580 */ be<s32> m0414;
    /* 0x0584 */ cXyz m0418[20];
    /* 0x0674 */ cXyz m0508[256];
    /* 0x1274 */ csXyz m1108[256];
    /* 0x1874 */ be<s32> m1708;
    /* 0x1878 */ be<s32> m170C;
    /* 0x187C */ be<s8> m1710;
    /* 0x187D */ u8 _187D[3];
    /* 0x1880 */ dKy_tevstr_c m1714;
    /* 0x1A48 */ be<u8> m17C4;
    /* 0x1A49 */ u8 _1A49[3];
    /* 0x1A4C */ cXyz m17C8;
    /* 0x1A58 */ be<f32> m17D4;
    /* 0x1A5C */ be<s16> m17D8;
    /* 0x1A5E */ u8 _1A5E[2];
    /* 0x1A60 */ be<f32> m17DC;
    /* 0x1A64 */ be<f32> m17E0;
    /* 0x1A68 */ be<s8> m17E4[2];
    /* 0x1A6A */ be<s16> m17E6[2];
    /* 0x1A6E */ u8 _1A6E[2];
    /* 0x1A70 */ gptr<J3DModel> m17EC[2];
    /* 0x1A78 */ gptr<mDoExt_btkAnm> m17F4[2];
    /* 0x1A80 */ Mtx34 mBgwMtx1[2];
    /* 0x1AE0 */ gptr<dBgW> mpBgW1[2];
    /* 0x1AE8 */ be<s8> m1864;
    /* 0x1AE9 */ be<s8> m1865;
    /* 0x1AEA */ u8 _1AEA[2];
    /* 0x1AEC */ gptr<J3DModel> mpTriforcePlatformModel;
    /* 0x1AF0 */ Mtx34 mBgwMtx2;
    /* 0x1B20 */ gptr<dBgW> mpBgW2;
    /* 0x1B24 */ cXyz m18A0;
    /* 0x1B30 */ be<s16> m18AC;
    /* 0x1B32 */ be<s16> m18AE;
    /* 0x1B34 */ be<s16> m18B0;
    /* 0x1B36 */ u8 _1B36[2];
    /* 0x1B38 */ cXyz m18B4;
    /* 0x1B44 */ u8 _1B44[4];
    /* 0x1B48 */ be<f32> m18C4;
    /* 0x1B4C */ be<f32> m18C8;
    /* 0x1B50 */ be<s16> m18CC[2];
    /* 0x1B54 */ be<s16> m18D0;
    /* 0x1B56 */ u8 _1B56[4];
    /* 0x1B5A */ be<s16> m18D6;
    /* 0x1B5C */ be<u8> m18D8;
    /* 0x1B5D */ u8 _1B5D[3];
    /* 0x1B60 */ be<f32> m18DC;
    /* 0x1B64 */ be<u32> m18E0;
    /* 0x1B68 */ cXyz m18E4;
    /* 0x1B74 */ be<s16> m18FC;   /* HD: no m18F0 */
    /* 0x1B76 */ be<s16> m18FE;
    /* 0x1B78 */ be<s16> m1900;
    /* 0x1B7A */ be<s16> m1902;
    /* 0x1B7C */ be<s32> m1904;
    /* 0x1B80 */ sita_s mTongueSegments[32];
    /* 0x1E00 */ cXyz m1B88;
    /* 0x1E0C */ cXyz m1B94;
    /* 0x1E18 */ be<f32> m1BA0;
    /* 0x1E1C */ be<f32> m1BA4;
    /* 0x1E20 */ be<f32> m1BA8;
    /* 0x1E24 */ be<f32> m1BAC;
    /* 0x1E28 */ be<s8> m1BB0;
    /* 0x1E29 */ be<s8> m1BB1;
    /* 0x1E2A */ be<s8> m1BB2;
    /* 0x1E2B */ be<s8> m1BB3;
    /* 0x1E2C */ be<s8> m1BB4;
    /* 0x1E2D */ be<s8> m1BB5;
    /* 0x1E2E */ be<s8> m1BB6;
    /* 0x1E2F */ u8 _1E2F;
    /* 0x1E30 */ be<f32> m1BB8;
    /* 0x1E34 */ be<u32> m1BBC;
    /* 0x1E38 */ be<u32> m1BC0;
    /* 0x1E3C */ dCcD_Stts mStts;
    /* 0x1E78 */ dCcD_Sph mBodySph[19];
    /* 0x34BC */ dCcD_Sph mTongueSph;
    /* 0x35E8 */ dCcD_Sph mTongueCoSph[5];
    /* 0x3BC4 */ be<s8> m394C;
    /* 0x3BC5 */ u8 _3BC5[7];
    /* 0x3BCC */ cXyz m3954;
    /* 0x3BD8 */ be<s8> m3960;
    /* 0x3BD9 */ u8 _3BD9[3];
    /* 0x3BDC */ gptr<JPABaseEmitter> m3964;
    /* 0x3BE0 */ gptr<JPABaseEmitter> m3968;
    /* 0x3BE4 */ u8 _3BE4[0xC];
    /* 0x3BF0 */ dPa_smokeEcallBack_bd m3978[10];
    /* 0x3D30 */ dPa_followEcallBack m3AB8[2];
    /* 0x3D58 */ be<s16> m3AE0[2];
    /* 0x3D5C */ be<s16> m3AE4;
    /* 0x3D5E */ be<s16> m3AE6;
    /* 0x3D60 */ be<s16> m3AE8;
    /* 0x3D62 */ be<s16> m3AEA;
    /* 0x3D64 */ be<s16> m3AEC;
    /* 0x3D66 */ u8 _3D66[6];
    /* 0x3D6C */ dPa_followEcallBack m3AF4;
    /* 0x3D80 */ be<s8> m3B08[2];
    /* 0x3D82 */ u8 _3D82[2];
    /* 0x3D84 */ cXyz m3B0C[2];
    /* 0x3D9C */ gptr<mDoExt_McaMorf> mpGspMorf[2];
    /* 0x3DA4 */ gptr<mDoExt_btkAnm> mpGspBtkAnm[2];
    /* 0x3DAC */ gptr<mDoExt_brkAnm> mpGspBrkAnm[2];
    /* 0x3DB4 */ be<f32> m3B3C;
    /* 0x3DB8 */ be<f32> m3B40;
    /* 0x3DBC */ be<f32> m3B44;
    /* 0x3DC0 */ csXyz m3B48[2];
    /* 0x3DCC */ dPa_smokeEcallBack_bd m3B54[6];
    /* 0x3E8C */ be<s8> m3C14;
    /* 0x3E8D */ be<s8> m3C15;
    /* 0x3E8E */ u8 _3E8E[2];
    /* 0x3E90 */ be<f32> m3C18;
    /* 0x3E94 */ be<s16> m3C1C;
    /* 0x3E96 */ be<s16> m3C1E;
    /* 0x3E98 */ be<s16> m3C20;
    /* 0x3E9A */ be<s16> m3C22;
    /* 0x3E9C */ be<s8> m3C24;
    /* 0x3E9D */ u8 _3E9D[3];
    /* 0x3EA0 */ cXyz m3C28;
    /* 0x3EAC */ cXyz m3C34;
    /* 0x3EB8 */ u8 _3EB8[4];
    /* 0x3EBC */ be<f32> m3C44;
    /* 0x3EC0 */ be<f32> m3C48;
    /* 0x3EC4 */ be<f32> m3C4C;
    /* 0x3EC8 */ be<s8> m3C50;
    /* 0x3EC9 */ be<u8> m3C51;
    /* 0x3ECA */ u8 _3ECA[2];
};
WWHD_OFFSET(bwd_class, mpHeadMorf, 0x3DC);
WWHD_OFFSET(bwd_class, m0414, 0x580);
WWHD_OFFSET(bwd_class, m1714, 0x1880);
WWHD_OFFSET(bwd_class, m17C4, 0x1A48);
WWHD_OFFSET(bwd_class, mpBgW2, 0x1B20);
WWHD_OFFSET(bwd_class, m18FC, 0x1B74);
WWHD_OFFSET(bwd_class, mTongueSegments, 0x1B80);
WWHD_OFFSET(bwd_class, m1B88, 0x1E00);
WWHD_OFFSET(bwd_class, mStts, 0x1E3C);
WWHD_OFFSET(bwd_class, mTongueSph, 0x34BC);
WWHD_OFFSET(bwd_class, m394C, 0x3BC4);
WWHD_OFFSET(bwd_class, m3978, 0x3BF0);
WWHD_OFFSET(bwd_class, m3AF4, 0x3D6C);
WWHD_OFFSET(bwd_class, mpGspMorf, 0x3D9C);
WWHD_OFFSET(bwd_class, m3B54, 0x3DCC);
WWHD_OFFSET(bwd_class, m3C1C, 0x3E94);
WWHD_OFFSET(bwd_class, m3C51, 0x3EC9);
WWHD_SIZE(bwd_class, 0x3ECC);

/* l_HIO (0x10462A68): daBwd_HIO_c, HD layout {mNo, m05.., floats.., vtable last}, 0x40 bytes */
struct daBwd_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<u8> m06;
    /* 0x03 */ be<u8> m07;
    /* 0x04 */ be<u8> m08;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<s16> m14;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> m18;
    /* 0x18 */ be<f32> m1C;
    /* 0x1C */ be<f32> m20;
    /* 0x20 */ be<s16> m24;
    /* 0x22 */ be<s16> m26;
    /* 0x24 */ be<f32> m28;
    /* 0x28 */ be<f32> m2C;
    /* 0x2C */ be<f32> m30;
    /* 0x30 */ be<f32> m34;
    /* 0x34 */ be<s16> m38;
    /* 0x36 */ be<s16> m3A;
    /* 0x38 */ be<s16> m3C;
    /* 0x3A */ be<s16> m3E;
    /* 0x3C */ be<u32> __vtbl;
};
WWHD_SIZE(daBwd_HIO_c, 0x40);
#define l_HIO (*gabi::at<daBwd_HIO_c>(0x10462A68))
#define hio_set (*gabi::at<be<u8>>(0x10192F24))
#define eff_col 0x10462A54u /* GXColor */
#define ko_count (*gabi::at<be<s32>>(0x10462A58))
static inline be<u32>* ko_ac() { return gabi::at<be<u32>>(0x10462B3C); }
static inline cXyz* suna_gr_pos() { return gabi::at<cXyz>(0x10462AF4); }
static inline csXyz* suna_gr_ang() { return gabi::at<csXyz>(0x10462AB8); }
#define center_pos (gabi::at<cXyz>(0x10462ADC))

#define BWD_VTBL 0x1000BE6C      /* bwd_class vtable (HD virtual destructor) */
#define BWD_HIO_VTBL 0x1000BE5C  /* daBwd_HIO_c vtable */
#define SAFESTRING_VTBL 0x1000BDA4

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define REG0_F(i) REG_F(0, i)
#define REG0_S(i) REG_S(0, i)
/* virtual remove() (vtable +0x44) of a particle callback */
static inline void bd_vremove(void* cb) { gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb); }
/* attention_info.flags (+0x39C) */
static inline be<u32>& attn_flags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* bd_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
/* 02518CC8 def_se_set(fopAc_ac_c*, cCcD_Obj*, u32) */
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }
struct CcAtInfo_bd {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ u8 _08[0x14 - 0x08];
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_bd, 0x1C);
static inline void cc_at_check(fopAc_ac_c* a, CcAtInfo_bd* info) { gabi::call(0x025192A8, a, info); }
/* dComIfGp_particle_setToon (HD): dPa_control_c::set with group 2, setupInfo = roomNo */
static inline JPABaseEmitter* dComIfGp_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha,
                                                        void* cb, s8 roomNo) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, roomNo, nullptr, nullptr, nullptr);
}
/* dPa_smokeEcallBack::setColor (inline): 4-byte copy */
static inline void smoke_setColor(dPa_smokeEcallBack_bd* cb, u32 col) {
    u8 c[4];
    for (int k = 0; k < 4; k++) c[k] = gabi::load<u8>(col + k);
    for (int k = 0; k < 4; k++) cb->mColor[k] = c[k];
}
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0 (the matcher calls it init), init 025E8154 */
static inline void* mDoExt_brkAnm_ct(void* p) { return gabi::call<void*>(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, mode, rate, start, end, modify, entry);
}
/* mDoExt_btkAnm (HD 0x74): constructor 025E7C6C (the matcher calls it init) */
static inline void* mDoExt_btkAnm_ct(void* p) { return gabi::call<void*>(0x025E7C6C, p); }
/* GHS array helpers */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 f7, u32 f8) { gabi::call(0x028F0164, p, n, size, dtor, f7, f8); }
/* 02515AE8 dCcD_Sph::~dCcD_Sph, 025166F0 dCcD_Sph::dCcD_Sph */
static inline void dCcD_Sph_dt(void* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }
static inline void dCcD_Sph_ct(void* s) { gabi::call(0x025166F0, s); }
/* dBgS_LinChk on the stack (this TU's vtables; layout as in d_a_mt.h) */
struct dBgS_LinChk_bd {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_bd, 0x6C);
static inline void bd_LinChk_ct(dBgS_LinChk_bd* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x1000BE1C;
    c->__vtbl_64 = 0x1000BE3C;
    c->__vtbl_58 = 0x1000BE4C;
    c->__vtbl_20 = 0x1000BE2C;
}
static inline void bd_LinChk_dt(dBgS_LinChk_bd* c) {
    c->__vtbl_58 = 0x1000BE4C;
    c->__vtbl_64 = 0x1000BDCC;
    c->__vtbl_20 = 0x1000BDBC;
    cBgS_LinChk_dt(c, 0);
}

/* calls between this unit's source files (by address) */
static inline void bwd_anm_init(bwd_class* i_this, int bck, f32 morf, u8 loopMode, f32 speed, int snd) {
    gabi::call(0x020F9AF4, i_this, bck, morf, loopMode, speed, snd);
}
static inline void bwd_fly_pos_move(bwd_class* i_this, s16 p2, s16 p3) { gabi::call(0x020F9F80, i_this, p2, p3); }
static inline void bwd_g_eff_on(bwd_class* i_this) { gabi::call(0x020F99F8, i_this); }
static inline void bwd_g_eff_off(bwd_class* i_this) { gabi::call(0x020F9A10, i_this); }
#define REG10_F(i) REG_F(10, i)
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt BD_GNDCHK_VT = {0x1000BDDC, 0x1000BDEC, 0x1000BE0C, 0x1000BDFC};
static inline void bd_GndChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x20, 0x1000BDEC);
    gabi::store<u32>(b + 0x40, 0x1000BE0C);
    gabi::store<u32>(b + 0x4C, 0x1000BDCC);
    gabi::call(0x02008DAC, c, 0); /* cBgS_Chk::~cBgS_Chk */
}
/* fopAcM_seStart as inlined in this unit (HD: only the eyePos address is checked) */
static inline void bd_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline u8 dComIfGp_getStartStageName0() { return gabi::load<u8>(dComIfGp_ea() + 0x5134); }
/* JAIZelBasic (HD): bgmStreamPrepare 025E1934, bgmStreamPlay 025E1944, bgmStart 025E18EC */
static inline void mDoAud_bgmStreamPrepare(u32 id) { gabi::call(0x025E1934, id); }
static inline void mDoAud_bgmStreamPlay() { gabi::call(0x025E1944); }
/* JPABaseEmitter (HD inline): becomeInvalidEmitter (+0x5C = -1, flags +0x254 |= 1); setGlobalRTMatrix */
static inline void JPA_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 b = gabi::ea(e);
    u32 f = gabi::load<u32>(b + 0x254);
    gabi::store<s32>(b + 0x5C, -1);
    gabi::store<u32>(b + 0x254, f | 1);
}
static inline void JPA_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    gabi::call(0x028249B0, m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C); /* JPASetRMtxTVecfromMtx */
}
#define REG10_S(i) REG_S(10, i)
