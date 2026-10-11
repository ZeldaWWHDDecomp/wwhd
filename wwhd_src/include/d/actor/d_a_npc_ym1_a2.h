#pragma once
#include "d/actor/d_a_npc_ym1.h"
#include "gc_api/f_op/f_op_actor.h"
// HD: private views retain shared-header compatibility and the verified GHS layouts.
struct Ym1SceneOffset { be<f32> x; be<f32> z; };
struct Ym1JointParam {
    be<s16> mMaxHeadX;
    be<s16> mMaxHeadY;
    be<s16> mMinHeadX;
    be<s16> mMinHeadY;
    be<s16> mMaxBackboneX;
    be<s16> mMaxBackboneY;
    be<s16> mMinBackboneX;
    be<s16> mMinBackboneY;
    be<s16> mTurnStep;
    be<s16> mTurnSpeed;
    u8 padding[0x2C-0x14];
};
WWHD_SIZE(Ym1JointParam, 0x2C);
struct Ym1DemoActor { void* getP_BtpData(const char*); };
struct Ym1DemoObject { Ym1DemoActor* getActor(u8); };
struct Ym1MassManager { u8 opaque; };
struct Ym1CollisionWorld { u8 padding[0x2854]; Ym1MassManager mMass; };
inline Ym1MassManager* ym1MassManager() { return &reinterpret_cast<Ym1CollisionWorld*>(dComIfG_Ccsp())->mMass; }
inline mDoExt_btpAnm* ym1Btp(daNpc_Ym1_c* a) { return reinterpret_cast<mDoExt_btpAnm*>(a->mBtpAnm); }
struct Ym1PositionWords { be<u32> x; be<u32> y; be<u32> z; };
// HD: the initial pose uses word copies, preserving float payloads.
inline Ym1PositionWords& ym1PositionWords(daNpc_Ym1_c* a) { return *reinterpret_cast<Ym1PositionWords*>(&a->current.pos); }
struct Ym1AngleView { u8 padding[0x9B6]; csXyz m846; };
inline Ym1AngleView& ym1Angles(daNpc_Ym1_c* a) { return *reinterpret_cast<Ym1AngleView*>(a); }
// HD: preserve the signed reads of state bytes whose GC names are known.
struct Ym1StateView {
    u8 padding[0xA0D];
    be<u8> m89D;
    be<u8> m89E;
    be<u8> m89F;
    be<u8> m8A0;
    u8 paddingA11[3];
    be<s8> m8A4;
    u8 paddingA15[6];
    be<s8> m8AB;
    u8 paddingA1C[3];
    be<s8> m8AF;
};
WWHD_OFFSET(Ym1StateView, m8AF, 0xA1F);
inline Ym1StateView& ym1State(daNpc_Ym1_c* a) { return *reinterpret_cast<Ym1StateView*>(a); }
inline gabi::ptmf<BOOL (daNpc_Ym1_c::*)(void*)>& ym1Action(daNpc_Ym1_c* a) {
    return *reinterpret_cast<gabi::ptmf<BOOL (daNpc_Ym1_c::*)(void*)>*>(&a->mCurrProcFunc);
}
enum { YM1_VTBL=0x100236A0, YM1_AAB_VTBL=0x10023460,
    Ym1_CheckCreateHeap=0x022F72AC, Ym1_area_check=0x022F68B8,
    Ym1_DEMO_ENABLE=0x6A, Ym1_PARTICLE_GRASS=0x3DA, Ym1_SE_GRASS_CUT=0x5812 };
// HD: u32 results and u8 head-only argument preserve the original full registers.
u32 Ym1_init_YM1_0(daNpc_Ym1_c*);
u32 Ym1_init_YM1_1(daNpc_Ym1_c*);
u32 Ym1_init_YM2_0(daNpc_Ym1_c*);
u32 Ym1_init_YM2_1(daNpc_Ym1_c*);
u32 Ym1_init_YM2_2(daNpc_Ym1_c*);
u32 Ym1_init_YM2_3(daNpc_Ym1_c*);
u32 Ym1_init_YMx_error(daNpc_Ym1_c*);
void Ym1_setMass(Ym1MassManager*, dCcD_Cyl*, u8);
void Ym1_play_animation(daNpc_Ym1_c*);
void Ym1_setMtx(daNpc_Ym1_c*, u32);
void Ym1_npcCtor(daNpc_Ym1_c*);
u32 Ym1_decideType(daNpc_Ym1_c*, u32);
u32 Ym1_chk_nbt_attn(daNpc_Ym1_c*);
void Ym1_setDemoData(daNpc_Ym1_c*, u32, mDoExt_McaMorf*, const char*, u32, u32, u32, u32);
void Ym1_lookAtTarget2(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
void Ym1_setMassArea(Ym1MassManager*, dCcD_Cyl*, u8, u32);

struct Ym1EventStaffView { gptr<const char> mpEvtStaffName; };
inline const char* ym1EventStaffName(daNpc_Ym1_c* a) {
    return reinterpret_cast<Ym1EventStaffView*>(&a->mEventCut)->mpEvtStaffName.get();
}
