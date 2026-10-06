/**
 * d_a_npc_p2_cut.cpp (WWHD)
 * NPC - Zuko, Niko, & Mako (Tetra's pirates): the event cuts of the "P2b" staff (Niko's rope
 * course: TALK, RIDE_SWITCH, RUN_WAIT, JUMP_TO_LIFT, LIFT_TO_ROPE, ROPE_TALK, ROPE_TO_LIFT,
 * JUMP_TO_GOAL, SET_ANM, JUMP, SW_ON, SW_OFF, SURPRISE, OMAMORI_INIT, OMAMORI_END).
 *
 * Written from the WWHD code (the GameCube functions
 * are "Nonmatching" stubs), verified against cking.rpx.
 */
#include "d/actor/d_a_npc_p2.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void* p2_getSubstance(s32 staffId, u32 name, s32 type) {
    return dComIfGp_evmng_getMySubstanceP(staffId, STR(name), type);
}
/* 0211D2F8 cLib_calcTimer<int> (out of line) */
static inline s32 p2_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
/* 025E1988 HD: mDoAud_seStart(id) without position */
static inline void p2_seStart(u32 id) { gabi::call(0x025E1988, id); }
static inline f32 p2_ldf(void* p, u32 off = 0) { return gabi::load<f32>(gabi::ea(p) + off); }
/* the smoke follows the actor: mSmokePos = current.pos, mSmokeAngle = current.angle */
static inline void p2_setSmokePos(daNpc_P2_c* a) {
    p2_copy12(gabi::ea(&a->mSmokePos), gabi::ea(&a->current.pos));
    u32 d = gabi::ea(&a->mSmokeAngle), s = gabi::ea(&a->current.angle);
    gabi::store<u16>(d, gabi::load<u16>(s));
    gabi::store<u16>(d + 2, gabi::load<u16>(s + 2));
    gabi::store<u16>(d + 4, gabi::load<u16>(s + 4));
}
static inline void p2_smokeEnd(daNpc_P2_c* a) { dPa_smokeEcallBack_end((dPa_smokeEcallBack*)a->mSmokeCB); }
/* distance in XZ to a position (cXyz::operator- and PSVECSquareMag/sqrtf on {x, 0, z}, float copies);
 * the result is not used by the cuts */
static inline void p2_distXZ(cXyz* to, cXyz* from) {
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> xz;
    cXyz_mi(to, diff.get(), from);
    f32 x = diff->x;
    f32 z = diff->z;
    xz->x = x;
    xz->z = z;
    xz->y = 0.0f;
    std_sqrtf(PSVECSquareMag(xz.get()));
}

/* 022B79F0 */
void daNpc_P2_c::cutTalkStart(int i_staffId) {
    WWHD_FUNC(0x022B79F0, void, this, i_staffId);
    be<u32>* msg = (be<u32>*)p2_getSubstance(i_staffId, 0x1001FD00 /* "MsgNum" */, 3);
    mMsgNo = msg != nullptr ? (u32)*msg : 0;
    void* attn = p2_getSubstance(i_staffId, 0x1001FD08 /* "Attention" */, 3);
    mbEvtAttention = attn != nullptr;
    talkInit();
}
VERIFY(0x022B79F0, &daNpc_P2_c::cutTalkStart);

/* 022B7A84 */
void daNpc_P2_c::cutRideSwitchStart(int i_staffId) {
    WWHD_FUNC(0x022B7A84, void, this, i_staffId);
    void* sy = p2_getSubstance(i_staffId, 0x1001FD1C /* "Speed_y" */, 0);
    void* g = p2_getSubstance(i_staffId, 0x1001FD24 /* "Gravity" */, 0);
    speed.y = sy != nullptr ? p2_ldf(sy) : 16.0f;
    mbEvtAttention = 1;
    gravity = g != nullptr ? p2_ldf(g) : -2.0f;
    mAnm = 9;
}
VERIFY(0x022B7A84, &daNpc_P2_c::cutRideSwitchStart);

/* 022B7B58 */
void daNpc_P2_c::cutRunWaitStart(int i_staffId) {
    WWHD_FUNC(0x022B7B58, void, this, i_staffId);
    void* t = p2_getSubstance(i_staffId, 0x1001FD2C /* "Timer" */, 3);
    mTimer = t != nullptr ? (s32)gabi::load<s16>(gabi::ea(t) + 2) : 0;
    mAnm = 6;
    mbEvtAttention = 1;
}
VERIFY(0x022B7B58, &daNpc_P2_c::cutRunWaitStart);

/* 022B7BCC (GameCube cutJumpToLiftStart; unnamed by the matcher) */
void daNpc_P2_c::cutJumpToLiftStart(int i_staffId) {
    WWHD_FUNC(0x022B7BCC, void, this, i_staffId);
    void* s = p2_getSubstance(i_staffId, 0x1001FD4C /* "Speed" */, 0);
    void* sy = p2_getSubstance(i_staffId, 0x1001FD3C /* "Speed_y" */, 0);
    void* g = p2_getSubstance(i_staffId, 0x1001FD44 /* "Gravity" */, 0);
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 15.0f;
    mAnm = 5;
    mJumpGravity = g != nullptr ? p2_ldf(g) : -2.5f;
    mbEvtAttention = 1;
}
VERIFY(0x022B7BCC, &daNpc_P2_c::cutJumpToLiftStart);

/* 022B7CF8 (GameCube cutLiftToRopeStart; unnamed by the matcher) */
void daNpc_P2_c::cutLiftToRopeStart(int i_staffId) {
    WWHD_FUNC(0x022B7CF8, void, this, i_staffId);
    void* s = p2_getSubstance(i_staffId, 0x1001FD64 /* "Speed" */, 0);
    void* sy = p2_getSubstance(i_staffId, 0x1001FD54 /* "Speed_y" */, 0);
    void* g = p2_getSubstance(i_staffId, 0x1001FD5C /* "Gravity" */, 0);
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 15.0f;
    mAnm = 5;
    mJumpGravity = g != nullptr ? p2_ldf(g) : -2.5f;
    mbEvtAttention = 1;
}
VERIFY(0x022B7CF8, &daNpc_P2_c::cutLiftToRopeStart);

