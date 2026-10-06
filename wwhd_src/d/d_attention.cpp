/* d_attention: targeting / Z-lock (dAttention_c, dAttList_c, dAttDraw_c, dAttHint_c, dAttCatch_c,
 * dAttLook_c), WWHD. 
 *
 * Translation unit 024EAFE4..024EE638, from the image: the functions in GameCube order with the
 * HD inline copies interleaved, then __sinit (024EE58C) and the inline copies of this unit's
 * sead::SafeString vtable 100430F8 (deleting destructor 024EE620, assureTerminationImpl
 * 024EE634; referenced only from that vtable). Those three are verified in the d_bg_s unit,
 * which starts at 024EE638. 024EAF50 (verified here) is the __sinit of a function-less unit
 * before this one, probably d_att_dist (its .data is the three registration descriptors at
 * 101D3C9C followed directly by dist_table 101D3CC0, its .rodata only the pi pair 100430D8);
 * the blr at 024EAF4C is a slot of d_a_ykgr's SafeString vtable 10043040.
 * Ported from the GameCube d_attention.cpp; HD-only and compiler-generated functions are
 * written from the WWHD code.
 *
 * dAttention_c (HD 0x198, GameCube 0x190): unchanged up to 0x170; dAttParam_c mAttParam at
 * 0x170 is 0x28 in HD (8 HD bytes at +0x1C, zeroed by its HD default constructor; vtable at
 * +0x24, 1004320C). dAttDraw_CallBack_c vtable 1004321C. Tables (.data): dist_table 101D3CC0
 * (0x1C per entry), ang_table 101D5118, ftp_table 101D5120, ang_table2 101D5144, l_bpkIdx
 * 101D5150, loc_type_num 101D5180, act_type_num 101D5184, loc_type_tbl 101D5188,
 * act_type_tbl 101D5194.
 * Actor fields: attention_info at 0x388 (distances[] 0x388, position 0x390, flags 0x39C),
 * eventInfo condition 0xFA, shape_angle.y 0x32A, eyePos 0x37C, current.pos 0x314. */
#include "bindings.h"

