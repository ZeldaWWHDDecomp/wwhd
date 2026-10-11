/* f_op_actor_mng (GameCube names) -> WWHD. Already in bindings.h: fopAcM_delete,
 * fopAcM_SearchByID (HD inline, id passed by address), fopAcM_GetRoomNo, fopAcM_GetParam,
 * fopAcM_seStart (HD inline), fopAcM_orderSpeakEvent, fopAcM_setCullSizeBox,
 * fopAcM_CheckCondition, fopAcM_OnCondition. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"

/* process names (d_procname.h) */
enum fpcNm_e {
    fpcNm_NZ_e = 0x00C6,
    fpcNm_ESA_e = 0x00DD,
    fpcNm_BOMB_e = 0x0126,
    fpcNm_TSUBO_e = 0x01C5,
    fpcNm_Obj_Barrel_e = 0x01C8,
};

/* HD: null-safe (every search callback of d_a_npc_nz tests the actor first) [v nz] */
inline s16 fopAcM_GetName(void* p) { return p ? gabi::load<s16>(gabi::ea(p) + 8) : -1; }
/* fopAcM_Search = fopAcIt_Judge (025D5218) [v nz] */
inline void* fopAcM_Search(void* (*func)(void*, void*), void* data) { return gabi::call<void*>(0x025D5218, func, data); }
inline BOOL fopAcM_entrySolidHeap(fopAc_ac_c* a, BOOL (*cb)(fopAc_ac_c*), u32 size) { return gabi::call<BOOL>(0x025D63E8, a, cb, size); }
/* 025D87E4 [v nz] */
inline fpc_ProcID fopAcM_createShopItem(cXyz* pos, int itemNo, csXyz* rot, int roomNo, cXyz* scale = nullptr,
                                        cPhs_State (*createFunc)(void*) = nullptr) {
    return gabi::call<fpc_ProcID>(0x025D87E4, pos, itemNo, rot, roomNo, scale, createFunc);
}
/* 025D7970 [v nz] */
inline BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* partner, s16 eventIdx, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D7970, a, partner, eventIdx, flag, hind);
}
inline void fopAcM_SetMtx(fopAc_ac_c* a, Mtx34* m) { a->cullMtx = gabi::ea(m); }
inline void fopAcM_setCullSizeFar(fopAc_ac_c* a, f32 f) { a->cullSizeFar = f; }
inline void fopAcM_SetGravity(fopAc_ac_c* a, f32 g) { a->gravity = g; }
inline cXyz* fopAcM_GetPosition_p(fopAc_ac_c* a) { return &a->current.pos; }
inline cXyz* fopAcM_GetOldPosition_p(fopAc_ac_c* a) { return &a->old.pos; }
inline cXyz* fopAcM_GetSpeed_p(fopAc_ac_c* a) { return &a->speed; }

/* fopAcM_ct: construct the actor in place once (HD: GHS's placement new tests the pointer) */
#define fopAcM_ct(ptr, ClassName)                                    \
    if (!fopAcM_CheckCondition(ptr, fopAcCnd_INIT_e)) {             \
        if (gabi::ea(ptr) != 0) new (ptr) ClassName();               \
        fopAcM_OnCondition(ptr, fopAcCnd_INIT_e);                    \
    }

/* compiler-generated construction/destruction of the base classes (HD) */
inline void __ct(fopNpc_npc_c* p) { gabi::call(0x025A1458, p); }      /* fopNpc_npc_c::fopNpc_npc_c (matcher: cDyl_LinkASync) */
inline void __dt(fopAc_ac_c* p) { gabi::call(0x025D50BC, p, 0); }     /* fopAc_ac_c::~fopAc_ac_c */

/* __sinit of every HD TU: the statics of the common headers (gc_tu.headerStatics) */
inline void __sinit_header_statics() { sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]); }

/* Item creation helpers [v d_a_disappear]. */
/* 025D8A5C fopAcM_createItemForBoss */
inline void fopAcM_createItemForBoss(cXyz* pos, s32 param, s32 roomNo, csXyz* angle, cXyz* scale, s32 flag) {
    gabi::call(0x025D8A5C, pos, param, roomNo, angle, scale, flag);
}
/* 025D8870 fopAcM_createItem */
inline fpc_ProcID fopAcM_createItem(cXyz* pos, s32 itemNo, s32 bitNo, s32 roomNo, s32 type, csXyz* angle, s32 action, cXyz* scale) {
    return gabi::call<fpc_ProcID>(0x025D8870, pos, itemNo, bitNo, roomNo, type, angle, action, scale);
}
/* 025D9154 fopAcM_createIball */
inline fpc_ProcID fopAcM_createIball(cXyz* pos, s32 tbl, s32 roomNo, csXyz* angle, s32 bitNo) {
    return gabi::call<fpc_ProcID>(0x025D9154, pos, tbl, roomNo, angle, bitNo);
}

// HD: an opt-in namespace avoids collisions with older units' private inline copies.
namespace fopAcMApi {
/* 025D5600 [v d_a_bmd]: the caller fills only the append's parameter prefix. */
inline void* createAppend() { return gabi::call<void*>(0x025D5600); }
/* 025D7B24 [v d_a_bmd]: preserve the full argument registers used by HD. */
inline void orderPotentialEvent(fopAc_ac_c* actor, s32 flag, s32 event, s32 priority) { gabi::call(0x025D7B24, actor, flag, event, priority); }
/* 025D9874 [v d_a_bmd] */
inline void createWarpFlower(cXyz* pos, s32 type, s32 room, s32 flag) { gabi::call(0x025D9874, pos, type, room, flag); }
}
