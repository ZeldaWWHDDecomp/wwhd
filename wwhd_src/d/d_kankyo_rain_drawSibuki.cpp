// Weather drawing: GameCube names; verified HD rendering buffers.
#include "d/d_kankyo_rain_drawSibuki.h"
namespace RainDraw {
/* 02571B1C */
void dKyr_drawSibuki() {

    u32 camera = (u32)(at<GameInfo>(getGameInfo())->camera),
        packet = (u32)(at<Environment>(getEnvironment())->mpRainPacket.v);
    if ((u32)(at<Environment>(getEnvironment())->mSnowCount))
        return;
    if (!(u32)(at<GameInfo>(getGameInfo())->view))
        return;
    gabi::Local<Matrix> inverse;
    sdk::MTXInverse(ea(&at<View>((u32)at<GameInfo>(getGameInfo())->view)->mRotation), inverse.get());
    f32 zero = (f32)(*at<be<f32>>(Zero)), target = (f32)(*at<be<f32>>(SkyRange));
    if ((u8)(at<RainPacket>(packet)->mStatus) & 1)
        target = zero;
    sdk::addCalc(gabi::ea(&at<RainPacket>(packet)->mSibukiAlpha), target, (f32)(*at<be<f32>>(Rate)),
                 (f32)(*at<be<f32>>(VelocityLimit)), (f32)(*at<be<f32>>(SprayMinimumStep)));
    gabi::Local<cXyz> eye, dir;
    sdk::dKy_set_eyevect_calc(camera, eye.get(), (f32)(*at<be<f32>>(SprayForward)),
                              (f32)(*at<be<f32>>(SprayHeight)));
    sdk::dKyr_get_vectle_calc(ea(&at<Camera>(camera)->mEye), ea(&at<Camera>(camera)->mCenter), dir.get());
    f32 direction = gabi::fadds_ppc((f32)dir->y, (f32)(*at<be<f32>>(DirectionBias)));
    dir->y = direction;
    f32 one = (f32)(*at<be<f32>>(One)), fade = one;
    if (direction > zero)
        fade = direction < (f32)(*at<be<f32>>(Half))
                   ? gabi::fsubs_ppc(one, gabi::fadds_ppc(direction, direction))
                   : zero;
    u8 alpha = (u8)gabi::ftoi(gabi::fmuls_ppc((f32)(at<RainPacket>(packet)->mSibukiAlpha), fade));
    if (!alpha)
        return;
    gabi::Local<Color> color;
    color->channels[0] = 180;
    color->channels[1] = 200;
    color->channels[2] = 220;
    color->channels[3] = alpha;
    gabi::Local<ColorF> floatColor;
    sdk::convertColor(floatColor.get(), color.get());
    sdk::setUniformColor(gabi::ea(&at<RainPacket>(packet)->mSibukiUniform), floatColor.get(), 0);
    sdk::setUniformColor(gabi::ea(&at<RainPacket>(packet)->mSibukiUniform), floatColor.get(), 1);
    f32 spread = (f32)(*at<be<f32>>(Rate));
    if ((u32)(at<GameInfo>(getGameInfo())->view)) {
        f32 ratio = (f32)((f64)(f32)(at<View>((u32)at<GameInfo>(getGameInfo())->view)->mFovy) /
                          (f64)(f32)(*at<be<f32>>(NarrowFovy)));
        if (!(ratio < one))
            ratio = one;
        spread = gabi::fsubs_ppc(one, ratio);
    }
    u32 geometry = ea(&at<RainPacket>(packet)->mSibukiGeometry);
    gabi::Local<Quad> quad;
    s32 i = 0;
    if (i >= ((s32)(u32)(at<Environment>(getEnvironment())->mRainCount) >> 1)) {
        sdk::drawSibukiPacket(packet);
        return;
    }
    f32 randomSize = (f32)(*at<be<f32>>(BaseHeightOffset)), yRandomRange = (f32)(*at<be<f32>>(SprayYRange)),
        positionRange = (f32)(*at<be<f32>>(SprayPositionRange)),
        positionScale = (f32)(*at<be<f32>>(SprayPositionScale));
    do {
        f32 size = gabi::fmadds(sdk::randomF<f32>(randomSize), spread, randomSize);
        f32 dx = gabi::fmuls_ppc(sdk::randomFX<f32>(positionRange), positionScale);
        f32 dy = sdk::randomFX<f32>(yRandomRange);
        f32 dz = sdk::randomFX<f32>(positionRange);
        f32 x = gabi::fadds_ppc((f32)eye->x, dx), y = gabi::fadds_ppc((f32)eye->y, dy);
        f32 z = gabi::fmadds(dz, positionScale, (f32)eye->z);
        f32 left = gabi::fsubs_ppc(x, size), right = gabi::fadds_ppc(x, size);
        f32 bottom = gabi::fsubs_ppc(z, size), top = gabi::fadds_ppc(z, size);
        quad->vertices[0].value.x = left;
        quad->vertices[0].value.y = y;
        quad->vertices[0].value.z = bottom;
        quad->vertices[1].value.x = right;
        quad->vertices[1].value.y = y;
        quad->vertices[1].value.z = bottom;
        quad->vertices[2].value.x = right;
        quad->vertices[2].value.y = y;
        quad->vertices[2].value.z = top;
        quad->vertices[3].value.x = left;
        quad->vertices[3].value.y = y;
        quad->vertices[3].value.z = top;
        u32 selected = (u32)(at<Geometry>(geometry)->selected),
            buffer = (u32)(bufferSlot(geometry, selected)->buffer), end = buffer + 64;
        for (u32 cursor = buffer; cursor < end; cursor += 32)
            for (u32 word = 0; word < 32; word += 4)
                at<CacheLine>(cursor & ~31u)->words[word / 4] = 0;
        selected = (u32)(at<Geometry>(geometry)->selected);
        buffer = (u32)(bufferSlot(geometry, selected)->buffer);
        f32 texOne = (f32)(*at<be<f32>>(TextureEdge));
        for (u32 corner = 0; corner < 4; corner++) {
            for (u32 component = 0; component < 12; component += 4)
                at<Vertex>(buffer + corner * u32(sizeof(Vertex)))->position.words[component / 4] =
                    (u32)(quad->words[corner][component / 4]);
            at<Vertex>(buffer + corner * u32(sizeof(Vertex)))->u = corner == 0 || corner == 3 ? zero : texOne;
            at<Vertex>(buffer + corner * u32(sizeof(Vertex)))->v = corner < 2 ? zero : texOne;
        }
        selected = (u32)(at<Geometry>(geometry)->selected);
        u32 slot = geometry + selected * u32(sizeof(BufferSlot));
        sdk::flushBuffer(ea(&at<BufferSlot>(slot)->flushObject), 0, (u32)(at<BufferSlot>(slot)->size));
        at<Geometry>(geometry)->selected = (u32)(at<Geometry>(geometry)->selected) == 0 ? 1 : 0;
        geometry += sizeof(Geometry);
        i++;
    } while (i < ((s32)(u32)(at<Environment>(getEnvironment())->mRainCount) >> 1));
    sdk::drawSibukiPacket(packet);
}
} // namespace RainDraw

using namespace RainDraw;
/* WWHD glue (tools/verify/autoglue.py)
 */
