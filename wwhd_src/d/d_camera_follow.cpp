/**
 * d_camera_follow.cpp (WWHD)
 * Follow camera dCamera_c: followCamera, the default third-person engine.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp, a "Nonmatching" draft there) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_follow.h"
namespace camera_follow {
static MatrixPointer followMatrix;
static GuestByte cb1FlyingFlag;
static GuestByte mdFlyingFlag;
static GuestByte hyoiKamomeFlag;
static GuestShort seagullDebug;
static GuestFloat seagullYaw;
static GuestFloat seagullFovy;

/* work area of followCamera (mWork, GC Work::Follow) */

static inline bool cb1Flying() {
    return cb1FlyingFlag != 0;
}
static inline bool mdFlying() {
    return mdFlyingFlag != 0;
}
static inline bool hyoiKamome() {
    return hyoiKamomeFlag != 0;
}
static inline f32 bezierRatio(f32 x, f32 r) {
    return cameraFollowBezierRatio(x, r);
} /* dCamMath::rationalBezierRatio */
static inline s16 angLoad(cSAngle_l* a) {
    return *a;
}

/* bit-exact float copy (lfs/stfs pass-through) */
static inline void cpf(void* dst, const void* src) {
    *reinterpret_cast<GuestWord*>(dst) = *reinterpret_cast<const GuestWord*>(src);
}
static inline f32 angDegree(const cSAngle_l* a) {
    return cameraFollowAngleDegree(a);
}
static inline s16 angCt(cSAngle_l* tmp, s16 v) {
    return *cameraFollowAngleShortCtor(tmp, v);
}

/* stack dBgS_CamLinChk (same as d_camera_bg.cpp camLinChk_ct / dt) */
static void camLinChk_ct_follow(dBgS_LinChk* o) {
    cameraFollowLineCtor(o);
    o->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(o));
    o->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(o));
    o->cBgS_Chk::__vtbl = CAM_LINE;
    o->cBgS_PolyInfo::__vtbl = CAM_POLY_INFO;
    o->dBgS_PolyPassChk::__vtbl = CAM_POLY_PASS;
    o->dBgS_GrpPassChk::__vtbl = CAM_GROUP_PASS;
    o->mbObjThrough = 0;
    o->mbCamThrough = 1;
    o->mbLinkThrough = 0;
    o->mbArrowThrough = 0;
    o->mbBombThrough = 0;
    o->mbBoomerangThrough = 0;
    o->mbRopeThrough = 0;
    o->mGrp = 3;
}
static void camLinChk_dt_follow(dBgS_LinChk* o) {
    o->dBgS_PolyPassChk::__vtbl = BASE_POLY_PASS;
    o->dBgS_GrpPassChk::__vtbl = BASE_GROUP_PASS;
    o->cBgS_PolyInfo::__vtbl = BASE_POLY_INFO;
    cameraFollowLineDtor(o, 0);
}
/* line check from start to mViewCache.mCenter; on a hit the centre is put on the wall, pushed out
 * along the wall normal (HD: only when the plane exists) */
/* inlined into dCamera_c::followCamera */
static void followWallCheck(dCamera_c* c, cXyz* start) {
    dBgS_LinChk lc;
    dBgS_LinChk* o = &lc;
    camLinChk_ct_follow(o);
    if (c->lineBGCheck(start, &c->mViewCache.mCenter, (u8*)&lc, 0x7F)) {
        dBgS* bgs = dComIfG_Bgsp();
        cM3dGPla* plane =
            cameraFollowGetTriPla(bgs, (u32)o->mBgIndex, (u32)o->mPolyIndex); /* GetTriPla */
        c->mViewCache.mCenter.copy(o->mLin.mEnd);                             /* GetCross() */
        if (plane != 0)
            PSVECAdd(&c->mViewCache.mCenter, &plane->mNormal,
                     &c->mViewCache.mCenter); /* += mNormal */
    }
    camLinChk_dt_follow(o);
}

/* turn ratio from the stick angle (no turn-behind sequence running) */
static inline f32 followStickTurn(dCamera_c* c, cSAngle_l* rel) {
    if (c->mStickMainValueLast < 0.1f)
        return 0.05f;
    f32 s = cameraFollowAngleSin(rel); /* Sin */
    return bezierRatio(__builtin_fabsf(s), 12.0f);
}

} // namespace camera_follow
using namespace camera_follow;
/* 025028B8 */
/* HD: the first-frame transition uses the frame weight for the radius, latitude, yaw
 * and fovy (GameCube: the constant follow ratio) and max(height, 10) as divisor; the free-turn
 * decay is Val20 * 0.86; the wall push-out checks the plane; flying with the seagull adds a
 * debug-register controlled yaw of the centre around the eye and a fovy limit */
