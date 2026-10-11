/* Nintendo Gallery hatch, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
struct OhatchAction {
    be<s16> delta, index;
    be<u32> target;
};
WWHD_SIZE(OhatchAction, 8);
struct daObjOhatch_c : fopAc_ac_c {
    daObjOhatch_c();
    enum : u32 { __vt = 0x1002DF44 };
    enum Param_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mModel;
    be<s32> mSwitch;
    gptr<dBgW> mClosedBg, mOpenBg;
    Mtx34 mBgMatrix;
    be<s16> mHingeAngle;
    u8 _3F6[2];
    OhatchAction mAction;
    be<f32> mTremorHeight;
    be<s32> mTremorPhase;
    be<s16> mVibrationAmplitude, mVibrationPhase;
    bool create_heap();
    void init_mtx();
    s32 _create();
    bool _delete();
    void set_mtx();
    bool _execute();
    bool _draw();
    void close_wait_act_proc();
    void tremor_act_proc();
    void open_act_proc();
    void vibrate_act_proc();
    void open_wait_act_proc();
};
WWHD_OFFSET(daObjOhatch_c, mModel, 0x3B4);
WWHD_OFFSET(daObjOhatch_c, mBgMatrix, 0x3C4);
WWHD_OFFSET(daObjOhatch_c, mAction, 0x3F8);
WWHD_OFFSET(daObjOhatch_c, mTremorHeight, 0x400);
WWHD_SIZE(daObjOhatch_c, 0x40C);

enum class HatchAction : u32 {
    WaitClosed = 0x0237F7D8,
    Tremor = 0x0237F990,
    Open = 0x0237FA0C,
    Vibrate = 0x0237FB74,
    WaitOpen = 0x0237FC60
};
struct OhatchHeaderZeros {
    be<u32> words[4];
};
struct OhatchPlay {
    u8 padding[0x52C4];
    u8 mEventManager[1];
};
inline void *ohatchEventManager(void *play) {
    return ((OhatchPlay *)play)->mEventManager;
}
inline bool ohatchCutIsOpen(u32 cut, const char *expected) {
    auto *left = gabi::at<be<u8>>(cut);
    auto *right = gabi::at<be<u8>>(gabi::ea(expected));
    u8 a, b;
    do {
        a = *left++;
        b = *right++;
    } while (a == b && a);
    return a == b;
}
s32 ohatchGetEventIdx(void *, const char *, s32);
u32 ohatchGetEventData(void *, s32);
u32 ohatchGetCutName(void *, s32);
void mDoMtx_XYZrotM(Mtx34 *, s16, s16, s16);
void ohatchSeStart(u32);
void ohatchActorDt(void *, s32);
void ohatchInitA(void *);
void ohatchInitB(void *);
