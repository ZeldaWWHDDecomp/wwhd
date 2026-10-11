#include "d/d_ev_camera_2.h"
namespace event_camera_fixed {
static cXyz fixedPositionZero;
static u32 fixedPositionZeroGuard;
static s32 fixedPositionDefaultTimer;
static EventString posEyeName;
static EventString posCtrGapName;
static EventString posFovyName;
static EventString posCtrCusName;
static EventString posRadiusName;
static EventString posStartRadiusName;
static EventString posBankName;
static EventString posRelUseMaskName;
static EventString posRelUseMaskDefault;
static EventString posTimerName;
static EventString posTargetName;
static EventString posTargetDefault;
static EventString posRelActorName;
static EventString frameEyeName;
static EventString frameCenterName;
static EventString frameBasePosName;
static EventString frameFovyName;
static EventString frameBankName;
static EventString frameTimerName;
static EventString frameRelUseMaskName;
static EventString frameRelUseMaskDefault;
static EventString frameRelActorName;
static EventString stokerEyeGapName;
static EventString stokerCtrGapName;
static EventString stokerFovyName;
static EventString stokerBankName;
static EventString stokerTimerName;
static EventString stokerName;
static EventString stokerDefault;
static EventString stokerTargetName;
static EventString stokerTargetDefault;

static void copyPoint(cXyz* destination, const cXyz* source) {
    // HD: keep the interleaved integer copies and NaN payloads.
    auto* out = reinterpret_cast<VectorWords*>(destination);
    auto* in = reinterpret_cast<const VectorWords*>(source);
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}
static void SkipSmoother(dCamera_c* camera) {
    camera->m102 = 1;
    camera->m101 = 1;
    camera->m100 = 1;
}
static cXyz* localZero() {
    if (fixedPositionZeroGuard == 0) {
        f32 x = cXyz::Zero.x;
        f32 y = cXyz::Zero.y;
        fixedPositionZero.x = x;
        f32 z = cXyz::Zero.z;
        fixedPositionZero.y = y;
        fixedPositionZero.z = z;
        fixedPositionZeroGuard = 1;
    }
    return &fixedPositionZero;
}
static f32 vectorLength(cXyz* vector) {
    return cameraFixedSqrt(cameraFixedSquareMag(vector));
}
static void mirrorX(cXyz* vector) {
    vector->x = -static_cast<f32>(vector->x);
}
/* inlined into fixedPositionEvCamera, fixedFrameEvCamera, stokerEvCamera */
static void setBank(dCamera_c* camera, f32 degrees) {
    cSAngle_l angle;
    auto* result = cameraFixedAngleCtor(
        &angle, static_cast<s32>(static_cast<s16>(gabi::ftoi(degrees * 182.04444885253906f))));
    u32 flags = camera->mEventFlags;
    s16 bank = *result;
    camera->mEventFlags = flags | BANK_SET;
    camera->mViewCache.mBank = bank;
}
/* inlined into fixedFrameEvCamera */
static void setView(dCamera_c* camera, cXyz* center, cXyz* eye) {
    cXyz direction;
    copyPoint(&camera->mViewCache.mCenter, center);
    copyPoint(&camera->mViewCache.mEye, eye);
    cameraFixedSubtract(&camera->mViewCache.mEye, &direction, &camera->mViewCache.mCenter);
    cSGlobe_Val(&camera->mViewCache.mDirection, &direction);
}
/* inlined into fixedFrameEvCamera */
static void relativeFacing(dCamera_c* camera, FixedFrameWork* work, fopAc_ac_c* actor, cXyz* offset,
                           cXyz* destination) {
    cXyz position;
    cXyz direction;
    cSGlobe_l globe;
    AnglePair angles;
    cXyz relative;
    cameraFixedPositionOf(camera, &position, actor);
    cameraFixedSubtract(&camera->mEye, &direction, &position);
    cameraFixedGlobeCtor(&globe, &direction);
    cameraFixedDirectionOf(camera, &angles.mDirection, work->mpRelActor.get());
    cameraFixedAngleSubtract(&globe.mU, &angles.mDifference, &angles.mDirection);
    if (angles.mDifference < zeroAngle())
        mirrorX(offset);
    cameraFixedRelationalPos(camera, &relative, work->mpRelActor.get(), offset);
    copyPoint(destination, &relative);
}
/* inlined into fixedFrameEvCamera */
static void relativePlayer(dCamera_c* camera, FixedFrameWork* work, fopAc_ac_c* actor, cXyz* offset,
                           cXyz* destination) {
    cXyz target;
    cXyz player;
    cXyz difference;
    cameraFixedRelationalPos(camera, &target, actor, offset);
    cameraFixedPositionOf(camera, &player, camera->mpPlayerActor.get());
    cameraFixedSubtract(&target, &difference, &player);
    f32 firstDistance = vectorLength(&difference);
    mirrorX(offset);
    cameraFixedRelationalPos(camera, &player, work->mpRelActor.get(), offset);
    copyPoint(&target, &player);
    cameraFixedPositionOf(camera, &difference, camera->mpPlayerActor.get());
    cameraFixedSubtract(&target, &player, &difference);
    f32 secondDistance = vectorLength(&player);
    if (firstDistance > secondDistance)
        mirrorX(offset);
    cameraFixedRelationalPos(camera, &player, work->mpRelActor.get(), offset);
    copyPoint(destination, &player);
}
/* inlined into fixedFrameEvCamera */
static void relativeAttention(dCamera_c* camera, fopAc_ac_c* actor, cXyz* offset,
                              cXyz* destination) {
    cXyz attention;
    cXyz relative;
    cameraFixedAttentionPos(camera, &attention, actor);
    cameraFixedAdd(&attention, &relative, offset);
    copyPoint(destination, &relative);
}
/* inlined into fixedFrameEvCamera */
static void relativeOffset(dCamera_c* camera, fopAc_ac_c* actor, cXyz* offset, cXyz* destination) {
    cXyz relative;
    cameraFixedRelationalPos(camera, &relative, actor, offset);
    copyPoint(destination, &relative);
}

/* 02531010 */
u32 fixedPositionEvCamera(dCamera_c* camera) {
    auto* zero = localZero();
    auto* work = reinterpret_cast<FixedPositionWork*>(camera->mWork);
    u32 done = 1;
    if (camera->m11C == 0) {
        cXyz fallback;
        cXyz eye;
        cXyz relative;
        f32 eyeZ = camera->mEye.z;
        f32 eyeX = camera->mEye.x;
        f32 eyeY = camera->mEye.y;
        fallback.z = eyeZ;
        fallback.x = eyeX;
        fallback.y = eyeY;
        cameraFixedGetXyz(camera, &eye, posEyeName.text, &fallback);
        f32 zeroX = zero->x;
        f32 zeroZ = zero->z;
        f32 zeroY = zero->y;
        fallback.z = zeroZ;
        fallback.y = zeroY;
        fallback.x = zeroX;
        cameraFixedGetXyz(camera, &work->mCtrGap, posCtrGapName.text, &fallback);
        cameraFixedGetFloat(camera, &work->mFovy, posFovyName.text, camera->mFovy);
        cameraFixedGetFloat(camera, &work->mCtrCus, posCtrCusName.text, 1.0f);
        cameraFixedGetFloat(camera, &work->mRadius, posRadiusName.text, 100000.0f);
        cameraFixedGetFloat(camera, &work->mStartRadius, posStartRadiusName.text, work->mRadius);
        work->mBankSet =
            static_cast<u8>(cameraFixedGetFloat(camera, &work->mBank, posBankName.text, 0.0f));
        cameraFixedGetString(camera, work->mRelUseMask, posRelUseMaskName.text,
                             posRelUseMaskDefault.text);
        work->mTimerSet = static_cast<u8>(
            cameraFixedGetInt(camera, &work->mTimer, posTimerName.text, fixedPositionDefaultTimer));
        auto* target =
            cameraFixedGetActorDefault(camera, posTargetName.text, posTargetDefault.text);
        work->mpTarget = target;
        if (!target)
            return 1;
        work->mTargetId = target ? static_cast<u32>(target->mBsPcId) : 0xFFFFFFFF;
        auto* actor = cameraFixedGetActor(camera, posRelActorName.text);
        work->mpRelActor = actor;
        if (actor && work->mRelUseMask[0] != '-') {
            cameraFixedRelationalPos(camera, &relative, actor, &eye);
            copyPoint(&work->mEye, &relative);
        } else {
            copyPoint(&work->mEye, &eye);
        }
        copyPoint(&work->mCenter, &camera->mCenter);
        SkipSmoother(camera);
    }
    GuestWord id;
    u32 processId = work->mTargetId;
    id = processId;
    if (processId == 0xFFFFFFFF)
        return 1;
    if (!fopAcIt_Judge(JUDGE_BY_ID, &id))
        return 1;
    cXyz target;
    cXyz difference;
    cameraFixedRelationalPos(camera, &target, work->mpTarget.get(), &work->mCtrGap);
    copyPoint(&work->mCenter, &target);
    cameraFixedSubtract(&work->mCenter, &difference, &camera->mViewCache.mCenter);
    cameraFixedScale(&difference, &target, work->mCtrCus);
    cameraFixedPSVECAdd(&camera->mViewCache.mCenter, &target, &camera->mViewCache.mCenter);
    copyPoint(&camera->mViewCache.mEye, &work->mEye);
    cameraFixedSubtract(&camera->mViewCache.mEye, &difference, &camera->mViewCache.mCenter);
    cSGlobe_Val(&camera->mViewCache.mDirection, &difference);
    f32 radius = work->mRadius;
    if (work->mTimerSet != 0 && camera->m11C < work->mTimer) {
        f32 start = work->mStartRadius;
        // HD: unsigned timer and signed duration are converted through double.
        f32 ratio = static_cast<f32>(static_cast<f64>(static_cast<u32>(camera->m11C))) /
                    static_cast<f32>(static_cast<f64>(static_cast<s32>(work->mTimer)));
        radius = gabi::fmadds(radius - start, ratio, start);
        done = 0;
    }
    if (camera->mViewCache.mDirection.mRadius > radius) {
        cXyz direction;
        cXyz eye;
        camera->mViewCache.mDirection.mRadius = radius;
        cameraFixedGlobeXyz(&camera->mViewCache.mDirection, &direction);
        cameraFixedAdd(&camera->mViewCache.mCenter, &eye, &direction);
        copyPoint(&camera->mViewCache.mEye, &eye);
    }
    camera->mViewCache.mFovy = work->mFovy;
    if (work->mBankSet != 0)
        setBank(camera, work->mBank);
    if (done)
        SkipSmoother(camera);
    return done;
}

/* 025314BC */
u32 fixedFrameEvCamera(dCamera_c* camera) {
    auto* work = reinterpret_cast<FixedFrameWork*>(camera->mWork);
    if (camera->m11C == 0) {
        cXyz eye;
        cXyz fallback;
        cXyz center;
        f32 x = camera->mEye.x;
        f32 y = camera->mEye.y;
        fallback.x = x;
        f32 z = camera->mEye.z;
        fallback.y = y;
        fallback.z = z;
        cameraFixedGetXyz(camera, &eye, frameEyeName.text, &fallback);
        x = camera->mCenter.x;
        y = camera->mCenter.y;
        fallback.x = x;
        z = camera->mCenter.z;
        fallback.y = y;
        fallback.z = z;
        cameraFixedGetXyz(camera, &center, frameCenterName.text, &fallback);
        x = cXyz::Zero.x;
        y = cXyz::Zero.y;
        fallback.x = x;
        z = cXyz::Zero.z;
        fallback.y = y;
        fallback.z = z;
        cameraFixedGetXyz(camera, &work->mBasePos, frameBasePosName.text, &fallback);
        cameraFixedGetFloat(camera, &work->mFovy, frameFovyName.text, camera->mFovy);
        work->mBankSet =
            static_cast<u8>(cameraFixedGetFloat(camera, &work->mBank, frameBankName.text, 0.0f));
        work->mTimerSet =
            static_cast<u8>(cameraFixedGetInt(camera, &work->mTimer, frameTimerName.text, -1));
        cameraFixedGetString(camera, work->mRelUseMask, frameRelUseMaskName.text,
                             frameRelUseMaskDefault.text);
        auto* actor = cameraFixedGetActor(camera, frameRelActorName.text);
        work->mpRelActor = actor;
        if (!actor) {
            copyPoint(&work->mCenter, &center);
            copyPoint(&work->mEye, &eye);
        } else {
            char centerMode = work->mRelUseMask[0];
            switch (centerMode) {
            case 'o':
                relativeOffset(camera, actor, &center, &work->mCenter);
                break;
            case 'n':
                relativeFacing(camera, work, actor, &center, &work->mCenter);
                break;
            case 'p':
                relativePlayer(camera, work, actor, &center, &work->mCenter);
                break;
            case 't':
                relativeAttention(camera, actor, &center, &work->mCenter);
                break;
            default:
                copyPoint(&work->mCenter, &center);
                break;
            }
            char eyeMode = work->mRelUseMask[1];
            actor = work->mpRelActor.get();
            if (eyeMode == 'o') {
                relativeOffset(camera, actor, &eye, &work->mEye);
            } else if (actor && eyeMode == 'r') {
                if (camera->m080 & 1)
                    mirrorX(&eye);
                relativeOffset(camera, work->mpRelActor.get(), &eye, &work->mEye);
                if (cameraFixedLineBGCheck(camera, &work->mCenter, &work->mEye, LINE_CHECK_FLAGS))
                    mirrorX(&eye);
                relativeOffset(camera, work->mpRelActor.get(), &eye, &work->mEye);
            } else if (eyeMode == 'n') {
                relativeFacing(camera, work, actor, &eye, &work->mEye);
                if (cameraFixedLineBGCheck(camera, &work->mCenter, &work->mEye, LINE_CHECK_FLAGS))
                    mirrorX(&eye);
            } else if (eyeMode == 'p') {
                relativePlayer(camera, work, actor, &eye, &work->mEye);
            } else if (eyeMode == 't') {
                relativeAttention(camera, actor, &eye, &work->mEye);
            } else {
                copyPoint(&work->mEye, &eye);
            }
        }
        SkipSmoother(camera);
    }
    setView(camera, &work->mCenter, &work->mEye);
    camera->mViewCache.mFovy = work->mFovy;
    if (work->mBankSet != 0)
        setBank(camera, work->mBank);
    if (work->mTimerSet != 0 && camera->m11C < work->mTimer)
        return 0;
    return 1;
}

/* inlined into stokerEvCamera */
static void stokerPosition(dCamera_c* camera, cSGlobe_l* globe, cXyz* gap, gptr<fopAc_ac_c>* actor,
                           cXyz* destination) {
    cSAngle_l sum;
    cSAngle_l pitch;
    cSAngle_l yaw;
    cXyz attention;
    cXyz direction;
    cXyz relative;
    cSGlobe_Val(globe, gap);
    cameraFixedAngleAdd(&globe->mV, &sum, actor->get()->shape_angle.x);
    auto* result = cameraFixedAngleCtor(&pitch, sum);
    globe->mV = *result;
    cameraFixedAngleAdd(&globe->mU, &sum, actor->get()->shape_angle.y);
    result = cameraFixedAngleCtor(&yaw, sum);
    globe->mU = *result;
    cameraFixedAttentionPos(camera, &attention, actor->get());
    cameraFixedGlobeXyz(globe, &direction);
    cameraFixedAdd(&attention, &relative, &direction);
    copyPoint(destination, &relative);
}

/* 0253B7E4 */
u32 stokerEvCamera(dCamera_c* camera) {
    auto* work = reinterpret_cast<StokerWork*>(camera->mWork);
    if (camera->m11C == 0) {
        cXyz fallback;
        f32 zeroX = cXyz::Zero.x;
        f32 zeroY = cXyz::Zero.y;
        fallback.x = zeroX;
        f32 zeroZ = cXyz::Zero.z;
        fallback.y = zeroY;
        fallback.z = zeroZ;
        cameraFixedGetXyz(camera, &work->mEyeGap, stokerEyeGapName.text, &fallback);
        fallback.x = zeroX;
        fallback.y = zeroY;
        fallback.z = zeroZ;
        cameraFixedGetXyz(camera, &work->mCtrGap, stokerCtrGapName.text, &fallback);
        cameraFixedGetFloat(camera, &work->mFovy, stokerFovyName.text, camera->mFovy);
        work->mBankSet =
            static_cast<u8>(cameraFixedGetFloat(camera, &work->mBank, stokerBankName.text, 0.0f));
        work->mTimerSet =
            static_cast<u8>(cameraFixedGetInt(camera, &work->mTimer, stokerTimerName.text, -1));
        work->mpStoker = cameraFixedGetActorDefault(camera, stokerName.text, stokerDefault.text);
        auto* target =
            cameraFixedGetActorDefault(camera, stokerTargetName.text, stokerTargetDefault.text);
        auto* actor = work->mpStoker.get();
        work->mpTarget = target;
        if (!actor || !target)
            return 1;
        auto* targetAgain = work->mpTarget.get();
        work->mStokerId = actor ? static_cast<u32>(actor->mBsPcId) : 0xFFFFFFFF;
        work->mTargetId = targetAgain ? static_cast<u32>(targetAgain->mBsPcId) : 0xFFFFFFFF;
        SkipSmoother(camera);
    }
    cSGlobe_l globe;
    cameraFixedGlobeDefaultCtor(&globe);
    if (work->mpTarget) {
        GuestWord id;
        u32 processId = work->mTargetId;
        id = processId;
        if (processId == 0xFFFFFFFF)
            return 1;
        if (!fopAcIt_Judge(JUDGE_BY_ID, &id))
            return 1;
        stokerPosition(camera, &globe, &work->mCtrGap, &work->mpTarget,
                       &camera->mViewCache.mCenter);
    }
    if (work->mpStoker) {
        GuestWord id;
        u32 processId = work->mStokerId;
        id = processId;
        if (processId == 0xFFFFFFFF)
            return 1;
        if (!fopAcIt_Judge(JUDGE_BY_ID, &id))
            return 1;
        stokerPosition(camera, &globe, &work->mEyeGap, &work->mpStoker, &camera->mViewCache.mEye);
    }
    {
        cXyz direction;
        cameraFixedSubtract(&camera->mViewCache.mEye, &direction, &camera->mViewCache.mCenter);
        cSGlobe_Val(&camera->mViewCache.mDirection, &direction);
    }
    camera->mViewCache.mFovy = work->mFovy;
    if (work->mBankSet != 0)
        setBank(camera, work->mBank);
    if (work->mTimerSet != 0 && camera->m11C < work->mTimer)
        return 0;
    return 1;
}
} // namespace event_camera_fixed
using namespace event_camera_fixed;

