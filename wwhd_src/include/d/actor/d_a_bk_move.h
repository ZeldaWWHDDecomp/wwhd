#pragma once
#include "d/actor/d_a_bk_local.h"
#include "d/d_path.h"

// HD: the searchlight actor keeps its original member offsets.
struct bkMoveSearchLight {
    u8 _00[0x780];
    be<u8> mBkControl;
    u8 _781[0x96C - 0x781];
    be<u32> mChildId;
};
WWHD_OFFSET(bkMoveSearchLight, mBkControl, 0x780);
WWHD_OFFSET(bkMoveSearchLight, mChildId, 0x96C);
WWHD_SIZE(bkMoveSearchLight, 0x970);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb); fopAcM_monsSeStart is an HD
 * inline with the actor/eyePos null checks of fopAcM_seStart */
static inline void fopAcM_monsSeStart_mv(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
/* 02516C14 dCcMassS_Mng::Set(cCcD_Obj*, u8 priority) on play+PLAY_CCMASS */
static inline void dComIfG_Ccsp_SetMass(void* obj, u8 prio) {
    gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, prio);
}
/* 0209AB64 (matcher: search_wepon) is daBk_wepon_view_check with search_wepon inlined (d_a_bk.cpp)
 */
#define daBk_wepon_view_check_mv daBk_wepon_view_check
/* 020A47A4 (unnamed) is carry(): speed.y = 0 */
static inline void carry_mv(bk_class* i_this) { gabi::call(0x020A47A4, i_this); }
/* REGn_F / REGn_S */
#define REG8_F(i) REG_F(8, i)
#define REG8_S(i) REG_S(8, i)
/* daObj_Search::Act_c: setChildId (+0x96C), setBkControl (+0x780) */
inline void daObj_Search_setChildId(daObj_Search_Act_c* s, u32 id) {
    gabi::at<bkMoveSearchLight>(gabi::ea(s))->mChildId = id;
}
inline void daObj_Search_setBkControl(daObj_Search_Act_c* s, bool v) {
    gabi::at<bkMoveSearchLight>(gabi::ea(s))->mBkControl = v;
}
static inline fopAc_ac_c* search_ac(daObj_Search_Act_c* s) { return (fopAc_ac_c*)(void*)s; }

static inline void set_ken(fopAc_ac_c* k) {
    gabi::store<u32>(0x1046231C, gabi::ea(k));
} /* static ken */

/* dPath / dPnt (unchanged from GameCube); points are addressed from the path pointer with plain
 * address arithmetic (after a failed JUT_ASSERT the original reads through a null path) */
using dPnt_mv = path_layout::dPnt;
inline path_layout::dPath* movementPath(bk_class* i_this) {
    return gabi::at<path_layout::dPath>(gabi::ea(i_this->ppd.get()));
}
inline u16 path_num(bk_class* i_this) { return movementPath(i_this)->m_num; }
inline u16 path_nextID(bk_class* i_this) { return movementPath(i_this)->m_nextID; }
inline bool path_closed(bk_class* i_this) { return (movementPath(i_this)->m_closed & 1) != 0; }
inline dPnt_mv* path_point(bk_class* i_this, s8 idx) {
    u32 pts = gabi::ea(movementPath(i_this)->m_points.get());
    return gabi::at<dPnt_mv>(pts + (s32)idx * sizeof(dPnt_mv));
}

/* lfs/stfs copies: the recompiled code moves the bits unchanged (no SNaN quieting) */
static inline void fcopy(be<f32>& dst, const be<f32>& src) {
    gabi::store<u32>(dst.addr(), gabi::load<u32>(src.addr()));
}
static inline void copy_f(cXyz* dst, const cXyz* src) {
    fcopy(dst->x, src->x);
    fcopy(dst->y, src->y);
    fcopy(dst->z, src->z);
}

// HD: weak forwarders bind the original functions in the owner source.
inline void bkMoveGuest_anm_init(bk_class* a, int b, f32 m, u8 l, f32 s, int f) {
    gabi::call(0x02098DA4, a, b, m, l, s, f);
}
inline BOOL bkMoveGuest_daBk_wepon_view_check(bk_class* a) {
    return gabi::call<BOOL>(0x0209AB64, a);
}
inline void bkMoveGuest_wait_set(bk_class* a) { gabi::call(0x0209B264, a); }
inline BOOL bkMoveGuest_daBk_bomb_view_check(bk_class* a) {
    return gabi::call<BOOL>(0x0209AE40, a);
}
inline BOOL bkMoveGuest_daBk_player_view_check(bk_class* a, cXyz* p, s16 b, s16 c) {
    return gabi::call<BOOL>(0x0209B05C, a, p, b, c);
}

enum bkMoveAction {
    BK_ACTION_JYUNKAI = 0,
    BK_ACTION_STAND = 1,
    BK_ACTION_STAND2 = 2,
    BK_ACTION_PATH_RUN = 3,
    BK_ACTION_FIGHT_RUN = 4,
    BK_ACTION_FIGHT = 5,
    BK_ACTION_OSHI = 7,
    BK_ACTION_P_LOST = 8,
    BK_ACTION_B_NIGE = 9,
    BK_ACTION_DEFENCE = 10,
    BK_ACTION_HUKKI = 11,
    BK_ACTION_WEPON_SEARCH = 12,
    BK_ACTION_AITE_MIRU = 14,
    BK_ACTION_TUBO_WAIT = 15,
    BK_ACTION_ROPE_ON = 16,
    BK_ACTION_D_MAHI = 18,
    BK_ACTION_B_HANG = 19,
    BK_ACTION_FAIL = 20,
    BK_ACTION_YOGAN_FAIL = 21,
    BK_ACTION_WATER_FAIL = 22,
    BK_ACTION_D_DOZOU = 23,
    BK_ACTION_Z_DEMO_1 = 29,
    BK_ACTION_CARRY = 30,
    BK_ACTION_CARRY_DROP = 31,
};
