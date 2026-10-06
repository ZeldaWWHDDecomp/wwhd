/* d_attention part 2: the lock-on lists and the lock-on state machine (initList .. runSoundProc).
 * WWHD. See d_attention.cpp for the unit's range and
 * layouts. Calls to functions of the other parts go by address. */
#include "bindings.h"

namespace d_attention_2_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void cSAngle_ct_l(u32 a) { gabi::call(0x020065FC, a); }
static inline void cSAngle_mi_l(u32 a, u32 res, s16 v) { gabi::call(0x02006908, a, res, v); }
static inline void cSGlobe_ct_l(u32 g, u32 v) { gabi::call(0x02007324, g, v); }
static inline void cXyz_mi_l(u32 a, u32 res, u32 b) { gabi::call(0x0201ADE0, a, res, b); }
static inline void memcpy_l(u32 d, u32 s, u32 n) { gabi::call(0xC000A848, d, s, n); }
static inline void fopAcIt_Executor_l(u32 fn, u32 data) { gabi::call(0x025D51DC, fn, data); }
static inline BOOL mDoCPd_L_LOCK_BUTTON_l(u32 pad) { return gabi::call<BOOL>(0x02007CA4, pad); }
static inline f32 CPad_GET_STICK_POS_Y_l(u32 pad) { return gabi::call<f32>(0x02007990, pad); }
static inline void mDoAud_bgmNowBattle_l(f32 v) { gabi::call(0x025E1DD8, v); }

