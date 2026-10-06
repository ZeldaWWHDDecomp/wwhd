/**
 * d_camera_talk.cpp (WWHD)
 * Follow camera dCamera_c: talktoCamera (conversations).
 *
 * A "Nonmatching" stub in the GameCube decompilation;
 * written from the WWHD code and verified against cking.rpx.
 */
#include "d/d_camera.h"

/* work area of talktoCamera (mWork) */
struct Talk_l {
    /* 0x00 */ be<u32> m00;            /* 'TALK' */
    /* 0x04 */ cXyz m04;               /* centre */
    /* 0x10 */ cXyz m10;               /* eye */
    /* 0x1C */ cXyz m1C;               /* shot centre / height offset (y) */
    /* 0x28 */ cSGlobe_l m28;          /* direction */
    /* 0x30 */ cSGlobe_l m30;          /* speaker -> listener */
    /* 0x38 */ be<s32> m38;            /* side (-1: not chosen) */
    /* 0x3C */ be<s32> m3C;            /* shot type (HD table index) */
    /* 0x40 */ be<s32> m40;            /* previous shot type */
    /* 0x44 */ be<s32> m44;            /* frames in this shot */
    /* 0x48 */ be<s32> m48;            /* transition frames */
    /* 0x4C */ be<f32> m4C;            /* remaining weight sum */
    /* 0x50 */ be<f32> m50;
    /* 0x54 */ u8 _54[4];
    /* 0x58 */ be<f32> m58;            /* Radius */
    /* 0x5C */ be<f32> m5C;            /* Fovy */
    /* 0x60 */ be<f32> m60;            /* Longitude (999.9: free) */
    /* 0x64 */ be<f32> m64;            /* Latitude */
    /* 0x68 */ gptr<fopAc_ac_c> m68;   /* Listener */
    /* 0x6C */ gptr<fopAc_ac_c> m6C;   /* Speaker */
    /* 0x70 */ gptr<fopAc_ac_c> m70;   /* last speaker */
    /* 0x74 */ be<s16> m74;            /* Smoothless */
    /* 0x76 */ be<s16> m76;            /* Mode */
};
WWHD_SIZE(Talk_l, 0x78);

