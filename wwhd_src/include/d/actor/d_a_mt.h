/* mt_class (Magtail), WWHD layout. 
 *
 * The GameCube decompilation has only a placeholder layout (size 0x1CC0, m454); every member
 * here was measured from the WWHD code: the constructor 021DCD10 (allocates 0x1DD8), the
 * deleting destructor 021DFE08, daMt_Create, CallbackCreateHeap, the node callbacks and Draw.
 * A Magtail is 8 segments (head, 6 body segments, tail), each with its own morf/btk/brk.
 *  - +0x11C up to the end of fopEn_enemy_c (0x3C8);
 *  - enemyice (0x3B8, as in d_a_cc/d_a_bk) at 0x1A1C, its mpActor written by daMt_Create. */
#pragma once
#include "bindings.h"


#ifndef WWHD_ENEMYICE_L
#define WWHD_ENEMYICE_L
/* enemyice (c_damagereaction), WWHD layout as in d_a_cc.h (SHARED-CANDIDATE) */
struct enemyice_l {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFreezeDuration;
    /* 0x006 */ be<s8> mLightShrinkTimer;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<f32> mYOffset;
    /* 0x00C */ be<s8> m00C;
    /* 0x00D */ be<s8> mMode;
    /* 0x00E */ be<s16> mFreezeTimer;
    /* 0x010 */ be<s16> mMoveDelayTimer;
    /* 0x012 */ be<s16> mAngleY;
    /* 0x014 */ be<s16> mAngularVelY;
    /* 0x016 */ u8 _016[2];
    /* 0x018 */ cXyz mSpeed;
    /* 0x024 */ be<f32> mSpeedF;
    /* 0x028 */ be<f32> m028;
    /* 0x02C */ be<f32> m02C;
    /* 0x030 */ dCcD_Stts mStts;
    /* 0x06C */ dCcD_Cyl mCyl;
    /* 0x19C */ be<f32> mCylHeight;
    /* 0x1A0 */ be<f32> mWallRadius;
    /* 0x1A4 */ be<f32> mScaleXZ;
    /* 0x1A8 */ be<f32> mScaleY;
    /* 0x1AC */ be<f32> mParticleScale;
    /* 0x1B0 */ be<u8> m1B0;
    /* 0x1B1 */ be<u8> mDeathSwitch;
    /* 0x1B2 */ u8 _1B2[2];
    /* 0x1B4 */ dBgS_AcchCir mBgAcchCir;
    /* 0x1F4 */ dBgS_ObjAcch mBgAcch;
};
WWHD_SIZE(enemyice_l, 0x3B8);
#endif


enum { MT_PART_NUM = 8, MT_JOINT_NUM = 64 };

