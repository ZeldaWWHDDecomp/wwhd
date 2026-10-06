#pragma once
#include "bindings.h"
namespace daObjHbrf1 {
struct Act_c : dBgS_MoveBgActor {
    be<f32> mYOffset;
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    be<s32> mMode;
    be<s16> mEventIdx;
    u8 pad[2];
    be<s32> mTimer;
    be<u8> mEventActive;
    u8 pad2[3];
    s32 Mthd_Create(); s32 Mthd_Delete(); s32 CreateHeap(); s32 Create(); s32 Delete();
    void set_mtx(); void init_mtx();
    void down_stop(); void up_demo_wait(); void up_demo_timer(); void up_demo();
    void up_stop(); void down_demo_wait(); void down_demo_timer(); void down_demo();
    s32 Execute(Mtx34**); s32 Draw();
};
WWHD_OFFSET(Act_c,mYOffset,0x3E0);
WWHD_OFFSET(Act_c,mpModel,0x3EC);
WWHD_OFFSET(Act_c,mMode,0x3F0);
WWHD_OFFSET(Act_c,mTimer,0x3F8);
WWHD_SIZE(Act_c,0x400);
}