/* 022B7E24 (GameCube searchNearRope; unnamed by the matcher): fopAcIt_Judge callback, keeps the
 * nearest rope (0x1BF) in mpNearActor and its position in mNearPos (the first one found only by position) */
static void* searchNearRope(void* i_actor, void* i_this) {
    WWHD_FUNC(0x022B7E24, void*, i_actor, i_this);
    fopAc_ac_c* a = (fopAc_ac_c*)i_actor;
    daNpc_P2_c* p = (daNpc_P2_c*)i_this;
    if (fopAc_IsActor(a) && a != nullptr && fpcM_GetName(a) == 0x1BF) {
        if (std_sqrtf(PSVECSquareMag(&p->mNearPos)) == 0.0f) {
            p2_copy12(gabi::ea(&p->mNearPos), gabi::ea(&a->current.pos));
            return nullptr;
        }
        gabi::Local<cXyz> diff;
        cXyz_mi(&p->mNearPos, diff.get(), &p->current.pos);
        f32 cur = std_sqrtf(PSVECSquareMag(diff.get()));
        cXyz_mi(&a->current.pos, diff.get(), &p->current.pos);
        if (std_sqrtf(PSVECSquareMag(diff.get())) < cur) {
            p2_copy12(gabi::ea(&p->mNearPos), gabi::ea(&a->current.pos));
            p->mpNearActor = a;
        }
    }
    return nullptr;
}
VERIFY(0x022B7E24, searchNearRope);

/* 022B7F30 */
void daNpc_P2_c::cutRopeTalkStart(int i_staffId) {
    WWHD_FUNC(0x022B7F30, void, this, i_staffId);
    be<u32>* msg = (be<u32>*)p2_getSubstance(i_staffId, 0x1001FD80 /* "MsgNum" */, 3);
    mMsgNo = msg != nullptr ? (u32)*msg : 0;
    void* attn = p2_getSubstance(i_staffId, 0x1001FD88 /* "Attention" */, 3);
    mAnm = 0xC;
    mbEvtAttention = attn != nullptr;
    talkInit();
    mbRopeHang = 1;
    fopAcIt_Judge(0x022B7E24 /* searchNearRope */, this);
    fopAc_ac_c* rope = mpNearActor;
    mpRope = rope;
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> d;
    cXyz_mi(&rope->current.pos, diff.get(), &current.pos);
    p2_copy12(gabi::ea(d.get()), gabi::ea(diff.get()));
    f32 len = std_sqrtf(PSVECSquareMag(d.get()));
    mRopeLen = len;
    f32 omega = std_sqrtf(2.0f / len);
    mRopeOmega = omega;
    f32 quarter = 1.5707964f / omega;
    u32 idx = ((u32)(u16)current.angle.y) >> 3;
    mRopeAmp = 0x2EE0;
    f32 dy = d->y;
    f32 dz = d->z;
    f32 s = gabi::load<f32>(0x104A44F8 + idx * 8); /* JMA sin/cos table */
    f32 c = gabi::load<f32>(0x104A44F8 + idx * 8 + 4);
    f32 dx = d->x;
    f32 rz = gabi::fmadds(s, dx, c * dz);
    f32 rx = gabi::fmsubs(c, dx, s * dz);
    s16 a = cM_atan2s(-rz, dy);
    s16 amp = mRopeAmp;
    f32 ratio;
    if (a > amp) {
        f32 f = (f32)(s32)amp;
        ratio = f / f;
    } else {
        if (a < -amp) a = (s16)-amp;
        ratio = (f32)(s32)a / (f32)(s32)amp;
    }
    f32 ang = gabi::call<f32>(0x0201971C, ratio, std_sqrtf(gabi::fnmsubs(ratio, ratio, 1.0f))); /* cM_atan2f */
    mRopeSwingX = ang / mRopeOmega;
    s16 tilt = cM_atan2s(-rx, std_sqrtf(gabi::fmadds(dy, dy, rz * rz)));
    mRopeTilt = tilt;
    rope = mpRope;
    if (tilt <= 0) quarter = -quarter;
    mRopeSwingZ = quarter;
    cXyz_mi(&rope->current.pos, diff.get(), &current.pos);
    p2_copy12(gabi::ea(d.get()), gabi::ea(diff.get()));
    f32 dist = std_sqrtf(PSVECSquareMag(d.get())) - gabi::load<f32>(0x10467FE0) /* l_rope_dist */;
    f32 maxLen = gabi::load<f32>(gabi::ea(rope) + 0x1914);
    if (dist > maxLen) dist = maxLen;
    gabi::Local<cXyz> scaled;
    gabi::call(0x0201B31C, d.get(), diff.get()); /* cXyz::normalize */
    cXyz_ml(d.get(), scaled.get(), dist);
    cXyz_mi(&mpRope->current.pos, diff.get(), scaled.get());
    fopAc_ac_c* r = mpRope;
    p2_copy12(gabi::ea(&current.pos), gabi::ea(diff.get()));
    speed.y = 0.0f;
    gravity = 0.0f;
    p2_copy12(gabi::ea(&mRopePos), gabi::ea(&r->current.pos));
    m924 = 0;
}
VERIFY(0x022B7F30, &daNpc_P2_c::cutRopeTalkStart);

