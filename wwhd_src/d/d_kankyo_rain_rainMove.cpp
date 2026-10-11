// HD: preserve rain collision checks, camera fades and the expanded particle packet layout.
#include "d/d_kankyo_rain_kazanbai_move.h"
namespace RainMotion {
/* 02566B88 */
void dKyr_rain_move() {
    u32 packet = (u32)at<Environment>(sdk::getEnvironment<u32>())->mpRainPacket,
        camera = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mCamera;
    sdk::getGameInfo<u32>();
    gabi::Local<cXyz> wind, center, direction, position, forward1, forward2;
    sdk::dKyw_get_wind_vecpow(wind.get());
    auto stageEquals = [&](u32 literal) {
        gabi::Local<SafeString> name, current;
        name->text = literal;
        name->vtable = SafeStringVtableAddress;
        current->text = ea(at<GameInfo>(sdk::getGameInfo<u32>())->mStartStageName);
        current->vtable = SafeStringVtableAddress;
        safe_string_callback(name.get());
        safe_string_callback(name.get());
        u32 left = name->text;
        safe_string_callback(current.get());
        u32 right = current->text;
        return left == right || equal_terminated_strings(left, right);
    };
    u32 stageType = 0;
    if (!stageEquals(RainSpecialStage)) {
        u32 stage = ea(&at<GameInfo>(sdk::getGameInfo<u32>())->mStageVtable);
        stage = gabi::call_ptr<u32>((u32)at<StageVtable>((u32)*at<be<u32>>(stage))->getInfo, stage);
        stageType = ((u32)at<StageInfo>(stage)->flags >> 16) & 7;
    }
    s32 count = (s16)at<RainPacket>(packet)->mRainCount;
    if (count > (s32)(u32)at<Environment>(sdk::getEnvironment<u32>())->mRainCount.word) {
        if ((s16)at<RainPacket>(packet)->mRainCount == 0)
            return;
    } else {
        s16 wanted = (s16)at<Environment>(sdk::getEnvironment<u32>())->mRainCount.halves.lower;
        at<RainPacket>(packet)->mRainCount = wanted;
        if (wanted == 0)
            return;
    }
    sdk::dKyr_get_vectle_calc(ea(&at<Camera>(camera)->mCenter), ea(&at<RainPacket>(packet)->mCamCenterPos),
                              direction.get());
    auto distance = [&](u32 a, u32 b) {
        sdk::distanceSquared(a, b);
        return sdk::squareRoot<f32>();
    };
    f32 eyeChange = distance(ea(&at<RainPacket>(packet)->mCamEyePos), ea(&at<Camera>(camera)->mEye)),
        zero = (f32)*at<be<f32>>(Zero), one = (f32)*at<be<f32>>(One), step;
    const f32 ten = (f32)*at<be<f32>>(RainEyeChangeThreshold), fifty = (f32)*at<be<f32>>(AshOffsetStep);
    if (eyeChange > ten)
        eyeChange = gabi::fsubs_ppc(eyeChange, ten) / fifty;
    else
        eyeChange = zero;
    if (eyeChange > one) {
        for (u32 component = 0; component < 3; component++)
            at<RainPacket>(packet)->mCamEyePos.words[component] =
                (u32)at<Camera>(camera)->mEye.words[component];
        eyeChange = one;
        f32 centerChange =
            distance(ea(&at<RainPacket>(packet)->mCamCenterPos), ea(&at<Camera>(camera)->mCenter));
        if (centerChange > ten) {
            centerChange = gabi::fsubs_ppc(centerChange, ten) / fifty;
            if (centerChange > one)
                centerChange = one;
        } else
            centerChange = zero;
        step = (f32)*at<be<f32>>(Increment);
        sdk::cLib_addCalc(ea(&at<RainPacket>(packet)->mCenterDeltaMul),
                          gabi::fmuls_ppc(eyeChange, centerChange), (f32)*at<be<f32>>(Rate),
                          (f32)*at<be<f32>>(Drag), step);
    } else {
        for (u32 component = 0; component < 3; component++)
            at<RainPacket>(packet)->mCamEyePos.words[component] =
                (u32)at<Camera>(camera)->mEye.words[component];
        f32 centerChange =
            distance(ea(&at<RainPacket>(packet)->mCamCenterPos), ea(&at<Camera>(camera)->mCenter));
        if (centerChange > ten) {
            centerChange = gabi::fsubs_ppc(centerChange, ten) / fifty;
            if (centerChange > one)
                centerChange = one;
        } else
            centerChange = zero;
        step = (f32)*at<be<f32>>(Increment);
        sdk::cLib_addCalc(ea(&at<RainPacket>(packet)->mCenterDeltaMul),
                          gabi::fmuls_ppc(eyeChange, centerChange), (f32)*at<be<f32>>(Rate),
                          (f32)*at<be<f32>>(Drag), step);
    }
    const f32 small = (f32)*at<be<f32>>(Drag);
    if ((f32)at<RainPacket>(packet)->mCenterDeltaMul > (f32)*at<be<f32>>(ThunderFogScale))
        at<RainPacket>(packet)->mCenterDeltaMul = (f32)*at<be<f32>>(ThunderFogScale);
    sdk::cLib_addCalc(ea(&at<RainPacket>(packet)->mCenterDelta), (f32)direction->x, (f32)*at<be<f32>>(Rate),
                      small, step);
    sdk::cLib_addCalc(packet + 0x377C, (f32)direction->y, (f32)*at<be<f32>>(Rate), small, step);
    sdk::cLib_addCalc(packet + 0x3780, (f32)direction->z, (f32)*at<be<f32>>(Rate), small, step);
    for (u32 component = 0; component < 3; component++)
        at<RainPacket>(packet)->mCamCenterPos.words[component] =
            (u32)at<Camera>(camera)->mCenter.words[component];
    sdk::dKy_set_eyevect_calc2(camera, center.get(), (f32)*at<be<f32>>(MoonDistanceGain),
                               (f32)*at<be<f32>>(ScalarOne));
    sdk::dKyw_get_wind_vec<u32>();
    sdk::dKyw_get_wind_pow<f32>();
    at<RainPacket>(packet)->field_0x36cc = zero;
    at<RainPacket>(packet)->field_0x36c8 = zero;
    at<RainPacket>(packet)->mStatus = ParticleInactive;
    u32 overhead = 0, front = 0, farFront = 0, radial = 0;
    f32 radialLimit = 0;
    bool special = stageType == 2 && !stageEquals(RainUnblockedStage) && !stageEquals(RainOutdoorStage);
    if (!special) {
        u32 checkCamera = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mCamera;
        sdk::getGameInfo<u32>();
        gabi::Local<GroundCheck> ground;
        gabi::Local<RoofCheck> roof;
        gabi::Local<cXyz> cameraPoint;
        u32 g = gabi::ea(ground.get()), r = gabi::ea(roof.get());
        sdk::constructGroundBase(ground.get());
        at<GroundCheck>(g)->pass.flags[1] = 0;
        at<GroundCheck>(g)->group.__vtbl = GroundGroupVtable;
        at<GroundCheck>(g)->pass.flags[2] = 0;
        at<GroundCheck>(g)->base.mPolyPassChk = ea(&at<GroundCheck>(g)->pass);
        at<GroundCheck>(g)->pass.flags[3] = 0;
        at<GroundCheck>(g)->pass.flags[4] = 0;
        at<GroundCheck>(g)->pass.flags[5] = 0;
        at<GroundCheck>(g)->base.mGrpPassChk = ea(&at<GroundCheck>(g)->group);
        at<GroundCheck>(g)->pass.vtable = ObjectPassVtable;
        at<GroundCheck>(g)->pass.flags[6] = 0;
        at<GroundCheck>(g)->group.mGrp = 15;
        at<GroundCheck>(g)->base.cBgS_PolyInfo::__vtbl = GroundPolyVtable;
        at<GroundCheck>(g)->base.cBgS_Chk::__vtbl = GroundBaseVtable;
        at<GroundCheck>(g)->pass.flags[0] = 1;
        sdk::constructRoofCheck(roof.get(), 1);
        cameraPoint->x = (f32)at<Camera>(checkCamera)->mEye.channels[0];
        cameraPoint->z = (f32)at<Camera>(checkCamera)->mEye.channels[2];
        f32 y = gabi::fadds_ppc((f32)at<Camera>(checkCamera)->mEye.channels[1], fifty);
        at<RoofCheck>(r)->base.m_pos.x = (f32)cameraPoint->x;
        at<RoofCheck>(r)->base.m_pos.y = y;
        at<RoofCheck>(r)->base.m_pos.z = (f32)cameraPoint->z;
        overhead = sdk::roofCheck<f32>(ea(at<GameInfo>(sdk::getGameInfo<u32>())->mBgSpace), roof.get()) !=
                   (f32)*at<be<f32>>(NoGroundHeight);
        at<GroundCheck>(g)->base.m_pos.x = (f32)cameraPoint->x;
        at<GroundCheck>(g)->base.m_pos.y = gabi::fadds_ppc(y, (f32)*at<be<f32>>(FarPlaneInset));
        at<GroundCheck>(g)->base.m_pos.z = (f32)cameraPoint->z;
        if (sdk::groundCross<f32>(ea(at<GameInfo>(sdk::getGameInfo<u32>())->mBgSpace), ground.get()) >
            gabi::fadds_ppc((f32)at<Camera>(checkCamera)->mEye.channels[1], fifty))
            overhead = 1;
        at<RoofCheck>(r)->base.dBgS_GrpPassChk::__vtbl = BaseGroupVtable;
        at<RoofCheck>(r)->base.cBgS_Chk::__vtbl = RoofBaseVtable;
        at<RoofCheck>(r)->base.dBgS_PolyPassChk::__vtbl = RoofPassVtable;
        sdk::destroyCheckBase(ea(&at<RoofCheck>(r)->base.cBgS_Chk::mPolyPassChk), 0);
        at<GroundCheck>(g)->pass.vtable = PolyPassVtable;
        at<GroundCheck>(g)->group.__vtbl = BaseGroupVtable;
        at<GroundCheck>(g)->base.cBgS_PolyInfo::__vtbl = BasePolyVtable;
        sdk::destroyGroundBase(ground.get(), 0);
        front = sdk::dKyr_get_rain_pos<u32>(forward1.get(), (f32)*at<be<f32>>(MoonDistanceGain));
        farFront = sdk::dKyr_get_rain_pos<u32>(forward2.get(), (f32)*at<be<f32>>(RainFarFrontDistance));
        if (overhead)
            at<RainPacket>(packet)->mStatus = (u8)at<RainPacket>(packet)->mStatus | 1;
        else if (front)
            at<RainPacket>(packet)->mStatus = (u8)at<RainPacket>(packet)->mStatus | 2;
        if (stageEquals(RainUnblockedStage)) {
            at<RainPacket>(packet)->mStatus = ParticleInactive;
            overhead = front = farFront = 0;
        }
    } else {
        at<RainPacket>(packet)->mStatus = (u8)at<RainPacket>(packet)->mStatus | 1;
        radial = 1;
        if (stageEquals(RainRadialStageA))
            radialLimit = (f32)*at<be<f32>>(RainRadialLimitA);
        else if (stageEquals(RainRadialStageB))
            radialLimit = (f32)*at<be<f32>>(RainRadialLimitB);
        else
            radialLimit = (f32)*at<be<f32>>(RainRadialLimitDefault);
        if (stageEquals(RainHeightFadeStage))
            radial = 2;
    }
    auto fade = [&](be<f32> *amount, u32 blocked) {
        if (blocked)
            sdk::cLib_addCalc(ea(amount), zero, (f32)*at<be<f32>>(Half), (f32)*at<be<f32>>(Rate), step);
        else
            sdk::cLib_addCalc(ea(amount), one, small, small, (f32)*at<be<f32>>(SprayMinimumStep));
    };
    fade(&at<RainPacket>(packet)->mOverheadFade, overhead);
    fade(&at<RainPacket>(packet)->mFwdFade1, front);
    fade(&at<RainPacket>(packet)->mFwdFade2, farFront);
    count = (s16)at<RainPacket>(packet)->mRainCount;
    if (count <= 0)
        return;
    const f32 scatter = (f32)*at<be<f32>>(RainScatterRadius), fallBase = (f32)*at<be<f32>>(RainFallBase),
              bottom = (f32)*at<be<f32>>(RainResetHeight), accel = (f32)*at<be<f32>>(RainMotionGain),
              jitter = (f32)*at<be<f32>>(RainShift), vertical = (f32)*at<be<f32>>(RainVerticalMotion);
    auto sumPosition = [&](u32 p) {
        position->x = gabi::fadds_ppc((f32)at<RAIN_EFF>(p)->mBasePos.channels[0],
                                      (f32)at<RAIN_EFF>(p)->mPos.channels[0]);
        position->y = gabi::fadds_ppc((f32)at<RAIN_EFF>(p)->mBasePos.channels[1],
                                      (f32)at<RAIN_EFF>(p)->mPos.channels[1]);
        position->z = gabi::fadds_ppc((f32)at<RAIN_EFF>(p)->mBasePos.channels[2],
                                      (f32)at<RAIN_EFF>(p)->mPos.channels[2]);
    };
    auto copyCenter = [&](u32 p) {
        at<RAIN_EFF>(p)->mBasePos.words[0] = (u32)(*at<Vector>(gabi::ea(center.get()))).words[0];
        at<RAIN_EFF>(p)->mBasePos.words[1] = (u32)(*at<Vector>(gabi::ea(center.get()))).words[1];
        at<RAIN_EFF>(p)->mBasePos.words[2] = (u32)(*at<Vector>(gabi::ea(center.get()))).words[2];
    };
    for (s32 i = count - 1; i >= 0; --i) {
        u32 p = ea(&at<RainPacket>(packet)->mEff[(u32)i]);
        u8 state = (u8)at<RAIN_EFF>(p)->mStatus;
        at<RAIN_EFF>(p)->mBasePos.channels[1] = (f32)center->y;
        if (state == ParticleInactive) {
            at<RAIN_EFF>(p)->field_0x24 = -gabi::fadds_ppc(sdk::cM_rndF<f32>(ten), fallBase);
            at<RAIN_EFF>(p)->mTimer = 0;
            copyCenter(p);
            at<RAIN_EFF>(p)->mPos.channels[0] = sdk::cM_rndFX<f32>(scatter);
            at<RAIN_EFF>(p)->mPos.channels[1] = sdk::cM_rndF<f32>((f32)*at<be<f32>>(ScalarOne));
            at<RAIN_EFF>(p)->mPos.channels[2] = sdk::cM_rndFX<f32>(scatter);
            at<RAIN_EFF>(p)->mAlpha = one;
            at<RAIN_EFF>(p)->field_0x1c = sdk::cM_rndF<f32>((f32)*at<be<f32>>(RainAngleRange));
            at<RAIN_EFF>(p)->field_0x20 = sdk::cM_rndF<f32>((f32)*at<be<f32>>(RainAngleRange));
            sdk::dKyr_set_rain_base(packet, i);
            at<RAIN_EFF>(p)->mStatus = (u8)at<RAIN_EFF>(p)->mStatus + 1;
            sumPosition(p);
        } else if (state <= ParticleSpecial) {
            f32 random = sdk::cM_rndFX<f32>(small);
            sdk::cLib_addCalc(ea(&at<RAIN_EFF>(p)->field_0x24),
                              gabi::fsubs_ppc((f32)at<RAIN_EFF>(p)->field_0x24, random),
                              (f32)*at<be<f32>>(Half), small, step);
            f32 a = gabi::fmuls_ppc((f32)at<RainPacket>(packet)->mCenterDelta.channels[0],
                                    (f32)at<RainPacket>(packet)->mCenterDeltaMul);
            a = gabi::fmadds(a, ten, (f32)wind->x);
            a = gabi::fmadds((f32)(i & 7), jitter, a);
            at<RAIN_EFF>(p)->mPos.channels[0] =
                gabi::fmadds(a, accel, (f32)at<RAIN_EFF>(p)->mPos.channels[0]);
            a = gabi::fmuls_ppc((f32)at<RainPacket>(packet)->mCenterDelta.channels[1],
                                (f32)at<RainPacket>(packet)->mCenterDeltaMul);
            a = gabi::fmadds(a, ten, (f32)wind->y);
            a = gabi::fadds_ppc(a, vertical);
            at<RAIN_EFF>(p)->mPos.channels[1] =
                gabi::fmadds(a, accel, (f32)at<RAIN_EFF>(p)->mPos.channels[1]);
            a = gabi::fmuls_ppc((f32)at<RainPacket>(packet)->mCenterDelta.channels[2],
                                (f32)at<RainPacket>(packet)->mCenterDeltaMul);
            a = gabi::fmadds(a, ten, (f32)wind->z);
            a = gabi::fmadds((f32)(i & 3), jitter, a);
            at<RAIN_EFF>(p)->mPos.channels[2] =
                gabi::fmadds(a, accel, (f32)at<RAIN_EFF>(p)->mPos.channels[2]);
            position->x = gabi::fadds_ppc((f32)at<RAIN_EFF>(p)->mBasePos.channels[0],
                                          (f32)at<RAIN_EFF>(p)->mPos.channels[0]);
            position->y = center->y;
            position->z = gabi::fadds_ppc((f32)at<RAIN_EFF>(p)->mBasePos.channels[2],
                                          (f32)at<RAIN_EFF>(p)->mPos.channels[2]);
            f32 dist = distance(gabi::ea(position.get()), gabi::ea(center.get()));
            s16 timer = (s16)at<RAIN_EFF>(p)->mTimer;
            if (timer != 0)
                at<RAIN_EFF>(p)->mTimer = (u16)(timer - 1);
            else {
                if (dist > scatter) {
                    at<RAIN_EFF>(p)->mTimer = 10;
                    at<RAIN_EFF>(p)->field_0x24 = -gabi::fadds_ppc(sdk::cM_rndF<f32>(ten), fallBase);
                    copyCenter(p);
                    if (distance(gabi::ea(position.get()), gabi::ea(center.get())) >
                        (f32)*at<be<f32>>(RainResetDistance)) {
                        at<RAIN_EFF>(p)->mPos.channels[0] = sdk::cM_rndFX<f32>(scatter);
                        at<RAIN_EFF>(p)->mPos.channels[1] = sdk::cM_rndFX<f32>(scatter);
                        at<RAIN_EFF>(p)->mPos.channels[2] = sdk::cM_rndFX<f32>(scatter);
                    } else {
                        f32 radius =
                            gabi::fadds_ppc(sdk::cM_rndFX<f32>((f32)*at<be<f32>>(RainShellJitter)), scatter);
                        sdk::dKyr_get_vectle_calc2(position.get(), center.get(), direction.get());
                        at<RAIN_EFF>(p)->mPos.channels[0] = gabi::fmuls_ppc((f32)direction->x, radius);
                        at<RAIN_EFF>(p)->mPos.channels[1] = gabi::fmuls_ppc((f32)direction->y, radius);
                        at<RAIN_EFF>(p)->mPos.channels[2] = gabi::fmuls_ppc((f32)direction->z, radius);
                    }
                    at<RAIN_EFF>(p)->mStatus = ParticleMoving;
                    sdk::dKyr_set_rain_base(packet, i);
                }
                position->y = gabi::fadds_ppc((f32)at<RAIN_EFF>(p)->mBasePos.channels[1],
                                              (f32)at<RAIN_EFF>(p)->mPos.channels[1]);
                if ((f32)position->y < gabi::fadds_ppc((f32)at<RAIN_EFF>(p)->field_0x30, accel)) {
                    copyCenter(p);
                    at<RAIN_EFF>(p)->mPos.channels[0] = sdk::cM_rndFX<f32>(scatter);
                    at<RAIN_EFF>(p)->mPos.channels[1] = bottom;
                    at<RAIN_EFF>(p)->mPos.channels[2] = sdk::cM_rndFX<f32>(scatter);
                    sdk::dKyr_set_rain_base(packet, i);
                    at<RAIN_EFF>(p)->mTimer = 10;
                }
            }
            sumPosition(p);
        } else
            sumPosition(p);
        if (i > (s32)((u32)at<Environment>(sdk::getEnvironment<u32>())->mRainCount.word - 1) &&
            (u32)i == (u32)((s16)at<RainPacket>(packet)->mRainCount - 1))
            at<RainPacket>(packet)->mRainCount = (u16)((s16)at<RainPacket>(packet)->mRainCount - 1);
        f32 alpha = one;
        if (overhead || (f32)at<RainPacket>(packet)->mOverheadFade < one) {
            gabi::Local<cXyz> horizontal;
            horizontal->x = position->x;
            horizontal->y = (f32)at<Camera>(camera)->mEye.channels[1];
            horizontal->z = position->z;
            if (distance(ea(&at<Camera>(camera)->mEye), gabi::ea(horizontal.get())) < scatter)
                alpha = (f32)at<RainPacket>(packet)->mOverheadFade;
        }
        if (front || (f32)at<RainPacket>(packet)->mFwdFade1 < one) {
            gabi::Local<cXyz> horizontal;
            horizontal->x = position->x;
            horizontal->y = forward1->y;
            horizontal->z = position->z;
            if (distance(gabi::ea(forward1.get()), gabi::ea(horizontal.get())) <
                (f32)*at<be<f32>>(FlareSizeGain))
                alpha = gabi::fmuls_ppc(alpha, (f32)at<RainPacket>(packet)->mFwdFade1);
        }
        if (farFront || (f32)at<RainPacket>(packet)->mFwdFade2 < one) {
            gabi::Local<cXyz> horizontal;
            horizontal->x = position->x;
            horizontal->y = forward2->y;
            horizontal->z = position->z;
            if (distance(gabi::ea(forward2.get()), gabi::ea(horizontal.get())) <
                (f32)*at<be<f32>>(FlareSizeGain))
                alpha = gabi::fmuls_ppc(alpha, (f32)at<RainPacket>(packet)->mFwdFade2);
        }
        if (radial) {
            gabi::Local<cXyz> horizontal;
            horizontal->x = position->x;
            horizontal->y = zero;
            horizontal->z = position->z;
            sdk::vectorSquareMagnitude(horizontal.get());
            f32 d = gabi::fsubs_ppc(sdk::squareRoot<f32>(), radialLimit);
            alpha = d >= 0 ? alpha : zero;
            if (radial == 2) {
                d = gabi::fsubs_ppc((f32)position->y, (f32)*at<be<f32>>(RainRadialLimitB));
                alpha = d >= 0 ? alpha : zero;
            }
        }
        at<RAIN_EFF>(p)->mAlpha = alpha;
    }
}

} // namespace RainMotion

using namespace RainMotion;
/* WWHD glue (tools/verify/autoglue.py)
 */
