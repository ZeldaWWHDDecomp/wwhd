/* kamome_class (seagull), WWHD layout. 
 *
 * GameCube -> WWHD: +0x11C up to mpMorf; mShadowId (GameCube 0x29C) is gone (HD shadows), so the
 * fields after it are +0x118; four more bytes go before the collision members (+0x114 from
 * mAcchCir on). Offsets from the verified functions. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"

enum { daPyStts0_SHIP_RIDE_e = 0x00010000 };

struct kamome_class {
    /* 0x000 */ fopAc_ac_c actor;
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ be<u8> mType;
    /* 0x3B9 */ be<u8> mKoMaxCount;
    /* 0x3BA */ be<u8> mPathIdx;
    /* 0x3BB */ be<u8> mSwitchNoPrm;
    /* 0x3BC */ be<s32> mGlobalTimer;
    /* 0x3C0 */ be<s8> mAnimState;
    /* 0x3C1 */ be<s8> mMoveState;
    /* 0x3C2 */ be<u8> mSwitchNo;
    /* 0x3C3 */ be<s8> m2AB;
    /* 0x3C4 */ be<s16> mJointRotY;
    /* 0x3C6 */ be<s16> mJointRotZ;
    /* 0x3C8 */ be<s16> mJointRotYTarget;
    /* 0x3CA */ be<s16> mJointRotZTarget;
    /* 0x3CC */ be<s16> m2B4;
    /* 0x3CE */ u8 _3CE[2];
    /* 0x3D0 */ be<f32> m2B8;
    /* 0x3D4 */ be<s8> mbNoDraw;
    /* 0x3D5 */ u8 _3D5[3];
    /* 0x3D8 */ cXyz mTargetPos;
    /* 0x3E4 */ be<f32> mVelocityFwdTarget;
    /* 0x3E8 */ be<f32> mVelocityFwdTargetMaxVel;
    /* 0x3EC */ be<f32> mRotVelFade;
    /* 0x3F0 */ be<f32> mRotVel;
    /* 0x3F4 */ u8 _3F4[4];
    /* 0x3F8 */ be<s16> mTimers[6];
    /* 0x404 */ be<s16> mGroundKeepTimer; /* HD: kamome_auto_move keeps the gull above the ground while it runs */
    /* 0x406 */ be<s16> mRiseTimer;
    /* 0x408 */ be<f32> mScale;
    /* 0x40C */ be<u32> mEsaProcID;
    /* 0x410 */ gptr<fopAc_ac_c> mpTargetActor;
    /* 0x414 */ be<s16> m2FC;
    /* 0x416 */ be<s8> mbUsePathMovement;
    /* 0x417 */ be<s8> mCurPointIdx;
    /* 0x418 */ be<s8> mPathIdxIncr;
    /* 0x419 */ u8 _419[3];
    /* 0x41C */ gptr<dPath> mpPath;
    /* 0x420 */ be<u8> m308;
    /* 0x421 */ u8 _421[0x430 - 0x421];
    /* 0x430 */ dBgS_AcchCir mAcchCir;
    /* 0x470 */ dBgS_ObjAcch mAcch;
    /* 0x634 */ dCcD_Stts mStts;
    /* 0x670 */ dCcD_Sph mSph;
    /* 0x79C */ be<u8> m688;
    /* 0x79D */ u8 _79D[3];
};
WWHD_OFFSET(kamome_class, mTargetPos, 0x3D8);
WWHD_OFFSET(kamome_class, mScale, 0x408);
WWHD_OFFSET(kamome_class, mpPath, 0x41C);
WWHD_OFFSET(kamome_class, mAcch, 0x470);
WWHD_OFFSET(kamome_class, mSph, 0x670);
WWHD_OFFSET(kamome_class, m688, 0x79C);


// HD: actor-private layouts and exact virtual metadata.
struct kamomeHIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ be<s16> m06;
    /* 0x04 */ be<s16> m08;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<u32> __vtbl; /* GHS: vtable pointer after the members */
};

