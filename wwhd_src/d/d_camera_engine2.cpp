/**
 * d_camera_engine2.cpp (WWHD)
 * Follow camera dCamera_c: the camera engines that are "Nonmatching" stubs in the GameCube
 * decompilation (vomit, shield, hung, tornado, manual, tower, ride), written from the WWHD code.
 *
 * Verified against cking.rpx.
 */
#include "d/d_camera_engine2.h"
#include "d/d_camera_key_t60.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "port_hooks.h"
#include "true60.h"
#include "d/d_camera_ride_t60.h"

/* true60 (the per-step smoothing rule, 4, mode-start-once): the engine-2 camera modes at N steps per 30 Hz frame (the converted
 * camera runs them on every step). hungCamera's entry transition moves a share k = r / W of the way per 30 Hz frame
 * (r = frames left, W the weights left) and subtracts r from W: per step k becomes 1 - (1 - k)^dt (e2R; k = 1 stays) and
 * W and the settle change on the step that ends the frame (e2FrameEnd); at N steps W reached 0 inside the transition
 * and k = r / 0 put NaN into the eye (prope5 B: halt in compWallMargin's sphere SetC); the style's start-up block
 * (mCurStyleTimer == 0) runs once per style start (the mode-start-once rule). vomitCamera, shieldCamera and towerCamera (-
 * camera_engine2#2) have the same transition and get the same three changes. hungCamera's settled body (the rope hang /
 * swing, the fall and the bud) ran its per-frame chases on every step (prope9-11: the centre led A by ~1 frame in y on
 * the swing's descent, 14 units): centre follow, stick, radius, latitude and fovy ratios per step (e2R), the radius
 * limit pull and the C stick's radius blend composed with the radius chase (e2Blend), the fall / blend frame counts on
 * frame ends. The settled bodies of vomitCamera, shieldCamera and towerCamera and the whole of tornadoCamera
 * the same way: their chase ratios per step (e2R; tower's chased rates - longitude, latitude,
 * radius and fovy rates - chase by e2R(0.01) and apply as e2R(rate)), the frame counts (vomit's fall weight and fall
 * count, tower's idle count: every step of a frame uses the frame's count) on frame ends; tornadoCamera's start-up once
 * per style start (slot 6), its transition shares e2R(1 / (T - timer)) settling on the frame's end, and its latitude
 * target leaning toward the eye's own latitude as rideCamera's (e2R((1 - B) x rate) / (1 - B)). dt = 1: the original
 * arithmetic. */
static inline f32 e2R(f32 r) { return t60::dt() == 1.0f ? r : t60_ratio(r, t60::dt()); }
static inline bool e2FrameEnd() { return t60::dt() == 1.0f || t60::tick30(); }
/* the radius chase radius += (R - radius) x r toward a target that is itself a blend R = b + (T - b) x c of a base b
 * (the eye-centre distance g, which follows the radius from step to step, or the radius itself) and a fixed target T
 * (hung: the limit pull toward the minimum / maximum radius, the C stick's blend toward 80 / away from 700): per 30 Hz
 * frame the radius moves (T - radius) x r x c toward T; per step that must be e2Rx(r x c) while the base part keeps the
 * plain chase e2R(r). With the target taken per step as b + (R - b) x e2Blend(c, r) the chase gives exactly that where
 * b = radius (the bud's base; g up to the centre's move of the step). c may be negative (the stick away from 700) or 0
 * (R = b: no blend). dt = 1: 1 (the original target). */
static inline f32 e2Rx(f32 r) {
    if (t60::dt() == 1.0f || r == 0.0f || !(r < 1.0f)) return r;
    if (r > 0.0f) return t60_ratio(r, t60::dt());
    return (float)(1.0 - __builtin_pow(1.0 - (double)r, (double)t60::dt())); /* r < 0: a growth per frame */
}
static inline f32 e2Blend(f32 c, f32 r) {
    if (t60::dt() == 1.0f || c == 0.0f) return 1.0f;
    f32 rs = e2R(r);
    return rs != 0.0f ? e2Rx(r * c) / (c * rs) : 1.0f;
}
/* the mode-start-once rule: per mode (slot: 0 hung, 1 vomit, 2 shield, 3 tower, 4 ride, 5 manual, 6 tornado) the first step that sees
 * the timer at 0 after a frame end, a gap or a non-zero timer is a start; host memory, this unit, one camera */
static camera_key_t60::PerCamArr<uint32_t, 7> s_e2ZeroStamp;
static camera_key_t60::PerCamArr<bool, 7> s_e2InFirstFrame;
static inline bool e2Start(u32 timer, int slot) {
    if (t60::dt() == 1.0f)
        return timer == 0;
    if (timer != 0) {
        s_e2InFirstFrame()[slot] = false;
        return false;
    }
    uint32_t now = t60_step_counter();
    bool start = !(s_e2InFirstFrame()[slot] && s_e2ZeroStamp()[slot] + 1 == now);
    s_e2InFirstFrame()[slot] = !t60::tick30();
    s_e2ZeroStamp()[slot] = now;
    return start;
}
using namespace camera_engine2;

// HD: ride entry tables have independent guards and observed initialization order.
using RideEyeGuardHD = be<u32>;
static RideEyeGuardHD l_rideHelmEyeGuard;
static RideEyeGuardHD l_rideSailEyeGuard;
static RideEyeTableHD l_rideHelmEyeOffsets;
static RideEyeTableHD l_rideSailEyeOffsets;

using RopeRadians = be<f32>;
static RopeRadians l_ropeRadians;
static constexpr CameraLineVtablesHD l_lineVtables = {
    0x1004A66C, 0x1004A67C, 0x1004A69C, 0x1004A68C, 0x1004A4DC, 0x1004A4EC, 0x1004A61C};

static inline u32 ownerStatus0(s32 pad) { return dComIfGp_getPlayerStatus0(pad); }
static inline u32 ownerStatus1(s32 pad) { return dComIfGp_getPlayerStatus1(pad); }
static inline void copyCameraWord(void* dst, const void* src) {
    *reinterpret_cast<be<u32>*>(dst) = (u32) * reinterpret_cast<const be<u32>*>(src);
}
static inline s16 copyCameraAngle(cSAngle_l* angleCopy, s16 value) {
    return *cameraEngineConstructAngle(angleCopy, value);
}
static inline void subtractCameraAngles(const void* a, cSAngle_l* out, const void* b) {
    cameraEngineAngleSubtract(a, out, b);
}
static inline void scaleCameraAngle(const void* a, cSAngle_l* out, f32 f) {
    cameraEngineAngleScale(a, out, f);
}
static inline void addCameraAngles(const void* a, cSAngle_l* out, const void* b) {
    cameraEngineAngleAdd(a, out, b);
}
static inline f32 cameraAngleDegree(const void* a) { return cameraEngineAngleDegree(a); }


// HD: inline construction keeps the original translation unit's vtables and store order.
/* inlined into dCamera_c::shieldCamera */
static void constructCameraLine(CameraLineCheckHD* check) {
    cameraEngineConstructLine(check);
    check->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(check));
    check->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(check));
    check->cBgS_Chk::__vtbl = l_lineVtables.mCheck;
    check->cBgS_PolyInfo::__vtbl = l_lineVtables.mPolygon;
    check->dBgS_PolyPassChk::__vtbl = l_lineVtables.mPass;
    check->dBgS_GrpPassChk::__vtbl = l_lineVtables.mGroup;
    check->mbObjThrough = 0;
    check->mbCamThrough = 1;
    check->mbLinkThrough = 0;
    check->mbArrowThrough = 0;
    check->mbBombThrough = 0;
    check->mbBoomerangThrough = 0;
    check->mbRopeThrough = 0;
    check->mGrp = 3;
}
/* inlined into dCamera_c::shieldCamera */
static void destroyCameraLine(CameraLineCheckHD* check) {
    check->cBgS_PolyInfo::__vtbl = l_lineVtables.mPolygonDestructor;
    check->dBgS_GrpPassChk::__vtbl = l_lineVtables.mGroupDestructor;
    check->dBgS_PolyPassChk::__vtbl = l_lineVtables.mPassDestructor;
    cameraEngineDestroyLine(check, 0);
}
// The hit centre is pushed one normal away from the wall.
/* inlined into dCamera_c::shieldCamera */
static void checkCameraWall(dCamera_c* camera, cXyz* start, CameraLineCheckHD* check) {
    if (camera->lineBGCheck(start, &camera->mViewCache.mCenter, reinterpret_cast<u8*>(check),
                            CAMERA_LINE_FLAGS)) {
        PlaneNormalHD* plane =
            cameraEngineGetTriPlane(dComIfG_Bgsp(), (u32)check->mBgIndex, (u32)check->mPolyIndex);
        camera->mViewCache.mCenter.copy(check->mLin.mEnd);
        if (plane != nullptr) {
            PSVECAdd(&camera->mViewCache.mCenter, &plane->mNormal, &camera->mViewCache.mCenter);
        }
    }
}

// HD: Link uses mBodyAngle; Medli uses her waist angles mWaistRotX/mWaistRotY.
static ShieldAnglesHD* shieldAngles(fopAc_ac_c* actor) {
    if (actor == nullptr) {
        return nullptr;
    }
    s16 name = actor->mProcName;
    if (name == fpcNm_PLAYER_e) {
        return reinterpret_cast<ShieldAnglesHD*>(&static_cast<daPy_py_c*>(actor)->mBodyAngle);
    }
    if (name == fpcNm_NPC_MD_e) {
        return reinterpret_cast<ShieldAnglesHD*>(
            &reinterpret_cast<MedliShieldViewHD*>(actor)->mWaistRotX);
    }
    return nullptr;
}


/* C stick ratio: +-1 beyond 0.7, linear inside */
static inline f32 hungStick(f32 stick) {
    if (stick > 0.7f) {
        return 1.0f;
    }
    if (stick < -0.7f) {
        return -1.0f;
    }
    return stick / 0.7f;
}


// HD: adapters retained for the engines still in address-level form.
static void camLinChk_ct_e2(u32 object) {
    constructCameraLine(gabi::at<CameraLineCheckHD>(object));
}
static void camLinChk_dt_e2(u32 object) { destroyCameraLine(gabi::at<CameraLineCheckHD>(object)); }
static void wallCheckE2(dCamera_c* camera, cXyz* start, u32 object) {
    checkCameraWall(camera, start, gabi::at<CameraLineCheckHD>(object));
}

