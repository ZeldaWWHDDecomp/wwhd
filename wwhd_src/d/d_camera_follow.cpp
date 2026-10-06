/**
 * d_camera_follow.cpp (WWHD)
 * Follow camera dCamera_c: followCamera, the default third-person engine.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp, a "Nonmatching" draft there) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera.h"

/* work area of followCamera (mWork, GC Work::Follow) */
struct Follow_l {
    /* 0x00 */ be<u32> m378;  /* 'FLLW' */
    /* 0x04 */ be<s32> m37C;  /* transition frames */
    /* 0x08 */ be<f32> m380;  /* remaining weight sum */
    /* 0x0C */ be<f32> m384;
    /* 0x10 */ be<s32> m388;  /* frames since free movement */
    /* 0x14 */ be<s32> m38C;  /* turn-behind counter */
    /* 0x18 */ be<s16> m390;
    /* 0x1A */ be<s16> m392;  /* charge frames */
    /* 0x1C */ be<f32> m394;
    /* 0x20 */ be<f32> m398;  /* min radius */
    /* 0x24 */ be<f32> m39C;  /* max radius */
    /* 0x28 */ be<f32> m3A0;  /* latitude (deg) */
    /* 0x2C */ be<f32> m3A4;  /* last player y */
    /* 0x30 */ be<f32> m3A8;
    /* 0x34 */ be<f32> m3AC;  /* centre offset x */
    /* 0x38 */ be<f32> m3B0;  /* centre offset z */
    /* 0x3C */ be<s32> m3B4;
    /* 0x40 */ be<f32> m3B8;  /* longitude follow ratio */
    /* 0x44 */ be<f32> m3BC;
    /* 0x48 */ cXyz m3C0;     /* smoothed centre */
    /* 0x54 */ cXyz m3CC;     /* target eye */
    /* 0x60 */ be<u8> m3D8;
    /* 0x61 */ be<u8> m3D9;
    /* 0x62 */ be<u8> m3DA;
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<f32> m3DC;
    /* 0x68 */ be<f32> m3E0;  /* latitude follow ratio */
    /* 0x6C */ be<f32> m3E4;
    /* 0x70 */ be<f32> m3E8;  /* fovy follow ratio */
    /* 0x74 */ be<f32> m3EC;  /* centre follow ratio xz */
    /* 0x78 */ be<f32> m3F0;  /* centre follow ratio y */
};
WWHD_SIZE(Follow_l, 0x7C);

static inline u32 fStatus0(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8); }
static inline u32 fStatus1(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CDC); }
static inline bool cb1Flying() { return gabi::load<u8>(0x101BDC2D) != 0; }   /* daNpc_Cb1_c::m_flying */
static inline bool mdFlying() { return gabi::load<u8>(0x101D5F3E) != 0; }    /* daNpc_Md_c::m_flying */
static inline bool hyoiKamome() { return gabi::load<u8>(0x101D5F45) != 0; }  /* daNpc_kam_c::m_hyoi_kamome */
static inline f32 bezierRatio(f32 x, f32 r) { return gabi::call<f32>(0x024F6F9C, x, r); } /* dCamMath::rationalBezierRatio */
static inline s16 angLoad(u32 a) { return gabi::load<s16>(a); }
enum : u32 { ANG_0 = 0x101FF354, ANG_90 = 0x101FF358, ANG_270 = 0x101FF35C };
/* bit-exact float copy (lfs/stfs pass-through) */
static inline void cpf(void* dst, const void* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }
static inline f32 angDegree(const cSAngle_l* a) { return gabi::call<f32>(0x02006720, a); }
static inline s16 angCt(cSAngle_l* tmp, s16 v) { return *gabi::call<cSAngle_l*>(0x0200658C, tmp, v); }

