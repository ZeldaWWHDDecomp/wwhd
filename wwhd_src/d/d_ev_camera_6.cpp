/** Ballad of Gales and two-actor event cameras, verified against WWHD.
 * GameCube provides method names and camera fields but no matching function bodies.
 * HD: explicit result arguments and partial work views preserve the verified ABI. */
#include "d/d_ev_camera_6.h"
using namespace event_camera_pair;
// HD: keep this partial camera view separate from other event-camera translation units.
namespace event_camera_pair {
static void copyPoint(cXyz* destination, const cXyz* source) {
    // HD: copy words in order to preserve signaling NaN payloads.
    auto* out = reinterpret_cast<CameraPointWordsHD*>(destination);
    auto* in = reinterpret_cast<const CameraPointWordsHD*>(source);
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}
static void copyFloatPoint(cXyz* destination, const cXyz* source) {
    // HD: these stack copies use floating loads and stores, unlike copyPoint.
    destination->x = (f32)source->x;
    destination->y = (f32)source->y;
    destination->z = (f32)source->z;
}
static void setPoint(cXyz* point, f32 x, f32 y, f32 z) {
    point->x = x;
    point->y = y;
    point->z = z;
}
static void SkipSmoother(dCamera_c* camera) {
    camera->m102 = 1;
    camera->m101 = 1;
    camera->m100 = 1;
}
}
/* 025383D0 */
bool event_camera_pair::dCamera_c::tornadoWarpEvCamera() {
    auto& work = mWork.tornadoWarp;
    if (m11C == 0) {
        work.mTimer = 0;
        work.mState = TORNADO_CHOOSE_EYE;
        SkipSmoother(this);
    }
    f32 zero = 0.0f, eyeHeight = 800.0f, eyeRadius = 900.0f, negativeEyeRadius = -900.0f;
    gabi::Local<cXyz[4]> eyeOffsets;
    gabi::Local<cXyz> centerOffset;
    gabi::Local<cXyz> endOffset;
    setPoint(&(*eyeOffsets.get())[0], eyeRadius, eyeHeight, zero);
    setPoint(&(*eyeOffsets.get())[1], negativeEyeRadius, eyeHeight, zero);
    setPoint(&(*eyeOffsets.get())[2], zero, eyeHeight, eyeRadius);
    setPoint(&(*eyeOffsets.get())[3], zero, eyeHeight, negativeEyeRadius);
    setPoint(centerOffset.get(), zero, -40.0f, zero);
    setPoint(endOffset.get(), zero, 60.0f, zero);
    gabi::Local<be<s16>> angle45;
    cameraPairAngleFromDegrees(angle45.get(), 45.0f); /* cSAngle(45.0) */
    u32 state = work.mState;
    f32 quarter = 0.25f;
    if (state == TORNADO_DONE) return true;
    gabi::Local<cXyz> eye;
    gabi::Local<cXyz> difference;
    gabi::Local<cXyz> scaled;
    if (state != TORNADO_TURN && state != TORNADO_EASE_OUT) {
        gabi::Local<CameraProcessNameHD> name;
        // HD: preserve the process-name key alignment and the eight-byte stack slot.
        name->mName = fpcNm_SHIP_e;
        // HD: external callbacks retain named guest targets until autoglue registers ext functions.
        work.mpShipActor = fopAcIt_Judge(JUDGE_FOR_PROCESS_NAME, &name->mName);
        work.mTimer = 100;
        work.mState = TORNADO_TURN;
        gabi::Local<cXyz> farPoint;
        gabi::Local<cXyz> center;
        fopAc_ac_c* pl = mpPlayerActor;
        setPoint(farPoint.get(), -180000.0f, 750.0f, -200000.0f); /* (-180000, 750, -200000) */
        cameraPairRelational(this, difference.get(), pl, centerOffset.get());
        copyPoint(center.get(), difference.get());
        if (m786 != 0) {
            /* the table entry whose eye is closest to (-180000, 750, -200000) */
            s32 best = 3;
            f32 bestDistance = 100000000.0f; /* 1e8 */
            gabi::Local<be<s16>> angleCopy;
            for (s32 k = 0; k < 4; k++) {
                be<s16>* ap = cameraPairAngleCopy(angleCopy.get(), angle45.get());
                cameraPairRelationalWithAngle(this, difference.get(), mpPlayerActor, &(*eyeOffsets.get())[k], ap);
                copyPoint(eye.get(), difference.get());
                cameraPairVectorSubtract(eye.get(), difference.get(), farPoint.get());
                f32 distance = cameraPairVectorLength(difference.get());
                if (distance < bestDistance) { /* bge: taken on NaN */
                    bestDistance = distance;
                    best = k;
                }
            }
            gabi::Local<be<s16>> chosenAngle;
            gabi::Local<cXyz> rot;
            be<s16>* ap = cameraPairAngleCopy(chosenAngle.get(), angle45.get());
            cameraPairRotateY(rot.get(), &(*eyeOffsets.get())[best], ap); /* dCamMath::xyzRotateY */
            cameraPairRelational(this, scaled.get(), mpPlayerActor, rot.get());
            f32 x = (f32)scaled->x, y = (f32)scaled->y, z = (f32)scaled->z;
            setPoint(eye.get(), x, y, z);
            work.mEye.y = y;
            work.mEye.z = z;
            work.mEye.x = x;
        } else {
            gabi::Local<be<s16>> angleCopy;
            gabi::Local<cXyz> p1;
            gabi::Local<cXyz> p2;
            f32 collisionRadius = 15.0f;
            for (u32 k = 0; k < 4; k++) {
                be<s16>* ap = cameraPairAngleCopy(angleCopy.get(), angle45.get());
                cameraPairRelationalWithAngle(this, difference.get(), mpPlayerActor, &(*eyeOffsets.get())[k], ap);
                copyPoint(eye.get(), difference.get());
                if (cameraPairLineCheck(this, center.get(), eye.get(), CAMERA_LINE_DEFAULT)) continue;
                copyFloatPoint(p1.get(), center.get());
                copyFloatPoint(p2.get(), eye.get());
                fopAc_ac_c* pl2 = mpPlayerActor;
                fopAc_ac_c* ship = work.mpShipActor;
                auto* collision = reinterpret_cast<dCcS*>(dComIfG_Ccsp());
                if (!cameraPairCheckCamera(collision, p1.get(), p2.get(), collisionRadius, pl2, ship)) break; /* dCcS::ChkCamera */
            }
            f32 z = (f32)eye->z, x = (f32)eye->x, y = (f32)eye->y;
            work.mEye.x = x;
            work.mEye.y = y;
            work.mEye.z = z;
        }
        state = TORNADO_TURN;
    }
    if (state == TORNADO_TURN) {
        cameraPairRelational(this, eye.get(), mpPlayerActor, centerOffset.get());
        cameraPairVectorSubtract(eye.get(), difference.get(), &mViewCache.mCenter);
        cameraPairVectorScale(difference.get(), scaled.get(), quarter);
        PSVECAdd(&mViewCache.mCenter, scaled.get(), &mViewCache.mCenter);
        f32 inv = 1.0f / (f32)(f64)(s32)work.mTimer;
        cameraPairVectorSubtract(&work.mEye, scaled.get(), &mViewCache.mEye);
        cameraPairVectorScale(scaled.get(), difference.get(), inv);
        cameraPairVectorAdd(&mViewCache.mEye, eye.get(), difference.get());
        gabi::Local<cXyz> tgt;
        copyPoint(tgt.get(), eye.get());
        cameraPairVectorSubtract(tgt.get(), difference.get(), &mViewCache.mEye);
        f32 eyeCushion = 0.15f;
        cameraPairVectorScale(difference.get(), eye.get(), eyeCushion);
        PSVECAdd(&mViewCache.mEye, eye.get(), &mViewCache.mEye);
        f32 fv = (f32)mViewCache.mFovy;
        f32 f1 = gabi::fmadds(70.0f - fv, inv, fv); /* towards 70 */
        mViewCache.mFovy = gabi::fmadds(f1 - fv, eyeCushion, fv);
        cameraPairVectorSubtract(&mViewCache.mEye, eye.get(), &mViewCache.mCenter);
        cameraPairGlobeVector(&mViewCache.mDirection, eye.get());
        f32 rnd = cM_rndFX(6.0f * inv); /* cM_rndFX(6 * inv) */
        gabi::Local<be<s16>> bankDifference;
        gabi::Local<be<s16>> bankStep;
        cameraPairScalarMinusAngle(bankDifference.get(), (s32)(s16)gabi::ftoi(rnd * 182.04444885253906f), &mViewCache.mBank); /* s16 - cSAngle */
        cameraPairAngleMultiply(bankDifference.get(), bankStep.get(), eyeCushion);       /* cSAngle::operator*(f32) */
        cameraPairAngleAddAssign(&mViewCache.mBank, bankStep.get());  /* cSAngle::operator+= */
        mEventFlags = mEventFlags | CAMERA_EVENT_TRANSFORM;
        u32 n = work.mTimer - 1;
        work.mTimer = n;
        if (n != 0) return true;
        work.mState = TORNADO_EASE_OUT;
        work.mTimer = 200;
    }
    // Ease the eye and bank back after the turning phase.
    cameraPairRelational(this, eye.get(), mpPlayerActor, endOffset.get());
    cameraPairVectorSubtract(eye.get(), difference.get(), &mViewCache.mCenter);
    cameraPairVectorScale(difference.get(), scaled.get(), quarter);
    PSVECAdd(&mViewCache.mCenter, scaled.get(), &mViewCache.mCenter);
    gabi::Local<cXyz> tgt;
    copyPoint(tgt.get(), &work.mEye);
    cameraPairAttention(this, eye.get(), mpPlayerActor);
    tgt->y = (f32)eye->y;
    cameraPairVectorSubtract(tgt.get(), difference.get(), &mViewCache.mEye);
    f32 eyeCushion = 0.05f;
    cameraPairVectorScale(difference.get(), eye.get(), eyeCushion);
    PSVECAdd(&mViewCache.mEye, eye.get(), &mViewCache.mEye);
    f32 fv = (f32)mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds(90.0f - fv, 0.05f, fv); /* towards 90 */
    cameraPairVectorSubtract(&mViewCache.mEye, eye.get(), &mViewCache.mCenter);
    cameraPairGlobeVector(&mViewCache.mDirection, eye.get());
    gabi::Local<be<s16>> b;
    cameraPairAngleMultiply(&mViewCache.mBank, b.get(), 0.02f); /* bank * 0.02 */
    cameraPairAngleSubtractAssign(&mViewCache.mBank, b.get());                  /* cSAngle::operator-= */
    mEventFlags = mEventFlags | CAMERA_EVENT_TRANSFORM;
    u32 n = work.mTimer - 1;
    work.mTimer = n;
    if (n == 0) work.mState = TORNADO_DONE;
    return true;
}

