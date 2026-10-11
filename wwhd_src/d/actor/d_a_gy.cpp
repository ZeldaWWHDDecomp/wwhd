/**
 * d_a_gy.cpp (WWHD)
 * Enemy - Gyorg
 *
 * The GameCube decompilation of this unit has only
 * "Nonmatching" stubs (zeldaret/tww src/d/actor/d_a_gy.cpp): every function here is written
 * from the WWHD code (cking.rpx) and verified against it. GameCube function names are kept.
 */
#include "d/actor/d_a_gy.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_kankyo.h"
#include "gc_api/d/d_lib.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/m_Do/m_Do_ext.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "gc_api/tu.h"
#include <cmath>

const gc_tu_data gc_tu = {.safeString = 0x100108B0,
                          .safeStringAssure = 0x0216B6E4,
                          .objAcch = {0x100108F8, 0x10010918, 0x10010908},
                          .objAcchDt = {0x10010918, 0x10010908},
                          .headerStatics = {0x10464450, 0x101B6CFC, 0}};

static BOOL createHeap_CB(fopAc_ac_c* i_this);
static BOOL nodeControl_CB(J3DNode* node, int calcTiming);
static cPhs_State daGyCreate(void* i_this);
static BOOL daGyDelete(void* i_this);
static BOOL daGyExecute(void* i_this);
static BOOL daGyDraw(void* i_this);
static BOOL daGyIsDelete(void* unused);
static void SafeString_dt(void* p, s32 flags);
static void SafeString_assureTermination(void* unused);
daGy_HIO_c* daGy_HIO_ct(daGy_HIO_c* hio);
static void __sinit_d_a_gy_cpp();
static void daGy_dt(daGy_c* i_this, s32 flags);
daGy_c* daGy_ct(daGy_c* i_this);

static GyColourGuardsHD l_colourGuards;
static GyColoursHD l_colours;
static GyColourSourcesHD l_colourSources;
static GyAnimationIndicesHD l_animationIndices;
static GyAnimationParametersHD l_animationParameters;
s32 gyGetCharacterIndex(void*, const char*, s32);
void gySetAnimation(const char*, mDoExt_McaMorf*, be<s8>*, be<s8>*, be<s8>*, const dLib_anm_idx_c*,
                    const dLib_anm_prm_c*, u32, mDoExt_McaMorf*);

static daGy_HIO_c l_HIO;
static GyDebugHD l_debug;
static GyWaveOffsetsHD l_waveOffsets;
static GyModeTableHD l_modeTable;
static Quaternion l_zeroQuaternion;

static GyJointHitDataHD l_jointHitData;
static dCcD_SrcSph l_sph_head_src;
static dCcD_SrcSph l_sph_src;
static GyCapsuleSourceHD l_cps_src;
void gyPublishModelFlags(J3DModel*, u32);
void gyRemoveWave(GyWaveHD*);
void gyRemoveSplash(GySplashHD*);
void gyDestroyLineBase(dBgS_LinChk*, s32);
void gyDestroyAcch(dBgS_ObjAcch*, s32);
void gyDestroyCircle(cM3dGCir_l*, s32);
void gyDestroyCapsule(dCcD_Cps*, s32);
void gyDestroySphere(dCcD_Sph*, s32);
void gyDestroyActor(fopAc_ac_c*, s32);
void gyConstructSphere(dCcD_Sph*);
void gyConstructObjectInfo(dCcD_Cps*);
void gyConstructCapsule(GyCapsuleHD*);
void gySetCapsule(dCcD_Cps*, GyCapsuleSourceHD*);
void gySetLine(cM3dGLin*, cXyz*, cXyz*);

using GyModelPointer = gptr<J3DModel>;
static GyModelPointer gyCurrentModel;
static Mtx34 gyCurrentMatrix;

// Private adapters preserve the HD native signatures.
/*  mDoExt_invisibleModel::entryDL (the matcher names it dMat_ice_c::entryDL; as in d_a_oq) */
void mDoExt_invisibleModel_entryDL_gy(mDoExt_McaMorf* morf, s32 p, void* inv);
BOOL enemy_ice_gy(GyIceHD* e);
/*  cLib_calcTimer<int> */
s32 cLib_calcTimer_gy(be<s32>* t);
s32 dBgS_GetPolyColor_gy(dBgS* bgs, void* p);
/*  mDoAud_onEnemyDamage() (as in d_a_am) */
void mDoAud_onEnemyDamage_gy();
/*  mDoAud_monsSeStart(id, pos, procId, param, reverb) */
void mDoAud_monsSeStart_gy(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb);
/*  dComIfGs_checkGetItem(u8) (as in d_a_warpf) */
/*  fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
BOOL fopAcM_SearchByID_gy(fpc_ProcID id, be<u32>* out);
static inline s32 dComIfGp_CharTbl_GetNameIndex_gy(const char* name, s32 n) {
    return gyGetCharacterIndex(reinterpret_cast<GyCharacterTableHD*>(dComIfGp_get())->mTable, name, n);
}
void gyConcatMatrices(const Mtx34*, const Mtx34*, Mtx34*);
void gyApplyQuaternion(const Quaternion*);
void gyQuaternionFromVectors(Quaternion*, const cXyz*, const cXyz*);
/*  mDoExt_invisibleModel::create(J3DModel*) */
/*  (HD): passes a J3DModel's new flags (+0x74) on to its material packets */
static inline void J3DModel_setFlags_gy(J3DModel* m, u32 mask) {
    auto* flags = reinterpret_cast<GyModelFlagsHD*>(m);
    u32 f = flags->mFlags & mask;
    flags->mFlags = f;
    gyPublishModelFlags(m, f);
}
/*  gyCreateJointHit(J3DModel*, __jnt_hit_data_c*, s16) */
u32 gyCreateJointHit(J3DModel* m, u32 data, s16 n);
// HD: the shared J3D methods retain dirty-matrix flags and the checked joint lookup.
static inline void setJointCallBack_gy(J3DModelData* data, u16 index, BOOL (*callback)(J3DNode*, int)) {
    data->getJointNodePointer(index)->mCallBack = gabi::guest_fn(reinterpret_cast<const void*>(callback));
}
/* a float moved by lfs/stfs without arithmetic is quieted on load: be<f32> models that */

/* the death splash: particle 0x3C at the actor's position (HD: the scale has the debug offsets
 * added, (2+a, 2+a+b, 2+a)) */
static inline void gy_deathSplash(daGy_c* i_this) {
    gabi::Local<cXyz> scale;
    f32 s = l_debug.mSplashScale + 2.0f;
    f32 sy = s + l_debug.mSplashHeight;
    scale->x = s;
    scale->z = s;
    scale->y = sy;
    dPa_control_set(dComIfGp_getParticle(), 0, 0x3C, &i_this->current.pos, nullptr, scale, 0xFF, nullptr, -1,
                    nullptr, nullptr, nullptr);
}
static inline be<u32>& gy_attnFlags(daGy_c* i_this) { return i_this->attention_info.flags; }

/* the head sphere's attack bits (raw GObjInf offsets: At SPrm +0x0 bit 1 "set", +0x4, Atp +0x14,
 * At Se +0x6C, At Spl +0x6F, Co SPrm +0x2C bit 1 "set") */
static inline void gy_headAtOn(daGy_c* i_this, bool coOn) {
    dCcD_Sph& head = i_this->mHeadSph;
    head.mGObjAt.mSpl = 1;
    head.mObjAt.mSPrm |= 1;
    head.mAtp = 2;
    head.mObjAt.mRPrm = 1;
    head.mGObjAt.mSe = 6;
    if (coOn)
        head.mObjCo.mSPrm |= 1;
    else
        head.mObjCo.mSPrm &= ~1u;
}
static inline void gy_headAtOff(daGy_c* i_this) {
    dCcD_Sph& head = i_this->mHeadSph;
    head.mObjAt.mSPrm &= ~1u;
    head.mObjAt.mRPrm &= ~1u;
    head.mAtp = 0;
}
static inline void gy_headCoOn(daGy_c* i_this) { i_this->mHeadSph.mObjCo.mSPrm |= 1; }

static inline void gy_seStart(daGy_c* i_this, u32 id, u32 param) {
    mDoAud_seStart(id, &i_this->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
}
static inline void gy_removeWaves(daGy_c* i_this) {
    gyRemoveSplash(&i_this->mSplash); /* dPa_splashEcallBack::remove */
    gyRemoveWave(&i_this->mWave[0]);  /* dPa_waveEcallBack::remove */
    gyRemoveWave(&i_this->mWave[1]);
}

/* a wave/splash emitter's "stop drawing" flag (JPABaseEmitter +0x254 bit 0) */
static inline void gy_emitterStop(JPABaseEmitter* e, bool stop) {
    auto* emitter = reinterpret_cast<GyEmitterHD*>(e);
    u32 flags = emitter->mStatus;
    emitter->mStatus = stop ? (flags | 1) : (flags & ~1u);
}

/* the landing splash (particles 0x203B and 0x3C, or 0x3C alone) with the HD debug scale offsets */
static inline void gy_landSplash(daGy_c* i_this, f32 base, bool big, u32 se) {
    gabi::Local<cXyz> sc;
    f32 s = l_debug.mSplashScale + base;
    f32 sy = s + l_debug.mSplashHeight;
    sc->x = s;
    sc->z = s;
    sc->y = sy;
    if (big) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x203B, &i_this->current.pos, nullptr, sc, 0xFF, nullptr,
                        -1, nullptr, nullptr, nullptr);
    }
    dPa_control_set(dComIfGp_getParticle(), 0, 0x3C, &i_this->current.pos, nullptr, sc, 0xFF, nullptr, -1,
                    nullptr, nullptr, nullptr);
    gy_seStart(i_this, se, 0);
}

/* a function-local static (4 bytes, e.g. a GXColor) initialised from .data on first use */
static inline bool gy_initStatic(be<s32>* guard, GXColor* value, const GXColor* initialValue) {
    if (*guard != 0)
        return false;
    *guard = 1;
    memcpy_g(value, initialValue, sizeof(GXColor));
    return true;
}
#define GY_DBG_COLOR0 &l_colourGuards.mHead, &l_colours.mHead, &l_colourSources.mHead
#define GY_DBG_COLOR1 &l_colourGuards.mShip, &l_colours.mShip, &l_colourSources.mShip
#define GY_DBG_COLOR2 &l_colourGuards.mBody, &l_colours.mBody, &l_colourSources.mBody

