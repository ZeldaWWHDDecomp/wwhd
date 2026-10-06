/* bdk_class (Helmaroc King, battle), WWHD layout. 
 *
 * GameCube -> WWHD: fopEn_enemy_c is +0x11C (mPhase .. mpMorf). HD has no mp2BC/m2C0 (the real
 * shadow model and its id): m2C4 .. mp8F0 are +0x114. HD has no bva (the mask's visibility
 * animation; the mask's materials are switched directly): m8F8 .. m90C are +0x110. The
 * kamen-break block shrinks by 0x10 (see below): mA14 .. mF00 are +0x100. HD has no mF14 (the
 * neck's Y rotation): mF18 .. m6224 are +0xFC. dKy_tevstr_c grows by 0x118: m62D4 .. the end
 * are +0x214. Size 0x65E0 (constructor 02069964). */
#pragma once
#include "bindings.h"

struct bdk_tail_s {
    /* 0x000 */ gptr<J3DModel> m000[9];
    /* 0x024 */ cXyz m024[10];
    /* 0x09C */ csXyz m09C[10];
    /* 0x0D8 */ cXyz m0D8[10];
    /* 0x150 */ cXyz m0150[2];
    /* 0x168 */ csXyz m0168;
    /* 0x16E */ u8 _16E[2];
    /* 0x170 */ cXyz m0170;
};
WWHD_SIZE(bdk_tail_s, 0x17C);

struct bdk_eff_s {
    /* 0x000 */ be<s8> m000;
    /* 0x001 */ be<s8> m001;
    /* 0x002 */ u8 _002[2];
    /* 0x004 */ cXyz m004;
    /* 0x010 */ cXyz m010;
    /* 0x01C */ be<f32> m01C;
    /* 0x020 */ be<f32> m020;
    /* 0x024 */ be<f32> m024;
    /* 0x028 */ be<f32> m028;
    /* 0x02C */ be<f32> m02C;
    /* 0x030 */ csXyz m030;
    /* 0x036 */ csXyz m036;
    /* 0x03C */ be<s16> m03C;
    /* 0x03E */ be<s16> m03E;
    /* 0x040 */ be<s8> m040;
    /* 0x041 */ u8 _041[3];
    /* 0x044 */ gptr<J3DModel> m044;
    /* 0x048 */ dCcD_Sph m048;
};
WWHD_SIZE(bdk_eff_s, 0x174);

/* dPa_smokeEcallBack (HD 0x20): vtable at +0, emitter at +4 */
struct dPa_smokeEcallBack_bdk {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[0x20 - 0x08];
};
WWHD_SIZE(dPa_smokeEcallBack_bdk, 0x20);

