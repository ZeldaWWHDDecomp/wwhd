// Weather drawing: GameCube names; verified HD rendering buffers.
#include "d/d_kankyo_rain_drawSibuki.h"
namespace RainDraw {
/* 02574D38 */
void drawWave(u32 ignored, u32 unused, u32 draw) {

    if ((s32)(u32)(at<DrawInfo>(draw)->mode) != 2)
        return;
    u32 packet = (u32)(at<Environment>(getEnvironment())->mpWavePacket.v),
        camera = (u32)(at<GameInfo>(getGameInfo())->camera);
    f32 one = (f32)(*at<be<f32>>(One));
    if (!((f32)(at<Environment>(getEnvironment())->mWaveFlatInter) < one))
        return;
    if (!(u32)(at<GameInfo>(getGameInfo())->view))
        return;
    gabi::Local<Matrix> view, rotation;
    gabi::Local<SafeString> stage, current;
    gabi::Local<Color> color, secondary;
    gabi::Local<cXyz> input, output;
    gabi::Local<Quad> quad;
    sdk::MTXInverse(ea(&at<View>((u32)at<GameInfo>(getGameInfo())->view)->mRotation), view.get());
    f32 angle = gabi::fmuls_ppc((f32)(s16)(at<Camera>(camera)->mBank), (f32)(*at<be<f32>>(BankToRadians)));
    stage->text = WaveStageName;
    stage->vtable = SafeStringVtableAddress;
    current->text = ea(&at<GameInfo>(getGameInfo())->mStageName);
    current->vtable = SafeStringVtableAddress;
    safe_string_callback(stage.get());
    safe_string_callback(stage.get());
    u32 expected = (u32)stage->text;
    safe_string_callback(current.get());
    equal_terminated_strings(expected, (u32)current->text);
    sdk::getSeaColor(color.get(), secondary.get());
    sdk::setSeaFog();
    sdk::MTXRotDeg(rotation.get(), 90, gabi::fmuls_ppc(angle, (f32)(*at<be<f32>>(DegreeToRadians))));
    sdk::MTXConcat(view.get(), rotation.get(), view.get());
    for (u32 i = 0; i < 300; i++)
        at<WavePacket>(packet)->mDrawn[i] = 0;
    f32 zero = (f32)(*at<be<f32>>(Zero)), uvScale = (f32)(*at<be<f32>>(EyeDistance)),
        fadeScale = (f32)(*at<be<f32>>(WaveFadeScale)), byteScale = (f32)(*at<be<f32>>(ByteAlpha));
    f32 scalarOne = (f32)(*at<be<f32>>(ScalarOne)), uvWide = (f32)(*at<be<f32>>(WideTexture)),
        uvTwo = (f32)(*at<be<f32>>(SkyRange)), widthScale = (f32)(*at<be<f32>>(WaveWidthScale));
    s32 i = 0;
    while (i < (s16)(at<Environment>(getEnvironment())->mWaveCount)) {
        u32 particle = ea(&at<WavePacket>(packet)->mEff[i]);
        f32 centerX = gabi::fadds_ppc((f32)(at<WAVE_EFF>(particle)->mBasePos.value.x),
                                      (f32)(at<WAVE_EFF>(particle)->mPos.value.x));
        f32 centerY = gabi::fadds_ppc((f32)(at<WAVE_EFF>(particle)->mBasePos.value.y),
                                      (f32)(at<WAVE_EFF>(particle)->mPos.value.y));
        f32 centerZ = gabi::fadds_ppc((f32)(at<WAVE_EFF>(particle)->mBasePos.value.z),
                                      (f32)(at<WAVE_EFF>(particle)->mPos.value.z));
        f64 sine = sdk::sine<f64>((f32)(at<WAVE_EFF>(particle)->mCounter));
        if (sine > zero) {
            f32 height =
                wave_multiply_double(gabi::fmuls_ppc((f32)(at<Environment>(getEnvironment())->mWaveScale),
                                                     (f32)(at<WAVE_EFF>(particle)->mScale)),
                                     sine);
            f32 width = gabi::fmuls_ppc((f32)(at<Environment>(getEnvironment())->mWaveScale),
                                        (f32)(at<WAVE_EFF>(particle)->mScale));
            u32 env = getEnvironment();
            f32 fade = (f32)(at<WAVE_EFF>(particle)->mStrengthEnv),
                stretch = (f32)(at<Environment>(env)->mWaveScaleBottom);
            height = gabi::fmuls_ppc(height, fade);
            f32 step = gabi::fmuls_ppc((f32)((u32)i * 31), fadeScale);
            f32 widthFade = gabi::fnmsubs(step, height, fade);
            width = gabi::fmuls_ppc(gabi::fmuls_ppc(width, stretch), widthFade);
            if (height > zero) {
                at<WavePacket>(packet)->mDrawn[i] = 1;
                color->channels[3] =
                    (u8)gabi::ftoi(gabi::fmuls_ppc(byteScale, (f32)(at<WAVE_EFF>(particle)->mAlpha)));
                sdk::prepareWaveColor(packet, i, secondary.get(), color.get());
                f32 skewY = (f32)(at<WavePacket>(packet)->mSkewDir),
                    skew = (f32)(at<WavePacket>(packet)->mSkewWidth);
                f32 offset = gabi::fmuls_ppc(
                    width, gabi::fmuls_ppc(widthScale, (f32)(at<WAVE_EFF>(particle)->mSpeed)));
                f32 first = skewY < zero ? gabi::fsubs_ppc(-gabi::fmuls_ppc(offset, skew), width)
                                         : gabi::fmadds(offset, skew, -width);
                input->x = first;
                input->y = height;
                input->z = zero;
                sdk::MTXMultVec(view.get(), input.get(), output.get());
                f32 x0 = gabi::fadds_ppc(centerX, (f32)output->x),
                    y0 = gabi::fadds_ppc(centerY, (f32)output->y),
                    z0 = gabi::fadds_ppc(centerZ, (f32)output->z);
                quad->vertices[0].value.x = x0;
                quad->vertices[0].value.y = y0;
                quad->vertices[0].value.z = z0;
                offset = gabi::fmuls_ppc(width,
                                         gabi::fmuls_ppc(widthScale, (f32)(at<WAVE_EFF>(particle)->mSpeed)));
                skewY = (f32)(at<WavePacket>(packet)->mSkewDir);
                skew = (f32)(at<WavePacket>(packet)->mSkewWidth);
                input->x =
                    skewY < zero ? gabi::fnmsubs(offset, skew, width) : gabi::fmadds(offset, skew, width);
                input->y = height;
                input->z = zero;
                sdk::MTXMultVec(view.get(), input.get(), output.get());
                quad->vertices[1].value.x = gabi::fadds_ppc(centerX, (f32)output->x);
                quad->vertices[1].value.y = gabi::fadds_ppc(centerY, (f32)output->y);
                quad->vertices[1].value.z = gabi::fadds_ppc(centerZ, (f32)output->z);
                input->x = width;
                input->y = zero;
                input->z = zero;
                sdk::MTXMultVec(view.get(), input.get(), output.get());
                quad->vertices[2].value.x = gabi::fadds_ppc(centerX, (f32)output->x);
                quad->vertices[2].value.y = gabi::fadds_ppc(centerY, (f32)output->y);
                quad->vertices[2].value.z = gabi::fadds_ppc(centerZ, (f32)output->z);
                input->x = -width;
                input->y = zero;
                input->z = zero;
                sdk::MTXMultVec(view.get(), input.get(), output.get());
                quad->vertices[3].value.x = gabi::fadds_ppc(centerX, (f32)output->x);
                quad->vertices[3].value.y = gabi::fadds_ppc(centerY, (f32)output->y);
                quad->vertices[3].value.z = gabi::fadds_ppc(centerZ, (f32)output->z);
                if (!(u32)(*at<be<u32>>(WaveOffsetsInitialized))) {
                    f32 values[12] = {zero,      zero, zero,  uvScale, zero, uvWide,
                                      scalarOne, zero, uvTwo, uvTwo,   zero, uvScale};
                    for (u32 k = 0; k < 12; k++)
                        at<be<f32>>(WaveOffsets)[k] = values[k];
                    *at<be<u32>>(WaveOffsetsInitialized) = 1;
                }
                u32 geometry = ea(&at<WavePacket>(packet)->mGeometry[i]);
                u32 selected = (u32)(at<Geometry>(geometry)->selected),
                    buffer = (u32)(bufferSlot(geometry, selected)->buffer), end = buffer + 64;
                if (buffer < end)
                    for (u32 cursor = buffer; cursor < end; cursor += 32)
                        for (u32 k = 0; k < 32; k += 4)
                            at<CacheLine>(cursor & ~31u)->words[k / 4] = 0;
                selected = (u32)(at<Geometry>(geometry)->selected);
                buffer = (u32)(bufferSlot(geometry, selected)->buffer);
                f32 texture[4][2] = {{zero, zero}, {one, zero}, {one, one}, {zero, one}};
                for (u32 corner = 0; corner < 4; corner++) {
                    for (u32 axis = 0; axis < 3; axis++)
                        at<Vertex>(buffer + corner * u32(sizeof(Vertex)))->position.channels[axis] =
                            gabi::fadds_ppc((f32)(quad->vertices[corner].channels[axis]),
                                            (f32)(at<Vector>(WaveOffsets)->channels[axis]));
                    at<Vertex>(buffer + corner * u32(sizeof(Vertex)))->u = texture[corner][0];
                    at<Vertex>(buffer + corner * u32(sizeof(Vertex)))->v = texture[corner][1];
                }
                selected = (u32)(at<Geometry>(geometry)->selected);
                u32 slot = geometry + selected * u32(sizeof(BufferSlot));
                sdk::flushBuffer(ea(&at<BufferSlot>(slot)->flushObject), 0,
                                 (u32)(at<BufferSlot>(slot)->size));
                at<Geometry>(geometry)->selected = (u32)(at<Geometry>(geometry)->selected) == 0 ? 1 : 0;
            }
        }
        i++;
    }
    sdk::drawWavePacket(packet, draw);
}
} // namespace RainDraw

using namespace RainDraw;
/* WWHD glue (tools/verify/autoglue.py)
 */
