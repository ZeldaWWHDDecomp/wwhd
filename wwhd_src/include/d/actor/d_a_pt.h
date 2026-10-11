/* pt_class (Miniblin), WWHD layout. 
 *
 * GameCube -> WWHD: +0x11C up to the timers (the GameCube header's m2C0.. are +0x11C); four bytes
 * fewer before the collision members (+0x118 from m328 on: GameCube 0x328 -> 0x440).
 * enemyice keeps its GameCube layout (0x3B8); enemyfire is 0x22C (GameCube 0x228: one float
 * appended, set to 1.0 by its constructor). Size 0xEE4 (constructor 02446CF4).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"

/* mDoExt_baseAnm (HD): J3DFrameCtrl first (frame at +4) */
struct mDoExt_baseAnm_l {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    f32 getFrame() { return mFrameCtrl.mFrame; }
    void setFrame(f32 f) { mFrameCtrl.mFrame = f; }
};

using PtMaterialAnimation = mDoExt_baseAnm_l;

/* dPa_smokeEcallBack (HD 0x20): vtable first */
struct dPa_smokeEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 _04[0x1C];
};

using PtSmokeCallback = dPa_smokeEcallBack_l;

/* enemyice (c_damagereaction), GameCube layout */
struct enemyice {
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
WWHD_OFFSET(enemyice, mCylHeight, 0x19C);
WWHD_SIZE(enemyice, 0x3B8);

// HD: enemyfire adds a particle scale multiplier to the GameCube layout.
struct enemyfire {
    /* 0x000 */ gptr<fopAc_ac_c> mpActor;
    /* 0x004 */ be<s16> mFireDuration;
    /* 0x006 */ be<s8> mMode;
    /* 0x007 */ u8 _007;
    /* 0x008 */ be<s16> mFireTimer;
    /* 0x00A */ u8 _00A[2];
    /* 0x00C */ gptr<mDoExt_McaMorf> mpMcaMorf;
    /* 0x010 */ be<s8> mFlameJntIdxs[10];
    /* 0x01A */ u8 _01A[2];
    /* 0x01C */ be<f32> mParticleScale[10];
    /* 0x044 */ be<s16> mFlameTimers[10];
    /* 0x058 */ be<u32> mpFlameEmitters[10];
    /* 0x080 */ cXyz mPrevPos;
    /* 0x08C */ cXyz mDirection;
    /* 0x098 */ be<f32> mFlameScaleY;
    /* 0x09C */ u8 _09C;
    /* 0x09D */ be<u8> mHitboxFlameIdx;
    /* 0x09E */ u8 _09E[2];
    /* 0x0A0 */ dCcD_Stts mStts;
    /* 0x0DC */ dCcD_Sph mSph;
    /* 0x208 */ u8 mLight[0x20]; /* LIGHT_INFLUENCE */
    /* 0x228 */ union { be<f32> mParticleScaleMultiplier; be<f32> m228; }; // HD: legacy field name kept for dependent actors.
};
WWHD_OFFSET(enemyfire, mSph, 0xDC);
WWHD_SIZE(enemyfire, 0x22C);

struct pt_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> mBehaviorType;
    /* 0x3D1 */ be<u8> mNoticeRange;
    /* 0x3D2 */ be<u8> mRespawnDelay;
    /* 0x3D3 */ be<u8> mEnableSpawnSwitch;
    /* 0x3D4 */ be<u8> mDisableRespawnSwitch;
    /* 0x3D5 */ be<s8> mInitialSpawnWaitsUntilOffScreen;          /* GameCube 0x2B9: not drawn, no action (waits for its spawn switch) */
    /* 0x3D6 */ be<s8> m2BA;       /* GameCube 0x2BA: Delete creates a new Miniblin */
    /* 0x3D7 */ u8 _3D7[0x3DC - 0x3D7];
    /* 0x3DC */ gptr<mDoExt_McaMorf> m2C0;
    /* 0x3E0 */ gptr<PtMaterialAnimation> m2C4;
    /* 0x3E4 */ gptr<PtMaterialAnimation> m2C8;
    /* 0x3E8 */ be<s32> m2CC;      /* body colour (random 0..3) */
    /* 0x3EC */ be<s16> m2D0;
    /* 0x3EE */ be<s16> m2D2;        /* GameCube 0x2D2 */
    /* 0x3F0 */ be<s16> mMode;
    /* 0x3F2 */ be<s16> m2D6;
    /* 0x3F4 */ be<s16> mInitialSpawnDelay; /* home angle.x */
    /* 0x3F6 */ u8 _3F6[0x404 - 0x3F6];
    /* 0x404 */ cXyz mJumpTarget;       /* landing point found by next_pos_set */
    /* 0x410 */ gptr<cXyz> m2F8;  /* bait (esa) or player position */
    /* 0x414 */ be<s16> m2FC;
    /* 0x416 */ u8 _416[2];
    /* 0x418 */ be<f32> m300;
    /* 0x41C */ be<f32> m304;
    /* 0x420 */ be<s16> mTimers[3];
    /* 0x426 */ be<s16> m30E;
    /* 0x428 */ be<s16> mSpinSpeed;     /* pt_koke modes 10..12 */
    /* 0x42A */ u8 _42A[2];
    /* 0x42C */ be<f32> m314;
    /* 0x430 */ be<f32> m318;
    /* 0x434 */ be<f32> m31C;
    /* 0x438 */ be<f32> m320;
    /* 0x43C */ be<s16> m324;
    /* 0x43E */ be<s8> mbBgCheck;       /* cleared by Draw */
    /* 0x43F */ be<s8> mbAttack;
    /* 0x440 */ dBgS_AcchCir m328;
    /* 0x480 */ dBgS_ObjAcch m368;
    /* 0x644 */ dCcD_Stts mStts;
    /* 0x680 */ dCcD_Sph mSph;          /* body (tg/co) */
    /* 0x7AC */ dCcD_Sph mAtSph;        /* attack */
    /* 0x8D8 */ be<s8> mSmokeType;
    /* 0x8D9 */ u8 _8D9[3];
    /* 0x8DC */ PtSmokeCallback mSmokeCb;
    /* 0x8FC */ enemyice mEnemyIce;
    /* 0xCB4 */ enemyfire mEnemyFire;
    /* 0xEE0 */ be<u8> mDC4;
    /* 0xEE1 */ u8 _EE1[3];
};
WWHD_OFFSET(pt_class, m2C0, 0x3DC);
WWHD_OFFSET(pt_class, m328, 0x440);
WWHD_OFFSET(pt_class, mStts, 0x644);
WWHD_OFFSET(pt_class, mSmokeCb, 0x8DC);
WWHD_OFFSET(pt_class, mEnemyIce, 0x8FC);
WWHD_OFFSET(pt_class, mEnemyFire, 0xCB4);
WWHD_SIZE(pt_class, 0xEE4);

