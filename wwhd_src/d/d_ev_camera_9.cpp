#include "d/d_ev_camera_9.h"
namespace event_camera_rolling {
static EventString rollEyeName;
static EventString rollCenterName;
static EventString rollCtrCusName;
static EventString rollTransTypeName;
static EventString rollFovyName;
static EventString rollBankName;
static EventString rollRollName;
static EventString rollRadiusAddName;
static EventString rollLatitudeName;
static EventString rollTimerName;
static EventString rollRelUseMaskName;
static EventString rollRelUseMaskDefault;
static EventString rollRelActorName;
static f32 degreesToAngle;
static EventString windBirdFlyDistName;
static EventString windTorishitaName;
static EventString windStopDistName;
static EventString windUpCountName;
static EventString windSideFlagName;
static EventString windFollowCushionName;
static EventString windNearFovyName;
static EventString windFarFovyName;
static EventString windFovyCushionName;
static EventString windTypeName;

static void copyPoint(cXyz* destination, const cXyz* source) {
    // HD: preserve interleaved integer loads/stores and NaN payloads.
    auto* out = reinterpret_cast<VectorWords*>(destination);
    auto* in = reinterpret_cast<const VectorWords*>(source);
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}
static void setPoint(cXyz* point, f32 x, f32 y, f32 z) {
    point->x = x;
    point->y = y;
    point->z = z;
}
static void mirrorX(cXyz* point) {
    point->x = -static_cast<f32>(point->x);
}
static f32 vectorLength(cXyz* vector) {
    return cameraRollingSqrt(cameraRollingSquareMag(vector));
}
static void SkipSmoother(dCamera_c* camera) {
    camera->m102 = 1;
    camera->m101 = 1;
    camera->m100 = 1;
}
/* inlined into rollingEvCamera */
static void relativeOffset(dCamera_c* camera, fopAc_ac_c* actor, cXyz* offset, cXyz* destination) {
    cXyz relative;
    cameraRollingRelationalPos(camera, &relative, actor, offset);
    copyPoint(destination, &relative);
}
/* inlined into rollingEvCamera */
static void relativeFacing(dCamera_c* camera, gptr<fopAc_ac_c>* actor, cXyz* offset,
                           cXyz* destination) {
    cXyz position;
    cXyz difference;
    cSGlobe_l globe;
    AnglePair angles;
    cameraRollingPositionOf(camera, &position, actor->get());
    cameraRollingSubtract(&camera->mEye, &difference, &position);
    cameraRollingGlobeCtor(&globe, &difference);
    cameraRollingDirectionOf(camera, &angles.m0, actor->get());
    cameraRollingAngleSubtract(&globe.mU, &angles.m1, &angles.m0);
    if (angles.m1 < zeroAngle())
        mirrorX(offset);
    cameraRollingRelationalPos(camera, &position, actor->get(), offset);
    copyPoint(destination, &position);
}
/* inlined into rollingEvCamera */
static void relativePlayer(dCamera_c* camera, gptr<fopAc_ac_c>* actor, cXyz* offset,
                           cXyz* destination) {
    cXyz target;
    cXyz player;
    cXyz difference;
    cameraRollingRelationalPos(camera, &target, actor->get(), offset);
    cameraRollingPositionOf(camera, &player, camera->mpPlayerActor.get());
    cameraRollingSubtract(&target, &difference, &player);
    f32 firstDistance = vectorLength(&difference);
    mirrorX(offset);
    cameraRollingRelationalPos(camera, &player, actor->get(), offset);
    copyPoint(&target, &player);
    cameraRollingPositionOf(camera, &difference, camera->mpPlayerActor.get());
    cameraRollingSubtract(&target, &player, &difference);
    f32 secondDistance = vectorLength(&player);
    if (firstDistance > secondDistance)
        mirrorX(offset);
    cameraRollingRelationalPos(camera, &player, actor->get(), offset);
    copyPoint(destination, &player);
}
static void relativeCenter(dCamera_c* camera, RollingWork* work, char mode) {
    if (mode == 'o')
        relativeOffset(camera, work->mpRelActor.get(), &work->mCenterOffset, &work->mCenter);
    else if (mode == 'n')
        relativeFacing(camera, &work->mpRelActor, &work->mCenterOffset, &work->mCenter);
    else if (mode == 'p')
        relativePlayer(camera, &work->mpRelActor, &work->mCenterOffset, &work->mCenter);
}

/* 025369BC */
u32 rollingEvCamera(dCamera_c* camera) {
    auto* work = reinterpret_cast<RollingWork*>(camera->mWork);
    if (camera->m11C == 0) {
        cXyz fallback;
        f32 x = camera->mEye.x;
        f32 y = camera->mEye.y;
        fallback.x = x;
        f32 z = camera->mEye.z;
        fallback.y = y;
        fallback.z = z;
        cameraRollingGetXyz(camera, &work->mEyeOffset, rollEyeName.text, &fallback);
        x = camera->mCenter.x;
        y = camera->mCenter.y;
        fallback.x = x;
        z = camera->mCenter.z;
        fallback.y = y;
        fallback.z = z;
        cameraRollingGetXyz(camera, &work->mCenterOffset, rollCenterName.text, &fallback);
        cameraRollingGetFloat(camera, &work->mCtrCus, rollCtrCusName.text, 1.0f);
        cameraRollingGetInt(camera, &work->mTransType, rollTransTypeName.text, 0);
        cameraRollingGetFloat(camera, &work->mFovy, rollFovyName.text, camera->mFovy);
        work->mBankSet =
            static_cast<u8>(cameraRollingGetFloat(camera, &work->mBank, rollBankName.text, 0.0f));
        cameraRollingGetFloat(camera, &work->mRoll, rollRollName.text, 2.0f);
        cameraRollingGetFloat(camera, &work->mRadiusAdd, rollRadiusAddName.text, 0.0f);
        cSGlobe_l globe;
        cameraRollingSubtract(&work->mEyeOffset, &fallback, &work->mCenterOffset);
        cameraRollingGlobeCtor(&globe, &fallback);
        f32 latitude = cameraRollingAngleDegree(&globe.mV);
        cameraRollingGetFloat(camera, &work->mLatitude, rollLatitudeName.text, latitude);
        work->mTimerSet =
            static_cast<u8>(cameraRollingGetInt(camera, &work->mTimer, rollTimerName.text, -1));
        cameraRollingGetString(camera, work->mRelUseMask, rollRelUseMaskName.text,
                               rollRelUseMaskDefault.text);
        auto* actor = cameraRollingGetActor(camera, rollRelActorName.text);
        work->mpRelActor = actor;
        if (!actor) {
            copyPoint(&work->mCenter, &work->mCenterOffset);
            copyPoint(&work->mEye, &work->mEyeOffset);
        } else {
            char centerMode = work->mRelUseMask[0];
            if (centerMode == 'o' || centerMode == 'n' || centerMode == 'p')
                relativeCenter(camera, work, centerMode);
            else
                copyPoint(&work->mCenter, &work->mCenterOffset);
            char eyeMode = work->mRelUseMask[1];
            if (eyeMode == 'o') {
                relativeOffset(camera, work->mpRelActor.get(), &work->mEyeOffset, &work->mEye);
            } else if (eyeMode == 'r') {
                if (camera->m080 & 1)
                    mirrorX(&work->mEyeOffset);
                relativeOffset(camera, work->mpRelActor.get(), &work->mEyeOffset, &work->mEye);
                if (cameraRollingLineBGCheck(camera, &work->mCenter, &work->mEye, 0x8F))
                    mirrorX(&work->mEyeOffset);
                relativeOffset(camera, work->mpRelActor.get(), &work->mEyeOffset, &work->mEye);
            } else if (eyeMode == 'n') {
                relativeFacing(camera, &work->mpRelActor, &work->mEyeOffset, &work->mEye);
            } else if (eyeMode == 'p') {
                relativePlayer(camera, &work->mpRelActor, &work->mEyeOffset, &work->mEye);
            } else {
                copyPoint(&work->mEye, &work->mEyeOffset);
            }
        }
        SkipSmoother(camera);
    }
    u32 translationType = work->mTransType;
    if ((translationType == 1 || translationType == 2) && work->mpRelActor)
        relativeCenter(camera, work, work->mRelUseMask[0]);
    cXyz difference;
    cXyz scaled;
    cameraRollingSubtract(&work->mCenter, &difference, &camera->mViewCache.mCenter);
    cameraRollingScale(&difference, &scaled, work->mCtrCus);
    cameraRollingPSVECAdd(&camera->mViewCache.mCenter, &scaled, &camera->mViewCache.mCenter);
    cameraRollingSubtract(&work->mEye, &difference, &work->mCenter);
    cSGlobe_Val(&camera->mViewCache.mDirection, &difference);
    AngleFour angles;
    AngleFour temporary;
    if (work->mTransType == 2) {
        auto* latitude = cameraRollingAngleFloatCtor(&temporary.m0, work->mLatitude);
        auto* result = cameraRollingAngleShortCtor(&temporary.m1, *latitude);
        camera->mViewCache.mDirection.mV = *result;
    }
    f32 timer = static_cast<f32>(static_cast<f64>(static_cast<u32>(camera->m11C)));
    auto* roll = cameraRollingAngleFloatCtor(&angles.m0, timer * work->mRoll);
    cameraRollingAngleAdd(&camera->mViewCache.mDirection.mU, &angles.m1, roll);
    auto* result = cameraRollingAngleShortCtor(&angles.m2, angles.m1);
    f32 radiusTimer = static_cast<f32>(static_cast<f64>(static_cast<u32>(camera->m11C)));
    camera->mViewCache.mDirection.mU = *result;
    camera->mViewCache.mDirection.mRadius =
        gabi::fmadds(radiusTimer, work->mRadiusAdd, camera->mViewCache.mDirection.mRadius);
    cameraRollingGlobeXyz(&camera->mViewCache.mDirection, &scaled);
    cameraRollingAdd(&camera->mViewCache.mCenter, &difference, &scaled);
    copyPoint(&camera->mViewCache.mEye, &difference);
    camera->mViewCache.mFovy = work->mFovy;
    if (work->mBankSet != 0) {
        result = cameraRollingAngleShortCtor(
            &angles.m3, static_cast<s16>(gabi::ftoi(work->mBank * degreesToAngle)));
        u32 flags = camera->mEventFlags;
        s16 bank = *result;
        camera->mEventFlags = flags | 0x400;
        camera->mViewCache.mBank = bank;
    }
    if (work->mTimerSet != 0 && camera->m11C < work->mTimer)
        return 0;
    return 1;
}

static void windBlurShock(dCamera_c* camera, cXyz* temporary) {
    cameraRollingResetBlure(camera, 0);
    cameraRollingBlureTimer(camera, 110);
    cameraRollingBlureAlpha(camera, 0.6f);
    cameraRollingBlureScale(camera, 0.99f);
    auto* vibration = dComIfGp_getVibration();
    setPoint(temporary, 0.0f, 1.0f, 0.0f);
    vibration->StartShock(7, 0x20, temporary);
}
static void relativeToPlayer(dCamera_c* camera, cXyz* temporary, cXyz* offset, cXyz* destination) {
    cameraRollingRelationalPos(camera, temporary, camera->mpPlayerActor.get(), offset);
    copyPoint(destination, temporary);
}

/* 0253775C */
u32 windDirectionEvCamera(dCamera_c* camera) {
    auto* work = reinterpret_cast<WindWork*>(camera->mWork);
    SideOffsets offsets;
    auto* points = offsets.mPoints;
    setPoint(&points[0], -25.0f, 12.0f, 0.0f);
    setPoint(&points[1], -25.0f, 12.0f, 0.0f);
    setPoint(&points[2], -25.0f, 12.0f, 0.0f);
    setPoint(&points[3], -40.0f, -10.0f, -90.0f);
    setPoint(&points[4], -25.0f, 0.0f, 90.0f);
    setPoint(&points[5], -25.0f, -30.0f, 90.0f);
    f32 cushion = 0.02f;
    f32 ratio = 1.0f;
    if (camera->m11C == 0) {
        work->mState = WIND_INITIAL;
        work->mCounter = 0;
        auto* wind = reinterpret_cast<WindAnglesView*>(dKy_getEnvlight());
        cameraRollingGlobeSet(&work->mWind, 1.0f, wind->mTactWindAngleX, wind->mTactWindAngleY);
        auto* play = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
        work->mpBird = cameraRollingConvPId(&play->mEvtCtrl, play->mEvtCtrl.mPtItem);
        cameraRollingGetFloat(camera, &work->mBirdFlyDist, windBirdFlyDistName.text, 1620.0f);
        cXyz fallback;
        setPoint(&fallback, 40.0f, -95.0f, 10.0f);
        cameraRollingGetXyz(camera, &work->mTorishita, windTorishitaName.text, &fallback);
        if (cameraRollingTactWindDirection() != 0)
            mirrorX(&work->mTorishita);
        cSGlobe_l globe;
        auto* inverse = cameraRollingGlobeInvert(&work->mWind);
        cameraRollingGlobeCopyCtor(&globe, inverse);
        globe.mRadius = work->mBirdFlyDist;
        cXyz mirrored;
        cXyz attention;
        cXyz player;
        cXyz bird;
        setPoint(&mirrored, -static_cast<f32>(work->mTorishita.x), 0.0f, 0.0f);
        cameraRollingAttentionPos(camera, &attention, camera->mpPlayerActor.get());
        cameraRollingRelationalPos(camera, &player, camera->mpPlayerActor.get(), &mirrored);
        cameraRollingRelationalPos(camera, &bird, work->mpBird.get(), &work->mTorishita);
        u32 blocked = 0;
        if (cameraRollingLineBGCheck(camera, &attention, &bird, 0x7F) ||
            cameraRollingLineBGCheck(camera, &player, &bird, 0x7F))
            blocked = 1;
        cameraRollingGetFloat(camera, &work->mStopDist, windStopDistName.text, 155.0f);
        cameraRollingGetInt(camera, &work->mFrameLimit, windUpCountName.text, 8);
        cameraRollingGetInt(camera, &work->mSideFlag, windSideFlagName.text, 0);
        cameraRollingGetFloat(camera, &work->mFollowCushion, windFollowCushionName.text, cushion);
        cameraRollingGetFloat(camera, &work->mNearFovy, windNearFovyName.text, 30.0f);
        cameraRollingGetFloat(camera, &work->mFarFovy, windFarFovyName.text, 85.0f);
        cameraRollingGetFloat(camera, &work->mFovyCushion, windFovyCushionName.text, 0.05f);
        cameraRollingPositionOf(camera, &attention, work->mpBird.get());
        cameraRollingPositionOf(camera, &player, camera->mpPlayerActor.get());
        cameraRollingSubtract(&attention, &fallback, &player);
        work->mStartDistance = vectorLength(&fallback);
        camera->mViewCache.mFovy = work->mFarFovy;
        cameraRollingGetInt(camera, &work->mType, windTypeName.text, static_cast<s32>(blocked));
        if (work->mType == 1)
            work->mState = WIND_SIDE_START;
    }
    if (cameraRollingTactWindDirection() != 0)
        for (u32 i = 0; i < 6; ++i)
            mirrorX(&points[i]);
    cXyz first;
    cXyz second;
    cXyz third;
    u32 state = work->mState;
    switch (state) {
    case WIND_WAIT:
        if (static_cast<s32>(work->mCounter) > static_cast<s32>(work->mFrameLimit))
            work->mState = WIND_FOLLOW_START;
        break;
    case WIND_FOLLOW_START:
        copyPoint(&work->mCenter, &camera->mViewCache.mCenter);
        work->mState = WIND_FOLLOW;
        [[fallthrough]];
    case WIND_FOLLOW: {
        cameraRollingAttentionPos(camera, &first, work->mpBird.get());
        cameraRollingSubtract(&first, &second, &work->mCenter);
        cameraRollingScale(&second, &third, cushion);
        cameraRollingAdd(&work->mCenter, &second, &third);
        copyPoint(&work->mCenter, &second);
        LineFrame line;
        cXyz playerAttention;
        cXyz nearest;
        GuestFloat parameter;
        cameraRollingLineCtor(&line.mLine, &work->mCenter, &camera->mViewCache.mEye);
        cameraRollingAttentionPos(camera, &playerAttention, camera->mpPlayerActor.get());
        if (cameraRollingNearestPoint(&line.mLine, &playerAttention, &nearest, &parameter))
            copyPoint(&camera->mViewCache.mCenter, &nearest);
        else
            copyPoint(&camera->mViewCache.mCenter, &work->mCenter);
        s32 pad = camera->mPadId;
        if (dComIfGp_getPlayerStatus0(pad) & 0x10000) {
            cameraRollingEyePos(camera, &third, camera->mpPlayerActor.get());
            if (camera->mViewCache.mEye.y < third.y) {
                cameraRollingEyePos(camera, &second, camera->mpPlayerActor.get());
                f32 eyeY = camera->mViewCache.mEye.y;
                camera->mViewCache.mEye.y = gabi::fmadds(second.y - eyeY, 0.05f, eyeY);
            }
        }
        cameraRollingSubtract(&camera->mViewCache.mEye, &first, &camera->mViewCache.mCenter);
        cSGlobe_Val(&camera->mViewCache.mDirection, &first);
        break;
    }
    case WIND_SIDE_START:
        relativeToPlayer(camera, &first, &points[0], &camera->mViewCache.mCenter);
        relativeToPlayer(camera, &first, &points[3], &camera->mViewCache.mEye);
        cameraRollingSubtract(&camera->mViewCache.mEye, &first, &camera->mViewCache.mCenter);
        cSGlobe_Val(&camera->mViewCache.mDirection, &first);
        work->mFrameLimit = 28;
        windBlurShock(camera, &first);
        [[fallthrough]];
    case WIND_SIDE_WAIT:
        if (static_cast<s32>(work->mCounter) > static_cast<s32>(work->mFrameLimit)) {
            work->mState = WIND_SIDE_SWING;
            relativeToPlayer(camera, &first, &points[1], &work->mCenter);
            relativeToPlayer(camera, &first, &points[4], &work->mEye);
            cameraRollingSubtract(&work->mEye, &first, &work->mCenter);
            cSGlobe_Val(&work->mEnd, &first);
            auto* start = reinterpret_cast<GlobeWords*>(&work->mStart);
            auto* view = reinterpret_cast<GlobeWords*>(&camera->mViewCache.mDirection);
            start->mRadius = view->mRadius;
            work->mSwingProgress = 0.0f;
            work->mFrameLimit = 40;
            start->mAngles = view->mAngles;
            work->mSwingLength = 820.0f;
            work->mCounter = 0;
        }
        break;
    case WIND_SIDE_SWING:
        if (static_cast<s32>(work->mCounter) < static_cast<s32>(work->mFrameLimit)) {
            f32 progress = work->mSwingProgress +
                           static_cast<f32>(static_cast<f64>(static_cast<s32>(work->mCounter)));
            f32 fraction = progress / work->mSwingLength;
            work->mSwingProgress = progress;
            f32 startRadius = work->mStart.mRadius;
            camera->mViewCache.mDirection.mRadius =
                gabi::fmadds(work->mEnd.mRadius - startRadius, fraction, startRadius);
            AngleFour angles;
            AnglePair temporary;
            cameraRollingAngleSubtract(&work->mEnd.mV, &angles.m2, &work->mStart.mV);
            cameraRollingAngleScale(&angles.m2, &angles.m1, fraction);
            cameraRollingAngleAdd(&work->mStart.mV, &angles.m0, &angles.m1);
            auto* result = cameraRollingAngleShortCtor(&angles.m3, angles.m0);
            camera->mViewCache.mDirection.mV = *result;
            cameraRollingAngleSubtract(&work->mEnd.mU, &angles.m0, &work->mStart.mU);
            cameraRollingAngleScale(&angles.m0, &angles.m1, fraction);
            cameraRollingAngleAdd(&work->mStart.mU, &angles.m2, &angles.m1);
            result = cameraRollingAngleShortCtor(&temporary.m0, angles.m2);
            camera->mViewCache.mDirection.mU = *result;
            copyPoint(&camera->mViewCache.mCenter, &work->mCenter);
            cameraRollingGlobeXyz(&camera->mViewCache.mDirection, &first);
            cameraRollingAdd(&camera->mViewCache.mCenter, &second, &first);
            copyPoint(&camera->mViewCache.mEye, &second);
            break;
        }
        [[fallthrough]];
    case WIND_CLOSE_START:
        work->mFrameLimit = 0;
        work->mCounter = 0;
        work->mState = WIND_CLOSE_WAIT;
        if (static_cast<s32>(work->mFrameLimit) < 0)
            work->mState = WIND_CLOSE_FOLLOW;
        relativeToPlayer(camera, &first, &points[2], &work->mCenter);
        relativeToPlayer(camera, &first, &points[5], &work->mEye);
        break;
    case WIND_CLOSE_WAIT:
        if (static_cast<s32>(work->mCounter) > static_cast<s32>(work->mFrameLimit))
            work->mState = WIND_CLOSE_FOLLOW;
        relativeToPlayer(camera, &first, &points[2], &work->mCenter);
        relativeToPlayer(camera, &first, &points[5], &work->mEye);
        break;
    case WIND_CLOSE_FOLLOW:
        cameraRollingSubtract(&work->mCenter, &first, &camera->mViewCache.mCenter);
        cameraRollingScale(&first, &second, cushion);
        cameraRollingPSVECAdd(&camera->mViewCache.mCenter, &second, &camera->mViewCache.mCenter);
        cameraRollingSubtract(&work->mEye, &second, &camera->mViewCache.mEye);
        cameraRollingScale(&second, &first, cushion);
        cameraRollingPSVECAdd(&camera->mViewCache.mEye, &first, &camera->mViewCache.mEye);
        cameraRollingSubtract(&camera->mViewCache.mEye, &first, &camera->mViewCache.mCenter);
        cSGlobe_Val(&camera->mViewCache.mDirection, &first);
        break;
    case WIND_HOLD:
        break;
    default: {
        cameraRollingAttentionPos(camera, &first, camera->mpPlayerActor.get());
        copyPoint(&camera->mViewCache.mCenter, &first);
        cameraRollingRelationalPos(camera, &first, work->mpBird.get(), &work->mTorishita);
        copyPoint(&camera->mViewCache.mEye, &first);
        f32 distance =
            cameraRollingHorizontalDistance(&camera->mViewCache.mCenter, &camera->mViewCache.mEye);
        windBlurShock(camera, &first);
        if (distance < work->mStopDist) {
            work->mCounter = 0;
            work->mState = WIND_WAIT;
        }
        break;
    }
    }
    if (work->mType != 0) {
        camera->mEventData.field_0x20 = 1;
        camera->mViewCache.mFovy = 82.0f;
    } else {
        cameraRollingPositionOf(camera, &first, work->mpBird.get());
        cameraRollingPositionOf(camera, &second, camera->mpPlayerActor.get());
        cameraRollingSubtract(&first, &third, &second);
        f32 distance = vectorLength(&third);
        f32 nearDistance = work->mStopDist;
        f32 fovy;
        if (distance < nearDistance) {
            f32 nearFovy = work->mNearFovy;
            fovy = gabi::fmadds(work->mFarFovy - nearFovy, 0.0f, nearFovy);
        } else {
            f32 farDistance = work->mStartDistance;
            if (!(distance > farDistance))
                ratio = (distance - nearDistance) / (farDistance - nearDistance);
            f32 nearFovy = work->mNearFovy;
            fovy = gabi::fmadds(work->mFarFovy - nearFovy, ratio, nearFovy);
        }
        f32 currentFovy = camera->mViewCache.mFovy;
        camera->mViewCache.mFovy =
            gabi::fmadds(fovy - currentFovy, work->mFovyCushion, currentFovy);
    }
    ++work->mCounter;
    SkipSmoother(camera);
    return 1;
}
} // namespace event_camera_rolling
using namespace event_camera_rolling;

