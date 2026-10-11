/* d_attention part 2: the lock-on lists and the lock-on state machine (initList .. runSoundProc).
 * WWHD. See d_attention.cpp for the unit's range and
 * layouts. Calls to functions of the other parts go by address. */
#include "d/d_attention.h"

// HD: endian-aware image accesses retained for word copies and dynamic virtual slots.
namespace AttentionMemory2 {
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
} // namespace AttentionMemory2

u32 att2_searchActor(u32, u32);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
void att2_cSAngle_ct(u32 a);
void att2_cSAngle_mi(u32 a, u32 res, s16 v);
void att2_cSGlobe_ct(u32 g, u32 v);
void att2_cXyz_mi(u32 a, u32 res, u32 b);
void att2_memcpy(u32 d, u32 s, u32 n);
void att2_fopAcIt_Executor(u32 fn, u32 data);
BOOL att2_mDoCPd_L_LOCK_BUTTON(u32 pad);
f32 att2_CPad_GET_STICK_POS_Y(u32 pad);
void att2_mDoAud_bgmNowBattle(f32 v);

/* functions of part 1 */
f32 att2_calcWeight(u32 t, s32 type, u32 ac, f32 d, s32 a1, s32 a2, u32 out);
void att2_setActor(u32 e, u32 ac);
u32 att2_getActor(u32 e);
s32 att2_check_event_condition(u32 type, u32 cond);
s32 att2_check_flontofplayer(u32 mask, s32 a1, s32 a2);
s32 att2_check_distace(u32 pp, s32 a, u32 ap, f32 b, f32 m, f32 mx, f32 mn);
f32 att2_distace_weight(f32 d, s32 a, f32 r);

static inline u32 searchByID(u32 id) {
    gabi::Local<be<u32>> key;
    *key = id;
    if (id == 0xFFFFFFFFu)
        return 0;
    return att2_searchActor(0x025E1234u, gabi::ea(key.get()));
}

enum : u32 { DIST_TABLE = 0x101D3CC0, LOC_TYPE_NUM = 0x101D5180, LOC_TYPE_TBL = 0x101D5188 };
enum : u32 {
    AC_ATTN_DIST = 0x388,
    AC_ATTN_POS = 0x390,
    AC_ATTN_FLAGS = 0x39C,
    AC_EVT_COND = 0xFA,
    AC_SHAPE_Y = 0x32A
};
/* dAttention_c */
enum : u32 {
    A_PLAYER = 0,
    A_TARGETID = 4,
    A_PADNO = 0xC,
    A_FLAGMASK = 0x10,
    A_LOCKSTATE = 0x18,
    A_BUTTON = 0x1A,
    A_FLAGS = 0x20,
    A_LLIST = 0x54,
    A_LCOUNT = 0xD4,
    A_LOFFSET = 0xD8,
    A_ALIST = 0xDC,
    A_ACOUNT = 0x11C,
    A_AOFFSET = 0x120,
    A_ENEMYID = 0x168,
    A_ENEMYDIST = 0x16C
};

/* 024EC6A8 */
static void dAttention_c_initList(u32 i_this, u32 flagMask) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    self->mFlagMask = flagMask;
    for (s32 i = 0; i < 8; i++) {
        att2_setActor(i_this + A_LLIST + i * 0x10, 0);
        AttentionMemory2::stf(i_this + A_LLIST + i * 0x10, 3.4028235e+38f);
    }
    self->mLockOnOffset = 0;
    self->mLockonCount = 0;
    for (s32 i = 0; i < 4; i++) {
        att2_setActor(i_this + A_ALIST + i * 0x10, 0);
        AttentionMemory2::stf(i_this + A_ALIST + i * 0x10, 3.4028235e+38f);
    }
    self->mActionOffset = 0;
    self->mActionCount = 0;
}

/* 024EC72C */
static s32 dAttention_c_makeList(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    att2_fopAcIt_Executor(0x024EB7E0 /* select_attention */, i_this);
    return (s32)((u32)self->mLockonCount + (u32)self->mActionCount);
}

static inline void sort(u32 i_this, u32 cntOff, u32 list, u32 swap) {
    for (s32 i = 0; i < (s32)AttentionMemory2::ld(i_this + cntOff) - 1; i++) {
        for (s32 j = i + 1; j < (s32)AttentionMemory2::ld(i_this + cntOff); j++) {
            if ((f32)AttentionHD::view<AttentionHD::List>(list)[i].mWeight >
                (f32)AttentionHD::view<AttentionHD::List>(list)[j].mWeight) {
                att2_memcpy(swap, list + j * 0x10, 0x10);
                att2_memcpy(list + j * 0x10, list + i * 0x10, 0x10);
                att2_memcpy(list + i * 0x10, swap, 0x10);
            }
        }
    }
}