// HD: GameCube body is undecompiled.
/* 0250EF98 */
bool dCamera_c::hungCamera(s32 style) {
    HungWorkHD* p = reinterpret_cast<HungWorkHD*>(mWork);
    s32 pad = mPadId;
    f32 radiusLimitRate = camParamVal(style, 13);
    f32 centerOffsetZ = camParamVal(style, 0);
    f32 swingRatio = camParamVal(style, 18);
    f32 maximumRadius = camParamVal(style, 10);
    f32 centerFollowY = camParamVal(style, 4);
    f32 fovy = camParamVal(style, 25);
    f32 radiusRate = camParamVal(style, 14);
    f32 angleRate = camParamVal(style, 19);
    f32 centerOffsetX = camParamVal(style, 1);
    f32 fovyRate = camParamVal(style, 29);
    f32 latitude = camParamVal(style, 15);
    f32 budLatitude = camParamVal(style, 16);
    f32 centerOffsetY = camParamVal(style, 5);
    f32 centerFollowXZ = camParamVal(style, 3);
    f32 minimumRadius = camParamVal(style, 11);
    if ((dComIfGp_getPlayerStatus0(pad) & PLAYER_HANG) && centerOffsetZ > -10.0f) {
        centerOffsetZ = -10.0f;
    }
    gabi::Local<cXyz> offset;
    offset->y = centerOffsetY;
    offset->z = centerOffsetZ;
    offset->x = centerOffsetX;
    gabi::Local<cSAngle_l> angleCopy;
    if (e2Start(mCurStyleTimer, 0)) {
        gabi::Local<cXyz> pos, relativePosition, d;
        p->mTag = HUNG_WORK;
        p->mCenter.copy(mViewCache.mCenter);
        p->mMovementBlend = 0.0f;
        p->mIdleFrames = 0;
        positionOf(pos, mpPlayerActor.get());
        p->mInitialized = 0;
        copyCameraWord(&p->mPlayerHeight, &pos->y);
        copyCameraWord(&p->mLastPlayerHeight, &pos->y);
        p->mLatitudeRate = 0.75f;
        p->mRadiusRate = 0.75f;
        p->mStickX = 0.0f;
        p->mStickY = 0.0f;
        p->mCenterHeightBlend = centerFollowY;
        p->mFallFrames = 0;
        relationalPos(relativePosition, mpPlayerActor.get(), offset);
        cXyz_mi(&mCenter, d, relativePosition);
        f32 l = std_sqrtf(PSVECSquareMag(d));
        f32 m = (l - maximumRadius >= 0.0f) ? l : maximumRadius; /* fsel */
        fopAc_ac_c* pl = mpPlayerActor.get();
        f32 h = heightOf(pl);
        f32 hh = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
        m = m / hh;
        gabi::Local<cSAngle_l> playerDirection, relativeDirection;
        directionOf(playerDirection, pl); /* GHS: r4 kept across heightOf */
        s16 inv = cSAngle_Inv(playerDirection);
        cameraEngineConstructDifference(relativeDirection.get(), inv, &mViewCache.mDirection.mU);
        f32 nrm = cameraEngineAngleNorm(relativeDirection.get()); /* cSAngle::Norm */
        f32 n2 = nrm + nrm;
        f32 r = std_sqrtf(m);
        s32 k = gabi::ftoi((5.0f * r) * (__builtin_fabsf(n2) + 1.0f));
        s32 n = k + 1;
        p->mTransitionFrames = n;
        p->mRemainingWeight = (f32)((n * (k + 2)) >> 1);
    }
    if (!(ownerStatus0(mPadId) & PLAYER_FALL)) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        f32 y = p->mPlayerHeight;
        p->mPlayerHeight = gabi::fmadds(pos->y - y, e2R(0.05f), y); /* true60: per-frame ratio */
    }
    if (mStyleSettle0 == 0) {
        gabi::Local<cXyz> t, d, s, x, e;
        gabi::Local<cSAngle_l> targetLatitude, playerDirection, targetLongitude, angleStep,
            angleResultA, scaledAngle;
        gabi::Local<cSGlobe_l> g;
        f32 r = (f32)(s32)((s32)p->mTransitionFrames - (s32)mCurStyleTimer);
        f32 k = e2R(r / p->mRemainingWeight); /* true60: the frame's share per step */
        p->mFrameWeight = r;
        relationalPos(t, mpPlayerActor.get(), offset);
        cXyz_mi(t, d, &mViewCache.mCenter);
        cXyz_ml(d, s, k);
        PSVECAdd(&p->mCenter, s, &p->mCenter);
        cXyz_mi(&p->mCenter, d, &mViewCache.mCenter);
        cXyz_ml(d, t, e2R(centerFollowXZ)); /* true60: per-frame ratio */
        PSVECAdd(&mViewCache.mCenter, t, &mViewCache.mCenter);
        f32 R = cameraEngineLimit((f32)mViewCache.mDirection.mRadius, minimumRadius,
                                  maximumRadius); /* limitf */
        cameraEngineConstructDegrees(targetLatitude.get(), latitude);
        directionOf(playerDirection, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(playerDirection);
        cSAngle_l* up = cameraEngineConstructAngle(targetLongitude.get(), inv);
        cameraEngineConstructGlobe(g.get(), R, targetLatitude.get(), up);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
        subtractCameraAngles(&g->mV, angleStep, &mViewCache.mDirection.mV);
        scaleCameraAngle(angleStep, angleResultA, k);
        addCameraAngles(&mViewCache.mDirection.mV, scaledAngle, angleResultA);
        mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *scaledAngle);
        subtractCameraAngles(&g->mU, angleResultA, &mViewCache.mDirection.mU);
        scaleCameraAngle(angleResultA, scaledAngle, k);
        addCameraAngles(&mViewCache.mDirection.mU, angleStep, scaledAngle);
        mViewCache.mDirection.mU = copyCameraAngle(angleCopy, *angleStep);
        cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        p->mLatitude = cameraAngleDegree(&mViewCache.mDirection.mV);
        copyCameraWord(&p->mTargetRadius, &mViewCache.mDirection.mRadius);
        copyCameraWord(&p->mRadius, &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
        if (e2FrameEnd()) /* true60: the weights count 30 Hz frames */
            p->mRemainingWeight = p->mRemainingWeight - p->mFrameWeight;
        if ((u32)mCurStyleTimer >= (u32)((s32)p->mTransitionFrames - 1) && e2FrameEnd()) {
            mStyleSettle2 = 1;
            mStyleSettle1 = 1;
            mStyleSettle0 = 1;
            p->mFrameWeight = 0.0f;
        }
        return true;
    }

    f32 cy = centerFollowY;
    if (ownerStatus0(mPadId) & PLAYER_FALL_OR_BUD) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        if (pos->y < p->mPlayerHeight - 1.0f) {
            /* true60: mFrameWeight counts 30 Hz frames (set on the frame's end step; every step of the frame uses that
             * frame's count) and c x 0.005 is a per-frame ratio */
            f32 c = p->mFrameWeight + 1.0f;
            f32 a = p->mCenterHeightBlend;
            f32 n = gabi::fmadds((1.0f - centerFollowY) - a, e2R(c * 0.005f), a);
            if (e2FrameEnd())
                p->mFrameWeight = c;
            p->mCenterHeightBlend = n;
            cy = centerFollowY + n;
        } else {
            p->mCenterHeightBlend = 0.0f;
            p->mFrameWeight = 0.0f;
        }
    }
    {
        gabi::Local<cXyz> centerFollow, t, d, s;
        centerFollow->x = e2R(centerFollowXZ); /* true60: per-frame ratios (the per-step smoothing rule) */
        centerFollow->y = e2R(cy);
        centerFollow->z = e2R(centerFollowXZ);
        relationalPos(t, mpPlayerActor.get(), offset);
        cXyz_mi(t, d, &mViewCache.mCenter);
        cameraEngineVectorProduct(d.get(), s.get(), centerFollow.get()); /* cXyz * cXyz */
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    }
    f32 swing = 0.0f;
    if (ownerStatus0(mPadId) & PLAYER_HANG) {
        // HD: swing follows the player rope anchor.
        gabi::Local<cXyz> ropeAnchor, pos, ropeProjection;
        gabi::Local<cSAngle_l> playerDirection, ta, dir2, relativePosition;
        daPy_py_c* player = static_cast<daPy_py_c*>(mpPlayerActor.get());
        copyCameraWord(&ropeAnchor->x, &player->mRopePos.x);
        copyCameraWord(&ropeAnchor->y, &player->mRopePos.y);
        copyCameraWord(&ropeAnchor->z, &player->mRopePos.z);
        positionOf(pos, mpPlayerActor.get());
        directionOf(playerDirection, mpPlayerActor.get());
        cameraEngineProjectYZ(ropeProjection.get(), playerDirection.get(), pos.get(),
                              ropeAnchor.get()); /* dCamMath::xyzProjPosOnYZ */
        f32 hd = cameraEngineHorizontalDistance(
            pos.get(), ropeProjection.get()); /* dCamMath::xyzHorizontalDistance */
        f32 at = cameraEngineAtan2(hd, ropeProjection->y - pos->y); /* cM_atan2f */
        f32 dg = 180.0f / (f32)l_ropeRadians;
        swing = at * dg;
        s16 ty = cameraEngineTargetAngleY(ropeAnchor.get(), pos.get()); /* cLib_targetAngleY */
        cSAngle_ct(ta, ty);
        directionOf(dir2, mpPlayerActor.get());
        subtractCameraAngles(ta, relativePosition, dir2);
        if (cameraEngineAngleCos(relativePosition.get()) > 0.0f) {
            swing = -swing; /* Cos */
        }
    } else if (ownerStatus0(mPadId) & PLAYER_FALL_OR_BUD) {
        f32 sc = (90.0f - latitude) * 0.9f;
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        if (pos->y > p->mPlayerHeight) {
            gabi::Local<cXyz> pos2;
            positionOf(pos2, mpPlayerActor.get());
            f32 q = (pos2->y - p->mPlayerHeight) / 1500.0f;
            if (q > 1.0f) {
                swing = sc * 1.0f;
            } else {
                swing = sc * ((q >= 0.0f) ? q : 0.0f); /* fsel */
            }
        } else {
            gabi::Local<cXyz> pos2;
            positionOf(pos2, mpPlayerActor.get());
            f32 q = (pos2->y - p->mPlayerHeight) / -3000.0f;
            if (q > 1.0f) {
                swing = sc * 1.0f;
            } else {
                swing = sc * ((q >= 0.0f) ? q : 0.0f);
            }
        }
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> targetLatitude, targetLongitude; /* sp08 latitude, sp0E longitude */
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cameraEngineConstructVectorGlobe(g.get(), d.get());
    }
    cSAngle_ct(targetLatitude);
    cSAngle_ct(targetLongitude);
    f32 R;
    bool stick;
    /* true60 (e2Blend): R = rBase + (T - rBase) x rBlend; the fall's R is g itself */
    f32 rBase = g->mRadius, rBlend = 0.0f;
    if ((ownerStatus0(mPadId) & PLAYER_FALL) && p->mFallFrames < 10) {
        *targetLongitude = (s16)g->mU;
        *targetLatitude = (s16)g->mV;
        s32 c = p->mFallFrames;
        R = g->mRadius;
        if (e2FrameEnd()) /* true60: a 30 Hz frame count (the 30 Hz timer rule) */
            p->mFallFrames = c + 1;
        f32 fr = 1.0f - (f32)c / 10.0f;
        radiusRate = gabi::fmadds(1.0f - radiusRate, fr, radiusRate);
        angleRate = gabi::fmadds(1.0f - angleRate, fr, angleRate);
        stick = (ownerStatus0(mPadId) & PLAYER_FALL_OR_BUD) != 0;
    } else {
        if (!(ownerStatus0(mPadId) & PLAYER_FALL)) {
            p->mFallFrames = 0;
        }
        R = g->mRadius;
        rBlend = 0.0f;
        if (ownerStatus0(mPadId) & PLAYER_IN_BUD) {
            f32 vr = mViewCache.mDirection.mRadius;
            R = gabi::fmadds(minimumRadius - vr, radiusLimitRate, vr);
            rBase = vr; /* true60 (e2Blend): the blend's base and weight */
            rBlend = radiusLimitRate;
        } else {
            f32 gr = g->mRadius;
            if (gr > maximumRadius) {
                R = gabi::fmadds(maximumRadius - gr, radiusLimitRate, gr);
                rBlend = radiusLimitRate;
            } else if (gr < minimumRadius) {
                R = gabi::fmadds(minimumRadius - gr, radiusLimitRate, gr);
                rBlend = radiusLimitRate;
            }
        }
        gabi::Local<cSAngle_l> playerDirection;
        if (!(ownerStatus0(mPadId) & PLAYER_IN_BUD)) {
            cameraEngineSetDegrees(targetLatitude.get(), gabi::fmadds(swing, swingRatio, latitude));
        } else {
            cameraEngineSetDegrees(targetLatitude.get(), budLatitude);
        }
        directionOf(playerDirection, mpPlayerActor.get());
        cSAngle_Val(targetLongitude, cSAngle_Inv(playerDirection));
        stick = (ownerStatus0(mPadId) & PLAYER_FALL_OR_BUD) != 0;
    }
    if (stick) {
        gabi::Local<cSAngle_l> lo, hi, unusedLatitude, stickAngleDelta, stickAngleStep;
        cameraEngineConstructDegrees(lo.get(), -20.0f);
        cameraEngineConstructDegrees(hi.get(), 75.0f);
        cameraEngineConstructDegrees(unusedLatitude.get(), 178.0f);
        f32 sx = hungStick(mStickCPosXLast);
        f32 sy = hungStick(mStickCPosYLast);
        f32 bx = cameraEngineBezierRatio(sx, 2.0f); /* dCamMath::rationalBezierRatio */
        f32 by = cameraEngineBezierRatio(sy, 2.0f);
        f32 stickY = p->mStickY;
        f32 stickX = p->mStickX;
        f32 nextStickY = gabi::fmadds(by - stickY, e2R(0.5f), stickY); /* true60: per-frame ratios */
        p->mStickX = gabi::fmadds(bx - stickX, e2R(0.15f), stickX);
        p->mStickY = nextStickY;
        if (sy > 0.0f) {
            R = gabi::fmadds(80.0f - R, nextStickY, R);
            rBlend = rBlend * (1.0f - nextStickY) + nextStickY; /* true60: toward 80 by the stick */
            subtractCameraAngles(lo, stickAngleDelta, targetLatitude);
            scaleCameraAngle(stickAngleDelta, stickAngleStep, p->mStickY);
            cameraEngineAngleAddAssign(targetLatitude.get(), stickAngleStep.get()); /* += */
        } else {
            R = gabi::fnmsubs(700.0f - R, nextStickY, R);
            rBlend = rBlend * (1.0f + nextStickY) - nextStickY; /* true60: away from 700 by the stick */
            subtractCameraAngles(hi, stickAngleDelta, targetLatitude);
            scaleCameraAngle(stickAngleDelta, stickAngleStep, p->mStickY);
            cameraEngineAngleSubtractAssign(targetLatitude.get(), stickAngleStep.get()); /* -= */
        }
    }
    {
        gabi::Local<cSAngle_l> angleStep, angleResultC, angleResultD;
        gabi::Local<cXyz> x, e, pos;
        f32 rr = mViewCache.mDirection.mRadius;
        if (t60::dt() != 1.0f) /* true60: the blend composed with the chase (e2Blend) */
            R = rBase + (R - rBase) * e2Blend(rBlend, radiusRate);
        mViewCache.mDirection.mRadius = gabi::fmadds(R - rr, e2R(radiusRate), rr); /* true60: per-frame ratios */
        subtractCameraAngles(targetLatitude, angleStep, &mViewCache.mDirection.mV);
        scaleCameraAngle(angleStep, angleResultC, e2R(angleRate));
        addCameraAngles(&mViewCache.mDirection.mV, angleResultD, angleResultC);
        mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *angleResultD);
        mViewCache.mDirection.mU = copyCameraAngle(angleCopy, *targetLongitude);
        cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        f32 fv = mViewCache.mFovy;
        mViewCache.mEye.copy(*e);
        mViewCache.mFovy = gabi::fmadds(fovy - fv, e2R(fovyRate), fv);
        positionOf(pos, mpPlayerActor.get());
        copyCameraWord(&p->mLastPlayerHeight, &pos->y);
    }
    return true;
}

// HD: GameCube body is undecompiled.
/* 02513518 */
bool dCamera_c::vomitCamera(s32 style) {
    VomitWorkHD* p = reinterpret_cast<VomitWorkHD*>(mWork);
    u32 fl = mEventFlags;
    f32 centerFollowXZ = camParamVal(style, 3);
    f32 maximumRadius = camParamVal(style, 10);
    f32 centerOffsetZ = camParamVal(style, 0);
    f32 centerOffsetX = camParamVal(style, 1);
    f32 fovyRate = camParamVal(style, 29);
    f32 centerOffsetY = camParamVal(style, 5);
    f32 latitude = camParamVal(style, 15);
    f32 centerFollowY = camParamVal(style, 4);
    f32 angleRate = camParamVal(style, 19);
    f32 fovy = camParamVal(style, 25);
    f32 heightOffsetRange = camParamVal(style, 7);
    f32 minimumRadius = camParamVal(style, 11);
    f32 radiusRate = camParamVal(style, 14);
    f32 speed = (fl & CAMERA_FAST_TRANSITION) ? 1.0f : 8.0f;
    gabi::Local<cXyz> offset;
    offset->y = centerOffsetY;
    offset->x = centerOffsetX;
    offset->z = centerOffsetZ;
    gabi::Local<cSAngle_l> angleCopy;
    if (e2Start(mCurStyleTimer, 1)) {
        gabi::Local<cXyz> pos, relativePosition, d;
        p->mTag = VOMIT_WORK;
        p->mCenter.copy(mViewCache.mCenter);
        positionOf(pos, mpPlayerActor.get());
        p->mPlayerPosition.copy(*pos);
        p->mMovementBlend = 0.0f;
        p->mIdleFrames = 0;
        positionOf(pos, mpPlayerActor.get());
        p->mFallFrames = 0;
        copyCameraWord(&p->mPlayerHeight, &pos->y);
        copyCameraWord(&p->mLastPlayerHeight, &pos->y);
        p->mLatitudeRate = 0.75f;
        p->mRadiusRate = 0.75f;
        p->mStickX = 0.0f;
        p->mStickY = 0.0f;
        p->mCenterHeightBlend = centerFollowY;
        p->mInitialized = 0;
        relationalPos(relativePosition, mpPlayerActor.get(), offset);
        cXyz_mi(&mCenter, d, relativePosition);
        f32 l = std_sqrtf(PSVECSquareMag(d));
        f32 m = (l - maximumRadius >= 0.0f) ? l : maximumRadius; /* fsel */
        fopAc_ac_c* pl = mpPlayerActor.get();
        f32 h = heightOf(pl);
        f32 hh = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
        m = m / hh;
        gabi::Local<cSAngle_l> playerDirection, relativeDirection;
        directionOf(playerDirection, pl); /* GHS: r4 kept across heightOf */
        s16 inv = cSAngle_Inv(playerDirection);
        cameraEngineConstructDifference(relativeDirection.get(), inv, &mViewCache.mDirection.mU);
        f32 nrm = cameraEngineAngleNorm(relativeDirection.get()); /* cSAngle::Norm */
        f32 n2 = nrm + nrm;
        f32 r = std_sqrtf(m);
        s32 k = gabi::ftoi((speed * r) * (__builtin_fabsf(n2) + 1.0f));
        s32 n = k + 1;
        p->mTransitionFrames = n;
        p->mRemainingWeight = (f32)((n * (k + 2)) >> 1);
    }
    if (ownerStatus0(mPadId) & PLAYER_IN_BUD) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        p->mPlayerPosition.copy(*pos);
        positionOf(pos, mpPlayerActor.get());
        f32 y = p->mPlayerHeight;
        p->mPlayerHeight = gabi::fmadds(pos->y - y, e2R(0.05f), y); /* true60: per-frame ratio */
    }
    if (mStyleSettle0 == 0) {
        gabi::Local<cXyz> t, d, s, x, e;
        gabi::Local<cSAngle_l> targetLatitude, playerDirection, targetLongitude, blendedAngle,
            scaledAngle, angleResult;
        gabi::Local<cSGlobe_l> g;
        f32 r = (f32)(s32)((s32)p->mTransitionFrames - (s32)mCurStyleTimer);
        f32 k = e2R(r / p->mRemainingWeight); /* true60: the frame's share per step */
        p->mFrameWeight = r;
        relationalPos(t, mpPlayerActor.get(), offset);
        cXyz_mi(t, d, &mViewCache.mCenter);
        cXyz_ml(d, s, k);
        PSVECAdd(&p->mCenter, s, &p->mCenter);
        cXyz_mi(&p->mCenter, d, &mViewCache.mCenter);
        cXyz_ml(d, t, e2R(centerFollowXZ)); /* true60: per-frame ratio */
        PSVECAdd(&mViewCache.mCenter, t, &mViewCache.mCenter);
        cameraEngineConstructDegrees(targetLatitude.get(), latitude);
        directionOf(playerDirection, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(playerDirection);
        cSAngle_l* up = cameraEngineConstructAngle(targetLongitude.get(), inv);
        cameraEngineConstructGlobe(g.get(), maximumRadius, targetLatitude.get(), up);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
        subtractCameraAngles(&g->mV, blendedAngle, &mViewCache.mDirection.mV);
        scaleCameraAngle(blendedAngle, scaledAngle, k);
        addCameraAngles(&mViewCache.mDirection.mV, angleResult, scaledAngle);
        mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *angleResult);
        subtractCameraAngles(&g->mU, scaledAngle, &mViewCache.mDirection.mU);
        scaleCameraAngle(scaledAngle, angleResult, k);
        addCameraAngles(&mViewCache.mDirection.mU, blendedAngle, angleResult);
        mViewCache.mDirection.mU = copyCameraAngle(angleCopy, *blendedAngle);
        cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        p->mLatitude = cameraAngleDegree(&mViewCache.mDirection.mV);
        copyCameraWord(&p->mTargetRadius, &mViewCache.mDirection.mRadius);
        copyCameraWord(&p->mRadius, &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
        if (e2FrameEnd()) /* true60: the weights count 30 Hz frames */
            p->mRemainingWeight = p->mRemainingWeight - p->mFrameWeight;
        if ((u32)mCurStyleTimer >= (u32)((s32)p->mTransitionFrames - 1) && e2FrameEnd()) {
            mStyleSettle2 = 1;
            mStyleSettle1 = 1;
            mStyleSettle0 = 1;
            p->mFrameWeight = 0.0f;
        }
        return true;
    }

    f32 cy = centerFollowY;
    {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        if (pos->y < p->mPlayerHeight - 1.0f) {
            /* true60: mFrameWeight counts 30 Hz frames (set on the frame's end step; every step of the frame uses that
             * frame's count) and c x 0.01 is a per-frame ratio (as hungCamera) */
            f32 c = p->mFrameWeight + 1.0f;
            f32 a = p->mCenterHeightBlend;
            f32 n = gabi::fmadds((1.0f - centerFollowY) - a, e2R(c * 0.01f), a);
            if (e2FrameEnd())
                p->mFrameWeight = c;
            cy = centerFollowY + n;
            p->mCenterHeightBlend = n;
        } else {
            gabi::Local<cXyz> pos2, pos3;
            positionOf(pos2, mpPlayerActor.get());
            positionOf(pos3, mpPlayerActor.get());
            f32 q = (pos3->y - p->mPlayerHeight) / heightOffsetRange;
            f32 w = 1.0f - q;
            offset->y = offset->y * w;
            offset->z = offset->z * w;
            p->mFrameWeight = 0.0f;
            p->mCenterHeightBlend = 0.0f;
        }
    }
    gabi::Local<cXyz> centerFollow, t, d, s;
    centerFollow->x = e2R(centerFollowXZ); /* true60: per-frame ratios (the per-step smoothing rule) */
    centerFollow->y = e2R(cy);
    centerFollow->z = e2R(centerFollowXZ);
    relationalPos(t, mpPlayerActor.get(), offset);
    cXyz_mi(t, d, &mViewCache.mCenter);
    cameraEngineVectorProduct(d.get(), s.get(), centerFollow.get()); /* cXyz * cXyz */
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    gabi::Local<cSGlobe_l> g;
    cXyz_mi(&mViewCache.mEye, t, &mViewCache.mCenter);
    cameraEngineConstructVectorGlobe(g.get(), t.get());
    gabi::Local<cSAngle_l> targetLatitude, targetLongitude, playerDirection;
    cSAngle_ct(targetLatitude);
    cSAngle_ct(targetLongitude);
    f32 gr = g->mRadius;
    *targetLatitude = (s16)g->mV;
    f32 R = (gr - minimumRadius >= 0.0f) ? gr : minimumRadius; /* fsel */
    directionOf(playerDirection, mpPlayerActor.get());
    cSAngle_Val(targetLongitude, cSAngle_Inv(playerDirection));
    if (ownerStatus0(mPadId) & PLAYER_FALL) {
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        if (pos->y < p->mPlayerHeight - 1.0f) {
            if (e2FrameEnd()) /* true60: a 30 Hz frame count (the 30 Hz timer rule) */
                p->mFallFrames = p->mFallFrames + 1;
            R = maximumRadius;
        } else {
            gabi::Local<cXyz> pos2;
            positionOf(pos2, mpPlayerActor.get());
            if (pos2->y < p->mLastPlayerHeight && R > maximumRadius) {
                R = maximumRadius;
            }
        }
    } else if (ownerStatus0(mPadId) & PLAYER_IN_BUD) {
        R = maximumRadius;
        cameraEngineSetDegrees(targetLatitude.get(), latitude); /* Val(f32) */
        gabi::Local<cSAngle_l> d2;
        directionOf(d2, mpPlayerActor.get());
        cSAngle_Val(targetLongitude, cSAngle_Inv(d2));
        p->mFallFrames = 0;
    } else if (!(ownerStatus0(mPadId) & PLAYER_FALL)) {
        p->mFallFrames = 0;
    }
    {
        gabi::Local<cSAngle_l> latitudeDelta, angleStep, angleResultA;
        gabi::Local<cXyz> x, e, pos;
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(R - rr, e2R(radiusRate), rr); /* true60: per-frame ratios */
        subtractCameraAngles(targetLatitude, latitudeDelta, &mViewCache.mDirection.mV);
        scaleCameraAngle(latitudeDelta, angleStep, e2R(angleRate));
        addCameraAngles(&mViewCache.mDirection.mV, angleResultA, angleStep);
        mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *angleResultA);
        mViewCache.mDirection.mU = copyCameraAngle(angleCopy, *targetLongitude);
        cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        f32 fv = mViewCache.mFovy;
        f32 nf = gabi::fmadds(fovy - fv, e2R(fovyRate), fv);
        mViewCache.mEye.copy(*e);
        mViewCache.mFovy = nf;
        positionOf(pos, mpPlayerActor.get());
        copyCameraWord(&p->mLastPlayerHeight, &pos->y);
    }
    return true;
}