static inline void cpfT(void* dst, const void* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }
static inline s16 angCtT(cSAngle_l* tmp, s16 v) { return *gabi::call<cSAngle_l*>(0x0200658C, tmp, v); }
static inline void tSub(const void* a, cSAngle_l* out, const void* b) { gabi::call(0x020068B0, a, out, b); }
static inline void tMul(const void* a, cSAngle_l* out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline void tAdd(const void* a, cSAngle_l* out, const void* b) { gabi::call(0x02006894, a, out, b); }
static inline f32 tCos(const void* a) { return gabi::call<f32>(0x02006838, a); }
static inline s16 tName(fopAc_ac_c* a) { return gabi::load<s16>(gabi::ea(a) + 8); }
static inline bool chkCamera(cXyz* a, cXyz* b, f32 r, fopAc_ac_c* a1, fopAc_ac_c* a2) {
    return gabi::call<bool>(0x025187D8, dComIfGp_ea() + 0x26A4, a, b, r, a1, a2); /* dCcS::ChkCamera */
}
static inline void cpXyz(cXyz* d, const cXyz* s) { /* lfs/stfs copy */
    cpfT(&d->x, &s->x);
    cpfT(&d->y, &s->y);
    cpfT(&d->z, &s->z);
}
/* mViewCache.mEye = mCenter + mDirection.Xyz() */
static void talkEye(dCamera_c* c) {
    gabi::Local<cXyz> x, e;
    gabi::call(0x020073AC, &c->mViewCache.mDirection, x.get());
    cXyz_pl(&c->mViewCache.mCenter, e, x);
    c->mViewCache.mEye.copy(*e);
}
static inline void flagsDone(dCamera_c* c) {
    c->m102 = 1;
    c->m101 = 1;
    c->m100 = 1;
}
/* m10 = m04 + m28.Xyz(), then pushed out by radiusActorInSight */
static void talkFinish(dCamera_c* c, Talk_l* p, fopAc_ac_c* a1, fopAc_ac_c* a2) {
    gabi::Local<cXyz> x, e;
    s32 n = p->m48;
    p->m4C = (f32)((n * (n + 1)) >> 1);
    gabi::call(0x020073AC, &p->m28, x.get());
    cXyz_pl(&p->m04, e, x);
    p->m10.copy(*e);
    f32 r = c->radiusActorInSight6(a1, a2, &p->m04, &p->m10, p->m5C, 0);
    if (r > 0.0f) {
        gabi::Local<cXyz> x2, e2;
        p->m28.mRadius = p->m28.mRadius + r;
        gabi::call(0x020073AC, &p->m28, x2.get());
        cXyz_pl(&p->m04, e2, x2);
        p->m10.copy(*e2);
    }
}

/* the over-the-shoulder shots (types 0x10/0x11, 0x14/0x15, 0x16/0x17, 0x1D): the centre at the
 * speaker, the direction from the listener; first frame: the radius and the height offset */
static void talkShoulder(dCamera_c* c, Talk_l* p, fopAc_ac_c* listener, fopAc_ac_c* speaker, f32 radius, f32 dy, f32 fovy,
                         bool attnFirst) {
    gabi::store<u32>(gabi::ea(listener) + 0x2E0, gabi::load<u32>(gabi::ea(listener) + 0x2E0) | 0x1000000);
    gabi::Local<cXyz> pos;
    c->positionOf(pos, speaker);
    c->mViewCache.mCenter.copy(*pos);
    if (p->m44 == 0) {
        gabi::Local<cXyz> a, b, d, ep, sp;
        c->positionOf(a, listener);
        c->positionOf(b, speaker);
        cXyz_mi(a, d, b);
        cSGlobe_Val(&c->mViewCache.mDirection, d);
        c->mViewCache.mDirection.mRadius = radius;
        if (attnFirst) c->attentionPos(d, speaker);
        else c->eyePos(d, speaker);
        c->positionOf(sp, speaker);
        p->m1C.y = (d->y - dy) - sp->y;
        flagsDone(c);
    }
    gabi::Local<cXyz> pos2;
    c->positionOf(pos2, speaker);
    c->mViewCache.mCenter.y = pos2->y + p->m1C.y;
    talkEye(c);
    c->mViewCache.mFovy = fovy;
}

/* the side shots (types 0x12/0x13, 0x18/0x19, 0x1A/0x1B): the centre from an offset globe around
 * B rotated by the A -> B direction (first frame), then a fixed globe around it */
static void talkSide(dCamera_c* c, Talk_l* p, fopAc_ac_c* A, fopAc_ac_c* B, bool side, f32 ofsY, bool attnA, f32 u0, f32 u1,
                     f32 radius, f32 lat, f32 fovy) {
    if (p->m44 == 0) {
        gabi::Local<cXyz> o, ea, ab, d, eb, x, s;
        gabi::Local<cSGlobe_l> gb, g30;
        gabi::Local<cSAngle_l> t, tmp;
        o->x = 0.0f;
        o->z = 20.0f;
        o->y = ofsY;
        flagsDone(c);
        if (attnA) c->attentionPos(ea, A);
        else c->eyePos(ea, A);
        c->attentionPos(ab, B);
        cXyz_mi(ea, d, ab);
        gabi::call(0x02007324, gb.get(), d.get());
        gabi::call(0x02007324, g30.get(), o.get());
        tAdd(&g30->mU, t, &gb->mU);
        g30->mU = angCtT(tmp, *t);
        c->eyePos(eb, B);
        gabi::call(0x020073AC, g30.get(), x.get());
        cXyz_pl(eb, s, x);
        cpXyz(&p->m1C, s);
        cpfT(&c->mViewCache.mCenter.y, &s->y);
        cpfT(&c->mViewCache.mCenter.x, &s->x);
        cpfT(&c->mViewCache.mCenter.z, &p->m1C.z);
    } else {
        cpfT(&c->mViewCache.mCenter.y, &p->m1C.y);
        cpfT(&c->mViewCache.mCenter.x, &p->m1C.x);
        cpfT(&c->mViewCache.mCenter.z, &p->m1C.z);
    }
    gabi::Local<cSAngle_l> a, dir, u, l;
    gabi::call(0x020066C0, a.get(), side ? u1 : u0);
    c->directionOf(dir, B);
    tAdd(a, u, dir);
    if (lat == 0.0f) {
        gabi::call(0x020071E0, &c->mViewCache.mDirection, radius, gabi::at<cSAngle_l>(0x101FF354), u.get());
    } else {
        cSAngle_l* lp = gabi::call<cSAngle_l*>(0x020066C0, l.get(), lat);
        gabi::call(0x020071E0, &c->mViewCache.mDirection, radius, lp, u.get());
    }
    talkEye(c);
    c->mViewCache.mFovy = fovy;
}

/* 02508A60 (GameCube: not decompiled). Conversation camera: picks the listener and speaker (event
 * data "Listener"/"Speaker", or the player and the lock-on target), sets up a two-shot that keeps
 * both in view (rotating away from walls and colliders), and plays the HD shot types of the talk
 * table (over-the-shoulder, side and close-up shots). */
bool dCamera_c::talktoCamera(s32 style) {
    WWHD_FUNC(0x02508A60, bool, this, style);
    Talk_l* p = (Talk_l*)mWork;
    f32 v5 = camParamVal(style, 5);
    f32 v0 = camParamVal(style, 0);
    f32 v10 = camParamVal(style, 10);
    f32 v18 = camParamVal(style, 18);
    f32 v25 = camParamVal(style, 25);
    f32 v1 = camParamVal(style, 1);
    f32 v24 = camParamVal(style, 24);
    f32 v15 = camParamVal(style, 15);
    f32 v19 = camParamVal(style, 19);
    f32 v3 = camParamVal(style, 3);
    bool ret = true;
    if (m11C == 0) {
        p->m44 = 0;
        p->m38 = -1;
        p->m48 = 20;
        p->m3C = 0;
        p->m40 = -1;
        p->m60 = 999.9f;
        p->m64 = v15;
        p->m00 = 0x54414C4B; /* 'TALK' */
        if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) == 0) {
            p->m74 = 0;
            p->m76 = 0;
            p->m58 = v10;
            p->m5C = v25;
            p->m68 = mpPlayerActor.get();
            fopAc_ac_c* t = mpLockonTarget.get();
            p->m6C = t;
            p->m70 = t;
        } else {
            gabi::Local<be<s32>> iv;
            gabi::call(0x02530634, this, iv.get(), 0x1004AD08, 0); /* getEvIntData("Smoothless") */
            p->m74 = (s16)(s32)*iv;
            gabi::call(0x02530634, this, iv.get(), 0x1004ACE4, 0); /* "Mode" */
            p->m76 = (s16)(s32)*iv;
            gabi::call(0x0253072C, this, &p->m58, 0x1004ACEC, v10);   /* getEvFloatData("Radius") */
            gabi::call(0x0253072C, this, &p->m60, 0x1004ACFC, 999.9f); /* "Longitude" */
            gabi::call(0x0253072C, this, &p->m5C, 0x1004ACF4, v25);   /* "Fovy" */
            gabi::call(0x0253072C, this, &p->m64, 0x1004AD14, v15);   /* "Latitude" */
            p->m68 = gabi::call<fopAc_ac_c*>(0x02530CB4, this, 0x1004AD20, 0x1004AD2C); /* getEvActor("Listener", "@STARTER") */
            fopAc_ac_c* s = gabi::call<fopAc_ac_c*>(0x02530CB4, this, 0x1004ACDC, 0x1004AD38); /* ("Speaker", "@TALKPARTNER") */
            p->m6C = s;
            p->m70 = s;
        }
    }
    fopAc_ac_c* a1;
    fopAc_ac_c* a2 = gabi::at<fopAc_ac_c>(gabi::call<u32>(0x02502364)); /* HD: talk table entry */
    bool same;
    if (a2 != nullptr) {
        a1 = p->m68.get();
        same = a1 == a2;
    } else if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) != 0) {
        u32 c11C = m11C;
        a1 = p->m68.get();
        if (c11C != 0) a2 = gabi::call<fopAc_ac_c*>(0x02530CB4, this, 0x1004ACDC, 0x1004AD38);
        else a2 = p->m6C.get();
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
    if (p->m70.get() != a2) {
        m11C = 0;
        m100 = 0;
        p->m44 = 0;
        p->m70 = a2;
    }
    {
        s32 t = gabi::call<s32>(0x025023C4); /* HD: talk shot type */
        s32 prev = p->m40;
        p->m3C = t;
        if (t != prev) p->m44 = 0;
    }
    gabi::Local<cSAngle_l> latHi, latLo, lonMax, lonMin, tmp;
    gabi::call(0x020066C0, latHi.get(), v19);
    gabi::call(0x020066C0, latLo.get(), v18);
    gabi::call(0x020066C0, lonMax.get(), 180.0f - v24);
    gabi::call(0x020066C0, lonMin.get(), v24);
    u32 c11C = m11C;
    if (c11C == 0 || (a2 != nullptr && tName(a2) == 0xA5 && c11C <= 1)) {
        gabi::Local<cXyz> ofs, at1, at2, ep, r;
        ofs->x = v1;
        ofs->y = v5;
        ofs->z = v0;
        attentionPos(at1, a1);
        eyePos(ep, a1);
        cpfT(&at1->y, &ep->y);
        attentionPos(at2, a2);
        eyePos(ep, a2);
        cpfT(&at2->y, &ep->y);
        gabi::call(0x02508804, this, r.get(), a1, a2, ofs.get(), 0.25f); /* relationalPos(a1, a2, ofs, 0.25) */
        p->m04.copy(*r);
        gabi::Local<cSAngle_l> lon;
        cSAngle_ct(lon);
        if (p->m60 != 999.9f) {
            gabi::call(0x02006694, lon.get(), (f32)p->m60);
        } else {
            s16 ty = cLib_targetAngleY(&p->m04, &mViewCache.mEye);
            cSAngle_Val(lon, ty);
        }
        gabi::Local<cSGlobe_l> g;
        {
            gabi::Local<cXyz> d;
            cpfT(&p->m28.mRadius, &mViewCache.mDirection.mRadius);
            cpfT(&p->m28.mV, &mViewCache.mDirection.mV);
            cXyz_mi(at1, d, at2);
            gabi::call(0x02007324, g.get(), d.get());
            cpfT(&p->m30.mRadius, &g->mRadius);
            cpfT(&p->m28.mRadius, &p->m58);
            cpfT(&p->m30.mV, &g->mV);
            /* the copy keeps the radius >= 88 on the stack (g is used below as p->m30's twin) */
            if (g->mRadius < 88.0f) g->mRadius = 88.0f;
            if (a2 != nullptr && tName(a2) == 0xA5) {
                /* ship: look past the ship's bow side */
                gabi::Local<cSAngle_l> tA, dir, rel, side, t2;
                s16 ty = cLib_targetAngleY(positionPntOf(a2), at2);
                cSAngle_ct(tA, ty);
                directionOf(dir, a2);
                tSub(dir, rel, tA);
                cSAngle_ct(side);
                cSAngle_l* sp;
                if ((s16)*rel < gabi::load<s16>(0x101FF354)) sp = gabi::call<cSAngle_l*>(0x020066C0, t2.get(), -30.0f);
                else sp = gabi::call<cSAngle_l*>(0x020066C0, t2.get(), 30.0f);
                *side = (s16)*sp;
                tAdd(&g->mU, dir, side);
                p->m28.mU = angCtT(tmp, *dir);
                cSAngle_l* lp = gabi::call<cSAngle_l*>(0x020066C0, t2.get(), v15);
                p->m28.mV = angCtT(tmp, *lp);
                talkFinish(this, p, a1, a2);
            } else if (a2 != nullptr && tName(a2) == 0xB6) {
                gabi::Local<cSAngle_l> dir, tf0, tee, t2;
                directionOf(dir, a2);
                g->mU = angCtT(tmp, *dir);
                tSub(lon, tf0, &g->mU);
                tMul(tf0, tee, 0.25f);
                tAdd(&g->mU, dir, tee);
                p->m28.mU = angCtT(tmp, *dir);
                cSAngle_l* lp = gabi::call<cSAngle_l*>(0x020066C0, t2.get(), (f32)p->m64);
                p->m28.mV = angCtT(tmp, *lp);
                talkFinish(this, p, a1, a2);
            } else {
                gabi::Local<cSAngle_l> rel, neg, t2c, tf4, tf2, lat15, step, t14;
                tSub(lon, rel, &g->mU);
                s16 r = *rel;
                if (r > (s16)*lonMax) {
                    r = *lonMax;
                    *rel = r;
                }
                s16 zero = gabi::load<s16>(0x101FF354);
                if (r > zero && r < (s16)*lonMin) *rel = (s16)*lonMin;
                gabi::call(0x02006880, lonMax.get(), t2c.get()); /* -lonMax */
                r = *rel;
                if (r < (s16)*t2c) {
                    gabi::call(0x02006880, lonMax.get(), neg.get());
                    r = *neg;
                    *rel = r;
                }
                if (r < zero) {
                    gabi::call(0x02006880, lonMin.get(), t2c.get()); /* -lonMin */
                    if ((s16)*rel > (s16)*t2c) {
                        gabi::call(0x02006880, lonMin.get(), neg.get());
                        *rel = (s16)*neg;
                    }
                }
                tAdd(&g->mU, t2c, rel);
                p->m28.mU = angCtT(tmp, *t2c);
                tSub(&g->mU, tf4, lon);
                f32 c = tCos(tf4) + 0.1f;
                tMul(&g->mV, tf2, c);
                tMul(tf2, t2c, v3);
                cSAngle_l* lp = gabi::call<cSAngle_l*>(0x020066C0, lat15.get(), v15);
                gabi::Local<cSAngle_l> va;
                tAdd(t2c, va, lp);
                s16 v = *va;
                if (v > (s16)*latHi) {
                    v = *latHi;
                    *va = v;
                }
                if (v < (s16)*latLo) {
                    v = *latLo;
                    *va = v;
                }
                p->m28.mV = angCtT(tmp, v);
                cSAngle_ct(step);
                tSub(&g->mU, tf4, &p->m28.mU);
                gabi::Local<cSAngle_l> ts;
                if ((s16)*tf4 > zero) *step = (s16)*gabi::call<cSAngle_l*>(0x020066C0, ts.get(), 10.0f);
                else *step = (s16)*gabi::call<cSAngle_l*>(0x020066C0, ts.get(), -10.0f);
                bool done = false;
                for (s32 i = 36; i != 0; i--) {
                    tSub(&p->m28.mU, t14, &g->mU);
                    f32 dg = gabi::call<f32>(0x02006720, t14.get()); /* Degree */
                    if (__builtin_fabsf(dg) < 10.0f) {
                        gabi::Local<cSAngle_l> t16;
                        tAdd(&p->m28.mU, t16, step);
                        p->m28.mU = angCtT(tmp, *t16);
                        continue;
                    }
                    {
                        gabi::Local<cXyz> x, e, pp;
                        gabi::call(0x020073AC, &p->m28, x.get());
                        cXyz_pl(&p->m04, e, x);
                        p->m10.copy(*e);
                        positionOf(pp, mpPlayerActor.get());
                        if (p->m10.y < pp->y + m368) {
                            gabi::Local<cXyz> pp2, dd;
                            positionOf(pp2, mpPlayerActor.get());
                            p->m10.y = (pp2->y + m368) + 10.0f;
                            cXyz_mi(&p->m10, dd, &p->m04);
                            cSGlobe_Val(&p->m28, dd);
                        }
                    }
                    bool hit = gabi::call<bool>(0x024FCBE8, this, at1.get(), &p->m10, 0x8F) || /* lineBGCheck */
                               gabi::call<bool>(0x024FCBE8, this, at2.get(), &p->m10, 0x8F) ||
                               gabi::call<bool>(0x024FCBE8, this, &p->m04, &p->m10, 0x8F);
                    if (!hit) {
                        gabi::Local<cXyz> q1, q2;
                        cpXyz(q1, at1);
                        cpXyz(q2, &p->m10);
                        hit = chkCamera(q1, q2, 15.0f, a1, a2);
                        if (!hit) {
                            gabi::Local<cXyz> q3, q4;
                            cpXyz(q3, at2);
                            cpXyz(q4, &p->m10);
                            if (!chkCamera(q3, q4, 15.0f, a1, a2)) {
                                done = true;
                                break;
                            }
                        }
                    }
                    /* blocked: rotate on */
                    gabi::Local<cSAngle_l> t8, t12, t10, te, l90;
                    tAdd(&p->m28.mU, t8, step);
                    p->m28.mU = angCtT(tmp, *t8);
                    tSub(&g->mU, t12, &p->m28.mU);
                    f32 c2 = tCos(t12) + 0.1f;
                    tMul(&g->mV, t10, c2);
                    tMul(t10, te, v3);
                    cSAngle_l* l2 = gabi::call<cSAngle_l*>(0x020066C0, l90.get(), v15);
                    tAdd(te, t8, l2);
                    s16 v2 = *t8;
                    if (v2 > (s16)*latHi) v2 = *latHi;
                    if (v2 < (s16)*latLo) v2 = *latLo;
                    *va = v2;
                    p->m28.mV = angCtT(tmp, v2);
                }
                (void)done;
                talkFinish(this, p, a1, a2);
            }
        }
        if (p->m38 == -1) {
            gabi::Local<cSAngle_l> tfa;
            tSub(&g->mU, tfa, &p->m28.mU);
            p->m38 = ((s16)*tfa > gabi::load<s16>(0x101FF354)) ? 0 : 1; /* mfcr: !CR0[GT] */
        }
    }
    s32 type = p->m3C;
    switch ((u32)type) {
    case 0: {
        if (a2 != nullptr && tName(a2) == 0xA5) {
            gabi::Local<cXyz> ofs, r;
            ofs->y = v5;
            ofs->z = v0;
            ofs->x = v1;
            gabi::call(0x02508804, this, r.get(), a1, a2, ofs.get(), 0.25f);
            p->m04.copy(*r);
        }
        if (m100 != 0) {
            mViewCache.mCenter.copy(p->m04);
            cpfT(&mViewCache.mDirection.mRadius, &p->m28.mRadius);
            cpfT(&mViewCache.mDirection.mV, &p->m28.mV);
            talkEye(this);
            cpfT(&mViewCache.mFovy, &p->m5C);
        } else if (p->m74 != 0) {
            flagsDone(this);
        } else {
            gabi::Local<cXyz> d, s;
            gabi::Local<cSAngle_l> tb0, tae, tac;
            f32 r = (f32)(s32)((s32)p->m48 - (s32)p->m44);
            f32 k = r / p->m4C;
            p->m50 = r;
            cXyz_mi(&p->m04, d, &mViewCache.mCenter);
            cXyz_ml(d, s, k);
            PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
            f32 rr = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(p->m28.mRadius - rr, k, rr);
            tSub(&p->m28.mV, tb0, &mViewCache.mDirection.mV);
            tMul(tb0, tae, k);
            tAdd(&mViewCache.mDirection.mV, tac, tae);
            mViewCache.mDirection.mV = angCtT(tmp, *tac);
            tSub(&p->m28.mU, tac, &mViewCache.mDirection.mU);
            tMul(tac, tae, k);
            tAdd(&mViewCache.mDirection.mU, tb0, tae);
            mViewCache.mDirection.mU = angCtT(tmp, *tb0);
            talkEye(this);
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(p->m5C - fv, k, fv);
            p->m4C = p->m4C - p->m50;
            if ((s32)p->m44 >= (s32)p->m48 - 1) flagsDone(this);
            ret = false;
        }
        break;
    }
    case 20:
    case 21:
        if (type == 0x15) talkShoulder(this, p, a1, a2, 135.0f, 25.0f, 60.0f, false);
        else talkShoulder(this, p, a2, a1, 135.0f, 25.0f, 60.0f, false);
        break;
    case 16:
    case 17:
        if (type == 0x11) talkShoulder(this, p, a1, a2, 93.0f, 5.0f, 50.0f, false);
        else talkShoulder(this, p, a2, a1, 93.0f, 5.0f, 50.0f, false);
        break;
    case 22:
    case 23:
        if (type == 0x17) talkShoulder(this, p, a1, a2, 135.0f, 15.0f, 45.0f, false);
        else talkShoulder(this, p, a2, a1, 135.0f, 15.0f, 45.0f, false);
        break;
    case 29:
        talkShoulder(this, p, a1, a2, 200.0f, 68.0f, 55.0f, true);
        break;
    case 14:
    case 15: {
        fopAc_ac_c* listener = (type == 0xE) ? a1 : a2;
        fopAc_ac_c* speaker = (type == 0xE) ? a2 : a1;
        gabi::store<u32>(gabi::ea(listener) + 0x2E0, gabi::load<u32>(gabi::ea(listener) + 0x2E0) | 0x1000000);
        gabi::Local<cXyz> pos;
        positionOf(pos, speaker);
        mViewCache.mCenter.copy(*pos);
        if (p->m44 == 0) {
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
        cpfT(&mViewCache.mCenter.y, &ep->y);
        talkEye(this);
        mViewCache.mFovy = 50.0f;
        break;
    }
    case 18:
    case 19: {
        bool side = (type == 0x12) ? (p->m38 != 0) : (p->m38 == 0);
        fopAc_ac_c* A = (type == 0x12) ? a2 : a1;
        fopAc_ac_c* B = (type == 0x12) ? a1 : a2;
        talkSide(this, p, A, B, side, -10.0f, false, 75.0f, -80.0f, 135.0f, 0.0f, 50.0f);
        break;
    }
    case 24:
    case 25: {
        bool side = (type == 0x18) ? (p->m38 != 0) : (p->m38 == 0);
        fopAc_ac_c* A = (type == 0x18) ? a2 : a1;
        fopAc_ac_c* B = (type == 0x18) ? a1 : a2;
        talkSide(this, p, A, B, side, -20.0f, false, 45.0f, -45.0f, 120.0f, 25.0f, 45.0f);
        break;
    }
    case 26:
    case 27: {
        bool side = (type == 0x1A) ? (p->m38 != 0) : (p->m38 == 0);
        fopAc_ac_c* A = (type == 0x1A) ? a2 : a1;
        fopAc_ac_c* B = (type == 0x1A) ? a1 : a2;
        talkSide(this, p, A, B, side, -20.0f, true, 35.0f, -30.0f, 145.0f, -35.0f, 50.0f);
        break;
    }
    case 11:
    case 12:
    case 30:
    case 31: {
        fopAc_ac_c* X = (type == 0xC || type == 0x1F) ? a2 : a1;
        fopAc_ac_c* Y = (type == 0xC || type == 0x1F) ? a1 : a2;
        bool side = (type == 0xC || type == 0x1F) ? (p->m38 != 0) : (p->m38 == 0);
        if (p->m44 != 0) break;
        gabi::Local<cXyz> e1, pX, eY, o, oB, a, b, d, x, s;
        gabi::Local<cSGlobe_l> g164, g30;
        gabi::Local<cSAngle_l> t, tmp2;
        eyePos(e1, X);
        positionOf(pX, X);
        f32 h = (e1->y - pX->y) * 1.2f;
        eyePos(eY, Y);
        eyePos(e1, X);
        o->z = p->m30.mRadius * 0.25f;
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
        gabi::call(0x02007324, g164.get(), d.get());
        gabi::call(0x02007324, g30.get(), o.get());
        tAdd(&g30->mU, t, &g164->mU);
        g30->mU = angCtT(tmp2, *t);
        attentionPos(d, X);
        gabi::call(0x020073AC, g30.get(), x.get());
        cXyz_pl(d, s, x);
        mViewCache.mCenter.copy(*s);
        positionOf(s, X);
        mViewCache.mCenter.y = s->y + cy;
        cSGlobe_Val(g30, oB);
        s16 inv = cSAngle_Inv(&g164->mU);
        gabi::call(0x020068F4, &g30->mU, t.get(), inv); /* + s16 */
        g30->mU = angCtT(tmp2, *t);
        attentionPos(d, Y);
        gabi::call(0x020073AC, g30.get(), x.get());
        cXyz_pl(d, s, x);
        mViewCache.mEye.copy(*s);
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        flagsDone(this);
        if (p->m3C == 0xB || p->m3C == 0xC) mViewCache.mFovy = 55.0f;
        else mViewCache.mFovy = 65.0f;
        break;
    }
    case 13: {
        if (p->m44 == 0) flagsDone(this);
        mViewCache.mCenter.copy(p->m04);
        gabi::Local<cSAngle_l> l, step, t18;
        cSAngle_l* lp = gabi::call<cSAngle_l*>(0x020066C0, l.get(), 35.0f);
        gabi::call(0x020071E0, &mViewCache.mDirection, 350.0f, lp, &p->m28.mU);
        s16 v = mViewCache.mDirection.mV;
        if (v > (s16)*latHi) {
            v = angCtT(tmp, *latHi);
            mViewCache.mDirection.mV = v;
        }
        if (v < (s16)*latLo) mViewCache.mDirection.mV = angCtT(tmp, *latLo);
        gabi::Local<cXyz> at1, at2;
        attentionPos(at1, a1);
        attentionPos(at2, a2);
        cSAngle_ct(step);
        gabi::Local<cSAngle_l> ts;
        if (p->m38 != 0) *step = (s16)*gabi::call<cSAngle_l*>(0x020066C0, ts.get(), 20.0f);
        else *step = (s16)*gabi::call<cSAngle_l*>(0x020066C0, ts.get(), -20.0f);
        for (s32 i = 18; i != 0; i--) {
            talkEye(this);
            bool hit = gabi::call<bool>(0x024FCBE8, this, at1.get(), &mViewCache.mEye, 0x8F) ||
                       gabi::call<bool>(0x024FCBE8, this, at2.get(), &mViewCache.mEye, 0x8F);
            if (!hit) {
                gabi::Local<cXyz> q1, q2;
                cpXyz(q1, at1);
                cpXyz(q2, &mViewCache.mEye);
                hit = chkCamera(q1, q2, 15.0f, a1, a2);
                if (!hit) {
                    gabi::Local<cXyz> q3, q4;
                    cpXyz(q3, at2);
                    cpXyz(q4, &mViewCache.mEye);
                    if (!chkCamera(q3, q4, 15.0f, a1, a2)) break;
                }
            }
            tAdd(&mViewCache.mDirection.mU, t18, step);
            mViewCache.mDirection.mU = angCtT(tmp, *t18);
        }
        mViewCache.mFovy = 60.0f;
        break;
    }
    case 28: {
        if (p->m44 == 0) flagsDone(this);
        gabi::Local<cXyz> b, a, d, h, c;
        gabi::Local<cSGlobe_l> g;
        gabi::Local<cSAngle_l> side, u;
        attentionPos(b, a2);
        attentionPos(a, a1);
        cXyz_mi(b, d, a);
        gabi::call(0x02007324, g.get(), d.get());
        cSAngle_ct(side);
        if (p->m38 != 0) *side = gabi::load<s16>(0x101FF35C); /* _270 */
        else *side = gabi::load<s16>(0x101FF358);             /* _90 */
        attentionPos(b, a1);
        cXyz_ml(d, h, 0.5f);
        cXyz_pl(b, c, h);
        cpfT(&mViewCache.mCenter.x, &c->x);
        cpfT(&mViewCache.mCenter.z, &c->z);
        mViewCache.mCenter.y = c->y - 30.0f;
        tAdd(&g->mU, u, side);
        gabi::call(0x020071E0, &mViewCache.mDirection, 200.0f, gabi::at<cSAngle_l>(0x101FF354), u.get());
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
    p->m40 = p->m3C;
    p->m44 = p->m44 + 1;
    return ret;
}
VERIFY(0x02508A60, &dCamera_c::talktoCamera);