/* functions of part 1 */
static inline f32 calcWeight_l(u32 t, s32 type, u32 ac, f32 d, s32 a1, s32 a2, u32 out) {
    return gabi::call<f32>(0x024EB2D4, t, type, ac, d, a1, a2, out);
}
static inline void setActor_l(u32 e, u32 ac) { gabi::call(0x024EB524, e, ac); }
static inline u32 getActor_l(u32 e) { return gabi::call<u32>(0x024EBA14, e); }
static inline s32 check_event_condition_l(u32 type, u32 cond) { return gabi::call<s32>(0x024EAFE4, type, cond); }
static inline s32 check_flontofplayer_l(u32 mask, s32 a1, s32 a2) { return gabi::call<s32>(0x024EB030, mask, a1, a2); }
static inline s32 check_distace_l(u32 pp, s32 a, u32 ap, f32 b, f32 m, f32 mx, f32 mn) {
    return gabi::call<s32>(0x024EB198, pp, a, ap, b, m, mx, mn);
}
static inline f32 distace_weight_l(f32 d, s32 a, f32 r) { return gabi::call<f32>(0x024EB0D8, d, a, r); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 searchByID(u32 id) {
    gabi::Local<be<u32>> key;
    *key = id;
    if (id == 0xFFFFFFFFu) return 0;
    return gabi::call<u32>(0x025D5218, 0x025E1234u, key.get());
}

struct cXyz_l { be<f32> x, y, z; };
struct cSGlobe_l { be<f32> r; be<s16> v, u; };
struct dAttList_l { be<f32> w, d; be<u32> type, id; };

enum : u32 { DIST_TABLE = 0x101D3CC0, LOC_TYPE_NUM = 0x101D5180, LOC_TYPE_TBL = 0x101D5188 };
enum : u32 { AC_ATTN_DIST = 0x388, AC_ATTN_POS = 0x390, AC_ATTN_FLAGS = 0x39C, AC_EVT_COND = 0xFA, AC_SHAPE_Y = 0x32A };
/* dAttention_c */
enum : u32 { A_PLAYER = 0, A_TARGETID = 4, A_PADNO = 0xC, A_FLAGMASK = 0x10, A_LOCKSTATE = 0x18, A_BUTTON = 0x1A,
             A_FLAGS = 0x20, A_LLIST = 0x54, A_LCOUNT = 0xD4, A_LOFFSET = 0xD8, A_ALIST = 0xDC, A_ACOUNT = 0x11C,
             A_AOFFSET = 0x120, A_ENEMYID = 0x168, A_ENEMYDIST = 0x16C };

/* 024EC6A8 */
static void dAttention_c_initList(u32 i_this, u32 flagMask) {
    WWHD_FUNC(0x024EC6A8, void, i_this, flagMask);
    st(i_this + A_FLAGMASK, flagMask);
    for (s32 i = 0; i < 8; i++) {
        setActor_l(i_this + A_LLIST + i * 0x10, 0);
        stf(i_this + A_LLIST + i * 0x10, 3.4028235e+38f);
    }
    st(i_this + A_LOFFSET, 0);
    st(i_this + A_LCOUNT, 0);
    for (s32 i = 0; i < 4; i++) {
        setActor_l(i_this + A_ALIST + i * 0x10, 0);
        stf(i_this + A_ALIST + i * 0x10, 3.4028235e+38f);
    }
    st(i_this + A_AOFFSET, 0);
    st(i_this + A_ACOUNT, 0);
}
VERIFY(0x024EC6A8, dAttention_c_initList);

/* 024EC72C */
static s32 dAttention_c_makeList(u32 i_this) {
    WWHD_FUNC(0x024EC72C, s32, i_this);
    fopAcIt_Executor_l(0x024EB7E0 /* select_attention */, i_this);
    return (s32)(ld(i_this + A_LCOUNT) + ld(i_this + A_ACOUNT));
}
VERIFY(0x024EC72C, dAttention_c_makeList);

static inline void sort(u32 i_this, u32 cntOff, u32 list, u32 swap) {
    for (s32 i = 0; i < (s32)ld(i_this + cntOff) - 1; i++) {
        for (s32 j = i + 1; j < (s32)ld(i_this + cntOff); j++) {
            if (ldf(list + i * 0x10) > ldf(list + j * 0x10)) {
                memcpy_l(swap, list + j * 0x10, 0x10);
                memcpy_l(list + j * 0x10, list + i * 0x10, 0x10);
                memcpy_l(list + i * 0x10, swap, 0x10);
            }
        }
    }
}

/* 024EC770 */
static void dAttention_c_sortList(u32 i_this) {
    WWHD_FUNC(0x024EC770, void, i_this);
    gabi::Local<dAttList_l> swap;
    sort(i_this, A_LCOUNT, i_this + A_LLIST, gabi::ea(swap.get()));
    sort(i_this, A_ACOUNT, i_this + A_ALIST, gabi::ea(swap.get()));
}
VERIFY(0x024EC770, dAttention_c_sortList);

/* 024EC8D0: LockonTarget (matcher: ActionTarget; the matcher's LockonTarget 024EE464 is ActionTarget) */
static u32 dAttention_c_LockonTarget(u32 i_this, s32 idx) {
    WWHD_FUNC(0x024EC8D0, u32, i_this, idx);
    s32 cnt = (s32)ld(i_this + A_LCOUNT);
    if (idx >= cnt) return 0;
    s32 listIdx = (s32)ld(i_this + A_LOFFSET) + idx;
    if (listIdx >= cnt) listIdx -= cnt;
    return getActor_l(i_this + A_LLIST + listIdx * 0x10);
}
VERIFY(0x024EC8D0, dAttention_c_LockonTarget);

/* 024EC908 */
static u32 dAttention_c_stockAttention(u32 i_this, u32 interactMask) {
    WWHD_FUNC(0x024EC908, u32, i_this, interactMask);
    u32 pTarget = dAttention_c_LockonTarget(i_this, 0);
    dAttention_c_initList(i_this, interactMask);
    if (dAttention_c_makeList(i_this) != 0) dAttention_c_sortList(i_this);
    if (pTarget != getActor_l(i_this + A_LLIST)) {
        if (pTarget != 0) {
            if (getActor_l(i_this + A_LLIST) != 0) st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 2 | 4);
            else st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 4);
        } else {
            st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 1 | 4);
        }
    }
    return dAttention_c_LockonTarget(i_this, 0);
}
VERIFY(0x024EC908, dAttention_c_stockAttention);

