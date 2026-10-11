/* d_npc (GameCube names): layouts and inline members in d/d_npc.h (fopNpc_npc_c, dNpc_JntCtrl_c,
 * dNpc_EventCut_c, dNpc_playerEyePos). */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_msg.h"
#include "gc_api/d/d_bg_s.h"
#include "gc_api/f_op/f_op_actor_mng.h"

/* ~fopNpc_npc_c (HD inline in the derived destructors): its members in reverse order, then
 * ~fopAc_ac_c [v nz] */
inline void __dt(fopNpc_npc_c* p) {
    __dt(&p->mCyl);
    __dt(&p->mStts);
    __dt(&p->mAcchCir);
    __dt(&p->mObjAcch);
    __dt(static_cast<fopAc_ac_c*>(p));
}

/* d_npc free functions (GameCube names) [g; raw units call the same addresses] */
inline BOOL dNpc_setAnmFNDirect(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, const char* anm, const char* sound, const char* arc) {
    return gabi::call<BOOL>(0x0259D344, morf, loopMode, morfF, speed, anm, sound, arc);
}
inline BOOL dNpc_setAnm(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D454, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}
inline BOOL dNpc_setAnm_2(mDoExt_McaMorf* morf, s32 loopMode, f32 morfF, f32 speed, s32 anmIdx, s32 soundIdx, const char* arc) {
    return gabi::call<BOOL>(0x0259D79C, morf, loopMode, morfF, speed, anmIdx, soundIdx, arc);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz, f32*, s16*): the cXyz by value are addresses [g] */
inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* angY) { gabi::call(0x0259D624, a, b, dist, angY); }
inline BOOL dNpc_chkArasoi() { return gabi::call<BOOL>(0x0259D6C8); }
inline BOOL dNpc_chkLetterPassed() { return gabi::call<BOOL>(0x0259D734); }
/* 0259D8BC dNpc_chkAttn(actor, cXyz destPos (address), f32 range, f32 extension, f32 angle, bool extended):
 * GameCube order; the bool goes in r5, the floats in f1..f3 [g] */
inline BOOL dNpc_chkAttn(fopAc_ac_c* a, cXyz* destPos, f32 range, f32 extension, f32 angle, bool extended) {
    return gabi::call<BOOL>(0x0259D8BC, a, destPos, range, extension, angle, extended);
}