/* 0253B168 */
bool event_camera_pair::dCamera_c::twoActor0EvCamera() {
    f32 zero = 0.0f;
    auto* zeroData = gabi::at<CameraEventZeroHD>(0x1047589C);
    auto* defaults = gabi::at<TwoActorDefaultsHD>(0x101D6244);
    if ((u32)zeroData->mGuard == 0) {
        zeroData->mZero.x = zero;
        zeroData->mZero.z = zero;
        zeroData->mGuard = 1;
        zeroData->mZero.y = zero;
    }
    auto& work = mWork.twoActor;
    if (m11C == 0) {
        work.mpActor1 = cameraPairGetActor(this, "Actor1", "@PLAYER");
        fopAc_ac_c* b = cameraPairGetActor(this, "Actor2", "@STARTER");
        fopAc_ac_c* a = work.mpActor1;
        work.mpActor2 = b;
        if (a == 0 || b == 0) return true;
        fopAc_ac_c* actor2 = work.mpActor2;
        work.mActorId1 = a != 0 ? (u32)a->mBsPcId : fpcM_ERROR_PROCESS_ID_e;
        work.mActorId2 = actor2 != 0 ? (u32)actor2->mBsPcId : fpcM_ERROR_PROCESS_ID_e;
        gabi::Local<cXyz> centerGapDefault;
        f32 y = (f32)zeroData->mZero.y, zz = (f32)zeroData->mZero.z, x = (f32)zeroData->mZero.x;
        centerGapDefault->y = y;
        centerGapDefault->x = x;
        centerGapDefault->z = zz;
        cameraPairGetXyz(this, &work.mCenterGap, "CtrGap", centerGapDefault.get());
        cameraPairGetFloat(this, &work.mCenterRatio, "CtrRatio", defaults->mCenterRatio);
        cameraPairGetFloat(this, &work.mCenterCushion, "CtrCus", defaults->mCenterCushion);
        cameraPairGetFloat(this, &work.mEyeCushion, "EyeCus", defaults->mEyeCushion);
        cameraPairGetFloat(this, &work.mRadiusMin, "RadiusMin", defaults->mRadiusMin);
        cameraPairGetFloat(this, &work.mRadiusMax, "RadiusMax", defaults->mRadiusMax);
        cameraPairGetFloat(this, &work.mLatitudeMin, "LatitudeMin", defaults->mLatitudeMin);
        cameraPairGetFloat(this, &work.mLatitudeMax, "LatitudeMax", defaults->mLatitudeMax);
        cameraPairGetFloat(this, &work.mLongitudeMin, "LongitudeMin", defaults->mLongitudeMin);
        cameraPairGetFloat(this, &work.mLongitudeMax, "LongitudeMax", defaults->mLongitudeMax);
        cameraPairGetFloat(this, &work.mFovy, "Fovy", defaults->mFovy);
        work.mRadius = (f32)mViewCache.mDirection.mRadius;
        work.mLatitude = cameraPairAngleDegree(&mViewCache.mDirection.mV);
        work.mLongitude = cameraPairAngleDegree(&mViewCache.mDirection.mU);
    }
    gabi::Local<cXyz> actor1Position;
    gabi::Local<cXyz> actor2Position;
    gabi::Local<cXyz> actorSeparation;
    gabi::Local<CameraEventGlobeHD> actorDirection;
    gabi::Local<cXyz> pairCenter;
    cameraPairAttention(this, actor1Position.get(), work.mpActor1);
    cameraPairAttention(this, actor2Position.get(), work.mpActor2);
    cameraPairVectorSubtract(actor1Position.get(), actorSeparation.get(), actor2Position.get());
    cameraPairGlobeFromVector(actorDirection.get(), actorSeparation.get()); /* cSGlobe(const cXyz&): the direction between the actors */
    {
        gabi::Local<be<u32>> id;
        u32 pid = work.mActorId1;
        *id = pid;
        if (pid == fpcM_ERROR_PROCESS_ID_e) return true;
        if (fopAcIt_Judge(JUDGE_BY_PROCESS_ID, id.get()) == 0) return true;
    }
    {
        gabi::Local<be<u32>> id;
        u32 pid = work.mActorId2;
        *id = pid;
        if (pid == fpcM_ERROR_PROCESS_ID_e) return true;
        if (fopAcIt_Judge(JUDGE_BY_PROCESS_ID, id.get()) == 0) return true;
    }
    f32 ratio = (f32)work.mCenterRatio;
    if (ratio < zero || ratio > 1.0f) { /* blt / ble: NaN takes the second branch */
        gabi::Local<cXyz> r;
        cameraPairRelationalBetween(this, r.get(), work.mpActor1, work.mpActor2, &work.mCenterGap, 0.25f); /* cameraPairRelational(a1, a2, gap, 0.25) */
        copyPoint(pairCenter.get(), r.get());
    } else {
        gabi::Local<cXyz> p1;
        gabi::Local<cXyz> p2;
        gabi::Local<cXyz> s;
        gabi::Local<cXyz> m;
        cameraPairAttention(this, p1.get(), work.mpActor1);
        cameraPairAttention(this, p2.get(), work.mpActor2);
        cameraPairVectorAdd(p1.get(), s.get(), p2.get());
        cameraPairVectorScale(s.get(), m.get(), (f32)work.mCenterRatio);
        copyPoint(pairCenter.get(), m.get());
    }
    cameraPairVectorSubtract(pairCenter.get(), actor1Position.get(), &mViewCache.mCenter);
    cameraPairVectorScale(actor1Position.get(), actor2Position.get(), (f32)work.mCenterCushion);
    PSVECAdd(&mViewCache.mCenter, actor2Position.get(), &mViewCache.mCenter);
    /* longitude */
    gabi::Local<be<s16>> du;
    cameraPairAngleSubtract(&actorDirection->mU, du.get(), &mViewCache.mDirection.mU); /* cSAngle::operator- */
    f32 lon = cameraPairAngleDegree(du.get());
    f32 cur = (f32)work.mLongitude;
    f32 lmin = (f32)work.mLongitudeMin;
    if (cur < lmin) lon = lmin; /* bge: taken on NaN */
    else if (cur > (f32)work.mLongitudeMax) lon = (f32)work.mLongitudeMax;
    lon = lon + cameraPairAngleDegree(&actorDirection->mU);
    cur = (f32)work.mLongitude;
    work.mLongitude = gabi::fmadds(lon - cur, (f32)work.mEyeCushion, cur);
    /* latitude */
    f32 lat = cameraPairAngleDegree(&mViewCache.mDirection.mV);
    f32 vmin = (f32)work.mLatitudeMin;
    f32 v = (f32)work.mLatitude;
    f32 tgt;
    if (v < vmin) tgt = vmin;
    else if (v > (f32)work.mLatitudeMax) tgt = (f32)work.mLatitudeMax;
    else tgt = lat;
    f32 eyeCus = (f32)work.mEyeCushion;
    f32 rad = (f32)work.mRadius;
    f32 rmin = (f32)work.mRadiusMin;
    work.mLatitude = gabi::fmadds(tgt - v, eyeCus, v);
    f32 curR = (f32)mViewCache.mDirection.mRadius;
    f32 rt;
    if (rad < rmin) rt = rmin;
    else if (rad > (f32)work.mRadiusMax) rt = (f32)work.mRadiusMax;
    else rt = curR;
    f32 newR = gabi::fmadds(rt - rad, (f32)work.mEyeCushion, rad);
    work.mRadius = newR;
    gabi::Local<be<s16>[2]> va;
    be<s16>* av = cameraPairAngleFromDegrees(&(*va.get())[0], (f32)work.mLatitude);     /* cSAngle(latitude) */
    be<s16>* au = cameraPairAngleFromDegrees(&(*va.get())[1], (f32)work.mLongitude); /* cSAngle(longitude) */
    cameraPairGlobeSet(&mViewCache.mDirection, newR, av, au);          /* cSGlobe::Val(r, V, U) */
    cameraPairGlobeXyz(&mViewCache.mDirection, actor1Position.get());
    cameraPairVectorAdd(&mViewCache.mCenter, actor2Position.get(), actor1Position.get());
    copyPoint(&mViewCache.mEye, actor2Position.get());
    f32 fv = (f32)mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds((f32)work.mFovy - fv, (f32)work.mCenterCushion, fv);
    return true;
}

