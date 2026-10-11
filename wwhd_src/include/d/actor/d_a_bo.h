// Boko Baba: WWHD expands the actor base and damage-reaction objects.
// HD: fields unnamed in the GameCube placeholder are named from their verified use.
#pragma once
#include "bindings.h"
#include "c/c_damagereaction.h"
#include "gc_api/f_op/f_op_actor.h"

// HD: the emitter's render state grows before the global transform and colors.
struct boEmitterView_c {
    u8 _dynamicsPrefix[0x34];
    be<f32> mRate;
    u8 _framePrefix[0x5C - 0x38];
    be<s32> mMaxFrame;
    u8 _velocityPrefix[8];
    be<f32> mInitialVelOmni, mInitialVelAxis;
    u8 _callbackPrefix[0x1E8 - 0x70];
    gptr<void> mpEmtrCallBack;
    u8 _rotationPrefix[4];
    Mtx34 mGlobalRotation;
    cXyz mGlobalDynamicsScale, mGlobalTranslation, mGlobalParticleScale;
    struct {
        be<u8> r, g, b, a;
    } mGlobalPrmColor, mGlobalEnvColor;
    u8 _statusPrefix[8];
    be<u32> mStatus;
};
WWHD_SIZE(boEmitterView_c, 0x258);
WWHD_OFFSET(boEmitterView_c, mMaxFrame, 0x5C);
WWHD_OFFSET(boEmitterView_c, mpEmtrCallBack, 0x1E8);
WWHD_OFFSET(boEmitterView_c, mGlobalRotation, 0x1F0);
WWHD_OFFSET(boEmitterView_c, mGlobalParticleScale, 0x238);
WWHD_OFFSET(boEmitterView_c, mStatus, 0x254);

struct boShapeAttrView_c {
    cXyz mMin, mMax;
    be<u32> mAabVtable, mShapeVtable;
};
WWHD_SIZE(boShapeAttrView_c, 0x20);
WWHD_OFFSET(boShapeAttrView_c, mAabVtable, 0x18);
struct boParticleView_c {
    u8 _positionPrefix[0x28];
    cXyz mGlobalPosition;
};
WWHD_OFFSET(boParticleView_c, mGlobalPosition, 0x28);
struct boParticleListView_c {
    be<u32> mpFirst, mpLast, mNum;
};
WWHD_SIZE(boParticleListView_c, 12);

struct boYodareEcallBack_c {
    be<u32> __vtbl;
    dBgS_GndChk mGroundCheck;
};
WWHD_SIZE(boYodareEcallBack_c, 0x58);
WWHD_OFFSET(boYodareEcallBack_c, mGroundCheck, 4);

