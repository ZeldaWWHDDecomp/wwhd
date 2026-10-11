/**
 * d_a_obj_tapestry.cpp — burnable cloth tapestry.
 * WWHD behavior verified against the recompiled original (unit d_a_obj_tapestry).
 * The nonmatching tww translation unit supplies the canonical class/function order.
 * HD: cloth draws through GX2 material, uniform and double-buffered vertex resources;
 * pointer arguments and full-register results retain the verified HD ABI.
 */
#include "d/actor/d_a_obj_tapestry.h"
static const f32 l_unitScale = 1.0f;
static const f32 l_anglePeriod = 65536.0f;
static const f32 l_zero = 0.0f;
static const f32 l_windAttenuation = 0.6000000238418579f;
static const f32 l_particleYSpeed = 0.10000000149011612f;
static const f32 l_negativeHalf = -0.5f;
static const f32 l_oneFifth = 0.20000000298023224f;
static const f32 l_threeTenths = 0.30000001192092896f;
static const f32 l_half = 0.5f;
static const f32 l_epsilon = 0.009999999776482582f;
static const f32 l_axialFireChance = 0.800000011920929f;
static const f32 l_diagonalFireChance = 0.3499999940395355f;
static const f32 l_rowCount = 8.0f;
static const f32 l_lastRow = 7.0f;
static const f32 l_lastColumn = 5.0f;
static const f32 l_smokeRightChance = 0.4000000059604645f;
static const f32 l_groundOffset = 2.0f;
static const f32 l_gravityUnits = 0.0011111111380159855f;
static const f32 l_rowWindScale = 0.02857142873108387f;
static const f32 l_centerColumn = 2.5f;
static const f32 l_edgeWindScale = 0.1599999964237213f;
static const f32 l_depthLimit = 10.0f;
static const f32 l_depthMidpoint = 6.5f;
static const f32 l_depthBottom = 3.5f;
static const f32 l_groundCheckHeight = 50.0f;
static const f32 l_negativeUnit = -1.0f;
static const f32 l_tangentWeightB = 0.35887500643730164f;
static const f32 l_tangentWeightC = 0.11137499660253525f;
static const f32 l_tangentWeightE = 0.38362500071525574f;
static const f32 l_tangentWeightF = 0.13612499833106995f;
static const f32 l_tangentWeightA = 0.5747500061988831f;
static const f32 l_tangentWeightD = 0.42524996399879456f;
static const f32 l_fireDrag = 0.03999999910593033f;
static const f32 l_hitPower = 800.0f;
static const f32 l_hitRadius = 250.0f;
static const f32 l_collisionXZScale = 1.5f;
static const f32 l_colorRange = 255.0f;
static const f32 l_clothLeft = -297.0f;
static const f32 l_clothTop = 200.0f;
static const f32 l_clothRight = 297.0f;
static const f32 l_inverseLastRow = 0.1428571492433548f;
static const f32 l_collisionYOffset = 29.70000648498535f;
static const f32 l_collisionYScale = 1.2000000476837158f;
static const f32 l_eyeInterpolation = 0.25f;
static const f32 l_cullMinX = -120.0f;
static const f32 l_cullMinY = -340.0f;
static const f32 l_cullMinZ = -30.0f;
static const f32 l_cullMaxX = 120.0f;
static const f32 l_cullMaxY = 100.0f;
static const f32 l_smokeScale = 0.699999988079071f;
static const f32 l_clothDiagonalSquared = 31684.0f;
static const f32 l_clothRowStep = 178.0f;
static const f32 l_negativePi = -3.1415927410125732f;
static const f32 l_pi = 3.1415927410125732f;
static const f32 l_clothColumnStep = 42.42856979370117f;
static const f32 l_clothDepthStep = 40.0f;
static const f32 l_clothCellArea = 3400.18359375f;
static const f32 l_shaderUnit = 1.0f;
static const f32 l_shaderZero = 0.0f;

static f32 l_restDepth;
static f32 l_restHorizontal;
static f32 l_restVertical;
static f32 l_restDiagonal;
static TapestryWindDefaults l_initialWinds;
static cXyz l_smokeParticleScale;
static TapestryWindGuards l_windInitialized;
static u32 l_groundInitialized;
static dBgS_GndChk l_groundCheck;

static TapestryHioDefaults l_hioDefaults;
static TapestryActionTable l_actionInit;
static TapestryActionTable l_actionProc;

static daObjTapestry_HIO_c l_HIO;
static daObjTapestryDrawData_c l_drawData;
// HD: the unit's GHS vtables differ from the GameCube build.
const gc_tu_data gc_tu = {
  .safeString = 0x10030654,
  .gndChk = {0x100306CC, 0x100306DC, 0x100306FC, 0x100306EC},
};

// HD: these views preserve component-wise float copies and GHS integer word copies.
static be<f32>* tapestryComponents(cXyz* v) { return reinterpret_cast<be<f32>*>(v); }
template<class T> static be<u32>* tapestryWords(T* v) { return reinterpret_cast<be<u32>*>(v); }

static u8 *ptr(u32 address) { return gabi::at<u8>(address); }
/* inlined into daObjTapestry_c::create_heap */
static u8 *tapestryResource(s32 index) {
  gabi::Local<be<u32>[2]> archive;
  (*archive)[0] = 0x1003064C;
  (*archive)[1] = 0x10030654;
  return tapestryApi_dRes_control_c_getRes(ptr(gabi::load<u32>(0x101F4F28)),
                          archive.get(), index);
}

static daObjTapestryPLight_c * daObjTapestryPLight_c__ct(daObjTapestryPLight_c *self);
static u32 tapestryParam(daObjTapestry_c *self, u32 width, u32 shift);
BOOL daObjTapestry_Delete(daObjTapestry_c *self);
BOOL daObjTapestry_Execute(daObjTapestry_c *self);
BOOL daObjTapestry_Draw(daObjTapestry_c *self);
BOOL daObjTapestry_IsDelete(daObjTapestry_c *self);
s32 daObjTapestry_Create(daObjTapestry_c *self);
static u8 *daObjTapestry_HIO_c__ct(u8 *self);
static u8 *tapestryTriangleConstruct(u8 *self);
static u8 *tapestryStatusConstruct(u8 *self);
static void daObjTapestryPLight_c__dt(u8 *self, u32 flags);
static u8 *tapestryVertexConstruct(u8 *self);
static u8 *tapestryQuadConstruct(u8 *self);
static u8 *tapestryFireConstruct(u8 *self);
static cXyz *tapestryPositionCopy(cXyz *self, cXyz *other);
static void tapestryLineDestruct(u8 *self, u32 flags);
static void tapestryVertexEmpty(u8 *self, u32 flags);
static void tapestryQuadEmpty(u8 *self, u32 flags);
static void tapestryPositionDestruct(u8 *self, u32 flags);
static void tapestryVertexDestruct(u8 *self, u32 flags);
static void tapestryFireEmpty(u8 *self, u32 flags);
static void daObjTapestry_c__dt(daObjTapestry_c *self, u32 flags);
static void tapestryHIOEmpty(u8 *self, u32 flags);
static u8 *daObjTapestryDrawData_c__ct(u8 *self);
static void tapestryStaticInit();
static void tapestrySignedColor(be<f32> *out, be<s16> *color);
static void tapestryByteColor(be<f32> *out, u8 *color);
static void tapestryMatrixCopy(be<f32> *out, be<f32> *matrix);
static daObjTapestryPacket_c * daObjTapestryPacket_c__ct(daObjTapestryPacket_c *self);
static void daObjTapestryPacket_c__dt(daObjTapestryPacket_c *self, u32 flags);

/* inlined into daObjTapestry_c::setup_action */
static void actionCall(const daObjTapestry_c::Action* action, daObjTapestry_c* actor) {
  s16 index = action->i, delta = action->d;
  u32 self = gabi::ea(actor) + (s32)delta;
  u32 target;
  if (index < 0)
    target = action->f;
  else {
    s16 offset = reinterpret_cast<const be<s16>*>(&action->f)[1];
    u32 vt = gabi::load<u32>(self + (s32)offset);
    target = gabi::load<u32>(vt + (s32)index * 8 + 4);
  }
  gabi::call(target, ptr(self));
}

/* inlined into daObjTapestryPacket_c__dt */
static void tapestryFreeBuffer(u32 vertex) {
  tapestryApi_resetVertexAttribute(ptr(vertex + 0x158));
  u32 buffer = gabi::load<u32>(vertex + 0x250);
  gabi::store<u32>(vertex, 0);
  if (buffer) {
    (void)gabi::load<u32>(vertex + 0x24C);
    u32 allocator = tapestryApi_findFromRoot(ptr(gabi::load<u32>(0x101F8B4C)), ptr(buffer));
    u32 table = gabi::load<u32>(allocator + 0xC),
        target = gabi::load<u32>(table + 0x3C);
    gabi::call(target, ptr(allocator), ptr(gabi::load<u32>(vertex + 0x250)));
    gabi::store<u32>(vertex + 0x24C, 0);
    gabi::store<u32>(vertex + 0x250, 0);
  }
}
/* inlined into daObjTapestryPacket_c__dt */
static void tapestryFreeVectorArray(u32 group, u32 countOffset,
                                    u32 pointerOffset) {
  u32 buffer = gabi::load<u32>(group + pointerOffset);
  if (!buffer)
    return;
  s32 index = 0;
  while (index < gabi::load<s32>(group + countOffset)) {
    u32 object = buffer + (u32)index * 0xF4,
        table = gabi::load<u32>(object + 0xF0),
        target = gabi::load<u32>(table + 0xC);
    gabi::call(target, ptr(object), 2);
    index++;
    buffer = gabi::load<u32>(group + pointerOffset);
  }
  u32 allocator = tapestryApi_findFromRoot(ptr(gabi::load<u32>(0x101F8B4C)),
                                  ptr(buffer));
  u32 table = gabi::load<u32>(allocator + 0xC),
      target = gabi::load<u32>(table + 0x3C);
  gabi::call(target, ptr(allocator),
             ptr(gabi::load<u32>(group + pointerOffset)));
  gabi::store<u32>(group + countOffset, 0);
  gabi::store<u32>(group + pointerOffset, 0);
}

/* inlined into daObjTapestryPacket_c::calc_nrm */
static void tapestryCopyVector(cXyz *destination, cXyz *source) {
  u32 from = gabi::ea(source), to = gabi::ea(destination);
  u32 x = gabi::load<u32>(from), y = gabi::load<u32>(from + 4),
      z = gabi::load<u32>(from + 8);
  gabi::store<u32>(to, x);
  gabi::store<u32>(to + 4, y);
  gabi::store<u32>(to + 8, z);
}
/* inlined into daObjTapestryPacket_c::calc_nrm */
static void tapestryInteriorTangent(u32 before, u32 center, u32 after,
                                    cXyz *out, const f32 *weights) {
  gabi::Local<cXyz> temporary, firstDelta, secondDelta, left, right;
  tapestryApi_cXyz___mi(ptr(center), temporary.get(), ptr(before));
  tapestryCopyVector(firstDelta.get(), temporary.get());
  tapestryApi_cXyz___mi(ptr(after), temporary.get(), ptr(center));
  tapestryCopyVector(secondDelta.get(), temporary.get());
  tapestryApi_cXyz___ml(ptr(before), temporary.get(), weights[0]);
  tapestryCopyVector(left.get(), temporary.get());
  tapestryApi_cXyz___ml(firstDelta.get(), temporary.get(), weights[1]);
  tapestryApi_PSVECAdd(left.get(), temporary.get(), left.get());
  tapestryApi_cXyz___ml(secondDelta.get(), temporary.get(), weights[2]);
  tapestryApi_PSVECAdd(left.get(), temporary.get(), left.get());
  tapestryApi_cXyz___ml(ptr(after), temporary.get(), weights[3]);
  tapestryApi_PSVECAdd(left.get(), temporary.get(), left.get());
  tapestryApi_cXyz___ml(ptr(before), temporary.get(), weights[3]);
  tapestryCopyVector(right.get(), temporary.get());
  tapestryApi_cXyz___ml(firstDelta.get(), temporary.get(), weights[4]);
  tapestryApi_PSVECAdd(right.get(), temporary.get(), right.get());
  tapestryApi_cXyz___ml(secondDelta.get(), temporary.get(), weights[5]);
  tapestryApi_PSVECAdd(right.get(), temporary.get(), right.get());
  tapestryApi_cXyz___ml(ptr(after), temporary.get(), weights[0]);
  tapestryApi_PSVECAdd(right.get(), temporary.get(), right.get());
  tapestryApi_cXyz___mi(right.get(), temporary.get(), left.get());
  tapestryCopyVector(out, temporary.get());
}

/* inlined into daObjTapestryPacket_c::drawSurface */
static void tapestryApplyMaterial(u32 packet, u32 material) {
  u32 target = gabi::load<u32>(gabi::load<u32>(packet + 0xAE4) + 0x2C);
  gabi::call(target, ptr(packet + 0xAD8), ptr(material));
  target = gabi::load<u32>(gabi::load<u32>(packet + 0xA3C) + 0x2C);
  gabi::call(target, ptr(packet + 0xA30), ptr(material));
}
/* inlined into daObjTapestryPacket_c::drawSurface */
static bool tapestryMaterialBindings(u32 material, u32 table) {
  u32 flags = gabi::load<u32>(material + 0xC),
      binding = flags ? gabi::load<u32>(material + 0x10) : 0;
  s16 texture = gabi::load<s16>(binding + 0xC);
  u32 descriptor = gabi::load<u32>(table + 4),
      data = gabi::load<u32>(table + 0xC);
  s16 color = gabi::load<s16>(binding + 0xE),
      position = gabi::load<s16>(binding + 0x10);
  if (color != -1)
    tapestryApi_setPixelUniform(color, ptr(data), ptr(descriptor));
  if (texture != -1)
    tapestryApi_setVertexUniform(texture, ptr(data), ptr(descriptor));
  if (position != -1)
    tapestryApi_setGeometryUniform(position, ptr(data), ptr(descriptor));
  return position != -1;
}
/* inlined into daObjTapestryPacket_c::drawSurface */
static void tapestryBindTexture(u32 packet, u32 material) {
  u32 texture =
      gabi::load<u32>(material + 0x14) ? gabi::load<u32>(material + 0x18) : 0;
  tapestryApi_bindTexture(ptr(packet + 0xED0), ptr(texture + 4), -1, 0);
}

/* inlined into daObjTapestryPacket_c::initializeRenderResources */
static u32 tapestryAllocate(u32 size, u32 alignment) {
  u32 allocator = tapestryApi_getCurrentAllocator(ptr(gabi::load<u32>(0x101F8B4C)));
  u32 target = gabi::load<u32>(gabi::load<u32>(allocator + 0xC) + 0x34);
  return gabi::call<u32>(target, ptr(allocator), size, alignment);
}

/* 0239F198 */
static u8 *daObjTapestry_HIO_c__ct(u8 *self) {
  if (!self)
    self = static_cast<u8*>(operator_new(0x6C));
  if (self) {
    auto* hio = reinterpret_cast<daObjTapestry_HIO_c*>(self);
    hio->mChild = -1;
    hio->mVtable = 0x1003070C;
    hio->mDebugEnabled = 0;
    hio->mDebugSelected = 0;
    hio->mDisplayFlags = 0;
    for (u32 i = 0; i < 23; i++)
      reinterpret_cast<be<u32>*>(&hio->mGravity)[i] = l_hioDefaults.mWords[i];
    hio->mUseDiffuse = 1;
  }
  return self;
}

/* 0239D8BC */
void daObjTapestryPLight_c::plight_delete() {
  if (mActive) {
    tapestryApi_dKy_plight_cut(&mInfluence);
    u32 emitter = gabi::ea((u8 *)mpEmitter);
    if (emitter) {
      u32 flags = gabi::load<u32>(emitter + 0x254);
      gabi::store<u32>(emitter + 0x5C, 0xFFFFFFFF);
      gabi::store<u32>(emitter + 0x254, flags | 1);
      emitter = gabi::ea((u8 *)mpEmitter);
      gabi::store<u32>(emitter + 0x1E4, 0);
      mpEmitter = nullptr;
    }
    mActive = 0;
  }
}

/* 0239C50C */
void daObjTapestryPLight_c::plight_move(cXyz* position, u8* rotation) {
  if (mActive) {
    gabi::Local<cXyz> p;
    gabi::Local<be<u16>[3]> r;
    f32 x = position->x, z = position->z, y = position->y;
    u32 a = gabi::ea(rotation);
    u16 ry = gabi::load<u16>(a + 2), rz = gabi::load<u16>(a + 4),
        rx = gabi::load<u16>(a);
    p->z = z;
    (*r)[0] = rx;
    (*r)[2] = rz;
    (*r)[1] = ry;
    p->y = y;
    p->x = x;
    tapestryApi_tapestrySetPointLight(this, p.get(), r.get());
  }
}

/* 0239C120 */
void daObjTapestryPLight_c::plight_make() {
  tapestryApi_dKy_plight_set(&mInfluence);
  mActive = 1;
}

