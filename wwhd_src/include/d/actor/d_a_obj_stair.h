/** Crumbling stair (Mkdan). HD retains the GC actor fields after the larger
 * actor base. HIO falls at -175 instead of -70; HD stores the vtable at the end
 * of HIO. */
#pragma once
#include "bindings.h"

enum { dRes_INDEX_MKDAN_BDL_MKDAN1_e = 4, dRes_INDEX_MKDAN_DZB_MKDAN1_e = 7 };
struct StairCapsuleGeometryHD {
  cXyz mStart, mEnd;
  be<u32> mVtable;
  be<f32> mRadius;
};
WWHD_SIZE(StairCapsuleGeometryHD, 0x20);
WWHD_OFFSET(StairCapsuleGeometryHD, mRadius, 0x1C);
struct StairCapsuleShapeHD {
  u8 mBounds[0x18];
  be<u32> mBoundsVtable, mShapeVtable;
};
struct StairCapsuleHD : dCcD_GObjInf {
  StairCapsuleShapeHD mShapeAttr;
  StairCapsuleGeometryHD mCapsule;
};
using StairRideCallback = void (*)(dBgW *, fopAc_ac_c *, fopAc_ac_c *);
struct StairBackgroundHD {
  u8 mReserved[0xB0];
  be<u32> mRideCallback;
  void SetRideCallback(StairRideCallback callback) {
    mRideCallback = gabi::guest_fn((const void *)callback);
  }
};
struct StairQuaternionWordsHD {
  be<u32> mWords[4];
};
WWHD_SIZE(StairCapsuleShapeHD, 0x20);
WWHD_OFFSET(StairCapsuleHD, mShapeAttr, 0xF8);
WWHD_OFFSET(StairCapsuleHD, mCapsule, 0x118);
WWHD_SIZE(StairCapsuleHD, 0x138);
WWHD_OFFSET(StairBackgroundHD, mRideCallback, 0xB0);
WWHD_SIZE(StairBackgroundHD, 0xB4);
WWHD_SIZE(StairQuaternionWordsHD, 16);
struct daobj_stairHIO_c {
  /* 0x00 */ be<s8> mNo;
  /* 0x01 */ u8 _01[3];
  /* 0x04 */ be<f32> mFallAccel;  /* was m08: _execute 02390AC4: after mBreakDelay, speed.y eases toward mFallSpeedMax with cLib_addCalc2 step it; daobj_stairHIO_c_ct 02391570 2.5 (tww the same) */
  /* 0x08 */ be<f32> mFallSpeedMax;  /* was m0C: _execute 02390AC4: the target of speed.y while the broken stair falls; daobj_stairHIO_c_ct 02391570 -175 (tww the same) */
  /* 0x0C */ be<f32> mBreakRotSpeed;  /* was m10: _execute 02390AC4: when mBreakTimer reaches mBreakDelay the spin per frame is it * 0.5 * the hit offset length * 0.01 * mBreakPower degrees (mRotSpeed); daobj_stairHIO_c_ct 02391570 5 (tww the same) */
  /* 0x10 */ be<f32> mBreakSpeed;  /* was m14: _execute 02390AC4: when mBreakTimer reaches mBreakDelay the speed is it * mBreakPower along mBreakDir (times the spin cosine); daobj_stairHIO_c_ct 02391570 2.5 (tww the same) */
  /* 0x14 */ be<s16> mBreakDelay;  /* was m18: ride_call_back 023905F4 and the hit code in _execute 02390AC4 start mBreakTimer below it (it - 2, or less for a far hit); at it the stair breaks loose (speed, spin, JA_SE_OBJ_BREAK_STEPS), it shakes in the 10 frames before, falls after and is deleted 240 frames later; daobj_stairHIO_c_ct 02391570 6 (tww the same) */
  /* 0x16 */ u8 _16[2];
  /* 0x18 */ be<u32> __vtbl;
};