// HD: GameCube body is undecompiled.
/* 02513E98 */
bool dCamera_c::shieldCamera(s32 style) {
    ShieldWorkHD* work = reinterpret_cast<ShieldWorkHD*>(mWork);
    gabi::Local<cXyz> offset;
    offset->x = camParamVal(style, 1);
    f32 fovy = camParamVal(style, 25);
    offset->y = camParamVal(style, 5);
    f32 shieldPitchRatio = camParamVal(style, 18);
    f32 latitude = camParamVal(style, 15);
    f32 followRate = camParamVal(style, 20);
    f32 shieldYawRatio = camParamVal(style, 23);
    f32 minimumRadius = camParamVal(style, 11);
    offset->z = camParamVal(style, 0);
    f32 maximumRadius = camParamVal(style, 10);
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> angleCopy;
    cameraEngineConstructVectorGlobe(g.get(), offset.get());
    fopAc_ac_c* pl = mpPlayerActor.get();
    ShieldAnglesHD* sa = shieldAngles(pl);
    if (sa != nullptr) {
        gabi::Local<cSAngle_l> shieldLatitude, shieldLongitude, playerDirection;
        cameraEngineAngleSubtractValue(&g->mV, shieldLatitude.get(), (s16)sa->x); /* V - angle x */
        g->mV = copyCameraAngle(angleCopy, *shieldLatitude);
        cameraEngineAngleAddValue(&g->mU, shieldLongitude.get(), (s16)sa->y); /* U + angle y */
        directionOf(playerDirection, mpPlayerActor.get());
        addCameraAngles(shieldLongitude, shieldLatitude, playerDirection);
        s16 nu = copyCameraAngle(angleCopy, *shieldLatitude);
        pl = mpPlayerActor.get();
        g->mU = nu;
    }
    gabi::Local<cXyz> target;
    {
        gabi::Local<cXyz> a, x;
        attentionPos(a, pl);
        cameraEngineGlobeXyz(g.get(), x.get());
        cXyz_pl(a, target, x);
    }
    if (e2Start(mCurStyleTimer, 2)) {
        gabi::Local<cXyz> d;
        gabi::Local<cSAngle_l> playerDirection, relativeDirection;
        work->mTag = SHIELD_WORK;
        cXyz_mi(&mCenter, d, target);
        f32 l = std_sqrtf(PSVECSquareMag(d));
        f32 m = (l - maximumRadius >= 0.0f) ? l : maximumRadius; /* fsel */
        fopAc_ac_c* p2 = mpPlayerActor.get();
        f32 h = heightOf(p2);
        f32 hh = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
        m = m / hh;
        directionOf(playerDirection, p2); /* GHS: r4 kept across heightOf */
        s16 inv = cSAngle_Inv(playerDirection);
        cameraEngineConstructDifference(relativeDirection.get(), inv, &mViewCache.mDirection.mU);
        f32 nrm = cameraEngineAngleNorm(relativeDirection.get()); /* cSAngle::Norm */
        f32 n2 = nrm + nrm;
        f32 r = std_sqrtf(m);
        s32 k = gabi::ftoi((3.0f * r) * (__builtin_fabsf(n2) + 1.0f));
        s32 n = k + 1;
        work->mTransitionFrames = n;
        work->mRemainingWeight = (f32)((n * (k + 2)) >> 1);
    }
    gabi::Local<CameraLineCheckHD> lineCheck;
    if (mStyleSettle0 == 0) {
        gabi::Local<cXyz> d, s, a, x, e;
        gabi::Local<cSAngle_l> playerDirection, targetLatitude, targetLongitude, angleDelta,
            angleStep, scaledAngle;
        gabi::Local<cSGlobe_l> g2;
        f32 r = (f32)(s32)(work->mTransitionFrames - (s32)mCurStyleTimer);
        f32 k = e2R(r / work->mRemainingWeight); /* true60: the frame's share per step */
        work->mFrameWeight = r;
        cXyz_mi(target, d, &mViewCache.mCenter);
        cXyz_ml(d, s, k);
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
        attentionPos(a, mpPlayerActor.get());
        a->y = a->y - 15.0f;
        constructCameraLine(lineCheck);
        f32 dist = cameraEngineHorizontalDistance(
            target.get(), &mViewCache.mCenter); /* dCamMath::xyzHorizontalDistance */
        f32 ox = offset->x, oz = offset->z;
        f32 om = (oz - ox >= 0.0f) ? oz : ox; /* fsel */
        if (dist < __builtin_fabsf(om) + 20.0f) {
            checkCameraWall(this, a, lineCheck);
        }
        f32 R = cameraEngineLimit((f32)mViewCache.mDirection.mRadius, minimumRadius,
                                  maximumRadius); /* limitf */
        directionOf(playerDirection, mpPlayerActor.get());
        cSAngle_l* lp = cameraEngineConstructDegrees(targetLatitude.get(), latitude);
        s16 inv = cSAngle_Inv(playerDirection);
        cSAngle_l* up = cameraEngineConstructAngle(targetLongitude.get(), inv);
        cameraEngineConstructGlobe(g2.get(), R, lp, up);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(g2->mRadius - rr, k, rr);
        subtractCameraAngles(&g2->mV, angleDelta, &mViewCache.mDirection.mV);
        scaleCameraAngle(angleDelta, angleStep, k);
        addCameraAngles(&mViewCache.mDirection.mV, scaledAngle, angleStep);
        mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *scaledAngle);
        subtractCameraAngles(&g2->mU, angleDelta, &mViewCache.mDirection.mU);
        scaleCameraAngle(angleDelta, angleStep, k);
        addCameraAngles(&mViewCache.mDirection.mU, scaledAngle, angleStep);
        mViewCache.mDirection.mU = copyCameraAngle(angleCopy, *scaledAngle);
        cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
        work->mLatitude = cameraAngleDegree(&mViewCache.mDirection.mV);
        copyCameraWord(&work->mTargetRadius, &mViewCache.mDirection.mRadius);
        copyCameraWord(&work->mRadius, &mViewCache.mDirection.mRadius);
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
        if (e2FrameEnd()) /* true60: the weights count 30 Hz frames */
            work->mRemainingWeight = work->mRemainingWeight - work->mFrameWeight;
        if ((u32)mCurStyleTimer >= (u32)(work->mTransitionFrames - 1) && e2FrameEnd()) {
            mStyleSettle2 = 1;
            mStyleSettle1 = 1;
            mStyleSettle0 = 1;
            work->mFrameWeight = 0.0f;
        }
        destroyCameraLine(lineCheck);
        return true;
    }
    mViewCache.mCenter.copy(*target);
    {
        gabi::Local<cXyz> a;
        attentionPos(a, mpPlayerActor.get());
        a->y = a->y - 15.0f;
        constructCameraLine(lineCheck);
        checkCameraWall(this, a, lineCheck);
    }
    gabi::Local<cSAngle_l> targetLatitude, shieldPitch, playerDirection, reverseDirection,
        shieldYaw, angleResultB, angleResultC, angleResultD, blendedAngle, angleResult;
    cameraEngineConstructDegrees(targetLatitude.get(), latitude);
    cSAngle_ct(shieldPitch);
    directionOf(playerDirection, mpPlayerActor.get());
    s16 inv = cSAngle_Inv(playerDirection);
    cSAngle_ct(reverseDirection, inv);
    cSAngle_ct(shieldYaw);
    ShieldAnglesHD* sa2 = shieldAngles(mpPlayerActor.get());
    s16 yv;
    if (sa2 != nullptr) {
        cSAngle_Val(shieldPitch, (s16)sa2->x);
        yv = (s16)sa2->y;
    } else {
        cSAngle_Val(shieldPitch, 0);
        yv = 0;
    }
    cSAngle_Val(shieldYaw, yv);
    scaleCameraAngle(shieldPitch, angleResultB, shieldPitchRatio);
    addCameraAngles(targetLatitude, angleResultC, angleResultB);
    subtractCameraAngles(angleResultC, angleResultD, &mViewCache.mDirection.mV);
    const f32 followStep = e2R(followRate); /* true60: per-frame ratio (the per-step smoothing rule) */
    scaleCameraAngle(angleResultD, blendedAngle, followStep);
    addCameraAngles(&mViewCache.mDirection.mV, angleResult, blendedAngle);
    mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *angleResult);
    scaleCameraAngle(shieldYaw, angleResult, shieldYawRatio);
    addCameraAngles(reverseDirection, blendedAngle, angleResult);
    subtractCameraAngles(blendedAngle, angleResultD, &mViewCache.mDirection.mU);
    scaleCameraAngle(angleResultD, angleResultC, followStep);
    addCameraAngles(&mViewCache.mDirection.mU, angleResultB, angleResultC);
    s16 nu = copyCameraAngle(angleCopy, *angleResultB);
    f32 R0 = mViewCache.mDirection.mRadius;
    mViewCache.mDirection.mU = nu;
    f32 lim = cameraEngineLimit(R0, minimumRadius, maximumRadius); /* limitf */
    mViewCache.mDirection.mRadius = gabi::fmadds(lim - R0, followStep, R0);
    gabi::Local<cXyz> x, e;
    cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, e, x);
    f32 fv = mViewCache.mFovy;
    mViewCache.mEye.copy(*e);
    mViewCache.mFovy = gabi::fmadds(fovy - fv, followStep, fv);
    destroyCameraLine(lineCheck);
    return true;
}

using CameraGuestByteHD = be<u8>;
using CameraGuestAngleHD = be<s16>;
static CameraGuestByteHD l_manualMakarFlying;
static CameraGuestByteHD l_manualMedliFlying;
static CameraGuestAngleHD l_tornadoZeroAngle;

// HD: separate guards retain the observed initialization order of the two
// side-eye tables.
using TornadoEyeGuardHD = be<u32>;
static TornadoEyeGuardHD l_tornadoSideEyeGuard;
static TornadoEyeGuardHD l_tornadoShipEyeGuard;
struct TornadoEyeTableHD {
    cXyz mOffsets[2];
};
static TornadoEyeTableHD l_tornadoSideEyeOffsets;
static TornadoEyeTableHD l_tornadoShipEyeOffsets;

/* the side eye of a tornado entry: a function-local static offset table
 * (initialised on first use), rotated to the side the player is on; mEntryMode
 * = 1 when the eye sees the player */
static void tornadoSideEye(dCamera_c *c, TornadoWorkHD *p, cSAngle_l *base, be<u32> *guard,
                           cXyz *offsets, bool ship) {
    if ((u32)*guard == 0) {
        if (ship) { /* status1 0x80 table */
            *guard = 1;
            offsets[0].x = -120.0f;
            offsets[0].y = 20.0f;
            offsets[0].z = 280.0f;
            offsets[1].x = -120.0f;
            offsets[1].y = 20.0f;
            offsets[1].z = 280.0f;
        } else {
            offsets[0].x = -120.0f;
            offsets[0].y = 50.0f;
            offsets[0].z = 280.0f;
            offsets[1].x = 240.0f;
            offsets[1].y = 160.0f;
            offsets[1].z = -80.0f;
            *guard = 1;
        }
    }
    gabi::Local<cSGlobe_l> g;
    gabi::Local<cSAngle_l> ta, rel, t, tmp;
    gabi::Local<cXyz> pos, x, e;
    cameraEngineConstructVectorGlobe(g.get(), &offsets[(u32)c->mFrameCounter & 1]);
    cXyz *a = c->positionPntOf(p->mpTornado.get());
    cXyz *b = c->positionPntOf(c->mpPlayerActor.get());
    s16 ty = cLib_targetAngleY(b, a);
    cSAngle_ct(ta, ty);
    subtractCameraAngles(ta, rel, base);
    if ((s16)*rel < (s16)l_tornadoZeroAngle)
        subtractCameraAngles(base, t, &g->mU);
    else
        addCameraAngles(base, t, &g->mU);
    g->mU = copyCameraAngle(tmp, *t);
    c->positionOf(pos, p->mpTornado.get());
    cameraEngineGlobeXyz(g.get(), x.get());
    cXyz_pl(pos, e, x);
    p->mSideEye.copy(*e);
    cXyz *pp = c->positionPntOf(c->mpPlayerActor.get());
    if (!cameraEngineLineBlocked(c, &p->mSideEye, pp, 0x7F))
        p->mEntryMode = ENTRY_SIDE_EYE; /* lineBGCheck */
}

// HD: GameCube body is undecompiled.
/* 025121E8 */
/* Tornado / Cyclos ride (status 0x1000000 / 0x10000 and
 * status1 2 / 0x80): the camera looks at the player from behind the tornado
 * (actor 0xA5), pulled out by the ship's distance (actor 0x1BB or 0x11B), with
 * an entry transition from a side eye; the C stick shifts the centre and the
 * stick banks the view. */
