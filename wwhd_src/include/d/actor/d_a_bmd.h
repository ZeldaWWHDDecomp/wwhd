/* bmd_class (Kalle Demos), WWHD layout. 
 *
 * HD size 0xD04 (GameCube 0xBE4; constructor 020B17EC). Members are GameCube +0x11C up to m2FA,
 * then HD adds an s32 (joint index of Makar's model used for the snap picture) at 0x41C, so
 * m2FE..m314 are +0x122; the padding before m318 absorbs 2 bytes, and the rest is +0x120. */
#pragma once
#include "bindings.h"

/* WIND_INFLUENCE (0x2C) */
struct WIND_INFLUENCE_l {
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ cXyz mDir;
    /* 0x18 */ be<f32> mRadius;
    /* 0x1C */ be<f32> mStrength;
    /* 0x20 */ be<f32> field_0x20;
    /* 0x24 */ be<f32> field_0x24;
    /* 0x28 */ be<u32> field_0x28;
};
WWHD_SIZE(WIND_INFLUENCE_l, 0x2C);

/* dPa_smokeEcallBack (0x20, vtable at +0) */
struct dPa_smokeEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x1C];
};
WWHD_SIZE(dPa_smokeEcallBack_l, 0x20);

struct bmd_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpBodyMorf;
    /* 0x3D4 */ gptr<mDoExt_baseAnm> mpBrkAnm;
    /* 0x3D8 */ gptr<mDoExt_baseAnm> mpBtkAnm;
    /* 0x3DC */ gptr<mDoExt_McaMorf> mpHeadMorf;
    /* 0x3E0 */ gptr<mDoExt_McaMorf> mpHeadDeadMorf;
    /* 0x3E4 */ gptr<mDoExt_McaMorf> mpMakarMorf;
    /* 0x3E8 */ gptr<J3DModel> mpMakarFaceModel;
    /* 0x3EC */ gptr<J3DModel> mpR00_EFModel;
    /* 0x3F0 */ gptr<mDoExt_baseAnm> mpR00_EFBrk;
    /* 0x3F4 */ be<f32> m2D8;
    /* 0x3F8 */ be<s8> m2DC;
    /* 0x3F9 */ u8 _3F9[3];
    /* 0x3FC */ cXyz m2E0;
    /* 0x408 */ u8 _408[0x416 - 0x408];
    /* 0x416 */ be<s16> m2FA;
    /* 0x418 */ u8 _418[4];
    /* 0x41C */ be<s32> mMakarSnapJnt;   /* HD */
    /* 0x420 */ be<s16> m2FE;
    /* 0x422 */ be<s16> mMode;
    /* 0x424 */ be<s16> m302;
    /* 0x426 */ be<s16> m304;
    /* 0x428 */ be<s16> m306;
    /* 0x42A */ be<s16> m308[4];
    /* 0x432 */ be<s16> m310;
    /* 0x434 */ be<s16> m312;
    /* 0x436 */ be<s16> m314;
    /* 0x438 */ cXyz m318;
    /* 0x444 */ be<f32> m324;
    /* 0x448 */ be<f32> m328;
    /* 0x44C */ u8 _44C[4];
    /* 0x450 */ be<u8> m330;
    /* 0x451 */ be<s8> m331;
    /* 0x452 */ be<s8> m332;
    /* 0x453 */ u8 _453;
    /* 0x454 */ be<s16> m334;
    /* 0x456 */ be<s16> m336;
    /* 0x458 */ be<f32> m338;
    /* 0x45C */ dBgS_AcchCir mAcchCir;
    /* 0x49C */ dBgS_ObjAcch mAcch;
    /* 0x660 */ dCcD_Stts mStts;
    /* 0x69C */ dCcD_Sph mBodySph;
    /* 0x7C8 */ dCcD_Sph mCoreSph;
    /* 0x8F4 */ dCcD_Cyl mCoCyl;
    /* 0xA24 */ be<s16> m904;
    /* 0xA26 */ u8 _A26[2];
    /* 0xA28 */ be<f32> m908;
    /* 0xA2C */ csXyz m90C[2];
    /* 0xA38 */ be<s16> m918;
    /* 0xA3A */ u8 _A3A[2];
    /* 0xA3C */ be<f32> m91C;
    /* 0xA40 */ be<f32> m920;
    /* 0xA44 */ cXyz m924;
    /* 0xA50 */ cXyz m930;
    /* 0xA5C */ be<s16> m93C;
    /* 0xA5E */ be<s16> m93E;
    /* 0xA60 */ be<s16> m940;
    /* 0xA62 */ be<s8> m942;
    /* 0xA63 */ u8 _A63;
    /* 0xA64 */ Mtx34 m944[5];
    /* 0xB54 */ Mtx34 mA34;
    /* 0xB84 */ gptr<dBgW> pm_bgw[6];
    /* 0xB9C */ gptr<JPABaseEmitter> mA7C[3];
    /* 0xBA8 */ be<s16> mA88[2];
    /* 0xBAC */ be<s16> mA8C;
    /* 0xBAE */ u8 _BAE[2];
    /* 0xBB0 */ dPa_smokeEcallBack_l mSmokeCb[7];
    /* 0xC90 */ be<u8> mB70;
    /* 0xC91 */ be<s8> mB71;
    /* 0xC92 */ be<s16> mB72;
    /* 0xC94 */ be<s16> mB74;
    /* 0xC96 */ be<s16> mB76;
    /* 0xC98 */ be<s16> mB78;
    /* 0xC9A */ u8 _C9A[2];
    /* 0xC9C */ cXyz mB7C;
    /* 0xCA8 */ cXyz mB88;
    /* 0xCB4 */ be<s16> mB94;
    /* 0xCB6 */ be<s16> mB96;
    /* 0xCB8 */ u8 _CB8[4];
    /* 0xCBC */ be<f32> mB9C;
    /* 0xCC0 */ be<f32> mBA0;
    /* 0xCC4 */ be<f32> mBA4;
    /* 0xCC8 */ be<f32> mBA8;
    /* 0xCCC */ WIND_INFLUENCE_l mWindInfluence;
    /* 0xCF8 */ be<f32> mBD8;
    /* 0xCFC */ be<f32> mBDC;
    /* 0xD00 */ be<u8> mBE0;
    /* 0xD01 */ u8 _D01[3];
};
WWHD_OFFSET(bmd_class, mpBodyMorf, 0x3D0);
WWHD_OFFSET(bmd_class, m2E0, 0x3FC);
WWHD_OFFSET(bmd_class, m2FE, 0x420);
WWHD_OFFSET(bmd_class, m318, 0x438);
WWHD_OFFSET(bmd_class, mAcchCir, 0x45C);
WWHD_OFFSET(bmd_class, mStts, 0x660);
WWHD_OFFSET(bmd_class, mCoCyl, 0x8F4);
WWHD_OFFSET(bmd_class, m90C, 0xA2C);
WWHD_OFFSET(bmd_class, m944, 0xA64);
WWHD_OFFSET(bmd_class, pm_bgw, 0xB84);
WWHD_OFFSET(bmd_class, mSmokeCb, 0xBB0);
WWHD_OFFSET(bmd_class, mB7C, 0xC9C);
WWHD_OFFSET(bmd_class, mWindInfluence, 0xCCC);
WWHD_OFFSET(bmd_class, mBE0, 0xD00);
WWHD_SIZE(bmd_class, 0xD04);

