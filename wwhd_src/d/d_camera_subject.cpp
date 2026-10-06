/**
 * d_camera_subject.cpp (WWHD)
 * Follow camera dCamera_c: first-person (subject) aiming.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera.h"
#include <bit>

/* round25(): frC operand rounding of the PowerPC single-precision multiply (from the recompiler's ppc.h) */

/* work area of subjectCamera (mWork, GC Work::Subject) */
struct Subject_l {
    /* 0x00 */ be<u32> m378; /* 'SUBJ' */
    /* 0x04 */ be<u8> m37C;
    /* 0x05 */ be<u8> m37D;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<s32> m380; /* style */
    /* 0x0C */ be<f32> m384; /* yaw input */
    /* 0x10 */ be<f32> m388; /* pitch input */
    /* 0x14 */ be<f32> m38C; /* zoom */
    /* 0x18 */ be<f32> m390;
    /* 0x1C */ u8 _1C[4];
    /* 0x20 */ be<s32> m398;
    /* 0x24 */ be<f32> m39C; /* zoomed-in fovy */
    /* 0x28 */ be<f32> m3A0; /* fovy at entry */
    /* 0x2C */ u8 _2C[4];
    /* 0x30 */ be<s32> m3A8;
    /* 0x34 */ u8 _34[0xC];
    /* 0x40 */ cSAngle_l m3B8; /* player body angle x */
    /* 0x42 */ cSAngle_l m3BA; /* player angle y */
    /* 0x44 */ be<u8> m3BC;    /* scope view set */
    /* 0x45 */ be<u8> m3BD;    /* low centre */
    /* 0x46 */ u8 _46[2];
    /* 0x48 */ be<s32> m3C0;   /* centre mode: 0 player, 1 eye, 2 extended position */
    /* 0x4C */ be<s32> m3C4;   /* C-stick down state */
};
WWHD_SIZE(Subject_l, 0x50);

/* 02506964. HD: aiming also reads the GamePad gyro (a sead controller at *101F5088): with the C
 * stick released, the pointing direction drives the angles (x30, Y inverted while swimming);
 * with the C stick held its value is shaped by a rational Bezier curve and split by its angle
 * (Y inversion from the save options); swimming uses a separate pitch range; the crawl status
 * zeroes the result */