// clang-format off
/* WWHD glue
str 1004D52C "Actor1"
str 1004D524 "@PLAYER"
str 1004D534 "Actor2"
str 1004D58C "@STARTER"
str 1004D53C "CtrGap"
str 1004D598 "CtrRatio"
str 1004D544 "CtrCus"
str 1004D54C "EyeCus"
str 1004D574 "RadiusMin"
str 1004D580 "RadiusMax"
str 1004D55C "LatitudeMin"
str 1004D568 "LatitudeMax"
str 1004D5A4 "LongitudeMin"
str 1004D5B4 "LongitudeMax"
str 1004D554 "Fovy"
ext 020066C0 be<s16>* cameraPairAngleFromDegrees(be<s16>*,f32)
ext 02006644 be<s16>* cameraPairAngleCopy(be<s16>*,const be<s16>*)
ext 020065EC void cameraPairScalarMinusAngle(be<s16>*,s32,const be<s16>*)
ext 0200693C void cameraPairAngleMultiply(const be<s16>*,be<s16>*,f32)
ext 020068CC void cameraPairAngleAddAssign(be<s16>*,const be<s16>*)
ext 020068E0 void cameraPairAngleSubtractAssign(be<s16>*,const be<s16>*)
ext 020068B0 void cameraPairAngleSubtract(const be<s16>*,be<s16>*,const be<s16>*)
ext 02006720 f32 cameraPairAngleDegree(const be<s16>*)
ext 0201ADE0 void cameraPairVectorSubtract(const cXyz*,cXyz*,const cXyz*)
ext 0201AD78 void cameraPairVectorAdd(const cXyz*,cXyz*,const cXyz*)
ext 0201AE48 void cameraPairVectorScale(const cXyz*,cXyz*,f32)
ext 028E8DD0 f32 cameraPairVectorMagnitudeSquared(const cXyz*)
ext 028F4384 f32 cameraPairSquareRoot(f32)
ext 020072E0 void cameraPairGlobeVector(CameraEventGlobeHD*,const cXyz*)
ext 02007324 void cameraPairGlobeFromVector(CameraEventGlobeHD*,const cXyz*)
ext 020071E0 void cameraPairGlobeSet(CameraEventGlobeHD*,f32,const be<s16>*,const be<s16>*)
ext 020073AC void cameraPairGlobeXyz(const CameraEventGlobeHD*,cXyz*)
ext 0250242C void cameraPairRelational(event_camera_pair::dCamera_c*,cXyz*,fopAc_ac_c*,const cXyz*)
ext 02514D00 void cameraPairRelationalWithAngle(event_camera_pair::dCamera_c*,cXyz*,fopAc_ac_c*,const cXyz*,const be<s16>*)
ext 02508804 void cameraPairRelationalBetween(event_camera_pair::dCamera_c*,cXyz*,fopAc_ac_c*,fopAc_ac_c*,const cXyz*,f32)
ext 024F8F3C void cameraPairAttention(event_camera_pair::dCamera_c*,cXyz*,fopAc_ac_c*)
ext 024FCBE8 bool cameraPairLineCheck(event_camera_pair::dCamera_c*,const cXyz*,const cXyz*,u32)
ext 024F7A40 void cameraPairRotateY(cXyz*,const cXyz*,const be<s16>*)
ext 025187D8 bool cameraPairCheckCamera(dCcS*,cXyz*,cXyz*,f32,fopAc_ac_c*,fopAc_ac_c*)
ext 02530CB4 fopAc_ac_c* cameraPairGetActor(event_camera_pair::dCamera_c*,const char*,const char*)
ext 0253072C u32 cameraPairGetFloat(event_camera_pair::dCamera_c*,be<f32>*,const char*,f32)
ext 0253086C u32 cameraPairGetXyz(event_camera_pair::dCamera_c*,cXyz*,const char*,const cXyz*)
*/
