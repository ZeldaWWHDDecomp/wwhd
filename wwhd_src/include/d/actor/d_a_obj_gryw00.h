#pragma once
#include "gc_api/gc_api.h"
enum { dBgS_MoveBGProc_Trans = 0x024EE76C };
// HD: extra inline-constructed SDK state within the animation.
struct GrywBckState {
    J3DFrameCtrl mFrameCtrl;
    be<u32> mVtable;
    u8 mObject[0x34];
    be<u32> mObjectVtable;
    u8 mPadding4C[0x0C];
    be<u32> field_0x58;
    u8 mPadding5C[0x20];
    be<u32> field_0x7C, field_0x80, field_0x84, field_0x88;
};
WWHD_SIZE(GrywBckState, 0x8C);
// HD: emitter translation and invalidation fields used by this actor.
struct GrywEmitter {
    u8 mPadding0[0x5C];
    be<s32> mMaxFrame;
    u8 mPadding60[0x22C - 0x60];
    cXyz mGlobalTranslation;
    u8 mPadding238[0x254 - 0x238];
    be<u32> mFlags;
    u8 mPadding258[0x0A];
    be<u8> mType;
};
WWHD_OFFSET(GrywEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(GrywEmitter, mType, 0x262);
void Gryw_frameInit(J3DFrameCtrl*, s32);
void Gryw_objectConstruct(void*);
void Gryw_objectDestruct(void*, s32);
BOOL Gryw_checkFrame(J3DFrameCtrl*, f32);
void Gryw_scaleM(f32, f32, f32);
void Gryw_copyMatrix(const Mtx34*, Mtx34*);
struct daObjGryw00_c : dBgS_MoveBgActor {
    daObjGryw00_c();
    using ModeFunc = void (daObjGryw00_c::*)();
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    Mtx34 mMtx;
    mDoExt_btkAnm mBtk;
    union { mDoExt_bckAnm mBck; GrywBckState mBckState; };
    be<s32> mActivationSwitch;
    gabi::ptmf<ModeFunc> modeFunc;
    be<s32> mIsHidden;
    be<f32> mWaterLv, mWaterMaxLv, mWaterLvIncrement;
    be<f32> mWaveAmplitude, mWaveMaxAmplitude, mWaveAmplitudeIncrement;
    be<f32> mWaterLvFinalYPos;
    be<s32> mShouldPlaySe, mGeyserSeRemaining;
    gptr<GrywEmitter> mpEmitters[2];
    be<s32> mParticleRetireRemaining;
    s32 Mthd_Create(); s32 Mthd_Delete();
    BOOL CreateHeap(); BOOL Create(); BOOL Execute(gptr<Mtx34>*); BOOL Draw(); BOOL Delete();
    f32 get_draw_water_lv(daObjGryw00_c*);
    BOOL setup_high_water_level_btk_anm();
    void particle_set(); void particle_move(); void particle_delete(); void set_se();
    void switch_wait_act_proc(); void spread_water_face_act_proc();
    void water_level_move_wait_act_proc(); void anime_loop_start_wait_act_proc();
    void high_water_level_act_proc();
};
WWHD_OFFSET(daObjGryw00_c, mPhs, 0x3E0);
WWHD_OFFSET(daObjGryw00_c, mBtk, 0x41C);
WWHD_OFFSET(daObjGryw00_c, mBck, 0x490);
WWHD_OFFSET(daObjGryw00_c, modeFunc, 0x520);
WWHD_OFFSET(daObjGryw00_c, mpEmitters, 0x550);
WWHD_SIZE(daObjGryw00_c, 0x55C);