struct mt_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<u8> m3D0;          /* param & 0xFF (0xFF -> 0): type; >= 10 rolls */
    /* 0x3D1 */ be<u8> m3D1;          /* (param >> 8) & 0x7F */
    /* 0x3D2 */ be<u8> m3D2;          /* (param >> 15) & 1: angle.z holds m3D6, not a switch */
    /* 0x3D3 */ be<u8> m3D3;          /* param >> 16: path index (0xFF none) */
    /* 0x3D4 */ be<u8> m3D4;          /* param >> 24: switch (0xFF none) */
    /* 0x3D5 */ be<u8> m3D5;          /* angle.z & 0xFF: death switch */
    /* 0x3D6 */ be<u8> m3D6;
    /* 0x3D7 */ be<u8> m3D7;          /* m3D4 + 1: hidden (not drawn) while set */
    /* 0x3D8 */ be<u8> m3D8;          /* path index + 1 */
    /* 0x3D9 */ be<u8> m3D9;
    /* 0x3DA */ be<u8> m3DA;
    /* 0x3DB */ u8 _3DB;
    /* 0x3DC */ gptr<dPath> ppd;
    /* 0x3E0 */ gptr<mDoExt_McaMorf> mpMorf[MT_PART_NUM];
    /* 0x400 */ be<u8> m400;          /* btk/brk frames: 0 per-segment offsets, else shared */
    /* 0x401 */ u8 _401[3];
    /* 0x404 */ be<s32> m404;         /* brk frame */
    /* 0x408 */ be<s32> m408;         /* btk frame */
    /* 0x40C */ gptr<mDoExt_btkAnm> mpBtk[MT_PART_NUM];
    /* 0x42C */ gptr<mDoExt_brkAnm> mpBrk[MT_PART_NUM];
    /* 0x44C */ be<f32> m44C;          /* body_control3: sideways wave amplitude */
    /* 0x450 */ be<s16> m450;         /* Execute: joint wave step */
    /* 0x452 */ u8 _452[2];
    /* 0x454 */ be<f32> m454;
    /* 0x458 */ gptr<J3DAnmTexPattern> mpBtpRes;
    /* 0x45C */ gptr<mDoExt_btpAnm> mpBtp;
    /* 0x460 */ be<u8> mBtpFrame;
    /* 0x461 */ be<u8> mBtpOn;
    /* 0x462 */ be<u8> mBtpMaxFrame;
    /* 0x463 */ u8 _463;
    /* 0x464 */ be<u8> m464;          /* water damage SE played */
    /* 0x465 */ u8 _465;
    /* 0x466 */ be<s16> m466;         /* water steam emitter timer */
    /* 0x468 */ be<u8> m468;          /* Execute: segment follow particles started */
    /* 0x469 */ u8 _469[3];
    /* 0x46C */ cXyz m46C[MT_PART_NUM]; /* Execute: segment particle positions */
    /* 0x4CC */ dPa_followEcallBack mFollowCb[MT_PART_NUM];
    /* 0x56C */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x570 */ be<u8> m570;          /* state: 1 counted by mt_a_d_sub, 3 after bakuha */
    /* 0x571 */ be<u8> m571;
    /* 0x572 */ be<s16> m572;         /* mt_move_maru: no body collision timer */
    /* 0x574 */ be<s16> m574;
    /* 0x576 */ be<s16> m576;
    /* 0x578 */ be<s16> m578;         /* Execute: idle voice timer */
    /* 0x57A */ be<s16> m57A;         /* Execute: idle sound timer */
    /* 0x57C */ be<s16> m57C;         /* body_control2: segment colliders shrink (radius -200) */
    /* 0x57E */ be<s16> m57E;         /* Execute: shake timer */
    /* 0x580 */ be<s16> m580;
    /* 0x582 */ be<s16> m582;         /* crawl wave phase */
    /* 0x584 */ be<s16> m584;         /* head wobble angle (body_control5) */
    /* 0x586 */ be<s16> m586;         /* wobble phase */
    /* 0x588 */ be<s16> m588;         /* mt_move: frames without wall contact */
    /* 0x58A */ be<s16> m58A;         /* Execute: joint wave phase */
    /* 0x58C */ be<f32> m58C;
    /* 0x590 */ be<f32> m590;         /* wobble amplitude */
    /* 0x594 */ be<f32> m594;         /* body_control3: segment offset */
    /* 0x598 */ cXyz m598;            /* first path point */
    /* 0x5A4 */ be<s16> m5A4;
    /* 0x5A6 */ be<s16> m5A6;         /* body_control3: per-segment pitch step */
    /* 0x5A8 */ be<s16> m5A8;         /* mt_move: wall direction */
    /* 0x5AA */ be<s16> m5AA;         /* body_control2: no sideways wave while set */
    /* 0x5AC */ be<s16> m5AC;         /* Execute: joint wave extent */
    /* 0x5AE */ be<s16> m5AE;         /* mt_move: climb timer */
    /* 0x5B0 */ be<s16> m5B0;         /* mt_fight: pitch to the player */
    /* 0x5B2 */ be<s16> m5B2;         /* mt_fight: yaw to the player */
    /* 0x5B4 */ u8 _5B4[2];
    /* 0x5B6 */ csXyz m5B6;           /* Execute: shake angle (added to shape_angle) */
    /* 0x5BC */ cXyz m5BC[MT_PART_NUM]; /* segment positions */
    /* 0x61C */ cXyz m61C[MT_PART_NUM]; /* previous segment positions */
    /* 0x67C */ csXyz m67C[MT_PART_NUM]; /* segment angles */
    /* 0x6AC */ cXyz m6AC[MT_PART_NUM]; /* bakuha: scatter speeds */
    /* 0x70C */ be<s16> m70C[MT_PART_NUM];
    /* 0x71C */ be<f32> m71C[MT_PART_NUM];
    /* 0x73C */ be<f32> m73C[MT_PART_NUM];
    /* 0x75C */ csXyz m75C[15];         /* joint angles (node callbacks: -x -> Y, -z -> Z) */
    /* 0x7B6 */ csXyz m7B6[15];         /* joint angles (x -> Y, -z -> Z) */
    /* 0x810 */ cXyz m810[MT_JOINT_NUM]; /* trail positions */
    /* 0xB10 */ csXyz mB10[MT_JOINT_NUM]; /* trail angles */
    /* 0xC90 */ be<s16> mC90[MT_JOINT_NUM]; /* trail head wobble */
    /* 0xD10 */ be<s32> mD10;
    /* 0xD14 */ u8 _D14[0xD1C - 0xD14];
    /* 0xD1C */ be<u8> mD1C;          /* body_control2: trail table select */
    /* 0xD1D */ be<u8> mD1D;          /* body_control2: write the trail (m810/mB10), then reset */
    /* 0xD1E */ be<s16> mD1E;         /* mt_fight: attack frame */
    /* 0xD20 */ be<u8> mD20;          /* head collider mode */
    /* 0xD21 */ u8 _D21[3];
    /* 0xD24 */ dBgS_AcchCir mAcchCir;
    /* 0xD64 */ dBgS_ObjAcch mAcch;
    /* 0xF28 */ dCcD_Stts mStts;
    /* 0xF64 */ dCcD_Sph mSph[MT_PART_NUM];
    /* 0x18C4 */ dCcD_Sph mAtSph;
    /* 0x19F0 */ be<u8> m19F0;
    /* 0x19F1 */ u8 _19F1[3];
    /* 0x19F4 */ gptr<J3DModel> mpBrModelA[3];
    /* 0x1A00 */ gptr<J3DModel> mpBrModelB[3];
    /* 0x1A0C */ be<f32> mHeadScale;
    /* 0x1A10 */ be<f32> m1A10;
    /* 0x1A14 */ be<s8> m1A14;
    /* 0x1A15 */ be<u8> m1A15;         /* Execute: line of sight to the player blocked */
    /* 0x1A16 */ be<u8> m1A16;
    /* 0x1A17 */ be<u8> m1A17;
    /* 0x1A18 */ be<s16> m1A18;
    /* 0x1A1A */ u8 _1A1A[2];
    /* 0x1A1C */ enemyice_l mEnemyIce;
    /* 0x1DD4 */ be<s8> m1DD4;          /* deleted by mt_bg_check (fell out) */
    /* 0x1DD5 */ u8 _1DD5[3];
};
WWHD_OFFSET(mt_class, mPhs, 0x3C8);
WWHD_OFFSET(mt_class, mpMorf, 0x3E0);
WWHD_OFFSET(mt_class, mFollowCb, 0x4CC);
WWHD_OFFSET(mt_class, m75C, 0x75C);
WWHD_OFFSET(mt_class, m810, 0x810);
WWHD_OFFSET(mt_class, mAcchCir, 0xD24);
WWHD_OFFSET(mt_class, mSph, 0xF64);
WWHD_OFFSET(mt_class, mAtSph, 0x18C4);
WWHD_OFFSET(mt_class, mEnemyIce, 0x1A1C);
WWHD_OFFSET(mt_class, m46C, 0x46C);
WWHD_OFFSET(mt_class, m5B6, 0x5B6);
WWHD_OFFSET(mt_class, mC90, 0xC90);
WWHD_OFFSET(mt_class, mD1E, 0xD1E);
WWHD_SIZE(mt_class, 0x1DD8);