bool dCamera_c::CalcSubjectAngle(s16* outV, s16* outU) {
    WWHD_FUNC(0x02506964, bool, this, outV, outU);
    Subject_l* p = (Subject_l*)mWork;
    s32 pad = mPadId;
    bool swim = (gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CDC) & 0x100000) != 0;
    bool res = true;
    if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) != 0 && gabi::load<u8>(dComIfGp_ea() + 0x5CEA) != 8) return false;
    if (p->m378 != 0x5355424A /* 'SUBJ' */) return false;
    if (m100 == 0) return false;
    if (p->m37C == 0) res = false;
    s32 st = p->m380;
    f32 pitchMax = camParamVal(st, 19);
    f32 yawMax = camParamVal(st, 24);
    f32 speed = camParamVal(st, 21);
    if (swim) pitchMax = gabi::load<f32>(ea() + 0x820);
    f32 slow = gabi::load<f32>(ea() + 0x770); /* mCamSetup.m030 */
    gabi::Local<cSAngle_l> a, b, c;
    if (!res) {
        gabi::call(0x020066C0, a.get(), yawMax * p->m384);
        gabi::call(0x020066C0, b.get(), pitchMax * p->m388);
        gabi::call(0x020065DC, c.get(), (s16)*a, &p->m3BA); /* (s16) + cSAngle */
        *(be<s16>*)outU = (s16)*c;
        *(be<s16>*)outV = (s16)*b;
    }
    /* HD: GamePad input */
    u32 ctl = gabi::load<u32>(0x101F5088);
    f32 sx = gabi::load<f32>(ctl + 0x138);
    f32 sy = gabi::load<f32>(ctl + 0x13C);
    f32 mag2 = gabi::fmadds(sx, sx, sy * sy);
    f32 mag;
    if (mag2 > 0.0f) {
        f64 est = gabi::frsqrte(mag2);
        f32 e2 = (f32)(est * round25(est));
        f32 eh = (f32)(est * 0.5);
        f32 nr = gabi::fnmsubs(e2, mag2, 3.0f);
        mag = (nr * eh) * mag2;
    } else {
        mag = 0.0f * mag2;
    }
    bool big = 0.1f < mag;
    f32 x, y;
    if (big) {
        u32 opt = gabi::call<u32>(0x027200D0, gabi::load<u32>(0x101F84DC) + 0x12C0);
        bool invert = gabi::load<u8>(opt + 2) != 0;
        x = gabi::call<f32>(0x02007AFC, (s32)mPadId); /* C stick x */
        y = gabi::call<f32>(0x02007B20, (s32)mPadId) * (invert ? 1.0f : -1.0f);
    } else {
        gabi::Local<be<f32>[9]> rot;
        gabi::Local<cXyz> r2, r1;
        gabi::call(0x02618604, rot.get(), gabi::load<u32>(0x101F5088)); /* gyro orientation */
        gabi::call(0x02515788, r2.get(), rot.get(), 2);
        gabi::Local<cXyz> r2c;
        r2c->copy(*r2);
        gabi::call(0x02515788, r1.get(), rot.get(), 1);
        x = (r2c->x - r1->x) * 30.0f;
        y = r2c->y * 30.0f;
        if (swim) y = -y;
    }
    gabi::call(0x026185BC, gabi::load<u32>(0x101F5088));
    fopAc_ac_c* player = mpPlayerActor.get();
    gabi::Local<cSAngle_l> tmp;
    if (player != nullptr && fpcM_GetName(player) == 0xA8) {
        cSAngle_l* r = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)gabi::load<s16>(gabi::ea(player) + 0x3D0)); /* getBodyAngleX */
        p->m3B8 = (s16)*r;
    } else {
        cSAngle_l* r = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)player->shape_angle.x);
        p->m3B8 = (s16)*r;
    }
    cSAngle_l* r = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)mpPlayerActor.get()->shape_angle.y);
    p->m3BA = (s16)*r;
    f32 yaw = x, pitch = y;
    f32 eyeY = mEye.y;
    bool keep = eyeY > m354 + 5.0f && eyeY > mBG_m5C.m58 + 5.0f;
    if (!keep) {
        if (p->m388 < 0.0f) {
            if (!(pitch > 0.0f)) pitch = 0.0f;
        } else {
            if (pitch > 0.0f) pitch = 0.0f;
        }
    }
    pad = mPadId;
    if (gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x8000000) pitch = 0.0f; /* CRAWL */
    pad = mPadId;
    f32 m384;
    if (gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x40000) {
        m384 = 0.0f;
        p->m384 = m384;
    } else if (gabi::load<u16>(CAM_STYLES + (s32)p->m380 * 0x84 + 0x80) & 0x20) {
        m384 = -yaw;
        p->m384 = m384;
        p->m388 = pitch;
    } else {
        f32 k = speed;
        if (big) {
            f32 r1v = gabi::call<f32>(0x024F6F9C, mag, 0.11f); /* dCamMath::rationalBezierRatio */
            f32 r08 = gabi::call<f32>(0x024F6F9C, 0.8f, 0.11f);
            f32 len = r1v * r1v;
            if (mag < 0.8f) len = (mag / 0.8f) * (r08 * r08);
            f32 ang = gabi::call<f32>(0x028F4D28, y, x); /* atan2f */
            s16 s1 = gabi::call<s16>(0x02019510, ang); /* cM_rad2s */
            yaw = len * cM_scos(s1);
            s16 s2 = gabi::call<s16>(0x02019510, ang);
            pitch = len * cM_ssin(s2);
        }
        pad = mPadId;
        if (!(gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x2000) /* SUBJECT */ &&
            (gabi::load<u16>(CAM_STYLES + (s32)p->m380 * 0x84 + 0x80) & 0x10)) {
            k = gabi::fnmsubs(k * slow, p->m38C, k);
        }
        m384 = -(yaw * k);
        f32 m388 = gabi::fmadds(pitch, k, p->m388);
        p->m384 = m384;
        p->m388 = m388;
        if (m384 > 1.0f) {
            m384 = 1.0f;
            p->m384 = m384;
        } else if (m384 < -1.0f) {
            m384 = -1.0f;
            p->m384 = m384;
        }
        m388 = p->m388;
        if (m388 > 1.0f) {
            p->m388 = 1.0f;
            m384 = p->m384;
        } else if (m388 < -1.0f) {
            p->m388 = -1.0f;
            m384 = p->m384;
        }
    }
    gabi::call(0x020066C0, a.get(), yawMax * m384);
    gabi::call(0x020066C0, b.get(), pitchMax * p->m388);
    gabi::call(0x020065DC, c.get(), (s16)*a, &p->m3BA);
    *(be<s16>*)outU = (s16)*c;
    *(be<s16>*)outV = (s16)*b;
    p->m37D = 0;
    pad = mPadId;
    if (gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x8000000) {
        gabi::call(0x020065DC, c.get(), (s16)0, &p->m3BA);
        *(be<s16>*)outU = (s16)*c;
        *(be<s16>*)outV = 0;
    }
    return res;
}
VERIFY(0x02506964, &dCamera_c::CalcSubjectAngle);