struct bdk_class : fopEn_enemy_c {
    /* 0x03C8 */ request_of_phase_process_class mPhase;
    /* 0x03D0 */ be<u8> m2B4;
    /* 0x03D1 */ u8 _3D1[3];
    /* 0x03D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x03D8 */ be<s16> m2C4;
    /* 0x03DA */ be<s16> mAction;
    /* 0x03DC */ be<s16> mState;
    /* 0x03DE */ be<s16> m2CA;
    /* 0x03E0 */ cXyz m2CC;
    /* 0x03EC */ be<s16> m2D8;
    /* 0x03EE */ u8 _3EE[2];
    /* 0x03F0 */ be<f32> m2DC;
    /* 0x03F4 */ be<f32> m2E0;
    /* 0x03F8 */ be<f32> m2E4;
    /* 0x03FC */ be<f32> m2E8;
    /* 0x0400 */ be<s16> m2EC[5];
    /* 0x040A */ be<s16> m2F6;
    /* 0x040C */ be<s16> m2F8;
    /* 0x040E */ be<s16> m2FA;
    /* 0x0410 */ be<u8> m2FC;
    /* 0x0411 */ u8 _411[3];
    /* 0x0414 */ bdk_tail_s m300[4];
    /* 0x0A04 */ gptr<J3DModel> mp8F0;
    /* 0x0A08 */ be<u8> m8F8;
    /* 0x0A09 */ u8 _A09[3];
    /* 0x0A0C */ gptr<J3DModel> m8FC[4];
    /* 0x0A1C */ be<s8> m90C[4];
    /* 0x0A20 */ cXyz m910[4];
    /* 0x0A50 */ cXyz m940[4];
    /* 0x0A80 */ cXyz m970[4];
    /* 0x0AB0 */ csXyz m9A0[4];
    /* 0x0AC8 */ u8 _AC8[0x20]; /* GameCube m9B8/m9D0 (csXyz[4] each): HD layout to be measured */
    /* 0x0AE8 */ be<s8> m9E8[4];
    /* 0x0AEC */ be<f32> m9EC[4];
    /* 0x0AFC */ be<f32> m9FC[4];
    /* 0x0B0C */ be<s16> mA0C[4];
    /* 0x0B14 */ dCcD_Stts mA14;
    /* 0x0B50 */ dCcD_Sph mA50[4];
    /* 0x1000 */ be<f32> mF00[4];
    /* 0x1010 */ be<u8> mF10;
    /* 0x1011 */ u8 _1011;
    /* 0x1012 */ be<s16> mF12;
    /* 0x1014 */ be<f32> mF18;
    /* 0x1018 */ dBgS_AcchCir mAcchCir;
    /* 0x1058 */ dBgS_ObjAcch mAcch;
    /* 0x121C */ be<s16> m1120;
    /* 0x121E */ be<s16> m1122;
    /* 0x1220 */ be<s16> m1124;
    /* 0x1222 */ be<s16> m1126;
    /* 0x1224 */ be<s16> m1128;
    /* 0x1226 */ be<s16> m112A;
    /* 0x1228 */ be<s16> m112C;
    /* 0x122A */ be<s16> m112E;
    /* 0x122C */ be<s16> m1130;
    /* 0x122E */ be<s16> m1132;
    /* 0x1230 */ be<s16> m1134;
    /* 0x1232 */ be<s16> m1136;
    /* 0x1234 */ be<s16> m1138;
    /* 0x1236 */ be<s8> m113A;
    /* 0x1237 */ u8 _1237;
    /* 0x1238 */ be<f32> m113C;
    /* 0x123C */ be<f32> m1140;
    /* 0x1240 */ cXyz m1144;
    /* 0x124C */ cXyz m1150;
    /* 0x1258 */ cXyz m115C;
    /* 0x1264 */ cXyz m1168;
    /* 0x1270 */ cXyz m1174[2];
    /* 0x1288 */ dCcD_Stts mStts;
    /* 0x12C4 */ dCcD_Sph mHeadAtSph;
    /* 0x13F0 */ dCcD_Sph mHeadTgSph;
    /* 0x151C */ dCcD_Sph mTosakaTgSph;
    /* 0x1648 */ dCcD_Sph mBodyCCSph;
    /* 0x1774 */ dCcD_Sph mFootCCSph[2];
    /* 0x19CC */ dCcD_Sph mWindAtSph[10];
    /* 0x2584 */ be<u8> m2488[10];
    /* 0x258E */ u8 _258E[2];
    /* 0x2590 */ cXyz m2494[10];
    /* 0x2608 */ cXyz m250C[10];
    /* 0x2680 */ be<u8> m2584;
    /* 0x2681 */ be<u8> m2585;
    /* 0x2682 */ be<s8> m2586;
    /* 0x2683 */ u8 _2683;
    /* 0x2684 */ be<s32> m2588;
    /* 0x2688 */ be<s32> m258C;
    /* 0x268C */ be<u8> m2590;
    /* 0x268D */ be<u8> m2591;
    /* 0x268E */ be<s8> m2592;
    /* 0x268F */ be<s8> m2593;
    /* 0x2690 */ be<s8> m2594;
    /* 0x2691 */ u8 _2691[3];
    /* 0x2694 */ gptr<fopAc_ac_c> mp2598;
    /* 0x2698 */ u8 m259C[2];
    /* 0x269A */ be<s16> m259E;
    /* 0x269C */ be<s16> m25A0;
    /* 0x269E */ be<s8> m25A2;
    /* 0x269F */ u8 _269F;
    /* 0x26A0 */ be<s16> m25A4;
    /* 0x26A2 */ be<s16> m25A6;
    /* 0x26A4 */ cXyz m25A8;
    /* 0x26B0 */ cXyz m25B4;
    /* 0x26BC */ csXyz m25C0;
    /* 0x26C2 */ u8 _26C2[2];
    /* 0x26C4 */ be<f32> m25C8;
    /* 0x26C8 */ be<f32> m25CC;
    /* 0x26CC */ be<f32> m25D0;
    /* 0x26D0 */ be<f32> m25D4;
    /* 0x26D4 */ be<s8> m25D8;
    /* 0x26D5 */ u8 _26D5[3];
    /* 0x26D8 */ cXyz m25DC;
    /* 0x26E4 */ cXyz m25E8;
    /* 0x26F0 */ cXyz m25F4;
    /* 0x26FC */ u8 m2600[8];
    /* 0x2704 */ be<f32> m2608;
    /* 0x2708 */ cXyz m260C;
    /* 0x2714 */ be<s8> m2618;
    /* 0x2715 */ be<s8> m2619;
    /* 0x2716 */ be<s8> m261A;
    /* 0x2717 */ u8 _2717;
    /* 0x2718 */ bdk_eff_s m261C[40];
    /* 0x6138 */ dCcD_Stts m603C;
    /* 0x6174 */ be<s16> m6078[4];
    /* 0x617C */ dPa_smokeEcallBack_bdk m6080[4];
    /* 0x61FC */ gptr<JPABaseEmitter> m6100[2];
    /* 0x6204 */ u8 _6204[8];
    /* 0x620C */ dPa_smokeEcallBack_bdk m6110;
    /* 0x622C */ dPa_smokeEcallBack_bdk m6130[4];
    /* 0x62AC */ dPa_followEcallBack m61B0;
    /* 0x62C0 */ dPa_followEcallBack m61C4[4];
    /* 0x6310 */ gptr<JPABaseEmitter> mp6214[4];
    /* 0x6320 */ dKy_tevstr_c m6224;
    /* 0x64E8 */ be<f32> m62D4;
    /* 0x64EC */ gptr<J3DModel> mp62D8;
    /* 0x64F0 */ Mtx34 m62DC;
    /* 0x6520 */ gptr<dBgW> pm_bgw;
    /* 0x6524 */ gptr<J3DModel> mp6310[3];
    /* 0x6530 */ be<f32> m631C;
    /* 0x6534 */ be<f32> m6320;
    /* 0x6538 */ be<f32> m6324;
    /* 0x653C */ u8 m6328[4];
    /* 0x6540 */ Mtx34 m632C[3];
    /* 0x65D0 */ gptr<dBgW> mp63BC[3];
    /* 0x65DC */ be<u32> mp63C8; /* JntHit_c* */
};
WWHD_OFFSET(bdk_class, mPhase, 0x3C8);
WWHD_OFFSET(bdk_class, m300, 0x414);
WWHD_OFFSET(bdk_class, mA14, 0xB14);
WWHD_OFFSET(bdk_class, mAcch, 0x1058);
WWHD_OFFSET(bdk_class, m1120, 0x121C);
WWHD_OFFSET(bdk_class, mWindAtSph, 0x19CC);
WWHD_OFFSET(bdk_class, m261C, 0x2718);
WWHD_OFFSET(bdk_class, m6224, 0x6320);
WWHD_OFFSET(bdk_class, mp63C8, 0x65DC);
WWHD_SIZE(bdk_class, 0x65E0);

