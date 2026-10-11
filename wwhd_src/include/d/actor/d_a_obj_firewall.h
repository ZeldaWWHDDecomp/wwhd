#pragma once
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/m_Do/m_Do_ext.h"
struct FirewallEmitter;
// HD: tww has no member layout yet; these names describe the verified HD fields.
struct daObjFirewall_c : fopAc_ac_c {
    request_of_phase_process_class mPhase;
    dCcD_Stts mCollisionStatus;
    dCcD_Cyl mCollisionCylinder;
    gptr<J3DModel> mpModel;
    gptr<dBgW> mpBgW;
    mDoExt_btkAnm mBtk;
    mDoExt_brkAnm mBrk;
    gptr<FirewallEmitter> mpEmitters[12];
    be<u32> mSaveSwitch;
    LIGHT_INFLUENCE mLight;
    cXyz mLightPosition;
    be<u32> field_0x678;
    be<f32> mLightStrength;
    gabi::ptmf<void (daObjFirewall_c::*)()> mAction;
    be<s16> mEventIndex;
    be<u8> mSoundEnabled;
    u8 field_0x68b;
    cXyz mSoundPositions[8];
    be<u8> mSoundInitialized, mBurning;
    u8 _6EE[2];
    be<u32> mEventStarted, mPlayerSoundIndex;
    void init_mtx();
    BOOL create_heap();
    void registCollisionTable();
    void setPointLight();
    void particle_set();
    void particle_delete();
    void seStart(u32);
    void set_se(u32);
    void seDelete();
    void set_pl_se();
    void setup_burn_up();
    void setup_put_the_fire_out();
    s32 _create();
    BOOL _delete();
    void wait_act_proc();
    void wait2_act_proc();
    void wait3_act_proc();
    void appear_act_proc();
    void demo_end_wait_act_proc();
    void burn_wait_act_proc();
    void retire_act_proc();
    BOOL _execute();
    BOOL _draw();
};
WWHD_OFFSET(daObjFirewall_c, mpModel, 0x520);
WWHD_OFFSET(daObjFirewall_c, mpEmitters, 0x614);
WWHD_OFFSET(daObjFirewall_c, mAction, 0x680);
WWHD_OFFSET(daObjFirewall_c, mSoundPositions, 0x68C);
WWHD_SIZE(daObjFirewall_c, 0x6F8);