/* close in on the spawn point and face away from the controller's centre */
static inline void gy_approachHome(daGy_c* i_this, daGyCtrl_c* ctrl) {
    gabi::Local<cXyz> toHome;
    i_this->mAimSpeedF = 0.0f;
    cXyz_mi(&i_this->mSpawnPos, toHome, &i_this->current.pos);
    cLib_addCalc2(&i_this->mApproachRate, 1.0f, 0.01f, 0.05f);
    gabi::Local<cXyz> step;
    cXyz_ml(toHome, step, i_this->mApproachRate);
    gabi::Local<cXyz> pos;
    cXyz_pl(&i_this->current.pos, pos, step);
    i_this->current.pos.copy(*pos);
    cLib_addCalcAngleS2(&i_this->current.angle.y, (s16)(ctrl->mSpawnAngles[i_this->mIndex] + 0x8000), 4,
                        0x400);
}
/* the horizontal distance to the spawn point (or another point) */
static inline f32 gy_homeDistXZ(daGy_c* i_this, cXyz* to = nullptr) {
    gabi::Local<cXyz> diff;
    cXyz_mi(&i_this->current.pos, diff, to != nullptr ? to : &i_this->mSpawnPos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->x = diff->x;
    diffXZ->y = 0.0f;
    diffXZ->z = diff->z;
    return std_sqrtf(PSVECSquareMag(diffXZ));
}

/* the head rammed a boat (process 0xA5): shake, sounds, swim back */
static inline bool gy_hitShip(daGy_c* i_this) {
    if (!i_this->mHeadSph.ChkCoHit())
        return false;
    fopAc_ac_c* ac = i_this->mHeadSph.GetCoHitAc();
    if (ac == nullptr || fpcM_GetName(ac) != fpcNm_SHIP_e)
        return false;
    return true;
}
static inline void gy_shipHitReaction(daGy_c* i_this) {
    gabi::Local<cXyz> dir;
    dir->z = 0.0f;
    dir->x = 0.0f;
    dir->y = 1.0f;
    dComIfGp_getVibration_StartShock(7, -0x21, dir);
    gy_seStart(i_this, 0x58D3, 0);
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
    mDoAud_monsSeStart_gy(0x48CF, &i_this->eyePos, (u32)i_this->mBsPcId, 0, reverb);
    i_this->modeAttackBackInit();
}

/* pushed back from the player by the damage speed */
static inline void gy_damagePush(daGy_c* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> push;
    gabi::Local<cXyz> move;
    f32 sp = i_this->mDamageSpeed;
    move->x = 0.0f;
    push->x = 0.0f;
    push->y = 0.0f;
    push->z = -sp;
    move->y = 0.0f;
    move->z = 0.0f;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), fopAcM_searchActorAngleY(i_this, player));
    PSMTXMultVec(mDoMtx_stack_c::get(), push, move);
    i_this->current.pos.x = i_this->current.pos.x + move->x;
    i_this->current.pos.y = i_this->current.pos.y + move->y;
    i_this->current.pos.z = i_this->current.pos.z + move->z;
    cLib_addCalc0(&i_this->mDamageSpeed, 1.0f, 2.0f);
}

/* 0216B100 */
daGy_HIO_c* daGy_HIO_ct(daGy_HIO_c* hio) {

    if (hio == nullptr) {
        hio = (daGy_HIO_c*)operator_new(sizeof(daGy_HIO_c));
        if (hio == nullptr)
            return hio;
    }
    hio->__vtbl = 0x100109A8;
    hio->mSwimAnimationScale = 1.0f;
    hio->mMinimumAnimationSpeed = 0.9f;
    hio->mFreezeDuration = 200;
    hio->mSplashSpeed = 200.0f;
    hio->mMaxSplashSpeed = 300.0f;
    hio->mWaveSpeed = 2.0f;
    hio->mWaveDistanceSpeed = 2.0f;
    hio->mWavePitchDifference = 0.0f;
    hio->mWaveMaxSpeed = 15.0f;
    hio->mCollapseNearX = -80.0f;
    hio->mCollapseNearY = -50.0f;
    hio->mCollapseNearZ = -150.0f;
    hio->mCollapseFarX = -40.0f;
    hio->mCollapseFarY = -100.0f;
    hio->mCollapseFarZ = -350.0f;
    hio->mEscortSpeedOffset = 60.0f;
    hio->mAttackApproachDistance = 2500.0f;
    hio->mSpeedSlow = 10.0f;
    hio->mSpeedSlowStroke = 20.0f;
    hio->mSpeedFast = 30.0f;
    hio->mSpeedFastStroke = 40.0f;
    hio->mPlayerApproachSpeed = 15.0f;
    hio->mPlayerBiteSpeed = 5.0f;
    hio->mPlayerLungeSpeed = 40.0f;
    hio->mShipAttackSpeed = 20.0f;
    hio->mEscortAttackSpeed = 55.0f;
    hio->mCircleShipSpeed = 10.0f;
    hio->mEscortShipSpeed = 20.0f;
    hio->mUnusedTuning0 = 70.0f;
    hio->mUnusedTuning1 = 70.0f;
    hio->mUnusedTuning2 = 80.0f;
    hio->mUnusedTuning3 = 70.0f;
    hio->mUnusedTuning4 = 70.0f;
    hio->mHeadRadius = 110.0f;
    hio->mBodyRadius = 100.0f;
    hio->mActorScale = 1.0f;
    hio->mDebugOffset = 0;
    hio->mDebugMode = 0;
    hio->mHideModel = 0;
    hio->mFloatOnWater = 0;
    hio->mUnusedFlag = 0;
    hio->mDeleteAfterEscortAttack = 0;
    hio->mEscortJumpPlayerDistance = 5000.0f;
    hio->mDiveHeight = -500.0f;
    hio->mBiteHeadHeight = 180.0f;
    hio->mAttackHeadHeight = -400.0f;
    hio->mCircleHeight = -80.0f;
    hio->mAttackHeight = 20.0f;
    hio->mBiteHeight = -25.0f;
    hio->mDebugOffsetPos.x = 0.0f;
    hio->mDebugOffsetPos.y = 0.0f;
    hio->mDebugOffsetPos.z = 0.0f;
    hio->mHeightStep = 8.0f;
    hio->mDiveHeightStep = 30.0f;
    hio->mStrokeStart = 2.0f;
    hio->mStrokeEnd = 20.0f;
    hio->mHeadOffset.x = 80.0f;
    hio->mHeadOffset.y = 0.0f;
    hio->mHeadOffset.z = 0.0f;
    hio->mUnusedTuning7 = 0.0f;
    hio->mUnusedTuning8 = 0.0f;
    hio->mUnusedTuning9 = 0.0f;
    hio->mSplashOffsetLow.x = 0.0f;
    hio->mSplashOffsetLow.y = 0.0f;
    hio->mSplashOffsetLow.z = 120.0f;
    hio->mSplashOffsetHigh.x = 0.0f;
    hio->mSplashOffsetHigh.y = 0.0f;
    hio->mSplashOffsetHigh.z = 300.0f;
    hio->mRecoilSpeed = -40.0f;
    hio->mRecoilVerticalSpeed = 25.0f;
    hio->mAttackTime = 120;
    hio->mEscortAttackTime = 60;
    hio->mEscortCircleTime = 120;
    hio->mShipCircleMinTime = 180;
    hio->mShipCircleMaxTime = 360;
    hio->mPlayerCircleMinTime = 90;
    hio->mPlayerCircleMaxTime = 240;
    for (int i = 0; i < 9; i++)
        hio->mUnusedFlags[i] = 0;
    hio->mUnusedTuning11 = 10.0f;
    hio->mMinEscortRandomTime = 100;
    hio->mEscortRandomTimeRange = 300;
    hio->mUnusedTuning12 = 45.0f;
    hio->mEscortJumpSpeedY = 45.0f;
    hio->mUnusedTuning13 = 30.0f;
    hio->mEscortOuterAngle = 0x1500;
    hio->mEscortInnerAngle = 0x800;
    hio->mHomeRadius = 400.0f;
    hio->mPlayerAvoidanceRadius = 350.0f;
    hio->mArrowDamageSpeed = 60.0f;
    hio->mHookshotDamageSpeed = 60.0f;
    hio->mBoomerangDamageSpeed = 40.0f;
    hio->mDeathVerticalSpeed = 45.0f;
    hio->mUnusedTuning14 = 10.0f;
    hio->mDeathFloatTime = 90;
    hio->mDeathSinkHeight = -1000.0f;
    hio->mDeathFloatHeight = -100.0f;
    hio->mDeathFloatAmplitude = 50.0f;
    hio->mHeightApproachRate = 0.1f;
    hio->mDeathFloatStep = 0.5f;
    hio->mDeathFloatApproachRate = 0.1f;
    hio->mHeadAngle = 0x2000;
    hio->mHeadDist = 4000.0f;
    hio->mHeadSlerp = 0.2f;
    hio->mUnusedTuning15 = 3000.0f;
    hio->mUnusedTuning16 = 200.0f;
    hio->mUnusedTuning17 = 3000.0f;
    hio->mBiteCount = 1;
    return hio;
}

/* 02166CC0 */
static BOOL nodeControl_CB(J3DNode* node, int calcTiming) {

    if (calcTiming == 0) {
        J3DModel* model = gyCurrentModel;
        daGy_c* i_this = gabi::at<daGy_c>(model->mUserArea);
        if (i_this != nullptr) {
            i_this->_nodeControl(node, (J3DModel*)model);
        }
    }
    return TRUE;
}

