/* Base/heap/draw bindings for the split P2 translation unit. Shared actor layout
 * remains in d_a_npc_p2.h; this file adds only private views used by this source. */
#pragma once
#include "d/actor/d_a_npc_p2.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/d/d_kankyo.h"

struct P2BasePatternVtable { u8 padding[0x14]; be<u32> getFrameMax; };
struct P2BasePattern { u8 padding[4]; gptr<P2BasePatternVtable> vtable; };
struct P2BaseSmokeVtable { u8 padding[0x44]; be<u32> remove; };
struct P2BaseSmoke { gptr<P2BaseSmokeVtable> vtable; };
struct P2BaseModelData { u8 padding[0x38]; be<u32> mTexPatternEntry; };
WWHD_SIZE(P2BasePatternVtable, 0x18);
WWHD_OFFSET(P2BasePatternVtable, getFrameMax, 0x14);
WWHD_SIZE(P2BasePattern, 8);
WWHD_OFFSET(P2BasePattern, vtable, 4);
WWHD_SIZE(P2BaseSmokeVtable, 0x48);
WWHD_OFFSET(P2BaseSmokeVtable, remove, 0x44);
WWHD_SIZE(P2BaseSmoke, 4);
WWHD_SIZE(P2BaseModelData, 0x3C);
WWHD_OFFSET(P2BaseModelData, mTexPatternEntry, 0x38);
static mDoExt_btpAnm* P2Base_btp(daNpc_P2_c* actor) {
    return reinterpret_cast<mDoExt_btpAnm*>(actor->mBtp);
}
static P2BasePattern* P2Base_pattern(J3DAnmTexPattern* pattern) {
    return reinterpret_cast<P2BasePattern*>(pattern);
}
static P2BaseSmoke* P2Base_smoke(daNpc_P2_c* actor) {
    return reinterpret_cast<P2BaseSmoke*>(actor->mSmokeCB);
}
static P2BaseModelData* P2Base_patternData(J3DModelData* data) {
    return reinterpret_cast<P2BaseModelData*>(data);
}
static Mtx34* P2Base_jointMatrix(J3DMtxBuffer_hd* matrices, u32 joint) {
    return gabi::at<Mtx34>(matrices->mpAnmMtx + joint * sizeof(Mtx34));
}
// HD: keep init's integer parameters and complete register result, as in the verified caller.
s32 P2Base_initBtp(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
s16 P2Base_calcTimer(be<s16>*);
void P2Base_ctorBtp(void*);
void P2Base_ctorJoint(dNpc_JntCtrl_c*);
void P2Base_ctorEventCut(dNpc_EventCut_c*);
void P2Base_ctorSmoke(void*, s32);
void P2Base_orderOther(fopAc_ac_c*, const char*, s32, u32);
void P2Base_registerFigure(s32, fopAc_ac_c*, cXyz*, s32, f32, f32, f32);
void P2Base_ctorNpcHIO(dNpc_HIO_p2*);
void P2Base_constructArray(void*, u32, u32, u32);
void P2Base_dtorCircle(void*, s32);
void P2Base_dtorAcch(dBgS_ObjAcch*, s32);
void P2Base_dtorActor(fopAc_ac_c*, s32);

// HD: nodeCallBack has already resolved the joint-data accessor before this field read.
struct P2BaseJointData { u8 padding[4]; be<u16> mJntNo; };
WWHD_OFFSET(P2BaseJointData, mJntNo, 4);
WWHD_SIZE(P2BaseJointData, 6);
