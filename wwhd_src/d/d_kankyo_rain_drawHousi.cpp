/* Weather particles.
 * HD: shader uniforms and double-buffered geometry replace immediate GX rendering.
 */
// HD: shader uniforms and cached double-buffered geometry replace GX draws.
#include "d/d_kankyo_rain_cloudShadowMove.h"
using namespace RainReadable;
namespace d_kankyo_rain_cpp {
/* 025727BC */
static void dKyr_drawHousi() {
  u32 packet = (u32)view<Environment>(environment())->mpHousiPacket;
  if (!(s16)view<dKankyo_housi_Packet>(packet)->mCount)
    return;
  gabi::Local<Color> color0, color1;
  gabi::Local<ColorF> floatColor;
  gabi::Local<Matrix> matrix, rotation;
  gabi::Local<cXyz> offset, transformed;
  gabi::Local<Quad> quad;
  color0->rgba = 0xE5FFC8FF;
  color1->rgba = 0x43D2CAFF;
  f32 alphaScale = (f32)constant(AlphaByteScale),
      size = (f32)constant(HousiSize);
  if (!(u32)view<GameInfo>(gameInfo())->view)
    return;
  sdk::MTXInverse<void>(
      gabi::ea(&view<View>(view<GameInfo>(gameInfo())->view)->rotation),
      matrix.get());
  f32 rotationScale = (f32)constant(RenderRotationScale);
  for (u32 pass = 0; pass < 2; pass++) {
    for (u32 i = 0; i < 300; i++)
      view<dKankyo_housi_Packet>(packet)->mDrawn[pass * 300 + i] = 0;
    sdk::MTXRotDeg<void>(
        rotation.get(), 90,
        gabi::fmuls_ppc((f32)(u32)housiRotation(), rotationScale));
    sdk::MTXConcat<void>(matrix.get(), rotation.get(), matrix.get());
    s16 count = (s16)view<dKankyo_housi_Packet>(packet)->mCount;
    f32 phaseScale = (f32)constant(HousiPhaseScale), zero = (f32)constant(Zero),
        opacityScale = (f32)constant(ReflectionOpacityScale),
        texOne = (f32)constant(TexOne),
        heightRange = (f32)constant(ReflectionHeightRange),
        deform = (f32)constant(HousiDeform),
        heightMax = (f32)constant(ReflectionHeightMax),
        one = (f32)constant(One), negativeSize = -size;
    for (s32 i = 0; i < count; i++) {
      u32 game = gameInfo(),
          particle = gabi::ea(&view<dKankyo_housi_Packet>(packet)->mEffect[i]);
      f32 worldX = gabi::fadds_ppc((f32)view<HOUSI_EFF>(particle)->mBasePos.x,
                                   (f32)view<HOUSI_EFF>(particle)->mPos.x);
      f32 worldY = gabi::fadds_ppc((f32)view<HOUSI_EFF>(particle)->mBasePos.y,
                                   (f32)view<HOUSI_EFF>(particle)->mPos.y);
      f32 worldZ = gabi::fadds_ppc((f32)view<HOUSI_EFF>(particle)->mBasePos.z,
                                   (f32)view<HOUSI_EFF>(particle)->mPos.z);
      u32 player = (u32)view<GameInfo>(game)->player;
      if (pass == 1) {
        if (i == 0)
          for (u32 c = 0; c < 3; c++) {
            color0->channels[c] = 0;
            color1->channels[c] = 0;
          }
        f32 playerY = (f32)view<Player>(player)->currentPos.y,
            alpha = (f32)view<HOUSI_EFF>(particle)->mAlpha;
        if (worldY > gabi::fadds_ppc(playerY, heightMax) ||
            worldY < gabi::fsubs_ppc(playerY, heightRange) || !(alpha > zero)) {
          count = (s16)view<dKankyo_housi_Packet>(packet)->mCount;
          continue;
        }
        f32 ratio =
            (f32)((f64)gabi::fsubs_ppc(worldY, playerY) / (f64)heightMax);
        f32 faded = gabi::fmuls_ppc(gabi::fmuls_ppc(alpha, opacityScale),
                                    gabi::fsubs_ppc(one, ratio));
        color0->channels[3] = (u8)gabi::ftoi(faded);
        worldY = gabi::fsubs_ppc((f32)view<Player>(player)->currentPos.y,
                                 heightRange);
      } else
        color0->channels[3] = (u8)gabi::ftoi(gabi::fmuls_ppc(
            (f32)view<HOUSI_EFF>(particle)->mAlpha, alphaScale));
      u32 angleX = sdk::floatAngle<u32>(gabi::fmuls_ppc(
          (f32)view<HOUSI_EFF>(particle)->mScale.x, phaseScale));
      f32 dx = gabi::fmuls_ppc(sampleSine(angleX), deform);
      u32 angleY = sdk::floatAngle<u32>(gabi::fmuls_ppc(
          (f32)view<HOUSI_EFF>(particle)->mScale.y, phaseScale));
      f32 dy = gabi::fmuls_ppc(sampleSine(angleY), deform);
      f32 cornerX[4] = {gabi::fsubs_ppc(size, dy), gabi::fsubs_ppc(dy, size),
                        gabi::fsubs_ppc(dy, size), gabi::fsubs_ppc(size, dy)};
      f32 cornerY[4] = {gabi::fsubs_ppc(size, dx), gabi::fadds_ppc(size, dx),
                        gabi::fsubs_ppc(dx, size),
                        gabi::fsubs_ppc(negativeSize, dx)};
      for (u32 c = 0; c < 4; c++) {
        offset->x = cornerX[c];
        offset->y = cornerY[c];
        offset->z = zero;
        sdk::MTXMultVec<void>(matrix.get(), offset.get(), transformed.get());
        quad->vertices[c].x = gabi::fadds_ppc(worldX, (f32)transformed->x);
        quad->vertices[c].y = gabi::fadds_ppc(worldY, (f32)transformed->y);
        quad->vertices[c].z = gabi::fadds_ppc(worldZ, (f32)transformed->z);
      }
      u32 index = pass * 300 + (u32)i;
      view<dKankyo_housi_Packet>(packet)->mDrawn[index] = 1;
      sdk::convertColor<void>(floatColor.get(), color0.get());
      sdk::setUniformColor<void>(
          gabi::ea(&view<dKankyo_housi_Packet>(packet)->mUniforms[index]),
          floatColor.get(), 0);
      sdk::convertColor<void>(floatColor.get(), color1.get());
      sdk::setUniformColor<void>(
          gabi::ea(&view<dKankyo_housi_Packet>(packet)->mUniforms[index]),
          floatColor.get(), 1);
      u32 geometry =
              gabi::ea(&view<dKankyo_housi_Packet>(packet)->mGeometry[index]),
          selected = (u32)view<Geometry>(geometry)->selected,
          buffer = (u32)bufferSlot(geometry, selected)->buffer,
          end = buffer + 64;
      for (u32 cursor = buffer; cursor < end; cursor += 32)
        for (u32 word = 0; word < 32; word += 4)
          view<CacheLine>(cursor & ~31u)->words[word / 4] = 0;
      selected = (u32)view<Geometry>(geometry)->selected;
      buffer = (u32)bufferSlot(geometry, selected)->buffer;
      for (u32 c = 0; c < 4; c++) {
        for (u32 component = 0; component < 12; component += 4)
          view<Vertex>(buffer)[c].position.words[component / 4] =
              (u32)quad->words[c][component / 4];
        view<Vertex>(buffer)[c].u = c == 0 || c == 3 ? zero : texOne;
        view<Vertex>(buffer)[c].v = c < 2 ? zero : texOne;
      }
      selected = (u32)view<Geometry>(geometry)->selected;
      u32 slot = gabi::ea(bufferSlot(geometry, selected));
      sdk::flushBuffer<void>(gabi::ea(view<BufferSlot>(slot)->flushObject), 0,
                             (u32)view<BufferSlot>(slot)->size);
      view<Geometry>(geometry)->selected =
          (u32)view<Geometry>(geometry)->selected == 0 ? 1 : 0;
      count = (s16)view<dKankyo_housi_Packet>(packet)->mCount;
    }
  }
  sdk::drawHousiPacket<void>(packet);
}
} // namespace d_kankyo_rain_cpp

using namespace d_kankyo_rain_cpp;
/* WWHD glue (tools/verify/autoglue.py)
*/