/* daMt_HIO_c (0x60, vtable 0x1001564C); field names unknown (GameCube placeholder) */
struct daMt_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ be<u8> m05;
    /* 0x06 */ be<u8> m06;
    /* 0x07 */ be<u8> m07;
    /* 0x08 */ be<s16> m08;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<f32> m0C;
    /* 0x10 */ be<s16> m10;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> m14;
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<f32> m1C;            /* Draw: rubble model scale */
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<f32> m2C;
    /* 0x30 */ be<s16> m30;
    /* 0x32 */ be<s16> m32;
    /* 0x34 */ be<f32> m34;
    /* 0x38 */ be<s16> m38;
    /* 0x3A */ be<s16> m3A;
    /* 0x3C */ be<s16> m3C;
    /* 0x3E */ be<s16> m3E;
    /* 0x40 */ be<f32> m40;
    /* 0x44 */ be<f32> m44;
    /* 0x48 */ be<f32> m48;
    /* 0x4C */ be<f32> m4C;
    /* 0x50 */ be<s16> m50;            /* Draw: per-segment btk/brk frame step */
    /* 0x52 */ be<s16> m52;
    /* 0x54 */ be<s16> m54;
    /* 0x56 */ u8 _56[2];
    /* 0x58 */ be<f32> m58;
    /* 0x5C */ be<f32> m5C;
};
WWHD_SIZE(daMt_HIO_c, 0x60);

