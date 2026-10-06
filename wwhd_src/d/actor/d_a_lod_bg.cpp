// Actual HD TU021B43BC..021B5A33. HD profile101B9090, size3F8.
#include "d/actor/d_a_lod_bg.h"
#include "bindings.h"
using Lod = daLodbg_c;
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }
template <class T> void write(u32 a, u32 o, T v) {
  *gabi::at<be<T>>(a + o) = v;
}
void *ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
u32 play() { return gabi::call<u32>(0x025200D4); }
s32 eventOn(u32 id) {
  return gabi::call<s32>(0x025B8B94, ptr(read<u32>(0x101F84DC), 0x644), id);
}
u32 virtualTarget(u32 self, u32 vtOffset, u32 slot) {
  return read<u32>(read<u32>(self, vtOffset), slot);
}
u32 actionTarget(Lod *l, u32 &self) {
  self = gabi::ea(l) + (s32)(s16)l->action.adjustment;
  s16 slot = l->action.slot;
  return slot < 0
             ? (u32)l->action.target
             : read<u32>(read<u32>(self, (s32)read<s16>(gabi::ea(l), 0x3DE)),
                         (u32)slot * 8 + 4);
}
void setAction(Lod *l, u32 target) {
  l->action.adjustment = 0;
  l->action.slot = -1;
  l->action.target = target;
}
void nameAccess(Lod *l) {
  u32 a = gabi::ea(l) + 0x3AC;
  gabi::call<void>(virtualTarget(a, 4, 0x14), ptr(a));
}
void copyMatrix(u32 dst) {
  f32 values[12];
  for (u32 i = 0; i < 12; i++)
    values[i] = read<f32>(0x1048D0CC, i * 4);
  for (u32 i = 0; i < 12; i++)
    write<f32>(dst, 0xC8 + i * 4, values[i]);
}
void drawModel(Lod *l, u32 slot) {
  u32 a = gabi::ea(l);
  gabi::call<void>(0x027F4D5C, ptr(read<u32>(a, slot)));
  gabi::call<void>(0x02838524, ptr(0x1048CFF0), ptr(read<u32>(a, slot)));
  gabi::call<void>(0x025E2E5C, ptr(read<u32>(a, slot)));
}
} // namespace
BOOL lodbg_createHeap(Lod *l) {
  WWHD_FUNC(0x021B43BC, BOOL, l);
  u32 data = l->modelData;
  if (!data) {
    gabi::call<void>(0x0273AA24, ptr(0x10013F6C), 0xB5, ptr(0x10013F58));
    data = l->modelData;
  }
  l->model = gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x11000022);
  if (!l->model)
    return 0;
  u32 room = read<u32>(gabi::ea(l), 0xB0);
  if (room == 11) {
    data = l->modelData2;
    if (!data) {
      gabi::call<void>(0x0273AA24, ptr(0x10013F6C), 0xC0, ptr(0x10013F7C));
      data = l->modelData2;
    }
    for (u32 i = 0; i < 2; i++) {
      l->model2[i] =
          gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x11000022);
      if (!l->model2[i]) {
        l->model = 0;
        l->model2[0] = 0;
        return 0;
      }
      if (i == 0)
        data = l->modelData2;
    }
  } else if (room == 1 || room == 13) {
    data = l->modelData2;
    if (data) {
      l->model2[0] =
          gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x11000022);
      if (!l->model2[0]) {
        l->model = 0;
        return 0;
      }
    }
  }
  return 1;
}
VERIFY(0x021B43BC, lodbg_createHeap);
BOOL lodbg_heapCallback(Lod *l) {
  WWHD_FUNC(0x021B4524, BOOL, l);
  return gabi::call<BOOL>(0x021B43BC, l);
}
VERIFY(0x021B4524, lodbg_heapCallback);
struct ColorF {
  be<f32> r, g, b, a;
};
void lodbg_colorConvert(ColorF *out, void *input) {
  WWHD_FUNC(0x021B4528, void, out, input);
  u32 a = gabi::ea(input);
  u8 r = read<u8>(a), g = read<u8>(a, 1), b = read<u8>(a, 2),
     alpha = read<u8>(a, 3);
  out->r = (f32)r / 255.0f;
  out->g = (f32)g / 255.0f;
  out->b = (f32)b / 255.0f;
  out->a = (f32)alpha / 255.0f;
}
VERIFY(0x021B4528, lodbg_colorConvert);
BOOL lodbg_execute(Lod *l) {
  WWHD_FUNC(0x021B4C6C, BOOL, l);
  u32 self;
  u32 target = actionTarget(l, self);
  return gabi::call<BOOL>(target, ptr(self));
}
VERIFY(0x021B4C6C, lodbg_execute);
BOOL lodbg_deleteResources(Lod *l) {
  WWHD_FUNC(0x021B4CB0, BOOL, l);
  if (l->resourceActive) {
    nameAccess(l);
    if (gabi::call<s32>(0x02523D08, ptr(l->resource.text)) > 0)
      return 0;
    nameAccess(l);
    gabi::call<void>(0x02524180, ptr(l->resource.text));
    l->resourceActive = 0;
    gabi::call<void>(0x025D6134, l);
    l->modelData = 0;
    l->model2[1] = 0;
    l->modelData2 = 0;
    l->drawModel2 = 0;
    l->model2[0] = 0;
    l->model = 0;
    l->alpha = 0;
  }
  return 1;
}
VERIFY(0x021B4CB0, lodbg_deleteResources);
BOOL lodbg_delete(Lod *l) {
  WWHD_FUNC(0x021B4D78, BOOL, l);
  return gabi::call<BOOL>(0x021B4CB0, l);
}
VERIFY(0x021B4D78, lodbg_delete);
BOOL lodbg_deleteWrapper(Lod *l) {
  WWHD_FUNC(0x021B4D7C, BOOL, l);
  return gabi::call<BOOL>(0x021B4D78, l);
}
VERIFY(0x021B4D7C, lodbg_deleteWrapper);
Lod *lodbg_constructor(Lod *l) {
  WWHD_FUNC(0x021B4D80, Lod *, l);
  if (!l) {
    l = gabi::at<Lod>(gabi::call<u32>(0x0273AD10, 0x3F8));
    if (!l)
      return l;
  }
  gabi::call<void>(0x025D4ED0, l);
  u32 a = gabi::ea(l);
  write<u32>(a, 0xB4, 0x10014004);
  u32 name = a + 0x3AC; // Embedded storage is non-null for a valid actor.
  write<u32>(name, 0, name + 0xC);
  write<u32>(name, 4, 0x10013FBC);
  write<u32>(name, 8, 0x20);
  write<u8>(name, 0x2B, 0);
  write<u32>(name, 4, 0x10013FD4);
  write<u8>(read<u32>(name), 0, 0);
  write<u32>(name, 4, 0x10013FEC);
  gabi::call<void>(0x02759C28, ptr(name), ptr(0x10014020), read<u32>(a, 0xB0));
  f32 scaled = read<f32>(a, 0x330) * 20000.0f;
  setAction(l, 0x021B5010);
  s32 room = (s8)read<u8>(a, 0xB3);
  write<f32>(a, 0x330, scaled);
  write<f32>(a, 0x338, scaled + 20000.0f);
  gabi::call<void>(0x0255FFF4, ptr(a, 0x110), room, 255);
  return l;
}
VERIFY(0x021B4D80, lodbg_constructor);
BOOL lodbg_createStep(Lod *l) {
  WWHD_FUNC(0x021B4EB8, BOOL, l);
  s32 room = (s8)read<u8>(play(), 0x513E);
  if ((u32)room == read<u32>(gabi::ea(l), 0xB0))
    return 4;
  u32 self;
  u32 target = actionTarget(l, self);
  gabi::call<void>(target, ptr(self));
  s16 slot = l->action.slot;
  if (slot != -1)
    return 4;
  if (slot == 0 ||
      ((s16)l->action.adjustment == 0 && (u32)l->action.target == 0x021B5224))
    return 0;
  return 4;
}
VERIFY(0x021B4EB8, lodbg_createStep);
BOOL lodbg_create(Lod *l) {
  WWHD_FUNC(0x021B4F90, BOOL, l);
  if (!read<u32>(play(), 0x5B2C))
    return 0;
  u32 a = gabi::ea(l), flags = read<u32>(a, 0x2E4);
  if (!(flags & 8)) {
    if (l) {
      gabi::call<void>(0x021B4D80, l);
      flags = read<u32>(a, 0x2E4);
    }
    write<u32>(a, 0x2E4, flags | 8);
  }
  return gabi::call<BOOL>(0x021B4EB8, l);
}
VERIFY(0x021B4F90, lodbg_create);
BOOL lodbg_createWait(Lod *l) {
  WWHD_FUNC(0x021B5010, BOOL, l);
  u32 player = read<u32>(play(), 0x5B2C);
  f32 distance = gabi::call<f32>(0x025D6958, l, ptr(player));
  if (distance > read<f32>(gabi::ea(l), 0x330))
    return 1;
  nameAccess(l);
  if (!gabi::call<s32>(0x02523A04, ptr(l->resource.text), 0)) {
    nameAccess(l);
    return 1;
  }
  l->resourceActive = 1;
  setAction(l, 0x021B5224);
  return 1;
}
VERIFY(0x021B5010, lodbg_createWait);
BOOL lodbg_createModelData(Lod *l) {
  WWHD_FUNC(0x021B50C4, BOOL, l);
  nameAccess(l);
  l->modelData =
      gabi::call<u32>(0x0252447C, ptr(l->resource.text), ptr(0x10014034));
  if (!l->modelData)
    return 0;
  u32 room = read<u32>(gabi::ea(l), 0xB0);
  if (room == 11) {
    nameAccess(l);
    l->modelData2 =
        gabi::call<u32>(0x0252447C, ptr(l->resource.text), ptr(0x10014028));
    return l->modelData2 != 0;
  }
  if ((room == 1 && !eventOn(0x1820)) || (room == 13 && !eventOn(0x3908))) {
    nameAccess(l);
    l->modelData2 =
        gabi::call<u32>(0x0252447C, ptr(l->resource.text), ptr(0x10014040));
    return l->modelData2 != 0;
  }
  return 1;
}
VERIFY(0x021B50C4, lodbg_createModelData);
BOOL lodbg_readWait(Lod *l) {
  WWHD_FUNC(0x021B5224, BOOL, l);
  nameAccess(l);
  s32 state = gabi::call<s32>(0x02523D08, ptr(l->resource.text));
  if (state > 0)
    return 1;
  setAction(l, 0x021B5340);
  if (state < 0)
    return 1;
  if (!gabi::call<BOOL>(0x021B50C4, l) ||
      !gabi::call<BOOL>(0x025D63E8, l, ptr(0x021B4524), 0))
    return 1;
  u32 self;
  u32 target = actionTarget(l, self);
  gabi::call<void>(target, ptr(self));
  u32 model = l->model;
  write<u32>(gabi::ea(l), 0x348, model ? model + 0xC8 : 0);
  return 1;
}
VERIFY(0x021B5224, lodbg_readWait);
void lodbg_staticInit() {
  WWHD_FUNC(0x021B58D8, void);
  write<u32>(0x10464E40, 8, 0);
  write<u32>(0x10464E40, 0, 0);
  write<u32>(0x10464E40, 12, 0);
  write<u32>(0x10464E40, 4, 0);
  gabi::call<void>(0x028F026C, ptr(0x101B906C));
  write<f32>(0x10464E34, 0, -3.1415927410125732f);
  write<f32>(0x10464E38, 0, 3.1415927410125732f);
  gabi::call<void>(0x028ED6F8, ptr(0x10464E3C));
  gabi::call<void>(0x028F026C, ptr(0x101B9078));
  gabi::call<void>(0x028EAB2C, ptr(0x10464E3D));
  gabi::call<void>(0x028F026C, ptr(0x101B9084));
}
VERIFY(0x021B58D8, lodbg_staticInit);
BOOL lodbg_isDelete(Lod *l) {
  WWHD_FUNC(0x021B596C, BOOL, l);
  return 1;
}
VERIFY(0x021B596C, lodbg_isDelete);
void lodbg_terminateName(void *obj) {
  WWHD_FUNC(0x021B5974, void, obj);
  u32 a = gabi::ea(obj);
  write<u8>(read<u32>(a) + read<u32>(a, 8) - 1, 0, 0);
}
VERIFY(0x021B5974, lodbg_terminateName);
void lodbg_safeStringDestructor(void *obj, s32 flags) {
  WWHD_FUNC(0x021B598C, void, obj, flags);
  if (obj && (flags & 1))
    gabi::call<void>(0x0273AF40, obj);
}
VERIFY(0x021B598C, lodbg_safeStringDestructor);
void lodbg_bufferDestructor(void *obj, s32 flags) {
  WWHD_FUNC(0x021B59A0, void, obj, flags);
  if (obj && (flags & 1))
    gabi::call<void>(0x0273AF40, obj);
}
VERIFY(0x021B59A0, lodbg_bufferDestructor);
void lodbg_actorDestructor(Lod *l, s32 flags) {
  WWHD_FUNC(0x021B59B4, void, l, flags);
  if (l) {
    gabi::call<void>(0x025D50BC, l, 0);
    if (flags & 1)
      gabi::call<void>(0x0273AF40, l);
  }
}
VERIFY(0x021B59B4, lodbg_actorDestructor);
void lodbg_baseNoop(void *obj) { WWHD_FUNC(0x021B5A08, void, obj); }
VERIFY(0x021B5A08, lodbg_baseNoop);
void lodbg_fixedStringDestructor(void *obj, s32 flags) {
  WWHD_FUNC(0x021B5A0C, void, obj, flags);
  if (obj && (flags & 1))
    gabi::call<void>(0x0273AF40, obj);
}
VERIFY(0x021B5A0C, lodbg_fixedStringDestructor);
void lodbg_resourceNameDestructor(void *obj, s32 flags) {
  WWHD_FUNC(0x021B5A20, void, obj, flags);
  if (obj && (flags & 1))
    gabi::call<void>(0x0273AF40, obj);
}
VERIFY(0x021B5A20, lodbg_resourceNameDestructor);
namespace {
void updateMaterials(Lod *l, u32 data) {
  gabi::Local<ColorF> normalized, tinted;
  u16 i = 0;
  u32 info = gabi::call<u32>(0x027F3F8C, ptr(data));
  while (i < read<u16>(info, 0x24)) {
    u32 material = read<u32>(data, 0x10);
    if ((u32)i < read<u32>(data, 0xC))
      material += (u32)i * 0x39C;
    u32 block = read<u32>(material, 0x18);
    u32 color = gabi::call<u32>(virtualTarget(block, 4, 0x4C), ptr(block), 3);
    write<u8>(color, 3, l->alpha);
    block = read<u32>(material, 0x18);
    color = gabi::call<u32>(virtualTarget(block, 4, 0x4C), ptr(block), 3);
    block = read<u32>(material, 0x18);
    gabi::call<void>(virtualTarget(block, 4, 0x3C), ptr(block), 3, ptr(color));
    gabi::call<void>(0x021B4528, normalized.get(), ptr(color));
    gabi::call<void>(0x0274D458, tinted.get(), normalized.get(), 1.0f);
    write<u32>(material, 0xA0, read<u32>(material, 0xA0) | 0x400);
    u32 out = gabi::call<u32>(0x027F9F0C, ptr(material, 0xA0), 10);
    f32 alpha = (f32)read<u8>(color, 3) / 255.0f;
    f32 red = tinted->r, green = tinted->g, blue = tinted->b;
    write<f32>(out, 4, green);
    write<f32>(out, 8, blue);
    write<f32>(out, 0, red);
    write<f32>(out, 12, alpha);
    i = (u16)(i + 1);
    info = gabi::call<u32>(0x027F3F8C, ptr(data));
  }
}
} // namespace
BOOL lodbg_draw(Lod *l) {
  WWHD_FUNC(0x021B45DC, BOOL, l);
  u32 a = gabi::ea(l);
  if (!read<u32>(a, 0xF4) || !l->alpha)
    return 1;
  u32 room = read<u32>(a, 0xB0);
  if (room == 26 && !eventOn(0x1E40))
    return 1;
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x025626A4, ptr(env), 1, (void *)nullptr, ptr(a, 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x02562F5C, ptr(env), ptr(l->model), ptr(a, 0x110));
  updateMaterials(l, read<u32>(l->model, 0xAC));
  write<f32>(0x1048CFF0, 0x54, 500000.0f);
  gabi::call<void>(0x0283801C, ptr(0x1048CFF0));
  drawModel(l, 0x3E0);
  if (l->model2[0] && l->drawModel2) {
    if (room == 11) {
      updateMaterials(l, read<u32>(l->model2[0], 0xAC));
      for (u32 i = 0; i < 2; i++)
        drawModel(l, 0x3E4 + i * 4);
    } else if (room == 1 || room == 13) {
      env = gabi::call<u32>(0x02555D0C);
      gabi::call<void>(0x02562F5C, ptr(env), ptr(l->model2[0]), ptr(a, 0x110));
      updateMaterials(l, read<u32>(l->model2[0], 0xAC));
      drawModel(l, 0x3E4);
    }
  }
  write<f32>(0x1048CFF0, 0x54, read<f32>(0x1048D04C));
  gabi::call<void>(0x0283801C, ptr(0x1048CFF0));
  return 1;
}
VERIFY(0x021B45DC, lodbg_draw);
BOOL lodbg_drawWrapper(Lod *l) {
  WWHD_FUNC(0x021B4C68, BOOL, l);
  return gabi::call<BOOL>(0x021B45DC, l);
}
VERIFY(0x021B4C68, lodbg_drawWrapper);
BOOL lodbg_deleteWait(Lod *l) {
  WWHD_FUNC(0x021B5340, BOOL, l);
  u32 a = gabi::ea(l);
  u32 room = 0;
  if (read<u32>(a, 0xF4)) {
    u32 view = read<u32>(play(), 0x5D08);
    gabi::Local<cXyz> delta, horizontal;
    gabi::call<void>(0x0201ADE0, ptr(view, 0xDC), delta.get(), ptr(a, 0x314));
    horizontal->y = 0;
    horizontal->x = delta->x;
    horizontal->z = delta->z;
    f64 square = gabi::call<f64>(0x028E8DD0, horizontal.get());
    f32 distance = (f32)(gabi::call<f64>(0x028F4384, square) + 700.0);
    if (distance < read<f32>(a, 0x338) + 100000.0f) {
      room = read<u32>(a, 0xB0);
      if (distance > read<f32>(a, 0x330) + 100000.0f) {
        l->alpha = 0;
        return 1;
      }
      play();
      u32 roomState = 0x1047E6CC + room * 0x22C;
      if (read<u8>(roomState, 0x21C) & 1) {
        play();
        if (!(read<u8>(roomState, 0x21C) & 4)) {
          l->alpha = 0;
          return 1;
        }
      }
      f32 y = 150000.0f - distance;
      l->alpha = 255;
      if (y < 0)
        y *= 0.1f;
      else
        y = 0;
      gabi::call<void>(0x028E93CC, ptr(0x1048D0CC), read<f32>(a, 0x314),
                       read<f32>(a, 0x318) + y, read<f32>(a, 0x31C));
      gabi::call<void>(0x025F1C28, ptr(0x1048D0CC), (s32)read<s16>(a, 0x32A));
      copyMatrix(l->model);
      if (l->model2[0]) {
        if (room == 11) {
          s32 light = gabi::call<s32>(0x025268C8);
          l->drawModel2 = (u8)light;
          if (light) {
            gabi::call<void>(0x028E93CC, ptr(0x1048D0CC), 630.46337890625f,
                             y + 4044.508056640625f, -202724.0f);
            s32 angle = gabi::call<s32>(0x02526944);
            gabi::call<void>(0x025F1C28, ptr(0x1048D0CC), angle);
            copyMatrix(l->model2[0]);
            gabi::call<void>(0x025F1C28, ptr(0x1048D0CC), -32768);
            copyMatrix(l->model2[1]);
          }
        } else if (room == 1 || room == 13) {
          copyMatrix(l->model2[0]);
          l->drawModel2 = 1;
        }
      }
      return 1;
    }
    if (l->alpha) {
      l->alpha = 0;
      return 1;
    }
  }
  if (gabi::call<BOOL>(0x021B4CB0, l))
    setAction(l, 0x021B5010);
  return 1;
}
VERIFY(0x021B5340, lodbg_deleteWait);
