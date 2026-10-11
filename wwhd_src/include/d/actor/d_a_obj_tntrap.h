/** HD layout: the tww actor member block is opaque; measured fields use descriptive names. */
#pragma once
#include "f_op/f_op_actor.h"

#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "JSystem/JParticle/JPA_emitter_calc.h"
#include "SSystem/SComponent/c_m3d_g_tri.h"

using TnTrapEmitter = jpa_emitter_calc::JPABaseEmitter;
class daObjTnTrap_c;
using TnTrapProc = BOOL (daObjTnTrap_c::*)();
using TnTrapInit = void (daObjTnTrap_c::*)();
struct TnTrapProcEntry {
    gabi::ptmf<TnTrapProc> fn;
};
struct TnTrapInitEntry {
    gabi::ptmf<TnTrapInit> fn;
};
WWHD_SIZE(TnTrapProcEntry, 8);
WWHD_SIZE(TnTrapInitEntry, 8);
// HD: copy the descriptor's packed adjustment/selector word before the action index store.
template <class F> inline void tntrapCopyPtmfHead(gabi::ptmf<F>& dst, const gabi::ptmf<F>& src) {
    *reinterpret_cast<be<u32>*>(&dst) = *reinterpret_cast<const be<u32>*>(&src);
}
struct TnTrapShapeAttr {
    u8 mBounds[0x18];
    be<u32> mBoundsVtable, mShapeVtable;
};
WWHD_OFFSET(TnTrapShapeAttr, mBoundsVtable, 0x18);
WWHD_SIZE(TnTrapShapeAttr, 0x20);
struct TnTrapTriangle : dCcD_GObjInf {
    TnTrapShapeAttr mShape;
    cM3dGTri mTri;
    void Set(const dCcD_SrcTri* source);
};
WWHD_OFFSET(TnTrapTriangle, mTri, 0x118);
WWHD_SIZE(TnTrapTriangle, 0x150);
// HD: ship access uses the second player-info pointer, rather than mpPlayerPtr[2].
struct TnTrapShip : fopAc_ac_c {
    u8 mUnused[0x644 - sizeof(fopAc_ac_c)];
    be<u32> mStateFlag;
    enum { daSFLG_UNK800000_e = 0x00800000 };
};
WWHD_OFFSET(TnTrapShip, mStateFlag, 0x644);
struct TnTrapMathStatics {
    be<f32> mMinAngle, mMaxAngle;
    u8 mSinTable, mAtanTable, mPadding[2];
    be<u32> mZero[4];
};
WWHD_SIZE(TnTrapMathStatics, 0x1C);
struct TnTrapDescriptors {
    u8 mEntries[3][12];
};
WWHD_SIZE(TnTrapDescriptors, 36);
struct TnTrapTriSource {
    u8 mBytes[0x54];
};
enum TnTrapAction {
    ACT_TRAP_OFF_WAIT,
    ACT_TRAP_ON_WAIT,
    ACT_DEMO_REGIST_WAIT,
    ACT_DEMO_WAIT,
    ACT_DEMO_WAIT2,
    ACT_DEMO_END_WAIT,
    ACT_HIDE_WAIT,
};
enum TnTrapType { TYPE_SWITCH, TYPE_EVENT, TYPE_EVENT2, TYPE_SHIP, TYPE_UNUSED4, TYPE_VERTICAL };
enum TnTrapVtables : u32 {
    VT_ACTOR = 0x10031094,
    VT_STATUS = 0x1004AE88,
    VT_GLOBAL_STATUS = 0x1004AEC0,
    VT_SHAPE = 0x100015A8,
    VT_BOUNDS = 0x10031084,
    VT_TRI_HIT = 0x1004B010,
    VT_TRI_PLANE = 0x1004B058,
    VT_TRI_SHAPE = 0x1004B068,
    VT_ARCHIVE = 0x1003106C,
    FN_TRI_DESTRUCTOR = 0x025159F8, // HD: array-runtime destructor callback address.
};

