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
#include "d/d_attention.h"

// HD: endian-aware image accesses retained for word copies and dynamic virtual slots.
namespace AttentionMemory1 {
static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
} // namespace AttentionMemory1

u32 att1_getObjectRes(u32, u32, s32);
u32 att1_searchActor(u32, u32);

static constexpr u32 SAFESTRING_VTBL_ATT = 0x100430F8; /* this unit's sead::SafeString vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
void att1_memzero(u32 p, u32 n);
u32 att1_operator_new(u32 size);
void att1_cSAngle_ct(u32 a);
/* cSAngle::operator-(s16) const: (this, result, value) */
void att1_cSAngle_mi(u32 a, u32 res, s16 v);
/* cSGlobe::cSGlobe(const cXyz&) */
void att1_cSGlobe_ct(u32 g, u32 v);
void att1_cXyz_mi(u32 a, u32 res, u32 b);
f32 att1_PSVECSquareMag(u32 v);
f32 att1_sqrtf(f32 x);
static inline u32 getRes_l(u32 str, s32 idx) {
    gabi::Local<SafeString> key;
    key->mStringTop = str;
    key->__vtbl = SAFESTRING_VTBL_ATT;
    return att1_getObjectRes(gabi::load<u32>(0x101F4F28), gabi::ea(key.get()), idx);
}
/* mDoExt_McaMorf::setAnm(anm, mode, morf, speed, start, end, soundAnm) */
void att1_McaMorf_setAnm(u32 morf, u32 anm, s32 mode, u32 snd, f32 m, f32 spd, f32 st, f32 en);
f32 att1_fopAcM_searchActorDistance(u32 a, u32 b);

static inline u32 getID(u32 p) { return p ? AttentionMemory1::ld(p + 4) : 0xFFFFFFFFu; }
static inline u32 searchByID(u32 id) {
    gabi::Local<be<u32>> key;
    *key = id;
    if (id == 0xFFFFFFFFu)
        return 0;
    return att1_searchActor(0x025E1234u, gabi::ea(key.get()));
}
static inline u32 player0() { return AttentionMemory1::ld(gabi::ea(dComIfGp_get()) + 0x5B2C); }

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
enum : u32 {
    AC_ATTN_DIST = 0x388,
    AC_ATTN_POS = 0x390,
    AC_ATTN_FLAGS = 0x39C,
    AC_EVT_COND = 0xFA,
    AC_SHAPE_Y = 0x32A,
    AC_EYE = 0x37C,
    AC_POS = 0x314
};

// HD: static initialisers (header statics) of the function-less unit before d_attention
// (probably d_att_dist, see above)
/* 024EAF50 */
static void __sinit_d_attention_cpp() { sinit_header_statics(0x1046ECB8, 0x101D3C9C); }

/* 024EAFE4 */
static s32 check_event_condition(u32 attnType, u32 flags) {
    switch (attnType) {
    case fopAc_Attn_TYPE_TALK_e:
    case fopAc_Attn_TYPE_SPEAK_e:
        if (flags & dEvtCnd_CANTALK_e)
            break;
        return 1;
    case fopAc_Attn_TYPE_DOOR_e:
    case fopAc_Attn_TYPE_TREASURE_e:
        if (flags & dEvtCnd_CANDOOR_e)
            break;
        return 1;
    }
    return 0;
}

/* 024EB030 */
static s32 check_flontofplayer(u32 checkMask, s32 angle1, s32 angle2) {
    if (angle1 < 0)
        angle1 = (s16)-angle1; /* GHS: the word is tested (callers pass sign-extended values) */
    if (angle2 < 0)
        angle2 = (s16)-angle2;
    for (int i = 0; i < 3; i++) {
        if (checkMask & (u32)AttentionHD::view<be<u32>>(FTP_TABLE)[i]) {
            if (angle1 > (s16)AttentionHD::view<be<s16>>(ANG_TABLE)[i])
                return 1;
        }
    }
    for (int i = 8; i > 2; i--) {
        if (checkMask & (u32)AttentionHD::view<be<u32>>(FTP_TABLE)[i]) {
            if (angle2 > (s16)AttentionHD::view<be<s16>>(ANG_TABLE2)[i - 3])
                return 1;
        }
    }
    return 0;
}

// HD: HD multiplies by 1/0x8000
/* 024EB0D8 */
static f32 distace_weight(f32 distance, s32 angle, f32 ratio) {
    f32 inv = 1.0f - ratio;
    f32 turns = (f32)angle * 3.0517578125e-05f;
    return distance * gabi::fmadds(ratio, turns * turns, inv);
}

