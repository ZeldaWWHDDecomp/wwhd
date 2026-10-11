/* Barrel actor: GameCube -> WWHD fields +0x11C, actor size 0x750 (GC 0x634).
 * tww member names, including its unknown numbered members, are retained. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"
namespace daObjBarrel {
struct Act_c : fopAc_ac_c {
    enum Prm_e { PRM_CULL_W = 3, PRM_CULL_S = 28 };
    enum Mode { MODE_WAIT, MODE_CARRY, MODE_VIB0, MODE_VIB1, MODE_VIB2, MODE_JUMP, MODE_WALK };
    // HD: bool return widths and damaged(s32) follow the verified original.
    bool create_heap();
    void cull_set_draw();
    void cull_set_move();
    void set_mtx();
    void init_mtx();
    void mode_wait_init();
    bool _delete();
    u32 get_se_map_hit();
    void set_senv(s32 p1, s32 p2);
    void mode_jump_init();
    void mode_walk_init();
    bool chk_sink_water();
    bool chk_sinkdown_water();
    void eff_hit_water_splash();
    void mode_vib0_init();
    void eff_land_smoke();
    void mode_vib1_init();
    void mode_vib2_init();
    void eff_break();
    void damaged(s32 animate);
    void se_fall_water();
    bool damage_bg_proc();
    bool damage_bg_proc_directly();
    bool _draw();
    void mode_carry();
    void vib_pos_ang();
    void mode_vib0();
    void mode_vib1();
    void mode_vib2();
    void set_walk_rot();
    void mode_wait();
    void mode_jump();
    void mode_walk();
    void mode_carry_init();
    bool mode_proc_call();
    s32 _create();
    bool damage_cc_proc();
    bool _execute();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dBgS_ObjAcch mAcch;
    /* 0x57C */ dBgS_AcchCir mAcchCir;
    /* 0x5BC */ dCcD_Stts mStts;
    /* 0x5F8 */ dCcD_Cyl mCyl;
    /* 0x728 */ be<s32> mMode;
    /* 0x72C */ be<s16> m610;
    /* 0x72E */ be<s16> m612;
    /* 0x730 */ be<s32> mTimer;
    /* 0x734 */ be<f32> mLastGroundY;
    /* 0x738 */ be<s16> m61C;
    u8 _73A[2];
    /* 0x73C */ be<u8> mOnGround;
    /* 0x73D */ be<s8> mInitTimer;
    /* 0x73E */ be<u8> mForceExec;
    /* 0x73F */ be<u8> mSunk;
    /* 0x740 */ cXyz mMove;
    /* 0x74C */ be<s16> m630;
    u8 _74E[2];
};
} // namespace daObjBarrel
WWHD_OFFSET(daObjBarrel::Act_c, mMode, 0x728);
WWHD_OFFSET(daObjBarrel::Act_c, mMove, 0x740);
WWHD_SIZE(daObjBarrel::Act_c, 0x750);

// HD: full-register return types preserve the original caller ABI.
struct BarrelEmitter;
J3DModelData* Barrel_getRes(void*, void*, s32);
void Barrel_assert(const char*, s32, const char*);
void Barrel_setCullSphere(fopAc_ac_c*, f32, f32, f32, f32);
void Barrel_transS(Mtx34*, f32, f32, f32);
void Barrel_transM(f32, f32, f32);
void Barrel_ZXYrotM(Mtx34*, s32, s16, s16);
void Barrel_YrotM(Mtx34*, s16);
void Barrel_XrotM(Mtx34*, s16);
void Barrel_Sound_set(cXyz*, s32, u32, s32);
void Barrel_make_land_effect(fopAc_ac_c*, dBgS_GndChk*, f32);
void Barrel_operator_delete(void*);
void Barrel_Cyl_dt(dCcD_Cyl*, s32);
void Barrel_Stts_dt(dCcD_Stts*, s32);
void Barrel_Acch_dt(dBgS_Acch*, s32);
void Barrel_actor_dt(fopAc_ac_c*, s32);
BarrelEmitter* Barrel_particle_set(dPa_control_c*, s32, s32, cXyz*, csXyz*, cXyz*, s32, void*, s32,
                                   GXColor*, GXColor*, s32);