/* 0239C32C */
void daObjTapestryPLight_c::setPointLight(cXyz* position, u8* rotation) {
  f32 half = l_half, one = l_unitScale;
  f32 random = tapestryApi_cM_rndF(half);
  tapestryApi_cLib_addCalc2(&mIntensity, (f32)(random + one), half,
             l_fireDrag);
  u32 x = (u32)(tapestryWords(position)[0]);
  f32 intensity = (f32)(mIntensity);
  f32 range = l_hitPower * intensity;
  tapestryWords(&mInfluence.mPosition)[0] = x;
  u32 y = (u32)(tapestryWords(position)[1]);
  tapestryWords(&mInfluence.mPosition)[1] = y;
  u32 z = (u32)(tapestryWords(position)[2]);
  s16 signedRange = (s16)gabi::ftoi(range);
  mInfluence.mColor[0] = 600;
  mInfluence.mColor[1] = 400;
  tapestryWords(&mInfluence.mPosition)[2] = z;
  mInfluence.mColor[2] = 120;
  f32 constant = l_hitRadius;
  mInfluence.mPower = (f32)signedRange;
  mInfluence.mFluctuation = constant;
  if (intensity > one) {
    f32 scale = intensity * l_collisionXZScale;
    gabi::Local<cXyz> size;
    size->x = scale;
    size->y = scale;
    size->z = scale;
    u32 emitter = gabi::ea(mpEmitter.get());
    if (!emitter) {
      u32 play = tapestryApi_getGameInfo(),
          particles = gabi::load<u32>(play + 0x5AB0);
      mpEmitter =
          tapestryApi_dPa_control_c_setVariantA(ptr(particles), 4, 0x4004, position,
                           rotation, size.get(), 255, 0, -1, 0, 0, 0);
    } else {
      gabi::store<f32>(emitter + 0x238, scale);
      gabi::store<f32>(emitter + 0x23C, scale);
      gabi::store<f32>(emitter + 0x240, scale);
      gabi::store<u32>(gabi::ea(mpEmitter.get()) + 0x1E4, 0x1047B2DC);
    }
  }
}

/* 0239E270 */
void daObjTapestryFireEff_c::execute(u8 *emitter) {
  f32 factor = (f32)l_HIO.mFireVelocityScale, limit = (f32)l_HIO.mFireVelocityLimit;
  gabi::Local<cXyz> scaled;
  tapestryApi_cXyz___ml(&mVelocity, scaled.get(), factor);
  f32 x = scaled->x, z = scaled->z, negative = -limit;
  if (x < negative)
    x = negative;
  else
    x = ((f32)(x - limit) >= 0.0f ? limit : x);
  if (z < negative)
    z = negative;
  else
    z = ((f32)(z - limit) >= 0.0f ? limit : z);
  u32 a = gabi::ea(emitter);
  gabi::store<f32>(a + 0x30, z);
  f32 y = l_particleYSpeed;
  gabi::store<f32>(a + 0x28, x);
  gabi::store<f32>(a + 0x2C, y);
  tapestryApi_dPa_followEcallBack_execute(this, emitter);
}

/* 0239E224 */
static u8 *daObjTapestryDrawData_c__ct(u8 *self) {
  if (!self)
    self = static_cast<u8*>(operator_new(0x228));
  if (self) {
    u8 *result = tapestryApi_tapestryTextureCoordinates(self);
    tapestryApi_tapestryDrawIndices(result);
  }
  return self;
}

/* 0239E0FC */
void daObjTapestryDrawData_c::ct_tex() {
  for (s32 row = 0; row < 8; row++) {
    f32 uStep = l_oneFifth,
        vStep = l_inverseLastRow;
    f32 v = (f32)row * vStep;
    for (s32 column = 0; column < 6; column++) {
      f32 u = (f32)column * uStep;
      mTexCoords[row][column][0] = u;
      mTexCoords[row][column][1] = v;
    }
  }
}

/* 0239E1A8 */
void daObjTapestryDrawData_c::ct_dl() {
  for (u32 row = 0; row < 7; row++)
    for (u32 column = 0; column < 6; column++) {
      mIndices[row][column][0] = row * 6 + column;
      mIndices[row][column][1] = row * 6 + column + 6;
    }
}

/* 0239992C */
static daObjTapestryPacket_c *
daObjTapestryPacket_c__ct(daObjTapestryPacket_c *self) {
  if (!self)
    self = static_cast<daObjTapestryPacket_c*>(operator_new(0x2520));
  if (!self)
    return self;
  u32 a = gabi::ea(self);
  tapestryApi_bgnPacket_construct(self);
  self->mVtable = 0x10030998;
  self->mMaterialCount = 0;
  u32 list = a + 0x9C;
  if (!list)
    list = gabi::ea(operator_new(8));
  if (list) {
    gabi::store<u32>(list + 4, 0);
    gabi::store<u32>(list, 0);
  }
  u32 vertices = a + 0xA4;
  if (!vertices)
    vertices = gabi::ea(operator_new(0x968));
  if (vertices) {
    tapestryApi_construct_arrayVariantB(ptr(vertices), 4, 0x254, gabi::guest_fn(reinterpret_cast<const void*>(&tapestryVertexConstruct)));
    gabi::store<u32>(vertices + 0x950, 0);
    gabi::store<u32>(vertices + 0x960, 0);
    gabi::store<u32>(vertices + 0x954, 0);
    gabi::store<u8>(vertices + 0x964, 0);
    gabi::store<u32>(vertices + 0x958, 48);
    for (u32 i = 0; i < 4; i++)
      gabi::store<u32>(vertices + i * 0x254, 0);
  }
  tapestryApi_constructIndexBuffer(&self->mIndexBuffer);
  tapestryApi_construct(&self->mLightingMaterial);
  tapestryApi_constructMatPacket(&self->mVertexUniform);
  self->mVertexUniform.mHeader.mVtable = 0x1016EF84;
  tapestryApi_clearMemory(&self->mVertexUniform.mMatrix, 0x34);
  if (a + 0xAA4 == 0)
    static_cast<void>(operator_new(0x30));
  tapestryApi_constructMatPacket(&self->mPixelUniform);
  self->mPixelUniform.mVtable = 0x1016EFB4;
  tapestryApi_clearMemory(&self->mLightingUniforms, 0x2F0);
  f32 zero = l_shaderZero, one = l_shaderUnit;
  for (u32 i = 0; i < 11; i++)
    for (u32 j = 0; j < 4; j++)
      self->mLightingUniforms.mColors[i][j] = j == 3 ? one : zero;
  for (u32 i = 0; i < 3; i++)
    tapestryApi_construct_arrayVariantB(ptr(a + 0xBFC + i * 0x20), 2, 0x10, gabi::guest_fn(reinterpret_cast<const void*>(&tapestryQuadConstruct)));
  for (u32 i = 0; i < 8; i++)
    if (a + 0xC5C + i * 0x30 == 0)
      static_cast<void>(operator_new(0x30));
  for (u32 i = 0; i < 6; i++)
    if (a + 0xDDC + i * 0x10 == 0)
      static_cast<void>(operator_new(0x10));
  self->mShaderFlags = 0;
  tapestryApi_constructTextureResource(&self->mTextureResource);
  tapestryApi_constructTextureSampler(&self->mTextureSampler);
  tapestryApi_construct_arrayVariantB(ptr(a + 0x20C0), 16, 0x2C, gabi::guest_fn(reinterpret_cast<const void*>(&tapestryFireConstruct)));
  tapestryApi_dPa_followEcallBack_dPa_followEcallBack(&self->mSmokeCallback, 0, 0);
  self->mSmokeCallback.__vtbl = 0x1003072C;
  tapestryApi_tapestryLightConstruct(&self->mLight);
  for (u32 buffer = 0; buffer < 2; buffer++)
    for (u32 row = 0; row < 8; row++) {
      f32 nz = gabi::load<f32>(0x101FFBD4), px = gabi::load<f32>(0x101FFBA8),
          ny = gabi::load<f32>(0x101FFBD0), nx = gabi::load<f32>(0x101FFBCC),
          pz = gabi::load<f32>(0x101FFBB0), py = gabi::load<f32>(0x101FFBAC);
      for (u32 column = 0; column < 6; column++) {
        TapestryClothVertices& cloth = self->mCloth[buffer];
        u32 index = row * 6 + column;
        cloth.mPositions[index].x = px;
        cloth.mNormals[index].x = nx;
        cloth.mPositions[index].y = py;
        cloth.mBackNormals[index].x = nx;
        cloth.mBackNormals[index].y = ny;
        cloth.mBackNormals[index].z = nz;
        cloth.mNormals[index].y = ny;
        cloth.mNormals[index].z = nz;
        cloth.mPositions[index].z = pz;
      }
    }
  for (u32 i = 0; i < 48; i++) {
    for (u32 j = 0; j < 3; j++)
      tapestryWords(&self->mVelocity[i])[j] = tapestryWords(&cXyz::Zero)[j];
    self->mVertexFlags[i] = 0;
    self->mFireTimers[i] = 0;
    self->mFireIndices[i] = 255;
  }
  self->mPositionBuffer = 0;
  self->mFireCount = 0;
  for (u32 i = 0; i < 3; i++)
    tapestryApi_PSMTXIdentity(ptr(a + 0x2390 + i * 0x30));
  self->mInverseValid = 1;
  f32 x = gabi::load<f32>(0x101FFBA8), y = gabi::load<f32>(0x101FFBAC),
      range = l_anglePeriod, z = gabi::load<f32>(0x101FFBB0);
  for (cXyz* vector : {&self->mWorldWind, &self->mWind, &self->mFlutter}) {
    vector->x = x;
    vector->y = y;
    vector->z = z;
  }
  self->mWindPhase = 0;
  for (u32 i = 0; i < 3; i++)
    self->mFlutterAngles[i] = (u16)gabi::ftoi(tapestryApi_cM_rndF(range));
  for (cXyz* vector : {&self->mTopWind, &self->mMiddleWind, &self->mBottomWind,
                       &self->mHitPosition, &self->mLocalHitPosition,
                       &self->mHitDirection, &self->mLocalHitDirection}) {
    vector->x = x;
    vector->y = y;
    vector->z = z;
  }
  for (be<u8>* flag : {&self->mHitFire, &self->mDetached, &self->mGroundCollision, &self->mSmokeType})
    *flag = 0;
  one = l_unitScale;
  zero = l_zero;
  self->mHitForce = zero;
  self->mHitRadius = one;
  self->mHitAttenuation = one;
  self->mHitRow = zero;
  self->mHitColumn = zero;
  self->mSmokeCallback.mTimer = 0;
  return self;
}

/* 0239CEF4 */
void daObjTapestryPacket_c::init(u8 *owner) {
  u32 a = gabi::ea(this);
  f32 zero = l_zero;
  if (!gabi::load<u32>(0x1046C228)) {
    gabi::store<f32>(0x1046C118, zero);
    gabi::store<u32>(0x1046C228, 1);
    gabi::store<f32>(0x1046C11C, zero);
    gabi::store<f32>(0x1046C120, l_negativeUnit);
  }
  gabi::store<u32>(a + 0x18, gabi::ea(owner));
  f32 width = (f32)(l_restDepth), nz = gabi::load<f32>(0x1046C120),
      ny = gabi::load<f32>(0x1046C11C);
  f32 tangentX = gabi::load<f32>(0x101FFBCC),
      tangentZ = gabi::load<f32>(0x101FFBD4), nx = gabi::load<f32>(0x1046C118),
      tangentY = gabi::load<f32>(0x101FFBD0);
  tapestryApi_PSMTXTrans(ptr(0x1048D0CC), -width, l_clothLeft,
             l_depthLimit);
  tapestryApi_mDoMtx_stack_c_scaleM(l_clothTop,
             l_clothRight, l_unitScale);
  f32 rowStep = l_inverseLastRow;
  gabi::Local<cXyz> vertex;
  vertex->z = zero;
  for (u32 buffer = 0; buffer < 2; buffer++) {
    f32 columnStep = l_oneFifth;
    for (s32 row = 0; row < 8; row++) {
      vertex->y = (f32)(7 - row) * rowStep;
      for (s32 column = 0; column < 6; column++) {
        vertex->x = (f32)column * columnStep;
        u32 position = a + 0x106C + buffer * 0x6C0 + (row * 6 + column) * 12;
        tapestryApi_PSMTXMultVec(ptr(0x1048D0CC), vertex.get(), ptr(position));
        gabi::store<f32>(position + 0x484, ny);
        gabi::store<f32>(position + 0x488, nz);
        gabi::store<f32>(position + 0x480, nx);
        gabi::store<f32>(position + 0x244, tangentY);
        gabi::store<f32>(position + 0x240, tangentX);
        gabi::store<f32>(position + 0x248, tangentZ);
      }
    }
  }
  u8 *result = tapestryApi_daObjTapestryPacket_c_smokeCallback_init(this);
  tapestryApi_tapestryPacketCalc(result, owner);
  tapestryApi_tapestryRenderInit(this);
}

/* 0239E020 */
void daObjTapestryPacket_c::update() {
  tapestryApi_PSMTXConcat(ptr(0x104B45F8), &mWorldMatrix, &mViewMatrix);
  tapestryApi_J3DDrawBuffer_entryImm(ptr(gabi::load<u32>(0x104B4634)), this, 0);
  tapestryApi_tapestryUploadVertices(this);
}

/* 0239AD14 */
void daObjTapestryPacket_c::calc_acc_spring_sub(cXyz *first, cXyz *second, f32 rest, f32 factor) {
  gabi::Local<cXyz> difference, scaled, force;
  f32 sx = second->x, fy = first->y, fx = first->x, sy = second->y;
  f32 x = fx - sx, sz = second->z, y = fy - sy, fz = first->z;
  difference->x = x;
  difference->y = y;
  difference->z = fz - sz;
  f32 square = tapestryApi_PSVECSquareMag(difference.get());
  f32 distance = tapestryApi_std_sqrtf(square);
  if (distance > l_epsilon) {
    f32 displacement = distance - rest, stiffness = (f32)l_HIO.mStiffness;
    f32 amount = -(f32)((f32)(displacement * stiffness) * factor);
    tapestryApi_cXyz___ml(difference.get(), scaled.get(), amount);
    f32 mass = (f32)l_HIO.mGravity;
    tapestryApi_cXyz___dv(scaled.get(), force.get(), (f32)(distance * mass));
    tapestryApi_PSVECAdd(ptr(gabi::ea(this) + 0x2384), force.get(),
               ptr(gabi::ea(this) + 0x2384));
  }
}

/* 0239AE28 */
void daObjTapestryPacket_c::calc_acc_spring(s32 row, s32 column) {
  u32 a = gabi::ea(this), index = (u32)row * 6 + (u32)column;
  u32 previous = a + 0x106C + ((u32)mPositionBuffer ^ 1) * 0x6C0;
  u8 *position = ptr(previous + index * 12);
  f32 axial = (f32)l_HIO.mAxialSpringScale,
      diagonal = (f32)l_HIO.mDiagonalSpringScale;
  auto spring = [&](u32 neighbor, f32 rest, f32 factor) {
    tapestryApi_tapestrySpringForce(this, position, ptr(previous + neighbor * 12), rest,
               factor);
  };
  if (row - 1 >= 0) {
    u32 above = index - 6;
    spring(above, (f32)(l_restHorizontal), axial);
    if (column - 1 >= 0)
      spring(above - 1, (f32)(l_restDiagonal), diagonal);
    if (column + 1 < 6)
      spring(above + 1, (f32)(l_restDiagonal), diagonal);
  }
  if (row + 1 < 8) {
    u32 below = index + 6;
    spring(below, (f32)(l_restHorizontal), axial);
    if (column - 1 >= 0)
      spring(below - 1, (f32)(l_restDiagonal), diagonal);
    if (column + 1 < 6)
      spring(below + 1, (f32)(l_restDiagonal), diagonal);
  }
  if (column - 1 >= 0)
    spring(index - 1, (f32)(l_restVertical), axial);
  if (column + 1 < 6)
    spring(index + 1, (f32)(l_restVertical), axial);
  f32 vertical = (f32)(l_restHorizontal),
      horizontal = (f32)(l_restVertical);
  vertical = vertical + vertical;
  horizontal = horizontal + horizontal;
  f32 bending = (f32)l_HIO.mBendingSpringScale;
  if (row - 2 >= 0)
    spring(index - 12, vertical, bending);
  if (row + 2 < 8)
    spring(index + 12, vertical, bending);
  if (column - 2 >= 0)
    spring(index - 2, horizontal, bending);
  if (column + 2 < 6)
    spring(index + 2, horizontal, bending);
}

/* 0239B100 */
void daObjTapestryPacket_c::calc_acc_gravity() {
  f32 coefficient = l_gravityUnits,
      mass = (f32)l_HIO.mGravityScale, gravity = (f32)l_HIO.mGravity;
  mAcceleration.y =
      gabi::fmadds(mass * coefficient, gravity, (f32)mAcceleration.y);
}