/* ---- this TU's statics and .data ---- */
#define MT_SAFESTRING_VTBL 0x10015494
#define MT_VTBL 0x1001565C        /* mt_class vtable (HD virtual destructor) */
#define MT_HIO_VTBL 0x1001564C
#define MT_AAB_VTBL 0x100154AC    /* this TU's cM3dGAab vtable */
inline daMt_HIO_c& l_HIO() { return *gabi::at<daMt_HIO_c>(0x10465870); }
/* nodeCallBack_body: index of the segment being calculated (static int, reset by Draw) */
inline be<s32>& body_idx() { return *gabi::at<be<s32>>(0x10465850); }
/* mt_a_d_sub counters */
inline be<s32>& mt_all_count() { return *gabi::at<be<s32>>(0x10465848); }
inline be<s32>& mt_fight_count() { return *gabi::at<be<s32>>(0x1046584C); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD debug-register floats (g_regHIO): REG_F base 0x1047B610 in bindings.h; mt reads the words
 * before it too (0x1047B608 + 0x18 == REG0_F(4)) */
inline f32 reg_f(u32 addr) { return gabi::load<f32>(addr); }
/* JPABaseEmitter::becomeInvalidEmitter() (HD inline): flags (+0x254) &= ~0x40, maxFrame (+0x5C)
 * = -1, flags |= 1 */
inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 a = gabi::ea(e);
    gabi::store<u32>(a + 0x254, gabi::load<u32>(a + 0x254) & ~0x40u);
}
inline u8* fopAcM_CreateAppend() { return gabi::call<u8*>(0x025D5600); }
inline u32 fpcLy_CurrentLayer() { return gabi::call<u32>(0x025DED64); }
inline u32 fpcSCtRq_Request(u32 layer, s16 name, u32 a, u32 b, void* params) { return gabi::call<u32>(0x025E14A8, layer, name, a, b, params); }
inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 n) { return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + 0x50AC), name, n); }
inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* 024F06B4 dBgS_Acch::Set(pos, old, actor, cirNum, cir, speed, angle, shapeAngle) */
inline void dBgS_Acch_Set(void* acch, cXyz* pos, cXyz* old, fopAc_ac_c* a, s32 n, dBgS_AcchCir* cir, cXyz* spd, csXyz* ang, csXyz* shape) {
    gabi::call(0x024F06B4, acch, pos, old, a, n, cir, spd, ang, shape);
}
/* 024EFF44 dBgS_AcchCir::SetWall(h, r) */
inline void dBgS_AcchCir_SetWall(dBgS_AcchCir* c, f32 h, f32 r) { gabi::call(0x024EFF44, c, h, r); }
/* 025E7820 mDoExt_btpAnm::mDoExt_btpAnm (allocates 0x74 when this == NULL) */
inline mDoExt_btpAnm* mDoExt_btpAnm_ct(mDoExt_btpAnm* p) { return gabi::call<mDoExt_btpAnm*>(0x025E7820, p); }
/* 025E789C mDoExt_btpAnm::init(modelData, btp, anmPlay, attr, speed, start, end, modify, entry) */
inline BOOL mDoExt_btpAnm_init(mDoExt_btpAnm* a, J3DModelData* d, J3DAnmTexPattern* btp, s32 anmPlay, s32 attr, f32 speed,
                               s16 start, s16 end, s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E789C, a, d, btp, anmPlay, attr, speed, start, end, modify, entry);
}
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (HD size 0x78); 025E8154 init(data, brk, play, mode, speed, start, end, modify, entry) */
inline mDoExt_brkAnm* mDoExt_brkAnm_ct(mDoExt_brkAnm* p) { return gabi::call<mDoExt_brkAnm*>(0x025E80D0, p); }
inline BOOL mDoExt_brkAnm_init(mDoExt_brkAnm* a, J3DModelData* d, J3DAnmTevRegKey* k, s32 play, s32 mode, f32 speed, s16 start, s16 end,
                               s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, speed, start, end, modify, entry);
}
/* 027F3F94 (the matcher's __nw): J3DModelData joint tree header (HD; +8 joint count) */
inline u32 modelData_getJointTree(J3DModelData* md) { return gabi::call<u32>(0x027F3F94, md); }