static inline u32 sStatus0(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8); }
static inline u32 sStatus1(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CDC); }
static inline void cpfS(void* dst, const void* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }
static inline s16 angCtS(cSAngle_l* tmp, s16 v) { return *gabi::call<cSAngle_l*>(0x0200658C, tmp, v); }
static inline void sSub(const void* a, cSAngle_l* out, const void* b) { gabi::call(0x020068B0, a, out, b); }
static inline void sMul(const void* a, cSAngle_l* out, f32 f) { gabi::call(0x0200693C, a, out, f); }
static inline void sAdd(const void* a, cSAngle_l* out, const void* b) { gabi::call(0x02006894, a, out, b); }
static inline f32 sDegree(const void* a) { return gabi::call<f32>(0x02006720, a); }
static inline f32 bezierS(f32 x, f32 r) { return gabi::call<f32>(0x024F6F9C, x, r); } /* dCamMath::rationalBezierRatio */

/* attention-height test of the subject camera: a low centre (60 up, 5 back) unless swimming */
static void subjectLowCentre(dCamera_c* c, Subject_l* p, bool swim, f32* y, f32* z) {
    gabi::Local<cXyz> a;
    c->attentionPos(a, c->mpPlayerActor.get());
    if (a->y < c->m354 + 20.0f && !swim) {
        *y = 60.0f;
        *z = -5.0f;
        p->m3BD = 1;
    } else {
        p->m3BD = 0;
    }
}

/* the subject centre for the centre mode (eye + rotated offset, extended position, or relative) */
static void subjectCentre(dCamera_c* c, Subject_l* p, cXyz* ofs, cXyz* out, cSAngle_l* baseYaw) {
    if ((u32)p->m3C0 == 1) {
        gabi::Local<cSGlobe_l> g;
        gabi::Local<cSAngle_l> dir, t, tmp;
        gabi::Local<cXyz> e, x, r;
        gabi::call(0x02007324, g.get(), ofs);
        c->directionOf(dir, c->mpPlayerActor.get());
        sAdd(dir, t, &g->mU);
        g->mU = angCtS(tmp, *t);
        c->eyePos(e, c->mpPlayerActor.get());
        gabi::call(0x020073AC, g.get(), x.get());
        cXyz_pl(e, r, x);
        out->copy(*r);
    } else if ((u32)p->m3C0 == 2) {
        gabi::Local<cSAngle_l> tmp;
        out->copy(c->mExtendedPos);
        cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)gabi::load<s16>(c->ea() + 0xFA)); /* m0FA */
        cSAngle_Val(baseYaw, cSAngle_Inv(a));
    } else {
        gabi::Local<cXyz> r;
        c->relationalPos(r, c->mpPlayerActor.get(), ofs);
        out->copy(*r);
    }
}