bool dCamera_c::tornadoCamera(s32 style) {
    TornadoWorkHD *p = reinterpret_cast<TornadoWorkHD *>(mWork);
    const TornadoStyleViewHD *entry = gabi::at<TornadoStyleViewHD>(CAM_STYLES + style * 0x84);
    f32 centerOffsetZ = camParamVal(style, 0);
    f32 speedRadiusScale = camParamVal(style, 13);
    f32 baseRadius = camParamVal(style, 10);
    f32 baseLatitude = camParamVal(style, 15);
    f32 centerFollowXZ = camParamVal(style, 3);
    f32 sideOffsetScale = camParamVal(style, 1);
    f32 centerOffsetY = camParamVal(style, 5);
    f32 centerFollowY = camParamVal(style, 4);
    f32 speedHeightScale = camParamVal(style, 8);
    f32 maximumSpeed = camParamVal(style, 14);
    f32 minimumLatitude = camParamVal(style, 16);
    f32 speedLatitudeScale = camParamVal(style, 18);
    gabi::Local<cSAngle_l> lo, hi, bankMax, base, tmp;
    cameraEngineConstructDegrees(lo.get(), minimumLatitude);
    cameraEngineConstructDegrees(hi.get(), camParamVal(style, 17));
    f32 speedFovyScale = camParamVal(style, 28);
    f32 maximumBank = camParamVal(style, 29);
    f32 rate = camParamVal(style, 21);  /* Val21: radius, latitude and fovy rate */
    f32 urate = camParamVal(style, 20); /* Val20: longitude rate */
    f32 baseFovy = camParamVal(style, 25);
    cameraEngineConstructDegrees(bankMax.get(), maximumBank);
    gabi::Local<cXyz> ofs;
    ofs->y = centerOffsetY;
    ofs->z = centerOffsetZ;
    ofs->x = 0.0f;
    cSAngle_ct(base);
    f32 w = cameraEngineLimit((f32)mMonitor.field_0x0C.y / maximumSpeed, 0.0f, 1.0f); /* limitf */
    {
        gabi::Local<be<s16>> nm;
        *nm = 0x1BB;
        fopAc_ac_c *ship = fopAcIt_Judge(0x025E121C, nm.get()); /* fpcSch_JudgeForPName */
        p->mpShip = ship;
        if (ship == nullptr) {
            gabi::Local<be<s16>> nm2;
            *nm2 = 0x11B;
            p->mpShip = fopAcIt_Judge(0x025E121C, nm2.get());
        }
    }
    {
        gabi::Local<be<s16>> nm;
        *nm = 0xA5;
        fopAc_ac_c *tor = fopAcIt_Judge(0x025E121C, nm.get());
        p->mpTornado = tor;
        if (tor == nullptr) {
            gabi::Local<cSAngle_l> dir;
            directionOf(dir, mpPlayerActor.get());
            cameraEngineCopyAngle(base.get(), dir.get()); /* Val(const cSAngle&) */
        } else if (p->mpShip.get() != nullptr) {
            gabi::Local<cXyz> a, b, d;
            gabi::Local<cSGlobe_l> g;
            gabi::Local<cSAngle_l> dir, t56, t54, t52;
            positionOf(a, p->mpShip.get());
            positionOf(b, p->mpTornado.get());
            cXyz_mi(a, d, b);
            cameraEngineConstructVectorGlobe(g.get(), d.get());
            f32 q = cameraEngineLimit(g->mRadius / 15000.0f, 0.0f, 1.0f); /* limitf */
            w = 1.0f - q;
            directionOf(dir, p->mpTornado.get());
            subtractCameraAngles(dir, t56, &g->mU);
            scaleCameraAngle(t56, t54, q * 0.5f);
            addCameraAngles(&g->mU, t52, t54);
            *base = (s16)*t52;
        } else {
            gabi::Local<cSAngle_l> dir;
            directionOf(dir, tor);
            cameraEngineCopyAngle(base.get(), dir.get());
        }
    }
    if (e2Start(mCurStyleTimer, 6)) { /* true60: the mode-start-once rule, once per style start */
        p->mTag = TORNADO_WORK;                 /* 'TRND' */
        p->mStickCenterOffset.copy(cXyz::Zero); /* cXyz::Zero */
        p->mEntryMode = ENTRY_NORMAL;
        p->mEntryViewIndex = (u32)mFrameCounter;
        bool side = (ownerStatus0(mPadId) & 0x1000000) || (ownerStatus1(mPadId) & 2);
        if (side || (ownerStatus1(mPadId) & 0x80)) {
            gabi::Local<be<s16>> nm;
            p->mCenterTarget.copy(mViewCache.mCenter);
            *nm = 0xA5;
            p->mpTornado = fopAcIt_Judge(0x025E121C, nm.get());
            if (side)
                tornadoSideEye(this, p, base, &l_tornadoSideEyeGuard,
                               l_tornadoSideEyeOffsets.mOffsets, false);
            else
                tornadoSideEye(this, p, base, &l_tornadoShipEyeGuard,
                               l_tornadoShipEyeOffsets.mOffsets, true);
        } else if (ownerStatus0(mPadId) & 0x1010000) {
            gabi::Local<be<s16>> nm;
            p->mCenterTarget.copy(mViewCache.mCenter);
            *nm = 0xA5;
            p->mpTornado = fopAcIt_Judge(0x025E121C, nm.get());
        } else {
            p->mpTornado = nullptr;
        }
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        if (mEventFlags & CAMERA_FAST_TRANSITION)
            p->mEntryMode = ENTRY_EVENT;
    }
    if (!(ownerStatus0(mPadId) & 0x1010000))
        p->mpTornado = nullptr;
    f32 radius = gabi::fmadds(speedRadiusScale * w, w, baseRadius);
    f32 lat = gabi::fmadds(speedLatitudeScale * w, w, baseLatitude);
    f32 fovy = gabi::fmadds(speedFovyScale, w, baseFovy);
    gabi::Local<cSGlobe_l> g;
    cameraEngineDefaultGlobe(g.get());
    if (p->mpTornado.get() != nullptr) {
        gabi::Local<cSAngle_l> a;
        cSAngle_ct(a);
        bool s1 = (ownerStatus1(mPadId) & 2) != 0;
        TornadoActorViewHD *tornado = reinterpret_cast<TornadoActorViewHD *>(p->mpTornado.get());
        f32 k = w;
        if (s1) {
            cSAngle_Val(a, (s16)tornado->mRideAngle);
            k = 1.0f;
        } else {
            cSAngle_Val(a, (s16)tornado->mSideAngle);
        }
        f32 s = cameraEngineAngleSin(a.get()); /* Sin */
        f32 t = sideOffsetScale * s;
        ofs->x = gabi::fnmsubs(t, k, ofs->x);
        ofs->y = gabi::fmadds(speedHeightScale, k, ofs->y);
        w = k;
        f32 bx =
            cameraEngineBezierRatio((f32)mStickCPosXLast, 1.0f); /* dCamMath::rationalBezierRatio */
        f32 cy = mStickCPosYLast;
        f32 by = 0.0f;
        if (!(cy < 0.0f))
            by = cameraEngineBezierRatio(cy, 1.0f);
        f32 mStickCenterOffset = p->mStickCenterOffset.x;
        const f32 q = e2R(0.25f); /* true60: per-frame ratios (the per-step smoothing rule) */
        f32 n20 =
            gabi::fmadds(gabi::fmsubs(bx, -200.0f, mStickCenterOffset), q, mStickCenterOffset);
        f32 m24 = p->mStickCenterOffset.y;
        p->mStickCenterOffset.x = n20;
        p->mStickCenterOffset.y = gabi::fmadds(gabi::fmsubs(by, 500.0f, m24), q, m24);
        ofs->x = ofs->x + n20;
        ofs->y = ofs->y + p->mStickCenterOffset.y;
        if ((u16)entry->mFlags & 4) {
            gabi::Local<cSGlobe_l> g2;
            gabi::Local<cXyz> pos, x, sp;
            cameraEngineConstructGlobe(g2.get(), 200.0f, &l_tornadoZeroAngle, base.get());
            positionOf(pos, p->mpTornado.get());
            cameraEngineGlobeXyz(g2.get(), x.get());
            cXyz_pl(pos, sp, x);
            if (daSea_ChkArea(sp->x, sp->z)) {
                gabi::Local<cXyz> pos2, d;
                sp->y = daSea_calcWave(sp->x, sp->z);
                positionOf(pos2, p->mpTornado.get());
                cXyz_mi(sp, d, pos2);
                cSGlobe_Val(g2, d);
                f32 dg = cameraAngleDegree(&g2->mV);
                lat = gabi::fmadds(dg * w, 0.8f, lat);
            }
        }
        s16 inv = cSAngle_Inv(base);
        cameraEngineSetGlobeAngles(g.get(), radius, (s16)gabi::ftoi(lat * 182.04445f),
                                   inv); /* Val(f32, s16, s16) */
    } else {
        gabi::Local<cSAngle_l> dir;
        directionOf(dir, mpPlayerActor.get());
        s16 inv = cSAngle_Inv(dir);
        cameraEngineSetGlobeAngles(g.get(), radius, (s16)gabi::ftoi(lat * 182.04445f), inv);
    }
    {
        gabi::Local<cXyz> r;
        relationalPos(r, mpPlayerActor.get(), ofs);
        p->mCenterTarget.copy(*r);
    }
    if (mStyleSettle0 == 0) {
        gabi::Local<cXyz> d, s, x, e;
        gabi::Local<cSAngle_l> t10, t12, t14;
        u32 mode = (u32)p->mEntryMode;
        s32 end;
        f32 k;
        if (mode == ENTRY_SIDE_EYE) {
            k = e2R(1.0f / (f32)(s32)(40 - (s32)mCurStyleTimer)); /* true60: the frame's share per step */
            cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
            cXyz_ml(d, s, k);
            PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
            mViewCache.mEye.copy(p->mSideEye);
            cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
            cSGlobe_Val(&mViewCache.mDirection, d);
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
            if (mCurStyleTimer != 39 || !e2FrameEnd()) /* true60: settle on the frame's end */
                return true;
        } else {
            if (mode == ENTRY_EVENT) {
                end = 8;
                k = e2R(1.0f / (f32)(s32)(8 - (s32)mCurStyleTimer)); /* true60: the frame's share per step */
                cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
                gabi::Local<cXyz> mul;
                mul->x = e2R(centerFollowXZ); /* true60: per-frame ratios */
                mul->y = e2R(centerFollowY);
                mul->z = e2R(centerFollowXZ);
                cameraEngineVectorProduct(d.get(), s.get(), mul.get()); /* cXyz * cXyz */
                PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
                subtractCameraAngles(&g->mU, t14, &mViewCache.mDirection.mU);
                scaleCameraAngle(t14, t12, k);
                addCameraAngles(&mViewCache.mDirection.mU, t10, t12);
                mViewCache.mDirection.mU = copyCameraAngle(tmp, *t10);
            } else {
                end = 20;
                k = e2R(1.0f / (f32)(s32)(20 - (s32)mCurStyleTimer)); /* true60: the frame's share per step */
                cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
                cXyz_ml(d, s, k);
                PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
                f32 rr = mViewCache.mDirection.mRadius;
                mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
                subtractCameraAngles(&g->mV, t10, &mViewCache.mDirection.mV);
                scaleCameraAngle(t10, t12, k);
                addCameraAngles(&mViewCache.mDirection.mV, t14, t12);
                mViewCache.mDirection.mV = copyCameraAngle(tmp, *t14);
                subtractCameraAngles(&g->mU, t10, &mViewCache.mDirection.mU);
                scaleCameraAngle(t10, t12, k);
                addCameraAngles(&mViewCache.mDirection.mU, t14, t12);
                mViewCache.mDirection.mU = copyCameraAngle(tmp, *t14);
            }
            cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
            cXyz_pl(&mViewCache.mCenter, e, x);
            mViewCache.mEye.copy(*e);
            f32 fv = mViewCache.mFovy;
            mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
            if ((u32)mCurStyleTimer != (u32)(end - 1) || !e2FrameEnd()) /* true60: settle on the frame's end */
                return true;
        }
        mStyleSettle2 = 1;
        mStyleSettle1 = 1;
        mStyleSettle0 = 1;
        return true;
    }
    {
        gabi::Local<cXyz> d, s, mul;
        mul->x = e2R(centerFollowXZ); /* true60: per-frame ratios */
        mul->y = e2R(centerFollowY);
        mul->z = e2R(centerFollowXZ);
        cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
        cameraEngineVectorProduct(d.get(), s.get(), mul.get());
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    }
    gabi::Local<cSAngle_l> t0a, t0c, t0e, u4e, vNew, dir, t9a;
    f32 rr = mViewCache.mDirection.mRadius;
    const f32 rateStep = e2R(rate); /* true60: per-frame ratio */
    f32 R = gabi::fmadds(g->mRadius - rr, rateStep, rr);
    directionOf(dir, mpPlayerActor.get());
    {
        s16 inv = cSAngle_Inv(dir);
        cameraEngineConstructDifference(t9a.get(), inv, &mViewCache.mDirection.mU);
    }
    subtractCameraAngles(&g->mU, t0c, &mViewCache.mDirection.mU);
    scaleCameraAngle(t0c, t0a, e2R(urate)); /* true60: per-frame ratio */
    addCameraAngles(&mViewCache.mDirection.mU, u4e, t0a);
    gabi::Local<cSGlobe_l> gd8;
    {
        gabi::Local<cXyz> d;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cameraEngineConstructVectorGlobe(gd8.get(), d.get());
    }
    f32 blend = 1.0f;
    if ((u16)entry->mFlags & 4) {
        if (w > 0.5f)
            blend = 0.0f;
        else {
            f32 h = 0.5f - w;
            blend = h + h;
        }
    }
    subtractCameraAngles(&gd8->mV, t0e, &g->mV);
    scaleCameraAngle(t0e, t0a, 0.25f * blend);
    addCameraAngles(&g->mV, t0c, t0a);
    g->mV = copyCameraAngle(tmp, *t0c);
    /* true60: the latitude target leans toward the eye's own latitude (x 0.25 blend): with T = A + B x current the
     * frame's pull is (1 - B) x rate; per step e2R((1 - B) x rate) / (1 - B) (as rideCamera) */
    f32 latStep = rate;
    if (t60::dt() != 1.0f) {
        const f32 B = 0.25f * blend;
        latStep = (1.0f - B) > 0.0f ? e2R((1.0f - B) * rate) / (1.0f - B) : rateStep;
    }
    subtractCameraAngles(&g->mV, t0c, &mViewCache.mDirection.mV);
    scaleCameraAngle(t0c, t0e, latStep);
    addCameraAngles(&mViewCache.mDirection.mV, vNew, t0e);
    if ((s16)*vNew < (s16)*lo)
        cameraEngineCopyAngle(vNew.get(), lo.get());
    else if ((s16)*vNew > (s16)*hi)
        cameraEngineCopyAngle(vNew.get(), hi.get());
    cameraEngineSetGlobe(&mViewCache.mDirection, R, vNew.get(), u4e.get()); /* Val(f32, V, U) */
    {
        gabi::Local<cXyz> x, e;
        cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        f32 fv = mViewCache.mFovy;
        mViewCache.mEye.copy(*e);
        mViewCache.mFovy = gabi::fmadds(fovy - fv, rateStep, fv);
    }
    scaleCameraAngle(bankMax, t0a, mStickMainPosXLast);
    subtractCameraAngles(t0a, t0c, &mViewCache.mBank);
    scaleCameraAngle(t0c, t0e, e2R(0.01f * __builtin_fabsf(w))); /* true60: per-frame ratio */
    cameraEngineAngleAddAssign(&mViewCache.mBank, t0e.get()); /* += */
    mEventFlags = mEventFlags | CAMERA_UPDATE_VIEW;
    return true;
}

/* work area of manualCamera (mWork) */

static inline bool rangeAdd(void *p, f32 add, f32 lo, f32 hi) {
    return cameraEngineRangeAdd(p, add, lo, hi);
} /* limited_range_addition */

/* true60 (manualCamera): this step is the first manualCamera runs in its 30 Hz frame (the
 * frame's first step, or a step after a gap in its calls: host memory, this unit, one camera). dt = 1: true. */
static camera_key_t60::PerCam<uint32_t> s_manStamp;
static inline bool manFrameFirst() {
    if (t60::dt() == 1.0f)
        return true;
    const uint32_t now = t60_step_counter();
    const bool first = t60::first_step() || s_manStamp() + 1 != now;
    s_manStamp() = now;
    return first;
}
/* the per-frame decisions of manualCamera at N steps (host memory, this unit, one camera), made on the frame's first
 * step (manFrameFirst) and kept by its later steps, as the original makes them once a frame from the frame
 * start's state: the C stick's range tests (slot 0 centre height, 1 radius, 2 latitude, 3 fovy: blocked and the limit),
 * the smoothed turn's chase step (0.1 or 0.18 by its sign against the turn) and the lock-on blend's branch. */
static camera_key_t60::PerCamArr<bool, 4> s_manBlocked;
static camera_key_t60::PerCamArr<f32, 4> s_manLimit;
static camera_key_t60::PerCam<f32> s_manChaseStep;
static camera_key_t60::PerCam<int> s_manBlendWay;
/* the lock-on blend's branch (1 the 0.05 step, 2 the clamp to the end value, 0 none): the frame start's (the original
 * steps by 0.05 a frame and may overshoot the end, clamped the next frame; at N steps the frame's steps step on to the
 * frame's overshoot and the next frame clamps). dt = 1: WAY itself. */
static inline int manBlendWay(int way, bool first) {
    if (t60::dt() == 1.0f)
        return way;
    if (first)
        s_manBlendWay() = way;
    return s_manBlendWay();
}
/* the C stick's range additions at N steps. Per 30 Hz frame the stick adds ADD to a value (limited_range_addition
 * 024F7DF4: the sum clamped into [lo, hi], false when it left the range; a descending range adds -ADD), and the caller
 * takes the blocked follow rate when it did. Per step: the frame's first step tests the frame's whole add from the
 * frame start's value (the original's test); inside, every step adds ADD x dt unclamped (the steps sum to ADD, which
 * ends inside even when the frame started outside the range, as the original's one add does); outside, every
 * step sets the value to the limit the frame's sum went past and answers false. The centre height and fovy pass a copy
 * of their current value (the target is that plus the add): their later steps' copies have moved, the decision and the
 * limit stay the frame's. dt = 1: limited_range_addition itself. */
static inline bool manRangeAdd(int slot, be<f32> *p, f32 add, f32 lo, f32 hi, bool first) {
    const f32 dt = t60::dt();
    if (dt == 1.0f)
        return rangeAdd(p, add, lo, hi);
    const f32 a = lo > hi ? -add : add; /* (limited_range_addition's descending range) */
    if (first) {
        const f32 mn = lo > hi ? hi : lo, mx = lo > hi ? lo : hi;
        const f32 v = gabi::fadds_ppc((f32)*p, a);
        s_manBlocked()[slot] = v < mn || v > mx;
        s_manLimit()[slot] = v < mn ? mn : mx;
    }
    const f32 cur = *p;
    rangeAdd(p, add * dt, lo, hi); /* (the original's call, its answer and value replaced) */
    if (!s_manBlocked()[slot]) { /* (unclamped: the frame's sum ends inside; a start outside the range moves in) */
        *p = gabi::fadds_ppc(cur, a * dt);
        return true;
    }
    *p = s_manLimit()[slot];
    return false;
}
/* the yaw / latitude targets are s16 angles stored from a float degree by truncation (ftoi(deg x 182.04445)): per 30 Hz
 * frame one truncation of (old + the frame's add). At N steps each step would truncate its own share (a slow turn of less
 * than a unit per step would never move). The steps carry the truncated remainder (host memory, degrees) to the next
 * step of the frame and the frame's last step drops it as the frame does: the frame end lands on
 * ftoi((old + the sum of the steps' adds) x 182.04445). A value someone else changed since the last store drops the
 * carry. Slot 0 the latitude, 1 the yaw. dt = 1: no carry. */
static camera_key_t60::PerCamArr<f32, 2> s_manCarry;
static camera_key_t60::PerCamArr<s16, 2> s_manStored;
static inline f32 manCarryIn(int slot, s16 now) {
    return !t60::first_step() && s_manStored()[slot] == now ? s_manCarry()[slot] : 0.0f;
}
static inline s16 manCarryOut(int slot, f32 deg) {
    const s16 v = (s16)gabi::ftoi(deg * 182.04445f);
    if (t60::dt() != 1.0f) {
        gabi::Local<be<s16>> a;
        *a = v;
        s_manStored()[slot] = v;
        s_manCarry()[slot] = t60::tick30() ? 0.0f : deg - cameraEngineAngleDegree(a.get());
    }
    return v;
}

// HD: GameCube body is undecompiled.
/* 0250FDC8 */
/* Free camera driven by the C stick (radius, latitude and
 * fovy within the style's ranges, yaw by a curved stick response with the HD
 * camera-x option); the centre follows the player (or the lock-on target with
 * wall checks) with an adjustable height; crawling, hanging, deku leaf and
 * seagull states adjust the ranges. */
