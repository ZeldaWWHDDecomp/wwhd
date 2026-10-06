#pragma once
#include "bindings.h"
namespace daObjLight {
struct Act_c : fopAc_ac_c {
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mModels[3];
    gptr<dBgW> mBgW;
    Mtx34 mBgMatrix;
    be<s16> mAngle, mLightTimer, mLit, mEventTimer;
    dCcD_Stts mStatus;
    dCcD_Cyl mCylinder;
    u8 mFireCallback[0x14];
    be<s16> mFirePhase, mFireRotation;
    be<u8> mFireAlpha;
    u8 _581[3];
    be<f32> mFireScale;
    Mtx34 mFireMatrix;
    be<s16> mEventId, mEventState, mMainEventId;
    void set_mtx();
    bool create_heap();
    void init_collision();
    void exe_fire();
    bool set_fire(s32);
    cPhs_State _create();
    void delete_fire();
    bool _delete();
    void exe_event();
    bool now_event(s16);
    void renew_angle();
    void set_collision();
    bool set_event(s16);
    void control_light();
    bool _execute();
    bool _draw();
};
WWHD_OFFSET(Act_c, mModels, 0x3B4);
WWHD_OFFSET(Act_c, mBgMatrix, 0x3C4);
WWHD_OFFSET(Act_c, mStatus, 0x3FC);
WWHD_OFFSET(Act_c, mCylinder, 0x438);
WWHD_OFFSET(Act_c, mFireCallback, 0x568);
WWHD_OFFSET(Act_c, mFireMatrix, 0x588);
WWHD_OFFSET(Act_c, mMainEventId, 0x5BC);
}