struct daObj_Stair_c : dBgS_MoveBgActor {
  cPhs_State _create();
  bool _delete();
  bool _execute();
  BOOL CreateHeap();
  BOOL Create();
  BOOL Execute(Mtx34 **);
  BOOL Draw();
  BOOL Delete();

  /* 0x3E0 */ cXyz mHitOffset;  /* was field_0x2C8: ride_call_back 023905F4 / _execute 02390AC4 (bomb hit): the rider's or bomb's position minus the step's (tww); _execute uses its direction and length for the break tilt */
  /* 0x3EC */ cXyz mBreakDir;  /* was field_0x2D4: ride_call_back 023905F4 (0, -1, 0) for the player; _execute 02390AC4 the bomb-to-step vector clamped to the step (tww); the break speed is l_HIO.mBreakSpeed * mBreakPower * it, the tilt axis is mHitOffset x it */
  /* 0x3F8 */ be<s32> mBreakTimer;  /* was field_0x2E0: _execute 02390AC4: frames since the break started; the break happens at l_HIO.mBreakDelay, the fall after it, fopAcM_delete at l_HIO.mBreakDelay + 0xF0 (tww); ride_call_back 023905F4 only triggers while it is below l_HIO.mBreakDelay; _create 0239069C 0 */
  /* 0x3FC */ be<s32> mBreaking;  /* was field_0x2E4: ride_call_back 023905F4 / _execute 02390AC4 (bomb hit) set 1; _execute runs the break and Execute 02391328 shakes the debris position while it is 1 (tww) */
  /* 0x400 */ be<f32> mShakeY;  /* was field_0x2E8: _execute 02390AC4: 1.5 * sin offset; Execute 02391328 adds it to current.pos.y (tww transS(.., current.pos.y + it, ..)); _create 0239069C 0 */
  /* 0x404 */ be<f32> mBreakPower;  /* was field_0x2EC: ride_call_back 023905F4 1; _execute 02390AC4 scales the break speed and the tilt angle by it (tww); _create 0239069C 0 */
  /* 0x408 */ request_of_phase_process_class mPhs;
  /* 0x410 */ gptr<J3DModel> mpModel;
  /* 0x414 */ Mtx34 mMtx;  /* was field_0x2FC: Execute 02391328: copy of mDoMtx_stack_c, handed out as the BG matrix (tww MTXCopy(.., it); *mtx = &it) */
  /* 0x444 */ dCcD_Stts mStts;
  /* 0x480 */ StairCapsuleHD mCps;
  /* 0x5B8 */ Quaternion mRot;  /* was field_0x4A0: _create 0239069C: the start rotation; Execute 02391328 multiplies it by mRotSpeed each frame (tww mDoMtx_quatMultiply(&it, &mRotSpeed, &quat); it = quat) and applies it (quatM) */
  /* 0x5C8 */ Quaternion mRotSpeed;  /* was field_0x4B0: _execute 02390AC4: the per-frame break rotation (axis mHitOffset x mBreakDir, angle from l_HIO.mBreakRotSpeed and mBreakPower; identity when the axis is 0) (tww); _create 0239069C identity */
  /* 0x5D8 */ cXyz field_0x4C0;
};
WWHD_OFFSET(daObj_Stair_c, mStts, 0x444);
WWHD_OFFSET(daObj_Stair_c, mRot, 0x5B8);
WWHD_SIZE(daObj_Stair_c, 0x5E4);

WWHD_SIZE(daobj_stairHIO_c, 0x1C);
WWHD_OFFSET(daobj_stairHIO_c, mBreakDelay, 0x14);
WWHD_OFFSET(daobj_stairHIO_c, __vtbl, 0x18);
WWHD_OFFSET(daObj_Stair_c, mCps, 0x480);
WWHD_OFFSET(daObj_Stair_c, mMtx, 0x414);
WWHD_OFFSET(daObj_Stair_c, field_0x4C0, 0x5D8);
