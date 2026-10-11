/**
 * d_camera_subject.cpp (WWHD)
 * Follow camera dCamera_c: first-person (subject) aiming.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_subject.h"
using namespace subjectDetail;

static u32 subjectGamepadAddress;
static s16 subjectAngleZero;
static u8 subjectAutoForcus;
static SubjectStylesHD subjectStyles;
static u32 subjectCheckVtable;
static u32 subjectPolyVtable;
static u32 subjectCameraPassVtable;
static u32 subjectCameraGroupVtable;
static u32 subjectPassVtable;
static u32 subjectGroupVtable;
static u32 subjectBasePolyVtable;
// Style and camera indices retain the original 32-bit guest address arithmetic.
static inline u16 subjectStyleFlags(s32 style) {
    return gabi::at<SubjectStyleHD>(gabi::ea(&subjectStyles.mStyle[0]) +
                                    (u32)style * sizeof(SubjectStyleHD))
        ->mFlags;
}

#include <bit>

/* round25(): frC operand rounding of the PowerPC single-precision multiply (from the recompiler's ppc.h) */

// HD: aiming also reads the GamePad gyro (a sead controller at *101F5088): with the C stick
// released, the pointing direction drives the angles (x30, Y inverted while swimming); with the C
// stick held its value is shaped by a rational Bezier curve and split by its angle (Y inversion
// from the save options); swimming uses a separate pitch range; the crawl status zeroes the result
/* 02506964 */
bool dCamera_c::CalcSubjectAngle(s16* outV, s16* outU) {
    SubjectWork* work = (SubjectWork*)mWork;
    auto& setup = *reinterpret_cast<SubjectSetupHD*>(mCamSetup);
    s32 pad = mPadId;
    bool swim = (dComIfGp_getPlayerStatus1(pad) & daPyStts1_SWIM_HD_e) != 0;
    bool res = true;
    if (reinterpret_cast<SubjectEventManagerHD*>(dComIfGp_getPEvtManager())->mCameraPlay.get() !=
            0 &&
        dComIfGp_getMiniGameType() != 8)
        return false;
    if (work->m378 != SUBJECT_WORK_TAG /* 'SUBJ' */)
        return false;
    if (m100 == 0)
        return false;
    if (work->m37C == 0)
        res = false;
    s32 st = work->m380;
    f32 pitchMax = camParamVal(st, 19);
    f32 yawMax = camParamVal(st, 24);
    f32 speed = camParamVal(st, 21);
    if (swim)
        pitchMax = setup.mSwimLatitude.get();
    f32 slow = setup.m030.get(); /* mCamSetup.m030 */
    gabi::Local<cSAngle_l> a, b, c;
    if (!res) {
        subject_angleFromDegrees(a.get(), yawMax * work->m384);
        subject_angleFromDegrees(b.get(), pitchMax * work->m388);
        subject_shortPlusAngle(c.get(), (s16)*a, &work->m3BA); /* (s16) + cSAngle */
        *(be<s16>*)outU = (s16)*c;
        *(be<s16>*)outV = (s16)*b;
    }
    // HD: GamePad input
    u32 ctl = subjectGamepadAddress;
    f32 sx = gabi::at<SubjectGamepadHD>(ctl)->mRightStick.x.get();
    f32 sy = gabi::at<SubjectGamepadHD>(ctl)->mRightStick.y.get();
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
        u32 opt = subject_getAimOptions(gabi::ea(dComIfGs_info()) + sizeof(dSv_info_c));
        bool invert = gabi::at<SubjectAimOptionsHD>(opt)->mInvertY.get() != 0;
        x = subject_CPadGetStickX((s32)mPadId); /* C stick x */
        y = subject_CPadGetStickY((s32)mPadId) * (invert ? 1.0f : -1.0f);
    } else {
        gabi::Local<SubjectRotationHD> rot;
        gabi::Local<cXyz> r2, r1;
        subject_getGyroOrientation(rot.get(), subjectGamepadAddress); /* gyro orientation */
        subject_getMatrixRow(r2.get(), rot.get(), 2);
        gabi::Local<cXyz> r2c;
        r2c->copy(*r2);
        subject_getMatrixRow(r1.get(), rot.get(), 1);
        x = (r2c->x - r1->x) * 30.0f;
        y = r2c->y * 30.0f;
        if (swim)
            y = -y;
    }
    subject_updateGyro(subjectGamepadAddress);
    fopAc_ac_c* player = mpPlayerActor.get();
    gabi::Local<cSAngle_l> tmp;
    if (player != nullptr && fpcM_GetName(player) == fpcNm_PLAYER_e) {
        cSAngle_l* r = subject_angleFromShort(
            tmp.get(),
            (s16) reinterpret_cast<daPy_py_c*>(player)->getBodyAngleX()); /* getBodyAngleX */
        work->m3B8 = (s16)*r;
    } else {
        cSAngle_l* r = subject_angleFromShort(tmp.get(), (s16)player->shape_angle.x);
        work->m3B8 = (s16)*r;
    }
    cSAngle_l* r = subject_angleFromShort(tmp.get(), (s16)mpPlayerActor.get()->shape_angle.y);
    work->m3BA = (s16)*r;
    f32 yaw = x, pitch = y;
    f32 eyeY = mEye.y;
    bool keep = eyeY > m354 + 5.0f && eyeY > mBG.m5C.m58 + 5.0f;
    if (!keep) {
        if (work->m388 < 0.0f) {
            if (!(pitch > 0.0f))
                pitch = 0.0f;
        } else {
            if (pitch > 0.0f)
                pitch = 0.0f;
        }
    }
    pad = mPadId;
    if (dComIfGp_getPlayerStatus0(pad) & daPyStts0_CRAWL_e)
        pitch = 0.0f; /* CRAWL */
    pad = mPadId;
    f32 m384;
    if (dComIfGp_getPlayerStatus0(pad) & daPyStts0_UNK40000_e) {
        m384 = 0.0f;
        work->m384 = m384;
    } else if (subjectStyleFlags(work->m380) & dCamPrmFlg_UNK020) {
        m384 = -yaw;
        work->m384 = m384;
        work->m388 = pitch;
    } else {
        f32 k = speed;
        if (big) {
            f32 r1v = subject_rationalBezierRatio(mag, 0.11f); /* dCamMath::rationalBezierRatio */
            f32 r08 = subject_rationalBezierRatio(0.8f, 0.11f);
            f32 len = r1v * r1v;
            if (mag < 0.8f)
                len = (mag / 0.8f) * (r08 * r08);
            f32 ang = subject_atan2f(y, x); /* atan2f */
            s16 s1 = subject_cM_rad2s(ang); /* cM_rad2s */
            yaw = len * cM_scos(s1);
            s16 s2 = subject_cM_rad2s(ang);
            pitch = len * cM_ssin(s2);
        }
        pad = mPadId;
        if (!(dComIfGp_getPlayerStatus0(pad) & daPyStts0_SUBJECT_e) /* SUBJECT */ &&
            (subjectStyleFlags(work->m380) & dCamPrmFlg_UNK010)) {
            k = gabi::fnmsubs(k * slow, work->m38C, k);
        }
        m384 = -(yaw * k);
        f32 m388 = gabi::fmadds(pitch, k, work->m388);
        work->m384 = m384;
        work->m388 = m388;
        if (m384 > 1.0f) {
            m384 = 1.0f;
            work->m384 = m384;
        } else if (m384 < -1.0f) {
            m384 = -1.0f;
            work->m384 = m384;
        }
        m388 = work->m388;
        if (m388 > 1.0f) {
            work->m388 = 1.0f;
            m384 = work->m384;
        } else if (m388 < -1.0f) {
            work->m388 = -1.0f;
            m384 = work->m384;
        }
    }
    subject_angleFromDegrees(a.get(), yawMax * m384);
    subject_angleFromDegrees(b.get(), pitchMax * work->m388);
    subject_shortPlusAngle(c.get(), (s16)*a, &work->m3BA);
    *(be<s16>*)outU = (s16)*c;
    *(be<s16>*)outV = (s16)*b;
    work->m37D = 0;
    pad = mPadId;
    if (dComIfGp_getPlayerStatus0(pad) & daPyStts0_CRAWL_e) {
        subject_shortPlusAngle(c.get(), (s16)0, &work->m3BA);
        *(be<s16>*)outU = (s16)*c;
        *(be<s16>*)outV = 0;
    }
    return res;
}

