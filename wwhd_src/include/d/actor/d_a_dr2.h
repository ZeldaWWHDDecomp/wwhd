// Valoo, ceiling rock and lava-pit layouts.
// HD: actor fields retain the GameCube offsets +0x11C; explicit vtables retain measured placement.
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
enum {
    dRes_INDEX_DR2_BCK_DR_BOSS_DEMO1_e = 9,
    dRes_INDEX_DR2_BDL_IWA00_e = 0xE,
    dRes_INDEX_DR2_BMD_DR_e = 0x17,
    dRes_INDEX_DR2_BMD_DR_SIPPO_e = 0x18,
    dRes_INDEX_DR2_BMD_GAN_MAGMA_e = 0x1B,
    dRes_INDEX_DR2_BMD_MBYO1_e = 0x1C,
    dRes_INDEX_DR2_BMD_MBYO2_e = 0x1D,
    dRes_INDEX_DR2_BRK_MBYO2_e = 0x20,
    dRes_INDEX_DR2_BTK_GAN_MAGMA_e = 0x23,
    dRes_INDEX_DR2_BTK_MBYO1_e = 0x24,
    dRes_INDEX_DR2_BTK_MBYO2_e = 0x25,
    dRes_INDEX_DR2_DZB_MBYO1_e = 0x28,
    dRes_INDEX_DR2_DZB_MBYO2_e = 0x29,
};
enum { DR_JNT_J_DR_HANE_L2_e = 11 };
enum {
    JA_SE_CM_BTD_STN_FALL = 0x5828,
    JA_SE_CM_BTD_ROCK_FALL = 0x582A,
    JA_SE_CM_BTD_ROCK_HIT = 0x582B,
    JA_SE_CM_BTD_ROCK_ATTACH = 0x582F,
};
enum {
    dPa_name_ID_AK_SN_BTDDRIPMAGMA00 = 0x80B7,
    dPa_name_ID_AK_SN_BTDBROKENROCK00 = 0x80B9,
    dPa_name_ID_AK_SN_O_BTDBROKENROCKTAIL00 = 0x8062,
    dPa_name_ID_AK_ST_BTDSMOKE02 = 0xA0B8,
};
enum { fpcNm_BTD_e = 0xEA, fpcNm_KUI_e = 0xFA };