/* 022B82F4 */
void daNpc_P2_c::cutRopeToLiftStart(int i_staffId) {
    WWHD_FUNC(0x022B82F4, void, this, i_staffId);
    void* s = p2_getSubstance(i_staffId, 0x1001FDAC /* "Speed" */, 0);
    void* sy = p2_getSubstance(i_staffId, 0x1001FD9C /* "Speed_y" */, 0);
    void* g = p2_getSubstance(i_staffId, 0x1001FDA4 /* "Gravity" */, 0);
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 6.0f;
    f32 grav = g != nullptr ? p2_ldf(g) : -0.5f;
    current.angle.z = 0;
    gravity = grav;
    mbEvtAttention = 1;
    mJumpGravity = grav;
    speedF = (f32)mJumpSpeed;
    current.angle.x = 0;
    mAnm = 9;
    speed.y = (f32)mJumpSpeedY;
}
VERIFY(0x022B82F4, &daNpc_P2_c::cutRopeToLiftStart);

/* 022B8460 */
void daNpc_P2_c::cutJumpToGoalStart(int i_staffId) {
    WWHD_FUNC(0x022B8460, void, this, i_staffId);
    void* s = p2_getSubstance(i_staffId, 0x1001FDC8 /* "Speed" */, 0);
    void* sy = p2_getSubstance(i_staffId, 0x1001FDB4 /* "Speed_y" */, 0);
    void* g = p2_getSubstance(i_staffId, 0x1001FDBC /* "Gravity" */, 0);
    void* pos = p2_getSubstance(i_staffId, 0x1001FDC4 /* "Pos" */, 1);
    if (pos == nullptr) {
        p2_copy12(gabi::ea(&mJumpPos), gabi::ea(&p2_child(mType)->mGoalTalkPos));
    } else {
        mJumpPos.x = p2_ldf(pos, 0);
        mJumpPos.y = p2_ldf(pos, 4);
        mJumpPos.z = p2_ldf(pos, 8);
    }
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 15.0f;
    mAnm = 5;
    mJumpGravity = g != nullptr ? p2_ldf(g) : -2.5f;
    mbEvtAttention = 1;
}
VERIFY(0x022B8460, &daNpc_P2_c::cutJumpToGoalStart);

static inline bool p2_streq(u32 a, u32 b) {
    for (u32 i = 0;; i++) {
        u8 c1 = gabi::load<u8>(a + i);
        u8 c2 = gabi::load<u8>(b + i);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
}

/* 022B85E4 */
void daNpc_P2_c::cutSetAnmStart(int i_staffId) {
    WWHD_FUNC(0x022B85E4, void, this, i_staffId);
    void* name = p2_getSubstance(i_staffId, 0x1001FDE4 /* "Name" */, 4);
    if (name != nullptr) {
        u32 n = gabi::ea(name);
        if (p2_streq(n, 0x1001FDD4 /* "KYORO" */)) {
            mAnm = 0x11;
            return;
        }
        if (p2_streq(n, 0x1001FDEC /* "SURPRISE" */)) {
            mAnm = 7;
            return;
        }
        if (p2_streq(n, 0x1001FDDC /* "THINK" */)) {
            mAnm = 0x12;
            return;
        }
        if (p2_streq(n, 0x1001FDD0 /* "NOD" */)) {
            mAnm = 0x13;
            return;
        }
    }
    mAnm = 1;
}
VERIFY(0x022B85E4, &daNpc_P2_c::cutSetAnmStart);

/* 022B8724 */
void daNpc_P2_c::cutJumpStart(int i_staffId) {
    WWHD_FUNC(0x022B8724, void, this, i_staffId);
    void* s = p2_getSubstance(i_staffId, 0x1001FE0C /* "Speed" */, 0);
    void* sy = p2_getSubstance(i_staffId, 0x1001FDF8 /* "Speed_y" */, 0);
    void* g = p2_getSubstance(i_staffId, 0x1001FE00 /* "Gravity" */, 0);
    void* pos = p2_getSubstance(i_staffId, 0x1001FE08 /* "Pos" */, 1);
    if (pos == nullptr) {
        p2_copy12(gabi::ea(&mJumpPos), gabi::ea(&p2_child(mType)->mGoalTalkPos));
    } else {
        mJumpPos.x = p2_ldf(pos, 0);
        mJumpPos.y = p2_ldf(pos, 4);
        mJumpPos.z = p2_ldf(pos, 8);
    }
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 15.0f;
    mJumpGravity = g != nullptr ? p2_ldf(g) : -2.5f;
    mbEvtAttention = 1;
    p2_monsSeStart(this, 0x4898);
    speedF = (f32)mJumpSpeed;
    mAnm = 8;
    speed.y = (f32)mJumpSpeedY;
    gravity = (f32)mJumpGravity;
}
VERIFY(0x022B8724, &daNpc_P2_c::cutJumpStart);

/* 022B8920 */
void daNpc_P2_c::cutSwOnStart(int i_staffId) {
    WWHD_FUNC(0x022B8920, void, this, i_staffId); /* unused */
    u8 sw = mSwitchNo;
    if (sw != 0xFF) {
        dComIfGs_onSwitch(sw, home.roomNo);
    }
}
VERIFY(0x022B8920, &daNpc_P2_c::cutSwOnStart);

/* 022B8944 */
void daNpc_P2_c::cutSwOffStart(int i_staffId) {
    WWHD_FUNC(0x022B8944, void, this, i_staffId); /* unused */
    u8 sw = mSwitchNo;
    if (sw != 0xFF) {
        dComIfGs_offSwitch(sw, home.roomNo);
    }
}
VERIFY(0x022B8944, &daNpc_P2_c::cutSwOffStart);

/* 022B8968 (GameCube cutSurpriseStart; unnamed by the matcher) */
void daNpc_P2_c::cutSurpriseStart(int i_staffId) {
    WWHD_FUNC(0x022B8968, void, this, i_staffId); /* unused */
    p2_seStart(0x852);
    /* the player reacts: virtual (vtable +0xE4) with 0x1C */
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(player) + 0xB4) + 0xE4), player, 0x1C);
}
VERIFY(0x022B8968, &daNpc_P2_c::cutSurpriseStart);