void Barrel_createSmoke(s32, s32, cXyz*, s32, csXyz*, cXyz*, s32, void*);
void Barrel_cancelCarryNow(fopAc_ac_c*);
s32 Barrel_getReverb(s8);
void Barrel_seStart(u32, cXyz*, u32, s32);
u32 Barrel_GetMtrlSndId(dBgS*, cBgS_PolyInfo*);
void Barrel_modelUpdateDL(J3DModel*, s32);
void Barrel_chaseAngleS(be<s16>*, s32, s32);
void Barrel_SetCurrentRoomNo(fopAc_ac_c*, dBgS_GndChk*);
void Barrel_posMoveF(fopAc_ac_c*, cXyz*);
void Barrel_xyzSubtract(cXyz*, cXyz*, cXyz*);
f32 Barrel_xyzAbs2(cXyz*);
f32 Barrel_sqrtf(f32);
s32 Barrel_atan2s(f32, f32);
cXyz* Barrel_GetTriNormal(dBgS*, u16, u16);
void Barrel_posMoveF_stream(fopAc_ac_c*, cXyz*, cXyz*, f32, f32);
void Barrel_posMoveF_grade(fopAc_ac_c*, cXyz*, cXyz*, f32, f32, cXyz*, f32, f32, cXyz*);
void Barrel_onEventBit(void*, s32);
s32 Barrel_ChkMoveBG(dBgS*, cBgS_PolyInfo*);
u32 Barrel_GetPolyColor(dBgS*, cBgS_PolyInfo*);
void Barrel_AcchCir_ct(dBgS_AcchCir*);
s32 Barrel_resLoad(request_of_phase_process_class*, const char*);
s32 Barrel_entrySolidHeap(fopAc_ac_c*, u32, u32);
void Barrel_xyzScale(cXyz*, cXyz*, f32);
fopAc_ac_c* Barrel_GetAc(dCcD_GObjTg*);
void Barrel_xyzMultiply(cXyz*, cXyz*, f32);
void Barrel_xyzAdd(cXyz*, cXyz*, cXyz*);
void Barrel_HitSeStart(cXyz*, s8, dCcD_Cyl*, u32);
void Barrel_HitEff_kikuzu(fopAc_ac_c*, dCcD_Cyl*);
s32 Barrel_cullingCheck(fopAc_ac_c*);
void Barrel_Cyl_SetC(cM3dGCyl*, cXyz*);
void Barrel_Cc_Set(cCcS*, dCcD_Cyl*);

