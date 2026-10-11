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
  /* 0x04 */ be<f32> m08;
  /* 0x08 */ be<f32> m0C;
  /* 0x0C */ be<f32> m10;
  /* 0x10 */ be<f32> m14;
  /* 0x14 */ be<s16> m18;
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

  /* 0x3E0 */ cXyz field_0x2C8;
  /* 0x3EC */ cXyz field_0x2D4;
  /* 0x3F8 */ be<s32> field_0x2E0;
  /* 0x3FC */ be<s32> field_0x2E4;
  /* 0x400 */ be<f32> field_0x2E8;
  /* 0x404 */ be<f32> field_0x2EC;
  /* 0x408 */ request_of_phase_process_class mPhs;
  /* 0x410 */ gptr<J3DModel> mpModel;
  /* 0x414 */ Mtx34 field_0x2FC;
  /* 0x444 */ dCcD_Stts mStts;
  /* 0x480 */ StairCapsuleHD mCps;
  /* 0x5B8 */ Quaternion field_0x4A0;
  /* 0x5C8 */ Quaternion field_0x4B0;
  /* 0x5D8 */ cXyz field_0x4C0;
};
WWHD_OFFSET(daObj_Stair_c, mStts, 0x444);
WWHD_OFFSET(daObj_Stair_c, field_0x4A0, 0x5B8);
WWHD_SIZE(daObj_Stair_c, 0x5E4);

WWHD_SIZE(daobj_stairHIO_c, 0x1C);
WWHD_OFFSET(daobj_stairHIO_c, m18, 0x14);
WWHD_OFFSET(daobj_stairHIO_c, __vtbl, 0x18);
WWHD_OFFSET(daObj_Stair_c, mCps, 0x480);
WWHD_OFFSET(daObj_Stair_c, field_0x2FC, 0x414);
WWHD_OFFSET(daObj_Stair_c, field_0x4C0, 0x5D8);