/* 024EC770 */
static void dAttention_c_sortList(u32 i_this) {
    gabi::Local<AttentionHD::List> swap;
    sort(i_this, A_LCOUNT, i_this + A_LLIST, gabi::ea(swap.get()));
    sort(i_this, A_ACOUNT, i_this + A_ALIST, gabi::ea(swap.get()));
}

// HD: LockonTarget (matcher: ActionTarget; the matcher's LockonTarget 024EE464 is ActionTarget)
/* 024EC8D0 */
static u32 dAttention_c_LockonTarget(u32 i_this, s32 idx) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    s32 cnt = (s32)(u32)self->mLockonCount;
    if (idx >= cnt)
        return 0;
    s32 listIdx = (s32)(u32)self->mLockOnOffset + idx;
    if (listIdx >= cnt)
        listIdx -= cnt;
    return att2_getActor(i_this + A_LLIST + listIdx * 0x10);
}

/* 024EC908 */
static u32 dAttention_c_stockAttention(u32 i_this, u32 interactMask) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    u32 pTarget = dAttention_c_LockonTarget(i_this, 0);
    dAttention_c_initList(i_this, interactMask);
    if (dAttention_c_makeList(i_this) != 0)
        dAttention_c_sortList(i_this);
    if (pTarget != att2_getActor(i_this + A_LLIST)) {
        if (pTarget != 0) {
            if (att2_getActor(i_this + A_LLIST) != 0)
                self->mFlags = (u32)self->mFlags | 2 | 4;
            else
                self->mFlags = (u32)self->mFlags | 4;
        } else {
            self->mFlags = (u32)self->mFlags | 1 | 4;
        }
    }
    return dAttention_c_LockonTarget(i_this, 0);
}

/* 024EC9D4 */
static u32 dAttention_c_nextAttention(u32 i_this, u32 interactMask) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    u32 pTarget = searchByID((u32)self->mLockonTargetID);
    dAttention_c_initList(i_this, interactMask);
    if (dAttention_c_makeList(i_this) != 0)
        dAttention_c_sortList(i_this);
    if (pTarget == att2_getActor(i_this + A_LLIST) && (s32)(u32)self->mLockonCount > 1)
        self->mLockOnOffset = 1;
    return dAttention_c_LockonTarget(i_this, 0);
}

/* 024ECA8C */
static s32 dAttention_c_freeAttention(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    self->mActionOffset = 0;
    self->mLockOnOffset = 0;
    self->mActionCount = 0;
    self->mLockonCount = 0;
    dAttention_c_initList(i_this, 0xFFFFFFFF);
    return 0;
}

// HD: chkAttMask (not named by the matcher)
/* 024ECAC8 */
static u32 dAttention_c_chkAttMask(u32 i_this, u32 type, u32 mask) {
    s32 num = (s32)(u32)*AttentionHD::view<be<u32>>(LOC_TYPE_NUM);
    for (s32 i = 0; i < num; i++) {
        if (type == (u32)(s32)AttentionMemory2::lds16(LOC_TYPE_TBL + i * 4))
            return mask & AttentionMemory2::ld16(LOC_TYPE_TBL + i * 4 + 2);
    }
    return 1;
}