class daObjTnTrap_c : public fopAc_ac_c {
  public:
    BOOL chk_appear();
    BOOL create_heap();
    void set_em_set_offsetY();
    void set_mtx();
    void set_tri(s32 group);
    void setup_action(s32 action);
    s32 _create();
    void particle_delete(s32 group);
    BOOL _delete();
    void set_se();
    BOOL _execute();
    BOOL _draw();
    void particle_set(s32 group, f32 height);
    BOOL chk_event_flg();
    void trap_off_wait_act_init_proc();
    void trap_on_wait_act_init_proc();
    void demo_regist_wait_act_init_proc();
    void demo_wait2_act_init_proc();
    void demo_end_wait_act_init_proc();
    BOOL trap_off_wait_act_proc();
    BOOL trap_on_wait_act_proc();
    BOOL demo_regist_wait_act_proc();
    BOOL demo_wait_act_proc();
    BOOL demo_wait2_act_proc();
    BOOL demo_end_wait_act_proc();
    BOOL hide_wait_act_proc();
    void dummy_proc();
    u32 getPrm(u32 bits, u32 shift);
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<s32> mTrapType;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ TnTrapTriangle mTri[8];
    /* 0xE74 */ gptr<dBgW> mpBgW;
    /* 0xE78 */ Mtx34 mBgMtx;
    /* 0xEA8 */ gptr<TnTrapEmitter> mpEmitterA[2][2];
    /* 0xEB8 */ gptr<TnTrapEmitter> mpEmitterB[2][3];
    /* 0xED0 */ gabi::ptmf<TnTrapProc> mActionProc;
    /* 0xED8 */ be<s32> mAction;
    /* 0xEDC */ be<u8> mAppears;
    u8 reservedEDD[3];
    /* 0xEE0 */ be<u32> mSwSave;
    /* 0xEE4 */ be<u32> mSwSave2;
    /* 0xEE8 */ be<u32> mArg0;
    /* 0xEEC */ be<u32> mMapType;
    /* 0xEF0 */ be<s16> mEventIdx;
    u8 reservedEF2[6];
    /* 0xEF8 */ be<u8> mParticleActive[2];
    u8 reservedEFA[2];
    /* 0xEFC */ be<f32> mEmSetOffsetY[2];
};
static_assert(sizeof(daObjTnTrap_c) == 0xF04);
WWHD_OFFSET(daObjTnTrap_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjTnTrap_c, mTrapType, 0x3B4);
WWHD_OFFSET(daObjTnTrap_c, mStts, 0x3B8);
WWHD_OFFSET(daObjTnTrap_c, mTri, 0x3F4);
WWHD_OFFSET(daObjTnTrap_c, mpBgW, 0xE74);
WWHD_OFFSET(daObjTnTrap_c, mBgMtx, 0xE78);
WWHD_OFFSET(daObjTnTrap_c, mpEmitterA, 0xEA8);
WWHD_OFFSET(daObjTnTrap_c, mpEmitterB, 0xEB8);
WWHD_OFFSET(daObjTnTrap_c, mActionProc, 0xED0);
WWHD_OFFSET(daObjTnTrap_c, mAction, 0xED8);
WWHD_OFFSET(daObjTnTrap_c, mAppears, 0xEDC);
WWHD_OFFSET(daObjTnTrap_c, mSwSave, 0xEE0);
WWHD_OFFSET(daObjTnTrap_c, mSwSave2, 0xEE4);
WWHD_OFFSET(daObjTnTrap_c, mArg0, 0xEE8);
WWHD_OFFSET(daObjTnTrap_c, mMapType, 0xEEC);
WWHD_OFFSET(daObjTnTrap_c, mEventIdx, 0xEF0);
WWHD_OFFSET(daObjTnTrap_c, mParticleActive, 0xEF8);
WWHD_OFFSET(daObjTnTrap_c, mEmSetOffsetY, 0xEFC);

// HD: full-FPR and full event-index return registers are used by these call chains.
f64 tntrapSquareRegister(const cXyz*);
f64 tntrapSqrtRegister(f64);
s32 tntrapEventIndexRegister(dEvent_manager_c*, const char*, u32);
void* tntrapEventData(dEvent_manager_c*, s32);
const char* tntrapCutName(dEvent_manager_c*, s32);
void tntrapConstructArray(void*, s32, u32, u32);
void tntrapDestroyArray(void*, s32, u32, u32, u32, u32);
void tntrapGlobalStatusConstruct(be<u32>*);
void tntrapStatusConstruct(dCcD_Stts*);
void tntrapTriangleConstruct(dCcD_GObjInf*);
void tntrapGeometryConstruct(cM3dGTri*);
dBgW* dBgW_NewSet(cBgD_t*, u32, Mtx34*);
void tntrapInitSinTable(u8*);
void tntrapInitAtanTable(u8*);
void tntrapActorDestruct(fopAc_ac_c*, s32);
s32 tntrapStaffId(dEvent_manager_c*, const char*, fopAc_ac_c*, s32);
BOOL tntrapEventEnd(dEvent_manager_c*, s32);
void tntrapOrderEventRegister(fopAc_ac_c*, s32, u32, u32, u32, u32);

void mDoMtx_XYZrotM(Mtx34*, s32, s32, s32);
void mDoAud_seStartCom(u32);

enum TnTrapMapType { MAP_EVENT, MAP_SHIP, MAP_VERTICAL };
enum TnTrapResources { TNTRAP_DZB = 3 };
enum TnTrapParticles { PARTICLE_SIDE = 0x82EA, PARTICLE_CENTER = 0x82EB };
enum {
    dSv_event_flag_UNK_3A04 = 0x3A04,
    dSv_event_flag_UNK_2C01 = 0x2C01,
    dSv_event_flag_UNK_3B40 = 0x3B40,
};