/* dBgS_LinChk on the stack (HD layout, as in d_a_mo2; this TU's vtables) */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;     /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;     /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);
struct LinChkVtbls { u32 v10, v20, v58, v64, dt64, dt20; };
#define MT_LINCHK_VTBLS (LinChkVtbls{0x100155CC, 0x100155DC, 0x100155FC, 0x100155EC, 0x100154CC, 0x100154BC})
inline void dBgS_LinChk_ct(dBgS_LinChk_l* c, LinChkVtbls v) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = v.v10;
    c->__vtbl_64 = v.v64;
    c->__vtbl_58 = v.v58;
    c->__vtbl_20 = v.v20;
}
inline void dBgS_LinChk_dt(dBgS_LinChk_l* c, LinChkVtbls v) {
    c->__vtbl_58 = v.v58;
    c->__vtbl_64 = v.dt64;
    c->__vtbl_20 = v.dt20;
    cBgS_LinChk_dt(c, 0);
}

/* HD J3D (as in d_a_mo2/d_a_kamome): j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at
 * 0x104B4868; a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices);
 * user area +0xB8 */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ be<u32> mUserArea;
};
inline J3DModel_l* j3dSys_getModel() { return gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C)); }
inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* getAnmMtx: HD marks the joint matrices dirty */
inline Mtx34* model_getAnmMtx(J3DModel_l* model, s32 jntNo) {
    J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
/* J3DModelData::getJointNodePointer(i) (HD inline): count at +4, nodes (0x1C each) at +8; an
 * index out of range gives the first node */
inline u32 modelData_getJointNode(J3DModelData* md, u32 i) {
    u32 num = gabi::load<u32>(gabi::ea(md) + 4);
    u32 base = gabi::load<u32>(gabi::ea(md) + 8);
    return i < num ? base + i * 0x1C : base;
}

/* ---- functions of the unit (natural calls between parts; see d_a_mt_pending.cpp) ---- */
void anm_init(mt_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx);
void mt_bg_check(mt_class* i_this);
void tex_anm_set(mt_class* i_this, u16 idx);
void body_wall_check(mt_class* i_this);
void bakuha(mt_class* i_this);
void water_damage_se_set(mt_class* i_this);
void body_control2(mt_class* i_this);
void body_control3(mt_class* i_this);
void body_control4(mt_class* i_this);
void body_control5(mt_class* i_this);
void mt_move_maru(mt_class* i_this);
BOOL daMt_Execute(mt_class* i_this);
