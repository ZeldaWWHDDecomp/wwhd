/**
 * d_camera_engine.cpp (WWHD)
 * Follow camera dCamera_c: camera engines (engine_tbl) - non-owner, fixed frame/position, crawl,
 * hookshot and the other smaller algorithms.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera.h"

/* stack dBgS_CamLinChk (same as d_camera_bg.cpp camLinChk_ct / dt) */
static void camLinChk_ct_engine(u32 o) {
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
static void camLinChk_dt_engine(u32 o) {
    gabi::store<u32>(o + 0x58, 0x1004A61C);
    gabi::store<u32>(o + 0x20, 0x1004A4DC);
    gabi::store<u32>(o + 0x64, 0x1004A4EC);
    gabi::call(0x02008B4C, o, 0);
}

/* 0251485C. HD: the per-frame direction follows the cannon's pitch on V and its yaw on U (the
 * GameCube source swapped the setters' names) */
bool dCamera_c::nonOwnerCamera(s32 style) {
    WWHD_FUNC(0x0251485C, bool, this, style);
    fopAc_ac_c* canon = gabi::at<fopAc_ac_c>(gabi::load<u32>(0x101D5F20)); /* daCanon_c::getCanonPtr() */
    mpLockonTarget = canon;
    if (canon == nullptr) return false;
    gabi::Local<cXyz> ofs, c, x, p, d, s;
    u32 first = m11C;
    ofs->y = camParamVal(style, 5);
    f32 ang = camParamVal(style, 23);
    f32 vAng = camParamVal(style, 15);
    f32 speed = camParamVal(style, 20);
    f32 fovy = camParamVal(style, 25);
    ofs->x = camParamVal(style, 1);
    f32 dist = camParamVal(style, 10);
    ofs->z = camParamVal(style, 0);
    f32 vScale = camParamVal(style, 18);
    if (first == 0) {
        gabi::Local<cSAngle_l> dir;
        relationalPos(c, canon, ofs);
        mViewCache.mCenter.copy(*c);
        directionOf(dir, mpLockonTarget.get());
        s16 inv = cSAngle_Inv(dir);
        s16 v = (s16)gabi::ftoi(vAng * 182.04445f);
        gabi::call(0x02006FE4, &mViewCache.mDirection, dist, v, inv); /* mViewCache.mDirection.Val(dist, v, inv) */
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, c, x);
        m100 = 1;
        m101 = 1;
        mViewCache.mEye.copy(*c);
        m102 = 1;
        mViewCache.mFovy = fovy;
        return true;
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> t, a, b, cc, dd, u, w, xx, y, tmp, dir;
    gabi::call(0x02007324, g.get(), ofs.get());
    gabi::call(0x02006908, &g->mV, t.get(), (s16)gabi::load<s16>(gabi::ea(mpLockonTarget.get()) + 0x328)); /* V - shape_angle.x */
    cSAngle_l* r = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*t);
    g->mV = (s16)*r;
    gabi::call(0x020068F4, &g->mU, t.get(), (s16)gabi::load<s16>(gabi::ea(mpLockonTarget.get()) + 0x32A)); /* U + shape_angle.y */
    r = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*t);
    g->mU = (s16)*r;
    attentionPos(c, mpLockonTarget.get());
    gabi::call(0x020073AC, g.get(), x.get());
    cXyz_pl(c, p, x);
    cXyz_mi(p, d, &mViewCache.mCenter);
    cXyz_ml(d, s, 0.2f);
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    gabi::call(0x020066C0, a.get(), vAng); /* cSAngle(f32) */
    cSAngle_ct(b);
    gabi::call(0x0200658C, cc.get(), (s16)0);
    cSAngle_ct(dd);
    cSAngle_Val(b, (s16)gabi::load<s16>(gabi::ea(mpLockonTarget.get()) + 0x328));
    directionOf(dir, mpLockonTarget.get());
    cSAngle_Val(dd, cSAngle_Inv(dir));
    gabi::call(0x0200693C, b.get(), t.get(), vScale);
    gabi::call(0x02006894, a.get(), u.get(), t.get());
    gabi::call(0x020068B0, u.get(), w.get(), &mViewCache.mDirection.mV);
    gabi::call(0x0200693C, w.get(), xx.get(), speed);
    gabi::call(0x02006894, &mViewCache.mDirection.mV, y.get(), xx.get());
    r = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*y);
    mViewCache.mDirection.mV = (s16)*r;
    gabi::call(0x0200693C, dd.get(), y.get(), ang);
    gabi::call(0x02006894, cc.get(), xx.get(), y.get());
    gabi::call(0x020068B0, xx.get(), w.get(), &mViewCache.mDirection.mU);
    gabi::call(0x0200693C, w.get(), u.get(), speed);
    gabi::call(0x02006894, &mViewCache.mDirection.mU, t.get(), u.get());
    r = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*t);
    f32 rr = mViewCache.mDirection.mRadius;
    mViewCache.mDirection.mU = (s16)*r;
    mViewCache.mDirection.mRadius = gabi::fmadds(dist - rr, speed, rr);
    gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, d, x);
    f32 fv = mViewCache.mFovy;
    mViewCache.mEye.copy(*d);
    mViewCache.mFovy = gabi::fmadds(fovy - fv, speed, fv);
    return true;
}
VERIFY(0x0251485C, &dCamera_c::nonOwnerCamera);