/* 024EC9D4 */
static u32 dAttention_c_nextAttention(u32 i_this, u32 interactMask) {
    WWHD_FUNC(0x024EC9D4, u32, i_this, interactMask);
    u32 pTarget = searchByID(ld(i_this + A_TARGETID));
    dAttention_c_initList(i_this, interactMask);
    if (dAttention_c_makeList(i_this) != 0) dAttention_c_sortList(i_this);
    if (pTarget == getActor_l(i_this + A_LLIST) && (s32)ld(i_this + A_LCOUNT) > 1) st(i_this + A_LOFFSET, 1);
    return dAttention_c_LockonTarget(i_this, 0);
}
VERIFY(0x024EC9D4, dAttention_c_nextAttention);

/* 024ECA8C */
static s32 dAttention_c_freeAttention(u32 i_this) {
    WWHD_FUNC(0x024ECA8C, s32, i_this);
    st(i_this + A_AOFFSET, 0);
    st(i_this + A_LOFFSET, 0);
    st(i_this + A_ACOUNT, 0);
    st(i_this + A_LCOUNT, 0);
    dAttention_c_initList(i_this, 0xFFFFFFFF);
    return 0;
}
VERIFY(0x024ECA8C, dAttention_c_freeAttention);

/* 024ECAC8: chkAttMask (not named by the matcher) */
static u32 dAttention_c_chkAttMask(u32 i_this, u32 type, u32 mask) {
    WWHD_FUNC(0x024ECAC8, u32, i_this, type, mask);
    s32 num = (s32)ld(LOC_TYPE_NUM);
    for (s32 i = 0; i < num; i++) {
        if (type == (u32)(s32)lds16(LOC_TYPE_TBL + i * 4)) return mask & ld16(LOC_TYPE_TBL + i * 4 + 2);
    }
    return 1;
}
VERIFY(0x024ECAC8, dAttention_c_chkAttMask);

/* 024ECB08 */
static bool dAttention_c_chaseAttention(u32 i_this) {
    WWHD_FUNC(0x024ECB08, bool, i_this);
    gabi::Local<be<s16>> angle1;
    gabi::Local<be<s16>> res;
    gabi::Local<be<s16>> angle2;
    gabi::Local<be<u32>> type;
    gabi::Local<cSGlobe_l> globe1;
    gabi::Local<cXyz_l> d;
    gabi::Local<cSGlobe_l> globe2;
    cSAngle_ct_l(gabi::ea(angle1.get()));
    cSAngle_ct_l(gabi::ea(angle2.get()));
    u32 e = i_this + A_LLIST + ld(i_this + A_LOFFSET) * 0x10;
    u32 actor = getActor_l(e);
    if (actor == 0) return false;
    cXyz_mi_l(actor + AC_ATTN_POS, gabi::ea(d.get()), ld(i_this + A_PLAYER) + AC_ATTN_POS);
    cSGlobe_ct_l(gabi::ea(globe1.get()), gabi::ea(d.get()));
    cSAngle_mi_l(gabi::ea(globe1.get()) + 6, gabi::ea(res.get()), lds16(ld(i_this + A_PLAYER) + AC_SHAPE_Y));
    *angle1 = (s16)*res;
    cXyz_mi_l(ld(i_this + A_PLAYER) + AC_ATTN_POS, gabi::ea(d.get()), actor + AC_ATTN_POS);
    cSGlobe_ct_l(gabi::ea(globe2.get()), gabi::ea(d.get()));
    cSAngle_mi_l(gabi::ea(globe2.get()) + 6, gabi::ea(res.get()), lds16(actor + AC_SHAPE_Y));
    *angle2 = (s16)*res;
    f32 weight = calcWeight_l(i_this, 'L', actor, globe1->r, *angle1, *angle2, gabi::ea(type.get()));
    if (weight > 0.0f) {
        setActor_l(e, actor);
        stf(e + 4, globe1->r);
        stf(e + 0, weight);
        st(e + 8, *type);
        return true;
    }
    u32 t = ld(e + 8);
    *type = t;
    u32 flags = ld(actor + AC_ATTN_FLAGS);
    u8 idx = ld8(actor + AC_ATTN_DIST + t);
    if (dAttention_c_chkAttMask(i_this, t, flags) == 0) return false;
    if (check_event_condition_l(t, ld16(actor + AC_EVT_COND)) != 0) return false;
    u32 dt = DIST_TABLE + idx * 0x1C;
    s16 a1 = *angle1;
    if (check_flontofplayer_l(ld(dt + 0x18), a1, *angle2) != 0) return false;
    if (check_distace_l(ld(i_this + A_PLAYER) + AC_ATTN_POS, a1, actor + AC_ATTN_POS, ldf(dt + 4), ldf(dt + 8),
                        ldf(dt + 0xC), ldf(dt + 0x10)) == 0)
        return false;
    stf(e + 0, distace_weight_l(globe1->r, *angle1, 0.5f));
    return true;
}
VERIFY(0x024ECB08, dAttention_c_chaseAttention);