// HD: word copies retain the original payload bits.
static inline void copySubjectFloatBits(void* dst, const void* src) {
    *gabi::at<be<u32>>(gabi::ea(dst)) = gabi::at<be<u32>>(gabi::ea(src))->get();
}
static inline s16 subjectAngleShortValue(cSAngle_l* out, s16 value) {
    return *subject_angleFromShort(out, value);
}

/* attention-height test of the subject camera: a low centre (60 up, 5 back) unless swimming */
static inline void subjectLowCentre(dCamera_c* c, SubjectWork* work, bool swim, f32* y, f32* z) {
    gabi::Local<cXyz> a;
    c->attentionPos(a, c->mpPlayerActor.get());
    if (a->y < c->m354 + 20.0f && !swim) {
        *y = 60.0f;
        *z = -5.0f;
        work->m3BD = 1;
    } else {
        work->m3BD = 0;
    }
}

/* the subject centre for the centre mode (eye + rotated offset, extended position, or relative) */
static inline void subjectCentre(dCamera_c* c, SubjectWork* work, cXyz* ofs, cXyz* out,
                                 cSAngle_l* baseYaw) {
    if ((u32)work->m3C0 == SUBJECT_CENTER_EYE) {
        gabi::Local<cSGlobe_l> g;
        gabi::Local<cSAngle_l> dir, t, tmp;
        gabi::Local<cXyz> e, x, r;
        subject_globeFromVector(g.get(), ofs);
        c->directionOf(dir, c->mpPlayerActor.get());
        subject_angleAdd(dir, t, &g->mU);
        g->mU = subjectAngleShortValue(tmp, *t);
        c->eyePos(e, c->mpPlayerActor.get());
        subject_globeXyz(g.get(), x.get());
        cXyz_pl(e, r, x);
        out->copy(*r);
    } else if ((u32)work->m3C0 == SUBJECT_CENTER_EXTENDED) {
        gabi::Local<cSAngle_l> tmp;
        out->copy(c->mExtendedPos);
        cSAngle_l* a = subject_angleFromShort(
            tmp.get(), (s16)gabi::at<SubjectExtendedAngleHD>(c->ea())->m0FA.get()); /* m0FA */
        cSAngle_Val(baseYaw, cSAngle_Inv(a));
    } else {
        gabi::Local<cXyz> r;
        c->relationalPos(r, c->mpPlayerActor.get(), ofs);
        out->copy(*r);
    }
}

