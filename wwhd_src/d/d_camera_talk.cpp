/**
 * d_camera_talk.cpp (WWHD)
 * Follow camera dCamera_c: talktoCamera (conversations).
 *
 * A "Nonmatching" stub in the GameCube decompilation;
 * written from the WWHD code and verified against cking.rpx.
 */
#include "d/d_camera_talk.h"
using namespace talkDetail;

static s16 talkAngleZero;
static s16 talkAngle90;
static s16 talkAngle270;
static TalkEventNames talkEventNames;

static inline void copyTalkFloatBits(void* dst, const void* src) {
    *gabi::at<be<u32>>(gabi::ea(dst)) = *gabi::at<be<u32>>(gabi::ea(src));
}
static inline s16 talkAngleShortValue(cSAngle_l* tmp, s16 v) {
    return *talk_angleFromShort(tmp, v);
}

static inline bool chkCamera(cXyz* a, cXyz* b, f32 r, fopAc_ac_c* a1, fopAc_ac_c* a2) {
    return talk_ChkCamera(dComIfG_Ccsp(), a, b, r, a1, a2); /* dCcS::ChkCamera */
}
static inline void cpXyz(cXyz* d, const cXyz* s) { /* lfs/stfs copy */
    copyTalkFloatBits(&d->x, &s->x);
    copyTalkFloatBits(&d->y, &s->y);
    copyTalkFloatBits(&d->z, &s->z);
}
/* mViewCache.mEye = mCenter + mDirection.Xyz() */
static void talkEye(dCamera_c* c) {
    gabi::Local<cXyz> x, e;
    talk_globeXyz(&c->mViewCache.mDirection, x.get());
    cXyz_pl(&c->mViewCache.mCenter, e, x);
    c->mViewCache.mEye.copy(*e);
}
static inline void flagsDone(dCamera_c* c) {
    c->m102 = 1;
    c->m101 = 1;
    c->m100 = 1;
}
/* m10 = m04 + m28.Xyz(), then pushed out by radiusActorInSight */
static void talkFinish(dCamera_c* c, TalkWork* work, fopAc_ac_c* a1, fopAc_ac_c* a2) {
    gabi::Local<cXyz> x, e;
    s32 n = work->mTransitionFrames;
    work->mWeightSum = (f32)((n * (n + 1)) >> 1);
    talk_globeXyz(&work->mDirection, x.get());
    cXyz_pl(&work->mCenter, e, x);
    work->mEye.copy(*e);
    f32 r = c->radiusActorInSight6(a1, a2, &work->mCenter, &work->mEye, work->mFovy, 0);
    if (r > 0.0f) {
        gabi::Local<cXyz> x2, e2;
        work->mDirection.mRadius = work->mDirection.mRadius + r;
        talk_globeXyz(&work->mDirection, x2.get());
        cXyz_pl(&work->mCenter, e2, x2);
        work->mEye.copy(*e2);
    }
}

/* the over-the-shoulder shots (types 0x10/0x11, 0x14/0x15, 0x16/0x17, 0x1D): the centre at the
 * speaker, the direction from the listener; first frame: the radius and the height offset */
static void talkShoulder(dCamera_c* c, TalkWork* work, fopAc_ac_c* listener, fopAc_ac_c* speaker,
                         f32 radius, f32 dy, f32 fovy, bool attnFirst) {
    listener->actor_status = listener->actor_status | fopAcStts_NODRAW_e;
    gabi::Local<cXyz> pos;
    c->positionOf(pos, speaker);
    c->mViewCache.mCenter.copy(*pos);
    if (work->mShotFrames == 0) {
        gabi::Local<cXyz> a, b, d, ep, sp;
        c->positionOf(a, listener);
        c->positionOf(b, speaker);
        cXyz_mi(a, d, b);
        cSGlobe_Val(&c->mViewCache.mDirection, d);
        c->mViewCache.mDirection.mRadius = radius;
        if (attnFirst)
            c->attentionPos(d, speaker);
        else
            c->eyePos(d, speaker);
        c->positionOf(sp, speaker);
        work->mShotCenter.y = (d->y - dy) - sp->y;
        flagsDone(c);
    }
    gabi::Local<cXyz> pos2;
    c->positionOf(pos2, speaker);
    c->mViewCache.mCenter.y = pos2->y + work->mShotCenter.y;
    talkEye(c);
    c->mViewCache.mFovy = fovy;
}

/* the side shots (types 0x12/0x13, 0x18/0x19, 0x1A/0x1B): the centre from an offset globe around
 * B rotated by the A -> B direction (first frame), then a fixed globe around it */
