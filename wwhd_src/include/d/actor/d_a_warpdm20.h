// Warp-demo actor: HD actor members are the GameCube layout +0x11C.
#pragma once
#include "bindings.h"
#include "d/d_demo.h"
#include "gc_api/tu.h"
struct Warpdm20ResourceKey;
struct daWarpdm20_c;
namespace warpdm20 {
// HD: the shared ripple callback is opaque; the verified producer needs its full 0x14 layout.
struct Emitter {
    u8 pad00[0x220];
    cXyz mGlobalDynamicsScale;
    u8 pad22C[12];
    cXyz mGlobalParticleScale;
    u8 pad244[0x10];
    be<u32> mStatus;
};
struct RippleCallback {
    be<u32> __vtbl;
    gptr<Emitter> mpEmitter;
    be<u32> m08, mStatus;
    be<f32> mRate;
};
WWHD_SIZE(RippleCallback, 0x14);
struct DemoObjects;
struct EventTable {
    gabi::ptmf<s32 (daWarpdm20_c::*)(s32)> entries[6];
};
struct ActionTable {
    gptr<const char> names[6];
};
struct PiPair {
    be<f32> negative, positive;
};
struct MtxCalc {
    u8 data[0x34];
};
struct BckConstructView : mDoExt_baseAnm {
    be<u32> mAnmVtable;
    MtxCalc mMtxCalc;
    be<u32> mMtxCalcVtable;
    u8 pad4C[12];
    be<u32> m58;
    u8 pad5C[0x20];
    be<u32> m7C, m80, m84, m88;
};
WWHD_SIZE(BckConstructView, 0x8C);
enum : u32 {
    ActorVtable = 0x1004211C,
    BaseAnmVtable = 0x1016E54C,
    MtxCalcVtable = 0x1016D820,
    BckAnmVtable = 0x100420F4
};
struct WordVector {
    be<u32> x, y, z;
};
inline WordVector* words(cXyz* p) {
    return reinterpret_cast<WordVector*>(p);
}
inline be<u32>* matrixWords(Mtx34* p) {
    return reinterpret_cast<be<u32>*>(p);
}
inline Mtx34* modelMatrix(J3DModel* p) {
    return gabi::at<Mtx34>(gabi::ea(p) + offsetof(J3DModel, mBaseTransformMtx));
}
inline u32 cullMatrixAddress(J3DModel* p) {
    return p ? gabi::ea(p) + offsetof(J3DModel, mBaseTransformMtx) : 0;
}
inline cXyz* modelScale(J3DModel* p) {
    return gabi::at<cXyz>(gabi::ea(p) + 0xBC);
}
inline J3DModelData* modelData(J3DModel* p) {
    return *gabi::at<gptr<J3DModelData>>(gabi::ea(p) + 0xAC);
}
inline cXyz* actorPosition(fopAc_ac_c* p) {
    return gabi::at<cXyz>(gabi::ea(p) + offsetof(fopAc_ac_c, current));
}
inline void* drawTag(fopAc_ac_c* p) {
    return gabi::at<void>(gabi::ea(p) + 0xDC);
}
inline DemoObjects* demoObjects() {
    return gabi::at<DemoObjects>(gabi::load<u32>(0x101D5FFC));
}
inline dComIfG_inf_c* game() {
    return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
}
inline const gabi::ptmf<s32 (daWarpdm20_c::*)(s32)>& eventAt(EventTable* p, u32 i) {
    return *gabi::at<gabi::ptmf<s32 (daWarpdm20_c::*)(s32)>>(gabi::ea(p) + i * 8);
}
} // namespace warpdm20
struct daWarpdm20_c : fopAc_ac_c {
    enum Sound {
        JA_SE_LK_HL_WAPR_D_IN = 0x288D,
        JA_SE_LK_HL_WAPR_U_OUT = 0x2890,
        JA_SE_OBJ_HL_WARP_OFF = 0x6A56,
        JA_SE_OBJ_HL_WAPR_EFF = 0x1088
    };
    enum EventBit { UNK_2D08 = 0x2D08, ZELDA_AWAKENED = 0x2D02, UNK_1820 = 0x1820 };
    enum Resource {
        dRes_INDEX_GDEMO20_BDL_GDEMO20_1_e = 10,
        dRes_INDEX_GDEMO20_BCK_GDEMO20_1_e = 6,
        dRes_INDEX_GDEMO20_BCK_GDEMO20_1A_e = 7,
        dRes_INDEX_GDEMO20_BTK_GDEMO20_1_e = 18,
        dRes_INDEX_GDEMO20_BRK_GDEMO20_1_e = 14
    };
    /*0x3AC*/ request_of_phase_process_class mPhase;
    /*0x3B4*/ gptr<J3DModel> mpModel;
    /*0x3B8*/ gptr<mDoExt_baseAnm> mpBtkAnm, mpBrkAnm, mpBckAnm;
    /*0x3C4*/ u8 m2A8[4];
    /*0x3C8*/ warpdm20::RippleCallback mRippleCallback;
    /*0x3DC*/ u8 m2C0[8];
    /*0x3E4*/ be<s32> mWarpOrder;  /* was m2C8: CreateInit 024D4D10 clears it; normal_execute 024D5354 sets 1 when check_warp sees the ship within 500; eventOrder 024D5708 orders TO_HYRULE_WARP while it is 1; checkOrder 024D5468 clears it when the event starts and runs normal_execute only while it is 0 (tww the same) */
    /*0x3E8*/ be<u8> mVisible;  /* was m2CC: CreateInit (event bit 0x2D08) / normal_execute 024D5354 / demo_execute 024D57E4 set 1; actDead 024D5B84 clears it when the bck ends; _draw 024D507C draws nothing while it is 0; _execute 024D58FC plays the warp sound while it is set (tww the same) */
    /*0x3E9*/ u8 m2CD[5];
    /*0x3EE*/ be<s16> mEventIdx;
    /*0x3F0*/ u8 m2D4[2];
    /*0x3F2*/ be<u8> m2D6;
    u8 pad3F3;
    /*0x3F4*/ be<u32> mDemoShapeId;  /* was m2D8: demo_execute 024D57E4: the demo actor's mShapeId; 0 hides the model, 1 starts the 0x82BD effect (tww switches on it) */
    /*0x3F8*/ be<s32> mStaffIdx;
    /*0x3FC*/ cXyz mEffectPos;  /* was m2E0: CreateInit 024D4D10 / _execute: current.pos at the sea height; set_effect 024D513C places the ripple particle there (tww the same) */
    /*0x408*/ cXyz mEffectScale;  /* was m2EC: CreateInit 024D4D10 sets (1, 1, 1); set_effect 024D513C passes it as the particle scale; actDead 024D5B84 chases it to 0 and copies it into the emitter's particle and dynamics scales (tww the same) */
    /*0x414*/ be<u8> mSenkoSet;  /* was m2F8: set_wpsenko 024D5768: the 0x82BC particle is set only once; set to 1 after it (tww the same) */
    u8 pad415[3];
    // HD: return widths and pointer arguments retain their verified register signatures.
    BOOL _delete();
    BOOL CreateHeap();
    void CreateInit();
    s32 _create();
    BOOL _execute();
    void normal_execute();
    void demo_execute();
    void demo_proc();
    void initWait(s32);
    BOOL actWait(s32);
    void initWait2(s32);
    BOOL actWait2(s32);
    void initWarp(s32);
    BOOL actWarp(s32);
    void initWaitDead(s32 action);
    BOOL actWaitDead(s32 action);
    void initDead(s32);
    BOOL actDead(s32 action);
    u32 initReturnWait(s32);
    BOOL actReturnWait(s32);
    void eventOrder();
    void checkOrder();
    void animPlay();
    void setEndAnm();
    f32 getSeaY(cXyz* position);
    BOOL set_effect(u32 effect);
    void set_wpsenko();
    void init_bck_anm(s32 index);
    BOOL check_warp();
    BOOL _draw();
};
WWHD_SIZE(daWarpdm20_c, 0x418);
WWHD_OFFSET(daWarpdm20_c, mRippleCallback, 0x3C8);
WWHD_OFFSET(daWarpdm20_c, mEventIdx, 0x3EE);
WWHD_OFFSET(daWarpdm20_c, mEffectScale, 0x408);
WWHD_OFFSET(warpdm20::Emitter, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(warpdm20::Emitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(warpdm20::Emitter, mStatus, 0x254);
s32 warpSeaArea(f32, f32);
f32 warpSeaWave(f32, f32);
f32 warpWaterHeight(const cXyz*);
void warpRippleRemove(warpdm20::RippleCallback*);
void warpResDelete(request_of_phase_process_class*, const char*);
s32 warpResLoad(request_of_phase_process_class*, const char*);
void warpActorConstruct(fopAc_ac_c*);
void warpRippleConstruct(warpdm20::RippleCallback*);
s32 warpEntryHeap(fopAc_ac_c*, u32, u32);
void warpOrderEvent(fopAc_ac_c*, s32, u32, u32, u32, u32);
f32 warpSqrt(f32);
s32 warpEventStart(dEvent_manager_c*, s32);
s32 warpEventEnd(dEvent_manager_c*, s32);
void warpDeleteActor(fopAc_ac_c*);
void warpParticle(dPa_control_c*, u32, u32, const cXyz*, const csXyz*, const cXyz*, u32,
                  warpdm20::RippleCallback*, s32, const GXColor*, const GXColor*, const cXyz*);
void* warpObjectResource(dRes_control_c*, Warpdm20ResourceKey*, s32);
s32 warpInitBck(mDoExt_baseAnm*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32);
void warpSetGoal(dEvent_manager_c*, const cXyz*);
u32 warpSystemSound(u32);
s32 warpIsEventBit(dSv_event_c*, u32);
void warpOnEventBit(dSv_event_c*, u32);
void warpHdAwakened(u32);
dDemo_actor_c* warpDemoActor(warpdm20::DemoObjects*, u32);
void warpOffDraw(void*);
u32 warpDrawPriority(fopAc_ac_c*);
void warpOnDraw(void*, u32);
s32 warpReverb(s32);
void warpLevelSound(u32, const cXyz*, u32, s32);
void warpCullBox(fopAc_ac_c*, f32, f32, f32, f32, f32, f32);
s32 warpEventIdx(dEvent_manager_c*, const char*, u32);
s32 warpStaffId(dEvent_manager_c*, const char*, u32, u32);
s32 warpActionIdx(dEvent_manager_c*, s32, warpdm20::ActionTable*, s32, u32, u32);
void warpCutEnd(dEvent_manager_c*, s32);
s32 warpAdvance(dEvent_manager_c*, s32);
mDoExt_baseAnm* warpCreateBtk(void*);
mDoExt_baseAnm* warpCreateBrk(void*);
s32 warpInitBtk(mDoExt_baseAnm*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32, s32);
s32 warpInitBrk(mDoExt_baseAnm*, J3DModelData*, void*, u32, s32, f32, s32, s32, u32, s32);
void warpFrameConstruct(mDoExt_baseAnm*, u32);
void warpMtxCalcConstruct(warpdm20::MtxCalc*);
void warpActorDestroy(fopAc_ac_c*, u32);
void warpEntryBtk(mDoExt_baseAnm*, J3DModelData*, f32);
void warpEntryBrk(mDoExt_baseAnm*, J3DModelData*, f32);
void warpEntryBck(mDoExt_baseAnm*, J3DModelData*, f32);
void warpChasePosition(cXyz*, const cXyz*, f32);