struct bo_class : fopEn_enemy_c {
    /* 0x3C8 */ u8 mPhase[8];
    /* 0x3D0 */ gptr<void> mpColorAnm;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpUpperMorph;
    /* 0x3D8 */ gptr<mDoExt_McaMorf> mpLowerMorph;
    /* 0x3DC */ be<u8> mType, mVariant;
    u8 _typePadding[2];
    /* 0x3E0 */ be<u8> mAction, mMode;
    /* 0x3E2 */ be<u8> mWaitParticleState, mHoldParticleState, mBendFlags;
    u8 _reactionPadding;
    /* 0x3E6 */ be<u8> mReaction, mCapturePending, mHoldingPlayer, mThrowReversed;
    /* 0x3EA */ be<u8> mSuppressDrop;
    u8 _damagePadding;
    /* 0x3EC */ be<u8> mDamageType;
    u8 _stickPadding[3];
    /* 0x3F0 */ be<s32> mStickXDirection, mStickYDirection, mAnimation;
    /* 0x3FC */ cXyz mHeadPos, mUnusedJointPos, mRootPos, mBodyPos, mLeafPos, mMouthPos, mTipPos;
    /* 0x450 */ csXyz mHeadAngle, mStemAngle;
    be<s16> mUnusedBendAngle, mStemTargetY, mStemTargetZ, mUnusedStemAngle;
    /* 0x464 */ be<s16> mFacingY;
    u8 _facingPadding[4];
    /* 0x46A */ be<s16> mTargetFacingY;
    u8 _targetPadding[2];
    /* 0x46E */ csXyz mHeadTargetAngle;
    /* 0x474 */ cXyz mHitPos;
    /* 0x480 */ be<s16> mDeathDelay;
    /* 0x482 */ be<s16> mTimers[5];
    /* 0x48C */ be<s16> mHitAngleY, mBendCountY, mBendCountZ;
    /* 0x492 */ union {
        be<s16> mEffectTimer;
        struct {
            be<u8> mEffectTimerHigh, mSmokeAlpha;
        };
    };
    u8 _recoilPadding[4];
    /* 0x498 */ be<s16> mAngleStep, mRecoilAngle, mRecoilVelocity, mRecoilFacingY;
    u8 _distancePadding[4];
    /* 0x4A4 */ be<f32> mRecoilDistance;
    u8 _groundPadding[4];
    /* 0x4AC */ be<f32> mGroundY, mBodyHeight, mTargetScaleY;
    /* 0x4B8 */ cXyz mSmokePos;
    u8 _callbackPadding[8];
    /* 0x4CC */ dPa_smokeEcallBack mSmokeCallback;
    /* 0x4EC */ gptr<void> mpEmergingEmitter, mpWaitEmitter, mpHoldEmitter;
    /* 0x4F8 */ boYodareEcallBack_c mSalivaCallback;
    /* 0x550 */ dBgS_AcchCir mAcchCir;
    /* 0x590 */ dBgS_ObjAcch mAcch;
    /* 0x754 */ dCcD_Stts mStts;
    /* 0x790 */ dCcD_Sph mHeadSph, mFootSph;
    /* 0x9E8 */ dCcD_Cyl mBodyCyl;
    /* 0xB18 */ enemyice mEnemyIce;
    /* 0xED0 */ enemyfire mEnemyFire;
    /* 0x10FC */ u8 mInvisibleModel[8];
};
WWHD_SIZE(bo_class, 0x1104);
WWHD_OFFSET(bo_class, mPhase, 0x3C8);
WWHD_OFFSET(bo_class, mpColorAnm, 0x3D0);
WWHD_OFFSET(bo_class, mpUpperMorph, 0x3D4);
WWHD_OFFSET(bo_class, mpLowerMorph, 0x3D8);
WWHD_OFFSET(bo_class, mType, 0x3DC);
WWHD_OFFSET(bo_class, mAction, 0x3E0);
WWHD_OFFSET(bo_class, mMode, 0x3E1);
WWHD_OFFSET(bo_class, mHeadPos, 0x3FC);
WWHD_OFFSET(bo_class, mHeadAngle, 0x450);
WWHD_OFFSET(bo_class, mStemAngle, 0x456);
WWHD_OFFSET(bo_class, mFacingY, 0x464);
WWHD_OFFSET(bo_class, mHeadTargetAngle, 0x46E);
WWHD_OFFSET(bo_class, mHitPos, 0x474);
WWHD_OFFSET(bo_class, mDeathDelay, 0x480);
WWHD_OFFSET(bo_class, mTimers, 0x482);
WWHD_OFFSET(bo_class, mEffectTimer, 0x492);
WWHD_OFFSET(bo_class, mAngleStep, 0x498);
WWHD_OFFSET(bo_class, mRecoilDistance, 0x4A4);
WWHD_OFFSET(bo_class, mGroundY, 0x4AC);
WWHD_OFFSET(bo_class, mSmokePos, 0x4B8);
WWHD_OFFSET(bo_class, mSmokeCallback, 0x4CC);
WWHD_OFFSET(bo_class, mpEmergingEmitter, 0x4EC);
WWHD_OFFSET(bo_class, mSalivaCallback, 0x4F8);
WWHD_OFFSET(bo_class, mAcchCir, 0x550);
WWHD_OFFSET(bo_class, mAcch, 0x590);
WWHD_OFFSET(bo_class, mStts, 0x754);
WWHD_OFFSET(bo_class, mHeadSph, 0x790);
WWHD_OFFSET(bo_class, mFootSph, 0x8BC);
WWHD_OFFSET(bo_class, mBodyCyl, 0x9E8);
WWHD_OFFSET(bo_class, mEnemyIce, 0xB18);
WWHD_OFFSET(bo_class, mEnemyFire, 0xED0);
WWHD_OFFSET(bo_class, mInvisibleModel, 0x10FC);

// HD: individual constructor calls retain their original outgoing linkage.
void boConstructBaseCollisionStatus(dCcD_Stts *status);
void boConstructGlobalCollisionStatus(void *status);
void boConstructCollisionObject(dCcD_GObjInf *collider);
void boConstructSphere(dCcD_Sph *sphere);
void boConstructCylinderGeometry(cM3dGCyl *cylinder);
void boConstructAcch(dBgS_Acch *acch);
void boDestroySphere(dCcD_Sph *sphere, s32 flags);
void boDestroyAcch(dBgS_Acch *acch, s32 flags);
void boDestroyCircle(cM3dGCir_l *circle, s32 flags);

