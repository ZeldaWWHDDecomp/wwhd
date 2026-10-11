// Pot modes, tensor updates and fragments. Non-owning extension of the shared pot layout.
// HD: members keep tww names with the actor delta +0x11C.
#pragma once
#include "d/actor/d_a_tsubo.h"
namespace daTsuboMode {
enum EffectID {
    JA_SE_OBJ_BOKKURI_RECOVER = 0x69F4,
    JA_SE_OBJ_BOKKURI_SWELL = 0x69F5,
    JA_SE_LK_CONVEY_WPOT = 0x283F,
    ID_IT_JN_TR_HAHEN_A = 0x3E5,
    ID_AK_JN_M_TUBOHAHEN = 0x17,
    ID_AK_JN_TUBOKONAGONA = 0x18,
    ID_IT_JN_TR_HAHEN_C = 0x3E7,
    ID_IT_JN_DOKURO00 = 0x3E8,
    ID_IT_SN_BOKKURI_MATSU00 = 0x816A,
    ID_AK_SN_KUTANIVESSEL = 0x814B,
    dRes_INDEX_ALWAYS_BDL_MPM_TUBO_e = 0x31,
    dRes_INDEX_ALWAYS_BTP_MPM_TUBO_e = 0x67,
    PROC_Obj_Eff = 0x1D3,
};
struct Act_c : daTsubo::Act_c {
    void moment_small(const cXyz* i_axis);
    void moment_big(const cXyz* i_axis);
    void moment_water(const cXyz* i_axis);
    void moment_barrel(const cXyz* i_axis);
    void moment_stool(const cXyz* i_axis);
    void moment_skull(const cXyz* i_axis);
    void moment_pail(const cXyz* i_axis);
    void moment_spine(const cXyz* i_axis);
    void moment_hbox2S(const cXyz* i_axis);
    void moment_tryColSun(const cXyz* i_axis);
    void moment_tryColMer(const cXyz* i_axis);
    void moment_tryColJup(const cXyz* i_axis);
    void moment_tryKeyGate(const cXyz* i_axis);
    void moment_pinecone(const cXyz* i_axis);
    void moment_kutani(const cXyz* i_axis);
    void moment_woodS(const cXyz* i_axis);
    void set_tensor_wait();
    void set_tensor_hide();
    void set_tensor_appear();
    void set_tensor_carry();
    void set_tensor_walk();
    void set_tensor_drop();
    void set_tensor_sink();
    void set_tensor_afl();
    void init_rot_throw();
    void mode_hide();
    void water_tention();
    void mode_sink();
    void calc_drop_param(be<f32>* outArg1, be<f32>* outArg2, be<f32>* outArg3);
    void calc_afl_param(be<f32>* outArg1, be<f32>* outArg2, be<f32>* outArg3);
    void mode_appear();
    void mode_wait();
    void mode_walk(u32 r4, u32 r5, u32 r6, u32 r7, u32 harness_r8);
    void mode_drop_init();
    void mode_drop();
    void se_pickup_carry();
    void spec_mode_carry(bool arg1);
    void spec_mode_put_init();
    void mode_carry(u32 r4, u32 harness_r5);
    void mode_afl();
    void moment_proc_call();
    void spec_set_room();
    void eff_kutani_set();
    bool mode_proc_call();
    bool _execute();
    bool _draw();
    void eff_break_barrel();
    void eff_break_tsubo();
    void eff_break_stool();
    void eff_break_skull();
    void eff_break_pinecone();
    void eff_break_pail();
};
WWHD_SIZE(Act_c, 0x93C);
WWHD_OFFSET(Act_c, mAcch, 0x3BC);
WWHD_OFFSET(Act_c, mCyl, 0x624);
WWHD_OFFSET(Act_c, mType, 0x790);
WWHD_OFFSET(Act_c, m814, 0x930);
struct ModeTable { gabi::ptmf<void (Act_c::*)()> mProc[8]; };
WWHD_SIZE(ModeTable, 0x40);
struct MomentTable { gabi::ptmf<void (Act_c::*)(const cXyz*)> mProc[16]; };
WWHD_SIZE(MomentTable, 0x80);
struct PlayerVtable { u8 mPadding[0xBC]; gfn getGrabActorID; };
WWHD_OFFSET(PlayerVtable, getGrabActorID, 0xBC);
WWHD_SIZE(PlayerVtable, 0xC0);
struct Player : fopAc_ac_c {
    u8 mPadding[0x14];
    be<u32> mStatus;
    u32 getGrabActorID() { return gabi::call_ptr<u32>(gabi::at<PlayerVtable>(__vtbl)->getGrabActorID.get(), this); }
};
WWHD_OFFSET(Player, mStatus, 0x3C0);
WWHD_SIZE(Player, 0x3C4);
struct AnimationVtable { u8 mPadding[0x14]; gfn getFrameMax; };
WWHD_OFFSET(AnimationVtable, getFrameMax, 0x14);
WWHD_SIZE(AnimationVtable, 0x18);
struct Animation {
    u8 mPadding[4]; gptr<AnimationVtable> mpVtable;
    s32 getFrameMax() { return gabi::call_ptr<s32>(mpVtable->getFrameMax.get(), this); }
};
WWHD_OFFSET(Animation, mpVtable, 4);
WWHD_SIZE(Animation, 8);
struct ModelAnimation { u8 mPadding[0x48]; be<u32> mAnmTevRegKey; };
WWHD_OFFSET(ModelAnimation, mAnmTevRegKey, 0x48);
WWHD_SIZE(ModelAnimation, 0x4C);
struct Emitter {
    u8 mPadding[8]; cXyz mEmitterScale;
    u8 mRest[0x70 - 0x14]; be<f32> mInitialVelDir;
};
WWHD_OFFSET(Emitter, mEmitterScale, 8);
WWHD_OFFSET(Emitter, mInitialVelDir, 0x70);
WWHD_SIZE(Emitter, 0x74);
struct MassPlay { u8 mPadding[0x4EF8]; u8 mMassManager; };
WWHD_OFFSET(MassPlay, mMassManager, 0x4EF8);
WWHD_SIZE(MassPlay, 0x4EF9);
struct TextureCallback { u8 mData[8]; };
WWHD_SIZE(TextureCallback, 8);
struct TextureCallbacks { TextureCallback mTsubo[4]; };
WWHD_SIZE(TextureCallbacks, 0x20);
struct ParticleLists { u8 mPadding[0x130]; be<u32> mModelEmitters; };
WWHD_OFFSET(ParticleLists, mModelEmitters, 0x130);
WWHD_SIZE(ParticleLists, 0x134);
}

