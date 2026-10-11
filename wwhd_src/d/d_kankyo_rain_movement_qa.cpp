// Lens movement, snow setup and thunder use the GameCube weather state machine.
// HD: packet layouts and SafeString shader resources differ from the GameCube implementation.
#include "d/d_kankyo_rain_kazanbai_move.h"
namespace RainMotion {
/* 02565B70 */
void dKyr_lenzflare_move() {
    u32 sun = (u32)at<Environment>(sdk::getEnvironment<u32>())->mpSunPacket,
        lens = (u32)at<Environment>(sdk::getEnvironment<u32>())->mpSunlenzPacket;
    u32 camera = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mCamera;
    sdk::getGameInfo<u32>();
    gabi::Local<ProjectionParameters> projection;
    gabi::Local<cXyz> eye, sunDirection, projected, center, screenDirection, cameraDirection;
    sdk::prepareProjection(projection.get());
    if ((f32)at<SunPacket>(sun)->mVisibility < (f32)*at<be<f32>>(FlareMinimumVisibility))
        return;
    f32 distance = (f32)*at<be<f32>>(FlareEyeDistance);
    sdk::dKy_set_eyevect_calc2(camera, eye.get(), distance, distance);
    sdk::dKyr_get_vectle_calc(eye.get(), ea(&at<SunPacket>(sun)->mPos[0]), sunDirection.get());
    for (u32 destination = 0; destination < 3; destination++)
        for (u32 component = 0; component < 3; component++)
            at<LensPacket>(lens)->mPositions[destination].words[component] =
                (u32)at<SunPacket>(sun)->mPos[0].words[component];
    sdk::project(ea(&at<LensPacket>(lens)->mPositions[0]), projected.get(), projection.get());
    center->x = (f32)*at<be<f32>>(ScreenCenterX);
    center->y = (f32)*at<be<f32>>(ScreenCenterY);
    center->z = (f32)*at<be<f32>>(Zero);
    sdk::dKyr_get_vectle_calc(center.get(), projected.get(), screenDirection.get());
    s32 angle = sdk::cM_atan2s<s32>((f32)screenDirection->x, (f32)screenDirection->y);
    at<LensPacket>(lens)->mAngleDeg =
        gabi::fmadds((f32)angle, (f32)*at<be<f32>>(FlareAngleScale), (f32)*at<be<f32>>(HalfTurn));
    sdk::dKyr_get_vectle_calc(ea(&at<Camera>(camera)->mEye), ea(&at<Camera>(camera)->mCenter),
                              cameraDirection.get());
    f32 magnitude = sdk::distanceSquared<f32>(sunDirection.get(), cameraDirection.get());
    magnitude = sdk::squareRoot<f32>(magnitude);
    f32 size = gabi::fmadds(magnitude, (f32)*at<be<f32>>(FlareSizeGain), (f32)*at<be<f32>>(FlareSizeBase));
    f32 displacement = (f32)*at<be<f32>>(FlareDisplacement), step = (f32)*at<be<f32>>(FlareStep);
    // HD: advances flare spacing by alternating increments.
    for (u32 i = 1; i <= 6; i++) {
        u32 parity = i % 2;
        Vector *destination = &at<LensPacket>(lens)->mPositions[i + 2];
        f32 x = gabi::fmuls_ppc((f32)sunDirection->x, size);
        destination->channels[0] =
            gabi::fnmsubs(displacement, x, std::bit_cast<f32>((u32)at<SunPacket>(sun)->mPos[0].words[0]));
        f32 y = gabi::fmuls_ppc((f32)sunDirection->y, size);
        destination->channels[1] =
            gabi::fnmsubs(displacement, y, std::bit_cast<f32>((u32)at<SunPacket>(sun)->mPos[0].words[1]));
        f32 z = gabi::fmuls_ppc((f32)sunDirection->z, size);
        f32 next = gabi::fadds_ppc((f32)parity, step);
        destination->channels[2] =
            gabi::fnmsubs(displacement, z, std::bit_cast<f32>((u32)at<SunPacket>(sun)->mPos[0].words[2]));
        displacement = gabi::fadds_ppc(displacement, next);
    }
}

/* 025688C4 */
void dKyr_snow_init() {
    u32 camera = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mCamera;
    gabi::Local<SafeString> stageName, currentStage;
    stageName->text = VolcanoStageName;
    stageName->vtable = SafeStringVtableAddress;
    currentStage->text = ea(at<GameInfo>(sdk::getGameInfo<u32>())->mStartStageName);
    currentStage->vtable = SafeStringVtableAddress;
    safe_string_callback(stageName.get());
    safe_string_callback(stageName.get());
    u32 expected = (u32)stageName->text;
    safe_string_callback(currentStage.get());
    bool special = equal_terminated_strings(expected, (u32)currentStage->text);
    gabi::Local<be<u32>> heapGuard;
    sdk::constructHeapGuard(heapGuard.get(), SnowHeapName, special ? VolcanoSnowHeapSize : SnowHeapSize, 0);
    u32 packet = sdk::allocate<u32>(SnowPacketSize, 32);
    if (packet)
        packet = sdk::constructSnowPacket<u32>(packet);
    at<Environment>(sdk::getEnvironment<u32>())->mpSnowPacket = packet;
    if (!snowPacket()) {
        sdk::reportHeapFailure(heapGuard.get());
        sdk::destroyHeapGuard(heapGuard.get(), 2);
        return;
    }
    gabi::Local<SafeString> archiveName, resourceName;
    archiveName->text = special ? VolcanoArchive : DefaultSnowArchive;
    archiveName->vtable = SafeStringVtableAddress;
    resourceName->text = special ? VolcanoSnowResource : DefaultSnowResource;
    resourceName->vtable = SafeStringVtableAddress;
    u32 resource = sdk::getArchiveResource<u32>((u32)*at<be<u32>>(ResourceManagerPointer), archiveName.get(),
                                                resourceName.get(), 0);
    u32 shader = sdk::getShaderResource<u32>(resource),
        relative = (u32)at<ShaderResource>(shader)->relativeData;
    u32 uniform = sdk::getShaderUniform<u32>(
        relative ? shader + (u32)offsetof(ShaderResource, relativeData) + relative : 0,
        special ? VolcanoSnowUniform : DefaultSnowUniform);
    at<SnowPacket>(snowPacket())->mpTexture = uniform;
    sdk::prepareSnowPacket(snowPacket());
    for (u32 i = 0; i < 250; i++)
        at<SnowPacket>(snowPacket())->mEff[i].mStatus = 0;
    at<SnowPacket>(snowPacket())->mEffCount = 0;
    packet = snowPacket();
    for (u32 component = 0; component < 3; component++)
        at<SnowPacket>(packet)->mOldEyePos.words[component] = (u32)at<Camera>(camera)->mEye.words[component];
    sdk::destroyHeapGuard(heapGuard.get(), 2);
}

/* 02577138 */
void dKyr_thunder_move() {
    u32 thunder = ea(&at<Environment>(sdk::getEnvironment<u32>())->mThunderEff),
        camera = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mCamera;
    f32 half = (f32)*at<be<f32>>(Half), zero = (f32)*at<be<f32>>(Zero), one = (f32)*at<be<f32>>(One),
        nearScale = (f32)*at<be<f32>>(ThunderNearScale);
    u8 state = (u8)at<EF_THUNDER>(thunder)->mState;
    switch (state) {
    case ThunderIdle:
        at<EF_THUNDER>(thunder)->mFlashTimer = zero;
        at<EF_THUNDER>(thunder)->field_0xc = zero;
        at<EF_THUNDER>(thunder)->field_0x10 = zero;
        if (sdk::cM_rndF<f32>(one) < (f32)*at<be<f32>>(ThunderFarChance)) {
            at<EF_THUNDER>(thunder)->mState = ThunderFarFlash;
            thunder_color(ThunderSkyColor, thunder, CloudSmallOffset);
            sdk::setSkyKasumiColor(90, 160, 245,
                                   gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, nearScale));
            thunder_color(ThunderFogColor, thunder, ThunderFarFogScale);
            return;
        }
        if (sdk::cM_rndF<f32>(one) < (f32)*at<be<f32>>(ThunderNearChance) &&
            (s32)(u32)at<Environment>(sdk::getEnvironment<u32>())->mThunderEff.mMode < 10) {
            for (u32 i = 0; i < 3; i++)
                at<EF_THUNDER>(thunder)->mLightInfluence.mPos.words[i] =
                    (u32)at<Camera>(camera)->mEye.words[i];
            at<EF_THUNDER>(thunder)->mLightInfluence.mColor[0] = 0;
            at<EF_THUNDER>(thunder)->mLightInfluence.mColor[1] = 0;
            at<EF_THUNDER>(thunder)->mLightInfluence.mColor[2] = 0;
            at<EF_THUNDER>(thunder)->mLightInfluence.mPower = (f32)*at<be<f32>>(ThunderPower);
            at<EF_THUNDER>(thunder)->mLightInfluence.mFluctuation = (f32)*at<be<f32>>(ThunderHeight);
            sdk::setEffectLight(ea(&at<EF_THUNDER>(thunder)->mLightInfluence));
            state = (u8)((u8)at<EF_THUNDER>(thunder)->mState + 1);
            at<EF_THUNDER>(thunder)->mState = state;
        } else
            state = (u8)at<EF_THUNDER>(thunder)->mState;
        break;
    case ThunderNearFlash:
    case ThunderFarFlash: {
        sdk::cLib_addCalc(ea(&at<EF_THUNDER>(thunder)->mFlashTimer), one, (f32)*at<be<f32>>(DirectionBias),
                          (f32)*at<be<f32>>(Rate), (f32)*at<be<f32>>(SprayMinimumStep));
        f32 random = sdk::cM_rndF<f32>(half);
        sdk::cLib_addCalc(ea(&at<EF_THUNDER>(thunder)->field_0xc), gabi::fadds_ppc(random, half), half,
                          (f32)*at<be<f32>>(DirectionBias), (f32)*at<be<f32>>(Increment));
        // HD: Native branches on less-than; unordered values also advance the state.
        if (!((f32)at<EF_THUNDER>(thunder)->mFlashTimer < one)) {
            state = (u8)at<EF_THUNDER>(thunder)->mState;
            if (state < 10) {
                sdk::startThunderSound(ThunderSound, 0);
                state = (u8)at<EF_THUNDER>(thunder)->mState;
            }
            at<EF_THUNDER>(thunder)->mState = (u8)(state + 1);
        }
        if (sdk::cM_rndF<f32>(one) < (f32)*at<be<f32>>(ThunderActorChance))
            sdk::createThunderActor(ThunderActor, -1, 0, 0, 0);
        state = (u8)at<EF_THUNDER>(thunder)->mState;
        break;
    }
    case ThunderNearFade:
    case ThunderFarFade: {
        sdk::cLib_addCalc(ea(&at<EF_THUNDER>(thunder)->mFlashTimer), zero, (f32)*at<be<f32>>(Drag),
                          (f32)*at<be<f32>>(AlphaStep), (f32)*at<be<f32>>(SprayMinimumStep));
        f32 random = sdk::cM_rndF<f32>(half);
        sdk::cLib_addCalc(ea(&at<EF_THUNDER>(thunder)->field_0xc), gabi::fadds_ppc(random, half), half,
                          (f32)*at<be<f32>>(DirectionBias), (f32)*at<be<f32>>(Increment));
        if (!((f32)at<EF_THUNDER>(thunder)->mFlashTimer > zero)) {
            if ((u8)at<EF_THUNDER>(thunder)->mState < 10)
                sdk::cutEffectLight(ea(&at<EF_THUNDER>(thunder)->mLightInfluence));
            at<EF_THUNDER>(thunder)->mState = ThunderIdle;
            if ((s32)(u32)at<Environment>(sdk::getEnvironment<u32>())->mThunderEff.mMode == 0)
                at<Environment>(sdk::getEnvironment<u32>())->mThunderEff.mStatus = 0;
        }
        state = (u8)at<EF_THUNDER>(thunder)->mState;
        break;
    }
    }
    if (!state)
        return;
    if (state >= 10) {
        thunder_color(ThunderSkyColor, thunder, CloudSmallOffset);
        sdk::setSkyKasumiColor(90, 160, 245,
                               gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, nearScale));
        thunder_color(ThunderFogColor, thunder, ThunderFarFogScale);
        return;
    }
    f32 intensity =
        gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, (f32)at<EF_THUNDER>(thunder)->field_0xc);
    f32 blue = gabi::fmuls_ppc(intensity, nearScale);
    at<EF_THUNDER>(thunder)->mLightInfluence.mPos.words[0] = (u32)at<Camera>(camera)->mEye.words[0];
    f32 green = gabi::fmuls_ppc(intensity, (f32)*at<be<f32>>(Rate));
    f32 y = gabi::fadds_ppc(std::bit_cast<f32>((u32)at<Camera>(camera)->mEye.words[1]),
                            (f32)*at<be<f32>>(ThunderHeight));
    f32 red = gabi::fmuls_ppc(intensity, (f32)*at<be<f32>>(AlphaStep));
    f32 multiplier = (f32)*at<be<f32>>(ThunderColorScale);
    s16 greenColor = (s16)gabi::ftoi(gabi::fmuls_ppc(multiplier, green));
    s16 redColor = (s16)gabi::ftoi(gabi::fmuls_ppc(multiplier, red));
    at<EF_THUNDER>(thunder)->mLightInfluence.mPos.words[1] = std::bit_cast<u32>((f32)(y));
    at<EF_THUNDER>(thunder)->mLightInfluence.mPos.words[2] = (u32)at<Camera>(camera)->mEye.words[2];
    s16 blueColor = (s16)gabi::ftoi(gabi::fmuls_ppc(multiplier, blue));
    at<EF_THUNDER>(thunder)->mLightInfluence.mColor[0] = redColor;
    at<EF_THUNDER>(thunder)->mLightInfluence.mColor[1] = greenColor;
    at<EF_THUNDER>(thunder)->mLightInfluence.mColor[2] = blueColor;
    if (!(u8)at<Environment>(sdk::getEnvironment<u32>())->field_0xc98) {
        sdk::actorAmbientColor(90, 160, 245,
                               gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, half));
        sdk::actorDiffuseColor(90, 160, 245,
                               gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, half));
    }
    thunder_color(ThunderBackgroundAmbientColor, thunder, ThunderBackgroundScale, 50, 120, 255);
    sdk::backgroundDiffuseColor(90, 160, 245,
                                gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, nearScale));
    sdk::background1AmbientColor(90, 160, 245,
                                 gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, nearScale));
    thunder_color(ThunderSkyColor, thunder, ThunderSkyScale);
    sdk::setSkyKasumiColor(90, 160, 245, gabi::fmuls_ppc((f32)at<EF_THUNDER>(thunder)->mFlashTimer, half));
    thunder_color(ThunderFogColor, thunder, ThunderFogScale);
}

} // namespace RainMotion
using namespace RainMotion;
/* WWHD glue (tools/verify/autoglue.py)
 */
