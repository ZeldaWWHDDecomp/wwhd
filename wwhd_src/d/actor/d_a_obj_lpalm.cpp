/* Palm tree: WWHD wind-quaternion animation and separate collision matrix.
 * Actual TU0236C290..0236CDB7 includes the two trailing virtual helpers. */
#include "d/actor/d_a_obj_lpalm.h"
#include "bindings.h"
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }
template <class T> void write(u32 a, u32 o, T v) {
  *gabi::at<be<T>>(a + o) = v;
}
void *ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
f32 sine(s16 angle) { return read<f32>(0x104A44F8, ((u16)angle >> 3) * 8); }
f32 cosine(s16 angle) { return read<f32>(0x104A44FC, ((u16)angle >> 3) * 8); }
struct LpalmArchive {
  be<u32> name, vt;
};
u32 resource(s32 index) {
  gabi::Local<LpalmArchive> archive;
  archive->name = 0x1002C1DC;
  archive->vt = 0x1002C12C;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(),
                         index);
}
void identity(LpalmQuaternion &q) {
  q.x = 0.f;
  q.y = 0.f;
  q.z = 0.f;
  q.w = 1.f;
}
void copyQuaternion(LpalmQuaternion &to, const LpalmQuaternion &from) {
  f32 x = from.x, y = from.y, z = from.z, w = from.w;
  to.x = x;
  to.y = y;
  to.z = z;
  to.w = w;
}
} // namespace
BOOL lpalm_nodeCallback(void *node, s32 timing) {
  WWHD_FUNC(0x0236C290, BOOL, node, timing);
  u32 model = read<u32>(0x104B462C);
  u32 joint = gabi::call<u32>(0x027F7878, node);
  u16 number = read<u16>(joint, 4);
  u32 actor = read<u32>(model, 0xB8);
  if (timing == 0 && (number == 2 || number == 3)) {
    u32 matrices = read<u32>(model, 0x2C);
    u32 source = read<u32>(matrices, 0x10) + number * 0x30;
    write<u16>(matrices, 4, read<u16>(matrices, 4) | 0x10);
    gabi::call(0x028E90D4, ptr(source), mDoMtx_stack_c::get());
    gabi::call(0x025F1C5C, mDoMtx_stack_c::get(), (s16)-0x4000);
    gabi::call(0x025F25CC, ptr(actor, 0x3B0));
    if (number == 2)
      gabi::call(0x025F25CC, ptr(actor, 0x3D0));
    else
      gabi::call(0x025F25CC, ptr(actor, 0x3E0));
    gabi::call(0x025F1C5C, mDoMtx_stack_c::get(), (s16)0x4000);
    matrices = read<u32>(model, 0x2C);
    u32 destination = read<u32>(matrices, 0x10) + number * 0x30;
    write<u16>(matrices, 4, read<u16>(matrices, 4) | 0x10);
    mtx_copy(gabi::at<Mtx34>(destination), mDoMtx_stack_c::get());
  }
  return 1;
}
VERIFY(0x0236C290, lpalm_nodeCallback);
BOOL lpalm_createHeap(daObjLpalm_c *palm) {
  WWHD_FUNC(0x0236C44C, BOOL, palm);
  u32 data = resource(4);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x1002C154), 269, ptr(0x1002C168));
  u16 index = 0;
  u32 header = gabi::call<u32>(0x027F3F94, ptr(data));
  while (index < read<u16>(header, 8)) {
    u32 joint = read<u32>(data, 8);
    if (index < read<u32>(data, 4))
      joint += index * 0x1C;
    write<u32>(joint, 8, 0x0236C290);
    index = (u16)(index + 1);
    header = gabi::call<u32>(0x027F3F94, ptr(data));
  }
  if (!data)
    return 0;
  palm->palmModel = gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x11000002);
  if (!palm->palmModel)
    return 0;
  write<u32>(palm->palmModel, 0xB8, gabi::ea(palm));
  u32 collision = resource(7);
  palm->background =
      gabi::call<u32>(0x024F2478, ptr(collision), 1, &palm->backgroundMatrix);
  return palm->background ? 1 : 0;
}
VERIFY(0x0236C44C, lpalm_createHeap);
BOOL lpalm_heapCallback(daObjLpalm_c *palm) {
  WWHD_FUNC(0x0236C594, BOOL, palm);
  return lpalm_createHeap(palm);
}
VERIFY(0x0236C594, lpalm_heapCallback);
void lpalm_createInit(daObjLpalm_c *palm) {
  WWHD_FUNC(0x0236C598, void, palm);
  identity(palm->targetRotation);
  identity(palm->baseRotation);
  identity(palm->leafRotation[1]);
  identity(palm->leafRotation[0]);
  palm->bendAngle[0] = 0;
  palm->bendAngle[1] = 0;
  palm->waveAngle[0] = 0;
  palm->waveAngle[1] = (s16)gabi::ftoi(gabi::call<f32>(0x02019918, 32768.f));
  u32 model = palm->palmModel;
  palm->cullMtx = model ? model + 0xC8 : 0;
  gabi::call(0x025D674C, palm, -350.f, -50.f, -350.f, 350.f, 1300.f, 350.f);
  palm->cullSizeFar = 2.37f;
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, ptr(play, 0x12A0), ptr(palm->background), palm);
  if (!read<u32>(0x101FDC08)) {
    write<u32>(0x101FDC08, 0, 1);
    write<f32>(0x101FDC0C, 0, .7f);
  }
  auto j3d = gabi::at<J3DModel>((u32)palm->palmModel);
  J3DModel_setBaseScale(j3d, &palm->scale);
  mDoMtx_stack_c::transS(palm->current.pos.x, palm->current.pos.y,
                         palm->current.pos.z);
  gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s16)palm->shape_angle.x,
             (s16)palm->shape_angle.y, (s16)palm->shape_angle.z);
  j3d = gabi::at<J3DModel>((u32)palm->palmModel);
  J3DModel_setBaseTRMtx(j3d, mDoMtx_stack_c::get());
  f32 collisionScale = read<f32>(0x101FDC0C);
  mDoMtx_stack_c::scaleM(collisionScale, 1.f, collisionScale);
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &palm->backgroundMatrix);
}
VERIFY(0x0236C598, lpalm_createInit);
s32 lpalm_create(daObjLpalm_c *palm) {
  WWHD_FUNC(0x0236C7C0, s32, palm);
  if (!(palm->actor_condition & 8)) {
    if (palm) {
      fopAc_ac_c_ct(palm);
      palm->__vtbl = 0x1002C144;
    }
    palm->actor_condition = palm->actor_condition | 8;
  }
  s32 phase = gabi::call<s32>(0x02520460, &palm->phase, ptr(0x1002C1DC));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, palm, ptr(0x0236C594), 0xF00))
      return 5;
    lpalm_createInit(palm);
  }
  return phase;
}
VERIFY(0x0236C7C0, lpalm_create);
BOOL lpalm_delete(daObjLpalm_c *palm) {
  WWHD_FUNC(0x0236C884, BOOL, palm);
  if (palm->heap && read<u32>(palm->background) < 0x100) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(play, 0x12A0), ptr(palm->background));
  }
  gabi::call(0x025204C8, &palm->phase, ptr(0x1002C1DC));
  return 1;
}
VERIFY(0x0236C884, lpalm_delete);
BOOL lpalm_execute(daObjLpalm_c *palm) {
  WWHD_FUNC(0x0236C8EC, BOOL, palm);
  gabi::Local<cXyz> up;
  up->set(0.f, 1.f, 0.f);
  gabi::Local<cXyz> wind;
  gabi::Local<cXyz> direction;
  gabi::Local<cXyz> normalized;
  gabi::Local<LpalmQuaternion> interpolated;
  gabi::call(0x025F1884, ptr(read<u32>(0x1018C7B0)),
             (s16) - (s16)palm->current.angle.y);
  u32 windVector = gabi::call<u32>(0x0257DAA8);
  gabi::call(0x0200FCD8, ptr(windVector), wind.get());
  f32 power = gabi::call<f32>(0x02578348);
  s16 angle = (s16)gabi::ftoi((f32)(1536.f * power));
  gabi::call(0x0201B080, up.get(), direction.get(), wind.get());
  f32 square = gabi::call<f32>(0x028E8DD0, direction.get());
  f64 magnitude = gabi::call<f64>(0x028F4384, (f64)square);
  if (magnitude < 8e-9) {
    identity(palm->targetRotation);
  } else {
    f32 sinAngle = sine(angle);
    gabi::call(0x0201B12C, direction.get(), normalized.get());
    f32 x = normalized->x, y = normalized->y, z = normalized->z;
    direction->set(x, y, z);
    palm->targetRotation.x = sinAngle * x;
    palm->targetRotation.y = sinAngle * y;
    palm->targetRotation.z = sinAngle * z;
    palm->targetRotation.w = cosine(angle);
  }
  gabi::call(0x028E9BC0, &palm->baseRotation, &palm->targetRotation,
             interpolated.get(), .25f);
  copyQuaternion(palm->baseRotation, *interpolated.get());
  s16 target = (s16)gabi::ftoi((f32)(power * 384.f));
  f32 waveStep = power * 2048.f;
  for (s32 i = 0; i < 2; i++) {
    s16 clamped = target;
    if (clamped > 0x100)
      clamped = 0x100;
    gabi::call(0x0200F428, &palm->bendAngle[i], clamped, 4, 0x20);
    f32 random = gabi::call<f32>(0x02019918, 128.f);
    s16 add = (s16)gabi::ftoi((f32)(waveStep + random));
    palm->waveAngle[i] = (s16)((u16)palm->waveAngle[i] + (u16)add);
    f32 weight = sine(palm->bendAngle[i]);
    palm->leafRotation[i].x = weight * sine(palm->waveAngle[i]);
    palm->leafRotation[i].y = 0.f;
    palm->leafRotation[i].z = weight * sine(palm->waveAngle[i]);
    palm->leafRotation[i].w = cosine(palm->bendAngle[i]);
  }
  gabi::call(0x024F43DC, ptr(palm->background));
  return 0;
}
VERIFY(0x0236C8EC, lpalm_execute);
BOOL lpalm_draw(daObjLpalm_c *palm) {
  WWHD_FUNC(0x0236CC18, BOOL, palm);
  auto light = dKy_getEnvlight();
  settingTevStruct(light, 1, &palm->current.pos, &palm->tevStr);
  light = dKy_getEnvlight();
  setLightTevColorType(light, gabi::at<J3DModel>((u32)palm->palmModel),
                       &palm->tevStr);
  u32 play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 0, read<u32>(play, 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 4, read<u32>(play, 0x5D74));
  mDoExt_modelUpdateDL(gabi::at<J3DModel>((u32)palm->palmModel), 0);
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 0, read<u32>(play, 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 4, read<u32>(play, 0x5D7C));
  return 1;
}
VERIFY(0x0236CC18, lpalm_draw);
void lpalm_staticInit() {
  WWHD_FUNC(0x0236CCB0, void);
  for (u32 o = 0; o < 16; o += 4)
    write<u32>(0x1046A4DC, o, 0);
  gabi::call(0x028F026C, ptr(0x101CA9F4));
  write<f32>(0x1046A4D0, 0, -3.1415927410125732f);
  write<f32>(0x1046A4D4, 0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046A4D8));
  gabi::call(0x028F026C, ptr(0x101CAA00));
  gabi::call(0x028EAB2C, ptr(0x1046A4D9));
  gabi::call(0x028F026C, ptr(0x101CAA0C));
}
VERIFY(0x0236CCB0, lpalm_staticInit);
void lpalm_staticDtor(void *object, u32 flags) {
  WWHD_FUNC(0x0236CD44, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x0236CD44, lpalm_staticDtor);
BOOL lpalm_IsDelete(daObjLpalm_c *palm) {
  WWHD_FUNC(0x0236CD58, BOOL, palm);
  return 1;
}
VERIFY(0x0236CD58, lpalm_IsDelete);
void lpalm_destructor(daObjLpalm_c *palm, u32 flags) {
  WWHD_FUNC(0x0236CD60, void, palm, flags);
  if (palm) {
    gabi::call(0x025D50BC, palm, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, palm);
  }
}
VERIFY(0x0236CD60, lpalm_destructor);
void lpalm_stringNoop(void *object) { WWHD_FUNC(0x0236CDB4, void, object); }
VERIFY(0x0236CDB4, lpalm_stringNoop);