/* 0239B12C */
void daObjTapestryPacket_c::calc_acc_wave(s32 row, s32 column) {
  u32 a = gabi::ea(this), buffer = (u32)mPositionBuffer,
      index = (u32)row * 6 + (u32)column;
  f32 rowScale = gabi::fmadds((f32)row, l_rowWindScale,
                              l_axialFireChance);
  u32 previous = a + 0x106C + (buffer ^ 1) * 0x6C0 + index * 12,
      acceleration = a + 0x2384;
  gabi::Local<cXyz> wind, flutter, top, middle, bottom;
  tapestryApi_cXyz___ml(&mWind, wind.get(), rowScale);
  tapestryApi_PSVECAdd(ptr(acceleration), wind.get(), ptr(acceleration));
  tapestryApi_cXyz___ml(&mFlutter, wind.get(), rowScale);
  tapestryApi_PSVECAdd(ptr(acceleration), wind.get(), ptr(acceleration));
  u8 active = (u8)mDetached;
  f32 attenuation = l_windAttenuation;
  if (active) {
    f32 center = l_centerColumn,
        distance = std::fabs((f32)column - center);
    f32 scale = gabi::fmadds((f32)(center - distance),
                             l_edgeWindScale, attenuation);
    cXyz *out = row < 2 ? top.get() : row < 5 ? middle.get() : bottom.get();
    u32 source = row < 2 ? a + 0x2454 : row < 5 ? a + 0x2460 : a + 0x246C;
    tapestryApi_cXyz___ml(ptr(source), out, scale);
    tapestryApi_PSVECAdd(ptr(acceleration), out, ptr(acceleration));
  }
  rowScale = (f32)l_HIO.mRandomWind * rowScale;
  f32 half = l_half;
  for (u32 i = 0; i < 3; i++) {
    f32 random = tapestryApi_cM_rnd(), delta = random - half,
        current = (f32)(tapestryComponents(gabi::at<cXyz>(acceleration))[i]);
    tapestryComponents(gabi::at<cXyz>(acceleration))[i] =
                     gabi::fmadds(delta, rowScale, current);
  }
  f32 z = (f32)(gabi::at<cXyz>(previous)->z), limit = l_depthLimit;
  if (z < limit) {
    f32 threshold = l_depthMidpoint,
        force = (f32)l_HIO.mDepthResponse;
    f32 current = (f32)(gabi::at<cXyz>(acceleration)->z);
    if (z > threshold)
      gabi::at<cXyz>(acceleration)->z =
          gabi::fmadds(force, l_particleYSpeed, current);
    else {
      if (z > l_depthBottom)
        force = force * attenuation;
      gabi::at<cXyz>(acceleration)->z = (f32)(current + force);
    }
  }
}

/* 0239B3AC */
void daObjTapestryPacket_c::calc_acc_hit(s32 row, s32 column) {
  f32 strength = (f32)mHitForce,
      epsilon = l_epsilon;
  if (strength > epsilon) {
    f32 rows = l_lastRow, rf = (f32)row / rows;
    f32 columns = l_lastColumn, cf = (f32)column / columns;
    f32 hr = (f32)mHitRow, hc = (f32)mHitColumn;
    f32 dc = cf - hc, dr = rf - hr, square = dc * dc;
    f32 distance = tapestryApi_std_sqrtf(gabi::fmadds(dr, dr, square));
    f32 radius = (f32)mHitRadius;
    if (distance < radius) {
      f32 attenuation = (f32)mHitAttenuation,
          product = distance * attenuation;
      strength = (f32)mHitForce;
      product = product * strength;
      f32 factor = (f32)l_HIO.mHitResponse;
      gabi::Local<cXyz> force;
      tapestryApi_cXyz___ml(&mLocalHitDirection, force.get(),
                 (f32)(product * factor));
      tapestryApi_PSVECAdd(&mAcceleration, force.get(), &mAcceleration);
    }
  }
}

/* 0239B498 */
void daObjTapestryPacket_c::calc_spd(s32 row, s32 column) {
  u32 a = gabi::ea(this), index = (u32)row * 6 + (u32)column;
  u8 flags = (u8)mVertexFlags[index];
  u8 *velocity = ptr(a + 0x1DEC + index * 12);
  f32 friction = -((flags & 2) ? (f32)l_HIO.mGroundFriction : (f32)l_HIO.mFriction);
  tapestryApi_PSVECAdd(velocity, &mAcceleration, velocity);
  gabi::Local<cXyz> drag;
  tapestryApi_cXyz___ml(velocity, drag.get(), friction);
  tapestryApi_PSVECAdd(velocity, drag.get(), velocity);
}

/* 0239B578 */
void daObjTapestryPacket_c::calc_pos_crr(s32 row, s32 column) {
  u32 a = gabi::ea(this), index = (u32)row * 6 + (u32)column;
  u32 position = a + 0x106C + (u32)mPositionBuffer * 0x6C0 + index * 12;
  f32 one = l_unitScale, z = (f32)(gabi::at<cXyz>(position)->z);
  if (z < one)
    gabi::at<cXyz>(position)->z = one;
  if (!(u8)mGroundCollision)
    return;
  u32 check = gabi::ea(&l_groundCheck);
  if ((u32)(l_groundInitialized) == 0) {
    l_groundInitialized = 1;
    tapestryApi_cBgS_GndChk_cBgS_GndChk(ptr(check));
    l_groundCheck.mGrp = 1;
    l_groundCheck.mPolyPassChk = check + 0x40;
    l_groundCheck.cBgS_Chk::__vtbl = gc_tu.gndChk.v10;
    l_groundCheck.mbCamThrough = 0;
    l_groundCheck.mbRopeThrough = 0;
    l_groundCheck.dBgS_GrpPassChk::__vtbl = gc_tu.gndChk.v4C;
    l_groundCheck.mbObjThrough = 1;
    gabi::store<u32>(check + 4, check + 0x4C);
    l_groundCheck.cBgS_PolyInfo::__vtbl = gc_tu.gndChk.v20;
    l_groundCheck.mbBombThrough = 0;
    l_groundCheck.mbBoomerangThrough = 0;
    l_groundCheck.mbArrowThrough = 0;
    l_groundCheck.mbLinkThrough = 0;
    l_groundCheck.dBgS_PolyPassChk::__vtbl = gc_tu.gndChk.v40;
    tapestryApi_register_global_object(ptr(0x101CD118));
  }
  gabi::Local<cXyz> world;
  tapestryApi_PSMTXMultVec(&mWorldMatrix, ptr(position), world.get());
  f32 wz = world->z, wy = world->y, offset = l_groundCheckHeight,
      wx = world->x;
  l_groundCheck.m_pos.z = wz;
  l_groundCheck.m_pos.x = wx;
  l_groundCheck.m_pos.y = (f32)(wy + offset);
  u32 play = tapestryApi_getGameInfo();
  f32 height = tapestryApi_cBgS_GroundCross(ptr(play + 0x12A0), ptr(check));
  height = height + l_groundOffset;
  wy = world->y;
  if (wy < height) {
    world->y = height;
    tapestryApi_PSMTXMultVec(&mInverseMatrix, world.get(), ptr(position));
    mVertexFlags[index] = (u8)mVertexFlags[index] | 2;
  } else
    mVertexFlags[index] =
                    (u8)mVertexFlags[index] & 0xFD;
}

/* 0239B71C */
void daObjTapestryPacket_c::calc_pos() {
  u32 a = gabi::ea(this), buffer = (u32)mPositionBuffer;
  u32 current = a + 0x106C + buffer * 0x6C0,
      previous = a + 0x106C + (buffer ^ 1) * 0x6C0;
  for (s32 row = 0; row < 8; row++)
    for (s32 column = 0; column < 6; column++) {
      u32 index = row * 6 + column;
      if (row == 0 && !((u8)mVertexFlags[index] & 1))
        continue;
      for (u32 i = 0; i < 3; i++)
        gabi::store<f32>(a + 0x2384 + i * 4,
                         gabi::load<f32>(0x101FFBA8 + i * 4));
      tapestryApi_tapestrySpringNeighbors(this, row, column);
      u8 *returned = tapestryApi_daObjTapestryPacket_c_calc_acc_gravity(this);
      tapestryApi_tapestryWaveForce(returned, row, column);
      tapestryApi_tapestryHitForce(this, row, column);
      tapestryApi_tapestryVelocity(this, row, column);
      gabi::Local<cXyz> sum;
      tapestryApi_cXyz___pl(ptr(previous + index * 12), sum.get(),
                 ptr(a + 0x1DEC + index * 12));
      for (u32 i = 0; i < 3; i++)
        gabi::store<u32>(current + index * 12 + i * 4,
                         (u32)(tapestryWords(sum.get())[i]));
      tapestryApi_tapestryPositionCorrection(this, row, column);
    }
}

/* 0239B85C */
void daObjTapestryPacket_c::calc_nrm() {
  u32 a = gabi::ea(this), buffer = (u32)mPositionBuffer;
  f32 weights[6] = {l_tangentWeightA, l_tangentWeightB,
                    l_tangentWeightC, l_tangentWeightD,
                    l_tangentWeightE, l_tangentWeightF};
  f32 reverse = l_negativeUnit;
  u32 current = a + 0x106C + buffer * 0x6C0,
      previous = a + 0x106C + (buffer ^ 1) * 0x6C0;
  for (u32 row = 0; row < 8; row++)
    for (u32 column = 0; column < 6; column++) {
      u32 index = row * 6 + column, center = previous + index * 12;
      gabi::Local<cXyz> vertical, horizontal, temporary, cross, normal;
      if (row == 0) {
        tapestryApi_cXyz___mi(ptr(center + 0x48), temporary.get(),
                   ptr(center));
        tapestryCopyVector(vertical.get(), temporary.get());
      } else if (row == 7) {
        tapestryApi_cXyz___mi(ptr(center), temporary.get(),
                   ptr(center - 0x48));
        tapestryCopyVector(vertical.get(), temporary.get());
      } else
        tapestryInteriorTangent(center - 0x48, center, center + 0x48,
                                vertical.get(), weights);
      if (column == 0) {
        tapestryApi_cXyz___mi(ptr(center + 12), temporary.get(), ptr(center));
        tapestryCopyVector(horizontal.get(), temporary.get());
      } else if (column == 5) {
        tapestryApi_cXyz___mi(ptr(center), temporary.get(), ptr(center - 12));
        tapestryCopyVector(horizontal.get(), temporary.get());
      } else
        tapestryInteriorTangent(center - 12, center, center + 12,
                                horizontal.get(), weights);
      tapestryApi_cXyz_outprod(horizontal.get(), cross.get(), vertical.get());
      tapestryCopyVector(normal.get(), cross.get());
      if (tapestryApi_cXyz_normalizeRS(normal.get())) {
        for (u32 i = 0; i < 3; i++)
          gabi::store<u32>(current + index * 12 + 0x240 + i * 4,
                           (u32)(tapestryWords(normal.get())[i]));
        for (u32 i = 0; i < 3; i++)
          gabi::store<u32>(current + index * 12 + 0x480 + i * 4,
                           (u32)(tapestryWords(normal.get())[i]));
        tapestryApi_PSVECScale(ptr(current + index * 12 + 0x480),
                   ptr(current + index * 12 + 0x480), reverse);
      }
    }
}

/* 02399F9C */
void daObjTapestryPacket_c::calc_wind() {
  u32 a = gabi::ea(this);
  f32 reduction = l_windAttenuation;
  tapestryApi_PSVECScale(&mWorldWind, &mWorldWind, reduction);
  f32 square = tapestryApi_PSVECSquareMag(&mWorldWind),
      epsilon = l_particleYSpeed;
  u8 valid = (u8)mInverseValid;
  f32 zero = l_zero;
  if (square < epsilon) {
    mWorldWind.y = zero;
    mWorldWind.x = zero;
    mWorldWind.z = zero;
  }
  if (valid) {
    tapestryApi_PSMTXMultVecSR(&mInverseMatrix, &mWorldWind, &mWind);
    f32 z = (f32)mWind.z, x = (f32)mWind.x;
    if (z < zero) {
      z = z * l_negativeHalf;
      mWind.z = z;
    }
    f32 multiplier = reduction;
    if (std::fabs(x) > reduction && std::fabs(z) < l_oneFifth)
      multiplier = l_threeTenths;
    mWind.x = (f32)(x * multiplier);
    tapestryApi_PSVECScale(&mWind, &mWind,
               (f32)l_HIO.mWindSpeed);
  } else
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(a + 0x2430 + i * 4, gabi::load<f32>(0x101FFBA8 + i * 4));
  s16 phase = (s16)((s16)mWindPhase + 500);
  mWindPhase = phase;
  gabi::Local<cXyz> scaled;
  if (phase > 0) {
    f32 factor = (f32)l_HIO.mFlutterSpeed,
        sine = gabi::load<f32>(0x104A44F8 + ((u16)phase >> 3) * 8);
    tapestryApi_cXyz___ml(ptr(0x101FFBCC), scaled.get(), (f32)(sine * factor));
    for (u32 i = 0; i < 3; i++)
      gabi::store<u32>(a + 0x2440 + i * 4,
                       (u32)(tapestryWords(scaled.get())[i]));
  } else
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(a + 0x2440 + i * 4, gabi::load<f32>(0x101FFBA8 + i * 4));
  if (!(u8)mDetached)
    return;
  if (!(u32)(l_windInitialized.mFlags[0])) {
    f32 y = gabi::load<f32>(0x104A7F90), z = gabi::load<f32>(0x104A7F94);
    l_windInitialized.mFlags[0] = 1;
    l_initialWinds.mDirections[0].z = z;
    l_initialWinds.mDirections[0].x = zero;
    l_initialWinds.mDirections[0].y = y;
  }
  if (!(u32)(l_windInitialized.mFlags[1])) {
    f32 y = gabi::load<f32>(0x104A6FF0), z = gabi::load<f32>(0x104A6FF4);
    l_initialWinds.mDirections[1].z = z;
    l_initialWinds.mDirections[1].x = zero;
    l_windInitialized.mFlags[1] = 1;
    l_initialWinds.mDirections[1].y = y;
  }
  if (!(u32)(l_windInitialized.mFlags[2])) {
    f32 z = gabi::load<f32>(0x104A5A74), y = gabi::load<f32>(0x104A5A70);
    l_windInitialized.mFlags[2] = 1;
    l_initialWinds.mDirections[2].z = z;
    l_initialWinds.mDirections[2].x = zero;
    l_initialWinds.mDirections[2].y = y;
  }
  f32 one = l_unitScale;
  s16 middle = (s16)((s16)mFlutterAngles[1] + 1133);
  mFlutterAngles[1] = middle;
  s16 top = (s16)((s16)mFlutterAngles[0] + 1366);
  mFlutterAngles[0] = top;
  s16 bottom = (s16)((s16)mFlutterAngles[2] + 811);
  mFlutterAngles[2] = bottom;
  for (u32 i = 0; i < 3; i++) {
    u16 angle = gabi::load<u16>(a + 0x244C + i * 2);
    f32 sine = gabi::load<f32>(0x104A44F8 + (angle >> 3) * 8);
    f32 factor = (f32)(reinterpret_cast<be<f32>*>(&l_HIO.mTopWindScale)[i]);
    tapestryApi_cXyz___ml(&l_initialWinds.mDirections[i], scaled.get(),
               (f32)((f32)(sine + one) * factor));
    for (u32 j = 0; j < 3; j++)
      gabi::store<u32>(a + 0x2454 + i * 12 + j * 4,
                       (u32)(tapestryWords(scaled.get())[j]));
  }
}

/* 0239A38C */
void daObjTapestryPacket_c::calc_hit() {
  u32 a = gabi::ea(this);
  f32 half = l_half, epsilon = l_epsilon;
  f32 strength = (f32)mHitForce * half;
  mHitForce = strength;
  if (strength > epsilon) {
    tapestryApi_PSMTXMultVec(&mInverseMatrix, &mHitPosition, &mLocalHitPosition);
    tapestryApi_PSMTXMultVecSR(&mInverseMatrix, &mHitDirection, &mLocalHitDirection);
    u32 previous = a + 0x106C + ((u32)mPositionBuffer ^ 1) * 0x6C0;
    gabi::Local<cXyz> crossing;
    f32 left = tapestryApi_cM3d_lineVsPosSuisenCross(ptr(previous), ptr(previous + 0x1F8),
                               &mLocalHitPosition, crossing.get());
    f32 right =
        tapestryApi_cM3d_lineVsPosSuisenCross(ptr(previous + 0x3C), ptr(previous + 0x234),
                        &mLocalHitPosition, crossing.get());
    f32 top = tapestryApi_cM3d_lineVsPosSuisenCross(ptr(previous), ptr(previous + 0x3C),
                              &mLocalHitPosition, crossing.get());
    f32 bottom =
        tapestryApi_cM3d_lineVsPosSuisenCross(ptr(previous + 0x1F8),
                        ptr(previous + 0x234), &mLocalHitPosition, crossing.get());
    f32 column = (f32)(top + bottom) * half, row = (f32)(left + right) * half;
    mHitColumn = column;
    f32 zero = l_zero, one = l_unitScale;
    mHitRow = row;
    if (row < zero)
      row = zero;
    else
      row = (f32)(row - one) >= 0 ? one : row;
    column = (f32)mHitColumn;
    if (column < zero)
      column = zero;
    else
      column = (f32)(column - one) >= 0 ? one : column;
    mHitRow = row;
    f32 radius = (f32)mHitRadius;
    mHitColumn = column;
    mHitAttenuation = radius > one ? (f32)(one / radius) : one;
  }
}