/* 02166944 */
void daGy_c::_nodeControl(J3DNode* node, J3DModel* model) {

    J3DJoint* joint = J3DNode_toJoint(node);
    Mtx34* now = mDoMtx_stack_c::get();
    u32 jntNo = reinterpret_cast<GyJointHD*>(joint)->mJointNo;
    daGy_HIO_c* hio = gy_hio();
    gabi::Local<cXyz> headOffset;
    if (jntNo == 2) {
        PSMTXCopy(model->getAnmMtx(2), now);
        headOffset->copy(hio->mHeadOffset);
        PSMTXMultVec(now, headOffset, &mHeadPos);
    }
    gabi::Local<Mtx34> mtx;
    PSMTXCopy(model->getAnmMtx(jntNo), mtx);
    f32 ty = mtx->m[1][3];
    f32 tz = mtx->m[2][3];
    mtx->m[2][3] = 0.0f;
    f32 tx = mtx->m[0][3];
    mtx->m[0][3] = 0.0f;
    mtx->m[1][3] = 0.0f;
    PSMTXTrans(now, tx, ty, tz);
    gabi::Local<cXyz> headOffset2;
    gabi::Local<cXyz> offset5;
    if (jntNo == 2) {
        /* turn the head towards the player */
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        gabi::Local<cXyz> diff;
        cXyz_mi(&current.pos, diff, &player->current.pos);
        gabi::Local<cXyz> diffXZ;
        diffXZ->x = diff->x;
        diffXZ->y = 0.0f;
        diffXZ->z = diff->z;
        f32 dist = std_sqrtf(PSVECSquareMag(diffXZ));
        s16 angle = fopAcM_searchActorAngleY(this, player);
        s32 dAngle = cLib_distanceAngleS(shape_angle.y, angle);
        s8 anm;
        if (dAngle < hio->mHeadAngle && dist < hio->mHeadDist && (anm = mAnmIdxNext) != 8 && anm != 9 &&
            anm != 6 && anm != 11 && anm != 3) {
            f32 px = player->current.pos.x;
            mHeadTarget.x = px;
            f32 py = player->current.pos.y;
            s32 mode = mMode;
            mHeadTarget.y = py;
            f32 pz = player->current.pos.z;
            mHeadTarget.z = pz;
            if (mode == MODE_ATTACK) {
                py = daSea_calcWave(px, pz) + hio->mAttackHeadHeight;
                mode = mMode;
                mHeadTarget.y = py;
            }
            if (mode == MODE_ATTACK_PLAYER && mSubMode == 1) {
                mHeadTarget.y = py + hio->mBiteHeadHeight;
            }
            gabi::Local<cXyz> toTarget;
            cXyz_mi(&mHeadTarget, toTarget, &mHeadPos);
            gabi::Local<cXyz> zero;
            cXyz_mi(&mHeadPos, zero, &mHeadPos);
            gabi::Local<Quaternion> q;
            gyQuaternionFromVectors(q, zero, toTarget);
            mDoMtx_quatSlerp(&mHeadQuat, q, &mHeadQuat, hio->mHeadSlerp);
            gyApplyQuaternion(&mHeadQuat);
        } else {
            mDoMtx_quatSlerp(&mHeadQuat, &l_zeroQuaternion /* ZeroQuat */, &mHeadQuat, hio->mHeadSlerp);
            gyApplyQuaternion(&mHeadQuat);
        }
    }
    gyConcatMatrices(now, mtx, now);
    if (jntNo == 2) {
        headOffset2->copy(hio->mHeadOffset);
        PSMTXMultVec(now, headOffset2, &mHeadPos);
    } else if (jntNo == 5) {
        offset5->x = 100.0f;
        offset5->y = -100.0f;
        offset5->z = 0.0f;
        PSMTXMultVec(now, offset5, &mBodyCollisionPos);
    }
    PSMTXCopy(now, &gyCurrentMatrix);
    mtx_copy(model->getAnmMtx(jntNo), now);
}

// createHeap_CB
/* 02166FC0 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) { return ((daGy_c*)i_this)->_createHeap(); }

/* 02166D08 */
BOOL daGy_c::_createHeap() {

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes("gy", GY_BDL_BODY_e, gc_tu.safeString);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_gy.cpp", 0x378, "modelData != (0)"); /* "d_a_gy.cpp", "modelData != (0)" */
    }
    mpMorf = mDoExt_McaMorf::create(
        nullptr, modelData, nullptr, nullptr,
        (J3DAnmTransform*)dComIfG_getObjectRes("gy", GY_BCK_INITIAL_e, gc_tu.safeString), 2, 1.0f, 0, -1, 1,
        nullptr, 0, 0x11020203);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    mpMorf->getModel()->mUserArea = gabi::ea(this);
    if (!mDoExt_invisibleModel_create(mInvisibleModel, mpMorf->getModel())) {
        return FALSE;
    }
    mpMorf2 = mDoExt_McaMorf::create(
        nullptr, modelData, nullptr, nullptr,
        (J3DAnmTransform*)dComIfG_getObjectRes("gy", GY_BCK_INITIAL_e, gc_tu.safeString), 2, 1.0f, 0, -1, 1,
        nullptr, 0, 0x11020203);
    if (mpMorf2 == nullptr || mpMorf2->getModel() == nullptr) {
        return FALSE;
    }
    mpMorf2->getModel()->mUserArea = gabi::ea(this);
    J3DModel_setFlags_gy(mpMorf2->getModel(), ~6u);
    J3DModel_setFlags_gy(mpMorf->getModel(), ~1u);
    for (u16 i = 0; i < modelData->getJointNum(); i++) {
        if (i == 2 || (i >= 5 && i <= 7) || (i >= 9 && i <= 10)) {
            setJointCallBack_gy(J3DModel_getModelData(mpMorf->getModel()), i, nodeControl_CB);
        }
    }
    mpJntHit = gyCreateJointHit(mpMorf->getModel(), gabi::ea(&l_jointHitData.mItems[0]), 8);
    if (mpJntHit == 0) {
        return FALSE;
    }
    jntHit = mpJntHit; /* jntHit */
    return TRUE;
}

/* 0216731C */
void daGy_c::setMtx() {

    J3DModel* model = mpMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    gabi::Local<cXyz> offset;
    offset->y = 0.0f;
    offset->z = 500.0f;
    offset->x = 0.0f;
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, &mLineEnd);
    daGy_HIO_c* hio = gy_hio();
    if (hio->mDebugOffset != 0) {
        mDoMtx_stack_c::transM(hio->mDebugOffsetPos.x, hio->mDebugOffsetPos.y, hio->mDebugOffsetPos.z);
    }
    mDoMtx_stack_c::transM(0.0f, mBodyY, 0.0f);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    /* the shadow model on the water surface */
    model = mpMorf2->getModel();
    cXyz* s2 = &model->mBaseScale;
    s2->x = scale.x;
    s2->z = scale.z;
    s2->y = 0.1f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    f32 waterY = getWaterY();
    mDoMtx_stack_c::transM(0.0f, waterY + 15.0f, 0.0f);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    if (mBodyY > hio->mCircleHeight + 10.0f) {
        mDoMtx_stack_c::transM(hio->mSplashOffsetHigh.x, hio->mSplashOffsetHigh.y, hio->mSplashOffsetHigh.z);
    } else {
        mDoMtx_stack_c::transM(hio->mSplashOffsetLow.x, hio->mSplashOffsetLow.y, hio->mSplashOffsetLow.z);
    }
    Mtx34* now = mDoMtx_stack_c::get();
    mSplashPos.x = now->m[0][3];
    mSplashPos.y = now->m[1][3];
    mSplashPos.z = now->m[2][3];
}

/* 021685EC */
void daGy_c::setAnm() {

    gySetAnimation("gy", mpMorf.get(), &mAnmIdx, &mAnmIdxNext, &mAnmIdxOld, &l_animationIndices.mItems[0],
                   &l_animationParameters.mItems[0], 0u,
                   mpMorf2.get()); /* dLib_setAnm (HD: the second morf as a stack argument) */
}

/* 02167C34 */
void daGy_c::setAtCollision() {

    daGy_HIO_c* hio = gy_hio();
    mHeadSph.SetR(gabi::fmuls_ppc(hio->mHeadRadius, hio->mActorScale));
    mHeadSph.SetC(&mHeadPos);
    s32 mode = mMode;
    if (mode == MODE_ATTACK_PLAYER) {
        s32 frame = gabi::ftoi(mpMorf->getFrame());
        if (mAnmIdxNext == 11 && frame < 0x12) {
            gy_headAtOn(this, false);
        } else {
            gy_headAtOff(this);
            gy_headCoOn(this);
        }
    } else if (mode == MODE_ATTACK || mode == MODE_WITH_ATTACK) {
        if (mpCtrl->mTarget == 1) {
            if (mHeadSph.ChkCoHit()) {
                fopAc_ac_c* ac = mHeadSph.GetCoHitAc();
                if (ac != nullptr && fpcM_GetName(ac) == fpcNm_SHIP_e) {
                    gy_headAtOn(this, true);
                } else {
                    gy_headCoOn(this);
                }
            } else {
                gy_headAtOff(this);
                gy_headCoOn(this);
            }
        }
    } else {
        gy_headAtOff(this);
        gy_headCoOn(this);
        if (mHeadSph.ChkCoHit() && mMode != MODE_DELETE_BOMB && mMode != MODE_DELETE) {
            modeDiveInit();
        }
    }
    dComIfG_Ccsp_Set(&mHeadSph);
}

/* 02167E84 */
void daGy_c::setCollision() {

    daGy_HIO_c* hio = gy_hio();
    mSph.SetR(140.0f * hio->mActorScale);
    mSph.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mSph);
    auto* capsule = reinterpret_cast<GyCapsuleHD*>(mCps.mCps);
    gySetLine(&capsule->mLine, &mBodyCollisionPos, &mBodyCollisionPos);     /* cM3dGLin::SetStartEnd */
    capsule->mRadius = gabi::fmuls_ppc(hio->mBodyRadius, hio->mActorScale); /* cM3dGCps radius */
    dComIfG_Ccsp_Set(&mCps);
}

/* 02169710 */
void daGy_c::setAimSpeedF() {

    daGy_HIO_c* hio = gy_hio();
    if (mpCtrl->mTarget == 0) {
        mAimSpeedF = hio->mSpeedSlow;
        f32 frame = mpMorf->getFrame();
        if (frame > hio->mStrokeStart && mpMorf->getFrame() < hio->mStrokeEnd) {
            mAimSpeedF = hio->mSpeedSlowStroke;
        }
    } else {
        mAimSpeedF = hio->mSpeedFast;
        f32 frame = mpMorf->getFrame();
        if (frame > hio->mStrokeStart && mpMorf->getFrame() < hio->mStrokeEnd) {
            mAimSpeedF = hio->mSpeedFastStroke;
        }
    }
}