static inline bool player_busy() {
    if (ld(gabi::ea(dComIfGp_get()) + 0x5CD8) & 0x37A02371) return true;
    return (ld(gabi::ea(dComIfGp_get()) + 0x5CDC) & 0x11) != 0;
}

/* 024ECCFC */
static void dAttention_c_judgementButton(u32 i_this) {
    WWHD_FUNC(0x024ECCFC, void, i_this);
    if (player_busy()) {
        u8 b = ld8(i_this + A_BUTTON);
        if (b < 1) return;
        if (b <= 2) st8(i_this + A_BUTTON, 0);
        return;
    }
    switch (ld8(i_this + A_BUTTON)) {
    case 0:
        if (!mDoCPd_L_LOCK_BUTTON_l(ld(i_this + A_PADNO))) break;
        st8(i_this + A_BUTTON, 1);
        break;
    case 1:
        st8(i_this + A_BUTTON, 2);
        /* fallthrough */
    case 2:
        if (!mDoCPd_L_LOCK_BUTTON_l(ld(i_this + A_PADNO))) st8(i_this + A_BUTTON, 0);
        break;
    }
}
VERIFY(0x024ECCFC, dAttention_c_judgementButton);

/* 024ECDDC */
static void dAttention_c_judgementTriggerProc(u32 i_this) {
    WWHD_FUNC(0x024ECDDC, void, i_this);
    if (dAttention_c_chaseAttention(i_this)) {
        u32 f = ld(i_this + A_FLAGS);
        st8(i_this + A_LOCKSTATE, 1);
        st(i_this + A_FLAGS, f | 8);
    }
}
VERIFY(0x024ECDDC, dAttention_c_judgementTriggerProc);

/* 024ECE24 */
static BOOL dAttention_c_judgementLostCheck(u32 i_this) {
    WWHD_FUNC(0x024ECE24, BOOL, i_this);
    if (dAttention_c_chaseAttention(i_this)) return FALSE;
    u32 f = ld(i_this + A_FLAGS);
    st8(i_this + A_LOCKSTATE, 0);
    st(i_this + A_FLAGS, f | 0x10);
    dAttention_c_freeAttention(i_this);
    st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 0x40);
    return TRUE;
}
VERIFY(0x024ECE24, dAttention_c_judgementLostCheck);

/* 024ECE9C */
static u32 dAttention_c_LockonTargetPId(u32 i_this, s32 idx) {
    WWHD_FUNC(0x024ECE9C, u32, i_this, idx);
    s32 cnt = (s32)ld(i_this + A_LCOUNT);
    if (idx >= cnt) return 0;
    s32 listIdx = (s32)ld(i_this + A_LOFFSET) + idx;
    if (listIdx >= cnt) listIdx -= cnt;
    return ld(i_this + A_LLIST + listIdx * 0x10 + 0xC);
}
VERIFY(0x024ECE9C, dAttention_c_LockonTargetPId);

