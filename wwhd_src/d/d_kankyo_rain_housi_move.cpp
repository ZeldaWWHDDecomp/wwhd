// Housi particle motion, GameCube names and verified HD behavior.
#include "d/d_kankyo_rain_drawSibuki.h"
namespace RainDraw {
/* 02567F68 */
void dKyr_housi_move() {
    // HD: retain the complete live GHS workspace and destructor sequence.
    gabi::Local<HousiWork> work;
    u32 env = getEnvironment();
    HousiPacket *packet = at<Environment>(env)->mpHousiPacket;
    u32 game = getGameInfo(), camera = (u32)(at<GameInfo>(game)->camera);
    getGameInfo();
    dKyw_get_wind_vecpow(ea(&work->wind));
    constructGroundCheck(ea(&work->ground));
    f32 zero = (f32)(*at<be<f32>>(Zero));
    work->ground.base.cBgS_Chk::__vtbl = GroundCheckVtable;
    work->ground.base.cBgS_PolyInfo::__vtbl = PolyInfoVtable;
    work->ground.base.mPolyPassChk = ea(&work->ground.pass);
    work->ground.base.mGrpPassChk = ea(&work->ground.group);
    work->ground.group.__vtbl = GroupPassVtable;
    for (u32 k = 0; k < 6; k++)
        work->ground.pass.flags[k] = 0;
    work->ground.group.mGrp = 1;
    work->ground.pass.__vtbl = PolyPassVtable;
    work->ground.pass.flags[6] = 0;
    if ((u32)(at<Environment>(getEnvironment())->mHousiCount) != 0 ||
        (u32)(at<Environment>(getEnvironment())->mHousiCount) != 0 || !((f32)(packet->field_0x5ddc) > zero))
        packet->mCount = (u32)(at<Environment>(getEnvironment())->mHousiCount);
    u32 wanted = (u32)(at<Environment>(getEnvironment())->mHousiCount);
    f32 rate = (f32)(*at<be<f32>>(Rate)), inc = (f32)(*at<be<f32>>(Increment));
    cLib_addCalc(ea(&packet->field_0x5ddc), wanted ? (f32)(*at<be<f32>>(One)) : zero, rate,
                 (f32)(*at<be<f32>>(AlphaStep)), inc);
    s32 count = (s16)(packet->mCount);
    if (count != 0) {
        f32 d = (f32)(*at<be<f32>>(EyeDistance));
        dKy_set_eyevect_calc2(camera, ea(&work->eye), d, d);
        dKyw_get_wind_pow();
        count = (s16)(packet->mCount);
        f32 drag = (f32)(*at<be<f32>>(Drag)), fall = (f32)(*at<be<f32>>(Fall)),
            degrees = (f32)(*at<be<f32>>(Degrees)), wind = (f32)(*at<be<f32>>(WindScale));
        f32 limit = (f32)(*at<be<f32>>(VelocityLimit)), step = (f32)(*at<be<f32>>(MotionStep)),
            rot = (f32)(*at<be<f32>>(RotationStep)), force = (f32)(*at<be<f32>>(WindForce));
        f32 radius = (f32)(*at<be<f32>>(Radius)), half = (f32)(*at<be<f32>>(Half)),
            floor = (f32)(*at<be<f32>>(Floor)), min = (f32)(*at<be<f32>>(MinimumStep));
        for (s32 i = count - 1; i >= 0; --i) {
            HOUSI_EFF *effect = at<HOUSI_EFF>(ea(packet) + u32(offsetof(HousiPacket, mEffect)) +
                                              u32(i) * u32(sizeof(HOUSI_EFF)));
            u8 state = (u8)(effect->mStatus);
            f32 alpha = (f32)(packet->field_0x5ddc);
            auto position = [&]() {
                for (u32 k = 0; k < 12; k += 4)
                    work->position.channels[0 + k / 4] =
                        (f32)(effect->mBasePos.channels[k / 4]) + (f32)(effect->mPos.channels[k / 4]);
            };
            if (state == 0) {
                f32 speed = cM_rndF((f32)(*at<be<f32>>(InitialSpeed)));
                effect->field_0x3c = 0;
                effect->field_0x34 = speed + rate;
                f32 angle = cM_rndFX((f32)(*at<be<f32>>(FullAngle)));
                effect->field_0x4c = u16(gabi::ftoi(angle));
                for (u32 k = 0; k < 12; k += 4)
                    effect->mBasePos.channels[k / 4] = (f32)(work->eye.channels[0 + k / 4]);
                effect->mPos.value.x = cM_rndFX(radius);
                effect->mPos.value.y = radius;
                effect->mPos.value.z = cM_rndFX(radius);
                effect->mAlpha = zero;
                effect->field_0x48 = zero;
                effect->mScale.value.x = cM_rndF(degrees);
                effect->mScale.value.y = cM_rndF(degrees);
                f32 zrot = cM_rndF(degrees), baseY = (f32)(effect->mBasePos.value.y),
                    offsetY = (f32)(effect->mPos.value.y);
                u8 now = (u8)(effect->mStatus);
                effect->mScale.value.z = zrot;
                effect->mSpeed.value.x = zero;
                effect->mSpeed.value.y = zero;
                effect->mSpeed.value.z = zero;
                if (baseY + offsetY < (f32)(*at<be<f32>>(LowestBase)))
                    effect->mPos.value.y = ((f32)(*at<be<f32>>(BaseResetHeight)) - baseY) +
                                           (f32)(*at<be<f32>>(BaseHeightOffset));
                effect->mStatus = now + 1;
            } else if (state <= 3) {
                if (state == 1) {
                    u32 a = u16(cM_rad2s((f32)(effect->mScale.value.x)));
                    f32 wx = (f32)(work->wind.channels[0]), x = (f32)(effect->mPos.value.x),
                        speed = (f32)(effect->field_0x34);
                    f32 sine = (f32)(at<SinCos>(SinCosTable + (a >> 3) * 8)->sine);
                    x = gabi::fmadds(wx * wind, speed, x);
                    effect->mPos.value.x = x;
                    f32 y = gabi::fmadds((f32)(work->wind.channels[1]) * wind, speed,
                                         (f32)(effect->mPos.value.y));
                    effect->mPos.value.y = y;
                    x = gabi::fmadds(sine, speed, x);
                    f32 z = gabi::fmadds((f32)(work->wind.channels[2]) * wind, speed,
                                         (f32)(effect->mPos.value.z));
                    f32 ry = (f32)(effect->mScale.value.y);
                    y = gabi::fnmsubs(speed, fall, y);
                    effect->mPos.value.z = z;
                    effect->mPos.value.y = y;
                    effect->mPos.value.x = x;
                    a = u16(cM_rad2s(ry));
                    speed = (f32)(effect->field_0x34);
                    y = (f32)(effect->mPos.value.y);
                    y = gabi::fmadds((f32)(at<SinCos>(SinCosTable + (a >> 3) * 8)->sine), speed * half, y);
                    f32 rz = (f32)(effect->mScale.value.z);
                    effect->mPos.value.y = y;
                    a = u16(cM_rad2s(rz));
                    speed = (f32)(effect->field_0x34);
                    z = (f32)(effect->mPos.value.z);
                    z = gabi::fmadds((f32)(at<SinCos>(SinCosTable + (a >> 3) * 8)->sine), speed, z);
                    f32 rx = (f32)(effect->mScale.value.x), ry2 = (f32)(effect->mScale.value.y),
                        rz2 = (f32)(effect->mScale.value.z);
                    effect->mPos.value.z = z;
                    effect->mScale.value.x = rx + rot;
                    effect->mScale.value.y = ry2 + step;
                    effect->mScale.value.z = rz2 + inc;
                }
                position();
                dKyw_pntwind_get_vecpow(ea(&work->pointWind), ea(&work->position));
                f32 vx = (f32)(work->pointWind.channels[0]), vy = (f32)(work->pointWind.channels[1]),
                    vz = (f32)(work->pointWind.channels[2]);
                work->velocity.channels[0] = vx;
                work->velocity.channels[1] = vy;
                work->velocity.channels[2] = vz;
                f32 oldx = (f32)(effect->mSpeed.value.x), oldy = (f32)(effect->mSpeed.value.y);
                if (oldx < limit)
                    effect->mSpeed.value.x = gabi::fmadds(vx, force, oldx);
                f32 oldz = (f32)(effect->mSpeed.value.z);
                if (oldy < limit)
                    effect->mSpeed.value.y = gabi::fmadds((f32)(work->velocity.channels[1]), force, oldy);
                if (oldz < limit)
                    effect->mSpeed.value.z = gabi::fmadds((f32)(work->velocity.channels[2]), force, oldz);
                for (u32 k = 0; k < 12; k += 4)
                    cLib_addCalc(ea(&effect->mSpeed.channels[k / 4]), zero, rate, drag, min);
                vx = (f32)(effect->mSpeed.value.x);
                vy = (f32)(effect->mSpeed.value.y);
                vz = (f32)(effect->mSpeed.value.z);
                f32 x = (f32)(effect->mPos.value.x) + vx, y = (f32)(effect->mPos.value.y) + vy,
                    z = (f32)(effect->mPos.value.z) + vz, bx = (f32)(effect->mBasePos.value.x);
                effect->mPos.value.x = x;
                effect->mPos.value.y = y;
                effect->mPos.value.z = z;
                work->position.channels[0] = bx + x;
                work->position.channels[1] = (f32)(effect->mBasePos.value.y) + (f32)(effect->mPos.value.y);
                work->position.channels[2] = (f32)(effect->mBasePos.value.z) + (f32)(effect->mPos.value.z);
                f32 distance = PSVECSquareDistance(ea(&work->position), ea(&work->eye));
                distance = sqrtFloatHD(distance);
                u16 delay = (u16)(effect->field_0x3c);
                if (delay == 0) {
                    if (distance > radius || (f32)(work->position.channels[1]) < floor) {
                        effect->field_0x3c = 10;
                        for (u32 k = 0; k < 12; k += 4)
                            effect->mBasePos.words[k / 4] = (u32)(work->eye.words[0 + k / 4]);
                        distance = PSVECSquareDistance(ea(&work->position), ea(&work->eye));
                        distance = sqrtFloatHD(distance);
                        if (distance > (f32)(*at<be<f32>>(RespawnRadius))) {
                            for (u32 k = 0; k < 12; k += 4)
                                effect->mPos.channels[k / 4] = cM_rndFX(radius);
                        } else {
                            f32 jitter = cM_rndFX((f32)(*at<be<f32>>(RespawnJitter)));
                            get_vectle_calc(ea(&work->position), ea(&work->eye), ea(&work->velocity));
                            f32 scale = jitter + radius;
                            for (u32 k = 0; k < 12; k += 4)
                                effect->mPos.channels[k / 4] =
                                    (f32)(work->velocity.channels[0 + k / 4]) * scale;
                        }
                        effect->mSpeed.value.x = zero;
                        effect->mSpeed.value.y = zero;
                        effect->mSpeed.value.z = zero;
                        effect->mStatus = 1;
                    }
                } else
                    effect->field_0x3c = delay - 1;
            }
            position();
            u16 angle = (u16)(effect->field_0x4c) + 600;
            effect->field_0x4c = angle;
            if (angle > 30000)
                alpha = zero;
            cLib_addCalc(ea(&effect->mAlpha), alpha, half, step, min);
        }
    }
    work->ground.base.cBgS_PolyInfo::__vtbl = PolyInfoVtable;
    work->ground.pass.__vtbl = PolyPassVtable;
    work->ground.group.__vtbl = GroupPassDestructorVtable;
    destroyGroundCheck(ea(&work->ground), 0);
}
} // namespace RainDraw
using namespace RainDraw;
/* WWHD glue (tools/verify/autoglue.py)
 */