/* telescope / picto box status and zoom scale 1 */
static inline void subjectZoomStat(dCamera_c* c) {
    if (dComIfGp_getPlayerStatus0(c->mPadId) & daPyStts0_TELESCOPE_LOOK_e)
        c->setComStat(dCamAttnStts_TELESCOPE_LOOK_e); /* TELESCOPE_LOOK */
    if (dComIfGp_getPlayerStatus1(c->mPadId) & daPyStts1_PICTO_BOX_AIM_e)
        c->setComStat(dCamAttnStts_PICTO_BOX_AIM_e); /* PICTO_BOX_AIM */
    c->setComZoomScale(1.0f);
}

// HD: swimming (status1 0x100000) aims with dCamSetup_c's swim parameters, keeps the player's
// facing as base yaw, pushes the centre out of walls, has no zoom and banks the view; the
// low-centre test runs every frame; the scope view is 1280x720 and the picto box wipe is mapped to
// the HD viewport; the transition's frame-count end also applies without the zoom flag; the zoom
// uses the main stick's Y; the C-stick-down sequence is replaced by a GamePad button
/* 025071FC */
bool dCamera_c::subjectCamera(s32 style) {
    SubjectWork* work = (SubjectWork*)mWork;
    auto& setup = *reinterpret_cast<SubjectSetupHD*>(mCamSetup);
    bool swim = (dComIfGp_getPlayerStatus1(mPadId) & daPyStts1_SWIM_HD_e) != 0;
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
        p0 = setup.mSwimOffsetZ.get();
        p26 = 0.0f;
        p1 = setup.mSwimOffsetX.get();
        p5 = setup.mSwimOffsetY.get();
        p10 = setup.mSwimRadius.get();
        p19 = setup.mSwimLatitude.get();
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> baseYaw, tmp;
    subject_globeDefault(g.get());
    {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(dir);
        cSAngle_ct(baseYaw, inv);
        if (swim) {
            gabi::Local<cSAngle_l> d2;
            directionOf(d2, mpPlayerActor.get());
            subject_angleVal(baseYaw.get(), d2.get()); /* Val(const cSAngle&) */
        }
    }
    (void)dComIfGp_ea();
    u16 sflags;
    if (m108 == 0) {
        work->m37C = 0;
        work->m384 = 0.0f;
        work->m380 = style;
        work->m37D = 1;
        work->m3A8 = 100;
        work->m3BC = 0;
        work->m378 = SUBJECT_WORK_TAG; /* 'SUBJ' */
        work->m388 = 0.0f;
        work->m3C0 = SUBJECT_CENTER_PLAYER;
        work->m38C = 0.0f;
        if (dComIfGp_getMiniGameType() == 3) { /* dComIfGp_getMiniGameType() */
            work->m3C0 = SUBJECT_CENTER_EXTENDED;
            sflags = subjectStyleFlags(style);
        } else {
            u32 s0 = dComIfGp_getPlayerStatus0(mPadId);
            sflags = subjectStyleFlags(style);
            if (!(s0 & daPyStts0_SUBJECT_e) && !(sflags & dCamPrmFlg_UNK010))
                work->m3C0 = SUBJECT_CENTER_EYE;
        }
        work->m3C4 = (mStickCPosYLast < -0.74f) ? 2 : 0;
    } else {
        sflags = subjectStyleFlags(style);
    }
    subjectLowCentre(this, work, swim, &p5, &p0);
    gabi::Local<cXyz> ofs;
    ofs->z = p0;
    ofs->x = p1;
    ofs->y = p5;
    if (!swim) {
        if (sflags & dCamPrmFlg_UNK080)
            mEventFlags = mEventFlags | 0x800;
        if (sflags & dCamPrmFlg_UNK100)
            mEventFlags = mEventFlags | 0x10000000;
    }
    bool zoomFlag = (sflags & dCamPrmFlg_UNK010) != 0;

    if (m100 == 0) {
        s32 end = (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_SUBJECT_e) ? 7 : 10;
        f32 t = 1.0f / (f32)(s32)(end - (s32)m108);
        gabi::Local<cXyz> target, d, s, x, e;
        gabi::Local<cSAngle_l> t20, t1e, t1c;
        subjectCentre(this, work, ofs, target, baseYaw);
        cXyz_mi(target, d, &mViewCache.mCenter);
        cXyz_ml(d, s, t);
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
        subject_globeVal(g.get(), p10, reinterpret_cast<cSAngle_l*>(&subjectAngleZero),
                         baseYaw.get());
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, t, rr);
        subject_angleSubtract(&g->mV, t20, &mViewCache.mDirection.mV);
        subject_angleMultiply(t20, t1e, t);
        subject_angleAdd(&mViewCache.mDirection.mV, t1c, t1e);
        mViewCache.mDirection.mV = subjectAngleShortValue(tmp, *t1c);
        subject_angleSubtract(&g->mU, t1c, &mViewCache.mDirection.mU);
        subject_angleMultiply(t1c, t1e, t);
        subject_angleAdd(&mViewCache.mDirection.mU, t20, t1e);
        mViewCache.mDirection.mU = subjectAngleShortValue(tmp, *t20);
        subject_globeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        if (!zoomFlag) {
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(p25 - fv, t, fv);
        } else if ((u32)m108 >= (u32)(end - 6)) {
            subjectZoomStat(this);
            f32 sc = gabi::at<dComIfG_inf_c>(dComIfGp_ea())
                         ->play.mItemScopeWipeScale.get(); /* mItemScopeWipeScale */
            f32 h = (sc - 1.0f) * 0.5f;
            f32 xw = gabi::fmadds(640.0f, h, 320.0f);
            if (xw < 1280.0f && !(sc < 1.0f) && !(sc > 3.0f)) {
                f32 k = (1280.0f - xw) / 960.0f;
                if ((dComIfGp_getPlayerStatus1(mPadId) & daPyStts1_PICTO_BOX_AIM_e) ||
                    gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mScopeType.get() !=
                        0) { /* PICTO_BOX_AIM, demo camera */
                    setView(160.0f * k, 35.0f * k, xw, gabi::fmadds(-400.0f, k, 720.0f));
                }
                f32 base = work->m3A0;
                mViewCache.mFovy = gabi::fmadds(work->m39C - base, k, base);
            }
        } else if ((u32)m108 >= (u32)(end - 7)) {
            subjectZoomStat(this);
        } else if (m108 == 0) {
            copySubjectFloatBits(&work->m3A0, &mViewCache.mFovy);
            work->m39C = mViewCache.mFovy * 0.44444445f; /* HD (GameCube 2/3) */
        }
        if ((u32)m108 == (u32)(end - 1)) {
            m100 = 1;
            m101 = 1;
            m102 = 1;
            work->m384 = 0.0f;
            work->m388 = 0.0f;
            work->m38C = 0.0f;
            work->m398 = 0;
            copySubjectFloatBits(&work->m39C, &mViewCache.mFovy);
            work->m390 = 0.0f;
        }
        return true;
    }

    if (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_CRAWL_e)
        setComStat(dCamAttnStts_00000080_e); /* CRAWL */
    if (gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mScopeType.get() != 0 ||
        (dComIfGp_getPlayerStatus1(mPadId) & daPyStts1_PICTO_BOX_AIM_e)) {
        setView(160.0f, 35.0f, 320.0f, 320.0f);
        work->m3BC = 1;
    } else if (work->m3BC != 0) {
        setView(0.0f, 0.0f, 1280.0f, 720.0f);
    }
    gabi::Local<cSAngle_l> angU, angV; /* sp08 (yaw), sp0A (pitch) */
    cSAngle_ct(angU);
    cSAngle_ct(angV);
    if (work->m37D != 0) {
        gabi::Local<be<s16>> ov, ou;
        gabi::Local<cSAngle_l> t3a;
        work->m37C = 1;
        CalcSubjectAngle((s16*)ov.get(), (s16*)ou.get());
        *angV = subjectAngleShortValue(tmp, *ov);
        s16 u = *ou;
        if (swim)
            u = (s16)(u - 0x1500);
        cSAngle_l* a = subject_angleFromShort(tmp.get(), u);
        subject_angleSubtract(a, t3a, &work->m3BA);
        *angU = (s16)*t3a;
    } else {
        gabi::Local<cSAngle_l> t3e;
        fopAc_ac_c* pl = mpPlayerActor.get();
        if (pl != nullptr && pl->mProcName.get() == fpcNm_PLAYER_e) {
            *angV = subjectAngleShortValue(
                tmp, reinterpret_cast<daPy_py_c*>(pl)->getBodyAngleX()); /* player body angle x */
        } else {
            *angV = subjectAngleShortValue(tmp, pl->shape_angle.x.get()); /* shape_angle.x */
        }
        cSAngle_l* a =
            subject_angleFromShort(tmp.get(), (s16)mpPlayerActor.get()->shape_angle.y.get());
        subject_angleSubtract(a, t3e, &work->m3BA);
        *angU = (s16)*t3e;
        work->m388 = subject_angleDegree(angV) / p19;
        work->m384 = subject_angleDegree(angU) / p24;
    }
    gabi::Local<cXyz> v1, v2;
    {
        gabi::Local<cSAngle_l> c1, c2;
        cSAngle_l* a = subject_angleCopy(c1.get(), angV.get());
        subject_xyzRotateX(v1.get(), ofs.get(), a); /* dCamMath::xyzRotateX */
        a = subject_angleCopy(c2.get(), angU.get());
        subject_xyzRotateY(v2.get(), v1.get(), a); /* dCamMath::xyzRotateY */
    }
    subjectCentre(this, work, v2, &mViewCache.mCenter, baseYaw);
    if (swim) {
        gabi::Local<cXyz> a;
        gabi::Local<dBgS_LinChk> lc;
        attentionPos(a, mpPlayerActor.get());
        subject_cBgS_LinChkCt(lc.get());
        auto* chk = static_cast<cBgS_Chk*>(lc.get());
        auto* poly = static_cast<cBgS_PolyInfo*>(lc.get());
        auto* pass = static_cast<dBgS_PolyPassChk*>(lc.get());
        auto* group = static_cast<dBgS_GrpPassChk*>(lc.get());
        chk->mPolyPassChk = gabi::ea(pass);
        chk->mGrpPassChk = gabi::ea(group);
        chk->__vtbl = gabi::ea(&subjectCheckVtable);
        poly->__vtbl = gabi::ea(&subjectPolyVtable);
        pass->__vtbl = gabi::ea(&subjectCameraPassVtable);
        group->__vtbl = gabi::ea(&subjectCameraGroupVtable);
        pass->mbObjThrough = 0;
        pass->mbCamThrough = 1;
        pass->mbLinkThrough = 0;
        pass->mbArrowThrough = 0;
        pass->mbBombThrough = 0;
        pass->mbBoomerangThrough = 0;
        pass->mbRopeThrough = 0;
        group->mGrp = 3;
        if (lineBGCheck(a, &mViewCache.mCenter, (u8*)lc.get(), 0x7F)) {
            cM3dGPla* plane = subject_GetTriPla(gabi::ea(dComIfG_Bgsp()), (u32)poly->mBgIndex.get(),
                                                (u32)poly->mPolyIndex.get());
            mViewCache.mCenter.copy(lc->mLin.mEnd);
            if (plane != nullptr)
                PSVECAdd(&mViewCache.mCenter, &plane->mNormal, &mViewCache.mCenter);
        }
        pass->__vtbl = gabi::ea(&subjectPassVtable);
        group->__vtbl = gabi::ea(&subjectGroupVtable);
        poly->__vtbl = gabi::ea(&subjectBasePolyVtable);
        subject_cBgS_LinChkDtor(lc.get(), 0);
    }
    bool flag25 = (mEventFlags & 0x2000000) != 0;
    bool useTarget = (flag25 || mLockOnActorId != 0xFFFFFFFFu) && mpLockonTarget.get() != nullptr;
    if (useTarget || (work->m3BC != 0 && flag25)) {
        /* towards the lock-on target (rate 0.04) or the extended position (0.05) */
        gabi::Local<cXyz> a, d;
        gabi::Local<cSAngle_l> ta, tb, tc;
        f32 rate;
        if (useTarget) {
            attentionPos(a, mpLockonTarget.get());
            a->y = a->y - 25.0f;
            rate = 0.04f;
        } else {
            copySubjectFloatBits(&a->x, &mExtendedPos.x);
            copySubjectFloatBits(&a->y, &mExtendedPos.y);
            copySubjectFloatBits(&a->z, &mExtendedPos.z);
            rate = 0.05f;
        }
        cXyz_mi(&mViewCache.mCenter, d, a);
        cSGlobe_Val(g, d);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(p10 - rr, rate, rr);
        g->mRadius = p10;
        subject_angleSubtract(&g->mV, ta, &mViewCache.mDirection.mV);
        subject_angleMultiply(ta, tb, rate);
        subject_angleAdd(&mViewCache.mDirection.mV, tc, tb);
        mViewCache.mDirection.mV = subjectAngleShortValue(tmp, *tc);
        subject_angleSubtract(&g->mU, tc, &mViewCache.mDirection.mU);
        subject_angleMultiply(tc, tb, rate);
        subject_angleAdd(&mViewCache.mDirection.mU, ta, tb);
        mViewCache.mDirection.mU = subjectAngleShortValue(tmp, *ta);
        subject_angleSubtract(&mViewCache.mDirection.mU, ta, baseYaw);
        *angU = (s16)*ta;
        *angV = (s16)mViewCache.mDirection.mV;
        work->m384 = subject_angleDegree(angU) / p24;
        work->m388 = subject_angleDegree(angV) / p19;
        work->m37C = 0;
        work->m3A8 = 0;
    } else {
        gabi::Local<cSAngle_l> t12, t2c, t2a;
        subject_angleAdd(baseYaw, t12, angU);
        subject_globeVal(g.get(), p10, angV.get(), t12.get());
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, p20, rr);
        subject_angleSubtract(&g->mV, t2c, &mViewCache.mDirection.mV);
        subject_angleMultiply(t2c, t2a, p20);
        subject_angleAdd(&mViewCache.mDirection.mV, t12, t2a);
        mViewCache.mDirection.mV = subjectAngleShortValue(tmp, *t12);
        subject_angleSubtract(&g->mU, t12, &mViewCache.mDirection.mU);
        subject_angleMultiply(t12, t2a, p20);
        subject_angleAdd(&mViewCache.mDirection.mU, t2c, t2a);
        mViewCache.mDirection.mU = subjectAngleShortValue(tmp, *t2c);
        if (work->m3A8 < 10)
            work->m3A8 = work->m3A8 + 1;
        work->m37C = 1;
    }
    {
        gabi::Local<cXyz> x, e;
        subject_globeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
    }
    if (zoomFlag) {
        f32 y = mStickMainPosYLast;
        f32 cw = setup.mCurveWeight.get();
        f32 diff;
        if (y > 0.0f) {
            f32 b = subject_rationalBezierRatio(y, cw);
            diff = b - 0.0f;
        } else {
            f32 b = subject_rationalBezierRatio(-y, cw);
            diff = 0.0f - b;
        }
        f32 next = gabi::fmadds(diff * p26, 0.1f, work->m38C);
        f32 z;
        if (swim)
            z = (1.0f - 0.0f >= 0.0f) ? 0.0f : 1.0f;
        else if (next < 0.0f)
            z = 0.0f;
        else
            z = (1.0f - next >= 0.0f) ? next : 1.0f; /* fsel */
        work->m38C = z;
        if (z == 0.0f || z == 0.5f || z == 1.0f)
            diff = 0.0f - 0.0f;
        f32 scale = gabi::fmadds(z, 8.0f, 1.0f);
        f32 sm = scale - 1.0f;
        f32 pos = (sm >= 0.0f) ? sm : 0.0f; /* fsel */
        f32 zf = subject_zoomFovy(work->m39C * 0.5f,
                                  gabi::fmadds(pos, 1.5f, 1.0f)); /* dCamMath::zoomFovy */
        f32 tf = zf + zf;
        f32 fv = mViewCache.mFovy;
        if (swim)
            tf = setup.mSwimFovy.get();
        mViewCache.mFovy = gabi::fmadds(tf - fv, p20, fv);
        setComZoomScale(scale);
        if (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_TELESCOPE_LOOK_e)
            setComStat(dCamAttnStts_TELESCOPE_LOOK_e);
        if (dComIfGp_getPlayerStatus1(mPadId) & daPyStts1_PICTO_BOX_AIM_e)
            setComStat(dCamAttnStts_PICTO_BOX_AIM_e);
        f32 focus = gabi::fnmsubs(__builtin_fabsf(diff), -511.0f, 1.0f);
        s32 id = mCameraID;
        u32 cameraInfo = gabi::ea(&gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mCameraInfo[0]);
        gabi::at<SubjectCameraInfoHD>(cameraInfo + (u32)id * sizeof(dComIfG_camera_info_class))
            ->mCameraZoomForcus = focus; /* setComZoomForcus */
        subjectAutoForcus = 0;           /* mDoGph_gInf_c::mAutoForcus */
        work->m37D = 1;
    } else {
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(p25 - fv, p20, fv);
        work->m37D = 1;
    }
    if (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_SUBJECT_e) { /* SUBJECT */
        s32 st = work->m3C4;
        if (st == 2) {
            if (!(mStickCPosYLast < -0.001f))
                work->m3C4 = 0;
        } else if (st == 0 && (gabi::at<SubjectGamepadHD>(subjectGamepadAddress)->mHold.get() &
                               SUBJECT_CAMERA_BUTTON_HD)) { // HD: GamePad button
            work->m3C4 = 2;
            setComStat(dCamAttnStts_00002000_e);
        } else if (mStickCPosYLast < -0.001f) {
            work->m3C4 = 1;
        } else {
            work->m3C4 = 0;
        }
    }
    if (swim) {
        subject_angleValDegrees(&mViewCache.mBank, (f32)setup.mSwimBank.get()); // HD: swim bank
        mEventFlags = mEventFlags | 0x400;
    } else {
        mViewCache.mBank = subjectAngleZero;
    }
    return true;
}