/* daPt_HIO_c (8 bytes, vtable after the members) */
struct daPt_HIO_c {
    /* 0x0 */ be<s8> mNo;
    /* 0x1 */ be<u8> mbStop;   /* action() is skipped while set */
    /* 0x2 */ be<u8> m02;
    /* 0x3 */ u8 _3;
    /* 0x4 */ be<u32> __vtbl;
};
WWHD_SIZE(daPt_HIO_c, 8);

/* process creation (fopAcM_create's inline body in HD) */
struct PtCreateAppend {
    /* 0x00 */ be<u32> mParameter;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ csXyz mAngle;
    /* 0x16 */ u8 _16[0x21 - 0x16];
    /* 0x21 */ be<s8> mRoomNo;
};
/* HD: HARNESS WORKAROUND: the matcher names 025E80D0 mDoExt_brkAnm::init (it is the constructor), so the
 * harness compares the GameCube init's stack argument; the original's outgoing-argument word is
 * still 0 from the preceding btpAnm::init call: give the call the same word. */
PtMaterialAnimation* pt_brkCtor(void* animation);
static inline PtMaterialAnimation* mDoExt_brkAnm_ct(void* p) {
    gabi::Local<be<u32>[4]> outArgs;
    (*outArgs)[2] = 0;
    return pt_brkCtor(p);
}
/* J3DModel (HD): model data at +0xAC; joint matrices in the block at +0x2C (flags +4, matrices +0x10) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
using PtJointMatrixBlock = J3DMtxBlock_l;
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<PtJointMatrixBlock> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
};
using PtModel = J3DModel_l;
static inline J3DModelData* getModelData(J3DModel* m) { return ((PtModel*)m)->mModelData; }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* getAnmMtx(J3DModel* m, s32 jnt) {
    PtJointMatrixBlock* blk = ((PtModel*)m)->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline void mDoMtx_stack_transS(const cXyz& p) { PSMTXTrans(mDoMtx_stack_c::get(), p.x, p.y, p.z); }

/* dBgS_LinChk on the stack (this TU's vtables) */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk;
    /* 0x04 */ be<u32> mpGrpPassChk;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz mStart;
    /* 0x30 */ cXyz mCross;
    /* 0x3C */ u8 _3C[0x58 - 0x3C];
    /* 0x58 */ be<u32> __vtbl_58;
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;
    /* 0x68 */ be<u32> mGrp;
};
using PtLineCheck = dBgS_LinChk_l;
WWHD_SIZE(PtLineCheck, 0x6C);
static inline void dBgS_LinChk_ct(PtLineCheck* c) {
    cBgS_LinChk_ct(c);
    c->mpGrpPassChk = gabi::ea(&c->__vtbl_64);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->__vtbl_58 = 0x10038214;
    c->__vtbl_20 = 0x100381F4;
    c->__vtbl_10 = 0x100381E4;
    c->__vtbl_64 = 0x10038204;
    c->mGrp = 1;
    c->mpPolyPassChk = gabi::ea(&c->__vtbl_58);
}
static inline void dBgS_LinChk_dt(PtLineCheck* c) {
    c->__vtbl_58 = 0x10038214;
    c->__vtbl_64 = 0x100380E4;
    c->__vtbl_20 = 0x100380D4;
    cBgS_LinChk_dt(c, 0);
}