/* WWHD glue (tools/verify/autoglue.py)
data rollEyeName 1004D134
data rollCenterName 1004D140
data rollCtrCusName 1004D148
data rollTransTypeName 1004D16C
data rollFovyName 1004D150
data rollBankName 1004D158
data rollRollName 1004D160
data rollRadiusAddName 1004D178
data rollLatitudeName 1004D184
data rollTimerName 1004D138
data rollRelUseMaskName 1004D190
data rollRelUseMaskDefault 1004D168
data rollRelActorName 1004D19C
data degreesToAngle 1004CDE8
data windBirdFlyDistName 1004D20C
data windTorishitaName 1004D224
data windStopDistName 1004D240
data windUpCountName 1004D1F4
data windSideFlagName 1004D24C
data windFollowCushionName 1004D230
data windNearFovyName 1004D258
data windFarFovyName 1004D1FC
data windFovyCushionName 1004D218
data windTypeName 1004D204
ext 02530634 u32 cameraRollingGetInt(dCamera_c*, GuestWord*, const char*, s32)
ext 0253072C u32 cameraRollingGetFloat(dCamera_c*, GuestFloat*, const char*, f32)
ext 0253086C u32 cameraRollingGetXyz(dCamera_c*, cXyz*, const char*, cXyz*)
ext 02530998 u32 cameraRollingGetString(dCamera_c*, be<char>*, const char*, const char*)
ext 02530B84 fopAc_ac_c* cameraRollingGetActor(dCamera_c*, const char*)
ext 0250242C void cameraRollingRelationalPos(dCamera_c*, cXyz*, fopAc_ac_c*, cXyz*)
ext 024F8D5C void cameraRollingPositionOf(dCamera_c*, cXyz*, fopAc_ac_c*)
ext 024F8F3C void cameraRollingAttentionPos(dCamera_c*, cXyz*, fopAc_ac_c*)
ext 024F8F30 void cameraRollingDirectionOf(dCamera_c*, cSAngle_l*, fopAc_ac_c*)
ext 024FC0B0 void cameraRollingEyePos(dCamera_c*, cXyz*, fopAc_ac_c*)
ext 0201ADE0 void cameraRollingSubtract(cXyz*, cXyz*, cXyz*)
ext 0201AD78 void cameraRollingAdd(cXyz*, cXyz*, cXyz*)
ext 0201AE48 void cameraRollingScale(cXyz*, cXyz*, f32)
ext 028E8D88 void cameraRollingPSVECAdd(cXyz*, cXyz*, cXyz*)
ext 020073AC void cameraRollingGlobeXyz(cSGlobe_l*, cXyz*)
ext 02007324 void cameraRollingGlobeCtor(cSGlobe_l*, cXyz*)
ext 02007178 void cameraRollingGlobeCopyCtor(cSGlobe_l*, cSGlobe_l*)
ext 02007400 cSGlobe_l* cameraRollingGlobeInvert(cSGlobe_l*)
ext 02006FE4 void cameraRollingGlobeSet(cSGlobe_l*, f32, s32, s32)
ext 020068B0 void cameraRollingAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 02006894 void cameraRollingAngleAdd(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 0200693C void cameraRollingAngleScale(cSAngle_l*, cSAngle_l*, f32)
ext 020066C0 cSAngle_l* cameraRollingAngleFloatCtor(cSAngle_l*, f32)
ext 0200658C cSAngle_l* cameraRollingAngleShortCtor(cSAngle_l*, s32)
ext 02006720 f32 cameraRollingAngleDegree(cSAngle_l*)
ext 028E8DD0 f32 cameraRollingSquareMag(cXyz*)
ext 028F4384 f32 cameraRollingSqrt(f32)
ext 024FCBE8 bool cameraRollingLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32)
ext 0257E560 u32 cameraRollingTactWindDirection()
ext 0253EE04 fopAc_ac_c* cameraRollingConvPId(dEvt_control_c*, u32)
ext 0250D464 void cameraRollingResetBlure(dCamera_c*, s32)
ext 0250D4C0 void cameraRollingBlureTimer(dCamera_c*, s32)
ext 0250D4C8 void cameraRollingBlureAlpha(dCamera_c*, f32)
ext 0250D4D0 void cameraRollingBlureScale(dCamera_c*, f32)
ext 02018780 void cameraRollingLineCtor(cM3dGLin*, cXyz*, cXyz*)
ext 02010AE4 bool cameraRollingNearestPoint(cM3dGLin*, cXyz*, cXyz*, GuestFloat*)
ext 024F7AC4 f32 cameraRollingHorizontalDistance(cXyz*, cXyz*)
*/