// HD: private measured views for material projection, animation functors and particles.
struct Dr2FrameFunctor {
    be<f32> mResult, mA, mB; u8 mPadding[4]; be<u32> mFunction, mArgument;
};
struct Dr2BtkAnimation {
    J3DFrameCtrl mFrameCtrl; gptr<Dr2FrameFunctor> mpFunctor;
    u8 mPadding[0x68-0x14]; gptr<be<f32>> mpAnimationFrame;
};
struct Dr2BrkAnimation {
    J3DFrameCtrl mFrameCtrl; gptr<be<f32>> mpAnimationFrame;
    u8 mPadding[0x20-0x14]; gptr<Dr2FrameFunctor> mpFunctor;
};
struct Dr2MaterialNameBlock {
    u8 mPadding[0x18]; be<s32> mNameOffset;
    JUTNameTab* getNames() {
        s32 offset = mNameOffset;
        return gabi::at<JUTNameTab>(offset != 0 ? gabi::ea(&mNameOffset) + offset : 0);
    }
};
struct Dr2ShapePacket { u8 mPadding[0x3C]; };
struct Dr2Model {
    u8 mPadding[0x14]; gptr<Dr2MaterialNameBlock> mpMaterialNames;
    u8 mPad18[0x34-0x18]; gptr<Dr2ShapePacket> mpShapePackets;
};
struct Dr2TextureMatrix {
    u8 mPadding[0x24]; be<u32> mProjection[11];
    be<f32> mTranslation, mRow0, mRow1, mRow3, mRow2;
    Mtx34 mEffectMatrix;
};
struct Dr2TexGenVtable { u8 mPadding[0x14]; be<u32> mGetTextureMatrix; };
struct Dr2TexGenBlock {
    gptr<Dr2TexGenVtable> mpVtable;
    Dr2TextureMatrix* getTextureMatrix(s32 index) {
        return gabi::call_ptr<Dr2TextureMatrix*>(mpVtable->mGetTextureMatrix, this, index);
    }
};
struct Dr2Material {
    u8 mPadding[0x14]; gptr<Dr2TexGenBlock> mpTexGen;
    u8 mRest[0x39C-0x18];
};
struct Dr2JointData { u8 mPadding[4]; be<u16> mJointNo; };
struct Dr2Particle { u8 mPadding[0x28]; cXyz mGlobalPosition; };
struct Dr2ParticleLink {
    gptr<Dr2Particle> mpObject; u8 mPadding[8]; gptr<Dr2ParticleLink> mpNext;
};
struct Dr2Emitter {
    u8 mPadding[0x34]; be<f32> mRate; u8 mPad38[0x24]; be<s32> mMaxFrame;
    u8 mPad60[0x1AC-0x60]; gptr<Dr2ParticleLink> mpFirst;
    u8 mPad1B0[4]; be<u32> mParticleCount; u8 mPad1B8[8]; be<u32> mChildCount;
    u8 mPad1C4[0x254-0x1C4]; be<u32> mFlags;
};
struct Dr2Btd {
    u8 mPadding[0x41E]; be<u8> mRockHitState; u8 mPad41F[5]; cXyz mJointPos[9];
};
struct Dr2Player { u8 mPadding[0x3F0]; cXyz mLeftHandPosition; };
struct Dr2SmokeCallback {
    be<u32> mVtable; u8 mPadding[0xE]; be<u8> mFollowOff; u8 mRest[0xD];
};
struct Dr2SmokeVtable { u8 mPadding[0x44]; be<u32> mRemove; };
struct Dr2CreateAppend { be<u32> mParameter; u8 mPadding[0x1D]; be<s8> mRoomNo; };
WWHD_SIZE(Dr2FrameFunctor, 0x18);
WWHD_SIZE(Dr2BtkAnimation, 0x6C);
WWHD_SIZE(Dr2BrkAnimation, 0x24);
WWHD_SIZE(Dr2MaterialNameBlock, 0x1C);
WWHD_SIZE(Dr2ShapePacket, 0x3C);
WWHD_SIZE(Dr2Model, 0x38);
WWHD_SIZE(Dr2TextureMatrix, 0x94);
WWHD_SIZE(Dr2TexGenVtable, 0x18);
WWHD_SIZE(Dr2TexGenBlock, 4);
WWHD_SIZE(Dr2Material, 0x39C);
WWHD_SIZE(Dr2JointData, 6);
WWHD_SIZE(Dr2Particle, 0x34);
WWHD_SIZE(Dr2ParticleLink, 0x10);
WWHD_SIZE(Dr2Emitter, 0x258);
WWHD_SIZE(Dr2Btd, 0x490);
WWHD_SIZE(Dr2Player, 0x3FC);
WWHD_SIZE(Dr2SmokeCallback, 0x20);
WWHD_SIZE(Dr2SmokeVtable, 0x48);
WWHD_SIZE(Dr2CreateAppend, 0x22);
WWHD_OFFSET(Dr2TextureMatrix, mTranslation, 0x50);
WWHD_OFFSET(Dr2TextureMatrix, mEffectMatrix, 0x64);
WWHD_OFFSET(Dr2Emitter, mpFirst, 0x1AC);
WWHD_OFFSET(Dr2Emitter, mParticleCount, 0x1B4);
WWHD_OFFSET(Dr2Emitter, mChildCount, 0x1C0);
WWHD_OFFSET(Dr2Emitter, mFlags, 0x254);
WWHD_OFFSET(Dr2SmokeCallback, mFollowOff, 0x12);
WWHD_OFFSET(Dr2Btd, mJointPos, 0x424);
/* daDr2_HIO_c (HD: vtable after the members) */
struct daDr2_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mScale;  /* was m08: daDr2_Execute 0212B3D0: scale.x/y/z are set to it every frame (tww the same) */
    /* 0x08 */ be<u32> __vtbl;
};
WWHD_SIZE(daDr2_HIO_c, 0xC);