static void talkSide(dCamera_c* c, TalkWork* work, fopAc_ac_c* A, fopAc_ac_c* B, bool side,
                     f32 ofsY, bool attnA, f32 u0, f32 u1, f32 radius, f32 lat, f32 fovy) {
    if (work->mShotFrames == 0) {
        gabi::Local<cXyz> o, ea, ab, d, eb, x, s;
        gabi::Local<cSGlobe_l> gb, g30;
        gabi::Local<cSAngle_l> t, tmp;
        o->x = 0.0f;
        o->z = 20.0f;
        o->y = ofsY;
        flagsDone(c);
        if (attnA)
            c->attentionPos(ea, A);
        else
            c->eyePos(ea, A);
        c->attentionPos(ab, B);
        cXyz_mi(ea, d, ab);
        talk_globeFromVector(gb.get(), d.get());
        talk_globeFromVector(g30.get(), o.get());
        talk_angleAdd(&g30->mU, t, &gb->mU);
        g30->mU = talkAngleShortValue(tmp, *t);
        c->eyePos(eb, B);
        talk_globeXyz(g30.get(), x.get());
        cXyz_pl(eb, s, x);
        cpXyz(&work->mShotCenter, s);
        copyTalkFloatBits(&c->mViewCache.mCenter.y, &s->y);
        copyTalkFloatBits(&c->mViewCache.mCenter.x, &s->x);
        copyTalkFloatBits(&c->mViewCache.mCenter.z, &work->mShotCenter.z);
    } else {
        copyTalkFloatBits(&c->mViewCache.mCenter.y, &work->mShotCenter.y);
        copyTalkFloatBits(&c->mViewCache.mCenter.x, &work->mShotCenter.x);
        copyTalkFloatBits(&c->mViewCache.mCenter.z, &work->mShotCenter.z);
    }
    gabi::Local<cSAngle_l> a, dir, u, l;
    talk_angleFromDegrees(a.get(), side ? u1 : u0);
    c->directionOf(dir, B);
    talk_angleAdd(a, u, dir);
    if (lat == 0.0f) {
        talk_globeVal(&c->mViewCache.mDirection, radius,
                      reinterpret_cast<cSAngle_l*>(&talkAngleZero), u.get());
    } else {
        cSAngle_l* lp = talk_angleFromDegrees(l.get(), lat);
        talk_globeVal(&c->mViewCache.mDirection, radius, lp, u.get());
    }
    talkEye(c);
    c->mViewCache.mFovy = fovy;
}