void *boAllocate(s32 size);
void boDeallocate(void *object);
s32 boEntrySolidHeap(bo_class *actor, u32 callback, s32 size);

// HD: out-of-line joint accessors preserve call and subsequent load order.
J3DJoint *boGetJointData(J3DNode *node);
void *boGetJointTree(J3DModelData *modelData);

BOOL boEnemyIce(enemyice *ice);
void boEnemyFire(enemyfire *fire);
void boRemoveEnemyFire(enemyfire *fire);
f32 boStickX(s32 port);
f32 boStickY(s32 port);
BOOL boTriggerA(s32 port);

s8 boReverb(s32 room);
void boMonsterSound(u32 sound, void *position, u32 actorId, s32 flags, s32 reverb);
void boEffectSound(u32 sound, void *position, u32 flags, s32 reverb);
void boStopBattleSound();
void boDeleteParticles(void *emitter, s32 flags);
void boConstructGroundCheck(dBgS_GndChk *check);
void boDestroyGroundCheck(dBgS_GndChk *check, s32 flags);

void boSetCollision(cCcS *collision, dCcD_GObjInf *object);

// HD: player fields used by Boko Baba; byte members remain guest loads.
struct boPlayerView_c : fopAc_ac_c {
    /* 0x3AC */ be<u8> mCutType;
    u8 _cutPadding[0x3BC - 0x3AD];
    /* 0x3BC */ be<u32> mNoResetFlg1, mResetFlg0;
    u8 _swordPadding[0x3E4 - 0x3C4];
    /* 0x3E4 */ cXyz mSwordTopPos;
    u8 _demoPadding[0x420 - 0x3F0];
    struct {
        be<s16> mDemoType, mDemoMoveAngle, mTimer, mParam2;
        be<u32> mParam0, mParam1, mDemoMode;
        be<f32> mStick;
    } mDemo;
};
WWHD_OFFSET(boPlayerView_c, mCutType, 0x3AC);
WWHD_OFFSET(boPlayerView_c, mNoResetFlg1, 0x3BC);
WWHD_OFFSET(boPlayerView_c, mResetFlg0, 0x3C0);
WWHD_OFFSET(boPlayerView_c, mSwordTopPos, 0x3E4);
WWHD_OFFSET(boPlayerView_c, mDemo, 0x420);
WWHD_SIZE(boPlayerView_c, 0x438);
struct boJointDataView_c {
    be<u32> mpData;
    be<u16> mJntNo;
};
WWHD_OFFSET(boJointDataView_c, mJntNo, 4);
struct boJointTreeView_c {
    u8 _jointCountPrefix[8];
    be<u16> mJointNum;
};
WWHD_OFFSET(boJointTreeView_c, mJointNum, 8);

void *boSetParticle(dPa_control_c *control, s32 group, u32 effect, const cXyz *position,
                    const csXyz *angle, const cXyz *scale, s32 alpha, void *callback, s32 room,
                    const void *primary, const void *environment, const cXyz *scale2D);

void boConstructSmoke(dPa_smokeEcallBack *callback, s32 flags);
void boConstructInvisibleModel(void *model);
void boDestroyInvisibleModel(void *model, s32 flags);
void boDestroyActor(bo_class *actor, s32 flags);
void boPlayColorAnimation(void *animation);
void boEntryColorAnimation(void *animation, void *modelData, f32 frame);
void boSetCullBox(bo_class *actor, f32 minX, f32 minY, f32 minZ, f32 maxX, f32 maxY, f32 maxZ);

void boCreateActor(s32 process, u32 parameters, cXyz *position, s32 room, csXyz *angle, cXyz *scale,
                   s32 parent, s32 subtype);
void boCreateDisappear(bo_class *actor, cXyz *position, s32 size, s32 type, u32 itemBit);
void boSaveActor(void *save, u32 actorId, s32 room);
void boCheckAttack(bo_class *actor, void *hitInfo);
s32 boLoadResource(void *phase, char *archive);
void boDeleteResource(void *phase, char *archive);
void boMoveActor(bo_class *actor, dCcD_Stts *status);
void boEndEventCamera(void *camera, s32 actorId);
void boOrderEvent(bo_class *actor, s32 eventId, s32 priority, s32 flags);

void boRegisterSnap(s32 type, bo_class *actor, cXyz *position, s32 yaw, f32 scaleX, f32 scaleY,
                    f32 scaleZ);
