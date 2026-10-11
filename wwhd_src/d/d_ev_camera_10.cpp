#include "d/d_ev_camera_10.h"
namespace event_camera_uniform {
static EventString asokoName;
static EventString transTimerName;
static EventString transCurveName;
static EventString transEyeName;
static EventString transCenterName;
static EventString transFovyName;
static EventString transStartEyeName;
static EventString transStartCenterName;
static EventString transStartFovyName;
static EventString transBankName;
static EventString transStartBankName;
static EventString transTypeName;
static EventString transMaskName;
static EventString transMaskDefault;
static EventString transActorName;
static EventString transCushionName;
static EventString tactWindowName;
static EventString brakeTimerName;
static EventString brakingPointName;
static EventString brakeTypeName;
static EventString acceleTimerName;
static EventString acceleRampName;
static EventString acceleTypeName;
static EventString brakeName0;
static EventString brakeName1;
static EventString brakeName2;
static EventString brakeName3;
static EventString brakeName4;
static EventString brakeName5;
static EventString brakeName6;
static EventString brakeName7;
static EventString brakeName8;
static EventString brakeName9;
static EventString brakeName10;
static EventString brakeName11;
static EventString brakeName12;
static EventString acceleName0;
static EventString acceleName1;
static EventString acceleName2;
static EventString acceleName3;
static EventString acceleName4;
static EventString acceleName5;
static EventString acceleName6;
static EventString acceleName7;
static EventString acceleName8;
static EventString acceleName9;
static EventString acceleName10;
static EventString acceleName11;
static EventString acceleName12;
static const char* const brakeNames[] = {
    brakeName0.text,  brakeName1.text,  brakeName2.text, brakeName3.text, brakeName4.text,
    brakeName5.text,  brakeName6.text,  brakeName7.text, brakeName8.text, brakeName9.text,
    brakeName10.text, brakeName11.text, brakeName12.text};
static const char* const acceleNames[] = {
    acceleName0.text,  acceleName1.text,  acceleName2.text, acceleName3.text, acceleName4.text,
    acceleName5.text,  acceleName6.text,  acceleName7.text, acceleName8.text, acceleName9.text,
    acceleName10.text, acceleName11.text, acceleName12.text};
static SinCosTable sinCosTable;
static SplineCurve splineCurve;
enum : u32 {
    STAGE_STRING_VTABLE = 0x1004CDBC,
    JUDGE_BY_NAME = 0x025E121C,
    JUDGE_BY_ID = 0x025E1234
};

// HD: named bindings retain the individual calls used by this event-camera unit.
static inline GuestWord& transType(MoveWork* w, MoveKind k) {
    return k == UNIFORM ? w->mTrans.mTransType : w->mBrake.mTransType;
}
static inline GuestFloat& cushionParam(MoveWork* w, MoveKind k) {
    return k == UNIFORM ? w->mTrans.mCushion : w->mBrake.mCushion;
}
static inline be<u8>& bankSet(MoveWork* w, MoveKind k) {
    return k == UNIFORM ? w->mTrans.mBankSet : w->mBrake.mBankSet;
}
static inline cSAngle_l& startLongitude(MoveWork* w, MoveKind k) {
    return k == UNIFORM ? w->mTrans.mStartGlobe.mU : w->mBrake.mStartGlobe.mU;
}
static void copyWords(cXyz* dst, cXyz* src) {
    auto* d = reinterpret_cast<VectorWords*>(dst);
    auto* s = reinterpret_cast<VectorWords*>(src);
    d->x = (u32)s->x;
    d->y = (u32)s->y;
    d->z = (u32)s->z;
}
static void copyFloats(cXyz* dst, cXyz* src) {
    f32 x = src->x, y = src->y, z = src->z;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}
static void negateX(cXyz* v) {
    v->x = -(f32)v->x;
}
static void setDone(dCamera_c* c) {
    c->m102 = 1;
    c->m101 = 1;
    c->m100 = 1;
}
static f32 vectorLength(cXyz* v) {
    return cameraUniformSqrt(cameraUniformSquareMag(v));
}
/* inlined into uniformTransEvCamera */
static bool stageIsAsoko() {
    StageString a;
    StageString b;
    a.mVtable = STAGE_STRING_VTABLE;
    a.mpText = asokoName.text;
    const char* name = dComIfGp_getStartStageName();
    b.mVtable = STAGE_STRING_VTABLE;
    b.mpText = const_cast<char*>(name);
    gabi::call_ptr(gabi::at<StageStringVtable>((u32)a.mVtable)->mEmpty, &a);
    gabi::call_ptr(gabi::at<StageStringVtable>((u32)a.mVtable)->mEmpty, &a);
    gabi::call_ptr(gabi::at<StageStringVtable>((u32)b.mVtable)->mEmpty, &b);
    const be<u8>* p = reinterpret_cast<const be<u8>*>(a.mpText.get());
    const be<u8>* q = reinterpret_cast<const be<u8>*>(b.mpText.get());
    if (p == q)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 x = p[i], y = q[i];
        if (x != y)
            return false;
        if (x == 0)
            return true;
    }
    return false;
}
static void finishInit(dCamera_c* c, MoveWork* w) {
    auto* g = reinterpret_cast<GlobeWords*>(cameraUniformGlobeInvert(&c->mDirection));
    auto* d = reinterpret_cast<GlobeWords*>(&w->mTrans.mStartGlobe);
    d->mRadius = (u32)g->mRadius;
    d->mAngles = (u32)g->mAngles;
    setDone(c);
}
/* inlined into uniformTransEvCamera, uniformBrakeEvCamera, uniformAcceleEvCamera */
static void relativeSide(dCamera_c* c, MoveWork* w, cXyz* center, cXyz* eye) {
    cXyz r;
    cXyz a;
    cXyz b;
    cameraUniformRelationalPos(c, &r, w->mpRelActor, center);
    copyWords(&a, &r);
    if (c->m080 & 1)
        negateX(eye);
    cameraUniformRelationalPos(c, &r, w->mpRelActor, eye);
    copyWords(&b, &r);
    if (cameraUniformLineBGCheck(c, &a, &b, 0x8F))
        negateX(eye);
}
/* inlined into uniformTransEvCamera, uniformBrakeEvCamera, uniformAcceleEvCamera */
static void relativeFacing(dCamera_c* c, MoveWork* w, cXyz* from, be<char>* mc, cXyz* center,
                           be<char>* me, cXyz* eye) {
    cXyz pos;
    cXyz difference;
    cSGlobe_l globe;
    AnglePair angles;
    cXyz a;
    cXyz b;
    cameraUniformPositionOf(c, &pos, w->mpRelActor);
    cameraUniformSubtract(from, &difference, &pos);
    cameraUniformGlobeCtor(&globe, &difference);
    cameraUniformDirectionOf(c, &angles.m0, w->mpRelActor);
    cameraUniformAngleSubtract(&globe.mU, &angles.m1, &angles.m0);
    if ((s16)angles.m1 < (s16)zeroAngle()) {
        u8 e = *me;
        if (*mc == 'n')
            negateX(center);
        if (e == 'n')
            negateX(eye);
    }
    cameraUniformRelationalPos(c, &pos, w->mpRelActor, center);
    copyWords(&a, &pos);
    cameraUniformRelationalPos(c, &pos, w->mpRelActor, eye);
    copyWords(&b, &pos);
    if (cameraUniformLineBGCheck(c, &a, &b, 0x8F))
        negateX(eye);
}
/* inlined into uniformTransEvCamera, uniformBrakeEvCamera, uniformAcceleEvCamera */
static void relativePlayer(dCamera_c* c, MoveWork* w, cXyz* offset) {
    cXyz original;
    cXyz target;
    cXyz player;
    cXyz difference;
    copyFloats(&original, offset);
    cameraUniformRelationalPos(c, &target, w->mpRelActor, &original);
    cameraUniformPositionOf(c, &player, c->mpPlayerActor);
    cameraUniformSubtract(&target, &difference, &player);
    f32 d1 = vectorLength(&difference);
    negateX(&original);
    cameraUniformRelationalPos(c, &player, w->mpRelActor, &original);
    copyWords(&target, &player);
    cameraUniformPositionOf(c, &difference, c->mpPlayerActor);
    cameraUniformSubtract(&target, &player, &difference);
    f32 d2 = vectorLength(&player);
    if (d1 < d2)
        negateX(offset);
}
/* inlined into uniformTransEvCamera, uniformBrakeEvCamera, uniformAcceleEvCamera */
static void relativePoint(dCamera_c* c, MoveWork* w, u8 mask, cXyz* offset, cXyz* dst, bool rotate,
                          MoveKind kind) {
    cXyz attention;
    cXyz xyz;
    cXyz result;
    if (mask == 't') {
        cameraUniformAttentionPos(c, &attention, w->mpRelActor);
        cameraUniformAdd(&attention, &result, offset);
        copyWords(dst, &result);
    } else if (mask == 'c') {
        cSGlobe_l globe;
        AnglePair angles;
        cameraUniformGlobeCtor(&globe, offset);
        cameraUniformAngleAdd(&startLongitude(w, kind), &angles.m0, &globe.mU);
        auto* angle = cameraUniformAngleShortCtor(&angles.m1, (s32)(s16)angles.m0);
        globe.mU = (s16)*angle;
        cameraUniformAttentionPos(c, &attention, w->mpRelActor);
        cameraUniformGlobeXyz(&globe, &xyz);
        cameraUniformAdd(&attention, &result, &xyz);
        copyWords(dst, &result);
    } else if (mask == '-') {
        if (rotate && (u32)transType(w, kind) == 2) {
            cSAngle_l direction;
            cameraUniformDirectionOf(c, &direction, w->mpRelActor);
            cameraUniformRotateY(&result, offset, &direction);
            copyWords(dst, &result);
        } else
            copyWords(dst, offset);
    } else {
        cameraUniformRelationalPos(c, &result, w->mpRelActor, offset);
        copyWords(dst, &result);
    }
}
static void blendFovy(dCamera_c* c, MoveWork* w, f32 ratio, GuestFloat* cushion) {
    f32 start = w->mStartFovy;
    f32 target = gabi::fmadds((f32)w->mFovy - start, ratio, start);
    f32 current = c->mViewCache.mFovy;
    c->mViewCache.mFovy = gabi::fmadds(target - current, *cushion, current);
}
static void cushionPosition(GuestFloat* cushion, cXyz* pos, cXyz* target, cXyz* tmp1, cXyz* tmp2) {
    cameraUniformSubtract(target, tmp1, pos);
    cameraUniformScale(tmp1, tmp2, *cushion);
    cameraUniformPSVECAdd(pos, tmp2, pos);
}
/* inlined into uniformTransEvCamera, uniformBrakeEvCamera, uniformAcceleEvCamera */
static void moveTail(dCamera_c* c, MoveWork* w, f32 ratio, MoveKind kind) {
    cXyz startCenter;
    cXyz startEye;
    cXyz endCenter;
    cXyz endEye;
    if (!w->mpRelActor) {
        copyWords(&startCenter, &w->mStartCenter);
        copyWords(&startEye, &w->mStartEye);
        copyWords(&endCenter, &w->mCenter);
        copyWords(&endEye, &w->mEye);
    } else {
        relativePoint(c, w, w->mRelUseMask[0], &w->mStartCenter, &startCenter, false, kind);
        relativePoint(c, w, w->mRelUseMask[1], &w->mStartEye, &startEye, false, kind);
        relativePoint(c, w, w->mRelUseMask[2], &w->mCenter, &endCenter, true, kind);
        relativePoint(c, w, w->mRelUseMask[3], &w->mEye, &endEye, true, kind);
    }
    cXyz tmp1;
    cXyz tmp2;
    cXyz tmp3;
    cXyz target;
    u32 type = transType(w, kind);
    if (type == 1) {
        cameraUniformSubtract(&endCenter, &tmp1, &startCenter);
        cameraUniformScale(&tmp1, &tmp2, ratio);
        cameraUniformAdd(&startCenter, &tmp3, &tmp2);
        copyWords(&target, &tmp3);
        cushionPosition(&cushionParam(w, kind), &c->mViewCache.mCenter, &target, &tmp2, &tmp1);
        cSGlobe_l start;
        cSGlobe_l end;
        cSGlobe_l interpolated;
        AngleSix angles;
        cameraUniformSubtract(&startEye, &tmp1, &startCenter);
        cameraUniformGlobeCtor(&start, &tmp1);
        cameraUniformSubtract(&endEye, &tmp2, &endCenter);
        cameraUniformGlobeCtor(&end, &tmp2);
        cameraUniformAngleSubtract(&end.mV, &angles.m0, &start.mV);
        cameraUniformAngleScale(&angles.m0, &angles.m1, ratio);
        cameraUniformAngleAdd(&start.mV, &angles.m2, &angles.m1);
        cameraUniformAngleSubtract(&end.mU, &angles.m3, &start.mU);
        cameraUniformAngleScale(&angles.m3, &angles.m4, ratio);
        cameraUniformAngleAdd(&start.mU, &angles.m5, &angles.m4);
        f32 radius = start.mRadius;
        cameraUniformGlobeAnglesCtor(&interpolated,
                                     gabi::fmadds((f32)end.mRadius - radius, ratio, radius),
                                     &angles.m2, &angles.m5);
        cameraUniformGlobeXyz(&interpolated, &tmp3);
        cameraUniformAdd(&c->mViewCache.mCenter, &tmp2, &tmp3);
        copyWords(&target, &tmp2);
        cushionPosition(&cushionParam(w, kind), &c->mViewCache.mEye, &target, &tmp1, &tmp3);
    } else if (type == 2) {
        cameraUniformScale(&endCenter, &tmp1, ratio);
        cameraUniformAdd(&startCenter, &tmp2, &tmp1);
        copyWords(&target, &tmp2);
        cushionPosition(&cushionParam(w, kind), &c->mViewCache.mCenter, &target, &tmp2, &tmp1);
        cameraUniformScale(&endEye, &tmp2, ratio);
        cameraUniformAdd(&startEye, &tmp1, &tmp2);
        copyWords(&target, &tmp1);
        cushionPosition(&cushionParam(w, kind), &c->mViewCache.mEye, &target, &tmp2, &tmp1);
    } else {
        cameraUniformSubtract(&endCenter, &tmp1, &startCenter);
        cameraUniformScale(&tmp1, &tmp2, ratio);
        cameraUniformAdd(&startCenter, &tmp3, &tmp2);
        copyWords(&target, &tmp3);
        cushionPosition(&cushionParam(w, kind), &c->mViewCache.mCenter, &target, &tmp2, &tmp1);
        cameraUniformSubtract(&endEye, &tmp3, &startEye);
        cameraUniformScale(&tmp3, &tmp2, ratio);
        cameraUniformAdd(&startEye, &tmp1, &tmp2);
        copyWords(&target, &tmp1);
        cushionPosition(&cushionParam(w, kind), &c->mViewCache.mEye, &target, &tmp2, &tmp1);
    }
    blendFovy(c, w, ratio, &cushionParam(w, kind));
    if (bankSet(w, kind)) {
        AngleFour angles;
        f32 start = w->mStartBank;
        f32 bank = gabi::fmadds((f32)w->mBank - start, ratio, start);
        // HD: braking uses a wrapped signed degree conversion; uniform translation calls
        // cSAngle(f32).
        if (kind == BRAKING)
            cameraUniformShortMinusAngle(
                &angles.m1, (s32)(s16)gabi::ftoi(bank * 182.04444885253906f), &c->mViewCache.mBank);
        else {
            auto* angle = cameraUniformAngleFloatCtor(&angles.m0, bank);
            cameraUniformAngleSubtract(angle, &angles.m1, &c->mViewCache.mBank);
        }
        cameraUniformAngleScale(&angles.m1, &angles.m2, cushionParam(w, kind));
        cameraUniformAngleAddAssign(&c->mViewCache.mBank, &angles.m2);
        c->mEventFlags = (u32)c->mEventFlags | 0x400;
    }
    cXyz difference;
    cameraUniformSubtract(&c->mViewCache.mEye, &difference, &c->mViewCache.mCenter);
    cameraUniformGlobeVal(&c->mViewCache.mDirection, &difference);
}
/* 02531D20 */
u32 uniformTransEvCamera(dCamera_c* camera) {
    auto* work = reinterpret_cast<MoveWork*>(camera->mWork);
    if (camera->m11C == 0) {
        if (!cameraUniformGetRequiredInt(camera, &work->mTimer, transTimerName.text))
            return 1;
        cameraUniformGetInt(camera, &work->mTrans.mBSpCurve, transCurveName.text, 1);
        if (work->mTrans.mBSpCurve)
            cameraUniformSplineInit(camera->mEventData.mSpline2DPath, 4, work->mTimer);
        cXyz fallback;
        copyFloats(&fallback, &camera->mEye);
        cameraUniformGetXyz(camera, &work->mEye, transEyeName.text, &fallback);
        copyFloats(&fallback, &camera->mCenter);
        cameraUniformGetXyz(camera, &work->mCenter, transCenterName.text, &fallback);
        if (camera->mEventData.mStaffIdx == 0x104 && stageIsAsoko() &&
            (f32)work->mEye.z < -1000.0f) {
            f32 eye = work->mEye.x, center = work->mCenter.x;
            work->mEye.x = eye + -80.0f;
            work->mCenter.x = center + -80.0f;
        }
        cameraUniformGetFloat(camera, &work->mFovy, transFovyName.text, camera->mFovy);
        copyFloats(&fallback, &camera->mEye);
        cameraUniformGetXyz(camera, &work->mStartEye, transStartEyeName.text, &fallback);
        copyFloats(&fallback, &camera->mCenter);
        cameraUniformGetXyz(camera, &work->mStartCenter, transStartCenterName.text, &fallback);
        cameraUniformGetFloat(camera, &work->mStartFovy, transStartFovyName.text, camera->mFovy);
        f32 degrees = cameraUniformAngleDegree(&camera->mBank);
        work->mTrans.mBankSet =
            (u8)cameraUniformGetFloat(camera, &work->mBank, transBankName.text, degrees);
        degrees = cameraUniformAngleDegree(&camera->mBank);
        u32 bank =
            cameraUniformGetFloat(camera, &work->mStartBank, transStartBankName.text, degrees);
        work->mTrans.mBankSet = (u8)((u8)work->mTrans.mBankSet | bank);
        cameraUniformGetInt(camera, &work->mTrans.mTransType, transTypeName.text, 0);
        cameraUniformGetString(camera, work->mRelUseMask, transMaskName.text,
                               transMaskDefault.text);
        work->mpRelActor = cameraUniformGetActor(camera, transActorName.text);
        cameraUniformGetFloat(camera, &work->mTrans.mCushion, transCushionName.text, 1.0f);
        fopAc_ac_c* actor = work->mpRelActor;
        if (actor) {
            work->mRelActorId = actor ? (u32)actor->mBsPcId : 0xFFFFFFFF;
            if (work->mRelUseMask[1] == 'r')
                relativeSide(camera, work, &work->mStartCenter, &work->mStartEye);
            if (work->mRelUseMask[0] == 'n' || work->mRelUseMask[1] == 'n')
                relativeFacing(camera, work, &camera->mEye, &work->mRelUseMask[0],
                               &work->mStartCenter, &work->mRelUseMask[1], &work->mStartEye);
            if (work->mRelUseMask[2] == 'n' || work->mRelUseMask[3] == 'n')
                relativeFacing(camera, work, &work->mEye, &work->mRelUseMask[2], &work->mCenter,
                               &work->mRelUseMask[3], &work->mEye);
            if (work->mRelUseMask[2] == 'p')
                relativePlayer(camera, work, &work->mCenter);
            u8 mask = work->mRelUseMask[3];
            if (mask == 'p')
                relativePlayer(camera, work, &work->mEye);
            else if (mask == 'r')
                relativeSide(camera, work, &work->mCenter, &work->mEye);
        }
        finishInit(camera, work);
    }
    // HD: TACT_WINDOW places the eye behind Link and clips the resulting path.
    s32 index = cameraUniformEventIndex(dComIfGp_getPEvtManager(), tactWindowName.text, 0xFF);
    auto* event = cameraUniformEventData(dComIfGp_getPEvtManager(), (s32)(s16)index);
    if (event && event->mState == 2 && (dComIfGp_getPlayerStatus0(0) & 0x10000)) {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        if (player) {
            u32 index = (u32)(u16)((s16)player->shape_angle.y + -0x8000) >> 3;
            f32 sine = sinCosTable.mEntries[index].mSin, cosine = sinCosTable.mEntries[index].mCos;
            work->mEye.y = 100.0f;
            work->mEye.x = 800.0f * sine;
            work->mEye.z = 800.0f * cosine;
            cSAngle_l actorName;
            actorName = 0xA5;
            auto* actor =
                reinterpret_cast<TactActorView*>(fopAcIt_Judge(JUDGE_BY_NAME, &actorName));
            cXyz base;
            cXyz point1;
            cXyz point2;
            cXyz cross;
            cXyz target;
            cXyz temporary;
            f32 x = player->current.pos.x;
            f32 y = (f32)actor->mHeight + 120.0f;
            f32 z = player->current.pos.z;
            base.x = x;
            base.z = z;
            base.y = y;
            cameraUniformAdd(&base, &point1, &work->mCenter);
            cameraUniformAdd(&base, &point2, &work->mEye);
            if (cameraUniformLineBGCheckCross(camera, &point1, &point2, &cross, 0x8F)) {
                cameraUniformSubtract(&cross, &temporary, &base);
                copyWords(&target, &temporary);
            } else
                copyWords(&target, &work->mEye);
            cameraUniformAddCalcPos(&work->mEye, &target, 5.0f, 20.0f, 1.0f);
            cameraUniformAdd(&base, &point2, &work->mEye);
            if (cameraUniformLineBGCheckCross(camera, &work->mStartEye, &point2, &cross, 0x8F)) {
                cameraUniformSubtract(&cross, &temporary, &base);
                copyWords(&work->mEye, &temporary);
            }
            finishInit(camera, work);
        }
    }
    if (work->mpRelActor) {
        GuestWord id;
        u32 actorId = work->mRelActorId;
        id = actorId;
        if (actorId == 0xFFFFFFFF)
            return 1;
        if (!fopAcIt_Judge(JUDGE_BY_ID, &id))
            return 1;
    }
    u32 result = 0;
    f32 ratio = 1.0f;
    u32 frame = camera->m11C;
    u32 timer = work->mTimer;
    if (frame >= timer)
        result = 1;
    else if (work->mTrans.mBSpCurve) {
        cameraUniformSplineStep(camera->mEventData.mSpline2DPath);
        ratio = cameraUniformSplineCalc(camera->mEventData.mSpline2DPath, splineCurve.mValues);
    } else
        ratio = (f32)(f64)(frame + 1) / (f32)(f64)(s32)timer;
    moveTail(camera, work, ratio, UNIFORM);
    return result;
}
// HD: PPC slw masks its count to six bits and returns zero for counts 32..63.
static u32 shiftLeft(u32 value, u32 shift) {
    shift &= 63;
    return shift >= 32 ? 0 : value << shift;
}
static void brakeTotal(MoveWork* w) {
    s32 ramp = (s32)(u32)w->mBrake.mRampFrames;
    if (w->mBrake.mBrakeType != 1) {
        f32 full = (f32)(f64)(s32)((u32)w->mBrake.mFullFrames * (u32)ramp);
        f32 tail = (f32)(f64)(s32)((s32)((u32)ramp * (u32)(ramp + 1)) >> 1);
        w->mBrake.mTotal = full;
        w->mBrake.mTotal = full + tail;
    } else {
        u32 value = shiftLeft(1, (u32)ramp - 1);
        f32 step = (f32)(f64)(s32)value;
        f32 frames = (f32)(f64)(s32)(u32)w->mBrake.mFullFrames;
        f32 tail = (step + step) - 1.0f;
        f32 full = frames * step;
        w->mBrake.mTotal = full + tail;
    }
}
/* inlined into uniformBrakeEvCamera, uniformAcceleEvCamera */
static void brakeParams(dCamera_c* c, MoveWork* w, const char* const* names) {
    cXyz fallback;
    copyFloats(&fallback, &c->mEye);
    cameraUniformGetXyz(c, &w->mEye, names[0], &fallback);
    copyFloats(&fallback, &c->mCenter);
    cameraUniformGetXyz(c, &w->mCenter, names[1], &fallback);
    cameraUniformGetFloat(c, &w->mFovy, names[2], c->mFovy);
    copyFloats(&fallback, &c->mEye);
    cameraUniformGetXyz(c, &w->mStartEye, names[3], &fallback);
    copyFloats(&fallback, &c->mCenter);
    cameraUniformGetXyz(c, &w->mStartCenter, names[4], &fallback);
    cameraUniformGetFloat(c, &w->mStartFovy, names[5], c->mFovy);
    f32 degrees = cameraUniformAngleDegree(&c->mBank);
    w->mBrake.mBankSet = (u8)cameraUniformGetFloat(c, &w->mBank, names[6], degrees);
    degrees = cameraUniformAngleDegree(&c->mBank);
    u32 bank = cameraUniformGetFloat(c, &w->mStartBank, names[7], degrees);
    w->mBrake.mBankSet = (u8)((u8)w->mBrake.mBankSet | bank);
    cameraUniformGetInt(c, &w->mBrake.mTransType, names[8], 0);
    cameraUniformGetString(c, w->mRelUseMask, names[9], names[10]);
    w->mpRelActor = cameraUniformGetActor(c, names[11]);
    cameraUniformGetFloat(c, &w->mBrake.mCushion, names[12], 1.0f);
    fopAc_ac_c* actor = w->mpRelActor;
    if (actor) {
        w->mRelActorId = actor ? (u32)actor->mBsPcId : 0xFFFFFFFF;
        if (w->mRelUseMask[1] == 'r')
            relativeSide(c, w, &w->mStartCenter, &w->mStartEye);
        if (w->mRelUseMask[0] == 'n' || w->mRelUseMask[1] == 'n')
            relativeFacing(c, w, &c->mEye, &w->mRelUseMask[0], &w->mStartCenter, &w->mRelUseMask[1],
                           &w->mStartEye);
        if (w->mRelUseMask[2] == 'n' || w->mRelUseMask[3] == 'n')
            relativeFacing(c, w, &w->mEye, &w->mRelUseMask[2], &w->mCenter, &w->mRelUseMask[3],
                           &w->mEye);
        if (w->mRelUseMask[2] == 'p')
            relativePlayer(c, w, &w->mCenter);
        u8 mask = w->mRelUseMask[3];
        if (mask == 'p')
            relativePlayer(c, w, &w->mEye);
        else if (mask == 'r')
            relativeSide(c, w, &w->mCenter, &w->mEye);
    }
    auto* globe = reinterpret_cast<GlobeWords*>(cameraUniformGlobeInvert(&c->mDirection));
    auto* dst = reinterpret_cast<GlobeWords*>(&w->mBrake.mStartGlobe);
    dst->mRadius = (u32)globe->mRadius;
    w->mBrake.mSteps = 0.0f;
    dst->mAngles = (u32)globe->mAngles;
    setDone(c);
}
/* inlined into uniformBrakeEvCamera, uniformAcceleEvCamera */
static bool relActorGone(MoveWork* w) {
    if (!w->mpRelActor)
        return false;
    GuestWord id;
    u32 actorId = w->mRelActorId;
    id = actorId;
    if (actorId == 0xFFFFFFFF)
        return true;
    return !fopAcIt_Judge(JUDGE_BY_ID, &id);
}
static f32 brakeStep(MoveWork* w, f32 step) {
    f32 total = (f32)w->mBrake.mSteps + step;
    w->mBrake.mSteps = total;
    return total / (f32)w->mBrake.mTotal;
}
/* 0253BC1C */
u32 uniformBrakeEvCamera(dCamera_c* camera) {
    auto* w = reinterpret_cast<MoveWork*>(camera->mWork);
    if (camera->m11C == 0) {
        if (!cameraUniformGetRequiredInt(camera, &w->mTimer, brakeTimerName.text))
            return 1;
        cameraUniformGetInt(camera, &w->mBrake.mFullFrames, brakingPointName.text, 0);
        u32 point = w->mBrake.mFullFrames;
        w->mBrake.mRampFrames = (u32)w->mTimer - point;
        cameraUniformGetInt(camera, &w->mBrake.mBrakeType, brakeTypeName.text, 0);
        brakeTotal(w);
        brakeParams(camera, w, brakeNames);
    }
    if (relActorGone(w))
        return 1;
    u32 result = 0;
    f32 ratio = 1.0f;
    u32 frame = camera->m11C;
    u32 timer = w->mTimer;
    if (frame >= timer)
        result = 1;
    else if (w->mBrake.mBrakeType != 1) {
        if (frame < (u32)w->mBrake.mFullFrames)
            ratio = brakeStep(w, (f32)(f64)(s32)(u32)w->mBrake.mRampFrames);
        else
            ratio = brakeStep(w, (f32)(f64)(timer - frame));
    } else {
        u32 shift =
            frame < (u32)w->mBrake.mFullFrames ? (u32)w->mBrake.mRampFrames - 1 : timer - frame - 1;
        ratio = brakeStep(w, (f32)(f64)(s32)shiftLeft(1, shift));
    }
    moveTail(camera, w, ratio, BRAKING);
    return result;
}
/* 0253D1A4 */
u32 uniformAcceleEvCamera(dCamera_c* camera) {
    auto* w = reinterpret_cast<MoveWork*>(camera->mWork);
    if (camera->m11C == 0) {
        if (!cameraUniformGetRequiredInt(camera, &w->mTimer, acceleTimerName.text))
            return 1;
        cameraUniformGetInt(camera, &w->mBrake.mRampFrames, acceleRampName.text,
                            (s32)(u32)w->mTimer);
        cameraUniformGetInt(camera, &w->mBrake.mBrakeType, acceleTypeName.text, 0);
        w->mBrake.mFullFrames = (u32)w->mTimer - (u32)w->mBrake.mRampFrames;
        brakeTotal(w);
        brakeParams(camera, w, acceleNames);
    }
    if (relActorGone(w))
        return 1;
    u32 result = 0;
    f32 ratio = 1.0f;
    u32 frame = camera->m11C;
    u32 timer = w->mTimer;
    if (frame >= timer)
        result = 1;
    else if (w->mBrake.mBrakeType != 1) {
        u32 ramp = w->mBrake.mRampFrames;
        if (frame < ramp)
            ratio = brakeStep(w, (f32)(f64)frame);
        else
            ratio = brakeStep(w, (f32)(f64)(s32)ramp);
    } else {
        u32 ramp = w->mBrake.mRampFrames;
        u32 shift = frame < ramp ? frame - 1 : ramp - 1;
        ratio = brakeStep(w, (f32)(f64)(s32)shiftLeft(1, shift));
    }
    moveTail(camera, w, ratio, BRAKING);
    return result;
}
} // namespace event_camera_uniform
using namespace event_camera_uniform;

