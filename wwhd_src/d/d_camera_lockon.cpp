/**
 * d_camera_lockon.cpp (WWHD)
 * Follow camera dCamera_c: lockonCamera (Z-targeting).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp, a "Nonmatching" draft there) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera.h"

/* work area of lockonCamera (mWork, GC Work::Lockon) */
struct Lockon_l {
    /* 0x00 */ be<u32> m378;     /* 'LOCK' */
    /* 0x04 */ u8 _04[4];
    /* 0x08 */ be<s32> m380;     /* charge frames */
    /* 0x0C */ be<f32> m384;
    /* 0x10 */ be<s32> m388;     /* frames the player is hidden */
    /* 0x14 */ be<u8> m38C;
    /* 0x15 */ u8 _15[3];
    /* 0x18 */ cXyz m390;        /* centre base */
    /* 0x24 */ be<u8> m39C;
    /* 0x25 */ be<u8> m39D;      /* shock given */
    /* 0x26 */ u8 _26[2];
    /* 0x28 */ be<s32> m3A0;     /* camera side */
    /* 0x2C */ be<s32> m3A4;     /* side to turn to while hidden */
    /* 0x30 */ cSGlobe_l m3A8;   /* centre offset */
    /* 0x38 */ be<f32> m3B0;
    /* 0x3C */ be<f32> m3B4;
    /* 0x40 */ be<f32> m3B8;     /* centre height cushion */
};
WWHD_SIZE(Lockon_l, 0x44);