/* WWHD glue (tools/verify/autoglue.py)
data fixedPositionZero 1047586C
data fixedPositionZeroGuard 104758A8
data fixedPositionDefaultTimer 101D61FC
data posEyeName 1004CDF4
data posCtrGapName 1004CE04
data posFovyName 1004CE0C
data posCtrCusName 1004CE14
data posRadiusName 1004CE1C
data posStartRadiusName 1004CE34
data posBankName 1004CE24
data posRelUseMaskName 1004CE40
data posRelUseMaskDefault 1004CDF8
data posTimerName 1004CDFC
data posTargetName 1004CE2C
data posTargetDefault 1004CDEC
data posRelActorName 1004CE4C
data frameEyeName 1004CE64
data frameCenterName 1004CE70
data frameBasePosName 1004CE5C
data frameFovyName 1004CE78
data frameBankName 1004CE80
data frameTimerName 1004CE68
data frameRelUseMaskName 1004CE8C
data frameRelUseMaskDefault 1004CE88
data frameRelActorName 1004CE98
data stokerEyeGapName 1004D5D4
data stokerCtrGapName 1004D5DC
data stokerFovyName 1004D5E4
data stokerBankName 1004D5EC
data stokerTimerName 1004D5CC
data stokerName 1004D5F4
data stokerDefault 1004D604
data stokerTargetName 1004D5FC
data stokerTargetDefault 1004D5C4
ext 02530634 u32 cameraFixedGetInt(dCamera_c*, GuestWord*, const char*, s32)
ext 0253072C u32 cameraFixedGetFloat(dCamera_c*, be<f32>*, const char*, f32)
ext 0253086C u32 cameraFixedGetXyz(dCamera_c*, cXyz*, const char*, cXyz*)
ext 02530998 u32 cameraFixedGetString(dCamera_c*, be<char>*, const char*, const char*)
ext 02530B84 fopAc_ac_c* cameraFixedGetActor(dCamera_c*, const char*)
ext 02530CB4 fopAc_ac_c* cameraFixedGetActorDefault(dCamera_c*, const char*, const char*)
ext 0250242C void cameraFixedRelationalPos(dCamera_c*, cXyz*, fopAc_ac_c*, cXyz*)
ext 024F8D5C void cameraFixedPositionOf(dCamera_c*, cXyz*, fopAc_ac_c*)
ext 024F8F3C void cameraFixedAttentionPos(dCamera_c*, cXyz*, fopAc_ac_c*)
ext 024F8F30 void cameraFixedDirectionOf(dCamera_c*, cSAngle_l*, fopAc_ac_c*)
ext 0201ADE0 void cameraFixedSubtract(cXyz*, cXyz*, cXyz*)
ext 0201AD78 void cameraFixedAdd(cXyz*, cXyz*, cXyz*)
ext 0201AE48 void cameraFixedScale(cXyz*, cXyz*, f32)
ext 020073AC void cameraFixedGlobeXyz(cSGlobe_l*, cXyz*)
ext 02007324 void cameraFixedGlobeCtor(cSGlobe_l*, cXyz*)
ext 02007100 void cameraFixedGlobeDefaultCtor(cSGlobe_l*)
ext 020068B0 void cameraFixedAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 020068F4 void cameraFixedAngleAdd(cSAngle_l*, cSAngle_l*, s32)
ext 0200658C cSAngle_l* cameraFixedAngleCtor(cSAngle_l*, s32)
ext 028E8DD0 f32 cameraFixedSquareMag(cXyz*)
ext 028F4384 f32 cameraFixedSqrt(f32)
ext 024FCBE8 bool cameraFixedLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32)
ext 028E8D88 void cameraFixedPSVECAdd(cXyz*, cXyz*, cXyz*)
*/