/* WWHD glue (tools/verify/autoglue.py)
ext 02530418 u32 cameraUniformGetRequiredInt(dCamera_c*,GuestWord*,const char*)
ext 02530634 u32 cameraUniformGetInt(dCamera_c*,GuestWord*,const char*,s32)
ext 0253072C u32 cameraUniformGetFloat(dCamera_c*,GuestFloat*,const char*,f32)
ext 0253086C u32 cameraUniformGetXyz(dCamera_c*,cXyz*,const char*,cXyz*)
ext 02530998 u32 cameraUniformGetString(dCamera_c*,be<char>*,const char*,const char*)
ext 02530B84 fopAc_ac_c* cameraUniformGetActor(dCamera_c*,const char*)
ext 0250242C void cameraUniformRelationalPos(dCamera_c*,cXyz*,fopAc_ac_c*,cXyz*)
ext 0201ADE0 void cameraUniformSubtract(cXyz*,cXyz*,cXyz*)
ext 0201AD78 void cameraUniformAdd(cXyz*,cXyz*,cXyz*)
ext 0201AE48 void cameraUniformScale(cXyz*,cXyz*,f32)
ext 028E8D88 void cameraUniformPSVECAdd(cXyz*,cXyz*,cXyz*)
ext 020072E0 void cameraUniformGlobeVal(cSGlobe_l*,cXyz*)
ext 020073AC void cameraUniformGlobeXyz(cSGlobe_l*,cXyz*)
ext 02007324 void cameraUniformGlobeCtor(cSGlobe_l*,cXyz*)
ext 020068B0 void cameraUniformAngleSubtract(cSAngle_l*,cSAngle_l*,cSAngle_l*)
ext 02006894 void cameraUniformAngleAdd(cSAngle_l*,cSAngle_l*,cSAngle_l*)
ext 0200693C void cameraUniformAngleScale(cSAngle_l*,cSAngle_l*,f32)
ext 0200658C cSAngle_l* cameraUniformAngleShortCtor(cSAngle_l*,s32)
ext 024F8D5C void cameraUniformPositionOf(dCamera_c*,cXyz*,fopAc_ac_c*)
ext 024F8F30 void cameraUniformDirectionOf(dCamera_c*,cSAngle_l*,fopAc_ac_c*)
ext 024F8F3C void cameraUniformAttentionPos(dCamera_c*,cXyz*,fopAc_ac_c*)
ext 028F4384 f32 cameraUniformSqrt(f32)
ext 028E8DD0 f32 cameraUniformSquareMag(cXyz*)
ext 024FCBE8 bool cameraUniformLineBGCheck(dCamera_c*,cXyz*,cXyz*,u32)
ext 024FB608 bool cameraUniformLineBGCheckCross(dCamera_c*,cXyz*,cXyz*,cXyz*,u32)
ext 02007400 cSGlobe_l* cameraUniformGlobeInvert(cSGlobe_l*)
ext 024F7A40 void cameraUniformRotateY(cXyz*,cXyz*,cSAngle_l*)
ext 02007248 void cameraUniformGlobeAnglesCtor(cSGlobe_l*,f32,cSAngle_l*,cSAngle_l*)
ext 020065EC void cameraUniformShortMinusAngle(cSAngle_l*,s32,cSAngle_l*)
ext 020066C0 cSAngle_l* cameraUniformAngleFloatCtor(cSAngle_l*,f32)
ext 020068CC void cameraUniformAngleAddAssign(cSAngle_l*,cSAngle_l*)
ext 025C0B18 void cameraUniformSplineInit(u8*,u32,u32)
ext 02006720 f32 cameraUniformAngleDegree(cSAngle_l*)
ext 02543F10 s32 cameraUniformEventIndex(void*,const char*,s32)
ext 02544044 StageEventView* cameraUniformEventData(void*,s32)
ext 0200EE00 void cameraUniformAddCalcPos(cXyz*,cXyz*,f32,f32,f32)
ext 025C0C80 void cameraUniformSplineStep(u8*)
ext 025C0F30 f32 cameraUniformSplineCalc(u8*,GuestFloat*)
data asokoName 1004CED4
data transTimerName 1004CECC
data transCurveName 1004CF38
data transEyeName 1004CEC8
data transCenterName 1004CEDC
data transFovyName 1004CEE4
data transStartEyeName 1004CF44
data transStartCenterName 1004CEFC
data transStartFovyName 1004CF14
data transBankName 1004CEEC
data transStartBankName 1004CF20
data transTypeName 1004CF2C
data transMaskName 1004CF50
data transMaskDefault 1004CEF4
data transActorName 1004CF5C
data transCushionName 1004CEC0
data tactWindowName 1004CF08
data brakeTimerName 1004D61C
data brakingPointName 1004D680
data brakeTypeName 1004D650
data acceleTimerName 1004D6C0
data acceleRampName 1004D6E8
data acceleTypeName 1004D724
data brakeName0 1004D618
data brakeName1 1004D624
data brakeName2 1004D62C
data brakeName3 1004D690
data brakeName4 1004D644
data brakeName5 1004D65C
data brakeName6 1004D634
data brakeName7 1004D668
data brakeName8 1004D674
data brakeName9 1004D69C
data brakeName10 1004D63C
data brakeName11 1004D6A8
data brakeName12 1004D610
data acceleName0 1004D6BC
data acceleName1 1004D6C8
data acceleName2 1004D6D0
data acceleName3 1004D730
data acceleName4 1004D6F4
data acceleName5 1004D700
data acceleName6 1004D6D8
data acceleName7 1004D70C
data acceleName8 1004D718
data acceleName9 1004D73C
data acceleName10 1004D6E0
data acceleName11 1004D748
data acceleName12 1004D6B4
data sinCosTable 104A44F8
data splineCurve 101D626C
*/