/* 02167258 */
void daGy_c::modeDiveInit() {

    mMode = MODE_DIVE;
    mApproachRate = 0.0f;
}

/* 02169798 */
void daGy_c::modeDive() {

    if (mAnmIdxNext == 5) {
        return;
    }
    daGyCtrl_c* ctrl = mpCtrl;
    daGy_HIO_c* hio = gy_hio();
    mAnmIdxNext = 1;
    if (ctrl->mMode == MODE_ATTACK_PLAYER || mLineHit) {
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &mSpawnPos), 8, 0x400);
        mAimSpeedF = 0.0f;
        f32 y = hio->mDiveHeight;
        mBodyYTarget = y + y;
        actor_status = actor_status & ~fopAcStts_SHOWMAP_e;
        mBodyYStep = hio->mDiveHeightStep;
        gy_attnFlags(this) = gy_attnFlags(this) & ~4u;
        return;
    }
    actor_status = actor_status | fopAcStts_SHOWMAP_e;
    gy_attnFlags(this) = gy_attnFlags(this) | 4;
    mBodyYTarget = hio->mDiveHeight;
    mBodyYStep = hio->mDiveHeightStep;
    f32 dist = fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0));
    if (ctrl->mPathFree[mIndex] != 0 && mBodyY < hio->mDiveHeight + 10.0f &&
        dist > hio->mPlayerAvoidanceRadius) {
        modeCircleInit();
        return;
    }
    fopAc_ac_c* ship = dComIfGp_getShipActor();
    f32 len = gy_homeDistXZ(this);
    if (ship == nullptr) {
        return;
    }
    if (len > hio->mHomeRadius || ship->speedF > hio->mCircleShipSpeed) {
        setAimSpeedF();
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &mSpawnPos), 8, 0x400);
        mApproachRate = 0.0f;
        return;
    }
    gy_approachHome(this, ctrl);
}

/* 021695C0 */
void daGy_c::modeCircleInit() {

    daGy_HIO_c* hio = gy_hio();
    mMode = MODE_CIRCLE;
    s32 target = mpCtrl->mTarget;
    if (target == 0) {
        f32 r = cM_rndF((f32)(hio->mPlayerCircleMaxTime - hio->mPlayerCircleMinTime));
        mTimer = gabi::ftoi(r + (f32)hio->mPlayerCircleMinTime);
    } else if (target == 1) {
        f32 r = cM_rndF((f32)(hio->mShipCircleMaxTime - hio->mShipCircleMinTime));
        mTimer = gabi::ftoi(r + (f32)hio->mShipCircleMinTime);
    }
    mApproachRate = 0.0f;
}

/* 02169BEC */
void daGy_c::modeCircle() {

    if (mAnmIdxNext == 5) {
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    daGyCtrl_c* ctrl = mpCtrl;
    mAnmIdxNext = 1;
    mBodyYTarget = hio->mCircleHeight;
    mBodyYStep = hio->mHeightStep;
    if (ctrl->mPathFree[mIndex] == 0) {
        modeDiveInit();
        return;
    }
    fopAc_ac_c* ship = dComIfGp_getShipActor();
    f32 len = gy_homeDistXZ(this);
    if (ship == nullptr) {
        return;
    }
    if (len > hio->mHomeRadius || ship->speedF > hio->mCircleShipSpeed) {
        setAimSpeedF();
        if (fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0)) < hio->mPlayerAvoidanceRadius) {
            modeDiveInit();
            return;
        }
        if (ship->speedF > hio->mEscortShipSpeed) {
            modeWithCircleInit();
            return;
        }
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &mSpawnPos), 8, 0x400);
        mApproachRate = 0.0f;
        return;
    }
    gy_approachHome(this, ctrl);
    if (cLib_calcTimer_gy(&mTimer) != 0) {
        return;
    }
    if (dComIfGp_checkPlayerStatus0(0, 0x1000000)) {
        return;
    }
    s32 target = ctrl->mTarget;
    if (target == 0) {
        modeAttackPlayerInit();
    } else if (target == 1) {
        modeAttackInit();
    }
}

/* 02169AA8 */
void daGy_c::modeWithCircleInit() {

    daGy_HIO_c* hio = gy_hio();
    mMode = MODE_WITH_CIRCLE;
    mEscortRandomTime = gabi::ftoi(cM_rndF((f32)(s16)hio->mEscortRandomTimeRange));
    if (mEscortRandomTime <= hio->mMinEscortRandomTime) {
        mEscortRandomTime = hio->mMinEscortRandomTime;
    }
    mTimer = hio->mEscortCircleTime + mEscortRandomTime;
}

/* 0216AA10 */
void daGy_c::modeWithCircle() {

    if (mAnmIdxNext == 5) {
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    s32 index = mIndex;
    mBodyYTarget = hio->mCircleHeight;
    mBodyYStep = hio->mHeightStep;
    if (mpCtrl->mPathFree[index] == 0) {
        modeDiveInit();
        return;
    }
    fopAc_ac_c* ship = dComIfGp_getShipActor();
    if (ship == nullptr) {
        return;
    }
    if (ship->speedF < hio->mSpeedFast) {
        modeCircleInit();
    }
    cLib_addCalcAngleS2(&current.angle.y, ship->shape_angle.y, 8, 0x400);
    s16 angle = fopAcM_searchActorAngleY(ship, this);
    s32 dAngle = cLib_distanceAngleS(ship->shape_angle.y, angle);
    f32 dist = fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0));
    if (dAngle < hio->mEscortInnerAngle && dist > hio->mEscortJumpPlayerDistance) {
        mAimSpeedF = ship->speedF - 5.0f;
    } else {
        mAimSpeedF = ship->speedF + hio->mEscortSpeedOffset;
    }
    if (dAngle >= hio->mEscortOuterAngle) {
        return;
    }
    if (dAngle > hio->mEscortInnerAngle && dist > hio->mEscortJumpPlayerDistance) {
        /* jump out of the water next to the ship */
        speed.y = hio->mEscortJumpSpeedY;
        gy_seStart(this, 0x58CF, 0);
        mAnmIdxNext = 5;
        gy_deathSplash(this);
        modeWithAttackInit();
    }
}

// modeAttackInit
/* 02169B68 */
void daGy_c::modeAttackInit() {

    mMode = MODE_ATTACK;
    mAnmIdxNext = 3;
    mTargetPos.copy(dComIfGp_getPlayer(0)->current.pos);
    mAttackStartPos.copy(mSpawnPos);
    mTimer = gy_hio()->mAttackTime;
    mSubMode = 0;
}

// modeWithAttackInit
/* 0216A674 */
void daGy_c::modeWithAttackInit() {

    mMode = MODE_WITH_ATTACK;
    mTargetPos.copy(dComIfGp_getPlayer(0)->current.pos);
    mAttackStartPos.copy(mSpawnPos);
    mTimer = gy_hio()->mAttackTime;
    mSubMode = 0;
}

/* 0216A6F0 */
void daGy_c::modeWithAttack() {

    daGyCtrl_c* ctrl = mpCtrl;
    if (mAnmIdxNext != 5) {
        mAnmIdxNext = 3;
    }
    if (ctrl->mTarget == 0) {
        modeCircleInit();
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    s16 angle = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
    f32 dist = fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0));
    u32 sub = mSubMode;
    if (sub == 0) {
        mBodyYTarget = hio->mCircleHeight;
        mBodyYStep = hio->mHeightStep;
        cLib_addCalcAngleS2(&current.angle.y, angle, 8, 0x400);
        mAimSpeedF = hio->mEscortAttackSpeed;
        if (dist < hio->mAttackApproachDistance) {
            mSubMode = mSubMode + 1;
            mTimer = hio->mEscortAttackTime;
        }
    } else if (sub == 1) {
        mBodyYTarget = hio->mAttackHeight;
        mBodyYStep = hio->mHeightStep;
        mAimSpeedF = hio->mEscortAttackSpeed;
        if (gy_hitShip(this)) {
            gy_shipHitReaction(this);
            return;
        }
    } else if (sub == 2) {
        f32 y = hio->mDiveHeight;
        f32 d = mBodyY - y;
        mBodyYTarget = y;
        mBodyYStep = hio->mHeightStep;
        if (std::fabs(d) < 10.0f) {
            fopAcM_delete(this);
        }
        return;
    } else {
        return;
    }
    if (cLib_calcTimer_gy(&mTimer) != 0) {
        return;
    }
    if (hio->mDeleteAfterEscortAttack == 0) {
        modeWithCircleInit();
        return;
    }
    mSubMode = mSubMode + 1;
}

/* 02169FB8 */
void daGy_c::modeAttack() {

    if (mpCtrl->mTarget == 0) {
        modeCircleInit();
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    mBodyYTarget = hio->mAttackHeight;
    mBodyYStep = hio->mHeightStep;
    if (cLib_calcTimer_gy(&mTimer) == 0) {
        modeCircleInit();
    }
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0)) < hio->mPlayerAvoidanceRadius) {
        mAimSpeedF = 0.0f;
    } else {
        mAimSpeedF = hio->mShipAttackSpeed;
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &player->current.pos), 8,
                            0x400);
    }
    if (gy_hitShip(this)) {
        gy_shipHitReaction(this);
    }
}

/* 02169B48 */
void daGy_c::modeAttackPlayerInit() {

    mMode = MODE_ATTACK_PLAYER;
    mAnmIdxNext = 2;
    mSubMode = 0;
    mBiteCount = 0;
}

