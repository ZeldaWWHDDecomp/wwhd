// Sun and moon drawing, using GameCube names and verified HD buffers.
#include "d/d_kankyo_rain_drawSibuki.h"
namespace RainDraw {
/* 0256F1C4 */
void dKyr_drawSun(u32 unusedMatrix, u32 input, u32 unusedColor, u32 unusedImages) {
    u32 env = sun_sdk::environment<u32>(), packet = (u32)(at<Environment>(env)->mpSunPacket.v);
    env = sun_sdk::environment<u32>();
    u32 lens = (u32)(at<Environment>(env)->mpSunlenzPacket.v), game = sun_sdk::gameInfo<u32>();
    Local<SunWork> work;
    u32 sun = ea(&work->sun), moon = ea(&work->moon), relative = ea(&work->relative),
        camDirection = ea(&work->cameraDirection), sunDirection = ea(&work->sunDirection),
        matrix = ea(&work->matrix), rotation = ea(&work->rotation), color = ea(&work->color),
        color2 = ea(&work->secondary);
    f32 zero = (f32)(*at<be<f32>>(Zero));
    work->zero = zero;
    bool drawSun = (f32)(at<SunPacket>(packet)->mSunAlpha) > zero,
         drawMoon = (f32)(at<SunPacket>(packet)->mMoonAlpha) > zero;
    u32 camera = (u32)(at<GameInfo>(game)->camera);
    if (!(drawSun || drawMoon))
        return;
    at<Vector>(sun)->channels[2] = (f32)(at<Vector>(input)->channels[2]);
    at<Vector>(sun)->channels[0] = (f32)(at<Vector>(input)->channels[0]);
    at<Vector>(sun)->channels[1] = (f32)(at<Vector>(input)->channels[1]);
    drawSun = true;
    drawMoon = true;
    u32 type = 0;
    if (!sun_stage(SunStageName, ea(&work->stage), ea(&work->current))) {
        game = sun_sdk::gameInfo<u32>();
        u32 info = sun_stage_info(game);
        type = ((u32)(at<StageInfo>(info)->flags) >> 16) & 7;
    }
    env = sun_sdk::environment<u32>();
    if ((s16)(at<Environment>(env)->mBaseLightRed) == 0 && type != 2) {
        env = sun_sdk::environment<u32>();
        if ((f32)(at<Environment>(env)->mCurTime) > (f32)(*at<be<f32>>(MoonTimeEnd)))
            drawMoon = false;
        else {
            env = sun_sdk::environment<u32>();
            if ((f32)(at<Environment>(env)->mCurTime) < (f32)(*at<be<f32>>(MoonTimeBegin)))
                drawMoon = false;
        }
        at<Vector>(moon)->channels[2] = (f32)(at<Vector>(input)->channels[2]);
        at<Vector>(moon)->channels[1] = (f32)(at<Vector>(input)->channels[1]);
        at<Vector>(moon)->channels[0] = (f32)(at<Vector>(input)->channels[0]);
    } else {
        f32 x =
            -fsubs_ppc((f32)(at<Vector>(input)->channels[0]), (f32)(at<Camera>(camera)->mEye.channels[0]));
        at<Vector>(relative)->channels[0] = x;
        f32 y =
            -fsubs_ppc((f32)(at<Vector>(input)->channels[1]), (f32)(at<Camera>(camera)->mEye.channels[1]));
        at<Vector>(relative)->channels[1] = y;
        f32 z =
            -fsubs_ppc((f32)(at<Vector>(input)->channels[2]), (f32)(at<Camera>(camera)->mEye.channels[2]));
        at<Vector>(relative)->channels[2] = z;
        at<Vector>(moon)->channels[0] = fadds_ppc(x, (f32)(at<Camera>(camera)->mEye.channels[0]));
        at<Vector>(moon)->channels[1] = fadds_ppc(y, (f32)(at<Camera>(camera)->mEye.channels[1]));
        at<Vector>(moon)->channels[2] = fadds_ppc(z, (f32)(at<Camera>(camera)->mEye.channels[2]));
    }
    u32 day = sun_sdk::moonDay<u32>(), save = (u32)(*at<be<u32>>(SaveInfoPointer));
    if ((f32)(at<SaveTime>(save)->time) < (f32)(*at<be<f32>>(MoonDayThreshold)))
        day = day ? day - 1 : 6;
    f32 one = (f32)(*at<be<f32>>(One)), flip = (f32)(*at<be<f32>>(MoonFlip));
    u32 texture;
    if (day < 4) {
        texture = day;
        flip = one;
    } else if (day == 4)
        texture = 3;
    else if (day == 5)
        texture = 2;
    else
        texture = 1;
    env = sun_sdk::environment<u32>();
    at<Color>(color)->channels[0] = (u8)(at<Environment>(env)->mFogColor.channels[0]);
    env = sun_sdk::environment<u32>();
    at<Color>(color)->channels[1] = (u8)(at<Environment>(env)->mFogColor.channels[1]);
    env = sun_sdk::environment<u32>();
    at<Color>(color)->channels[2] = (u8)(at<Environment>(env)->mFogColor.channels[2]);
    at<Color>(color)->channels[3] = 255;
    at<Color>(color2)->channels[0] = 0;
    at<Color>(color2)->channels[1] = 0;
    at<Color>(color2)->channels[2] = 0;
    at<Color>(color2)->channels[3] = 255;
    game = sun_sdk::gameInfo<u32>();
    if (!(u32)(at<GameInfo>(game)->view)) {
        u8 timer = (u8)(at<SunPacket>(packet)->field_0x3c);
        if (timer < 5)
            at<SunPacket>(packet)->field_0x3c = timer + 2;
        at<SunPacket>(packet)->field_0x3d = 1;
        return;
    }
    game = sun_sdk::gameInfo<u32>();
    u32 view = (u32)(at<GameInfo>(game)->view);
    sun_sdk::inverse(at<void>(ea(&at<View>(view)->mRotation)), at<void>(matrix));
    work->degree = (f32)(*at<be<f32>>(DegreeToRadians));
    if (drawMoon) {
        for (u32 j = 0; j < 7; j++)
            work->dayScales[j] = (u32)(at<be<u32>>(MoonDayScaleTable)[j]);
        sun_sdk::preparePosition(at<void>(moon), texture);
        sun_sdk::direction(at<void>(ea(&at<Camera>(camera)->mEye)),
                           at<void>(ea(&at<Camera>(camera)->mCenter)), at<void>(camDirection));
        f32 z = (f32)(at<Vector>(camDirection)->channels[2]),
            x = (f32)(at<Vector>(camDirection)->channels[0]),
            radius = sun_sdk::squareRoot<f32>(fmadds(x, x, fmuls_ppc(z, z)));
        f32 camTheta = sun_sdk::atan2<f32>((f32)(at<Vector>(camDirection)->channels[0]),
                                           (f32)(at<Vector>(camDirection)->channels[2]));
        sun_sdk::atan2<f32>((f32)(at<Vector>(camDirection)->channels[1]), radius);
        z = (f32)(at<Vector>(relative)->channels[2]);
        x = (f32)(at<Vector>(relative)->channels[0]);
        radius = sun_sdk::squareRoot<f32>(fmadds(x, x, fmuls_ppc(z, z)));
        f32 moonTheta = sun_sdk::atan2<f32>(x, z),
            moonPhi = sun_sdk::atan2<f32>((f32)(at<Vector>(relative)->channels[1]), radius);
        f32 angle =
            fmadds((f32)(*at<be<f32>>(Degrees)),
                   fmuls_ppc(fmuls_ppc(fsubs_ppc(moonTheta, camTheta), (f32)(*at<be<f32>>(MoonAngleScale))),
                             moonPhi),
                   (f32)(*at<be<f32>>(SpiralHeight)));
        sun_sdk::rotate(at<void>(rotation), 0x5A, fmuls_ppc(angle, (f32)(work->degree)));
        sun_sdk::concat(at<void>(matrix), at<void>(rotation), at<void>(matrix));
        f32 uv0 = (f32)(work->zero);
        at<Color>(color)->channels[0] = 243;
        at<Color>(color)->channels[1] = 255;
        at<Color>(color)->channels[2] = 148;
        f32 size = (f32)(*at<be<f32>>(MoonDistanceGain)), factor = (f32)(*at<be<f32>>(LargeStarGain)),
            alphaScale = (f32)(*at<be<f32>>(MoonAlphaScale)), degree = (f32)(work->degree),
            secondAngle = fmuls_ppc((f32)(*at<be<f32>>(RespawnJitter)), flip);
        at<Color>(color)->channels[3] =
            (u8)ftoi(fmuls_ppc((f32)(at<SunPacket>(packet)->mMoonAlpha), (f32)(*at<be<f32>>(ByteAlpha))));
        u32 manager = ea(&at<SunPacket>(packet)->mMoonGeometry);
        for (u32 j = 0; j < 2; j++) {
            if (j == 1) {
                at<Color>(color2)->channels[1] = 105;
                at<Color>(color)->channels[0] = 255;
                at<Color>(color2)->channels[0] = 197;
                at<Color>(color2)->channels[2] = 35;
                size = fmuls_ppc(size, factor);
                at<Color>(color)->channels[3] =
                    (u8)ftoi(fmuls_ppc((f32)(at<SunPacket>(packet)->mMoonAlpha), alphaScale));
                at<Color>(color)->channels[1] = 255;
                at<Color>(color)->channels[2] = 207;
                sun_sdk::rotate(at<void>(rotation), 0x5A, fmuls_ppc(secondAngle, degree));
                sun_sdk::concat(at<void>(matrix), at<void>(rotation), at<void>(matrix));
            }
            sun_sdk::setSunColor(at<void>(packet), j, at<void>(color), at<void>(color2));
            f32 positiveX = fmuls_ppc(size, flip), negativeX = -positiveX, negativeY = -size;
            work->input.channels[0] = negativeX;
            work->input.channels[1] = size;
            work->input.channels[2] = uv0;
            sun_vertex(work.get(), matrix, moon, ea(&work->quad));
            work->input.channels[0] = positiveX;
            work->input.channels[1] = size;
            work->input.channels[2] = uv0;
            sun_vertex(work.get(), matrix, moon, ea(&work->quad.vertices[1]));
            if (j == 0) {
                work->input.channels[0] = positiveX;
                work->input.channels[1] = negativeY;
            } else {
                f32 scale = (f32)(std::bit_cast<f32>((u32)work->dayScales[day]));
                work->input.channels[0] = fmuls_ppc(positiveX, scale);
                work->input.channels[1] = -fmuls_ppc(size, scale);
            }
            work->input.channels[2] = uv0;
            sun_vertex(work.get(), matrix, moon, ea(&work->quad.vertices[2]));
            work->input.channels[0] = negativeX;
            work->input.channels[1] = negativeY;
            work->input.channels[2] = uv0;
            sun_vertex(work.get(), matrix, moon, ea(&work->quad.vertices[3]));
            sun_quad(manager, work.get(), uv0, one);
            manager += sizeof(Geometry);
        }
    }
    if (drawSun) {
        f32 fade = fsubs_ppc(one, (f32)(at<LensPacket>(lens)->mDistFalloff));
        sun_sdk::preparePosition(at<void>(sun), 9);
        f32 z = (f32)(at<Vector>(sun)->channels[2]), x = (f32)(at<Vector>(sun)->channels[0]),
            radius = sun_sdk::squareRoot<f32>(fmadds(x, x, fmuls_ppc(z, z)));
        f32 sunTheta =
            sun_sdk::atan2<f32>((f32)(at<Vector>(sun)->channels[0]), (f32)(at<Vector>(sun)->channels[2]));
        sun_sdk::atan2<f32>((f32)(at<Vector>(sun)->channels[1]), radius);
        sun_sdk::direction(at<void>(ea(&at<Camera>(camera)->mEye)),
                           at<void>(ea(&at<Camera>(camera)->mCenter)), at<void>(sunDirection));
        z = (f32)(at<Vector>(sunDirection)->channels[2]);
        x = (f32)(at<Vector>(sunDirection)->channels[0]);
        radius = sun_sdk::squareRoot<f32>(fmadds(x, x, fmuls_ppc(z, z)));
        f32 camTheta = sun_sdk::atan2<f32>((f32)(at<Vector>(sunDirection)->channels[0]),
                                           (f32)(at<Vector>(sunDirection)->channels[2]));
        sun_sdk::atan2<f32>((f32)(at<Vector>(sunDirection)->channels[1]), radius);
        f32 angle = fmadds((f32)(*at<be<f32>>(Degrees)),
                           fmuls_ppc(fsubs_ppc(sunTheta, camTheta), (f32)(*at<be<f32>>(MoonAngleScale))),
                           (f32)(*at<be<f32>>(SunAngleOffset)));
        sun_sdk::rotate(at<void>(rotation), 0x5A, fmuls_ppc(angle, (f32)(work->degree)));
        sun_sdk::concat(at<void>(matrix), at<void>(rotation), at<void>(matrix));
        work->uvOne = one;
        at<Color>(color2)->channels[0] = 255;
        at<Color>(color2)->channels[1] = 40;
        at<Color>(color2)->channels[2] = 0;
        work->alphaScale = (f32)(*at<be<f32>>(ByteAlpha));
        at<Color>(color)->channels[0] = 255;
        at<Color>(color)->channels[1] = 255;
        at<Color>(color)->channels[2] = 235;
        f32 scale = fmuls_ppc((f32)(*at<be<f32>>(ScreenMaximumY)), fmuls_ppc(fade, fade));
        work->degree = scale;
        f32 size = (f32)(*at<be<f32>>(SunSize)), growth = (f32)(*at<be<f32>>(SunGrowth)),
            alphaScale = (f32)(*at<be<f32>>(SunAlphaScale)), uv0 = (f32)(work->zero),
            uv1 = (f32)(work->uvOne);
        u32 manager = ea(&at<SunPacket>(packet)->mSunGeometry);
        for (u32 j = 0; j < 2; j++) {
            if (j == 0) {
                size = (f32)(*at<be<f32>>(SunSize));
                uv0 = (f32)(work->zero);
                uv1 = (f32)(work->uvOne);
                scale = (f32)(work->degree);
                growth = (f32)(*at<be<f32>>(SunGrowth));
                alphaScale = (f32)(*at<be<f32>>(SunAlphaScale));
                at<Color>(color)->channels[3] =
                    (u8)ftoi(fmuls_ppc((f32)(work->alphaScale), (f32)(at<SunPacket>(packet)->mSunAlpha)));
            } else {
                f32 alpha = (f32)(at<SunPacket>(packet)->mSunAlpha),
                    intensity = (f32)(at<SunPacket>(packet)->mVisibility);
                at<Color>(color)->channels[3] = (u8)ftoi(fmuls_ppc(alphaScale, alpha));
                size = fmuls_ppc(fmadds(scale, intensity, size), growth);
            }
            sun_sdk::setSunColor(at<void>(packet), j + 2, at<void>(color), at<void>(color2));
            f32 neg = -size;
            work->input.channels[0] = neg;
            work->input.channels[1] = size;
            work->input.channels[2] = uv0;
            sun_vertex(work.get(), matrix, sun, ea(&work->quad));
            work->input.channels[0] = size;
            work->input.channels[1] = size;
            work->input.channels[2] = uv0;
            sun_vertex(work.get(), matrix, sun, ea(&work->quad.vertices[1]));
            work->input.channels[0] = size;
            work->input.channels[1] = neg;
            work->input.channels[2] = uv0;
            sun_vertex(work.get(), matrix, sun, ea(&work->quad.vertices[2]));
            work->input.channels[0] = neg;
            work->input.channels[1] = neg;
            work->input.channels[2] = uv0;
            sun_vertex(work.get(), matrix, sun, ea(&work->quad.vertices[3]));
            sun_quad(manager, work.get(), uv0, uv1);
            manager += sizeof(Geometry);
        }
    }
    sun_sdk::drawSunPacket(at<void>(packet), texture, (u32)drawMoon, (u32)drawSun);
}
} // namespace RainDraw
using namespace RainDraw;
/* WWHD glue (tools/verify/autoglue.py)
 */