// HD: private views for inlined virtual dispatch, without altering shared layouts.
struct BarrelEmitter {
    u8 _pad[8];
    cXyz mScale;
};
WWHD_OFFSET(BarrelEmitter, mScale, 8);
WWHD_SIZE(BarrelEmitter, 0x14);
struct BarrelStringVtable {
    u8 _pad[0x14];
    gfn mAssureTermination;
};
WWHD_OFFSET(BarrelStringVtable, mAssureTermination, 0x14);
WWHD_SIZE(BarrelStringVtable, 0x18);
inline void Barrel_assureTermination(u32 function, void* string) {
    gabi::call_ptr(function, string);
}
struct BarrelStageInfo {
    u8 _pad[0x12];
    be<u16> mCullPoint;
};
WWHD_OFFSET(BarrelStageInfo, mCullPoint, 0x12);
WWHD_SIZE(BarrelStageInfo, 0x14);
struct BarrelStageVtable {
    u8 _pad[0x15C];
    gfn mGetStagInfo;
};
WWHD_OFFSET(BarrelStageVtable, mGetStagInfo, 0x15C);
WWHD_SIZE(BarrelStageVtable, 0x160);
struct BarrelStageDt {
    gptr<BarrelStageVtable> mpVtable;
    BarrelStageInfo* getStagInfo() {
        return gabi::call_ptr<BarrelStageInfo*>(mpVtable->mGetStagInfo.get(), this);
    }
};
WWHD_SIZE(BarrelStageDt, 4);
struct BarrelShapeVtable {
    u8 _pad[0x9C];
    gfn mGetNVec;
};
WWHD_OFFSET(BarrelShapeVtable, mGetNVec, 0x9C);
WWHD_SIZE(BarrelShapeVtable, 0xA0);
struct BarrelShape {
    u8 _pad[0x1C];
    gptr<BarrelShapeVtable> mpVtable;
    s32 getNVec(cXyz* position, cXyz* normal) {
        return gabi::call_ptr<s32>(mpVtable->mGetNVec.get(), this, position, normal);
    }
};
WWHD_OFFSET(BarrelShape, mpVtable, 0x1C);
WWHD_SIZE(BarrelShape, 0x20);
struct BarrelHitVtable {
    u8 _pad[0x2C];
    gfn mGetShapeAttr;
};
WWHD_OFFSET(BarrelHitVtable, mGetShapeAttr, 0x2C);
WWHD_SIZE(BarrelHitVtable, 0x30);
inline BarrelShape* Barrel_getShapeAttr(dCcD_GObjInf* hit) {
    return gabi::call_ptr<BarrelShape*>(
        gabi::at<BarrelHitVtable>(hit->__vtbl_hitinf)->mGetShapeAttr.get(), hit);
}
struct BarrelModeTable {
    gabi::ptmf<void (daObjBarrel::Act_c::*)()> mProc[7];
};
WWHD_SIZE(BarrelModeTable, 0x38);
using BarrelResPointer = gptr<void>;
using BarrelWord = be<u32>;
WWHD_OFFSET(daObjBarrel::Act_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjBarrel::Act_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjBarrel::Act_c, mAcch, 0x3B8);
WWHD_OFFSET(daObjBarrel::Act_c, mAcchCir, 0x57C);
WWHD_OFFSET(daObjBarrel::Act_c, mStts, 0x5BC);
WWHD_OFFSET(daObjBarrel::Act_c, mCyl, 0x5F8);
WWHD_OFFSET(daObjBarrel::Act_c, m610, 0x72C);
WWHD_OFFSET(daObjBarrel::Act_c, m612, 0x72E);
WWHD_OFFSET(daObjBarrel::Act_c, mTimer, 0x730);
WWHD_OFFSET(daObjBarrel::Act_c, mLastGroundY, 0x734);
WWHD_OFFSET(daObjBarrel::Act_c, m61C, 0x738);
WWHD_OFFSET(daObjBarrel::Act_c, mOnGround, 0x73C);
WWHD_OFFSET(daObjBarrel::Act_c, mInitTimer, 0x73D);
WWHD_OFFSET(daObjBarrel::Act_c, mForceExec, 0x73E);
WWHD_OFFSET(daObjBarrel::Act_c, mSunk, 0x73F);
WWHD_OFFSET(daObjBarrel::Act_c, m630, 0x74C);

enum { BARREL_ACTOR_VTABLE = 0x100255B0 };
void Barrel_AcchCirShape_dt(void*, s32);

inline dComIfG_inf_c* Barrel_gameInfo(u8* play) { return gabi::at<dComIfG_inf_c>(gabi::ea(play)); }
enum {
    BARREL_AAB_VTABLE = 0x10025570,
    dRes_INDEX_KTARU_01_BDL_KTARU_01_e = 3,
    BARREL_AT_UNKNOWN8 = 0x8,
    BARREL_AT_BOMB = 0x20,
    BARREL_AT_SKULL_HAMMER = 0x10000,
    BARREL_AT_WIND = 0x200000,
    BARREL_PARTICLE_BREAK = 0x3E5,
    BARREL_SMOKE_PROCESS = 0x1D3,
    BARREL_SE_BREAK = 0x6805,
    BARREL_SE_WATER = 0x6919,
    BARREL_SE_TAKE_OFF = 0x282A,
};
