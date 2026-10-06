/* Generic ice block. HD adds actor-owned effect matrices and material uniforms.
 * Actual TU0235D460..0235EAFF includes leading RGBA conversion used in
 * create/draw. */
#include "d/actor/d_a_obj_ice.h"
#include "bindings.h"
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }
template <class T> void write(u32 a, u32 o, T v) {
  *gabi::at<be<T>>(a + o) = v;
}
void *ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
f32 cosine(s16 a) { return read<f32>(0x104A44FC, ((u16)a >> 3) * 8); }
f32 sine(s16 a) { return read<f32>(0x104A44F8, ((u16)a >> 3) * 8); }
struct IceColor {
  be<f32> r, g, b, a;
};
struct IceArchive {
  be<u32> name, vt;
};
u32 resource(s32 index) {
  gabi::Local<IceArchive> arc;
  arc->name = 0x1002B0EC;
  arc->vt = 0x1002B0F4;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), arc.get(),
                         index);
}
u32 play() { return gabi::call<u32>(0x025200D4); }
void setAction(daObjIce_c *ice, u32 fn) {
  ice->actionDelta = 0;
  ice->actionIndex = -1;
  ice->actionTarget = fn;
}
void particle(daObjIce_c *ice, u32 id, void *position, void *angle,
              void *scale) {
  u32 manager = read<u32>(play(), 0x5AB0);
  gabi::call(0x025A847C, ptr(manager), 0, id, position, angle, scale, 255,
             ptr(0), -1, ptr(gabi::ea(ice), 0x1A8), 0, 0);
}
u32 materialAt(u32 model, u16 i) {
  u32 data = read<u32>(model, 0xAC), material = read<u32>(data, 0x10);
  if (i < read<u32>(data, 0xC))
    material += i * 0x39C;
  return material;
}
void setRenderList(u32 first, u32 second) {
  write<u32>(0x104B4634, 0, read<u32>(play(), first));
  write<u32>(0x104B4634, 4, read<u32>(play(), second));
}
} // namespace
void ice_normalizeColor(IceColor *out, void *rgba) {
  WWHD_FUNC(0x0235D460, void, out, rgba);
  u32 a = gabi::ea(rgba);
  f32 r = read<u8>(a), g = read<u8>(a, 1), b = read<u8>(a, 2),
      alpha = read<u8>(a, 3);
  out->r = r / 255.f;
  out->g = g / 255.f;
  out->b = b / 255.f;
  out->a = alpha / 255.f;
}
VERIFY(0x0235D460, ice_normalizeColor);
u32 ice_parameter(daObjIce_c *ice, u32 width, u32 shift) {
  WWHD_FUNC(0x0235EAE4, u32, ice, width, shift);
  u32 mask = (width & 32) ? 0 : (1u << (width & 31));
  u32 value = (shift & 32) ? 0 : ((u32)ice->mParameters >> (shift & 31));
  return value & (mask - 1);
}
VERIFY(0x0235EAE4, ice_parameter);
u8 ice_checkAppear(daObjIce_c *ice) {
  WWHD_FUNC(0x0235D514, u8, ice);
  u32 sw = ice_parameter(ice, 8, 0);
  if (sw == 255)
    return 1;
  u32 save = read<u32>(0x101F84DC);
  return gabi::call<s32>(0x025BA0C0, ptr(save, 0x20), sw,
                         (s32)(s8)ice->home.roomNo)
             ? 0
             : 1;
}
VERIFY(0x0235D514, ice_checkAppear);
BOOL ice_createHeap(daObjIce_c *ice) {
  WWHD_FUNC(0x0235D588, BOOL, ice);
  u32 data = resource(4);
  if (!data) {
    gabi::call(0x0273AA24, ptr(0x1002B174), 395, ptr(0x1002B170));
    return 0;
  }
  ice->iceModel = gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x11000222);
  u32 collision = resource(7);
  ice->background =
      gabi::call<u32>(0x024F2478, ptr(collision), 1, &ice->backgroundMatrix);
  return ice->iceModel && ice->background;
}
VERIFY(0x0235D588, ice_createHeap);
BOOL ice_heapCallback(daObjIce_c *ice) {
  WWHD_FUNC(0x0235D660, BOOL, ice);
  return ice_createHeap(ice);
}
VERIFY(0x0235D660, ice_heapCallback);
void ice_setMatrix(daObjIce_c *ice) {
  WWHD_FUNC(0x0235D664, void, ice);
  mDoMtx_stack_c::transS(ice->current.pos.x, ice->current.pos.y,
                         ice->current.pos.z);
  gabi::call(0x025F19F8, mDoMtx_stack_c::get(), (s16)ice->shape_angle.x,
             (s16)ice->shape_angle.y, (s16)ice->shape_angle.z);
  mDoMtx_stack_c::scaleM(ice->width, ice->height, ice->width);
  J3DModel_setBaseTRMtx(gabi::at<J3DModel>((u32)ice->iceModel),
                        mDoMtx_stack_c::get());
  mDoMtx_stack_c::scaleM(ice->scale.x, ice->scale.y, ice->scale.z);
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &ice->backgroundMatrix);
}
VERIFY(0x0235D664, ice_setMatrix);
void ice_initMatrix(daObjIce_c *ice) {
  WWHD_FUNC(0x0235D754, void, ice);
  J3DModel_setBaseScale(gabi::at<J3DModel>((u32)ice->iceModel), &ice->scale);
  ice_setMatrix(ice);
}
VERIFY(0x0235D754, ice_initMatrix);
void ice_hitCallback(daObjIce_c *ice, void *collider, void *other,
                     void *otherCollider) {
  WWHD_FUNC(0x0235D774, void, ice, collider, other, otherCollider);
  u32 hit = gabi::call<u32>(0x02516300, collider);
  if (!hit)
    return;
  u32 col = gabi::ea(collider);
  s16 angle = (s16) - (s16)ice->shape_angle.y;
  f32 sin = sine(angle), cos = cosine(angle);
  f32 dx = read<f32>(col, 0xCC) - (f32)ice->current.pos.x,
      dz = read<f32>(col, 0xD4) - (f32)ice->current.pos.z;
  f32 x = gabi::fmadds(dx, cos, dz * sin), z = gabi::fmsubs(dz, cos, dx * sin),
      y = read<f32>(col, 0xD0) - (f32)ice->current.pos.y;
  ice->hitState = 0;
  for (u32 i = 0; i < 3; i++) {
    f32 radius = read<f32>(0x1046A068, i * 4),
        height = read<f32>(0x1046A074, i * 4);
    f32 rx = (radius * (f32)ice->scale.x) * (f32)ice->width,
        rz = (radius * (f32)ice->scale.z) * (f32)ice->width,
        hy = (height * (f32)ice->scale.y) * (f32)ice->height;
    if (y < hy && ((x * x) / (rx * rx) + (z * z) / (rz * rz)) < 1.f) {
      u32 type = read<u32>(hit, 0x10);
      if (type == 0x200 || type == 0x20000 || type == 0x40000) {
        ice->hitState = 2;
        write<u32>(col, 0x94, read<u32>(col, 0x94) | 2);
        return;
      }
      if (type != 0x20) {
        f32 width = (f32)ice->width - .0009f;
        if (width < 0.f)
          width = 0.f;
        ice->width = width;
        ice->height = 1.f - cosine((s16)gabi::ftoi(width * 16384.f));
        if (width < .1f)
          ice->hitState = 1;
        gabi::Local<csXyz> rot;
        gabi::call(0x0201A478, rot.get(), 0, 0, 0);
        rot->x = gabi::call<s16>(
            0x020195B0, (f32)ice->current.pos.y - read<f32>(col, 0xD0),
            read<f32>(col, 0xD4) - (f32)ice->current.pos.z);
        rot->y = gabi::call<s16>(
            0x020195B0, read<f32>(col, 0xCC) - (f32)ice->current.pos.x,
            read<f32>(col, 0xD4) - (f32)ice->current.pos.z);
        particle(ice, 0x465, ptr(col, 0xCC), rot.get(), ptr(0));
        if (ice->hitState) {
          write<u32>(col, 0x94, read<u32>(col, 0x94) | 2);
          return;
        }
      }
    }
  }
}
VERIFY(0x0235D774, ice_hitCallback);
namespace {
void updateUniform(u32 material, void *rgba) {
  gabi::Local<IceColor> normalized;
  ice_normalizeColor(normalized.get(), rgba);
  gabi::Local<IceColor> converted;
  gabi::call(0x0274D458, converted.get(), normalized.get(), 1.f);
  write<u32>(material, 0xA0, read<u32>(material, 0xA0) | 0x400);
  u32 uniform = gabi::call<u32>(0x027F9F0C, ptr(material, 0xA0), 10);
  f32 r = converted->r, g = converted->g, b = converted->b;
  f32 alpha = (f32)read<u8>(gabi::ea(rgba), 3) / 255.f;
  write<f32>(uniform, 0, r);
  write<f32>(uniform, 4, g);
  write<f32>(uniform, 8, b);
  write<f32>(uniform, 12, alpha);
}
void constructCollision(daObjIce_c *ice) {
  u32 a = gabi::ea(ice);
  fopAc_ac_c_ct(ice);
  ice->__vtbl = 0x1002B11C;
  gabi::call(0x0200BD2C, ice->collisionStatus);
  gabi::call(0x02515DA0, ptr(a, 0x3D4));
  write<u32>(a, 0x3D0, 0x1004AE88);
  write<u32>(a, 0x3D4, 0x1004AEC0);
  gabi::call(0x02515FB8, ice->cylinder);
  write<u32>(a, 0x508, 0x100015A8);
  write<u32>(a, 0x504, 0x1002B10C);
  gabi::call(0x02018590, ptr(a, 0x50C));
  write<u32>(a, 0x430, 0x1004B108);
  write<u32>(a, 0x520, 0x1004B150);
  write<u32>(a, 0x508, 0x1004B160);
  // Default matrix constructors do nothing for a valid embedded object.
  if (a + 0x578 == 0)
    gabi::call(0x0273AD10, 0x30);
  if (a + 0x5A8 == 0)
    gabi::call(0x0273AD10, 0x30);
}
} // namespace
s32 ice_create(daObjIce_c *ice) {
  WWHD_FUNC(0x0235DB5C, s32, ice);
  if (!(ice->actor_condition & 8)) {
    if (ice)
      constructCollision(ice);
    ice->actor_condition = ice->actor_condition | 8;
  }
  u32 a = gabi::ea(ice);
  s32 phase = 5;
  if (read<u8>(a, 0xC) == 0)
    ice->appears = ice_checkAppear(ice);
  if (!ice->appears)
    return phase;
  phase = gabi::call<s32>(0x02520460, &ice->phase, ptr(0x1002B0EC));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, ice, ptr(0x0235D660), 0x1A20))
    return 5;
  if (gabi::call<s32>(0x024EEA6C, ptr(play(), 0x12A0), ptr(ice->background),
                      ice))
    return 5;
  u32 model = ice->iceModel;
  ice->cullMtx = model ? model + 0xC8 : 0;
  ice->width = 1.f;
  ice->height = 1.f;
  ice_initMatrix(ice);
  f32 radiusScale = ice->scale.x;
  if (radiusScale < (f32)ice->scale.z)
    radiusScale = ice->scale.z;
  gabi::call(0x02515F14, ice->collisionStatus, 255, 255, ice);
  gabi::call(0x02516518, ice->cylinder, ptr(0x1002B12C));
  write<u32>(a, 0x438, a + 0x3B8);
  gabi::call(0x020182E0, ptr(a, 0x50C), &ice->current.pos);
  gabi::call(0x020184DC, ptr(a, 0x50C), read<f32>(0x1046A044) * radiusScale);
  gabi::call(0x02018428, ptr(a, 0x50C),
             read<f32>(0x1046A048) * (f32)ice->scale.y);
  write<u32>(a, 0x40C, read<u32>(a, 0x40C) | 0x10);
  write<u32>(a, 0x490, 0x0235D774);
  model = ice->iceModel;
  for (s32 i = 0; i < read<u16>(model, 0x2A); i++) {
    u32 material = materialAt(model, (u16)i),
        materials = read<u32>(model, 0x34);
    gabi::Local<u8[4]> color;
    gabi::call(0x027FA3F4, color.get(), ptr(materials, i * 0x3C), 3);
    u32 block = read<u32>(material, 0x18), vt = read<u32>(block, 4);
    gabi::call_ptr(read<u32>(vt, 0x3C), ptr(block), 3, color.get());
    updateUniform(material, color.get());
    model = ice->iceModel;
  }
  ice->alpha = 255.f;
  gabi::call(0x025D674C, ice, -80.f * (f32)ice->scale.x,
             0.f * (f32)ice->scale.y, -80.f * (f32)ice->scale.z,
             80.f * (f32)ice->scale.x, 150.f * (f32)ice->scale.y,
             80.f * (f32)ice->scale.z);
  f32 x = ice->current.pos.x,
      y = gabi::fmadds(60.f, ice->scale.y, ice->current.pos.y),
      z = ice->current.pos.z;
  ice->eyePos.set(x, y, z);
  write<f32>(a, 0x390, x);
  write<f32>(a, 0x394, y);
  write<f32>(a, 0x398, z);
  setAction(ice, 0x0235E6C4);
  return phase;
}
VERIFY(0x0235DB5C, ice_create);
s32 ice_Create(daObjIce_c *ice) {
  WWHD_FUNC(0x0235DF6C, s32, ice);
  return ice_create(ice);
}
VERIFY(0x0235DF6C, ice_Create);
BOOL ice_delete(daObjIce_c *ice) {
  WWHD_FUNC(0x0235DF70, BOOL, ice);
  if (ice->appears) {
    gabi::call(0x025204C8, &ice->phase, ptr(0x1002B0EC));
    if (ice->heap && ice->background) {
      if (read<u32>(ice->background) < 0x100)
        gabi::call(0x020087EC, ptr(play(), 0x12A0), ptr(ice->background));
      ice->background = 0;
    }
  }
  return 1;
}
VERIFY(0x0235DF70, ice_delete);
BOOL ice_Delete(daObjIce_c *ice) {
  WWHD_FUNC(0x0235DFF4, BOOL, ice);
  return ice_delete(ice);
}
VERIFY(0x0235DFF4, ice_Delete);
BOOL ice_execute(daObjIce_c *ice) {
  WWHD_FUNC(0x0235DFF8, BOOL, ice);
  u32 a = gabi::ea(ice);
  gabi::call(0x020184DC, ptr(a, 0x50C),
             (75.f * (f32)ice->scale.x) * (f32)ice->width);
  gabi::call(0x02018428, ptr(a, 0x50C),
             (120.f * (f32)ice->scale.y) * (f32)ice->height);
  ice_setMatrix(ice);
  u32 bg = ice->background;
  if (bg && read<u32>(bg) < 0x100)
    gabi::call(0x024F43DC, ptr(bg));
  ptmf_call(a + 0x558, ice);
  return 1;
}
VERIFY(0x0235DFF8, ice_execute);
BOOL ice_Execute(daObjIce_c *ice) {
  WWHD_FUNC(0x0235E0CC, BOOL, ice);
  return ice_execute(ice);
}
VERIFY(0x0235E0CC, ice_Execute);
void ice_wait(daObjIce_c *ice) {
  WWHD_FUNC(0x0235E6C4, void, ice);
  u32 a = gabi::ea(ice);
  if (!gabi::call<s32>(0x025162A4, ice->cylinder) ||
      read<u32>(a, 0x3E8) != read<u32>(a, 0x3EC))
    write<u32>(a, 0x444, read<u32>(a, 0x444) & ~1u);
  gabi::call(0x02515E50, ptr(a, 0x3D4));
  gabi::call(0x0251621C, ice->cylinder);
  s32 state = ice->hitState;
  if (state == 2) {
    s32 reverb = gabi::call<s32>(0x02520540, (s32)(s8)ice->current.roomNo);
    gabi::call(0x025E1A40, 0x6A16, &ice->current.pos, 0, reverb);
  }
  if (state == 1 || state == 2) {
    ice->fadeTimer = 90;
    u32 manager = read<u32>(play(), 0x5AB0);
    u32 emitter = gabi::call<u32>(0x025A847C, ptr(manager), 0, 0x464,
                                  &ice->current.pos, ptr(0), &ice->scale, 255,
                                  ptr(0), -1, ptr(a, 0x1A8), 0, 0);
    if (emitter) {
      f32 x = ice->scale.x, y = ice->scale.y, z = ice->scale.z;
      write<f32>(emitter, 0x220, x);
      write<f32>(emitter, 0x224, y);
      write<f32>(emitter, 0x228, z);
      x = ice->scale.x;
      write<f32>(emitter, 0x238, x);
      write<f32>(emitter, 0x23C, x);
      write<f32>(emitter, 0x240, x);
    }
    setAction(ice, 0x0235E820);
  } else
    gabi::call(0x0200E240, ptr(play(), 0x26A4), ice->cylinder);
}
VERIFY(0x0235E6C4, ice_wait);
void ice_fade(daObjIce_c *ice) {
  WWHD_FUNC(0x0235E820, void, ice);
  f32 alpha = (f32)ice->alpha - read<f32>(0x1046A050),
      width = (f32)ice->width - read<f32>(0x1046A04C);
  ice->alpha = alpha;
  if (alpha < 0.f)
    ice->alpha = 0.f;
  width = width >= 0.f ? width : 0.f;
  ice->width = width;
  ice->height = 1.f - cosine((s16)gabi::ftoi(width * 16384.f));
  if (width < .4f && ice->background && read<u32>(ice->background) < 0x100) {
    gabi::call(0x020087EC, ptr(play(), 0x12A0), ptr(ice->background));
    u32 sw = ice_parameter(ice, 8, 0);
    if (sw != 255) {
      u32 save = read<u32>(0x101F84DC);
      gabi::call(0x025B9E38, ptr(save, 0x20), sw, (s32)(s8)ice->home.roomNo);
    }
  }
  ice->fadeTimer = (s32)((u32)ice->fadeTimer - 1);
  if (ice->fadeTimer <= 0)
    gabi::call(0x025D57E0, ice);
}
VERIFY(0x0235E820, ice_fade);
namespace {
void setAlpha(u32 data, u8 alpha) {
  u16 i = 0;
  u32 header = gabi::call<u32>(0x027F3F94, ptr(data));
  while (i < read<u16>(header, 8)) {
    u32 joint = read<u32>(data, 8);
    if (i < read<u32>(data, 4))
      joint += i * 0x1C;
    u32 material = read<u32>(joint, 0x10);
    while (material) {
      u32 shape = read<u32>(material, 8);
      write<u8>(shape, 4, alpha ? 1 : 0);
      if (alpha) {
        u32 block = read<u32>(material, 0x18), vt = read<u32>(block, 4);
        u32 color = gabi::call_ptr<u32>(read<u32>(vt, 0x4C), ptr(block), 3);
        write<u8>(color, 3, alpha);
        block = read<u32>(material, 0x18);
        vt = read<u32>(block, 4);
        color = gabi::call_ptr<u32>(read<u32>(vt, 0x4C), ptr(block), 3);
        block = read<u32>(material, 0x18);
        vt = read<u32>(block, 4);
        gabi::call_ptr(read<u32>(vt, 0x3C), ptr(block), 3, ptr(color));
        updateUniform(material, ptr(color));
      }
      material = read<u32>(material, 4);
    }
    i = (u16)(i + 1);
    header = gabi::call<u32>(0x027F3F94, ptr(data));
  }
}
u32 shaderVariableTable(u32 header) {
  s32 relative = read<s32>(header, 0x34);
  return relative ? header + 0x34 + relative : 0;
}
void dirtyVariable(u32 entry, s32 index, u32 variable) {
  if (read<s32>(variable, 4) < 0)
    return;
  write<u16>(entry, 4, read<u16>(entry, 4) | 4);
  u32 words = read<u32>(entry, 0xC) + (u32)(index >> 5) * 4;
  write<u32>(words, 0, read<u32>(words) | (1u << ((u32)index & 31)));
}
void effectMatrices(daObjIce_c *ice) {
  if (!read<u32>(0x101FDBD4)) {
    write<u32>(0x101FDBD4, 0, 1);
    memcpy_g(ptr(0x101FDBD8), ptr(0x101CA364), 48);
  }
  f32 sx = 1.8f / (f32)ice->scale.x, sy = 1.8f / (f32)ice->scale.y;
  u32 camera = gabi::call<u32>(0x024F8020);
  gabi::Local<cXyz> view, light, half;
  gabi::Local<Mtx34> look;
  gabi::call(0x0201ADE0, &ice->eyePos, view.get(), ptr(camera, 0xDC));
  gabi::call(0x02563F64, ptr(gabi::ea(ice), 0x194), &ice->eyePos, light.get());
  gabi::call(0x028E9E88, view.get(), light.get(), half.get());
  gabi::call(0x028E9684, look.get(), ptr(0x101FFBA8), ptr(0x101FFBC0),
             half.get());
  Mtx34 *matrix = mDoMtx_stack_c::get();
  gabi::call(0x028E945C, matrix, sx, sy, 1.f);
  gabi::call(0x028E9108, matrix, ptr(0x101FDBD8), matrix);
  gabi::call(0x028E9108, matrix, look.get(), matrix);
  matrix->m[0][3] = 0.f;
  matrix->m[1][3] = 0.f;
  matrix->m[2][3] = 0.f;
  gabi::call(0x028E90D4, matrix, &ice->effectLarge);
  gabi::call(0x028E945C, matrix, .5f, .5f, 1.f);
  gabi::call(0x028E9108, matrix, ptr(0x101FDBD8), matrix);
  gabi::call(0x028E9108, matrix, look.get(), matrix);
  matrix->m[0][3] = 0.f;
  matrix->m[1][3] = 0.f;
  matrix->m[2][3] = 0.f;
  gabi::call(0x028E90D4, matrix, &ice->effectSmall);
  u32 model = ice->iceModel;
  u16 count = read<u16>(model, 0x2A);
  for (u16 i = 0; i < count; i++) {
    u32 entry = read<u32>(model, 0x34) + i * 0x3C;
    for (u32 j = 0; j < 8; j++) {
      gabi::Local<be<s32>> index;
      *index.get() = -1;
      u32 info = gabi::call<u32>(0x027FA974, ptr(entry), j, index.get());
      if (!info)
        continue;
      u32 type = read<u32>(info);
      if (type < 10 || type > 11)
        continue;
      u32 header = read<u32>(entry), variables = shaderVariableTable(header);
      s32 n = *index.get();
      u32 variable = variables + (u32)n * 0x14;
      dirtyVariable(entry, n, variable);
      header = read<u32>(entry);
      variables = shaderVariableTable(header);
      u16 linked = read<u16>(variable, 0xC);
      dirtyVariable(entry, linked, variables + linked * 0x14);
      u32 destination = gabi::call<u32>(0x027FA678, ptr(entry), j);
      if (destination && read<u32>(destination)) {
        if (i == 0)
          write<u32>(destination, 0, gabi::ea(&ice->effectSmall));
        else if (i == 1)
          write<u32>(destination, 0, gabi::ea(&ice->effectLarge));
      }
    }
  }
}
} // namespace
BOOL ice_draw(daObjIce_c *ice) {
  WWHD_FUNC(0x0235E0D0, BOOL, ice);
  auto light = dKy_getEnvlight();
  settingTevStruct(light, 0, &ice->current.pos, &ice->tevStr);
  light = dKy_getEnvlight();
  setLightTevColorType(light, gabi::at<J3DModel>((u32)ice->iceModel),
                       &ice->tevStr);
  gabi::call(0x02543F10, ptr(play(), 0x52C4), ptr(0x1002B1B8), 255);
  if (read<u8>(play(), 0x5292) &&
      gabi::call<s32>(0x025445B8, ptr(play(), 0x52C4), ptr(0x1002B1B8)))
    setRenderList(0x5D78, 0x5D7C);
  else
    setRenderList(0x5D84, 0x5D88);
  u32 data = read<u32>(ice->iceModel, 0xAC);
  u8 alpha = (u8)gabi::ftoi(ice->alpha);
  setAlpha(data, alpha);
  effectMatrices(ice);
  mDoExt_modelUpdateDL(gabi::at<J3DModel>((u32)ice->iceModel), 0);
  setRenderList(0x5D78, 0x5D7C);
  return 1;
}
VERIFY(0x0235E0D0, ice_draw);
BOOL ice_Draw(daObjIce_c *ice) {
  WWHD_FUNC(0x0235E6B8, BOOL, ice);
  return ice_draw(ice);
}
VERIFY(0x0235E6B8, ice_Draw);
BOOL ice_IsDelete(daObjIce_c *ice) {
  WWHD_FUNC(0x0235E6BC, BOOL, ice);
  return 1;
}
VERIFY(0x0235E6BC, ice_IsDelete);
void ice_staticInit() {
  WWHD_FUNC(0x0235E94C, void);
  for (u32 i = 0; i < 16; i += 4)
    write<u32>(0x1046A058, i, 0);
  gabi::call(0x028F026C, ptr(0x101CA3B4));
  write<f32>(0x1046A03C, 0, -3.1415927410125732f);
  write<f32>(0x1046A040, 0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046A054));
  gabi::call(0x028F026C, ptr(0x101CA3C0));
  gabi::call(0x028EAB2C, ptr(0x1046A055));
  gabi::call(0x028F026C, ptr(0x101CA3CC));
  write<f32>(0x1046A068, 0, 40.f);
  write<f32>(0x1046A068, 4, 65.f);
  write<f32>(0x1046A068, 8, 75.f);
  write<f32>(0x1046A074, 0, 120.f);
  write<f32>(0x1046A074, 4, 90.f);
  write<f32>(0x1046A074, 8, 60.f);
  write<f32>(0x1046A044, 0, 75.f);
  write<f32>(0x1046A048, 0, 120.f);
  write<f32>(0x1046A04C, 0, .011111111f);
  write<f32>(0x1046A050, 0, 2.8333333f);
}
VERIFY(0x0235E94C, ice_staticInit);
void ice_staticDtor(void *object, u32 flags) {
  WWHD_FUNC(0x0235EA60, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x0235EA60, ice_staticDtor);
void ice_destructor(daObjIce_c *ice, u32 flags) {
  WWHD_FUNC(0x0235EA74, void, ice, flags);
  if (ice) {
    gabi::call(0x02515A70, ice->cylinder, 2);
    gabi::call(0x02515860, ice->collisionStatus, 2);
    gabi::call(0x025D50BC, ice, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, ice);
  }
}
VERIFY(0x0235EA74, ice_destructor);
void ice_stringNoop(void *object) { WWHD_FUNC(0x0235EAE0, void, object); }
VERIFY(0x0235EAE0, ice_stringNoop);