struct iwa_hahen_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ be<u8> mActive;  /* was unk_04: iwa_move (in daDr2_Execute 0212B3D0) sets it for the 6 fragments when the rock breaks; hahen_move (same) clears it below y -100; hahen_draw (in daDr2_Draw 0212AEEC) draws only active fragments */
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ cXyz mPos;  /* was unk_08: iwa_move (in daDr2_Execute 0212B3D0): starts at mRockPos; hahen_move (same) adds mSpeed and places the fragment model there */
    /* 0x14 */ cXyz mSpeed;  /* was unk_14: iwa_move (in daDr2_Execute 0212B3D0): MtxPosition of (hsx[i], 0, hsz[i]) * 3 along mRockAngle; hahen_move (same) adds it to mPos and -0.5 to its y per frame */
    /* 0x20 */ csXyz mAngle;  /* was unk_20: iwa_move (in daDr2_Execute 0212B3D0): starts at mRockAngle; hahen_move (same) spins x / z each frame and rotates the fragment model by it */
    /* 0x26 */ u8 _26[2];
};
WWHD_SIZE(iwa_hahen_s, 0x28);

/* GameCube +0x11C throughout */
struct dr2_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf1;
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf2;
    /* 0x3D8 */ be<s16> mFrameCounter;  /* was unk_2BC: daDr2_Execute 0212B3D0 adds 1 per frame; move (in daDr2_Execute 0212B3D0) uses it as the phase of the rock-shake sines of the tail joints */
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ iwa_hahen_s mRockFragments[6];
    /* 0x4CC */ cXyz mHookTargetPos;  /* was unk_3B0: nodeCallBack 0212AACC: world position of joint HANE_L2 (+ 215, 0, -5); daKui_Execute 021ACB0C (d_a_kui, view KuiDragonTailView) puts the hookshot target there */
    /* 0x4D8 */ be<s16> mJointRotY[12];  /* was unk_3BC: move (in daDr2_Execute 0212B3D0): [2..11] sin(mFrameCounter) * mShakeAmplitude per joint while the rock shakes, [0] the grabbed tail's angle; nodeCallBack 0212AACC YrotM of the joint by it */
    /* 0x4F0 */ be<s16> mJointRotZ[12];  /* was unk_3D4: move (in daDr2_Execute 0212B3D0): [2..11] cos(mFrameCounter) * mShakeAmplitude per joint while the rock shakes, [0] eased to 0 while grabbed; nodeCallBack 0212AACC ZrotM of the joint by it */
    /* 0x508 */ be<f32> mSwayRotY;  /* was unk_3EC: move (in daDr2_Execute 0212B3D0): sin(mSwayPhaseY) * mSwayAmplitude, eased to -0.175 while pulled; nodeCallBack 0212AACC rotates the tail joints about Y by it (radians) */
    /* 0x50C */ be<f32> mSwayRotZ;  /* was unk_3F0: move (in daDr2_Execute 0212B3D0): sin(mSwayPhaseZ) * mSwayAmplitude, eased to 0 while pulled; nodeCallBack 0212AACC rotates the tail joints about Z by it (radians) */
    /* 0x510 */ be<f32> mSwayPhaseY;  /* was unk_3F4: move (in daDr2_Execute 0212B3D0): + 0.04 per frame; the phase of mSwayRotY */
    /* 0x514 */ be<f32> mSwayPhaseZ;  /* was unk_3F8: move (in daDr2_Execute 0212B3D0): + 0.05 per frame; the phase of mSwayRotZ */
    /* 0x518 */ be<u32> mKuiId;  /* was unk_3FC: daDr2_Create 0212D394: the process id of the KUI actor (hookshot target, parameter 0x511) it requests; move (in daDr2_Execute 0212B3D0) looks it up and reads its health (1..3) for the tail state */
    /* 0x51C */ be<s16> mTimers[5];  /* was unk_400: daDr2_Execute 0212B3D0 counts all 5 down each frame; iwa_move (in daDr2_Execute 0212B3D0) sets [0] = 0x32 when the rock lands on Gohma */
    /* 0x526 */ be<u8> mTailState;  /* was unk_40A: move (in daDr2_Execute 0212B3D0) / iwa_move set it (Dr2TailState: idle, pulled, grabbed, rock shake); nodeCallBack 0212AACC bends the tail joints by it; new_himo2_move 0216F7FC (d_a_himo2, view himo2_dr_c) sets 3 / 0 in the boss-fall demo */
    /* 0x527 */ u8 _527;
    /* 0x528 */ be<f32> mPullScaleX;  /* was unk_40C: move (in daDr2_Execute 0212B3D0): 0.98 at rest, eased to 1.04 while pulled; nodeCallBack 0212AACC scales the tail joints' x by it while pulled */
    /* 0x52C */ be<f32> mSwayAmplitude;  /* was unk_410: move (in daDr2_Execute 0212B3D0): eased to 0.048 (0 once the KUI's health is 1); scales mSwayRotY / mSwayRotZ; iwa_move clears it when the tail is far from home */
    /* 0x530 */ be<f32> mShakeAmplitude;  /* was unk_414: iwa_move (in daDr2_Execute 0212B3D0) eases it to 2000 while the rock rides Gohma; new_himo2_move 0216F7FC (view himo2_dr_c) eases it to 2000 / 100; move scales the joint shake mJointRotY / Z by it */
    /* 0x534 */ gptr<J3DModel> mpRockModel;  /* was unk_418: useHeapInit 0212CDAC: model IWA00; daDr2_Execute 0212B3D0 places it at mRockPos / mRockAngle; iwa_draw (in daDr2_Draw 0212AEEC) draws it while mRockState is below 10 */
    /* 0x538 */ gptr<J3DModel> mpMagmaModel;  /* was unk_41C: useHeapInit 0212CDAC: model GAN_MAGMA (lava on the rock); daDr2_Execute 0212B3D0 places it with the rock; iwa_draw (in daDr2_Draw 0212AEEC) draws it while mMagmaFrame runs */
    /* 0x53C */ gptr<void> mpMagmaBtk;  /* was unk_420: useHeapInit 0212CDAC: btk GAN_MAGMA of mpMagmaModel; iwa_move (in daDr2_Execute 0212B3D0) sets its frame from mMagmaFrame, iwa_draw (in daDr2_Draw 0212AEEC) enters it; mDoExt_btkAnm* */
    /* 0x540 */ be<s16> mMagmaFrame;  /* was unk_424: iwa_move (in daDr2_Execute 0212B3D0) sets 1 when the rock leaves Gohma dripping lava, + 1 per frame up to 180, then 0; the frame of mpMagmaBtk; iwa_draw (in daDr2_Draw 0212AEEC) draws the lava while it is not 0 */
    /* 0x542 */ u8 _542[2];
    /* 0x544 */ gptr<J3DModel> mpFloorModel;  /* was unk_428: useHeapInit 0212CDAC: model MBYO1 (the floor before the boss is beaten); yuka_move (in daDr2_Execute 0212B3D0) places it, yuka_draw (in daDr2_Draw 0212AEEC) draws it unless mFloorBroken */
    /* 0x548 */ gptr<J3DModel> mpBrokenFloorModel;  /* was unk_42C: useHeapInit 0212CDAC: model MBYO2; yuka_move (in daDr2_Execute 0212B3D0) places it, yuka_draw (in daDr2_Draw 0212AEEC) draws it (with the view-projected lava texture) when mFloorBroken */
    /* 0x54C */ gptr<void> mpFloorBtk;  /* was unk_430: useHeapInit 0212CDAC: btk MBYO1 (loop) of mpFloorModel; yuka_move (in daDr2_Execute 0212B3D0) plays it, yuka_draw (in daDr2_Draw 0212AEEC) enters it; mDoExt_btkAnm* */
    /* 0x550 */ gptr<void> mpBrokenFloorBtk;  /* was unk_434: useHeapInit 0212CDAC: btk MBYO2 of mpBrokenFloorModel; yuka_move (in daDr2_Execute 0212B3D0) plays it or sets frame 299, yuka_draw (in daDr2_Draw 0212AEEC) enters it; mDoExt_btkAnm* */
    /* 0x554 */ gptr<void> mpBrokenFloorBrk;  /* was unk_438: useHeapInit 0212CDAC: brk MBYO2 of mpBrokenFloorModel; yuka_move (in daDr2_Execute 0212B3D0) plays it or sets frame 299, yuka_draw (in daDr2_Draw 0212AEEC) enters it; mDoExt_brkAnm* */
    /* 0x558 */ be<u8> mFloorBroken;  /* was unk_43C: yuka_move (in daDr2_Execute 0212B3D0) sets 1 when the stage boss is beaten (and the start stage is not an X stage); then mpBrokenFloorModel / mpBgW2 are used and the other floor is moved to y -10000 */
    /* 0x559 */ u8 _559[3];
    /* 0x55C */ Mtx34 mFloorMtx;  /* was unk_440: yuka_move (in daDr2_Execute 0212B3D0): the matrix of mpFloorModel (scaled), given to mpBgW1 by cBgW_Set in useHeapInit 0212CDAC */
    /* 0x58C */ Mtx34 mBrokenFloorMtx;  /* was unk_470: yuka_move (in daDr2_Execute 0212B3D0): the matrix of mpBrokenFloorModel (scaled), given to mpBgW2 by cBgW_Set in useHeapInit 0212CDAC */
    /* 0x5BC */ gptr<dBgW> mpBgW1;
    /* 0x5C0 */ gptr<dBgW> mpBgW2;
    /* 0x5C4 */ cXyz mRockPos;  /* was unk_4A8: daDr2_Create 0212D394: home + (0, -50, 0); iwa_move (in daDr2_Execute 0212B3D0) moves the rock (shake, fall onto Gohma, rise back); daDr2_Execute 0212B3D0 places mpRockModel there; the rock particles and sounds start there */
    /* 0x5D0 */ csXyz mRockAngle;  /* was unk_4B4: iwa_move (in daDr2_Execute 0212B3D0): x / y follow Gohma while riding, z the shake; daDr2_Execute 0212B3D0 rotates mpRockModel by it; hahen fragments start with it */
    /* 0x5D6 */ be<s16> mRockState;  /* was unk_4BA: iwa_move (in daDr2_Execute 0212B3D0) steps through it (Dr2RockState: return, attached, fall, ride Gohma, rise, break, gone); iwa_draw (in daDr2_Draw 0212AEEC) draws the rock below 10; new_himo2_move 0216F7FC (view himo2_dr_c) and daKui_Execute 021ACB0C (view KuiDragonTailView) read it */
    /* 0x5D8 */ u8 _5D8[4];
    /* 0x5DC */ be<f32> mRockRideOffsetY;  /* was unk_4C0: iwa_move (in daDr2_Execute 0212B3D0): while riding Gohma, (100 - Gohma's joint 3 height) * 0.3 when that joint is low; raises the rock's tilt target; cleared on return */
    /* 0x5E0 */ be<u8> mDragonAnmState;  /* was unk_4C4: dr_move (in daDr2_Execute 0212B3D0): 0, then 1 after starting DR_BOSS_DEMO1 on mpMorf2 once mShowDragon is set; back to 0 when it is cleared */
    /* 0x5E1 */ u8 _5E1;
    /* 0x5E2 */ be<s16> unk_4C6;
    /* 0x5E4 */ be<s16> mRockReturnWait;  /* was unk_4C8: iwa_move (in daDr2_Execute 0212B3D0) sets 0x82 when the rock is back at the top and counts it down before easing the actor back to home */
    /* 0x5E6 */ be<s16> mRockFallTimer;  /* was unk_4CA: new_himo2_move 0216F7FC (d_a_himo2, view himo2_dr_c) sets 0x5A in the rope jump demo; iwa_move (in daDr2_Execute 0212B3D0) shakes the rock while it runs, counts it down and drops the rock at 1 */
    /* 0x5E8 */ be<s16> mShakeStepTimer;  /* was unk_4CC: iwa_move (in daDr2_Execute 0212B3D0): every 3 (+ reg) frames a new shake target (mRockShakeAngleZ / mRockShakeOffsetY); move sets 20 once the KUI's health is 1 */
    /* 0x5EA */ be<s16> mRockShakeAngleZ;  /* was unk_4CE: iwa_move (in daDr2_Execute 0212B3D0): random +-300 per shake step; mRockAngle.z is eased to it; 0 when not shaking */
    /* 0x5EC */ be<f32> mRockShakeOffsetY;  /* was unk_4D0: iwa_move (in daDr2_Execute 0212B3D0): random +-20 per shake step; mRockPos.y is eased to home - 50 + it; 0 when not shaking */
    /* 0x5F0 */ gptr<JPABaseEmitter> mpBrokenRockEmitter;  /* was unk_4D4: iwa_move (in daDr2_Execute 0212B3D0): the BTDBROKENROCK00 emitter (rate mBrokenRockRate); its particles spawn BTDBROKENROCKTAIL00; invalidated when mBrokenRockTimer runs out */
    /* 0x5F4 */ dPa_followEcallBack mDripCb;  /* was unk_4D8: iwa_move (in daDr2_Execute 0212B3D0) passes it to the BTDDRIPMAGMA00 (dripping lava) particle and ends it when the rock is back; daDr2_Delete 0212CD10 removes it */
    /* 0x608 */ Dr2SmokeCallback mSmokeCb;  /* was unk_4EC: iwa_move (in daDr2_Execute 0212B3D0) passes it to the BTDSMOKE02 particle (toon); daDr2_Create 0212D394 sets follow off; daDr2_Delete 0212CD10 removes it; dPa_smokeEcallBack (HD 0x20, vtable at +0) */
    /* 0x628 */ be<u8> mShowDragon;  /* was unk_50C: new_himo2_move 0216F7FC (d_a_himo2, view himo2_dr_c) sets 1 / 0 in the rope demo; dr_draw (in daDr2_Draw 0212AEEC) draws and dr_move (in daDr2_Execute 0212B3D0) animates the dragon (mpMorf2) while it is set */
    /* 0x629 */ be<u8> mBrokenRockRate;  /* was unk_50D: iwa_move (in daDr2_Execute 0212B3D0): 1 / 30 / 10 (+ reg); the rate given to mpBrokenRockEmitter */
    /* 0x62A */ be<u8> mBrokenRockState;  /* was unk_50E: iwa_move (in daDr2_Execute 0212B3D0): 1 = create mpBrokenRockEmitter, 2 = running (spawning tail particles), 0 = none */
    /* 0x62B */ u8 _62B;
    /* 0x62C */ be<s16> mBrokenRockTimer;  /* was unk_510: iwa_move (in daDr2_Execute 0212B3D0): 20 / 1 / 2 with a new mpBrokenRockEmitter, counted down while it runs; at 0 the emitter is invalidated */
    /* 0x62E */ u8 _62E[2];
    /* 0x630 */ gptr<fopAc_ac_c> mpBtd;  /* was unk_514: iwa_move (in daDr2_Execute 0212B3D0): Gohma (btd_class), found by s_a_d_sub 0212B380 and kept; the rock rides its joints; btd_class* */
};
WWHD_OFFSET(dr2_class, mRockFragments, 0x3DC);
WWHD_OFFSET(dr2_class, mTailState, 0x526);
WWHD_OFFSET(dr2_class, mpBgW1, 0x5BC);
WWHD_OFFSET(dr2_class, mDripCb, 0x5F4);
WWHD_OFFSET(dr2_class, mShowDragon, 0x628);
WWHD_SIZE(dr2_class, 0x634);