/* 022B89A8 (GameCube cutOmamoriInitStart; unnamed by the matcher) */
u32 daNpc_P2_c::cutOmamoriInitStart(int i_staffId) {
    WWHD_FUNC(0x022B89A8, u32, this, i_staffId); /* unused */
    return gabi::call<u32>(0x025E1988, 0x854u); /* p2_seStart */
}
VERIFY(0x022B89A8, &daNpc_P2_c::cutOmamoriInitStart);

/* 022B89B0 (GameCube cutOmamoriEndStart; unnamed by the matcher) */
u32 daNpc_P2_c::cutOmamoriEndStart(int i_staffId) {
    WWHD_FUNC(0x022B89B0, u32, this, i_staffId); /* unused */
    return gabi::call<u32>(0x025E1988, 0x855u); /* p2_seStart */
}
VERIFY(0x022B89B0, &daNpc_P2_c::cutOmamoriEndStart);

/* 022B8FA8 */
void daNpc_P2_c::cutTalkProc(int i_staffId) {
    WWHD_FUNC(0x022B8FA8, void, this, i_staffId);
    if (talk(true) == 0x12) {
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}
VERIFY(0x022B8FA8, &daNpc_P2_c::cutTalkProc);

/* 022B8FF0 */
void daNpc_P2_c::cutRideSwitchProc(int i_staffId) {
    WWHD_FUNC(0x022B8FF0, void, this, i_staffId);
    gabi::Local<be<s16>> name;
    *name = 0x1B; /* the switch */
    fopAc_ac_c* sw = fopAcIt_Judge(0x025E121C /* fpcSch_JudgeByName */, name.get());
    fopAcM_searchActorAngleY(this, sw);
    gabi::Local<cXyz> pos;
    pos->x = (f32)sw->current.pos.x;
    pos->y = (f32)sw->current.pos.y;
    pos->z = (f32)sw->current.pos.z;
    p2_distXZ(pos.get(), &current.pos);
    if (mAnm != 0xA) {
        cLib_addCalc2(&current.pos.x, pos->x, 0.1f, 24.0f);
        cLib_addCalc2(&current.pos.z, pos->z, 0.1f, 24.0f);
    }
    if (mObjAcch.m_flags & 0x80) {
        mTimer = 0x11;
        mAnm = 0xA;
    }
    if (p2_calcTimer(&mTimer) == 0 && mAnm == 0xA) {
        speed.y = 0.0f;
        mAnm = 1;
        gravity = -9.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}
VERIFY(0x022B8FF0, &daNpc_P2_c::cutRideSwitchProc);

/* 022B9190 */
void daNpc_P2_c::cutRunWaitProc(int i_staffId) {
    WWHD_FUNC(0x022B9190, void, this, i_staffId);
    p2_setSmokePos(this);
    if (mAnm == 6 && p2_isStop(mpMorf)) {
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}
VERIFY(0x022B9190, &daNpc_P2_c::cutRunWaitProc);

/* 022B922C (GameCube searchNearLift; unnamed by the matcher): fopAcIt_Judge callback, keeps the
 * nearest lift (0x2E) in mpNearActor and its position in mNearPos (the first one found only by position) */
static void* searchNearLift(void* i_actor, void* i_this) {
    WWHD_FUNC(0x022B922C, void*, i_actor, i_this);
    fopAc_ac_c* a = (fopAc_ac_c*)i_actor;
    daNpc_P2_c* p = (daNpc_P2_c*)i_this;
    if (fopAc_IsActor(a) && a != nullptr && fpcM_GetName(a) == 0x2E) {
        if (std_sqrtf(PSVECSquareMag(&p->mNearPos)) == 0.0f) {
            p2_copy12(gabi::ea(&p->mNearPos), gabi::ea(&a->current.pos));
            return nullptr;
        }
        gabi::Local<cXyz> diff;
        cXyz_mi(&p->mNearPos, diff.get(), &p->current.pos);
        f32 cur = std_sqrtf(PSVECSquareMag(diff.get()));
        cXyz_mi(&a->current.pos, diff.get(), &p->current.pos);
        if (std_sqrtf(PSVECSquareMag(diff.get())) < cur) {
            p2_copy12(gabi::ea(&p->mNearPos), gabi::ea(&a->current.pos));
            p->mpNearActor = a;
        }
    }
    return nullptr;
}
VERIFY(0x022B922C, searchNearLift);

/* 022B9338 */
void daNpc_P2_c::smoke_set() {
    WWHD_FUNC(0x022B9338, void, this);
    /* static cXyz l_scale(1.25f, 1.25f, 1.25f) */
    if (gabi::load<u32>(0x104682F0) == 0) {
        gabi::store<u32>(0x104682F0, 1);
        gabi::store<f32>(0x104682F4, 1.25f);
        gabi::store<f32>(0x104682FC, 1.25f);
        gabi::store<f32>(0x104682F8, 1.25f);
    }
    u32 cb = gabi::ea(mSmokeCB);
    if (gabi::load<u32>(cb + 4) == 0) { /* mSmokeCB.getEmitter() */
        s8 room = current.roomNo;
        dPa_control_set(dComIfGp_getParticle(), 2, 0x2022, &mSmokePos, &mSmokeAngle, nullptr, 0xB9,
                        (dPa_levelEcallBack*)mSmokeCB, room, nullptr, nullptr, nullptr);
        if (gabi::load<u32>(cb + 4) == 0) return;
    }
    u32 e = gabi::load<u32>(cb + 4);
    gabi::store<f32>(e + 0x34, 2.0f);
    gabi::store<f32>(gabi::load<u32>(cb + 4) + 0x58, 0.25f);
    gabi::store<f32>(gabi::load<u32>(cb + 4) + 0x68, 0.0f);
    gabi::store<f32>(gabi::load<u32>(cb + 4) + 0x6C, 5.0f);
    gabi::store<f32>(gabi::load<u32>(cb + 4) + 0x70, 20.0f);
    e = gabi::load<u32>(cb + 4);
    f32 sx = gabi::load<f32>(0x104682F4);
    f32 sy = gabi::load<f32>(0x104682F8);
    f32 sz = gabi::load<f32>(0x104682FC);
    gabi::store<f32>(e + 0x220, sx); /* setGlobalScale */
    gabi::store<f32>(e + 0x224, sy);
    gabi::store<f32>(e + 0x228, sz);
    gabi::store<f32>(e + 0x238, sx); /* setGlobalParticleScale */
    gabi::store<f32>(e + 0x23C, sy);
    gabi::store<f32>(e + 0x240, sz);
}
VERIFY(0x022B9338, &daNpc_P2_c::smoke_set);

/* 022B9470 */
void daNpc_P2_c::cutJumpToLiftProc(int i_staffId) {
    WWHD_FUNC(0x022B9470, void, this, i_staffId);
    fopAcIt_Judge(0x022B922C /* searchNearLift */, this);
    fopAc_ac_c* lift = mpNearActor;
    gabi::Local<cXyz> pos;
    pos->x = (f32)lift->current.pos.x;
    pos->y = (f32)lift->current.pos.y;
    pos->z = (f32)lift->current.pos.z;
    f32 spd = mJumpSpeed;
    p2_distXZ(pos.get(), &current.pos);
    u32 flags = mObjAcch.m_flags;
    int next; /* 0: smoke off, 1: landing (smoke on), 2: smoke stays */
    if (flags & 0x100) {
        p2_monsSeStart(this, 0x4898);
        speedF = (f32)mJumpSpeed;
        mAnm = 8;
        speed.y = (f32)mJumpSpeedY;
        gravity = (f32)mJumpGravity;
        next = (mObjAcch.m_flags & 0x80) ? 1 : 0;
    } else {
        s8 anm = mAnm;
        if ((flags & 0x20) && (anm == 9 || anm == 0xA)) {
            mLookOffs.z = 0.0f;
            speedF = 0.0f;
            mAnm = 0xA;
            mLookOffs.x = 0.0f;
            mLookOffs.y = -100.0f;
            next = (mObjAcch.m_flags & 0x80) ? 1 : 0;
        } else if (!(speed.y > 0.0f) && anm == 8) {
            mLookOffs.z = 0.0f;
            mAnm = 9;
            mLookOffs.x = 0.0f;
            mLookOffs.y = -50.0f;
            next = (mObjAcch.m_flags & 0x80) ? 1 : 0;
        } else if (anm == 5) {
            next = 2;
        } else {
            next = (mObjAcch.m_flags & 0x80) ? 1 : 0;
        }
    }
    if (next == 0) {
        p2_smokeEnd(this);
    } else if (next == 1) {
        p2_monsSeStart(this, 0x489B);
        mTimer = 0x11;
        smoke_set();
    } else {
        smoke_set();
    }
    p2_setSmokePos(this);
    if (mObjAcch.m_flags & 0x20) {
        if (mAnm == 5) {
            speedF = spd + spd;
            return;
        }
        s8 anm = mAnm;
        if (anm != 0xA && anm != 1) {
            cLib_addCalc2(&current.pos.x, pos->x, 0.1f, spd);
            cLib_addCalc2(&current.pos.z, pos->z, 0.1f, spd);
            return;
        }
    }
    if (p2_calcTimer(&mTimer) == 0 && mAnm == 0xA) {
        mLookOffs.z = 0.0f;
        speedF = 0.0f;
        mAnm = 1;
        mNearPos.y = 0.0f;
        mNearPos.z = 0.0f;
        mNearPos.x = 0.0f;
        speed.y = 0.0f;
        mLookOffs.y = 0.0f;
        mLookOffs.x = 0.0f;
        gravity = -9.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}
VERIFY(0x022B9470, &daNpc_P2_c::cutJumpToLiftProc);

/* 022B9848 */
void daNpc_P2_c::cutLiftToRopeProc(int i_staffId) {
    WWHD_FUNC(0x022B9848, void, this, i_staffId);
    fopAcIt_Judge(0x022B7E24 /* searchNearRope */, this);
    fopAc_ac_c* lift = mpNearActor;
    gabi::Local<cXyz> pos;
    pos->x = (f32)lift->current.pos.x;
    pos->y = (f32)lift->current.pos.y;
    pos->z = (f32)lift->current.pos.z;
    f32 spd = mJumpSpeed;
    p2_distXZ(pos.get(), &current.pos);
    if (mObjAcch.m_flags & 0x100) {
        p2_monsSeStart(this, 0x4898);
        f32 s = mJumpSpeed;
        f32 sy = mJumpSpeedY;
        f32 g = mJumpGravity;
        speedF = s;
        mAnm = 8;
        gravity = g;
        speed.y = sy;
        p2_smokeEnd(this);
    } else if (mAnm == 5) {
        smoke_set();
    } else {
        p2_smokeEnd(this);
    }
    if (mObjAcch.m_flags & 0x20) {
        speedF = spd + spd;
    }
    if (mCyl.ChkCoHit()) {
        p2_monsSeStart(this, 0x4899);
        gravity = 0.0f;
        speed.y = 0.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}
VERIFY(0x022B9848, &daNpc_P2_c::cutLiftToRopeProc);

/* 022BA1EC */
void daNpc_P2_c::cutRopeToLiftProc(int i_staffId) {
    WWHD_FUNC(0x022BA1EC, void, this, i_staffId);
    fopAcIt_Judge(0x022B922C /* searchNearLift */, this);
    fopAc_ac_c* lift = mpNearActor;
    gabi::Local<cXyz> pos;
    pos->x = (f32)lift->current.pos.x;
    pos->y = (f32)lift->current.pos.y;
    pos->z = (f32)lift->current.pos.z;
    p2_distXZ(pos.get(), &current.pos);
    p2_setSmokePos(this);
    if (mObjAcch.m_flags & 0x80) {
        p2_monsSeStart(this, 0x489B);
        mTimer = 0x11;
        smoke_set();
    } else {
        p2_smokeEnd(this);
    }
    if (mObjAcch.m_flags & 0x20) {
        if (mAnm == 9 || mAnm == 0xA) {
            speedF = 0.0f;
            mLookOffs.x = 0.0f;
            mAnm = 0xA;
            mLookOffs.z = 0.0f;
            mLookOffs.y = -100.0f;
            speed.y = 0.0f;
            gravity = -9.0f;
        }
    }
    if (p2_calcTimer(&mTimer) == 0 && mAnm == 0xA) {
        mNearPos.y = 0.0f;
        mLookOffs.y = 0.0f;
        mAnm = 1;
        mLookOffs.z = 0.0f;
        mNearPos.x = 0.0f;
        mLookOffs.x = 0.0f;
        mNearPos.z = 0.0f;
        p2_smokeEnd(this);
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}
VERIFY(0x022BA1EC, &daNpc_P2_c::cutRopeToLiftProc);

/* 022BA3D0 */
void daNpc_P2_c::cutJumpToGoalProc(int i_staffId) {
    WWHD_FUNC(0x022BA3D0, void, this, i_staffId);
    gabi::Local<cXyz> pos;
    pos->y = (f32)mJumpPos.y;
    pos->z = (f32)mJumpPos.z;
    pos->x = (f32)mJumpPos.x;
    p2_distXZ(pos.get(), &current.pos);
    f32 spd = mJumpSpeed;
    if (mObjAcch.m_flags & 0x100) {
        p2_monsSeStart(this, 0x4898);
        speedF = (f32)mJumpSpeed;
        speed.y = (f32)mJumpSpeedY;
        mAnm = 8;
        gravity = (f32)mJumpGravity;
    }
    bool landed = false;
    if (mObjAcch.m_flags & 0x20) {
        s8 anm = mAnm;
        bool chk = true;
        if (anm != 0xA && anm != 1) {
            cLib_addCalc2(&current.pos.x, pos->x, 0.1f, spd);
            cLib_addCalc2(&current.pos.z, pos->z, 0.1f, spd);
            chk = (mObjAcch.m_flags & 0x20) != 0;
        }
        if (chk && (mAnm == 9 || mAnm == 0xA)) {
            mLookOffs.x = 0.0f;
            mAnm = 0xA;
            mLookOffs.y = -100.0f;
            mLookOffs.z = 0.0f;
            landed = true;
        }
    }
    if (!landed) {
        if (!(speed.y > 0.0f) && mAnm == 8) {
            mLookOffs.x = 0.0f;
            mLookOffs.z = 0.0f;
            mLookOffs.y = -50.0f;
            mAnm = 9;
        }
    }
    if (mObjAcch.m_flags & 0x80) {
        p2_monsSeStart(this, 0x489B);
        speedF = 0.0f;
        mTimer = 0x11;
        smoke_set();
    } else {
        p2_smokeEnd(this);
    }
    p2_setSmokePos(this);
    if (p2_calcTimer(&mTimer) == 0 && mAnm == 0xA) {
        mLookOffs.z = 0.0f;
        mLookOffs.x = 0.0f;
        mLookOffs.y = 0.0f;
        speed.y = 0.0f;
        speedF = 0.0f;
        gravity = -9.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
        mAnm = 1;
    }
}
VERIFY(0x022BA3D0, &daNpc_P2_c::cutJumpToGoalProc);

/* 022BA6C4 */
void daNpc_P2_c::cutSetAnmProc(int i_staffId) {
    WWHD_FUNC(0x022BA6C4, void, this, i_staffId);
    if (p2_isStop(mpMorf)) {
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}
VERIFY(0x022BA6C4, &daNpc_P2_c::cutSetAnmProc);

/* 022BA720 */
void daNpc_P2_c::cutJumpProc(int i_staffId) {
    WWHD_FUNC(0x022BA720, void, this, i_staffId);
    gabi::Local<cXyz> pos;
    pos->x = (f32)mJumpPos.x;
    pos->y = (f32)mJumpPos.y;
    pos->z = (f32)mJumpPos.z;
    p2_distXZ(pos.get(), &current.pos);
    if ((mObjAcch.m_flags & 0x20) && (mAnm == 9 || mAnm == 0xA)) {
        mLookOffs.x = 0.0f;
        mLookOffs.y = -100.0f;
        mAnm = 0xA;
        mLookOffs.z = 0.0f;
    } else if (!(speed.y > 0.0f) && mAnm == 8) {
        mLookOffs.x = 0.0f;
        mAnm = 9;
        mLookOffs.y = -50.0f;
        mLookOffs.z = 0.0f;
    }
    if (mObjAcch.m_flags & 0x80) {
        p2_monsSeStart(this, 0x489B);
        speedF = 0.0f;
        mTimer = 0x11;
        smoke_set();
    } else {
        p2_smokeEnd(this);
    }
    p2_setSmokePos(this);
    if (p2_calcTimer(&mTimer) == 0 && mAnm == 0xA) {
        mLookOffs.z = 0.0f;
        mLookOffs.x = 0.0f;
        mLookOffs.y = 0.0f;
        speed.y = 0.0f;
        speedF = 0.0f;
        gravity = -9.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
        mAnm = 1;
    }
}
VERIFY(0x022BA720, &daNpc_P2_c::cutJumpProc);

/* 022BA944 */
void daNpc_P2_c::cutSwOnProc(int i_staffId) {
    WWHD_FUNC(0x022BA944, void, this, i_staffId);
    u8 sw = mSwitchNo;
    if (sw != 0xFF) {
        dComIfGs_onSwitch(sw, home.roomNo);
    }
    dComIfGp_evmng_cutEnd(i_staffId);
}
VERIFY(0x022BA944, &daNpc_P2_c::cutSwOnProc);

/* 022BA9A0 */
void daNpc_P2_c::cutSwOffProc(int i_staffId) {
    WWHD_FUNC(0x022BA9A0, void, this, i_staffId);
    u8 sw = mSwitchNo;
    if (sw != 0xFF) {
        dComIfGs_offSwitch(sw, home.roomNo);
    }
    dComIfGp_evmng_cutEnd(i_staffId);
}
VERIFY(0x022BA9A0, &daNpc_P2_c::cutSwOffProc);

/* 022BA9FC (GameCube cutSurpriseProc; unnamed by the matcher) */
void daNpc_P2_c::cutSurpriseProc(int i_staffId) {
    WWHD_FUNC(0x022BA9FC, void, this, i_staffId);
    dComIfGp_evmng_cutEnd(i_staffId);
}
VERIFY(0x022BA9FC, &daNpc_P2_c::cutSurpriseProc);

/* 022BAA34 (GameCube cutOmamoriInitProc; unnamed by the matcher) */
void daNpc_P2_c::cutOmamoriInitProc(int i_staffId) {
    WWHD_FUNC(0x022BAA34, void, this, i_staffId);
    dComIfGp_evmng_cutEnd(i_staffId);
}
VERIFY(0x022BAA34, &daNpc_P2_c::cutOmamoriInitProc);

/* 022BAA6C (GameCube cutOmamoriEndProc; unnamed by the matcher) */
void daNpc_P2_c::cutOmamoriEndProc(int i_staffId) {
    WWHD_FUNC(0x022BAA6C, void, this, i_staffId);
    dComIfGp_evmng_cutEnd(i_staffId);
}
VERIFY(0x022BAA6C, &daNpc_P2_c::cutOmamoriEndProc);

/* 022BAAA4 */
void daNpc_P2_c::cutProc() {
    WWHD_FUNC(0x022BAAA4, void, this);
    s32 staffId = dComIfGp_evmng_getMyStaffId(STR(0x1001FE70) /* "P2b" */, nullptr, 0);
    if (staffId == -1) {
        mbInEvent = 0;
        return;
    }
    /* l_cut_name_tbl (0x101C2E1C): TALK .. OMAMORI_END */
    s32 actIdx = gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, 0x101C2E1C, 15, 1, 0);
    if (actIdx == -1) {
        mbInEvent = 0;
        dComIfGp_evmng_cutEnd(staffId);
        return;
    }
    mbInEvent = 1;
    if (dComIfGp_evmng_getIsAddvance(staffId)) {
        mbEvtAttention = 0;
        switch ((u32)actIdx) {
        case 0: cutTalkStart(staffId); break;
        case 1: cutRideSwitchStart(staffId); break;
        case 2: cutRunWaitStart(staffId); break;
        case 3: cutJumpToLiftStart(staffId); break;
        case 4: cutLiftToRopeStart(staffId); break;
        case 5: cutRopeTalkStart(staffId); break;
        case 6: cutRopeToLiftStart(staffId); break;
        case 7: cutJumpToGoalStart(staffId); break;
        case 8: cutSetAnmStart(staffId); break;
        case 9: cutJumpStart(staffId); break;
        case 10: cutSwOnStart(staffId); break;
        case 11: cutSwOffStart(staffId); break;
        case 12: cutSurpriseStart(staffId); break;
        case 13: cutOmamoriInitStart(staffId); break;
        case 14: cutOmamoriEndStart(staffId); break;
        }
    }
    switch ((u32)actIdx) {
    case 0: cutTalkProc(staffId); break;
    case 1: cutRideSwitchProc(staffId); break;
    case 2: cutRunWaitProc(staffId); break;
    case 3: cutJumpToLiftProc(staffId); break;
    case 4: cutLiftToRopeProc(staffId); break;
    case 5: cutRopeTalkProc(staffId); break;
    case 6: cutRopeToLiftProc(staffId); break;
    case 7: cutJumpToGoalProc(staffId); break;
    case 8: cutSetAnmProc(staffId); break;
    case 9: cutJumpProc(staffId); break;
    case 10: cutSwOnProc(staffId); break;
    case 11: cutSwOffProc(staffId); break;
    case 12: cutSurpriseProc(staffId); break;
    case 13: cutOmamoriInitProc(staffId); break;
    case 14: cutOmamoriEndProc(staffId); break;
    }
}
VERIFY(0x022BAAA4, &daNpc_P2_c::cutProc);

/* JMA sine table (0x104A44F8): {sin, cos} pairs indexed by the angle >> 3 */
static inline f32 p2_sinS(s16 a) { return gabi::load<f32>(0x104A44F8 + ((u32)(u16)a >> 3) * 8); }
static inline f32 p2_cosS(s16 a) { return gabi::load<f32>(0x104A44F8 + ((u32)(u16)a >> 3) * 8 + 4); }
static inline s16 p2_rad2s(f32 r) { return gabi::call<s16>(0x02019510, r); } /* cM_rad2s */

/* 022B99FC */
void daNpc_P2_c::cutRopeTalkProc(int i_staffId) {
    WWHD_FUNC(0x022B99FC, void, this, i_staffId);
    mDoExt_McaMorf* morf = mpMorf;
    fopAc_ac_c* rope = mpRope;
    J3DModel* model = morf->getModel();
    gabi::store<u32>(gabi::ea(rope) + 0xB0, 2); /* the rope's state: hanging */
    p2_copy12(gabi::ea(&mRopePos), gabi::ea(&mpRope->current.pos));
    u32 hand = p2_getAnmMtx(model, 0x14);
    gabi::Local<cXyz> handPos;
    gabi::call(0x02587C88, hand, handPos.get()); /* dLib_getPosFromMtx */
    f32 omega = mRopeOmega;
    f32 px = mRopeSwingX * omega;
    f32 pz = mRopeSwingZ * omega;
    Mtx34* stack = mDoMtx_stack_c::get();
    gabi::call(0x025F181C, stack, (s32)(s16)-current.angle.z); /* mDoMtx_ZrotS */
    mDoMtx_XrotM(stack, (s16)-current.angle.x);
    mDoMtx_YrotM(stack, (s16)-current.angle.y);
    mDoMtx_stack_c::transM(-current.pos.x, -current.pos.y, -current.pos.z);
    gabi::Local<cXyz> local;
    PSMTXMultVec(stack, handPos.get(), local.get());
    /* the swing phases advance faster near the bottom of the swing */
    f32 inc;
    if (!(0.0f > px) && !(px > 1.5707964f)) {
        inc = -0.45f * p2_sinS(p2_rad2s(px)) + 1.0f;
    } else if (px > -1.5707964f) {
        inc = 0.0f + 1.0f;
    } else {
        inc = gabi::fmadds(0.45f, p2_sinS(p2_rad2s(px)), 1.0f);
    }
    mRopeSwingX = mRopeSwingX + inc;
    if (!(0.0f > pz) && !(pz > 1.5707964f)) {
        inc = gabi::fmadds(-0.45f, p2_sinS(p2_rad2s(pz)), 1.0f);
    } else if (pz > -1.5707964f) {
        inc = 0.0f + 1.0f;
    } else {
        inc = gabi::fmadds(0.45f, p2_sinS(p2_rad2s(pz)), 1.0f);
    }
    omega = mRopeOmega;
    f32 sx = mRopeSwingX;
    f32 sz = mRopeSwingZ;
    px = sx * omega;
    sz = sz + inc;
    pz = sz * omega;
    mRopeSwingZ = sz;
    if (!(px < 3.1415927f)) {
        px = px - 6.2831855f;
        mRopeSwingX = px / omega;
    }
    if (!(pz < 3.1415927f)) {
        pz = pz - 6.2831855f;
        mRopeSwingZ = pz / (f32)mRopeOmega;
    }
    gabi::call<BOOL>(0x0200F564, &mRopeTilt, 0, 0x20); /* cLib_chaseS */
    f32 c = p2_cosS(p2_rad2s(fabsf(px)));
    s16 amp = (s16)gabi::ftoi(gabi::fmadds(64.0f, fabsf(c), (f32)(s32)mRopeAmp));
    if (amp > 0x2EE0) {
        mRopeAmp = 0x2EE0;
    } else {
        mRopeAmp = amp;
    }
    s16 a = p2_rad2s(px);
    s16 rotX = (s16)gabi::ftoi((f32)(s32)-mRopeAmp * p2_sinS(a));
    a = p2_rad2s(pz);
    s16 rotZ = (s16)gabi::ftoi((f32)(s32)mRopeTilt * p2_sinS(a));
    a = p2_rad2s(px - 0.62831855f);
    s16 target = (s16)gabi::ftoi((f32)(s32)-mRopeAmp * p2_sinS(a));
    cLib_addCalcAngleS(&current.angle.x, target, 8, 0xC00, 0x100);
    f32 cz = p2_cosS(p2_rad2s(pz));
    a = p2_rad2s(pz - 1.0995574f);
    target = (s16)gabi::ftoi((f32)(s32)mRopeTilt * p2_sinS(a));
    s16 maxStep = (s16)gabi::ftoi(gabi::fmadds(2048.0f, cz, 1024.0f));
    s16 minStep = (s16)gabi::ftoi(gabi::fmadds(128.0f, cz, 128.0f));
    cLib_addCalcAngleS(&current.angle.z, target, 8, maxStep, minStep);
    gabi::Local<cXyz> v;
    v->z = 0.0f;
    v->y = -mRopeLen;
    v->x = 0.0f;
    PSMTXTrans(stack, mRopePos.x, mRopePos.y, mRopePos.z);
    mDoMtx_ZXYrotM(stack, rotX, current.angle.y, rotZ);
    PSMTXMultVec(stack, v.get(), &current.pos);
    PSMTXTrans(stack, current.pos.x, current.pos.y, current.pos.z);
    a = p2_rad2s(px + mRopeOmega);
    s16 rotX2 = (s16)gabi::ftoi((f32)(s32)-mRopeAmp * p2_sinS(a));
    mDoMtx_ZXYrotM(stack, rotX2, current.angle.y, rotZ);
    gabi::Local<cXyz> out;
    PSMTXMultVec(stack, local.get(), out.get());
    if (mRopeSwingZ > 0.0f && mRopeSwingX > 0.0f && mAnm == 0xB) {
        gabi::call(0x025E19CC, 0x201E, &mRopePos); /* mDoAud_seStart(id, pos) */
    }
    u16 r = talk(true);
    f32 z = mRopeSwingZ;
    f32 x;
    if (z > 0.0f) {
        x = mRopeSwingX;
        if (x > 0.0f) {
            if (r == 0x12 && mAnm == 0xB) {
                p2_monsSeStart(this, 0x489A);
                mAnm = 9;
                mbRopeHang = 0;
                gabi::store<u32>(gabi::ea(rope) + 0xB0, 3); /* the rope's state: released */
                dComIfGp_evmng_cutEnd(i_staffId);
                return;
            }
            mAnm = 0xC;
            return;
        }
        if (x < 0.0f) {
            mAnm = 0xB;
        }
        return;
    }
    if (!(z < 0.0f)) return;
    x = mRopeSwingX;
    if (x > 0.0f) {
        mAnm = 0xB;
        return;
    }
    if (!(x < 0.0f)) return;
    mAnm = 0xC;
}
VERIFY(0x022B99FC, &daNpc_P2_c::cutRopeTalkProc);
