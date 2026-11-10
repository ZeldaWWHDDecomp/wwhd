/* Wind Temple Iron Boots springboard. HD: MoveBg base is 0x3E0 and the
 * GameCube ground-check/shadow fields are absent; TWW spring state names
 * retained. */
#pragma once
#include "bindings.h"
namespace daObjJump {
struct Attr_c {
  be<u32> heapSize;
  be<s16> dzbResIndex, bdlResIndex;
  csXyz cullSizeBoxMin, cullSizeBoxMax;
  be<u8> hasShadow;
  u8 _15;
  be<u16> springJntNum;
  be<f32> field_0x018;
  be<f32> field_0x01C;
  be<f32> field_0x020;
  /* 0x24 */ be<f32> mScalePivotY;  /* was field_0x024: set_mtx 023660A4: the Y about which the body is scaled (tww transM(0, it, 0), scaleM, transM(0, -it, 0)) */
  /* 0x28 */ be<f32> mSpringDamping;  /* was field_0x028: calc_vib_pos 02366668: mScaleSpeed -= mScaleSpeed * it (tww) */
  /* 0x2C */ be<f32> mSpringStiffness;  /* was field_0x02C: calc_vib_pos 02366668: mScaleSpeed -= (mScale - mScaleTarget) * it (tww) */
  /* 0x30 */ be<f32> mPushImpulse;  /* was field_0x030: mode_wait 0236687C / mode_lower 023669E0: added to mScaleSpeed on a push */
  /* 0x34 */ be<f32> mFastRideImpulse;  /* was field_0x034: Execute 023666C8: added to mScaleSpeed while mbFastRide */
  /* 0x38 */ be<f32> mLowerScale;  /* was field_0x038: mode_w_l_init 0236685C: mScaleTarget when lowering */
  /* 0x3C */ be<f32> mUpperScale;  /* was field_0x03C: mode_l_u_init 023669C0: mScaleTarget when rising */
  /* 0x40 */ be<u8> mHeavyRideFrames;  /* was field_0x040: mode_wait 0236687C: lowers (JA_SE_OBJ_JUMP_SPR_CLOSE) once mHeavyRideCount exceeds it (tww) */
  /* 0x41 */ be<u8> mHeavyUnrideFrames;  /* was field_0x041: mode_lower 023669E0: rises (JA_SE_OBJ_JUMP_SPR_OPEN) once mHeavyUnrideCount exceeds it (tww) */
  /* 0x42 */ be<u8> mUpperTime;  /* was field_0x042: mode_upper_init 02366AD4: mTimer = it (tww mode_upper_init) */
  /* 0x43 */ be<u8> mRidePulseFrames;  /* was field_0x043: set_push_flag 0236642C: mbRidePulse when mRideCount reaches it */
  /* 0x44 */ be<u8> mUnridePulseFrames;  /* was field_0x044: set_push_flag 0236642C: mbUnridePulse when mUnrideCount reaches it */
  /* 0x45 */ be<u8> mHangPulseFrames;  /* was field_0x045: set_push_flag 0236642C: mbHangPulse when mHangCount reaches it */
  /* 0x46 */ be<u8> mHeavyRidePulseFrames;  /* was field_0x046: set_push_flag 0236642C: mbHeavyRidePulse when mHeavyRideCount reaches it */
  /* 0x47 */ be<u8> mHeavyUnridePulseFrames;  /* was field_0x047: set_push_flag 0236642C: mbHeavyUnridePulse when mHeavyUnrideCount reaches it */
  /* 0x48 */ be<u8> mPushInterval;  /* was field_0x048: mode_wait 0236687C / mode_lower 023669E0: mPushTimer after a push */
  u8 _49[3];
  /* 0x4C */ be<f32> mFastRideSpeed;  /* was field_0x04C: rideCB 02366268: mbFastRide when the riding player's speedF exceeds it */
};
WWHD_OFFSET(Attr_c, springJntNum, 0x16);
WWHD_OFFSET(Attr_c, field_0x018, 0x18);
WWHD_OFFSET(Attr_c, mHeavyRideFrames, 0x40);
WWHD_OFFSET(Attr_c, mFastRideSpeed, 0x4C);
WWHD_SIZE(Attr_c, 0x50);
struct Act_c : dBgS_MoveBgActor {
  enum Type_e { Type_SPRING_e, Type_SPRING_ON_BOX_e };
  enum Mode_e {
    Mode_WAIT_e,
    Mode_WAIT_LOWER_e,
    Mode_LOWER_e,
    Mode_LOWER_UPPER_e,
    Mode_UPPER_e,
    Mode_UPPER_WAIT_e
  };
  enum Prm_e { PRM_TYPE_W = 1, PRM_TYPE_S = 0 };
  const Attr_c &attr() const;
  /* 0x3E0 */ request_of_phase_process_class mPhase;
  /* 0x3E8 */ gptr<J3DModel> mModel;
  /* 0x3EC */ be<s32> mType;
  /* 0x3F0 */ be<f32> mGroundY;
  /* 0x3F4 */ be<s32> mMode;
  /* 0x3F8 */ be<s16> mTimer;
  /* 0x3FA */ be<u8> mIsSetup;
  /* 0x3FB */ u8 pad;
  /* 0x3FC */ be<f32> mScaleTarget;  /* was field_0x338: calc_vib_pos 02366668: the spring target of mScale (tww calc_vib_pos); Create 023662E4 / mode_wait_init 023662C8 1, mode_w_l_init 0236685C attr mLowerScale, mode_l_u_init 023669C0 attr mUpperScale; mode_w_l 023669AC / mode_l_u compare mScale with it */
  /* 0x400 */ be<f32> mScale;  /* was field_0x33C: calc_vib_pos 02366668 adds mScaleSpeed to it; jnodeCB_lower 02365DD8 scales the joint by (it, 1, 1) and back (tww scaleM(i_this->field_0x33C, 1, 1)); set_mtx 023660A4 offsets by it; Create 023662E4 1 */
  /* 0x404 */ be<f32> mScaleSpeed;  /* was field_0x340: calc_vib_pos 02366668: spring speed (pull attr mSpringStiffness toward mScaleTarget, damping attr mSpringDamping); mode_wait 0236687C / mode_lower 023669E0 add the push impulse attr mPushImpulse, Execute 023666C8 attr mFastRideImpulse; mode_l_u 02366AF4 0 */
  /* 0x408 */ be<u8> mIsRide;
  /* 0x409 */ be<u8> mbRidePulse;  /* was field_0x345: set_push_flag 0236642C: 1 for the frame mRideCount reaches attr mRidePulseFrames (tww set_push_flag); mode_wait 0236687C pushes on it */
  /* 0x40A */ be<u8> mbUnridePulse;  /* was field_0x346: set_push_flag 0236642C: 1 for the frame mUnrideCount reaches attr mUnridePulseFrames; mode_wait 0236687C pushes on it */
  /* 0x40B */ be<u8> mRideCount;  /* was field_0x347: set_push_flag 0236642C: frames with something riding (mIsRide), capped at 0xFF, 0 otherwise */
  /* 0x40C */ be<u8> mUnrideCount;  /* was field_0x348: set_push_flag 0236642C: frames with nothing riding, capped at 0xFF, 0 while ridden */
  /* 0x40D */ be<u8> mIsPlayerRide;
  /* 0x40E */ be<u8> mbHangPulse;  /* was field_0x34a: set_push_flag 0236642C: 1 for the frame mHangCount reaches attr mHangPulseFrames; mode_wait 0236687C pushes on it */
  /* 0x40F */ be<u8> mHangCount;  /* was field_0x34b: set_push_flag 0236642C: frames the player rides it while hanging (tww mIsPlayerRide && daPyStts0_HANG_e), capped at 0xFF */
  /* 0x410 */ be<u8> mIsHeavyRide;
  /* 0x411 */ be<u8> mbHeavyRidePulse;  /* was field_0x34d: set_push_flag 0236642C: 1 for the frame mHeavyRideCount reaches attr mHeavyRidePulseFrames; mode_wait 0236687C pushes on it */
  /* 0x412 */ be<u8> mbHeavyUnridePulse;  /* was field_0x34e: set_push_flag 0236642C: 1 for the frame mHeavyUnrideCount reaches attr mHeavyUnridePulseFrames; mode_lower 023669E0 pushes on it */
  /* 0x413 */ be<u8> mHeavyRideCount;  /* was field_0x34f: set_push_flag 0236642C: frames ridden with the Iron Boots (mIsHeavyRide), capped at 0xFF; mode_wait 0236687C lowers above attr mHeavyRideFrames */
  /* 0x414 */ be<u8> mHeavyUnrideCount;  /* was field_0x350: set_push_flag 0236642C: frames without the Iron Boots on it, capped at 0xFF; mode_lower 023669E0 rises above attr mHeavyUnrideFrames */
  /* 0x415 */ u8 pad2;
  /* 0x416 */ be<s16> mPushTimer;  /* was field_0x352: mode_wait 0236687C / mode_lower 023669E0: after a push it is set to attr mPushInterval and counts down, no new push before 0; mode_wait_init 023662C8 / mode_lower_init 02366998 0 */
  /* 0x418 */ be<u8> mbFastRide;  /* was field_0x354: rideCB 02366268 sets it when the riding player's speedF exceeds attr mFastRideSpeed (tww rideCB); clear_push_flag 023666B0 clears it; Execute 023666C8 adds attr mFastRideImpulse to mScaleSpeed in Mode_WAIT_e / Mode_LOWER_e while set */
  /* 0x419 */ u8 pad3[3];
  s32 Mthd_Create();
  s32 Mthd_Delete();
  s32 CreateHeap();
  s32 Create();
  s32 Delete();
  void set_mtx();
  void init_mtx();
  void set_push_flag();
  void clear_push_flag();
  void calc_vib_pos();
  void mode_wait_init();
  void mode_wait();
  void mode_w_l_init();
  void mode_w_l();
  void mode_lower_init();
  void mode_lower();
  void mode_l_u_init();
  void mode_l_u();
  void mode_upper_init();
  void mode_upper();
  void mode_u_w_init();
  void mode_u_w();
  s32 Execute(gptr<Mtx34> *);
  s32 Draw();
};
WWHD_OFFSET(Act_c, mPhase, 0x3E0);
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_OFFSET(Act_c, mType, 0x3EC);
WWHD_OFFSET(Act_c, mGroundY, 0x3F0);
WWHD_OFFSET(Act_c, mMode, 0x3F4);
WWHD_OFFSET(Act_c, mTimer, 0x3F8);
WWHD_OFFSET(Act_c, mIsSetup, 0x3FA);
WWHD_OFFSET(Act_c, pad, 0x3FB);
WWHD_OFFSET(Act_c, mScaleTarget, 0x3FC);
WWHD_OFFSET(Act_c, mIsRide, 0x408);
WWHD_OFFSET(Act_c, mIsPlayerRide, 0x40D);
WWHD_OFFSET(Act_c, mIsHeavyRide, 0x410);
WWHD_OFFSET(Act_c, pad2, 0x415);
WWHD_OFFSET(Act_c, mPushTimer, 0x416);
WWHD_OFFSET(Act_c, mbFastRide, 0x418);
WWHD_OFFSET(Act_c, pad3, 0x419);
WWHD_SIZE(Act_c, 0x41C);
} // namespace daObjJump