/* 024ECED4 */
static void dAttention_c_judgementStatusSw(u32 i_this, u32 interactMask) {
    WWHD_FUNC(0x024ECED4, void, i_this, interactMask);
    switch (ld8(i_this + A_LOCKSTATE)) {
    case 0:
        st(i_this + A_TARGETID, 0xFFFFFFFF);
        dAttention_c_stockAttention(i_this, interactMask);
        if (ld8(i_this + A_BUTTON) == 1) dAttention_c_judgementTriggerProc(i_this);
        break;
    case 1: {
        u32 id = dAttention_c_LockonTargetPId(i_this, 0);
        u8 b = ld8(i_this + A_BUTTON);
        st(i_this + A_TARGETID, id);
        if (b == 1) {
            f32 stickY = CPad_GET_STICK_POS_Y_l(ld(i_this + A_PADNO));
            if (-0.9f < stickY && dAttention_c_nextAttention(i_this, interactMask) != 0 &&
                (s32)ld(i_this + A_LCOUNT) > 1) {
                st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 8);
            } else {
                u32 f = ld(i_this + A_FLAGS);
                st8(i_this + A_LOCKSTATE, 2);
                st(i_this + A_FLAGS, f | 0x10);
            }
        } else {
            dAttention_c_judgementLostCheck(i_this);
        }
        break;
    }
    case 2: {
        u32 f = ld(i_this + A_FLAGS);
        u8 b = ld8(i_this + A_BUTTON);
        st(i_this + A_FLAGS, f | 0x40);
        if (b == 1) {
            st8(i_this + A_LOCKSTATE, 0);
            dAttention_c_judgementTriggerProc(i_this);
        } else if (dAttention_c_LockonTarget(i_this, 0) == 0 || !(ld(i_this + A_FLAGS) & 0x40000000)) {
            st8(i_this + A_LOCKSTATE, 0);
            dAttention_c_freeAttention(i_this);
        }
        break;
    }
    }
}
VERIFY(0x024ECED4, dAttention_c_judgementStatusSw);

/* 024ED0A4 */
static void dAttention_c_judgementStatusHd(u32 i_this, u32 interactMask) {
    WWHD_FUNC(0x024ED0A4, void, i_this, interactMask);
    switch (ld8(i_this + A_LOCKSTATE)) {
    case 0:
        st(i_this + A_TARGETID, 0xFFFFFFFF);
        dAttention_c_stockAttention(i_this, interactMask);
        if (ld8(i_this + A_BUTTON) == 1) dAttention_c_judgementTriggerProc(i_this);
        break;
    case 1:
        st(i_this + A_TARGETID, dAttention_c_LockonTargetPId(i_this, 0));
        if (dAttention_c_judgementLostCheck(i_this) == 0 && ld8(i_this + A_BUTTON) == 0) {
            u32 f = ld(i_this + A_FLAGS);
            st8(i_this + A_LOCKSTATE, 2);
            st(i_this + A_FLAGS, f | 0x10);
        }
        break;
    case 2: {
        u32 f = ld(i_this + A_FLAGS);
        u8 b = ld8(i_this + A_BUTTON);
        st(i_this + A_FLAGS, f | 0x40);
        if (b == 1) {
            if (dAttention_c_nextAttention(i_this, interactMask) != 0) {
                u32 f2 = ld(i_this + A_FLAGS);
                st8(i_this + A_LOCKSTATE, 1);
                st(i_this + A_FLAGS, f2 | 8);
                break;
            }
        } else if (dAttention_c_LockonTarget(i_this, 0) != 0 && (ld(i_this + A_FLAGS) & 0x40000000)) {
            break;
        }
        st8(i_this + A_LOCKSTATE, 0);
        dAttention_c_freeAttention(i_this);
        break;
    }
    }
}
VERIFY(0x024ED0A4, dAttention_c_judgementStatusHd);

/* 024ED200 */
static void dAttention_c_runSoundProc(u32 i_this) {
    WWHD_FUNC(0x024ED200, void, i_this);
    u32 f = ld(i_this + A_FLAGS);
    st(i_this + A_ENEMYID, 0xFFFFFFFF);
    stf(i_this + A_ENEMYDIST, 10000.0f);
    if (f & 0x80000000) return;
    fopAcIt_Executor_l(0x024EB88C /* sound_attention */, i_this);
    if (searchByID(ld(i_this + A_ENEMYID)) != 0) {
        mDoAud_bgmNowBattle_l(ldf(i_this + A_ENEMYDIST) * 0.1f);
        st(i_this + A_FLAGS, ld(i_this + A_FLAGS) | 0x100);
    }
}
VERIFY(0x024ED200, dAttention_c_runSoundProc);

} // namespace d_attention_2_cpp