/* telescope / picto box status and zoom scale 1 */
static void subjectZoomStat(dCamera_c* c) {
    if (sStatus0(c->mPadId) & 0x200000) c->setComStat(8);     /* TELESCOPE_LOOK */
    if (sStatus1(c->mPadId) & 8) c->setComStat(0x40);         /* PICTO_BOX_AIM */
    c->setComZoomScale(1.0f);
}

/* 025071FC. HD: swimming (status1 0x100000) aims with dCamSetup_c's swim parameters, keeps the
 * player's facing as base yaw, pushes the centre out of walls, has no zoom and banks the view; the
 * low-centre test runs every frame; the scope view is 1280x720 and the picto box wipe is mapped to
 * the HD viewport; the transition's frame-count end also applies without the zoom flag; the zoom
 * uses the main stick's Y; the C-stick-down sequence is replaced by a GamePad button */
bool dCamera_c::subjectCamera(s32 style) {
    WWHD_FUNC(0x025071FC, bool, this, style);
    Subject_l* p = (Subject_l*)mWork;
    bool swim = (sStatus1(mPadId) & 0x100000) != 0;
    f32 p0 = camParamVal(style, 0);
    f32 p10 = camParamVal(style, 10);
    f32 p19 = camParamVal(style, 19);
    f32 p26 = camParamVal(style, 26);
    f32 p20 = camParamVal(style, 20);
    f32 p25 = camParamVal(style, 25);
    f32 p1 = camParamVal(style, 1);
    f32 p24 = camParamVal(style, 24);
    f32 p5 = camParamVal(style, 5);
    if (swim) {
        p0 = gabi::load<f32>(ea() + 0x818);
        p26 = 0.0f;
        p1 = gabi::load<f32>(ea() + 0x810);
        p5 = gabi::load<f32>(ea() + 0x814);
        p10 = gabi::load<f32>(ea() + 0x81C);
        p19 = gabi::load<f32>(ea() + 0x820);
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> baseYaw, tmp;
    gabi::call(0x02007100, g.get());
    {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(dir);
        cSAngle_ct(baseYaw, inv);
        if (swim) {
            gabi::Local<cSAngle_l> d2;
            directionOf(d2, mpPlayerActor.get());
            gabi::call(0x02006638, baseYaw.get(), d2.get()); /* Val(const cSAngle&) */
        }
    }
    (void)dComIfGp_ea();
    u16 sflags;
    if (m108 == 0) {
        p->m37C = 0;
        p->m384 = 0.0f;
        p->m380 = style;
        p->m37D = 1;
        p->m3A8 = 100;
        p->m3BC = 0;
        p->m378 = 0x5355424A; /* 'SUBJ' */
        p->m388 = 0.0f;
        p->m3C0 = 0;
        p->m38C = 0.0f;
        if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 3) { /* dComIfGp_getMiniGameType() */
            p->m3C0 = 2;
            sflags = gabi::load<u16>(CAM_STYLES + style * 0x84 + 0x80);
        } else {
            u32 s0 = sStatus0(mPadId);
            sflags = gabi::load<u16>(CAM_STYLES + style * 0x84 + 0x80);
            if (!(s0 & 0x2000) && !(sflags & 0x10)) p->m3C0 = 1;
        }
        p->m3C4 = (mStickCPosYLast < -0.74f) ? 2 : 0;
    } else {
        sflags = gabi::load<u16>(CAM_STYLES + style * 0x84 + 0x80);
    }
    subjectLowCentre(this, p, swim, &p5, &p0);
    gabi::Local<cXyz> ofs;
    ofs->z = p0;
    ofs->x = p1;
    ofs->y = p5;
    if (!swim) {
        if (sflags & 0x80) mEventFlags = mEventFlags | 0x800;
        if (sflags & 0x100) mEventFlags = mEventFlags | 0x10000000;
    }
    bool zoomFlag = (sflags & 0x10) != 0;

    if (m100 == 0) {
        s32 end = (sStatus0(mPadId) & 0x2000) ? 7 : 10;
        f32 t = 1.0f / (f32)(s32)(end - (s32)m108);
        gabi::Local<cXyz> target, d, s, x, e;
        gabi::Local<cSAngle_l> t20, t1e, t1c;
        subjectCentre(this, p, ofs, target, baseYaw);
        cXyz_mi(target, d, &mViewCache.mCenter);
        cXyz_ml(d, s, t);
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
        gabi::call(0x020071E0, g.get(), p10, gabi::at<cSAngle_l>(0x101FF354), baseYaw.get());
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, t, rr);
        sSub(&g->mV, t20, &mViewCache.mDirection.mV);
        sMul(t20, t1e, t);
        sAdd(&mViewCache.mDirection.mV, t1c, t1e);
        mViewCache.mDirection.mV = angCtS(tmp, *t1c);
        sSub(&g->mU, t1c, &mViewCache.mDirection.mU);
        sMul(t1c, t1e, t);
        sAdd(&mViewCache.mDirection.mU, t20, t1e);
        mViewCache.mDirection.mU = angCtS(tmp, *t20);
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        if (!zoomFlag) {
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(p25 - fv, t, fv);
        } else if ((u32)m108 >= (u32)(end - 6)) {
            subjectZoomStat(this);
            f32 sc = gabi::load<f32>(dComIfGp_ea() + 0x5B40); /* mItemScopeWipeScale */
            f32 h = (sc - 1.0f) * 0.5f;
            f32 xw = gabi::fmadds(640.0f, h, 320.0f);
            if (xw < 1280.0f && !(sc < 1.0f) && !(sc > 3.0f)) {
                f32 k = (1280.0f - xw) / 960.0f;
                if ((sStatus1(mPadId) & 8) || gabi::load<u8>(dComIfGp_ea() + 0x5BCF) != 0) { /* PICTO_BOX_AIM, demo camera */
                    setView(160.0f * k, 35.0f * k, xw, gabi::fmadds(-400.0f, k, 720.0f));
                }
                f32 base = p->m3A0;
                mViewCache.mFovy = gabi::fmadds(p->m39C - base, k, base);
            }
        } else if ((u32)m108 >= (u32)(end - 7)) {
            subjectZoomStat(this);
        } else if (m108 == 0) {
            cpfS(&p->m3A0, &mViewCache.mFovy);
            p->m39C = mViewCache.mFovy * 0.44444445f; /* HD (GameCube 2/3) */
        }
        if ((u32)m108 == (u32)(end - 1)) {
            m100 = 1;
            m101 = 1;
            m102 = 1;
            p->m384 = 0.0f;
            p->m388 = 0.0f;
            p->m38C = 0.0f;
            p->m398 = 0;
            cpfS(&p->m39C, &mViewCache.mFovy);
            p->m390 = 0.0f;
        }
        return true;
    }

    if (sStatus0(mPadId) & 0x8000000) setComStat(0x80); /* CRAWL */
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BCF) != 0 || (sStatus1(mPadId) & 8)) {
        setView(160.0f, 35.0f, 320.0f, 320.0f);
        p->m3BC = 1;
    } else if (p->m3BC != 0) {
        setView(0.0f, 0.0f, 1280.0f, 720.0f);
    }
    gabi::Local<cSAngle_l> angU, angV; /* sp08 (yaw), sp0A (pitch) */
    cSAngle_ct(angU);
    cSAngle_ct(angV);
    if (p->m37D != 0) {
        gabi::Local<be<s16>> ov, ou;
        gabi::Local<cSAngle_l> t3a;
        p->m37C = 1;
        CalcSubjectAngle((s16*)ov.get(), (s16*)ou.get());
        *angV = angCtS(tmp, *ov);
        s16 u = *ou;
        if (swim) u = (s16)(u - 0x1500);
        cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), u);
        sSub(a, t3a, &p->m3BA);
        *angU = (s16)*t3a;
    } else {
        gabi::Local<cSAngle_l> t3e;
        fopAc_ac_c* pl = mpPlayerActor.get();
        if (pl != nullptr && gabi::load<s16>(gabi::ea(pl) + 8) == 0xA8) {
            *angV = angCtS(tmp, gabi::load<s16>(gabi::ea(pl) + 0x3D0)); /* player body angle x */
        } else {
            *angV = angCtS(tmp, gabi::load<s16>(gabi::ea(pl) + 0x328)); /* shape_angle.x */
        }
        cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), (s16)gabi::load<s16>(gabi::ea(mpPlayerActor.get()) + 0x32A));
        sSub(a, t3e, &p->m3BA);
        *angU = (s16)*t3e;
        p->m388 = sDegree(angV) / p19;
        p->m384 = sDegree(angU) / p24;
    }
    gabi::Local<cXyz> v1, v2;
    {
        gabi::Local<cSAngle_l> c1, c2;
        cSAngle_l* a = gabi::call<cSAngle_l*>(0x02006644, c1.get(), angV.get());
        gabi::call(0x024F79BC, v1.get(), ofs.get(), a); /* dCamMath::xyzRotateX */
        a = gabi::call<cSAngle_l*>(0x02006644, c2.get(), angU.get());
        gabi::call(0x024F7A40, v2.get(), v1.get(), a); /* dCamMath::xyzRotateY */
    }
    subjectCentre(this, p, v2, &mViewCache.mCenter, baseYaw);
    if (swim) {
        gabi::Local<cXyz> a;
        gabi::Local<u8[0x6C]> lc;
        attentionPos(a, mpPlayerActor.get());
        u32 o = gabi::ea(lc.get());
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
        if (lineBGCheck(a, &mViewCache.mCenter, (u8*)lc.get(), 0x7F)) {
            u32 plane = gabi::call<u32>(0x020084C8, dComIfGp_ea() + PLAY_BGS, (u32)gabi::load<u16>(o + 0x16), (u32)gabi::load<u16>(o + 0x14));
            mViewCache.mCenter.copy(*gabi::at<cXyz>(o + 0x30));
            if (plane != 0) PSVECAdd(&mViewCache.mCenter, gabi::at<cXyz>(plane), &mViewCache.mCenter);
        }
        gabi::store<u32>(o + 0x58, 0x1004A61C);
        gabi::store<u32>(o + 0x64, 0x1004A4EC);
        gabi::store<u32>(o + 0x20, 0x1004A4DC);
        gabi::call(0x02008B4C, o, 0);
    }
    bool flag25 = (mEventFlags & 0x2000000) != 0;
    bool useTarget = (flag25 || mLockOnActorId != 0xFFFFFFFFu) && mpLockonTarget.get() != nullptr;
    if (useTarget || (p->m3BC != 0 && flag25)) {
        /* towards the lock-on target (rate 0.04) or the extended position (0.05) */
        gabi::Local<cXyz> a, d;
        gabi::Local<cSAngle_l> ta, tb, tc;
        f32 rate;
        if (useTarget) {
            attentionPos(a, mpLockonTarget.get());
            a->y = a->y - 25.0f;
            rate = 0.04f;
        } else {
            cpfS(&a->x, &mExtendedPos.x);
            cpfS(&a->y, &mExtendedPos.y);
            cpfS(&a->z, &mExtendedPos.z);
            rate = 0.05f;
        }
        cXyz_mi(&mViewCache.mCenter, d, a);
        cSGlobe_Val(g, d);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(p10 - rr, rate, rr);
        g->mRadius = p10;
        sSub(&g->mV, ta, &mViewCache.mDirection.mV);
        sMul(ta, tb, rate);
        sAdd(&mViewCache.mDirection.mV, tc, tb);
        mViewCache.mDirection.mV = angCtS(tmp, *tc);
        sSub(&g->mU, tc, &mViewCache.mDirection.mU);
        sMul(tc, tb, rate);
        sAdd(&mViewCache.mDirection.mU, ta, tb);
        mViewCache.mDirection.mU = angCtS(tmp, *ta);
        sSub(&mViewCache.mDirection.mU, ta, baseYaw);
        *angU = (s16)*ta;
        *angV = (s16)mViewCache.mDirection.mV;
        p->m384 = sDegree(angU) / p24;
        p->m388 = sDegree(angV) / p19;
        p->m37C = 0;
        p->m3A8 = 0;
    } else {
        gabi::Local<cSAngle_l> t12, t2c, t2a;
        sAdd(baseYaw, t12, angU);
        gabi::call(0x020071E0, g.get(), p10, angV.get(), t12.get());
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, p20, rr);
        sSub(&g->mV, t2c, &mViewCache.mDirection.mV);
        sMul(t2c, t2a, p20);
        sAdd(&mViewCache.mDirection.mV, t12, t2a);
        mViewCache.mDirection.mV = angCtS(tmp, *t12);
        sSub(&g->mU, t12, &mViewCache.mDirection.mU);
        sMul(t12, t2a, p20);
        sAdd(&mViewCache.mDirection.mU, t2c, t2a);
        mViewCache.mDirection.mU = angCtS(tmp, *t2c);
        if (p->m3A8 < 10) p->m3A8 = p->m3A8 + 1;
        p->m37C = 1;
    }
    {
        gabi::Local<cXyz> x, e;
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
    }
    if (zoomFlag) {
        f32 y = mStickMainPosYLast;
        f32 cw = gabi::load<f32>(ea() + 0x78C);
        f32 diff;
        if (y > 0.0f) {
            f32 b = bezierS(y, cw);
            diff = b - 0.0f;
        } else {
            f32 b = bezierS(-y, cw);
            diff = 0.0f - b;
        }
        f32 next = gabi::fmadds(diff * p26, 0.1f, p->m38C);
        f32 z;
        if (swim) z = (1.0f - 0.0f >= 0.0f) ? 0.0f : 1.0f;
        else if (next < 0.0f) z = 0.0f;
        else z = (1.0f - next >= 0.0f) ? next : 1.0f; /* fsel */
        p->m38C = z;
        if (z == 0.0f || z == 0.5f || z == 1.0f) diff = 0.0f - 0.0f;
        f32 scale = gabi::fmadds(z, 8.0f, 1.0f);
        f32 sm = scale - 1.0f;
        f32 pos = (sm >= 0.0f) ? sm : 0.0f; /* fsel */
        f32 zf = gabi::call<f32>(0x024F7948, p->m39C * 0.5f, gabi::fmadds(pos, 1.5f, 1.0f)); /* dCamMath::zoomFovy */
        f32 tf = zf + zf;
        f32 fv = mViewCache.mFovy;
        if (swim) tf = gabi::load<f32>(ea() + 0x824);
        mViewCache.mFovy = gabi::fmadds(tf - fv, p20, fv);
        setComZoomScale(scale);
        if (sStatus0(mPadId) & 0x200000) setComStat(8);
        if (sStatus1(mPadId) & 8) setComStat(0x40);
        f32 focus = gabi::fnmsubs(__builtin_fabsf(diff), -511.0f, 1.0f);
        s32 id = mCameraID;
        gabi::store<f32>(dComIfGp_ea() + id * 0x34 + 0x5B08, focus); /* setComZoomForcus */
        gabi::store<u8>(0x101F4828, 0); /* mDoGph_gInf_c::mAutoForcus */
        p->m37D = 1;
    } else {
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(p25 - fv, p20, fv);
        p->m37D = 1;
    }
    if (sStatus0(mPadId) & 0x2000) { /* SUBJECT */
        s32 st = p->m3C4;
        if (st == 2) {
            if (!(mStickCPosYLast < -0.001f)) p->m3C4 = 0;
        } else if (st == 0 && (gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x18) & 0x40)) { /* HD: GamePad button */
            p->m3C4 = 2;
            setComStat(0x2000);
        } else if (mStickCPosYLast < -0.001f) {
            p->m3C4 = 1;
        } else {
            p->m3C4 = 0;
        }
    }
    if (swim) {
        gabi::call(0x02006694, &mViewCache.mBank, (f32)gabi::load<f32>(ea() + 0x828)); /* HD: swim bank */
        mEventFlags = mEventFlags | 0x400;
    } else {
        mViewCache.mBank = gabi::load<s16>(0x101FF354);
    }
    return true;
}
VERIFY(0x025071FC, &dCamera_c::subjectCamera);