bool dCamera_c::manualCamera(s32 style) {
    ManualWorkHD *p = reinterpret_cast<ManualWorkHD *>(mWork);
    f32 centerHeightLower = camParamVal(style, 7);
    f32 fovyLower = camParamVal(style, 27);
    f32 centerHeightStep = camParamVal(style, 9);
    f32 yawStep = camParamVal(style, 24);
    f32 centerOffsetZ = camParamVal(style, 0);
    f32 centerFollowXZ = camParamVal(style, 3);
    f32 maximumRadius = camParamVal(style, 12);
    f32 fovyStep = camParamVal(style, 29);
    f32 minimumRadius = camParamVal(style, 11);
    f32 latitudeStep = camParamVal(style, 19);
    f32 minimumLatitude = camParamVal(style, 16);
    f32 centerHeightUpper = camParamVal(style, 6);
    f32 fovyUpper = camParamVal(style, 26);
    f32 radiusStep = camParamVal(style, 14);
    f32 followRate = camParamVal(style, 21);
    f32 blockedFollowRate = camParamVal(style, 20);
    f32 fovyClampFollowRate = blockedFollowRate; /* table value (the override below changes
                                                    blockedFollowRate) */
    f32 maximumLatitude = camParamVal(style, 17);
    f32 centerFollowY = camParamVal(style, 4);
    f32 centerOffsetX = camParamVal(style, 1);
    /* true60: manualCamera at N steps per 30 Hz frame. The style's start-up runs once per style
     * start (the mode-start-once rule; the start frame's later steps run as the original's next frame, the 30 Hz timer rule). Per step x dt: the
     * stick turn's linear chase (cLib_chaseF 0.25 / 0.1 / 0.18 per frame; cLib_chaseF scales its step itself), the
     * stick's range additions (centre height,
     * radius, latitude, fovy: manRangeAdd), the yaw turn (ax x yawStep per frame; the s16 targets carry their truncated
     * remainder inside the frame: manCarryOut), the lock-on blend (+-0.05 per frame) and the leaf's fovy swing. Per step
     * (e2R): the ratios toward a fixed target (the centre follow mx / my and their 0.05 chase, the view's radius and
     * angle chases, the centre height and fovy at a limit or at Ganon's -50, the leaf bank). The chases whose target is
     * the value plus the stick's add (centre height and fovy inside their ranges) keep their ratio: the add is x dt.
     * The frame's decisions (the range tests, the turn chase's step, the lock-on blend's branch) are made on its first
     * step from the frame start's state and kept by the later steps.
     * The fan's random fovy kick once per frame (tick30: one draw, the full kick). The port's mouse / swipe camera
     * (PORT_CAMERA_*) gives its per-frame amounts on every step of the frame, which
     * the x dt here spreads over the steps. dt = 1: the original. */
    const f32 manDt = t60::dt();
    const bool manStart0 = e2Start(mCurStyleTimer, 5);
    const bool manFirst = manFrameFirst(); /* the frame's first step: its decisions (s_manBlocked ...) */
#define manualStart (manDt == 1.0f ? mCurStyleTimer == 0 : manStart0)
    if (manualStart) {
        gabi::Local<cXyz> a, d;
        mStyleSettle0 = 1;
        mStyleSettle1 = 1;
        mStyleSettle2 = 1;
        if ((mEventFlags & CAMERA_MANUAL_LOCKON) && mpLockonTarget.get() != nullptr)
            p->mLockonBlend = 1.0f;
        else
            p->mLockonBlend = 0.0f;
        attentionPos(a, mpPlayerActor.get());
        p->mCenterHeight = mViewCache.mCenter.y - a->y;
        p->mInitialRadius = minimumRadius;
        cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, d);
        p->mChargingAtStart = (ownerStatus1(mPadId) >> 17) & 1;
        copyCameraWord(&p->mDirection.mRadius, &mViewCache.mDirection.mRadius);
        copyCameraWord(&p->mDirection.mV, &mViewCache.mDirection.mV); /* V and U */
        p->mLockonCenterOffset.copy(cXyz::Zero);                      /* cXyz::Zero */
        p->mStateCounter = 0;
    }
    if (ownerStatus0(mPadId) & 0x8000000) { /* CRAWL */
        if (minimumLatitude < 4.0f)
            minimumLatitude = 4.0f;
        if (centerHeightUpper < -10.0f)
            centerHeightUpper = -10.0f;
    } else if ((ownerStatus0(mPadId) & 0x2800100) || (ownerStatus1(mPadId) & 0x10020) ||
               (u8)l_manualMakarFlying != 0 || (u8)l_manualMedliFlying != 0) {
        if (ownerStatus0(mPadId) & 0x2000000) {
            maximumLatitude = 70.0f;
            minimumLatitude = -58.0f;
        } else {
            maximumLatitude = 70.0f;
            minimumLatitude = -70.0f;
        }
        latitudeStep = 4.0f;
        centerFollowXZ = 0.45f;
        if (maximumRadius > 500.0f)
            maximumRadius = 500.0f;
    } else if (ownerStatus1(mPadId) & 0x40000) {
        minimumLatitude = -10.0f;
        maximumLatitude = 10.0f;
    }
    if (p->mChargingAtStart == 0 && (ownerStatus1(mPadId) & 0x20000)) {
        mManualInactive = 1;
        mEventFlags = mEventFlags | CAMERA_MANUAL_ROTATING;
    }
    if ((ownerStatus0(mPadId) & 0x800000) && mIsGanonKMap != 0) {
        centerHeightLower = 0.0f;
        fovyStep = 1.0f;
        followRate = 0.2f;
        fovyUpper = 55.0f;
        fovyLower = 55.0f;
        centerFollowXZ = 0.75f;
        centerFollowY = 0.25f;
        blockedFollowRate = 0.33f;
        centerHeightUpper = 0.0f;
        yawStep = 6.0f;
        latitudeStep = 6.0f;
        minimumLatitude = 60.0f;
        maximumLatitude = -60.0f;
        centerOffsetX = 0.0f;
        centerOffsetZ = 1.0f;
        minimumRadius = 300.0f;
        maximumRadius = 300.0f;
        centerHeightStep = 0.0f;
        radiusStep = 1.0f;
    }
    /* C stick */
    port_rate_t sign; /* the vertical rate: f64 in the game build, where the port may set it (port_hooks.h) */
    f32 ax;
    f32 cy = mStickCPosYLast;
    if (!(cy < 0.75f)) {
        ax = __builtin_fabsf(mStickCPosXLast);
        sign = 1.0f;
    } else if (!(cy > -0.75f)) {
        ax = __builtin_fabsf(mStickCPosXLast);
        sign = -1.0f;
    } else {
        sign = cameraEngineBezierRatio(cy * 1.333333f, 2.0f); /* dCamMath::rationalBezierRatio */
        ax = __builtin_fabsf(mStickCPosXLast);
    }
    f32 b = cameraEngineBezierRatio(ax, 0.13f);
    f32 b87 = cameraEngineBezierRatio(0.87f, 0.13f);
    f32 turn = b * b;
    f32 cx = mStickCPosXLast;
    if (ax < 0.87f)
        turn = (ax / 0.87f) * (b87 * b87);
    if (!(cx > 0.0f))
        turn = -turn;
    {
        be<f32> *smoothedTurn = &mSmoothedCStickX; // HD: third stick-filter float at 0x154.
        if (ax < 0.01f) {
            cameraEngineChaseFloat(smoothedTurn, turn, 0.25f); /* cLib_chaseF (true60: it takes x dt itself) */
            PORT_CAMERA_MANUAL_AT(0x02510448, this, yawStep);
        } else {
            f32 step = 0.1f;
            if ((f32)*smoothedTurn * turn < 0.0f)
                step = 0.18f;
            if (manDt != 1.0f) { /* true60: the frame's step (its first step's sign test); cLib_chaseF takes x dt */
                if (manFirst)
                    s_manChaseStep() = step;
                step = s_manChaseStep();
            }
            cameraEngineChaseFloat(smoothedTurn, turn, step);
            PORT_CAMERA_MANUAL_AT(0x025104A0, this, yawStep);
        }
        ax = (f32)*smoothedTurn * 0.92f;
        CameraOptionViewHD *options =
            cameraEngineGetOptions(dComIfGs_ea() + 0x12C0); /* HD: save options */
        if ((u8)options->mCameraX == 1)
            ax = -ax; /* camera x inverted */
    }
    f32 rH = followRate, rR = followRate, rV = followRate, rU = followRate;
    if (((ownerStatus0(mPadId) & 0x6800061) || (ownerStatus1(mPadId) & 0x10000)) &&
        mDMCSystem.field_0x0 == 0)
        setDMCAngle();
    sign = PORT_CAMERA_RATE(0x0251053C, this, sign, latitudeStep);
    {
        gabi::Local<be<f32>> h;
        copyCameraWord(h.get(), &p->mCenterHeight);
        bool hFree = true; /* true60: the target is the height plus the stick's add (x dt): the ratio stays */
        if (!manRangeAdd(0, h.get(), -(sign * centerHeightStep), centerHeightUpper,
                         centerHeightLower, manFirst)) {
            rH = blockedFollowRate;
            hFree = false;
        }
        if ((mEventFlags & CAMERA_MANUAL_LOCKON) && mpLockonTarget.get() != nullptr &&
            (mIsGanonKMap != 0 || mIsGTowerMap != 0)) {
            *h = -50.0f;
            f32 mCenterHeight = p->mCenterHeight;
            p->mCenterHeight = gabi::fmadds(-50.0f - mCenterHeight, e2R(rH), mCenterHeight); /* true60: fixed target */
        } else {
            f32 mCenterHeight = p->mCenterHeight;
            p->mCenterHeight = gabi::fmadds(*h - mCenterHeight, hFree ? rH : e2R(rH), mCenterHeight); /* true60: the limit */
        }
    }
    if (ownerStatus0(mPadId) & 0x100) {
        if (centerOffsetZ > -10.0f)
            centerOffsetZ = -10.0f;
        if (p->mCenterHeight < 30.0f)
            p->mCenterHeight = 30.0f;
    }
    gabi::Local<cXyz> ofs;
    ofs->x = centerOffsetX;
    copyCameraWord(&ofs->y, &p->mCenterHeight);
    ofs->z = centerOffsetZ;
    if ((mEventFlags & CAMERA_MANUAL_LOCKON) && mpLockonTarget.get() != nullptr) {
        gabi::Local<cXyz> r, rp, d, t, s;
        gabi::Local<CameraLineCheckHD> lc1, lc2;
        cameraEngineRelationalPosition(this, r.get(), mpPlayerActor.get(), mpLockonTarget.get(),
                                       ofs.get(), 0.5f); /* relationalPos(a1, a2, ofs, 0.5) */
        p->mCenterTarget.copy(*r);
        relationalPos(rp, mpPlayerActor.get(), ofs);
        constructCameraLine(lc1);
        constructCameraLine(lc2);
        bool blocked = false;
        if (lineBGCheck(rp, &p->mCenterTarget, (u8 *)lc1.get(), 0x7F) &&
            lineBGCheckBoth(&mEye, rp, (u8 *)lc2.get(), 0x7F)) {
            PlaneEquationHD *plane = reinterpret_cast<PlaneEquationHD *>(
                cameraEngineGetTriPlane(dComIfG_Bgsp(), (u32)lc2->mBgIndex, (u32)lc2->mPolyIndex));
            blocked = true;
            if (plane != nullptr) {
                f32 dot = PSVECDotProduct(&plane->mNormal, &mEye);
                if (dot + (f32)plane->mDistance < 0.0f)
                    mEventFlags = mEventFlags | 0x80000000; /* eye behind the wall */
            }
        }
        if (!blocked) {
            f32 w = getWaterSurfaceHeight(&p->mCenterTarget);
            if (w > p->mCenterTarget.y)
                p->mCenterTarget.y = w;
            cXyz_mi(&p->mCenterTarget, d, rp);
            p->mLockonCenterOffset.copy(*d);
            f32 bl = p->mLockonBlend;
            const int way = manBlendWay(bl < 1.0f ? 1 : bl > 1.0f ? 2 : 0, manFirst); /* true60: the frame's branch */
            if (way == 1) {
                f32 nb = bl + 0.05f * manDt; /* true60: x dt */
                p->mLockonBlend = nb;
                cXyz_ml(&p->mLockonCenterOffset, t, nb);
                cXyz_pl(rp, s, t);
                p->mCenterTarget.copy(*s);
            } else if (way == 2) {
                p->mLockonBlend = 1.0f;
            }
        }
        destroyCameraLine(lc2);
        destroyCameraLine(lc1);
    } else {
        gabi::Local<cXyz> r, t, s;
        relationalPos(r, mpPlayerActor.get(), ofs);
        p->mCenterTarget.copy(*r);
        f32 w = getWaterSurfaceHeight(&p->mCenterTarget);
        f32 bl = p->mLockonBlend;
        if (w > p->mCenterTarget.y)
            p->mCenterTarget.y = w;
        const int way = manBlendWay(bl > 0.0f ? 1 : bl < 0.0f ? 2 : 0, manFirst); /* true60: the frame's branch */
        if (way == 1) {
            f32 nb = bl - 0.05f * manDt; /* true60: x dt */
            p->mLockonBlend = nb;
            cXyz_ml(&p->mLockonCenterOffset, t, nb);
            cXyz_pl(&p->mCenterTarget, s, t);
            p->mCenterTarget.copy(*s);
        } else if (way == 2) {
            p->mLockonBlend = 0.0f;
        }
    }
    {
        gabi::Local<cXyz> d, s, mul;
        f32 mx, my;
        if (manualStart) {
            cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
            f32 l = std_sqrtf(PSVECSquareMag(d));
            f32 k = (l > 100.0f) ? 0.0f : 1.0f - l / 100.0f;
            mx = centerFollowXZ * k;
            my = centerFollowY * k;
        } else {
            f32 a = p->mCenterFollowXZ, c = p->mCenterFollowY;
            mx = gabi::fmadds(centerFollowXZ - a, e2R(0.05f), a); /* true60: per-frame ratios */
            my = gabi::fmadds(centerFollowY - c, e2R(0.05f), c);
        }
        p->mCenterFollowXZ = mx;
        p->mCenterFollowY = my;
        mul->x = e2R(mx); /* true60: per-frame ratios */
        mul->z = e2R(mx);
        mul->y = e2R(my);
        cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
        cameraEngineVectorProduct(d.get(), s.get(), mul.get()); /* cXyz * cXyz */
        PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    }
    if (mpLockonTarget.get() == nullptr && mIsSeaMap == 0) {
        gabi::Local<cXyz> a;
        gabi::Local<CameraLineCheckHD> lc;
        attentionPos(a, mpPlayerActor.get());
        if (ownerStatus0(mPadId) & 0x8100100) {
            gabi::Local<cXyz> ep;
            eyePos(ep, mpPlayerActor.get());
            a->y = ep->y + 30.0f;
        } else {
            a->y = a->y - 15.0f;
        }
        constructCameraLine(lc);
        checkCameraWall(this, a, lc);
        destroyCameraLine(lc);
    }
    gabi::Local<be<f32>> R, lat, fv;
    copyCameraWord(R.get(), &p->mDirection.mRadius);
    if (!manRangeAdd(1, R.get(), -(sign * radiusStep), minimumRadius, maximumRadius, manFirst))
        rR = blockedFollowRate;
    *lat = cameraEngineAngleDegree(&p->mDirection.mV); /* Degree */
    if (manDt != 1.0f) /* true60: the latitude's truncated remainder inside the frame */
        *lat = (f32)*lat + manCarryIn(0, (s16)p->mDirection.mV);
    if (!manRangeAdd(2, lat.get(),
                     PORT_CAMERA_MANUAL(0x02510DEC, this, -(sign * latitudeStep), minimumLatitude,
                                        maximumLatitude),
                     minimumLatitude, maximumLatitude, manFirst))
        rV = blockedFollowRate;
    {
        f32 du = cameraEngineAngleDegree(&p->mDirection.mU);
        f32 nu;
        if (manDt != 1.0f) /* true60: the yaw x dt, its truncated remainder inside the frame */
            nu = gabi::fmadds(ax, yawStep * manDt, du + manCarryIn(1, (s16)p->mDirection.mU));
        else
            nu = gabi::fmadds(ax, yawStep, du);
        s16 sv = manCarryOut(0, *lat);
        s16 su = manCarryOut(1, nu);
        cameraEngineSetGlobeAngles(&p->mDirection, (f32)*R, sv, su); /* Val(f32, s16, s16) */
    }
    if ((mEventFlags & CAMERA_MANUAL_LOCKON) && mpLockonTarget.get() != nullptr)
        mEventFlags = mEventFlags | 0x2000;
    {
        gabi::Local<cSAngle_l> t34, t32, t30, tmp;
        gabi::Local<cXyz> x, e;
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(p->mDirection.mRadius - rr, e2R(rR), rr); /* true60: per-frame ratios */
        subtractCameraAngles(&p->mDirection.mV, t34, &mViewCache.mDirection.mV);
        scaleCameraAngle(t34, t32, e2R(PORT_CAMERA_MANUAL(0x02510EA0, this, rV, 0.0f, 0.0f)));
        addCameraAngles(&mViewCache.mDirection.mV, t30, t32);
        mViewCache.mDirection.mV = copyCameraAngle(tmp, *t30);
        subtractCameraAngles(&p->mDirection.mU, t30, &mViewCache.mDirection.mU);
        scaleCameraAngle(t30, t32, e2R(PORT_CAMERA_MANUAL(0x02510EE4, this, rU, 0.0f, 0.0f)));
        addCameraAngles(&mViewCache.mDirection.mU, t34, t32);
        mViewCache.mDirection.mU = copyCameraAngle(tmp, *t34);
        cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
        cXyz_pl(&mViewCache.mCenter, e, x);
        mViewCache.mEye.copy(*e);
    }
    f32 rF = rH;
    copyCameraWord(fv.get(), &mViewCache.mFovy);
    bool fFree = true; /* true60: as the centre height */
    if (!manRangeAdd(3, fv.get(), -(sign * fovyStep), fovyUpper, fovyLower, manFirst)) {
        rF = fovyClampFollowRate;
        fFree = false;
    }
    f32 cur = mViewCache.mFovy;
    f32 q = (*R - minimumRadius) / (maximumRadius - minimumRadius);
    mViewCache.mFovy = gabi::fmadds(*fv - cur, fFree ? rF : e2R(rF), cur);
    f32 w = 1.0f - q;
    if ((ownerStatus1(mPadId) & 0x40) && (manDt == 1.0f || t60::tick30())) { /* DEKU_LEAF_FAN; true60: once a frame */
        f32 r = cM_rndFX((f32) reinterpret_cast<ManualSetupViewHD *>(mCamSetup)->mFanFovySwing * w);
        mViewCache.mFovy = mViewCache.mFovy + r;
    }
    if (ownerStatus1(mPadId) & 0x20) { /* deku leaf flying */
        gabi::Local<cXyz> pos;
        positionOf(pos, mpPlayerActor.get());
        pos->y = pos->y + 10.0f;
        f32 foot = footHeightOf(mpPlayerActor.get());
        f32 h = foot - groundHeight(pos);
        u32 c7c = mFrameCounter;
        f32 bank = mStickMainPosXLast;
        f32 ratio = 0.05f;
        if (h < 200.0f) {
            f32 qq = h / 200.0f;
            ratio = gabi::fnmsubs(0.95f, qq, 1.0f);
            bank = bank * qq;
        }
        gabi::Local<cSAngle_l> t36, t3a, t38;
        cSAngle_l *sp = cameraEngineConstructAngle(t36.get(), (s16)(c7c << 7));
        f32 sn = cameraEngineAngleSin(sp); /* Sin */
        mViewCache.mFovy = gabi::fmadds(
            (f32) reinterpret_cast<ManualSetupViewHD *>(mCamSetup)->mLeafFovySwing * sn * manDt, w,
            mViewCache.mFovy); /* true60: x dt */
        f32 fan = cameraEngineFanBank(mCamSetup); /* dCamSetup_c::FanBank */
        cameraEngineConstructDegrees(t3a.get(), (fan * bank) * w);
        subtractCameraAngles(t3a, t38, &mViewCache.mBank);
        scaleCameraAngle(t38, t36, e2R(ratio)); /* true60: per-frame ratio */
        cameraEngineAngleAddAssign(&mViewCache.mBank, t36.get()); /* += */
        mEventFlags = mEventFlags | CAMERA_UPDATE_VIEW;
    }
    return true;
