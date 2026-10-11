/* Weather particles.
 * HD: shader uniforms and double-buffered geometry replace immediate GX rendering.
 */
// HD: shader uniforms and cached double-buffered geometry replace GX draws.
#include "d/d_kankyo_rain_cloudShadowMove.h"
using namespace RainReadable;
namespace d_kankyo_rain_cpp {
/* 025720D8 */
static void drawPoison() {
  environment();
  u32 packet = (u32)view<Environment>(environment())->mpPoisonPacket;
  if (!(u32)view<GameInfo>(gameInfo())->view)
    return;
  gabi::Local<Matrix> matrix, rotation;
  gabi::Local<Color> color0, color1;
  gabi::Local<ColorF> floatColor;
  gabi::Local<cXyz> offset, transformed;
  gabi::Local<Quad> quad;
  sdk::MTXInverse<void>(
      gabi::ea(&view<View>(view<GameInfo>(gameInfo())->view)->rotation),
      matrix.get());
  const u8 initial0[3] = {45, 136, 170}, initial1[3] = {109, 60, 205};
  for (u32 c = 0; c < 3; c++) {
    color0->channels[c] = initial0[c];
    color1->channels[c] = initial1[c];
  }
  sdk::reinitializeRenderer<void>();
  sdk::MTXRotDeg<void>(
      rotation.get(), 90,
      gabi::fmuls_ppc((f32)view<dKankyo_poison_Packet>(packet)->mRot,
                      (f32)constant(RenderRotationScale)));
  sdk::MTXConcat<void>(matrix.get(), rotation.get(), matrix.get());
  s32 count = (s32)(u32)view<Environment>(environment())->mPoisonCount;
  f32 zero = (f32)constant(Zero), one = (f32)constant(One),
      threshold = (f32)constant(DrawAlphaThreshold),
      alphaScale = (f32)constant(AlphaByteScale);
  for (s32 i = 0; i < count; i++) {
    u32 particle = gabi::ea(&view<dKankyo_poison_Packet>(packet)->mEff[i]);
    view<dKankyo_poison_Packet>(packet)->mDrawn[i] = 0;
    f32 alpha = (f32)view<POISON_EFF>(particle)->mAlpha,
        radius = (f32)view<POISON_EFF>(particle)->mSize;
    if (alpha > threshold) {
      f32 phase = (f32)(s32)((u32)i * 4000);
      f32 counter = (f32)(s32)(u32)view<dKankyo_poison_Packet>(
                        view<Environment>(environment())->mpPoisonPacket)
                        ->mCount;
      u16 angle = (u16)gabi::ftoi(
          gabi::fmadds(counter, (f32)constant(PoisonPhaseScale), phase));
      f32 cosine = std::fabs(sampleCosine(angle));
      f32 squared = gabi::fmuls_ppc(cosine, cosine);
      f32 common = gabi::fmuls_ppc(squared, (f32)constant(PoisonColorCommon));
      f32 c0[3] = {gabi::fadds_ppc((f32)constant(PoisonRedBase), common),
                   gabi::fadds_ppc((f32)constant(PoisonGreenBase), common),
                   gabi::fmadds(squared, (f32)constant(PoisonBlueSlope),
                                (f32)constant(PoisonBlueBase))};
      f32 c1[3] = {
          gabi::fmadds(squared, (f32)constant(PoisonSecondRedSlope),
                       (f32)constant(PoisonSecondRedBase)),
          gabi::fmadds(squared, (f32)constant(PoisonSecondGreenSlope),
                       (f32)constant(PoisonSecondGreenBase)),
          gabi::fmadds(squared, (f32)constant(PoisonColorCommon), alphaScale)};
      for (u32 c = 0; c < 3; c++) {
        color0->channels[c] = (u8)gabi::ftoi(c0[c]);
        color1->channels[c] = (u8)gabi::ftoi(c1[c]);
      }
      color0->channels[3] = (u8)gabi::ftoi(
          gabi::fmuls_ppc((f32)view<POISON_EFF>(particle)->mAlpha, alphaScale));
      f32 world[3];
      for (u32 c = 0; c < 3; c++)
        world[c] = gabi::fadds_ppc(
            (f32)vectorComponent(&view<dKankyo_poison_Packet>(packet)->mBasePos,
                                 c),
            (f32)vectorComponent(&view<POISON_EFF>(particle)->mPos, c));
      f32 negative = -radius;
      for (u32 corner = 0; corner < 4; corner++) {
        offset->x = corner == 0 || corner == 3 ? negative : radius;
        offset->y = corner < 2 ? radius : negative;
        offset->z = zero;
        sdk::MTXMultVec<void>(matrix.get(), offset.get(), transformed.get());
        for (u32 c = 0; c < 3; c++)
          vectorComponent(&quad->vertices[corner], c) = gabi::fadds_ppc(
              world[c], (f32)vectorComponent(transformed.get(), c));
      }
      view<dKankyo_poison_Packet>(packet)->mDrawn[i] = 1;
      sdk::convertColor<void>(floatColor.get(), color0.get());
      sdk::setUniformColor<void>(
          gabi::ea(&view<dKankyo_poison_Packet>(packet)->mUniforms[i]),
          floatColor.get(), 0);
      sdk::convertColor<void>(floatColor.get(), color1.get());
      sdk::setUniformColor<void>(
          gabi::ea(&view<dKankyo_poison_Packet>(packet)->mUniforms[i]),
          floatColor.get(), 1);
      u32 geometry =
              gabi::ea(&view<dKankyo_poison_Packet>(packet)->mGeometry[i]),
          selected = (u32)view<Geometry>(geometry)->selected,
          buffer = (u32)bufferSlot(geometry, selected)->buffer,
          end = buffer + 64;
      for (u32 cursor = buffer; cursor < end; cursor += 32)
        for (u32 word = 0; word < 32; word += 4)
          view<CacheLine>(cursor & ~31u)->words[word / 4] = 0;
      selected = (u32)view<Geometry>(geometry)->selected;
      buffer = (u32)bufferSlot(geometry, selected)->buffer;
      for (u32 corner = 0; corner < 4; corner++) {
        for (u32 c = 0; c < 12; c += 4)
          view<Vertex>(buffer)[corner].position.words[c / 4] =
              (u32)quad->words[corner][c / 4];
        view<Vertex>(buffer)[corner].u =
            corner == 0 || corner == 3 ? zero : one;
        view<Vertex>(buffer)[corner].v = corner < 2 ? zero : one;
      }
      selected = (u32)view<Geometry>(geometry)->selected;
      u32 slot = gabi::ea(bufferSlot(geometry, selected));
      sdk::flushBuffer<void>(gabi::ea(view<BufferSlot>(slot)->flushObject), 0,
                             (u32)view<BufferSlot>(slot)->size);
      view<Geometry>(geometry)->selected =
          (u32)view<Geometry>(geometry)->selected == 0 ? 1 : 0;
    }
    count = (s32)(u32)view<Environment>(environment())->mPoisonCount;
  }
  sdk::drawPoisonPacket<void>(packet);
}
} // namespace d_kankyo_rain_cpp

using namespace d_kankyo_rain_cpp;
/* WWHD glue (tools/verify/autoglue.py)
*/