/* 0239A90C */
void daObjTapestryPacket_c::calc_fire_leap(s32 row, s32 column) {
  u32 index = (u32)row * 6 + (u32)column;
  if ((u8)mFireTimers[index])
    return;
  bool special =
      row == 0 && ((column == 0 && (u8)mSmokeType == 1) ||
                   (column == 5 && (u8)mSmokeType == 2));
  u8 delay;
  if (special)
    delay = 1;
  else {
    f32 random = tapestryApi_cM_rndF(l_rowCount);
    delay = gabi::ftoi((f32)(random + l_unitScale));
  }
  mFireTimers[index] = delay;
  u32 result = tapestryApi_tapestryStartFire(this, row, column);
  mFireIndices[index] = result;
}

/* 0239A9EC */
void daObjTapestryPacket_c::calc_fire() {
  if ((u8)mHitFire) {
    f32 r = (f32)mHitRow, rows = l_lastRow,
        columns = l_lastColumn;
    f32 rv = r * rows, cv = (f32)mHitColumn * columns;
    u32 index = (u32)gabi::ftoi(rv) * 6 + (u32)gabi::ftoi(cv);
    mFireTimers[index] = (u8)mFireTimers[index] + 1;
    mHitFire = 0;
  }
  f32 edgeChance = l_smokeRightChance,
      normalChance = l_axialFireChance;
  for (s32 row = 0; row < 8; row++)
    for (s32 column = 0; column < 6; column++) {
      u32 index = row * 6 + column;
      u8 timer = (u8)mFireTimers[index];
      if (!timer || timer >= (u8)l_HIO.mBurnEnd)
        continue;
      bool special =
          row == 0 && ((column == 0 && (u8)mSmokeType == 1) ||
                       (column == 5 && (u8)mSmokeType == 2));
      if (tapestryApi_cM_rnd() < (special ? edgeChance : normalChance))
        mFireTimers[index] =
                        (u8)mFireTimers[index] + 1;
      timer = (u8)mFireTimers[index];
      if (timer < (u8)l_HIO.mBurnStart ||
          ((u8)mVertexFlags[index] & 1))
        continue;
      mVertexFlags[index] =
                      (u8)mVertexFlags[index] | 1;
      if (row - 1 >= 0) {
        tapestryApi_tapestryFireLeap(this, row - 1, column);
        if (column - 1 >= 0)
          tapestryApi_tapestryFireLeap(this, row - 1, column - 1);
        if (column + 1 < 6)
          tapestryApi_tapestryFireLeap(this, row - 1, column + 1);
      }
      if (row + 1 < 8) {
        tapestryApi_tapestryFireLeap(this, row + 1, column);
        if (column - 1 >= 0)
          tapestryApi_tapestryFireLeap(this, row + 1, column - 1);
        if (column + 1 < 6)
          tapestryApi_tapestryFireLeap(this, row + 1, column + 1);
      }
      if (column - 1 >= 0)
        tapestryApi_tapestryFireLeap(this, row, column - 1);
      if (column + 1 < 6)
        tapestryApi_tapestryFireLeap(this, row, column + 1);
    }
  if (!(u8)mDetached) {
    bool burned = true;
    for (u32 column = 0; column < 6; column++)
      if (!((u8)mVertexFlags[column] & 1)) {
        burned = false;
        break;
      }
    if (burned)
      mDetached = 1;
  }
}

/* 0239C574 */
void daObjTapestryPacket_c::calc(daObjTapestry_c *actor) {
  u32 model = gabi::ea(reinterpret_cast<u8*>(actor->mpModel.get()));
  tapestryApi_PSMTXCopy(ptr(model ? model + 0xC8 : 0), ptr(0x1048D0CC));
  tapestryApi_PSMTXCopy(ptr(0x1048D0CC), &mWorldMatrix);
  u32 result = tapestryApi_PSMTXInverse(&mWorldMatrix, &mInverseMatrix);
  u32 index = (u32)mPositionBuffer;
  mInverseValid = result != 0;
  mPositionBuffer = index ^ 1;
  tapestryApi_tapestryWind(this);
  tapestryApi_tapestryHitCoordinates(this);
  tapestryApi_tapestryFireSimulation(this);
  tapestryApi_tapestryPositions(this);
  tapestryApi_tapestryNormals(this);
  tapestryApi_tapestryParticlePositions(this);
  tapestryApi_tapestrySmokeMove(this, actor);
  gabi::Local<cXyz> p;
  gabi::Local<be<u16>[3]> r;
  u16 z = (u16)mSmokeCallback.mAngle.z, x = (u16)mSmokeCallback.mAngle.x;
  f32 py = (f32)mSmokeCallback.mPosition.y;
  u16 y = (u16)mSmokeCallback.mAngle.y;
  p->y = py;
  p->x = (f32)mSmokeCallback.mPosition.x;
  (*r)[1] = y;
  (*r)[2] = z;
  f32 pz = (f32)mSmokeCallback.mPosition.z;
  (*r)[0] = x;
  p->z = pz;
  tapestryApi_tapestryLightMove(&mLight, p.get(), r.get());
}

/* 0239E3B4 */
void daObjTapestryPacket_c::set_hit(cXyz *position, cXyz *direction, s32 smoke, f32 force, f32 radius) {
  u32 a = gabi::ea(this);
  for (u32 i = 0; i < 3; i++)
    gabi::store<f32>(a + 0x2478 + i * 4,
                     gabi::load<f32>(gabi::ea(position) + i * 4));
  f32 x = direction->x;
  mHitDirection.x = x;
  f32 y = direction->y;
  mHitDirection.y = y;
  f32 z = direction->z;
  mHitRadius = radius;
  mHitDirection.z = z;
  mHitFire = smoke;
  mHitForce = force;
  if (smoke) {
    f32 random = tapestryApi_cM_rnd();
    f32 first = l_smokeRightChance;
    mGroundCollision = 1;
    u8 type;
    if (random < first)
      type = 1;
    else
      type = random < l_axialFireChance ? 2 : 0;
    mSmokeType = type;
    mSmokeCallback.mTimer = 30;
  }
}

/* 0239A7A0 */
cXyz *daObjTapestryPacket_c::get_now_pos(s32 row, s32 column) {
  return gabi::at<cXyz>(gabi::ea(&mCloth[(u32)mPositionBuffer].mPositions[(u32)row * 6 + (u32)column]));
}

/* 0239A7C4 */
u8 daObjTapestryPacket_c::eff_start(s32 row, s32 column) {
  u32 a = gabi::ea(this);
  u8 result = 255;
  if ((s32)mFireCount < 16 &&
      tapestryApi_tapestryCanStartFire(this, row, column)) {
    u32 index = (u32)mFireCount,
        callback = a + 0x20C0 + index * 0x2C;
    cXyz *position = tapestryApi_daObjTapestryPacket_c_get_now_posVariantB(this, row, column);
    gabi::Local<cXyz> transformed, size;
    tapestryApi_PSMTXMultVec(&mWorldMatrix, position, transformed.get());
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(callback + 0x14 + i * 4,
                       gabi::load<f32>(gabi::ea(transformed.get()) + i * 4));
    f32 minimum = (f32)l_HIO.mFireScaleMin,
        maximum = (f32)l_HIO.mFireScaleMax;
    f32 random = tapestryApi_cM_rndF((f32)(maximum - minimum));
    f32 scale = minimum + random;
    size->x = scale;
    size->z = scale;
    size->y = scale;
    u32 play = tapestryApi_getGameInfo();
    tapestryApi_dPa_control_c_setVariantC(ptr(gabi::load<u32>(play + 0x5AB0)), 0, 0x1EA,
               ptr(callback + 0x14), 0, size.get(), 255, ptr(callback), -1, 0,
               0, 0);
    index = (u32)mFireCount;
    mFireCount = index + 1;
    result = index;
  }
  return result;
}

/* 0239A550 */
BOOL daObjTapestryPacket_c::eff_start_chk(s32 row, s32 column) {
  f32 diagonal = l_diagonalFireChance,
      axial = l_axialFireChance;
  BOOL allowed = 1;
  auto occupied = [&](s32 r, s32 c) {
    return (u8)mFireIndices[(u32)r*6+(u32)c] != 255;
  };
  for (s32 neighbor : {row - 1, row + 1}) {
    if (neighbor == row - 1 ? neighbor < 0 : neighbor >= 8)
      continue;
    if (tapestryApi_cM_rnd() < axial && occupied(neighbor, column))
      allowed = 0;
    if (tapestryApi_cM_rnd() < diagonal && column - 1 >= 0 &&
        occupied(neighbor, column - 1))
      allowed = 0;
    if (tapestryApi_cM_rnd() < diagonal && column + 1 < 6 &&
        occupied(neighbor, column + 1))
      allowed = 0;
  }
  if (tapestryApi_cM_rnd() < axial && column - 1 >= 0 &&
      occupied(row, column - 1))
    allowed = 0;
  if (tapestryApi_cM_rnd() < axial && column + 1 < 6 &&
      occupied(row, column + 1))
    allowed = 0;
  if (occupied(row, column))
    allowed = 0;
  if (row == 0)
    allowed = 0;
  return allowed;
}

/* 0239E334 */
void daObjTapestryPacket_c::eff_end() {
  s32 i = 0;
  u32 a = gabi::ea(this);
  while (i < (s32)mFireCount) {
    tapestryApi_dPa_followEcallBack_end(ptr(a + 0x20C0 + (u32)i * 0x2C));
    i++;
  }
  mFireCount = 0;
  tapestryApi_dPa_followEcallBack_end(&mSmokeCallback);
  gabi::at<daObjTapestryPLight_c>(a + 0x24F0)->plight_delete();
}

/* 0239BF60 */
void daObjTapestryPacket_c::eff_pos() {
  u32 a = gabi::ea(this);
  if ((s32)mFireCount <= 0)
    return;
  u32 buffer = (u32)mPositionBuffer,
      previous = a + 0x106C + (buffer ^ 1) * 0x6C0;
  gabi::Local<cXyz[144]> old, current;
  for (u32 group = 0; group < 3; group++)
    tapestryApi_constructArrayCopy(&(*old)[group * 48], 48, 12, gabi::guest_fn(reinterpret_cast<const void*>(&tapestryPositionCopy)),
               ptr(previous + group * 0x240));
  u32 now = a + 0x106C + (u32)mPositionBuffer * 0x6C0;
  for (u32 group = 0; group < 3; group++)
    tapestryApi_constructArrayCopy(&(*current)[group * 48], 48, 12, gabi::guest_fn(reinterpret_cast<const void*>(&tapestryPositionCopy)),
               ptr(now + group * 0x240));
  for (u32 index = 0; index < 48; index++) {
    u8 id = (u8)mFireIndices[index];
    if (id == 255)
      continue;
    gabi::Local<cXyz> world, difference, copied, velocity;
    tapestryApi_PSMTXMultVec(&mWorldMatrix, &(*current)[index], world.get());
    u32 callback = a + 0x20C0 + (u32)id * 0x2C;
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(callback + 0x14 + i * 4,
                       gabi::load<f32>(gabi::ea(world.get()) + i * 4));
    tapestryApi_cXyz___mi(&(*current)[index], difference.get(),
               &(*old)[index]);
    f32 z = difference->z, y = difference->y;
    copied->z = z;
    f32 x = difference->x;
    copied->y = y;
    copied->x = x;
    tapestryApi_PSMTXMultVecSR(&mWorldMatrix, copied.get(), velocity.get());
    for (u32 i = 0; i < 3; i++)
      gabi::store<f32>(callback + 0x20 + i * 4,
                       gabi::load<f32>(gabi::ea(velocity.get()) + i * 4));
  }
}

/* 0239D92C */
void daObjTapestryPacket_c::eff_delete() {
  u32 a = gabi::ea(this);
  for (u32 i = 0; i < 16; i++) {
    u32 cb = a + 0x20C0 + i * 0x2C,
        target = gabi::load<u32>(gabi::load<u32>(cb) + 0x44);
    gabi::call(target, ptr(cb));
  }
  u32 cb = a + 0x24C4, target = gabi::load<u32>(gabi::load<u32>(cb) + 0x44);
  gabi::call(target, ptr(cb));
  gabi::at<daObjTapestryPLight_c>(a + 0x24F0)->plight_delete();
}

/* 02399F90 */
void daObjTapestryPacket_c::smokeCallback_init() {
  mSmokeCallback.m11 = 0;
}

/* 0239C158 */
void daObjTapestryPacket_c::smoke_set() {
  u32 a = gabi::ea(this);
  if (gabi::load<u32>(0x1046C224) == 0) {
    f32 one = l_unitScale;
    gabi::store<u32>(0x1046C224, 1);
    l_smokeParticleScale.x = one;
    l_smokeParticleScale.z = one;
    l_smokeParticleScale.y = one;
  }
  tapestryApi_dPa_followEcallBack_end(&mSmokeCallback);
  u32 play = tapestryApi_getGameInfo();
  tapestryApi_dPa_control_c_setVariantB(ptr(gabi::load<u32>(play + 0x5AB0)), 2, 0xA329,
             ptr(a + 0x24D8), ptr(a + 0x24E4), &l_smokeParticleScale, 255,
             &mSmokeCallback, -1, 0, 0, 0);
  tapestryApi_daObjTapestryPLight_c_plight_make(&mLight);
}

/* 0239C210 */
void daObjTapestryPacket_c::smoke_move(daObjTapestry_c *actor) {
  u32 a = gabi::ea(this);
  s32 timer = (s32)mSmokeCallback.mTimer;
  if (timer > 0) {
    timer--;
    mSmokeCallback.mTimer = timer;
    if (timer <= 0)
      tapestryApi_tapestrySmokeSet(this);
  }
  f32 rowFraction = (f32)mHitRow;
  f32 rows = l_lastRow, columns = l_lastColumn;
  f32 rowValue = rowFraction * rows,
      columnValue = (f32)mHitColumn * columns;
  u32 buffer = (u32)mPositionBuffer, row = (u32)gabi::ftoi(rowValue),
      column = (u32)gabi::ftoi(columnValue);
  u32 normal = a + 0x106C + buffer * 0x6C0 + (row * 6 + column) * 12 + 0x240;
  f32 nx = (f32)(gabi::at<cXyz>(normal)->x), nz = (f32)(gabi::at<cXyz>(normal)->z);
  s32 yaw = tapestryApi_cM_atan2s(nx, -nz);
  nz = (f32)(gabi::at<cXyz>(normal)->z);
  f32 square = nz * nz;
  nx = (f32)(gabi::at<cXyz>(normal)->x);
  f32 length = tapestryApi_std_sqrtf(gabi::fmadds(nx, nx, square));
  s32 pitch = tapestryApi_cM_atan2s((f32)(gabi::at<cXyz>(normal)->y), length);
  u32 owner = gabi::ea(actor);
  mSmokeCallback.mPosition.x = gabi::load<f32>(owner + 0x37C);
  mSmokeCallback.mPosition.y = gabi::load<f32>(owner + 0x380);
  f32 z = gabi::load<f32>(owner + 0x384);
  mSmokeCallback.mAngle.x = pitch;
  mSmokeCallback.mAngle.y = yaw;
  mSmokeCallback.mPosition.z = z;
  mSmokeCallback.mAngle.z = 0;
}

