// Wind Temple elevator. HD: fields shift from GameCube 0x2C8 to 0x3E0.
#pragma once
#include "gc_api/gc_api.h"
namespace daObjHbrf1 {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c();
    enum Mode_e {
        Mode_DOWN_STOP_e,
        Mode_UP_DEMO_WAIT_e,
        Mode_UP_DEMO_TIMER_e,
        Mode_UP_DEMO_e,
        Mode_UP_STOP_e,
        Mode_DOWN_DEMO_WAIT_e,
        Mode_DOWN_DEMO_TIMER_e,
        Mode_DOWN_DEMO_e,
    };
    enum Prm_e {
        PRM_SWSAVE_W = 0x08,
        PRM_SWSAVE_S = 0x00,

        PRM_TYPE_W = 0x01,
        PRM_TYPE_S = 0x08,

        PRM_EVENT_W = 0x08,
        PRM_EVENT_S = 0x10,
    };
    s32 prm_get_swSave();
    s32 prm_get_Type();
    s32 prm_get_Event();
    s32 Mthd_Create();
    s32 Mthd_Delete();
    s32 CreateHeap();
    s32 Create();
    s32 Delete();
    void set_mtx();
    void init_mtx();
    void daObjHbrf1_down_stop();
    void daObjHbrf1_up_demo_wait();
    void daObjHbrf1_up_demo_timer();
    void daObjHbrf1_up_demo();
    void daObjHbrf1_up_stop();
    void daObjHbrf1_down_demo_wait();
    void daObjHbrf1_down_demo_timer();
    void daObjHbrf1_down_demo();
    s32 Execute(gptr<Mtx34>*);
    s32 Draw();
    /* 0x3E0 */ be<f32> mYOffset;
    /* 0x3E4 */ request_of_phase_process_class mPhs;
    /* 0x3EC */ gptr<J3DModel> mpModel;
    /* 0x3F0 */ be<s32> mMode;
    /* 0x3F4 */ be<s16> mEventIdx;
    /* 0x3F6 */ u8 pad[2];
    /* 0x3F8 */ be<s32> mTimer;
    /* 0x3FC */ be<u8> m2E4;
    /* 0x3FD */ u8 pad2[3];
};
WWHD_OFFSET(Act_c, mYOffset, 0x3E0);
WWHD_OFFSET(Act_c, mpModel, 0x3EC);
WWHD_OFFSET(Act_c, mMode, 0x3F0);
WWHD_OFFSET(Act_c, mTimer, 0x3F8);
WWHD_SIZE(Act_c, 0x400);
} // namespace daObjHbrf1
