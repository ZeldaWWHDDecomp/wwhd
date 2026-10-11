// Tower of the Gods pillar.
#pragma once
#include "gc_api/gc_api.h"

// HD: tww has no member layout for this actor. These names describe the verified HD fields.
struct daObjHcbh_c : fopAc_ac_c {
    enum BreakCondition { BREAK_NONE, BREAK_HEAVY_WEAPON, BREAK_HAMMER, BREAK_BOMB, BREAK_STATUE };
    BOOL chk_appear();
    void set_mtx();
    void init_mtx();
    BOOL create_heap();
    void setup_break_condition(fopAc_ac_c*);
    void checkCollision();
    void particle_set();
    void make_item();
    cPhs_State _create();
    BOOL _delete();
    void wait_act_proc();
    void fall_act_proc();
    BOOL _execute();
    BOOL _draw();
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mModel;
    /* 0x3B8 */ gptr<J3DModel> mFragmentModels[4];
    /* 0x3C8 */ gptr<dBgW> mBackground[2];
    /* 0x3D0 */ dBgS_ObjAcch mAcch;
    /* 0x594 */ dBgS_AcchCir mAcchCir;
    /* 0x5D4 */ dCcD_Stts mStts;
    /* 0x610 */ dCcD_Cyl mCyl;
    /* 0x740 */ dCcD_Sph mSph[6];
    /* 0xE48 */ be<f32> mPillarHeight;
    /* 0xE4C */ be<f32> mFallVelocity;
    /* 0xE50 */ be<s16> mPillarAngle;
    /* 0xE52 */ be<s16> mPillarAngularVelocity;
    /* 0xE54 */ cXyz mFragmentPositions[4];
    /* 0xE84 */ be<f32> mFragmentVelocities[4];
    /* 0xE94 */ be<s16> mFragmentAngles[4];
    /* 0xE9C */ be<s16> mFragmentYaws[4];
    /* 0xEA4 */ be<s16> mFragmentAngularVelocities[4];
    /* 0xEAC */ be<s32> mFragmentDelays[4];
    /* 0xEBC */ be<s16> mBreakYaw;
    /* 0xEBE */ u8 _EBE[2];
    /* 0xEC0 */ be<s32> mBreakCondition;
    /* 0xEC4 */ dPa_smokeEcallBack mSmoke;
    /* 0xEE4 */ be<u8> mAppears;
    /* 0xEE5 */ u8 _EE5[3];
    /* 0xEE8 */ gabi::ptmf<void (daObjHcbh_c::*)()> mAction;
};
WWHD_OFFSET(daObjHcbh_c, mAcch, 0x3D0);
WWHD_OFFSET(daObjHcbh_c, mPillarHeight, 0xE48);
WWHD_OFFSET(daObjHcbh_c, mAction, 0xEE8);
WWHD_SIZE(daObjHcbh_c, 0xEF0);