/* daBmd_HIO_c (HD 0x18: vtable at +0x14) */
struct daBmd_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ u8 _02[2];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<s16> m14;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<u32> __vtbl;
};
WWHD_SIZE(daBmd_HIO_c, 0x18);

/* ---- TU constants ---- */
#define SAFESTRING_VTBL 0x10009AA8
#define BMD_VTBL 0x10009B10
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x10009AD0, 0x10009AF0, 0x10009AE0};
enum { TEV_TYPE_BG1 = 2 };
/* joints (res/Object/Bmd.h) */
enum {
    BKM_COA_JNT_KUBI1_e = 0,
    BKM_COA_JNT_SITAAGO_e = 5,
    BKM_COA_JNT_TOSAKA1_e = 7,
    CB_JNT_BACKBONE_e = 1,
};

/* ---- statics ---- */
static inline daBmd_HIO_c& l_HIO() { return *gabi::at<daBmd_HIO_c>(0x104624A0); }
static inline cXyz* g_pos() { return gabi::at<cXyz>(0x104624C8); } /* daBmd_Draw's static g_pos */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices), user area at +0xB8
 * (as d_a_bb.h's J3DModel_l) */
struct J3DModel_bl {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ be<u32> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline J3DModel_bl* j3dSys_getModel_bl() { return gabi::at<J3DModel_bl>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx_bl() { return gabi::at<Mtx34>(0x104B4868); }
/* J3DModel::getAnmMtx: HD marks the joint matrices dirty */
static inline Mtx34* bl_getAnmMtx(J3DModel_bl* model, s32 jnt) {
    u32 blk = model->mpMtxBlock;
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
static inline Mtx34* bl_getAnmMtx(J3DModel* model, s32 jnt) { return bl_getAnmMtx((J3DModel_bl*)model, jnt); }
/* cXyz assignment through FPRs (lfs/stfs: a signalling NaN is quieted) */
static inline void cXyz_fcopy(cXyz* dst, const cXyz* src) {
    f32 x = src->x, y = src->y, z = src->z;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}
/* 025BEBB8 dSnap_RegistFig(type, actor, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void dSnap_RegistFig_pos(s32 type, fopAc_ac_c* a, cXyz* pos, s16 angY, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, angY, x, y, z);
}
/* 025E1A7C mDoAud_monsSeStart(id, pos, param, reverb) */
static inline void mDoAud_monsSeStart4(u32 id, cXyz* pos, u32 param, s32 reverb) { gabi::call(0x025E1A7C, id, pos, param, reverb); }
/* fopAcM_monsSeStart (HD inline): 025E1AA4 mDoAud_monsSeStart(id, pos, actorId, param, reverb) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
static inline BOOL dScnPly_isPause_bl() { return gabi::call<BOOL>(0x025AF2A4); }
static inline void dKyw_pntwind_set(WIND_INFLUENCE_l* w) { gabi::call(0x0257DC90, w); }
static inline void dKyw_pntwind_cut(WIND_INFLUENCE_l* w) { gabi::call(0x0257D1B8, w); }
static inline void mDoAud_bgmStart(u32 id) { gabi::call(0x025E18EC, id); }
static inline void mDoAud_bgmStop(u32 frames) { gabi::call(0x025E1904, frames); }
static inline void mDoAud_bgmStreamPrepare(u32 id) { gabi::call(0x025E1934, id); }
static inline void mDoAud_bgmStreamPlay() { gabi::call(0x025E1944); }

/* J3DModelData (HD): 027F3F94 returns the joint tree, joint count (u16) at +8; joint nodes of
 * 0x1C bytes from +8, index checked against +4 (out of range: node 0) (as d_a_kb.h) */
static inline u16 J3DModelData_getJointNum_bl(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
static inline void setJointCallBack_bl(J3DModelData* d, u16 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* joint names: 027F68FC returns the name table header, 027DF9B0 JUTNameTab::getIndex (as d_a_npc_people.h) */
static inline u32 J3DModelData_getJointName_bl(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s32 JUTNameTab_getIndex_bl(u32 tab, const char* name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* mDoExt_brkAnm (HD 0x78): constructor 025E80D0, init 025E8154; mDoExt_btkAnm (0x74): 025E7C6C, 025E7CE0 */
static inline void* mDoExt_brkAnm_ct_bl(void* p) { return gabi::call<void*>(0x025E80D0, p); }
static inline BOOL mDoExt_brkAnm_init_bl(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                         bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, mode, rate, start, end, modify, entry);
}
static inline void* mDoExt_btkAnm_ct_bl(void* p) { return gabi::call<void*>(0x025E7C6C, p); }
static inline BOOL mDoExt_btkAnm_init_bl(void* a, J3DModelData* d, void* btk, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                         bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E7CE0, a, d, btk, anmPlay, mode, rate, start, end, modify, entry);
}
/* dBgW (HD): crr function at +0xA8, ride callback at +0xB0 */
static inline void dBgW_SetCrrFunc(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xA8, fn); }
static inline void dBgW_SetRideCallback(dBgW* w, u32 fn) { gabi::store<u32>(gabi::ea(w) + 0xB0, fn); }
/* save: dComIfGs_isStageBossEnemy / isStageBossDemo = dSv_memBit_c::isDungeonItem(save + 0x798, 3 / 5) */
static inline u32 save_bl() { return gabi::load<u32>(0x101F84DC); }
static inline BOOL dComIfGs_isDungeonItem_bl(s32 i) { return gabi::call<BOOL>(0x025B9100, save_bl() + 0x798, i); }
static inline void dComIfGs_onDungeonItem_bl(s32 i) { gabi::call(0x025B9098, save_bl() + 0x798, i); }
static inline BOOL dComIfGs_checkGetItem_bl(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* dComIfGs_offTmpBit(flag): dSv_event_c::offEventBit(save + 0x1178, flag) */
static inline void dComIfGs_offTmpBit_bl(u16 flag) { gabi::call(0x025B8B7C, save_bl() + 0x1178, flag); }
static inline u8 dComIfGp_getStartStageName0() { return gabi::load<u8>(dComIfGp_ea() + 0x5134); }
/* fopAcM_Create(name, NULL, append): fpcSCtRq_Request(fpcLy_CurrentLayer(), name, 0, 0, append) */
static inline u8* fopAcM_CreateAppend_bl() { return gabi::call<u8*>(0x025D5600); }
static inline u32 fpcLy_CurrentLayer_bl() { return gabi::call<u32>(0x025DED64); }
static inline u32 fpcSCtRq_Request_bl(u32 layer, s16 name, u32 a, u32 b, void* params) { return gabi::call<u32>(0x025E14A8, layer, name, a, b, params); }

/* dComIfGp_getCamera(0): camera_process_class* at play+0x5AF8 (dCamera_c at +0x248) */
static inline u32 dComIfGp_getCamera0_bl() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }

/* ---- functions of this unit called from other files ---- */
void anm_init(bmd_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx);
void move1(bmd_class* i_this);
void mk_voice_set(bmd_class* i_this, u32 param_2);
void move(bmd_class* i_this);
void demo_camera(bmd_class* i_this);
