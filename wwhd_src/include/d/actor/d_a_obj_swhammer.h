#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_particle.h"
#include "gc_api/gc_api.h"
namespace daObjSwhammer {
// HD: actor-tail members move by 0x118; emitter fields remain packed.
struct SwhammerEmitterHD {
    u8 _0[0x14];
    cXyz mEmitterTranslation;
    u8 _20[0x34 - 0x20];
    be<f32> mRate;
    u8 _38[0x5C - 0x38];
    be<s32> mMaxFrame;
    be<u16> mLifeTime;
    u8 _62[6];
    be<f32> mInitialVelOmni, mInitialVelAxis;
};
WWHD_OFFSET(SwhammerEmitterHD, mRate, 0x34);
WWHD_OFFSET(SwhammerEmitterHD, mLifeTime, 0x60);
WWHD_SIZE(SwhammerEmitterHD, 0x70);
struct SwhammerPlayerHD : fopAc_ac_c {
    be<u8> mCutType;
};
WWHD_OFFSET(SwhammerPlayerHD, mCutType, 0x3AC);
struct SwhammerCylinderSourceHD {
    u8 mInfo[0x3C];
    be<f32> mRadius, mHeight;
};
WWHD_SIZE(SwhammerCylinderSourceHD, 0x44);
struct SwhammerHeaderHD {
    be<f32> mPiMin, mPiMax, mLowerHeight;
    be<u8> mAngleByte, mAngleFlag;
    u8 _E[2];
    be<u32> mZero[4];
};
WWHD_SIZE(SwhammerHeaderHD, 0x20);
struct Act_c : dBgS_MoveBgActor {
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    dCcD_Cyl mCylCo;
    dCcD_Stts mSttsCo;
    dCcD_Cyl mCylTg;
    dCcD_Stts mSttsTg;
    be<s32> mMode;
    be<s16> mCrushTimer;
    be<u8> mCrushState;
    u8 pad;
    be<f32> mScaleYSpeed, mScaleY, mAngleZ, mAngleX, mAngleSpeedZ, mAngleSpeedX, mTargetHFrac, mCurHFrac,
        mVSpeed, mTopPos;
    dPa_smokeEcallBack mSmokeCb;
    be<u8> m5FC;
    u8 pad2[3];

    s32 _create();
    bool _delete();
    s32 CreateHeap();
    s32 Create();
    s32 Delete();
    s32 Draw();
    s32 Execute(Mtx34 **);
    void set_mtx();
    void init_mtx();
    void set_damage();
    void vib_start(s16, f32);
    void vib_proc();
    void crush_start();
    void crush_proc();
    void eff_crush();
    void calc_top_pos();
    void mode_upper_init();
    void mode_lower_init();
    void mode_u_l_init();
    void mode_l_u_init();
    void mode_upper();
    void mode_lower();
    void mode_u_l();
    void mode_l_u();
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mMode, 0x6C4);
WWHD_OFFSET(Act_c, mSmokeCb, 0x6F4);
WWHD_SIZE(Act_c, 0x718);
} // namespace daObjSwhammer