/* 0239DD2C */
void daObjTapestryPacket_c::setup_vtx() {
  u32 a = gabi::ea(this), owner = gabi::ea(mpOwner.get());
  gabi::store<u32>(a + 0x1068, owner + 0x110);
  for (u32 face = 0; face < 2; face++) {
    u32 index = face * 2 + (u32)mVertexBuffer;
    u32 destination = gabi::ea(reinterpret_cast<TapestryVertexBuffer*>(mVertexBuffers)[index].mpData.get());
    for (u32 vertex = 0; vertex < 48; vertex++) {
      u32 source =
          a + 0x106C + (u32)mPositionBuffer * 0x6C0 + vertex * 12;
      f32 z = (f32)(gabi::at<cXyz>(source)->z), x = (f32)(gabi::at<cXyz>(source)->x),
          y = (f32)(gabi::at<cXyz>(source)->y);
      gabi::at<TapestryRenderVertex>(destination)[vertex].mPosition.x = x;
      gabi::at<TapestryRenderVertex>(destination)[vertex].mPosition.y = y;
      gabi::at<TapestryRenderVertex>(destination)[vertex].mPosition.z = z;
      source = a + 0x106C + (u32)mPositionBuffer * 0x6C0 + vertex * 12 +
               (face ? 0x480 : 0x240);
      x = (f32)(gabi::at<cXyz>(source)->x);
      y = (f32)(gabi::at<cXyz>(source)->y);
      z = (f32)(gabi::at<cXyz>(source)->z);
      gabi::at<TapestryRenderVertex>(destination)[vertex].mNormal.x = x;
      gabi::at<TapestryRenderVertex>(destination)[vertex].mNormal.y = y;
      gabi::at<TapestryRenderVertex>(destination)[vertex].mNormal.z = z;
    }
  }
  u32 first = a + 0xA8 + (u32)mVertexBuffer * 0x254;
  for (u32 i = 0; i < 2; i++)
    tapestryApi_flushBuffer(ptr(first + i * 0x4A8), 0,
               gabi::load<u32>(first + i * 0x4A8 + 0x14C));
  mVertexBuffer = (u32)mVertexBuffer == 0;
  tapestryApi_prepareTevColors(ptr(gabi::ea(mpTevStr.get())));
  gabi::Local<be<f32>[12]> view, world;
  gabi::Local<be<f32>[4]> color, adjusted;
  tapestryApi_tapestryMatrixCopy(view.get(), ptr(0x104B45F8));
  u32 camera = gabi::load<u32>(0x104B4708);
  tapestryApi_setupShader(&mLightingMaterial, 0, view.get(), ptr(0x104B470C),
             ptr(camera + 0x240));
  for (u32 i = 0; i < 2; i++) {
    u32 settings = gabi::ea(mpTevStr.get()),
        material = gabi::load<u32>(a + 0xA28);
    tapestryApi_tapestrySignedColor(color.get(), ptr(settings + (i ? 0x160 : 0x90)));
    settings = gabi::ea(mpTevStr.get());
    tapestryApi_scaleColor(adjusted.get(), color.get(),
               gabi::load<f32>(settings + (i ? 0x16C : 0x28)));
    for (u32 j = 0; j < 4; j++)
      gabi::store<u32>(material + 0x1C4 + i * 0x10 + j * 4,
                       (u32)(tapestryWords(adjusted.get())[j]));
  }
  tapestryApi_selectUniform(&mLightingMaterial, 0);
  u32 settings = gabi::ea(mpTevStr.get()), diffuse = settings + 0x98,
      ambient = settings + 0x9C;
  tapestryApi_tapestrySignedColor(&mLightingUniforms.mColors[4], ptr(settings + 0x90));
  tapestryApi_tapestryByteColor(&mLightingUniforms.mColors[5], ptr(diffuse));
  settings = gabi::ea(mpTevStr.get());
  tapestryApi_scaleColorRGB(&mLightingUniforms.mColors[5], gabi::load<f32>(settings + 0x24));
  tapestryApi_tapestryByteColor(&mLightingUniforms.mColors[6], ptr(ambient));
  mLightingUniforms.mColors[10][3] = gabi::load<f32>(owner + 0x710);
  if ((u8)l_HIO.mUseDiffuse == 1)
    tapestryApi_tapestryByteColor(&mLightingUniforms.mColors[7], ptr(diffuse));
  tapestryApi_prepareUniform(&mPixelUniform);
  tapestryApi_tapestryMatrixCopy(world.get(), &mWorldMatrix);
  tapestryApi_PSMTXCopy(world.get(), &mVertexUniform.mMatrix);
  tapestryApi_prepareUniform(&mVertexUniform);
}

/* 0239C678 */
void daObjTapestryPacket_c::initializeRenderResources() {
  u32 a = gabi::ea(this);
  gabi::Local<be<u32>[2]> materialName;
  (*materialName)[0] = 0x100308F4;
  (*materialName)[1] = 0x10030654;
  u32 model = tapestryApi_getPacketModel(this, ptr(0x100308F4));
  s32 found = tapestryApi_findMaterialName(ptr(gabi::load<u32>(model + 4)),
                              materialName.get());
  u32 material = 0;
  if (found >= 0) {
    u32 count = gabi::load<u32>(model + 8), base = gabi::load<u32>(model + 0xC);
    u32 selected = base + ((u32)found < count ? (u32)found * 0x24 : 0);
    if (!gabi::load<u8>(selected + 0x20)) {
      u32 data = gabi::load<u32>(model + 4),
          limit = gabi::load<u32>(data + 0x1C), definition = 0;
      if ((u32)found < limit)
        definition = gabi::load<u32>(data + 0x20) + (u32)found * 0x84;
      tapestryApi_initializeMaterial(ptr(selected), ptr(definition), 0);
      count = gabi::load<u32>(model + 8);
      base = gabi::load<u32>(model + 0xC);
    }
    material = base + ((u32)found < count ? (u32)found * 0x24 : 0);
  }
  tapestryApi_initializeMaterialGroups(ptr(a + 0x98), ptr(material), 0);
  mVertexFormat = 0x1013;
  gabi::store<u32>(a + 0xA00, 0x10030988);
  for (u32 buffer = 0; buffer < 2; buffer++)
    for (u32 face = 0; face < 2; face++) {
      u32 vertex = a + 0xA4 + buffer * 0x254 + face * 0x4A8,
          data = gabi::load<u32>(vertex);
      if (!data) {
        u32 allocated = tapestryAllocate(0x900, 64);
        if (allocated) {
          gabi::store<u32>(vertex + 0x250, allocated);
          gabi::store<u32>(vertex + 0x24C, 48);
        }
        data = gabi::load<u32>(vertex + 0x250);
        gabi::store<u32>(vertex, data);
      }
      tapestryApi_initializeVertexBuffer(ptr(vertex + 4), ptr(data), 48, ptr(a + 0x9F8));
    }
  mRenderMode = 0;
  mRenderEnabled = 1;
  for (u32 groupIndex = 0; groupIndex < (u32)mMaterialCount;
       groupIndex++) {
    u32 groups = gabi::ea(mpMaterialGroups.get()), count = (u32)mMaterialCapacity;
    u32 group = groups + (groupIndex < count ? groupIndex * 0x14 : 0),
        definition = gabi::load<u32>(group);
    gabi::store<u32>(group, 0);
    tapestryFreeVectorArray(group, 4, 8);
    tapestryFreeVectorArray(group, 0xC, 0x10);
    gabi::store<u32>(group, definition);
    for (u32 side = 0; side < 2; side++) {
      u32 allocated = tapestryAllocate(0x1E8, 4);
      for (u32 i = 0; i < 2; i++)
        if (allocated + i * 0xF4)
          tapestryApi_constructVertexAttribute(ptr(allocated + i * 0xF4));
      if (allocated) {
        gabi::store<u32>(group + 4 + side * 8, 2);
        gabi::store<u32>(group + 8 + side * 8, allocated);
      }
    }
    for (u32 side = 0; side < 2; side++)
      for (u32 face = 0; face < 2; face++) {
        u32 arrayCount = gabi::load<u32>(group + 4 + side * 8),
            data = gabi::load<u32>(group + 8 + side * 8);
        if (face < arrayCount)
          data += face * 0xF4;
        tapestryApi_initializeVertexAttribute(ptr(definition), ptr(data),
                   ptr(a + 0xA8 + side * 0x254 + face * 0x4A8), ptr(a + 0x9F8),
                   0);
      }
  }
  tapestryApi_initializeLightingMaterial(&mLightingMaterial, 1, 0);
  tapestryApi_initializeIndexBuffer(&mIndexBuffer, &l_drawData, 4, 0x54);
  f32 divisor = l_colorRange;
  gabi::store<u32>(a + 0xA10, 6);
  for (u32 face = 0; face < 2; face++) {
    u32 index = face * 2 + (u32)mVertexBuffer,
        data = gabi::ea(reinterpret_cast<TapestryVertexBuffer*>(mVertexBuffers)[index].mpData.get()), end = data + 0x900;
    for (u32 block = data; block < end; block += 0x20)
      for (u32 word = 0; word < 8; word++)
        gabi::store<u32>((block & ~31u) + word * 4, 0);
    index = face * 2 + (u32)mVertexBuffer;
    data = gabi::ea(reinterpret_cast<TapestryVertexBuffer*>(mVertexBuffers)[index].mpData.get());
    for (u32 vertex = 0; vertex < 48; vertex++) {
      u32 source =
          a + 0x106C + (u32)mPositionBuffer * 0x6C0 + vertex * 12;
      f32 position[3];
      for (u32 component = 0; component < 3; component++)
        position[component] = (f32)(tapestryComponents(gabi::at<cXyz>(source))[component]);
      for (u32 component = 0; component < 3; component++)
        gabi::store<f32>(data + vertex * 0x30 + component * 4,
                         position[component]);
      source = a + 0x106C + (u32)mPositionBuffer * 0x6C0 + vertex * 12 +
               (face ? 0x480 : 0x240);
      f32 normal[3];
      normal[0] = (f32)(gabi::at<cXyz>(source)->x);
      normal[face ? 2 : 1] = (f32)((face ? gabi::at<cXyz>(source)->z : gabi::at<cXyz>(source)->y));
      normal[face ? 1 : 2] = (f32)((face ? gabi::at<cXyz>(source)->y : gabi::at<cXyz>(source)->z));
      gabi::at<TapestryRenderVertex>(data)[vertex].mNormal.y = normal[1];
      gabi::at<TapestryRenderVertex>(data)[vertex].mNormal.x = normal[0];
      gabi::at<TapestryRenderVertex>(data)[vertex].mNormal.z = normal[2];
      for (u32 channel = 0; channel < 4; channel++)
        gabi::at<TapestryRenderVertex>(data)[vertex].mColor[channel] =
                         (f32)gabi::load<u8>(0x101CD194 + channel) / divisor;
      f32 u = gabi::load<f32>(0x1046C2D4 + vertex * 8),
          v = gabi::load<f32>(0x1046C2D8 + vertex * 8);
      gabi::at<TapestryRenderVertex>(data)[vertex].mTexCoords[0] = u;
      gabi::at<TapestryRenderVertex>(data)[vertex].mTexCoords[1] = v;
    }
  }
  u32 index = (u32)mVertexBuffer, destinationIndex = index == 0;
  for (u32 face = 0; face < 2; face++) {
    u32 destination =
        gabi::ea(reinterpret_cast<TapestryVertexBuffer*>(mVertexBuffers)[(destinationIndex + face * 2)].mpData.get());
    u32 source = gabi::ea(reinterpret_cast<TapestryVertexBuffer*>(mVertexBuffers)[(index + face * 2)].mpData.get());
    for (u32 vertex = 0; vertex < 48; vertex++) {
      source = gabi::ea(reinterpret_cast<TapestryVertexBuffer*>(mVertexBuffers)[(index + face * 2)].mpData.get());
      destination =
          gabi::ea(reinterpret_cast<TapestryVertexBuffer*>(mVertexBuffers)[(destinationIndex + face * 2)].mpData.get());
      for (u32 word = 0; word < 12; word++)
        reinterpret_cast<be<f32>*>(gabi::at<TapestryRenderVertex>(destination)+vertex)[word] =
                         (f32)(reinterpret_cast<be<f32>*>(gabi::at<TapestryRenderVertex>(source)+vertex)[word]);
    }
    index = (u32)mVertexBuffer;
  }
  u32 first = a + 0xA8 + index * 0x254;
  for (u32 face = 0; face < 2; face++)
    tapestryApi_flushBuffer(ptr(first + face * 0x4A8), 0,
               gabi::load<u32>(first + face * 0x4A8 + 0x14C));
  mVertexBuffer = (u32)mVertexBuffer == 0;
  tapestryApi_beginTextureAccess(ptr(gabi::load<u32>(0x101F8B18)));
  gabi::Local<be<u32>[2]> archive, resource;
  (*archive)[0] = 0x10030900;
  (*archive)[1] = 0x10030654;
  (*resource)[0] = 0x10030910;
  (*resource)[1] = 0x10030654;
  u8 *texture = tapestryApi_loadArchiveResource(ptr(gabi::load<u32>(0x101F4F7C)),
                                 archive.get(), resource.get(), 0);
  tapestryApi_initializeTextureResource(&mTextureResource, texture, ptr(0x100308E8));
  tapestryApi_endTextureAccess(ptr(gabi::load<u32>(0x101F8B18)));
  bool same = true;
  for (u32 offset : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u})
    if (gabi::load<u32>(a + 0xED0 + offset) !=
        gabi::load<u32>(a + 0xE40 + offset)) {
      same = false;
      break;
    }
  if (!same)
    tapestryApi_copyTextureSampler(&mTextureSampler, &mTextureResource);
  u8 flags = (u8)mTextureFlags;
  gabi::store<u32>(a + 0x102C, 0);
  gabi::store<u32>(a + 0x1030, 0);
  gabi::store<u32>(a + 0x1034, 0);
  if (same) {
    u32 first = gabi::load<u32>(a + 0xE68), second = gabi::load<u32>(a + 0xE70);
    gabi::store<u32>(a + 0xFA4, first);
    gabi::store<u32>(a + 0xFAC, second);
    gabi::store<u32>(a + 0xEF8, first);
    gabi::store<u32>(a + 0xF00, second);
  }
  mTextureFlags = flags | 2;
}

/* 0239E474 */
void daObjTapestryPacket_c::drawSurface(u8 *surface) {
  u32 a = gabi::ea(this), s = gabi::ea(surface),
      index = gabi::load<u32>(s + 0xC), material = 0;
  if ((s32)index < 4) {
    u32 groups = gabi::ea(mpMaterialGroups.get());
    if (index < (u32)mMaterialCapacity)
      groups += index * 0x14;
    material = gabi::load<u32>(groups);
  }
  u32 state = tapestryApi_shaderCache(ptr(0x104B45C0)),
      shape = gabi::load<u32>(material), active = gabi::load<u32>(state + 4);
  if (shape != active) {
    u8 flags = gabi::load<u8>(shape);
    u32 current = gabi::load<u32>(state);
    if (flags & 2) {
      gabi::store<u8>(shape, flags & ~2);
      tapestryApi_updateMaterial(ptr(shape), 0);
    }
    u32 table = gabi::load<u32>(shape + 0x7C),
        next = gabi::load<u32>(table + 0x28);
    if (current != next)
      tapestryApi_setShader(ptr(next));
    u32 display = gabi::load<u32>(shape + 0xC);
    if (display)
      tapestryApi_setMaterialData(ptr(gabi::load<u32>(shape + 4)), display);
    else
      tapestryApi_applyMaterial(ptr(shape));
    gabi::store<u32>(state, next);
    gabi::store<u32>(state + 4, shape);
  }
  index = gabi::load<u32>(s + 0xC);
  if (index == 0) {
    tapestryApplyMaterial(a, material);
    u32 source = gabi::load<u32>(s + 0x14);
    if (source) {
      u32 data = gabi::load<u32>(source + 4),
          table = data + 0x10 + gabi::load<u32>(data + 0x4C) * 0x1C;
      tapestryMaterialBindings(material, table);
    }
  } else if (index == 1 || index == 2) {
    u32 data = gabi::load<u32>(a + 0xA28),
        table = data + 0x10 + gabi::load<u32>(data + 0x4C) * 0x1C;
    tapestryMaterialBindings(material, table);
    tapestryApplyMaterial(a, material);
    if (index == 2) {
      u32 extra = gabi::load<u32>(s + 0x30);
      if (extra)
        gabi::call(gabi::load<u32>(gabi::load<u32>(extra + 0xC) + 0x2C),
                   ptr(extra), ptr(material));
    }
    tapestryBindTexture(a, material);
    if (index == 2)
      tapestryApi_applySurfaceShader(surface, ptr(material));
  }
  gabi::Local<u8[0x11C]> drawState; /* HD sizeof (ctor 02750250 allocates 0x11C when this == NULL, vtable at +0x118); was 240 */
  tapestryApi_constructRenderState(drawState.get());
  u32 local = gabi::ea(drawState.get());
  gabi::store<u32>(local + 8, 0);
  index = gabi::load<u32>(s + 0xC);
  u32 flags = gabi::load<u32>(local + 0xEC);
  gabi::store<u32>(local + 0xC, 2);
  gabi::store<f32>(local + 0xE8, l_half);
  gabi::store<u8>(local + 0xE0, 1);
  gabi::store<u32>(local + 0xE4, 4);
  gabi::store<u32>(local + 0xEC,
                   (((flags & 0xFFFFFFF0) + 7) & 0xFFFFFF0F) + 0x10);
  tapestryApi_configureSurfaceRenderState(index, drawState.get());
  tapestryApi_restoreRenderState(drawState.get());
  auto group = [&]() {
    u32 id = gabi::load<u32>(s + 0xC), count = (u32)mMaterialCapacity,
        groups = gabi::ea(mpMaterialGroups.get());
    if (id < count)
      groups += id * 0x14;
    return groups + ((u32)mVertexBuffer == 0 ? 8 : 0);
  };
  u32 selected = group();
  tapestryApi_prepareGeometry(ptr(gabi::load<u32>(selected + 8)));
  auto strips = [&]() {
    for (u32 row = 0; row < 7; row++) {
      u32 size = gabi::load<u32>(a + 0xA0C),
          stride = gabi::load<u32>(a + 0xA1C),
          indices = gabi::load<u32>(a + 0xA14),
          buffer = gabi::load<u32>(a + 0xA10);
      tapestryApi_drawIndexed(ptr(buffer), 12, size,
                 ptr(indices + stride * row * 12), 0, 1);
    }
  };
  strips();
  gabi::store<u32>(local + 8, 1);
  tapestryApi_restoreRenderState(drawState.get());
  selected = group();
  u32 count = gabi::load<u32>(selected + 4),
      buffer = gabi::load<u32>(selected + 8);
  if (count > 1)
    buffer += 0xF4;
  tapestryApi_prepareGeometry(ptr(buffer));
  strips();
  tapestryApi_restoreRenderState(ptr(0x104B474C));
}