void dr2_rotateY(f32 radians, u8 concatenate);
void dr2_updateModel(J3DModel* model);
s16 dr2_radiansToAngle(f32 radians);
void dr2_inverse(const Mtx34* source, Mtx34* result);
void dr2_scale(Mtx34* matrix, f32 x, f32 y, f32 z);
void dr2_concat(const Mtx34* left, const Mtx34* right, Mtx34* result);
void dr2_brkEntry(void* animation, J3DModelData* data, f32 frame);
void dr2_btkEntry(void* animation, J3DModelData* data, f32 frame);
void dr2_shock(dVibration_c* vibration, s32 strength, s32 flags, cXyz* position);
void dr2_playAnimation(void* animation);
void dr2_finishFrame(gptr<Dr2FrameFunctor>* functor);
gptr<Mtx34>* dr2_shapeMatrixSlot(Dr2ShapePacket* packet, s32 index);
void* dr2_btkCtor(void* animation);
BOOL dr2_btkInit(void* animation, J3DModelData* data, void* key, u32 play, s32 mode, f32 rate, s16 start, s16 end, u32 modify, s32 entry);
void* dr2_brkCtor(void* animation);
BOOL dr2_brkInit(void* animation, J3DModelData* data, void* key, u32 play, s32 mode, f32 rate, s16 start, s16 end, u32 modify, s32 entry);
void dr2_smokeCtor(Dr2SmokeCallback* callback, u8 mode);
Dr2CreateAppend* dr2_createAppend();
u32 dr2_currentLayer();
u32 dr2_createRequest(u32 layer, s16 process, u32 callback, u32 data, Dr2CreateAppend* append);
void dr2_actorDtor(fopAc_ac_c* actor, s32 flags);