/* 024EB12C */
static f32 distace_angle_adjust(f32 distance, s32 angle, f32 ratio) {
    f32 turns = (f32)angle * 3.0517578125e-05f;
    if (turns < 0.0f)
        turns = -turns;
    f32 a = 1.0f - turns;
    f32 inv = 1.0f - ratio;
    return distance * gabi::fmadds(ratio, a * a, inv);
}

/* 024EB198 */
static s32 check_distace(u32 playerPos, s32 angle, u32 actorPos, f32 maxDistXZBase,
                         f32 maxDistAngleMul, f32 maxDeltaY, f32 minDeltaY) {
    gabi::Local<cXyz> dist;
    att1_cXyz_mi(actorPos, gabi::ea(dist.get()), playerPos);
    f32 y = dist->y;
    if (!(y > minDeltaY))
        return 0;
    if (!(y < maxDeltaY))
        return 0;
    f32 adjust = maxDistXZBase + distace_angle_adjust(maxDistAngleMul, angle, 1.0f);
    gabi::Local<cXyz> xz;
    xz->x = (f32)dist->x;
    xz->y = 0.0f;
    xz->z = (f32)dist->z;
    f32 d = att1_sqrtf(att1_PSVECSquareMag(gabi::ea(xz.get())));
    if (adjust < d)
        return 0;
    return 1;
}