#undef manualStart
}

// HD: GameCube body is undecompiled.
/* Tower camera: a follow camera that keeps the map arrow (the
 * tower) in view: the yaw eases between the param longitude range around the
 * tower direction, scaled by the stick; without an arrow followCamera runs
 * first. */
/* 0250B30C */
bool dCamera_c::towerCamera(s32 style) {
  TowerWorkHD *p = reinterpret_cast<TowerWorkHD *>(mWork);
  f32 movementBlendLimit = 0.9f;
  gabi::Local<cSAngle_l> chargeLatitude, maximumLatitude, latitudeMinimum,
      latitudeMaximum, yawRangeStart, yawRangeEnd, angleCopy;
  cameraEngineConstructDegrees(
      chargeLatitude.get(),
      (f32) reinterpret_cast<TowerSetupHD *>(mCamSetup)->mChargeLatitude);
  cameraEngineConstructDegrees(maximumLatitude.get(), 80.0f);
  f32 minimumRadius = camParamVal(style, 11);
  f32 maximumRadius = camParamVal(style, 10);
  f32 centerFollowY = camParamVal(style, 4);
  f32 radiusRate = camParamVal(style, 14);
  f32 centerOffsetY = camParamVal(style, 5);
  f32 centerFollowXZ = camParamVal(style, 3);
  f32 nearCenterHeight = camParamVal(style, 6);
  f32 centerOffsetX = camParamVal(style, 1);
  f32 latitude = camParamVal(style, 15);
  f32 centerOffsetZ = camParamVal(style, 0);
  f32 radiusLimitRate = camParamVal(style, 13);
  cameraEngineConstructDegrees(latitudeMinimum.get(), camParamVal(style, 16));
  cameraEngineConstructDegrees(latitudeMaximum.get(), camParamVal(style, 17));
  f32 fovy = camParamVal(style, 25);
  f32 yawCurveWeight = camParamVal(style, 27);
  f32 fovyRate = camParamVal(style, 29);
  f32 latitudeRate = camParamVal(style, 19);
  cameraEngineConstructDegrees(yawRangeStart.get(), camParamVal(style, 23));
  cameraEngineConstructDegrees(yawRangeEnd.get(), camParamVal(style, 24));
  f32 movingLongitudeRate = camParamVal(style, 20);
  f32 idleLongitudeRate = camParamVal(style, 21);
  (void)dComIfGp_ea(); /* dComIfGp_getAttention() (unused) */
  if (mCurArrowIdx == 0xFF)
    followCamera(style);
  if ((u8)l_manualMakarFlying != 0 || (u8)l_manualMedliFlying != 0) {
    movementBlendLimit = 0.66f;
    if (latitude < 20.0f)
      latitude = 20.0f;
    cameraRideScaleAngleAssign(&mForwardTiltOffset, 0.33f); /* mForwardTiltOffset *= 0.33f */
  }
  if (ownerStatus0(mPadId) & 0x8100000) { /* CRAWL | SWIM */
    gabi::Local<cSAngle_l> a;
    cSAngle_l *ap = cameraEngineConstructDegrees(a.get(), 4.0f);
    if ((s16)*latitudeMinimum < (s16)*ap)
      cameraEngineSetDegrees(latitudeMinimum.get(), 4.0f);
    if (centerOffsetY < -10.0f)
      centerOffsetY = -10.0f;
  }
  if (ownerStatus1(mPadId) & 0x40000) {
    latitude = 0.0f;
    mForwardTiltOffset = (s16)l_tornadoZeroAngle;
  }
  if (ownerStatus0(mPadId) & 0x20)
    centerOffsetX = -centerOffsetX;
  const bool towerStart = e2Start(mCurModeTimer, 3); /* the mode-start-once rule: once per mode start */
  if (towerStart) {
    p->mLookLeft = 0;
    p->mCenterOffsetX = 0.0f;
  }
  if (((mEventFlags & CAMERA_FORCE_FOLLOW) &&
       (ownerStatus0(mPadId) & 0xA5000)) ||
      (u8)l_manualMakarFlying != 0 || (u8)l_manualMedliFlying != 0) {
    f32 tgt;
    if (mStickMainPosXLast < -0.2f)
      p->mLookLeft = 1;
    if (mStickMainPosXLast > 0.2f) {
      p->mLookLeft = 0;
      tgt = 45.0f;
    } else if (p->mLookLeft != 0) {
      tgt = -45.0f;
    } else {
      tgt = 45.0f;
    }
    f32 c = p->mCenterOffsetX;
    p->mCenterOffsetX = gabi::fmadds(tgt - c, e2R(0.04f), c); /* true60: per-frame ratio */
  } else {
    f32 c = p->mCenterOffsetX;
    p->mCenterOffsetX = gabi::fmadds(centerOffsetX - c, e2R(0.06f), c); /* true60: per-frame ratio */
  }
  gabi::Local<cXyz> centerOffset;
  {
    f32 x = p->mCenterOffsetX;
    f32 t = cameraEngineLimit((mViewCache.mDirection.mRadius - minimumRadius) /
                                  (maximumRadius - minimumRadius),
                              0.0f, 1.0f); /* limitf */
    centerOffset->z = centerOffsetZ * t;
    centerOffset->x = x;
    centerOffset->y =
        gabi::fmadds(centerOffsetY - nearCenterHeight, t, nearCenterHeight);
  }
  f32 range = maximumRadius - minimumRadius;
  /* (dt = 1: the timer read again here, in the original order: a callee may have changed it since) */
  if (t60::dt() == 1.0f ? mCurModeTimer == 0 : towerStart) {
    gabi::Local<cXyz> d;
    p->mTag = TOWER_WORK; /* 'TOWR' */
    cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, d);
    p->mMovementBlend = 0.0f;
    copyCameraWord(&p->mMaximumRadius, &mDirection.mRadius);
    p->mFallFrames = 0;
    p->mIdleFrames = 0;
    copyCameraWord(&p->mMinimumRadius, &mDirection.mRadius);
    f32 deg = cameraAngleDegree(&mDirection.mV);
    p->mInitialLatitude = deg;
    p->mLatitude = deg;
    p->mCenter.copy(mViewCache.mCenter);
    p->mInitialized = 1;
    p->mLongitudeRate = 0.01f;
    p->mLatitudeRate = 0.01f;
    p->mFovyRate = 0.01f;
    p->mRadiusRate = 0.75f;
    p->mCenterFollowXZ = centerFollowXZ;
    p->mCenterFollowY = centerFollowY;
    p->mStateCounter = 0;
    copyCameraWord(&p->mInitialFovy, &mFovy);
    p->mHeightBlend = 0.0f;
    p->mLongitude = (s16)mViewCache.mDirection.mU;
    copyCameraWord(&p->mTowerPosition.x,
                   &reinterpret_cast<TowerArrowHD *>(mCurRoomArrowEntry)
                        ->mPosition.x); /* mCurRoomArrowEntry.position */
    copyCameraWord(
        &p->mTowerPosition.y,
        &reinterpret_cast<TowerArrowHD *>(mCurRoomArrowEntry)->mPosition.y);
    copyCameraWord(
        &p->mTowerPosition.z,
        &reinterpret_cast<TowerArrowHD *>(mCurRoomArrowEntry)->mPosition.z);
    p->mYawRangeSide = ((s16)*yawRangeStart > (s16)*cameraRideRightAngle())
                           ? 0
                           : 1; /* mfcr: !CR0[GT] (<= _90) */
    if ((mEventFlags & CAMERA_FAST_TRANSITION) || mCurMode == 1) {
      mStyleSettle2 = 1;
      mStyleSettle1 = 1;
      mStyleSettle0 = 1;
      p->mFrameWeight = 0.0f;
      p->mTransitionFrames = 1;
    } else {
      gabi::Local<cXyz> rel;
      relationalPos(rel, mpPlayerActor.get(), centerOffset);
      cXyz_mi(&mEye, d, rel);
      f32 l1 = std_sqrtf(PSVECSquareMag(d)) - maximumRadius;
      cXyz_mi(&mCenter, d, rel);
      f32 l2 = std_sqrtf(PSVECSquareMag(d)) - maximumRadius;
      f32 m = (l2 - l1 >= 0.0f) ? l2 : l1; /* fsel */
      m = __builtin_fabsf(m);
      f32 h = heightOf(mpPlayerActor.get());
      f32 hh = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
      f32 r = std_sqrtf(m / hh);
      s32 k = gabi::ftoi(8.0f * r);
      s32 n = k + 1;
      p->mTransitionFrames = n;
      p->mRemainingWeight = (f32)((n * (k + 2)) >> 1);
      p->mFrameWeight = 0.0f;
    }
  }
  gabi::Local<cSGlobe_l> towerDirection, offsetDirection;
  gabi::Local<cXyz> target;
  {
    gabi::Local<cXyz> a, d, x, b;
    gabi::Local<cSAngle_l> t14;
    attentionPos(a, mpPlayerActor.get());
    cXyz_mi(a, d, &p->mTowerPosition);
    cameraEngineConstructVectorGlobe(towerDirection.get(), d.get());
    cameraEngineConstructVectorGlobe(offsetDirection.get(), centerOffset.get());
    addCameraAngles(&offsetDirection->mU, t14, &towerDirection->mU);
    offsetDirection->mU = copyCameraAngle(angleCopy, *t14);
    attentionPos(b, mpPlayerActor.get());
    cameraEngineGlobeXyz(offsetDirection.get(), x.get());
    cXyz_pl(b, a, x);
    target->copy(*a);
    target->y = getWaterSurfaceHeight(target);
  }
  gabi::Local<CameraLineCheckHD> lineCheck;
  if (mStyleSettle0 == 0) {
    gabi::Local<cXyz> d, s, a, x, e;
    gabi::Local<cSAngle_l> vv, ay, t40, t2e, t28;
    gabi::Local<cSGlobe_l> g;
    if (mFollowMoveBg != 0)
      cameraTowerMoveBackground(dComIfG_Bgsp(),
                                static_cast<cBgS_PolyInfo *>(&mBG.mCameraGround.mGndChk), 1,
                                &p->mCenter, 0, 0); /* MoveBgMatrixCrrPos */
    f32 r = (f32)(s32)((s32)p->mTransitionFrames - (s32)mCurModeTimer);
    f32 k = e2R(r / p->mRemainingWeight); /* true60: the frame's share per step */
    p->mFrameWeight = r;
    cXyz_mi(target, d, &p->mCenter);
    cXyz_ml(d, s, k);
    PSVECAdd(&p->mCenter, s, &p->mCenter);
    cXyz_mi(&p->mCenter, s, &mViewCache.mCenter);
    cXyz_ml(s, d, e2R(centerFollowXZ)); /* true60: per-frame ratio */
    PSVECAdd(&mViewCache.mCenter, d, &mViewCache.mCenter);
    attentionPos(a, mpPlayerActor.get());
    a->y = a->y - 15.0f;
    constructCameraLine(lineCheck);
    checkCameraWall(this, a, lineCheck);
    f32 R = cameraEngineLimit((f32)mViewCache.mDirection.mRadius, minimumRadius,
                              maximumRadius); /* limitf */
    cSAngle_ct(vv);
    s16 V = mViewCache.mDirection.mV;
    if (V < (s16)*latitudeMinimum)
      V = *latitudeMinimum;
    if (V > (s16)*latitudeMaximum)
      V = *latitudeMaximum;
    *vv = V;
    s16 inv = cSAngle_Inv(&mAngleY);
    cSAngle_l *ayp = cameraEngineConstructAngle(ay.get(), inv);
    cameraEngineConstructGlobe(g.get(), R, vv.get(), ayp);
    f32 rr = mViewCache.mDirection.mRadius;
    mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
    subtractCameraAngles(&g->mV, t40, &mViewCache.mDirection.mV);
    scaleCameraAngle(t40, t2e, k);
    addCameraAngles(&mViewCache.mDirection.mV, t28, t2e);
    mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *t28);
    cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, e, x);
    mViewCache.mEye.copy(*e);
    if ((u32)mCurModeTimer >= (u32)((s32)p->mTransitionFrames - 1) && e2FrameEnd()) {
      mStyleSettle2 = 1;
      mStyleSettle1 = 1;
      mStyleSettle0 = 1;
    }
    p->mLatitude = cameraAngleDegree(&mViewCache.mDirection.mV);
    copyCameraWord(&p->mMaximumRadius, &mViewCache.mDirection.mRadius);
    copyCameraWord(&p->mMinimumRadius, &mViewCache.mDirection.mRadius);
    f32 fv = mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
    if (e2FrameEnd()) /* true60: the weights count 30 Hz frames */
        p->mRemainingWeight = p->mRemainingWeight - p->mFrameWeight;
    destroyCameraLine(lineCheck);
    return true;
  }
  {
    gabi::Local<cXyz> pos;
    positionOf(pos, mpPlayerActor.get());
    pos->y = pos->y + 10.0f;
    groundHeight(pos);
  }
  /* true60: mIdleFrames counts 30 Hz frames: stored on the frame's end step, every step of the frame uses the frame's
   * count n (idleNow: the count as the frame ends, for the latitude branch below) and b is a per-frame ratio */
  s32 idleNow = p->mIdleFrames;
  if (mPlayerOnFloor != 0 || (ownerStatus0(mPadId) & 0x100000)) {
    p->mMovementBlend = 0.0f;
    p->mIdleFrames = 0;
    idleNow = 0;
  } else if (p->mIdleFrames < 0x50) {
    s32 n = p->mIdleFrames + 1;
    if (e2FrameEnd())
      p->mIdleFrames = n;
    idleNow = n;
    f32 old = p->mMovementBlend;
    f32 b = cameraEngineBezierRatio((f32)n / 80.0f,
                                    1.25f); /* dCamMath::rationalBezierRatio */
    p->mMovementBlend = gabi::fmadds(movementBlendLimit - old, e2R(b), old);
  }
  gabi::Local<cSAngle_l> t14, dir, tA0;
  subtractCameraAngles(&towerDirection->mU, t14, &mViewCache.mDirection.mU);
  s16 zero = (s16)l_tornadoZeroAngle;
  bool gt = (s16)*t14 > zero; /* mfcr: CR0[GT] */
  directionOf(dir, mpPlayerActor.get());
  subtractCameraAngles(dir, tA0, &mViewCache.mDirection.mU);
  {
    gabi::Local<cXyz> mul, d, s, a;
    copyCameraWord(&mul->z, &p->mCenterFollowXZ);
    copyCameraWord(&mul->x, &p->mCenterFollowXZ);
    copyCameraWord(&mul->y, &p->mCenterFollowY);
    if (t60::dt() != 1.0f) { /* true60: per-frame ratios (the per-step smoothing rule) */
      mul->x = e2R(mul->x);
      mul->y = e2R(mul->y);
      mul->z = e2R(mul->z);
    }
    cXyz_mi(target, d, &mViewCache.mCenter);
    cameraEngineVectorProduct(d.get(), s.get(), mul.get()); /* cXyz * cXyz */
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
    attentionPos(a, mpPlayerActor.get());
    a->y = a->y - 15.0f;
    constructCameraLine(lineCheck);
    checkCameraWall(this, a, lineCheck);
  }
  gabi::Local<cSGlobe_l> eyeDirection;
  {
    gabi::Local<cXyz> d;
    cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
    cameraEngineConstructVectorGlobe(eyeDirection.get(), d.get());
  }
  f32 t = (mViewCache.mDirection.mRadius - minimumRadius) / range;
  if (t < 0.0f)
    t = 0.0f;
  else if (t > 1.0f)
    t = 1.0f;
  gabi::Local<cSAngle_l> yaw, t16, t1c;
  cSAngle_ct(yaw);
  subtractCameraAngles(yawRangeStart, t16, yawRangeEnd);
  scaleCameraAngle(t16, t14, mStickMainValueLast);
  addCameraAngles(yawRangeStart, t1c, t14);
  {
    gabi::Local<cSAngle_l> ta, tb, tc, td;
    if (!gt) {
      subtractCameraAngles(&eyeDirection->mU, ta, &towerDirection->mU);
      t = cameraEngineBezierRatio(t, yawCurveWeight);
      if ((s16)*ta > (s16)*t1c)
        t = 1.0f;
      addCameraAngles(&towerDirection->mU, tb, t1c);
    } else {
      subtractCameraAngles(&towerDirection->mU, ta, &eyeDirection->mU);
      t = cameraEngineBezierRatio(t, yawCurveWeight);
      if ((s16)*ta > (s16)*t1c)
        t = 1.0f;
      subtractCameraAngles(&towerDirection->mU, tb, t1c);
    }
    subtractCameraAngles(tb, tc, &eyeDirection->mU);
    scaleCameraAngle(tc, td, t);
    addCameraAngles(&eyeDirection->mU, ta, td);
    cameraEngineCopyAngle(yaw.get(), ta.get()); /* Val(const cSAngle&) */
  }
  subtractCameraAngles(yaw, t14, &p->mLongitude);
  scaleCameraAngle(t14, t16, e2R(0.33f)); /* true60: per-frame ratio */
  cameraEngineAngleAddAssign(&p->mLongitude, t16.get()); /* += */
  f32 sv = mStickMainValueLast;
  if ((ownerStatus0(mPadId) & 0x2000161) || (ownerStatus1(mPadId) & 0x10000))
    sv = 0.0f;
  p->mLongitudeRate = gabi::fmadds(movingLongitudeRate, sv, idleLongitudeRate);
  {
    gabi::Local<cSAngle_l> t1e, t16b;
    subtractCameraAngles(&p->mLongitude, t1e, &mViewCache.mDirection.mU);
    scaleCameraAngle(t1e, t14, e2R(p->mLongitudeRate)); /* true60: per-frame ratio */
    addCameraAngles(&mViewCache.mDirection.mU, t16b, t14);
    mViewCache.mDirection.mU = copyCameraAngle(angleCopy, *t16b);
  }
  {
    f32 a = p->mLatitude;
    p->mLatitude = gabi::fmadds(latitude - a, e2R(radiusRate), a); /* true60: per-frame ratio */
  }
  if (!(cameraRideStyle(style)->mFlags & 4))
    mForwardTiltOffset = zero;
  gabi::Local<cSAngle_l> lat;
  cSAngle_ct(lat);
  if ((t60::dt() == 1.0f ? (s32)p->mIdleFrames : idleNow) == 0) {
    f32 deg = cameraAngleDegree(&mForwardTiltOffset);
    cameraEngineSetDegrees(lat.get(), p->mLatitude + deg);
    f32 c = p->mLatitudeRate;
    p->mLatitudeRate = gabi::fmadds(latitudeRate - c, e2R(0.01f), c); /* true60: per-frame ratio */
  } else {
    *lat = (s16)eyeDirection->mV;
    p->mLatitude = cameraAngleDegree(lat);
    p->mLatitudeRate = cameraEngineBezierRatio((f32)p->mMovementBlend, 0.7f);
  }
  if ((s16)*lat < (s16)*latitudeMinimum)
    cameraEngineCopyAngle(lat.get(), latitudeMinimum.get());
  else if ((s16)*lat > (s16)*latitudeMaximum)
    cameraEngineCopyAngle(lat.get(), latitudeMaximum.get());
  {
    gabi::Local<cSAngle_l> t16c, t1ec;
    subtractCameraAngles(lat, t14, &mViewCache.mDirection.mV);
    scaleCameraAngle(t14, t16c, e2R(p->mLatitudeRate)); /* true60: per-frame ratio */
    addCameraAngles(&mViewCache.mDirection.mV, t1ec, t16c);
    mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *t1ec);
  }
  {
    gabi::Local<cXyz> x, e;
    f32 a = p->mMinimumRadius, b = p->mMaximumRadius;
    const f32 radiusStep = e2R(radiusRate); /* true60: per-frame ratios (the per-step smoothing rule) */
    f32 n1C = gabi::fmadds(minimumRadius - a, radiusStep, a);
    f32 n20 = gabi::fmadds(maximumRadius - b, radiusStep, b);
    f32 r3 = eyeDirection->mRadius;
    p->mMinimumRadius = n1C;
    p->mMaximumRadius = n20;
    if (r3 < n1C) {
      f32 c = p->mRadiusRate;
      c = gabi::fmadds(radiusLimitRate - c, e2R(0.01f), c); /* true60: per-frame ratios */
      f32 lim = p->mMinimumRadius;
      p->mRadiusRate = c;
      f32 R = mViewCache.mDirection.mRadius;
      mViewCache.mDirection.mRadius = gabi::fmadds(lim - R, e2R(c), R);
    } else if (r3 > n20) {
      f32 c = p->mRadiusRate;
      c = gabi::fmadds(radiusLimitRate - c, e2R(0.01f), c); /* true60: per-frame ratios */
      f32 lim = p->mMaximumRadius;
      p->mRadiusRate = c;
      f32 R = mViewCache.mDirection.mRadius;
      mViewCache.mDirection.mRadius = gabi::fmadds(lim - R, e2R(c), R);
    } else {
      p->mRadiusRate = 1.0f;
      f32 R = mViewCache.mDirection.mRadius;
      mViewCache.mDirection.mRadius = gabi::fmadds(r3 - R, 1.0f, R);
    }
    cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, e, x);
    mViewCache.mEye.copy(*e);
    f32 c6 = p->mFovyRate;
    c6 = gabi::fmadds(fovyRate - c6, e2R(0.01f), c6); /* true60: per-frame ratios */
    p->mFovyRate = c6;
    f32 fv = mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds(fovy - fv, e2R(c6), fv);
  }
  destroyCameraLine(lineCheck);
  return true;
}