/* WWHD glue
data subjectGamepadAddress 101F5088
data subjectAngleZero 101FF354
data subjectAutoForcus 101F4828
data subjectStyles 1004487C
data subjectCheckVtable 1004A66C
data subjectPolyVtable 1004A67C
data subjectCameraPassVtable 1004A69C
data subjectCameraGroupVtable 1004A68C
data subjectPassVtable 1004A61C
data subjectGroupVtable 1004A4EC
data subjectBasePolyVtable 1004A4DC
ext 020066C0 void subject_angleFromDegrees(cSAngle_l*, f32)
ext 020065DC void subject_shortPlusAngle(cSAngle_l*, s16, cSAngle_l*)
ext 027200D0 u32 subject_getAimOptions(u32)
ext 02007AFC f32 subject_CPadGetStickX(s32)
ext 02007B20 f32 subject_CPadGetStickY(s32)
ext 02618604 void subject_getGyroOrientation(SubjectRotationHD*, u32)
ext 02515788 void subject_getMatrixRow(cXyz*, SubjectRotationHD*, s32)
ext 026185BC void subject_updateGyro(u32)
ext 0200658C cSAngle_l* subject_angleFromShort(cSAngle_l*, s16)
ext 024F6F9C f32 subject_rationalBezierRatio(f32, f32)
ext 028F4D28 f32 subject_atan2f(f32, f32)
ext 02019510 s16 subject_cM_rad2s(f32)
ext 020068B0 void subject_angleSubtract(const void*, cSAngle_l*, const void*)
ext 0200693C void subject_angleMultiply(const void*, cSAngle_l*, f32)
ext 02006894 void subject_angleAdd(const void*, cSAngle_l*, const void*)
ext 02006720 f32 subject_angleDegree(const void*)
ext 02007324 void subject_globeFromVector(cSGlobe_l*, cXyz*)
ext 020073AC void subject_globeXyz(cSGlobe_l*, cXyz*)
ext 02007100 void subject_globeDefault(cSGlobe_l*)
ext 02006638 void subject_angleVal(cSAngle_l*, cSAngle_l*)
ext 020071E0 void subject_globeVal(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*)
ext 02006644 cSAngle_l* subject_angleCopy(cSAngle_l*, cSAngle_l*)
ext 024F79BC void subject_xyzRotateX(cXyz*, cXyz*, cSAngle_l*)
ext 024F7A40 void subject_xyzRotateY(cXyz*, cXyz*, cSAngle_l*)
ext 02008FEC void subject_cBgS_LinChkCt(dBgS_LinChk*)
ext 020084C8 cM3dGPla* subject_GetTriPla(u32, u32, u32)
ext 02008B4C void subject_cBgS_LinChkDtor(dBgS_LinChk*, s32)
ext 024F7948 f32 subject_zoomFovy(f32, f32)
ext 02006694 void subject_angleValDegrees(cSAngle_l*, f32)
*/
