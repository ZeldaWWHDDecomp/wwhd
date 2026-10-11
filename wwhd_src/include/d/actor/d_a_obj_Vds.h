#pragma once
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "gc_api/d/d_kankyo.h"

namespace daObjVds {
// HD: tww has no recovered field layout. Names in assertions identify the animation resources.
struct Act_c : fopAc_ac_c {
    enum Process { PROCESS_OFF, PROCESS_ON, PROCESS_COUNT };
    enum LightStage { LIGHT_SEARCH, LIGHT_CREATE, LIGHT_EXECUTE };
    enum EventState { EVENT_NONE, EVENT_ORDER, EVENT_RUNNING };
    request_of_phase_process_class mPhase;
    Mtx34 mBgMtx;
    gptr<mDoExt_McaMorf> M_anm0;
    gptr<J3DAnmTransform> M_bck_data0;
    mDoExt_brkAnm mBrk0;
    gptr<J3DAnmTevRegKey> M_brk_data0;
    gptr<mDoExt_McaMorf> M_anm1;
    gptr<J3DAnmTransform> M_bck_data1;
    mDoExt_brkAnm mBrk1;
    gptr<J3DAnmTevRegKey> M_brk_data1;
    gptr<dBgW> mpBgW;
    be<s32> mLoopAnimation;
    be<s32> mProcess;
    be<s32> mLightStage;
    be<s32> mSwitchActor[2];
    be<f32> mStrength[2];
    be<s16> mEvent;
    be<s16> mEventState;
    be<s16> mEventIndex;
    u8 _512[2];
    LIGHT_INFLUENCE mLights[2];
    cXyz mLightPosition[2];

    void set_mtx();
    BOOL create_heap();
    BOOL process_init(s32);
    void set_first_process();
    void Event_init();
    s32 _create();
    void delete_point_light();
    BOOL _delete();
    void Event_exe();
    BOOL PlayLoopJointAnimation();
    void* search_switchCB(fopAc_ac_c*);
    void create_point_light(s32, cXyz*);
    void execute_point_light();
    void process_common();
    void process_main();
    BOOL _execute();
    BOOL _draw();
    BOOL SetLoopJointAnimation(J3DAnmTransform*, J3DAnmTransform*, f32, f32);
    BOOL process_off_init();
    void process_off_main();
    BOOL process_on_init();
    void process_on_main();
};
} // namespace daObjVds
WWHD_OFFSET(daObjVds::Act_c, M_anm0, 0x3E4);
WWHD_OFFSET(daObjVds::Act_c, mProcess, 0x4F4);
WWHD_OFFSET(daObjVds::Act_c, mLights, 0x514);
WWHD_OFFSET(daObjVds::Act_c, mLightPosition, 0x55C);
WWHD_SIZE(daObjVds::Act_c, 0x574);
