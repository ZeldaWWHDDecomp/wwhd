/* Weather particles.
 * HD: shader uniforms and double-buffered geometry replace immediate GX rendering.
 */
// HD: shader uniforms and cached double-buffered geometry replace GX draws.
#include "d/d_kankyo_rain_cloudShadowMove.h"
using namespace RainReadable;
namespace d_kankyo_rain_cpp {
/* 025712A8 */
static void dKyr_drawRain() {
  u32 packet = (u32)view<Environment>(environment())->mpRainPacket,
      camera = (u32)view<GameInfo>(gameInfo())->camera;
  gabi::Local<cXyz> wind, offset, center, scaled, delta, input, output;
  gabi::Local<Matrix> viewMatrix, rotation;
  gabi::Local<Color> primary, secondary;
  gabi::Local<ColorF> color;
  gabi::Local<Quad> quad;
  sdk::getWindVectorPower<void>(wind.get());
  if ((u32)view<Environment>(environment())->mSnowCount)
    return;
  f32 zero = (f32)constant(Zero);
  offset->x = zero;
  offset->y = (f32)constant(RainFallVelocity);
  offset->z = zero;
  if (!(s16)view<dKankyo_rain_Packet>(packet)->mRainCount)
    return;
  primary->rgba = 0xFFFFFF07u;
  secondary->rgba = 0x80808007u;
  if (!(u32)view<GameInfo>(gameInfo())->view)
    return;
  sdk::MTXInverse<void>(
      gabi::ea(&view<View>(view<GameInfo>(gameInfo())->view)->rotation),
      viewMatrix.get());
  sdk::MTXRotDeg<void>(rotation.get(), 90,
                       gabi::fmuls_ppc((f32)(u32)rainRotation(),
                                       (f32)constant(RenderRotationScale)));
  sdk::MTXConcat<void>(viewMatrix.get(), rotation.get(), viewMatrix.get());
  f32 one = (f32)constant(One), shift = (f32)constant(RainShift),
      distanceScale = (f32)constant(RainDistanceScale),
      uvDepth = (f32)constant(RainUvDepth),
      widthScale = (f32)constant(HousiPhaseScale),
      step = (f32)constant(RainOffsetStep),
      base = (f32)constant(RainOffsetBase);
  for (s32 i = 0; i < (s16)view<dKankyo_rain_Packet>(packet)->mRainCount; i++) {
    u32 particle = gabi::ea(&view<dKankyo_rain_Packet>(packet)->mEff[i]);
    view<dKankyo_rain_Packet>(packet)->mDrawn[i] = 0;
    if (!((f32)view<RAIN_EFF>(particle)->mAlpha > zero))
      continue;
    center->x = gabi::fadds_ppc((f32)view<RAIN_EFF>(particle)->mBasePos.x,
                                (f32)view<RAIN_EFF>(particle)->mPos.x);
    center->y = gabi::fadds_ppc((f32)view<RAIN_EFF>(particle)->mBasePos.y,
                                (f32)view<RAIN_EFF>(particle)->mPos.y);
    center->z = gabi::fadds_ppc((f32)view<RAIN_EFF>(particle)->mBasePos.z,
                                (f32)view<RAIN_EFF>(particle)->mPos.z);
    sdk::scaleVector<void>(
        gabi::ea(&view<dKankyo_rain_Packet>(packet)->mCenterDelta),
        scaled.get(), (f32)view<dKankyo_rain_Packet>(packet)->mCenterDeltaMul);
    sdk::scaleVector<void>(scaled.get(), delta.get(),
                           (f32)constant(RainCenterBias));
    sdk::addVector<void>(center.get(), delta.get(), center.get());
    f32 squared = sdk::distanceSquared<f32>(
        center.get(), gabi::ea(&view<Camera>(camera)->eye));
    f32 ratio = (f32)(sdk::squareRoot<f64>((f64)squared) / (f64)distanceScale);
    if (ratio > one)
      ratio = one;
    primary->channels[3] = (u8)gabi::ftoi(
        gabi::fmuls_ppc(gabi::fmuls_ppc((f32)constant(RainAlphaScale),
                                        (f32)view<RAIN_EFF>(particle)->mAlpha),
                        gabi::fsubs_ppc(one, ratio)));
    view<dKankyo_rain_Packet>(packet)->mDrawn[i] = 1;
    sdk::convertColor<void>(color.get(), primary.get());
    u32 material = gabi::ea(&view<dKankyo_rain_Packet>(packet)->mUniforms[i]);
    sdk::setUniformColor<void>(material, color.get(), 0);
    sdk::convertColor<void>(color.get(), secondary.get());
    sdk::setUniformColor<void>(material, color.get(), 1);
    squared = sdk::distanceSquared<f32>(center.get(),
                                        gabi::ea(&view<Camera>(camera)->eye));
    ratio = gabi::fadds_ppc(
        (f32)(sdk::squareRoot<f64>((f64)squared) / (f64)distanceScale),
        (f32)constant(CloudPointStep));
    if (ratio > one)
      ratio = one;
    f32 speed = (f32)view<dKankyo_rain_Packet>(packet)->mCenterDeltaMul;
    f32 x = gabi::fadds_ppc(
        gabi::fmadds(
            gabi::fmuls_ppc(
                (f32)view<dKankyo_rain_Packet>(packet)->mCenterDelta.x, speed),
            widthScale, (f32)wind->x),
        gabi::fmadds((f32)(i & 7), shift, (f32)offset->x));
    f32 z = gabi::fadds_ppc(
        gabi::fmadds(
            gabi::fmuls_ppc(
                (f32)view<dKankyo_rain_Packet>(packet)->mCenterDelta.z, speed),
            widthScale, (f32)wind->z),
        gabi::fmadds((f32)(i & 3), shift, (f32)offset->z));
    f32 y = gabi::fadds_ppc(
        gabi::fmadds((f32)view<dKankyo_rain_Packet>(packet)->mCenterDelta.y,
                     speed, (f32)wind->y),
        (f32)offset->y);
    f32 thickness = gabi::fmadds((f32)constant(RainThicknessScale), ratio,
                                 (f32)constant(CloudVerticalGain));
    f32 dx = gabi::fmuls_ppc(x, thickness), dy = gabi::fmuls_ppc(y, thickness),
        dz = gabi::fmuls_ppc(z, thickness);
    f32 halfWidth = gabi::fadds_ppc(
        ((f32)i / (f32)constant(ParticleIndexScale)), (f32)constant(RainWidth));
    for (u32 corner = 0; corner < 4; corner++) {
      input->x = corner == 0 || corner == 3 ? halfWidth : -halfWidth;
      input->y = zero;
      input->z = zero;
      sdk::MTXMultVec<void>(viewMatrix.get(), input.get(), output.get());
      f32 px = gabi::fadds_ppc((f32)center->x, (f32)output->x),
          py = gabi::fadds_ppc((f32)center->y, (f32)output->y),
          pz = gabi::fadds_ppc((f32)center->z, (f32)output->z);
      quad->vertices[corner].x = corner < 2 ? gabi::fsubs_ppc(px, dx) : px;
      quad->vertices[corner].y = corner < 2 ? gabi::fsubs_ppc(py, dy) : py;
      quad->vertices[corner].z = corner < 2 ? gabi::fsubs_ppc(pz, dz) : pz;
    }
    f32 angle = (f32)constant(OffsetExtent);
    for (u32 copy = 0; copy < 4; copy++) {
      if (!(u32)rainOffsetsInitialized()) {
        f32 values[12] = {angle, zero, zero,  zero,    angle, angle,
                          angle, base, angle, uvDepth, step,  uvDepth};
        for (u32 k = 0; k < 12; k++)
          vectorComponent(&rainOffsets()[k / 3], k % 3) = values[k];
        rainOffsetsInitialized() = 1;
      }
      u32 geometry = gabi::ea(
          &view<dKankyo_rain_Packet>(packet)->mGeometry[(u32)i * 4 + copy]);
      u32 selected = (u32)view<Geometry>(geometry)->selected,
          buffer = (u32)bufferSlot(geometry, selected)->buffer,
          end = buffer + 64;
      if (buffer < end)
        for (u32 cursor = buffer; cursor < end; cursor += 32)
          for (u32 k = 0; k < 32; k += 4)
            view<CacheLine>(cursor & ~31u)->words[k / 4] = 0;
      selected = (u32)view<Geometry>(geometry)->selected;
      buffer = (u32)bufferSlot(geometry, selected)->buffer;
      for (u32 corner = 0; corner < 4; corner++) {
        for (u32 axis = 0; axis < 3; axis++)
          vectorComponent(&view<Vertex>(buffer)[corner].position.value, axis) =
              gabi::fadds_ppc(
                  (f32)vectorComponent(&quad->vertices[corner], axis),
                  (f32)vectorComponent(&rainOffsets()[copy], axis));
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
  }
  sdk::drawRainPacket<void>(packet);
}
} // namespace d_kankyo_rain_cpp

using namespace d_kankyo_rain_cpp;
/* WWHD glue (tools/verify/autoglue.py)
*/
