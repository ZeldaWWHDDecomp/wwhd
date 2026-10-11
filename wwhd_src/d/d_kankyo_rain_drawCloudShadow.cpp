/* Weather particles.
 * HD: shader uniforms and double-buffered geometry replace immediate GX rendering.
 */
// HD: shader uniforms and cached double-buffered geometry replace GX draws.
#include "d/d_kankyo_rain_cloudShadowMove.h"
using namespace RainReadable;
namespace d_kankyo_rain_cpp {
/* 0257556C */
static void drawCloudShadow() {
  u32 colors = environment(),
      packet = (u32)view<Environment>(environment())->mpMoyaPacket;
  sdk::getWindPower<void>();
  if ((s32)(u32)view<dKankyo_cloud_Packet>(packet)->mCount <= 0)
    return;
  if (!(u32)view<GameInfo>(gameInfo())->view)
    return;
  gabi::Local<Matrix> cameraMatrix, rotation;
  gabi::Local<Color> color0, color1;
  gabi::Local<ColorF> floatColor;
  gabi::Local<cXyz> offset, transformed;
  gabi::Local<Quad> quad;
  sdk::MTXInverse<void>(
      gabi::ea(&view<View>(view<GameInfo>(gameInfo())->view)->rotation),
      cameraMatrix.get());
  u32 rgb = gabi::ea(&view<Environment>(colors)->mBG0_K0);
  if ((u8)view<Environment>(environment())->mMoyaMode == 3 ||
      (u8)view<Environment>(environment())->mMoyaMode == 4)
    rgb = gabi::ea(&view<Environment>(colors)->mBG3_K0);
  // Alpha remains the native stack byte until the per-particle update below.
  for (u32 c = 0; c < 3; c++)
    color0->channels[c] = (u8)view<Color>(rgb)->channels[c];
  for (u32 c = 0; c < 3; c++)
    color1->channels[c] = (u8)view<Color>(rgb)->channels[c];
  sdk::reinitializeRenderer<void>();
  sdk::MTXRotDeg<void>(
      rotation.get(), 90,
      gabi::fmuls_ppc((f32)view<dKankyo_cloud_Packet>(packet)->mRot,
                      (f32)constant(RenderRotationScale)));
  sdk::MTXConcat<void>(cameraMatrix.get(), rotation.get(), cameraMatrix.get());
  s32 index = 0, drawn = 0;

  f32 zero = (f32)constant(Zero), one = (f32)constant(One),
      threshold = (f32)constant(DrawAlphaThreshold),
      alphaScale = (f32)constant(AlphaByteScale);
  for (; index < (s32)(u32)view<dKankyo_cloud_Packet>(packet)->mCount;
       index++) {
    u32 effect = gabi::ea(&view<dKankyo_cloud_Packet>(packet)->mEff[index]);
    f32 alpha = (f32)view<CLOUD_EFF>(effect)->mAlpha,
        radius = (f32)view<CLOUD_EFF>(effect)->mSize;
    if (alpha < threshold)
      continue;
    color0->channels[3] = (u8)gabi::ftoi(gabi::fmuls_ppc(alpha, alphaScale));
    sdk::convertColor<void>(floatColor.get(), color0.get());
    sdk::setUniformColor<void>(
        gabi::ea(&view<dKankyo_cloud_Packet>(packet)->mUniforms[drawn]),
        floatColor.get(), 0);
    sdk::convertColor<void>(floatColor.get(), color1.get());
    sdk::setUniformColor<void>(
        gabi::ea(&view<dKankyo_cloud_Packet>(packet)->mUniforms[drawn]),
        floatColor.get(), 1);
    f32 worldX = gabi::fadds_ppc((f32)view<CLOUD_EFF>(effect)->mBasePos.x,
                                 (f32)view<CLOUD_EFF>(effect)->mPos.x);
    f32 worldY = gabi::fadds_ppc((f32)view<CLOUD_EFF>(effect)->mBasePos.y,
                                 (f32)view<CLOUD_EFF>(effect)->mPos.y);
    f32 worldZ = gabi::fadds_ppc((f32)view<CLOUD_EFF>(effect)->mBasePos.z,
                                 (f32)view<CLOUD_EFF>(effect)->mPos.z);
    f32 negative = -radius;
    for (u32 corner = 0; corner < 4; corner++) {
      offset->x = corner == 0 || corner == 3 ? negative : radius;
      offset->y = corner < 2 ? radius : negative;
      offset->z = zero;
      sdk::MTXMultVec<void>(cameraMatrix.get(), offset.get(),
                            transformed.get());
      quad->vertices[corner].x = gabi::fadds_ppc(worldX, (f32)transformed->x);
      quad->vertices[corner].y = gabi::fadds_ppc(worldY, (f32)transformed->y);
      quad->vertices[corner].z = gabi::fadds_ppc(worldZ, (f32)transformed->z);
    }
    u32 geometry =
            gabi::ea(&view<dKankyo_cloud_Packet>(packet)->mGeometry[drawn]),
        selected = (u32)view<Geometry>(geometry)->selected;
    u32 buffer = (u32)bufferSlot(geometry, selected)->buffer, end = buffer + 64;
    // The HD renderer clears two cache lines before filling four 20-byte vertices.
    for (u32 cursor = buffer; cursor < end; cursor += 32)
      for (u32 word = 0; word < 32; word += 4)
        view<CacheLine>(cursor & ~31u)->words[word / 4] = 0;
    selected = (u32)view<Geometry>(geometry)->selected;
    buffer = (u32)bufferSlot(geometry, selected)->buffer;
    for (u32 corner = 0; corner < 4; corner++) {
      for (u32 component = 0; component < 12; component += 4)
        view<Vertex>(buffer)[corner].position.words[component / 4] =
            (u32)quad->words[corner][component / 4];
      view<Vertex>(buffer)[corner].u = corner == 0 || corner == 3 ? zero : one;
      view<Vertex>(buffer)[corner].v = corner < 2 ? zero : one;
    }
    selected = (u32)view<Geometry>(geometry)->selected;
    u32 slot = gabi::ea(bufferSlot(geometry, selected));
    sdk::flushBuffer<void>(gabi::ea(view<BufferSlot>(slot)->flushObject), 0,
                           (u32)view<BufferSlot>(slot)->size);
    view<Geometry>(geometry)->selected =
        (u32)view<Geometry>(geometry)->selected == 0 ? 1 : 0;

    drawn++;
  }
  sdk::drawCloudPacket<void>(packet, drawn);
}
} // namespace d_kankyo_rain_cpp

using namespace d_kankyo_rain_cpp;
/* WWHD glue (tools/verify/autoglue.py)
*/