inline Dr2Material* dr2_materialBase(J3DModelData* data) {
    return gabi::at<Dr2Material>(data->mMaterials);
}
inline Mtx34* dr2_viewMatrix() { return gabi::at<Mtx34>(0x104B45F8); }

struct Dr2MoveBg { u8 mPadding[0xA8]; be<u32> mCorrectionFunction; };
WWHD_SIZE(Dr2MoveBg, 0xAC);
constexpr u32 DR2_MOVE_BG_TYPICAL = 0x024EE658;
inline void dr2_setMoveBgCorrection(dBgW* background) {
    reinterpret_cast<Dr2MoveBg*>(background)->mCorrectionFunction = DR2_MOVE_BG_TYPICAL;
}
inline void dr2_removeSmoke(Dr2SmokeCallback* callback) {
    gabi::call_ptr(gabi::at<Dr2SmokeVtable>(callback->mVtable)->mRemove, callback);
}

enum Dr2TailState { DR2_TAIL_IDLE, DR2_TAIL_PULLED, DR2_TAIL_GRABBED, DR2_TAIL_ROCK_SHAKE };
enum Dr2RockState {
    DR2_ROCK_RETURN = -1, DR2_ROCK_ATTACHED, DR2_ROCK_FALL, DR2_ROCK_RIDE_GOHMA,
    DR2_ROCK_RISE, DR2_ROCK_BREAK = 10, DR2_ROCK_GONE
};
