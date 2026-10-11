// HD: volcanic ash shares the snow packet and preserves the two collision-check lifetimes.
#include "d/d_kankyo_rain_kazanbai_move.h"
namespace RainMotion {
/* 02569408 */
void dKyr_kazanbai_move() {
    u32 env = sdk::getEnvironment<u32>(), packet = (u32)at<Environment>(env)->mpSnowPacket,
        game = sdk::getGameInfo<u32>(), camera = (u32)at<GameInfo>(game)->mCamera;
    sdk::getGameInfo();
    Local<AshMotionWork> work;
    u32 absolute = ea(&work->absolute), center = ea(&work->center), wind = ea(&work->wind),
        direction = ea(&work->direction), wind2 = ea(&work->wind2), viewDirection = ea(&work->viewDirection),
        doublePos = ea(&work->doublePos), ground = ea(&work->ground), ground2 = ea(&work->ground2);
    sdk::dKyw_get_wind_vecpow(at<void>(wind));
    constructGround(&work->ground, work.get());
    env = sdk::getEnvironment<u32>();
    s32 count = (s16)at<SnowPacket>(packet)->mEffCount, wanted = (s32)at<Environment>(env)->mSnowCount.word;
    bool execute;
    if (count > wanted) {
        execute = (s16)at<SnowPacket>(packet)->mEffCount != 0;
    } else {
        env = sdk::getEnvironment<u32>();
        count = (s16)at<Environment>(env)->mSnowCount.halves.lower;
        at<SnowPacket>(packet)->mEffCount = (u16)count;
        execute = count != 0;
    }
    if (execute) {
        at<GroundCheck>(ground)->base.m_pos.y = (f32)*at<be<f32>>(AshGroundY);
        at<GroundCheck>(ground)->base.m_pos.x = (f32)*at<be<f32>>(AshGroundX);
        at<GroundCheck>(ground)->base.m_pos.z = (f32)*at<be<f32>>(AshGroundZ);
        game = sdk::getGameInfo<u32>();
        f32 height = sdk::groundCross<f32>(at<void>(ea(at<GameInfo>(game)->mBgSpace)), at<void>(ground));
        f32 radius = (f32)*at<be<f32>>(AshScatterRadius);
        sdk::dKy_set_eyevect_calc2(at<void>(camera), at<void>(center), radius, radius);
        if (ashDistance(ea(&at<SnowPacket>(packet)->mOldEyePos), ea(&at<Camera>(camera)->mEye)) >
            (f32)*at<be<f32>>(AshEyeResetDistance))
            at<SnowPacket>(packet)->mEffCount = 0;
        at<SnowPacket>(packet)->mOldEyePos.words[0] = (u32)at<Camera>(camera)->mEye.words[0];
        at<SnowPacket>(packet)->mOldEyePos.words[1] = (u32)at<Camera>(camera)->mEye.words[1];
        at<SnowPacket>(packet)->mOldEyePos.words[2] = (u32)at<Camera>(camera)->mEye.words[2];
        u32 windVector = sdk::dKyw_get_wind_vec<u32>();
        f32 power = sdk::dKyw_get_wind_pow<f32>();
        at<DOUBLE_POS>(doublePos)->y = (f64)*at<be<f64>>(DoubleZero);
        at<DOUBLE_POS>(doublePos)->x = (f64)fsubs_ppc((f32)at<Camera>(camera)->mCenter.channels[0],
                                                      (f32)at<Camera>(camera)->mEye.channels[0]);
        at<DOUBLE_POS>(doublePos)->z = (f64)fsubs_ppc((f32)at<Camera>(camera)->mCenter.channels[2],
                                                      (f32)at<Camera>(camera)->mEye.channels[2]);
        sdk::vectle_calc(at<void>(doublePos), at<void>(viewDirection));
        f32 zero = (f32)*at<be<f32>>(Zero);
        work->groundVtableOrZero.value = zero;
        f32 cross = sdk::cM3d_VectorProduct2d<f32>(zero, zero, -(f32)(*at<Vector>(windVector)).channels[0],
                                                   -(f32)(*at<Vector>(windVector)).channels[2],
                                                   (f32)(*at<Vector>(viewDirection)).channels[0],
                                                   (f32)(*at<Vector>(viewDirection)).channels[2]);
        at<SnowPacket>(packet)->field_0x36d4 = cross;
        f32 dot = fmadds((f32)(*at<Vector>(windVector)).channels[0],
                         (f32)(*at<Vector>(viewDirection)).channels[0],
                         fmuls_ppc((f32)(*at<Vector>(windVector)).channels[2],
                                   (f32)(*at<Vector>(viewDirection)).channels[2])),
            one = (f32)*at<be<f32>>(One), atten = fsubs_ppc(one, std::fabs(dot));
        s32 initial = (s16)at<SnowPacket>(packet)->mEffCount;
        work->one = one;
        work->angleRange = (f32)*at<be<f32>>(AshAngleRange);
        at<SnowPacket>(packet)->field_0x36d0 =
            fmuls_ppc(fmuls_ppc(fmuls_ppc(atten, power),
                                fsubs_ppc(one, std::fabs((f32)(*at<Vector>(windVector)).channels[1]))),
                      std::fabs(cross));
        if (initial > 0) {
            one = (f32)work->one;
            zero = (f32)work->groundVtableOrZero.value;
            f32 half = (f32)*at<be<f32>>(Half), gravityBase = (f32)*at<be<f32>>(AshGravityBase),
                speedBase = (f32)*at<be<f32>>(AshWindBase), speedRange = (f32)*at<be<f32>>(AshWindRange),
                rate = (f32)*at<be<f32>>(Drag), epsilon = (f32)*at<be<f32>>(Increment),
                angleRange = (f32)work->angleRange, waveStep = (f32)*at<be<f32>>(AshWaveStep),
                settledRate = (f32)*at<be<f32>>(AshSettledDrift), fade = (f32)*at<be<f32>>(Rate),
                gravityRange = (f32)*at<be<f32>>(AshGravityRange), bias = (f32)*at<be<f32>>(AshDistanceBias);
            work->minimumStep = (f32)*at<be<f32>>(AshMinimumFadeStep);
            bool positive = height > zero;
            u32 settled = 0;
            for (s32 i = initial - 1; i >= 0; i--) {
                f32 gravity = -fadds_ppc(sdk::cM_rndFX<f32>(gravityRange), gravityBase);
                f32 speed = fadds_ppc(sdk::cM_rndF<f32>(speedRange), speedBase);
                u32 p = ea(&at<SnowPacket>(packet)->mEff[(u32)i]);
                u8 state = (u8)at<SNOW_EFF>(p)->mStatus;
                if (state == ParticleInactive) {
                    at<SNOW_EFF>(p)->mWindSpeed = speed;
                    at<SNOW_EFF>(p)->mGravity = gravity;
                    at<SNOW_EFF>(p)->mTimer = 0;
                    at<SNOW_EFF>(p)->mBasePos.channels[0] = (f32)(*at<Vector>(center)).channels[0];
                    at<SNOW_EFF>(p)->mBasePos.channels[1] = (f32)(*at<Vector>(center)).channels[1];
                    at<SNOW_EFF>(p)->mBasePos.channels[2] = (f32)(*at<Vector>(center)).channels[2];
                    at<SNOW_EFF>(p)->mPos.channels[0] = sdk::cM_rndFX<f32>(radius);
                    at<SNOW_EFF>(p)->mPos.channels[1] = radius;
                    at<SNOW_EFF>(p)->mPos.channels[2] = sdk::cM_rndFX<f32>(radius);
                    at<SNOW_EFF>(p)->mScale = zero;
                    at<SNOW_EFF>(p)->mPosWaveX = sdk::cM_rndF<f32>(angleRange);
                    f32 angle = sdk::cM_rndF<f32>(angleRange);
                    at<SNOW_EFF>(p)->mStatus = (u8)at<SNOW_EFF>(p)->mStatus + 1;
                    at<SNOW_EFF>(p)->mPosWaveZ = angle;
                } else if (state == ParticleMoving) {
                    f32 random = sdk::cM_rndFX<f32>(rate);
                    sdk::cLib_addCalc(at<void>(ea(&at<SNOW_EFF>(p)->mWindSpeed)),
                                      fsubs_ppc((f32)at<SNOW_EFF>(p)->mWindSpeed, random), half, rate,
                                      epsilon);
                    f32 wx = (f32)(*at<Vector>(wind)).channels[0], ws = (f32)at<SNOW_EFF>(p)->mWindSpeed,
                        px = (f32)at<SNOW_EFF>(p)->mPos.channels[0],
                        pz = (f32)at<SNOW_EFF>(p)->mPos.channels[2];
                    at<SNOW_EFF>(p)->mPos.channels[0] = fmadds(wx, ws, px);
                    at<SNOW_EFF>(p)->mPos.channels[2] = fmadds((f32)(*at<Vector>(wind)).channels[2], ws, pz);
                    f32 gy = (f32)at<SNOW_EFF>(p)->mGravity, wy = (f32)(*at<Vector>(wind)).channels[1],
                        py = (f32)at<SNOW_EFF>(p)->mPos.channels[1];
                    f32 dy = fmadds(wy, ws, gy);
                    f32 ax = (f32)at<SNOW_EFF>(p)->mPosWaveX;
                    at<SNOW_EFF>(p)->mPos.channels[1] = fadds_ppc(py, dy);
                    u32 angle = sdk::cM_deg2s<u32>(ax);
                    f32 x = (f32)at<SNOW_EFF>(p)->mPos.channels[0];
                    at<SNOW_EFF>(p)->mPos.channels[0] =
                        fmadds((f32)at<SinCos>(SinCosTable)[((angle & 0xFFFF) >> 3)].sine, gravityBase, x);
                    angle = sdk::cM_deg2s<u32>((f32)at<SNOW_EFF>(p)->mPosWaveZ);
                    f32 z = (f32)at<SNOW_EFF>(p)->mPos.channels[2],
                        three = (f32)*at<be<f32>>(AshVerticalWaveScale);
                    at<SNOW_EFF>(p)->mPos.channels[2] =
                        fmadds((f32)at<SinCos>(SinCosTable)[((angle & 0xFFFF) >> 3)].sine, three, z);
                    angle = sdk::cM_deg2s<u32>((f32)at<SNOW_EFF>(p)->mPosWaveZ);
                    f32 az = (f32)at<SNOW_EFF>(p)->mPosWaveZ, y = (f32)at<SNOW_EFF>(p)->mPos.channels[1],
                        sin = (f32)at<SinCos>(SinCosTable)[((angle & 0xFFFF) >> 3)].sine,
                        oldAx = (f32)at<SNOW_EFF>(p)->mPosWaveX;
                    f32 x0 = (f32)at<SNOW_EFF>(p)->mPos.channels[0],
                        bx = (f32)at<SNOW_EFF>(p)->mBasePos.channels[0];
                    at<SNOW_EFF>(p)->mPos.channels[1] = fadds_ppc(y, sin);
                    at<SNOW_EFF>(p)->mPosWaveX = fadds_ppc(oldAx, waveStep);
                    at<SNOW_EFF>(p)->mPosWaveZ = fadds_ppc(az, waveStep);
                    (*at<Vector>(absolute)).channels[0] = fadds_ppc(bx, x0);
                    (*at<Vector>(absolute)).channels[1] = fadds_ppc(
                        (f32)at<SNOW_EFF>(p)->mBasePos.channels[1], (f32)at<SNOW_EFF>(p)->mPos.channels[1]);
                    (*at<Vector>(absolute)).channels[2] = fadds_ppc(
                        (f32)at<SNOW_EFF>(p)->mBasePos.channels[2], (f32)at<SNOW_EFF>(p)->mPos.channels[2]);
                    f32 dist = ashDistance(absolute, center);
                    s16 timer = (s16)at<SNOW_EFF>(p)->mTimer;
                    if (!timer) {
                        if (dist > radius) {
                            at<SNOW_EFF>(p)->mWindSpeed = speed;
                            at<SNOW_EFF>(p)->mGravity = gravity;
                            at<SNOW_EFF>(p)->mTimer = 10;
                            at<SNOW_EFF>(p)->mBasePos.channels[0] = (f32)(*at<Vector>(center)).channels[0];
                            at<SNOW_EFF>(p)->mBasePos.channels[1] = (f32)(*at<Vector>(center)).channels[1];
                            at<SNOW_EFF>(p)->mBasePos.channels[2] = (f32)(*at<Vector>(center)).channels[2];
                            if (ashDistance(absolute, center) > (f32)*at<be<f32>>(AshScatterResetDistance)) {
                                at<SNOW_EFF>(p)->mPos.channels[0] = sdk::cM_rndFX<f32>(radius);
                                at<SNOW_EFF>(p)->mPos.channels[1] = sdk::cM_rndFX<f32>(radius);
                                at<SNOW_EFF>(p)->mPos.channels[2] = sdk::cM_rndFX<f32>(radius);
                                at<SNOW_EFF>(p)->mStatus = ParticleMoving;
                            } else {
                                f32 random = sdk::cM_rndFX<f32>((f32)*at<be<f32>>(AshShellJitter));
                                sdk::dKyr_get_vectle_calc2(at<void>(absolute), at<void>(center),
                                                           at<void>(direction));
                                f32 shell = fadds_ppc(radius, random);
                                at<SNOW_EFF>(p)->mPos.channels[0] =
                                    fmuls_ppc((f32)(*at<Vector>(direction)).channels[0], shell);
                                at<SNOW_EFF>(p)->mPos.channels[1] =
                                    fmuls_ppc((f32)(*at<Vector>(direction)).channels[1], shell);
                                at<SNOW_EFF>(p)->mStatus = ParticleMoving;
                                at<SNOW_EFF>(p)->mPos.channels[2] =
                                    fmuls_ppc((f32)(*at<Vector>(direction)).channels[2], shell);
                            }
                        }
                    } else
                        at<SNOW_EFF>(p)->mTimer = (u16)(timer - 1);
                    if (positive && (f32)(*at<Vector>(absolute)).channels[1] < height) {
                        if (settled < 100 && ((u32)*at<be<u32>>(FrameCounter) & 7) == ((u32)i & 7)) {
                            at<SNOW_EFF>(p)->mScale = epsilon;
                            (*at<Vector>(absolute)).channels[1] = height;
                            at<SNOW_EFF>(p)->mTimer = ((u32)i & 15) + 20;
                            at<SNOW_EFF>(p)->mStatus = ParticleSettled;
                            settled++;
                        } else
                            at<SNOW_EFF>(p)->mStatus = ParticleInactive;
                    }
                } else if (state == ParticleSettled) {
                    f32 speed = (f32)at<SNOW_EFF>(p)->mWindSpeed, wx = (f32)(*at<Vector>(wind)).channels[0],
                        scaled = fmuls_ppc(speed, settledRate), x = (f32)at<SNOW_EFF>(p)->mPos.channels[0],
                        baseY = (f32)at<SNOW_EFF>(p)->mBasePos.channels[1],
                        alpha = (f32)at<SNOW_EFF>(p)->mScale, z = (f32)at<SNOW_EFF>(p)->mPos.channels[2],
                        dy = fsubs_ppc(height, baseY);
                    at<SNOW_EFF>(p)->mPos.channels[0] = fmadds(wx, scaled, x);
                    f32 wz = (f32)(*at<Vector>(wind)).channels[2], delta = fsubs_ppc(one, alpha),
                        seven = (f32)*at<be<f32>>(AshSettledHeight);
                    s16 timer = (s16)at<SNOW_EFF>(p)->mTimer;
                    at<SNOW_EFF>(p)->mPos.channels[2] = fmadds(wz, scaled, z);
                    at<SNOW_EFF>(p)->mPos.channels[1] = fnmsubs(delta, seven, dy);
                    if (timer)
                        at<SNOW_EFF>(p)->mTimer = (u16)(timer - 1);
                    if (!((f32)at<SNOW_EFF>(p)->mScale > (f32)*at<be<f32>>(SprayMinimumStep)))
                        at<SNOW_EFF>(p)->mStatus = ParticleInactive;
                    settled++;
                }
                absolutePosition(absolute, p);
                f32 d = fadds_ppc(ashDistance(absolute, center), bias);
                if (!(d >= 0))
                    d = zero;
                f32 alpha = fsubs_ppc(one, d / radius);
                if (alpha > one)
                    alpha = one;
                else if (!(alpha >= 0))
                    alpha = zero;
                env = sdk::getEnvironment<u32>();
                s32 requested = (s32)at<Environment>(env)->mSnowCount.word;
                state = (u8)at<SNOW_EFF>(p)->mStatus;
                if (i >= requested - 1 || state == ParticleSettled) {
                    if (state == ParticleSettled) {
                        if ((s16)at<SNOW_EFF>(p)->mTimer)
                            sdk::cLib_addCalc(at<void>(ea(&at<SNOW_EFF>(p)->mScale)), one, half, rate,
                                              (f32)work->minimumStep);
                        else
                            sdk::cLib_addCalc(at<void>(ea(&at<SNOW_EFF>(p)->mScale)), zero, rate, waveStep,
                                              (f32)work->minimumStep);
                    } else
                        sdk::cLib_addCalc(at<void>(ea(&at<SNOW_EFF>(p)->mScale)), zero, fade, rate, epsilon);
                } else
                    at<SNOW_EFF>(p)->mScale = alpha;
                env = sdk::getEnvironment<u32>();
                requested = (s32)at<Environment>(env)->mSnowCount.word;
                if (i >= requested - 1 && (f32)at<SNOW_EFF>(p)->mScale < epsilon) {
                    s32 n = (s16)at<SnowPacket>(packet)->mEffCount;
                    if ((u32)i == (u32)(n - 1))
                        at<SnowPacket>(packet)->mEffCount = (u16)(n - 1);
                }
            }
        }
        env = sdk::getEnvironment<u32>();
        u32 settlingPacket = (u32)at<Environment>(env)->mpSnowPacket;
        sdk::getGameInfo();
        sdk::getGameInfo();
        sdk::dKyw_get_wind_vecpow(at<void>(wind2));
        sdk::constructGroundBase(at<void>(ground2));
        u32 flags = ea(&at<GroundCheck>(ground2)->pass), types = ea(&at<GroundCheck>(ground2)->group);
        at<GroundCheck>(ground2)->base.cBgS_PolyInfo::__vtbl = (u32)work->polygonVtable;
        at<dBgS_GrpPassChk>(types)->__vtbl = (u32)work->groupVtable;
        at<GroundCheck>(ground2)->group.mGrp = 15;
        for (u32 j = 5; j <= 10; j++)
            at<PolyPass>(flags)->flags[j - 4] = 0;
        at<GroundCheck>(ground2)->base.mGrpPassChk = types;
        at<GroundCheck>(ground2)->base.mPolyPassChk = flags;
        at<PolyPass>(flags)->vtable = ObjectPassVtable;
        at<PolyPass>(flags)->flags[0] = 1;
        at<GroundCheck>(ground2)->base.cBgS_Chk::__vtbl = (u32)work->polyVtableOrHeight.word;
        env = sdk::getEnvironment<u32>();
        if ((u32)at<Environment>(env)->mSnowCount.word) {
            at<GroundCheck>(ground2)->base.m_pos.y = (f32)*at<be<f32>>(AshGroundY);
            at<SnowPacket>(settlingPacket)->mTamariStart = 49;
            at<GroundCheck>(ground2)->base.m_pos.x = (f32)*at<be<f32>>(AshGroundX);
            at<GroundCheck>(ground2)->base.m_pos.z = (f32)*at<be<f32>>(AshGroundZ);
            game = sdk::getGameInfo<u32>();
            f32 groundHeight =
                sdk::groundCross<f32>(at<void>(ea(at<GameInfo>(game)->mBgSpace)), at<void>(ground2));
            f32 zero2 = (f32)work->groundVtableOrZero.value, range, bx, bz;
            if (groundHeight > zero2) {
                range = (f32)*at<be<f32>>(AshWarmRange);
                bx = (f32)*at<be<f32>>(AshWarmBaseX);
                bz = (f32)*at<be<f32>>(AshWarmBaseZ);
            } else {
                range = (f32)*at<be<f32>>(AshColdRange);
                bx = (f32)*at<be<f32>>(AshColdBaseX);
                bz = (f32)*at<be<f32>>(AshColdBaseZ);
            }
            sdk::dKyw_get_wind_vec();
            sdk::dKyw_get_wind_pow();
            s32 n = (s16)at<SnowPacket>(settlingPacket)->mTamariStart;
            if (n + 200 >= 200) {
                f32 angleRange = (f32)work->angleRange, drift = (f32)*at<be<f32>>(AshTamariDrift),
                    gravityRange = (f32)*at<be<f32>>(AshVerticalWaveScale),
                    tiny = (f32)*at<be<f32>>(FlareMinimumVisibility),
                    gravityBase = (f32)*at<be<f32>>(AshTamariGravityBase),
                    timerRange = (f32)*at<be<f32>>(AshTimerRange),
                    speedRange = (f32)*at<be<f32>>(AshWindRange), one2 = (f32)work->one,
                    speedBase = (f32)*at<be<f32>>(AshWindBase), half = (f32)*at<be<f32>>(Half),
                    fadeHeight = fsubs_ppc(groundHeight, (f32)*at<be<f32>>(AshTamariFadeHeight)),
                    five = (f32)*at<be<f32>>(CloudVerticalGain), rate = (f32)*at<be<f32>>(Drag);
                work->polyVtableOrHeight.value = fadeHeight;
                for (s32 i = n + 200, remaining = n + 1; remaining; i--, remaining--) {
                    f32 gravity = -fadds_ppc(sdk::cM_rndFX<f32>(gravityRange), gravityBase),
                        speed = fadds_ppc(sdk::cM_rndF<f32>(speedRange), speedBase);
                    u32 p = ea(&at<SnowPacket>(settlingPacket)->mEff[(u32)i]);
                    u8 state = (u8)at<SNOW_EFF>(p)->mStatus;
                    if (state == ParticleInactive) {
                        at<SNOW_EFF>(p)->mWindSpeed = speed;
                        at<SNOW_EFF>(p)->mGravity = gravity;
                        at<SNOW_EFF>(p)->mBasePos.channels[0] = bx;
                        at<SNOW_EFF>(p)->mBasePos.channels[1] = zero2;
                        at<SNOW_EFF>(p)->mBasePos.channels[2] = bz;
                        at<SNOW_EFF>(p)->mPos.channels[0] = sdk::cM_rndFX<f32>(range);
                        at<SNOW_EFF>(p)->mPos.channels[1] = groundHeight;
                        at<SNOW_EFF>(p)->mPos.channels[2] = sdk::cM_rndFX<f32>(range);
                        at<SNOW_EFF>(p)->mScale = zero2;
                        at<SNOW_EFF>(p)->mPosWaveX = sdk::cM_rndF<f32>(angleRange);
                        at<SNOW_EFF>(p)->mPosWaveZ = sdk::cM_rndF<f32>(angleRange);
                        f32 t = fadds_ppc(sdk::cM_rndF<f32>(timerRange), timerRange);
                        at<SNOW_EFF>(p)->mStatus = (u8)at<SNOW_EFF>(p)->mStatus + 1;
                        at<SNOW_EFF>(p)->mTimer = (u16)ftoi(t);
                    } else if (state == ParticleMoving) {
                        s16 timer = (s16)at<SNOW_EFF>(p)->mTimer;
                        f32 speed = (f32)at<SNOW_EFF>(p)->mWindSpeed, alpha = (f32)at<SNOW_EFF>(p)->mScale;
                        if (timer)
                            at<SNOW_EFF>(p)->mTimer = (u16)(timer - 1);
                        f32 motion = fmuls_ppc(fmuls_ppc(speed, alpha), drift),
                            wx = (f32)(*at<Vector>(wind2)).channels[0],
                            x = (f32)at<SNOW_EFF>(p)->mPos.channels[0],
                            z = (f32)at<SNOW_EFF>(p)->mPos.channels[2];
                        at<SNOW_EFF>(p)->mPos.channels[0] = fmadds(wx, motion, x);
                        f32 wz = (f32)(*at<Vector>(wind2)).channels[2], delta = fsubs_ppc(one2, alpha),
                            height = (f32)work->polyVtableOrHeight.value;
                        timer = (s16)at<SNOW_EFF>(p)->mTimer;
                        at<SNOW_EFF>(p)->mPos.channels[2] = fmadds(wz, motion, z);
                        at<SNOW_EFF>(p)->mPos.channels[1] = fnmsubs(delta, five, height);
                        if (timer <= 0) {
                            if ((f32)at<SNOW_EFF>(p)->mScale < tiny) {
                                timer = (s16)at<SNOW_EFF>(p)->mTimer;
                                at<SNOW_EFF>(p)->mStatus = ParticleInactive;
                            }
                            if (!timer) {
                                sdk::cLib_addCalc(at<void>(ea(&at<SNOW_EFF>(p)->mScale)), zero2, rate, drift,
                                                  tiny);
                                continue;
                            }
                        }
                        sdk::cLib_addCalc(at<void>(ea(&at<SNOW_EFF>(p)->mScale)), one2, half, rate, tiny);
                    }
                }
            }
        }
        at<GroundCheck>(ground2)->base.cBgS_PolyInfo::__vtbl = BasePolyVtable;
        at<PolyPass>(flags)->vtable = (u32)work->passVtable;
        at<dBgS_GrpPassChk>(types)->__vtbl = BaseGroupVtable;
        sdk::destroyGroundBase(at<void>(ground2), 0);
    }
    at<GroundCheck>(ground)->base.cBgS_PolyInfo::__vtbl = BasePolyVtable;
    at<GroundCheck>(ground)->pass.vtable = (u32)work->passVtable;
    at<dBgS_GrpPassChk>((u32)work->groupAddress)->__vtbl = BaseGroupVtable;
    sdk::destroyGroundBase(at<void>(ground), 0);
}
} // namespace RainMotion
using namespace RainMotion;
/* WWHD glue (tools/verify/autoglue.py)
 */