struct esa_class {
    fopAc_ac_c actor;
    /* 0x3AC */ u8 _3AC[8];
    /* 0x3B4 */ be<u8> field_0x298;
    /* 0x3B5 */ u8 _3B5[0x3BD - 0x3B5];
    /* 0x3BD */ be<s8> mState;
};
WWHD_OFFSET(esa_class, mState, 0x3BD);

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

struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;     /* dBgS_PolyPassChk */
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;     /* dBgS_GrpPassChk */
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_GndChk_l, 0x54);

struct dPnt_l {
    /* 0x0 */ u8 _0[3];
    /* 0x3 */ be<u8> mArg3;
    /* 0x4 */ cXyz m_position;
};

struct dPath_l {
    /* 0x0 */ be<u16> m_num;
    /* 0x2 */ be<u16> m_nextID;
    /* 0x4 */ u8 _4;
    /* 0x5 */ be<u8> m_closed;
    /* 0x6 */ u8 _6[2];
    /* 0x8 */ gptr<dPnt_l> m_points;
};

static void dBgS_LinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x100123AC;
    c->__vtbl_64 = 0x100123CC;
    c->__vtbl_58 = 0x100123DC;
    c->__vtbl_20 = 0x100123BC;
}

static void dBgS_LinChk_dt(dBgS_LinChk_l* c) {
    c->__vtbl_58 = 0x100123DC;
    c->__vtbl_64 = 0x1001232C;
    c->__vtbl_20 = 0x1001231C;
    cBgS_LinChk_dt(c, 0);
}

static void dBgS_GndChk_ct(dBgS_GndChk_l* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->__vtbl_10 = 0x1001233C;
    c->__vtbl_4C = 0x1001235C;
    c->__vtbl_40 = 0x1001236C;
    c->__vtbl_20 = 0x1001234C;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
}

static void dBgS_GndChk_dt(dBgS_GndChk_l* c) {
    c->__vtbl_20 = 0x1001234C;
    c->__vtbl_40 = 0x1001236C;
    c->__vtbl_4C = 0x1001232C;
    gabi::call(0x02008DAC, c, 0); /* destructor */
}

static inline void ground_chk_dt(dBgS_GndChk_l* chk) {
    chk->__vtbl_20 = 0x1001234C;
    chk->__vtbl_40 = 0x1001236C;
    chk->__vtbl_4C = 0x1001232C;
    gabi::call(0x02008DAC, chk, 0); /* cBgS_Chk::~cBgS_Chk */
}
extern kamomeHIO_c& l_kamomeHIO;
extern be<u8>& hio_set;
extern be<s32>& esa_check_count;
extern be<s32>& ko_count;
extern gptr<fopAc_ac_c> (&esa_info)[100];
WWHD_OFFSET(kamome_class, actor, 0x000);
WWHD_OFFSET(kamome_class, mPhase, 0x3AC);
WWHD_OFFSET(kamome_class, mpMorf, 0x3B4);
WWHD_OFFSET(kamome_class, mType, 0x3B8);
WWHD_OFFSET(kamome_class, mKoMaxCount, 0x3B9);
WWHD_OFFSET(kamome_class, mPathIdx, 0x3BA);
WWHD_OFFSET(kamome_class, mSwitchNoPrm, 0x3BB);
WWHD_OFFSET(kamome_class, mGlobalTimer, 0x3BC);
WWHD_OFFSET(kamome_class, mAnimState, 0x3C0);
WWHD_OFFSET(kamome_class, mMoveState, 0x3C1);
WWHD_OFFSET(kamome_class, mSwitchNo, 0x3C2);
WWHD_OFFSET(kamome_class, m2AB, 0x3C3);
WWHD_OFFSET(kamome_class, mJointRotY, 0x3C4);
WWHD_OFFSET(kamome_class, mJointRotZ, 0x3C6);
WWHD_OFFSET(kamome_class, mJointRotYTarget, 0x3C8);
WWHD_OFFSET(kamome_class, mJointRotZTarget, 0x3CA);
WWHD_OFFSET(kamome_class, m2B4, 0x3CC);
WWHD_OFFSET(kamome_class, _3CE, 0x3CE);
WWHD_OFFSET(kamome_class, m2B8, 0x3D0);
WWHD_OFFSET(kamome_class, mbNoDraw, 0x3D4);
WWHD_OFFSET(kamome_class, _3D5, 0x3D5);
WWHD_OFFSET(kamome_class, mVelocityFwdTarget, 0x3E4);
WWHD_OFFSET(kamome_class, mVelocityFwdTargetMaxVel, 0x3E8);
WWHD_OFFSET(kamome_class, mRotVelFade, 0x3EC);
WWHD_OFFSET(kamome_class, mRotVel, 0x3F0);
WWHD_OFFSET(kamome_class, _3F4, 0x3F4);
WWHD_OFFSET(kamome_class, mTimers, 0x3F8);
WWHD_OFFSET(kamome_class, mGroundKeepTimer, 0x404);
WWHD_OFFSET(kamome_class, mRiseTimer, 0x406);
WWHD_OFFSET(kamome_class, mEsaProcID, 0x40C);
WWHD_OFFSET(kamome_class, mpTargetActor, 0x410);
WWHD_OFFSET(kamome_class, m2FC, 0x414);
WWHD_OFFSET(kamome_class, mbUsePathMovement, 0x416);
WWHD_OFFSET(kamome_class, mCurPointIdx, 0x417);
WWHD_OFFSET(kamome_class, mPathIdxIncr, 0x418);
WWHD_OFFSET(kamome_class, _419, 0x419);
WWHD_OFFSET(kamome_class, m308, 0x420);
WWHD_OFFSET(kamome_class, _421, 0x421);
WWHD_OFFSET(kamome_class, mAcchCir, 0x430);
WWHD_OFFSET(kamome_class, mStts, 0x634);
WWHD_OFFSET(kamome_class, _79D, 0x79D);
WWHD_SIZE(kamome_class, 0x7A0);

