/** Firefly: tww members shifted by +0x11C for the larger HD actor base.
 * HD: the appended point light extends the class to 0x5C0.
 */
#pragma once
#include "bindings.h"

enum {
    dRes_INDEX_FF_BMD_HO_e = 5,
    dRes_INDEX_FF_BMD_HOP_e = 6,
    dRes_INDEX_FF_BRK_HO_e = 12,
    dRes_INDEX_FF_BRK_HOP_e = 13
};

struct fopAcM_prmBase_class {
    /* 0x00 */ be<u32> parameters;
    /* 0x04 */ cXyz position;
    /* 0x10 */ csXyz angle;
    /* 0x16 */ be<u16> setID;
};
WWHD_OFFSET(fopAcM_prmBase_class, position, 4);
WWHD_OFFSET(fopAcM_prmBase_class, angle, 0x10);
WWHD_SIZE(fopAcM_prmBase_class, 0x18);
struct fopAcM_prmScale_class {
    be<u8> x, y, z;
};
WWHD_SIZE(fopAcM_prmScale_class, 3);
struct fopAcM_prm_class {
    /* 0x00 */ fopAcM_prmBase_class base;
    /* 0x18 */ fopAcM_prmScale_class scale;
    /* 0x1B */ be<u8> gbaName;
    /* 0x1C */ be<u32> parent_id;
    /* 0x20 */ be<s8> argument;
    /* 0x21 */ be<s8> room_no;
    /* 0x22 */ u8 _22[2];
};
WWHD_OFFSET(fopAcM_prm_class, base, 0);
WWHD_OFFSET(fopAcM_prm_class, scale, 0x18);
WWHD_OFFSET(fopAcM_prm_class, parent_id, 0x1C);
WWHD_OFFSET(fopAcM_prm_class, room_no, 0x21);
WWHD_SIZE(fopAcM_prm_class, 0x24);
struct FfPositionWords {
    be<u32> x, y, z;
};
WWHD_SIZE(FfPositionWords, 0xC);

struct ff_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel[2];
    /* 0x3BC */ gptr<mDoExt_brkAnm> mBrkAnm[2];
    /* 0x3C4 */ u8 m2A8[1];
    /* 0x3C5 */ be<u8> mbNoUseGroundY;
    /* 0x3C6 */ u8 m2AA[2];
    /* 0x3C8 */ be<f32> mRotVel;
    /* 0x3CC */ be<f32> mVelocityFwdTarget;
    /* 0x3D0 */ cXyz mVelImpulse;
    /* 0x3DC */ cXyz m2C0;
    /* 0x3E8 */ be<f32> mScale;
    /* 0x3EC */ be<f32> mScaleTarget;
    /* 0x3F0 */ be<f32> mGlowScale;
    /* 0x3F4 */ be<f32> mGlowScaleY;
    /* 0x3F8 */ be<f32> mGroundY;
    /* 0x3FC */ cXyz mScatterPos;
    /* 0x408 */ cXyz m2EC;
    /* 0x414 */ cXyz mHomePos;
    /* 0x420 */ be<s16> mTargetRotX;
    /* 0x422 */ be<s16> mTargetRotY;
    /* 0x424 */ be<s16> mTimers[5];
    /* 0x42E */ be<s16> mLiveTimer;
    /* 0x430 */ be<s8> mMode;
    /* 0x431 */ u8 m315[2];
    /* 0x433 */ be<u8> mbNotVisibleZ;
    /* 0x434 */ dCcD_Stts mStts;
    /* 0x470 */ dCcD_Sph mSph;
    /* 0x59C */ LIGHT_INFLUENCE mInf; /* HD */
};
WWHD_OFFSET(ff_class, mStts, 0x434);
WWHD_OFFSET(ff_class, mSph, 0x470);
WWHD_OFFSET(ff_class, mInf, 0x59C);
WWHD_SIZE(ff_class, 0x5C0);
WWHD_OFFSET(ff_class, mPhs, 0x3AC);
WWHD_OFFSET(ff_class, mpModel, 0x3B4);
WWHD_OFFSET(ff_class, mBrkAnm, 0x3BC);
WWHD_OFFSET(ff_class, m2A8, 0x3C4);
WWHD_OFFSET(ff_class, mbNoUseGroundY, 0x3C5);
WWHD_OFFSET(ff_class, m2AA, 0x3C6);
WWHD_OFFSET(ff_class, mRotVel, 0x3C8);
WWHD_OFFSET(ff_class, mVelocityFwdTarget, 0x3CC);
WWHD_OFFSET(ff_class, mVelImpulse, 0x3D0);
WWHD_OFFSET(ff_class, m2C0, 0x3DC);
WWHD_OFFSET(ff_class, mScale, 0x3E8);
WWHD_OFFSET(ff_class, mScaleTarget, 0x3EC);
WWHD_OFFSET(ff_class, mGlowScale, 0x3F0);
WWHD_OFFSET(ff_class, mGlowScaleY, 0x3F4);
WWHD_OFFSET(ff_class, mGroundY, 0x3F8);
WWHD_OFFSET(ff_class, mScatterPos, 0x3FC);
WWHD_OFFSET(ff_class, m2EC, 0x408);
WWHD_OFFSET(ff_class, mHomePos, 0x414);
WWHD_OFFSET(ff_class, mTargetRotX, 0x420);
WWHD_OFFSET(ff_class, mTargetRotY, 0x422);
WWHD_OFFSET(ff_class, mTimers, 0x424);
WWHD_OFFSET(ff_class, mLiveTimer, 0x42E);
WWHD_OFFSET(ff_class, mMode, 0x430);
WWHD_OFFSET(ff_class, m315, 0x431);
WWHD_OFFSET(ff_class, mbNotVisibleZ, 0x433);