// HD: the distance weight is computed once before the loop
/* 024EB2D4 */
static f32 dAttention_c_calcWeight(u32 i_this, s32 listType, u32 actor, f32 distance, s32 angle,
                                   s32 invAngle, u32 attnType) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    s32 num;
    u32 table;
    if (listType == 'L') {
        num = (s32)(u32)*AttentionHD::view<be<u32>>(LOC_TYPE_NUM);
        table = LOC_TYPE_TBL;
    } else {
        num = (s32)(u32)*AttentionHD::view<be<u32>>(ACT_TYPE_NUM);
        table = ACT_TYPE_TBL;
    }
    f32 weight = 0.0f;
    f32 max_weight = 0.0f;
    u32 player = player0();
    if (player != 0) {
        u32 id = gabi::call_ptr<u32>(
            AttentionMemory1::ld(AttentionMemory1::ld(player + 0xB4) + 0xBC), player);
        if (actor == searchByID(id))
            return 0.0f;
    }
    if (num <= 0)
        return weight;
    f32 dw = distace_weight(distance, angle, 0.5f);
    for (s32 i = 0; i < num; i++) {
        u32 e = table + i * 4;
        u16 mask = (u16)AttentionHD::view<AttentionHD::LocTbl>(e)->mMask;
        if ((u32)self->mFlagMask & mask &
            (u32)AttentionHD::view<fopAc_ac_c>(actor)->attention_info.flags) {
            s16 type = (s16)AttentionHD::view<AttentionHD::LocTbl>(e)->mType;
            u16 cond = (u16)AttentionHD::view<fopAc_ac_c>(actor)->eventInfo.mCondition;
            u8 idx = (u8)AttentionHD::view<fopAc_ac_c>(actor)->attention_info.distances[type];
            if (check_event_condition((u32)(s32)type, cond) != 0)
                continue;
            u32 dt = DIST_TABLE + idx * 0x1C;
            if (check_flontofplayer(
                    (u32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mFrontAngleCheckBits, angle,
                    invAngle) != 0)
                continue;
            u32 p = (u32)self->mpPlayer;
            if (check_distace(p + AC_ATTN_POS, angle, actor + AC_ATTN_POS,
                              (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDistXZMax,
                              (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDistXZAngleAdjust,
                              (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDeltaYMax,
                              (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDeltaYMin) == 0)
                continue;
            if (!(dw > 0.0f))
                continue;
            f32 w = (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mWeightDivisor;
            if (!(w > max_weight))
                continue;
            max_weight = w;
            weight = dw / max_weight;
            AttentionMemory1::st(attnType,
                                 (u32)(s32)(s16)AttentionHD::view<AttentionHD::LocTbl>(e)->mType);
        }
    }
    return weight;
}

/* 024EB524 */
static void dAttList_c_setActor(u32 i_this, u32 actor) {
    auto* self = AttentionHD::view<AttentionHD::List>(i_this);
    self->mActorID = getID(actor);
}

static inline void setList(u32 i_this, u32 cntOff, u32 listOff, s32 n, u32 actor, f32 weight,
                           f32 dist, u32 type) {
    if (!(weight > 0.0f))
        return;
    auto* count = AttentionHD::view<be<s32>>(i_this + cntOff);
    s32 cnt = *count;
    u32 list = i_this + listOff;
    s32 maxIndex;
    if (cnt < n) {
        *count = cnt + 1;
        maxIndex = cnt;
    } else {
        f32 best = 0.0f;
        maxIndex = 0;
        for (s32 i = 0; i < n; i++) {
            f32 w = (f32)AttentionHD::view<AttentionHD::List>(list)[i].mWeight;
            if (w > best) {
                best = w;
                maxIndex = i;
            }
        }
    }
    u32 e = list + maxIndex * 0x10;
    if (!((f32)AttentionHD::view<AttentionHD::List>(e)->mWeight > weight))
        return;
    dAttList_c_setActor(e, actor);
    AttentionHD::view<AttentionHD::List>(e)->mDistance = dist;
    AttentionHD::view<AttentionHD::List>(e)->mWeight = weight;
    AttentionHD::view<AttentionHD::List>(e)->mType = type;
}

/* 024EB53C */
static void dAttention_c_setLList(u32 i_this, u32 actor, f32 weight, f32 dist, u32 type) {
    setList(i_this, 0xD4, 0x54, 8, actor, weight, dist, type);
}

/* 024EB5F0 */
static void dAttention_c_setAList(u32 i_this, u32 actor, f32 weight, f32 dist, u32 type) {
    setList(i_this, 0x11C, 0xDC, 4, actor, weight, dist, type);
}

/* 024EB6A4 */
static s32 dAttention_c_SelectAttention(u32 i_this, u32 ac) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    gabi::Local<be<s16>> angle1;
    gabi::Local<be<s16>> angle2;
    gabi::Local<be<s16>> res;
    gabi::Local<be<u32>> type;
    gabi::Local<cXyz> d;
    gabi::Local<AttentionHD::Globe> globe1;
    gabi::Local<AttentionHD::Globe> globe2;
    att1_cSAngle_ct(gabi::ea(angle1.get()));
    att1_cSAngle_ct(gabi::ea(angle2.get()));
    u32 p = (u32)self->mpPlayer;
    if (ac == p || p == 0)
        return 0;
    self->mFlagMask = (u32)AttentionHD::view<fopAc_ac_c>(p)->attention_info.flags;
    att1_cXyz_mi(ac + AC_ATTN_POS, gabi::ea(d.get()), (u32)self->mpPlayer + AC_ATTN_POS);
    att1_cSGlobe_ct(gabi::ea(globe1.get()), gabi::ea(d.get()));
    att1_cSAngle_mi(gabi::ea(globe1.get()) + 6, gabi::ea(res.get()),
                    AttentionMemory1::lds16((u32)self->mpPlayer + AC_SHAPE_Y));
    *angle1 = (s16)*res;
    att1_cXyz_mi((u32)self->mpPlayer + AC_ATTN_POS, gabi::ea(d.get()), ac + AC_ATTN_POS);
    att1_cSGlobe_ct(gabi::ea(globe2.get()), gabi::ea(d.get()));
    att1_cSAngle_mi(gabi::ea(globe2.get()) + 6, gabi::ea(res.get()),
                    (s16)AttentionHD::view<fopAc_ac_c>(ac)->shape_angle.y);
    *angle2 = (s16)*res;
    f32 weight = dAttention_c_calcWeight(i_this, 'L', ac, globe1->mRadius, *angle1, *angle2,
                                         gabi::ea(type.get()));
    dAttention_c_setLList(i_this, ac, weight, globe1->mRadius, *type);
    weight = dAttention_c_calcWeight(i_this, 'A', ac, globe1->mRadius, *angle1, *angle2,
                                     gabi::ea(type.get()));
    dAttention_c_setAList(i_this, ac, weight, globe1->mRadius, *type);
    return 0;
}

// HD: select_attention (fopAcIt_Executor callback)
/* 024EB7E0 */
static s32 select_attention(u32 actor, u32 i_attention) {
    return dAttention_c_SelectAttention(i_attention, actor);
}

// HD: fopAcM_GetProfName is null-checked
/* 024EB7F0 */
static f32 dAttention_c_EnemyDistance(u32 i_this, u32 actor) {
    auto* self = AttentionHD::view<AttentionHD::Attention>(i_this);
    u32 p = (u32)self->mpPlayer;
    if (actor == p || p == 0)
        return -1.0f;
    if (actor != 0 && AttentionMemory1::lds16(actor + 0xE) == 0xA8)
        return -1.0f;
    if (!((u32)AttentionHD::view<fopAc_ac_c>(actor)->attention_info.flags &
          (fopAc_Attn_LOCKON_BATTLE_e | fopAc_Attn_ENEMYFLAG_NOLOCKON_e)))
        return -1.0f;
    f32 dist = att1_fopAcM_searchActorDistance(p, actor);
    u32 dt = DIST_TABLE + (u8)AttentionHD::view<fopAc_ac_c>(actor)
                                  ->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] *
                              0x1C;
    if (dist < (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDistXZMax +
                   (f32)AttentionHD::view<AttentionHD::DistTbl>(dt)->mDistXZAngleAdjust)
        return dist;
    return -1.0f;
}

/* 024EB88C */
static s32 sound_attention(u32 actor, u32 userWork) {
    f32 dist = dAttention_c_EnemyDistance(userWork, actor);
    if (dist < 0.0f)
        return 0;
    if (dist < (f32)AttentionHD::view<AttentionHD::Attention>(userWork)->mEnemyDistance) {
        u32 id = getID(actor);
        AttentionHD::view<AttentionHD::Attention>(userWork)->mEnemyDistance = dist;
        AttentionHD::view<AttentionHD::Attention>(userWork)->mEnemyID = id;
    }
    return 0;
}

// HD: HD dAttParam_c() (value-initialising default constructor)
/* 024EB904 */
static u32 dAttParam_c_ct0(u32 i_this) {
    if (i_this == 0) {
        i_this = att1_operator_new(0x28);
        if (i_this == 0)
            return 0;
    }
    AttentionHD::view<AttentionHD::Param>(i_this)->mFlags = 0;
    AttentionHD::view<AttentionHD::Param>(i_this)->field_0x04 = 0.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->field_0x08 = 0.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->field_0x0c = 0.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->mDangerBGMDistance = 0.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->mBGMDistMargin = 0.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->mSWModeDisable = 0.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->__vtbl = 0x1004320C;
    att1_memzero(i_this + 0x1C, 8);
    return i_this;
}

/* 024EB984 */
static u32 dAttParam_c_ct(u32 i_this, s32 unused) {
    if (i_this == 0) {
        i_this = att1_operator_new(0x28);
        if (i_this == 0)
            return 0;
    }
    AttentionHD::view<AttentionHD::Param>(i_this)->__vtbl = 0x1004320C;
    AttentionHD::view<AttentionHD::Param>(i_this)->field_0x04 = 45.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->field_0x08 = 30.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->field_0x0c = 90.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->mFlags = 1;
    AttentionHD::view<AttentionHD::Param>(i_this)->mSWModeDisable = -0.9f;
    AttentionHD::view<AttentionHD::Param>(i_this)->mDangerBGMDistance = 3000.0f;
    AttentionHD::view<AttentionHD::Param>(i_this)->mBGMDistMargin = 1000.0f;
    return i_this;
}

/* 024EBA14 */
static u32 dAttList_c_getActor(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::List>(i_this);
    return searchByID((u32)self->mActorID);
}

/* 024EBA54 */
static u32 dAttHint_c_getPId(u32 i_this, u32 proc) { return getID(proc); }

/* 024EBA68 */
static s32 dAttHint_c_request(u32 i_this, u32 actor, s32 priority) {
    auto* self = AttentionHD::view<AttentionHD::Hint>(i_this);
    s32 cur = (s32)(u32)self->mPriority;
    if (priority < 0)
        priority = 0x1FF;
    if (priority <= cur) {
        u32 id = dAttHint_c_getPId(i_this, actor);
        self->mPriority = (u32)priority;
        self->mHintActorID = id;
    }
    return 1;
}

/* 024EBAB4 */
static void dAttHint_c_init(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Hint>(i_this);
    self->mHintActorID = 0xFFFFFFFF;
    self->mPriority = 0x200;
    self->mZHintTargetID = 0xFFFFFFFF;
}

/* 024EBACC */
static void dAttHint_c_proc(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Hint>(i_this);
    u32 id = (u32)self->mHintActorID;
    self->mHintActorID = 0xFFFFFFFF;
    self->mPriority = 0x200;
    self->mZHintTargetID = id;
}

/* 024EBAE8 */
static u32 dAttCatch_c_convPId(u32 i_this, u32 id) { return searchByID(id); }

/* 024EBB24 */
static void dAttCatch_c_init(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Catch>(i_this);
    self->mCatchItemNo = 0x56;
    self->mRequestActorID = 0xFFFFFFFF;
    self->field_0x4 = 3;
    self->mCatghTargetID = 0xFFFFFFFF;
}

/* 024EBB44 */
static void dAttCatch_c_proc(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Catch>(i_this);
    u32 id = (u32)self->mRequestActorID;
    u8 item = (u8)self->mCatchItemNo;
    self->mCatghTargetID = id;
    self->field_0x4 = 3;
    self->mCatchItemNo = 0x56;
    self->mChangeItem = item;
    self->mRequestActorID = 0xFFFFFFFF;
}

/* |s16 angle of (v) - player shape angle y| through cSGlobe / cSAngle */
static inline s16 globe_angle(u32 v, u32 player) {
    gabi::Local<AttentionHD::Globe> globe;
    gabi::Local<be<s16>> res;
    att1_cSGlobe_ct(gabi::ea(globe.get()), v);
    att1_cSAngle_mi(gabi::ea(globe.get()) + 6, gabi::ea(res.get()),
                    (s16)AttentionHD::view<fopAc_ac_c>(player)->shape_angle.y);
    s16 a = *res;
    if (a < 0)
        a = (s16)-a;
    return a;
}

static inline f32 absXZ(u32 v) {
    gabi::Local<cXyz> xz;
    xz->x = (f32)AttentionHD::view<cXyz>(v)->x;
    xz->y = 0.0f;
    xz->z = (f32)AttentionHD::view<cXyz>(v)->z;
    return att1_sqrtf(att1_PSVECSquareMag(gabi::ea(xz.get())));
}

/* 024EBB70 */
static bool dAttCatch_c_request(u32 i_this, u32 reqActor, u8 itemNo, f32 horizontalDist, f32 upDist,
                                f32 downDist, s32 angle, s32 param_7) {
    auto* self = AttentionHD::view<AttentionHD::Catch>(i_this);
    u32 player = player0();
    if (param_7 > (s32)(u32)self->field_0x4)
        return false;
    gabi::Local<cXyz> v;
    att1_cXyz_mi(reqActor + AC_ATTN_POS, gabi::ea(v.get()), player + AC_ATTN_POS);
    f32 y = v->y;
    if (y < downDist)
        return false;
    if (y > upDist)
        return false;
    f32 dist = absXZ(gabi::ea(v.get()));
    if (dist > horizontalDist)
        return false;
    if (angle != 0) {
        if (globe_angle(gabi::ea(v.get()), player) > angle)
            return false;
    }
    if (param_7 < (s32)(u32)self->field_0x4 || dist < (f32)self->mDistance) {
        self->field_0x4 = (u32)param_7;
        self->mCatchItemNo = itemNo;
        u32 id = getID(reqActor);
        self->mDistance = dist;
        self->mRequestActorID = id;
        return true;
    }
    return false;
}

/* 024EBD10 */
static u32 dAttLook_c_convPId(u32 i_this, u32 id) { return searchByID(id); }

/* 024EBD4C */
static void dAttLook_c_init(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Look>(i_this);
    self->mRequestActorID = 0xFFFFFFFF;
    self->field_0x4 = 3;
    self->mLookTargetID = 0xFFFFFFFF;
}

/* 024EBD64 */
static void dAttLook_c_proc(u32 i_this) {
    auto* self = AttentionHD::view<AttentionHD::Look>(i_this);
    u32 id = (u32)self->mRequestActorID;
    self->mRequestActorID = 0xFFFFFFFF;
    self->field_0x4 = 3;
    self->mLookTargetID = id;
}

/* the angle check of dAttLook_c::request/requestF: on the positions, copied into v */
static inline bool look_angle_fails(u32 v, u32 reqActor, u32 player, s32 angle) {
    gabi::Local<cXyz> d;
    att1_cXyz_mi(reqActor + AC_POS, gabi::ea(d.get()), player + AC_POS);
    u32 x = AttentionMemory1::ld(gabi::ea(d.get()) + 0),
        y = AttentionMemory1::ld(gabi::ea(d.get()) + 4),
        z = AttentionMemory1::ld(gabi::ea(d.get()) + 8);
    AttentionMemory1::st(v + 8, z);
    AttentionMemory1::st(v + 4, y);
    AttentionMemory1::st(v + 0, x);
    return globe_angle(v, player) > angle;
}

/* 024EBD80 */
static bool dAttLook_c_request(u32 i_this, u32 reqActor, f32 horizontalDist, f32 upDist,
                               f32 downDist, s32 angle, s32 param_6) {
    auto* self = AttentionHD::view<AttentionHD::Look>(i_this);
    u32 player = player0();
    if (param_6 > (s32)(u32)self->field_0x4)
        return false;
    gabi::Local<cXyz> v;
    att1_cXyz_mi(reqActor + AC_EYE, gabi::ea(v.get()), player + AC_EYE);
    f32 y = v->y;
    if (y < downDist)
        return false;
    if (y > upDist)
        return false;
    f32 dist = absXZ(gabi::ea(v.get()));
    if (dist > horizontalDist)
        return false;
    if (angle != 0) {
        if (look_angle_fails(gabi::ea(v.get()), reqActor, player, angle))
            return false;
    }
    if (param_6 < (s32)(u32)self->field_0x4 || dist < (f32)self->mDistance) {
        u32 id = getID(reqActor);
        self->field_0x4 = (u32)param_6;
        self->mDistance = dist;
        self->mRequestActorID = id;
        return true;
    }
    return false;
}

/* 024EBF40 */
static bool dAttLook_c_requestF(u32 i_this, u32 reqActor, s32 angle, s32 param_3) {
    auto* self = AttentionHD::view<AttentionHD::Look>(i_this);
    u32 player = player0();
    if (param_3 > (s32)(u32)self->field_0x4)
        return false;
    gabi::Local<cXyz> v;
    att1_cXyz_mi(reqActor + AC_EYE, gabi::ea(v.get()), player + AC_EYE);
    f32 dist = absXZ(gabi::ea(v.get()));
    if (angle != 0) {
        if (look_angle_fails(gabi::ea(v.get()), reqActor, player, angle))
            return false;
    }
    if (param_3 < (s32)(u32)self->field_0x4 || dist < (f32)self->mDistance) {
        u32 id = getID(reqActor);
        self->field_0x4 = (u32)param_3;
        self->mDistance = dist;
        self->mRequestActorID = id;
        return true;
    }
    return false;
}

/* 024EC0A0 */
static u32 dAttHint_c_convPId(u32 i_this, u32 id) { return searchByID(id); }

// HD: the HD caller passes the timing zero-extended; the word is tested
/* 024EC0DC */
static bool dAttDraw_CallBack_c_execute(u32 i_this, u32 timing, u32 xform) {
    if (timing == 0) {
        f32 s = AttentionMemory1::ldf(0x1047B9B4) + 0.6f; /* REG6_F(17) + 0.6f */
        AttentionMemory1::stf(xform + 0x18, AttentionMemory1::ldf(xform + 0x18) * s);
    }
    return true;
}

/* 024EC10C */
static void dAttDraw_c_setAnm(u32 i_this, s32 resIdxTransform, s32 resIdxColor, s32 loopMode) {
    auto* self = AttentionHD::view<AttentionHD::Draw>(i_this);
    u32 anm = getRes_l(STR_ALWAYS_SETANM, resIdxTransform);
    att1_McaMorf_setAnm((u32)self->anm, anm, loopMode, 0, 0.0f, 1.0f, 0.0f, -1.0f);
    if (resIdxColor < 0) {
        self->mpAnmClr = 0;
    } else {
        self->mpAnmClr = getRes_l(STR_ALWAYS_SETANM, resIdxColor);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
ext 028F521C void att1_memzero(u32,u32)
ext 0273AD10 u32 att1_operator_new(u32)
ext 020065FC void att1_cSAngle_ct(u32)
ext 02006908 void att1_cSAngle_mi(u32,u32,s16)
ext 02007324 void att1_cSGlobe_ct(u32,u32)
ext 0201ADE0 void att1_cXyz_mi(u32,u32,u32)
ext 028E8DD0 f32 att1_PSVECSquareMag(u32)
ext 028F4384 f32 att1_sqrtf(f32)
ext 025E4A98 void att1_McaMorf_setAnm(u32,u32,s32,u32,f32,f32,f32,f32)
ext 025D68EC f32 att1_fopAcM_searchActorDistance(u32,u32)
ext 025D5218 u32 att1_searchActor(u32,u32)
ext 026066C4 u32 att1_getObjectRes(u32,u32,s32)
*/
