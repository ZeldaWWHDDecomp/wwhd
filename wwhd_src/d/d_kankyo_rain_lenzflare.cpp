// HD: lens flares use nine textured quads and a bank of double-buffered triangle rays.
#include "d/d_kankyo_rain_kazanbai_move.h"
namespace RainMotion {
/* 02570210 */
void dKyr_drawLenzflare(u32 unused, u32 positions) {
    // Live original SP08..SP19B, including Vec12, matrix48, converter16 and projection40.
    Local<LensDrawWork> frame;
    u32 packet = (u32)at<Environment>(sdk::getEnvironment<u32>())->mpSunlenzPacket;
    u32 sun = (u32)at<Environment>(sdk::getEnvironment<u32>())->mpSunPacket;
    u32 game = sdk::getGameInfo<u32>();
    f32 falloff = (f32)at<LensPacket>(packet)->mDistFalloff,
        visibility = (f32)at<SunPacket>(sun)->mVisibility;
    u32 camera = (u32)at<GameInfo>(game)->mCamera;
    f32 tenth = (f32)*at<be<f32>>(Drag), one = (f32)*at<be<f32>>(One), vis2 = visibility * visibility;
    frame->visibility.value = visibility;
    frame->visibilityPower.value = vis2;
    frame->unit.value = one;
    frame->tenth.value = tenth;
    frame->visibilitySquared.value = vis2;
    frame->falloff.value = one - falloff;
    if (visibility < tenth)
        return;
    f32 zero = (f32)*at<be<f32>>(Zero);
    frame->zeroOrScale.value = zero;
    f32 eased = zero;
    if (falloff > (f32)frame->tenth.value)
        eased = (f32)((f64)(falloff - (f32)frame->tenth.value) / (f64)*at<be<f64>>(LensFalloffRange));
    f32 inverse = (f32)frame->unit.value - eased;
    frame->fade.value = inverse;
    f32 cube = (inverse * inverse) * inverse;
    eased = eased * (eased * eased);
    frame->fade.value = gabi::fnmsubs((f32)frame->fade.value, cube, (f32)frame->unit.value);
    f32 distance = (f32)*at<be<f32>>(LensEyeDistance);
    sdk::dKy_set_eyevect_calc2(camera, ea(&frame->eye), distance, distance);
    frame->secondaryColor.channels[0] = 255;
    frame->colorAngles[0] = 255;
    frame->color.channels[3] = 40;
    frame->color.channels[2] = 235;
    frame->secondaryColor.channels[3] = 30;
    frame->secondaryColor.channels[2] = 0;
    frame->colorAngles[1] = 255;
    frame->secondaryColor.channels[1] = 40;
    frame->color.channels[1] = 255;
    frame->color.channels[0] = 255;
    if ((u32)at<GameInfo>(sdk::getGameInfo<u32>())->mView == 0)
        return;
    u32 view = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mView;
    sdk::matrixCopy(ea(&at<LensView>(view)->mInverseView), ea(&frame->matrix));
    sdk::prepareProjection(ea(&frame->projection));
    sdk::project(ea(&at<SunPacket>(sun)->mPos[0]), ea(&frame->projected), ea(&frame->projection));
    frame->center.channels[1] = (f32)*at<be<f32>>(ScreenCenterY);
    frame->center.channels[0] = (f32)*at<be<f32>>(ScreenCenterX);
    frame->center.channels[2] = (f32)frame->zeroOrScale.value;
    sdk::dKyr_get_vectle_calc(ea(&frame->center), ea(&frame->projected), ea(&frame->screenDirection));
    s32 angle =
        sdk::cM_atan2s<s32>((f32)frame->screenDirection.channels[0], (f32)frame->screenDirection.channels[1]);
    f32 turns = gabi::fmadds((f32)angle, (f32)*at<be<f32>>(FlareAngleScale), (f32)*at<be<f32>>(HalfTurn)) /
                (f32)*at<be<f32>>(LensFullTurn);
    s32 angleA = (s16)gabi::ftoi(
        gabi::fmadds(turns, (f32)*at<be<f32>>(LensAngleAScale), (f32)*at<be<f32>>(LensAngleABias)));
    s32 angleB = (s16)gabi::ftoi(
        gabi::fmadds(turns, (f32)*at<be<f32>>(LensAngleBScale), (f32)*at<be<f32>>(LensAngleBBias)));
    s32 phaseA = -2038, phaseB = 16747;
    if ((u32)at<GameInfo>(sdk::getGameInfo<u32>())->mView != 0) {
        view = (u32)at<GameInfo>(sdk::getGameInfo<u32>())->mView;
        sdk::matrixCopy(ea(&at<LensView>(view)->mInverseView), ea(&frame->matrix));
    }
    sdk::matrixRotateAxis(ea(&frame->rotation), 90, (f32)frame->zeroOrScale.value);
    sdk::matrixConcat(ea(&frame->matrix), ea(&frame->rotation), ea(&frame->matrix));
    f32 v2 = (f32)frame->visibilityPower.value;
    f32 half = (f32)*at<be<f32>>(Half);
    frame->half.value = half;
    f32 intensity = (v2 * v2) * (f32)frame->falloff.value;
    frame->color.channels[1] = 255;
    frame->color.channels[0] = 255;
    frame->color.channels[2] = 235;
    f32 a;
    if (intensity < (f32)frame->half.value)
        a = (f32)*at<be<f32>>(LensColorChannelMax) * (f32)frame->zeroOrScale.value;
    else {
        f32 t = intensity - (f32)frame->half.value;
        a = (f32)*at<be<f32>>(LensColorChannelMax) * (t + t);
    }
    frame->color.channels[3] = u8(gabi::ftoi(a));
    sdk::convertColor(ea(&at<LensPacket>(packet)->mStarColor), ea(&frame->color));
    if ((u8)frame->color.channels[3] != 0 && (f32)frame->falloff.value > (f32)frame->zeroOrScale.value) {
        f32 fall = (f32)frame->falloff.value, v = (f32)frame->visibility.value;
        f32 scale = gabi::fmadds((fall * fall) * v, (f32)*at<be<f32>>(LensRayScaleBase),
                                 (f32)*at<be<f32>>(LensRayScaleBase));
        f32 amp = ((f32)*at<be<f32>>(LensRayAmplitude) * v) * ((f32)frame->unit.value - eased);
        for (s32 i = 0; i < 16; i++) {
            f32 base = (f32)((i & 1) ? phaseA : phaseB);
            f32 oscillation = lensSine(
                gabi::ftoi((f32)((i & 1) ? angleA : angleB) * (f32)*at<be<f32>>(LensOscillationScale)));
            if (oscillation < (f32)frame->zeroOrScale.value)
                oscillation = -oscillation;
            f32 h = gabi::fmadds((f32)frame->half.value, oscillation, (f32)frame->half.value);
            f32 width = (f32)*at<be<f32>>(LensWidthScale) * (h + (f32)frame->half.value);
            s32 left = gabi::ftoi(base + width), right = gabi::ftoi(base - width);
            f32 ring = (f32)*at<be<f32>>(LensRingRadius) * scale;
            f32 lx = lensSine(left) * ring, ly = lensCosine(left) * ring, rx = lensSine(right) * ring,
                ry = lensCosine(right) * ring;
            f32 radial = (((f32)*at<be<f32>>(ScalarOne) * scale) *
                          ((f32)frame->falloff.value + (f32)*at<be<f32>>(LensRadialBias))) *
                         gabi::fmadds((f32)frame->half.value, h, (f32)frame->half.value);
            radial = radial * amp;
            if (i & 3)
                radial = radial * (f32)*at<be<f32>>(LensRayScale);
            f32 tx = lensSine(gabi::ftoi(base)) * radial, ty = lensCosine(gabi::ftoi(base)) * radial;
            phaseA = (s16)(phaseA + 4096);
            phaseB = (s16)(phaseB + 7281);
            angleA = (s16)(angleA + 4096);
            angleB = (s16)(angleB + 7281);
            frame->local.channels[0] = tx;
            frame->local.channels[1] = ty;
            frame->local.channels[2] = (f32)frame->zeroOrScale.value;
            sdk::matrixTransform(ea(&frame->matrix), ea(&frame->local), ea(&frame->transformed));
            f32 py = (f32)(*at<Vector>(positions)).channels[1] + (f32)frame->transformed.channels[1],
                px = (f32)(*at<Vector>(positions)).channels[0] + (f32)frame->transformed.channels[0],
                pz = (f32)(*at<Vector>(positions)).channels[2] + (f32)frame->transformed.channels[2];
            frame->position.channels[1] = py;
            frame->position.channels[0] = px;
            frame->position.channels[2] = pz;
            f64 d = sdk::distanceSquared<f64>(ea(&frame->eye), ea(&frame->position));
            d = sdk::squareRoot<f64>(d);
            if (d < (f32)at<LensPacket>(packet)->field_0x88)
                at<LensPacket>(packet)->field_0x88 = (f32)d;
            else if (d > (f32)at<LensPacket>(packet)->field_0x8c)
                at<LensPacket>(packet)->field_0x8c = (f32)d;
            f32 bump;
            switch (i & 3) {
            case 0:
                bump = (f32)frame->tenth.value;
                break;
            case 1:
                bump = (f32)*at<be<f32>>(LensGainBias);
                break;
            case 2:
                bump = (f32)*at<be<f32>>(Rate);
                break;
            default:
                bump = (f32)*at<be<f32>>(ThunderSkyScale);
            }
            f32 gain = ((f32)frame->visibilitySquared.value + bump) * (f32)frame->visibility.value;
            tx = tx * gain;
            ty = ty * gain;
            auto transform = [&](f32 x, f32 y) {
                frame->local.channels[0] = x;
                frame->local.channels[1] = y;
                frame->local.channels[2] = (f32)frame->zeroOrScale.value;
                sdk::matrixTransform(ea(&frame->matrix), ea(&frame->local), ea(&frame->transformed));
            };
            transform(lx, ly);
            f32 ax = (f32)at<SunPacket>(sun)->mPos[0].channels[0] + (f32)frame->transformed.channels[0],
                ay = (f32)at<SunPacket>(sun)->mPos[0].channels[1] + (f32)frame->transformed.channels[1],
                az = (f32)at<SunPacket>(sun)->mPos[0].channels[2] + (f32)frame->transformed.channels[2];
            frame->cornerA.channels[0] = ax;
            frame->cornerA.channels[1] = ay;
            frame->cornerA.channels[2] = az;
            transform(tx, ty);
            f32 bx = (f32)at<SunPacket>(sun)->mPos[0].channels[0] + (f32)frame->transformed.channels[0],
                by = (f32)at<SunPacket>(sun)->mPos[0].channels[1] + (f32)frame->transformed.channels[1],
                bz = (f32)at<SunPacket>(sun)->mPos[0].channels[2] + (f32)frame->transformed.channels[2];
            transform(rx, ry);
            f32 cx = (f32)at<SunPacket>(sun)->mPos[0].channels[0] + (f32)frame->transformed.channels[0],
                cy = (f32)at<SunPacket>(sun)->mPos[0].channels[1] + (f32)frame->transformed.channels[1],
                cz = (f32)at<SunPacket>(sun)->mPos[0].channels[2] + (f32)frame->transformed.channels[2];
            u32 table = ea(&at<LensPacket>(packet)->mTriangles),
                index = u32(i) * 2 + (u32)at<TriangleBank>(table)->selected;
            u32 buf = (u32)at<TriangleBank>(table)->slots[index].buffer;
            clear_cache(buf, 0x1C0);
            index = u32(i) * 2 + (u32)at<TriangleBank>(table)->selected;
            buf = (u32)at<TriangleBank>(table)->slots[index].buffer;
            at<FlareBuffer>(buf)->vertices[2].mPos.channels[0] = cx;
            at<FlareBuffer>(buf)->vertices[1].mPos.channels[1] = by;
            at<FlareBuffer>(buf)->vertices[1].mPos.channels[2] = bz;
            at<FlareBuffer>(buf)->vertices[2].mPos.channels[1] = cy;
            at<FlareBuffer>(buf)->vertices[2].mPos.channels[2] = cz;
            at<FlareBuffer>(buf)->vertices[0].mPos.channels[1] = (f32)frame->cornerA.channels[1];
            at<FlareBuffer>(buf)->vertices[0].mPos.channels[2] = (f32)frame->cornerA.channels[2];
            at<FlareBuffer>(buf)->vertices[0].mPos.channels[0] = (f32)frame->cornerA.channels[0];
            at<FlareBuffer>(buf)->vertices[1].mPos.channels[0] = bx;
        }
    }
    sdk::matrixRotateAxis(ea(&frame->rotation), 90,
                          (f32)at<LensPacket>(packet)->mAngleDeg * (f32)*at<be<f32>>(LensRotationScale));
    sdk::matrixConcat(ea(&frame->matrix), ea(&frame->rotation), ea(&frame->matrix));
    f32 v = (f32)frame->visibility.value, fade = (f32)frame->fade.value, half2 = (f32)frame->half.value;
    f32 baseSize = gabi::fmadds(v, (f32)*at<be<f32>>(LensBaseSizeGain), (f32)*at<be<f32>>(RainShift));
    f32 unit = (f32)frame->unit.value, colorTenth = (f32)frame->tenth.value;
    f32 angleGreen = (f32)(s16)frame->colorAngles[0], angleRed = (f32)(s16)frame->colorAngles[1];
    frame->alphaRate.value = (f32)*at<be<f32>>(Rate);
    frame->unit.value = unit - ((fade - half2) + (fade - half2));
    frame->half.value = (f32)*at<be<f32>>(AshGravityRange);
    frame->tenth.value = (f32)*at<be<f32>>(DirectionBias);
    f32 invFade = unit - fade, fall = (f32)frame->falloff.value, fallSq = fall * fall;
    frame->red.value = angleRed;
    frame->green.value = angleGreen;
    frame->zeroOrScale.value = (f32)*at<be<f32>>(LensLargeSizeScale);
    bool upper = !(fade < half2);
    f32 zero2 = (f32)*at<be<f32>>(Zero), extra = (f32)*at<be<f32>>(CloudSmallOffset);
    for (s32 i = 0; i < 9; i++) {
        u32 material = ea(&at<LensPacket>(packet)->mMaterials[u32(i)]),
            record = ea(&at<LensPacket>(packet)->mFlares[u32(i)]);
        frame->visibilitySquared.value = (f32)frame->visibilityPower.value;
        f32 alpha;
        if (i < 3) {
            if (i == 1)
                alpha = ((f32)*at<be<f32>>(LensMiddleAlpha) * (f32)frame->visibilitySquared.value) * invFade;
            else
                alpha = (((f32)at<be<f32>>(FlareAlphaTable)[u32(i)] * (f32)frame->visibilitySquared.value) *
                         (f32)frame->tenth.value) *
                        invFade;
        } else {
            frame->color.channels[0] =
                u8(gabi::ftoi((f32)frame->green.value * (f32)at<be<f32>>(FlareRedTable)[u32(i)]));
            frame->color.channels[1] =
                u8(gabi::ftoi((f32)frame->red.value * (f32)at<be<f32>>(FlareGreenTable)[u32(i)]));
            f32 x = ((f32)at<be<f32>>(FlareAlphaTable)[u32(i)] * (f32)frame->visibilitySquared.value) *
                    (f32)frame->alphaRate.value;
            alpha = x * (upper ? (f32)frame->unit.value : ((f32)frame->fade.value + (f32)frame->fade.value));
        }
        frame->color.channels[3] = u8(gabi::ftoi(alpha));
        sdk::convertColor(ea(&frame->convertedColor), ea(&frame->color));
        for (u32 k = 0; k < 4; k++)
            at<FlareMaterial>(material)->mColor.words[k] = (u32)frame->convertedColor.words[k];
        sdk::convertColor(ea(&frame->convertedSecondaryColor), ea(&frame->secondaryColor));
        for (u32 k = 0; k < 4; k++)
            at<FlareMaterial>(material)->mSecondaryColor.words[k] =
                (u32)frame->convertedSecondaryColor.words[k];
        f32 size, scaleDat = (f32)at<be<f32>>(FlareSizeTable)[u32(i)];
        if (i >= 3)
            size = ((scaleDat * (f32)frame->visibility.value) * (f32)frame->zeroOrScale.value) *
                   (unit - (f32)at<LensPacket>(packet)->mDistFalloff);
        else {
            size = scaleDat * baseSize;
            f32 term = scaleDat * (f32)frame->visibility.value;
            if (i == 0)
                term = term * half2;
            else if (i == 1)
                term = term * colorTenth;
            else
                term = term * extra;
            size =
                i == 0 ? gabi::fmadds(scaleDat, baseSize, term * fallSq) : gabi::fmadds(term, fallSq, size);
        }
        f32 negative = -size;
        u32 pos = ea(&at<Vector>(positions)[u32(i)]);
        auto transform = [&](f32 x, f32 y) {
            frame->local.channels[0] = x;
            frame->local.channels[1] = y;
            frame->local.channels[2] = zero2;
            sdk::matrixTransform(ea(&frame->matrix), ea(&frame->local), ea(&frame->transformed));
        };
        transform(negative, size);
        f32 ax = (f32)(*at<Vector>(pos)).channels[0] + (f32)frame->transformed.channels[0],
            ay = (f32)(*at<Vector>(pos)).channels[1] + (f32)frame->transformed.channels[1],
            az = (f32)(*at<Vector>(pos)).channels[2] + (f32)frame->transformed.channels[2];
        frame->cornerA.channels[0] = ax;
        frame->cornerA.channels[1] = ay;
        frame->cornerA.channels[2] = az;
        transform(size, size);
        f32 bx = (f32)(*at<Vector>(pos)).channels[0] + (f32)frame->transformed.channels[0],
            by = (f32)(*at<Vector>(pos)).channels[1] + (f32)frame->transformed.channels[1],
            bz = (f32)(*at<Vector>(pos)).channels[2] + (f32)frame->transformed.channels[2];
        frame->cornerB.channels[0] = bx;
        frame->cornerB.channels[1] = by;
        frame->cornerB.channels[2] = bz;
        transform(size, negative);
        f32 cx = (f32)(*at<Vector>(pos)).channels[0] + (f32)frame->transformed.channels[0],
            cy = (f32)(*at<Vector>(pos)).channels[1] + (f32)frame->transformed.channels[1],
            cz = (f32)(*at<Vector>(pos)).channels[2] + (f32)frame->transformed.channels[2];
        frame->cornerC.channels[0] = cx;
        frame->cornerC.channels[1] = cy;
        frame->cornerC.channels[2] = cz;
        frame->cornerZ.value = cz;
        transform(negative, negative);
        f32 dx = (f32)(*at<Vector>(pos)).channels[0] + (f32)frame->transformed.channels[0],
            dy = (f32)(*at<Vector>(pos)).channels[1] + (f32)frame->transformed.channels[1],
            dz = (f32)(*at<Vector>(pos)).channels[2] + (f32)frame->transformed.channels[2];
        frame->cornerD.channels[0] = dx;
        frame->cornerD.channels[1] = dy;
        frame->cornerD.channels[2] = dz;
        u32 index = (u32)at<Geometry>(record)->selected, buf = (u32)at<Geometry>(record)->slots[index].buffer;
        clear_cache(buf, 0x260);
        index = (u32)at<Geometry>(record)->selected;
        buf = (u32)at<Geometry>(record)->slots[index].buffer;
        f32 uv = i <= 1 ? unit : (f32)frame->half.value;
        at<FlareBuffer>(buf)->vertices[3].mPos.channels[1] = (f32)frame->cornerD.channels[1];
        at<FlareBuffer>(buf)->vertices[3].mPos.channels[2] = (f32)frame->cornerD.channels[2];
        at<FlareBuffer>(buf)->vertices[3].mPos.channels[0] = (f32)frame->cornerD.channels[0];
        at<FlareBuffer>(buf)->vertices[3].mU = zero2;
        at<FlareBuffer>(buf)->vertices[1].mPos.channels[0] = (f32)frame->cornerB.channels[0];
        at<FlareBuffer>(buf)->vertices[1].mU = uv;
        at<FlareBuffer>(buf)->vertices[1].mPos.channels[1] = (f32)frame->cornerB.channels[1];
        at<FlareBuffer>(buf)->vertices[1].mPos.channels[2] = (f32)frame->cornerB.channels[2];
        at<FlareBuffer>(buf)->vertices[0].mU = zero2;
        at<FlareBuffer>(buf)->vertices[1].mV = zero2;
        at<FlareBuffer>(buf)->vertices[0].mV = zero2;
        at<FlareBuffer>(buf)->vertices[0].mPos.channels[1] = (f32)frame->cornerA.channels[1];
        at<FlareBuffer>(buf)->vertices[0].mPos.channels[2] = (f32)frame->cornerA.channels[2];
        at<FlareBuffer>(buf)->vertices[3].mV = uv;
        at<FlareBuffer>(buf)->vertices[2].mV = uv;
        at<FlareBuffer>(buf)->vertices[2].mPos.channels[1] = (f32)frame->cornerC.channels[1];
        at<FlareBuffer>(buf)->vertices[2].mPos.channels[2] = (f32)frame->cornerZ.value;
        at<FlareBuffer>(buf)->vertices[0].mPos.channels[0] = (f32)frame->cornerA.channels[0];
        at<FlareBuffer>(buf)->vertices[2].mPos.channels[0] = (f32)frame->cornerC.channels[0];
        at<FlareBuffer>(buf)->vertices[2].mU = uv;
        index = (u32)at<Geometry>(record)->selected;
        u32 owner = ea(&at<Geometry>(record)->slots[index]);
        sdk::flushBuffer(ea(at<BufferSlot>(owner)->flushObject), 0, (u32)at<BufferSlot>(owner)->size);
        at<Geometry>(record)->selected = (u32)at<Geometry>(record)->selected == 0 ? 1 : 0;
    }
    sdk::drawLenzPacket(packet);
}
} // namespace RainMotion
using namespace RainMotion;
/* WWHD glue (tools/verify/autoglue.py)
 */