void boDrawIce(mDoExt_McaMorf *morph, s32 index, void *invisibleModel);
mDoExt_McaMorf *boConstructMorph(s32 object, void *model, s32 callbackA, s32 callbackB,
                                 void *animation, s32 mode, s32 start, s32 end, f32 rate,
                                 s32 argument9, s32 argument10, u32 flags, u32 sound);
void *boConstructColorAnimation(void *animation);
s32 boInitColorAnimation(void *animation, void *modelData, void *resource, s32 mode, s32 argument4,
                         s32 start, s32 end, s32 argument7, f32 speed, s32 argument9);
s32 boCreateInvisibleModel(void *invisibleModel, void *model);
s32 boFigureIndex(void *table, char *name, s32 flag);
s16 boDistanceAngle(s32 first, s32 second);

void boRegisterGlobal(void *record);
void boConstructHeaderByte(void *byte);
void boConstructHeaderFlag(void *flag);
void boSetEmitterTransform(void *source, void *rotation, void *scale, void *translation);
void boSetEmitterRotation(void *source, void *rotation, void *translation);
void boPlayerThrowAlternate(void *player, f32 speed);
void boPlayerThrow(void *player, f32 speed);
void boStartEventCamera(void *camera, s32 type, s32 actorId, void *mode, void *subtype,
                        void *argument5, void *argument6, void *argument7, void *argument8,
                        void *argument9, void *center, void *argument11, void *eye,
                        void *argument13, void *argument14, s32 end);

struct boPlayerThrowView_c : boPlayerView_c {
    u8 _throwPadding[0x699E - 0x438];
    be<s16> mThrowMode;
};
WWHD_OFFSET(boPlayerThrowView_c, mThrowMode, 0x699E);
WWHD_SIZE(boPlayerThrowView_c, 0x69A0);
// HD: the smoke color is written as one word, preserving the original store.
struct boSmokeColorView_c {
    u8 _colorPrefix[0x16];
    be<u32> mColorRGBA;
};
WWHD_OFFSET(boSmokeColorView_c, mColorRGBA, 0x16);

// Resource and joint names follow the GameCube Bo archive.
enum dRes_INDEX_BO {
    /* BCK  */
    dRes_INDEX_BO_BCK_ATTACK1_e = 0x5,
    dRes_INDEX_BO_BCK_BOU1_e = 0x6,
    dRes_INDEX_BO_BCK_DAMAGE1_e = 0x7,
    dRes_INDEX_BO_BCK_DERU_S1_e = 0x8,
    dRes_INDEX_BO_BCK_DERU_U1_e = 0x9,
    dRes_INDEX_BO_BCK_FUKKATU1_e = 0xA,
    dRes_INDEX_BO_BCK_HAKIDASU1_e = 0xB,
    dRes_INDEX_BO_BCK_HANEKAERI1_e = 0xC,
    dRes_INDEX_BO_BCK_KAMU1_e = 0xD,
    dRes_INDEX_BO_BCK_KARERU1_e = 0xE,
    dRes_INDEX_BO_BCK_KUSIZASI1_e = 0xF,
    dRes_INDEX_BO_BCK_MODORU_S1_e = 0x10,
    dRes_INDEX_BO_BCK_MODORU_U1_e = 0x11,
    dRes_INDEX_BO_BCK_PUNCH1_e = 0x12,
    dRes_INDEX_BO_BCK_SIORERU1_e = 0x13,
    dRes_INDEX_BO_BCK_TABERU1_e = 0x14,
    dRes_INDEX_BO_BCK_WAIT_L1_e = 0x15,
    dRes_INDEX_BO_BCK_WAIT_R1_e = 0x16,
    /* BDLM */
    dRes_INDEX_BO_BDL_BO_SITA1_e = 0x19,
    dRes_INDEX_BO_BDL_BO_UE1_e = 0x1A,
    /* BRK  */
    dRes_INDEX_BO_BRK_TO_BOKO1_e = 0x1D,
};
enum BO_SITA1_JNT {
    BO_SITA1_JNT_JH_ALLROOT_e = 0x0,
    BO_SITA1_JNT_JH_SKLROOT_e = 0x1,
    BO_SITA1_JNT_J_JH_HA1_e = 0x2,
    BO_SITA1_JNT_J_JH_HA2_e = 0x3,
    BO_SITA1_JNT_J_JH_HA3_e = 0x4,
    BO_SITA1_JNT_J_JH_HA4_e = 0x5,
    BO_SITA1_JNT_J_JH_MAIN1_e = 0x6,
    BO_SITA1_JNT_J_JH_MAIN2_e = 0x7,
    BO_SITA1_JNT_J_JH_ME1_e = 0x8,
    BO_SITA1_JNT_J_JH_ME_E_e = 0x9,
    BO_SITA1_JNT_J_JH_SAKI1_e = 0xA,
    BO_SITA1_JNT_J_JH_SAKI2_e = 0xB,
    BO_SITA1_JNT_J_JH_SAKI3_e = 0xC,
    BO_SITA1_JNT_J_JH_SAKI4_e = 0xD,
};
enum BO_UE1_JNT {
    BO_UE1_JNT_LOCATOR1_e = 0x0,
    BO_UE1_JNT_BOKOBABA_SKLROOT_e = 0x1,
    BO_UE1_JNT_J_BOKOBABA_NECK1_e = 0x2,
    BO_UE1_JNT_J_BOKOBABA_NECK2_e = 0x3,
    BO_UE1_JNT_J_BOKOBABA_NECK3_e = 0x4,
    BO_UE1_JNT_J_BOKOBABA_NECK4_e = 0x5,
    BO_UE1_JNT_J_BOKOBABA_HA_e = 0x6,
    BO_UE1_JNT_J_BOKOBABA_NECK5_e = 0x7,
    BO_UE1_JNT_J_BOKOBABA_ATAMA_e = 0x8,
    BO_UE1_JNT_J_BOKOBABA_AGO_e = 0x9,
    BO_UE1_JNT_J_BOKOBABA_SITA1_e = 0xA,
    BO_UE1_JNT_J_BOKOBABA_SITA2_e = 0xB,
    BO_UE1_JNT_J_BOKOBABA_SITA3_e = 0xC,
    BO_UE1_JNT_J_BOKOBABA_TOGE2_e = 0xD,
    BO_UE1_JNT_J_BOKOBABA_TYONMAGE_e = 0xE,
    BO_UE1_JNT_J_BOKOBABA_TYONMAGE2_e = 0xF,
};