/* work area of fixedFrameCamera (mWork, GC Work::FixedFrame) */
struct FixedFrame_l {
    /* 0x00 */ cXyz m378;      /* arrow position */
    /* 0x0C */ cSGlobe_l m384; /* direction to frame */
    /* 0x14 */ be<s32> m38C;   /* frames */
    /* 0x18 */ be<f32> m390;   /* remaining weight sum */
    /* 0x1C */ be<f32> m394;
    /* 0x20 */ be<f32> m398;   /* fovy */
    /* 0x24 */ cXyz m39C;      /* centre */
    /* 0x30 */ cXyz m3A8;
};

/* 0250C77C. HD: the nearest point on the arrow line and the minimum distance are applied to the
 * centre (m39C; GameCube used m3A8); the frame count is k+1 with a weight sum (k+1)(k+2)/2 */
bool dCamera_c::fixedFrameCamera(s32 style) {
    WWHD_FUNC(0x0250C77C, bool, this, style);
    f32 v24 = camParamVal(style, 24);
    FixedFrame_l* p = (FixedFrame_l*)mWork;
    f32 fovy = camParamVal(style, 25);
    if (mCurArrowIdx == 0xFF) return false;
    if (m11C == 0) {
        gabi::Local<cXyz> t, x, att, out, d, t2;
        gabi::Local<cSGlobe_l> g;
        gabi::Local<u8[0x20]> line;
        gabi::Local<be<f32>> tparam;
        u32 arrow = ea() + 0x5D8; /* mCurRoomArrowEntry.position */
        u32 ax = gabi::load<u32>(arrow), ay = gabi::load<u32>(arrow + 4), az = gabi::load<u32>(arrow + 8);
        gabi::store<u32>(gabi::ea(&p->m378), ax);
        gabi::store<u32>(gabi::ea(&p->m378) + 4, ay);
        gabi::store<u32>(gabi::ea(&p->m378) + 8, az);
        s16 angX = gabi::load<s16>(ea() + 0x5E4);
        gabi::call(0x02007068, g.get(), 3000.0f, (s16)-angX, (s16)gabi::load<s16>(ea() + 0x5E6)); /* cSGlobe(3000, -angle.x, angle.y) */
        gabi::call(0x020073AC, g.get(), x.get());
        cXyz_pl(&p->m378, t, x);
        p->m3A8.copy(*t);
        p->m39C.copy(*t);
        gabi::call(0x02018780, line.get(), &p->m39C, &p->m378); /* cM3dGLin line(m39C, m378) */
        attentionPos(att, mpPlayerActor.get());
        if (gabi::call<bool>(0x02010AE4, line.get(), att.get(), out.get(), tparam.get())) { /* cM3d_Len3dSqPntAndSegLine */
            p->m39C.copy(*out);
        }
        cXyz_mi(&p->m378, d, &p->m39C);
        cSGlobe_Val(&p->m384, d);
        if (m530 != 0 && p->m384.mRadius < 350.0f) {
            g->mRadius = 350.0f;
            gabi::call(0x020073AC, g.get(), x.get());
            cXyz_pl(&p->m378, t2, x);
            p->m39C.copy(*t2);
            p->m384.mRadius = 350.0f;
        }
        u32 fv = gabi::load<u8>(ea() + 0x5D6); /* mCurRoomCamEntry.field_0x12 */
        if (fv != 0xFF) fovy = (f32)fv;
        p->m398 = fovy;
        u32 n = gabi::load<u8>(ea() + 0x5D7); /* mCurRoomCamEntry.field_0x13 */
        if (n == 0xFF) {
            cXyz_mi(&p->m378, d, &mEye);
            f32 len = std_sqrtf(PSVECSquareMag(d));
            f32 h = heightOf(mpPlayerActor.get());
            f32 r = std_sqrtf(len / h);
            s32 k = gabi::ftoi(v24 * r);
            s32 frames = k + 1;
            p->m38C = frames;
            p->m390 = (f32)((frames * (k + 2)) >> 1);
        } else {
            s32 frames = (s32)n;
            p->m38C = frames;
            p->m390 = (f32)((frames * (frames + 1)) >> 1);
        }
    }
    if (m100 == 0) {
        gabi::Local<cXyz> d, s, cr;
        gabi::Local<cSAngle_l> t, u, w, tmp;
        f32 r = (f32)((s32)p->m38C - (s32)m11C);
        f32 sum = p->m390;
        p->m394 = r;
        f32 k = r / sum;
        p->m390 = sum - r;
        cXyz_mi(&p->m39C, d, &mViewCache.mCenter);
        cXyz_ml(d, s, k);
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(p->m384.mRadius - rr, k, rr);
        gabi::call(0x020068B0, &p->m384.mV, t.get(), &mViewCache.mDirection.mV);
        gabi::call(0x0200693C, t.get(), u.get(), k);
        gabi::call(0x02006894, &mViewCache.mDirection.mV, w.get(), u.get());
        cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*w);
        mViewCache.mDirection.mV = (s16)*a;
        gabi::call(0x020068B0, &p->m384.mU, w.get(), &mViewCache.mDirection.mU);
        gabi::call(0x0200693C, w.get(), u.get(), k);
        gabi::call(0x02006894, &mViewCache.mDirection.mU, t.get(), u.get());
        a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*t);
        mViewCache.mDirection.mU = (s16)*a;
        gabi::call(0x020073AC, &mViewCache.mDirection, s.get());
        cXyz_pl(&mViewCache.mCenter, d, s);
        mViewCache.mEye.copy(*d);
        if (gabi::call<bool>(0x024FB608, this, &mViewCache.mEye, &mViewCache.mCenter, cr.get(), 0x7F)) {
            mViewCache.mCenter.copy(*cr);
        }
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(p->m398 - fv, k, fv);
        if ((u32)m11C >= (u32)((s32)p->m38C - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
    }
    return true;
}
VERIFY(0x0250C77C, &dCamera_c::fixedFrameCamera);