/* daBdk_HIO_c (0x2C; HD: the JORReflexible vtable is at the end, +0x28) */
struct daBdk_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m005;
    /* 0x02 */ be<s16> m006;
    /* 0x04 */ be<f32> m008;
    /* 0x08 */ be<f32> m00C;
    /* 0x0C */ be<f32> m010;
    /* 0x10 */ be<u8> m014;
    /* 0x11 */ u8 _11[3];
    /* 0x14 */ be<f32> m018;
    /* 0x18 */ be<f32> m01C;
    /* 0x1C */ be<f32> m020;
    /* 0x20 */ be<f32> m024;
    /* 0x24 */ be<s16> m028;
    /* 0x26 */ u8 _26[2];
    /* 0x28 */ be<u32> __vtbl;
};
WWHD_SIZE(daBdk_HIO_c, 0x2C);
#define l_HIO (*gabi::at<daBdk_HIO_c>(0x1046191C))
#define DABDK_HIO_VTBL 0x10007F48
#define BDK_VTBL 0x10007F58          /* bdk_class vtable (HD virtual destructor) */
#define BDK_SAFESTRING_VTBL 0x10007E60 /* this TU's sead::SafeString vtable */

/* ---- local bindings and HD inlines ---- */
#define STR_BDK_ANM STR(0x10007FE8) /* "Bdk" (anm_init) */
#define REG8_F(i) REG_F(8, i)
#define REG6_F(i) REG_F(6, i)
#define REG12_F(i) REG_F(12, i)
#define REG13_F(i) REG_F(13, i)
#define REG14_F(i) REG_F(14, i)
#define REG0_F_(i) REG_F(0, i)
/* statics (.bss) */
#define center_pos (*gabi::at<cXyz>(0x104618F8))
#define center_pos2 (*gabi::at<cXyz>(0x10461904))
#define wind_se_pos (*gabi::at<cXyz>(0x10461910))
/* mDoExt_McaMorf (HD): model +0x90, frame +0x9C */
static inline J3DModel* morf_model(mDoExt_McaMorf* m) { return gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(m) + 0x90)); }
static inline f32 morf_frame(mDoExt_McaMorf* m) { return gabi::load<f32>(gabi::ea(m) + 0x9C); }
static inline void morf_setAnm(mDoExt_McaMorf* m, void* anm, s32 loopMode, f32 morf, f32 speed, f32 start, f32 end, void* sound) {
    gabi::call(0x025E4A98, m, anm, loopMode, morf, speed, start, end, sound);
}
/* camera_process_class* dComIfGp_getCamera(0): play+0x5AF8; view.mLookat.mEye at +0xDC */
static inline u32 bdk_camera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at model+0x2C */
struct J3DMtxBlock_bdk {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_bdk {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_bdk> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* bdk_getAnmMtx(J3DModel_bdk* m, s32 jnt) {
    J3DMtxBlock_bdk* blk = m->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline J3DModel_bdk* bdk_j3dSys_getModel() { return gabi::at<J3DModel_bdk>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* bdk_J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* virtual remove() (vtable +0x44) of the particle callbacks */
template <class CB> static inline void bdk_vremove(CB* cb) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb);
}
/* dKy_tevstr_c (HD): three light blocks of 0x44 bytes at +0x00, +0xC0, +0x144 (floats 0..0x14,
 * bytes 0x18..0x1B, shorts 0x1C..0x22, floats 0x24..0x40) */
/* float members are copied bit-exactly (an lfs/stfs pair keeps a signalling NaN) */
static inline void bdk_fbits(u32 d, u32 s) { gabi::store<u32>(d, gabi::load<u32>(s)); }
static inline void bdk_tev_light_copy(u32 d, u32 s) {
    for (int k = 0; k < 0x18; k += 4) bdk_fbits(d + k, s + k);
    for (int k = 0x18; k < 0x1C; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    for (int k = 0x1C; k < 0x24; k += 2) gabi::store<s16>(d + k, gabi::load<s16>(s + k));
    for (int k = 0x24; k < 0x44; k += 4) bdk_fbits(d + k, s + k);
}
/* dKy_tevstr_c::operator= (HD, inline): the three light blocks and the members at 0x84..0xBC */
static inline void bdk_tevstr_copy(dKy_tevstr_c* dst, const dKy_tevstr_c* src) {
    u32 d = gabi::ea(dst), s = gabi::ea(src);
    bdk_tev_light_copy(d, s);
    for (int k = 0x84; k < 0x90; k += 4) gabi::store<u32>(d + k, gabi::load<u32>(s + k));
    for (int k = 0x90; k < 0x98; k += 2) gabi::store<u16>(d + k, gabi::load<u16>(s + k));
    for (int k = 0x98; k < 0xA0; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    for (int k = 0xA0; k < 0xA8; k += 2) gabi::store<u16>(d + k, gabi::load<u16>(s + k));
    for (int k = 0xA8; k < 0xB4; k += 4) bdk_fbits(d + k, s + k);
    for (int k = 0xB4; k < 0xBD; k++) gabi::store<u8>(d + k, gabi::load<u8>(s + k));
    bdk_tev_light_copy(d + 0xC0, s + 0xC0);
    bdk_tev_light_copy(d + 0x144, s + 0x144);
}
/* dComIfGs_isStageBossEnemy / onStageBossEnemy (HD: dungeon item bit 3 of the save's memory bits,
 * *(0x101F84DC) + 0x798) */
static inline u32 bdk_memBit() { return gabi::load<u32>(0x101F84DC) + 0x798; }
static inline BOOL dSv_memBit_isDungeonItem(u32 mb, s32 i) { return gabi::call<BOOL>(0x025B9100, mb, i); }
static inline void dSv_memBit_onDungeonItem(u32 mb, s32 i) { gabi::call(0x025B9098, mb, i); }
static inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
/* dPa_smokeEcallBack::setFollowOff: +0x12 = 1 */
static inline void smoke_setFollowOff(dPa_smokeEcallBack_bdk* cb) { gabi::store<u8>(gabi::ea(cb) + 0x12, 1); }
/* float copies through FPRs without arithmetic keep the bits (lfs/stfs) */
static inline void bdk_fcopy(be<f32>& d, const be<f32>& s) { gabi::store<u32>(gabi::ea(&d), gabi::load<u32>(gabi::ea(&s))); }
/* matrix copy: GHS loads all twelve values into FPRs, then stores them (float stores: the
 * recompiled original quiets some signalling NaNs on the way, the harness's NaN tolerance covers it) */
static inline void bdk_mtx_copy(Mtx34* dst, const Mtx34* src) {
    f32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<f32>(gabi::ea(src) + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<f32>(gabi::ea(dst) + 4 * i, t[i]);
}
static inline void bdk_setBaseTRMtx(J3DModel* m, const Mtx34* src) { bdk_mtx_copy(gabi::at<Mtx34>(gabi::ea(m) + 0xC8), src); }
/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void MtxRotY(f32 rad, u8 concat) { gabi::call(0x0200FBA4, rad, concat); }
static inline void dCcD_Sph_StartCAt(dCcD_Sph* s, cXyz* c) { gabi::call(0x025167C0, s, c); }
static inline void dCcD_Sph_MoveCAt(dCcD_Sph* s, cXyz* c) { gabi::call(0x025167E4, s, c); }
/* functions of the unit called across its source files (WWHD_FUNC turns the calls into guest calls) */
void eff_hane_set(bdk_class* i_this, cXyz* offset, int param_3, s8 param_4);
void anm_init(bdk_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx, u8 param7);
void pos_move(bdk_class* i_this);
void ground_move(bdk_class* i_this);
void wind_set(bdk_class* i_this, cXyz* param2);
void kankyo_cont(bdk_class* i_this);
void obj_move(bdk_class* i_this);
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) { gabi::call(0x025E1AA4, id, pos, pid, param, reverb); }
/* fopAcM_seStart (HD inline: tests &eyePos only) */
static inline void bdk_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    cXyz* eye = &a->eyePos;
    if (gabi::ea(eye) != 0) mDoAud_seStart(id, eye, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* fopAcM_monsSeStart (HD inline: tests the actor and &eyePos) */
static inline void bdk_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 pid = a != nullptr ? gabi::load<u32>(gabi::ea(a) + 4) : 0xFFFFFFFFu; /* fopAcM_GetID */
        mDoAud_monsSeStart(id, &a->eyePos, pid, param, dComIfGp_getReverb(room));
    }
}
/* JPABaseEmitter::setGlobalRTMatrix (inline) */
static inline void bdk_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    gabi::call(0x028249B0, m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C); /* JPASetRMtxTVecfromMtx */
}
static inline Mtx34* bdk_morfAnmMtx(bdk_class* i_this, s32 jnt) { return bdk_getAnmMtx((J3DModel_bdk*)morf_model(i_this->mpMorf), jnt); }
/* tevStr.mColorK0 (HD +0x98) */
static inline GXColor* bdk_colorK0(fopAc_ac_c* a) { return gabi::at<GXColor>(gabi::ea(a) + 0x1A8); }
static inline void dComIfGs_offSwitch_bdk(s32 no, s32 roomNo) { gabi::call(0x025B9F7C, gabi::load<u32>(0x101F84DC) + 0x20, no, roomNo); }
/* mDoExt_McaMorf frame control (HD +0x98): rate +0x98, frame +0x9C */
static inline void morf_setPlaySpeed(mDoExt_McaMorf* m, f32 v) { gabi::store<f32>(gabi::ea(m) + 0x98, v); }
static inline void morf_setFrame(mDoExt_McaMorf* m, f32 v) { gabi::store<f32>(gabi::ea(m) + 0x9C, v); }
/* fopAcM_monsSeStart (HD inline variant: tests &eyePos only) */
static inline void bdk_monsSeStart_e(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0) {
        s32 room = fopAcM_GetRoomNo(a);
        u32 pid = a != nullptr ? gabi::load<u32>(gabi::ea(a) + 4) : 0xFFFFFFFFu;
        mDoAud_monsSeStart(id, &a->eyePos, pid, param, dComIfGp_getReverb(room));
    }
}
/* fopAcM_seStart (HD inline variant: tests the actor and &eyePos) */
static inline void bdk_seStart_a(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void dComIfGs_onSwitch_bdk(s32 no, s32 roomNo) { gabi::call(0x025B9E38, gabi::load<u32>(0x101F84DC) + 0x20, no, roomNo); }
/* dComIfGp_particle_setToon: dPa_control_c::set with group 2 */
static inline JPABaseEmitter* bdk_particle_setToon(u16 id, const cXyz* pos, const csXyz* angle, const cXyz* scale, u8 alpha, void* cb, s8 setup) {
    dPa_control_c* pa = dComIfGp_getParticle();
    return dPa_control_set(pa, 2, id, pos, angle, scale, alpha, (dPa_levelEcallBack*)cb, setup, nullptr, nullptr, nullptr);
}
/* dComIfGp_getVibration().StartShock(REG0_S(2) + add, -0x21, cXyz(0, 1, 0)): the register is read
 * after the play object */
static inline void bdk_StartShock(s32 add) {
    dVibration_c* vib = dComIfGp_getVibration();
    s32 strength = REG0_S(2) + add;
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    gabi::call(0x025CB374, vib, strength, -0x21, v.get());
}
/* JPABaseEmitter::becomeInvalidEmitter (inline): +0x5C = -1, flags (+0x254) |= 1 */
static inline void bdk_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 a = gabi::ea(e);
    u32 f = gabi::load<u32>(a + 0x254);
    gabi::store<s32>(a + 0x5C, -1);
    gabi::store<u32>(a + 0x254, f | 1);
}
