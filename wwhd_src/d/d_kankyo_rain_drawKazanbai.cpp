// HD: shader uniforms and cached double-buffered geometry replace GX draws.
#include "d/d_kankyo_rain_cloudShadowMove.h"
using namespace RainReadable;
namespace rain_draw_ash {
// HD: SafeString callbacks retain their original ordering before stage comparison.
static bool isAshStage(u32 a, u32 b) {
  auto firstString = view<SafeString>(a);
  firstString->text = AshStageAdanmae;
  firstString->vtable = StringVtable;
  u32 game = sdk::getGameInfo<u32>();
  auto secondString = view<SafeString>(b);
  secondString->text = ea(view<GameInfo>(game)->mStartStageName);
  secondString->vtable = StringVtable;
  safeStringCallback(firstString);
  safeStringCallback(firstString);
  u32 first = firstString->text;
  safeStringCallback(secondString);
  u32 second = secondString->text;
  return equalTerminatedStrings(first, second);
}
static void vertex(u32 matrix, u32 in, u32 out, u32 v, f32 x, f32 y, f32 z) {
  sdk::MTXMultVec(at<void>(matrix), at<void>(in), at<void>(out));
  view<cXyz>(v)->x = fadds_ppc(x, view<cXyz>(out)->x);
  view<cXyz>(v)->y = fadds_ppc(y, view<cXyz>(out)->y);
  view<cXyz>(v)->z = fadds_ppc(z, view<cXyz>(out)->z);
}
// HD: cached double buffers replace GX immediate vertices. Reload selection after flush.
static void emit(u32 manager, u32 vertices, u32 offset, f32 zero, f32 one) {
  u32 index = view<Geometry>(manager)->selected,
      buffer = bufferSlot(manager, index)->buffer, end = buffer + 0x40;
  for (u32 cursor = buffer; cursor < end; cursor += 32)
    for (u32 k = 0; k < 32; k++)
      view<be<u8>>(cursor & ~31u)[k] = 0;
  index = view<Geometry>(manager)->selected;
  buffer = bufferSlot(manager, index)->buffer;
  for (u32 i = 0; i < 4; i++) {
    auto v = &view<Vertex>(buffer)[i];
    auto p = &view<cXyz>(vertices)[i];
    v->position.value.x = fadds_ppc(p->x, view<cXyz>(offset)->x);
    v->position.value.y = fadds_ppc(p->y, view<cXyz>(offset)->y);
    v->position.value.z = fadds_ppc(p->z, view<cXyz>(offset)->z);
    v->u = (i == 1 || i == 2) ? one : zero;
    v->v = i >= 2 ? one : zero;
  }
  index = view<Geometry>(manager)->selected;
  auto slot = bufferSlot(manager, index);
  sdk::flushBuffer(at<void>(ea(slot->flushObject)), 0, (u32)slot->size);
  index = view<Geometry>(manager)->selected;
  view<Geometry>(manager)->selected = index == 0 ? 1 : 0;
}
static void ashOffsets(f32 zero, f32 twoHundred) {
  if (ashOffsetsInitialized())
    return;
  vectorComponent(&RainReadable::ashOffsets()[5], 2) = zero;
  vectorComponent(&RainReadable::ashOffsets()[7], 1) = constant(AshOffsetStep);
  vectorComponent(&RainReadable::ashOffsets()[4], 2) =
      constant(AshOffsetNegative);
  vectorComponent(&RainReadable::ashOffsets()[1], 2) = zero;
  vectorComponent(&RainReadable::ashOffsets()[6], 1) = constant(AshYOffset);
  vectorComponent(&RainReadable::ashOffsets()[3], 1) =
      constant(ReflectionHeightRange);
  vectorComponent(&RainReadable::ashOffsets()[0], 0) = zero;
  vectorComponent(&RainReadable::ashOffsets()[2], 2) =
      constant(AshOffsetExtent);
  vectorComponent(&RainReadable::ashOffsets()[0], 1) = zero;
  vectorComponent(&RainReadable::ashOffsets()[0], 2) = zero;
  vectorComponent(&RainReadable::ashOffsets()[2], 1) =
      constant(PoisonColorCommon);
  vectorComponent(&RainReadable::ashOffsets()[1], 0) =
      constant(AshOffsetExtent);
  vectorComponent(&RainReadable::ashOffsets()[3], 2) = constant(OffsetExtent);
  vectorComponent(&RainReadable::ashOffsets()[6], 2) = zero;
  vectorComponent(&RainReadable::ashOffsets()[2], 0) = zero;
  vectorComponent(&RainReadable::ashOffsets()[5], 0) = zero;
  vectorComponent(&RainReadable::ashOffsets()[7], 2) = constant(AshOffsetStep);
  vectorComponent(&RainReadable::ashOffsets()[5], 1) =
      constant(ReflectionHeightMax);
  vectorComponent(&RainReadable::ashOffsets()[4], 1) = zero;
  vectorComponent(&RainReadable::ashOffsets()[7], 0) = twoHundred;
  vectorComponent(&RainReadable::ashOffsets()[6], 0) = twoHundred;
  vectorComponent(&RainReadable::ashOffsets()[1], 1) = constant(AshOffsetStep);
  ashOffsetsInitialized() = 1;
  vectorComponent(&RainReadable::ashOffsets()[3], 0) = constant(OffsetExtent);
  vectorComponent(&RainReadable::ashOffsets()[4], 0) = zero;
}
static void snowOffsets(f32 zero, f32 quarter, f32 negativeQuarter, f32 bias,
                        f32 half) {
  if (snowOffsetsInitialized())
    return;
  vectorComponent(&RainReadable::snowOffsets()[1], 0) = half;
  vectorComponent(&RainReadable::snowOffsets()[1], 2) = zero;
  vectorComponent(&RainReadable::snowOffsets()[0], 1) = zero;
  vectorComponent(&RainReadable::snowOffsets()[0], 2) = zero;
  vectorComponent(&RainReadable::snowOffsets()[3], 2) = bias;
  vectorComponent(&RainReadable::snowOffsets()[1], 1) = quarter;
  vectorComponent(&RainReadable::snowOffsets()[2], 1) = negativeQuarter;
  vectorComponent(&RainReadable::snowOffsets()[0], 0) = zero;
  vectorComponent(&RainReadable::snowOffsets()[3], 0) = bias;
  vectorComponent(&RainReadable::snowOffsets()[3], 1) = bias;
  snowOffsetsInitialized() = 1;
  vectorComponent(&RainReadable::snowOffsets()[2], 2) = half;
  vectorComponent(&RainReadable::snowOffsets()[2], 0) = zero;
}
/* 02572FC4 */
void dKyr_drawKazanbai(u32 unusedMatrix, u32 images) {
  u32 env = sdk::getEnvironment<u32>(),
      initialPacket = (u32)view<Environment>(env)->mpSnowPacket;
  if ((s16)view<dKankyo_snow_Packet>(initialPacket)->mEffCount == 0)
    return;
  Local<AshWork> work;
  bool special = isAshStage(ea(&work->name), ea(&work->stage));
  work->rotationScale = (f32)constant(RenderRotationScale);
  work->zero = (f32)constant(Zero);
  work->one = (f32)constant(One);
  if (special) {
    env = sdk::getEnvironment<u32>();
    u32 packet = (u32)view<Environment>(env)->mpSnowPacket,
        game = sdk::getGameInfo<u32>();
    s32 initialCount = (s16)view<dKankyo_snow_Packet>(packet)->mEffCount;
    u32 player = (u32)view<GameInfo>(game)->player;
    work->reflectionCount = 0;
    if (!initialCount)
      return;
    work->floor = fsubs_ppc((f32)view<Player>(player)->currentPos.y,
                            (f32)constant(ReflectionHeightMax));
    work->particleSize = (f32)constant(AshSizeBase);
    u32 color = ea(&work->ashColor0), color2 = ea(&work->ashColor1);
    view<Color>(color)->channels[0] = 69;
    view<Color>(color)->channels[1] = 60;
    view<Color>(color)->channels[2] = 39;
    view<Color>(color)->channels[3] = 255;
    view<Color>(color2)->channels[0] = 124;
    view<Color>(color2)->channels[1] = 124;
    view<Color>(color2)->channels[2] = 104;
    view<Color>(color2)->channels[3] = 255;
    sdk::prepareAshColor<void>(at<void>(color));
    sdk::prepareAshColor<void>(at<void>(color2));
    game = sdk::getGameInfo<u32>();
    if (!(u32)view<GameInfo>(game)->view)
      return;
    game = sdk::getGameInfo<u32>();
    u32 cameraView = (u32)view<GameInfo>(game)->view,
        matrix = ea(&work->ashMatrix), rotation = ea(&work->ashRotation),
        in = ea(&work->ashInput), out = ea(&work->ashOutput),
        vertices = ea(&work->ashVertices);
    sdk::MTXInverse<void>(
        at<void>(ea(&RainReadable::view<View>(cameraView)->rotation)),
        at<void>(matrix));
    if (!(u32)*view<be<u32>>(images))
      return;
    u32 emitted = 0;
    work->heightRange = (f32)constant(AshHeightRange);
    for (u32 mode = AshNormal; mode < AshPassCount; mode++) {
      if (mode == AshReflection)
        work->reflectionCount = (u16)emitted;
      sdk::MTXRotDeg<void>(
          at<void>(rotation), 0x5A,
          fmuls_ppc((f32)ashRotation(), (f32)work->rotationScale));
      sdk::MTXConcat<void>(at<void>(matrix), at<void>(rotation),
                           at<void>(matrix));
      for (u32 group = AshFalling; group < AshGroupCount; group++) {
        if (mode == AshReflection && group == AshAccumulated)
          continue;
        s32 start = group == AshFalling ? 0 : 200,
            limit =
                group == AshFalling
                    ? (s16)view<dKankyo_snow_Packet>(packet)->mEffCount
                    : 200 +
                          (s16)view<dKankyo_snow_Packet>(packet)->mTamariStart;
        if (start >= limit)
          continue;
        work->phaseZero = (f32)work->zero;
        work->alphaScale = (f32)constant(AlphaByteScale);
        work->tint = 124;
        work->uvOne = (f32)work->one;
        work->sineTable = SineTableAddress;
        for (s32 i = start; i < limit; i++) {
          u32 p = ea(&view<dKankyo_snow_Packet>(packet)->mEff[i]);
          if (mode == AshReflection &&
              (u8)view<SNOW_EFF>(p)->mStatus == ParticleSettled)
            continue;
          f32 y = fadds_ppc((f32)view<SNOW_EFF>(p)->mBasePos.y,
                            (f32)view<SNOW_EFF>(p)->mPos.y),
              z = fadds_ppc((f32)view<SNOW_EFF>(p)->mBasePos.z,
                            (f32)view<SNOW_EFF>(p)->mPos.z),
              x = fadds_ppc((f32)view<SNOW_EFF>(p)->mBasePos.x,
                            (f32)view<SNOW_EFF>(p)->mPos.x);
          u32 copies = 4;
          u8 red, green, blue;
          view<Color>(color)->channels[3] = 255;
          if (mode == AshReflection) {
            f32 floor = (f32)work->floor;
            if (!(y > floor) ||
                !(y < fadds_ppc((f32)work->floor, (f32)work->heightRange)))
              continue;
            f32 alpha = fmuls_ppc(
                (f32)constant(ReflectionHeightRange),
                fsubs_ppc((f32)work->uvOne, fsubs_ppc(y, (f32)work->floor) /
                                                (f32)work->heightRange));
            red = green = blue = 0;
            view<Color>(color2)->channels[0] = 0;
            view<Color>(color2)->channels[1] = 0;
            view<Color>(color2)->channels[2] = 0;
            view<Color>(color)->channels[3] = (u8)ftoi(alpha);
            y = (f32)work->floor;
          } else {
            f32 n = (f32)(s16)view<dKankyo_snow_Packet>(packet)->mEffCount;
            if (i <
                (s32)ftoi(fmuls_ppc(n, (f32)constant(CloudInitialJitter)))) {
              f32 phase =
                  fmadds((f32)(u32)counter(), (f32)constant(AshCounterPhase),
                         (f32)(i * 4000));
              u32 angle = (u32)ftoi(phase) & 0xFFFF;
              f32 cosine = std::fabs((f32)*view<be<f32>>((u32)work->sineTable +
                                                         (angle >> 3) * 8 + 4)),
                  a = fmuls_ppc(cosine, cosine);
              red = (u8)ftoi(fmuls_ppc(a, (f32)work->alphaScale));
              green = blue = (u8)ftoi(fmuls_ppc(a, (f32)work->phaseZero));
              view<Color>(color2)->channels[0] = red;
              view<Color>(color2)->channels[1] = red;
              view<Color>(color2)->channels[2] = blue;
            } else {
              red = 69;
              green = 60;
              blue = 39;
              view<Color>(color2)->channels[0] = 124;
              view<Color>(color2)->channels[1] = 124;
              view<Color>(color2)->channels[2] = 104;
            }
          }
          if (group == AshFalling) {
            view<Color>(color)->channels[0] = red;
            view<Color>(color)->channels[1] = green;
            view<Color>(color)->channels[2] = blue;
          }
          bool settled = group == AshAccumulated ||
                         (u8)view<SNOW_EFF>(p)->mStatus == ParticleSettled;
          f32 size = fadds_ppc((f32)work->particleSize, (f32)(i / 250));
          if (settled) {
            view<Color>(color)->channels[0] = 0;
            view<Color>(color)->channels[1] = 0;
            view<Color>(color)->channels[2] = 0;
            view<Color>(color2)->channels[0] = 0;
            view<Color>(color2)->channels[1] = 0;
            view<Color>(color2)->channels[2] = 0;
            view<Color>(color)->channels[3] = (u8)ftoi(fmuls_ppc(
                (f32)work->alphaScale, (f32)view<SNOW_EFF>(p)->mScale));
            copies = 1;
            size = fmuls_ppc(size, (f32)constant(AshSettledScale));
          }
          view<cXyz>(in)->x = size;
          view<cXyz>(in)->y = size;
          view<cXyz>(in)->z = (f32)work->phaseZero;
          vertex(matrix, in, out, vertices, x, y, z);
          f32 neg = -size;
          view<cXyz>(in)->x = neg;
          view<cXyz>(in)->y = size;
          view<cXyz>(in)->z = (f32)work->phaseZero;
          vertex(matrix, in, out, vertices + 12, x, y, z);
          view<cXyz>(in)->x = neg;
          view<cXyz>(in)->y = neg;
          view<cXyz>(in)->z = (f32)work->phaseZero;
          vertex(matrix, in, out, vertices + 24, x, y, z);
          view<cXyz>(in)->x = size;
          view<cXyz>(in)->y = neg;
          view<cXyz>(in)->z = (f32)work->phaseZero;
          vertex(matrix, in, out, vertices + 36, x, y, z);
          work->vertexZ2 = (f32)view<Quad>(vertices)->vertices[2].z;
          work->vertexZ3 = (f32)view<Quad>(vertices)->vertices[3].z;
          f32 uv1 = (f32)work->uvOne, uv0 = (f32)work->phaseZero,
              height = (f32)work->heightRange;
          work->copyFlags = 1;
          for (u32 k = 0; k < copies; k++) {
            ashOffsets(uv0, height);
            sdk::convertColor<void>(at<void>(ea(&work->ashFloatColor)),
                                    at<void>(color));
            u32 material =
                ea(&view<dKankyo_snow_Packet>(packet)->mAshUniforms[emitted]);
            sdk::setUniformColor<void>(at<void>(material),
                                       at<void>(ea(&work->ashFloatColor)), 0);
            sdk::convertColor<void>(at<void>(ea(&work->ashFloatColor)),
                                    at<void>(color2));
            sdk::setUniformColor<void>(at<void>(material),
                                       at<void>(ea(&work->ashFloatColor)), 1);
            emit(ea(&view<dKankyo_snow_Packet>(packet)->mAshGeometry[emitted]),
                 vertices, ea(&RainReadable::ashOffsets()[k]), uv0, uv1);
            emitted = (emitted + 1) & 0xFFFF;
          }
        }
      }
    }
    sdk::drawAshPacket<void>(at<void>(packet), emitted,
                             (u32)(u16)work->reflectionCount);
    return;
  }
  u32 packet = initialPacket;
  work->snowColor1.channels[0] = 128;
  work->snowColor0.channels[1] = 255;
  work->snowColor1.channels[2] = 128;
  work->snowColor1.channels[1] = 128;
  work->snowColor0.channels[3] = 120;
  work->snowColor0.channels[2] = 255;
  work->snowColor0.channels[0] = 255;
  work->snowColor1.channels[3] = 120;
  work->size = (f32)constant(WindDecay);
  u32 game = sdk::getGameInfo<u32>();
  if (!(u32)view<GameInfo>(game)->view)
    return;
  game = sdk::getGameInfo<u32>();
  u32 cameraView = (u32)view<GameInfo>(game)->view,
      matrix = ea(&work->snowMatrix), rotation = ea(&work->snowRotation),
      in = ea(&work->snowInput), out = ea(&work->snowOutput),
      vertices = ea(&work->snowVertices);
  sdk::MTXInverse<void>(
      at<void>(ea(&RainReadable::view<View>(cameraView)->rotation)),
      at<void>(matrix));
  if (!(u32)*view<be<u32>>(images))
    return;
  sdk::MTXRotDeg<void>(
      at<void>(rotation), 0x5A,
      fmuls_ppc((f32)snowRotation(), (f32)work->rotationScale));
  sdk::MTXConcat<void>(at<void>(matrix), at<void>(rotation), at<void>(matrix));
  sdk::convertColor<void>(at<void>(ea(&work->snowFloatColor)),
                          at<void>(ea(&work->snowColor0)));
  sdk::setUniformColor<void>(
      at<void>(ea(&view<dKankyo_snow_Packet>(packet)->mSnowUniform)),
      at<void>(ea(&work->snowFloatColor)), 0);
  sdk::convertColor<void>(at<void>(ea(&work->snowFloatColor)),
                          at<void>(ea(&work->snowColor1)));
  sdk::setUniformColor<void>(
      at<void>(ea(&view<dKankyo_snow_Packet>(packet)->mSnowUniform)),
      at<void>(ea(&work->snowFloatColor)), 1);
  s32 count = (s16)view<dKankyo_snow_Packet>(packet)->mEffCount;
  if (count > 0) {
    f32 uv0 = (f32)work->zero, uv1 = (f32)work->one;
    work->quarter = (f32)constant(CloudDepth);
    work->particleSize = (f32)constant(SnowOffsetNegative);
    work->floor = (f32)constant(RainUvDepth);
    f32 half = (f32)constant(OffsetExtent);
    for (s32 i = 0; i < count; i++) {
      u32 p = ea(&view<dKankyo_snow_Packet>(packet)->mEff[i]);
      f32 x = fadds_ppc((f32)view<SNOW_EFF>(p)->mBasePos.x,
                        (f32)view<SNOW_EFF>(p)->mPos.x),
          y = fadds_ppc((f32)view<SNOW_EFF>(p)->mBasePos.y,
                        (f32)view<SNOW_EFF>(p)->mPos.y),
          z = fadds_ppc((f32)view<SNOW_EFF>(p)->mBasePos.z,
                        (f32)view<SNOW_EFF>(p)->mPos.z),
          part = (f32)i / (f32)constant(ParticleIndexScale),
          alpha = (f32)view<SNOW_EFF>(p)->mScale;
      f32 motion = fmuls_ppc(fadds_ppc(part, part),
                             (f32)view<dKankyo_snow_Packet>(packet)->mWave),
          shape = fadds_ppc(fnmsubs(alpha, alpha, alpha), alpha),
          size = fmuls_ppc(fadds_ppc((f32)work->size, (f32)(i / 250)), shape),
          neg = -size;
      view<cXyz>(in)->x = size;
      view<cXyz>(in)->y = fsubs_ppc(size, motion);
      view<cXyz>(in)->z = uv0;
      vertex(matrix, in, out, vertices, x, y, z);
      view<cXyz>(in)->x = neg;
      view<cXyz>(in)->y = size;
      view<cXyz>(in)->z = uv0;
      vertex(matrix, in, out, vertices + 12, x, y, z);
      view<cXyz>(in)->x = neg;
      view<cXyz>(in)->y = neg;
      view<cXyz>(in)->z = uv0;
      vertex(matrix, in, out, vertices + 24, x, y, z);
      view<cXyz>(in)->x = size;
      view<cXyz>(in)->y = fsubs_ppc(neg, motion);
      view<cXyz>(in)->z = uv0;
      vertex(matrix, in, out, vertices + 36, x, y, z);
      f32 quarter = (f32)work->quarter,
          negativeQuarter = (f32)work->particleSize;
      for (u32 k = 0; k < 4; k++) {
        snowOffsets(uv0, quarter, negativeQuarter, (f32)work->floor, half);
        emit(ea(&view<dKankyo_snow_Packet>(packet)->mGeometry[(u32)i * 4 + k]),
             vertices, ea(&RainReadable::snowOffsets()[k]), uv0, uv1);
      }
      count = (s16)view<dKankyo_snow_Packet>(packet)->mEffCount;
    }
  }
  sdk::drawSnowPacket<void>(at<void>(packet));
}
} // namespace rain_draw_ash
using namespace rain_draw_ash;
/* WWHD glue (tools/verify/autoglue.py)
*/