/* side eye of a ride entry from a function-local static offset table
 * (initialised on first use) */
/* inlined into dCamera_c::rideCamera */
static void rideSideEye(dCamera_c *c, RideWorkHD *p, bool sail) {
  auto &guard = sail ? l_rideSailEyeGuard : l_rideHelmEyeGuard;
  RideEyeTableHD &table = sail ? l_rideSailEyeOffsets : l_rideHelmEyeOffsets;
  if (guard == 0) {
    if (!sail)
      guard = 1;
    if (sail) {
      table.mWords[0] = 320.0f;
      table.mWords[1] = 200.0f;
      table.mWords[2] = -150.0f;
      table.mWords[3] = 320.0f;
      table.mWords[4] = 200.0f;
      table.mWords[5] = -120.0f;
      table.mWords[6] = 320.0f;
      table.mWords[7] = 80.0f;
      table.mWords[8] = -150.0f;
      table.mWords[9] = 115.0f;
      table.mWords[10] = 215.0f;
      table.mWords[11] = 315.0f;
    } else {
      table.mWords[0] = -120.0f;
      table.mWords[1] = 50.0f;
      table.mWords[2] = 280.0f;
      table.mWords[3] = 240.0f;
      table.mWords[4] = 160.0f;
      table.mWords[5] = -80.0f;
      table.mWords[6] = 240.0f;
      table.mWords[7] = 120.0f;
      table.mWords[8] = -60.0f;
      table.mWords[9] = 200.0f;
      table.mWords[10] = 160.0f;
      table.mWords[11] = -80.0f;
    }
    if (sail)
      guard = 1;
  }
  gabi::Local<cSGlobe_l> g;
  gabi::Local<cSAngle_l> ta, rel, t, angleCopy;
  gabi::Local<cXyz> pos, pos2, x, e;
  cameraEngineConstructVectorGlobe(
      g.get(), reinterpret_cast<cXyz *>(&table.mWords[((u32)c->mFrameCounter & 3) * 3]));
  cXyz *a = c->positionPntOf(p->mpShip.get());
  cXyz *b = c->positionPntOf(c->mpPlayerActor.get());
  s16 ty = cLib_targetAngleY(b, a);
  cSAngle_ct(ta, ty);
  subtractCameraAngles(ta, rel, &p->mShipHeading);
  if ((s16)*rel < (s16)l_tornadoZeroAngle)
    subtractCameraAngles(&p->mShipHeading, t, &g->mU);
  else
    addCameraAngles(&p->mShipHeading, t, &g->mU);
  g->mU = copyCameraAngle(angleCopy, *t);
  c->positionOf(pos, p->mpShip.get());
  c->mViewCache.mCenter.copy(*pos);
  c->positionOf(pos2, p->mpShip.get());
  cameraEngineGlobeXyz(g.get(), x.get());
  cXyz_pl(pos2, e, x);
  p->mSideEye.copy(*e);
  cXyz *pp = c->positionPntOf(c->mpPlayerActor.get());
  if (!cameraEngineLineBlocked(c, &p->mSideEye, pp, CAMERA_LINE_FLAGS))
    p->mEntryMode = 1; /* lineBGCheck */
}

// HD: GameCube body is undecompiled.
/* Riding the King of Red Lions (status1 4, sail 0x1000000,
 * helm 0x10000): the camera follows the ship's heading with a speed-dependent
 * radius, latitude and fovy (ship speed from the monitor), a side offset while
 * steering, a sea-wave latitude correction, an entry side eye and a boost blur
 * with vibration. */