void kamomeMain_sphereCtor(dCcD_Sph* sphere);
void kamomeMain_registerGlobal(u32 record);
void kamomeMain_initAngle(u32 storage);
void kamomeMain_initFlag(u32 storage);
void kamomeAuto_animation(kamome_class* actor, int resource, f32 blend, u8 mode, f32 speed, int sound);
fopAc_ac_c* kamomeAuto_bait(kamome_class* actor);
void kamomeAuto_flight(kamome_class* actor);
void kamomeAuto_background(kamome_class* actor);
void kamomeAuto_groundMove(kamome_class* actor);
void kamomeAuto_sphereDtor(dCcD_Sph* sphere,s32 flags);
void kamomeAuto_acchDtor(dBgS_ObjAcch* acch,s32 flags);
void kamomeAuto_circleDtor(void* circle,s32 flags);
void kamomeAuto_actorDtor(fopAc_ac_c* actor,s32 flags);
void kamomeExecute_animation(kamome_class* actor, int resource, f32 blend, u8 mode, f32 speed, int sound);
fopAc_ac_c* kamomeExecute_bait(kamome_class* actor);
void kamomeExecute_flight(kamome_class* actor);
void kamomeExecute_background(kamome_class* actor);
void kamomeExecute_groundMove(kamome_class* actor);
void kamomeExecute_matrix(kamome_class* actor);
void kamomeExecute_autoMove(kamome_class* actor);
void kamomeExecute_groundCtor(dBgS_GndChk_l* check);
f32 kamomeExecute_groundCross(dBgS* bg,dBgS_GndChk_l* check);
inline esa_class* kamomeBait(fopAc_ac_c* bait) { return reinterpret_cast<esa_class*>(bait); }
struct KamomeJointView { u8 mUnaccessed[4]; be<u16> mJntNo; };
WWHD_OFFSET(KamomeJointView,mJntNo,4); WWHD_SIZE(KamomeJointView,6);
inline KamomeJointView* kamomeJoint(J3DJoint* joint) { return reinterpret_cast<KamomeJointView*>(joint); }
enum : u32 { KAMOME_ACTOR_VTABLE=0x100123FC, KAMOME_HIO_VTABLE=0x100123EC, KAMOME_ACCH_VTABLE=0x1001238C, KAMOME_POLY_VTABLE=0x1001239C };
inline void kamomeAcchCtor(dBgS_ObjAcch* acch) { const dBgS_ObjAcch_vt table={0x1001237C,0x1001239C,0x1001238C}; dBgS_ObjAcch_ct(acch,table); }