static inline u32 lStatus0(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8); }
static inline u32 lStatus1(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CDC); }
static inline void cpfL(void* dst, const void* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }
static inline f32 bezierL(f32 x, f32 r) { return gabi::call<f32>(0x024F6F9C, x, r); }   /* dCamMath::rationalBezierRatio */
static inline f32 customRB(f32 x, f32 r) { return gabi::call<f32>(0x024F7194, x, r); }  /* dCamMath::customRBRatio */
static inline void angSub(const void* a, cSAngle_l* out, const void* b) { gabi::call(0x020068B0, a, out, b); }
static inline void angMul(const void* a, cSAngle_l* out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline void angAdd(const void* a, cSAngle_l* out, const void* b) { gabi::call(0x02006894, a, out, b); }
static inline void angAddEq(void* a, const void* b) { gabi::call(0x020068CC, a, b); }
static inline void angSubEq(void* a, const void* b) { gabi::call(0x020068E0, a, b); }

/* 025052E8. HD: the side flip negates the angle difference itself (GameCube: the negated target
 * longitude); the hidden-player sway uses the target's longitude and its latitude * 0.7 (GameCube:
 * the camera's yaw and the target longitude); the boomerang distance is computed but unused; the
 * default radius is taken directly when dCamParam_c::DefaultRadius accepts it and eased otherwise */
bool dCamera_c::lockonCamera(s32 style) {
    WWHD_FUNC(0x025052E8, bool, this, style);
    Lockon_l* p = (Lockon_l*)mWork;
    gabi::Local<cSAngle_l> chargeLat, a254, a258, a25c, a274, a26c, a270, t, tmp;
    f32 lat0 = gabi::load<f32>(ea() + 0x7E8);
    f32 m0B4 = gabi::load<f32>(ea() + 0x7F8);
    s32 m0B0 = gabi::load<s32>(ea() + 0x7F4);
    gabi::call(0x020066C0, chargeLat.get(), lat0); /* mCamSetup.ChargeLatitude() */
    f32 chargeBRatio = gabi::load<f32>(ea() + 0x7F0);
    s32 chargeTimer = gabi::load<s32>(ea() + 0x7EC);
    f32 v3 = camParamVal(style, 3);
    f32 v4 = camParamVal(style, 4);
    u32 attn = dComIfGp_ea() + 0x5804; /* dComIfGp_getAttention() */
    if (m108 == 0) {
        gabi::Local<cXyz> a, d;
        m100 = 1;
        m101 = 1;
        m102 = 1;
        p->m378 = 0x4C4F434B; /* 'LOCK' */
        p->m380 = 0;
        p->m38C = 0;
        p->m39C = 0;
        p->m384 = 1.0f;
        p->m390.copy(mViewCache.mCenter);
        attentionPos(a, mpPlayerActor.get());
        cXyz_mi(&mViewCache.mCenter, d, a);
        cSGlobe_Val(&p->m3A8, d);
        p->m3B4 = 0.0f;
        p->m3B0 = 0.0f;
        cpfL(&p->m3B8, gabi::at<be<f32>>(ea() + 0x778)); /* mCamSetup.Cushion4Base() */
        p->m39D = 0;
        p->m388 = 0;
        p->m3A0 = 0;
        p->m3A4 = 0;
        if (mpLockonTarget.get() != nullptr) radiusActorInSight(mpPlayerActor.get(), mpLockonTarget.get());
    }
    if (m31D != 0) gabi::call(0x024EFB08, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4, 1, &p->m390, 0, 0); /* MoveBgMatrixCrrPos */
    if (!gabi::call<bool>(0x024EDFCC, attn) && (lStatus0(mPadId) & 0x400000)) p->m39C = 1; /* LockonTruth, BOOMERANG_WAIT */
    else p->m39C = 0;
    if (lStatus0(mPadId) & 0x400) {
        if (p->m39D == 0) {
            u32 vib = dComIfGp_ea() + 0x599C;
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            gabi::call(0x025CB374, vib, 2, 0x10, v.get()); /* dVibration_c::StartShock */
        }
        p->m39D = 1;
    } else {
        p->m39D = 0;
    }
    u32 af = gabi::load<u32>(dComIfGp_ea() + 0x5824); /* attention flags */
    if ((af & 8) || (af & 0x20)) {
        mEventFlags = mEventFlags & ~0x100u;
        m11C = 0;
        m108 = 0;
    }

    gabi::Local<cSGlobe_l> g230;
    gabi::call(0x02007100, g230.get());
    f32 curveW = gabi::load<f32>(ea() + 0x78C); /* mCamSetup.CurveWeight() */
    f32 relDist = gabi::call<f32>(0x024EE49C, attn); /* LockonReleaseDistanse */
    f32 dist = 10000.0f;
    f32 ratio;
    if (lStatus0(mPadId) & 0x61) {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(dir);
        gabi::call(0x02006FE4, g230.get(), (f32)gabi::load<f32>(ea() + 0x780), (s16)0, inv); /* Val(ParallelDist, 0, inv) */
        ratio = 1.0f;
        mpLockonTarget = nullptr;
    } else if (mpLockonTarget.get() != nullptr) {
        gabi::Local<cXyz> f0, fc, pp, d;
        attentionPos(f0, mpLockonTarget.get());
        attentionPos(fc, mpPlayerActor.get());
        fopAc_ac_c* tg = mpLockonTarget.get();
        if (tg != nullptr && gabi::load<s16>(gabi::ea(tg) + 8) == 0xEE) { /* BDK */
            positionOf(pp, tg);
            cpfL(&f0->x, &pp->x);
            positionOf(pp, mpLockonTarget.get());
            cpfL(&f0->z, &pp->z);
        }
        cXyz_mi(f0, d, fc);
        cSGlobe_Val(g230, d);
        ratio = g230->mRadius / relDist;
        if (ratio > 1.0f) ratio = 1.0f;
        dist = gabi::call<f32>(0x024F7AC4, f0.get(), fc.get()); /* dCamMath::xyzHorizontalDistance */
    } else {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        gabi::call(0x020071E0, g230.get(), (f32)gabi::load<f32>(ea() + 0x780), gabi::at<cSAngle_l>(0x101FF354), dir.get());
        ratio = 1.0f;
    }
    cSAngle_ct(a254, &g230->mU);
    s16 lon = gabi::call<s16>(0x024F748C, ea() + 0x8A4, ratio); /* LockonLongitude */
    cSAngle_ct(a258, lon);
    u32 c11C = m11C;
    u32 fl = mEventFlags;
    if (c11C < (u32)m0B0) {
        if (!(fl & 0x100)) gabi::call(0x020069A0, a258.get(), (f32)(u32)m108 / (f32)m0B0); /* *= */
    } else {
        mEventFlags = fl | 0x100;
    }
    {
        s16 inv = cSAngle_Inv(&mViewCache.mDirection.mU);
        gabi::call(0x020065EC, a25c.get(), inv, a254.get());
    }
    s16 zero = gabi::load<s16>(0x101FF354);
    if ((s16)*a25c < zero) {
        p->m3A0 = 0;
        angSubEq(a254, a258);
        gabi::call(0x02006880, a25c.get(), t.get()); /* -a25c */
        *a25c = (s16)*t;
    } else {
        p->m3A0 = 1;
        angAddEq(a254, a258);
    }
    gabi::Local<cXyz> a108;
    attentionPos(a108, mpPlayerActor.get());
    bool b6 = false;
    if (mEventFlags & 0x80080) {
        gabi::Local<cXyz> a114;
        attentionPos(a114, mpPlayerActor.get());
        if (!pointInSight(a114)) {
            if (p->m388 == 0) p->m3A4 = (p->m3A0 != 1) ? 1 : 0;
            b6 = true;
            p->m388 = 60;
        }
    }
    if (gabi::call<bool>(0x025051BC, this, &mViewCache.mCenter, a108.get(), 0x7F) &&       /* lineBGCheckBack */
        gabi::call<bool>(0x024FCBE8, this, &mViewCache.mEye, &mViewCache.mCenter, 0x7F)) { /* lineBGCheck */
        p->m388 = 59; /* 60, minus this frame */
        b6 = true;
    } else if (p->m388 != 0) {
        s32 n = p->m388 - 1;
        p->m388 = n;
        if (n == 0 && !(mStickMainValueLast > 0.1f)) p->m388 = 1;
        b6 = true;
    }
    f32 cRatio = 1.0f - __builtin_fabsf(mStickCPosYLast);
    {
        f32 rr = gabi::call<f32>(0x024F7370, ea() + 0x8A4, (f32)mViewCache.mDirection.mRadius); /* RadiusRatio */
        customRB(rr, curveW);
    }
    if (mEventFlags & 0x10) {
        f32 b = 0.01f;
        p->m3B8 = b;
        p->m3B8 = gabi::fmadds(b - b, gabi::load<f32>(ea() + 0x760), b);
    } else if (m360 == 0) {
        f32 b = p->m3B8;
        p->m3B8 = gabi::fmadds(gabi::load<f32>(ea() + 0x77C) - b, gabi::load<f32>(ea() + 0x760), b); /* Cushion4Jump, CusCus */
    } else {
        f32 b = p->m3B8;
        p->m3B8 = gabi::fmadds(gabi::load<f32>(ea() + 0x778) - b, gabi::load<f32>(ea() + 0x760), b); /* Cushion4Base */
    }
    cpfL(&p->m390.x, &a108->x);
    cpfL(&p->m390.z, &a108->z);
    {
        f32 h = gabi::call<f32>(0x024F75CC, ea() + 0x8A4, ratio); /* LockonCenterHeight */
        if (b6) h = h + 25.0f;
        f32 y = p->m390.y;
        p->m390.y = gabi::fmadds((a108->y + h) - y, p->m3B8, y);
    }
    f32 R230 = g230->mRadius;
    f64 d21 = (f64)R230 * 0.05;
    f64 d20;
    if (mpLockonTarget.get() != nullptr) {
        f32 c1 = gabi::call<f32>(0x02006838, a25c.get()); /* Cos */
        angMul(&g230->mV, t, 1.3f);
        f32 c2 = gabi::call<f32>(0x02006838, t.get());
        f64 diff = (f64)__builtin_fabsf(c1) - (f64)__builtin_fabsf(c2);
        f64 sel = (diff >= 0.0) ? (f64)c2 : (f64)c1; /* fsel */
        f64 w = gabi::fmadd(sel, -0.5, 0.5);
        d20 = ((f64)(f32)g230->mRadius - (d21 + d21)) * __builtin_fabs(w);
    } else {
        f32 c = gabi::call<f32>(0x02006838, a25c.get());
        f64 w = gabi::fmadd((f64)c, -0.5, 0.5);
        d20 = (f64)R230 * __builtin_fabs(w);
    }
    {
        gabi::Local<cSAngle_l> a260, t36, t34, a264, a268, t48, t46, t44;
        cSAngle_ct(a260, &g230->mU);
        f32 b4 = p->m3B4;
        f32 nb4 = gabi::fmadds(v3 - b4, gabi::load<f32>(ea() + 0x760), b4);
        f32 b0 = p->m3B0;
        p->m3B4 = nb4;
        p->m3B0 = gabi::fmadds(v4 - b0, gabi::load<f32>(ea() + 0x760), b0);
        angSub(a260, t36, &p->m3A8.mU);
        angMul(t36, t34, p->m3B4);
        angAdd(&p->m3A8.mU, a264, t34);
        cSAngle_ct(a268);
        f32 R0 = p->m3A8.mRadius;
        f32 newR;
        if (b6) {
            newR = R0 * 0.75f;
            angSub(&g230->mV, t48, &p->m3A8.mV);
            angMul(t48, t46, 0.05f);
            angAdd(&p->m3A8.mV, t44, t46);
            gabi::call(0x02006638, a268.get(), t44.get()); /* Val(const cSAngle&) */
        } else {
            f32 s = (f32)(d20 + d21);
            newR = gabi::fmadds(gabi::fmsubs(s, p->m384, R0), p->m3B0, R0);
            angSub(&g230->mV, t48, &p->m3A8.mV);
            angMul(t48, t46, p->m3B4);
            angAdd(&p->m3A8.mV, t44, t46);
            gabi::call(0x02006638, a268.get(), t44.get());
        }
        gabi::call(0x020071E0, &p->m3A8, newR, a268.get(), a264.get()); /* Val(f32, V, U) */
        gabi::Local<cXyz> x, c;
        gabi::call(0x020073AC, &p->m3A8, x.get());
        cXyz_pl(&p->m390, c, x);
        mViewCache.mCenter.copy(*c);
    }
    if (mpLockonTarget.get() != nullptr && mLockOnActorId != 0xFFFFFFFFu) {
        gabi::Local<cXyz> a120;
        attentionPos(a120, mpPlayerActor.get());
        if (gabi::call<bool>(0x024FCBE8, this, a120.get(), &mViewCache.mCenter, 0x7F)) ForceLockOff(mLockOnActorId);
    }
    gabi::Local<cSGlobe_l> g238, g240;
    {
        gabi::Local<cXyz> d, d2;
        cXyz_mi(&mEye, d, &mViewCache.mCenter);
        gabi::call(0x02007324, g238.get(), d.get());
        cXyz_mi(&mViewCache.mEye, d2, &mViewCache.mCenter);
        gabi::call(0x02007324, g240.get(), d2.get());
    }
    cSAngle_ct(a26c, &mViewCache.mDirection.mU);
    cSAngle_ct(a270, &mViewCache.mDirection.mV);
    f32 R = mViewCache.mDirection.mRadius;
    angSub(a25c, a274, a258);
    f32 cw = gabi::load<f32>(ea() + 0x78C);

    if (b6) {
        gabi::Local<cSAngle_l> a278, t54, t52, t50;
        cSAngle_ct(a278);
        if (p->m3A4 == 1) gabi::call(0x02006694, a278.get(), 15.0f);
        else gabi::call(0x02006694, a278.get(), -15.0f);
        s16 inv = cSAngle_Inv(&g230->mU);
        gabi::call(0x020065DC, t54.get(), inv, a278.get()); /* s16 + cSAngle */
        angSub(t54, t52, a26c);
        angMul(t52, t50, 0.05f);
        angAddEq(a26c, t50);
    } else {
        f32 k;
        if (mpLockonTarget.get() == nullptr) {
            k = gabi::load<f32>(ea() + 0x768); /* mCamSetup.m028 */
        } else if (!(mEventFlags & 0x100)) {
            s16 l = *a258;
            if (l == zero) {
                k = 0.15f;
            } else {
                u32 n11C = m11C;
                f32 frac = (f32)n11C / (f32)m0B0;
                f32 x = -((f32)(s16)*a274 / (f32)l);
                f32 cr = customRB(x, cw);
                f32 a = camParamVal(style, 21) * cr;
                k = gabi::fmadds(m0B4 - a, 1.0f - frac, a);
            }
        } else {
            s16 l = *a258;
            if ((s16)*a25c < l) {
                f32 x = -((f32)(s16)*a274 / (f32)l);
                f32 cr = customRB(x, cw);
                k = camParamVal(style, 21) * cr;
            } else {
                gabi::Local<cSAngle_l> a27c, t6e, t56;
                gabi::call(0x020066C0, a27c.get(), 45.0f);
                cSAngle_l* c135 = gabi::call<cSAngle_l*>(0x020066C0, t6e.get(), 135.0f);
                s16 den;
                if ((s16)*a258 > (s16)*c135) {
                    angSub(gabi::at<cSAngle_l>(0x101FF35A), t56, a258); /* _180 - a258 */
                    den = *t56;
                    *a27c = den;
                } else {
                    den = *a27c;
                }
                f32 b = bezierL((f32)(s16)*a274 / (f32)den, cw);
                k = camParamVal(style, 20) * b;
                if (dist < 100.0f) k = k * bezierL(dist / 100.0f, 1.0f);
            }
        }
        gabi::Local<cSAngle_l> t1c;
        s16 inv = cSAngle_Inv(a254);
        gabi::call(0x020065EC, t1c.get(), inv, &mViewCache.mDirection.mU);
        *a274 = (s16)*t1c;
        gabi::call<f32>(0x02006720, a274.get()); /* Degree (unused) */
        angMul(a274, t1c, k);
        angAddEq(a26c, t1c);
    }

    bool radius280;
    if (lStatus1(mPadId) & 0x20000) {
        s32 n = p->m380;
        if (n <= chargeTimer) {
            gabi::Local<cSAngle_l> t5a, t58;
            f32 fr = (f32)n / (f32)chargeTimer;
            angSub(chargeLat, t5a, a270);
            f32 b = bezierL(fr, chargeBRatio);
            angMul(t5a, t58, b);
            angAddEq(a270, t58);
            mEventFlags = mEventFlags | 0x4000000;
            p->m380 = p->m380 + 1;
        } else {
            *a270 = (s16)*chargeLat;
        }
        radius280 = b6;
    } else if (b6) {
        gabi::Local<cSAngle_l> t30, t1e, t5c;
        cSAngle_ct(t30);
        angMul(&g230->mV, t1e, 0.7f);
        *t30 = (s16)*t1e;
        angAdd(t30, t5c, a270);
        angMul(t5c, t1e, 0.1f);
        angSubEq(a270, t1e);
        radius280 = true;
    } else {
        p->m380 = 0;
        if (mEventFlags & 0x10) {
            gabi::Local<cSAngle_l> t60, t5e;
            s16 lat = gabi::call<s16>(0x024F7520, ea() + 0x8A4, ratio); /* LockonLatitude */
            gabi::call(0x020065EC, t60.get(), lat, a270.get());
            angMul(t60, t5e, 0.05f);
            angAddEq(a270, t5e);
        } else if (m360 == 0 && !(lStatus0(mPadId) & 0x400)) {
            gabi::Local<cSAngle_l> t66, t64, t62;
            angSub(&g240->mV, t66, a270);
            angMul(t66, t64, cRatio);
            f32 c = gabi::call<f32>(0x02006838, &mViewCache.mDirection.mV);
            angMul(t64, t62, __builtin_fabsf(c));
            angAddEq(a270, t62);
        } else {
            fopAc_ac_c* pl = mpPlayerActor.get();
            bool eq;
            if (pl != nullptr && gabi::load<s16>(gabi::ea(pl) + 8) == 0xA8) { /* is_player */
                gabi::Local<be<u32>> key;
                u32 vt = gabi::load<u32>(gabi::ea(pl) + 0xB4);
                u32 id = gabi::call_ptr<u32>(gabi::load<u32>(vt + 0xB4), pl); /* getThrowBoomerangID */
                *key = id;
                fopAc_ac_c* boom = nullptr;
                if (id != 0xFFFFFFFFu) boom = fopAcIt_Judge(0x025E1234, key.get());
                eq = mpLockonTarget.get() == boom;
            } else {
                eq = mpLockonTarget.get() == nullptr;
            }
            if (eq && m784 != 0) {
                gabi::Local<cSAngle_l> t68;
                angMul(a270, t68, gabi::load<f32>(ea() + 0x768));
                angSubEq(a270, t68);
            } else {
                gabi::Local<cSAngle_l> t6c, t6a;
                s16 lat = gabi::call<s16>(0x024F7520, ea() + 0x8A4, ratio);
                gabi::call(0x020065EC, t6c.get(), lat, a270.get());
                angMul(t6c, t6a, gabi::load<f32>(ea() + 0x768));
                angAddEq(a270, t6a);
            }
            if (mpLockonTarget.get() != nullptr && (lStatus0(mPadId) & 0x400)) {
                /* HD: the horizontal distance to the target is computed but not used */
                gabi::Local<cXyz> q1, q2, q3;
                attentionPos(q1, mpLockonTarget.get());
                attentionPos(q2, mpPlayerActor.get());
                q2->y = 0.0f;
                q1->y = 0.0f;
                cXyz_mi(q1, q3, q2);
                q1->copy(*q3);
                std_sqrtf(PSVECSquareMag(q1));
            }
        }
        if (lStatus0(mPadId) & 0x1010000) {
            f32 deg = gabi::call<f32>(0x02006720, a270.get());
            f32 lo = camParamVal(style, 16);
            if (deg < lo) gabi::call(0x02006694, a270.get(), lo);
            f32 hi = camParamVal(style, 17);
            if (deg > hi) gabi::call(0x02006694, a270.get(), hi);
        } else {
            gabi::Local<be<s16>> lr;
            *lr = (s16)*a270;
            if (!gabi::call<bool>(0x024F7878, ea() + 0x73C, lr.get())) cSAngle_Val(a270, *lr); /* CheckLatitudeRange */
        }
        radius280 = false;
    }
    f32 newR;
    if (radius280) {
        newR = gabi::fmadds(280.0f - R, 0.05f, R);
    } else {
        gabi::Local<be<f32>> r70;
        cpfL(r70.get(), &g240->mRadius);
        if (!gabi::call<bool>(0x024F7400, ea() + 0x8A4, r70.get())) { /* DefaultRadius */
            newR = gabi::fmadds(*r70 - R, 0.05f, R);
        } else {
            newR = *r70;
        }
    }
    gabi::Local<cXyz> x, e;
    gabi::call(0x020071E0, &mViewCache.mDirection, newR, a270.get(), a26c.get());
    gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, e, x);
    mViewCache.mEye.copy(*e);
    f32 fv = gabi::call<f32>(0x024F75B4, ea() + 0x8A4, ratio); /* LockonFovy */
    f32 cur = mViewCache.mFovy;
    f32 m028 = gabi::load<f32>(ea() + 0x768);
    mEventFlags = mEventFlags | 0x2000;
    mViewCache.mFovy = gabi::fmadds(fv - cur, m028, cur);
    return true;
}
VERIFY(0x025052E8, &dCamera_c::lockonCamera);
