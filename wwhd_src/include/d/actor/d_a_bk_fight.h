#ifndef D_A_BK_FIGHT_H
#define D_A_BK_FIGHT_H

#include "d/actor/d_a_bk_local.h"

struct attack_info_s {
    /* 0x00 */ be<s32> bckFileIdx;
    /* 0x04 */ be<f32> speed;
    /* 0x08 */ be<s32> soundFileIdx;
};
WWHD_OFFSET(attack_info_s, bckFileIdx, 0x0);
WWHD_OFFSET(attack_info_s, speed, 0x4);
WWHD_OFFSET(attack_info_s, soundFileIdx, 0x8);
WWHD_SIZE(attack_info_s, 0xC);

// HD: prefix layouts used by the combat-only bindings below.
struct bkFightAttentionView {
    u8 padding[0x20];
    be<u32> mFlags;
};
WWHD_OFFSET(bkFightAttentionView, mFlags, 0x20);
WWHD_SIZE(bkFightAttentionView, 0x24);
struct bkFightPlayerView {
    u8 padding[0x3AC];
    u8 mCutType;
};
WWHD_OFFSET(bkFightPlayerView, mCutType, 0x3AC);
WWHD_SIZE(bkFightPlayerView, 0x3AD);
struct bkFightCollisionStatusView {
    u8 padding[0xC];
    gptr<fopAc_ac_c> mp_actor;
};
WWHD_OFFSET(bkFightCollisionStatusView, mp_actor, 0xC);
WWHD_SIZE(bkFightCollisionStatusView, 0x10);
struct bkFightCollisionObjectView {
    u8 padding[0x44];
    gptr<bkFightCollisionStatusView> mStts;
};
WWHD_OFFSET(bkFightCollisionObjectView, mStts, 0x44);
WWHD_SIZE(bkFightCollisionObjectView, 0x48);
// HD: guest loads preserve wrapped and null-address reads of the original bindings.

/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 procId, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, procId, param, reverb);
}
/* fopAcM_monsSeStart: HD inline with the actor/eyePos null checks of fopAcM_seStart */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
        mDoAud_monsSeStart(id, &a->eyePos, pid, param, reverb);
    }
}
/* 0200796C: CPad stick X of a port (HD; 0 for ports other than 0) */
static inline f32 CPad_GET_STICK_POS_X(s32 port) { return gabi::call<f32>(0x0200796C, port); }
/* dAttention_c (play+0x5804) */
static inline BOOL dAttention_LockonTruth(dAttention_c* a) {
    return gabi::call<BOOL>(0x024EDFCC, a);
}
/* 024EC8D0 (matcher: ActionTarget; used where GameCube calls LockonTarget(0)) */
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) {
    return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i);
}
/* dAttention_c::Lockon(): LockonTruth() || mFlags (+0x20) & 0x20000000 */
static inline bool dAttention_Lockon(dAttention_c* a) {
    return dAttention_LockonTruth(a) ||
           (gabi::load<u32>(gabi::ea(a) + offsetof(bkFightAttentionView, mFlags)) & 0x20000000) !=
               0;
}
static inline u16 cc_pl_cut_bit_get() { return gabi::call<u16>(0x02518B28); }
/* daPy_py_c::getCutType(): u8 at +0x3AC */
static inline u8 daPy_getCutType(fopAc_ac_c* p) {
    return gabi::load<u8>(gabi::ea(p) + offsetof(bkFightPlayerView, mCutType));
}
enum { daPy_CUT_TYPE_JUMPCUT_SWORD = 0xA };
/* dCcD_Sph At / mass (out of line) */
static inline void dCcD_Sph_StartCAt(dCcD_Sph* s, cXyz* p) { gabi::call(0x025167C0, s, p); }
static inline void dCcD_Sph_MoveCAt(dCcD_Sph* s, cXyz* p) { gabi::call(0x025167E4, s, p); }
static inline void dCcMassS_Mng_Set(void* mng, void* obj, u8 prio) {
    gabi::call(0x02516C14, mng, obj, prio);
}
static inline void* dCcD_GObjInf_GetAtHitObj(dCcD_GObjInf* o) {
    return gabi::call<void*>(0x02516178, o);
}
/* cCcD_Obj::GetAc(): mStts (+0x44) ? mStts->mp_actor (+0xC) : NULL */
static inline fopAc_ac_c* cCcD_Obj_GetAc(void* obj) {
    u32 stts = gabi::load<u32>(gabi::ea(obj) + offsetof(bkFightCollisionObjectView, mStts));
    return stts ? gabi::at<fopAc_ac_c>(
                      gabi::load<u32>(stts + offsetof(bkFightCollisionStatusView, mp_actor)))
                : nullptr;
}
/* daPy_py_c::checkPlayerGuard(): virtual (vtable at +0xB4, slot +0x3C) */
static inline BOOL daPy_checkPlayerGuard(fopAc_ac_c* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 0xB4);
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + 0x3C), p);
}
enum { fpcNm_PLAYER_e = 0xA8, fpcNm_BK_e = 0xBD };
static inline f32 cXyz_abs(cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }
#define REG0_F_(i) REG_F(0, i)
#define REG6_F(i) REG_F(6, i)
#define REG8_F(i) REG_F(8, i)

static inline void bkFight_anm_init(bk_class* a, int b, f32 m, u8 l, f32 s, int snd) {
    gabi::call(0x02098DA4, a, b, m, l, s, snd);
}
static inline void bkFight_wait_set(bk_class* a) { gabi::call(0x0209B264, a); }
static inline BOOL bkFight_daBk_player_view_check(bk_class* a, cXyz* p, s16 b, s16 c) {
    return gabi::call<BOOL>(0x0209B05C, a, p, b, c);
}
static inline BOOL bkFight_daBk_player_bg_check(bk_class* a, cXyz* p) {
    return gabi::call<BOOL>(0x0209AE7C, a, p);
}
static inline BOOL bkFight_daBk_player_way_check(bk_class* a) {
    return gabi::call<BOOL>(0x0209B208, a);
}
static inline BOOL bkFight_daBk_bomb_view_check(bk_class* a) {
    return gabi::call<BOOL>(0x0209AE40, a);
}

#endif