// HD: path lookup uses the stage's virtual getPathInf slot.
struct KamomeStageVtable { u8 mUnaccessed[0x18C]; be<u32> getPathInf; };
WWHD_OFFSET(KamomeStageVtable,getPathInf,0x18C); WWHD_SIZE(KamomeStageVtable,0x190);
struct KamomePathInfo { be<u16> mCount; u8 mPadding[2]; gptr<dPath_l> m_path; };
WWHD_OFFSET(KamomePathInfo,m_path,4); WWHD_SIZE(KamomePathInfo,8);
inline dPath* kamomeNextPath(u16 index) {
    u32 stage=dComIfGp_ea()+PLAY_STAGEDATA;
    u32 fn=gabi::load<u32>(gabi::load<u32>(stage)+offsetof(KamomeStageVtable,getPathInf));
    KamomePathInfo* info=gabi::call_ptr<KamomePathInfo*>(fn,stage);
    return gabi::at<dPath>(gabi::ea(info->m_path.get())+index*sizeof(dPath_l));
}
inline dPnt_l* kamomePathPoint(dPath_l* path,s8 index) { return gabi::at<dPnt_l>(gabi::ea(path->m_points.get())+index*sizeof(dPnt_l)); }
enum : u32 { KAMOME_CHILD_SEARCH=0x02185FE4,KAMOME_MASTER_SEARCH=0x02186050,KAMOME_ARYLL_SEARCH=0x02185960 };

enum { fpcNm_KAMOME_e=0xC2, fpcNm_ESA_e=0xDD, fpcNm_NPC_LS1_e=0x141, DSNAP_TYPE_KAMOME=0x55, JA_SE_CV_KAMOME=0x4805 };
enum { dRes_INDEX_KAMOME_BAS_KA_FLY1_e=5,dRes_INDEX_KAMOME_BAS_KA_LAND1_e=6,dRes_INDEX_KAMOME_BAS_KA_MOVE1_e=7,dRes_INDEX_KAMOME_BAS_KA_WAIT1_e=8,dRes_INDEX_KAMOME_BAS_KA_WAIT2_e=9,dRes_INDEX_KAMOME_BCK_KA_EAT1_e=0xC,dRes_INDEX_KAMOME_BCK_KA_FLY1_e=0xD,dRes_INDEX_KAMOME_BCK_KA_LAND1_e=0xE,dRes_INDEX_KAMOME_BCK_KA_MOVE1_e=0xF,dRes_INDEX_KAMOME_BCK_KA_SING1_e=0x10,dRes_INDEX_KAMOME_BCK_KA_SING2_e=0x11,dRes_INDEX_KAMOME_BCK_KA_WAIT1_e=0x12,dRes_INDEX_KAMOME_BCK_KA_WAIT2_e=0x13,dRes_INDEX_KAMOME_BCK_KA_WAIT3_e=0x14,dRes_INDEX_KAMOME_BDL_KA_e=0x17 };

WWHD_SIZE(kamomeHIO_c, 0x14);
WWHD_OFFSET(kamomeHIO_c, __vtbl, 0x10);
WWHD_OFFSET(esa_class, field_0x298, 0x3B4);
WWHD_SIZE(esa_class, 0x3BE);
WWHD_SIZE(dPnt_l, 0x10);
WWHD_OFFSET(dPnt_l, m_position, 4);
WWHD_SIZE(dPath_l, 0xC);
WWHD_OFFSET(dPath_l, m_points, 8);

// HD: landing-check subobjects are initialized in the original store order.
inline void kamomeInitLandingCheck(dBgS_GndChk_l& chk) {
    chk.__vtbl_10 = 0x1001233C;
    for (int i = 0; i < 7; i++) chk.mPass[i] = 0;
    chk.mpGrpPassChk = gabi::ea(&chk) + 0x4C;
    chk.__vtbl_4C = 0x1001235C;
    chk.__vtbl_40 = 0x1001236C;
    chk.mpPolyPassChk = gabi::ea(&chk) + 0x40;
    chk.__vtbl_20 = 0x1001234C;
    chk.mGrp = 1;
}