/* 0239D550 */
BOOL daObjTapestry_c::chk_appear() {
  u32 sw = tapestryParam(this, 8, 0), save = gabi::load<u32>(0x101F84DC);
  s8 room = (s8)home.roomNo;
  return !tapestryApi_dSv_info_c_isSwitch(ptr(save + 0x20), sw, room);
}

/* 023997E0 */
void daObjTapestry_c::set_mtx() {
  tapestryApi_PSMTXTrans(ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  tapestryApi_mDoMtx_ZXYrotM(ptr(0x1048D0CC), (s16)shape_angle.x,
             (s16)shape_angle.y, (s16)shape_angle.z);
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  u32 model = gabi::ea(reinterpret_cast<u8*>(mpModel.get()));
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
  tapestryApi_mDoMtx_stack_c_scaleM((f32)scale.x, (f32)scale.y, (f32)scale.z);
  tapestryApi_PSMTXCopy(ptr(0x1048D0CC), mBgMatrix);
}

/* 023998C0 */
void daObjTapestry_c::init_mtx() {
  u32 model = gabi::ea(reinterpret_cast<u8*>(mpModel.get()));
  f32 y = scale.y, x = scale.x, z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  set_mtx();
}

/* 023997DC */
BOOL daObjTapestry_c::solidHeapCB(daObjTapestry_c *self) {
  return self->create_heap();
}

/* 02399704 */
BOOL daObjTapestry_c::create_heap() {
  u8 *data = tapestryResource(4);
  if (!data) {
    tapestryApi_assertionShow(ptr(0x10030828), 0xA25, ptr(0x10030824));
    return 0;
  }
  mpModel = tapestryApi_mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
  data = tapestryResource(7);
  mpBg = tapestryApi_dBgW_NewSet(data, 1, mBgMatrix);
  return mpModel != nullptr && mpBg != nullptr;
}

/* 023996F4 */
s32 daObjTapestry_c::create_res_load() {
  return tapestryApi_dComIfG_resLoad(&mPhase, ptr(0x1003064C));
}

/* 0239D3F0 */
void daObjTapestry_c::init_cc() {
  u32 a = gabi::ea(this);
  for (u32 i = 0; i < 2; i++) {
    u32 triangle = a + 0x3F0 + i * 0x150, status = a + 0x690 + i * 0x3C;
    tapestryApi_dCcD_Stts_Init(ptr(status), 0xFF, 0xFF, this);
    tapestryApi_dCcD_Tri_Set(ptr(triangle), ptr(0x10030774));
    u32 flags = gabi::load<u32>(triangle + 0x94);
    gabi::store<u32>(triangle + 0x44, status);
    gabi::store<u32>(triangle + 0x94, flags | 4);
  }
  set_cc_pos();
}

/* 0239D26C */
void daObjTapestry_c::set_cc_pos() {
  u32 a = gabi::ea(this);
  f32 zero = l_zero, offset = l_collisionYOffset,
      horizontal = l_collisionXZScale,
      vertical = l_collisionYScale;
  gabi::Local<cXyz[3]> points;
  for (u32 tri = 0; tri < 2; tri++) {
    u32 model = gabi::ea(reinterpret_cast<u8*>(mpModel.get()));
    tapestryApi_PSMTXCopy(ptr(model ? model + 0xC8 : 0), ptr(0x1048D0CC));
    tapestryApi_mDoMtx_stack_c_scaleM(horizontal, vertical, horizontal);
    tapestryApi_mDoMtx_stack_c_transM(zero, offset, zero);
    for (u32 vertex = 0; vertex < 3; vertex++) {
      u32 table = 0x101CD124 + tri * 6 + vertex * 2;
      u8 row = gabi::load<u8>(table), column = gabi::load<u8>(table + 1);
      cXyz *position = tapestryApi_daObjTapestryPacket_c_get_now_posVariantA((daObjTapestryPacket_c *)mpPacket, row, column);
      tapestryApi_PSMTXMultVec(ptr(0x1048D0CC), position, &(*points)[vertex]);
    }
    tapestryApi_setTriangleVertices(ptr(a + 0x508 + tri * 0x150), &(*points)[0],
               &(*points)[1], &(*points)[2]);
  }
}

/* 0239EA0C */
BOOL daObjTapestry_c::checkCollision() {
  using namespace TapestryCollision;
  f32 zero = l_zero, force = l_negativeUnit,
      maxSquare = l_clothDiagonalSquared;
  f32 small = l_oneFifth, one = l_unitScale,
      two = l_groundOffset, half = l_half,
      large = l_smokeScale;
  f32 radius = zero;
  BOOL fire = 0;
  u32 hitPosition = 0;
  gabi::Local<cXyz> direction, wind;
  for (u32 i = 0; i < 3; i++)
    gabi::store<f32>(gabi::ea(direction.get()) + i * 4,
                     gabi::load<f32>(0x101FFBA8 + i * 4));
  for (u32 i = 0; i < 2; i++) {
    dCcD_Tri* triangle = &mTriangles[i];
    if (!tapestryApi_dCcD_GObjInf_ChkTgHit(triangle))
      continue;
    u32 object = tapestryApi_dCcD_GObjInf_GetTgHitObj(triangle);
    if (object) {
      u32 attack = (u32)gabi::at<dCcD_GObjInf>(object)->mAtType;
      for (u32 j = 0; j < 3; j++)
        gabi::store<f32>(gabi::ea(direction.get()) + j * 4,
                         (f32)tapestryComponents(&triangle->mGObjTg.mRVec)[j]);
      hitPosition = gabi::ea(&triangle->mGObjTg.mHitPos);
      if (!tapestryApi_cXyz_normalizeRS(direction.get())) {
        u16 angle = (u16)shape_angle.y;
        u32 table = 0x104A44F8 + (angle >> 3) * 8;
        f32 sine = gabi::load<f32>(table), cosine = gabi::load<f32>(table + 4);
        direction->x = sine;
        direction->y = zero;
        direction->z = -cosine;
      }
      if (attack == AT_TYPE_FIRE || attack == AT_TYPE_UNK20000 || attack == AT_TYPE_FIRE_ARROW)
        fire = 1;
      switch (attack) {
      case AT_TYPE_SWORD:
      case AT_TYPE_UNK8:
      case AT_TYPE_BOOMERANG:
      case AT_TYPE_BOKO_STICK:
      case AT_TYPE_FIRE:
      case AT_TYPE_MACHETE:
      case AT_TYPE_UNK800:
      case AT_TYPE_SPIKE:
      case AT_TYPE_UNK2000:
      case AT_TYPE_SKULL_HAMMER:
      case AT_TYPE_STALFOS_MACE:
      case AT_TYPE_DARKNUT_SWORD:
      case AT_TYPE_MOBLIN_SPEAR:
        force = one;
        radius = half;
        break;
      case AT_TYPE_NORMAL_ARROW:
      case AT_TYPE_HOOKSHOT:
      case AT_TYPE_FIRE_ARROW:
      case AT_TYPE_ICE_ARROW:
      case AT_TYPE_LIGHT_ARROW:
        force = two;
        radius = small;
        break;
      case AT_TYPE_UNK20000:
        force = zero;
        radius = small;
        break;
      case AT_TYPE_BOMB:
        radius = large;
        force = two;
        break;
      case AT_TYPE_WIND: {
        for (u32 j = 0; j < 3; j++)
          gabi::store<f32>(gabi::ea(wind.get()) + j * 4,
                           (f32)tapestryComponents(&triangle->mGObjTg.mRVec)[j]);
        f32 square = tapestryApi_PSVECSquareMag(wind.get());
        if (square > maxSquare) {
          f32 length = tapestryApi_std_sqrtf(square);
          tapestryApi_PSVECScale(wind.get(), wind.get(),
                     (f32)(l_clothRowStep / length));
        }
        u32 packet = gabi::ea(mpPacket.get());
        for (u32 j = 0; j < 3; j++)
          gabi::store<f32>(packet + 0x2424 + j * 4,
                           gabi::load<f32>(gabi::ea(wind.get()) + j * 4));
        break;
      }
      default:
        break;
      }
    }
    tapestryApi_dCcD_GObjInf_ClrTgHit(triangle);
  }
  if (!(force < zero) && hitPosition) {
    gabi::Local<cXyz> position, copiedDirection;
    for (u32 j = 0; j < 3; j++)
      gabi::store<f32>(gabi::ea(position.get()) + j * 4,
                       gabi::load<f32>(hitPosition + j * 4));
    tapestryCopyVector(copiedDirection.get(), direction.get());
    tapestryApi_tapestrySetHit(ptr(gabi::ea(mpPacket.get())), position.get(),
               copiedDirection.get(), fire, force, radius);
  }
  return fire;
}

/* 0239D470 */
void daObjTapestry_c::set_eye_pos() {
  u32 a = gabi::ea(this);
  daObjTapestryPacket_c *packet = mpPacket;
  cXyz *right = tapestryApi_daObjTapestryPacket_c_get_now_posVariantB(packet, 0, 5),
       *left = tapestryApi_daObjTapestryPacket_c_get_now_posVariantB(packet, 0, 0);
  gabi::Local<cXyz[4]> corners;
  tapestryApi_cXyz___pl(left, &(*corners)[0], right);
  left =
      tapestryApi_daObjTapestryPacket_c_get_now_posVariantB((daObjTapestryPacket_c *)mpPacket, 7, 0);
  tapestryApi_cXyz___pl(&(*corners)[0], &(*corners)[1], left);
  right =
      tapestryApi_daObjTapestryPacket_c_get_now_posVariantB((daObjTapestryPacket_c *)mpPacket, 7, 5);
  tapestryApi_cXyz___pl(&(*corners)[1], &(*corners)[2], right);
  tapestryApi_cXyz___ml(&(*corners)[2], &(*corners)[3],
             l_eyeInterpolation);
  u32 model = gabi::ea(reinterpret_cast<u8*>(mpModel.get()));
  tapestryApi_PSMTXMultVec(ptr(model ? model + 0xC8 : 0), &(*corners)[3],
             ptr(a + 0x37C));
  f32 z = (f32)eyePos.z, y = (f32)eyePos.y;
  attention_info.position.z = z;
  f32 x = (f32)eyePos.x;
  attention_info.position.y = y;
  attention_info.position.x = x;
}

/* 0239D658 */
s32 daObjTapestry_c::_create() {
  u32 a = gabi::ea(this), flags = gabi::load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (gabi::ea(this)) {
      tapestryApi_fopAc_ac_c_fopAc_ac_cVariantA(this);
      gabi::store<u32>(a + 0xB4, 0x1003071C);
      tapestryApi_construct_arrayVariantA(mTriangles, 2, 0x150, ptr(gabi::guest_fn(reinterpret_cast<const void*>(&tapestryTriangleConstruct))));
      tapestryApi_construct_arrayVariantA(mCollisionStatus, 2, 0x3C, ptr(gabi::guest_fn(reinterpret_cast<const void*>(&tapestryStatusConstruct))));
      flags = gabi::load<u32>(a + 0x2E4);
    }
    gabi::store<u32>(a + 0x2E4, flags | 8);
  }
  s32 phase = create_res_load();
  if (phase == cPhs_COMPLEATE_e) {
    if (!tapestryApi_fopAcM_entrySolidHeap(this, ptr(gabi::guest_fn(reinterpret_cast<const void*>(&daObjTapestry_c::solidHeapCB))), 0x8A0))
      phase = cPhs_ERROR_e;
    else {
      u32 play = tapestryApi_getGameInfo();
      if (tapestryApi_dBgS_Regist(ptr(play + 0x12A0), reinterpret_cast<u8*>(mpBg.get()), this))
        phase = cPhs_ERROR_e;
      else {
        u32 model = gabi::ea(reinterpret_cast<u8*>(mpModel.get()));
        gabi::store<u32>(a + 0x348, model ? model + 0xC8 : 0);
        init_mtx();
        tapestryApi_fopAcM_setCullSizeBox(this, l_cullMinX,
                   l_cullMinY, l_cullMinZ,
                   l_cullMaxX, l_lastColumn,
                   l_cullMaxY);
        daObjTapestryPacket_c *packet =
            static_cast<daObjTapestryPacket_c*>(operator_new(0x2520));
        if (packet)
          packet = tapestryApi_tapestryPacketConstruct(packet);
        mpPacket = packet;
        tapestryApi_tapestryPacketInit(packet, this);
        init_cc();
        set_eye_pos();
        mBurned = 0;
        u8 event = tapestryParam(this, 8, 8);
        play = tapestryApi_getGameInfo();
        mEvent = tapestryApi_dEvent_manager_c_getEventIdx(ptr(play + 0x52C4), 0, event);
        if (chk_appear()) {
          mOpacity = l_unitScale;
          setup_action(WAIT);
        } else {
          u32 bg = gabi::ea(reinterpret_cast<u8*>(mpBg.get()));
          mOpacity = l_zero;
          if (bg && gabi::load<u32>(bg) < 0x100) {
            play = tapestryApi_getGameInfo();
            tapestryApi_cBgS_Release(ptr(play + 0x12A0), reinterpret_cast<u8*>(mpBg.get()));
          }
          setup_action(FINE);
        }
      }
    }
  }
  if ((s8)l_HIO.mChild < 0) {
    s8 child = tapestryApi_mDoHIO_subRoot_c_createChild(ptr(0x10030958), &l_HIO);
    l_HIO.mChild = child;
  }
  return phase;
}

/* 0239D9A4 */
BOOL daObjTapestry_c::_delete() {
  u32 a = gabi::ea(this);
  tapestryApi_daObjTapestryPacket_c_eff_delete((daObjTapestryPacket_c *)mpPacket);
  u32 packet = gabi::ea((daObjTapestryPacket_c *)mpPacket);
  if (packet) {
    u32 target = gabi::load<u32>(gabi::load<u32>(packet + 0xC) + 0xC);
    gabi::call(target, ptr(packet), 3);
  }
  tapestryApi_dComIfG_resDelete(&mPhase, ptr(0x1003064C));
  if (gabi::load<u32>(a + 0xF4)) {
    u32 bg = gabi::ea(reinterpret_cast<u8*>(mpBg.get()));
    if (bg) {
      if (gabi::load<u32>(bg) < 0x100) {
        u32 play = tapestryApi_getGameInfo();
        tapestryApi_cBgS_Release(ptr(play + 0x12A0), reinterpret_cast<u8*>(mpBg.get()));
      }
      mpBg = nullptr;
    }
  }
  s8 child = (s8)l_HIO.mChild;
  if (child >= 0) {
    tapestryApi_mDoHIO_subRoot_c_deleteChild(child);
    l_HIO.mChild = -1;
  }
  return 1;
}

/* 0239EEEC */
void daObjTapestry_c::wait_act_proc() {
  u32 a = gabi::ea(this);
  if (tapestryApi_tapestryCheckCollision(this)) {
    gabi::store<u32>(a + 0x2E0, (u32)actor_status & ~0x80u);
    mBurned = 0;
    u32 bg = gabi::ea(reinterpret_cast<u8*>(mpBg.get()));
    if (bg && gabi::load<u32>(bg) < 0x100) {
      u32 play = tapestryApi_getGameInfo();
      tapestryApi_cBgS_Release(ptr(play + 0x12A0), reinterpret_cast<u8*>(mpBg.get()));
    }
    s16 event = mEvent;
    u32 play = tapestryApi_getGameInfo();
    s32 action = tapestryApi_dEvent_manager_c_getEventData(ptr(play + 0x52C4), event) ? DEMO_REQUEST : BURN;
    setup_action(action);
  } else {
    set_cc_pos();
    for (u32 i = 0; i < 2; i++) {
      u32 play = tapestryApi_getGameInfo();
      tapestryApi_cCcS_Set(ptr(play + 0x26A4), ptr(a + 0x3F0 + i * 0x150));
    }
  }
}

/* 0239EFE8 */
void daObjTapestry_c::demo_request_act_proc() {
  u32 a = gabi::ea(this);
  if (gabi::load<u16>(a + 0xF8) == 2) {
    mBurned = 1;
    setup_action(BURN);
  } else {
    u8 id = tapestryParam(this, 8, 8);
    tapestryApi_fopAcM_orderOtherEventId(this, (s16)mEvent, id, 0xFFFF, 0, 1);
    gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 2);
  }
}

