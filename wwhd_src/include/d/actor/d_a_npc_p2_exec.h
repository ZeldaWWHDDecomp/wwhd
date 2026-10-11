#pragma once
#include "d/actor/d_a_npc_p2.h"
#include "gc_api/f_op/f_op_actor.h"

void P2_setNpcAnm(mDoExt_McaMorf*, u32, f32, f32, s32, s32, const char*);
void P2_setActorInfo(dNpc_EventCut_c*, const char*, fopAc_ac_c*);
// HD: keep head-only as u8, preserving the complete input byte rather than a bool.
void P2_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
s8 P2_getRoomId(dBgS*, cBgS_PolyInfo*);
u8 P2_getPolyColor(dBgS*, cBgS_PolyInfo*);
void P2_getPosFromMtx(Mtx34*, cXyz*);
void P2_setActorHang(fopAc_ac_c*, cXyz*, s32);
u32 P2_lockonTruth(dAttention_c*);
fopAc_ac_c* P2_actionTarget(dAttention_c*, s32);
fopAc_ac_c* P2_lockonTarget(dAttention_c*, s32);
enum { fpcNm_Obj_Pirateship_e = 0x39 };

// HD: SafeString calls assureTermination through this TU's private vtable copy.
struct P2SafeStringVtable { u8 _00[0x14]; be<u32> assureTermination; };
WWHD_OFFSET(P2SafeStringVtable, assureTermination, 0x14);
static P2SafeStringVtable* P2_safeVtable(u32 table) { return gabi::at<P2SafeStringVtable>(table); }
static const be<u8>* P2_stringBytes(u32 string) { return gabi::at<const be<u8>>(string); }
// Shared NPC cylinder storage: its owner is outside this split translation unit.
static dCcD_SrcCyl* P2_sharedCyl() { return gabi::at<dCcD_SrcCyl>(0x101EA190); }
static Mtx34* P2_jointMatrix(J3DMtxBuffer_hd* matrices, u32 joint) {
    return gabi::at<Mtx34>(matrices->mpAnmMtx + joint * sizeof(Mtx34));
}
// Pure word copies preserve signalling-NaN payloads and the original load/store order.
struct P2PositionWords { be<u32> x, y, z; };
WWHD_SIZE(P2PositionWords, sizeof(cXyz));
static void P2_copyPos(cXyz* dest, const cXyz* source) {
    auto& in = *reinterpret_cast<const P2PositionWords*>(source);
    auto& out = *reinterpret_cast<P2PositionWords*>(dest);
    u32 x = in.x, y = in.y, z = in.z;
    out.x = x; out.y = y; out.z = z;
}

// HD: the start stage name is embedded in the play object returned by the accessor.
struct P2StartStageView {
    u8 _00[0x5134];
    be<u8> mStartStageName[8];
};
WWHD_OFFSET(P2StartStageView, mStartStageName, 0x5134);