// HD: material animation attachment is cleared after the upper model is drawn.
struct boModelDataView_c {
    u8 _animationPrefix[0x48];
    be<u32> mTevRegAnimation;
};
WWHD_OFFSET(boModelDataView_c, mTevRegAnimation, 0x48);
WWHD_SIZE(boModelDataView_c, 0x4C);
// HD: camera object prefix preceding its event-camera body.
struct boCameraView_c {
    u8 _cameraPrefix[0x248];
    u8 mCameraBody[1];
};
WWHD_OFFSET(boCameraView_c, mCameraBody, 0x248);
WWHD_SIZE(boCameraView_c, 0x249);

// HD: lists preceding the emitter's render state.
struct boEmitterListsView_c {
    u8 _listPrefix[0x1AC];
    boParticleListView_c mAlivePtcl, mAliveChildPtcl;
};
WWHD_OFFSET(boEmitterListsView_c, mAlivePtcl, 0x1AC);
WWHD_OFFSET(boEmitterListsView_c, mAliveChildPtcl, 0x1B8);
WWHD_SIZE(boEmitterListsView_c, 0x1C4);
struct boSaveView_c {
    u8 _savePrefix[0x20];
    dSv_info_c mSaveInfo;
};
WWHD_OFFSET(boSaveView_c, mSaveInfo, 0x20);
WWHD_SIZE(boSaveView_c, 0x12C0);
struct boNameTableView_c {
    u8 _tablePrefix[0x50AC];
    u8 mNameTable[1];
};
WWHD_OFFSET(boNameTableView_c, mNameTable, 0x50AC);
WWHD_SIZE(boNameTableView_c, 0x50AD);

WWHD_SIZE(boParticleView_c, 0x34);
WWHD_SIZE(boJointDataView_c, 6);
WWHD_SIZE(boJointTreeView_c, 10);
WWHD_SIZE(boSmokeColorView_c, 0x1A);

namespace bo {
// HD: process IDs differ from the GameCube profile table.
enum ProcessName {
    fpcNm_JBO_e = 0xD5,
    fpcNm_BO_e = 0xD6,
    fpcNm_BOKO_e = 0x1CF,
};
// GameCube collision names for the attack types this translation unit handles.
enum dCcD_ObjAtType : u32 {
    AT_TYPE_SWORD = 0x2,
    AT_TYPE_BOOMERANG = 0x40,
    AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_FIRE = 0x200,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    AT_TYPE_FIRE_ARROW = 0x40000,
    AT_TYPE_ICE_ARROW = 0x80000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_GRAPPLING_HOOK = 0x8000000,
};
enum AnimationLoop { LOOP_ONCE = 0, LOOP_REPEAT = 2 };
} // namespace bo