/* work area of fixedPositionCamera (mWork, GC Work::FixedPos) */
struct FixedPos_l {
    /* 0x00 */ be<s32> m378; /* frames */
    /* 0x04 */ be<f32> m37C; /* remaining weight sum */
    /* 0x08 */ be<f32> m380;
    /* 0x0C */ cXyz m384;    /* smoothed target */
    /* 0x18 */ cXyz m390;    /* last arrow position */
    /* 0x24 */ be<u8> m39C;  /* fixed frame count from the map */
};

/* moves mViewCache.mDirection towards g by k and rebuilds the eye and fovy */
static void fixedPosApproach(dCamera_c* c, cSGlobe_l* g, f32 k, f32 fovy, f32 minR, f32 maxR) {
    gabi::Local<cSAngle_l> t, u, w, tmp;
    gabi::Local<cXyz> x, e;
    f32 r = g->mRadius;
    if (r < minR) {
        r = minR;
        g->mRadius = r;
    }
    f32 vr = c->mViewCache.mDirection.mRadius;
    if (r > maxR) {
        r = maxR;
        g->mRadius = r;
    }
    c->mViewCache.mDirection.mRadius = gabi::fmadds(r - vr, k, vr);
    gabi::call(0x020068B0, &g->mV, t.get(), &c->mViewCache.mDirection.mV);
    gabi::call(0x0200693C, t.get(), u.get(), k);
    gabi::call(0x02006894, &c->mViewCache.mDirection.mV, w.get(), u.get());
    cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*w);
    c->mViewCache.mDirection.mV = (s16)*a;
    gabi::call(0x020068B0, &g->mU, w.get(), &c->mViewCache.mDirection.mU);
    gabi::call(0x0200693C, w.get(), u.get(), k);
    gabi::call(0x02006894, &c->mViewCache.mDirection.mU, t.get(), u.get());
    a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*t);
    c->mViewCache.mDirection.mU = (s16)*a;
    gabi::call(0x020073AC, &c->mViewCache.mDirection, x.get());
    cXyz_pl(&c->mViewCache.mCenter, e, x);
    f32 fv = c->mViewCache.mFovy;
    c->mViewCache.mEye.copy(*e);
    c->mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
}