/* dBgS_GndChk on the stack (layout) */
struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk;
    /* 0x04 */ be<u32> mpGrpPassChk;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;
    /* 0x50 */ be<u32> mGrp;
};
using PtGroundCheck = dBgS_GndChk_l;
WWHD_SIZE(PtGroundCheck, 0x54);
/* cBgS_LinChk::GetCross (lin end) */
static inline cXyz* LinChk_GetCross(PtLineCheck* c) { return gabi::at<cXyz>(gabi::ea(c) + 0x30); }

// HD: guest-backed views used by the inlined camera, player and smoke operations.
struct PtCamera { u8 mPadding[0xDC]; cXyz mEye; cXyz mCenter; };
WWHD_OFFSET(PtCamera, mEye, 0xDC);
struct PtPlayer { u8 mPadding[0x3AC]; be<u8> mCutType; u8 mPad[0x13]; be<u32> mStateFlags; };
WWHD_OFFSET(PtPlayer, mCutType, 0x3AC);
WWHD_OFFSET(PtPlayer, mStateFlags, 0x3C0);
struct PtPlayerVtable { u8 mPadding[0x3C]; be<u32> mCheckGuard; };
struct PtSmokeVtable { u8 mPadding[0x44]; be<u32> mRemove; };
struct PtEmitter {
    u8 mPadding[0x34]; be<f32> mRate; u8 mPad38[0x20]; be<f32> mSpread;
    u8 mPad5C[0x220-0x5C]; cXyz mGlobalScale;
    u8 mPad22C[0xC]; cXyz mGlobalParticleScale;
};
WWHD_OFFSET(PtEmitter, mSpread, 0x58);
WWHD_OFFSET(PtEmitter, mGlobalScale, 0x220);
WWHD_OFFSET(PtEmitter, mGlobalParticleScale, 0x238);
enum PtAction { PT_WAIT, PT_MOVE, PT_ATTACK, PT_KOKE, PT_PLES, PT_BAT };
constexpr u32 PT_ESA_SEARCH_CALLBACK = 0x02446324;
struct PtAttackInfo {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz mHitAngle;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(PtAttackInfo, 0x1C);
BOOL pt_enemyIce(enemyice* ice);
void pt_enemyFire(enemyfire* fire);
void pt_enemyFireRemove(enemyfire* fire);
PtCreateAppend* pt_createAppend();
void* pt_currentLayer();
u32 pt_createRequest(void* layer, s16 name, u32 callback, void* data, void* append);
PtMaterialAnimation* pt_btpCtor(void* animation);
BOOL pt_btpInit(PtMaterialAnimation* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry);
PtMaterialAnimation* pt_brkCtor(void* animation);
BOOL pt_brkInit(PtMaterialAnimation* animation, J3DModelData* model, void* resource, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry);
s32 pt_nameIndex(void* table, const char* name, s32 parameter);
void pt_sphereCtor(dCcD_Sph* sphere);
void pt_circleCtor(dBgS_AcchCir* circle);
void pt_acchCtor(dBgS_ObjAcch* collision);
void pt_smokeCtor(PtSmokeCallback* callback, s32 mode);
void pt_sphereDtor(dCcD_Sph* sphere, s32 flags);
void pt_acchDtor(dBgS_ObjAcch* collision, s32 flags);
void pt_circleDtor(cM3dGCir_l* circle, s32 flags);
void pt_actorDtor(fopAc_ac_c* actor, s32 flags);
void pt_action(pt_class* actor);
void* pt_atHitObj(dCcD_GObjInf* collision);
void pt_checkAttack(fopAc_ac_c* actor, PtAttackInfo* info);
void pt_monsterSound(u32 sound, cXyz* position, u32 processId, u32 parameter, s32 reverb);
void pt_groundCtor(PtGroundCheck* check);
void pt_groundDtor(PtGroundCheck* check, s32 flags);
void pt_anmInit(pt_class* actor, int animation, f32 morph, u8 loop, f32 rate, int sound);
void pt_smokeSet(pt_class* actor, s8 type);
s16 pt_wallAngle(pt_class* actor);

enum {
    dRes_INDEX_PT_BCK_WAIT_e = 0xC,   /* used by useHeapInit */
    dRes_INDEX_PT_BDL_PT_e = 0xF,
    dRes_INDEX_PT_BRK_e = 0x12,
    dRes_INDEX_PT_BTP_e = 0x15,
};

inline BOOL pt_checkGuard(fopAc_ac_c* player) {
    u32 function = gabi::at<PtPlayerVtable>(player->__vtbl)->mCheckGuard;
    return gabi::call_ptr<BOOL>(function, player);
}
inline void pt_removeSmoke(PtSmokeCallback* callback) {
    gabi::call_ptr(gabi::at<PtSmokeVtable>(callback->__vtbl)->mRemove, callback);
}
inline s32 pt_characterIndex(const char* name, s32 parameter) {
    return pt_nameIndex(gabi::at<u8>(dComIfGp_ea() + PLAY_NAMETBL), name, parameter);
}
WWHD_SIZE(PtMaterialAnimation, 0x10);
WWHD_SIZE(PtSmokeCallback, 0x20);
WWHD_SIZE(PtCreateAppend, 0x22);
WWHD_SIZE(PtJointMatrixBlock, 0x14);
WWHD_SIZE(PtModel, 0xB0);
WWHD_SIZE(PtCamera, 0xF4);
WWHD_SIZE(PtPlayer, 0x3C4);
WWHD_SIZE(PtPlayerVtable, 0x40);
WWHD_SIZE(PtSmokeVtable, 0x48);
WWHD_SIZE(PtEmitter, 0x244);
WWHD_OFFSET(PtLineCheck, mCross, 0x30);

struct PtSphereSource {
    be<u32> mFlags, mAttackType, mAttackPower, mAttackPrm;
    be<u32> mTargetType, mTargetPrm, mCollisionPrm;
    be<u32> mAttackInfo, mAttackFlags, mTargetInfo, mTargetFlags, mCollisionFlags;
    be<f32> mCenter[3]; be<f32> mRadius;
};
WWHD_SIZE(PtSphereSource, 0x40);

WWHD_OFFSET(PtCreateAppend, mParameter, 0);
WWHD_OFFSET(PtCreateAppend, mPos, 4);
WWHD_OFFSET(PtCreateAppend, mAngle, 0x10);
WWHD_OFFSET(PtCreateAppend, mRoomNo, 0x21);
WWHD_OFFSET(PtAttackInfo, pParticlePos, 0x14);
WWHD_OFFSET(PtJointMatrixBlock, mpMtx, 0x10);
WWHD_OFFSET(PtModel, mModelData, 0xAC);

// HD: preserve existing damage-reaction bindings used by dependent actor sources.
static inline BOOL enemy_ice(enemyice* ice) { return gabi::call<BOOL>(0x020402C8, ice); }
static inline void enemy_fire(enemyfire* fire) { gabi::call(0x02041570, fire); }
static inline void enemy_fire_remove(enemyfire* fire) { gabi::call(0x02041C30, fire); }