/* 02508A60 */
// HD: conversation shot selection uses the talk table, shoulder/side/close-up shots and collision
// avoidance. The corresponding GameCube method is still a nonmatching stub.
bool dCamera_c::talktoCamera(s32 style) {
    TalkWork* work = (TalkWork*)mWork;
    f32 centerOffsetY = camParamVal(style, 5);
    f32 centerOffsetZ = camParamVal(style, 0);
    f32 defaultRadius = camParamVal(style, 10);
    f32 minLatitude = camParamVal(style, 18);
    f32 defaultFovy = camParamVal(style, 25);
    f32 centerOffsetX = camParamVal(style, 1);
    f32 minLongitude = camParamVal(style, 24);
    f32 defaultLatitude = camParamVal(style, 15);
    f32 maxLatitude = camParamVal(style, 19);
    f32 latitudeScale = camParamVal(style, 3);
    bool ret = true;
    if (m11C == 0) {
        work->mShotFrames = 0;
        work->mSide = -1;
        work->mTransitionFrames = 20;
        work->mShotType = 0;
        work->mPreviousShotType = -1;
        work->mLongitude = 999.9f;
        work->mLatitude = defaultLatitude;
        work->mTag = TALK_WORK_TAG; /* 'TALK' */
        if (reinterpret_cast<TalkEventManagerHD*>(dComIfGp_getPEvtManager())->mCameraPlay.get() ==
            0) {
            work->mSmoothless = 0;
            work->mMode = 0;
            work->mRadius = defaultRadius;
            work->mFovy = defaultFovy;
            work->mpListener = mpPlayerActor.get();
            fopAc_ac_c* t = mpLockonTarget.get();
            work->mpSpeaker = t;
            work->mpPreviousSpeaker = t;
        } else {
            gabi::Local<be<s32>> iv;
            talk_getEvIntData(this, iv.get(), talkEventNames.mSmoothless,
                              0); /* getEvIntData("Smoothless") */
            work->mSmoothless = (s16)(s32)*iv;
            talk_getEvIntData(this, iv.get(), talkEventNames.mMode, 0); /* "Mode" */
            work->mMode = (s16)(s32)*iv;
            talk_getEvFloatData(this, &work->mRadius, talkEventNames.mRadius,
                                defaultRadius); /* getEvFloatData("Radius") */
            talk_getEvFloatData(this, &work->mLongitude, talkEventNames.mLongitude,
                                999.9f); /* "Longitude" */
            talk_getEvFloatData(this, &work->mFovy, talkEventNames.mFovy, defaultFovy); /* "Fovy" */
            talk_getEvFloatData(this, &work->mLatitude, talkEventNames.mLatitude,
                                defaultLatitude); /* "Latitude" */
            work->mpListener =
                talk_getEvActor(this, talkEventNames.mListener,
                                talkEventNames.mStarter); /* getEvActor("Listener", "@STARTER") */
            fopAc_ac_c* s =
                talk_getEvActor(this, talkEventNames.mSpeaker,
                                talkEventNames.mTalkPartner); /* ("Speaker", "@TALKPARTNER") */
            work->mpSpeaker = s;
            work->mpPreviousSpeaker = s;
        }
    }
    fopAc_ac_c* a1;
    fopAc_ac_c* a2 = gabi::at<fopAc_ac_c>(talk_getTableEntryHD()); // HD: talk table entry
    bool same;
    if (a2 != nullptr) {
        a1 = work->mpListener.get();
        same = a1 == a2;
    } else if (reinterpret_cast<TalkEventManagerHD*>(dComIfGp_getPEvtManager())
                   ->mCameraPlay.get() != 0) {
        u32 c11C = m11C;
        a1 = work->mpListener.get();
        if (c11C != 0)
            a2 = talk_getEvActor(this, talkEventNames.mSpeaker, talkEventNames.mTalkPartner);
        else
            a2 = work->mpSpeaker.get();
        same = a1 == a2;
    } else {
        a2 = mpLockonTarget.get();
        a1 = mpPlayerActor.get();
        same = a1 == a2;
    }
    if (same || a1 == nullptr || a2 == nullptr) {
        flagsDone(this);
        return false;
    }
    if (work->mpPreviousSpeaker.get() != a2) {
        m11C = 0;
        m100 = 0;
        work->mShotFrames = 0;
        work->mpPreviousSpeaker = a2;
    }
    {
        s32 t = talk_getTableIndexHD(); // HD: talk shot type
        s32 prev = work->mPreviousShotType;
        work->mShotType = t;
        if (t != prev)
            work->mShotFrames = 0;
    }
    gabi::Local<cSAngle_l> latHi, latLo, lonMax, lonMin, tmp;
    talk_angleFromDegrees(latHi.get(), maxLatitude);
    talk_angleFromDegrees(latLo.get(), minLatitude);
    talk_angleFromDegrees(lonMax.get(), 180.0f - minLongitude);
    talk_angleFromDegrees(lonMin.get(), minLongitude);
    u32 c11C = m11C;
    if (c11C == 0 || (a2 != nullptr && fpcM_GetName(a2) == fpcNm_SHIP_e && c11C <= 1)) {
        gabi::Local<cXyz> ofs, at1, at2, ep, r;
        ofs->x = centerOffsetX;
        ofs->y = centerOffsetY;
        ofs->z = centerOffsetZ;
        attentionPos(at1, a1);
        eyePos(ep, a1);
        copyTalkFloatBits(&at1->y, &ep->y);
        attentionPos(at2, a2);
        eyePos(ep, a2);
        copyTalkFloatBits(&at2->y, &ep->y);
        talk_relationalPos(this, r.get(), a1, a2, ofs.get(),
                           0.25f); /* relationalPos(a1, a2, ofs, 0.25) */
        work->mCenter.copy(*r);
        gabi::Local<cSAngle_l> lon;
        cSAngle_ct(lon);
        if (work->mLongitude != 999.9f) {
            talk_angleValDegrees(lon.get(), (f32)work->mLongitude);
        } else {
            s16 ty = cLib_targetAngleY(&work->mCenter, &mViewCache.mEye);
            cSAngle_Val(lon, ty);
        }
        gabi::Local<cSGlobe_l> g;
        {
            gabi::Local<cXyz> d;
            copyTalkFloatBits(&work->mDirection.mRadius, &mViewCache.mDirection.mRadius);
            copyTalkFloatBits(&work->mDirection.mV, &mViewCache.mDirection.mV);
            cXyz_mi(at1, d, at2);
            talk_globeFromVector(g.get(), d.get());
            copyTalkFloatBits(&work->mActorDirection.mRadius, &g->mRadius);
            copyTalkFloatBits(&work->mDirection.mRadius, &work->mRadius);
            copyTalkFloatBits(&work->mActorDirection.mV, &g->mV);
            /* the copy keeps the radius >= 88 on the stack (g is used below as work->mActorDirection's twin) */
            if (g->mRadius < 88.0f)
                g->mRadius = 88.0f;
            if (a2 != nullptr && fpcM_GetName(a2) == fpcNm_SHIP_e) {
                /* ship: look past the ship's bow side */
                gabi::Local<cSAngle_l> tA, dir, rel, side, t2;
                s16 ty = cLib_targetAngleY(positionPntOf(a2), at2);
                cSAngle_ct(tA, ty);
                directionOf(dir, a2);
                talk_angleSubtract(dir, rel, tA);
                cSAngle_ct(side);
                cSAngle_l* sp;
                if ((s16)*rel < talkAngleZero)
                    sp = talk_angleFromDegrees(t2.get(), -30.0f);
                else
                    sp = talk_angleFromDegrees(t2.get(), 30.0f);
                *side = (s16)*sp;
                talk_angleAdd(&g->mU, dir, side);
                work->mDirection.mU = talkAngleShortValue(tmp, *dir);
                cSAngle_l* lp = talk_angleFromDegrees(t2.get(), defaultLatitude);
                work->mDirection.mV = talkAngleShortValue(tmp, *lp);
                talkFinish(this, work, a1, a2);
            } else if (a2 != nullptr && fpcM_GetName(a2) == fpcNm_KANBAN_e) {
                gabi::Local<cSAngle_l> dir, tf0, tee, t2;
                directionOf(dir, a2);
                g->mU = talkAngleShortValue(tmp, *dir);
                talk_angleSubtract(lon, tf0, &g->mU);
                talk_angleMultiply(tf0, tee, 0.25f);
                talk_angleAdd(&g->mU, dir, tee);
                work->mDirection.mU = talkAngleShortValue(tmp, *dir);
                cSAngle_l* lp = talk_angleFromDegrees(t2.get(), (f32)work->mLatitude);
                work->mDirection.mV = talkAngleShortValue(tmp, *lp);
                talkFinish(this, work, a1, a2);
            } else {
                gabi::Local<cSAngle_l> rel, neg, t2c, tf4, tf2, lat15, step, t14;
                talk_angleSubtract(lon, rel, &g->mU);
                s16 r = *rel;
                if (r > (s16)*lonMax) {
                    r = *lonMax;
                    *rel = r;
                }
                s16 zero = talkAngleZero;
                if (r > zero && r < (s16)*lonMin)
                    *rel = (s16)*lonMin;
                talk_angleNegate(lonMax.get(), t2c.get()); /* -lonMax */
                r = *rel;
                if (r < (s16)*t2c) {
                    talk_angleNegate(lonMax.get(), neg.get());
                    r = *neg;
                    *rel = r;
                }
                if (r < zero) {
                    talk_angleNegate(lonMin.get(), t2c.get()); /* -lonMin */
                    if ((s16)*rel > (s16)*t2c) {
                        talk_angleNegate(lonMin.get(), neg.get());
                        *rel = (s16)*neg;
                    }
                }
                talk_angleAdd(&g->mU, t2c, rel);
                work->mDirection.mU = talkAngleShortValue(tmp, *t2c);
                talk_angleSubtract(&g->mU, tf4, lon);
                f32 c = talk_angleCos(tf4) + 0.1f;
                talk_angleMultiply(&g->mV, tf2, c);
                talk_angleMultiply(tf2, t2c, latitudeScale);
                cSAngle_l* lp = talk_angleFromDegrees(lat15.get(), defaultLatitude);
                gabi::Local<cSAngle_l> va;
                talk_angleAdd(t2c, va, lp);
                s16 v = *va;
                if (v > (s16)*latHi) {
                    v = *latHi;
                    *va = v;
                }
                if (v < (s16)*latLo) {
                    v = *latLo;
                    *va = v;
                }
                work->mDirection.mV = talkAngleShortValue(tmp, v);
                cSAngle_ct(step);
                talk_angleSubtract(&g->mU, tf4, &work->mDirection.mU);
                gabi::Local<cSAngle_l> ts;
                if ((s16)*tf4 > zero)
                    *step = (s16)*talk_angleFromDegrees(ts.get(), 10.0f);
                else
                    *step = (s16)*talk_angleFromDegrees(ts.get(), -10.0f);
                bool done = false;
                for (s32 i = 36; i != 0; i--) {
                    talk_angleSubtract(&work->mDirection.mU, t14, &g->mU);
                    f32 dg = talk_angleDegree(t14.get()); /* Degree */
                    if (__builtin_fabsf(dg) < 10.0f) {
                        gabi::Local<cSAngle_l> t16;
                        talk_angleAdd(&work->mDirection.mU, t16, step);
                        work->mDirection.mU = talkAngleShortValue(tmp, *t16);
                        continue;
                    }
                    {
                        gabi::Local<cXyz> x, e, pp;
                        talk_globeXyz(&work->mDirection, x.get());
                        cXyz_pl(&work->mCenter, e, x);
                        work->mEye.copy(*e);
                        positionOf(pp, mpPlayerActor.get());
                        if (work->mEye.y < pp->y + m368) {
                            gabi::Local<cXyz> pp2, dd;
                            positionOf(pp2, mpPlayerActor.get());
                            work->mEye.y = (pp2->y + m368) + 10.0f;
                            cXyz_mi(&work->mEye, dd, &work->mCenter);
                            cSGlobe_Val(&work->mDirection, dd);
                        }
                    }
                    bool hit =
                        talk_lineBGCheck(this, at1.get(), &work->mEye, 0x8F) || /* lineBGCheck */
                        talk_lineBGCheck(this, at2.get(), &work->mEye, 0x8F) ||
                        talk_lineBGCheck(this, &work->mCenter, &work->mEye, 0x8F);
                    if (!hit) {
                        gabi::Local<cXyz> q1, q2;
                        cpXyz(q1, at1);
                        cpXyz(q2, &work->mEye);
                        hit = chkCamera(q1, q2, 15.0f, a1, a2);
                        if (!hit) {
                            gabi::Local<cXyz> q3, q4;
                            cpXyz(q3, at2);
                            cpXyz(q4, &work->mEye);
                            if (!chkCamera(q3, q4, 15.0f, a1, a2)) {
                                done = true;
                                break;
                            }
                        }
                    }
                    /* blocked: rotate on */
                    gabi::Local<cSAngle_l> t8, t12, t10, te, l90;
                    talk_angleAdd(&work->mDirection.mU, t8, step);
                    work->mDirection.mU = talkAngleShortValue(tmp, *t8);
                    talk_angleSubtract(&g->mU, t12, &work->mDirection.mU);
                    f32 c2 = talk_angleCos(t12) + 0.1f;
                    talk_angleMultiply(&g->mV, t10, c2);
                    talk_angleMultiply(t10, te, latitudeScale);
                    cSAngle_l* l2 = talk_angleFromDegrees(l90.get(), defaultLatitude);
                    talk_angleAdd(te, t8, l2);
                    s16 v2 = *t8;
                    if (v2 > (s16)*latHi)
                        v2 = *latHi;
                    if (v2 < (s16)*latLo)
                        v2 = *latLo;
                    *va = v2;
                    work->mDirection.mV = talkAngleShortValue(tmp, v2);
                }
                (void)done;
                talkFinish(this, work, a1, a2);
            }
        }
        if (work->mSide == -1) {
            gabi::Local<cSAngle_l> tfa;
            talk_angleSubtract(&g->mU, tfa, &work->mDirection.mU);
            work->mSide = ((s16)*tfa > talkAngleZero) ? 0 : 1; /* mfcr: !CR0[GT] */
        }
    }
    s32 type = work->mShotType;
    switch ((u32)type) {
    case TALK_PAIR: {
        if (a2 != nullptr && fpcM_GetName(a2) == fpcNm_SHIP_e) {
            gabi::Local<cXyz> ofs, r;
            ofs->y = centerOffsetY;
            ofs->z = centerOffsetZ;
            ofs->x = centerOffsetX;
            talk_relationalPos(this, r.get(), a1, a2, ofs.get(), 0.25f);
            work->mCenter.copy(*r);
        }
        if (m100 != 0) {
            mViewCache.mCenter.copy(work->mCenter);
            copyTalkFloatBits(&mViewCache.mDirection.mRadius, &work->mDirection.mRadius);
            copyTalkFloatBits(&mViewCache.mDirection.mV, &work->mDirection.mV);
            talkEye(this);
            copyTalkFloatBits(&mViewCache.mFovy, &work->mFovy);
        } else if (work->mSmoothless != 0) {
            flagsDone(this);
        } else {
            gabi::Local<cXyz> d, s;
            gabi::Local<cSAngle_l> tb0, tae, tac;
            f32 r = (f32)(s32)((s32)work->mTransitionFrames - (s32)work->mShotFrames);
            f32 k = r / work->mWeightSum;
            work->mFrameWeight = r;
            cXyz_mi(&work->mCenter, d, &mViewCache.mCenter);
            cXyz_ml(d, s, k);
            PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
            f32 rr = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(work->mDirection.mRadius - rr, k, rr);
            talk_angleSubtract(&work->mDirection.mV, tb0, &mViewCache.mDirection.mV);
            talk_angleMultiply(tb0, tae, k);
            talk_angleAdd(&mViewCache.mDirection.mV, tac, tae);
            mViewCache.mDirection.mV = talkAngleShortValue(tmp, *tac);
            talk_angleSubtract(&work->mDirection.mU, tac, &mViewCache.mDirection.mU);
            talk_angleMultiply(tac, tae, k);
            talk_angleAdd(&mViewCache.mDirection.mU, tb0, tae);
            mViewCache.mDirection.mU = talkAngleShortValue(tmp, *tb0);
            talkEye(this);
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(work->mFovy - fv, k, fv);
            work->mWeightSum = work->mWeightSum - work->mFrameWeight;
            if ((s32)work->mShotFrames >= (s32)work->mTransitionFrames - 1)
                flagsDone(this);
            ret = false;
        }
        break;
    }
    case TALK_SHOULDER_A:
    case TALK_SHOULDER_B:
        if (type == TALK_SHOULDER_B)
            talkShoulder(this, work, a1, a2, 135.0f, 25.0f, 60.0f, false);
        else
            talkShoulder(this, work, a2, a1, 135.0f, 25.0f, 60.0f, false);
        break;
    case TALK_SHOULDER_NEAR_A:
    case TALK_SHOULDER_NEAR_B:
        if (type == TALK_SHOULDER_NEAR_B)
            talkShoulder(this, work, a1, a2, 93.0f, 5.0f, 50.0f, false);
        else
            talkShoulder(this, work, a2, a1, 93.0f, 5.0f, 50.0f, false);
        break;
    case TALK_SHOULDER_LOW_A:
    case TALK_SHOULDER_LOW_B:
        if (type == TALK_SHOULDER_LOW_B)
            talkShoulder(this, work, a1, a2, 135.0f, 15.0f, 45.0f, false);
        else
            talkShoulder(this, work, a2, a1, 135.0f, 15.0f, 45.0f, false);
        break;
    case TALK_SHOULDER_ATTN:
        talkShoulder(this, work, a1, a2, 200.0f, 68.0f, 55.0f, true);
        break;
    case TALK_EYE_A:
    case TALK_EYE_B: {
        fopAc_ac_c* listener = (type == TALK_EYE_A) ? a1 : a2;
        fopAc_ac_c* speaker = (type == TALK_EYE_A) ? a2 : a1;
        listener->actor_status = listener->actor_status | fopAcStts_NODRAW_e;
        gabi::Local<cXyz> pos;
        positionOf(pos, speaker);
        mViewCache.mCenter.copy(*pos);
        if (work->mShotFrames == 0) {
            gabi::Local<cXyz> e1, e2, d;
            eyePos(e1, listener);
            eyePos(e2, speaker);
            cXyz_mi(e1, d, e2);
            cSGlobe_Val(&mViewCache.mDirection, d);
            f32 R = mViewCache.mDirection.mRadius;
            flagsDone(this);
            mViewCache.mDirection.mRadius = R - 5.0f;
        }
        gabi::Local<cXyz> ep;
        eyePos(ep, speaker);
        copyTalkFloatBits(&mViewCache.mCenter.y, &ep->y);
        talkEye(this);
        mViewCache.mFovy = 50.0f;
        break;
    }
    case TALK_SIDE_A:
    case TALK_SIDE_B: {
        bool side = (type == TALK_SIDE_A) ? (work->mSide != 0) : (work->mSide == 0);
        fopAc_ac_c* A = (type == TALK_SIDE_A) ? a2 : a1;
        fopAc_ac_c* B = (type == TALK_SIDE_A) ? a1 : a2;
        talkSide(this, work, A, B, side, -10.0f, false, 75.0f, -80.0f, 135.0f, 0.0f, 50.0f);
        break;
    }
    case TALK_SIDE_HIGH_A:
    case TALK_SIDE_HIGH_B: {
        bool side = (type == TALK_SIDE_HIGH_A) ? (work->mSide != 0) : (work->mSide == 0);
        fopAc_ac_c* A = (type == TALK_SIDE_HIGH_A) ? a2 : a1;
        fopAc_ac_c* B = (type == TALK_SIDE_HIGH_A) ? a1 : a2;
        talkSide(this, work, A, B, side, -20.0f, false, 45.0f, -45.0f, 120.0f, 25.0f, 45.0f);
        break;
    }
    case TALK_SIDE_ATTN_A:
    case TALK_SIDE_ATTN_B: {
        bool side = (type == TALK_SIDE_ATTN_A) ? (work->mSide != 0) : (work->mSide == 0);
        fopAc_ac_c* A = (type == TALK_SIDE_ATTN_A) ? a2 : a1;
        fopAc_ac_c* B = (type == TALK_SIDE_ATTN_A) ? a1 : a2;
        talkSide(this, work, A, B, side, -20.0f, true, 35.0f, -30.0f, 145.0f, -35.0f, 50.0f);
        break;
    }
    case TALK_CLOSEUP_A:
    case TALK_CLOSEUP_B:
    case TALK_CLOSEUP_WIDE_A:
    case TALK_CLOSEUP_WIDE_B: {
        fopAc_ac_c* X = (type == TALK_CLOSEUP_B || type == TALK_CLOSEUP_WIDE_B) ? a2 : a1;
        fopAc_ac_c* Y = (type == TALK_CLOSEUP_B || type == TALK_CLOSEUP_WIDE_B) ? a1 : a2;
        bool side = (type == TALK_CLOSEUP_B || type == TALK_CLOSEUP_WIDE_B) ? (work->mSide != 0)
                                                                            : (work->mSide == 0);
        if (work->mShotFrames != 0)
            break;
        gabi::Local<cXyz> e1, pX, eY, o, oB, a, b, d, x, s;
        gabi::Local<cSGlobe_l> g164, g30;
        gabi::Local<cSAngle_l> t, tmp2;
        eyePos(e1, X);
        positionOf(pX, X);
        f32 h = (e1->y - pX->y) * 1.2f;
        eyePos(eY, Y);
        eyePos(e1, X);
        o->z = work->mActorDirection.mRadius * 0.25f;
        o->y = 0.0f;
        f32 dy = (eY->y - e1->y) * 0.25f;
        oB->y = dy;
        f32 cy = gabi::fmadds(h, 0.5f, dy);
        oB->z = -75.0f;
        if (!side) {
            oB->x = 75.0f;
            o->x = 25.0f;
        } else {
            o->x = -25.0f;
            oB->x = -75.0f;
        }
        attentionPos(a, Y);
        attentionPos(b, X);
        cXyz_mi(a, d, b);
        talk_globeFromVector(g164.get(), d.get());
        talk_globeFromVector(g30.get(), o.get());
        talk_angleAdd(&g30->mU, t, &g164->mU);
        g30->mU = talkAngleShortValue(tmp2, *t);
        attentionPos(d, X);
        talk_globeXyz(g30.get(), x.get());
        cXyz_pl(d, s, x);
        mViewCache.mCenter.copy(*s);
        positionOf(s, X);
        mViewCache.mCenter.y = s->y + cy;
        cSGlobe_Val(g30, oB);
        s16 inv = cSAngle_Inv(&g164->mU);
        talk_angleAddShort(&g30->mU, t.get(), inv); /* + s16 */
        g30->mU = talkAngleShortValue(tmp2, *t);
        attentionPos(d, Y);
        talk_globeXyz(g30.get(), x.get());
        cXyz_pl(d, s, x);
        mViewCache.mEye.copy(*s);
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        flagsDone(this);
        if (work->mShotType == TALK_CLOSEUP_A || work->mShotType == TALK_CLOSEUP_B)
            mViewCache.mFovy = 55.0f;
        else
            mViewCache.mFovy = 65.0f;
        break;
    }
    case TALK_WIDE: {
        if (work->mShotFrames == 0)
            flagsDone(this);
        mViewCache.mCenter.copy(work->mCenter);
        gabi::Local<cSAngle_l> l, step, t18;
        cSAngle_l* lp = talk_angleFromDegrees(l.get(), 35.0f);
        talk_globeVal(&mViewCache.mDirection, 350.0f, lp, &work->mDirection.mU);
        s16 v = mViewCache.mDirection.mV;
        if (v > (s16)*latHi) {
            v = talkAngleShortValue(tmp, *latHi);
            mViewCache.mDirection.mV = v;
        }
        if (v < (s16)*latLo)
            mViewCache.mDirection.mV = talkAngleShortValue(tmp, *latLo);
        gabi::Local<cXyz> at1, at2;
        attentionPos(at1, a1);
        attentionPos(at2, a2);
        cSAngle_ct(step);
        gabi::Local<cSAngle_l> ts;
        if (work->mSide != 0)
            *step = (s16)*talk_angleFromDegrees(ts.get(), 20.0f);
        else
            *step = (s16)*talk_angleFromDegrees(ts.get(), -20.0f);
        for (s32 i = 18; i != 0; i--) {
            talkEye(this);
            bool hit = talk_lineBGCheck(this, at1.get(), &mViewCache.mEye, 0x8F) ||
                       talk_lineBGCheck(this, at2.get(), &mViewCache.mEye, 0x8F);
            if (!hit) {
                gabi::Local<cXyz> q1, q2;
                cpXyz(q1, at1);
                cpXyz(q2, &mViewCache.mEye);
                hit = chkCamera(q1, q2, 15.0f, a1, a2);
                if (!hit) {
                    gabi::Local<cXyz> q3, q4;
                    cpXyz(q3, at2);
                    cpXyz(q4, &mViewCache.mEye);
                    if (!chkCamera(q3, q4, 15.0f, a1, a2))
                        break;
                }
            }
            talk_angleAdd(&mViewCache.mDirection.mU, t18, step);
            mViewCache.mDirection.mU = talkAngleShortValue(tmp, *t18);
        }
        mViewCache.mFovy = 60.0f;
        break;
    }
    case TALK_MIDPOINT: {
        if (work->mShotFrames == 0)
            flagsDone(this);
        gabi::Local<cXyz> b, a, d, h, c;
        gabi::Local<cSGlobe_l> g;
        gabi::Local<cSAngle_l> side, u;
        attentionPos(b, a2);
        attentionPos(a, a1);
        cXyz_mi(b, d, a);
        talk_globeFromVector(g.get(), d.get());
        cSAngle_ct(side);
        if (work->mSide != 0)
            *side = talkAngle270; /* _270 */
        else
            *side = talkAngle90; /* _90 */
        attentionPos(b, a1);
        cXyz_ml(d, h, 0.5f);
        cXyz_pl(b, c, h);
        copyTalkFloatBits(&mViewCache.mCenter.x, &c->x);
        copyTalkFloatBits(&mViewCache.mCenter.z, &c->z);
        mViewCache.mCenter.y = c->y - 30.0f;
        talk_angleAdd(&g->mU, u, side);
        talk_globeVal(&mViewCache.mDirection, 200.0f, reinterpret_cast<cSAngle_l*>(&talkAngleZero),
                      u.get());
        talkEye(this);
        mViewCache.mFovy = 60.0f;
        f32 r = radiusActorInSight(a1, a2);
        if (r > 0.0f) {
            mViewCache.mDirection.mRadius = mViewCache.mDirection.mRadius + r;
            talkEye(this);
        }
        break;
    }
    default:
        break;
    }
    work->mPreviousShotType = work->mShotType;
    work->mShotFrames = work->mShotFrames + 1;
    return ret;
}