bool dCamera_c::followCamera(s32 style) {
    Follow* p = (Follow*)mWork;
    cSAngle_l chargeLat;
    cSAngle_l lim80;
    cSAngle_l lo;
    cSAngle_l hi;
    cSAngle_l peep;
    cSAngle_l tmp;
    cSAngle_l tmp2;
    f32 f31 = 0.9f;
    cameraFollowAngleFloatCtor(&chargeLat,
                               (f32)setup(this)->mChargeLatitude); /* mCamSetup.ChargeLatitude() */
    f32 chargeBRatio = setup(this)->mChargeBRatio;
    s32 chargeTimer = setup(this)->mChargeTimer;
    cameraFollowAngleFloatCtor(&lim80, 80.0f);
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
    cameraFollowAngleFloatCtor(&lo, camParamVal(style, 16));
    cameraFollowAngleFloatCtor(&hi, camParamVal(style, 17));
    f32 v19 = camParamVal(style, 19);
    f32 v29 = camParamVal(style, 29);
    f32 v25 = camParamVal(style, 25);
    f32 v23 = camParamVal(style, 23);
    f32 v18 = camParamVal(style, 18);
    f32 v24 = camParamVal(style, 24);
    f32 v20 = camParamVal(style, 20) * 0.86f; /* HD */
    (void)dComIfGp_getAttention();            /* dComIfGp_getAttention() (unused) */
    bool bVar2 = false;
    if (m108 == 0) {
        p->m3AC = 0.0f;
        p->m3B0 = 0.0f;
        p->m3D9 = 0;
    }
    if (cb1Flying() || mdFlying()) {
        f31 = 0.66f;
        if (v15 < 35.0f)
            v15 = 35.0f;
        cameraFollowAngleScaleAssign(&m148, 0.33f); /* m148 *= 0.33f */
        if (v10 < 420.0f)
            v10 = 420.0f;
        if (v11 < 350.0f)
            v11 = 350.0f;
        cameraFollowAngleValFloat(&hi, 80.0f);
        if (cb1Flying() && m788 != 0) {
            if (m787 != 0) {
                cSAngle_l a;
                cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 30.0f);
                if ((s16)lo < (s16)*ap)
                    cameraFollowAngleValFloat(&lo, 30.0f);
                if (v15 < 50.0f)
                    v15 = 50.0f;
                v11 = 600.0f;
                v10 = 800.0f;
            } else {
                cSAngle_l a;
                cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 10.0f);
                if ((s16)lo < (s16)*ap)
                    cameraFollowAngleValFloat(&lo, 10.0f);
            }
        }
    }
    if (dComIfGp_getPlayerStatus0(mPadId) &
        (daPyStts0_SWIM_e | daPyStts0_CRAWL_e)) { /* CRAWL | SWIM */
        cSAngle_l a;
        cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 4.0f);
        if ((s16)lo < (s16)*ap)
            cameraFollowAngleValFloat(&lo, 4.0f);
        if (v5 < -10.0f)
            v5 = -10.0f;
    }
    if ((dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_HANG_e | daPyStts0_UNK200_e)) &&
        !(dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK2000000_e))) {
        if (v0 > -10.0f)
            p->m3B0 = -10.0f;
    } else {
        f32 b0 = p->m3B0;
        p->m3B0 = gabi::fmadds(v0 - b0, 0.06f, b0);
    }
    if (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK40000_e)) {
        v10 = 420.0f;
        v15 = -24.0f;
        v5 = 140.0f;
        bVar2 = true;
        v25 = 80.0f;
        v11 = v10;
        m148 = angLoad(&angle0());
    }
    cSAngle_ct(&peep, &angle0());
    if (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK20_e | daPyStts0_UNK40_e)) {
        cSAngle_l pa;
        calcPeepAngle(&pa);
        peep = (s16)pa;
        if (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK20_e))
            v1 = -v1;
    }
    u16 sflags = styleEntry(style)->mFlags;
    if (sflags & dCamPrmFlg_UNK200)
        bVar2 = true;
    if (((mEventFlags & 0x100000) && (dComIfGp_getPlayerStatus0(mPadId) &
                                      (daPyStts0_BOW_AIM_e | daPyStts0_HOOKSHOT_AIM_e |
                                       daPyStts0_ROPE_AIM_e | daPyStts0_BOOMERANG_AIM_e))) ||
        cb1Flying() || mdFlying()) {
        f32 tgt;
        if (mStickMainPosXLast < -0.2f)
            p->m3D9 = 1;
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
    cXyz ofs;
    cpf(&ofs.z, &p->m3B0);
    cpf(&ofs.x, &p->m3AC);
    ofs.y = v5;
    if (m108 == 0) {
        cXyz d;
        cXyz pos;
        cXyz_mi(&mViewCache.mEye, &d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, &d);
        p->m394 = f31;
        p->m390 = 0;
        p->m392 = 0;
        p->m388 = 0x50;
        p->m398 = v11;
        p->m38C = 0;
        p->m378 = FOLLOW_WORK; /* 'FLLW' */
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
        positionOf(&pos, mpPlayerActor.get());
        cpf(&p->m3A4, &pos.y);
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
            cXyz c;
            cXyz x;
            cXyz e;
            cSAngle_l u;
            cSAngle_l dir;
            cSAngle_l v;
            cSGlobe_l g;
            relationalPos(&c, mpPlayerActor.get(), &ofs);
            cSAngle_ct(&u);
            if (mEventFlags & 0x100000) {
                directionOf(&dir, mpPlayerActor.get());
                cSAngle_Val(&u, cSAngle_Inv(&dir));
            } else {
                cameraFollowAngleVal(&u, &mViewCache.mDirection.mU); /* Val(const cSAngle&) */
            }
            cSAngle_l* vp = cameraFollowAngleFloatCtor(&v, v15);
            cameraFollowGlobeCtorAngles(&g, v10, vp, &u);
            cameraFollowGlobeXyz(&g, &x);
            cXyz_pl(&c, &e, &x);
            cXyz_mi(&mEye, &d, &e);
            f32 l1 = std_sqrtf(PSVECSquareMag(&d));
            cXyz_mi(&mCenter, &d, &c);
            f32 l2 = std_sqrtf(PSVECSquareMag(&d)) * 4.0f;
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
    cXyz tgt;
    cSAngle_l pdir;
    cSAngle_l rel;
    relationalPos(&tgt, mpPlayerActor.get(), &ofs);
    directionOf(&pdir, mpPlayerActor.get());
    cameraFollowAngleSubtract(&pdir, &rel, &mViewCache.mDirection.mU); /* local_4ac */
    tgt.y = getWaterSurfaceHeight(&tgt);
    if (m100 == 0) {
        cXyz d;
        cXyz s;
        cXyz x;
        cXyz e;
        if (m31D != 0)
            cameraFollowMoveBgPosition(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mBG.m5C.m04), 1,
                                       &p->m3C0, 0, 0); /* MoveBgMatrixCrrPos */
        f32 r = (f32)(s32)((s32)p->m37C - (s32)m108);
        f32 k = r / p->m380;
        p->m384 = r;
        cXyz_mi(&tgt, &d, &p->m3C0);
        cXyz_ml(&d, &s, k);
        PSVECAdd(&p->m3C0, &s, &p->m3C0);
        cXyz_mi(&p->m3C0, &s, &mViewCache.mCenter);
        cXyz_ml(&s, &d, v3);
        PSVECAdd(&mViewCache.mCenter, &d, &mViewCache.mCenter);
        f32 dist =
            cameraFollowHorizontalDistance(&tgt, &p->m3C0); /* dCamMath::xyzHorizontalDistance */
        f32 ox = ofs.x, oz = ofs.z;
        f32 om = (oz - ox >= 0.0f) ? oz : ox; /* fsel */
        if (dist < __builtin_fabsf(om) + 20.0f) {
            cXyz a;
            attentionPos(&a, mpPlayerActor.get());
            a.y = a.y - 15.0f;
            followWallCheck(this, &a);
        }
        f32 R = cameraFollowLimit((f32)mViewCache.mDirection.mRadius, v11, v10); /* limitf */
        cSAngle_l vv;
        cSAngle_l ay;
        cSAngle_l t42;
        cSAngle_l t40;
        cSAngle_l t30;
        cSAngle_l t48;
        cSAngle_l t46;
        cSAngle_l t44;
        cSGlobe_l g;
        cSAngle_ct(&vv);
        s16 V = mViewCache.mDirection.mV;
        if (V < (s16)lo)
            V = lo;
        if (V > (s16)hi)
            V = hi;
        vv = V;
        s16 inv = cSAngle_Inv(&mAngleY);
        cSAngle_l* ayp = cameraFollowAngleShortCtor(&ay, inv);
        cameraFollowGlobeCtorAngles(&g, R, &vv, ayp);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g.mRadius - rr, k, rr);
        cameraFollowAngleSubtract(&g.mV, &t42, &mViewCache.mDirection.mV);
        cameraFollowAngleScale(&t42, &t40, k);
        cameraFollowAngleAdd(&mViewCache.mDirection.mV, &t30, &t40);
        mViewCache.mDirection.mV = angCt(&tmp, t30);
        if (mEventFlags & 0x100000) {
            s16 pi = cSAngle_Inv(&pdir);
            cameraFollowShortSubtractAngle(&t48, pi, &mViewCache.mDirection.mU);
            cameraFollowAngleScale(&t48, &t46, k);
            cameraFollowAngleAdd(&mViewCache.mDirection.mU, &t44, &t46);
            mViewCache.mDirection.mU = angCt(&tmp, t44);
        }
        cameraFollowGlobeXyz(&mViewCache.mDirection, &x);
        cXyz_pl(&mViewCache.mCenter, &e, &x);
        mViewCache.mEye.copy(e);
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

    cXyz pp;
    cXyz mul;
    cXyz d;
    cXyz s;
    positionOf(&pp, mpPlayerActor.get());
    pp.y = pp.y + 10.0f;
    f32 foot = footHeightOf(mpPlayerActor.get());
    f32 height = foot - groundHeight(&pp);
    if ((m360 != 0 || (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_SWIM_e)) || hyoiKamome()) &&
        !(dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK200_e))) {
        p->m388 = 0;
        p->m394 = 0.0f;
    } else if (p->m388 < 0x50) {
        s32 n = p->m388 + 1;
        p->m388 = n;
        f32 old = p->m394;
        f32 b = bezierRatio((f32)n / 80.0f, 1.25f);
        p->m394 = gabi::fmadds(f31 - old, b, old);
    }
    if (((dComIfGp_getPlayerStatus0(mPadId) &
          (daPyStts0_UNK1_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e | daPyStts0_UNK800000_e |
           daPyStts0_UNK2000000_e | daPyStts0_UNK4000000_e)) ||
         (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e))) &&
        mDMCSystem.field_0x0 == 0)
        setDMCAngle();
    if (((dComIfGp_getPlayerStatus0(mPadId) &
          (daPyStts0_UNK1_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e | daPyStts0_HANG_e |
           daPyStts0_UNK2000000_e)) ||
         (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e))) &&
        ((s16)rel <= angLoad(&angle270()) || (s16)rel >= angLoad(&angle90()))) {
        p->m3EC = 0.1f;
    } else {
        p->m3EC = v3;
    }
    if (p->m388 == 0) {
        p->m3F0 = v4;
        cpf(&mul.x, &p->m3EC);
        mul.y = v4;
        cpf(&mul.z, &p->m3EC);
    } else {
        f32 a = v4 * 0.1f;
        p->m3F0 = gabi::fmadds(1.0f - a, p->m394, a);
        if (((mEventFlags & 0x100000) && p->m3EC > 0.25f) || cb1Flying() || mdFlying() ||
            (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK40000_e))) {
            p->m3EC = gabi::fmadds(p->m394, 0.75f, 0.25f);
        }
        cpf(&mul.x, &p->m3EC);
        cpf(&mul.y, &p->m3F0);
        cpf(&mul.z, &p->m3EC);
    }
    bool bVar4 = true;
    bool bVar3 = false;
    if ((mEventFlags & 0x80) && mDirection.mRadius < v11)
        bVar3 = true;
    if ((mEventFlags & 0x100000) ||
        (dComIfGp_getPlayerStatus0(mPadId) &
         (daPyStts0_UNK1_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e | daPyStts0_HANG_e |
          daPyStts0_ROPE_AIM_e | daPyStts0_UNK800000_e | daPyStts0_UNK2000000_e |
          daPyStts0_UNK4000000_e)) ||
        (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_DEKU_LEAF_FAN_e | daPyStts1_UNK10000_e)) ||
        p->m388 != 0)
        bVar4 = false;
    if (mCurMode == 1) {
        cXyz a;
        attentionPos(&a, mpPlayerActor.get());
        if (tgt.y < a.y + 50.0f) {
            cSAngle_l dd;
            cSGlobe_l g;
            cXyz a2;
            cXyz x;
            cXyz e;
            directionOf(&dd, mpPlayerActor.get());
            s16 inv = cSAngle_Inv(&dd);
            cameraFollowGlobeCtor(&g, v11, (s16)0, inv); /* cSGlobe(f32, s16, s16) */
            attentionPos(&a2, mpPlayerActor.get());
            cameraFollowGlobeXyz(&g, &x);
            cXyz_pl(&a2, &e, &x);
            if (cameraFollowLineBlocked(this, &a2, &e, 0x7F))
                tgt.y = a2.y + 50.0f;
        }
    }
    cXyz_mi(&tgt, &d, &mViewCache.mCenter);
    cameraFollowVectorMultiply(&d, &s, &mul); /* cXyz * cXyz */
    PSVECAdd(&mViewCache.mCenter, &s, &mViewCache.mCenter);
    if (m780 == 0) {
        cXyz a;
        attentionPos(&a, mpPlayerActor.get());
        if (dComIfGp_getPlayerStatus0(mPadId) &
            (daPyStts0_HANG_e | daPyStts0_SWIM_e | daPyStts0_CRAWL_e)) {
            cXyz ep;
            eyePos(&ep, mpPlayerActor.get());
            a.y = ep.y + 30.0f;
        } else {
            a.y = a.y - 15.0f;
        }
        followWallCheck(this, &a);
    }
    cSGlobe_l g484;
    {
        cXyz d2;
        cXyz_mi(&mViewCache.mEye, &d2, &mViewCache.mCenter);
        cameraFollowGlobeFromVector(&g484, &d2);
    }

    s32 c392 = (s16)p->m392;
    if (c392 > 0 && c392 <= chargeTimer) {
        f32 b = bezierRatio((f32)c392 / (f32)chargeTimer, chargeBRatio);
        f32 old = p->m3B8;
        p->m3D8 = 1;
        p->m3B8 = (1.0f - old) * b;
    } else if ((mEventFlags & 0x100000) || cb1Flying()) {
        if (p->m3D8 != 0)
            p->m3B8 = 0.05f;
        f32 o = p->m3B8;
        p->m3D8 = 0;
        p->m3B8 = gabi::fmadds(1.0f - o, 0.2f, o);
    } else if (mdFlying() || hyoiKamome()) {
        p->m3D8 = 1;
    } else if (dComIfGp_getPlayerStatus1(mPadId) &
               (daPyStts1_DEKU_LEAF_FAN_e | daPyStts1_UNK40000_e)) {
        if (p->m3D8 != 0)
            p->m3B8 = 0.05f;
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
            cSAngle_l a;
            if (!(mStickMainPosYLast < 0.0f)) {
                f32 b = bezierRatio(mStickMainPosXLast, v23);
                cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 180.0f * b);
                f32 c = cameraFollowAngleCos(ap); /* Cos */
                v = 1.0f - gabi::fmadds(c, 0.5f, 0.5f);
            } else {
                f32 b = bezierRatio(mStickMainPosXLast, v24);
                cSAngle_l* ap = cameraFollowAngleFloatCtor(&a, 180.0f * b);
                f32 c = cameraFollowAngleCos(ap);
                v = 1.0f - gabi::fmadds(c, 0.25f, 0.75f);
            }
            p->m3B8 = v;
            v = v * mStickMainValueLast;
            p->m3B8 = v;
        }
        if (mEventFlags & 0x80000)
            p->m3B8 = v * 0.5f;
        else
            p->m3B8 = v * 0.1f;
        if ((dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_HANG_e | daPyStts0_UNK2000000_e)) ||
            (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e))) {
            s32 c = p->m38C;
            if (c < 0) {
                c = c - 1;
                p->m38C = (c <= -0x20) ? 0x10 : c;
            } else if (c == 0) {
                if ((s16)rel > angLoad(&angle270()) && (s16)rel < angLoad(&angle90()))
                    p->m38C = 1;
                else
                    p->m38C = -1;
            } else if (c < 0xF) {
                p->m3B8 = (f32)c * 0.033333335f;
                p->m38C = p->m38C + 1;
            } else if (!(dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK2000000_e))) {
                p->m3B8 = followStickTurn(this, &rel);
            }
        } else if (bVar2) {
            s32 c = p->m38C;
            if (c == 0 && ((s16)rel <= angLoad(&angle270()) || (s16)rel >= angLoad(&angle90()))) {
                p->m38C = 1;
            } else if (c < 0xF) {
                p->m3B8 = (f32)c * 0.033333335f;
                p->m38C = p->m38C + 1;
            } else if (dComIfGp_getPlayerStatus0(mPadId) &
                       (daPyStts0_UNK20_e | daPyStts0_UNK40_e)) {
                p->m3B8 = 0.15f;
            } else {
                p->m3B8 = followStickTurn(this, &rel);
            }
        } else {
            p->m3B8 = p->m3B8 * v20;
            p->m38C = 0;
        }
    }

    {
        cSAngle_l a4b4;
        cSAngle_l z;
        cSAngle_l t28;
        cSAngle_l t26;
        cSAngle_l t24;
        cSAngle_l t34;
        cSAngle_ct(&a4b4);
        if ((mEventFlags & 0x80) && !(mEventFlags & 0x80000) && mCurMode == 0 && p->m38C == 0) {
            a4b4 = angCt(&tmp, mDirection.mU);
        } else if (bVar2) {
            a4b4 = (s16)pdir;
            if (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK20_e | daPyStts0_UNK40_e))
                cameraFollowAngleAddAssign(&a4b4, &peep); /* += */
        } else {
            s16 inv = cSAngle_Inv(&pdir);
            a4b4 = angCt(&tmp, inv);
        }
        cSAngle_ct(&z, (s16)0);
        cameraFollowAngleSubtract(&a4b4, &t28, &g484.mU);
        f32 c = cameraFollowAngleCos(&g484.mV); /* Cos */
        f32 f = p->m3B8 * c;
        cameraFollowAngleScale(&t28, &t26, f);
        cameraFollowAngleAdd(&g484.mU, &t24, &t26);
        cameraFollowAngleAdd(&t24, &t34, &z);
        mViewCache.mDirection.mU = angCt(&tmp, t34);
    }

    cSAngle_l v4bc;
    cSAngle_ct(&v4bc);
    {
        cXyz pos;
        bool charging = (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK20000_e)) != 0;
        s32 n392 = (s16)p->m392;
        if (charging) {
            v4bc = (s16)chargeLat;
            if (n392 <= chargeTimer) {
                f32 b = bezierRatio((f32)(s32)(s16)p->m392 / (f32)chargeTimer, chargeBRatio);
                p->m3E0 = b;
                mEventFlags = mEventFlags | 0x4000000;
                p->m392 = (s16)(p->m392 + 1);
            } else {
                p->m3E0 = 1.0f;
            }
            f32 deg = angDegree(&v4bc);
            p->m3A0 = deg;
            p->m3BC = deg;
        } else {
            if (n392 != 0)
                p->m3E0 = 0.0f;
            p->m392 = 0;
            if (mCurMode == 1) {
                f32 a0 = p->m3A0;
                f32 nn = gabi::fmadds(v15 - a0, p->m3B8, a0);
                p->m3E0 = 0.5f;
                p->m3A0 = nn;
                v4bc = angCt(&tmp, (s16)gabi::ftoi(nn * 182.04445f)); /* cAngle::d2s */
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
                v4bc = angCt(&tmp, (s16)gabi::ftoi(nn * 182.04445f));
                if (mEventFlags & 0x80000) {
                    p->m3E0 = 0.0f;
                } else if (p->m3B4 == 0 && mMonitor.field_0x0C.x < 0.01f && mCurMode == 0) {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(0.1f - e, 0.05f, e);
                } else {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(f31 - e, w, e);
                }
            } else if ((dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK2000000_e)) ||
                       (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e))) {
                v4bc = (s16)g484.mV;
                f32 deg = angDegree(&v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
                p->m3E0 = 0.95f;
            } else if ((dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_DEKU_LEAF_FLY_e)) ||
                       cb1Flying() || mdFlying() || hyoiKamome()) {
                cXyz pos2;
                v4bc = (s16)g484.mV;
                positionOf(&pos2, mpPlayerActor.get());
                if (p->m3A4 < pos2.y) {
                    p->m3E0 = bezierRatio(p->m394, v18);
                } else {
                    f32 e = p->m3E0;
                    p->m3E0 = gabi::fmadds(0.75f - e, 0.15f, e);
                }
                f32 deg = angDegree(&v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
            } else {
                v4bc = (s16)g484.mV;
                f32 deg = angDegree(&v4bc);
                p->m3A0 = deg;
                p->m3BC = deg;
                p->m3E0 = bezierRatio(p->m394, v18);
            }
        }
        positionOf(&pos, mpPlayerActor.get());
        cpf(&p->m3A4, &pos.y);
    }
    if ((dComIfGp_getPlayerStatus0(mPadId) &
         (daPyStts0_UNK1_e | daPyStts0_UNK20_e | daPyStts0_UNK40_e | daPyStts0_HANG_e |
          daPyStts0_UNK2000000_e)) ||
        (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_UNK10000_e)))
        p->m3B4 = 1;
    else
        p->m3B4 = 0;
    if ((s16)v4bc < (s16)lo)
        cameraFollowAngleVal(&v4bc, &lo);
    else if ((s16)v4bc > (s16)hi)
        cameraFollowAngleVal(&v4bc, &hi);
    {
        cSAngle_l t24;
        cSAngle_l t26;
        cSAngle_l t28;
        cameraFollowAngleSubtract(&v4bc, &t24, &mViewCache.mDirection.mV);
        cameraFollowAngleScale(&t24, &t26, (f32)p->m3E0);
        cameraFollowAngleAdd(&mViewCache.mDirection.mV, &t28, &t26);
        s16 nv = angCt(&tmp, t28);
        mViewCache.mDirection.mV = nv;
        if (nv > (s16)lim80)
            mViewCache.mDirection.mV = angCt(&tmp2, lim80);
    }
    {
        cXyz x;
        cXyz e;
        f32 a = p->m398, b = p->m39C;
        f32 n398 = gabi::fmadds(v11 - a, v14, a);
        f32 n39C = gabi::fmadds(v10 - b, v14, b);
        f32 r3 = g484.mRadius;
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
        cameraFollowGlobeXyz(&mViewCache.mDirection, &x);
        cXyz_pl(&mViewCache.mCenter, &e, &x);
        p->m3CC.copy(e);
    }
    if (bVar3 && bVar4 && (sflags & dCamPrmFlg_UNK001)) {
        cSGlobe_l gg;
        cSAngle_l a;
        cXyz x;
        cXyz e;
        cameraFollowGlobeCopyCtor(&gg, &mViewCache.mDirection); /* cSGlobe(const cSGlobe&) */
        cSAngle_l* av = cameraFollowAngleFloatCtor(&a, v15);
        gg.mV = angCt(&tmp, *av);
        cameraFollowGlobeXyz(&gg, &x);
        cXyz_pl(&mViewCache.mCenter, &e, &x);
        if (cameraFollowLineBlocked(this, &mViewCache.mCenter, &e, 0x7F))
            mEventFlags = mEventFlags | 8;
    }
    cXyz_mi(&p->m3CC, &d, &mViewCache.mEye);
    cXyz_ml(&d, &s, 0.75f);
    PSVECAdd(&mViewCache.mEye, &s, &mViewCache.mEye);
    cXyz_mi(&mViewCache.mEye, &s, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, &s);
    f32 e8 = p->m3E8;
    if (mEventFlags & 8)
        p->m3DA = 1;
    e8 = gabi::fmadds(v29 - e8, 0.01f, e8);
    p->m3E8 = e8;
    f32 fv = mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds(v25 - fv, e8, fv);
    if (dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_DEKU_LEAF_FAN_e)) { /* DEKU_LEAF_FAN */
        f32 r = cM_rndFX(setup(this)->m078);                               /* mCamSetup.m078 */
        mViewCache.mFovy = mViewCache.mFovy + r;
    }
    if ((dComIfGp_getPlayerStatus1(mPadId) & (daPyStts1_DEKU_LEAF_FLY_e)) || cb1Flying() ||
        mdFlying() || hyoiKamome()) {
        f32 bank, ratio;
        if (height < 200.0f && (!hyoiKamome() || seagullDebug != 0)) {
            f32 q = height / 200.0f;
            ratio = gabi::fnmsubs(0.96f, q, 1.0f);
            bank = mStickMainPosXLast * q;
        } else {
            bank = mStickMainPosXLast;
            ratio = 0.04f;
        }
        cSAngle_l t32;
        cSAngle_l t4E;
        cSAngle_l t4C;
        cSAngle_l* sp = cameraFollowAngleShortCtor(&t32, (s16)(m07C << 7));
        f32 sn = cameraFollowAngleSin(sp); /* Sin */
        mViewCache.mFovy =
            gabi::fmadds(setup(this)->m07C, sn, mViewCache.mFovy); /* mCamSetup.m07C */
        f32 fan = cameraFollowFanBank(mCamSetup);                  /* dCamSetup_c::FanBank */
        cameraFollowAngleFloatCtor(&t4E, fan * bank);
        cameraFollowAngleSubtract(&t4E, &t4C, &mViewCache.mBank);
        cameraFollowAngleScale(&t4C, &t32, ratio);
        cameraFollowAngleAddAssign(&mViewCache.mBank, &t32); /* += */
        mEventFlags = mEventFlags | 0x400;
        if (hyoiKamome() && seagullDebug == 0) {
            /* HD: debug-register yaw of the centre around the eye while riding a seagull */
            cXyz dv;
            cXyz rv;
            cXyz c;
            cXyz_mi(&mViewCache.mCenter, &dv, &mViewCache.mEye);
            // HD: follow-camera reads this mutable matrix pointer; the shared stack helper uses a
            // fixed address.
            Mtx34* mtx = followMatrix.get(); /* mDoMtx_stack_c::now */
            f32 deg = angDegree(&mViewCache.mBank);
            f32 k = seagullYaw + -75.0f;
            mDoMtx_YrotS(mtx, (s16)gabi::ftoi(deg * k)); /* mDoMtx_YrotS */
            MtxPosition(&dv, &rv);                       /* MtxPosition */
            cXyz_pl(&mViewCache.mEye, &c, &rv);
            mViewCache.mCenter.copy(c);
            f32 lim = seagullFovy + 60.0f;
            if (mViewCache.mFovy > lim)
                mViewCache.mFovy = lim;
        }
    }
    return true;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 02008FEC void cameraFollowLineCtor(dBgS_LinChk*)