/* 0239F078 */
void daObjTapestry_c::burn_act_proc() {
  if (mBurnTimer > 0) {
    mBurnTimer = (s16)mBurnTimer - 1;
    return;
  }
  f32 target = l_zero, speed = (f32)l_HIO.mFadeSpeed,
      old = mOpacity;
  s32 finished = tapestryApi_cLib_chaseF(&mOpacity, target, speed);
  f32 threshold = l_axialFireChance;
  if (!(old < threshold) && ((f32)mOpacity < threshold)) {
    u32 sw = tapestryParam(this, 8, 0), save = gabi::load<u32>(0x101F84DC);
    s8 room = (s8)home.roomNo;
    tapestryApi_dSv_info_c_onSwitch(ptr(save + 0x20), sw, room);
  }
  threshold = l_half;
  if (!(old < threshold) && ((f32)mOpacity < threshold))
    tapestryApi_daObjTapestryPacket_c_eff_end((daObjTapestryPacket_c *)mpPacket);
  if (finished)
    setup_action(FINE);
}

/* 0239EE88 */
void daObjTapestry_c::burn_act_init_proc() {
  s8 room = (s8)current.roomNo;
  mBurnTimer = (u8)l_HIO.mBurnDelay;
  s32 reverb = tapestryApi_dComIfGp_getReverb(room);
  tapestryApi_startSound(0x69C1, ptr(gabi::ea(this) + 0x37C), 0, reverb);
}

/* 0239EEDC */
void daObjTapestry_c::fine_act_init_proc() {
  u32 a = gabi::ea(this);
  gabi::store<u32>(a + 0x2E0, (u32)actor_status | 0x80);
}

/* 0239D5B0 */
void daObjTapestry_c::setup_action(s32 action) {
  const Action* descriptor = &l_actionInit.mEntries[(u32)action];
  if (descriptor->i != 0)
    actionCall(descriptor, this);
  const be<u32>* table = reinterpret_cast<const be<u32>*>(&l_actionProc.mEntries[(u32)action]);
  // GHS copies the descriptor as two words, with the state store between them.
  reinterpret_cast<be<u32>*>(&mActionDescriptor)[0] = table[0];
  u32 second = table[1];
  mAction = action;
  reinterpret_cast<be<u32>*>(&mActionDescriptor)[1] = second;
}

/* 0239DA64 */
BOOL daObjTapestry_c::_execute() {
  tapestryApi_daObjTapestry_c_set_eye_pos(this);
  u32 bg = gabi::ea(reinterpret_cast<u8*>(mpBg.get()));
  if (bg && gabi::load<u32>(bg) < 0x100)
    tapestryApi_dBgW_Move(ptr(bg));
  if (mActionDescriptor.i != 0)
    actionCall(&mActionDescriptor, this);
  if (mAction != FINE)
    tapestryApi_tapestryPacketCalc((daObjTapestryPacket_c *)mpPacket, this);
  return 1;
}

/* 0239E080 */
BOOL daObjTapestry_c::_draw() {
  u32 env = tapestryApi_getEnvironment();
  tapestryApi_dScnKy_env_light_c_settingTevStruct(ptr(env), 1, &current.pos,
             &tevStr);
  env = tapestryApi_getEnvironment();
  tapestryApi_setLightTevColorType(ptr(env), reinterpret_cast<u8*>(mpModel.get()), &tevStr);
  tapestryApi_mDoExt_modelUpdateDL(reinterpret_cast<u8*>(mpModel.get()), 0);
  if (mAction != FINE)
    tapestryApi_tapestryPacketUpdate((daObjTapestryPacket_c *)mpPacket);
  return 1;
}

/* 0239D8B8 */
s32 daObjTapestry_Create(daObjTapestry_c *self) {
  return self->_create();
}

/* 0239DA60 */
BOOL daObjTapestry_Delete(daObjTapestry_c *self) {
  return self->_delete();
}

/* 0239DB10 */
BOOL daObjTapestry_Execute(daObjTapestry_c *self) {
  return self->_execute();
}

/* 0239E0F0 */
BOOL daObjTapestry_Draw(daObjTapestry_c *self) {
  return self->_draw();
}

/* 0239E0F4 */
BOOL daObjTapestry_IsDelete(daObjTapestry_c *self) {
  return 1;
}

// HD: compiler-generated, inlined and SDK adapter code of this translation unit.

/* 023998E0 */
static daObjTapestryPLight_c *
daObjTapestryPLight_c__ct(daObjTapestryPLight_c *self) {
  if (!self)
    self = static_cast<daObjTapestryPLight_c*>(operator_new(0x30));
  if (self) {
    self->mpEmitter = nullptr;
    self->mPower = l_unitScale;
    self->mActive = 0;
  }
  return self;
}

/* 0239FB40 */
static u32 tapestryParam(daObjTapestry_c *self, u32 width, u32 shift) {
  u32 p = gabi::load<u32>(gabi::ea(self) + 0xB0),
      bit = (width & 32) ? 0 : 1u << (width & 31),
      shifted = (shift & 32) ? 0 : p >> (shift & 31);
  return shifted & (bit - 1);
}

/* 0239F308 */
static u8 *tapestryTriangleConstruct(u8 *self) {
  if (!self)
    self = static_cast<u8*>(operator_new(0x150));
  if (self) {
    tapestryApi_dCcD_GObjInf_dCcD_GObjInf(self);
    u32 a = gabi::ea(self);
    gabi::store<u32>(a + 0x114, 0x100015A8);
    gabi::store<u32>(a + 0x110, 0x1003066C);
    tapestryApi_constructTrianglePlane(ptr(a + 0x118));
    gabi::store<u32>(a + 0x3C, 0x1004B010);
    gabi::store<u32>(a + 0x128, 0x1004B058);
    gabi::store<u32>(a + 0x114, 0x1004B068);
  }
  return self;
}

/* 0239F394 */
static u8 *tapestryStatusConstruct(u8 *self) {
  if (!self)
    self = static_cast<u8*>(operator_new(0x3C));
  if (self) {
    tapestryApi_constructCollisionStatus(self);
    tapestryApi_dCcD_GStts_dCcD_GStts(self + 0x1C);
    gabi::store<u32>(gabi::ea(self) + 0x18, 0x1004AE88);
    gabi::store<u32>(gabi::ea(self) + 0x1C, 0x1004AEC0);
  }
  return self;
}

/* 0239F3FC */
static void daObjTapestryPLight_c__dt(u8 *self, u32 flags) {
  if (self && (flags & 1))
    operator_delete(self);
}

/* 0239F410 */
static u8 *tapestryVertexConstruct(u8 *self) {
  if (!self)
    self = static_cast<u8*>(operator_new(0x254));
  if (self) {
    tapestryApi_constructVertexBuffer(self + 4);
    tapestryApi_constructVertexAttribute(self + 0x158);
    gabi::store<u32>(gabi::ea(self) + 0x250, 0);
    gabi::store<u32>(gabi::ea(self) + 0x24C, 0);
  }
  return self;
}

/* 0239F46C */
static u8 *tapestryQuadConstruct(u8 *self) {
  if (!self)
    self = static_cast<u8*>(operator_new(0x10));
  return self;
}

/* 0239F498 */
static u8 *tapestryFireConstruct(u8 *self) {
  if (!self)
    self = static_cast<u8*>(operator_new(0x2C));
  if (self) {
    tapestryApi_dPa_followEcallBack_dPa_followEcallBack(self, 0, 0);
    auto* callback = reinterpret_cast<daObjTapestryFireEff_c*>(self);
    callback->__vtbl = 0x100309C8;
    f32 x = gabi::load<f32>(0x101FFBA8);
    callback->mPosition.x = x;
    f32 y = gabi::load<f32>(0x101FFBAC);
    callback->mPosition.y = y;
    f32 z = gabi::load<f32>(0x101FFBB0);
    callback->mVelocity.x = x;
    callback->mPosition.z = z;
    callback->mVelocity.y = y;
    callback->mVelocity.z = z;
  }
  return self;
}

/* 0239F51C */
static cXyz *tapestryPositionCopy(cXyz *self, cXyz *other) {
  if (!self)
    self = static_cast<cXyz*>(operator_new(12));
  if (self) {
    self->x = other->x;
    self->y = other->y;
    self->z = other->z;
  }
  return self;
}

/* 0239F574 */
static void tapestryLineDestruct(u8 *self, u32 flags) {
  if (self) {
    u32 a = gabi::ea(self);
    gabi::store<u32>(a + 0x20, 0x1003069C);
    gabi::store<u32>(a + 0x40, 0x100306BC);
    gabi::store<u32>(a + 0x4C, 0x1003067C);
    tapestryApi_cBgS_Chk_cBgS_Chk(self, 0);
    if (flags & 1)
      operator_delete(self);
  }
}

/* 0239F5EC */
static void tapestryVertexEmpty(u8 *self, u32 flags) {
}

/* 0239F5F0 */
static void tapestryQuadEmpty(u8 *self, u32 flags) {
}

/* 0239F5F4 */
static void tapestryPositionDestruct(u8 *self, u32 flags) {
  if (self && (flags & 1))
    operator_delete(self);
}

/* 0239F608 */
static void tapestryVertexDestruct(u8 *self, u32 flags) {
  if (self) {
    tapestryApi_destroyVertexAttribute(self + 0x158, 2);
    tapestryApi_destroyVertexBuffer(self + 4, 2);
    if (flags & 1)
      operator_delete(self);
  }
}

/* 0239FAA4 */
static void tapestryFireEmpty(u8 *self, u32 flags) {
}

/* 0239FAA8 */
static void daObjTapestry_c__dt(daObjTapestry_c *self, u32 flags) {
  if (self) {
    u32 a = gabi::ea(self);
    tapestryApi_destroy_arr(ptr(a + 0x690), 2, 0x3C, 0x02515860, 0, 0);
    tapestryApi_destroy_arr(ptr(a + 0x3F0), 2, 0x150, 0x025159F8, 0, 0);
    tapestryApi_fopAc_ac_c_fopAc_ac_cVariantB(self, 0);
    if (flags & 1)
      operator_delete(self);
  }
}

/* 0239FB3C */
static void tapestryHIOEmpty(u8 *self, u32 flags) {
}

/* 0239F218 */
static void tapestryStaticInit() {
  for (s32 i = 3; i >= 0; i--)
    gabi::store<u32>(0x1046C108 + i * 4, 0);
  tapestryApi_register_global_object(ptr(0x101CD170));
  f32 low = l_negativePi, high = l_pi;
  gabi::store<f32>(0x1046C0EC, low);
  gabi::store<f32>(0x1046C0F0, high);
  tapestryApi_initializeMatrixStatics(ptr(0x1046C104));
  tapestryApi_register_global_object(ptr(0x101CD17C));
  tapestryApi_initializeMathStatics(ptr(0x1046C105));
  tapestryApi_register_global_object(ptr(0x101CD188));
  f32 value = l_clothColumnStep, input = l_clothCellArea;
  l_restHorizontal = value;
  f32 second = l_clothDepthStep, third = l_cullMaxY;
  l_restVertical = second;
  l_restDepth = third;
  l_restDiagonal = tapestryApi_std_sqrtf(input);
  tapestryApi_tapestryHIOConstruct(&l_HIO);
  tapestryApi_tapestryDrawDataConstruct(&l_drawData);
}

/* 0239DBB4 */
static void tapestrySignedColor(be<f32> *out, be<s16> *color) {
  s16 r = color[0], b = color[2], g = color[1], a = color[3];
  f32 divisor = l_colorRange;
  f32 rf = (f32)r / divisor, gf = (f32)g / divisor, bf = (f32)b / divisor,
      af = (f32)a / divisor;
  out[0] = rf;
  out[1] = gf;
  out[2] = bf;
  out[3] = af;
}

/* 0239DC78 */
static void tapestryByteColor(be<f32> *out, u8 *color) {
  u32 address = gabi::ea(color);
  u8 r = gabi::load<u8>(address), g = gabi::load<u8>(address + 1);
  f32 divisor = l_colorRange;
  u8 b = gabi::load<u8>(address + 2), a = gabi::load<u8>(address + 3);
  f32 rf = (f32)r / divisor, gf = (f32)g / divisor, bf = (f32)b / divisor,
      af = (f32)a / divisor;
  out[0] = rf;
  out[1] = gf;
  out[2] = bf;
  out[3] = af;
}

/* 0239DB14 */
static void tapestryMatrixCopy(be<f32> *out, be<f32> *matrix) {
  f32 copy[12];
  for (u32 i = 0; i < 12; i++)
    copy[i] = matrix[i];
  for (u32 i = 0; i < 12; i++)
    out[i] = copy[i];
}

/* 0239F668 */
static void daObjTapestryPacket_c__dt(daObjTapestryPacket_c *self, u32 flags) {
  if (!self)
    return;
  u32 a = gabi::ea(self), vertices = a + 0xA4;
  self->mVtable = 0x10030998;
  for (u32 i = 0; i < 4; i++)
    tapestryFreeBuffer(vertices + i * 0x254);
  gabi::store<u32>(vertices + 0x960, 0);
  for (s32 i = 0; i < gabi::load<s32>(a + 0xA24); i++) {
    u32 buffer = gabi::load<u32>(a + 0xA28);
    if ((u32)i < gabi::load<u32>(a + 0xA24))
      buffer += (u32)i * 0x23C;
    for (u32 j = 0; j < 2; j++)
      tapestryApi_resetUniformBuffer(ptr(buffer + 0x10 + j * 0x1C));
  }
  for (u32 base : {0xA40u, 0xAE8u})
    for (u32 i = 0; i < 2; i++)
      tapestryApi_resetUniformBuffer(ptr(a + base + i * 0x1C));
  tapestryApi_destroy_arr(ptr(a + 0x20C0), 16, 0x2C, gabi::guest_fn(reinterpret_cast<const void*>(&tapestryPositionDestruct)), 0, 0);
  tapestryApi_destroyTextureSampler(&self->mTextureSampler, 2);
  tapestryApi_destroyMatPacket(&self->mPixelUniform, 0);
  tapestryApi_destroyMatPacket(&self->mVertexUniform, 0);
  tapestryApi_destroyModelPacket(&self->mLightingMaterial, 2);
  tapestryApi_destroyIndexBuffer(&self->mIndexBuffer, 2);
  if (vertices) {
    for (u32 i = 0; i < 4; i++)
      tapestryFreeBuffer(vertices + i * 0x254);
    gabi::store<u32>(vertices + 0x960, 0);
    tapestryApi_destroy_arr(ptr(vertices), 4, 0x254, gabi::guest_fn(reinterpret_cast<const void*>(&tapestryVertexDestruct)), 0, 0);
  }
  u32 groups = gabi::ea(self->mpMaterialGroups.get());
  if (groups) {
    s32 i = 0;
    while (i < (s32)self->mMaterialCapacity) {
      u32 group = groups + (u32)i * 0x14;
      if (group) {
        gabi::store<u32>(group, 0);
        tapestryFreeVectorArray(group, 4, 8);
        tapestryFreeVectorArray(group, 0xC, 0x10);
      }
      i++;
      groups = gabi::ea(self->mpMaterialGroups.get());
    }
    u32 allocator = tapestryApi_findFromRoot(ptr(gabi::load<u32>(0x101F8B4C)), ptr(groups)),
        table = gabi::load<u32>(allocator + 0xC),
        target = gabi::load<u32>(table + 0x3C);
    gabi::call(target, ptr(allocator), ptr(gabi::ea(self->mpMaterialGroups.get())));
    self->mMaterialCapacity = 0;
    gabi::store<u32>(a + 0xA0, 0);
  }
  tapestryApi_destroyPacket(self, 0);
  if (flags & 1)
    operator_delete(self);
}

