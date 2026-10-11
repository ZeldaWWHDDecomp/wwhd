/* Gyorg controller, WWHD layout.  HD: the actor base grows by 0x11C; controller members follow it
 * at 0x3AC. Retail child/death tracking and hide timers are retained; size
 * 0x5D0 (GC 0x4B4). GameCube's unnamed controller and HIO fields have
 * descriptive names here. */
#pragma once
#include "f_op/f_op_actor.h"
#include "d/d_bg_s.h"
class daGyCtrl_c : public fopAc_ac_c {
public:
  enum Proc_e { PROC_INIT_e, PROC_EXEC_e };
  enum Mode_e { MODE_SWITCH_WAIT_e, MODE_CREATE_e, MODE_WAIT_e, MODE_HIDE_e };
  enum Target_e { TARGET_PLAYER_e, TARGET_SHIP_e };
  enum Type_e { TYPE_NO_AREA_e, TYPE_AREA_e };
  daGyCtrl_c();
  void *searchNearActor(fopAc_ac_c *);
  void setTarget();
  BOOL setPathTargetPos();
  f32 getWaterY(cXyz *);
  BOOL lineCheck(cXyz *, cXyz *);
  BOOL checkPath();
  void setPathPos();
  void modeProc(s32, s32);
  void modeSwWaitInit();
  void modeSwWait();
  void modeCreateInit();
  void modeCreate();
  void modeWaitInit();
  void modeWait();
  void modeHideInit();
  void modeHide();
  void deadCheckGy();
  s32 _execute();
  BOOL _draw();
  void createInitNoArer();
  void createInit();
  u8 getParam(u32, u32, u32);
  void getArg();
  BOOL checkGyCtrlExist();
  s32 _create();
  /* 0x3AC */ cXyz mSpawnPositions[5];
  /* 0x3E8 */ cXyz mCheckPositions[5];
  /* 0x424 */ be<s16> mSpawnAngles[5];
  /* 0x42E */ be<u8> mPathFree[5];
  /* 0x433 */ u8 mPadPath;
  /* 0x434 */ be<f32> mDesiredRadius;
  /* 0x438 */ be<s32> mSpawnIndex;
  /* 0x43C */ be<s32> mTarget;
  /* 0x440 */ be<s32> mMode;
  /* 0x444 */ be<u8> mType;
  /* 0x445 */ be<u8> mCount;
  /* 0x446 */ u8 mPadCount[2];
  /* 0x448 */ be<f32> mMaxDistance;
  /* 0x44C */ be<u8> mSwitch;
  /* 0x44D */ be<u8> mLayerInitialized;
  /* 0x44E */ u8 mPadLayer[2];
  /* 0x450 */ be<s32> mRoom;
  /* 0x454 */ cXyz mCenter;
  /* 0x460 */ be<u8> mTargetInRange;
  /* 0x461 */ u8 mPadTarget[3];
  /* 0x464 */ be<f32> mRadius;
  /* 0x468 */ be<s16> mAngle;
  /* 0x46A */ u8 mPadAngle[2];
  /* 0x46C */ be<s32> mCreateTimer;
  /* 0x470 */ u8 mUnusedTimer[4];
  /* 0x474 */ be<s32> mCameraTimer;
  /* 0x478 */ be<s32> mCameraMode;
  /* 0x47C */ be<u32> mChildIds[5];
  /* 0x490 */ be<u8> mChildActive[5];
  /* 0x495 */ u8 mPadChild[3];
  /* 0x498 */ dBgS_ObjLinChk mLinChk;
  /* 0x504 */ be<u8> mPathClear;
  /* 0x505 */ u8 mPadLine[3];
  /* 0x508 */ cXyz mRadialPaths[16];
  /* 0x5C8 */ be<u8> mAllChildrenDead;
  /* 0x5C9 */ u8 mPadDead[3];
  /* 0x5CC */ be<s32> mHideTimer;
};
WWHD_OFFSET(daGyCtrl_c, mLinChk, 0x498);
WWHD_OFFSET(daGyCtrl_c, mHideTimer, 0x5CC);
WWHD_SIZE(daGyCtrl_c, 0x5D0);
// Keep the retained Gyorg helper ABI; controller source uses the tww spelling.
using daGy_Ctrl_c = daGyCtrl_c;
class daGy_Ctrl_HIO_c {
public:
  /* 0x00 */ be<u32> mVtable;
  /* 0x04 */ be<u8> mDrawDebug;
  /* 0x05 */ be<u8> mDebugEnabled;
  /* 0x06 */ be<s16> mCreateDelay;
  /* 0x08 */ be<s16> mSpawnDelay;
  /* 0x0A */ be<s16> mPlayerAngleStep;
  /* 0x0C */ be<s16> mShipAngleStep;
  /* 0x0E */ u8 mPadAngle[2];
  /* 0x10 */ be<f32> mRadiusWave;
  /* 0x14 */ u8 mUnused[4];
  /* 0x18 */ be<f32> mShipRadius;
  /* 0x1C */ be<f32> mPlayerRadius;
  /* 0x20 */ be<f32> mMaximumDistance;
  /* 0x24 */ be<f32> mLineStartHeight;
  /* 0x28 */ be<f32> mLineEndHeight;
  /* 0x2C */ be<f32> mRadiusStep;
};
WWHD_SIZE(daGy_Ctrl_HIO_c, 0x30);
struct GyModeEntryHD {
  using ModeFunc = void (daGy_Ctrl_c::*)();
  gabi::ptmf<ModeFunc> init, exec;
  gptr<const char> name;
};
struct GyChildHD : fopAc_ac_c {
  u8 mPadding[0x20];
  be<u32> mAlive;
};
struct GyHeaderZerosHD {
  be<u32> words[4];
};
struct GyRegistrationHD {
  u8 bytes[12];
};
struct GyHideRegsHD {
  be<s16> initial, retry;
};
// GHS copies matrix translation words without quieting signalling NaNs.
inline void Gy_copyTranslationHD(cXyz &out, const Mtx34 &matrix) {
  reinterpret_cast<be<u32> &>(out.x) =
      reinterpret_cast<const be<u32> &>(matrix.m[0][3]);
  reinterpret_cast<be<u32> &>(out.y) =
      reinterpret_cast<const be<u32> &>(matrix.m[1][3]);
  reinterpret_cast<be<u32> &>(out.z) =
      reinterpret_cast<const be<u32> &>(matrix.m[2][3]);
}
void Gy_constructLine(cBgS_LinChk *);
void Gy_destructLine(cBgS_LinChk *, s32);