// HD: private views of fields used by this actor; no alternate shared class
// definitions.
struct JumpPlayerHD : fopAc_ac_c {
  u8 _3AC[12];
  be<u32> mNoResetFlg0, mNoResetFlg1;
};
WWHD_OFFSET(JumpPlayerHD, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(JumpPlayerHD, mNoResetFlg1, 0x3BC);
WWHD_SIZE(JumpPlayerHD, 0x3C0);
struct JumpPlayHD {
  u8 _0[0x5B2C];
  gptr<JumpPlayerHD> mPlayer;
  u8 _5B30[0x5CD8 - 0x5B30];
  be<u32> mPlayerStatus0;
  u8 _5CDC[0x5D70 - 0x5CDC];
  be<u32> mBackgroundOpaque, mBackgroundTranslucent, mOpaque, mTranslucent;
};
WWHD_OFFSET(JumpPlayHD, mPlayer, 0x5B2C);
WWHD_OFFSET(JumpPlayHD, mPlayerStatus0, 0x5CD8);
WWHD_OFFSET(JumpPlayHD, mBackgroundOpaque, 0x5D70);
WWHD_SIZE(JumpPlayHD, 0x5D80);
struct JumpDrawListsHD {
  be<u32> mOpaque, mTranslucent;
};
WWHD_SIZE(JumpDrawListsHD, 8);
struct JumpActorVtableHD {
  static const JumpActorVtableHD *get(const daObjJump::Act_c *actor) {
    return gabi::at<JumpActorVtableHD>(actor->__vtbl);
  }
  s32 draw(daObjJump::Act_c *actor) const {
    return gabi::call<s32>(mDraw, actor);
  }
  s32 isDelete(daObjJump::Act_c *actor) const {
    return gabi::call<s32>(mIsDelete, actor);
  }
  u8 _0[0x2C];
  be<u32> mDraw;
  u8 _30[12];
  be<u32> mIsDelete;
};
WWHD_OFFSET(JumpActorVtableHD, mIsDelete, 0x3C);
WWHD_SIZE(JumpActorVtableHD, 0x40);
struct JumpAttributesHD {
  daObjJump::Attr_c mEntries[2];
};
WWHD_SIZE(JumpAttributesHD, 0xA0);
struct JumpModeTableHD {
  gabi::ptmf<void (daObjJump::Act_c::*)()> mModes[6];
};
WWHD_SIZE(JumpModeTableHD, 48);
struct JumpHeaderStaticsHD {
  be<f32> mFirst, mSecond;
  be<u8> mTrigState, mRandomState;
  u8 _A[2];
  be<u32> mZero[4];
};
WWHD_OFFSET(JumpHeaderStaticsHD, mZero, 12);
WWHD_SIZE(JumpHeaderStaticsHD, 28);
struct JumpHeaderConstantsHD {
  be<f32> mFirst, mSecond;
};
WWHD_SIZE(JumpHeaderConstantsHD, 8);
struct JumpRegistrationHD {
  u8 mData[12];
};
WWHD_SIZE(JumpRegistrationHD, 12);
struct JumpRegistrationsHD {
  JumpRegistrationHD mEntries[3];
};
WWHD_SIZE(JumpRegistrationsHD, 36);

struct JumpBackgroundHD {
  u8 _0[0xB0];
  gfn mRideCallback;
  void setRideCallback(void (*callback)(void *, daObjJump::Act_c *,
                                        fopAc_ac_c *)) {
    mRideCallback.v = gabi::guest_fn((const void *)callback);
  }
};
WWHD_OFFSET(JumpBackgroundHD, mRideCallback, 0xB0);
WWHD_SIZE(JumpBackgroundHD, 0xB4);
