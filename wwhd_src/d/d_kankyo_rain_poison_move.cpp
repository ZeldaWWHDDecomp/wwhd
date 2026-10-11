// HD: poison particle packet and room pattern data retain their native layouts.
#include "d/d_kankyo_rain_kazanbai_move.h"
namespace RainMotion {
/* 0256CA54 */
void poison_move() {
    Local<PoisonMotionWork> frame;
    u32 packet = (u32)at<Environment>(sdk::getEnvironment<u32>())->mpPoisonPacket,
        camera = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mCamera,
        player = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mPlayer,
        link = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mPlayer;
    sdk::dKyw_get_wind_vecpow<void>(ea(&frame->wind));
    for (u32 i = 0; i < 60; i++)
        frame->patterns[i / 12].words[i % 12] =
            (u32)at<PoisonPatternData>(PoisonPatterns)[i / 12].words[i % 12];
    s32 room = (s8)*at<be<s8>>(CurrentRoom);
    u32 env = sdk::getEnvironment<u32>();
    if ((u8)at<Environment>(env)->mPoisonRoom != 255)
        room = (u8)at<Environment>(sdk::getEnvironment<u32>())->mPoisonRoom;
    if ((u32)*at<be<u32>>(PreviousPoisonRoom) != (u32)room) {
        *at<be<u32>>(PreviousPoisonRoom) = room;
        for (u32 i = 0; i < 1000; i++)
            at<PoisonPacket>(packet)->mEff[i].mStatus = 0;
    }
    s32 pattern = room < 16 ? (s32)(u32)at<be<s32>>(RoomPatternMap)[room] : -1;
    u32 data = ea(&frame->patterns[pattern >= 0 ? pattern : 1].value);
    f32 baseX = (f32)at<PoisonPattern>(data)->mBasePos.channels[0],
        baseY = (f32)at<PoisonPattern>(data)->mBasePos.channels[1],
        baseZ = (f32)at<PoisonPattern>(data)->mBasePos.channels[2],
        rangeX = (f32)at<PoisonPattern>(data)->mRange.channels[0],
        rangeZ = (f32)at<PoisonPattern>(data)->mRange.channels[2], size = (f32)at<PoisonPattern>(data)->mSize,
        rise = (f32)at<PoisonPattern>(data)->mRise, rangeY = (f32)at<PoisonPattern>(data)->mRange.channels[1];
    frame->rise.value = rise;
    frame->rangeY.value = rangeY;
    env = sdk::getEnvironment<u32>();
    at<Environment>(env)->mPoisonCount = pattern >= 0 ? (u32)at<PoisonPattern>(data)->mCount : 0;
    f32 drift = (f32)at<PoisonPattern>(data)->mDrift;
    u32 tilt = (u16)at<PoisonPattern>(data)->mTilt, heading = (u16)at<PoisonPattern>(data)->mHeading;
    u32 base = ea(&at<PoisonPacket>(packet)->mBasePos);
    (*at<Vector>(base)).channels[0] = baseX;
    (*at<Vector>(base)).channels[2] = baseZ;
    (*at<Vector>(base)).channels[1] = fadds_ppc(baseY, size);
    rangeX = fsubs_ppc(rangeX, size);
    rangeZ = fsubs_ppc(rangeZ, size);
    f32 phase = fadds_ppc((f32)at<PoisonPacket>(packet)->mPhase, (f32)*at<be<f32>>(PoisonPhaseStep));
    if (!(phase < (f32)*at<be<f32>>(PoisonPhaseWrap)))
        phase = fsubs_ppc(phase, (f32)*at<be<f32>>(PoisonPhaseWrap));
    at<PoisonPacket>(packet)->mPhase = phase;
    frame->rangeX.value = rangeX;
    frame->rangeZ.value = rangeZ;
    frame->closestDistance.value = (f32)*at<be<f32>>(NoGroundHeight);
    s32 count = (u32)at<Environment>(sdk::getEnvironment<u32>())->mPoisonCount;
    if (count > 0) {
        const f32 zero = (f32)*at<be<f32>>(Zero), one = (f32)*at<be<f32>>(One),
                  tiny = (f32)*at<be<f32>>(SprayMinimumStep), fifth = (f32)*at<be<f32>>(Rate),
                  half = (f32)*at<be<f32>>(Half), five = (f32)*at<be<f32>>(PoisonRapidRise),
                  ten = (f32)*at<be<f32>>(PoisonWindGain), push = (f32)*at<be<f32>>(PoisonInfluenceGain),
                  limit = (f32)*at<be<f32>>(PoisonInfluenceLimit),
                  decay = (f32)*at<be<f32>>(PoisonInfluenceDecay),
                  far = (f32)*at<be<f32>>(PoisonForwardEyeDistance);
        frame->middleLowerZ.value = fadds_ppc(size, (f32)*at<be<f32>>(PoisonMiddleLowerZ));
        frame->hallLowerZ.value = fadds_ppc(size, (f32)*at<be<f32>>(PoisonHallLowerZ));
        frame->corridorUpperX.value = fsubs_ppc((f32)*at<be<f32>>(PoisonCorridorUpperX), size);
        frame->middleUpperZ.value = fsubs_ppc((f32)*at<be<f32>>(PoisonMiddleUpperZ), size);
        frame->corridorLowerX.value = fadds_ppc(size, (f32)*at<be<f32>>(PoisonCorridorLowerX));
        frame->edgeWidth.value = (f32)*at<be<f32>>(PoisonEdgeWidth);
        frame->outerLowerZ.value = fadds_ppc(size, (f32)*at<be<f32>>(PoisonOuterLowerZ));
        frame->hallUpperZ.value = fsubs_ppc((f32)*at<be<f32>>(PoisonHallUpperZ), size);
        frame->phaseStep.value = (f32)*at<be<f32>>(AlphaStep);
        frame->playerRadiusScale.value = (f32)*at<be<f32>>(PoisonPlayerRadiusScale);
        frame->sizeWaveAmplitude.value = fmuls_ppc(size, fifth);
        frame->nearDistance.value = (f32)*at<be<f32>>(PoisonNearFadeDistance);
        for (s32 i = 0; i < count; i++) {
            u32 slot = ea(&at<PoisonPacket>(packet)->mEff[i]), state = (u8)at<POISON_EFF>(slot)->mStatus;
            rangeZ = (f32)frame->rangeZ.value;
            rangeX = (f32)frame->rangeX.value;
            rangeY = (f32)frame->rangeY.value;
            if (!state) {
                at<POISON_EFF>(slot)->mSize = zero;
                if (pattern == PoisonCircular) {
                    u32 x = (u16)ftoi(sdk::cM_rndFX<f32>((f32)*at<be<f32>>(PoisonAngleRange)));
                    u32 z = (u16)ftoi(sdk::cM_rndFX<f32>((f32)*at<be<f32>>(PoisonAngleRange)));
                    at<POISON_EFF>(slot)->mPos.channels[0] = fmuls_ppc(poisonSine(x), rangeX);
                    at<POISON_EFF>(slot)->mPos.channels[2] = fmuls_ppc(poisonCosine(z), rangeX);
                    at<POISON_EFF>(slot)->mPos.channels[1] = sdk::cM_rndF<f32>(rangeY);
                } else {
                    at<POISON_EFF>(slot)->mPos.channels[0] = sdk::cM_rndFX<f32>(rangeX);
                    at<POISON_EFF>(slot)->mPos.channels[1] = sdk::cM_rndF<f32>(rangeY);
                    at<POISON_EFF>(slot)->mPos.channels[2] = sdk::cM_rndFX<f32>(rangeZ);
                }
                at<POISON_EFF>(slot)->field_0x10 = zero;
                at<POISON_EFF>(slot)->field_0x14 = zero;
                at<POISON_EFF>(slot)->field_0x18 = zero;
                at<POISON_EFF>(slot)->field_0x1c =
                    sdk::cM_rndF<f32>((f32)*at<be<f32>>(PoisonInitialPhaseRange));
                at<POISON_EFF>(slot)->field_0x20 = sdk::cM_rndF<f32>(one);
                f32 angle = sdk::cM_rndF<f32>((f32)*at<be<f32>>(PoisonPhaseRange));
                u8 old = (u8)at<POISON_EFF>(slot)->mStatus;
                at<POISON_EFF>(slot)->mAlpha = zero;
                at<POISON_EFF>(slot)->field_0x2c = (u16)ftoi(angle);
                at<POISON_EFF>(slot)->field_0x2e = 0;
                at<POISON_EFF>(slot)->mStatus = old + 1;
            } else if (state <= ParticleSettled) {
                f32 swirl = state == ParticleMoving ? (f32)*at<be<f32>>(PoisonSwirlBase) : ten,
                    up = state == ParticleMoving ? (f32)frame->rise.value : five;
                frame->swirl.value = swirl;
                at<POISON_EFF>(slot)->field_0x2c = (s16)at<POISON_EFF>(slot)->field_0x2c + 400;
                f32 random = (f32)at<POISON_EFF>(slot)->field_0x20,
                    dy = fmuls_ppc(up, fadds_ppc(random, half)),
                    dx = fmuls_ppc((f32)frame->wind.channels[0], ten),
                    dz = fmuls_ppc((f32)frame->wind.channels[2], ten);
                f32 maxY = fmuls_ppc(rangeY, fmadds(random, (f32)frame->playerRadiusScale.value, one));
                dx = fmadds(fmuls_ppc(dx, half), random, dx);
                dy = fmadds(fmuls_ppc(dy, half), random, dy);
                dz = fmadds(fmuls_ppc(dz, half), random, dz);
                frame->windZ.value = dz;
                sdk::vectorAdd<void>(base, ea(&frame->influence), ea(&at<POISON_EFF>(slot)->mPos));
                frame->total.words[0] = (u32)frame->influence.words[0];
                frame->total.channels[1] =
                    fadds_ppc((f32)frame->influence.channels[1], (f32)*at<be<f32>>(PoisonInfluenceHeight));
                frame->total.words[2] = (u32)frame->influence.words[2];
                sdk::dKyw_get_AllWind_vec<void>(ea(&frame->influence), ea(&frame->total));
                f32 windY = (f32)frame->influence.channels[1], windX = (f32)frame->influence.channels[0],
                    windZ = (f32)frame->influence.channels[2];
                frame->windY.value = windY;
                f32 px = fmadds(windX, push, (f32)at<POISON_EFF>(slot)->field_0x10);
                at<POISON_EFF>(slot)->field_0x10 = px;
                f32 py = fmadds((f32)frame->windY.value, push, (f32)at<POISON_EFF>(slot)->field_0x14);
                f32 keptX = (f32)at<POISON_EFF>(slot)->field_0x10, pz = (f32)at<POISON_EFF>(slot)->field_0x18;
                if (py < zero)
                    py = zero;
                pz = fmadds(windZ, push, pz);
                at<POISON_EFF>(slot)->field_0x14 = py;
                at<POISON_EFF>(slot)->field_0x18 = pz;
                if (keptX > limit || (f32)at<POISON_EFF>(slot)->field_0x14 > limit || pz > limit)
                    at<POISON_EFF>(slot)->field_0x2e = 180;
                for (be<f32> *motion : {&at<POISON_EFF>(slot)->field_0x10, &at<POISON_EFF>(slot)->field_0x14,
                                        &at<POISON_EFF>(slot)->field_0x18})
                    sdk::cLib_addCalc<void>(ea(motion), zero, fifth, decay, tiny);
                dy = fadds_ppc(dy, (f32)at<POISON_EFF>(slot)->field_0x14);
                dz = fadds_ppc((f32)frame->windZ.value, (f32)at<POISON_EFF>(slot)->field_0x18);
                dx = fadds_ppc(dx, (f32)at<POISON_EFF>(slot)->field_0x10);
                f32 z = fadds_ppc((f32)at<POISON_EFF>(slot)->mPos.channels[2], dz),
                    x = fadds_ppc((f32)at<POISON_EFF>(slot)->mPos.channels[0], dx),
                    y = fadds_ppc((f32)at<POISON_EFF>(slot)->mPos.channels[1], dy);
                at<POISON_EFF>(slot)->mPos.channels[0] = x;
                at<POISON_EFF>(slot)->mPos.channels[1] = y;
                at<POISON_EFF>(slot)->mPos.channels[2] = z;
                frame->windZ.value = dz;
                f32 cosine = poisonCosine(tilt), sine = poisonSine(tilt),
                    horizontal = fmuls_ppc(fmuls_ppc(cosine, poisonSine(heading)), drift),
                    vertical = fmuls_ppc(sine, drift);
                random = (f32)at<POISON_EFF>(slot)->field_0x20;
                y = fmadds(vertical, five, y);
                x = fmadds(horizontal, five, x);
                f32 hz = fmuls_ppc(fmuls_ppc(cosine, poisonCosine(heading)), drift);
                u32 angle = (u16)ftoi(fmuls_ppc(random, (f32)*at<be<f32>>(PoisonSwirlAngleScale)));
                z = fmadds(hz, five, z);
                at<POISON_EFF>(slot)->mPos.channels[0] = x;
                at<POISON_EFF>(slot)->mPos.channels[1] = y;
                at<POISON_EFF>(slot)->mPos.channels[2] = z;
                f32 amplitude = fmuls_ppc(random, (f32)frame->swirl.value);
                x = fmadds(poisonSine(angle), amplitude, x);
                z = fmadds(poisonCosine(angle), amplitude, z);
                at<POISON_EFF>(slot)->mPos.channels[0] = x;
                at<POISON_EFF>(slot)->mPos.channels[2] = z;
                if (!(s16)at<POISON_EFF>(slot)->field_0x2e) {
                    bool wrap = false;
                    if (pattern == PoisonCircular) {
                        frame->radialPosition.channels[0] =
                            fadds_ppc((f32)(*at<Vector>(base)).channels[0], x);
                        frame->radialPosition.channels[1] = (f32)(*at<Vector>(base)).channels[1];
                        frame->radialPosition.channels[2] =
                            fadds_ppc((f32)(*at<Vector>(base)).channels[2],
                                      (f32)at<POISON_EFF>(slot)->mPos.channels[2]);
                        f32 distance = poisonDistance(ea(&frame->radialPosition), base);
                        if (!(distance < rangeX)) {
                            at<POISON_EFF>(slot)->mPos.channels[0] =
                                -fmuls_ppc((f32)at<POISON_EFF>(slot)->mPos.channels[0],
                                           (f32)*at<be<f32>>(PoisonWrapScale));
                            at<POISON_EFF>(slot)->mPos.channels[2] =
                                -fmuls_ppc((f32)at<POISON_EFF>(slot)->mPos.channels[2],
                                           (f32)*at<be<f32>>(PoisonWrapScale));
                            wrap = true;
                        }
                    } else if (pattern == PoisonHall) {
                        f32 worldX = fadds_ppc((f32)(*at<Vector>(base)).channels[0], x),
                            worldZ = fadds_ppc((f32)(*at<Vector>(base)).channels[2], z);
                        if (x > rangeX) {
                            at<POISON_EFF>(slot)->mPos.channels[0] = -rangeX;
                            wrap = true;
                        } else if (x < -rangeX) {
                            at<POISON_EFF>(slot)->mPos.channels[0] = rangeX;
                            wrap = true;
                        }
                        if (worldX > (f32)*at<be<f32>>(PoisonOuterBoundaryX)) {
                            if (worldZ < (f32)frame->outerLowerZ.value) {
                                at<POISON_EFF>(slot)->mPos.channels[2] = fsubs_ppc(
                                    (f32)frame->outerLowerZ.value, (f32)(*at<Vector>(base)).channels[2]);
                                wrap = true;
                            } else if (worldZ > (f32)frame->middleUpperZ.value) {
                                at<POISON_EFF>(slot)->mPos.channels[2] = fsubs_ppc(
                                    (f32)frame->middleUpperZ.value, (f32)(*at<Vector>(base)).channels[2]);
                                wrap = true;
                            }
                        } else if (worldX > (f32)*at<be<f32>>(PoisonMiddleBoundaryX)) {
                            if (worldZ < (f32)frame->middleLowerZ.value) {
                                at<POISON_EFF>(slot)->mPos.channels[2] = fsubs_ppc(
                                    (f32)frame->middleLowerZ.value, (f32)(*at<Vector>(base)).channels[2]);
                                wrap = true;
                            } else if (worldZ > (f32)frame->middleUpperZ.value) {
                                at<POISON_EFF>(slot)->mPos.channels[2] = fsubs_ppc(
                                    (f32)frame->middleUpperZ.value, (f32)(*at<Vector>(base)).channels[2]);
                                wrap = true;
                            }
                        } else if (worldX < (f32)*at<be<f32>>(PoisonHallMaximumX) &&
                                   worldX > (f32)*at<be<f32>>(PoisonHallMinimumX)) {
                            if (worldZ < (f32)frame->hallLowerZ.value) {
                                at<POISON_EFF>(slot)->mPos.channels[2] = fsubs_ppc(
                                    (f32)frame->hallLowerZ.value, (f32)(*at<Vector>(base)).channels[2]);
                                wrap = true;
                            } else if (worldZ > (f32)frame->hallUpperZ.value) {
                                at<POISON_EFF>(slot)->mPos.channels[2] = fsubs_ppc(
                                    (f32)frame->hallUpperZ.value, (f32)(*at<Vector>(base)).channels[2]);
                                wrap = true;
                            }
                        } else if ((f32)at<POISON_EFF>(slot)->mPos.channels[2] > rangeZ) {
                            at<POISON_EFF>(slot)->mPos.channels[2] = rangeZ;
                            wrap = true;
                        } else if ((f32)at<POISON_EFF>(slot)->mPos.channels[2] < -rangeZ) {
                            at<POISON_EFF>(slot)->mPos.channels[2] = -rangeZ;
                            wrap = true;
                        }
                        if (wrap)
                            at<POISON_EFF>(slot)->field_0x20 = sdk::cM_rndF<f32>(one);
                    } else if (pattern == PoisonCorridor) {
                        f32 worldX = fadds_ppc((f32)(*at<Vector>(base)).channels[0], x),
                            worldZ = fadds_ppc((f32)(*at<Vector>(base)).channels[2], z);
                        if (z > rangeZ) {
                            at<POISON_EFF>(slot)->mPos.channels[2] = -rangeZ;
                            wrap = true;
                        } else if (z < -rangeZ) {
                            at<POISON_EFF>(slot)->mPos.channels[2] = rangeZ;
                            wrap = true;
                        }
                        if (worldZ > (f32)*at<be<f32>>(PoisonCorridorBoundaryZ)) {
                            if (worldX < (f32)frame->corridorLowerX.value) {
                                at<POISON_EFF>(slot)->mPos.channels[0] = fsubs_ppc(
                                    (f32)frame->corridorLowerX.value, (f32)(*at<Vector>(base)).channels[0]);
                                wrap = true;
                            } else if (worldX > (f32)frame->corridorUpperX.value) {
                                at<POISON_EFF>(slot)->mPos.channels[0] = fsubs_ppc(
                                    (f32)frame->corridorUpperX.value, (f32)(*at<Vector>(base)).channels[0]);
                                wrap = true;
                            }
                        } else if ((f32)at<POISON_EFF>(slot)->mPos.channels[0] > rangeX) {
                            at<POISON_EFF>(slot)->mPos.channels[0] = rangeX;
                            wrap = true;
                        } else if ((f32)at<POISON_EFF>(slot)->mPos.channels[0] < -rangeX) {
                            at<POISON_EFF>(slot)->mPos.channels[0] = -rangeX;
                            wrap = true;
                        }
                        if (wrap)
                            at<POISON_EFF>(slot)->field_0x20 = sdk::cM_rndF<f32>(one);
                    } else {
                        if (x > rangeX) {
                            at<POISON_EFF>(slot)->mPos.channels[0] = -rangeX;
                            wrap = true;
                            z = (f32)at<POISON_EFF>(slot)->mPos.channels[2];
                        } else if (x < -rangeX) {
                            at<POISON_EFF>(slot)->mPos.channels[0] = rangeX;
                            wrap = true;
                            z = (f32)at<POISON_EFF>(slot)->mPos.channels[2];
                        }
                        if (z > rangeZ) {
                            at<POISON_EFF>(slot)->mPos.channels[2] = -rangeZ;
                            wrap = true;
                        } else if (z < -rangeZ) {
                            at<POISON_EFF>(slot)->mPos.channels[2] = rangeZ;
                            wrap = true;
                        }
                    }
                    y = (f32)at<POISON_EFF>(slot)->mPos.channels[1];
                    if (y > maxY && !((f32)at<POISON_EFF>(slot)->mAlpha > tiny))
                        at<POISON_EFF>(slot)->mPos.channels[1] = zero;
                    else if (y < zero && !((f32)at<POISON_EFF>(slot)->mAlpha > tiny))
                        at<POISON_EFF>(slot)->mPos.channels[1] = maxY;
                    if (wrap) {
                        at<POISON_EFF>(slot)->field_0x2e = 120;
                        at<POISON_EFF>(slot)->mAlpha = zero;
                    }
                }
                rangeY = maxY;
            }
            poisonTotal(ea(&frame->total), base, slot);
            sdk::dKyr_get_vectle_calc<void>(ea(&at<Camera>(camera)->mEye), ea(&at<Camera>(camera)->mCenter),
                                            ea(&frame->viewDirection));
            if (pattern == PoisonForwardEye) {
                for (u32 component = 0; component < 3; component++)
                    frame->eye.channels[component] =
                        fmadds((f32)frame->viewDirection.channels[component], far,
                               (f32)at<Camera>(camera)->mEye.channels[component]);
            } else
                poisonCopy(ea(&frame->eye), ea(&at<Camera>(camera)->mEye));
            f32 distance = poisonDistance(ea(&frame->total), ea(&frame->eye));
            if (distance < (f32)frame->closestDistance.value) {
                poisonCopy(ea(&at<PoisonPacket>(packet)->field_0xbb9c), ea(&frame->total));
                frame->closestDistance.value = distance;
            }
            f32 ratio = distance / fadds_ppc(rangeX, rangeZ);
            if (ratio > one)
                ratio = one;
            u32 angle = sdk::cM_deg2s<u32>((f32)at<POISON_EFF>(slot)->field_0x1c);
            f32 phase = (f32)at<POISON_EFF>(slot)->field_0x1c,
                y = (f32)at<POISON_EFF>(slot)->mPos.channels[1], upper = fmuls_ppc(rangeY, half);
            f32 newSize =
                fmadds((f32)frame->sizeWaveAmplitude.value, poisonSine(angle), fmadds(size, ratio, size));
            at<POISON_EFF>(slot)->field_0x1c = fadds_ppc(phase, (f32)frame->phaseStep.value);
            at<POISON_EFF>(slot)->mSize = newSize;
            f32 verticalAlpha;
            if (y > upper)
                verticalAlpha = fsubs_ppc(rangeY, y) / upper;
            else {
                f32 lower = fmuls_ppc(rangeY, (f32)*at<be<f32>>(Drag));
                verticalAlpha = y < lower ? y / lower : one;
            }
            f32 radial = one;
            if (pattern == PoisonCircular) {
                poisonCopy(ea(&frame->flatPosition), ea(&frame->total));
                frame->flatPosition.channels[1] = (f32)(*at<Vector>(base)).channels[1];
                ratio = poisonDistance(ea(&frame->flatPosition), base) / rangeX;
                if (ratio > one)
                    ratio = one;
                f32 square = fmuls_ppc(ratio, ratio), cube = fmuls_ppc(square, ratio),
                    fourth = fmuls_ppc(cube, ratio);
                radial = fnmsubs(ratio, fourth, one);
                newSize = (f32)at<POISON_EFF>(slot)->mSize;
            } else {
                f32 edgeX = fsubs_ppc(rangeX, (f32)frame->edgeWidth.value),
                    x = std::fabs((f32)at<POISON_EFF>(slot)->mPos.channels[0]);
                if (x > edgeX) {
                    f32 span = fsubs_ppc(rangeX, edgeX);
                    radial = span > zero ? fsubs_ppc(rangeX, x) / span : zero;
                }
                f32 edgeZ = fsubs_ppc(rangeZ, (f32)frame->edgeWidth.value),
                    z = std::fabs((f32)at<POISON_EFF>(slot)->mPos.channels[2]);
                if (z > edgeZ) {
                    f32 span = fsubs_ppc(rangeZ, edgeZ);
                    radial = span > zero ? fmuls_ppc(radial, fsubs_ppc(rangeZ, z) / span) : zero;
                }
            }
            at<POISON_EFF>(slot)->mSize = fmuls_ppc(newSize, radial);
            f32 deficit = fsubs_ppc(one, verticalAlpha);
            verticalAlpha = fnmsubs(deficit, deficit, one);
            s32 timer = (s16)at<POISON_EFF>(slot)->field_0x2e;
            if (timer) {
                at<POISON_EFF>(slot)->field_0x2e = timer - 1;
                verticalAlpha = zero;
            } else if ((u8)at<POISON_EFF>(slot)->mStatus == 2)
                at<POISON_EFF>(slot)->mStatus = ParticleInactive;
            if (sdk::dKy_contrast_flg_get<u32>())
                verticalAlpha = zero;
            sdk::cLib_addCalc<void>(ea(&at<POISON_EFF>(slot)->mAlpha),
                                    fmuls_ppc(fmuls_ppc(fifth, verticalAlpha), radial),
                                    (f32)frame->phaseStep.value, (f32)*at<be<f32>>(Increment), tiny);
            f32 near = (f32)*at<be<f32>>(PoisonInfluenceHeight);
            if (pattern == PoisonForwardEye)
                poisonCopy(ea(&frame->eye), ea(&at<Camera>(camera)->mEye));
            else if (pattern == PoisonCorridor)
                near = (f32)frame->nearDistance.value;
            distance = poisonDistance(ea(&frame->total), ea(&frame->eye));
            if (distance < near) {
                ratio = distance / (f32)frame->nearDistance.value;
                if (ratio > one)
                    ratio = one;
                at<POISON_EFF>(slot)->mAlpha =
                    fmuls_ppc((f32)at<POISON_EFF>(slot)->mAlpha, fmuls_ppc(fmuls_ppc(ratio, ratio), ratio));
            }
            u32 current = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mPlayer,
                hero = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mLinkPlayer;
            if (current == hero &&
                (f32)at<POISON_EFF>(slot)->mAlpha > (f32)*at<be<f32>>(FlareMinimumVisibility) &&
                !(s16)at<POISON_EFF>(slot)->field_0x2e) {
                frame->total.channels[1] =
                    fsubs_ppc((f32)frame->total.channels[1], (f32)*at<be<f32>>(AshOffsetStep));
                distance = poisonDistance(ea(&frame->total), ea(&at<PoisonPlayer>(player)->mCurrentPos));
                if (distance <
                    fmuls_ppc((f32)at<POISON_EFF>(slot)->mSize, (f32)frame->playerRadiusScale.value))
                    at<PoisonPlayer>(link)->mResetFlg0 =
                        (u32)at<PoisonPlayer>(link)->mResetFlg0 | PoisonCurseFlag;
            }
            count = (u32)at<Environment>(sdk::getEnvironment<u32>())->mPoisonCount;
        }
    }
    packet = (u32)at<Environment>(sdk::getEnvironment<u32>())->mpPoisonPacket;
    at<PoisonPacket>(packet)->mCount = (u32)at<PoisonPacket>(packet)->mCount + 1;
}
} // namespace RainMotion
using namespace RainMotion;
/* WWHD glue (tools/verify/autoglue.py)
 */