BOOL tsuboMode_fopAcM_cullingCheck(fopAc_ac_c* a);
void tsuboMode_dCcD_Cyl_MoveCAtTg(dCcD_Cyl* c, cXyz* p);
u8 tsuboMode_dBgS_GetPolyColor(dBgS* bgs, cBgS_PolyInfo* p);
void tsuboMode_cull_set_move(daTsuboMode::Act_c* a);
void tsuboMode_cull_set_draw(daTsuboMode::Act_c* a);
void tsuboMode_spec_clr_actor(daTsuboMode::Act_c* a);
void tsuboMode_spec_carry_spec(daTsuboMode::Act_c* a);
void tsuboMode_spec_set_actor(daTsuboMode::Act_c* a);
void tsuboMode_set_mtx(daTsuboMode::Act_c* a);
bool tsuboMode_damage_kill_proc(daTsuboMode::Act_c* a);
bool tsuboMode_damage_bg_proc(daTsuboMode::Act_c* a);
bool tsuboMode_damage_cc_proc(daTsuboMode::Act_c* a);
void tsuboMode_bound(daTsuboMode::Act_c* a, f32 v);
bool tsuboMode_damage_bg_proc_directly(daTsuboMode::Act_c* a);
void tsuboMode_crr_pos(daTsuboMode::Act_c* a, cXyz* p);
void tsuboMode_mode_carry_init(daTsuboMode::Act_c* a);
void tsuboMode_se_pickup(daTsuboMode::Act_c* a);
void tsuboMode_eff_drop_water(daTsuboMode::Act_c* a);
void tsuboMode_fopAcM_setRoomLayer(fopAc_ac_c* a, s32 room);
cXyz* tsuboMode_daObj_get_path_spd(cBgS_PolyInfo* p, f32 v);
void tsuboMode_daObj_SetCurrentRoomNo_r5(fopAc_ac_c* a, void* gnd, u32 harness_r5);
void tsuboMode_daObj_posMoveF_grade(fopAc_ac_c* a, const cXyz* cc, const cXyz* stream, f32 k1, f32 k2, const void* nrm, f32 g, f32 cosg, const cXyz* mom, u32 harness_r8);
void tsuboMode_cLib_addCalc(be<f32>* v, f32 t, f32 s, f32 mx, f32 mn);
void tsuboMode_daObj_SetCurrentRoomNo(fopAc_ac_c* a, void* gnd);
be<s16>* tsuboMode_cSAngle_ct_s16(be<s16>* a, s16 v);
s16 tsuboMode_cM_rad2s(f32 r);
f32 tsuboMode_cSAngle_Radian(be<s16>* a);
bool tsuboMode_chk_sink_water(daTsuboMode::Act_c* a);
void tsuboMode_init_rot_clean(daTsuboMode::Act_c* a);
void tsuboMode_mode_wait_init(daTsuboMode::Act_c* a);
void tsuboMode_mode_appear_init(daTsuboMode::Act_c* a);
void tsuboMode_set_tensor(daTsuboMode::Act_c* a, const cXyz* v);
void* tsuboMode_modelEmitter(u32 self, JPABaseEmitter* e, J3DModelData* d, u32 hd, dKy_tevstr_c* tev, void* btp, u16 n, s32 i);
void tsuboMode_setMass(void* manager, void* object, u8 priority);
void tsuboMode_addModelEmitter(u32 list, void* emitter);
