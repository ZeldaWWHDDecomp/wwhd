/**
 * d_camera_lockon.cpp (WWHD)
 * Follow camera dCamera_c: lockonCamera (Z-targeting).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp, a "Nonmatching" draft there) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_lockon.h"
#include "gc_api/d/d_com_inf_game.h"

using namespace lockonDetail;

// HD: shared status accessors retain 32-bit guest-address wrap for unchecked pad IDs.

// HD: word copies preserve NaN payloads in the original stores.
static inline void copyFloatBits(void* dst, const void* src) {
    *gabi::at<be<u32>>(gabi::ea(dst)) = gabi::at<be<u32>>(gabi::ea(src))->get();
}
static s16 lockonAngleZero;
static s16 lockonAngle180;

// HD: the side flip negates the angle difference, and hidden-player sway uses the target's
// longitude and latitude. The boomerang distance is computed but unused. DefaultRadius's
// accepted value is used directly; otherwise it is eased. These differ from the GC draft.
/* 025052E8 */
bool dCamera_c::lockonCamera(s32 style) {
    auto& setup = *reinterpret_cast<LockonSetupHD*>(mCamSetup);
    LockonWork* work = (LockonWork*)mWork;
    gabi::Local<cSAngle_l> chargeLat, targetLongitude, lockonLongitude, angleDifference,
        longitudeError, viewLongitude, viewLatitude, t, tmp;
    f32 lat0 = setup.mChargeLatitude.get();
    f32 m0B4 = setup.m0B4.get();
    s32 m0B0 = setup.m0B0.get();
    lockon_angleFromDegrees(chargeLat.get(), lat0); /* mCamSetup.ChargeLatitude() */
    f32 chargeBRatio = setup.mChargeBRatio.get();
    s32 chargeTimer = setup.mChargeTimer.get();
    f32 v3 = camParamVal(style, 3);
    f32 v4 = camParamVal(style, 4);
    u32 attn = gabi::ea(dComIfGp_getAttention()); /* dComIfGp_getAttention() */
    if (m108 == 0) {
        gabi::Local<cXyz> a, d;
        m100 = 1;
        m101 = 1;
        m102 = 1;
        work->m378 = 0x4C4F434B; /* 'LOCK' */
        work->m380 = 0;
        work->m38C = 0;
        work->m39C = 0;
        work->m384 = 1.0f;
        work->m390.copy(mViewCache.mCenter);
        attentionPos(a, mpPlayerActor.get());
        cXyz_mi(&mViewCache.mCenter, d, a);
        cSGlobe_Val(&work->m3A8, d);
        work->m3B4 = 0.0f;
        work->m3B0 = 0.0f;
        copyFloatBits(&work->m3B8, &setup.mBaseCushion); /* mCamSetup.Cushion4Base() */
        work->m39D = 0;
        work->m388 = 0;
        work->m3A0 = 0;
        work->m3A4 = 0;
        if (mpLockonTarget.get() != nullptr)
            radiusActorInSight(mpPlayerActor.get(), mpLockonTarget.get());
    }
    if (m31D != 0)
        lockon_MoveBgMatrixCrrPos(gabi::ea(dComIfG_Bgsp()),
                                  gabi::ea(static_cast<cBgS_PolyInfo*>(&mBG.m5C.m04)), 1,
                                  &work->m390, 0, 0); /* MoveBgMatrixCrrPos */
    if (!lockon_LockonTruth(attn) &&
        (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_BOOMERANG_WAIT_e))
        work->m39C = 1; /* LockonTruth, BOOMERANG_WAIT */
    else
        work->m39C = 0;
    if (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_UNK400_e) {
        if (work->m39D == 0) {
            u32 vib = gabi::ea(dComIfGp_getVibration());
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            lockon_StartShock(vib, 2, 0x10, v.get()); /* dVibration_c::StartShock */
        }
        work->m39D = 1;
    } else {
        work->m39D = 0;
    }
    u32 af = gabi::at<LockonAttentionHD>(gabi::ea(dComIfGp_getAttention()))
                 ->mFlags.get(); /* attention flags */
    if ((af & 8) || (af & 0x20)) {
        mEventFlags = mEventFlags & ~0x100u;
        m11C = 0;
        m108 = 0;
    }

    gabi::Local<cSGlobe_l> targetDirection;
    lockon_globeDefault(targetDirection.get());
    f32 curveW = setup.mCurveWeight.get();            /* mCamSetup.CurveWeight() */
    f32 relDist = lockon_LockonReleaseDistanse(attn); /* LockonReleaseDistanse */
    f32 dist = 10000.0f;
    f32 ratio;
    if (dComIfGp_getPlayerStatus0(mPadId) &
        (daPyStts0_UNK40_e | daPyStts0_UNK20_e | daPyStts0_UNK1_e)) {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(dir);
        lockon_globeValShort(targetDirection.get(), (f32)setup.mParallelDist.get(), (s16)0,
                             inv); /* Val(ParallelDist, 0, inv) */
        ratio = 1.0f;
        mpLockonTarget = nullptr;
    } else if (mpLockonTarget.get() != nullptr) {
        gabi::Local<cXyz> f0, fc, pp, d;
        attentionPos(f0, mpLockonTarget.get());
        attentionPos(fc, mpPlayerActor.get());
        fopAc_ac_c* tg = mpLockonTarget.get();
        if (tg != nullptr && tg->mProcName.get() == fpcNm_BDK_e) { /* BDK */
            positionOf(pp, tg);
            copyFloatBits(&f0->x, &pp->x);
            positionOf(pp, mpLockonTarget.get());
            copyFloatBits(&f0->z, &pp->z);
        }
        cXyz_mi(f0, d, fc);
        cSGlobe_Val(targetDirection, d);
        ratio = targetDirection->mRadius / relDist;
        if (ratio > 1.0f)
            ratio = 1.0f;
        dist =
            lockon_xyzHorizontalDistance(f0.get(), fc.get()); /* dCamMath::xyzHorizontalDistance */
    } else {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        lockon_globeVal(targetDirection.get(), (f32)setup.mParallelDist.get(),
                        reinterpret_cast<cSAngle_l*>(&lockonAngleZero), dir.get());
        ratio = 1.0f;
    }
    cSAngle_ct(targetLongitude, &targetDirection->mU);
    s16 lon = lockon_LockonLongitude(gabi::ea(mCamParam), ratio); /* LockonLongitude */
    cSAngle_ct(lockonLongitude, lon);
    u32 c11C = m11C;
    u32 fl = mEventFlags;
    if (c11C < (u32)m0B0) {
        if (!(fl & 0x100))
            lockon_angleMultiplyAssign(lockonLongitude.get(), (f32)(u32)m108 / (f32)m0B0); /* *= */
    } else {
        mEventFlags = fl | 0x100;
    }
    {
        s16 inv = cSAngle_Inv(&mViewCache.mDirection.mU);
        lockon_shortMinusAngle(angleDifference.get(), inv, targetLongitude.get());
    }
    s16 zero = lockonAngleZero;
    if ((s16)*angleDifference < zero) {
        work->m3A0 = 0;
        lockon_angleSubtractAssign(targetLongitude, lockonLongitude);
        lockon_angleNegate(angleDifference.get(), t.get()); /* -angleDifference */
        *angleDifference = (s16)*t;
    } else {
        work->m3A0 = 1;
        lockon_angleAddAssign(targetLongitude, lockonLongitude);
    }
    gabi::Local<cXyz> playerAttention;
    attentionPos(playerAttention, mpPlayerActor.get());
    bool playerHidden = false;
    if (mEventFlags & 0x80080) {
        gabi::Local<cXyz> a114;
        attentionPos(a114, mpPlayerActor.get());
        if (!pointInSight(a114)) {
            if (work->m388 == 0)
                work->m3A4 = (work->m3A0 != 1) ? 1 : 0;
            playerHidden = true;
            work->m388 = 60;
        }
    }
    if (lockon_lineBGCheckBack(this, &mViewCache.mCenter, playerAttention.get(),
                               0x7F) && /* lineBGCheckBack */
        lockon_lineBGCheck(this, &mViewCache.mEye, &mViewCache.mCenter, 0x7F)) { /* lineBGCheck */
        work->m388 = 59; /* 60, minus this frame */
        playerHidden = true;
    } else if (work->m388 != 0) {
        s32 n = work->m388 - 1;
        work->m388 = n;
        if (n == 0 && !(mStickMainValueLast > 0.1f))
            work->m388 = 1;
        playerHidden = true;
    }
    f32 cRatio = 1.0f - __builtin_fabsf(mStickCPosYLast);
    {
        f32 rr = lockon_RadiusRatio(gabi::ea(mCamParam),
                                    (f32)mViewCache.mDirection.mRadius); /* RadiusRatio */
        lockon_customRBRatio(rr, curveW);
    }
    if (mEventFlags & 0x10) {
        f32 b = 0.01f;
        work->m3B8 = b;
        work->m3B8 = gabi::fmadds(b - b, setup.mCusCus.get(), b);
    } else if (m360 == 0) {
        f32 b = work->m3B8;
        work->m3B8 = gabi::fmadds(setup.mJumpCushion.get() - b, setup.mCusCus.get(),
                                  b); /* Cushion4Jump, CusCus */
    } else {
        f32 b = work->m3B8;
        work->m3B8 =
            gabi::fmadds(setup.mBaseCushion.get() - b, setup.mCusCus.get(), b); /* Cushion4Base */
    }
    copyFloatBits(&work->m390.x, &playerAttention->x);
    copyFloatBits(&work->m390.z, &playerAttention->z);
    {
        f32 h = lockon_LockonCenterHeight(gabi::ea(mCamParam), ratio); /* LockonCenterHeight */
        if (playerHidden)
            h = h + 25.0f;
        f32 y = work->m390.y;
        work->m390.y = gabi::fmadds((playerAttention->y + h) - y, work->m3B8, y);
    }
    f32 R230 = targetDirection->mRadius;
    f64 d21 = (f64)R230 * 0.05;
    f64 d20;
    if (mpLockonTarget.get() != nullptr) {
        f32 c1 = lockon_angleCos(angleDifference.get()); /* Cos */
        lockon_angleMultiply(&targetDirection->mV, t, 1.3f);
        f32 c2 = lockon_angleCos(t.get());
        f64 diff = (f64)__builtin_fabsf(c1) - (f64)__builtin_fabsf(c2);
        f64 sel = (diff >= 0.0) ? (f64)c2 : (f64)c1; /* fsel */
        f64 w = gabi::fmadd(sel, -0.5, 0.5);
        d20 = ((f64)(f32)targetDirection->mRadius - (d21 + d21)) * __builtin_fabs(w);
    } else {
        f32 c = lockon_angleCos(angleDifference.get());
        f64 w = gabi::fmadd((f64)c, -0.5, 0.5);
        d20 = (f64)R230 * __builtin_fabs(w);
    }
    {
        gabi::Local<cSAngle_l> a260, t36, t34, a264, a268, t48, t46, t44;
        cSAngle_ct(a260, &targetDirection->mU);
        f32 b4 = work->m3B4;
        f32 nb4 = gabi::fmadds(v3 - b4, setup.mCusCus.get(), b4);
        f32 b0 = work->m3B0;
        work->m3B4 = nb4;
        work->m3B0 = gabi::fmadds(v4 - b0, setup.mCusCus.get(), b0);
        lockon_angleSubtract(a260, t36, &work->m3A8.mU);
        lockon_angleMultiply(t36, t34, work->m3B4);
        lockon_angleAdd(&work->m3A8.mU, a264, t34);
        cSAngle_ct(a268);
        f32 R0 = work->m3A8.mRadius;
        f32 newR;
        if (playerHidden) {
            newR = R0 * 0.75f;
            lockon_angleSubtract(&targetDirection->mV, t48, &work->m3A8.mV);
            lockon_angleMultiply(t48, t46, 0.05f);
            lockon_angleAdd(&work->m3A8.mV, t44, t46);
            lockon_angleVal(a268.get(), t44.get()); /* Val(const cSAngle&) */
        } else {
            f32 s = (f32)(d20 + d21);
            newR = gabi::fmadds(gabi::fmsubs(s, work->m384, R0), work->m3B0, R0);
            lockon_angleSubtract(&targetDirection->mV, t48, &work->m3A8.mV);
            lockon_angleMultiply(t48, t46, work->m3B4);
            lockon_angleAdd(&work->m3A8.mV, t44, t46);
            lockon_angleVal(a268.get(), t44.get());
        }
        lockon_globeVal(&work->m3A8, newR, a268.get(), a264.get()); /* Val(f32, V, U) */
        gabi::Local<cXyz> x, c;
        lockon_globeXyz(&work->m3A8, x.get());
        cXyz_pl(&work->m390, c, x);
        mViewCache.mCenter.copy(*c);
    }
    if (mpLockonTarget.get() != nullptr && mLockOnActorId != 0xFFFFFFFFu) {
        gabi::Local<cXyz> a120;
        attentionPos(a120, mpPlayerActor.get());
        if (lockon_lineBGCheck(this, a120.get(), &mViewCache.mCenter, 0x7F))
            ForceLockOff(mLockOnActorId);
    }
    gabi::Local<cSGlobe_l> oldEyeDirection, viewEyeDirection;
    {
        gabi::Local<cXyz> d, d2;
        cXyz_mi(&mEye, d, &mViewCache.mCenter);
        lockon_globeFromVector(oldEyeDirection.get(), d.get());
        cXyz_mi(&mViewCache.mEye, d2, &mViewCache.mCenter);
        lockon_globeFromVector(viewEyeDirection.get(), d2.get());
    }
    cSAngle_ct(viewLongitude, &mViewCache.mDirection.mU);
    cSAngle_ct(viewLatitude, &mViewCache.mDirection.mV);
    f32 R = mViewCache.mDirection.mRadius;
    lockon_angleSubtract(angleDifference, longitudeError, lockonLongitude);
    f32 cw = setup.mCurveWeight.get();

    if (playerHidden) {
        gabi::Local<cSAngle_l> a278, t54, t52, t50;
        cSAngle_ct(a278);
        if (work->m3A4 == 1)
            lockon_angleValDegrees(a278.get(), 15.0f);
        else
            lockon_angleValDegrees(a278.get(), -15.0f);
        s16 inv = cSAngle_Inv(&targetDirection->mU);
        lockon_shortPlusAngle(t54.get(), inv, a278.get()); /* s16 + cSAngle */
        lockon_angleSubtract(t54, t52, viewLongitude);
        lockon_angleMultiply(t52, t50, 0.05f);
        lockon_angleAddAssign(viewLongitude, t50);
    } else {
        f32 k;
        if (mpLockonTarget.get() == nullptr) {
            k = setup.m028.get(); /* mCamSetup.m028 */
        } else if (!(mEventFlags & 0x100)) {
            s16 l = *lockonLongitude;
            if (l == zero) {
                k = 0.15f;
            } else {
                u32 n11C = m11C;
                f32 frac = (f32)n11C / (f32)m0B0;
                f32 x = -((f32)(s16)*longitudeError / (f32)l);
                f32 cr = lockon_customRBRatio(x, cw);
                f32 a = camParamVal(style, 21) * cr;
                k = gabi::fmadds(m0B4 - a, 1.0f - frac, a);
            }
        } else {
            s16 l = *lockonLongitude;
            if ((s16)*angleDifference < l) {
                f32 x = -((f32)(s16)*longitudeError / (f32)l);
                f32 cr = lockon_customRBRatio(x, cw);
                k = camParamVal(style, 21) * cr;
            } else {
                gabi::Local<cSAngle_l> a27c, t6e, t56;
                lockon_angleFromDegrees(a27c.get(), 45.0f);
                cSAngle_l* c135 = lockon_angleFromDegrees(t6e.get(), 135.0f);
                s16 den;
                if ((s16)*lockonLongitude > (s16)*c135) {
                    lockon_angleSubtract(reinterpret_cast<cSAngle_l*>(&lockonAngle180), t56,
                                         lockonLongitude); /* _180 - lockonLongitude */
                    den = *t56;
                    *a27c = den;
                } else {
                    den = *a27c;
                }
                f32 b = lockon_rationalBezierRatio((f32)(s16)*longitudeError / (f32)den, cw);
                k = camParamVal(style, 20) * b;
                if (dist < 100.0f)
                    k = k * lockon_rationalBezierRatio(dist / 100.0f, 1.0f);
            }
        }
        gabi::Local<cSAngle_l> t1c;
        s16 inv = cSAngle_Inv(targetLongitude);
        lockon_shortMinusAngle(t1c.get(), inv, &mViewCache.mDirection.mU);
        *longitudeError = (s16)*t1c;
        lockon_angleDegree(longitudeError.get()); /* Degree (unused) */
        lockon_angleMultiply(longitudeError, t1c, k);
        lockon_angleAddAssign(viewLongitude, t1c);
    }

    bool radius280;
    if (dComIfGp_getPlayerStatus1(mPadId) & daPyStts1_UNK20000_e) {
        s32 n = work->m380;
        if (n <= chargeTimer) {
            gabi::Local<cSAngle_l> t5a, t58;
            f32 fr = (f32)n / (f32)chargeTimer;
            lockon_angleSubtract(chargeLat, t5a, viewLatitude);
            f32 b = lockon_rationalBezierRatio(fr, chargeBRatio);
            lockon_angleMultiply(t5a, t58, b);
            lockon_angleAddAssign(viewLatitude, t58);
            mEventFlags = mEventFlags | 0x4000000;
            work->m380 = work->m380 + 1;
        } else {
            *viewLatitude = (s16)*chargeLat;
        }
        radius280 = playerHidden;
    } else if (playerHidden) {
        gabi::Local<cSAngle_l> t30, t1e, t5c;
        cSAngle_ct(t30);
        lockon_angleMultiply(&targetDirection->mV, t1e, 0.7f);
        *t30 = (s16)*t1e;
        lockon_angleAdd(t30, t5c, viewLatitude);
        lockon_angleMultiply(t5c, t1e, 0.1f);
        lockon_angleSubtractAssign(viewLatitude, t1e);
        radius280 = true;
    } else {
        work->m380 = 0;
        if (mEventFlags & 0x10) {
            gabi::Local<cSAngle_l> t60, t5e;
            s16 lat = lockon_LockonLatitude(gabi::ea(mCamParam), ratio); /* LockonLatitude */
            lockon_shortMinusAngle(t60.get(), lat, viewLatitude.get());
            lockon_angleMultiply(t60, t5e, 0.05f);
            lockon_angleAddAssign(viewLatitude, t5e);
        } else if (m360 == 0 && !(dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_UNK400_e)) {
            gabi::Local<cSAngle_l> t66, t64, t62;
            lockon_angleSubtract(&viewEyeDirection->mV, t66, viewLatitude);
            lockon_angleMultiply(t66, t64, cRatio);
            f32 c = lockon_angleCos(&mViewCache.mDirection.mV);
            lockon_angleMultiply(t64, t62, __builtin_fabsf(c));
            lockon_angleAddAssign(viewLatitude, t62);
        } else {
            fopAc_ac_c* pl = mpPlayerActor.get();
            bool eq;
            if (pl != nullptr && pl->mProcName.get() == fpcNm_PLAYER_e) { /* is_player */
                gabi::Local<be<u32>> key;
                u32 vt = pl->__vtbl.get();
                u32 id = gabi::call_ptr<u32>(
                    gabi::at<LockonPlayerVtableHD>(vt)->getThrowBoomerangID.get(),
                    pl); /* getThrowBoomerangID */
                *key = id;
                fopAc_ac_c* boom = nullptr;
                if (id != 0xFFFFFFFFu)
                    boom = fopAcIt_Judge(0x025E1234, key.get());
                eq = mpLockonTarget.get() == boom;
            } else {
                eq = mpLockonTarget.get() == nullptr;
            }
            if (eq && m784 != 0) {
                gabi::Local<cSAngle_l> t68;
                lockon_angleMultiply(viewLatitude, t68, setup.m028.get());
                lockon_angleSubtractAssign(viewLatitude, t68);
            } else {
                gabi::Local<cSAngle_l> t6c, t6a;
                s16 lat = lockon_LockonLatitude(gabi::ea(mCamParam), ratio);
                lockon_shortMinusAngle(t6c.get(), lat, viewLatitude.get());
                lockon_angleMultiply(t6c, t6a, setup.m028.get());
                lockon_angleAddAssign(viewLatitude, t6a);
            }
            if (mpLockonTarget.get() != nullptr &&
                (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_UNK400_e)) {
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
        if (dComIfGp_getPlayerStatus0(mPadId) & (daPyStts0_UNK1000000_e | daPyStts0_SHIP_RIDE_e)) {
            f32 deg = lockon_angleDegree(viewLatitude.get());
            f32 lo = camParamVal(style, 16);
            if (deg < lo)
                lockon_angleValDegrees(viewLatitude.get(), lo);
            f32 hi = camParamVal(style, 17);
            if (deg > hi)
                lockon_angleValDegrees(viewLatitude.get(), hi);
        } else {
            gabi::Local<be<s16>> lr;
            *lr = (s16)*viewLatitude;
            if (!lockon_CheckLatitudeRange(gabi::ea(mCamSetup), lr.get()))
                cSAngle_Val(viewLatitude, *lr); /* CheckLatitudeRange */
        }
        radius280 = false;
    }
    f32 newR;
    if (radius280) {
        newR = gabi::fmadds(280.0f - R, 0.05f, R);
    } else {
        gabi::Local<be<f32>> r70;
        copyFloatBits(r70.get(), &viewEyeDirection->mRadius);
        if (!lockon_DefaultRadius(gabi::ea(mCamParam), r70.get())) { /* DefaultRadius */
            newR = gabi::fmadds(*r70 - R, 0.05f, R);
        } else {
            newR = *r70;
        }
    }
    gabi::Local<cXyz> x, e;
    lockon_globeVal(&mViewCache.mDirection, newR, viewLatitude.get(), viewLongitude.get());
    lockon_globeXyz(&mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, e, x);
    mViewCache.mEye.copy(*e);
    f32 fv = lockon_LockonFovy(gabi::ea(mCamParam), ratio); /* LockonFovy */
    f32 cur = mViewCache.mFovy;
    f32 m028 = setup.m028.get();
    mEventFlags = mEventFlags | 0x2000;
    mViewCache.mFovy = gabi::fmadds(fv - cur, m028, cur);
    return true;
}

/* WWHD glue
data lockonAngleZero 101FF354
data lockonAngle180 101FF35A
ext 020066C0 cSAngle_l* lockon_angleFromDegrees(cSAngle_l*, f32)
ext 024EFB08 void lockon_MoveBgMatrixCrrPos(u32, u32, s32, cXyz*, u32, u32)
ext 024EDFCC bool lockon_LockonTruth(u32)
ext 025CB374 void lockon_StartShock(u32, s32, s32, cXyz*)
ext 02007100 void lockon_globeDefault(cSGlobe_l*)
ext 024EE49C f32 lockon_LockonReleaseDistanse(u32)
ext 02006FE4 void lockon_globeValShort(cSGlobe_l*, f32, s16, s16)
ext 024F7AC4 f32 lockon_xyzHorizontalDistance(cXyz*, cXyz*)
ext 020071E0 void lockon_globeVal(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*)
ext 024F748C s16 lockon_LockonLongitude(u32, f32)
ext 020069A0 void lockon_angleMultiplyAssign(cSAngle_l*, f32)
ext 020065EC void lockon_shortMinusAngle(cSAngle_l*, s16, cSAngle_l*)
ext 02006880 void lockon_angleNegate(cSAngle_l*, cSAngle_l*)
ext 025051BC bool lockon_lineBGCheckBack(dCamera_c*, cXyz*, cXyz*, u32)
ext 024FCBE8 bool lockon_lineBGCheck(dCamera_c*, cXyz*, cXyz*, u32)
ext 024F7370 f32 lockon_RadiusRatio(u32, f32)
ext 024F75CC f32 lockon_LockonCenterHeight(u32, f32)
ext 02006838 f32 lockon_angleCos(cSAngle_l*)
ext 02006638 void lockon_angleVal(cSAngle_l*, cSAngle_l*)
ext 020073AC void lockon_globeXyz(cSGlobe_l*, cXyz*)
ext 02007324 void lockon_globeFromVector(cSGlobe_l*, cXyz*)
ext 02006694 void lockon_angleValDegrees(cSAngle_l*, f32)
ext 020065DC void lockon_shortPlusAngle(cSAngle_l*, s16, cSAngle_l*)
ext 02006720 f32 lockon_angleDegree(cSAngle_l*)
ext 024F7520 s16 lockon_LockonLatitude(u32, f32)
ext 024F7878 bool lockon_CheckLatitudeRange(u32, be<s16>*)
ext 024F7400 bool lockon_DefaultRadius(u32, be<f32>*)
ext 024F75B4 f32 lockon_LockonFovy(u32, f32)
ext 024F6F9C f32 lockon_rationalBezierRatio(f32, f32)
ext 024F7194 f32 lockon_customRBRatio(f32, f32)
ext 020068B0 void lockon_angleSubtract(const void*, cSAngle_l*, const void*)
ext 0200693C void lockon_angleMultiply(const void*, cSAngle_l*, f32)
ext 02006894 void lockon_angleAdd(const void*, cSAngle_l*, const void*)
ext 020068CC void lockon_angleAddAssign(void*, const void*)
ext 020068E0 void lockon_angleSubtractAssign(void*, const void*)
*/