/* 024ECB08 */
static bool dAttention_c_chaseAttention(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    gabi::Local<be<s16>> angle1;
    gabi::Local<be<s16>> res;
    gabi::Local<be<s16>> angle2;
    gabi::Local<be<u32>> type;
    gabi::Local<AttentionHD::Globe> globe1;
    gabi::Local<cXyz> d;
    gabi::Local<AttentionHD::Globe> globe2;
    att2_cSAngle_ct(gabi::ea(angle1.get()));
    att2_cSAngle_ct(gabi::ea(angle2.get()));
    u32 e = i_this + A_LLIST + (u32)self->mLockOnOffset * 0x10;
    u32 actor = att2_getActor(e);
    if (actor == 0)
        return false;
    att2_cXyz_mi(actor + AC_ATTN_POS, gabi::ea(d.get()), (u32)self->mpPlayer + AC_ATTN_POS);
    att2_cSGlobe_ct(gabi::ea(globe1.get()), gabi::ea(d.get()));
    att2_cSAngle_mi(gabi::ea(globe1.get()) + 6, gabi::ea(res.get()),
                    AttentionMemory2::lds16((u32)self->mpPlayer + AC_SHAPE_Y));
    *angle1 = (s16)*res;
    att2_cXyz_mi((u32)self->mpPlayer + AC_ATTN_POS, gabi::ea(d.get()), actor + AC_ATTN_POS);
    att2_cSGlobe_ct(gabi::ea(globe2.get()), gabi::ea(d.get()));
    att2_cSAngle_mi(gabi::ea(globe2.get()) + 6, gabi::ea(res.get()),
                    (s16)AttentionHD::view<fopAc_ac_c>(actor)->shape_angle.y);
    *angle2 = (s16)*res;
    f32 weight = att2_calcWeight(i_this, 'L', actor, globe1->mRadius, *angle1, *angle2,
                                 gabi::ea(type.get()));
    if (weight > 0.0f) {
        att2_setActor(e, actor);
        AttentionMemory2::stf(e + 4, globe1->mRadius);
        AttentionMemory2::stf(e + 0, weight);
        AttentionMemory2::st(e + 8, *type);
        return true;
    }
    u32 t = (u32)AttentionHD::view<AttentionHD::List>(e)->mType;
    *type = t;
    u32 flags = (u32)AttentionHD::view<fopAc_ac_c>(actor)->attention_info.flags;
    u8 idx = (u8)AttentionHD::view<fopAc_ac_c>(actor)->attention_info.distances[t];
    if (dAttention_c_chkAttMask(i_this, t, flags) == 0)
        return false;
    if (att2_check_event_condition(
            t, (u16)AttentionHD::view<fopAc_ac_c>(actor)->eventInfo.mCondition) != 0)
        return false;
    u32 dt = DIST_TABLE + idx * 0x1C;
    s16 a1 = *angle1;
    if (att2_check_flontofplayer(
            (u32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mFrontAngleCheckBits, a1, *angle2) !=
        0)
        return false;
    if (att2_check_distace((u32)self->mpPlayer + AC_ATTN_POS, a1, actor + AC_ATTN_POS,
                           (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDistXZMaxRelease,
                           (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDistXZAngleAdjust,
                           (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDeltaYMax,
                           (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDeltaYMin) == 0)
        return false;
    AttentionMemory2::stf(e + 0, att2_distace_weight(globe1->mRadius, *angle1, 0.5f));
    return true;
}

static inline bool player_busy() {
    if (AttentionMemory2::ld(gabi::ea(dComIfGp_get()) + 0x5CD8) & 0x37A02371)
        return true;
    return (AttentionMemory2::ld(gabi::ea(dComIfGp_get()) + 0x5CDC) & 0x11) != 0;
}

/* 024ECCFC */
static void dAttention_c_judgementButton(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    if (player_busy()) {
        u8 b = (u8)self->field_0x01a;
        if (b < 1)
            return;
        if (b <= 2)
            self->field_0x01a = 0;
        return;
    }
    switch ((u8)self->field_0x01a) {
    case 0:
        if (!att2_mDoCPd_L_LOCK_BUTTON((u32)self->mPadNo))
            break;
        self->field_0x01a = 1;
        break;
    case 1:
        self->field_0x01a = 2;
        /* fallthrough */
    case 2:
        if (!att2_mDoCPd_L_LOCK_BUTTON((u32)self->mPadNo))
            self->field_0x01a = 0;
        break;
    }
}

/* 024ECDDC */
static void dAttention_c_judgementTriggerProc(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    if (dAttention_c_chaseAttention(i_this)) {
        u32 f = (u32)self->mFlags;
        self->mLockOnState = 1;
        self->mFlags = f | 8;
    }
}

/* 024ECE24 */
static BOOL dAttention_c_judgementLostCheck(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    if (dAttention_c_chaseAttention(i_this))
        return FALSE;
    u32 f = (u32)self->mFlags;
    self->mLockOnState = 0;
    self->mFlags = f | 0x10;
    dAttention_c_freeAttention(i_this);
    self->mFlags = (u32)self->mFlags | 0x40;
    return TRUE;
}

/* 024ECE9C */
static u32 dAttention_c_LockonTargetPId(u32 i_this, s32 idx) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    s32 cnt = (s32)(u32)self->mLockonCount;
    if (idx >= cnt)
        return 0;
    s32 listIdx = (s32)(u32)self->mLockOnOffset + idx;
    if (listIdx >= cnt)
        listIdx -= cnt;
    return AttentionMemory2::ld(i_this + A_LLIST + listIdx * 0x10 + 0xC);
}

/* 024ECED4 */
static void dAttention_c_judgementStatusSw(u32 i_this, u32 interactMask) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    switch ((u8)self->mLockOnState) {
    case 0:
        self->mLockonTargetID = 0xFFFFFFFF;
        dAttention_c_stockAttention(i_this, interactMask);
        if ((u8)self->field_0x01a == 1)
            dAttention_c_judgementTriggerProc(i_this);
        break;
    case 1: {
        u32 id = dAttention_c_LockonTargetPId(i_this, 0);
        u8 b = (u8)self->field_0x01a;
        self->mLockonTargetID = id;
        if (b == 1) {
            f32 stickY = att2_CPad_GET_STICK_POS_Y((u32)self->mPadNo);
            if (-0.9f < stickY && dAttention_c_nextAttention(i_this, interactMask) != 0 &&
                (s32)(u32)self->mLockonCount > 1) {
                self->mFlags = (u32)self->mFlags | 8;
            } else {
                u32 f = (u32)self->mFlags;
                self->mLockOnState = 2;
                self->mFlags = f | 0x10;
            }
        } else {
            dAttention_c_judgementLostCheck(i_this);
        }
        break;
    }
    case 2: {
        u32 f = (u32)self->mFlags;
        u8 b = (u8)self->field_0x01a;
        self->mFlags = f | 0x40;
        if (b == 1) {
            self->mLockOnState = 0;
            dAttention_c_judgementTriggerProc(i_this);
        } else if (dAttention_c_LockonTarget(i_this, 0) == 0 || !((u32)self->mFlags & 0x40000000)) {
            self->mLockOnState = 0;
            dAttention_c_freeAttention(i_this);
        }
        break;
    }
    }
}

/* 024ED0A4 */
static void dAttention_c_judgementStatusHd(u32 i_this, u32 interactMask) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    switch ((u8)self->mLockOnState) {
    case 0:
        self->mLockonTargetID = 0xFFFFFFFF;
        dAttention_c_stockAttention(i_this, interactMask);
        if ((u8)self->field_0x01a == 1)
            dAttention_c_judgementTriggerProc(i_this);
        break;
    case 1:
        self->mLockonTargetID = dAttention_c_LockonTargetPId(i_this, 0);
        if (dAttention_c_judgementLostCheck(i_this) == 0 && (u8)self->field_0x01a == 0) {
            u32 f = (u32)self->mFlags;
            self->mLockOnState = 2;
            self->mFlags = f | 0x10;
        }
        break;
    case 2: {
        u32 f = (u32)self->mFlags;
        u8 b = (u8)self->field_0x01a;
        self->mFlags = f | 0x40;
        if (b == 1) {
            if (dAttention_c_nextAttention(i_this, interactMask) != 0) {
                u32 f2 = (u32)self->mFlags;
                self->mLockOnState = 1;
                self->mFlags = f2 | 8;
                break;
            }
        } else if (dAttention_c_LockonTarget(i_this, 0) != 0 && ((u32)self->mFlags & 0x40000000)) {
            break;
        }
        self->mLockOnState = 0;
        dAttention_c_freeAttention(i_this);
        break;
    }
    }
}

/* 024ED200 */
static void dAttention_c_runSoundProc(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    u32 f = (u32)self->mFlags;
    self->mEnemyID = 0xFFFFFFFF;
    self->mEnemyDistance = 10000.0f;
    if (f & 0x80000000)
        return;
    att2_fopAcIt_Executor(0x024EB88C /* sound_attention */, i_this);
    if (searchByID((u32)self->mEnemyID) != 0) {
        att2_mDoAud_bgmNowBattle((f32)self->mEnemyDistance * 0.1f);
        self->mFlags = (u32)self->mFlags | 0x100;
    }
}

/* WWHD glue (tools/verify/autoglue.py)
ext 020065FC void att2_cSAngle_ct(u32)
ext 02006908 void att2_cSAngle_mi(u32,u32,s16)
ext 02007324 void att2_cSGlobe_ct(u32,u32)
ext 0201ADE0 void att2_cXyz_mi(u32,u32,u32)
ext C000A848 void att2_memcpy(u32,u32,u32)
ext 025D51DC void att2_fopAcIt_Executor(u32,u32)
ext 02007CA4 BOOL att2_mDoCPd_L_LOCK_BUTTON(u32)
ext 02007990 f32 att2_CPad_GET_STICK_POS_Y(u32)
ext 025E1DD8 void att2_mDoAud_bgmNowBattle(f32)
ext 024EB2D4 f32 att2_calcWeight(u32,s32,u32,f32,s32,s32,u32)
ext 024EB524 void att2_setActor(u32,u32)
ext 024EBA14 u32 att2_getActor(u32)
ext 024EAFE4 s32 att2_check_event_condition(u32,u32)
ext 024EB030 s32 att2_check_flontofplayer(u32,s32,s32)
ext 024EB198 s32 att2_check_distace(u32,s32,u32,f32,f32,f32,f32)
ext 024EB0D8 f32 att2_distace_weight(f32,s32,f32)
ext 025D5218 u32 att2_searchActor(u32,u32)
*/