/* 0250CC80. HD: the DMC angle is only taken when none is active; the first-frame distance uses
 * max(height, 10); the map frame count is used as the weight sum directly */
bool dCamera_c::fixedPositionCamera(s32 style) {
    WWHD_FUNC(0x0250CC80, bool, this, style);
    FixedPos_l* p = (FixedPos_l*)mWork;
    u32 first = m11C;
    f32 v0 = camParamVal(style, 0), v1 = camParamVal(style, 1), v10 = camParamVal(style, 10), v11 = camParamVal(style, 11);
    f32 v24 = camParamVal(style, 24), v4 = camParamVal(style, 4), v5 = camParamVal(style, 5), fovy = camParamVal(style, 25);
    f32 v23 = camParamVal(style, 23), v3 = camParamVal(style, 3);
    if (first == 0) p->m390.copy(*gabi::at<cXyz>(0x101FFBA8)); /* cXyz::Zero */
    p->m39C = 0;
    gabi::Local<cXyz> pos, ofs, mul, target, d, s, c;
    s32 frames = -1;
    if ((gabi::load<u16>(CAM_STYLES + style * 0x84 + 0x80) & 0x40) && mCurArrowIdx != 0xFF) {
        pos->copy(*gabi::at<cXyz>(ea() + 0x5D8)); /* mCurRoomArrowEntry.position */
        if (gabi::call<bool>(0x0201AFD8, &p->m390, pos.get()) && mDMCSystem.field_0x0 == 0) setDMCAngle(); /* cXyz::operator!= */
        p->m390.copy(*pos);
        u32 fv = gabi::load<u8>(ea() + 0x5D6);
        u32 n = gabi::load<u8>(ea() + 0x5D7);
        if (fv != 0xFF) fovy = (f32)fv;
        if (n != 0xFF) {
            p->m39C = 1;
            frames = (s32)n;
        }
    } else {
        pos->copy(mEye);
    }
    ofs->y = v5;
    ofs->x = v1;
    ofs->z = v0;
    if (m11C == 0) {
        if (p->m39C == 0) {
            cXyz_mi(pos, d, &mEye);
            f32 l = std_sqrtf(PSVECSquareMag(d));
            if (l > v10) l = v10;
            relationalPos(c, mpPlayerActor.get(), ofs);
            cXyz_mi(&mCenter, d, c);
            f32 l2 = std_sqrtf(PSVECSquareMag(d));
            f32 m = (l2 - l >= 0.0f) ? l2 : l; /* fsel */
            f32 h = heightOf(mpPlayerActor.get());
            h = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
            f32 r = std_sqrtf(m / h);
            s32 k = gabi::ftoi(v24 * r);
            s32 n = k + 1;
            p->m378 = n;
            p->m37C = (f32)((n * (k + 2)) >> 1);
        } else {
            p->m378 = frames;
            p->m37C = (f32)frames;
        }
        p->m384.copy(mViewCache.mCenter);
    }
    mul->x = v3;
    mul->y = v4;
    mul->z = v3;
    relationalPos(target, mpPlayerActor.get(), ofs);
    gabi::Local<cSGlobe_l> g;
    if (m100 == 0) {
        f32 k;
        if (p->m39C == 0) {
            f32 r = (f32)((s32)p->m378 - (s32)m11C);
            f32 sum = p->m37C;
            p->m380 = r;
            k = r / sum;
            p->m37C = sum - r;
        } else {
            f32 sum = p->m37C;
            k = 1.0f / sum;
            p->m37C = sum - 1.0f;
        }
        cXyz_mi(target, d, &p->m384);
        cXyz_ml(d, s, k);
        PSVECAdd(&p->m384, s, &p->m384);
        cXyz_mi(&p->m384, s, &mViewCache.mCenter);
        gabi::call(0x0201AF1C, s.get(), d.get(), mul.get()); /* cXyz::operator*(const cXyz&) */
        PSVECAdd(&mViewCache.mCenter, d, &mViewCache.mCenter);
        cXyz_mi(pos, d, &mViewCache.mCenter);
        gabi::call(0x02007324, g.get(), d.get());
        fixedPosApproach(this, g, k, fovy, v11, v10);
        if ((u32)m11C >= (u32)((s32)p->m378 - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
        return true;
    }
    cXyz_mi(target, d, &mViewCache.mCenter);
    gabi::call(0x0201AF1C, d.get(), s.get(), mul.get());
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    cXyz_mi(pos, d, &mViewCache.mCenter);
    gabi::call(0x02007324, g.get(), d.get());
    fixedPosApproach(this, g, v23, fovy, v11, v10);
    return true;
}
VERIFY(0x0250CC80, &dCamera_c::fixedPositionCamera);

/* work area of crawlCamera (mWork, GC Work::Crawl) */
struct Crawl_l {
    /* 0x00 */ be<u32> m378; /* 'CRWL' */
    /* 0x04 */ be<s32> m37C; /* frames */
    /* 0x08 */ be<f32> m380; /* remaining weight sum */
    /* 0x0C */ be<f32> m384;
    /* 0x10 */ be<s32> m388; /* look from behind */
    /* 0x14 */ cXyz m38C;    /* map arrow eye */
    /* 0x20 */ be<u8> m398;  /* low tunnel */
    /* 0x21 */ be<u8> m399;  /* arrow eye active */
    /* 0x22 */ u8 _22[2];
    /* 0x24 */ be<s32> m39C;
};

static inline bool angGt(s16 a, u32 lim) { return a > gabi::load<s16>(lim); }
static inline bool angLt(s16 a, u32 lim) { return a < gabi::load<s16>(lim); }

/* 02511200. HD: the tunnel camera eases U by 0.25 per frame (GameCube 0.2); the arrow eye is used
 * while the camera faces along the map arrow */
bool dCamera_c::crawlCamera(s32 style) {
    WWHD_FUNC(0x02511200, bool, this, style);
    Crawl_l* p = (Crawl_l*)mWork;
    gabi::Local<cXyz> ofs, mul, c, d, s, x, e, pp;
    gabi::Local<cSAngle_l> dir, rel, t, u, w, a, vNew, uNew, sp70, sp74, sp78, tmp;
    u32 entry = CAM_STYLES + style * 0x84;
    ofs->x = camParamVal(style, 1);
    f32 v3 = camParamVal(style, 3);
    f32 v4 = camParamVal(style, 4);
    ofs->z = camParamVal(style, 0);
    f32 fovy = camParamVal(style, 25);
    ofs->y = camParamVal(style, 5);
    f32 v10 = camParamVal(style, 10);
    directionOf(dir, mpPlayerActor.get());
    gabi::call(0x020068B0, dir.get(), rel.get(), &mViewCache.mDirection.mU); /* local_118 */
    u16 flags;
    if (m108 == 0) {
        p->m378 = 0x4352574C; /* 'CRWL' */
        flags = gabi::load<u16>(entry + 0x80);
        p->m37C = 10;
        p->m398 = (flags & 0x80) != 0;
        p->m399 = 0;
        s32 pad = mPadId;
        if ((gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x8000000) && m1AE != 0) p->m37C = 6;
        s32 n = p->m37C;
        p->m39C = 5;
        p->m380 = (f32)((n * (n + 1)) >> 1);
        if ((flags & 0x40) && angGt(*rel, 0x101FF35C) && angLt(*rel, 0x101FF358)) p->m388 = 0; /* _270 < rel < _90 */
        else p->m388 = 1;
    } else {
        flags = gabi::load<u16>(entry + 0x80);
        if (!(flags & 0x40)) {
            p->m388 = 1;
        } else {
            cSAngle_l* lim = gabi::call<cSAngle_l*>(0x020066C0, a.get(), -75.0f);
            bool done = false;
            if ((s16)*rel > (s16)*lim) {
                lim = gabi::call<cSAngle_l*>(0x020066C0, a.get(), 75.0f);
                if ((s16)*rel < (s16)*lim) {
                    p->m388 = 0;
                    done = true;
                }
            }
            if (!done) {
                lim = gabi::call<cSAngle_l*>(0x020066C0, a.get(), -135.0f);
                if ((s16)*rel < (s16)*lim) {
                    p->m388 = 1;
                } else {
                    lim = gabi::call<cSAngle_l*>(0x020066C0, a.get(), 135.0f);
                    if ((s16)*rel > (s16)*lim) p->m388 = 1;
                }
            }
        }
    }
    if (m1AE != 0) setComStat(0x80);
    bool arrowEye = false;
    if ((flags & 0x40) && mCurArrowIdx != 0xFF && gabi::load<u8>(ea() + 0x5D6) != 0xFF) {
        gabi::call(0x02006908, &mViewCache.mDirection.mU, t.get(), (s16)gabi::load<s16>(ea() + 0x5E6)); /* U - arrow.angle.y */
        s16 r = *t;
        *rel = r;
        if (angGt(r, 0x101FF35C) && angLt(r, 0x101FF358)) {
            p->m38C.copy(*gabi::at<cXyz>(ea() + 0x5D8)); /* mCurRoomArrowEntry.position */
            p->m399 = 1;
            mul->x = v3;
            m102 = 1;
            m101 = 1;
            m100 = 1;
            mul->y = v4;
            mul->z = v3;
            arrowEye = true;
        }
    } else {
        p->m399 = 0;
    }
    if (!arrowEye && m100 == 0) {
        f32 r = (f32)((s32)p->m37C - (s32)m108);
        f32 k = r / p->m380;
        p->m384 = r;
        relationalPos(c, mpPlayerActor.get(), ofs);
        cXyz_mi(c, d, &mViewCache.mCenter);
        cXyz_ml(d, s, k);
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
        f32 vr = mViewCache.mDirection.mRadius;
        s16 sx = gabi::load<s16>(gabi::ea(mpPlayerActor.get()) + 0x328); /* shape_angle.x */
        f32 dist = (flags & 0x80) ? 2.0f : v10;
        f32 rad = gabi::fmadds(dist - vr, k, vr);
        gabi::call(0x020065EC, t.get(), sx, &mViewCache.mDirection.mV); /* shape_angle.x - V */
        gabi::call(0x0200693C, t.get(), u.get(), k);
        gabi::call(0x02006894, &mViewCache.mDirection.mV, vNew.get(), u.get());
        cSAngle_ct(sp70);
        if (p->m388 != 0) {
            directionOf(dir, mpPlayerActor.get());
            cSAngle_Val(sp70, cSAngle_Inv(dir));
        } else {
            directionOf(dir, mpPlayerActor.get());
            gabi::call(0x02006638, sp70.get(), dir.get()); /* Val(const cSAngle&) */
        }
        gabi::call(0x020068B0, sp70.get(), w.get(), &mViewCache.mDirection.mU);
        gabi::call(0x0200693C, w.get(), t.get(), k);
        gabi::call(0x02006894, &mViewCache.mDirection.mU, uNew.get(), t.get());
        gabi::call(0x020071E0, &mViewCache.mDirection, rad, vNew.get(), uNew.get()); /* Val(f32, V, U) */
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        f32 fv = mViewCache.mFovy;
        mViewCache.mEye.copy(*e);
        mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
        if ((u32)m108 >= (u32)((s32)p->m37C - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
        p->m380 = p->m380 - p->m384;
        return true;
    }
    if (!arrowEye) {
        mul->x = v3;
        mul->y = v4;
        mul->z = v3;
    }
    relationalPos(c, mpPlayerActor.get(), ofs);
    cXyz_mi(c, d, &mViewCache.mCenter);
    gabi::call(0x0201AF1C, d.get(), s.get(), mul.get()); /* * cXyz */
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    if (p->m399 == 1 && !gabi::call<bool>(0x024FCBE8, this, &mViewCache.mCenter, &p->m38C, 0x7F)) {
        mViewCache.mEye.copy(p->m38C);
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        if (mDMCSystem.field_0x0 == 0) setDMCAngle();
        f32 fv = mViewCache.mFovy;
        m1AE = 0;
        mViewCache.mFovy = gabi::fmadds(fovy - fv, 0.5f, fv);
        return true;
    }
    cSAngle_ct(sp78);
    cSAngle_ct(sp74);
    cSAngle_ct(sp70);
    if (p->m388 != 0) {
        directionOf(dir, mpPlayerActor.get());
        cSAngle_Val(sp70, cSAngle_Inv(dir));
    } else {
        directionOf(dir, mpPlayerActor.get());
        gabi::call(0x02006638, sp70.get(), dir.get());
    }
    f32 rad;
    if (p->m398 == 0) {
        f32 vr = mViewCache.mDirection.mRadius;
        rad = gabi::fmadds(v10 - vr, 0.25f, vr);
        gabi::call(0x020068B0, sp70.get(), w.get(), &mViewCache.mDirection.mU);
        gabi::call(0x0200693C, w.get(), t.get(), 0.25f);
        gabi::call(0x02006894, &mViewCache.mDirection.mU, uNew.get(), t.get());
        s16 nu = *uNew;
        s16 sx = gabi::load<s16>(gabi::ea(mpPlayerActor.get()) + 0x328);
        *sp78 = nu;
        gabi::call(0x020065EC, t.get(), sx, &mViewCache.mDirection.mV);
    } else {
        positionOf(pp, mpPlayerActor.get());
        f32 vr = mViewCache.mDirection.mRadius;
        mViewCache.mCenter.y = pp->y + 30.0f;
        rad = gabi::fmadds(2.0f - vr, 0.8f, vr);
        gabi::call(0x020068B0, sp70.get(), w.get(), &mViewCache.mDirection.mU);
        gabi::call(0x0200693C, w.get(), t.get(), 0.8f);
        gabi::call(0x02006894, &mViewCache.mDirection.mU, uNew.get(), t.get());
        u32 f = mEventFlags;
        s16 nu = *uNew;
        s16 sx = gabi::load<s16>(gabi::ea(mpPlayerActor.get()) + 0x328);
        *sp78 = nu;
        mEventFlags = f | 0x800;
        gabi::call(0x020065EC, t.get(), sx, &mViewCache.mDirection.mV);
    }
    gabi::call(0x0200693C, t.get(), u.get(), 0.25f);
    gabi::call(0x02006894, &mViewCache.mDirection.mV, vNew.get(), u.get());
    *sp74 = (s16)*vNew;
    gabi::call(0x020071E0, &mViewCache.mDirection, rad, sp74.get(), sp78.get());
    gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, e, x);
    mViewCache.mEye.copy(*e);
    f32 fv = mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds(fovy - fv, 0.5f, fv);
    return true;
}
VERIFY(0x02511200, &dCamera_c::crawlCamera);

/* 02511B5C (GameCube: not decompiled; written from the WWHD code). First frame: when the hookshot
 * tip is far enough away, up to eight eye candidates around the tip (four offsets, mirrored every
 * 16 frames and after the fourth try) are tested for a clear line to the player; the first clear
 * one is kept. Later frames: the centre follows the player and the direction eases towards the
 * kept eye (immediately after 8 frames) or the current eye. */
bool dCamera_c::hookshotCamera(s32 style) {
    WWHD_FUNC(0x02511B5C, bool, this, style);
    static const f32 kOffsets[4][3] = {{180.0f, 40.0f, -120.0f}, {160.0f, 80.0f, -160.0f},
                                       {150.0f, -80.0f, -100.0f}, {100.0f, -120.0f, -140.0f}};
    u32 w = ea() + 0x37C; /* mWork: 'HOOK', +8 centre, +0x14 eye, +0x20 eye found */
    gabi::Local<cXyz> ofs, mul, pp, tp, d, v, x, e, c, s, eye;
    f32 fovy = camParamVal(style, 25);
    f32 maxR = camParamVal(style, 10);
    f32 minDist = camParamVal(style, 14);
    f32 v4 = camParamVal(style, 4);
    f32 minR = camParamVal(style, 11);
    f32 v3 = camParamVal(style, 3);
    f32 speed = camParamVal(style, 23);
    ofs->x = camParamVal(style, 1);
    ofs->y = camParamVal(style, 5);
    ofs->z = camParamVal(style, 0);
    if (m11C == 0) {
        gabi::store<u32>(w, 0x484F4F4B); /* 'HOOK' */
        gabi::at<cXyz>(w + 8)->copy(mViewCache.mCenter);
        fopAc_ac_c* player = mpPlayerActor.get();
        u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
        u32 id = gabi::call_ptr<u32>(gabi::load<u32>(vt + 0xAC), player); /* hookshot tip process id */
        gabi::store<u8>(w + 0x20, 0);
        if (id != 0xFFFFFFFFu) {
            gabi::Local<be<u32>> key;
            *key = id;
            fopAc_ac_c* tip = fopAcIt_Judge(0x025E1234, key.get());
            u32 ang = gabi::ea(tip) + 0xD510; /* tip angles (s16 x, s16 y) */
            positionOf(pp, mpPlayerActor.get());
            positionOf(tp, tip);
            cXyz_mi(tp, d, pp);
            if (std_sqrtf(PSVECSquareMag(d)) > minDist) {
                u32 mirror = ((u32)m07C >> 4) & 1;
                u32 cnt = (u32)m07C;
                for (s32 i = 0; i < 8; i++) {
                    gabi::Local<cSGlobe_l> g;
                    gabi::Local<cSAngle_l> t, tmp;
                    gabi::Local<u8[0x6C]> lc;
                    const f32* o = kOffsets[(cnt + i) & 3];
                    v->y = o[1];
                    v->z = o[2];
                    v->x = mirror ? -o[0] : o[0];
                    gabi::call(0x02007324, g.get(), v.get());
                    gabi::call(0x020068F4, &g->mU, t.get(), (s16)gabi::load<s16>(ang + 2)); /* U + tip angle y */
                    cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*t);
                    g->mU = (s16)*a;
                    gabi::call(0x020068F4, &g->mV, t.get(), (s16)gabi::load<s16>(ang)); /* V + tip angle x */
                    a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)*t);
                    g->mV = (s16)*a;
                    positionOf(tp, tip);
                    gabi::call(0x020073AC, g.get(), x.get());
                    cXyz_pl(tp, e, x);
                    gabi::at<cXyz>(w + 0x14)->copy(*e);
                    u32 lo = gabi::ea(lc.get());
                    camLinChk_ct_engine(lo);
                    if (!lineBGCheck(pp, gabi::at<cXyz>(w + 0x14), (u8*)lc.get(), 0xF)) {
                        gabi::store<u8>(w + 0x20, 1);
                        camLinChk_dt_engine(lo);
                        break;
                    }
                    if (i == 3) mirror ^= 1;
                    camLinChk_dt_engine(lo);
                    if (i < 7) cnt = (u32)m07C;
                }
            }
        }
        m100 = 1;
        m101 = 1;
        m102 = 1;
        return true;
    }
    f32 k;
    if (gabi::load<u8>(w + 0x20) != 0 && (u32)m11C > 8) {
        eye->copy(*gabi::at<cXyz>(w + 0x14));
        k = 1.0f;
    } else {
        eye->copy(mEye);
        k = speed;
    }
    mul->y = v4;
    mul->x = v3;
    mul->z = v3;
    relationalPos(c, mpPlayerActor.get(), ofs);
    if (m31D != 0) {
        gabi::call(0x024EFB08, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4, 1, w + 8, 0, 0);    /* MoveBgMatrixCrrPos */
        gabi::call(0x024EFB08, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4, 1, w + 0x14, 0, 0);
    }
    cXyz_mi(c, d, &mViewCache.mCenter);
    gabi::call(0x0201AF1C, d.get(), s.get(), mul.get());
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    cXyz_mi(eye, d, &mViewCache.mCenter);
    gabi::Local<cSGlobe_l> g;
    gabi::call(0x02007324, g.get(), d.get());
    fixedPosApproach(this, g, k, fovy, minR, maxR);
    return true;
}
VERIFY(0x02511B5C, &dCamera_c::hookshotCamera);