/* WWHD glue
ext 0200658C cSAngle_l* talk_angleFromShort(cSAngle_l*,s16)
ext 020068B0 void talk_angleSubtract(const void*,cSAngle_l*,const void*)
ext 0200693C void talk_angleMultiply(const void*,cSAngle_l*,f32)
ext 02006894 void talk_angleAdd(const void*,cSAngle_l*,const void*)
ext 02006838 f32 talk_angleCos(const void*)
ext 025187D8 bool talk_ChkCamera(void*,cXyz*,cXyz*,f32,fopAc_ac_c*,fopAc_ac_c*)
ext 020073AC void talk_globeXyz(cSGlobe_l*,cXyz*)
ext 02007324 void talk_globeFromVector(cSGlobe_l*,cXyz*)
ext 020066C0 cSAngle_l* talk_angleFromDegrees(cSAngle_l*,f32)
ext 020071E0 void talk_globeVal(cSGlobe_l*,f32,cSAngle_l*,cSAngle_l*)
ext 02530634 void talk_getEvIntData(dCamera_c*,be<s32>*,const char*,s32)
ext 0253072C void talk_getEvFloatData(dCamera_c*,be<f32>*,const char*,f32)
ext 02530CB4 fopAc_ac_c* talk_getEvActor(dCamera_c*,const char*,const char*)
ext 02502364 u32 talk_getTableEntryHD()
ext 025023C4 s32 talk_getTableIndexHD()
ext 02508804 void talk_relationalPos(dCamera_c*,cXyz*,fopAc_ac_c*,fopAc_ac_c*,cXyz*,f32)
ext 02006694 void talk_angleValDegrees(cSAngle_l*,f32)
ext 02006880 void talk_angleNegate(cSAngle_l*,cSAngle_l*)
ext 02006720 f32 talk_angleDegree(cSAngle_l*)
ext 024FCBE8 bool talk_lineBGCheck(dCamera_c*,cXyz*,cXyz*,u32)
ext 020068F4 void talk_angleAddShort(cSAngle_l*,cSAngle_l*,s16)
data talkAngleZero 101FF354
data talkAngle90 101FF358
data talkAngle270 101FF35C
data talkEventNames 1004ACDC
*/
