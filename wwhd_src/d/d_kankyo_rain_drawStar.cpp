// Weather drawing: GameCube names; verified HD rendering buffers.
#include "d/d_kankyo_rain_drawSibuki.h"
namespace RainDraw {
/* 02574144 */
void dKyr_drawStar() {

    u32 env = getEnvironment(), packet = (u32)(at<Environment>(getEnvironment())->mpStarPacket.v),
        camera = (u32)(at<GameInfo>(getGameInfo())->camera);
    gabi::Local<ProjectParams> params, starParams;
    gabi::Local<Matrix> view, rotation;
    gabi::Local<cXyz> reference, position, projected, input, output;
    gabi::Local<Quad> triangle;
    sdk::prepareProjection(params.get());
    if (!(u32)(*at<be<u32>>(ConstellationInitialized))) {
        *at<be<u32>>(ConstellationInitialized) = 1;
        const s32 angles[16][3] = {
            {0x32C8, 0x2904, -16000}, {0x24B8, 0x2648, -12646}, {0x27D8, 0x2E18, -13525},
            {0x283C, 0x348A, -13525}, {0x3A98, 0x47E0, -16162}, {0x30D4, 0x4D58, -15000},
            {0x23DB, 0x4330, -14404}, {0x251C, 0x2648, -12646}, {-7421, 0x791D, 0x496E},
            {-10937, 0x6D60, 0x3A98}, {-10000, 0x6146, 0x47E0}, {-9400, 0x57E4, 0x3E1C},
            {-9179, 0x5334, 0x37DC},  {-10300, 0x55F0, 0x5208}, {-16000, 0x639C, 0x4E20},
            {0, 0x7530, 0x4A38}};
        for (u32 i = 0; i < 16; i++)
            sdk::constructAngle(ConstellationPositions + i * 6, angles[i][0], angles[i][1], angles[i][2]);
    }
    if (!(s16)(at<StarPacket>(packet)->mEffectNum))
        return;
    if (!(u32)(at<GameInfo>(getGameInfo())->view))
        return;
    sdk::MTXInverse(ea(&at<View>((u32)at<GameInfo>(getGameInfo())->view)->mRotation), view.get());
    sdk::project(ea(&at<Environment>(env)->mMoonPos), reference.get(), params.get());
    sdk::MTXRotDeg(
        rotation.get(), 90,
        gabi::fmuls_ppc((f32)(u32)(at<StarPacket>(packet)->mRot), (f32)(*at<be<f32>>(DegreeToRadians))));
    sdk::MTXConcat(view.get(), rotation.get(), view.get());
    f32 cameraY = (f32)(at<Camera>(camera)->mEye.value.y), cameraX = (f32)(at<Camera>(camera)->mEye.value.x),
        zero = (f32)(*at<be<f32>>(Zero)), cameraZ = (f32)(at<Camera>(camera)->mEye.value.z), fade = zero;
    u32 info = getGameInfo();
    f32 one = (f32)(*at<be<f32>>(One));
    if ((u32)(at<GameInfo>(info)->view)) {
        fade = (f32)(at<View>((u32)at<GameInfo>(getGameInfo())->view)->mFovy) / (f32)(*at<be<f32>>(WideFovy));
        if (!(fade < one))
            fade = one;
        fade = gabi::fsubs_ppc(one, fade);
    }
    f32 sizeBase = (f32)(*at<be<f32>>(StarSizeBase)), half = (f32)(*at<be<f32>>(Half));
    f32 radius = gabi::fmuls_ppc(gabi::fnmsubs((f32)(*at<be<f32>>(Fall)), fade, sizeBase), half);
    f32 bottom = -gabi::fmuls_ppc(radius, half);
    for (u32 vertex = 0; vertex < 3; vertex++) {
        input->x = vertex == 0 ? zero : vertex == 1 ? radius : -radius;
        input->y = vertex == 0 ? radius : bottom;
        input->z = zero;
        sdk::MTXMultVec(view.get(), input.get(), output.get());
        triangle->vertices[vertex].value.x = gabi::fadds_ppc(cameraX, (f32)output->x);
        triangle->vertices[vertex].value.y = gabi::fadds_ppc(cameraY, (f32)output->y);
        triangle->vertices[vertex].value.z = gabi::fadds_ppc(cameraZ, (f32)output->z);
    }
    f32 spiral = zero;
    f32 distanceLimit = gabi::fmadds((f32)(*at<be<f32>>(MoonDistanceGain)), gabi::fmuls_ppc(fade, fade),
                                     (f32)(*at<be<f32>>(MoonDistanceBase)));
    f32 minCoord = (f32)(*at<be<f32>>(ScreenMinimum)), spiralShrink = (f32)(*at<be<f32>>(SpiralShrink)),
        two = (f32)(*at<be<f32>>(SkyRange)), maxX = (f32)(*at<be<f32>>(ScreenMaximumX)),
        maxY = (f32)(*at<be<f32>>(ScreenMaximumY)), distanceFactor = (f32)(*at<be<f32>>(SpiralDistanceGain)),
        tableScale = (f32)(*at<be<f32>>(StarTableScale));
    u32 phase = 0, phaseStep = 0;
    for (s32 i = 0; i < (s16)(at<StarPacket>(packet)->mEffectNum); i++) {
        u32 flags = ea(&at<StarPacket>(packet)->mColorKind);
        at<StarPacket>(packet)->mDrawn[i] = 0;
        if (i < 16)
            at<be<u8>>(flags)[(u32)i] = i == 6 || i == 8 ? 0 : 1;
        else {
            u32 kind = (u32)i & 7;
            at<be<u8>>(flags)[(u32)i] = kind < 2 ? 2 : kind;
        }
        f32 x, y, z, scale;
        if (i < 16) {
            u32 angle = ConstellationPositions + (u32)i * 6;
            x = (f32)(s16)(at<csXyz>(angle)->x);
            y = (f32)(s16)(at<csXyz>(angle)->y);
            z = (f32)(s16)(at<csXyz>(angle)->z);
            scale = gabi::fmuls_ppc((f32)(at<StarPacket>(packet)->mSin),
                                    (f32)(*at<be<f32>>(i > 7 ? u32(LargeStarScale) : u32(Degrees))));
            scale = gabi::fnmsubs(fade, gabi::fmuls_ppc(scale, half), scale);
        } else {
            scale = gabi::fmadds(
                gabi::fmuls_ppc(gabi::fmuls_ppc((f32)(i & 63), (f32)(*at<be<f32>>(StarVariation))),
                                (f32)(*at<be<f32>>(StarVariationScale))),
                (f32)(at<StarPacket>(packet)->mSin), sizeBase);
            if ((i & 15) == 2) {
                scale = gabi::fmuls_ppc(scale, (f32)(*at<be<f32>>(LargeStarGain)));
                at<be<u8>>(flags)[(u32)i] = 2;
            }
            u32 table = SinCosTable + (((phase - 0x8000) & 0xFFFFu) >> 3) * 8;
            f32 shrink = gabi::fnmsubs(spiral, spiralShrink, one);
            f32 px = gabi::fmuls_ppc(gabi::fmuls_ppc((f32)(at<SinCos>(table)->sine), tableScale), shrink),
                pz = gabi::fmuls_ppc(gabi::fmuls_ppc((f32)(at<SinCos>(table)->cosine), tableScale), shrink);
            f32 term = spiral / two;
            term = gabi::fmuls_ppc(term, gabi::fmuls_ppc(term, term));
            f32 increment = gabi::fmadds(term, distanceFactor, one);
            y = gabi::fadds_ppc(spiral, (f32)(*at<be<f32>>(SpiralHeight)));
            spiral = gabi::fadds_ppc(spiral, increment);
            phase += phaseStep;
            phaseStep += 2500;
            x = -px;
            z = pz;
            if (spiral > two)
                spiral = gabi::fmuls_ppc((f32)i / (f32)(*at<be<f32>>(SpiralResetScale)),
                                         (f32)(*at<be<f32>>(NarrowFovy)));
        }
        sdk::prepareProjection(starParams.get());
        position->x = gabi::fadds_ppc(cameraX, x);
        position->y = gabi::fadds_ppc(cameraY, y);
        position->z = gabi::fadds_ppc(cameraZ, z);
        sdk::project(position.get(), projected.get(), starParams.get());
        f32 squared = sdk::distanceSquared<f32>(reference.get(), projected.get());
        f64 distance = sdk::squareRoot<f64>((f64)squared);
        f32 screenX = (f32)projected->x, screenY = (f32)projected->y;
        if (!(screenX > minCoord && screenX < maxX && screenY > minCoord && screenY < maxY &&
              distance > distanceLimit))
            continue;
        u32 geometry = ea(&at<StarPacket>(packet)->mGeometry[i]);
        u32 selected = (u32)(at<StarGeometry>(geometry)->selected),
            slot = geometry + selected * u32(sizeof(BufferSlot));
        u32 buffer = (u32)(at<BufferSlot>(slot)->buffer), end = buffer + 32;
        if (buffer < end)
            for (u32 cursor = buffer; cursor < end; cursor += 32)
                for (u32 k = 0; k < 32; k += 4)
                    at<CacheLine>(cursor & ~31u)->words[k / 4] = 0;
        selected = (u32)(at<StarGeometry>(geometry)->selected);
        slot = geometry + selected * u32(sizeof(BufferSlot));
        buffer = (u32)(at<BufferSlot>(slot)->buffer);
        for (u32 vertex = 0; vertex < 3; vertex++)
            for (u32 axis = 0; axis < 3; axis++) {
                f32 origin = axis == 0 ? cameraX : axis == 1 ? cameraY : cameraZ;
                f32 offset = gabi::fmuls_ppc(
                    gabi::fsubs_ppc((f32)(triangle->vertices[vertex].channels[axis]), origin), scale);
                at<Vector>(buffer + vertex * u32(sizeof(Vector)))->channels[axis] =
                    gabi::fadds_ppc((f32)(at<Vector>(gabi::ea(position.get()))->channels[axis]), offset);
            }
        selected = (u32)(at<StarGeometry>(geometry)->selected);
        slot = geometry + selected * u32(sizeof(BufferSlot));
        buffer = (u32)(at<StarGeometry>(slot)->slots[1][0].buffer);
        end = buffer + 32;
        if (buffer < end)
            for (u32 cursor = buffer; cursor < end; cursor += 32)
                for (u32 k = 0; k < 32; k += 4)
                    at<CacheLine>(cursor & ~31u)->words[k / 4] = 0;
        selected = (u32)(at<StarGeometry>(geometry)->selected);
        slot = geometry + selected * u32(sizeof(BufferSlot));
        buffer = (u32)(at<StarGeometry>(slot)->slots[1][0].buffer);
        for (u32 vertex = 0; vertex < 3; vertex++)
            for (u32 axis = 0; axis < 3; axis++) {
                f32 origin = axis == 0 ? cameraX : axis == 1 ? cameraY : cameraZ;
                f32 offset = gabi::fmuls_ppc(
                    gabi::fsubs_ppc((f32)(triangle->vertices[vertex].channels[axis]), origin), scale);
                at<Vector>(buffer + vertex * u32(sizeof(Vector)))->channels[axis] =
                    gabi::fsubs_ppc((f32)(at<Vector>(gabi::ea(position.get()))->channels[axis]), offset);
            }
        selected = (u32)(at<StarGeometry>(geometry)->selected);
        slot = geometry + selected * u32(sizeof(BufferSlot));
        for (u32 face = 0; face < 2; face++) {
            sdk::flushBuffer(ea(&at<BufferSlot>(slot)->flushObject), 0, (u32)(at<BufferSlot>(slot)->size));
            slot += 2 * sizeof(BufferSlot);
        }
        at<StarGeometry>(geometry)->selected = (u32)(at<StarGeometry>(geometry)->selected) == 0 ? 1 : 0;
        at<StarPacket>(packet)->mDrawn[i] = 1;
    }
    sdk::drawStarPacket(packet);
}
} // namespace RainDraw

using namespace RainDraw;
/* WWHD glue (tools/verify/autoglue.py)
 */