namespace d_attention_cpp {

static constexpr u32 SAFESTRING_VTBL_ATT = 0x100430F8; /* this unit's sead::SafeString vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void memzero_l(u32 p, u32 n) { gabi::call(0x028F521C, p, n); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void cSAngle_ct_l(u32 a) { gabi::call(0x020065FC, a); }
/* cSAngle::operator-(s16) const: (this, result, value) */
static inline void cSAngle_mi_l(u32 a, u32 res, s16 v) { gabi::call(0x02006908, a, res, v); }
/* cSGlobe::cSGlobe(const cXyz&) */
static inline void cSGlobe_ct_l(u32 g, u32 v) { gabi::call(0x02007324, g, v); }
static inline void cXyz_mi_l(u32 a, u32 res, u32 b) { gabi::call(0x0201ADE0, a, res, b); }
static inline f32 PSVECSquareMag_l(u32 v) { return gabi::call<f32>(0x028E8DD0, v); }
static inline f32 sqrtf_l(f32 x) { return gabi::call<f32>(0x028F4384, x); }
static inline u32 getRes_l(u32 str, s32 idx) {
    gabi::Local<SafeString> key;
    key->mStringTop = str;
    key->__vtbl = SAFESTRING_VTBL_ATT;
    return gabi::call<u32>(0x026066C4, gabi::load<u32>(0x101F4F28), key.get(), idx);
}
/* mDoExt_McaMorf::setAnm(anm, mode, morf, speed, start, end, soundAnm) */
static inline void McaMorf_setAnm_l(u32 morf, u32 anm, s32 mode, u32 snd, f32 m, f32 spd, f32 st, f32 en) {
    gabi::call(0x025E4A98, morf, anm, mode, snd, m, spd, st, en);
}
static inline f32 fopAcM_searchActorDistance_l(u32 a, u32 b) { return gabi::call<f32>(0x025D68EC, a, b); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 getID(u32 p) { return p ? ld(p + 4) : 0xFFFFFFFFu; }
static inline u32 searchByID(u32 id) {
    gabi::Local<be<u32>> key;
    *key = id;
    if (id == 0xFFFFFFFFu) return 0;
    return gabi::call<u32>(0x025D5218, 0x025E1234u, key.get());
}
static inline u32 player0() { return ld(gabi::ea(dComIfGp_get()) + 0x5B2C); }

struct cXyz_l { be<f32> x, y, z; };
struct cSGlobe_l { be<f32> r; be<s16> v, u; };

enum : u32 {
    DIST_TABLE = 0x101D3CC0,
    ANG_TABLE = 0x101D5118,
    FTP_TABLE = 0x101D5120,
    ANG_TABLE2 = 0x101D5144,
    LOC_TYPE_NUM = 0x101D5180,
    ACT_TYPE_NUM = 0x101D5184,
    LOC_TYPE_TBL = 0x101D5188,
    ACT_TYPE_TBL = 0x101D5194,
    STR_ALWAYS_SETANM = 0x10043134,
};

/* actor fields */
enum : u32 { AC_ATTN_DIST = 0x388, AC_ATTN_POS = 0x390, AC_ATTN_FLAGS = 0x39C, AC_EVT_COND = 0xFA, AC_SHAPE_Y = 0x32A,
             AC_EYE = 0x37C, AC_POS = 0x314 };

/* 024EAF50: static initialisers (header statics) of the function-less unit before d_attention
 * (probably d_att_dist, see above) */
static void __sinit_d_attention_cpp() {
    WWHD_FUNC(0x024EAF50, void, (u32)0);
    sinit_header_statics(0x1046ECB8, 0x101D3C9C);
}
VERIFY(0x024EAF50, __sinit_d_attention_cpp);

/* 024EAFE4 */
static s32 check_event_condition(u32 attnType, u32 flags) {
    WWHD_FUNC(0x024EAFE4, s32, attnType, flags);
    switch (attnType) {
    case 1:
    case 3:
        if (flags & 1) break;
        return 1;
    case 5:
    case 6:
        if (flags & 4) break;
        return 1;
    }
    return 0;
}
VERIFY(0x024EAFE4, check_event_condition);

/* 024EB030 */
static s32 check_flontofplayer(u32 checkMask, s32 angle1, s32 angle2) {
    WWHD_FUNC(0x024EB030, s32, checkMask, angle1, angle2);
    if (angle1 < 0) angle1 = (s16)-angle1; /* GHS: the word is tested (callers pass sign-extended values) */
    if (angle2 < 0) angle2 = (s16)-angle2;
    for (int i = 0; i < 3; i++) {
        if (checkMask & ld(FTP_TABLE + i * 4)) {
            if (angle1 > lds16(ANG_TABLE + i * 2)) return 1;
        }
    }
    for (int i = 8; i > 2; i--) {
        if (checkMask & ld(FTP_TABLE + i * 4)) {
            if (angle2 > lds16(ANG_TABLE2 + (i - 3) * 2)) return 1;
        }
    }
    return 0;
}
VERIFY(0x024EB030, check_flontofplayer);

/* 024EB0D8: HD multiplies by 1/0x8000 */
static f32 distace_weight(f32 distance, s32 angle, f32 ratio) {
    WWHD_FUNC(0x024EB0D8, f32, distance, angle, ratio);
    f32 inv = 1.0f - ratio;
    f32 turns = (f32)angle * 3.0517578125e-05f;
    return distance * gabi::fmadds(ratio, turns * turns, inv);
}
VERIFY(0x024EB0D8, distace_weight);

/* 024EB12C */
static f32 distace_angle_adjust(f32 distance, s32 angle, f32 ratio) {
    WWHD_FUNC(0x024EB12C, f32, distance, angle, ratio);
    f32 turns = (f32)angle * 3.0517578125e-05f;
    if (turns < 0.0f) turns = -turns;
    f32 a = 1.0f - turns;
    f32 inv = 1.0f - ratio;
    return distance * gabi::fmadds(ratio, a * a, inv);
}
VERIFY(0x024EB12C, distace_angle_adjust);

/* 024EB198 */
static s32 check_distace(u32 playerPos, s32 angle, u32 actorPos, f32 maxDistXZBase, f32 maxDistAngleMul, f32 maxDeltaY,
                         f32 minDeltaY) {
    WWHD_FUNC(0x024EB198, s32, playerPos, angle, actorPos, maxDistXZBase, maxDistAngleMul, maxDeltaY, minDeltaY);
    gabi::Local<cXyz_l> dist;
    cXyz_mi_l(actorPos, gabi::ea(dist.get()), playerPos);
    f32 y = dist->y;
    if (!(y > minDeltaY)) return 0;
    if (!(y < maxDeltaY)) return 0;
    f32 adjust = maxDistXZBase + distace_angle_adjust(maxDistAngleMul, angle, 1.0f);
    gabi::Local<cXyz_l> xz;
    xz->x = (f32)dist->x;
    xz->y = 0.0f;
    xz->z = (f32)dist->z;
    f32 d = sqrtf_l(PSVECSquareMag_l(gabi::ea(xz.get())));
    if (adjust < d) return 0;
    return 1;
}
VERIFY(0x024EB198, check_distace);

/* 024EB2D4: HD: the distance weight is computed once before the loop */
static f32 dAttention_c_calcWeight(u32 i_this, s32 listType, u32 actor, f32 distance, s32 angle, s32 invAngle, u32 attnType) {
    WWHD_FUNC(0x024EB2D4, f32, i_this, listType, actor, distance, angle, invAngle, attnType);
    s32 num;
    u32 table;
    if (listType == 'L') {
        num = (s32)ld(LOC_TYPE_NUM);
        table = LOC_TYPE_TBL;
    } else {
        num = (s32)ld(ACT_TYPE_NUM);
        table = ACT_TYPE_TBL;
    }
    f32 weight = 0.0f;
    f32 max_weight = 0.0f;
    u32 player = player0();
    if (player != 0) {
        u32 id = gabi::call_ptr<u32>(ld(ld(player + 0xB4) + 0xBC), player);
        if (actor == searchByID(id)) return 0.0f;
    }
    if (num <= 0) return weight;
    f32 dw = distace_weight(distance, angle, 0.5f);
    for (s32 i = 0; i < num; i++) {
        u32 e = table + i * 4;
        u16 mask = ld16(e + 2);
        if (ld(i_this + 0x10) & mask & ld(actor + AC_ATTN_FLAGS)) {
            s16 type = lds16(e);
            u16 cond = ld16(actor + AC_EVT_COND);
            u8 idx = ld8(actor + AC_ATTN_DIST + type);
            if (check_event_condition((u32)(s32)type, cond) != 0) continue;
            u32 dt = DIST_TABLE + idx * 0x1C;
            if (check_flontofplayer(ld(dt + 0x18), angle, invAngle) != 0) continue;
            u32 p = ld(i_this);
            if (check_distace(p + AC_ATTN_POS, angle, actor + AC_ATTN_POS, ldf(dt + 0), ldf(dt + 8), ldf(dt + 0xC),
                              ldf(dt + 0x10)) == 0)
                continue;
            if (!(dw > 0.0f)) continue;
            f32 w = ldf(dt + 0x14);
            if (!(w > max_weight)) continue;
            max_weight = w;
            weight = dw / max_weight;
            st(attnType, (u32)(s32)lds16(e));
        }
    }
    return weight;
}
VERIFY(0x024EB2D4, dAttention_c_calcWeight);

/* 024EB524 */
static void dAttList_c_setActor(u32 i_this, u32 actor) {
    WWHD_FUNC(0x024EB524, void, i_this, actor);
    st(i_this + 0xC, getID(actor));
}
VERIFY(0x024EB524, dAttList_c_setActor);

static inline void setList(u32 i_this, u32 cntOff, u32 listOff, s32 n, u32 actor, f32 weight, f32 dist, u32 type) {
    if (!(weight > 0.0f)) return;
    s32 cnt = (s32)ld(i_this + cntOff);
    u32 list = i_this + listOff;
    s32 maxIndex;
    if (cnt < n) {
        st(i_this + cntOff, (u32)(cnt + 1));
        maxIndex = cnt;
    } else {
        f32 best = 0.0f;
        maxIndex = 0;
        for (s32 i = 0; i < n; i++) {
            f32 w = ldf(list + i * 0x10);
            if (w > best) {
                best = w;
                maxIndex = i;
            }
        }
    }
    u32 e = list + maxIndex * 0x10;
    if (!(ldf(e) > weight)) return;
    dAttList_c_setActor(e, actor);
    stf(e + 4, dist);
    stf(e + 0, weight);
    st(e + 8, type);
}

/* 024EB53C */
static void dAttention_c_setLList(u32 i_this, u32 actor, f32 weight, f32 dist, u32 type) {
    WWHD_FUNC(0x024EB53C, void, i_this, actor, weight, dist, type);
    setList(i_this, 0xD4, 0x54, 8, actor, weight, dist, type);
}
VERIFY(0x024EB53C, dAttention_c_setLList);

/* 024EB5F0 */
static void dAttention_c_setAList(u32 i_this, u32 actor, f32 weight, f32 dist, u32 type) {
    WWHD_FUNC(0x024EB5F0, void, i_this, actor, weight, dist, type);
    setList(i_this, 0x11C, 0xDC, 4, actor, weight, dist, type);
}
VERIFY(0x024EB5F0, dAttention_c_setAList);

/* 024EB6A4 */
static s32 dAttention_c_SelectAttention(u32 i_this, u32 ac) {
    WWHD_FUNC(0x024EB6A4, s32, i_this, ac);
    gabi::Local<be<s16>> angle1;
    gabi::Local<be<s16>> angle2;
    gabi::Local<be<s16>> res;
    gabi::Local<be<u32>> type;
    gabi::Local<cXyz_l> d;
    gabi::Local<cSGlobe_l> globe1;
    gabi::Local<cSGlobe_l> globe2;
    cSAngle_ct_l(gabi::ea(angle1.get()));
    cSAngle_ct_l(gabi::ea(angle2.get()));
    u32 p = ld(i_this);
    if (ac == p || p == 0) return 0;
    st(i_this + 0x10, ld(p + AC_ATTN_FLAGS));
    cXyz_mi_l(ac + AC_ATTN_POS, gabi::ea(d.get()), ld(i_this) + AC_ATTN_POS);
    cSGlobe_ct_l(gabi::ea(globe1.get()), gabi::ea(d.get()));
    cSAngle_mi_l(gabi::ea(globe1.get()) + 6, gabi::ea(res.get()), lds16(ld(i_this) + AC_SHAPE_Y));
    *angle1 = (s16)*res;
    cXyz_mi_l(ld(i_this) + AC_ATTN_POS, gabi::ea(d.get()), ac + AC_ATTN_POS);
    cSGlobe_ct_l(gabi::ea(globe2.get()), gabi::ea(d.get()));
    cSAngle_mi_l(gabi::ea(globe2.get()) + 6, gabi::ea(res.get()), lds16(ac + AC_SHAPE_Y));
    *angle2 = (s16)*res;
    f32 weight = dAttention_c_calcWeight(i_this, 'L', ac, globe1->r, *angle1, *angle2, gabi::ea(type.get()));
    dAttention_c_setLList(i_this, ac, weight, globe1->r, *type);
    weight = dAttention_c_calcWeight(i_this, 'A', ac, globe1->r, *angle1, *angle2, gabi::ea(type.get()));
    dAttention_c_setAList(i_this, ac, weight, globe1->r, *type);
    return 0;
}
VERIFY(0x024EB6A4, dAttention_c_SelectAttention);

/* 024EB7E0: select_attention (fopAcIt_Executor callback) */
static s32 select_attention(u32 actor, u32 i_attention) {
    WWHD_FUNC(0x024EB7E0, s32, actor, i_attention);
    return dAttention_c_SelectAttention(i_attention, actor);
}
VERIFY(0x024EB7E0, select_attention);

/* 024EB7F0: HD: fopAcM_GetProfName is null-checked */
static f32 dAttention_c_EnemyDistance(u32 i_this, u32 actor) {
    WWHD_FUNC(0x024EB7F0, f32, i_this, actor);
    u32 p = ld(i_this);
    if (actor == p || p == 0) return -1.0f;
    if (actor != 0 && lds16(actor + 0xE) == 0xA8) return -1.0f;
    if (!(ld(actor + AC_ATTN_FLAGS) & 0x04000004)) return -1.0f;
    f32 dist = fopAcM_searchActorDistance_l(p, actor);
    u32 dt = DIST_TABLE + ld8(actor + AC_ATTN_DIST + 2) * 0x1C;
    if (dist < ldf(dt) + ldf(dt + 8)) return dist;
    return -1.0f;
}
VERIFY(0x024EB7F0, dAttention_c_EnemyDistance);

/* 024EB88C */
static s32 sound_attention(u32 actor, u32 userWork) {
    WWHD_FUNC(0x024EB88C, s32, actor, userWork);
    f32 dist = dAttention_c_EnemyDistance(userWork, actor);
    if (dist < 0.0f) return 0;
    if (dist < ldf(userWork + 0x16C)) {
        u32 id = getID(actor);
        stf(userWork + 0x16C, dist);
        st(userWork + 0x168, id);
    }
    return 0;
}
VERIFY(0x024EB88C, sound_attention);

/* 024EB904: HD dAttParam_c() (value-initialising default constructor) */
static u32 dAttParam_c_ct0(u32 i_this) {
    WWHD_FUNC(0x024EB904, u32, i_this);
    if (i_this == 0) {
        i_this = operator_new_l(0x28);
        if (i_this == 0) return 0;
    }
    st16(i_this + 0, 0);
    stf(i_this + 0x04, 0.0f);
    stf(i_this + 0x08, 0.0f);
    stf(i_this + 0x0C, 0.0f);
    stf(i_this + 0x10, 0.0f);
    stf(i_this + 0x14, 0.0f);
    stf(i_this + 0x18, 0.0f);
    st(i_this + 0x24, 0x1004320C);
    memzero_l(i_this + 0x1C, 8);
    return i_this;
}
VERIFY(0x024EB904, dAttParam_c_ct0);

/* 024EB984 */
static u32 dAttParam_c_ct(u32 i_this, s32) {
    WWHD_FUNC(0x024EB984, u32, i_this, (s32)0);
    if (i_this == 0) {
        i_this = operator_new_l(0x28);
        if (i_this == 0) return 0;
    }
    st(i_this + 0x24, 0x1004320C);
    stf(i_this + 0x04, 45.0f);
    stf(i_this + 0x08, 30.0f);
    stf(i_this + 0x0C, 90.0f);
    st16(i_this + 0, 1);
    stf(i_this + 0x18, -0.9f);
    stf(i_this + 0x10, 3000.0f);
    stf(i_this + 0x14, 1000.0f);
    return i_this;
}
VERIFY(0x024EB984, dAttParam_c_ct);

/* 024EBA14 */
static u32 dAttList_c_getActor(u32 i_this) {
    WWHD_FUNC(0x024EBA14, u32, i_this);
    return searchByID(ld(i_this + 0xC));
}
VERIFY(0x024EBA14, dAttList_c_getActor);

/* 024EBA54 */
static u32 dAttHint_c_getPId(u32 i_this, u32 proc) {
    WWHD_FUNC(0x024EBA54, u32, i_this, proc);
    return getID(proc);
}
VERIFY(0x024EBA54, dAttHint_c_getPId);

/* 024EBA68 */
static s32 dAttHint_c_request(u32 i_this, u32 actor, s32 priority) {
    WWHD_FUNC(0x024EBA68, s32, i_this, actor, priority);
    s32 cur = (s32)ld(i_this + 4);
    if (priority < 0) priority = 0x1FF;
    if (priority <= cur) {
        u32 id = dAttHint_c_getPId(i_this, actor);
        st(i_this + 4, (u32)priority);
        st(i_this + 0, id);
    }
    return 1;
}
VERIFY(0x024EBA68, dAttHint_c_request);

/* 024EBAB4 */
static void dAttHint_c_init(u32 i_this) {
    WWHD_FUNC(0x024EBAB4, void, i_this);
    st(i_this + 0, 0xFFFFFFFF);
    st(i_this + 4, 0x200);
    st(i_this + 8, 0xFFFFFFFF);
}
VERIFY(0x024EBAB4, dAttHint_c_init);

/* 024EBACC */
static void dAttHint_c_proc(u32 i_this) {
    WWHD_FUNC(0x024EBACC, void, i_this);
    u32 id = ld(i_this + 0);
    st(i_this + 0, 0xFFFFFFFF);
    st(i_this + 4, 0x200);
    st(i_this + 8, id);
}
VERIFY(0x024EBACC, dAttHint_c_proc);

/* 024EBAE8 */
static u32 dAttCatch_c_convPId(u32 i_this, u32 id) {
    WWHD_FUNC(0x024EBAE8, u32, i_this, id);
    return searchByID(id);
}
VERIFY(0x024EBAE8, dAttCatch_c_convPId);

/* 024EBB24 */
static void dAttCatch_c_init(u32 i_this) {
    WWHD_FUNC(0x024EBB24, void, i_this);
    st8(i_this + 0xC, 0x56);
    st(i_this + 0, 0xFFFFFFFF);
    st(i_this + 4, 3);
    st(i_this + 0x10, 0xFFFFFFFF);
}
VERIFY(0x024EBB24, dAttCatch_c_init);

/* 024EBB44 */
static void dAttCatch_c_proc(u32 i_this) {
    WWHD_FUNC(0x024EBB44, void, i_this);
    u32 id = ld(i_this + 0);
    u8 item = ld8(i_this + 0xC);
    st(i_this + 0x10, id);
    st(i_this + 4, 3);
    st8(i_this + 0xC, 0x56);
    st8(i_this + 0x14, item);
    st(i_this + 0, 0xFFFFFFFF);
}
VERIFY(0x024EBB44, dAttCatch_c_proc);

/* |s16 angle of (v) - player shape angle y| through cSGlobe / cSAngle */
static inline s16 globe_angle(u32 v, u32 player) {
    gabi::Local<cSGlobe_l> globe;
    gabi::Local<be<s16>> res;
    cSGlobe_ct_l(gabi::ea(globe.get()), v);
    cSAngle_mi_l(gabi::ea(globe.get()) + 6, gabi::ea(res.get()), lds16(player + AC_SHAPE_Y));
    s16 a = *res;
    if (a < 0) a = (s16)-a;
    return a;
}

static inline f32 absXZ(u32 v) {
    gabi::Local<cXyz_l> xz;
    xz->x = ldf(v + 0);
    xz->y = 0.0f;
    xz->z = ldf(v + 8);
    return sqrtf_l(PSVECSquareMag_l(gabi::ea(xz.get())));
}

/* 024EBB70 */
static bool dAttCatch_c_request(u32 i_this, u32 reqActor, u8 itemNo, f32 horizontalDist, f32 upDist, f32 downDist,
                                s32 angle, s32 param_7) {
    WWHD_FUNC(0x024EBB70, bool, i_this, reqActor, itemNo, horizontalDist, upDist, downDist, angle, param_7);
    u32 player = player0();
    if (param_7 > (s32)ld(i_this + 4)) return false;
    gabi::Local<cXyz_l> v;
    cXyz_mi_l(reqActor + AC_ATTN_POS, gabi::ea(v.get()), player + AC_ATTN_POS);
    f32 y = v->y;
    if (y < downDist) return false;
    if (y > upDist) return false;
    f32 dist = absXZ(gabi::ea(v.get()));
    if (dist > horizontalDist) return false;
    if (angle != 0) {
        if (globe_angle(gabi::ea(v.get()), player) > angle) return false;
    }
    if (param_7 < (s32)ld(i_this + 4) || dist < ldf(i_this + 8)) {
        st(i_this + 4, (u32)param_7);
        st8(i_this + 0xC, itemNo);
        u32 id = getID(reqActor);
        stf(i_this + 8, dist);
        st(i_this + 0, id);
        return true;
    }
    return false;
}
VERIFY(0x024EBB70, dAttCatch_c_request);

/* 024EBD10 */
static u32 dAttLook_c_convPId(u32 i_this, u32 id) {
    WWHD_FUNC(0x024EBD10, u32, i_this, id);
    return searchByID(id);
}
VERIFY(0x024EBD10, dAttLook_c_convPId);

/* 024EBD4C */
static void dAttLook_c_init(u32 i_this) {
    WWHD_FUNC(0x024EBD4C, void, i_this);
    st(i_this + 0, 0xFFFFFFFF);
    st(i_this + 4, 3);
    st(i_this + 0xC, 0xFFFFFFFF);
}
VERIFY(0x024EBD4C, dAttLook_c_init);

/* 024EBD64 */
static void dAttLook_c_proc(u32 i_this) {
    WWHD_FUNC(0x024EBD64, void, i_this);
    u32 id = ld(i_this + 0);
    st(i_this + 0, 0xFFFFFFFF);
    st(i_this + 4, 3);
    st(i_this + 0xC, id);
}
VERIFY(0x024EBD64, dAttLook_c_proc);

/* the angle check of dAttLook_c::request/requestF: on the positions, copied into v */
static inline bool look_angle_fails(u32 v, u32 reqActor, u32 player, s32 angle) {
    gabi::Local<cXyz_l> d;
    cXyz_mi_l(reqActor + AC_POS, gabi::ea(d.get()), player + AC_POS);
    u32 x = ld(gabi::ea(d.get()) + 0), y = ld(gabi::ea(d.get()) + 4), z = ld(gabi::ea(d.get()) + 8);
    st(v + 8, z);
    st(v + 4, y);
    st(v + 0, x);
    return globe_angle(v, player) > angle;
}

/* 024EBD80 */
static bool dAttLook_c_request(u32 i_this, u32 reqActor, f32 horizontalDist, f32 upDist, f32 downDist, s32 angle,
                               s32 param_6) {
    WWHD_FUNC(0x024EBD80, bool, i_this, reqActor, horizontalDist, upDist, downDist, angle, param_6);
    u32 player = player0();
    if (param_6 > (s32)ld(i_this + 4)) return false;
    gabi::Local<cXyz_l> v;
    cXyz_mi_l(reqActor + AC_EYE, gabi::ea(v.get()), player + AC_EYE);
    f32 y = v->y;
    if (y < downDist) return false;
    if (y > upDist) return false;
    f32 dist = absXZ(gabi::ea(v.get()));
    if (dist > horizontalDist) return false;
    if (angle != 0) {
        if (look_angle_fails(gabi::ea(v.get()), reqActor, player, angle)) return false;
    }
    if (param_6 < (s32)ld(i_this + 4) || dist < ldf(i_this + 8)) {
        u32 id = getID(reqActor);
        st(i_this + 4, (u32)param_6);
        stf(i_this + 8, dist);
        st(i_this + 0, id);
        return true;
    }
    return false;
}
VERIFY(0x024EBD80, dAttLook_c_request);

/* 024EBF40 */
static bool dAttLook_c_requestF(u32 i_this, u32 reqActor, s32 angle, s32 param_3) {
    WWHD_FUNC(0x024EBF40, bool, i_this, reqActor, angle, param_3);
    u32 player = player0();
    if (param_3 > (s32)ld(i_this + 4)) return false;
    gabi::Local<cXyz_l> v;
    cXyz_mi_l(reqActor + AC_EYE, gabi::ea(v.get()), player + AC_EYE);
    f32 dist = absXZ(gabi::ea(v.get()));
    if (angle != 0) {
        if (look_angle_fails(gabi::ea(v.get()), reqActor, player, angle)) return false;
    }
    if (param_3 < (s32)ld(i_this + 4) || dist < ldf(i_this + 8)) {
        u32 id = getID(reqActor);
        st(i_this + 4, (u32)param_3);
        stf(i_this + 8, dist);
        st(i_this + 0, id);
        return true;
    }
    return false;
}
VERIFY(0x024EBF40, dAttLook_c_requestF);

/* 024EC0A0 */
static u32 dAttHint_c_convPId(u32 i_this, u32 id) {
    WWHD_FUNC(0x024EC0A0, u32, i_this, id);
    return searchByID(id);
}
VERIFY(0x024EC0A0, dAttHint_c_convPId);

/* 024EC0DC: the HD caller passes the timing zero-extended; the word is tested */
static bool dAttDraw_CallBack_c_execute(u32 i_this, u32 timing, u32 xform) {
    WWHD_FUNC(0x024EC0DC, bool, i_this, timing, xform);
    if (timing == 0) {
        f32 s = ldf(0x1047B9B4) + 0.6f; /* REG6_F(17) + 0.6f */
        stf(xform + 0x18, ldf(xform + 0x18) * s);
    }
    return true;
}
VERIFY(0x024EC0DC, dAttDraw_CallBack_c_execute);

/* 024EC10C */
static void dAttDraw_c_setAnm(u32 i_this, s32 resIdxTransform, s32 resIdxColor, s32 loopMode) {
    WWHD_FUNC(0x024EC10C, void, i_this, resIdxTransform, resIdxColor, loopMode);
    u32 anm = getRes_l(STR_ALWAYS_SETANM, resIdxTransform);
    McaMorf_setAnm_l(ld(i_this), anm, loopMode, 0, 0.0f, 1.0f, 0.0f, -1.0f);
    if (resIdxColor < 0) {
        st(i_this + 4, 0);
    } else {
        st(i_this + 4, getRes_l(STR_ALWAYS_SETANM, resIdxColor));
    }
}
VERIFY(0x024EC10C, dAttDraw_c_setAnm);

} // namespace d_attention_cpp