/* 0216A18C */
void daGy_c::modeAttackPlayer() {

    if (mpCtrl->mTarget == 1) {
        modeCircleInit();
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u32 sub = mSubMode;
    if (sub == 0) {
        /* approach */
        mBodyYTarget = hio->mBiteHeight;
        mBodyYStep = hio->mHeightStep;
        mAimSpeedF = hio->mPlayerApproachSpeed;
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &player->current.pos), 8,
                            0x400);
        gabi::Local<cXyz> diff;
        cXyz_mi(&mHeadPos, diff, &player->current.pos);
        gabi::Local<cXyz> diffXZ;
        diffXZ->x = diff->x;
        diffXZ->y = 0.0f;
        diffXZ->z = diff->z;
        if (std_sqrtf(PSVECSquareMag(diffXZ)) < hio->mPlayerAvoidanceRadius) {
            mAnmIdxNext = 10;
            mSubMode = mSubMode + 1;
        }
    } else if (sub == 1) {
        /* bite */
        mBodyYTarget = hio->mBiteHeight;
        mBodyYStep = hio->mHeightStep;
        mAimSpeedF = hio->mPlayerBiteSpeed;
        if (mAnmIdxNext == 10 && mpMorf->isStop()) {
            mAnmIdxNext = 2;
            setAnm();
            mAnmIdxNext = 10;
            mBiteCount = mBiteCount + 1;
        }
        if (mBiteCount > hio->mBiteCount) {
            mAnmIdxNext = 11;
            mSubMode = mSubMode + 1;
        }
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &player->current.pos), 8,
                            0x400);
    } else if (sub == 2) {
        /* the final bite */
        if (mHeadSph.ChkCoHit()) {
            fopAc_ac_c* ac = mHeadSph.GetCoHitAc();
            if (ac != nullptr && fpcM_GetName(ac) == fpcNm_PLAYER_e && mHitPlayer == 0) {
                s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
                mDoAud_monsSeStart_gy(0x48CF, &eyePos, (u32)mBsPcId, 0, reverb);
                gabi::Local<cXyz> dir;
                dir->z = 0.0f;
                dir->x = 0.0f;
                dir->y = 1.0f;
                dComIfGp_getVibration_StartShock(4, -0x21, dir);
                mHitPlayer = 1;
            }
        }
        f32 thr = l_debug.mBiteFrame + 10.0f; /* HD debug offset */
        if (mpMorf->getFrame() > thr) {
            mBodyYTarget = hio->mDiveHeight;
        } else {
            mBodyYTarget = hio->mBiteHeight;
        }
        mBodyYStep = hio->mHeightStep;
        mAimSpeedF = hio->mPlayerLungeSpeed;
        if (mAnmIdxNext == 11 && mpMorf->isStop()) {
            mAnmIdxNext = 1;
            mHitPlayer = 0;
            modeDiveInit();
        }
    }
}

/* 02169EE8 */
void daGy_c::modeAttackBackInit() {

    mMode = MODE_ATTACK_BACK;
    mAnmIdxNext = 4;
    speedF = gy_hio()->mRecoilSpeed;
    speed.y = gy_hio()->mRecoilVerticalSpeed;
    mDoAud_seStart(0x58D1, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    gy_deathSplash(this);
}

/* 0216A544 */
void daGy_c::modeAttackBack() {

    daGy_HIO_c* hio = gy_hio();
    mAimSpeedF = 0.0f;
    mBodyYTarget = hio->mDiveHeight;
    cLib_addCalcAngleS2(&mRecoilAngle, 0, 4, 0x800);
    if (gy_homeDistXZ(this, &mAttackStartPos) > hio->mHomeRadius) {
        setAimSpeedF();
        if (mAnmIdxNext == 1) {
            modeWithCircleInit();
        }
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &mAttackStartPos), 8, 0x400);
    } else if (mAnmIdxNext == 1) {
        modeCircleInit();
    }
}

/* 0216810C */
void daGy_c::modeDamageInit() {

    mAnmIdxNext = 6;
    mMode = MODE_DAMAGE;
    speed.y = 30.0f;
    mDoAud_seStart(0x58D1, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
}

/* 0216AC30 */
void daGy_c::modeDamage() {

    f32 sp = mDamageSpeed;
    mAimSpeedF = 0.0f;
    if (sp > 0.01f) {
        gy_damagePush(this);
    }
    if (mAnmIdxNext == 6 && mpMorf->isStop()) {
        s8 old = mAnmIdxOld;
        mAnmIdxNext = 1;
        if (old == 0) {
            modeDiveInit();
            return;
        }
        if ((u32)old == 6) {
            modeWithCircleInit();
            return;
        }
        modeCircleInit();
    }
}

/* 02167FE0 */
void daGy_c::modeDeleteInit() {

    actor_status = actor_status & ~fopAcStts_SHOWMAP_e;
    gy_attnFlags(this) = gy_attnFlags(this) & ~4u;
    s32 n = dSv_event_getEventReg(dComIfGs_getEvent(), 0x7EFF) + 1;
    u8 v = n > 0xFF ? 0xFF : (u8)n;
    dSv_event_setEventReg(dComIfGs_getEvent(), 0x7EFF, v);
    mMode = MODE_DELETE;
    mAimSpeedF = 0.0f;
    speedF = 0.0f;
    mAnmIdxNext = 8;
    speed.y = gy_hio()->mDeathVerticalSpeed;
    mDoAud_seStart(0x58CF, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    gy_deathSplash(this);
}

/* 0216ADD8 */
void daGy_c::modeDelete() {

    if (mLightTimer != -1) {
        if (cLib_calcTimer_gy(&mLightTimer) == 0) {
            mEnemyIce.mLightShrinkTimer = 1;
        }
        return;
    }
    if (mIceTimer != -1) {
        if (cLib_calcTimer_gy(&mIceTimer) == 0) {
            mEnemyIce.mFreezeDuration = gy_hio()->mFreezeDuration;
        }
        return;
    }
    if (mAnmIdxNext == 8 && mpMorf->isStop()) {
        fopAcM_createDisappear(this, &current.pos, 10, 0, 0xFF);
        fopAcM_delete(this);
    }
}

/* 02167F30 */
void daGy_c::modeDeleteBombInit() {

    gy_attnFlags(this) = gy_attnFlags(this) & ~4u;
    mAimSpeedF = 0.0f;
    mBodyY = 0.0f;
    speedF = 0.0f;
    mMode = MODE_DELETE_BOMB;
    mAnmIdxNext = 7;
    actor_status = actor_status & ~fopAcStts_SHOWMAP_e;
    speed.y = gy_hio()->mDeathVerticalSpeed;
    mDoAud_seStart(0x58CF, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    mBodyYStep = gy_hio()->mDiveHeightStep * 10.0f;
}

/* 0216AEC8 */
void daGy_c::modeDeleteBomb() {

    if (mDamageSpeed > 0.01f) {
        gy_damagePush(this);
    }
    daGy_HIO_c* hio = gy_hio();
    s8 anm = mAnmIdxNext;
    if (anm == 7) {
        mBodyYTarget = hio->mDeathSinkHeight;
        if (!(mBodyY > hio->mDeathSinkHeight + 10.0f)) {
            mAnmIdxNext = 9;
            mBodyYStep = hio->mHeightStep;
        }
        anm = mAnmIdxNext;
    }
    if (anm != 9) {
        return;
    }
    f32 bottom = hio->mDeathFloatHeight;
    f32 y = mBodyY;
    if (!(y < bottom - 10.0f)) {
        if ((f32)(s32)mDisappearTimer == -1.0f) {
            mDisappearTimer = hio->mDeathFloatTime;
        }
        mBodyYStep = hio->mDeathFloatStep;
        y = mBodyY;
        mBodyYScale = hio->mDeathFloatApproachRate;
        bottom = hio->mDeathFloatHeight;
    }
    /* bob between bottom - m170 and bottom + m170 */
    f32 range = hio->mDeathFloatAmplitude;
    f32 low = bottom - range;
    f32 high = bottom + range;
    if (!(y > low + 10.0f)) {
        mBodyYTarget = high;
    } else if (!(y < high - 10.0f)) {
        mBodyYTarget = low;
    }
    if ((f32)(s32)mDisappearTimer != -1.0f && cLib_calcTimer_gy(&mDisappearTimer) == 0) {
        fopAcM_createDisappear(this, &current.pos, 10, 0, 0xFF);
        fopAcM_delete(this);
    }
}

// HD: GHS pointer-to-member dispatch retains HIO mode overrides.
/* 02168938 */
void daGy_c::modeProcCall() {

    if (gy_hio()->mDebugMode != 0) {
        mMode = gy_hio()->mDebugModeNo;
    }
    ptmf_call(gabi::ea(&l_modeTable.mWords[2 * (s32)mMode]), this);
}

/* 021675C0 */
void daGy_c::createWave() {

    /* function-local statics (guard word + value) */
    if (l_waveOffsets.mLeftGuard == 0) {
        l_waveOffsets.mLeft.y = 1.0f;
        l_waveOffsets.mLeftGuard = 1;
        l_waveOffsets.mLeft.x = 0.5f;
        l_waveOffsets.mLeft.z = -0.3f;
    }
    if (l_waveOffsets.mRightGuard == 0) {
        l_waveOffsets.mRight.y = 1.0f;
        l_waveOffsets.mRightGuard = 1;
        l_waveOffsets.mRight.x = -0.5f;
        l_waveOffsets.mRight.z = -0.3f;
    }
    cXyz* waveOffsetL = &l_waveOffsets.mLeft;
    cXyz* waveOffsetR = &l_waveOffsets.mRight;
    if (mWave[0].mpBaseEmitter == nullptr) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x37, &mSplashPos, &mSplashAngle, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&mWave[0], -1, nullptr, nullptr, nullptr);
        if (mWave[0].mpBaseEmitter != nullptr) {
            auto* emitter = reinterpret_cast<GyEmitterHD*>(mWave[0].mpBaseEmitter.get());
            emitter->mGlobalScale.x = waveOffsetL->x;
            emitter->mGlobalScale.y = waveOffsetL->y;
            emitter->mGlobalScale.z = waveOffsetL->z;
        }
    }
    if (mWave[1].mpBaseEmitter == nullptr) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x37, &mSplashPos, &mSplashAngle, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&mWave[1], -1, nullptr, nullptr, nullptr);
        if (mWave[1].mpBaseEmitter != nullptr) {
            auto* emitter = reinterpret_cast<GyEmitterHD*>(mWave[1].mpBaseEmitter.get());
            emitter->mGlobalScale.x = waveOffsetR->x;
            emitter->mGlobalScale.y = waveOffsetR->y;
            emitter->mGlobalScale.z = waveOffsetR->z;
        }
    }
    if (mSplash.mpBaseEmitter == nullptr) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x35, &mSplashPos, &mSplashAngle, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&mSplash, -1, nullptr, nullptr, nullptr);
    }
}