ext 02008B4C void cameraFollowLineDtor(dBgS_LinChk*, s32)
ext 020073AC void cameraFollowGlobeXyz(cSGlobe_l*, cXyz*)
ext 02007324 void cameraFollowGlobeFromVector(cSGlobe_l*, cXyz*)
ext 0200658C cSAngle_l* cameraFollowAngleShortCtor(cSAngle_l*, s32)
ext 020066C0 cSAngle_l* cameraFollowAngleFloatCtor(cSAngle_l*, f32)
ext 0200693C void cameraFollowAngleScale(cSAngle_l*, cSAngle_l*, f32)
ext 02006894 void cameraFollowAngleAdd(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 020068B0 void cameraFollowAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 02007068 void cameraFollowGlobeCtor(cSGlobe_l*, f32, s16, s16)
ext 0201AF1C void cameraFollowVectorMultiply(cXyz*, cXyz*, cXyz*)
ext 020065EC void cameraFollowShortSubtractAngle(cSAngle_l*, s16, cSAngle_l*)
ext 02006638 void cameraFollowAngleVal(cSAngle_l*, cSAngle_l*)
ext 024FCBE8 bool cameraFollowLineBlocked(dCamera_c*, cXyz*, cXyz*, u32)
ext 024EFB08 void cameraFollowMoveBgPosition(dBgS*, cBgS_PolyInfo*, s32, cXyz*, u32, u32)
ext 024F6F9C f32 cameraFollowBezierRatio(f32, f32)
ext 02006720 f32 cameraFollowAngleDegree(const cSAngle_l*)
ext 02006814 f32 cameraFollowAngleSin(cSAngle_l*)
ext 02006838 f32 cameraFollowAngleCos(cSAngle_l*)
ext 020069A0 void cameraFollowAngleScaleAssign(cSAngle_l*, f32)
ext 02006694 void cameraFollowAngleValFloat(cSAngle_l*, f32)
ext 02007248 void cameraFollowGlobeCtorAngles(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*)
ext 024F7AC4 f32 cameraFollowHorizontalDistance(cXyz*, cXyz*)
ext 025028A0 f32 cameraFollowLimit(f32, f32, f32)
ext 020068CC void cameraFollowAngleAddAssign(cSAngle_l*, cSAngle_l*)
ext 02007178 void cameraFollowGlobeCopyCtor(cSGlobe_l*, cSGlobe_l*)
ext 024F78F0 f32 cameraFollowFanBank(u8*)
ext 020084C8 cM3dGPla* cameraFollowGetTriPla(dBgS*, u32, u32)
data followMatrix 1018C7B0
data cb1FlyingFlag 101BDC2D
data mdFlyingFlag 101D5F3E
data hyoiKamomeFlag 101D5F45
data seagullDebug 1047B688
data seagullYaw 1047C038
data seagullFovy 1047C03C
*/