/* 0250D4E8 */
bool dCamera_c::rideCamera(s32 style) {
  RideWorkHD *p = reinterpret_cast<RideWorkHD *>(mWork);
  /* true60: the mode-start-once rule, the style's start-up once per style start (cached at the first
   * check; dt = 1 reads the timer at each check as the original does: a callee in between may change it) */
  const bool rideStart0 = e2Start(mCurStyleTimer, 4);
#define rideStart (t60::dt() == 1.0f ? mCurStyleTimer == 0 : rideStart0)
  f32 centerHeightSpeedScale = camParamVal(style, 8);
  f32 targetLatitude = camParamVal(style, 15);
  f32 targetRadius = camParamVal(style, 10);
  f32 radiusSpeedScale = camParamVal(style, 13);
  f32 centerOffsetY = camParamVal(style, 5);
  f32 centerFollowXZ = camParamVal(style, 3);
  f32 centerFollowY = camParamVal(style, 4);
  f32 latitudeSpeedScale = camParamVal(style, 18);
  f32 centerOffsetX = camParamVal(style, 1);
  f32 speedMaximum = camParamVal(style, 14);
  f32 centerOffsetZ = camParamVal(style, 0);
  gabi::Local<cSAngle_l> latitudeMinimum, latitudeMaximum, bankMax, angleCopy;
  cameraEngineConstructDegrees(latitudeMinimum.get(), camParamVal(style, 16));
  cameraEngineConstructDegrees(latitudeMaximum.get(), camParamVal(style, 17));
  f32 fovySpeedScale = camParamVal(style, 28);
  f32 targetFovy = camParamVal(style, 25);
  f32 followRate = camParamVal(style, 21);
  f32 longitudeRate = camParamVal(style, 20);
  cameraEngineConstructDegrees(bankMax.get(), camParamVal(style, 29));
  f32 slowCenterHeight = camParamVal(style, 6);
  f32 fastCenterHeight = camParamVal(style, 7);
  if (mEventFlags & CAMERA_FORCE_FOLLOW)
    longitudeRate = 1.0f;
  p->mpShip = nullptr;
  gabi::Local<cXyz> centerOffset;
  centerOffset->z = centerOffsetZ;
  centerOffset->x = 0.0f;
  centerOffset->y = centerOffsetY;
  if (rideStart)
    p->mSteeringLeft = 0;
  if (ownerStatus1(mPadId) & 4) {
    gabi::Local<be<s16>> nm;
    *nm = RIDE_SHIP_PROCESS;
    p->mpShip = fopAcIt_Judge(
        RIDE_JUDGE_FOR_PROCESS_NAME, nm.get());
    if (p->mpShip.get() == nullptr)
      JUTAssertion::showAssert("d_camera.cpp", 0x1EE2,
                               "wk->ship != (0)"); /* JUTAssertion (debug) */
    RideShipHD *ship = reinterpret_cast<RideShipHD *>(p->mpShip.get());
    gabi::Local<cSAngle_l> hd, d1a, c, t48;
    cSAngle_ct(hd, (s16)(ship->shape_angle.y + ship->mHeadingOffset));
    cSAngle_ct(d1a);
    if (rideStart) {
      *d1a = (s16)l_tornadoZeroAngle;
    } else {
      subtractCameraAngles(&p->mShipHeading, t48, hd);
      cameraEngineCopyAngle(d1a.get(), t48.get());
    }
    /* true60: the heading change since the last call against +-1 degree per 30 Hz frame: +-dt degrees per step */
    const f32 turnDeg = t60::dt() == 1.0f ? 1.0f : t60::dt();
    cSAngle_l *cp = cameraEngineConstructDegrees(c.get(), -turnDeg);
    if ((s16)*d1a < (s16)*cp)
      p->mSteeringLeft = 1;
    cp = cameraEngineConstructDegrees(c.get(), turnDeg);
    if ((s16)*d1a > (s16)*cp)
      p->mSteeringLeft = 0;
    p->mShipHeading = (s16)*hd;
  } else if ((ownerStatus0(mPadId) & 0x1010000) ||
             (ownerStatus1(mPadId) & 0x80) || (ownerStatus1(mPadId) & 2)) {
    gabi::Local<be<s16>> nm;
    gabi::Local<cSAngle_l> dir;
    *nm = RIDE_SHIP_PROCESS;
    p->mpShip = fopAcIt_Judge(
        RIDE_JUDGE_FOR_PROCESS_NAME, nm.get());
    directionOf(dir, mpPlayerActor.get());
    cameraEngineCopyAngle(&p->mShipHeading, dir.get());
  } else {
    gabi::Local<cSAngle_l> dir;
    directionOf(dir, mpPlayerActor.get());
    cameraEngineCopyAngle(&p->mShipHeading, dir.get());
    p->mpShip = nullptr;
  }
  if (rideStart) {
    p->mTag = RIDE_WORK;                                 /* 'RIDE' */
    p->mStickCenterOffset.copy(*cameraRideZeroVector()); /* cXyz::Zero */
    p->mEntryMode = 0;
    p->mBoostBlurArmed = 1;
    p->mEntryViewIndex = (u32)mFrameCounter;
    p->mSideOffsetRatio = 0.0f;
    copyCameraWord(&p->mRadius, &mViewCache.mDirection.mRadius);
    p->mRadiusSpeedScale = 0.0f;
    f32 deg = cameraAngleDegree(&mViewCache.mDirection.mV);
    p->mpShip = nullptr;
    p->mLatitude = deg;
    p->mLatitudeSpeedScale = 0.0f;
    p->mCenterFollowRatio = 0.2f;
    bool done = false;
    if (ownerStatus0(mPadId) & 0x1000000) {
      gabi::Local<be<s16>> nm;
      *nm = RIDE_SHIP_PROCESS;
      p->mpShip = fopAcIt_Judge(
          RIDE_JUDGE_FOR_PROCESS_NAME, nm.get());
      p->mCenterTarget.copy(mViewCache.mCenter);
      rideSideEye(this, p, false);
      done = true;
    } else if (ownerStatus1(mPadId) & 0x80) {
      gabi::Local<be<s16>> nm;
      *nm = RIDE_SHIP_PROCESS;
      p->mpShip = fopAcIt_Judge(
          RIDE_JUDGE_FOR_PROCESS_NAME, nm.get());
      p->mCenterTarget.copy(mViewCache.mCenter);
      rideSideEye(this, p, true);
      done = true;
    }
    if (!done) {
      if ((ownerStatus0(mPadId) & 0x1010000) || (ownerStatus1(mPadId) & 2)) {
        gabi::Local<be<s16>> nm;
        p->mCenterTarget.copy(mViewCache.mCenter);
        *nm = RIDE_SHIP_PROCESS;
        p->mpShip = fopAcIt_Judge(
            RIDE_JUDGE_FOR_PROCESS_NAME, nm.get());
      } else {
        p->mCenterTarget.copy(mViewCache.mCenter);
        p->mpShip = nullptr;
      }
      p->mSideEye.copy(mViewCache.mCenter);
    }
    gabi::Local<cXyz> d;
    cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
    cSGlobe_Val(&mViewCache.mDirection, d);
    if (mEventFlags & CAMERA_FAST_TRANSITION)
      p->mEntryMode = 2;
    if (mEventFlags & CAMERA_FORCE_FOLLOW) {
      mStyleSettle2 = 1;
      mStyleSettle1 = 1;
      mStyleSettle0 = 1;
    }
  }
  f32 speedRatio;
  if (ownerStatus0(mPadId) & 0x1010000) {
    fopAc_ac_c *ship = p->mpShip.get();
    speedRatio = mMonitor.field_0x0C.y / speedMaximum;
    if (ship != nullptr &&
        (reinterpret_cast<RideShipHD *>(ship)->mStateFlags & 0x80000000) &&
        cameraRidePlay()->mBoostStatus == 1 && mMonitor.field_0x0C.z > 1.9f &&
        (ownerStatus1(mPadId) & 0x400) && !(ownerStatus0(mPadId) & 0x1000000) &&
        !(ownerStatus1(mPadId) & 0x80)) {
      if (p->mBoostBlurArmed != 0) {
        ResetBlure(0);
        SetBlureTimer(0x14);
        SetBlureAlpha(0.55f);
        cameraRideSetBlurScale(this, 0.95f); /* SetBlureScale */
        gabi::Local<cXyz> v;
        auto *vibration = dComIfGp_getVibration();
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        vibration->StartShock(7, 0x20, v.get()); /* dVibration_c::StartShock */
        p->mBoostBlurArmed = 0;
      }
    } else if (cameraRidePlay()->mBoostStatus == 0) {
      p->mBoostBlurArmed = 1;
    }
  } else {
    p->mpShip = nullptr;
    speedRatio = mMonitor.field_0x0C.y / speedMaximum;
    if (cameraRidePlay()->mBoostStatus == 0)
      p->mBoostBlurArmed = 1;
  }
  if (speedRatio < -1.0f)
    speedRatio = -1.0f;
  else if (speedRatio > 1.0f)
    speedRatio = 1.0f;
  f32 radius, lat, fovy;
  {
    f32 a3C = p->mRadius, a40 = p->mRadiusSpeedScale, a44 = p->mLatitude,
        a48 = p->mLatitudeSpeedScale;
    const f32 q = e2R(0.08f); /* true60: per-frame ratios (the per-step smoothing rule) */
    {
      /* true60: the eased fields' frame start (d_camera_ride_t60.h) */
      const f32 f0[camera_ride_t60::N_FIELDS] = {a3C, a40, a44, a48, p->mSideOffsetRatio, p->mCenterFollowRatio};
      camera_ride_t60::begin(f0);
    }
    using camera_ride_t60::ease;
    f32 n3C = ease(camera_ride_t60::RADIUS, a3C, targetRadius, 0.08f, q);
    fovy = gabi::fmadds(fovySpeedScale, speedRatio, targetFovy);
    f32 n40 = ease(camera_ride_t60::RADIUS_SPEED, a40, radiusSpeedScale, 0.08f, q);
    f32 n48 = ease(camera_ride_t60::LATITUDE_SPEED, a48, latitudeSpeedScale, 0.08f, q);
    f32 n44 = ease(camera_ride_t60::LATITUDE, a44, targetLatitude, 0.08f, q);
    p->mRadius = n3C;
    p->mRadiusSpeedScale = n40;
    p->mLatitude = n44;
    radius = gabi::fmadds(n40 * speedRatio, speedRatio, n3C);
    p->mLatitudeSpeedScale = n48;
    lat = gabi::fmadds(n48 * speedRatio, speedRatio, n44);
    if (fovy > 80.0f)
      fovy = 80.0f;
  }
  gabi::Local<cSGlobe_l> g;
  gabi::Local<cSAngle_l> side;
  cameraEngineDefaultGlobe(g.get());
  cSAngle_ct(side);
  if (p->mpShip.get() != nullptr) {
    u32 RIDE_STYLE_BE07 = 0x42453037; /* 'RIDE_STYLE_BE07' */
    bool s4 = false;
    if (ownerStatus1(mPadId) & 2) {
      cSAngle_Val(side,
                  reinterpret_cast<RideShipHD *>(p->mpShip.get())->mRideAngle);
      f32 c = p->mSideOffsetRatio;
      p->mSideOffsetRatio = camera_ride_t60::ease(camera_ride_t60::SIDE, c, 1.0f, 0.05f, e2R(0.05f)); /* true60: per-frame ratio */
      f32 s = cameraEngineAngleSin(side.get()); /* Sin */
      centerOffset->y =
          gabi::fmadds(centerHeightSpeedScale, speedRatio, centerOffset->y);
      centerOffset->x = gabi::fnmsubs(centerOffsetX * s, p->mSideOffsetRatio,
                                      centerOffset->x);
      s4 = (ownerStatus1(mPadId) & 4) != 0;
    } else {
      bool st4 = (ownerStatus1(mPadId) & 4) != 0;
      RideShipHD *ship = reinterpret_cast<RideShipHD *>(p->mpShip.get());
      if (st4) {
        gabi::Local<cSAngle_l> dir, t68, t66;
        directionOf(dir, p->mpShip.get());
        RideShipHD *sh = reinterpret_cast<RideShipHD *>(p->mpShip.get());
        cameraEngineAngleSubtractValue(
            dir.get(), t68.get(),
            (s16)(sh->shape_angle.y + sh->mHeadingOffset)); /* - s16 */
        addCameraAngles(t68, t66, cameraRideRightAngle());  /* + _90 */
        cameraEngineCopyAngle(side.get(), t66.get());
        f32 c = p->mSideOffsetRatio;
        if (p->mSteeringLeft != 0) /* true60: per-frame ratios */
          p->mSideOffsetRatio = camera_ride_t60::ease(camera_ride_t60::SIDE, c, -1.0f, 0.05f, e2R(0.05f));
        else
          p->mSideOffsetRatio = camera_ride_t60::ease(camera_ride_t60::SIDE, c, 1.0f, 0.05f, e2R(0.05f));
        if (cameraRideStyle((s32)mCurStyle)->mName == RIDE_STYLE_BE07) {
          gabi::Local<cSAngle_l> pa;
          RideShipHD *sh2 = reinterpret_cast<RideShipHD *>(p->mpShip.get());
          cSAngle_l *ap = cameraEngineConstructAngle(
              pa.get(), (s16)(sh2->shape_angle.x + sh2->mPitchOffset - 0x4000));
          f32 dg = cameraEngineAngleDegree(ap);                    /* Degree */
          speedRatio = cameraEngineLimit(-dg / 60.0f, 0.0f, 1.0f); /* limitf */
        }
      } else {
        cSAngle_Val(side, ship->mSideAngle);
        f32 c = p->mSideOffsetRatio;
        p->mSideOffsetRatio = camera_ride_t60::ease(camera_ride_t60::SIDE, c, speedRatio, 0.1f, e2R(0.1f)); /* true60: per-frame ratio */
      }
      f32 s = cameraEngineAngleSin(side.get()); /* Sin */
      centerOffset->x = gabi::fnmsubs(centerOffsetX * s, p->mSideOffsetRatio,
                                      centerOffset->x);
      centerOffset->y =
          gabi::fmadds(centerHeightSpeedScale, speedRatio, centerOffset->y);
      s4 = (ownerStatus1(mPadId) & 4) != 0;
    }
    if (s4) {
      u32 alg = cameraRideStyle((s32)mCurStyle)->mName;
      if (alg == RIDE_STYLE_BE07)
        centerOffset->y = gabi::fmadds(fastCenterHeight - slowCenterHeight,
                                       speedRatio, slowCenterHeight);
      else if (alg == RIDE_STYLE_BE08)
        centerOffset->y = centerOffset->y + centerOffset->y; /* 'BE08' */
    }
    if (mInSeaArea != 0 && mSeaWaveHeight == mGroundHeight && (cameraRideStyle(style)->mFlags & 4)) {
      gabi::Local<cSGlobe_l> g2;
      gabi::Local<cXyz> pos, x, sp;
      cameraEngineConstructGlobe(
          g2.get(), 200.0f, reinterpret_cast<cSAngle_l *>(&l_tornadoZeroAngle),
          &p->mShipHeading);
      positionOf(pos, p->mpShip.get());
      cameraEngineGlobeXyz(g2.get(), x.get());
      cXyz_pl(pos, sp, x);
      if (daSea_ChkArea(sp->x, sp->z)) {
        gabi::Local<cXyz> pos2, d;
        sp->y = daSea_calcWave(sp->x, sp->z);
        positionOf(pos2, p->mpShip.get());
        cXyz_mi(sp, d, pos2);
        cSGlobe_Val(g2, d);
        f32 dg = cameraAngleDegree(&g2->mV);
        lat = gabi::fmadds(dg * speedRatio, 0.8f, lat);
      }
    }
  }
  {
    s16 inv = cSAngle_Inv(&p->mShipHeading);
    cameraEngineSetGlobeAngles(g.get(), radius,
                               (s16)gabi::ftoi(lat * 182.04445f),
                               inv); /* Val(f32, s16, s16) */
    f32 c = p->mCenterFollowRatio;
    p->mCenterFollowRatio = camera_ride_t60::ease(camera_ride_t60::CENTER_FOLLOW, c, 1.0f, 0.2f, e2R(0.2f)); /* true60: per-frame ratios */
    {
      const f32 f1[camera_ride_t60::N_FIELDS] = {p->mRadius, p->mRadiusSpeedScale, p->mLatitude, p->mLatitudeSpeedScale,
                                                 p->mSideOffsetRatio, p->mCenterFollowRatio};
      camera_ride_t60::leave(f1);
    }
    gabi::Local<cXyz> r, d, s;
    relationalPos(r, mpPlayerActor.get(), centerOffset);
    cXyz_mi(r, d, &p->mCenterTarget);
    cXyz_ml(d, s, e2R(p->mCenterFollowRatio));
    PSVECAdd(&p->mCenterTarget, s, &p->mCenterTarget);
  }
  if (mStyleSettle0 == 0) {
    gabi::Local<cXyz> d, s, x, e;
    gabi::Local<cSAngle_l> t22, t20, t1e;
    u32 mode = (u32)p->mEntryMode;
    f32 k;
    s32 end;
    if (mode == 1) {
      k = e2R(1.0f / (f32)(s32)(40 - (s32)mCurStyleTimer)); /* true60: the frame's share per step */
      cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
      cXyz_ml(d, s, k);
      PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
      mViewCache.mEye.copy(p->mSideEye);
      cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
      cSGlobe_Val(&mViewCache.mDirection, d);
      f32 fv = mViewCache.mFovy;
      mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
      if (mCurStyleTimer != 39 || !e2FrameEnd()) /* true60: settle on the frame's end */
        return true;
    } else if (mode == 2) {
      end = 1;
      k = 0.0f;
      f32 fv = mViewCache.mFovy;
      mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
      if ((u32)mCurStyleTimer != (u32)(end - 1) || !e2FrameEnd()) /* true60: settle on the frame's end */
        return true;
    } else {
      end = 20;
      k = e2R(1.0f / (f32)(s32)(20 - (s32)mCurStyleTimer)); /* true60: the frame's share per step */
      cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
      cXyz_ml(d, s, k);
      PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
      f32 rr = mViewCache.mDirection.mRadius;
      mViewCache.mDirection.mRadius = gabi::fmadds(g->mRadius - rr, k, rr);
      subtractCameraAngles(&g->mV, t22, &mViewCache.mDirection.mV);
      scaleCameraAngle(t22, t20, k);
      addCameraAngles(&mViewCache.mDirection.mV, t1e, t20);
      mViewCache.mDirection.mV = copyCameraAngle(angleCopy, *t1e);
      subtractCameraAngles(&g->mU, t1e, &mViewCache.mDirection.mU);
      scaleCameraAngle(t1e, t20, k);
      addCameraAngles(&mViewCache.mDirection.mU, t22, t20);
      mViewCache.mDirection.mU = copyCameraAngle(angleCopy, *t22);
      cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
      cXyz_pl(&mViewCache.mCenter, e, x);
      f32 fv = mViewCache.mFovy;
      mViewCache.mEye.copy(*e);
      mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
      if ((u32)mCurStyleTimer != (u32)(end - 1) || !e2FrameEnd()) /* true60: settle on the frame's end */
        return true;
    }
    mStyleSettle2 = 1;
    mStyleSettle1 = 1;
    mStyleSettle0 = 1;
    return true;
  }
  {
    gabi::Local<cXyz> mul, d, s;
    mul->x = e2R(centerFollowXZ); /* true60: per-frame ratios */
    mul->y = e2R(centerFollowY);
    mul->z = e2R(centerFollowXZ);
    cXyz_mi(&p->mCenterTarget, d, &mViewCache.mCenter);
    cameraEngineVectorProduct(d.get(), s.get(), mul.get()); /* cXyz * cXyz */
    PSVECAdd(&mViewCache.mCenter, s, &mViewCache.mCenter);
  }
  f32 rr0 = mViewCache.mDirection.mRadius;
  const f32 followStep = e2R(followRate); /* true60: per-frame ratio */
  f32 R = gabi::fmadds(g->mRadius - rr0, followStep, rr0);
  gabi::Local<cSAngle_l> dir, t40, t12, t10, u42, vNew, t14;
  directionOf(dir, mpPlayerActor.get());
  {
    s16 inv = cSAngle_Inv(dir);
    cameraEngineConstructDifference(t40.get(), inv, &mViewCache.mDirection.mU);
  }
  if (!(mEventFlags & CAMERA_FORCE_FOLLOW))
    cameraEngineAngleSin(t40.get()); /* Sin (unused) */
  subtractCameraAngles(&g->mU, t12, &mViewCache.mDirection.mU);
  scaleCameraAngle(t12, t10, e2R(longitudeRate));
  addCameraAngles(&mViewCache.mDirection.mU, u42, t10);
  gabi::Local<cSGlobe_l> eyeDirection;
  {
    gabi::Local<cXyz> d;
    cXyz_mi(&mViewCache.mEye, d, &mViewCache.mCenter);
    cameraEngineConstructVectorGlobe(eyeDirection.get(), d.get());
  }
  f32 blend = 1.0f;
  if (mInSeaArea != 0 && mSeaWaveHeight == mGroundHeight && (cameraRideStyle(style)->mFlags & 4)) {
    if (speedRatio > 0.5f)
      blend = 0.0f;
    else {
      f32 h = 0.5f - speedRatio;
      blend = h + h;
    }
  }
  gabi::Local<cSAngle_l> lat18;
  cSAngle_ct(lat18);
  subtractCameraAngles(&eyeDirection->mV, t14, &g->mV);
  scaleCameraAngle(t14, t10, 0.25f * blend);
  addCameraAngles(&g->mV, t12, t10);
  g->mV = copyCameraAngle(angleCopy, *t12);
  if (p->mpShip.get() != nullptr && (ownerStatus1(mPadId) & 4)) {
    gabi::Local<cSAngle_l> t6c;
    RideShipHD *sh = reinterpret_cast<RideShipHD *>(p->mpShip.get());
    cameraEngineAngleAddValue(
        &g->mV, t6c.get(),
        (s16)(sh->shape_angle.x + sh->mPitchOffset - 0x4000)); /* + s16 */
    cameraEngineCopyAngle(lat18.get(), t6c.get());
    cameraRideScaleAngleAssign(lat18.get(), 0.3f); /* *= */
  } else {
    *lat18 = (s16)g->mV;
  }
  /* true60: the latitude target leans toward the eye's own latitude (x 0.25 blend, x 0.3 on the ship): with
   * T = A + B x current the frame's pull is (1 - B) x followRate; per step e2R((1 - B) x followRate) / (1 - B) */
  f32 latStep = followRate;
  if (t60::dt() != 1.0f) {
    const f32 B = 0.25f * blend * ((p->mpShip.get() != nullptr && (ownerStatus1(mPadId) & 4)) ? 0.3f : 1.0f);
    latStep = (1.0f - B) > 0.0f ? e2R((1.0f - B) * followRate) / (1.0f - B) : followStep;
  }
  subtractCameraAngles(lat18, t12, &mViewCache.mDirection.mV);
  scaleCameraAngle(t12, t14, latStep);
  addCameraAngles(&mViewCache.mDirection.mV, vNew, t14);
  if ((s16)*vNew < (s16)*latitudeMinimum)
    cameraEngineCopyAngle(vNew.get(), latitudeMinimum.get());
  else if ((s16)*vNew > (s16)*latitudeMaximum)
    cameraEngineCopyAngle(vNew.get(), latitudeMaximum.get());
  cameraEngineSetGlobe(&mViewCache.mDirection, R, vNew.get(),
                       u42.get()); /* Val(f32, V, U) */
  {
    gabi::Local<cXyz> x, e;
    cameraEngineGlobeXyz(&mViewCache.mDirection, x.get());
    cXyz_pl(&mViewCache.mCenter, e, x);
    f32 fv = mViewCache.mFovy;
    mViewCache.mEye.copy(*e);
    mViewCache.mFovy = gabi::fmadds(fovy - fv, followStep, fv);
  }
  scaleCameraAngle(bankMax, t10, mStickMainPosXLast);
  subtractCameraAngles(t10, t12, &mViewCache.mBank);
  scaleCameraAngle(t12, t14, e2R(0.01f * __builtin_fabsf(speedRatio)));
  cameraEngineAngleAddAssign(&mViewCache.mBank, t14.get()); /* += */
  mEventFlags = mEventFlags | CAMERA_UPDATE_VIEW;
  return true;
#undef rideStart
}
/* WWHD glue
ext 0200658C cSAngle_l* cameraEngineConstructAngle(cSAngle_l* angle, s16 value)
ext 020065EC void cameraEngineConstructDifference(cSAngle_l* result, s16 angle, const cSAngle_l* other)
ext 020066C0 cSAngle_l* cameraEngineConstructDegrees(cSAngle_l* angle, f32 degrees)
ext 02006694 void cameraEngineSetDegrees(cSAngle_l* angle, f32 degrees)
ext 02006720 f32 cameraEngineAngleDegree(const void* angle)
ext 020067AC f32 cameraEngineAngleNorm(const cSAngle_l* angle)
ext 02006838 f32 cameraEngineAngleCos(const cSAngle_l* angle)
ext 02006894 void cameraEngineAngleAdd(const void* angle, cSAngle_l* result, const void* other)
ext 020068B0 void cameraEngineAngleSubtract(const void* angle, cSAngle_l* result, const void* other)
ext 0200693C void cameraEngineAngleScale(const void* angle, cSAngle_l* result, f32 scale)
ext 020068F4 void cameraEngineAngleAddValue(const cSAngle_l* angle, cSAngle_l* result, s16 value)
ext 02006908 void cameraEngineAngleSubtractValue(const cSAngle_l* angle, cSAngle_l* result, s16 value)
ext 020068CC void cameraEngineAngleAddAssign(cSAngle_l* angle, const cSAngle_l* other)
ext 020068E0 void cameraEngineAngleSubtractAssign(cSAngle_l* angle, const cSAngle_l* other)
ext 02007248 void cameraEngineConstructGlobe(cSGlobe_l* globe, f32 radius, const cSAngle_l* latitude, const cSAngle_l* longitude)
ext 02007324 void cameraEngineConstructVectorGlobe(cSGlobe_l* globe, const cXyz* vector)
ext 020073AC void cameraEngineGlobeXyz(const cSGlobe_l* globe, cXyz* result)
ext 0201AF1C void cameraEngineVectorProduct(const cXyz* vector, cXyz* result, const cXyz* scale)
ext 024F7AEC void cameraEngineProjectYZ(cXyz* result, const cSAngle_l* angle, const cXyz* position, const cXyz* anchor)
ext 024F7AC4 f32 cameraEngineHorizontalDistance(const cXyz* first, const cXyz* second)
ext 0201971C f32 cameraEngineAtan2(f32 y, f32 x)
ext 0200F93C s16 cameraEngineTargetAngleY(const cXyz* first, const cXyz* second)
ext 024F6F9C f32 cameraEngineBezierRatio(f32 ratio, f32 weight)
ext 025028A0 f32 cameraEngineLimit(f32 value, f32 minimum, f32 maximum)
ext 02008FEC void cameraEngineConstructLine(camera_engine2::CameraLineCheckHD* check)
ext 02008B4C void cameraEngineDestroyLine(camera_engine2::CameraLineCheckHD* check, s32 flags)
ext 020084C8 camera_engine2::PlaneNormalHD* cameraEngineGetTriPlane(dBgS* background, u32 backgroundIndex, u32 polygonIndex)
ext 02006638 void cameraEngineCopyAngle(cSAngle_l* angle, const cSAngle_l* other)
ext 02006814 f32 cameraEngineAngleSin(const cSAngle_l* angle)
ext 02007100 void cameraEngineDefaultGlobe(cSGlobe_l* globe)
ext 02006FE4 void cameraEngineSetGlobeAngles(cSGlobe_l* globe, f32 radius, s16 latitude, s16 longitude)
ext 020071E0 void cameraEngineSetGlobe(cSGlobe_l* globe, f32 radius, const cSAngle_l* latitude, const cSAngle_l* longitude)
ext 0200F5C8 void cameraEngineChaseFloat(be<f32>* value, f32 target, f32 step)
ext 024F7DF4 bool cameraEngineRangeAdd(void* value, f32 add, f32 minimum, f32 maximum)
data l_tornadoSideEyeGuard 1046EFC0
data l_tornadoShipEyeGuard 1046EFC4
data l_tornadoSideEyeOffsets 1046EED8
data l_tornadoShipEyeOffsets 1046EEF0
ext 024F78F0 f32 cameraEngineFanBank(const void* setup)
ext 02508804 void cameraEngineRelationalPosition(dCamera_c* camera, cXyz* result, fopAc_ac_c* player, fopAc_ac_c* target, cXyz* offset, f32 scale)
ext 027200D0 camera_engine2::CameraOptionViewHD* cameraEngineGetOptions(u32 saveOptions)
data l_manualMakarFlying 101BDC2D
data l_manualMedliFlying 101D5F3E
data l_tornadoZeroAngle 101FF354
data l_ropeRadians 1046EED0
ext 020069A0 void cameraRideScaleAngleAssign(cSAngle_l* angle, f32 scale)
ext 024EFB08 void cameraTowerMoveBackground(dBgS* background, cBgS_PolyInfo* check, u32 mode, cXyz* position, u32 angle, u32 shape)
ext 0250D4D0 void cameraRideSetBlurScale(dCamera_c* camera, f32 scale)
data l_rideHelmEyeGuard 1046EFB8
data l_rideSailEyeGuard 1046EFBC
data l_rideHelmEyeOffsets 1046EF08
data l_rideSailEyeOffsets 1046EF38
str 1004AD80 "d_camera.cpp" in 0250D4E8
str 1004AD70 "wk->ship != (0)" in 0250D4E8
*/
