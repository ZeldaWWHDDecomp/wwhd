/**
 * d_camera_engine2.cpp (WWHD)
 * Follow camera dCamera_c: the camera engines that are "Nonmatching" stubs in the GameCube
 * decompilation (vomit, shield, hung, tornado, manual, tower, ride), written from the WWHD code.
 *
 * Verified against cking.rpx.
 */
#include "d/d_camera.h"

static inline u32 eStatus0(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8); }
static inline u32 eStatus1(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CDC); }
static inline void cpfE(void* dst, const void* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }
static inline s16 angCtE(cSAngle_l* tmp, s16 v) { return *gabi::call<cSAngle_l*>(0x0200658C, tmp, v); }
static inline void eSub(const void* a, cSAngle_l* out, const void* b) { gabi::call(0x020068B0, a, out, b); }
static inline void eMul(const void* a, cSAngle_l* out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline void eAdd(const void* a, cSAngle_l* out, const void* b) { gabi::call(0x02006894, a, out, b); }
static inline f32 eDegree(const void* a) { return gabi::call<f32>(0x02006720, a); }

/* work area of vomitCamera (mWork) */
struct Vomit_l {
    /* 0x00 */ be<u32> m00;  /* 'VMIT' */
    /* 0x04 */ be<s32> m04;  /* transition frames */
    /* 0x08 */ be<f32> m08;  /* remaining weight sum */
    /* 0x0C */ be<f32> m0C;  /* frame weight / fall time */
    /* 0x10 */ be<s32> m10;
    /* 0x14 */ be<s32> m14;  /* frames below the start height */
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<f32> m1C;  /* smoothed player height */
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<f32> m2C;  /* centre height ratio */
    /* 0x30 */ u8 _30[4];
    /* 0x34 */ be<f32> m34;  /* last player height */
    /* 0x38 */ cXyz m38;     /* smoothed centre */
    /* 0x44 */ cXyz m44;     /* player position */
    /* 0x50 */ be<u8> m50;
    /* 0x51 */ u8 _51[3];
    /* 0x54 */ be<f32> m54;
    /* 0x58 */ be<f32> m58;
    /* 0x5C */ u8 _5C[0x10];
    /* 0x6C */ be<f32> m6C;
    /* 0x70 */ be<f32> m70;
};

/* 02513518 (GameCube: not decompiled). Falling through a vomit hole (status 0x80000000) or
 * jumping: the camera follows the player at a fixed latitude and pulls in while the player is
 * below the start height. */
bool dCamera_c::vomitCamera(s32 style) {
    WWHD_FUNC(0x02513518, bool, this, style);
    Vomit_l* p = (Vomit_l*)mWork;
    u32 fl = mEventFlags;
    f32 v3 = camParamVal(style, 3);
    f32 v10 = camParamVal(style, 10);
    f32 v0 = camParamVal(style, 0);
    f32 v1 = camParamVal(style, 1);
    f32 v29 = camParamVal(style, 29);
    f32 v5 = camParamVal(style, 5);
    f32 v15 = camParamVal(style, 15);
    f32 v4 = camParamVal(style, 4);
    f32 v19 = camParamVal(style, 19);
    f32 v25 = camParamVal(style, 25);
    f32 v7 = camParamVal(style, 7);
    f32 v11 = camParamVal(style, 11);
    f32 v14 = camParamVal(style, 14);
    f32 speed = (fl & 0x8000) ? 1.0f : 8.0f;
    gabi::Local<cXyz> ofs;
    ofs->y = v5;
    ofs->x = v1;
    ofs->z = v0;
    gabi::Local<cSAngle_l> tmp;
    if (m11C == 0) {
        gabi::Local<cXyz> pos, rel, d;
        p->m00 = 0x564D4954; /* 'VMIT' */
        p->m38.copy(mViewCache.mCenter);
        positionOf(pos, mpPlayerActor.get());
        p->m44.copy(*pos);
        p->m18 = 0.0f;
        p->m10 = 0;
        positionOf(pos, mpPlayerActor.get());
        p->m14 = 0;
        cpfE(&p->m1C, &pos->y);
        cpfE(&p->m34, &pos->y);
        p->m58 = 0.75f;
        p->m54 = 0.75f;
        p->m6C = 0.0f;
        p->m70 = 0.0f;
        p->m2C = v4;
        p->m50 = 0;
        relationalPos(rel, mpPlayerActor.get(), ofs);
        cXyz_mi(&mCenter, d, rel);
        f32 l = std_sqrtf(PSVECSquareMag(d));
        f32 m = (l - v10 >= 0.0f) ? l : v10; /* fsel */
        fopAc_ac_c* pl = mpPlayerActor.get();
        f32 h = heightOf(pl);
        f32 hh = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
        m = m / hh;
        gabi::Local<cSAngle_l> dir, rd;
        directionOf(dir, pl); /* GHS: r4 kept across heightOf */
        s16 inv = cSAngle_Inv(dir);
        gabi::call(0x020065EC, rd.get(), inv, &mViewCache.mDirection.mU);
        f32 nrm = gabi::call<f32>(0x020067AC, rd.get()); /* cSAngle::Norm */
        f32 n2 = nrm + nrm;
        f32 r = std_sqrtf(m);
        s32 k = gabi::ftoi((speed * r) * (__builtin_fabsf(n2) + 1.0f));
        s32 n = k + 1;
        p->m04 = n;
        p->m08 = (f32)((n * (k + 2)) >> 1);
    }
    if (eStatus0(mPadId) & 0x80) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        p->m44.copy(*pos);
        positionOf(pos, mpPlayerActor.get());
        f32 y = p->m1C;
        p->m1C = gabi::fmadds(pos->y - y, 0.05f, y);
    }
    if (m100 == 0) {
        gabi::Local<cXyz> t, d, s, x, e;
        gabi::Local<cSAngle_l> lat, dir, u, t12, t0a, t08;
        gabi::Local<cSGlobe_l> g;
        f32 r = (f32)(s32)((s32)p->m04 - (s32)m11C);
        f32 k = r / p->m08;
        p->m0C = r;
        relationalPos(t, mpPlayerActor.get(), ofs);
        cXyz_mi(t, d, &mViewCache.mCenter);
        cXyz_ml(d, s, k);
        PSVECAdd(&p->m38, s, &p->m38);
        cXyz_mi(&p->m38, d, &mViewCache.mCenter);
        cXyz_ml(d, t, v3);
        PSVECAdd(&mViewCache.mCenter, t, &mViewCache.mCenter);
        gabi::call(0x020066C0, lat.get(), v15);
        directionOf(dir, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(dir);
        cSAngle_l* up = gabi::call<cSAngle_l*>(0x0200658C, u.get(), inv);
        gabi::call(0x02007248, g.get(), v10, lat.get(), up);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
        eSub(&g->mV, t12, &mViewCache.mDirection.mV);
        eMul(t12, t0a, k);
        eAdd(&mViewCache.mDirection.mV, t08, t0a);
        mViewCache.mDirection.mV = angCtE(tmp, *t08);
        eSub(&g->mU, t0a, &mViewCache.mDirection.mU);
        eMul(t0a, t08, k);
        eAdd(&mViewCache.mDirection.mU, t12, t08);
        mViewCache.mDirection.mU = angCtE(tmp, *t12);
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        p->m28 = eDegree(&mViewCache.mDirection.mV);
        cpfE(&p->m24, &mViewCache.mDirection.mRadius);
        cpfE(&p->m20, &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(v25 - fv, k, fv);
        p->m08 = p->m08 - p->m0C;
        if ((u32)m11C >= (u32)((s32)p->m04 - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
            p->m0C = 0.0f;
        }
        return true;
    }

    f32 cy = v4;
    {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        if (pos->y < p->m1C - 1.0f) {
            f32 c = p->m0C + 1.0f;
            f32 a = p->m2C;
            f32 n = gabi::fmadds((1.0f - v4) - a, c * 0.01f, a);
            p->m0C = c;
            cy = v4 + n;
            p->m2C = n;
        } else {
            gabi::Local<cXyz> pos2, pos3;
            positionOf(pos2, mpPlayerActor.get());
            positionOf(pos3, mpPlayerActor.get());
            f32 q = (pos3->y - p->m1C) / v7;
            f32 w = 1.0f - q;
            ofs->y = ofs->y * w;
            ofs->z = ofs->z * w;
            p->m0C = 0.0f;
            p->m2C = 0.0f;
        }
    }
    gabi::Local<cXyz> mul, t, d, s;
    mul->x = v3;
    mul->y = cy;
    mul->z = v3;
    relationalPos(t, mpPlayerActor.get(), ofs);
    cXyz_mi(t, d, &mViewCache.mCenter);
    gabi::call(0x0201AF1C, d.get(), s.get(), mul.get()); /* cXyz * cXyz */
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    gabi::Local<cSGlobe_l> g;
    cXyz_mi(&mViewCache.mEye, t, &mViewCache.mCenter);
    gabi::call(0x02007324, g.get(), t.get());
    gabi::Local<cSAngle_l> v, u, dir;
    cSAngle_ct(v);
    cSAngle_ct(u);
    f32 gr = g->mRadius;
    *v = (s16)g->mV;
    f32 R = (gr - v11 >= 0.0f) ? gr : v11; /* fsel */
    directionOf(dir, mpPlayerActor.get());
    cSAngle_Val(u, cSAngle_Inv(dir));
    if (eStatus0(mPadId) & 0x80000000) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        if (pos->y < p->m1C - 1.0f) {
            p->m14 = p->m14 + 1;
            R = v10;
        } else {
            gabi::Local<cXyz> pos2;
            positionOf(pos2, mpPlayerActor.get());
            if (pos2->y < p->m34 && R > v10) R = v10;
        }
    } else if (eStatus0(mPadId) & 0x80) {
        R = v10;
        gabi::call(0x02006694, v.get(), v15); /* Val(f32) */
        gabi::Local<cSAngle_l> d2;
        directionOf(d2, mpPlayerActor.get());
        cSAngle_Val(u, cSAngle_Inv(d2));
        p->m14 = 0;
    } else if (!(eStatus0(mPadId) & 0x80000000)) {
        p->m14 = 0;
    }
    {
        gabi::Local<cSAngle_l> t22, t20, t0c;
        gabi::Local<cXyz> x, e, pos;
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(R - rr, v14, rr);
        eSub(v, t22, &mViewCache.mDirection.mV);
        eMul(t22, t20, v19);
        eAdd(&mViewCache.mDirection.mV, t0c, t20);
        mViewCache.mDirection.mV = angCtE(tmp, *t0c);
        mViewCache.mDirection.mU = angCtE(tmp, *u);
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        f32 fv = mViewCache.mFovy;
        f32 nf = gabi::fmadds(v25 - fv, v29, fv);
        mViewCache.mEye.copy(*e);
        mViewCache.mFovy = nf;
        positionOf(pos, mpPlayerActor.get());
        cpfE(&p->m34, &pos->y);
    }
    return true;
}
VERIFY(0x02513518, &dCamera_c::vomitCamera);

/* stack dBgS_CamLinChk */
static void camLinChk_ct_e2(u32 o) {
    gabi::call(0x02008FEC, o);
    gabi::store<u32>(o + 0x00, o + 0x58);
    gabi::store<u32>(o + 0x04, o + 0x64);
    gabi::store<u32>(o + 0x10, 0x1004A66C);
    gabi::store<u32>(o + 0x20, 0x1004A67C);
    gabi::store<u32>(o + 0x58, 0x1004A69C);
    gabi::store<u32>(o + 0x64, 0x1004A68C);
    gabi::store<u8>(o + 0x5C, 0);
    gabi::store<u8>(o + 0x5D, 1);
    gabi::store<u8>(o + 0x5E, 0);
    gabi::store<u8>(o + 0x5F, 0);
    gabi::store<u8>(o + 0x60, 0);
    gabi::store<u8>(o + 0x61, 0);
    gabi::store<u8>(o + 0x62, 0);
    gabi::store<u32>(o + 0x68, 3);
}
static void camLinChk_dt_e2(u32 o) {
    gabi::store<u32>(o + 0x20, 0x1004A4DC);
    gabi::store<u32>(o + 0x64, 0x1004A4EC);
    gabi::store<u32>(o + 0x58, 0x1004A61C);
    gabi::call(0x02008B4C, o, 0);
}
/* line check to the centre with an existing check object; the centre is put on the wall and
 * pushed out along its normal */
static void wallCheckE2(dCamera_c* c, cXyz* start, u32 o) {
    if (c->lineBGCheck(start, &c->mViewCache.mCenter, (u8*)gabi::at<u8>(o), 0x7F)) {
        u32 plane = gabi::call<u32>(0x020084C8, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(o + 0x16), (u32)gabi::load<u16>(o + 0x14)); /* GetTriPla */
        c->mViewCache.mCenter.copy(*gabi::at<cXyz>(o + 0x30));
        if (plane != 0) PSVECAdd(&c->mViewCache.mCenter, gabi::at<cXyz>(plane), &c->mViewCache.mCenter);
    }
}

/* the shield angles of the player (body angles at 0x3D0) or of actor 0x16F (at 0x4230) */
static u32 shieldAngles(fopAc_ac_c* a) {
    if (a == nullptr) return 0;
    s16 nm = gabi::load<s16>(gabi::ea(a) + 8);
    if (nm == 0xA8) return gabi::ea(a) + 0x3D0;
    if (nm == 0x16F) return gabi::ea(a) + 0x4230;
    return 0;
}

/* 02513E98 (GameCube: not decompiled). Shield camera: the eye follows the offset globe rotated by
 * the player's (or actor 0x16F's) shield angles; after the frame-weighted transition the
 * direction eases towards the param latitude/yaw plus a fraction of the shield angles. */
bool dCamera_c::shieldCamera(s32 style) {
    WWHD_FUNC(0x02513E98, bool, this, style);
    u32 w = ea() + 0x37C; /* mWork: 'SHLD', +4 frames, +8 weight sum, +0xC weight, +0x10/+0x14 radius, +0x18 latitude */
    gabi::Local<cXyz> ofs;
    ofs->x = camParamVal(style, 1);
    f32 v25 = camParamVal(style, 25);
    ofs->y = camParamVal(style, 5);
    f32 v18 = camParamVal(style, 18);
    f32 v15 = camParamVal(style, 15);
    f32 v20 = camParamVal(style, 20);
    f32 v23 = camParamVal(style, 23);
    f32 v11 = camParamVal(style, 11);
    ofs->z = camParamVal(style, 0);
    f32 v10 = camParamVal(style, 10);
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> tmp;
    gabi::call(0x02007324, g.get(), ofs.get());
    fopAc_ac_c* pl = mpPlayerActor.get();
    u32 sa = shieldAngles(pl);
    if (sa != 0) {
        gabi::Local<cSAngle_l> t1a, t2c, dir;
        gabi::call(0x02006908, &g->mV, t1a.get(), (s16)gabi::load<s16>(sa)); /* V - angle x */
        g->mV = angCtE(tmp, *t1a);
        gabi::call(0x020068F4, &g->mU, t2c.get(), (s16)gabi::load<s16>(sa + 2)); /* U + angle y */
        directionOf(dir, mpPlayerActor.get());
        eAdd(t2c, t1a, dir);
        s16 nu = angCtE(tmp, *t1a);
        pl = mpPlayerActor.get();
        g->mU = nu;
    }
    gabi::Local<cXyz> target;
    {
        gabi::Local<cXyz> a, x;
        attentionPos(a, pl);
        gabi::call(0x020073AC, g.get(), x.get());
        cXyz_pl(a, target, x);
    }
    if (m11C == 0) {
        gabi::Local<cXyz> d;
        gabi::Local<cSAngle_l> dir, rd;
        gabi::store<u32>(w, 0x53484C44); /* 'SHLD' */
        cXyz_mi(&mCenter, d, target);
        f32 l = std_sqrtf(PSVECSquareMag(d));
        f32 m = (l - v10 >= 0.0f) ? l : v10; /* fsel */
        fopAc_ac_c* p2 = mpPlayerActor.get();
        f32 h = heightOf(p2);
        f32 hh = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
        m = m / hh;
        directionOf(dir, p2); /* GHS: r4 kept across heightOf */
        s16 inv = cSAngle_Inv(dir);
        gabi::call(0x020065EC, rd.get(), inv, &mViewCache.mDirection.mU);
        f32 nrm = gabi::call<f32>(0x020067AC, rd.get()); /* cSAngle::Norm */
        f32 n2 = nrm + nrm;
        f32 r = std_sqrtf(m);
        s32 k = gabi::ftoi((3.0f * r) * (__builtin_fabsf(n2) + 1.0f));
        s32 n = k + 1;
        gabi::store<s32>(w + 4, n);
        gabi::store<f32>(w + 8, (f32)((n * (k + 2)) >> 1));
    }
    gabi::Local<u8[0x6C]> lc;
    u32 o = gabi::ea(lc.get());
    if (m100 == 0) {
        gabi::Local<cXyz> d, s, a, x, e;
        gabi::Local<cSAngle_l> dir, lat, u, t0e, t10, t0a;
        gabi::Local<cSGlobe_l> g2;
        f32 r = (f32)(s32)(gabi::load<s32>(w + 4) - (s32)m11C);
        f32 k = r / gabi::load<f32>(w + 8);
        gabi::store<f32>(w + 0xC, r);
        cXyz_mi(target, d, &mViewCache.mCenter);
        cXyz_ml(d, s, k);
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
        attentionPos(a, mpPlayerActor.get());
        a->y = a->y - 15.0f;
        camLinChk_ct_e2(o);
        f32 dist = gabi::call<f32>(0x024F7AC4, target.get(), &mViewCache.mCenter); /* dCamMath::xyzHorizontalDistance */
        f32 ox = ofs->x, oz = ofs->z;
        f32 om = (oz - ox >= 0.0f) ? oz : ox; /* fsel */
        if (dist < __builtin_fabsf(om) + 20.0f) wallCheckE2(this, a, o);
        f32 R = gabi::call<f32>(0x025028A0, (f32)mViewCache.mDirection.mRadius, v11, v10); /* limitf */
        directionOf(dir, mpPlayerActor.get());
        cSAngle_l* lp = gabi::call<cSAngle_l*>(0x020066C0, lat.get(), v15);
        s16 inv = cSAngle_Inv(dir);
        cSAngle_l* up = gabi::call<cSAngle_l*>(0x0200658C, u.get(), inv);
        gabi::call(0x02007248, g2.get(), R, lp, up);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g2->mRadius - rr, k, rr);
        eSub(&g2->mV, t0e, &mViewCache.mDirection.mV);
        eMul(t0e, t10, k);
        eAdd(&mViewCache.mDirection.mV, t0a, t10);
        mViewCache.mDirection.mV = angCtE(tmp, *t0a);
        eSub(&g2->mU, t0e, &mViewCache.mDirection.mU);
        eMul(t0e, t10, k);
        eAdd(&mViewCache.mDirection.mU, t0a, t10);
        mViewCache.mDirection.mU = angCtE(tmp, *t0a);
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        gabi::store<f32>(w + 0x18, eDegree(&mViewCache.mDirection.mV));
        cpfE(gabi::at<u32>(w + 0x14), &mViewCache.mDirection.mRadius);
        cpfE(gabi::at<u32>(w + 0x10), &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(v25 - fv, k, fv);
        gabi::store<f32>(w + 8, gabi::load<f32>(w + 8) - gabi::load<f32>(w + 0xC));
        if ((u32)m11C >= (u32)(gabi::load<s32>(w + 4) - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
            gabi::store<f32>(w + 0xC, 0.0f);
        }
        camLinChk_dt_e2(o);
        return true;
    }
    mViewCache.mCenter.copy(*target);
    {
        gabi::Local<cXyz> a;
        attentionPos(a, mpPlayerActor.get());
        a->y = a->y - 15.0f;
        camLinChk_ct_e2(o);
        wallCheckE2(this, a, o);
    }
    gabi::Local<cSAngle_l> lat, ax, dir, uinv, ay, t18, t16, t14, t12, t08;
    gabi::call(0x020066C0, lat.get(), v15);
    cSAngle_ct(ax);
    directionOf(dir, mpPlayerActor.get());
    s16 inv = cSAngle_Inv(dir);
    cSAngle_ct(uinv, inv);
    cSAngle_ct(ay);
    u32 sa2 = shieldAngles(mpPlayerActor.get());
    s16 yv;
    if (sa2 != 0) {
        cSAngle_Val(ax, gabi::load<s16>(sa2));
        yv = gabi::load<s16>(sa2 + 2);
    } else {
        cSAngle_Val(ax, 0);
        yv = 0;
    }
    cSAngle_Val(ay, yv);
    eMul(ax, t18, v18);
    eAdd(lat, t16, t18);
    eSub(t16, t14, &mViewCache.mDirection.mV);
    eMul(t14, t12, v20);
    eAdd(&mViewCache.mDirection.mV, t08, t12);
    mViewCache.mDirection.mV = angCtE(tmp, *t08);
    eMul(ay, t08, v23);
    eAdd(uinv, t12, t08);
    eSub(t12, t14, &mViewCache.mDirection.mU);
    eMul(t14, t16, v20);
    eAdd(&mViewCache.mDirection.mU, t18, t16);
    s16 nu = angCtE(tmp, *t18);
    f32 R0 = mViewCache.mDirection.mRadius;
    mViewCache.mDirection.mU = nu;
    f32 lim = gabi::call<f32>(0x025028A0, R0, v11, v10); /* limitf */
    mViewCache.mDirection.mRadius = gabi::fmadds(lim - R0, v20, R0);
    gabi::Local<cXyz> x, e;
    gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, e, x);
    f32 fv = mViewCache.mFovy;
    mViewCache.mEye.copy(*e);
    mViewCache.mFovy = gabi::fmadds(v25 - fv, v20, fv);
    camLinChk_dt_e2(o);
    return true;
}
VERIFY(0x02513E98, &dCamera_c::shieldCamera);

/* work area of hungCamera (mWork) */
struct Hung_l {
    /* 0x00 */ be<u32> m00;  /* 'HUNG' */
    /* 0x04 */ be<s32> m04;  /* transition frames */
    /* 0x08 */ be<f32> m08;  /* remaining weight sum */
    /* 0x0C */ be<f32> m0C;  /* frame weight / fall time */
    /* 0x10 */ be<s32> m10;
    /* 0x14 */ be<s32> m14;  /* frames hanging */
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<f32> m1C;  /* smoothed player height */
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<f32> m2C;  /* centre height ratio */
    /* 0x30 */ u8 _30[4];
    /* 0x34 */ be<f32> m34;  /* last player height */
    /* 0x38 */ cXyz m38;     /* smoothed centre */
    /* 0x44 */ be<u8> m44;
    /* 0x45 */ u8 _45[3];
    /* 0x48 */ be<f32> m48;
    /* 0x4C */ be<f32> m4C;
    /* 0x50 */ u8 _50[0x10];
    /* 0x60 */ be<f32> m60;  /* C stick x (smoothed) */
    /* 0x64 */ be<f32> m64;  /* C stick y (smoothed) */
};

/* C stick ratio: +-1 beyond 0.7, linear inside */
static inline f32 hungStick(f32 v) {
    if (v > 0.7f) return 1.0f;
    if (v < -0.7f) return -1.0f;
    return v / 0.7f;
}

/* 0250EF98 (GameCube: not decompiled). Hanging from a ledge or rope (status 0x80000000 / 0x80):
 * the camera eases to a fixed latitude behind the player, swings with the rope (status 0x100)
 * and can be tilted with the C stick. */
bool dCamera_c::hungCamera(s32 style) {
    WWHD_FUNC(0x0250EF98, bool, this, style);
    Hung_l* p = (Hung_l*)mWork;
    s32 pad = mPadId;
    f32 v13 = camParamVal(style, 13);
    f32 v0 = camParamVal(style, 0);
    f32 v18 = camParamVal(style, 18);
    f32 v10 = camParamVal(style, 10);
    f32 v4 = camParamVal(style, 4);
    f32 v25 = camParamVal(style, 25);
    f32 v14 = camParamVal(style, 14);
    f32 v19 = camParamVal(style, 19);
    f32 v1 = camParamVal(style, 1);
    f32 v29 = camParamVal(style, 29);
    f32 v15 = camParamVal(style, 15);
    f32 v16 = camParamVal(style, 16);
    f32 v5 = camParamVal(style, 5);
    f32 v3 = camParamVal(style, 3);
    f32 v11 = camParamVal(style, 11);
    if ((gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x100) && v0 > -10.0f) v0 = -10.0f;
    gabi::Local<cXyz> ofs;
    ofs->y = v5;
    ofs->z = v0;
    ofs->x = v1;
    gabi::Local<cSAngle_l> tmp;
    if (m11C == 0) {
        gabi::Local<cXyz> pos, rel, d;
        p->m00 = 0x48554E47; /* 'HUNG' */
        p->m38.copy(mViewCache.mCenter);
        p->m18 = 0.0f;
        p->m10 = 0;
        positionOf(pos, mpPlayerActor.get());
        p->m44 = 0;
        cpfE(&p->m1C, &pos->y);
        cpfE(&p->m34, &pos->y);
        p->m4C = 0.75f;
        p->m48 = 0.75f;
        p->m60 = 0.0f;
        p->m64 = 0.0f;
        p->m2C = v4;
        p->m14 = 0;
        relationalPos(rel, mpPlayerActor.get(), ofs);
        cXyz_mi(&mCenter, d, rel);
        f32 l = std_sqrtf(PSVECSquareMag(d));
        f32 m = (l - v10 >= 0.0f) ? l : v10; /* fsel */
        fopAc_ac_c* pl = mpPlayerActor.get();
        f32 h = heightOf(pl);
        f32 hh = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
        m = m / hh;
        gabi::Local<cSAngle_l> dir, rd;
        directionOf(dir, pl); /* GHS: r4 kept across heightOf */
        s16 inv = cSAngle_Inv(dir);
        gabi::call(0x020065EC, rd.get(), inv, &mViewCache.mDirection.mU);
        f32 nrm = gabi::call<f32>(0x020067AC, rd.get()); /* cSAngle::Norm */
        f32 n2 = nrm + nrm;
        f32 r = std_sqrtf(m);
        s32 k = gabi::ftoi((5.0f * r) * (__builtin_fabsf(n2) + 1.0f));
        s32 n = k + 1;
        p->m04 = n;
        p->m08 = (f32)((n * (k + 2)) >> 1);
    }
    if (!(eStatus0(mPadId) & 0x80000000)) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        f32 y = p->m1C;
        p->m1C = gabi::fmadds(pos->y - y, 0.05f, y);
    }
    if (m100 == 0) {
        gabi::Local<cXyz> t, d, s, x, e;
        gabi::Local<cSAngle_l> lat, dir, u, t10, t0c, t0a;
        gabi::Local<cSGlobe_l> g;
        f32 r = (f32)(s32)((s32)p->m04 - (s32)m11C);
        f32 k = r / p->m08;
        p->m0C = r;
        relationalPos(t, mpPlayerActor.get(), ofs);
        cXyz_mi(t, d, &mViewCache.mCenter);
        cXyz_ml(d, s, k);
        PSVECAdd(&p->m38, s, &p->m38);
        cXyz_mi(&p->m38, d, &mViewCache.mCenter);
        cXyz_ml(d, t, v3);
        PSVECAdd(&mViewCache.mCenter, t, &mViewCache.mCenter);
        f32 R = gabi::call<f32>(0x025028A0, (f32)mViewCache.mDirection.mRadius, v11, v10); /* limitf */
        gabi::call(0x020066C0, lat.get(), v15);
        directionOf(dir, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(dir);
        cSAngle_l* up = gabi::call<cSAngle_l*>(0x0200658C, u.get(), inv);
        gabi::call(0x02007248, g.get(), R, lat.get(), up);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
        eSub(&g->mV, t10, &mViewCache.mDirection.mV);
        eMul(t10, t0c, k);
        eAdd(&mViewCache.mDirection.mV, t0a, t0c);
        mViewCache.mDirection.mV = angCtE(tmp, *t0a);
        eSub(&g->mU, t0c, &mViewCache.mDirection.mU);
        eMul(t0c, t0a, k);
        eAdd(&mViewCache.mDirection.mU, t10, t0a);
        mViewCache.mDirection.mU = angCtE(tmp, *t10);
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        p->m28 = eDegree(&mViewCache.mDirection.mV);
        cpfE(&p->m24, &mViewCache.mDirection.mRadius);
        cpfE(&p->m20, &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(v25 - fv, k, fv);
        p->m08 = p->m08 - p->m0C;
        if ((u32)m11C >= (u32)((s32)p->m04 - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
            p->m0C = 0.0f;
        }
        return true;
    }

    f32 cy = v4;
    if (eStatus0(mPadId) & 0x80000080) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        if (pos->y < p->m1C - 1.0f) {
            f32 c = p->m0C + 1.0f;
            f32 a = p->m2C;
            f32 n = gabi::fmadds((1.0f - v4) - a, c * 0.005f, a);
            p->m0C = c;
            p->m2C = n;
            cy = v4 + n;
        } else {
            p->m2C = 0.0f;
            p->m0C = 0.0f;
        }
    }
    {
        gabi::Local<cXyz> mul, t, d, s;
        mul->x = v3;
        mul->y = cy;
        mul->z = v3;
        relationalPos(t, mpPlayerActor.get(), ofs);
        cXyz_mi(t, d, &mViewCache.mCenter);
        gabi::call(0x0201AF1C, d.get(), s.get(), mul.get()); /* cXyz * cXyz */
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    }
    f32 swing = 0.0f;
    if (eStatus0(mPadId) & 0x100) {
        /* rope: the swing angle from the rope anchor (player +0x408) */
        gabi::Local<cXyz> anchor, pos, proj;
        gabi::Local<cSAngle_l> dir, ta, dir2, rel;
        u32 pl = gabi::ea(mpPlayerActor.get());
        cpfE(&anchor->x, gabi::at<u32>(pl + 0x408));
        cpfE(&anchor->y, gabi::at<u32>(pl + 0x40C));
        cpfE(&anchor->z, gabi::at<u32>(pl + 0x410));
        positionOf(pos, mpPlayerActor.get());
        directionOf(dir, mpPlayerActor.get());
        gabi::call(0x024F7AEC, proj.get(), dir.get(), pos.get(), anchor.get()); /* dCamMath::xyzProjPosOnYZ */
        f32 hd = gabi::call<f32>(0x024F7AC4, pos.get(), proj.get()); /* dCamMath::xyzHorizontalDistance */
        f32 at = gabi::call<f32>(0x0201971C, hd, proj->y - pos->y); /* cM_atan2f */
        f32 dg = 180.0f / gabi::load<f32>(0x1046EED0);
        swing = at * dg;
        s16 ty = gabi::call<s16>(0x0200F93C, anchor.get(), pos.get()); /* cLib_targetAngleY */
        cSAngle_ct(ta, ty);
        directionOf(dir2, mpPlayerActor.get());
        eSub(ta, rel, dir2);
        if (gabi::call<f32>(0x02006838, rel.get()) > 0.0f) swing = -swing; /* Cos */
    } else if (eStatus0(mPadId) & 0x80000080) {
        f32 sc = (90.0f - v15) * 0.9f;
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        if (pos->y > p->m1C) {
            gabi::Local<cXyz> pos2;
            positionOf(pos2, mpPlayerActor.get());
            f32 q = (pos2->y - p->m1C) / 1500.0f;
            if (q > 1.0f) swing = sc * 1.0f;
            else swing = sc * ((q >= 0.0f) ? q : 0.0f); /* fsel */
        } else {
            gabi::Local<cXyz> pos2;
            positionOf(pos2, mpPlayerActor.get());
            f32 q = (pos2->y - p->m1C) / -3000.0f;
            if (q > 1.0f) swing = sc * 1.0f;
            else swing = sc * ((q >= 0.0f) ? q : 0.0f);
        }
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> av, au; /* sp08 latitude, sp0E longitude */
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        gabi::call(0x02007324, g.get(), d.get());
    }
    cSAngle_ct(av);
    cSAngle_ct(au);
    f32 R;
    bool stick;
    if ((eStatus0(mPadId) & 0x80000000) && p->m14 < 10) {
        *au = (s16)g->mU;
        *av = (s16)g->mV;
        s32 c = p->m14;
        R = g->mRadius;
        p->m14 = c + 1;
        f32 fr = 1.0f - (f32)c / 10.0f;
        v14 = gabi::fmadds(1.0f - v14, fr, v14);
        v19 = gabi::fmadds(1.0f - v19, fr, v19);
        stick = (eStatus0(mPadId) & 0x80000080) != 0;
    } else {
        if (!(eStatus0(mPadId) & 0x80000000)) p->m14 = 0;
        R = g->mRadius;
        if (eStatus0(mPadId) & 0x80) {
            f32 vr = mViewCache.mDirection.mRadius;
            R = gabi::fmadds(v11 - vr, v13, vr);
        } else {
            f32 gr = g->mRadius;
            if (gr > v10) R = gabi::fmadds(v10 - gr, v13, gr);
            else if (gr < v11) R = gabi::fmadds(v11 - gr, v13, gr);
        }
        gabi::Local<cSAngle_l> dir;
        if (!(eStatus0(mPadId) & 0x80)) {
            gabi::call(0x02006694, av.get(), gabi::fmadds(swing, v18, v15));
        } else {
            gabi::call(0x02006694, av.get(), v16);
        }
        directionOf(dir, mpPlayerActor.get());
        cSAngle_Val(au, cSAngle_Inv(dir));
        stick = (eStatus0(mPadId) & 0x80000080) != 0;
    }
    if (stick) {
        gabi::Local<cSAngle_l> lo, hi, x178, t1, t2;
        gabi::call(0x020066C0, lo.get(), -20.0f);
        gabi::call(0x020066C0, hi.get(), 75.0f);
        gabi::call(0x020066C0, x178.get(), 178.0f);
        f32 sx = hungStick(mStickCPosXLast);
        f32 sy = hungStick(mStickCPosYLast);
        f32 bx = gabi::call<f32>(0x024F6F9C, sx, 2.0f); /* dCamMath::rationalBezierRatio */
        f32 by = gabi::call<f32>(0x024F6F9C, sy, 2.0f);
        f32 m64 = p->m64;
        f32 m60 = p->m60;
        f32 n64 = gabi::fmadds(by - m64, 0.5f, m64);
        p->m60 = gabi::fmadds(bx - m60, 0.15f, m60);
        p->m64 = n64;
        if (sy > 0.0f) {
            R = gabi::fmadds(80.0f - R, n64, R);
            eSub(lo, t1, av);
            eMul(t1, t2, p->m64);
            gabi::call(0x020068CC, av.get(), t2.get()); /* += */
        } else {
            R = gabi::fnmsubs(700.0f - R, n64, R);
            eSub(hi, t1, av);
            eMul(t1, t2, p->m64);
            gabi::call(0x020068E0, av.get(), t2.get()); /* -= */
        }
    }
    {
        gabi::Local<cSAngle_l> t20, t16, t14;
        gabi::Local<cXyz> x, e, pos;
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(R - rr, v14, rr);
        eSub(av, t20, &mViewCache.mDirection.mV);
        eMul(t20, t16, v19);
        eAdd(&mViewCache.mDirection.mV, t14, t16);
        mViewCache.mDirection.mV = angCtE(tmp, *t14);
        mViewCache.mDirection.mU = angCtE(tmp, *au);
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        f32 fv = mViewCache.mFovy;
        mViewCache.mEye.copy(*e);
        mViewCache.mFovy = gabi::fmadds(v25 - fv, v29, fv);
        positionOf(pos, mpPlayerActor.get());
        cpfE(&p->m34, &pos->y);
    }
    return true;
}
VERIFY(0x0250EF98, &dCamera_c::hungCamera);

/* work area of tornadoCamera (mWork) */
struct Tornado_l {
    /* 0x00 */ be<u32> m00;           /* 'TRND' */
    /* 0x04 */ gptr<fopAc_ac_c> m04;  /* tornado actor (0xA5) */
    /* 0x08 */ gptr<fopAc_ac_c> m08;  /* ship actor (0x1BB or 0x11B) */
    /* 0x0C */ be<u32> m0C;           /* m07C at start */
    /* 0x10 */ be<s32> m10;           /* entry: 0 normal, 1 side eye, 2 event */
    /* 0x14 */ cXyz m14;              /* side eye */
    /* 0x20 */ cXyz m20;              /* C-stick centre offset */
    /* 0x2C */ cXyz m2C;              /* centre target */
};

/* the side eye of a tornado entry: a function-local static offset table (initialised on first
 * use), rotated to the side the player is on; m10 = 1 when the eye sees the player */
static void tornadoSideEye(dCamera_c* c, Tornado_l* p, cSAngle_l* base, u32 guard, u32 table, bool ship) {
    if (gabi::load<u32>(guard) == 0) {
        if (ship) { /* status1 0x80 table */
            gabi::store<u32>(guard, 1);
            gabi::store<f32>(table + 0x00, -120.0f);
            gabi::store<f32>(table + 0x04, 20.0f);
            gabi::store<f32>(table + 0x08, 280.0f);
            gabi::store<f32>(table + 0x0C, -120.0f);
            gabi::store<f32>(table + 0x10, 20.0f);
            gabi::store<f32>(table + 0x14, 280.0f);
        } else {
            gabi::store<f32>(table + 0x00, -120.0f);
            gabi::store<f32>(table + 0x04, 50.0f);
            gabi::store<f32>(table + 0x08, 280.0f);
            gabi::store<f32>(table + 0x0C, 240.0f);
            gabi::store<f32>(table + 0x10, 160.0f);
            gabi::store<f32>(table + 0x14, -80.0f);
            gabi::store<u32>(guard, 1);
        }
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> ta, rel, t, tmp;
    gabi::Local<cXyz> pos, x, e;
    gabi::call(0x02007324, g.get(), gabi::at<cXyz>(table + ((u32)c->m07C & 1) * 0xC));
    cXyz* a = c->positionPntOf(p->m04.get());
    cXyz* b = c->positionPntOf(c->mpPlayerActor.get());
    s16 ty = cLib_targetAngleY(b, a);
    cSAngle_ct(ta, ty);
    eSub(ta, rel, base);
    if ((s16)*rel < gabi::load<s16>(0x101FF354)) eSub(base, t, &g->mU);
    else eAdd(base, t, &g->mU);
    g->mU = angCtE(tmp, *t);
    c->positionOf(pos, p->m04.get());
    gabi::call(0x020073AC, g.get(), x.get());
    cXyz_pl(pos, e, x);
    p->m14.copy(*e);
    cXyz* pp = c->positionPntOf(c->mpPlayerActor.get());
    if (!gabi::call<bool>(0x024FCBE8, c, &p->m14, pp, 0x7F)) p->m10 = 1; /* lineBGCheck */
}

/* 025121E8 (GameCube: not decompiled). Tornado / Cyclos ride (status 0x1000000 / 0x10000 and
 * status1 2 / 0x80): the camera looks at the player from behind the tornado (actor 0xA5),
 * pulled out by the ship's distance (actor 0x1BB or 0x11B), with an entry transition from a side
 * eye; the C stick shifts the centre and the stick banks the view. */
bool dCamera_c::tornadoCamera(s32 style) {
    WWHD_FUNC(0x025121E8, bool, this, style);
    Tornado_l* p = (Tornado_l*)mWork;
    u32 entry = CAM_STYLES + style * 0x84;
    f32 v0 = camParamVal(style, 0);
    f32 v13 = camParamVal(style, 13);
    f32 v10 = camParamVal(style, 10);
    f32 v15 = camParamVal(style, 15);
    f32 v3 = camParamVal(style, 3);
    f32 v1 = camParamVal(style, 1);
    f32 v5 = camParamVal(style, 5);
    f32 v4 = camParamVal(style, 4);
    f32 v8 = camParamVal(style, 8);
    f32 v14 = camParamVal(style, 14);
    f32 v16 = camParamVal(style, 16);
    f32 v18 = camParamVal(style, 18);
    gabi::Local<cSAngle_l> lo, hi, bankMax, base, tmp;
    gabi::call(0x020066C0, lo.get(), v16);
    gabi::call(0x020066C0, hi.get(), camParamVal(style, 17));
    f32 v28 = camParamVal(style, 28);
    f32 v29 = camParamVal(style, 29);
    f32 rate = camParamVal(style, 21);  /* Val21: radius, latitude and fovy rate */
    f32 urate = camParamVal(style, 20);     /* Val20: longitude rate */
    f32 v25 = camParamVal(style, 25);
    gabi::call(0x020066C0, bankMax.get(), v29);
    gabi::Local<cXyz> ofs;
    ofs->y = v5;
    ofs->z = v0;
    ofs->x = 0.0f;
    cSAngle_ct(base);
    f32 w = gabi::call<f32>(0x025028A0, (f32)mMonitor.field_0x0C.y / v14, 0.0f, 1.0f); /* limitf */
    {
        gabi::Local<be<s16>> nm;
        *nm = 0x1BB;
        fopAc_ac_c* ship = fopAcIt_Judge(0x025E121C, nm.get()); /* fpcSch_JudgeForPName */
        p->m08 = ship;
        if (ship == nullptr) {
            gabi::Local<be<s16>> nm2;
            *nm2 = 0x11B;
            p->m08 = fopAcIt_Judge(0x025E121C, nm2.get());
        }
    }
    {
        gabi::Local<be<s16>> nm;
        *nm = 0xA5;
        fopAc_ac_c* tor = fopAcIt_Judge(0x025E121C, nm.get());
        p->m04 = tor;
        if (tor == nullptr) {
            gabi::Local<cSAngle_l> dir;
            directionOf(dir, mpPlayerActor.get());
            gabi::call(0x02006638, base.get(), dir.get()); /* Val(const cSAngle&) */
        } else if (p->m08.get() != nullptr) {
            gabi::Local<cXyz> a, b, d;
            gabi::Local<cSGlobe_l> g;
            gabi::Local<cSAngle_l> dir, t56, t54, t52;
            positionOf(a, p->m08.get());
            positionOf(b, p->m04.get());
            cXyz_mi(a, d, b);
            gabi::call(0x02007324, g.get(), d.get());
            f32 q = gabi::call<f32>(0x025028A0, g->mRadius / 15000.0f, 0.0f, 1.0f); /* limitf */
            w = 1.0f - q;
            directionOf(dir, p->m04.get());
            eSub(dir, t56, &g->mU);
            eMul(t56, t54, q * 0.5f);
            eAdd(&g->mU, t52, t54);
            *base = (s16)*t52;
        } else {
            gabi::Local<cSAngle_l> dir;
            directionOf(dir, tor);
            gabi::call(0x02006638, base.get(), dir.get());
        }
    }
    if (m11C == 0) {
        p->m00 = 0x54524E44; /* 'TRND' */
        p->m20.copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
        p->m10 = 0;
        p->m0C = (u32)m07C;
        bool side = (eStatus0(mPadId) & 0x1000000) || (eStatus1(mPadId) & 2);
        if (side || (eStatus1(mPadId) & 0x80)) {
            gabi::Local<be<s16>> nm;
            p->m2C.copy(mViewCache.mCenter);
            *nm = 0xA5;
            p->m04 = fopAcIt_Judge(0x025E121C, nm.get());
            if (side) tornadoSideEye(this, p, base, 0x1046EFC0, 0x1046EED8, false);
            else tornadoSideEye(this, p, base, 0x1046EFC4, 0x1046EEF0, true);
        } else if (eStatus0(mPadId) & 0x1010000) {
            gabi::Local<be<s16>> nm;
            p->m2C.copy(mViewCache.mCenter);
            *nm = 0xA5;
            p->m04 = fopAcIt_Judge(0x025E121C, nm.get());
        } else {
            p->m04 = nullptr;
        }
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        if (mEventFlags & 0x8000) p->m10 = 2;
    }
    if (!(eStatus0(mPadId) & 0x1010000)) p->m04 = nullptr;
    f32 radius = gabi::fmadds(v13 * w, w, v10);
    f32 lat = gabi::fmadds(v18 * w, w, v15);
    f32 fovy = gabi::fmadds(v28, w, v25);
    gabi::Local<cSGlobe_l> g;
    gabi::call(0x02007100, g.get());
    if (p->m04.get() != nullptr) {
        gabi::Local<cSAngle_l> a;
        cSAngle_ct(a);
        bool s1 = (eStatus1(mPadId) & 2) != 0;
        u32 tor = gabi::ea(p->m04.get());
        f32 k = w;
        if (s1) {
            cSAngle_Val(a, gabi::load<s16>(tor + 0x680));
            k = 1.0f;
        } else {
            cSAngle_Val(a, gabi::load<s16>(tor + 0x64C));
        }
        f32 s = gabi::call<f32>(0x02006814, a.get()); /* Sin */
        f32 t = v1 * s;
        ofs->x = gabi::fnmsubs(t, k, ofs->x);
        ofs->y = gabi::fmadds(v8, k, ofs->y);
        w = k;
        f32 bx = gabi::call<f32>(0x024F6F9C, (f32)mStickCPosXLast, 1.0f); /* dCamMath::rationalBezierRatio */
        f32 cy = mStickCPosYLast;
        f32 by = 0.0f;
        if (!(cy < 0.0f)) by = gabi::call<f32>(0x024F6F9C, cy, 1.0f);
        f32 m20 = p->m20.x;
        f32 n20 = gabi::fmadds(gabi::fmsubs(bx, -200.0f, m20), 0.25f, m20);
        f32 m24 = p->m20.y;
        p->m20.x = n20;
        p->m20.y = gabi::fmadds(gabi::fmsubs(by, 500.0f, m24), 0.25f, m24);
        ofs->x = ofs->x + n20;
        ofs->y = ofs->y + p->m20.y;
        if (gabi::load<u16>(entry + 0x80) & 4) {
            gabi::Local<cSGlobe_l> g2;
            gabi::Local<cXyz> pos, x, sp;
            gabi::call(0x02007248, g2.get(), 200.0f, gabi::at<cSAngle_l>(0x101FF354), base.get());
            positionOf(pos, p->m04.get());
            gabi::call(0x020073AC, g2.get(), x.get());
            cXyz_pl(pos, sp, x);
            if (daSea_ChkArea(sp->x, sp->z)) {
                gabi::Local<cXyz> pos2, d;
                sp->y = daSea_calcWave(sp->x, sp->z);
                positionOf(pos2, p->m04.get());
                cXyz_mi(sp, d, pos2);
                cSGlobe_Val(g2, d);
                f32 dg = eDegree(&g2->mV);
                lat = gabi::fmadds(dg * w, 0.8f, lat);
            }
        }
        s16 inv = cSAngle_Inv(base);
        gabi::call(0x02006FE4, g.get(), radius, (s16)gabi::ftoi(lat * 182.04445f), inv); /* Val(f32, s16, s16) */
    } else {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(dir);
        gabi::call(0x02006FE4, g.get(), radius, (s16)gabi::ftoi(lat * 182.04445f), inv);
    }
    {
        gabi::Local<cXyz> r;
        relationalPos(r, mpPlayerActor.get(), ofs);
        p->m2C.copy(*r);
    }
    if (m100 == 0) {
        gabi::Local<cXyz> d, s, x, e;
        gabi::Local<cSAngle_l> t10, t12, t14;
        u32 mode = (u32)p->m10;
        s32 end;
        f32 k;
        if (mode == 1) {
            k = 1.0f / (f32)(s32)(40 - (s32)m11C);
            cXyz_mi(&p->m2C, d, &mViewCache.mCenter);
            cXyz_ml(d, s, k);
            PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
            mViewCache.mEye.copy(p->m14);
            cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
            cSGlobe_Val(&mViewCache.mDirection, d);
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
            if (m11C != 39) return true;
        } else {
            if (mode == 2) {
                end = 8;
                k = 1.0f / (f32)(s32)(8 - (s32)m11C);
                cXyz_mi(&p->m2C, d, &mViewCache.mCenter);
                gabi::Local<cXyz> mul;
                mul->x = v3;
                mul->y = v4;
                mul->z = v3;
                gabi::call(0x0201AF1C, d.get(), s.get(), mul.get()); /* cXyz * cXyz */
                PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
                eSub(&g->mU, t14, &mViewCache.mDirection.mU);
                eMul(t14, t12, k);
                eAdd(&mViewCache.mDirection.mU, t10, t12);
                mViewCache.mDirection.mU = angCtE(tmp, *t10);
            } else {
                end = 20;
                k = 1.0f / (f32)(s32)(20 - (s32)m11C);
                cXyz_mi(&p->m2C, d, &mViewCache.mCenter);
                cXyz_ml(d, s, k);
                PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
                f32 rr = mViewCache.mDirection.mRadius;
                mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
                eSub(&g->mV, t10, &mViewCache.mDirection.mV);
                eMul(t10, t12, k);
                eAdd(&mViewCache.mDirection.mV, t14, t12);
                mViewCache.mDirection.mV = angCtE(tmp, *t14);
                eSub(&g->mU, t10, &mViewCache.mDirection.mU);
                eMul(t10, t12, k);
                eAdd(&mViewCache.mDirection.mU, t14, t12);
                mViewCache.mDirection.mU = angCtE(tmp, *t14);
            }
            gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
            cXyz_pl(&mViewCache.mCenter, e, x);
            mViewCache.mEye.copy(*e);
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
            if ((u32)m11C != (u32)(end - 1)) return true;
        }
        m102 = 1;
        m101 = 1;
        m100 = 1;
        return true;
    }
    {
        gabi::Local<cXyz> d, s, mul;
        mul->x = v3;
        mul->y = v4;
        mul->z = v3;
        cXyz_mi(&p->m2C, d, &mViewCache.mCenter);
        gabi::call(0x0201AF1C, d.get(), s.get(), mul.get());
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    }
    gabi::Local<cSAngle_l> t0a, t0c, t0e, u4e, vNew, dir, t9a;
    f32 rr = mViewCache.mDirection.mRadius;
    f32 R = gabi::fmadds(g->mRadius - rr, rate, rr);
    directionOf(dir, mpPlayerActor.get());
    {
        s16 inv = cSAngle_Inv(dir);
        gabi::call(0x020065EC, t9a.get(), inv, &mViewCache.mDirection.mU);
    }
    eSub(&g->mU, t0c, &mViewCache.mDirection.mU);
    eMul(t0c, t0a, urate);
    eAdd(&mViewCache.mDirection.mU, u4e, t0a);
    gabi::Local<cSGlobe_l> gd8;
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        gabi::call(0x02007324, gd8.get(), d.get());
    }
    f32 blend = 1.0f;
    if (gabi::load<u16>(entry + 0x80) & 4) {
        if (w > 0.5f) blend = 0.0f;
        else {
            f32 h = 0.5f - w;
            blend = h + h;
        }
    }
    eSub(&gd8->mV, t0e, &g->mV);
    eMul(t0e, t0a, 0.25f * blend);
    eAdd(&g->mV, t0c, t0a);
    g->mV = angCtE(tmp, *t0c);
    eSub(&g->mV, t0c, &mViewCache.mDirection.mV);
    eMul(t0c, t0e, rate);
    eAdd(&mViewCache.mDirection.mV, vNew, t0e);
    if ((s16)*vNew < (s16)*lo) gabi::call(0x02006638, vNew.get(), lo.get());
    else if ((s16)*vNew > (s16)*hi) gabi::call(0x02006638, vNew.get(), hi.get());
    gabi::call(0x020071E0, &mViewCache.mDirection, R, vNew.get(), u4e.get()); /* Val(f32, V, U) */
    {
        gabi::Local<cXyz> x, e;
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        f32 fv = mViewCache.mFovy;
        mViewCache.mEye.copy(*e);
        mViewCache.mFovy = gabi::fmadds(fovy - fv, rate, fv);
    }
    eMul(bankMax, t0a, mStickMainPosXLast);
    eSub(t0a, t0c, &mViewCache.mBank);
    eMul(t0c, t0e, 0.01f * __builtin_fabsf(w));
    gabi::call(0x020068CC, &mViewCache.mBank, t0e.get()); /* += */
    mEventFlags = mEventFlags | 0x400;
    return true;
}
VERIFY(0x025121E8, &dCamera_c::tornadoCamera);

/* work area of manualCamera (mWork) */
struct Manual_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ cXyz m04;       /* lock-on centre offset */
    /* 0x10 */ cXyz m10;       /* centre target */
    /* 0x1C */ be<f32> m1C;    /* lock-on blend */
    /* 0x20 */ be<f32> m20;    /* centre height */
    /* 0x24 */ u8 _24[4];
    /* 0x28 */ be<u8> m28;     /* charging at start */
    /* 0x29 */ u8 _29[3];
    /* 0x2C */ be<f32> m2C;
    /* 0x30 */ cSGlobe_l m30;  /* manual direction */
    /* 0x38 */ be<f32> m38;    /* centre follow ratio xz */
    /* 0x3C */ be<f32> m3C;    /* centre follow ratio y */
    /* 0x40 */ be<s32> m40;
};

static inline bool rangeAdd(void* p, f32 add, f32 lo, f32 hi) { return gabi::call<bool>(0x024F7DF4, p, add, lo, hi); } /* limited_range_addition */

/* 0250FDC8 (GameCube: not decompiled). Free camera driven by the C stick (radius, latitude and
 * fovy within the style's ranges, yaw by a curved stick response with the HD camera-x option);
 * the centre follows the player (or the lock-on target with wall checks) with an adjustable
 * height; crawling, hanging, deku leaf and seagull states adjust the ranges. */
bool dCamera_c::manualCamera(s32 style) {
    WWHD_FUNC(0x0250FDC8, bool, this, style);
    Manual_l* p = (Manual_l*)mWork;
    f32 v7 = camParamVal(style, 7);
    f32 v27 = camParamVal(style, 27);
    f32 v9 = camParamVal(style, 9);
    f32 v24 = camParamVal(style, 24);
    f32 v0 = camParamVal(style, 0);
    f32 v3 = camParamVal(style, 3);
    f32 v12 = camParamVal(style, 12);
    f32 v29 = camParamVal(style, 29);
    f32 v11 = camParamVal(style, 11);
    f32 v19 = camParamVal(style, 19);
    f32 v16 = camParamVal(style, 16);
    f32 v6 = camParamVal(style, 6);
    f32 v26 = camParamVal(style, 26);
    f32 v14 = camParamVal(style, 14);
    f32 v21 = camParamVal(style, 21);
    f32 v20 = camParamVal(style, 20);
    f32 v20t = v20; /* table value (the override below changes v20) */
    f32 v17 = camParamVal(style, 17);
    f32 v4 = camParamVal(style, 4);
    f32 v1 = camParamVal(style, 1);
    if (m11C == 0) {
        gabi::Local<cXyz> a, d;
        m100 = 1;
        m101 = 1;
        m102 = 1;
        if ((mEventFlags & 0x1000) && mpLockonTarget.get() != nullptr) p->m1C = 1.0f;
        else p->m1C = 0.0f;
        attentionPos(a, mpPlayerActor.get());
        p->m20 = mViewCache.mCenter.y - a->y;
        p->m2C = v11;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        p->m28 = (eStatus1(mPadId) >> 17) & 1;
        cpfE(&p->m30.mRadius, &mViewCache.mDirection.mRadius);
        cpfE(&p->m30.mV, &mViewCache.mDirection.mV); /* V and U */
        p->m04.copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
        p->m40 = 0;
    }
    if (eStatus0(mPadId) & 0x8000000) { /* CRAWL */
        if (v16 < 4.0f) v16 = 4.0f;
        if (v6 < -10.0f) v6 = -10.0f;
    } else if ((eStatus0(mPadId) & 0x2800100) || (eStatus1(mPadId) & 0x10020) || gabi::load<u8>(0x101BDC2D) != 0 ||
               gabi::load<u8>(0x101D5F3E) != 0) {
        if (eStatus0(mPadId) & 0x2000000) {
            v17 = 70.0f;
            v16 = -58.0f;
        } else {
            v17 = 70.0f;
            v16 = -70.0f;
        }
        v19 = 4.0f;
        v3 = 0.45f;
        if (v12 > 500.0f) v12 = 500.0f;
    } else if (eStatus1(mPadId) & 0x40000) {
        v16 = -10.0f;
        v17 = 10.0f;
    }
    if (p->m28 == 0 && (eStatus1(mPadId) & 0x20000)) {
        m144 = 1;
        mEventFlags = mEventFlags | 0x4000000;
    }
    if ((eStatus0(mPadId) & 0x800000) && m784 != 0) {
        v7 = 0.0f;
        v29 = 1.0f;
        v21 = 0.2f;
        v26 = 55.0f;
        v27 = 55.0f;
        v3 = 0.75f;
        v4 = 0.25f;
        v20 = 0.33f;
        v6 = 0.0f;
        v24 = 6.0f;
        v19 = 6.0f;
        v16 = 60.0f;
        v17 = -60.0f;
        v1 = 0.0f;
        v0 = 1.0f;
        v11 = 300.0f;
        v12 = 300.0f;
        v9 = 0.0f;
        v14 = 1.0f;
    }
    /* C stick */
    f32 sign, ax;
    f32 cy = mStickCPosYLast;
    if (!(cy < 0.75f)) {
        ax = __builtin_fabsf(mStickCPosXLast);
        sign = 1.0f;
    } else if (!(cy > -0.75f)) {
        ax = __builtin_fabsf(mStickCPosXLast);
        sign = -1.0f;
    } else {
        sign = gabi::call<f32>(0x024F6F9C, cy * 1.333333f, 2.0f); /* dCamMath::rationalBezierRatio */
        ax = __builtin_fabsf(mStickCPosXLast);
    }
    f32 b = gabi::call<f32>(0x024F6F9C, ax, 0.13f);
    f32 b87 = gabi::call<f32>(0x024F6F9C, 0.87f, 0.13f);
    f32 turn = b * b;
    f32 cx = mStickCPosXLast;
    if (ax < 0.87f) turn = (ax / 0.87f) * (b87 * b87);
    if (!(cx > 0.0f)) turn = -turn;
    {
        u32 m154 = ea() + 0x154;
        if (ax < 0.01f) {
            gabi::call(0x0200F5C8, m154, turn, 0.25f); /* cLib_chaseF */
        } else {
            f32 step = 0.1f;
            if (gabi::load<f32>(m154) * turn < 0.0f) step = 0.18f;
            gabi::call(0x0200F5C8, m154, turn, step);
        }
        ax = gabi::load<f32>(m154) * 0.92f;
        u32 opt = gabi::call<u32>(0x027200D0, gabi::load<u32>(0x101F84DC) + 0x12C0); /* HD: save options */
        if (gabi::load<u8>(opt + 1) == 1) ax = -ax; /* camera x inverted */
    }
    f32 rH = v21, rR = v21, rV = v21, rU = v21;
    if (((eStatus0(mPadId) & 0x6800061) || (eStatus1(mPadId) & 0x10000)) && mDMCSystem.field_0x0 == 0) setDMCAngle();
    {
        gabi::Local<be<f32>> h;
        cpfE(h.get(), &p->m20);
        if (!rangeAdd(h.get(), -(sign * v9), v6, v7)) rH = v20;
        if ((mEventFlags & 0x1000) && mpLockonTarget.get() != nullptr && (m784 != 0 || m785 != 0)) {
            *h = -50.0f;
            f32 m20 = p->m20;
            p->m20 = gabi::fmadds(-50.0f - m20, rH, m20);
        } else {
            f32 m20 = p->m20;
            p->m20 = gabi::fmadds(*h - m20, rH, m20);
        }
    }
    if (eStatus0(mPadId) & 0x100) {
        if (v0 > -10.0f) v0 = -10.0f;
        if (p->m20 < 30.0f) p->m20 = 30.0f;
    }
    gabi::Local<cXyz> ofs;
    ofs->x = v1;
    cpfE(&ofs->y, &p->m20);
    ofs->z = v0;
    if ((mEventFlags & 0x1000) && mpLockonTarget.get() != nullptr) {
        gabi::Local<cXyz> r, rp, d, t, s;
        gabi::Local<u8[0x6C]> lc1, lc2;
        gabi::call(0x02508804, this, r.get(), mpPlayerActor.get(), mpLockonTarget.get(), ofs.get(), 0.5f); /* relationalPos(a1, a2, ofs, 0.5) */
        p->m10.copy(*r);
        relationalPos(rp, mpPlayerActor.get(), ofs);
        u32 o1 = gabi::ea(lc1.get()), o2 = gabi::ea(lc2.get());
        camLinChk_ct_e2(o1);
        camLinChk_ct_e2(o2);
        bool blocked = false;
        if (lineBGCheck(rp, &p->m10, (u8*)lc1.get(), 0x7F) && lineBGCheckBoth(&mEye, rp, (u8*)lc2.get(), 0x7F)) {
            u32 plane = gabi::call<u32>(0x020084C8, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(o2 + 0x16), (u32)gabi::load<u16>(o2 + 0x14)); /* GetTriPla */
            blocked = true;
            if (plane != 0) {
                f32 dot = PSVECDotProduct(gabi::at<cXyz>(plane), &mEye);
                if (dot + gabi::load<f32>(plane + 0xC) < 0.0f) mEventFlags = mEventFlags | 0x80000000; /* eye behind the wall */
            }
        }
        if (!blocked) {
            f32 w = getWaterSurfaceHeight(&p->m10);
            if (w > p->m10.y) p->m10.y = w;
            cXyz_mi(&p->m10, d, rp);
            p->m04.copy(*d);
            f32 bl = p->m1C;
            if (bl < 1.0f) {
                f32 nb = bl + 0.05f;
                p->m1C = nb;
                cXyz_ml(&p->m04, t, nb);
                cXyz_pl(rp, s, t);
                p->m10.copy(*s);
            } else if (bl > 1.0f) {
                p->m1C = 1.0f;
            }
        }
        camLinChk_dt_e2(o2);
        camLinChk_dt_e2(o1);
    } else {
        gabi::Local<cXyz> r, t, s;
        relationalPos(r, mpPlayerActor.get(), ofs);
        p->m10.copy(*r);
        f32 w = getWaterSurfaceHeight(&p->m10);
        f32 bl = p->m1C;
        if (w > p->m10.y) p->m10.y = w;
        if (bl > 0.0f) {
            f32 nb = bl - 0.05f;
            p->m1C = nb;
            cXyz_ml(&p->m04, t, nb);
            cXyz_pl(&p->m10, s, t);
            p->m10.copy(*s);
        } else if (bl < 0.0f) {
            p->m1C = 0.0f;
        }
    }
    {
        gabi::Local<cXyz> d, s, mul;
        f32 mx, my;
        if (m11C == 0) {
            cXyz_mi(&p->m10, d, &mViewCache.mCenter);
            f32 l = std_sqrtf(PSVECSquareMag(d));
            f32 k = (l > 100.0f) ? 0.0f : 1.0f - l / 100.0f;
            mx = v3 * k;
            my = v4 * k;
        } else {
            f32 a = p->m38, c = p->m3C;
            mx = gabi::fmadds(v3 - a, 0.05f, a);
            my = gabi::fmadds(v4 - c, 0.05f, c);
        }
        p->m38 = mx;
        p->m3C = my;
        mul->x = mx;
        mul->z = mx;
        mul->y = my;
        cXyz_mi(&p->m10, d, &mViewCache.mCenter);
        gabi::call(0x0201AF1C, d.get(), s.get(), mul.get()); /* cXyz * cXyz */
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    }
    if (mpLockonTarget.get() == nullptr && m780 == 0) {
        gabi::Local<cXyz> a;
        gabi::Local<u8[0x6C]> lc;
        attentionPos(a, mpPlayerActor.get());
        if (eStatus0(mPadId) & 0x8100100) {
            gabi::Local<cXyz> ep;
            eyePos(ep, mpPlayerActor.get());
            a->y = ep->y + 30.0f;
        } else {
            a->y = a->y - 15.0f;
        }
        u32 o = gabi::ea(lc.get());
        camLinChk_ct_e2(o);
        wallCheckE2(this, a, o);
        camLinChk_dt_e2(o);
    }
    gabi::Local<be<f32>> R, lat, fv;
    cpfE(R.get(), &p->m30.mRadius);
    if (!rangeAdd(R.get(), -(sign * v14), v11, v12)) rR = v20;
    *lat = gabi::call<f32>(0x02006720, &p->m30.mV); /* Degree */
    if (!rangeAdd(lat.get(), -(sign * v19), v16, v17)) rV = v20;
    {
        f32 du = gabi::call<f32>(0x02006720, &p->m30.mU);
        f32 nu = gabi::fmadds(ax, v24, du);
        s16 sv = (s16)gabi::ftoi(*lat * 182.04445f);
        s16 su = (s16)gabi::ftoi(nu * 182.04445f);
        gabi::call(0x02006FE4, &p->m30, (f32)*R, sv, su); /* Val(f32, s16, s16) */
    }
    if ((mEventFlags & 0x1000) && mpLockonTarget.get() != nullptr) mEventFlags = mEventFlags | 0x2000;
    {
        gabi::Local<cSAngle_l> t34, t32, t30, tmp;
        gabi::Local<cXyz> x, e;
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(p->m30.mRadius - rr, rR, rr);
        eSub(&p->m30.mV, t34, &mViewCache.mDirection.mV);
        eMul(t34, t32, rV);
        eAdd(&mViewCache.mDirection.mV, t30, t32);
        mViewCache.mDirection.mV = angCtE(tmp, *t30);
        eSub(&p->m30.mU, t30, &mViewCache.mDirection.mU);
        eMul(t30, t32, rU);
        eAdd(&mViewCache.mDirection.mU, t34, t32);
        mViewCache.mDirection.mU = angCtE(tmp, *t34);
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
    }
    f32 rF = rH;
    cpfE(fv.get(), &mViewCache.mFovy);
    if (!rangeAdd(fv.get(), -(sign * v29), v26, v27)) rF = v20t;
    f32 cur = mViewCache.mFovy;
    f32 q = (*R - v11) / (v12 - v11);
    mViewCache.mFovy = gabi::fmadds(*fv - cur, rF, cur);
    f32 w = 1.0f - q;
    if (eStatus1(mPadId) & 0x40) { /* DEKU_LEAF_FAN */
        f32 r = cM_rndFX(gabi::load<f32>(ea() + 0x7BC) * w);
        mViewCache.mFovy = mViewCache.mFovy + r;
    }
    if (eStatus1(mPadId) & 0x20) { /* deku leaf flying */
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        pos->y = pos->y + 10.0f;
        f32 foot = footHeightOf(mpPlayerActor.get());
        f32 h = foot - groundHeight(pos);
        u32 c7c = m07C;
        f32 bank = mStickMainPosXLast;
        f32 ratio = 0.05f;
        if (h < 200.0f) {
            f32 qq = h / 200.0f;
            ratio = gabi::fnmsubs(0.95f, qq, 1.0f);
            bank = bank * qq;
        }
        gabi::Local<cSAngle_l> t36, t3a, t38;
        cSAngle_l* sp = gabi::call<cSAngle_l*>(0x0200658C, t36.get(), (s16)(c7c << 7));
        f32 sn = gabi::call<f32>(0x02006814, sp); /* Sin */
        mViewCache.mFovy = gabi::fmadds(gabi::load<f32>(ea() + 0x7C0) * sn, w, mViewCache.mFovy);
        f32 fan = gabi::call<f32>(0x024F78F0, ea() + 0x73C); /* dCamSetup_c::FanBank */
        gabi::call(0x020066C0, t3a.get(), (fan * bank) * w);
        eSub(t3a, t38, &mViewCache.mBank);
        eMul(t38, t36, ratio);
        gabi::call(0x020068CC, &mViewCache.mBank, t36.get()); /* += */
        mEventFlags = mEventFlags | 0x400;
    }
    return true;
}
VERIFY(0x0250FDC8, &dCamera_c::manualCamera);

/* work area of towerCamera (mWork) */
struct Tower_l {
    /* 0x00 */ be<u32> m00;      /* 'TOWR' */
    /* 0x04 */ be<s32> m04;      /* transition frames */
    /* 0x08 */ be<f32> m08;      /* remaining weight sum */
    /* 0x0C */ be<f32> m0C;
    /* 0x10 */ be<s32> m10;      /* frames since free movement */
    /* 0x14 */ be<s32> m14;
    /* 0x18 */ be<f32> m18;
    /* 0x1C */ be<f32> m1C;      /* min radius */
    /* 0x20 */ be<f32> m20;      /* max radius */
    /* 0x24 */ be<f32> m24;      /* latitude (deg) */
    /* 0x28 */ cSAngle_l m28;    /* longitude */
    /* 0x2A */ u8 _2A[2];
    /* 0x2C */ be<f32> m2C;
    /* 0x30 */ be<f32> m30;      /* centre offset x */
    /* 0x34 */ be<s32> m34;
    /* 0x38 */ be<f32> m38;
    /* 0x3C */ be<f32> m3C;
    /* 0x40 */ cXyz m40;         /* smoothed centre */
    /* 0x4C */ cXyz m4C;         /* tower (map arrow) position */
    /* 0x58 */ be<s32> m58;
    /* 0x5C */ be<u8> m5C;
    /* 0x5D */ be<u8> m5D;
    /* 0x5E */ u8 _5E[2];
    /* 0x60 */ be<f32> m60;
    /* 0x64 */ be<f32> m64;
    /* 0x68 */ be<f32> m68;
    /* 0x6C */ be<f32> m6C;
    /* 0x70 */ be<f32> m70;
    /* 0x74 */ be<f32> m74;
};

/* 0250B30C (GameCube: not decompiled). Tower camera: a follow camera that keeps the map arrow (the
 * tower) in view: the yaw eases between the param longitude range around the tower direction,
 * scaled by the stick; without an arrow followCamera runs first. */
bool dCamera_c::towerCamera(s32 style) {
    WWHD_FUNC(0x0250B30C, bool, this, style);
    Tower_l* p = (Tower_l*)mWork;
    f32 f31 = 0.9f;
    gabi::Local<cSAngle_l> chargeLat, lim80, lo, hi, a23, a24, tmp;
    gabi::call(0x020066C0, chargeLat.get(), (f32)gabi::load<f32>(ea() + 0x7E8));
    gabi::call(0x020066C0, lim80.get(), 80.0f);
    f32 v11 = camParamVal(style, 11);
    f32 v10 = camParamVal(style, 10);
    f32 v4 = camParamVal(style, 4);
    f32 v14 = camParamVal(style, 14);
    f32 v5 = camParamVal(style, 5);
    f32 v3 = camParamVal(style, 3);
    f32 v6 = camParamVal(style, 6);
    f32 v1 = camParamVal(style, 1);
    f32 v15 = camParamVal(style, 15);
    f32 v0 = camParamVal(style, 0);
    f32 v13 = camParamVal(style, 13);
    gabi::call(0x020066C0, lo.get(), camParamVal(style, 16));
    gabi::call(0x020066C0, hi.get(), camParamVal(style, 17));
    f32 v25 = camParamVal(style, 25);
    f32 v27 = camParamVal(style, 27);
    f32 v29 = camParamVal(style, 29);
    f32 v19 = camParamVal(style, 19);
    gabi::call(0x020066C0, a23.get(), camParamVal(style, 23));
    gabi::call(0x020066C0, a24.get(), camParamVal(style, 24));
    f32 v20 = camParamVal(style, 20);
    f32 v21 = camParamVal(style, 21);
    (void)dComIfGp_ea(); /* dComIfGp_getAttention() (unused) */
    if (mCurArrowIdx == 0xFF) followCamera(style);
    if (gabi::load<u8>(0x101BDC2D) != 0 || gabi::load<u8>(0x101D5F3E) != 0) {
        f31 = 0.66f;
        if (v15 < 20.0f) v15 = 20.0f;
        gabi::call(0x020069A0, &m148, 0.33f); /* m148 *= 0.33f */
    }
    if (eStatus0(mPadId) & 0x8100000) { /* CRAWL | SWIM */
        gabi::Local<cSAngle_l> a;
        cSAngle_l* ap = gabi::call<cSAngle_l*>(0x020066C0, a.get(), 4.0f);
        if ((s16)*lo < (s16)*ap) gabi::call(0x02006694, lo.get(), 4.0f);
        if (v5 < -10.0f) v5 = -10.0f;
    }
    if (eStatus1(mPadId) & 0x40000) {
        v15 = 0.0f;
        m148 = gabi::load<s16>(0x101FF354);
    }
    if (eStatus0(mPadId) & 0x20) v1 = -v1;
    if (m108 == 0) {
        p->m5D = 0;
        p->m30 = 0.0f;
    }
    if (((mEventFlags & 0x100000) && (eStatus0(mPadId) & 0xA5000)) || gabi::load<u8>(0x101BDC2D) != 0 ||
        gabi::load<u8>(0x101D5F3E) != 0) {
        f32 tgt;
        if (mStickMainPosXLast < -0.2f) p->m5D = 1;
        if (mStickMainPosXLast > 0.2f) {
            p->m5D = 0;
            tgt = 45.0f;
        } else if (p->m5D != 0) {
            tgt = -45.0f;
        } else {
            tgt = 45.0f;
        }
        f32 c = p->m30;
        p->m30 = gabi::fmadds(tgt - c, 0.04f, c);
    } else {
        f32 c = p->m30;
        p->m30 = gabi::fmadds(v1 - c, 0.06f, c);
    }
    gabi::Local<cXyz> ofs;
    {
        f32 x = p->m30;
        f32 t = gabi::call<f32>(0x025028A0, (mViewCache.mDirection.mRadius - v11) / (v10 - v11), 0.0f, 1.0f); /* limitf */
        ofs->z = v0 * t;
        ofs->x = x;
        ofs->y = gabi::fmadds(v5 - v6, t, v6);
    }
    f32 range = v10 - v11;
    if (m108 == 0) {
        gabi::Local<cXyz> d;
        p->m00 = 0x544F5752; /* 'TOWR' */
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        p->m18 = 0.0f;
        cpfE(&p->m20, &mDirection.mRadius);
        p->m14 = 0;
        p->m10 = 0;
        cpfE(&p->m1C, &mDirection.mRadius);
        f32 deg = eDegree(&mDirection.mV);
        p->m3C = deg;
        p->m24 = deg;
        p->m40.copy(mViewCache.mCenter);
        p->m5C = 1;
        p->m68 = 0.01f;
        p->m64 = 0.01f;
        p->m6C = 0.01f;
        p->m60 = 0.75f;
        p->m70 = v3;
        p->m74 = v4;
        p->m34 = 0;
        cpfE(&p->m2C, &mFovy);
        p->m38 = 0.0f;
        p->m28 = (s16)mViewCache.mDirection.mU;
        cpfE(&p->m4C.x, gabi::at<u32>(ea() + 0x5D8)); /* mCurRoomArrowEntry.position */
        cpfE(&p->m4C.y, gabi::at<u32>(ea() + 0x5DC));
        cpfE(&p->m4C.z, gabi::at<u32>(ea() + 0x5E0));
        p->m58 = ((s16)*a23 > gabi::load<s16>(0x101FF358)) ? 0 : 1; /* mfcr: !CR0[GT] (<= _90) */
        if ((mEventFlags & 0x8000) || mCurMode == 1) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
            p->m0C = 0.0f;
            p->m04 = 1;
        } else {
            gabi::Local<cXyz> rel;
            relationalPos(rel, mpPlayerActor.get(), ofs);
            cXyz_mi(&mEye, d, rel);
            f32 l1 = std_sqrtf(PSVECSquareMag(d)) - v10;
            cXyz_mi(&mCenter, d, rel);
            f32 l2 = std_sqrtf(PSVECSquareMag(d)) - v10;
            f32 m = (l2 - l1 >= 0.0f) ? l2 : l1; /* fsel */
            m = __builtin_fabsf(m);
            f32 h = heightOf(mpPlayerActor.get());
            f32 hh = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
            f32 r = std_sqrtf(m / hh);
            s32 k = gabi::ftoi(8.0f * r);
            s32 n = k + 1;
            p->m04 = n;
            p->m08 = (f32)((n * (k + 2)) >> 1);
            p->m0C = 0.0f;
        }
    }
    gabi::Local<cSGlobe_l> g58, g84;
    gabi::Local<cXyz> target;
    {
        gabi::Local<cXyz> a, d, x, b;
        gabi::Local<cSAngle_l> t14;
        attentionPos(a, mpPlayerActor.get());
        cXyz_mi(a, d, &p->m4C);
        gabi::call(0x02007324, g58.get(), d.get());
        gabi::call(0x02007324, g84.get(), ofs.get());
        eAdd(&g84->mU, t14, &g58->mU);
        g84->mU = angCtE(tmp, *t14);
        attentionPos(b, mpPlayerActor.get());
        gabi::call(0x020073AC, g84.get(), x.get());
        cXyz_pl(b, a, x);
        target->copy(*a);
        target->y = getWaterSurfaceHeight(target);
    }
    gabi::Local<u8[0x6C]> lc;
    u32 o = gabi::ea(lc.get());
    if (m100 == 0) {
        gabi::Local<cXyz> d, s, a, x, e;
        gabi::Local<cSAngle_l> vv, ay, t40, t2e, t28;
        gabi::Local<cSGlobe_l> g;
        if (m31D != 0) gabi::call(0x024EFB08, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4, 1, &p->m40, 0, 0); /* MoveBgMatrixCrrPos */
        f32 r = (f32)(s32)((s32)p->m04 - (s32)m108);
        f32 k = r / p->m08;
        p->m0C = r;
        cXyz_mi(target, d, &p->m40);
        cXyz_ml(d, s, k);
        PSVECAdd(&p->m40, s, &p->m40);
        cXyz_mi(&p->m40, s, &mViewCache.mCenter);
        cXyz_ml(s, d, v3);
        PSVECAdd(&mViewCache.mCenter, d, &mViewCache.mCenter);
        attentionPos(a, mpPlayerActor.get());
        a->y = a->y - 15.0f;
        camLinChk_ct_e2(o);
        wallCheckE2(this, a, o);
        f32 R = gabi::call<f32>(0x025028A0, (f32)mViewCache.mDirection.mRadius, v11, v10); /* limitf */
        cSAngle_ct(vv);
        s16 V = mViewCache.mDirection.mV;
        if (V < (s16)*lo) V = *lo;
        if (V > (s16)*hi) V = *hi;
        *vv = V;
        s16 inv = cSAngle_Inv(&mAngleY);
        cSAngle_l* ayp = gabi::call<cSAngle_l*>(0x0200658C, ay.get(), inv);
        gabi::call(0x02007248, g.get(), R, vv.get(), ayp);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
        eSub(&g->mV, t40, &mViewCache.mDirection.mV);
        eMul(t40, t2e, k);
        eAdd(&mViewCache.mDirection.mV, t28, t2e);
        mViewCache.mDirection.mV = angCtE(tmp, *t28);
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        if ((u32)m108 >= (u32)((s32)p->m04 - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
        p->m24 = eDegree(&mViewCache.mDirection.mV);
        cpfE(&p->m20, &mViewCache.mDirection.mRadius);
        cpfE(&p->m1C, &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(v25 - fv, k, fv);
        p->m08 = p->m08 - p->m0C;
        camLinChk_dt_e2(o);
        return true;
    }
    {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        pos->y = pos->y + 10.0f;
        groundHeight(pos);
    }
    if (m360 != 0 || (eStatus0(mPadId) & 0x100000)) {
        p->m18 = 0.0f;
        p->m10 = 0;
    } else if (p->m10 < 0x50) {
        s32 n = p->m10 + 1;
        p->m10 = n;
        f32 old = p->m18;
        f32 b = gabi::call<f32>(0x024F6F9C, (f32)n / 80.0f, 1.25f); /* dCamMath::rationalBezierRatio */
        p->m18 = gabi::fmadds(f31 - old, b, old);
    }
    gabi::Local<cSAngle_l> t14, dir, tA0;
    eSub(&g58->mU, t14, &mViewCache.mDirection.mU);
    s16 zero = gabi::load<s16>(0x101FF354);
    bool gt = (s16)*t14 > zero; /* mfcr: CR0[GT] */
    directionOf(dir, mpPlayerActor.get());
    eSub(dir, tA0, &mViewCache.mDirection.mU);
    {
        gabi::Local<cXyz> mul, d, s, a;
        cpfE(&mul->z, &p->m70);
        cpfE(&mul->x, &p->m70);
        cpfE(&mul->y, &p->m74);
        cXyz_mi(target, d, &mViewCache.mCenter);
        gabi::call(0x0201AF1C, d.get(), s.get(), mul.get()); /* cXyz * cXyz */
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
        attentionPos(a, mpPlayerActor.get());
        a->y = a->y - 15.0f;
        camLinChk_ct_e2(o);
        wallCheckE2(this, a, o);
    }
    gabi::Local<cSGlobe_l> g38;
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        gabi::call(0x02007324, g38.get(), d.get());
    }
    f32 t = (mViewCache.mDirection.mRadius - v11) / range;
    if (t < 0.0f) t = 0.0f;
    else if (t > 1.0f) t = 1.0f;
    gabi::Local<cSAngle_l> yaw, t16, t1c;
    cSAngle_ct(yaw);
    eSub(a23, t16, a24);
    eMul(t16, t14, mStickMainValueLast);
    eAdd(a23, t1c, t14);
    {
        gabi::Local<cSAngle_l> ta, tb, tc, td;
        if (!gt) {
            eSub(&g38->mU, ta, &g58->mU);
            t = gabi::call<f32>(0x024F6F9C, t, v27);
            if ((s16)*ta > (s16)*t1c) t = 1.0f;
            eAdd(&g58->mU, tb, t1c);
        } else {
            eSub(&g58->mU, ta, &g38->mU);
            t = gabi::call<f32>(0x024F6F9C, t, v27);
            if ((s16)*ta > (s16)*t1c) t = 1.0f;
            eSub(&g58->mU, tb, t1c);
        }
        eSub(tb, tc, &g38->mU);
        eMul(tc, td, t);
        eAdd(&g38->mU, ta, td);
        gabi::call(0x02006638, yaw.get(), ta.get()); /* Val(const cSAngle&) */
    }
    eSub(yaw, t14, &p->m28);
    eMul(t14, t16, 0.33f);
    gabi::call(0x020068CC, &p->m28, t16.get()); /* += */
    f32 sv = mStickMainValueLast;
    if ((eStatus0(mPadId) & 0x2000161) || (eStatus1(mPadId) & 0x10000)) sv = 0.0f;
    p->m68 = gabi::fmadds(v20, sv, v21);
    {
        gabi::Local<cSAngle_l> t1e, t16b;
        eSub(&p->m28, t1e, &mViewCache.mDirection.mU);
        eMul(t1e, t14, p->m68);
        eAdd(&mViewCache.mDirection.mU, t16b, t14);
        mViewCache.mDirection.mU = angCtE(tmp, *t16b);
    }
    {
        f32 a = p->m24;
        p->m24 = gabi::fmadds(v15 - a, v14, a);
    }
    if (!(gabi::load<u16>(CAM_STYLES + style * 0x84 + 0x80) & 4)) m148 = zero;
    gabi::Local<cSAngle_l> lat;
    cSAngle_ct(lat);
    if (p->m10 == 0) {
        f32 deg = eDegree(&m148);
        gabi::call(0x02006694, lat.get(), p->m24 + deg);
        f32 c = p->m64;
        p->m64 = gabi::fmadds(v19 - c, 0.01f, c);
    } else {
        *lat = (s16)g38->mV;
        p->m24 = eDegree(lat);
        p->m64 = gabi::call<f32>(0x024F6F9C, (f32)p->m18, 0.7f);
    }
    if ((s16)*lat < (s16)*lo) gabi::call(0x02006638, lat.get(), lo.get());
    else if ((s16)*lat > (s16)*hi) gabi::call(0x02006638, lat.get(), hi.get());
    {
        gabi::Local<cSAngle_l> t16c, t1ec;
        eSub(lat, t14, &mViewCache.mDirection.mV);
        eMul(t14, t16c, p->m64);
        eAdd(&mViewCache.mDirection.mV, t1ec, t16c);
        mViewCache.mDirection.mV = angCtE(tmp, *t1ec);
    }
    {
        gabi::Local<cXyz> x, e;
        f32 a = p->m1C, b = p->m20;
        f32 n1C = gabi::fmadds(v11 - a, v14, a);
        f32 n20 = gabi::fmadds(v10 - b, v14, b);
        f32 r3 = g38->mRadius;
        p->m1C = n1C;
        p->m20 = n20;
        if (r3 < n1C) {
            f32 c = p->m60;
            c = gabi::fmadds(v13 - c, 0.01f, c);
            f32 lim = p->m1C;
            p->m60 = c;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(lim - R, c, R);
        } else if (r3 > n20) {
            f32 c = p->m60;
            c = gabi::fmadds(v13 - c, 0.01f, c);
            f32 lim = p->m20;
            p->m60 = c;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(lim - R, c, R);
        } else {
            p->m60 = 1.0f;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(r3 - R, 1.0f, R);
        }
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        f32 c6 = p->m6C;
        c6 = gabi::fmadds(v29 - c6, 0.01f, c6);
        p->m6C = c6;
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(v25 - fv, c6, fv);
    }
    camLinChk_dt_e2(o);
    return true;
}
VERIFY(0x0250B30C, &dCamera_c::towerCamera);

/* work area of rideCamera (mWork) */
struct Ride_l {
    /* 0x00 */ be<u32> m00;           /* 'RIDE' */
    /* 0x04 */ gptr<fopAc_ac_c> m04;  /* ship (actor 0xA5) */
    /* 0x08 */ be<u32> m08;           /* m07C at start */
    /* 0x0C */ be<u8> m0C;            /* steering left */
    /* 0x0D */ be<u8> m0D;            /* boost blur armed */
    /* 0x0E */ u8 _0E[2];
    /* 0x10 */ be<s32> m10;           /* entry: 0 normal, 1 side eye, 2 event */
    /* 0x14 */ cXyz m14;              /* side eye */
    /* 0x20 */ cXyz m20;
    /* 0x2C */ cXyz m2C;              /* centre target */
    /* 0x38 */ cSAngle_l m38;         /* ship heading */
    /* 0x3A */ u8 _3A[2];
    /* 0x3C */ be<f32> m3C;           /* radius */
    /* 0x40 */ be<f32> m40;           /* radius speed scale */
    /* 0x44 */ be<f32> m44;           /* latitude */
    /* 0x48 */ be<f32> m48;           /* latitude speed scale */
    /* 0x4C */ be<f32> m4C;           /* side offset ratio */
    /* 0x50 */ be<f32> m50;           /* centre follow ratio */
};

/* side eye of a ride entry from a function-local static offset table (initialised on first use) */
static void rideSideEye(dCamera_c* c, Ride_l* p, bool sail) {
    u32 guard = sail ? 0x1046EFBC : 0x1046EFB8;
    u32 table = sail ? 0x1046EF38 : 0x1046EF08;
    if (gabi::load<u32>(guard) == 0) {
        static const f32 kHelm[12] = {-120.0f, 50.0f, 280.0f, 240.0f, 160.0f, -80.0f, 240.0f, 120.0f, -60.0f, 200.0f, 160.0f, -80.0f};
        static const f32 kSail[12] = {320.0f, 200.0f, -150.0f, 320.0f, 200.0f, -120.0f, 320.0f, 80.0f, -150.0f, 115.0f, 215.0f, 315.0f};
        if (!sail) gabi::store<u32>(guard, 1);
        for (int i = 0; i < 12; i++) gabi::store<f32>(table + i * 4, sail ? kSail[i] : kHelm[i]);
        if (sail) gabi::store<u32>(guard, 1);
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> ta, rel, t, tmp;
    gabi::Local<cXyz> pos, pos2, x, e;
    gabi::call(0x02007324, g.get(), gabi::at<cXyz>(table + ((u32)c->m07C & 3) * 0xC));
    cXyz* a = c->positionPntOf(p->m04.get());
    cXyz* b = c->positionPntOf(c->mpPlayerActor.get());
    s16 ty = cLib_targetAngleY(b, a);
    cSAngle_ct(ta, ty);
    eSub(ta, rel, &p->m38);
    if ((s16)*rel < gabi::load<s16>(0x101FF354)) eSub(&p->m38, t, &g->mU);
    else eAdd(&p->m38, t, &g->mU);
    g->mU = angCtE(tmp, *t);
    c->positionOf(pos, p->m04.get());
    c->mViewCache.mCenter.copy(*pos);
    c->positionOf(pos2, p->m04.get());
    gabi::call(0x020073AC, g.get(), x.get());
    cXyz_pl(pos2, e, x);
    p->m14.copy(*e);
    cXyz* pp = c->positionPntOf(c->mpPlayerActor.get());
    if (!gabi::call<bool>(0x024FCBE8, c, &p->m14, pp, 0x7F)) p->m10 = 1; /* lineBGCheck */
}

/* 0250D4E8 (GameCube: not decompiled). Riding the King of Red Lions (status1 4, sail 0x1000000,
 * helm 0x10000): the camera follows the ship's heading with a speed-dependent radius, latitude and
 * fovy (ship speed from the monitor), a side offset while steering, a sea-wave latitude
 * correction, an entry side eye and a boost blur with vibration. */
bool dCamera_c::rideCamera(s32 style) {
    WWHD_FUNC(0x0250D4E8, bool, this, style);
    Ride_l* p = (Ride_l*)mWork;
    u32 entry = CAM_STYLES + style * 0x84;
    f32 v8 = camParamVal(style, 8);
    f32 v15 = camParamVal(style, 15);
    f32 v10 = camParamVal(style, 10);
    f32 v13 = camParamVal(style, 13);
    f32 v5 = camParamVal(style, 5);
    f32 v3 = camParamVal(style, 3);
    f32 v4 = camParamVal(style, 4);
    f32 v18 = camParamVal(style, 18);
    f32 v1 = camParamVal(style, 1);
    f32 v14 = camParamVal(style, 14);
    f32 v0 = camParamVal(style, 0);
    gabi::Local<cSAngle_l> lo, hi, bankMax, tmp;
    gabi::call(0x020066C0, lo.get(), camParamVal(style, 16));
    gabi::call(0x020066C0, hi.get(), camParamVal(style, 17));
    f32 v28 = camParamVal(style, 28);
    f32 v25 = camParamVal(style, 25);
    f32 v21 = camParamVal(style, 21);
    f32 v20 = camParamVal(style, 20);
    gabi::call(0x020066C0, bankMax.get(), camParamVal(style, 29));
    f32 v6 = camParamVal(style, 6);
    f32 v7 = camParamVal(style, 7);
    if (mEventFlags & 0x100000) v20 = 1.0f;
    p->m04 = nullptr;
    gabi::Local<cXyz> ofs;
    ofs->z = v0;
    ofs->x = 0.0f;
    ofs->y = v5;
    if (m11C == 0) p->m0C = 0;
    if (eStatus1(mPadId) & 4) {
        gabi::Local<be<s16>> nm;
        *nm = 0xA5;
        p->m04 = fopAcIt_Judge(0x025E121C, nm.get());
        if (p->m04.get() == nullptr) gabi::call(0x0273AA24, 0x1004AD80, 0x1EE2, 0x1004AD70); /* JUTAssertion (debug) */
        u32 ship = gabi::ea(p->m04.get());
        gabi::Local<cSAngle_l> hd, d1a, c, t48;
        cSAngle_ct(hd, (s16)(gabi::load<s16>(ship + 0x32A) + gabi::load<s16>(ship + 0x67C)));
        cSAngle_ct(d1a);
        if (m11C == 0) {
            *d1a = gabi::load<s16>(0x101FF354);
        } else {
            eSub(&p->m38, t48, hd);
            gabi::call(0x02006638, d1a.get(), t48.get());
        }
        cSAngle_l* cp = gabi::call<cSAngle_l*>(0x020066C0, c.get(), -1.0f);
        if ((s16)*d1a < (s16)*cp) p->m0C = 1;
        cp = gabi::call<cSAngle_l*>(0x020066C0, c.get(), 1.0f);
        if ((s16)*d1a > (s16)*cp) p->m0C = 0;
        p->m38 = (s16)*hd;
    } else if ((eStatus0(mPadId) & 0x1010000) || (eStatus1(mPadId) & 0x80) || (eStatus1(mPadId) & 2)) {
        gabi::Local<be<s16>> nm;
        gabi::Local<cSAngle_l> dir;
        *nm = 0xA5;
        p->m04 = fopAcIt_Judge(0x025E121C, nm.get());
        directionOf(dir, mpPlayerActor.get());
        gabi::call(0x02006638, &p->m38, dir.get());
    } else {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        gabi::call(0x02006638, &p->m38, dir.get());
        p->m04 = nullptr;
    }
    if (m11C == 0) {
        p->m00 = 0x52494445; /* 'RIDE' */
        p->m20.copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
        p->m10 = 0;
        p->m0D = 1;
        p->m08 = (u32)m07C;
        p->m4C = 0.0f;
        cpfE(&p->m3C, &mViewCache.mDirection.mRadius);
        p->m40 = 0.0f;
        f32 deg = eDegree(&mViewCache.mDirection.mV);
        p->m04 = nullptr;
        p->m44 = deg;
        p->m48 = 0.0f;
        p->m50 = 0.2f;
        bool done = false;
        if (eStatus0(mPadId) & 0x1000000) {
            gabi::Local<be<s16>> nm;
            *nm = 0xA5;
            p->m04 = fopAcIt_Judge(0x025E121C, nm.get());
            p->m2C.copy(mViewCache.mCenter);
            rideSideEye(this, p, false);
            done = true;
        } else if (eStatus1(mPadId) & 0x80) {
            gabi::Local<be<s16>> nm;
            *nm = 0xA5;
            p->m04 = fopAcIt_Judge(0x025E121C, nm.get());
            p->m2C.copy(mViewCache.mCenter);
            rideSideEye(this, p, true);
            done = true;
        }
        if (!done) {
            if ((eStatus0(mPadId) & 0x1010000) || (eStatus1(mPadId) & 2)) {
                gabi::Local<be<s16>> nm;
                p->m2C.copy(mViewCache.mCenter);
                *nm = 0xA5;
                p->m04 = fopAcIt_Judge(0x025E121C, nm.get());
            } else {
                p->m2C.copy(mViewCache.mCenter);
                p->m04 = nullptr;
            }
            p->m14.copy(mViewCache.mCenter);
        }
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        if (mEventFlags & 0x8000) p->m10 = 2;
        if (mEventFlags & 0x100000) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
    }
    f32 spd;
    if (eStatus0(mPadId) & 0x1010000) {
        fopAc_ac_c* ship = p->m04.get();
        spd = mMonitor.field_0x0C.y / v14;
        if (ship != nullptr && (gabi::load<u32>(gabi::ea(ship) + 0x644) & 0x80000000) &&
            gabi::load<u8>(dComIfGp_ea() + 0x5D2C) == 1 && mMonitor.field_0x0C.z > 1.9f && (eStatus1(mPadId) & 0x400) &&
            !(eStatus0(mPadId) & 0x1000000) && !(eStatus1(mPadId) & 0x80)) {
            if (p->m0D != 0) {
                ResetBlure(0);
                SetBlureTimer(0x14);
                SetBlureAlpha(0.55f);
                gabi::call(0x0250D4D0, this, 0.95f); /* SetBlureScale */
                gabi::Local<cXyz> v;
                u32 vib = dComIfGp_ea() + 0x599C;
                v->x = 0.0f;
                v->y = 1.0f;
                v->z = 0.0f;
                gabi::call(0x025CB374, vib, 7, 0x20, v.get()); /* dVibration_c::StartShock */
                p->m0D = 0;
            }
        } else if (gabi::load<u8>(dComIfGp_ea() + 0x5D2C) == 0) {
            p->m0D = 1;
        }
    } else {
        p->m04 = nullptr;
        spd = mMonitor.field_0x0C.y / v14;
        if (gabi::load<u8>(dComIfGp_ea() + 0x5D2C) == 0) p->m0D = 1;
    }
    if (spd < -1.0f) spd = -1.0f;
    else if (spd > 1.0f) spd = 1.0f;
    f32 radius, lat, fovy;
    {
        f32 a3C = p->m3C, a40 = p->m40, a44 = p->m44, a48 = p->m48;
        f32 n3C = gabi::fmadds(v10 - a3C, 0.08f, a3C);
        fovy = gabi::fmadds(v28, spd, v25);
        f32 n40 = gabi::fmadds(v13 - a40, 0.08f, a40);
        f32 n48 = gabi::fmadds(v18 - a48, 0.08f, a48);
        f32 n44 = gabi::fmadds(v15 - a44, 0.08f, a44);
        p->m3C = n3C;
        p->m40 = n40;
        p->m44 = n44;
        radius = gabi::fmadds(n40 * spd, spd, n3C);
        p->m48 = n48;
        lat = gabi::fmadds(n48 * spd, spd, n44);
        if (fovy > 80.0f) fovy = 80.0f;
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> side;
    gabi::call(0x02007100, g.get());
    cSAngle_ct(side);
    if (p->m04.get() != nullptr) {
        u32 BE07 = 0x42453037; /* 'BE07' */
        bool s4 = false;
        if (eStatus1(mPadId) & 2) {
            cSAngle_Val(side, gabi::load<s16>(gabi::ea(p->m04.get()) + 0x680));
            f32 c = p->m4C;
            p->m4C = gabi::fmadds(1.0f - c, 0.05f, c);
            f32 s = gabi::call<f32>(0x02006814, side.get()); /* Sin */
            ofs->y = gabi::fmadds(v8, spd, ofs->y);
            ofs->x = gabi::fnmsubs(v1 * s, p->m4C, ofs->x);
            s4 = (eStatus1(mPadId) & 4) != 0;
        } else {
            bool st4 = (eStatus1(mPadId) & 4) != 0;
            u32 ship = gabi::ea(p->m04.get());
            if (st4) {
                gabi::Local<cSAngle_l> dir, t68, t66;
                directionOf(dir, p->m04.get());
                u32 sh = gabi::ea(p->m04.get());
                gabi::call(0x02006908, dir.get(), t68.get(), (s16)(gabi::load<s16>(sh + 0x32A) + gabi::load<s16>(sh + 0x67C))); /* - s16 */
                eAdd(t68, t66, gabi::at<cSAngle_l>(0x101FF358)); /* + _90 */
                gabi::call(0x02006638, side.get(), t66.get());
                f32 c = p->m4C;
                if (p->m0C != 0) p->m4C = gabi::fmadds(-1.0f - c, 0.05f, c);
                else p->m4C = gabi::fmadds(1.0f - c, 0.05f, c);
                if (gabi::load<u32>(CAM_STYLES + (s32)mCurStyle * 0x84) == BE07) {
                    gabi::Local<cSAngle_l> pa;
                    u32 sh2 = gabi::ea(p->m04.get());
                    cSAngle_l* ap = gabi::call<cSAngle_l*>(0x0200658C, pa.get(), (s16)(gabi::load<s16>(sh2 + 0x328) + gabi::load<s16>(sh2 + 0x67E) - 0x4000));
                    f32 dg = gabi::call<f32>(0x02006720, ap); /* Degree */
                    spd = gabi::call<f32>(0x025028A0, -dg / 60.0f, 0.0f, 1.0f); /* limitf */
                }
            } else {
                cSAngle_Val(side, gabi::load<s16>(ship + 0x64C));
                f32 c = p->m4C;
                p->m4C = gabi::fmadds(spd - c, 0.1f, c);
            }
            f32 s = gabi::call<f32>(0x02006814, side.get()); /* Sin */
            ofs->x = gabi::fnmsubs(v1 * s, p->m4C, ofs->x);
            ofs->y = gabi::fmadds(v8, spd, ofs->y);
            s4 = (eStatus1(mPadId) & 4) != 0;
        }
        if (s4) {
            u32 alg = gabi::load<u32>(CAM_STYLES + (s32)mCurStyle * 0x84);
            if (alg == BE07) ofs->y = gabi::fmadds(v7 - v6, spd, v6);
            else if (alg == BE07 + 1) ofs->y = ofs->y + ofs->y; /* 'BE08' */
        }
        if (m314 != 0 && m318 == m354 && (gabi::load<u16>(entry + 0x80) & 4)) {
            gabi::Local<cSGlobe_l> g2;
            gabi::Local<cXyz> pos, x, sp;
            gabi::call(0x02007248, g2.get(), 200.0f, gabi::at<cSAngle_l>(0x101FF354), &p->m38);
            positionOf(pos, p->m04.get());
            gabi::call(0x020073AC, g2.get(), x.get());
            cXyz_pl(pos, sp, x);
            if (daSea_ChkArea(sp->x, sp->z)) {
                gabi::Local<cXyz> pos2, d;
                sp->y = daSea_calcWave(sp->x, sp->z);
                positionOf(pos2, p->m04.get());
                cXyz_mi(sp, d, pos2);
                cSGlobe_Val(g2, d);
                f32 dg = eDegree(&g2->mV);
                lat = gabi::fmadds(dg * spd, 0.8f, lat);
            }
        }
    }
    {
        s16 inv = cSAngle_Inv(&p->m38);
        gabi::call(0x02006FE4, g.get(), radius, (s16)gabi::ftoi(lat * 182.04445f), inv); /* Val(f32, s16, s16) */
        f32 c = p->m50;
        p->m50 = gabi::fmadds(1.0f - c, 0.2f, c);
        gabi::Local<cXyz> r, d, s;
        relationalPos(r, mpPlayerActor.get(), ofs);
        cXyz_mi(r, d, &p->m2C);
        cXyz_ml(d, s, p->m50);
        PSVECAdd(&p->m2C, s, &p->m2C);
    }
    if (m100 == 0) {
        gabi::Local<cXyz> d, s, x, e;
        gabi::Local<cSAngle_l> t22, t20, t1e;
        u32 mode = (u32)p->m10;
        f32 k;
        s32 end;
        if (mode == 1) {
            k = 1.0f / (f32)(s32)(40 - (s32)m11C);
            cXyz_mi(&p->m2C, d, &mViewCache.mCenter);
            cXyz_ml(d, s, k);
            PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
            mViewCache.mEye.copy(p->m14);
            cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
            cSGlobe_Val(&mViewCache.mDirection, d);
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
            if (m11C != 39) return true;
        } else if (mode == 2) {
            end = 1;
            k = 0.0f;
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
            if ((u32)m11C != (u32)(end - 1)) return true;
        } else {
            end = 20;
            k = 1.0f / (f32)(s32)(20 - (s32)m11C);
            cXyz_mi(&p->m2C, d, &mViewCache.mCenter);
            cXyz_ml(d, s, k);
            PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
            f32 rr = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
            eSub(&g->mV, t22, &mViewCache.mDirection.mV);
            eMul(t22, t20, k);
            eAdd(&mViewCache.mDirection.mV, t1e, t20);
            mViewCache.mDirection.mV = angCtE(tmp, *t1e);
            eSub(&g->mU, t1e, &mViewCache.mDirection.mU);
            eMul(t1e, t20, k);
            eAdd(&mViewCache.mDirection.mU, t22, t20);
            mViewCache.mDirection.mU = angCtE(tmp, *t22);
            gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
            cXyz_pl(&mViewCache.mCenter, e, x);
            f32 fv = mViewCache.mFovy;
            mViewCache.mEye.copy(*e);
            mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
            if ((u32)m11C != (u32)(end - 1)) return true;
        }
        m102 = 1;
        m101 = 1;
        m100 = 1;
        return true;
    }
    {
        gabi::Local<cXyz> mul, d, s;
        mul->x = v3;
        mul->y = v4;
        mul->z = v3;
        cXyz_mi(&p->m2C, d, &mViewCache.mCenter);
        gabi::call(0x0201AF1C, d.get(), s.get(), mul.get()); /* cXyz * cXyz */
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    }
    f32 rr0 = mViewCache.mDirection.mRadius;
    f32 R = gabi::fmadds(g->mRadius - rr0, v21, rr0);
    gabi::Local<cSAngle_l> dir, t40, t12, t10, u42, vNew, t14;
    directionOf(dir, mpPlayerActor.get());
    {
        s16 inv = cSAngle_Inv(dir);
        gabi::call(0x020065EC, t40.get(), inv, &mViewCache.mDirection.mU);
    }
    if (!(mEventFlags & 0x100000)) gabi::call<f32>(0x02006814, t40.get()); /* Sin (unused) */
    eSub(&g->mU, t12, &mViewCache.mDirection.mU);
    eMul(t12, t10, v20);
    eAdd(&mViewCache.mDirection.mU, u42, t10);
    gabi::Local<cSGlobe_l> gfc;
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        gabi::call(0x02007324, gfc.get(), d.get());
    }
    f32 blend = 1.0f;
    if (m314 != 0 && m318 == m354 && (gabi::load<u16>(entry + 0x80) & 4)) {
        if (spd > 0.5f) blend = 0.0f;
        else {
            f32 h = 0.5f - spd;
            blend = h + h;
        }
    }
    gabi::Local<cSAngle_l> lat18;
    cSAngle_ct(lat18);
    eSub(&gfc->mV, t14, &g->mV);
    eMul(t14, t10, 0.25f * blend);
    eAdd(&g->mV, t12, t10);
    g->mV = angCtE(tmp, *t12);
    if (p->m04.get() != nullptr && (eStatus1(mPadId) & 4)) {
        gabi::Local<cSAngle_l> t6c;
        u32 sh = gabi::ea(p->m04.get());
        gabi::call(0x020068F4, &g->mV, t6c.get(), (s16)(gabi::load<s16>(sh + 0x328) + gabi::load<s16>(sh + 0x67E) - 0x4000)); /* + s16 */
        gabi::call(0x02006638, lat18.get(), t6c.get());
        gabi::call(0x020069A0, lat18.get(), 0.3f); /* *= */
    } else {
        *lat18 = (s16)g->mV;
    }
    eSub(lat18, t12, &mViewCache.mDirection.mV);
    eMul(t12, t14, v21);
    eAdd(&mViewCache.mDirection.mV, vNew, t14);
    if ((s16)*vNew < (s16)*lo) gabi::call(0x02006638, vNew.get(), lo.get());
    else if ((s16)*vNew > (s16)*hi) gabi::call(0x02006638, vNew.get(), hi.get());
    gabi::call(0x020071E0, &mViewCache.mDirection, R, vNew.get(), u42.get()); /* Val(f32, V, U) */
    {
        gabi::Local<cXyz> x, e;
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        f32 fv = mViewCache.mFovy;
        mViewCache.mEye.copy(*e);
        mViewCache.mFovy = gabi::fmadds(fovy - fv, v21, fv);
    }
    eMul(bankMax, t10, mStickMainPosXLast);
    eSub(t10, t12, &mViewCache.mBank);
    eMul(t12, t14, 0.01f * __builtin_fabsf(spd));
    gabi::call(0x020068CC, &mViewCache.mBank, t14.get()); /* += */
    mEventFlags = mEventFlags | 0x400;
    return true;
}
VERIFY(0x0250D4E8, &dCamera_c::rideCamera);