/* 02168640 */
void daGy_c::setWave() {

    daGy_HIO_c* hio = gy_hio();
    s8 anm = mAnmIdxNext;
    f32 scaleZ = hio->mWaveMaxSpeed;
    f32 waveY;
    f32 splashRate;
    u32 mode;
    if (((u32)anm >= 5 && (u32)anm <= 6) || ((u32)anm >= 8 && (u32)anm <= 9)) {
        mode = mMode;
        waveY = 0.0f;
        splashRate = 0.0f;
    } else {
        mode = mMode;
        waveY = hio->mWaveSpeed;
        splashRate = hio->mSplashSpeed;
    }
    if (mode < 1 || (mode != MODE_CIRCLE && mode == MODE_DELETE)) {
        waveY = 0.0f;
        splashRate = 0.0f;
    } else if (mode == MODE_CIRCLE) {
        f32 g = l_debug.mWaveScale; /* HD debug offset */
        f32 a = g + 0.7f;
        f32 b = g + 0.6f;
        waveY = gabi::fmuls_ppc(waveY, a);
        scaleZ = gabi::fmuls_ppc(hio->mWaveMaxSpeed, b);
    }
    if (anm == 10) {
        splashRate = 0.0f;
        waveY = 0.0f;
    }
    if (mWave[0].mpBaseEmitter != nullptr) {
        gy_emitterStop(mWave[0].mpBaseEmitter, !(waveY > 0.0f));
    }
    if (mWave[1].mpBaseEmitter != nullptr) {
        gy_emitterStop(mWave[1].mpBaseEmitter, !(waveY > 0.0f));
    }
    mSplashPos.y = daSea_calcWave(mSplashPos.x, mSplashPos.z);
    mSplashAngle.y = current.angle.y;
    GyWaveHD* w0 = &mWave[0];
    GyWaveHD* w1 = &mWave[1];
    w1->mVelFade1 = waveY;
    w0->mVelFade1 = waveY;
    w0->mMaxParticleVelocity = scaleZ;
    w1->mMaxParticleVelocity = scaleZ;
    w1->mVelFade2 = hio->mWavePitchDifference + 1.0f;
    w0->mVelFade2 = 1.0f - hio->mWavePitchDifference;
    f32 v38 = hio->mCollapseFarY, v34 = hio->mCollapseFarX, v2C = hio->mCollapseNearY,
        v28 = hio->mCollapseNearX, v30 = hio->mCollapseNearZ, v3C = hio->mCollapseFarZ;
    w0->mCollapsePos[0].z = v30;
    w1->mCollapsePos[0].y = v2C;
    w0->mCollapsePos[1].z = v3C;
    w0->mCollapsePos[0].y = v2C;
    w0->mCollapsePos[1].x = -v34;
    w1->mCollapsePos[0].z = v30;
    w0->mCollapsePos[0].x = -v28;
    w1->mCollapsePos[1].x = v34;
    w1->mCollapsePos[0].x = v28;
    w1->mCollapsePos[1].y = v38;
    w1->mCollapsePos[1].z = v3C;
    w0->mCollapsePos[1].y = v38;
    w1->mVelSpeed = hio->mWaveDistanceSpeed;
    w0->mVelSpeed = hio->mWaveDistanceSpeed;
    cLib_addCalc2(&mSplashRate, splashRate, 0.1f, 10.0f);
    if (mSplash.mpBaseEmitter != nullptr) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        bool stop = true;
        if (mSplashRate > 0.1f && player->current.pos.y < 1500.0f) {
            stop = false;
        }
        gy_emitterStop(mSplash.mpBaseEmitter, stop);
    }
    mSplash.mScaleTimer = mSplashRate;
    mSplash.mMaxScaleTimer = hio->mMaxSplashSpeed;
}

// HD keeps LineCross's result in r3 (the GameCube signature is void)
/* 02167BBC */
void daGy_c::lineCheck(cXyz* start, cXyz* end) {

    dBgS_LinChk_Set(&mLinChk, start, end, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
        end->copy(mLinChk.mLin.mEnd); /* the cross point */
        mLineHit = 1;
    }
}

/* 02168170 */
void daGy_c::checkTgHit() {

    mStts.Move();
    gabi::Local<cXyz> hitPos;
    void* hitObj = mCps.GetTgHitObj();
    f32 hx = mCps.mGObjTg.mHitPos.x;
    f32 hy = mCps.mGObjTg.mHitPos.y;
    hitPos->x = hx;
    f32 hz = mCps.mGObjTg.mHitPos.z;
    hitPos->y = hy;
    hitPos->z = hz;
    if (hitObj == nullptr) {
        hitObj = mHeadSph.GetTgHitObj();
        hitPos->copy(mHeadSph.mGObjTg.mHitPos);
        if (hitObj == nullptr) {
            hitObj = mSph.GetTgHitObj();
            hitPos->copy(mSph.mGObjTg.mHitPos);
            if (hitObj == nullptr) {
                return;
            }
        }
    }
    daGy_HIO_c* hio = gy_hio();
    u32 type = static_cast<dCcD_GObjInf*>(hitObj)->mAtType; /* the hit object's At type */
    bool damaged = true;
    if (type & GY_AT_BOMB) {
        health = 0;
        mDamageSpeed = hio->mBombDamageSpeed;
    } else if (type & GY_AT_NORMAL_ARROW /* normal arrow */) {
        gy_seStart(this, 0x2834, 0x20);
        health = health - 2;
        mDamageSpeed = hio->mArrowDamageSpeed;
    } else if (type & GY_AT_ICE_ARROW /* ice arrow */) {
        mFreezeType = 9;
        gy_seStart(this, 0x2834, 0x20);
        mIceTimer = 10;
        gy_removeWaves(this);
        damaged = false;
    } else if (type & GY_AT_FIRE_ARROW /* fire arrow */) {
        gy_seStart(this, 0x2834, 0x20);
        health = health - 2;
        mDamageSpeed = hio->mArrowDamageSpeed;
    } else if (type & GY_AT_LIGHT_ARROW /* light arrow */) {
        mFreezeType = 11;
        gy_seStart(this, 0x2834, 0x20);
        mLightTimer = 10;
        gy_removeWaves(this);
        damaged = false;
    } else if (type & GY_AT_HOOKSHOT) {
        gy_seStart(this, 0x2834, 0x20);
        health = health - 2;
        mDamageSpeed = hio->mHookshotDamageSpeed;
    } else if (type & GY_AT_BOOMERANG /* boomerang */) {
        gy_seStart(this, 0x2833, 0x20);
        health = health - 2;
        mDamageSpeed = hio->mBoomerangDamageSpeed;
    } else {
        damaged = false;
    }
    if (damaged) {
        mDoAud_onEnemyDamage_gy();
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        dPa_control_set(dComIfGp_getParticle(), 0, 0x10, hitPos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr,
                        nullptr, nullptr);
        if (mFreezeType == 11) {
            gabi::Local<cXyz> sc;
            sc->y = 2.0f;
            sc->z = 2.0f;
            sc->x = 2.0f;
            dPa_control_set(dComIfGp_getParticle(), 0, 0xF, hitPos, &player->shape_angle, sc, 0xFF, nullptr,
                            -1, nullptr, nullptr, nullptr);
            gy_seStart(this, 0x2828, 0);
            return;
        }
        gabi::Local<cXyz> sc2;
        if (health <= 0) {
            sc2->y = 2.0f;
            sc2->z = 2.0f;
            sc2->x = 2.0f;
            dPa_control_set(dComIfGp_getParticle(), 0, 0xF, hitPos, &player->shape_angle, sc2, 0xFF, nullptr,
                            -1, nullptr, nullptr, nullptr);
            gy_seStart(this, 0x2828, 0);
            health = 0;
            s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
            mDoAud_monsSeStart_gy(0x48CE, &eyePos, (u32)mBsPcId, 0, reverb);
            if (mDamageSpeed == hio->mBombDamageSpeed) {
                modeDeleteBombInit();
                return;
            }
            modeDeleteInit();
            return;
        }
        mPrevMode = mMode;
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_monsSeStart_gy(0x48CD, &eyePos, (u32)mBsPcId, 0, reverb);
        modeDamageInit();
        return;
    }
    if (mFreezeType == 9 || mFreezeType == 11) {
        modeDeleteInit();
    }
}

/* 02167270 */
f32 daGy_c::getWaterY() {

    f32 waterY;
    if (mAcch.m_flags & dBgS_Acch::WATER_HIT) {
        waterY = mAcch.m_wtr.m_height; /* the water check's height */
    } else {
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x;
        f32 y = current.pos.y + 1000.0f;
        f32 z = current.pos.z;
        pos->x = x;
        pos->z = z;
        pos->y = y;
        waterY = dBgS_GetWaterHeight(pos);
    }
    if (current.pos.y > waterY) {
        gravity = -2.5f;
        return current.pos.y;
    }
    gravity = 0.0f;
    return waterY;
}