/* WWHD glue (tools/verify/autoglue.py): unit-local ABI and data.
data l_HIO 1046C154
data l_hioDefaults 100307C8
data l_actionInit 101CD130
data l_actionProc 101CD150
data l_restDepth 1046C0F4
data l_restHorizontal 1046C0F8
data l_restVertical 1046C0FC
data l_restDiagonal 1046C100
data l_initialWinds 1046C124
data l_smokeParticleScale 1046C148
data l_windInitialized 1046C214
data l_groundInitialized 1046C220
data l_groundCheck 1046C1C0
data l_drawData 1046C22C
data l_unitScale 10030840
data l_anglePeriod 10030844
data l_zero 10030848
data l_windAttenuation 1003084C
data l_particleYSpeed 10030850
data l_negativeHalf 10030854
data l_oneFifth 10030858
data l_threeTenths 1003085C
data l_half 10030860
data l_epsilon 10030864
data l_axialFireChance 10030868
data l_diagonalFireChance 1003086C
data l_rowCount 10030870
data l_lastRow 10030874
data l_lastColumn 10030878
data l_smokeRightChance 1003087C
data l_groundOffset 10030880
data l_gravityUnits 10030884
data l_rowWindScale 10030890
data l_centerColumn 10030894
data l_edgeWindScale 10030898
data l_depthLimit 1003089C
data l_depthMidpoint 100308A0
data l_depthBottom 100308A4
data l_groundCheckHeight 100308A8
data l_negativeUnit 100308AC
data l_tangentWeightB 100308B0
data l_tangentWeightC 100308B4
data l_tangentWeightE 100308B8
data l_tangentWeightF 100308BC
data l_tangentWeightA 100308C0
data l_tangentWeightD 100308C4
data l_fireDrag 100308C8
data l_hitPower 100308CC
data l_hitRadius 100308D0
data l_collisionXZScale 100308D4
data l_colorRange 100308D8
data l_clothLeft 10030928
data l_clothTop 1003092C
data l_clothRight 10030930
data l_inverseLastRow 10030934
data l_collisionYOffset 10030938
data l_collisionYScale 1003093C
data l_eyeInterpolation 10030940
data l_cullMinX 10030944
data l_cullMinY 10030948
data l_cullMinZ 1003094C
data l_cullMaxX 10030950
data l_cullMaxY 10030954
data l_smokeScale 10030964
data l_clothDiagonalSquared 10030968
data l_clothRowStep 1003096C
data l_negativePi 10030970
data l_pi 10030974
data l_clothColumnStep 10030978
data l_clothDepthStep 1003097C
data l_clothCellArea 10030980
data l_shaderUnit 1014517C
data l_shaderZero 10145180
ext 026066C4 u8* tapestryApi_dRes_control_c_getRes(void*, void*, s32)
ext 02520460 s32 tapestryApi_dComIfG_resLoad(void*, void*)
ext 0273AA24 void tapestryApi_assertionShow(void*, s32, void*)
ext 025E38E0 J3DModel* tapestryApi_mDoExt_J3DModel__create(void*, s32, s32)
ext 024F2478 dBgW* tapestryApi_dBgW_NewSet(void*, s32, void*)
ext 028E93CC void tapestryApi_PSMTXTrans(void*, f32, f32, f32)
ext 025F1B48 void tapestryApi_mDoMtx_ZXYrotM(void*, s16, s16, s16)
ext 025F2518 void tapestryApi_mDoMtx_stack_c_scaleM(f32, f32, f32)
ext 028E90D4 void tapestryApi_PSMTXCopy(void*, void*)
ext 025564B4 void tapestryApi_dKy_plight_set(void*)
ext 0255A374 void tapestryApi_dKy_plight_cut(void*)
ext 025A5AC8 void tapestryApi_dPa_followEcallBack_end(void*)
ext 02520540 s32 tapestryApi_dComIfGp_getReverb(s8)
ext 025E1A40 void tapestryApi_startSound(s32, void*, s32, s32)
ext 025BA0C0 s32 tapestryApi_dSv_info_c_isSwitch(void*, u32, s8)
ext 025F24E0 void tapestryApi_mDoMtx_stack_c_transM(f32, f32, f32)
ext 0239A7A0 cXyz* tapestryApi_daObjTapestryPacket_c_get_now_posVariantA(void*, u8, u8)
ext 028E8F64 void tapestryApi_PSMTXMultVec(void*, void*, void*)
ext 0201924C void tapestryApi_setTriangleVertices(void*, void*, void*, void*)
ext 02515F14 void tapestryApi_dCcD_Stts_Init(void*, s32, s32, void*)
ext 0251650C void tapestryApi_dCcD_Tri_Set(void*, void*)
ext 0239A7A0 cXyz* tapestryApi_daObjTapestryPacket_c_get_now_posVariantB(void*, s32, s32)
ext 0201AD78 void tapestryApi_cXyz___pl(void*, void*, void*)
ext 0201AE48 void tapestryApi_cXyz___ml(void*, void*, f32)
ext 0239D92C void tapestryApi_daObjTapestryPacket_c_eff_delete(void*)
ext 025204C8 void tapestryApi_dComIfG_resDelete(void*, void*)
ext 025200D4 u32 tapestryApi_getGameInfo()
ext 020087EC void tapestryApi_cBgS_Release(void*, void*)
ext 025F0A18 void tapestryApi_mDoHIO_subRoot_c_deleteChild(s8)
ext 0239D470 void tapestryApi_daObjTapestry_c_set_eye_pos(void*)
ext 024F43DC void tapestryApi_dBgW_Move(void*)
ext 0239C574 void tapestryApi_tapestryPacketCalc(void*, void*)
ext 02555D0C u32 tapestryApi_getEnvironment()
ext 025626A4 void tapestryApi_dScnKy_env_light_c_settingTevStruct(void*, s32, void*, void*)
ext 02562F5C void tapestryApi_setLightTevColorType(void*, void*, void*)
ext 025E2DE0 void tapestryApi_mDoExt_modelUpdateDL(void*, s32)
ext 0239E020 void tapestryApi_tapestryPacketUpdate(void*)
ext 0239EA0C s32 tapestryApi_tapestryCheckCollision(void*)
ext 02544044 u32 tapestryApi_dEvent_manager_c_getEventData(void*, s16)
ext 0200E240 void tapestryApi_cCcS_Set(void*, void*)
ext 025D7A58 void tapestryApi_fopAcM_orderOtherEventId(void*, s16, u8, s32, s32, s32)
ext 0200F5C8 s32 tapestryApi_cLib_chaseF(void*, f32, f32)
ext 025B9E38 void tapestryApi_dSv_info_c_onSwitch(void*, u32, s8)
ext 0239E334 void tapestryApi_daObjTapestryPacket_c_eff_end(void*)
ext 025D4ED0 void tapestryApi_fopAc_ac_c_fopAc_ac_cVariantA(void*)
ext 028EFFD0 void tapestryApi_construct_arrayVariantA(void*, s32, s32, void*)
ext 025D63E8 s32 tapestryApi_fopAcM_entrySolidHeap(void*, void*, s32)
ext 024EEA6C s32 tapestryApi_dBgS_Regist(void*, void*, void*)
ext 025D674C void tapestryApi_fopAcM_setCullSizeBox(void*, f32, f32, f32, f32, f32, f32)
ext 0239992C daObjTapestryPacket_c* tapestryApi_tapestryPacketConstruct(void*)
ext 0239CEF4 void tapestryApi_tapestryPacketInit(void*, void*)
ext 02543F10 s16 tapestryApi_dEvent_manager_c_getEventIdx(void*, s32, u8)
ext 025F0A10 s8 tapestryApi_mDoHIO_subRoot_c_createChild(void*, void*)
ext 02515FB8 void tapestryApi_dCcD_GObjInf_dCcD_GObjInf(void*)
ext 02019040 void tapestryApi_constructTrianglePlane(void*)
ext 0200BD2C void tapestryApi_constructCollisionStatus(void*)
ext 02515DA0 void tapestryApi_dCcD_GStts_dCcD_GStts(void*)
ext 027B5BD8 void tapestryApi_constructVertexBuffer(void*)
ext 027BF734 void tapestryApi_constructVertexAttribute(void*)
ext 025A5894 void tapestryApi_dPa_followEcallBack_dPa_followEcallBack(void*, s32, s32)
ext 02008DAC void tapestryApi_cBgS_Chk_cBgS_Chk(void*, s32)
ext 027BF880 void tapestryApi_destroyVertexAttribute(void*, s32)
ext 027B5CBC void tapestryApi_destroyVertexBuffer(void*, s32)
ext 028F0164 void tapestryApi_destroy_arr(void*, s32, s32, s32, s32, s32)
ext 025D50BC void tapestryApi_fopAc_ac_c_fopAc_ac_cVariantB(void*, s32)
ext 028E9108 void tapestryApi_PSMTXConcat(void*, void*, void*)
ext 027F0E04 void tapestryApi_J3DDrawBuffer_entryImm(void*, void*, s32)
ext 0239DD2C void tapestryApi_tapestryUploadVertices(void*)
ext 0239E0FC u8* tapestryApi_tapestryTextureCoordinates(void*)
ext 0239E1A8 void tapestryApi_tapestryDrawIndices(void*)
ext 028F026C void tapestryApi_register_global_object(void*)
ext 028ED6F8 void tapestryApi_initializeMatrixStatics(void*)
ext 028EAB2C void tapestryApi_initializeMathStatics(void*)
ext 028F4384 f32 tapestryApi_std_sqrtf(f32)
ext 0239F198 void tapestryApi_tapestryHIOConstruct(void*)
ext 0239E224 void tapestryApi_tapestryDrawDataConstruct(void*)
ext 0239C32C void tapestryApi_tapestrySetPointLight(void*, void*, void*)
ext 028E91EC u32 tapestryApi_PSMTXInverse(void*, void*)
ext 02399F9C void tapestryApi_tapestryWind(void*)
ext 0239A38C void tapestryApi_tapestryHitCoordinates(void*)
ext 0239A9EC void tapestryApi_tapestryFireSimulation(void*)
ext 0239B71C void tapestryApi_tapestryPositions(void*)
ext 0239B85C void tapestryApi_tapestryNormals(void*)
ext 0239BF60 void tapestryApi_tapestryParticlePositions(void*)
ext 0239C210 void tapestryApi_tapestrySmokeMove(void*, void*)
ext 0239C50C void tapestryApi_tapestryLightMove(void*, void*, void*)
ext 02019788 f32 tapestryApi_cM_rnd()
ext 025A590C void tapestryApi_dPa_followEcallBack_execute(void*, void*)
ext 020198D8 f32 tapestryApi_cM_rndF(f32)
ext 0239A7C4 u32 tapestryApi_tapestryStartFire(void*, s32, s32)
ext 028E8DD0 f32 tapestryApi_PSVECSquareMag(void*)
ext 0201AEAC void tapestryApi_cXyz___dv(void*, void*, f32)
ext 028E8D88 void tapestryApi_PSVECAdd(void*, void*, void*)
ext 0239C158 void tapestryApi_tapestrySmokeSet(void*)
ext 020195B0 s32 tapestryApi_cM_atan2s(f32, f32)
ext 0200ED84 void tapestryApi_cLib_addCalc2(void*, f32, f32, f32)
ext 025A847C u8* tapestryApi_dPa_control_c_setVariantA(void*, s32, s32, void*, void*, void*, s32, s32, s32, s32, s32, s32)
ext 025A847C void tapestryApi_dPa_control_c_setVariantB(void*, s32, s32, void*, void*, void*, s32, void*, s32, s32, s32, s32)
ext 0239C120 void tapestryApi_daObjTapestryPLight_c_plight_make(void*)
ext 0239A550 s32 tapestryApi_tapestryCanStartFire(void*, s32, s32)
ext 025A847C void tapestryApi_dPa_control_c_setVariantC(void*, s32, s32, void*, s32, void*, s32, void*, s32, s32, s32, s32)
ext 0239AE28 void tapestryApi_tapestrySpringNeighbors(void*, s32, s32)
ext 0239B100 u8* tapestryApi_daObjTapestryPacket_c_calc_acc_gravity(void*)
ext 0239B12C void tapestryApi_tapestryWaveForce(void*, s32, s32)
ext 0239B3AC void tapestryApi_tapestryHitForce(void*, s32, s32)
ext 0239B498 void tapestryApi_tapestryVelocity(void*, s32, s32)
ext 0239B578 void tapestryApi_tapestryPositionCorrection(void*, s32, s32)
ext 0239AD14 void tapestryApi_tapestrySpringForce(void*, void*, void*, f32, f32)
ext 028E9044 void tapestryApi_PSMTXMultVecSR(void*, void*, void*)
ext 0201794C f32 tapestryApi_cM3d_lineVsPosSuisenCross(void*, void*, void*, void*)
ext 02008E0C void tapestryApi_cBgS_GndChk_cBgS_GndChk(void*)
ext 02008974 f32 tapestryApi_cBgS_GroundCross(void*, void*)
ext 0239A90C void tapestryApi_tapestryFireLeap(void*, s32, s32)
ext 028F0294 void tapestryApi_constructArrayCopy(void*, s32, s32, s32, void*)
ext 0201ADE0 void tapestryApi_cXyz___mi(void*, void*, void*)
ext 02399F90 u8* tapestryApi_daObjTapestryPacket_c_smokeCallback_init(void*)
ext 0239C678 void tapestryApi_tapestryRenderInit(void*)
ext 028E8E64 void tapestryApi_PSVECScale(void*, void*, f32)
ext 027F1278 void tapestryApi_bgnPacket_construct(void*)
ext 028EFFD0 void tapestryApi_construct_arrayVariantB(void*, s32, s32, s32)
ext 027B5430 void tapestryApi_constructIndexBuffer(void*)
ext 027FD6F4 void tapestryApi_construct(void*)
ext 027FB40C void tapestryApi_constructMatPacket(void*)
ext 028F521C void tapestryApi_clearMemory(void*, s32)
ext 027BE6B8 void tapestryApi_constructTextureResource(void*)
ext 027BDF7C void tapestryApi_constructTextureSampler(void*)
ext 023998E0 void tapestryApi_tapestryLightConstruct(void*)
ext 028E9098 void tapestryApi_PSMTXIdentity(void*)
ext 027B5E94 void tapestryApi_flushBuffer(void*, s32, u32)
ext 0255F8F4 void tapestryApi_prepareTevColors(void*)
ext 0239DB14 void tapestryApi_tapestryMatrixCopy(void*, void*)
ext 027FDA54 void tapestryApi_setupShader(void*, s32, void*, void*, void*)
ext 0239DBB4 void tapestryApi_tapestrySignedColor(void*, void*)
ext 0274D458 void tapestryApi_scaleColor(void*, void*, f32)
ext 027FDFF4 void tapestryApi_selectUniform(void*, s32)
ext 0239DC78 void tapestryApi_tapestryByteColor(void*, void*)
ext 0274D2AC void tapestryApi_scaleColorRGB(void*, f32)
ext 027FB678 void tapestryApi_prepareUniform(void*)
ext 027BF7E8 void tapestryApi_resetVertexAttribute(void*)
ext 02755FEC u32 tapestryApi_findFromRoot(void*, void*)
ext 027BEBEC void tapestryApi_resetUniformBuffer(void*)
ext 027BE2B0 void tapestryApi_destroyTextureSampler(void*, s32)
ext 027FB528 void tapestryApi_destroyMatPacket(void*, s32)
ext 027FD764 void tapestryApi_destroyModelPacket(void*, s32)
ext 027B54A0 void tapestryApi_destroyIndexBuffer(void*, s32)
ext 027F13DC void tapestryApi_destroyPacket(void*, s32)
ext 0201B080 void tapestryApi_cXyz_outprod(void*, void*, void*)
ext 0201B47C s32 tapestryApi_cXyz_normalizeRS(void*)
ext 025162A4 s32 tapestryApi_dCcD_GObjInf_ChkTgHit(void*)
ext 02516300 u32 tapestryApi_dCcD_GObjInf_GetTgHitObj(void*)
ext 0251621C void tapestryApi_dCcD_GObjInf_ClrTgHit(void*)
ext 0239E3B4 void tapestryApi_tapestrySetHit(void*, void*, void*, s32, f32, f32)
ext C0006900 void tapestryApi_setPixelUniform(s16, void*, void*)
ext C0006A38 void tapestryApi_setVertexUniform(s16, void*, void*)
ext C00068A8 void tapestryApi_setGeometryUniform(s16, void*, void*)
ext 027BE53C void tapestryApi_bindTexture(void*, void*, s32, s32)
ext 027F29D4 u32 tapestryApi_shaderCache(void*)
ext 027BB9E0 void tapestryApi_updateMaterial(void*, s32)
ext 027B9F68 void tapestryApi_setShader(void*)
ext C00060E0 void tapestryApi_setMaterialData(void*, u32)
ext 027BB7CC void tapestryApi_applyMaterial(void*)
ext 027FFE54 void tapestryApi_applySurfaceShader(void*, void*)
ext 02750250 void tapestryApi_constructRenderState(void*)
ext 0280037C void tapestryApi_configureSurfaceRenderState(u32, void*)
ext 02750370 void tapestryApi_restoreRenderState(void*)
ext 027BFE5C void tapestryApi_prepareGeometry(void*)
ext C0006178 void tapestryApi_drawIndexed(void*, s32, u32, void*, s32, s32)
ext 02756140 u32 tapestryApi_getCurrentAllocator(void*)
ext 027FFCBC u32 tapestryApi_getPacketModel(void*, void*)
ext 027B90AC s32 tapestryApi_findMaterialName(void*, void*)
ext 02800B0C void tapestryApi_initializeMaterial(void*, void*, s32)
ext 0280068C void tapestryApi_initializeMaterialGroups(void*, void*, s32)
ext 027FF478 void tapestryApi_initializeVertexBuffer(void*, void*, s32, void*)
ext 027FF530 void tapestryApi_initializeVertexAttribute(void*, void*, void*, void*, s32)
ext 027FE084 void tapestryApi_initializeLightingMaterial(void*, s32, s32)
ext 027B54E0 void tapestryApi_initializeIndexBuffer(void*, void*, s32, s32)
ext 0274FBF8 void tapestryApi_beginTextureAccess(void*)
ext 026124B0 u8* tapestryApi_loadArchiveResource(void*, void*, void*, s32)
ext 02773870 void tapestryApi_initializeTextureResource(void*, void*, void*)
ext 0274FCCC void tapestryApi_endTextureAccess(void*)
ext 027BDEB4 void tapestryApi_copyTextureSampler(void*, void*)
*/