/* stack dBgS_CamLinChk (same as d_camera_bg.cpp camLinChk_ct / dt) */
static void camLinChk_ct_follow(u32 o) {
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
static void camLinChk_dt_follow(u32 o) {
    gabi::store<u32>(o + 0x58, 0x1004A61C);
    gabi::store<u32>(o + 0x64, 0x1004A4EC);
    gabi::store<u32>(o + 0x20, 0x1004A4DC);
    gabi::call(0x02008B4C, o, 0);
}

/* line check from start to mViewCache.mCenter; on a hit the centre is put on the wall, pushed out
 * along the wall normal (HD: only when the plane exists) */
static void followWallCheck(dCamera_c* c, cXyz* start) {
    gabi::Local<u8[0x6C]> lc;
    u32 o = gabi::ea(lc.get());
    camLinChk_ct_follow(o);
    if (c->lineBGCheck(start, &c->mViewCache.mCenter, (u8*)lc.get(), 0x7F)) {
        u32 bgs = dComIfGp_ea() + PLAY_BGS;
        u32 plane = gabi::call<u32>(0x020084C8, bgs, (u32)gabi::load<u16>(o + 0x16), (u32)gabi::load<u16>(o + 0x14)); /* GetTriPla */
        c->mViewCache.mCenter.copy(*gabi::at<cXyz>(o + 0x30)); /* GetCross() */
        if (plane != 0) PSVECAdd(&c->mViewCache.mCenter, gabi::at<cXyz>(plane), &c->mViewCache.mCenter); /* += mNormal */
    }
    camLinChk_dt_follow(o);
}

/* turn ratio from the stick angle (no turn-behind sequence running) */
static inline f32 followStickTurn(dCamera_c* c, cSAngle_l* rel) {
    if (c->mStickMainValueLast < 0.1f) return 0.05f;
    f32 s = gabi::call<f32>(0x02006814, rel); /* Sin */
    return bezierRatio(__builtin_fabsf(s), 12.0f);
}

/* 025028B8. HD: the first-frame transition uses the frame weight for the radius, latitude, yaw
 * and fovy (GameCube: the constant follow ratio) and max(height, 10) as divisor; the free-turn
 * decay is Val20 * 0.86; the wall push-out checks the plane; flying with the seagull adds a
 * debug-register controlled yaw of the centre around the eye and a fovy limit */
bool dCamera_c::followCamera(s32 style) {
    WWHD_FUNC(0x025028B8, bool, this, style);
    Follow_l* p = (Follow_l*)mWork;
    gabi::Local<cSAngle_l> chargeLat, lim80, lo, hi, peep, tmp, tmp2;
    f32 f31 = 0.9f;
    gabi::call(0x020066C0, chargeLat.get(), (f32)gabi::load<f32>(ea() + 0x7E8)); /* mCamSetup.ChargeLatitude() */
    f32 chargeBRatio = gabi::load<f32>(ea() + 0x7F0);
    s32 chargeTimer = gabi::load<s32>(ea() + 0x7EC);
    gabi::call(0x020066C0, lim80.get(), 80.0f);
    f32 v15 = camParamVal(style, 15);
    f32 v3 = camParamVal(style, 3);
    f32 v0 = camParamVal(style, 0);
    f32 v13 = camParamVal(style, 13);
    f32 v1 = camParamVal(style, 1);
    f32 v10 = camParamVal(style, 10);
    f32 v4 = camParamVal(style, 4);
    f32 v11 = camParamVal(style, 11);
    f32 v14 = camParamVal(style, 14);
    f32 v5 = camParamVal(style, 5);
    gabi::call(0x020066C0, lo.get(), camParamVal(style, 16));
    gabi::call(0x020066C0, hi.get(), camParamVal(style, 17));
    f32 v19 = camParamVal(style, 19);
    f32 v29 = camParamVal(style, 29);
    f32 v25 = camParamVal(style, 25);
    f32 v23 = camParamVal(style, 23);
    f32 v18 = camParamVal(style, 18);
    f32 v24 = camParamVal(style, 24);
    f32 v20 = camParamVal(style, 20) * 0.86f; /* HD */
    (void)dComIfGp_ea(); /* dComIfGp_getAttention() (unused) */
    bool bVar2 = false;
    if (m108 == 0) {
        p->m3AC = 0.0f;
        p->m3B0 = 0.0f;
        p->m3D9 = 0;
    }
    if (cb1Flying() || mdFlying()) {
        f31 = 0.66f;
        if (v15 < 35.0f) v15 = 35.0f;
        gabi::call(0x020069A0, &m148, 0.33f); /* m148 *= 0.33f */
        if (v10 < 420.0f) v10 = 420.0f;
        if (v11 < 350.0f) v11 = 350.0f;
        gabi::call(0x02006694, hi.get(), 80.0f);
        if (cb1Flying() && m788 != 0) {
            if (m787 != 0) {
                gabi::Local<cSAngle_l> a;
                cSAngle_l* ap = gabi::call<cSAngle_l*>(0x020066C0, a.get(), 30.0f);
                if ((s16)*lo < (s16)*ap) gabi::call(0x02006694, lo.get(), 30.0f);
                if (v15 < 50.0f) v15 = 50.0f;
                v11 = 600.0f;
                v10 = 800.0f;
            } else {
                gabi::Local<cSAngle_l> a;
                cSAngle_l* ap = gabi::call<cSAngle_l*>(0x020066C0, a.get(), 10.0f);
                if ((s16)*lo < (s16)*ap) gabi::call(0x02006694, lo.get(), 10.0f);
            }
        }
    }
    if (fStatus0(mPadId) & 0x08100000) { /* CRAWL | SWIM */
        gabi::Local<cSAngle_l> a;
        cSAngle_l* ap = gabi::call<cSAngle_l*>(0x020066C0, a.get(), 4.0f);
        if ((s16)*lo < (s16)*ap) gabi::call(0x02006694, lo.get(), 4.0f);
        if (v5 < -10.0f) v5 = -10.0f;
    }
    if ((fStatus0(mPadId) & 0x300) && !(fStatus0(mPadId) & 0x2000000)) {
        if (v0 > -10.0f) p->m3B0 = -10.0f;
    } else {
        f32 b0 = p->m3B0;
        p->m3B0 = gabi::fmadds(v0 - b0, 0.06f, b0);
    }
    if (fStatus1(mPadId) & 0x40000) {
        v10 = 420.0f;
        v15 = -24.0f;
        v5 = 140.0f;
        bVar2 = true;
        v25 = 80.0f;
        v11 = v10;
        m148 = angLoad(ANG_0);
    }
    cSAngle_ct(peep, gabi::at<cSAngle_l>(ANG_0));
    if (fStatus0(mPadId) & 0x60) {
        gabi::Local<cSAngle_l> pa;
        calcPeepAngle(pa);
        *peep = (s16)*pa;
        if (fStatus0(mPadId) & 0x20) v1 = -v1;
    }
    u16 sflags = gabi::load<u16>(CAM_STYLES + style * 0x84 + 0x80);
    if (sflags & 0x200) bVar2 = true;
    if (((mEventFlags & 0x100000) && (fStatus0(mPadId) & 0xA5000)) || cb1Flying() || mdFlying()) {
        f32 tgt;
        if (mStickMainPosXLast < -0.2f) p->m3D9 = 1;
        if (mStickMainPosXLast > 0.2f) {
            p->m3D9 = 0;
            tgt = 45.0f;
        } else if (p->m3D9 != 0) {
            tgt = -45.0f;
        } else {
            tgt = 45.0f;
        }
        f32 ac = p->m3AC;
        p->m3AC = gabi::fmadds(tgt - ac, 0.04f, ac);
    } else {
        f32 ac = p->m3AC;
        p->m3AC = gabi::fmadds(v1 - ac, 0.06f, ac);
    }
    gabi::Local<cXyz> ofs;
    cpf(&ofs->z, &p->m3B0);
    cpf(&ofs->x, &p->m3AC);
    ofs->y = v5;
    if (m108 == 0) {
        gabi::Local<cXyz> d, pos;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        p->m394 = f31;
        p->m390 = 0;
        p->m392 = 0;
        p->m388 = 0x50;
        p->m398 = v11;
        p->m38C = 0;
        p->m378 = 0x464C4C57; /* 'FLLW' */
        p->m39C = v10;
        f32 deg = angDegree(&mDirection.mV);
        p->m3BC = deg;
        p->m3A0 = deg;
        p->m3C0.copy(mViewCache.mCenter);
        p->m3CC.copy(mViewCache.mEye);
        p->m3E4 = 0.01f;
        p->m3E0 = 0.01f;
        p->m3E8 = 0.01f;
        p->m3DC = 0.75f;
        p->m3EC = v3;
        p->m3F0 = v4;
        p->m3D8 = 1;
        p->m3B4 = 0;
        cpf(&p->m3A8, &mViewCache.mFovy);
        p->m3DA = 0;
        p->m3B8 = 0.0f;
        positionOf(pos, mpPlayerActor.get());
        cpf(&p->m3A4, &pos->y);
        if ((mEventFlags & 0x8000) || m110 == 0) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
            p->m37C = 1;
            cpf(&p->m39C, &mDirection.mRadius);
            cpf(&p->m398, &mDirection.mRadius);
            cpf(&p->m3A8, &mFovy);
            p->m380 = 1.0f;
            p->m384 = 0.0f;
        } else {
            gabi::Local<cXyz> c, x, e;
            gabi::Local<cSAngle_l> u, dir, v;
            gabi::Local<cSGlobe_l> g;
            relationalPos(c, mpPlayerActor.get(), ofs);
            cSAngle_ct(u);
            if (mEventFlags & 0x100000) {
                directionOf(dir, mpPlayerActor.get());
                cSAngle_Val(u, cSAngle_Inv(dir));
            } else {
                gabi::call(0x02006638, u.get(), &mViewCache.mDirection.mU); /* Val(const cSAngle&) */
            }
            cSAngle_l* vp = gabi::call<cSAngle_l*>(0x020066C0, v.get(), v15);
            gabi::call(0x02007248, g.get(), v10, vp, u.get());
            gabi::call(0x020073AC, g.get(), x.get());
            cXyz_pl(c, e, x);
            cXyz_mi(&mEye, d, e);
            f32 l1 = std_sqrtf(PSVECSquareMag(d));
            cXyz_mi(&mCenter, d, c);
            f32 l2 = std_sqrtf(PSVECSquareMag(d)) * 4.0f;
            f32 m = (l2 - l1 >= 0.0f) ? l2 : l1; /* fsel */
            m = __builtin_fabsf(m);
            f32 h = heightOf(mpPlayerActor.get());
            f32 hh = (10.0f - h >= 0.0f) ? 10.0f : h; /* fsel */
            f32 r = std_sqrtf(m / hh);
            p->m37C = gabi::ftoi(3.8f * r) + 1;
            s32 n = p->m37C;
            cpf(&p->m39C, &mDirection.mRadius);
            cpf(&p->m398, &mDirection.mRadius);
            cpf(&p->m3A8, &mFovy);
            p->m380 = (f32)((n * (n + 1)) >> 1);
            p->m384 = 0.0f;
        }
    }
    gabi::Local<cXyz> tgt;
    gabi::Local<cSAngle_l> pdir, rel;
    relationalPos(tgt, mpPlayerActor.get(), ofs);
    directionOf(pdir, mpPlayerActor.get());
    gabi::call(0x020068B0, pdir.get(), rel.get(), &mViewCache.mDirection.mU); /* local_4ac */
    tgt->y = getWaterSurfaceHeight(tgt);
    if (m100 == 0) {
        gabi::Local<cXyz> d, s, x, e;
        if (m31D != 0) gabi::call(0x024EFB08, dComIfGp_ea() + PLAY_BGS, ea() + 0x2D4, 1, &p->m3C0, 0, 0); /* MoveBgMatrixCrrPos */
        f32 r = (f32)(s32)((s32)p->m37C - (s32)m108);
        f32 k = r / p->m380;
        p->m384 = r;
        cXyz_mi(tgt, d, &p->m3C0);
        cXyz_ml(d, s, k);
        PSVECAdd(&p->m3C0, s, &p->m3C0);
        cXyz_mi(&p->m3C0, s, &mViewCache.mCenter);
        cXyz_ml(s, d, v3);
        PSVECAdd(&mViewCache.mCenter, d, &mViewCache.mCenter);
        f32 dist = gabi::call<f32>(0x024F7AC4, tgt.get(), &p->m3C0); /* dCamMath::xyzHorizontalDistance */
        f32 ox = ofs->x, oz = ofs->z;
        f32 om = (oz - ox >= 0.0f) ? oz : ox; /* fsel */
        if (dist < __builtin_fabsf(om) + 20.0f) {
            gabi::Local<cXyz> a;
            attentionPos(a, mpPlayerActor.get());
            a->y = a->y - 15.0f;
            followWallCheck(this, a);
        }
        f32 R = gabi::call<f32>(0x025028A0, (f32)mViewCache.mDirection.mRadius, v11, v10); /* limitf */
        gabi::Local<cSAngle_l> vv, ay, t42, t40, t30, t48, t46, t44;
        gabi::Local<cSGlobe_l> g;
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
        gabi::call(0x020068B0, &g->mV, t42.get(), &mViewCache.mDirection.mV);
        gabi::call(0x0200693C, t42.get(), t40.get(), k);
        gabi::call(0x02006894, &mViewCache.mDirection.mV, t30.get(), t40.get());
        mViewCache.mDirection.mV = angCt(tmp, *t30);
        if (mEventFlags & 0x100000) {
            s16 pi = cSAngle_Inv(pdir);
            gabi::call(0x020065EC, t48.get(), pi, &mViewCache.mDirection.mU);
            gabi::call(0x0200693C, t48.get(), t46.get(), k);
            gabi::call(0x02006894, &mViewCache.mDirection.mU, t44.get(), t46.get());
            mViewCache.mDirection.mU = angCt(tmp, *t44);
        }
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        p->m3CC.copy(mViewCache.mEye);
        if ((u32)m108 >= (u32)((s32)p->m37C - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
        p->m3A0 = angDegree(&mViewCache.mDirection.mV);
        cpf(&p->m39C, &mViewCache.mDirection.mRadius);
        cpf(&p->m398, &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(v25 - fv, k, fv);
        p->m380 = p->m380 - p->m384;
        return true;
    }

    gabi::Local<cXyz> pp, mul, d, s;
    positionOf(pp, mpPlayerActor.get());
    pp->y = pp->y + 10.0f;
    f32 foot = footHeightOf(mpPlayerActor.get());
    f32 height = foot - groundHeight(pp);
    if ((m360 != 0 || (fStatus0(mPadId) & 0x100000) || hyoiKamome()) && !(fStatus0(mPadId) & 0x200)) {
        p->m388 = 0;
        p->m394 = 0.0f;
    } else if (p->m388 < 0x50) {
        s32 n = p->m388 + 1;
        p->m388 = n;
        f32 old = p->m394;
        f32 b = bezierRatio((f32)n / 80.0f, 1.25f);
        p->m394 = gabi::fmadds(f31 - old, b, old);
    }
    if (((fStatus0(mPadId) & 0x6800061) || (fStatus1(mPadId) & 0x10000)) && mDMCSystem.field_0x0 == 0) setDMCAngle();
    if (((fStatus0(mPadId) & 0x2000161) || (fStatus1(mPadId) & 0x10000)) &&
        ((s16)*rel <= angLoad(ANG_270) || (s16)*rel >= angLoad(ANG_90))) {
        p->m3EC = 0.1f;
    } else {
        p->m3EC = v3;
    }
    if (p->m388 == 0) {
        p->m3F0 = v4;
        cpf(&mul->x, &p->m3EC);
        mul->y = v4;
        cpf(&mul->z, &p->m3EC);
    } else {
        f32 a = v4 * 0.1f;
        p->m3F0 = gabi::fmadds(1.0f - a, p->m394, a);
        if (((mEventFlags & 0x100000) && p->m3EC > 0.25f) || cb1Flying() || mdFlying() || (fStatus1(mPadId) & 0x40000)) {
            p->m3EC = gabi::fmadds(p->m394, 0.75f, 0.25f);
        }
        cpf(&mul->x, &p->m3EC);
        cpf(&mul->y, &p->m3F0);
        cpf(&mul->z, &p->m3EC);
    }
    bool bVar4 = true;
    bool bVar3 = false;
    if ((mEventFlags & 0x80) && mDirection.mRadius < v11) bVar3 = true;
    if ((mEventFlags & 0x100000) || (fStatus0(mPadId) & 0x6820161) || (fStatus1(mPadId) & 0x10040) || p->m388 != 0) bVar4 = false;
    if (mCurMode == 1) {
        gabi::Local<cXyz> a;
        attentionPos(a, mpPlayerActor.get());
        if (tgt->y < a->y + 50.0f) {
            gabi::Local<cSAngle_l> dd;
            gabi::Local<cSGlobe_l> g;
            gabi::Local<cXyz> a2, x, e;
            directionOf(dd, mpPlayerActor.get());
            s16 inv = cSAngle_Inv(dd);
            gabi::call(0x02007068, g.get(), v11, (s16)0, inv); /* cSGlobe(f32, s16, s16) */
            attentionPos(a2, mpPlayerActor.get());
            gabi::call(0x020073AC, g.get(), x.get());
            cXyz_pl(a2, e, x);
            if (gabi::call<bool>(0x024FCBE8, this, a2.get(), e.get(), 0x7F)) tgt->y = a2->y + 50.0f;
        }
    }
    cXyz_mi(tgt, d, &mViewCache.mCenter);
    gabi::call(0x0201AF1C, d.get(), s.get(), mul.get()); /* cXyz * cXyz */
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    if (m780 == 0) {
        gabi::Local<cXyz> a;
        attentionPos(a, mpPlayerActor.get());
        if (fStatus0(mPadId) & 0x8100100) {
            gabi::Local<cXyz> ep;
            eyePos(ep, mpPlayerActor.get());
            a->y = ep->y + 30.0f;
        } else {
            a->y = a->y - 15.0f;
        }
        followWallCheck(this, a);
    }
    gabi::Local<cSGlobe_l> g484;
    {
        gabi::Local<cXyz> d2;
        cXyz_mi(&mViewCache.mEye, d2, &mViewCache.mCenter);
        gabi::call(0x02007324, g484.get(), d2.get());
    }

    s32 c392 = (s16)p->m392;
    if (c392 > 0 && c392 <= chargeTimer) {
        f32 b = bezierRatio((f32)c392 / (f32)chargeTimer, chargeBRatio);
        f32 old = p->m3B8;
        p->m3D8 = 1;
        p->m3B8 = (1.0f - old) * b;
    } else if ((mEventFlags & 0x100000) || cb1Flying()) {
        if (p->m3D8 != 0) p->m3B8 = 0.05f;
        f32 o = p->m3B8;
        p->m3D8 = 0;
        p->m3B8 = gabi::fmadds(1.0f - o, 0.2f, o);
    } else if (mdFlying() || hyoiKamome()) {
        p->m3D8 = 1;
    } else if (fStatus1(mPadId) & 0x40040) {
        if (p->m3D8 != 0) p->m3B8 = 0.05f;
        f32 o = p->m3B8;
        p->m3D8 = 0;
        p->m3B8 = gabi::fmadds(0.5f - o, 0.05f, o);
    } else {
        p->m3D8 = 1;
        f32 v;
        if (mDMCSystem.field_0x0 != 0) {
            p->m3B8 = 0.0f;
            v = 0.0f * mStickMainValueLast;
            p->m3B8 = v;
        } else {
            gabi::Local<cSAngle_l> a;
            if (!(mStickMainPosYLast < 0.0f)) {
                f32 b = bezierRatio(mStickMainPosXLast, v23);
                cSAngle_l* ap = gabi::call<cSAngle_l*>(0x020066C0, a.get(), 180.0f * b);
                f32 c = gabi::call<f32>(0x02006838, ap); /* Cos */
                v = 1.0f - gabi::fmadds(c, 0.5f, 0.5f);
            } else {
                f32 b = bezierRatio(mStickMainPosXLast, v24);
                cSAngle_l* ap = gabi::call<cSAngle_l*>(0x020066C0, a.get(), 180.0f * b);
                f32 c = gabi::call<f32>(0x02006838, ap);
                v = 1.0f - gabi::fmadds(c, 0.25f, 0.75f);
            }
            p->m3B8 = v;
            v = v * mStickMainValueLast;
            p->m3B8 = v;
        }
        if (mEventFlags & 0x80000) p->m3B8 = v * 0.5f;
        else p->m3B8 = v * 0.1f;
        if ((fStatus0(mPadId) & 0x2000100) || (fStatus1(mPadId) & 0x10000)) {
            s32 c = p->m38C;
            if (c < 0) {
                c = c - 1;
                p->m38C = (c <= -0x20) ? 0x10 : c;
            } else if (c == 0) {
                if ((s16)*rel > angLoad(ANG_270) && (s16)*rel < angLoad(ANG_90)) p->m38C = 1;
                else p->m38C = -1;
            } else if (c < 0xF) {
                p->m3B8 = (f32)c * 0.033333335f;
                p->m38C = p->m38C + 1;
            } else if (!(fStatus0(mPadId) & 0x2000000)) {
                p->m3B8 = followStickTurn(this, rel);
            }
        } else if (bVar2) {
            s32 c = p->m38C;
            if (c == 0 && ((s16)*rel <= angLoad(ANG_270) || (s16)*rel >= angLoad(ANG_90))) {
                p->m38C = 1;
            } else if (c < 0xF) {
                p->m3B8 = (f32)c * 0.033333335f;
                p->m38C = p->m38C + 1;
            } else if (fStatus0(mPadId) & 0x60) {
                p->m3B8 = 0.15f;
            } else {
                p->m3B8 = followStickTurn(this, rel);
            }
        } else {
            p->m3B8 = p->m3B8 * v20;
            p->m38C = 0;
        }
    }

    {
        gabi::Local<cSAngle_l> a4b4, z, t28, t26, t24, t34;
        cSAngle_ct(a4b4);
        if ((mEventFlags & 0x80) && !(mEventFlags & 0x80000) && mCurMode == 0 && p->m38C == 0) {
            *a4b4 = angCt(tmp, mDirection.mU);
        } else if (bVar2) {
            *a4b4 = (s16)*pdir;
            if (fStatus0(mPadId) & 0x60) gabi::call(0x020068CC, a4b4.get(), peep.get()); /* += */
        } else {
            s16 inv = cSAngle_Inv(pdir);
            *a4b4 = angCt(tmp, inv);
        }
        cSAngle_ct(z, (s16)0);
        gabi::call(0x020068B0, a4b4.get(), t28.get(), &g484->mU);
        f32 c = gabi::call<f32>(0x02006838, &g484->mV); /* Cos */
        f32 f = p->m3B8 * c;
        gabi::call(0x0200693C, t28.get(), t26.get(), f);
        gabi::call(0x02006894, &g484->mU, t24.get(), t26.get());
        gabi::call(0x02006894, t24.get(), t34.get(), z.get());
        mViewCache.mDirection.mU = angCt(tmp, *t34);
    }

    gabi::Local<cSAngle_l> v4bc;
    cSAngle_ct(v4bc);
    {
        gabi::Local<cXyz> pos;
        bool charging = (fStatus1(mPadId) & 0x20000) != 0;
        s32 n392 = (s16)p->m392;
        if (charging) {
            *v4bc = (s16)*chargeLat;
            if (n392 <= chargeTimer) {
                f32 b = bezierRatio((f32)(s32)(s16)p->m392 / (f32)chargeTimer, chargeBRatio);
                p->m3E0 = b;
                mEventFlags = mEventFlags | 0x4000000;
                p->m392 = (s16)(p->m392 + 1);
            } else {
                p->m3E0 = 1.0f;
            }
            f32 deg = angDegree(v4bc);
            p->m3A0 = deg;
            p->m3BC = deg;
        } else {
            if (n392 != 0) p->m3E0 = 0.0f;
            p->m392 = 0;
            if (mCurMode == 1) {
                f32 a0 = p->m3A0;
                f32 nn = gabi::fmadds(v15 - a0, p->m3B8, a0);
                p->m3E0 = 0.5f;
                p->m3A0 = nn;
                *v4bc = angCt(tmp, (s16)gabi::ftoi(nn * 182.04445f)); /* cAngle::d2s */
                p->m3BC = nn;
                f32 e = p->m3E0;
                p->m3E0 = gabi::fmadds(f31 - e, 0.5f, e);
            } else if (p->m388 == 0) {
                f32 bc = p->m3BC;
                bc = gabi::fmadds(v15 - bc, v14, bc);
                f32 w = 0.01f;
                p->m3BC = bc;
                if (p->m3B4 != 0) {
                    w = 0.25f;
                    v19 = 0.25f;
                }
                f32 deg = angDegree(&m148);
                f32 a0 = p->m3A0;
                f32 nn = gabi::fmadds((bc + deg) - a0, v19, a0);
                p->m3A0 = nn;
                *v4bc = angCt(tmp, (s16)gabi::ftoi(nn * 182.04445f));
                if (mEventFlags & 0x80000) {
                    p->m3E0 = 0.0f;
                } else if (p->m3B4 == 0 && mMonitor.field_0x0C.x < 0.01f && mCurMode == 0) {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(0.1f - e, 0.05f, e);
                } else {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(f31 - e, w, e);
                }
            } else if ((fStatus0(mPadId) & 0x2000000) || (fStatus1(mPadId) & 0x10000)) {
                *v4bc = (s16)g484->mV;
                f32 deg = angDegree(v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
                p->m3E0 = 0.95f;
            } else if ((fStatus1(mPadId) & 0x20) || cb1Flying() || mdFlying() || hyoiKamome()) {
                gabi::Local<cXyz> pos2;
                *v4bc = (s16)g484->mV;
                positionOf(pos2, mpPlayerActor.get());
                if (p->m3A4 < pos2->y) {
                    p->m3E0 = bezierRatio(p->m394, v18);
                } else {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(0.75f - e, 0.15f, e);
                }
                f32 deg = angDegree(v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
            } else {
                *v4bc = (s16)g484->mV;
                f32 deg = angDegree(v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
                p->m3E0 = bezierRatio(p->m394, v18);
            }
        }
        positionOf(pos, mpPlayerActor.get());
        cpf(&p->m3A4, &pos->y);
    }
    if ((fStatus0(mPadId) & 0x2000161) || (fStatus1(mPadId) & 0x10000)) p->m3B4 = 1;
    else p->m3B4 = 0;
    if ((s16)*v4bc < (s16)*lo) gabi::call(0x02006638, v4bc.get(), lo.get());
    else if ((s16)*v4bc > (s16)*hi) gabi::call(0x02006638, v4bc.get(), hi.get());
    {
        gabi::Local<cSAngle_l> t24, t26, t28;
        gabi::call(0x020068B0, v4bc.get(), t24.get(), &mViewCache.mDirection.mV);
        gabi::call(0x0200693C, t24.get(), t26.get(), (f32)p->m3E0);
        gabi::call(0x02006894, &mViewCache.mDirection.mV, t28.get(), t26.get());
        s16 nv = angCt(tmp, *t28);
        mViewCache.mDirection.mV = nv;
        if (nv > (s16)*lim80) mViewCache.mDirection.mV = angCt(tmp2, *lim80);
    }
    {
        gabi::Local<cXyz> x, e;
        f32 a = p->m398, b = p->m39C;
        f32 n398 = gabi::fmadds(v11 - a, v14, a);
        f32 n39C = gabi::fmadds(v10 - b, v14, b);
        f32 r3 = g484->mRadius;
        p->m398 = n398;
        p->m39C = n39C;
        if (r3 < n398) {
            f32 dc = p->m3DC;
            dc = gabi::fmadds(v13 - dc, 0.01f, dc);
            p->m3DC = dc;
            f32 lim = p->m398;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(lim - R, dc, R);
        } else if (r3 > n39C) {
            f32 dc = p->m3DC;
            dc = gabi::fmadds(v13 - dc, 0.01f, dc);
            p->m3DC = dc;
            f32 lim = p->m39C;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(lim - R, dc, R);
        } else {
            p->m3DC = 1.0f;
            f32 R = mViewCache.mDirection.mRadius;
            mViewCache.mDirection.mRadius = gabi::fmadds(r3 - R, 1.0f, R);
        }
        gabi::call(0x020073AC, &mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        p->m3CC.copy(*e);
    }
    if (bVar3 && bVar4 && (sflags & 1)) {
        gabi::Local<cSGlobe_l> gg;
        gabi::Local<cSAngle_l> a;
        gabi::Local<cXyz> x, e;
        gabi::call(0x02007178, gg.get(), &mViewCache.mDirection); /* cSGlobe(const cSGlobe&) */
        cSAngle_l* av = gabi::call<cSAngle_l*>(0x020066C0, a.get(), v15);
        gg->mV = angCt(tmp, *av);
        gabi::call(0x020073AC, gg.get(), x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        if (gabi::call<bool>(0x024FCBE8, this, &mViewCache.mCenter, e.get(), 0x7F)) mEventFlags = mEventFlags | 8;
    }
    cXyz_mi(&p->m3CC, d, &mViewCache.mEye);
    cXyz_ml(d, s, 0.75f);
    PSVECAdd(&mViewCache.mEye, s, &mViewCache.mEye);
    cXyz_mi(&mViewCache.mEye, s, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, s);
    f32 e8 = p->m3E8;
    if (mEventFlags & 8) p->m3DA = 1;
    e8 = gabi::fmadds(v29 - e8, 0.01f, e8);
    p->m3E8 = e8;
    f32 fv = mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds(v25 - fv, e8, fv);
    if (fStatus1(mPadId) & 0x40) { /* DEKU_LEAF_FAN */
        f32 r = cM_rndFX(gabi::load<f32>(ea() + 0x7BC)); /* mCamSetup.m078 */
        mViewCache.mFovy = mViewCache.mFovy + r;
    }
    if ((fStatus1(mPadId) & 0x20) || cb1Flying() || mdFlying() || hyoiKamome()) {
        f32 bank, ratio;
        if (height < 200.0f && (!hyoiKamome() || gabi::load<s16>(0x1047B688) != 0)) {
            f32 q = height / 200.0f;
            ratio = gabi::fnmsubs(0.96f, q, 1.0f);
            bank = mStickMainPosXLast * q;
        } else {
            bank = mStickMainPosXLast;
            ratio = 0.04f;
        }
        gabi::Local<cSAngle_l> t32, t4E, t4C;
        cSAngle_l* sp = gabi::call<cSAngle_l*>(0x0200658C, t32.get(), (s16)(m07C << 7));
        f32 sn = gabi::call<f32>(0x02006814, sp); /* Sin */
        mViewCache.mFovy = gabi::fmadds(gabi::load<f32>(ea() + 0x7C0), sn, mViewCache.mFovy); /* mCamSetup.m07C */
        f32 fan = gabi::call<f32>(0x024F78F0, ea() + 0x73C); /* dCamSetup_c::FanBank */
        gabi::call(0x020066C0, t4E.get(), fan * bank);
        gabi::call(0x020068B0, t4E.get(), t4C.get(), &mViewCache.mBank);
        gabi::call(0x0200693C, t4C.get(), t32.get(), ratio);
        gabi::call(0x020068CC, &mViewCache.mBank, t32.get()); /* += */
        mEventFlags = mEventFlags | 0x400;
        if (hyoiKamome() && gabi::load<s16>(0x1047B688) == 0) {
            /* HD: debug-register yaw of the centre around the eye while riding a seagull */
            gabi::Local<cXyz> dv, rv, c;
            cXyz_mi(&mViewCache.mCenter, dv, &mViewCache.mEye);
            u32 mtx = gabi::load<u32>(0x1018C7B0); /* mDoMtx_stack_c::now */
            f32 deg = angDegree(&mViewCache.mBank);
            f32 k = gabi::load<f32>(0x1047C038) + -75.0f;
            gabi::call(0x025F1884, mtx, (s16)gabi::ftoi(deg * k)); /* mDoMtx_YrotS */
            gabi::call(0x0200FCD8, dv.get(), rv.get()); /* MtxPosition */
            cXyz_pl(&mViewCache.mEye, c, rv);
            mViewCache.mCenter.copy(*c);
            f32 lim = gabi::load<f32>(0x1047C03C) + 60.0f;
            if (mViewCache.mFovy > lim) mViewCache.mFovy = lim;
        }
    }
    return true;
}
VERIFY(0x025028B8, &dCamera_c::followCamera);
