/**
 * d_a_npc_p2_cut.cpp (WWHD)
 * NPC - Zuko, Niko, & Mako (Tetra's pirates): the event cuts of the "P2b" staff (Niko's rope
 * course: TALK, RIDE_SWITCH, RUN_WAIT, JUMP_TO_LIFT, LIFT_TO_ROPE, ROPE_TALK, ROPE_TO_LIFT,
 * JUMP_TO_GOAL, SET_ANM, JUMP, SW_ON, SW_OFF, SURPRISE, OMAMORI_INIT, OMAMORI_END).
 *
 * Written from the WWHD code (the GameCube functions
 * are "Nonmatching" stubs), verified against cking.rpx.
 */
#include "d/actor/d_a_npc_p2_cut.h"
using namespace P2Cut;
static SinCosTable sinCosTable;
static CutNames cutNames;
static f32 ropeDistance;
static u32 smokeScaleInitialized;
static cXyz smokeScale;

// Bindings and inline forms used by the event cuts.
static inline void* p2_getSubstance(s32 staffId, const char* name, s32 type) {
    return dComIfGp_evmng_getMySubstanceP(staffId, name, type);
}

static inline f32 p2_ldf(void* p, u32 index = 0) { return reinterpret_cast<be<f32>*>(p)[index]; }
/* the smoke follows the actor: mSmokePos = current.pos, mSmokeAngle = current.angle */
static inline void p2_setSmokePos(daNpc_P2_c* a) {
    a->mSmokePos = a->current.pos;
    a->mSmokeAngle.x = a->current.angle.x;
    a->mSmokeAngle.y = a->current.angle.y;
    a->mSmokeAngle.z = a->current.angle.z;
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

static inline bool p2_streq(const char* a, const char* literal) {
    const be<u8>* bytes = reinterpret_cast<const be<u8>*>(a);
    const be<u8>* b = gabi::at<be<u8>>(gabi::guest_addr(literal));
    for (u32 i = 0;; i++) {
        u8 c1 = bytes[i];
        u8 c2 = (u8)b[i];
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
}

static inline f32 p2_sinS(s16 a) { return sinCosTable.entries[(u32)(u16)a >> 3].sin; }
static inline f32 p2_cosS(s16 a) { return sinCosTable.entries[(u32)(u16)a >> 3].cos; }

/* 022B9338 */
void daNpc_P2_c::smoke_set() {
    // HD: the guard is stored first; scale components are written in X, Z, Y order.
    if (smokeScaleInitialized == 0) {
        smokeScaleInitialized = 1;
        smokeScale.x = 1.25f;
        smokeScale.z = 1.25f;
        smokeScale.y = 1.25f;
    }
    auto* cb = reinterpret_cast<SmokeCallback*>(mSmokeCB);
    if (cb->mpEmitter == nullptr) { /* mSmokeCB.getEmitter() */
        s8 room = current.roomNo;
        dPa_control_set(dComIfGp_getParticle(), 2, ID_AK_JT_ELEMENTSMOKE00, &mSmokePos, &mSmokeAngle, nullptr, 0xB9,
                        (dPa_levelEcallBack*)mSmokeCB, room, nullptr, nullptr, nullptr);
        if (cb->mpEmitter == nullptr) return;
    }
    SmokeEmitter* e = cb->mpEmitter;
    e->mRate = 2.0f;
    cb->mpEmitter->mSpread = 0.25f;
    cb->mpEmitter->mInitialVelOmni = 0.0f;
    cb->mpEmitter->mInitialVelAxis = 5.0f;
    cb->mpEmitter->mInitialVelDir = 20.0f;
    e = cb->mpEmitter;
    f32 sx = smokeScale.x;
    f32 sy = smokeScale.y;
    f32 sz = smokeScale.z;
    e->mGlobalDynamicsScale.x = sx; /* setGlobalScale */
    e->mGlobalDynamicsScale.y = sy;
    e->mGlobalDynamicsScale.z = sz;
    e->mGlobalParticleScale.x = sx; /* setGlobalParticleScale */
    e->mGlobalParticleScale.y = sy;
    e->mGlobalParticleScale.z = sz;
}

/* 022BAAA4 */
void daNpc_P2_c::cutProc() {
    s32 staffId = dComIfGp_evmng_getMyStaffId("P2b", nullptr, 0);
    if (staffId == -1) {
        mbInEvent = 0;
        return;
    }
    
    s32 actIdx = p2Cut_getMyActIdx(dComIfGp_getPEvtManager(), staffId, &cutNames, 15, 1, 0);
    if (actIdx == -1) {
        mbInEvent = 0;
        dComIfGp_evmng_cutEnd(staffId);
        return;
    }
    mbInEvent = 1;
    if (dComIfGp_evmng_getIsAddvance(staffId)) {
        mbEvtAttention = 0;
        switch ((u32)actIdx) {
        case CUT_TALK: cutTalkStart(staffId); break;
        case CUT_RIDE_SWITCH: cutRideSwitchStart(staffId); break;
        case CUT_RUN_WAIT: cutRunWaitStart(staffId); break;
        case CUT_JUMP_TO_LIFT: cutJumpToLiftStart(staffId); break;
        case CUT_LIFT_TO_ROPE: cutLiftToRopeStart(staffId); break;
        case CUT_ROPE_TALK: cutRopeTalkStart(staffId); break;
        case CUT_ROPE_TO_LIFT: cutRopeToLiftStart(staffId); break;
        case CUT_JUMP_TO_GOAL: cutJumpToGoalStart(staffId); break;
        case CUT_SET_ANM: cutSetAnmStart(staffId); break;
        case CUT_JUMP: cutJumpStart(staffId); break;
        case CUT_SW_ON: cutSwOnStart(staffId); break;
        case CUT_SW_OFF: cutSwOffStart(staffId); break;
        case CUT_SURPRISE: cutSurpriseStart(staffId); break;
        case CUT_OMAMORI_INIT: cutOmamoriInitStart(staffId); break;
        case CUT_OMAMORI_END: cutOmamoriEndStart(staffId); break;
        }
    }
    switch ((u32)actIdx) {
    case CUT_TALK: cutTalkProc(staffId); break;
    case CUT_RIDE_SWITCH: cutRideSwitchProc(staffId); break;
    case CUT_RUN_WAIT: cutRunWaitProc(staffId); break;
    case CUT_JUMP_TO_LIFT: cutJumpToLiftProc(staffId); break;
    case CUT_LIFT_TO_ROPE: cutLiftToRopeProc(staffId); break;
    case CUT_ROPE_TALK: cutRopeTalkProc(staffId); break;
    case CUT_ROPE_TO_LIFT: cutRopeToLiftProc(staffId); break;
    case CUT_JUMP_TO_GOAL: cutJumpToGoalProc(staffId); break;
    case CUT_SET_ANM: cutSetAnmProc(staffId); break;
    case CUT_JUMP: cutJumpProc(staffId); break;
    case CUT_SW_ON: cutSwOnProc(staffId); break;
    case CUT_SW_OFF: cutSwOffProc(staffId); break;
    case CUT_SURPRISE: cutSurpriseProc(staffId); break;
    case CUT_OMAMORI_INIT: cutOmamoriInitProc(staffId); break;
    case CUT_OMAMORI_END: cutOmamoriEndProc(staffId); break;
    }
}

/* 022B79F0 */
void daNpc_P2_c::cutTalkStart(int i_staffId) {
    be<u32>* msg = (be<u32>*)p2_getSubstance(i_staffId, "MsgNum", 3);
    mMsgNo = msg != nullptr ? (u32)*msg : 0;
    void* attn = p2_getSubstance(i_staffId, "Attention", 3);
    mbEvtAttention = attn != nullptr;
    talkInit();
}

/* 022B8FA8 */
void daNpc_P2_c::cutTalkProc(int i_staffId) {
    if (talk(true) == 0x12) {
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}

/* 022B7A84 */
void daNpc_P2_c::cutRideSwitchStart(int i_staffId) {
    void* sy = p2_getSubstance(i_staffId, "Speed_y", 0);
    void* g = p2_getSubstance(i_staffId, "Gravity", 0);
    speed.y = sy != nullptr ? p2_ldf(sy) : 16.0f;
    mbEvtAttention = 1;
    gravity = g != nullptr ? p2_ldf(g) : -2.0f;
    mAnm = ANM_JUMP_DOWN;
}

/* 022B8FF0 */
void daNpc_P2_c::cutRideSwitchProc(int i_staffId) {
    gabi::Local<be<s16>> name;
    *name = PROC_COURSE_SWITCH; /* the switch */
    fopAc_ac_c* sw = fopAcIt_Judge(P2Cut::JudgeByName, name.get());
    fopAcM_searchActorAngleY(this, sw);
    cXyz pos;
    pos.x = (f32)sw->current.pos.x;
    pos.y = (f32)sw->current.pos.y;
    pos.z = (f32)sw->current.pos.z;
    p2_distXZ(&pos, &current.pos);
    if (mAnm != ANM_LAND) {
        cLib_addCalc2(&current.pos.x, pos.x, 0.1f, 24.0f);
        cLib_addCalc2(&current.pos.z, pos.z, 0.1f, 24.0f);
    }
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_LANDING) {
        mTimer = 0x11;
        mAnm = ANM_LAND;
    }
    if (p2Cut_calcTimer(&mTimer) == 0 && mAnm == ANM_LAND) {
        speed.y = 0.0f;
        mAnm = ANM_WAIT;
        gravity = -9.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}

/* 022B7B58 */
void daNpc_P2_c::cutRunWaitStart(int i_staffId) {
    void* t = p2_getSubstance(i_staffId, "Timer", 3);
    mTimer = t != nullptr ? (s32)reinterpret_cast<be<s16>*>(t)[1] : 0;
    mAnm = ANM_RUN_WAIT;
    mbEvtAttention = 1;
}

/* 022B9190 */
void daNpc_P2_c::cutRunWaitProc(int i_staffId) {
    p2_setSmokePos(this);
    if (mAnm == ANM_RUN_WAIT && mpMorf->isStop()) {
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}

/* 022B922C */
static void* searchNearLift(void* i_actor, void* i_this) {
    fopAc_ac_c* a = (fopAc_ac_c*)i_actor;
    daNpc_P2_c* p = (daNpc_P2_c*)i_this;
    if (fopAc_IsActor(a) && a != nullptr && fpcM_GetName(a) == PROC_COURSE_LIFT) {
        if (std_sqrtf(PSVECSquareMag(&p->mNearPos)) == 0.0f) {
            p->mNearPos = a->current.pos;
            return nullptr;
        }
        cXyz diff;
        cXyz_mi(&p->mNearPos, &diff, &p->current.pos);
        f32 cur = std_sqrtf(PSVECSquareMag(&diff));
        cXyz_mi(&a->current.pos, &diff, &p->current.pos);
        if (std_sqrtf(PSVECSquareMag(&diff)) < cur) {
            p->mNearPos = a->current.pos;
            p->mpNearActor = a;
        }
    }
    return nullptr;
}

/* 022B7BCC */
void daNpc_P2_c::cutJumpToLiftStart(int i_staffId) {
    void* s = p2_getSubstance(i_staffId, "Speed", 0);
    void* sy = p2_getSubstance(i_staffId, "Speed_y", 0);
    void* g = p2_getSubstance(i_staffId, "Gravity", 0);
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 15.0f;
    mAnm = ANM_JUMP_PREPARE;
    mJumpGravity = g != nullptr ? p2_ldf(g) : -2.5f;
    mbEvtAttention = 1;
}

/* 022B9470 */
void daNpc_P2_c::cutJumpToLiftProc(int i_staffId) {
    fopAcIt_Judge(gabi::guest_fn((const void*)searchNearLift), this);
    fopAc_ac_c* lift = mpNearActor;
    cXyz pos;
    pos.x = (f32)lift->current.pos.x;
    pos.y = (f32)lift->current.pos.y;
    pos.z = (f32)lift->current.pos.z;
    f32 spd = mJumpSpeed;
    p2_distXZ(&pos, &current.pos);
    u32 flags = mObjAcch.m_flags;
    int next; /* 0: smoke off, 1: landing (smoke on), 2: smoke stays */
    if (flags & dBgS_Acch::GROUND_AWAY) {
        p2Cut_monsSeStart(this, JA_SE_CV_NK_JUMP_UP);
        speedF = (f32)mJumpSpeed;
        mAnm = ANM_JUMP_UP;
        speed.y = (f32)mJumpSpeedY;
        gravity = (f32)mJumpGravity;
        next = (mObjAcch.m_flags & dBgS_Acch::GROUND_LANDING) ? 1 : 0;
    } else {
        s8 anm = mAnm;
        if ((flags & dBgS_Acch::GROUND_HIT) && (anm == 9 || anm == 0xA)) {
            mLookOffs.z = 0.0f;
            speedF = 0.0f;
            mAnm = ANM_LAND;
            mLookOffs.x = 0.0f;
            mLookOffs.y = -100.0f;
            next = (mObjAcch.m_flags & dBgS_Acch::GROUND_LANDING) ? 1 : 0;
        } else if (!(speed.y > 0.0f) && anm == 8) {
            mLookOffs.z = 0.0f;
            mAnm = ANM_JUMP_DOWN;
            mLookOffs.x = 0.0f;
            mLookOffs.y = -50.0f;
            next = (mObjAcch.m_flags & dBgS_Acch::GROUND_LANDING) ? 1 : 0;
        } else if (anm == 5) {
            next = 2;
        } else {
            next = (mObjAcch.m_flags & dBgS_Acch::GROUND_LANDING) ? 1 : 0;
        }
    }
    if (next == 0) {
        p2_smokeEnd(this);
    } else if (next == 1) {
        p2Cut_monsSeStart(this, JA_SE_CV_NK_LANDING);
        mTimer = 0x11;
        smoke_set();
    } else {
        smoke_set();
    }
    p2_setSmokePos(this);
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_HIT) {
        if (mAnm == ANM_JUMP_PREPARE) {
            speedF = spd + spd;
            return;
        }
        s8 anm = mAnm;
        if (anm != 0xA && anm != 1) {
            cLib_addCalc2(&current.pos.x, pos.x, 0.1f, spd);
            cLib_addCalc2(&current.pos.z, pos.z, 0.1f, spd);
            return;
        }
    }
    if (p2Cut_calcTimer(&mTimer) == 0 && mAnm == ANM_LAND) {
        mLookOffs.z = 0.0f;
        speedF = 0.0f;
        mAnm = ANM_WAIT;
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

/* 022B7E24 */
static void* searchNearRope(void* i_actor, void* i_this) {
    fopAc_ac_c* a = (fopAc_ac_c*)i_actor;
    daNpc_P2_c* p = (daNpc_P2_c*)i_this;
    if (fopAc_IsActor(a) && a != nullptr && fpcM_GetName(a) == PROC_COURSE_ROPE) {
        if (std_sqrtf(PSVECSquareMag(&p->mNearPos)) == 0.0f) {
            p->mNearPos = a->current.pos;
            return nullptr;
        }
        cXyz diff;
        cXyz_mi(&p->mNearPos, &diff, &p->current.pos);
        f32 cur = std_sqrtf(PSVECSquareMag(&diff));
        cXyz_mi(&a->current.pos, &diff, &p->current.pos);
        if (std_sqrtf(PSVECSquareMag(&diff)) < cur) {
            p->mNearPos = a->current.pos;
            p->mpNearActor = a;
        }
    }
    return nullptr;
}

/* 022B7CF8 */
void daNpc_P2_c::cutLiftToRopeStart(int i_staffId) {
    void* s = p2_getSubstance(i_staffId, "Speed", 0);
    void* sy = p2_getSubstance(i_staffId, "Speed_y", 0);
    void* g = p2_getSubstance(i_staffId, "Gravity", 0);
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 15.0f;
    mAnm = ANM_JUMP_PREPARE;
    mJumpGravity = g != nullptr ? p2_ldf(g) : -2.5f;
    mbEvtAttention = 1;
}

/* 022B9848 */
void daNpc_P2_c::cutLiftToRopeProc(int i_staffId) {
    fopAcIt_Judge(gabi::guest_fn((const void*)searchNearRope), this);
    fopAc_ac_c* lift = mpNearActor;
    cXyz pos;
    pos.x = (f32)lift->current.pos.x;
    pos.y = (f32)lift->current.pos.y;
    pos.z = (f32)lift->current.pos.z;
    f32 spd = mJumpSpeed;
    p2_distXZ(&pos, &current.pos);
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_AWAY) {
        p2Cut_monsSeStart(this, JA_SE_CV_NK_JUMP_UP);
        f32 s = mJumpSpeed;
        f32 sy = mJumpSpeedY;
        f32 g = mJumpGravity;
        speedF = s;
        mAnm = ANM_JUMP_UP;
        gravity = g;
        speed.y = sy;
        p2_smokeEnd(this);
    } else if (mAnm == ANM_JUMP_PREPARE) {
        smoke_set();
    } else {
        p2_smokeEnd(this);
    }
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_HIT) {
        speedF = spd + spd;
    }
    if (mCyl.ChkCoHit()) {
        p2Cut_monsSeStart(this, JA_SE_CV_NK_GRAB);
        gravity = 0.0f;
        speed.y = 0.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}

/* 022B7F30 */
void daNpc_P2_c::cutRopeTalkStart(int i_staffId) {
    be<u32>* msg = (be<u32>*)p2_getSubstance(i_staffId, "MsgNum", 3);
    mMsgNo = msg != nullptr ? (u32)*msg : 0;
    void* attn = p2_getSubstance(i_staffId, "Attention", 3);
    mAnm = ANM_ROPE_FRONT;
    mbEvtAttention = attn != nullptr;
    talkInit();
    mbRopeHang = 1;
    fopAcIt_Judge(gabi::guest_fn((const void*)searchNearRope), this);
    fopAc_ac_c* rope = mpNearActor;
    mpRope = rope;
    cXyz diff;
    cXyz d;
    cXyz_mi(&rope->current.pos, &diff, &current.pos);
    d = diff;
    f32 len = std_sqrtf(PSVECSquareMag(&d));
    mRopeLen = len;
    f32 omega = std_sqrtf(2.0f / len);
    mRopeOmega = omega;
    f32 quarter = 1.5707964f / omega;
    u32 idx = ((u32)(u16)current.angle.y) >> 3;
    mRopeAmp = 0x2EE0;
    f32 dy = d.y;
    f32 dz = d.z;
    f32 s = sinCosTable.entries[idx].sin; /* JMA sin/cos table */
    f32 c = sinCosTable.entries[idx].cos;
    f32 dx = d.x;
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
    f32 ang = p2Cut_atan2f( ratio, std_sqrtf(gabi::fnmsubs(ratio, ratio, 1.0f))); /* cM_atan2f */
    mRopeSwingX = ang / mRopeOmega;
    s16 tilt = cM_atan2s(-rx, std_sqrtf(gabi::fmadds(dy, dy, rz * rz)));
    mRopeTilt = tilt;
    rope = mpRope;
    if (tilt <= 0) quarter = -quarter;
    mRopeSwingZ = quarter;
    cXyz_mi(&rope->current.pos, &diff, &current.pos);
    d = diff;
    f32 dist = std_sqrtf(PSVECSquareMag(&d)) - ropeDistance /* l_rope_dist */;
    f32 maxLen = reinterpret_cast<Rope*>(rope)->mMaxLength;
    if (dist > maxLen) dist = maxLen;
    cXyz scaled;
    p2Cut_normalize( &d, &diff); /* cXyz::normalize */
    cXyz_ml(&d, &scaled, dist);
    cXyz_mi(&mpRope->current.pos, &diff, &scaled);
    fopAc_ac_c* r = mpRope;
    current.pos = diff;
    speed.y = 0.0f;
    gravity = 0.0f;
    mRopePos = r->current.pos;
    m924 = 0;
}

/* 022B99FC */
void daNpc_P2_c::cutRopeTalkProc(int i_staffId) {
    mDoExt_McaMorf* morf = mpMorf;
    fopAc_ac_c* rope = mpRope;
    J3DModel* model = morf->getModel();
    reinterpret_cast<RopeState*>(rope)->mHangState = 2; /* the rope's state: hanging */
    mRopePos = mpRope->current.pos;
    u32 hand = gabi::ea(p2Cut_getAnmMtx(model, 0x14));
    cXyz handPos;
    p2Cut_getPosFromMtx( hand, &handPos); /* dLib_getPosFromMtx */
    f32 omega = mRopeOmega;
    f32 px = mRopeSwingX * omega;
    f32 pz = mRopeSwingZ * omega;
    Mtx34* stack = mDoMtx_stack_c::get();
    p2Cut_ZrotS( stack, (s32)(s16)-current.angle.z); /* mDoMtx_ZrotS */
    mDoMtx_XrotM(stack, (s16)-current.angle.x);
    mDoMtx_YrotM(stack, (s16)-current.angle.y);
    mDoMtx_stack_c::transM(-current.pos.x, -current.pos.y, -current.pos.z);
    cXyz local;
    PSMTXMultVec(stack, &handPos, &local);
    /* the swing phases advance faster near the bottom of the swing */
    f32 inc;
    if (!(0.0f > px) && !(px > 1.5707964f)) {
        inc = -0.45f * p2_sinS(p2Cut_rad2s(px)) + 1.0f;
    } else if (px > -1.5707964f) {
        inc = 0.0f + 1.0f;
    } else {
        inc = gabi::fmadds(0.45f, p2_sinS(p2Cut_rad2s(px)), 1.0f);
    }
    mRopeSwingX = mRopeSwingX + inc;
    if (!(0.0f > pz) && !(pz > 1.5707964f)) {
        inc = gabi::fmadds(-0.45f, p2_sinS(p2Cut_rad2s(pz)), 1.0f);
    } else if (pz > -1.5707964f) {
        inc = 0.0f + 1.0f;
    } else {
        inc = gabi::fmadds(0.45f, p2_sinS(p2Cut_rad2s(pz)), 1.0f);
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
    p2Cut_chaseS( &mRopeTilt, 0, 0x20); /* cLib_chaseS */
    f32 c = p2_cosS(p2Cut_rad2s(fabsf(px)));
    s16 amp = (s16)gabi::ftoi(gabi::fmadds(64.0f, fabsf(c), (f32)(s32)mRopeAmp));
    if (amp > 0x2EE0) {
        mRopeAmp = 0x2EE0;
    } else {
        mRopeAmp = amp;
    }
    s16 a = p2Cut_rad2s(px);
    s16 rotX = (s16)gabi::ftoi((f32)(s32)-mRopeAmp * p2_sinS(a));
    a = p2Cut_rad2s(pz);
    s16 rotZ = (s16)gabi::ftoi((f32)(s32)mRopeTilt * p2_sinS(a));
    a = p2Cut_rad2s(px - 0.62831855f);
    s16 target = (s16)gabi::ftoi((f32)(s32)-mRopeAmp * p2_sinS(a));
    cLib_addCalcAngleS(&current.angle.x, target, 8, 0xC00, 0x100);
    f32 cz = p2_cosS(p2Cut_rad2s(pz));
    a = p2Cut_rad2s(pz - 1.0995574f);
    target = (s16)gabi::ftoi((f32)(s32)mRopeTilt * p2_sinS(a));
    s16 maxStep = (s16)gabi::ftoi(gabi::fmadds(2048.0f, cz, 1024.0f));
    s16 minStep = (s16)gabi::ftoi(gabi::fmadds(128.0f, cz, 128.0f));
    cLib_addCalcAngleS(&current.angle.z, target, 8, maxStep, minStep);
    cXyz v;
    v.z = 0.0f;
    v.y = -mRopeLen;
    v.x = 0.0f;
    PSMTXTrans(stack, mRopePos.x, mRopePos.y, mRopePos.z);
    mDoMtx_ZXYrotM(stack, rotX, current.angle.y, rotZ);
    PSMTXMultVec(stack, &v, &current.pos);
    PSMTXTrans(stack, current.pos.x, current.pos.y, current.pos.z);
    a = p2Cut_rad2s(px + mRopeOmega);
    s16 rotX2 = (s16)gabi::ftoi((f32)(s32)-mRopeAmp * p2_sinS(a));
    mDoMtx_ZXYrotM(stack, rotX2, current.angle.y, rotZ);
    cXyz out;
    PSMTXMultVec(stack, &local, &out);
    if (mRopeSwingZ > 0.0f && mRopeSwingX > 0.0f && mAnm == ANM_ROPE_BACK) {
        p2Cut_seStartPos( JA_SE_LK_ROPE_SWING_F, &mRopePos); /* mDoAud_seStart(id, pos) */
    }
    u16 r = talk(true);
    f32 z = mRopeSwingZ;
    f32 x;
    if (z > 0.0f) {
        x = mRopeSwingX;
        if (x > 0.0f) {
            if (r == 0x12 && mAnm == ANM_ROPE_BACK) {
                p2Cut_monsSeStart(this, JA_SE_CV_NK_JUMP_DOWN);
                mAnm = ANM_JUMP_DOWN;
                mbRopeHang = 0;
                reinterpret_cast<RopeState*>(rope)->mHangState = 3; /* the rope's state: released */
                dComIfGp_evmng_cutEnd(i_staffId);
                return;
            }
            mAnm = ANM_ROPE_FRONT;
            return;
        }
        if (x < 0.0f) {
            mAnm = ANM_ROPE_BACK;
        }
        return;
    }
    if (!(z < 0.0f)) return;
    x = mRopeSwingX;
    if (x > 0.0f) {
        mAnm = ANM_ROPE_BACK;
        return;
    }
    if (!(x < 0.0f)) return;
    mAnm = ANM_ROPE_FRONT;
}

/* 022B82F4 */
void daNpc_P2_c::cutRopeToLiftStart(int i_staffId) {
    void* s = p2_getSubstance(i_staffId, "Speed", 0);
    void* sy = p2_getSubstance(i_staffId, "Speed_y", 0);
    void* g = p2_getSubstance(i_staffId, "Gravity", 0);
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 6.0f;
    f32 grav = g != nullptr ? p2_ldf(g) : -0.5f;
    current.angle.z = 0;
    gravity = grav;
    mbEvtAttention = 1;
    mJumpGravity = grav;
    speedF = (f32)mJumpSpeed;
    current.angle.x = 0;
    mAnm = ANM_JUMP_DOWN;
    speed.y = (f32)mJumpSpeedY;
}

/* 022BA1EC */
void daNpc_P2_c::cutRopeToLiftProc(int i_staffId) {
    fopAcIt_Judge(gabi::guest_fn((const void*)searchNearLift), this);
    fopAc_ac_c* lift = mpNearActor;
    cXyz pos;
    pos.x = (f32)lift->current.pos.x;
    pos.y = (f32)lift->current.pos.y;
    pos.z = (f32)lift->current.pos.z;
    p2_distXZ(&pos, &current.pos);
    p2_setSmokePos(this);
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_LANDING) {
        p2Cut_monsSeStart(this, JA_SE_CV_NK_LANDING);
        mTimer = 0x11;
        smoke_set();
    } else {
        p2_smokeEnd(this);
    }
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_HIT) {
        if (mAnm == ANM_JUMP_DOWN || mAnm == ANM_LAND) {
            speedF = 0.0f;
            mLookOffs.x = 0.0f;
            mAnm = ANM_LAND;
            mLookOffs.z = 0.0f;
            mLookOffs.y = -100.0f;
            speed.y = 0.0f;
            gravity = -9.0f;
        }
    }
    if (p2Cut_calcTimer(&mTimer) == 0 && mAnm == ANM_LAND) {
        mNearPos.y = 0.0f;
        mLookOffs.y = 0.0f;
        mAnm = ANM_WAIT;
        mLookOffs.z = 0.0f;
        mNearPos.x = 0.0f;
        mLookOffs.x = 0.0f;
        mNearPos.z = 0.0f;
        p2_smokeEnd(this);
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}

/* 022B8460 */
void daNpc_P2_c::cutJumpToGoalStart(int i_staffId) {
    void* s = p2_getSubstance(i_staffId, "Speed", 0);
    void* sy = p2_getSubstance(i_staffId, "Speed_y", 0);
    void* g = p2_getSubstance(i_staffId, "Gravity", 0);
    void* pos = p2_getSubstance(i_staffId, "Pos", 1);
    if (pos == nullptr) {
        mJumpPos = p2_child(mType)->mGoalTalkPos;
    } else {
        mJumpPos.x = p2_ldf(pos, 0);
        mJumpPos.y = p2_ldf(pos, 1);
        mJumpPos.z = p2_ldf(pos, 2);
    }
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 15.0f;
    mAnm = ANM_JUMP_PREPARE;
    mJumpGravity = g != nullptr ? p2_ldf(g) : -2.5f;
    mbEvtAttention = 1;
}

/* 022BA3D0 */
void daNpc_P2_c::cutJumpToGoalProc(int i_staffId) {
    cXyz pos;
    pos.y = (f32)mJumpPos.y;
    pos.z = (f32)mJumpPos.z;
    pos.x = (f32)mJumpPos.x;
    p2_distXZ(&pos, &current.pos);
    f32 spd = mJumpSpeed;
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_AWAY) {
        p2Cut_monsSeStart(this, JA_SE_CV_NK_JUMP_UP);
        speedF = (f32)mJumpSpeed;
        speed.y = (f32)mJumpSpeedY;
        mAnm = ANM_JUMP_UP;
        gravity = (f32)mJumpGravity;
    }
    bool landed = false;
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_HIT) {
        s8 anm = mAnm;
        bool chk = true;
        if (anm != 0xA && anm != 1) {
            cLib_addCalc2(&current.pos.x, pos.x, 0.1f, spd);
            cLib_addCalc2(&current.pos.z, pos.z, 0.1f, spd);
            chk = (mObjAcch.m_flags & dBgS_Acch::GROUND_HIT) != 0;
        }
        if (chk && (mAnm == ANM_JUMP_DOWN || mAnm == ANM_LAND)) {
            mLookOffs.x = 0.0f;
            mAnm = ANM_LAND;
            mLookOffs.y = -100.0f;
            mLookOffs.z = 0.0f;
            landed = true;
        }
    }
    if (!landed) {
        if (!(speed.y > 0.0f) && mAnm == ANM_JUMP_UP) {
            mLookOffs.x = 0.0f;
            mLookOffs.z = 0.0f;
            mLookOffs.y = -50.0f;
            mAnm = ANM_JUMP_DOWN;
        }
    }
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_LANDING) {
        p2Cut_monsSeStart(this, JA_SE_CV_NK_LANDING);
        speedF = 0.0f;
        mTimer = 0x11;
        smoke_set();
    } else {
        p2_smokeEnd(this);
    }
    p2_setSmokePos(this);
    if (p2Cut_calcTimer(&mTimer) == 0 && mAnm == ANM_LAND) {
        mLookOffs.z = 0.0f;
        mLookOffs.x = 0.0f;
        mLookOffs.y = 0.0f;
        speed.y = 0.0f;
        speedF = 0.0f;
        gravity = -9.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
        mAnm = ANM_WAIT;
    }
}

/* 022B8724 */
void daNpc_P2_c::cutJumpStart(int i_staffId) {
    void* s = p2_getSubstance(i_staffId, "Speed", 0);
    void* sy = p2_getSubstance(i_staffId, "Speed_y", 0);
    void* g = p2_getSubstance(i_staffId, "Gravity", 0);
    void* pos = p2_getSubstance(i_staffId, "Pos", 1);
    if (pos == nullptr) {
        mJumpPos = p2_child(mType)->mGoalTalkPos;
    } else {
        mJumpPos.x = p2_ldf(pos, 0);
        mJumpPos.y = p2_ldf(pos, 1);
        mJumpPos.z = p2_ldf(pos, 2);
    }
    mJumpSpeed = s != nullptr ? p2_ldf(s) : 10.0f;
    mJumpSpeedY = sy != nullptr ? p2_ldf(sy) : 15.0f;
    mJumpGravity = g != nullptr ? p2_ldf(g) : -2.5f;
    mbEvtAttention = 1;
    p2Cut_monsSeStart(this, JA_SE_CV_NK_JUMP_UP);
    speedF = (f32)mJumpSpeed;
    mAnm = ANM_JUMP_UP;
    speed.y = (f32)mJumpSpeedY;
    gravity = (f32)mJumpGravity;
}

/* 022BA720 */
void daNpc_P2_c::cutJumpProc(int i_staffId) {
    cXyz pos;
    pos.x = (f32)mJumpPos.x;
    pos.y = (f32)mJumpPos.y;
    pos.z = (f32)mJumpPos.z;
    p2_distXZ(&pos, &current.pos);
    if ((mObjAcch.m_flags & dBgS_Acch::GROUND_HIT) && (mAnm == ANM_JUMP_DOWN || mAnm == ANM_LAND)) {
        mLookOffs.x = 0.0f;
        mLookOffs.y = -100.0f;
        mAnm = ANM_LAND;
        mLookOffs.z = 0.0f;
    } else if (!(speed.y > 0.0f) && mAnm == ANM_JUMP_UP) {
        mLookOffs.x = 0.0f;
        mAnm = ANM_JUMP_DOWN;
        mLookOffs.y = -50.0f;
        mLookOffs.z = 0.0f;
    }
    if (mObjAcch.m_flags & dBgS_Acch::GROUND_LANDING) {
        p2Cut_monsSeStart(this, JA_SE_CV_NK_LANDING);
        speedF = 0.0f;
        mTimer = 0x11;
        smoke_set();
    } else {
        p2_smokeEnd(this);
    }
    p2_setSmokePos(this);
    if (p2Cut_calcTimer(&mTimer) == 0 && mAnm == ANM_LAND) {
        mLookOffs.z = 0.0f;
        mLookOffs.x = 0.0f;
        mLookOffs.y = 0.0f;
        speed.y = 0.0f;
        speedF = 0.0f;
        gravity = -9.0f;
        dComIfGp_evmng_cutEnd(i_staffId);
        mAnm = ANM_WAIT;
    }
}

/* 022B85E4 */
void daNpc_P2_c::cutSetAnmStart(int i_staffId) {
    void* name = p2_getSubstance(i_staffId, "Name", 4);
    if (name != nullptr) {
        const char* n = static_cast<char*>(name);
        if (p2_streq(n, "KYORO")) {
            mAnm = ANM_KYORO;
            return;
        }
        if (p2_streq(n, "SURPRISE")) {
            mAnm = ANM_SURPRISE;
            return;
        }
        if (p2_streq(n, "THINK")) {
            mAnm = ANM_THINK;
            return;
        }
        if (p2_streq(n, "NOD")) {
            mAnm = ANM_NOD;
            return;
        }
    }
    mAnm = ANM_WAIT;
}

/* 022BA6C4 */
void daNpc_P2_c::cutSetAnmProc(int i_staffId) {
    if (mpMorf->isStop()) {
        dComIfGp_evmng_cutEnd(i_staffId);
    }
}

/* 022B8920 */
void daNpc_P2_c::cutSwOnStart(int i_staffId) {
    u8 sw = mSwitchNo;
    if (sw != 0xFF) {
        dComIfGs_onSwitch(sw, home.roomNo);
    }
}

/* 022BA944 */
void daNpc_P2_c::cutSwOnProc(int i_staffId) {
    u8 sw = mSwitchNo;
    if (sw != 0xFF) {
        dComIfGs_onSwitch(sw, home.roomNo);
    }
    dComIfGp_evmng_cutEnd(i_staffId);
}

/* 022B8944 */
void daNpc_P2_c::cutSwOffStart(int i_staffId) {
    u8 sw = mSwitchNo;
    if (sw != 0xFF) {
        dComIfGs_offSwitch(sw, home.roomNo);
    }
}

/* 022BA9A0 */
void daNpc_P2_c::cutSwOffProc(int i_staffId) {
    u8 sw = mSwitchNo;
    if (sw != 0xFF) {
        dComIfGs_offSwitch(sw, home.roomNo);
    }
    dComIfGp_evmng_cutEnd(i_staffId);
}

/* 022B8968 */
void daNpc_P2_c::cutSurpriseStart(int i_staffId) {
    p2Cut_seStart(JA_SE_ITM_OMAMORI_BLINK);
    // HD: ask the player to enter the omamori reaction demo mode.
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    reinterpret_cast<Player*>(player)->changeDemoMode(0x1C);
}

/* 022BA9FC */
void daNpc_P2_c::cutSurpriseProc(int i_staffId) {
    dComIfGp_evmng_cutEnd(i_staffId);
}

/* 022B89A8 */
u32 daNpc_P2_c::cutOmamoriInitStart(int i_staffId) {
    return p2Cut_seStart( JA_SE_ITM_OMAMORI_TETLA); /* p2_seStart */
}

/* 022BAA34 */
void daNpc_P2_c::cutOmamoriInitProc(int i_staffId) {
    dComIfGp_evmng_cutEnd(i_staffId);
}

/* 022B89B0 */
u32 daNpc_P2_c::cutOmamoriEndStart(int i_staffId) {
    return p2Cut_seStart( JA_SE_ITM_OMAMORI_ED_TALK); /* p2_seStart */
}

/* 022BAA6C */
void daNpc_P2_c::cutOmamoriEndProc(int i_staffId) {
    dComIfGp_evmng_cutEnd(i_staffId);
}

/* WWHD glue
str 1001FE70 "P2b" in 022BAAA4
ext 02019510 s16 p2Cut_rad2s(f32 radians)
ext 025E1AA4 void p2Cut_monsSeStartCall(u32 sound, cXyz* position, u32 processId, u32 param, s32 reverb)
data sinCosTable 104A44F8
data cutNames 101C2E1C
data ropeDistance 10467FE0
data smokeScaleInitialized 104682F0
data smokeScale 104682F4
str 1001FD00 "MsgNum" in 022B79F0
str 1001FD08 "Attention" in 022B79F0
str 1001FD1C "Speed_y" in 022B7A84
str 1001FD24 "Gravity" in 022B7A84
str 1001FD2C "Timer" in 022B7B58
str 1001FD4C "Speed" in 022B7BCC
str 1001FD3C "Speed_y" in 022B7BCC
str 1001FD44 "Gravity" in 022B7BCC
str 1001FD64 "Speed" in 022B7CF8
str 1001FD54 "Speed_y" in 022B7CF8
str 1001FD5C "Gravity" in 022B7CF8
str 1001FD80 "MsgNum" in 022B7F30
str 1001FD88 "Attention" in 022B7F30
str 1001FDAC "Speed" in 022B82F4
str 1001FD9C "Speed_y" in 022B82F4
str 1001FDA4 "Gravity" in 022B82F4
str 1001FDC8 "Speed" in 022B8460
str 1001FDB4 "Speed_y" in 022B8460
str 1001FDBC "Gravity" in 022B8460
str 1001FDC4 "Pos" in 022B8460
str 1001FDE4 "Name" in 022B85E4
str 1001FDD4 "KYORO" in 022B85E4
str 1001FDEC "SURPRISE" in 022B85E4
str 1001FDDC "THINK" in 022B85E4
str 1001FDD0 "NOD" in 022B85E4
str 1001FE0C "Speed" in 022B8724
str 1001FDF8 "Speed_y" in 022B8724
str 1001FE00 "Gravity" in 022B8724
str 1001FE08 "Pos" in 022B8724
ext 0211D2F8 s32 p2Cut_calcTimer(be<s32>* timer)
ext 025E1988 u32 p2Cut_seStart(u32 sound)
ext 0201971C f32 p2Cut_atan2f(f32 y, f32 x)
ext 0201B31C void p2Cut_normalize(const cXyz* vector, cXyz* out)
ext 02587C88 void p2Cut_getPosFromMtx(u32 matrix, cXyz* out)
ext 025F181C void p2Cut_ZrotS(Mtx34* matrix, s32 angle)
ext 0200F564 BOOL p2Cut_chaseS(be<s16>* value, s16 target, s16 step)
ext 025E19CC void p2Cut_seStartPos(u32 sound, cXyz* pos)
ext 02542EDC s32 p2Cut_getMyActIdx(void* manager, s32 staffId, P2Cut::CutNames* names, s32 count, s32 force, s32 unused)
*/