/* 021689C8 */
BOOL daGy_c::_execute() {

    mpCtrl = nullptr;
    gabi::Local<be<u32>> parent;
    if (parentActorID == (u32)-1 || !fopAcM_SearchByID_gy(parentActorID, parent) || *parent == 0) {
        fopAcM_delete(this);
        return TRUE;
    }
    daGyCtrl_c* ctrl;
    if (!fopAc_IsActor(gabi::at<void>(*parent)) || (ctrl = gabi::at<daGyCtrl_c>(*parent)) == nullptr ||
        (fpcM_GetName(ctrl) != fpcNm_GY_CTRL_e &&
         (ctrl == nullptr || fpcM_GetName(ctrl) != fpcNm_GY_CTRLB_e))) {
        if (cLib_calcTimer_gy(&mParentWaitTimer) != 0) {
            return TRUE;
        }
        fopAcM_delete(this);
        return TRUE;
    }
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    mpCtrl = ctrl;
    shape_angle.z = current.angle.z;
    if (enemy_ice_gy(&mEnemyIce)) {
        J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
        mpMorf->calc();
        mpMorf2->calc();
        return TRUE;
    }
    lineCheck(&current.pos, &mLineEnd);
    if (mMode != MODE_DIVE) {
        if (mLineHit) {
            modeDiveInit();
        }
        if (mMode != MODE_DIVE && ctrl->mMode == MODE_ATTACK_PLAYER) {
            modeDiveInit();
        }
    }
    mSpawnPos.copy(ctrl->mSpawnPositions[mIndex]);
    mpMorf->play(nullptr, 0, 0);
    mpMorf->calc();
    mpMorf2->play(nullptr, 0, 0);
    mpMorf2->calc();
    if (mMode != MODE_DIVE) {
        setAtCollision();
        setCollision();
        checkTgHit();
    }
    if (gravity == 0.0f) {
        cLib_addCalc2(&mBodyY, mBodyYTarget, mBodyYScale, mBodyYStep);
    }
    cLib_addCalc2(&speedF, mAimSpeedF, 0.3f, 4.0f);
    setAnm();
    daGy_HIO_c* hio = gy_hio();
    if (hio->mFloatOnWater != 0) {
        /* HIO: float on the water */
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x;
        f32 y = current.pos.y + 1000.0f;
        f32 z = current.pos.z;
        pos->x = x;
        pos->z = z;
        pos->y = y;
        current.pos.y = dBgS_GetWaterHeight(pos);
        setMtx();
        setWave();
        return TRUE;
    }
    modeProcCall();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    current.pos.y = getWaterY();
    s8 anm = mAnmIdxNext;
    if (anm == 5) {
        f32 sy = speed.y;
        if (sy < -5.0f) {
            cLib_addCalcAngleS2(&current.angle.x, (s16)(l_debug.mDiveAngle + 0x2000), 8, 0x400);
        } else if (sy > 5.0f) {
            cLib_addCalcAngleS2(&current.angle.x, (s16)(l_debug.mRiseAngle - 0x1000), 8, 0x400);
        } else {
            cLib_addCalcAngleS2(&current.angle.x, 0, 8, 0x800);
        }
        if (gravity == 0.0f && mOldGravity < 0.0f) {
            mAnmIdxNext = 1;
            gy_landSplash(this, 2.0f, true, 0x58D0);
        }
    } else if (anm == 6 || anm == 4) {
        if (mMode == MODE_ATTACK_BACK) {
            cLib_addCalcAngleS2(&current.angle.x, 0, 8, 0x800);
        }
        daSea_calcWave(current.pos.x, current.pos.z);
        if (gravity == 0.0f && mOldGravity < 0.0f) {
            gy_landSplash(this, 1.0f, false, 0x58D2);
        }
    } else if (anm == 7 || anm == 9) {
        daSea_calcWave(current.pos.x, current.pos.z);
        if (gravity == 0.0f && mOldGravity < 0.0f) {
            gy_landSplash(this, 2.0f, true, 0x58D0);
        }
    } else {
        cLib_addCalcAngleS2(&current.angle.x, 0, 8, 0x800);
    }
    if (mMode != MODE_DIVE) {
        mAcch.CrrPos(dComIfG_Bgsp());
    }
    tevStr.mRoomNo = current.roomNo; /* tevStr.mRoomNo = roomNo */
    tevStr.mEnvrIdxOverride =
        (u8)dBgS_GetPolyColor_gy(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
    setMtx();
    setWave();
    {
        f32 z = current.pos.z;
        f32 y = current.pos.y;
        attention_info.position.z = z; /* attention_info.position */
        eyePos.z = z;
        f32 x = current.pos.x;
        attention_info.position.x = x;
        eyePos.x = x;
        f32 g = gravity;
        attention_info.position.y = y + 160.0f;
        mOldGravity = g;
        eyePos.y = y + 60.0f;
    }
    if (mAnmIdxNext != 5 && mMode != MODE_DIVE) {
        gabi::Local<cXyz> move;
        cXyz_mi(&current.pos, move, &old.pos);
        f32 rate = std_sqrtf(PSVECSquareMag(move)) / 30.0f;
        if (rate > 0.0f) {
            if (!(rate < 1.0f)) {
                rate = 1.0f;
            }
        } else {
            rate = 0.0f;
        }
        s8 vol = (s8)gabi::ftoi(rate * 100.0f);
        mDoAud_seStart(0x50CE, &eyePos, vol, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        if (mAnmIdxNext == 1) {
            f32 r = gabi::fmuls_ppc(rate, hio->mSwimAnimationScale);
            f32 lo = hio->mMinimumAnimationSpeed;
            f32 d = gabi::fsubs_ppc(r, lo);
            f32 speedRate = d >= 0.0f ? r : lo; /* fsel */
            mpMorf->setPlaySpeed(speedRate);
            mpMorf2->setPlaySpeed(speedRate);
        }
    }
    g_env_light.settingTevStruct(0, &current.pos, &tevStr);
    return TRUE;
}

// HD: debug drawing is compiled out; inlined colour initializations remain.
/* 02169268 */
void daGy_c::drawDebug() {

    if (gy_initStatic(GY_DBG_COLOR0) && gy_initStatic(GY_DBG_COLOR0))
        gy_initStatic(GY_DBG_COLOR0);
    if (gy_initStatic(GY_DBG_COLOR1) && gy_initStatic(GY_DBG_COLOR1))
        gy_initStatic(GY_DBG_COLOR1);
    gy_initStatic(GY_DBG_COLOR2);
    if (dComIfGp_getShipActor() != nullptr) {
        if (gy_initStatic(GY_DBG_COLOR0))
            gy_initStatic(GY_DBG_COLOR0);
    }
    if (dComIfGp_getShipActor() != nullptr) {
        if (gy_initStatic(GY_DBG_COLOR1))
            gy_initStatic(GY_DBG_COLOR1);
    }
    dComIfGp_ea();
    if (gy_initStatic(GY_DBG_COLOR1))
        gy_initStatic(GY_DBG_COLOR1);
    gy_initStatic(GY_DBG_COLOR0);
}

/* 021694E0 */
BOOL daGy_c::_draw() {

    daGy_HIO_c* hio = gy_hio();
    if (hio->mDebugOffset != 0) {
        drawDebug();
    }
    if ((mBodyY > hio->mDiveHeight || mAnmIdxNext == 5) && hio->mHideModel == 0) {
        mDoExt_McaMorf* morf = mpMorf;
        g_env_light.setLightTevColorType(morf->getModel(), &tevStr);
        if (mEnemyIce.mFreezeTimer > 20) {
            mDoExt_invisibleModel_entryDL_gy(mpMorf, -1, mInvisibleModel);
            return TRUE;
        }
        mpMorf->entryDL();
        dSnap_RegistFig(0xBC, this, 1.0f, 1.0f, 1.0f);
        mpMorf2->entryDL();
    }
    return TRUE;
}

/* 0216779C */
void daGy_c::createInit() {

    daGy_HIO_c* hio = gy_hio();
    // HD: preserve each integer load/store pair, including signaling NaN bits.
    auto* source = reinterpret_cast<GyQuaternionWordsHD*>(&l_zeroQuaternion);
    auto* destination = reinterpret_cast<GyQuaternionWordsHD*>(&mHeadQuat);
    for (s32 i = 0; i < 4; i++)
        destination->mWords[i] = source->mWords[i];
    gbaName = 0x1D;
    f32 s = hio->mActorScale;
    mLightTimer = -1;
    scale.z = s;
    mIceTimer = -1;
    scale.x = s;
    mEnemyIce.mWallRadius = 0.0f;
    mEnemyIce.mReactionFlag = 1;
    mEnemyIce.mpActor = this;
    scale.y = s;
    mEnemyIce.mCylHeight = 0.0f;
    itemTableIdx = dComIfGp_CharTbl_GetNameIndex_gy("GyCtrl" /* "GyCtrl" */, 0);
    mAnmIdxNext = 1;
    gy_attnFlags(this) = 4;
    attention_info.distances[2] = 0x22; /* attention_info.distances[2] */
    mDisappearTimer = -1;
    mBodyYScale = hio->mHeightApproachRate;
    mParentWaitTimer = 0x78;
    mBodyY = hio->mDiveHeight;
    modeDiveInit();
    mSpawnPos.copy(current.pos);
    for (s32 i = 0; i < J3DModel_getModelData(mpMorf->getModel())->getJointNum(); i++) {
    }
    setMtx();
    mpMorf->calc();
    mpMorf2->calc();
    g_env_light.settingTevStruct(0, &current.pos, &tevStr);
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mAcch.m_flags =
        mAcch.m_flags | dBgS_Acch::WALL_NONE | dBgS_Acch::ROOF_NONE; /* SetWallNone, SetRoofNone */
    gravity = -2.5f;
    fopAcM_posMoveF(this, nullptr);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel()));
    mStts.Init(100, 100, this);
    mSph.Set(&l_sph_src); /* l_sph_src */
    mSph.SetStts(&mStts);
    gySetCapsule(&mCps, &l_cps_src); /* dCcD_Cps::Set(l_cps_src) */
    mCps.SetStts(&mStts);
    mHeadSph.Set(&l_sph_head_src); /* l_sph_head_src */
    mHeadSph.SetStts(&mStts);
    max_health = 1;
    health = 1;
    createWave();
}

/* 02167A00 */
cPhs_State daGy_c::_create() {

    if (!(actor_condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            daGy_ct(this);
        }
        actor_condition = actor_condition | fopAcCnd_INIT_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhase, "gy");
    if (phase == cPhs_COMPLEATE_e) {
        if (!dComIfGs_checkGetItem(0x2D)) {
            return cPhs_ERROR_e;
        }
        if (parentActorID == (u32)-1) {
            return cPhs_ERROR_e;
        }
        gabi::Local<be<u32>> parent;
        if (!fopAcM_SearchByID_gy(parentActorID, parent)) {
            return cPhs_ERROR_e;
        }
        if (*parent == 0 || !fopAc_IsActor(gabi::at<void>(*parent))) {
            return cPhs_ERROR_e;
        }
        u32 ctrl = *parent;
        if (ctrl == 0) {
            return cPhs_ERROR_e;
        }
        if (fpcM_GetName(gabi::at<void>(ctrl)) != fpcNm_GY_CTRL_e) {
            if (ctrl == 0 || fpcM_GetName(gabi::at<void>(ctrl)) != fpcNm_GY_CTRLB_e) {
                return cPhs_ERROR_e;
            }
        }
        mpCtrl = gabi::at<daGyCtrl_c>(ctrl);
        mIndex = mpCtrl->mSpawnIndex;
        mpCtrl->mSpawnIndex = mpCtrl->mSpawnIndex + 1;
        ctrl = *parent;
        if (ctrl != 0 && fpcM_GetName(gabi::at<void>(ctrl)) == fpcNm_GY_CTRL_e) {
            fopAcM_setStageLayer(this);
        }
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn(reinterpret_cast<const void*>(createHeap_CB)),
                                   0x3FA0)) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return phase;
}

/* 02167B64 */
BOOL daGy_c::_delete() {

    dComIfG_resDelete(&mPhase, "gy");
    gyRemoveWave(&mWave[1]); /* dPa_waveEcallBack::remove */
    gyRemoveWave(&mWave[0]);
    gyRemoveSplash(&mSplash); /* dPa_splashEcallBack::remove */
    return TRUE;
}

/* 02167B60 */
static cPhs_State daGyCreate(void* i_this) { return ((daGy_c*)i_this)->_create(); }

/* 02167BB8 */
static BOOL daGyDelete(void* i_this) { return ((daGy_c*)i_this)->_delete(); }

/* 02169264 */
static BOOL daGyExecute(void* i_this) { return ((daGy_c*)i_this)->_execute(); }

/* 021695BC */
static BOOL daGyDraw(void* i_this) { return ((daGy_c*)i_this)->_draw(); }

/* 0216B5A4 */
static BOOL daGyIsDelete(void* unused) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
// This TU's SafeString deleting destructor.
/* 0216B590 */
static void SafeString_dt(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// this TU's sead::SafeString::assureTerminationImpl_ (empty)
/* 0216B6E4 */
static void SafeString_assureTermination(void* unused) {}

// static initialisation of the translation unit
/* 0216B4F0 */
static void __sinit_d_a_gy_cpp() {

    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    daGy_HIO_ct(gy_hio());
}

// Actor deleting destructor.
/* 0216B5AC */
static void daGy_dt(daGy_c* i_this, s32 flags) {

    if (i_this == nullptr)
        return;
    /* ~dBgS_ObjLinChk (inline: the per-TU vtables, then cBgS_LinChk::~cBgS_LinChk) */
    i_this->mLinChk.dBgS_PolyPassChk::__vtbl = 0x10010958;
    i_this->mLinChk.dBgS_GrpPassChk::__vtbl = 0x100108E8;
    i_this->mLinChk.cBgS_PolyInfo::__vtbl = 0x100108D8;
    gyDestroyLineBase(&i_this->mLinChk, 0);
    /* ~dBgS_ObjAcch (mAcch2) */
    i_this->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    i_this->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    gyDestroyAcch(&i_this->mEnemyIce.mBgAcch, 0);
    gyDestroyCircle(&i_this->mEnemyIce.mBgAcchCir.m_cir,
                    2); /* ~dBgS_AcchCir (inline): ~cM3dGCir on its circle */
    dCcD_Cyl_dt(&i_this->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&i_this->mEnemyIce.mStts, 2);
    dCcD_Stts_dt(&i_this->mStts, 2);
    gyDestroyCapsule(&i_this->mCps, 2); /* ~dCcD_Cps (the matcher: ~dCcD_GObjInf) */
    gyDestroySphere(&i_this->mSph, 2);  /* ~dCcD_Sph */
    gyDestroySphere(&i_this->mHeadSph, 2);
    gyDestroyCircle(&i_this->mAcchCir.m_cir, 2);
    i_this->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    i_this->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    gyDestroyAcch(&i_this->mAcch, 0);
    mDoExt_invisibleModel_dt(i_this->mInvisibleModel); /* ~mDoExt_invisibleModel */
    gyDestroyActor(i_this, 0);                         /* ~fopAc_ac_c */
    if (flags & 1)
        operator_delete(i_this);
}

// daGy_c::daGy_c (GameCube: weak, out of line)
/* 02166FC4 */
daGy_c* daGy_ct(daGy_c* i_this) {

    if (i_this == nullptr) {
        i_this = (daGy_c*)operator_new(sizeof(daGy_c));
        if (i_this == nullptr)
            return i_this;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = GY_VTBL;
    mDoExt_invisibleModel_ct(i_this->mInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    dBgS_ObjAcch_ct(&i_this->mAcch, gc_tu.objAcch);
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    gyConstructSphere(&i_this->mHeadSph); /* dCcD_Sph::dCcD_Sph */
    gyConstructSphere(&i_this->mSph);
    /* dCcD_Cps (inline) */
    gyConstructObjectInfo(&i_this->mCps); /* dCcD_GObjInf::dCcD_GObjInf */
    auto* shape = reinterpret_cast<GyShapeVtablesHD*>(i_this->mCps.mShapeAttr);
    auto* capsule = reinterpret_cast<GyCapsuleHD*>(i_this->mCps.mCps);
    shape->mShapeVtable = 0x100015A8; /* cCcD_ShapeAttr */
    shape->mAabVtable = GY_AAB_VTBL;
    gyConstructCapsule(capsule); /* cM3dGCps::cM3dGCps */
    i_this->mCps.__vtbl_hitinf = 0x1004AF18;
    capsule->mLine.__vtbl = 0x1004AF60;
    shape->mShapeVtable = 0x1004AF70;
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Stts_ct(&i_this->mEnemyIce.mStts);
    dCcD_Cyl_ct(&i_this->mEnemyIce.mCyl, GY_AAB_VTBL);
    dBgS_AcchCir_ct(&i_this->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&i_this->mEnemyIce.mBgAcch, gc_tu.objAcch);
    /* the wave callbacks (their cXyz members' constructors allocate only for a null this) */
    for (int w = 0; w < 2; w++) {
        u32 cb = gabi::ea(&i_this->mWave[w]);
        i_this->mWave[w].__vtbl = 0x100521A8;
        if (cb + 0x3C == 0)
            operator_new(0xC);
        if (cb + 0x48 == 0)
            operator_new(0xC);
        if (cb + 0x54 == 0)
            operator_new(0xC);
    }
    i_this->mSplash.__vtbl = 0x100521E8;
    dBgS_LinChk_ct(&i_this->mLinChk, GY_LINCHK_VT, true);
    return i_this;
}
/* WWHD glue
data l_jointHitData 101B6C24
str 10010C4C "gy"
str 100109D8 "d_a_gy.cpp"
str 100109E4 "modelData != (0)"
str 10010A20 "GyCtrl"
data l_colourGuards 101FDA44
data l_colours 101FEBE8
data l_colourSources 100108A0
data l_animationIndices 10010A30
data l_animationParameters 10010A78
ext 0200E814 s32 gyGetCharacterIndex(void* table,const char* name,s32 index)
ext 025877F8 void gySetAnimation(const char* arc,mDoExt_McaMorf* model,be<s8>* animation,be<s8>* parameter,be<s8>* previous,const dLib_anm_idx_c* indices,const dLib_anm_prm_c* parameters,u32 modify,mDoExt_McaMorf* waterModel)
data l_sph_head_src 101B6B38
data l_sph_src 101B6B78
data l_cps_src 101B6BD8
ext 027F596C void gyPublishModelFlags(J3DModel* model,u32 flags)
ext 025A92C0 void gyRemoveWave(GyWaveHD* callback)
ext 025A99B8 void gyRemoveSplash(GySplashHD* callback)
ext 02008B4C void gyDestroyLineBase(dBgS_LinChk* check,s32 flags)
ext 024EFD9C void gyDestroyAcch(dBgS_ObjAcch* check,s32 flags)
ext 02018034 void gyDestroyCircle(cM3dGCir_l* circle,s32 flags)
ext 02515980 void gyDestroyCapsule(dCcD_Cps* capsule,s32 flags)
ext 02515AE8 void gyDestroySphere(dCcD_Sph* sphere,s32 flags)
ext 025D50BC void gyDestroyActor(fopAc_ac_c* actor,s32 flags)
ext 025166F0 void gyConstructSphere(dCcD_Sph* sphere)
ext 02515FB8 void gyConstructObjectInfo(dCcD_Cps* object)
ext 02018150 void gyConstructCapsule(GyCapsuleHD* capsule)
ext 025164C0 void gySetCapsule(dCcD_Cps* capsule,GyCapsuleSourceHD* source)
ext 02018808 void gySetLine(cM3dGLin* line,cXyz* start,cXyz* end)
data l_HIO 1046446C
data l_debug 1047BBB0
data l_waveOffsets 10464608
data l_modeTable 10010B44
data l_zeroQuaternion 101E9C38
ext 0259138C void mDoExt_invisibleModel_entryDL_gy(mDoExt_McaMorf* morf, s32 p, void* inv)
ext 020402C8 BOOL enemy_ice_gy(GyIceHD* e)
ext 0211D2F8 s32 cLib_calcTimer_gy(be<s32>* t)
ext 024EEEB8 s32 dBgS_GetPolyColor_gy(dBgS* bgs, void* p)
ext 025E1FD8 void mDoAud_onEnemyDamage_gy()
ext 025E1AA4 void mDoAud_monsSeStart_gy(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb)
ext 025D54C4 BOOL fopAcM_SearchByID_gy(fpc_ProcID id, be<u32>* out)
ext 02552B60 u32 gyCreateJointHit(J3DModel* m, u32 data, s16 n)
data gyCurrentModel 104B462C
data gyCurrentMatrix 104B4868
ext 028E9108 void gyConcatMatrices(const Mtx34* first,const Mtx34* second,Mtx34* output)
ext 025F25CC void gyApplyQuaternion(const Quaternion* quaternion)
ext 023127F8 void gyQuaternionFromVectors(Quaternion* quaternion,const cXyz* first,const cXyz* second)
*/
