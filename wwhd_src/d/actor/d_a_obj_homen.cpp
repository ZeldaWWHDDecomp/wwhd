/** Wind Temple stone Hookshot target, derived from HD disassembly. */
#include "d/actor/d_a_obj_homen.h"
namespace Homen {
// Guest callees save LR at caller SP+4; temporaries must remain above linkage.
template <class T> struct Local : gabi::Local<T> {
  gabi::Local<be<u8>[16]> linkage;
};
template <class T> T *at(u32 p, u32 off = 0) { return gabi::at<T>(p + off); }
template <class T> T *sub(daObjHomen_c *a, u32 off) {
  return at<T>(gabi::ea(a), off);
}
u32 word(u32 p) { return *at<be<u32>>(p); }
void put(u32 p, u32 v) { *at<be<u32>>(p) = v; }
constexpr u32 matrix = 0x1048D0CC;
struct SafeString {
  be<u32> text, vtable;
};
void setMatrix(daObjHomen_c *a) {
  WWHD_FUNC(0x02357EC4, void, a);
  if ((s32)a->type == 0 && (s16)a->pivoted == 1) {
    gabi::call<void>(0x028E93CC, at<void>(matrix), (f32)a->current.pos.x,
                     (f32)a->current.pos.y, (f32)a->current.pos.z);
    gabi::call<void>(0x025F24E0, 0.0f, 0.0f, 200.0f);
    gabi::call<void>(0x025F1C5C, at<void>(matrix), (s16)a->shape_angle.z);
    gabi::call<void>(0x025F1C28, at<void>(matrix), (s16)a->shape_angle.y);
    gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->shape_angle.x);
    gabi::call<void>(0x025F24E0, 0.0f, 0.0f, -200.0f);
  } else {
    gabi::call<void>(0x028E93CC, at<void>(matrix), (f32)a->current.pos.x,
                     (f32)a->current.pos.y, (f32)a->current.pos.z);
    gabi::call<void>(0x025F1C5C, at<void>(matrix), (s16)a->shape_angle.z);
    gabi::call<void>(0x025F1C28, at<void>(matrix), (s16)a->shape_angle.y);
    gabi::call<void>(0x025F1BF4, at<void>(matrix), (s16)a->shape_angle.x);
  }
  f32 values[12];
  for (int i = 0; i < 12; i++)
    values[i] = *at<be<f32>>(matrix + i * 4);
  u32 model = gabi::ea((void *)a->model);
  for (int i = 0; i < 12; i++)
    *at<be<f32>>(model + 0xc8 + i * 4) = values[i];
  gabi::call<void>(0x028E90D4, at<void>(matrix), a->matrix);
}
VERIFY(0x02357EC4, setMatrix);
void initMatrix(daObjHomen_c *a) {
  WWHD_FUNC(0x023580B0, void, a);
  // These three plain scale copies preserve the stored float payload bits.
  u32 y = word(gabi::ea(a) + 0x334), x = word(gabi::ea(a) + 0x330);
  u32 model = gabi::ea((void *)a->model);
  u32 z = word(gabi::ea(a) + 0x338);
  put(model + 0xc0, y);
  put(model + 0xbc, x);
  put(model + 0xc4, z);
  gabi::call<void>(0x02357EC4, a);
  gabi::call<void>(0x027F4D5C, (void *)a->model);
}
VERIFY(0x023580B0, initMatrix);
BOOL createHeap(daObjHomen_c *a) {
  WWHD_FUNC(0x02358100, BOOL, a);
  Local<SafeString> modelName, collisionName;
  modelName->vtable = 0x1002A9E8;
  modelName->text = 0x1002AC30;
  u32 type = (s32)a->type;
  u32 data = gabi::call<u32>(0x026066C4, at<void>(word(0x101F4F28)),
                             modelName.get(), word(0x101CA054 + type * 4));
  if (!data)
    gabi::call<void>(0x0273AA24, at<void>(0x1002AABC), 0x267,
                     at<void>(0x1002AAD0));
  else
    a->model =
        at<void>(gabi::call<u32>(0x025E38E0, at<void>(data), 0, 0x11000002));
  gabi::call<void>(0x023580B0, a);
  u32 index = word(0x101CA05C + (u32)(s32)a->type * 4);
  collisionName->text = 0x1002AC30;
  collisionName->vtable = 0x1002A9E8;
  u32 collision = gabi::call<u32>(0x026066C4, at<void>(word(0x101F4F28)),
                                  collisionName.get(), index);
  if (!collision)
    gabi::call<void>(0x0273AA24, at<void>(0x1002AABC), 0x274,
                     at<void>(0x1002AAE0));
  else {
    u32 bg = gabi::call<u32>(0x024F23F4, (void *)nullptr);
    a->background = at<void>(bg);
    if (bg && gabi::call<s32>(0x0200A030, at<void>(bg), at<void>(collision), 1,
                              a->matrix))
      return 0;
  }
  return data && (void *)a->model && (void *)a->background;
}
VERIFY(0x02358100, createHeap);
BOOL heapCallback(daObjHomen_c *a) {
  WWHD_FUNC(0x02358280, BOOL, a);
  return createHeap(a);
}
VERIFY(0x02358280, heapCallback);
BOOL processInit(daObjHomen_c *a, s16 state) {
  WWHD_FUNC(0x02358284, BOOL, a, state);
  if ((u32)(s32)state >= 5)
    return 0;
  u32 entry = 0x1002AAF0 + (u32)(s32)state * 8;
  s16 vslot = *at<be<s16>>(entry + 2), adjust = *at<be<s16>>(entry);
  u32 self = gabi::ea(a) + (s32)adjust, target;
  if (vslot < 0)
    target = word(entry + 4);
  else {
    s16 vptr = *at<be<s16>>(entry + 6);
    target = word(word(self + (s32)vptr) + (s32)vslot * 8 + 4);
  }
  if (!gabi::call<s32>(target, at<void>(self)))
    return 0;
  a->state = state;
  return 1;
}
VERIFY(0x02358284, processInit);
BOOL remove(daObjHomen_c *a) {
  WWHD_FUNC(0x023586DC, BOOL, a);
  gabi::call<void>(0x025204C8, sub<void>(a, 0x850), at<void>(0x1002AC30));
  if (word(gabi::ea(a) + 0xf4)) {
    u32 bg = gabi::ea((void *)a->background);
    if (bg && word(bg) < 256) {
      u32 p = gabi::call<u32>(0x025200D4);
      gabi::call<void>(0x020087EC, at<void>(p + 0x12a0), (void *)a->background);
    }
  }
  return 1;
}
VERIFY(0x023586DC, remove);
void getNorseOffset(daObjHomen_c *a, cXyz *out, s32 which) {
  WWHD_FUNC(0x0235874C, void, a, out, which);
  if (!word(0x10469F9C)) {
    put(0x10469F9C, 1);
    *at<be<f32>>(0x10469EFC) = 0;
    *at<be<f32>>(0x10469F00) = 453;
    *at<be<f32>>(0x10469F04) = 140;
    *at<be<f32>>(0x10469F08) = 0;
    *at<be<f32>>(0x10469F0C) = 181.5f;
    *at<be<f32>>(0x10469F10) = 48;
  }
  if (!word(0x10469FA0)) {
    put(0x10469FA0, 1);
    *at<be<f32>>(0x10469F14) = 0;
    *at<be<f32>>(0x10469F18) = 453;
    *at<be<f32>>(0x10469F1C) = 190;
    *at<be<f32>>(0x10469F20) = 0;
    *at<be<f32>>(0x10469F24) = 181.5f;
    *at<be<f32>>(0x10469F28) = 68;
  }
  u32 data = word(0x101CA04C + ((u32)which & 1) * 4) + (u32)(s32)a->type * 12;
  put(gabi::ea(out), word(data));
  put(gabi::ea(out) + 4, word(data + 4));
  put(gabi::ea(out) + 8, word(data + 8));
}
VERIFY(0x0235874C, getNorseOffset);
void makeItem(daObjHomen_c *a) {
  WWHD_FUNC(0x023589FC, void, a);
  Local<csXyz> angles;
  Local<cXyz> scale;
  gabi::call<void>(0x0201A478, angles.get(), 0, 0, 0);
  scale->z = 1;
  scale->x = 1;
  scale->y = 1;
  s32 item = gabi::call<s32>(0x02359AB0, a, 6, 12);
  if (item != 63) {
    s32 bit = gabi::call<s32>(0x02359AB0, a, 7, 0);
    gabi::call<void>(0x025D8120, &a->current.pos, item, bit,
                     (s8)*sub<be<u8>>(a, 0x326), 0, angles.get(), 1,
                     scale.get());
  }
}
VERIFY(0x023589FC, makeItem);
void manageItemTimer(daObjHomen_c *a) {
  WWHD_FUNC(0x02358AA8, void, a);
  s16 timer = a->itemTimer;
  if (timer <= 0)
    return;
  timer = (s16)(timer - 1);
  a->itemTimer = timer;
  if (!timer)
    gabi::call<void>(0x023589FC, a);
}
VERIFY(0x02358AA8, manageItemTimer);
void makeEnemy(daObjHomen_c *a) {
  WWHD_FUNC(0x02358AC8, void, a);
  s32 enemy = gabi::call<s32>(0x02359AB0, a, 4, 18);
  if (enemy == 15 || (u32)enemy >= 11)
    return;
  if (enemy < 0)
    gabi::call<void>(0x0273AA24, at<void>(0x1002AB48), 0x3a9,
                     at<void>(0x1002AB5C));
  Local<be<f32>[6]> offsets;
  Local<cXyz> position;
  *at<be<f32>>(gabi::ea(offsets.get()) + 8) = 350;
  *at<be<f32>>(gabi::ea(offsets.get()) + 4) = 300;
  *at<be<f32>>(gabi::ea(offsets.get()) + 12) = 0;
  *at<be<f32>>(gabi::ea(offsets.get()) + 0) = 0;
  *at<be<f32>>(gabi::ea(offsets.get()) + 20) = 25;
  *at<be<f32>>(gabi::ea(offsets.get()) + 16) = 0;
  u32 index = (u32)(s32)a->type & 1;
  gabi::call<void>(0x028E8F64, a->matrix,
                   at<void>(gabi::ea(offsets.get()) + index * 12),
                   position.get());
  u32 dat = 0x1002AB6C + (u32)enemy * 8;
  s16 proc = *at<be<s16>>(dat);
  u32 params = word(dat + 4);
  u32 id = gabi::call<u32>(0x025D5834, proc, params, position.get(),
                           (s8)*sub<be<u8>>(a, 0x326), 0, 0, -1, 0);
  a->enemyID = id;
  if (id == 0xffffffff)
    *sub<be<u8>>(a, 0x2da) = 0;
}
VERIFY(0x02358AC8, makeEnemy);
void manageEnemyTimer(daObjHomen_c *a) {
  WWHD_FUNC(0x02358BDC, void, a);
  s16 timer = a->enemyTimer;
  if (timer <= 0)
    return;
  timer = (s16)(timer - 1);
  a->enemyTimer = timer;
  if (!timer)
    gabi::call<void>(0x02358AC8, a);
}
VERIFY(0x02358BDC, manageEnemyTimer);
void getNorsePoint(daObjHomen_c *a, cXyz *out, s32 which) {
  WWHD_FUNC(0x02358DB8, void, a, out, which);
  Local<cXyz> offset, result;
  gabi::call<void>(0x0235874C, a, offset.get(), which);
  gabi::call<void>(0x028E8F64, a->matrix, offset.get(), result.get());
  put(gabi::ea(out), word(gabi::ea(result.get())));
  put(gabi::ea(out) + 4, word(gabi::ea(result.get()) + 4));
  put(gabi::ea(out) + 8, word(gabi::ea(result.get()) + 8));
}
VERIFY(0x02358DB8, getNorsePoint);
BOOL freeInit(daObjHomen_c *a) {
  WWHD_FUNC(0x02359114, BOOL, a);
  a->enemyID = 0xffffffff;
  return 1;
}
VERIFY(0x02359114, freeInit);
BOOL methodCreate(daObjHomen_c *a) {
  WWHD_FUNC(0x0235981C, BOOL, a);
  return gabi::call<s32>(0x0235833C, a);
}
VERIFY(0x0235981C, methodCreate);
BOOL methodDelete(daObjHomen_c *a) {
  WWHD_FUNC(0x02359820, BOOL, a);
  return remove(a);
}
VERIFY(0x02359820, methodDelete);
BOOL methodExecute(daObjHomen_c *a) {
  WWHD_FUNC(0x02359824, BOOL, a);
  return gabi::call<s32>(0x02358C54, a);
}
VERIFY(0x02359824, methodExecute);
BOOL methodDraw(daObjHomen_c *a) {
  WWHD_FUNC(0x02359828, BOOL, a);
  return gabi::call<s32>(0x02358D48, a);
}
VERIFY(0x02359828, methodDraw);
BOOL isDelete(daObjHomen_c *a) {
  WWHD_FUNC(0x02359AA8, BOOL, a);
  return 1;
}
VERIFY(0x02359AA8, isDelete);
u32 parameter(const daObjHomen_c *a, u32 width, u32 shift) {
  WWHD_FUNC(0x02359AB0, u32, a, width, shift);
  u32 value = word(gabi::ea(a) + 0xb0), n = width & 63, s = shift & 63;
  u32 mask = (n < 32 ? (1u << n) : 0u) - 1u;
  u32 shifted = s < 32 ? value >> s : 0u;
  return shifted & mask;
}
VERIFY(0x02359AB0, parameter);
void processMain(daObjHomen_c *a) {
  WWHD_FUNC(0x02358BFC, void, a);
  s16 state = a->state;
  if ((u32)(s32)state >= 5)
    return;
  u32 entry = 0x1002ABC4 + (u32)(s32)state * 8;
  s16 slot = *at<be<s16>>(entry + 2), adjust = *at<be<s16>>(entry);
  u32 self = gabi::ea(a) + (s32)adjust, target;
  if (slot < 0)
    target = word(entry + 4);
  else {
    s16 off = *at<be<s16>>(entry + 6);
    target = word(word(self + (s32)off) + (s32)slot * 8 + 4);
  }
  gabi::call<void>(target, at<void>(self));
}
VERIFY(0x02358BFC, processMain);
BOOL execute(daObjHomen_c *a) {
  WWHD_FUNC(0x02358C54, BOOL, a);
  gabi::call<void>(0x02358820, a);
  gabi::call<void>(0x023588F4, a);
  gabi::call<void>(0x02358AA8, a);
  gabi::call<void>(0x02358BDC, a);
  gabi::call<void>(0x02358BFC, a);
  u32 bg = gabi::ea((void *)a->background);
  if (bg)
    gabi::call<void>(0x024F43DC, at<void>(bg));
  s16 increment = a->spinZ, spin = a->spinX;
  spin = (s16)(spin + increment);
  if (spin > 2000)
    spin = 2000;
  s16 angle = (s16)((s16)a->shape_angle.x + spin);
  a->spinX = spin;
  if (angle >= 0x5000) {
    a->shape_angle.x = 0x5000;
    a->spinX = 0;
    a->spinZ = 0;
  } else
    a->shape_angle.x = angle;
  gabi::call<void>(0x025D67A8, a);
  gabi::call<void>(0x025D6800, a, 0);
  gabi::call<void>(0x02357EC4, a);
  return 1;
}
VERIFY(0x02358C54, execute);
BOOL draw(daObjHomen_c *a) {
  WWHD_FUNC(0x02358D48, BOOL, a);
  s16 state = a->state;
  if (state != 4 && state != 3) {
    u32 env = gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x025626A4, at<void>(env), 1, &a->current.pos,
                     sub<void>(a, 0x110));
    env = gabi::call<u32>(0x02555D0C);
    gabi::call<void>(0x02562F5C, at<void>(env), (void *)a->model,
                     sub<void>(a, 0x110));
    gabi::call<void>(0x025E2DE0, (void *)a->model, 0);
  }
  return 1;
}
VERIFY(0x02358D48, draw);
BOOL create(daObjHomen_c *a) {
  WWHD_FUNC(0x0235833C, BOOL, a);
  u32 status = word(gabi::ea(a) + 0x2e4);
  if (!(status & 8)) {
    if (a) {
      gabi::call<void>(0x025D4ED0, a);
      put(gabi::ea(a) + 0xb4, 0x1002AAA0);
      gabi::call<void>(0x0200BD2C, sub<void>(a, 0x3c8));
      gabi::call<void>(0x02515DA0, sub<void>(a, 0x3e4));
      put(gabi::ea(a) + 0x3e0, 0x1004AE88);
      put(gabi::ea(a) + 0x3e4, 0x1004AEC0);
      gabi::call<void>(0x025166F0, sub<void>(a, 0x404));
      gabi::call<void>(0x028EFFD0, sub<void>(a, 0x530), 2, 0x3c,
                       at<void>(0x023598D4));
      gabi::call<void>(0x028EFFD0, sub<void>(a, 0x5a8), 2, 0x138,
                       at<void>(0x0235993C));
      gabi::call<void>(0x02008E0C, sub<void>(a, 0x890));
      *sub<be<u8>>(a, 0x8d5) = 0;
      *sub<be<u8>>(a, 0x8da) = 0;
      put(gabi::ea(a) + 0x890, gabi::ea(a) + 0x8d0);
      *sub<be<u8>>(a, 0x8d4) = 1;
      *sub<be<u8>>(a, 0x8d8) = 0;
      put(gabi::ea(a) + 0x894, gabi::ea(a) + 0x8dc);
      put(gabi::ea(a) + 0x8e0, 1);
      put(gabi::ea(a) + 0x8a0, 0x1002AA60);
      put(gabi::ea(a) + 0x8d0, 0x1002AA90);
      *sub<be<u8>>(a, 0x8d9) = 0;
      put(gabi::ea(a) + 0x8b0, 0x1002AA70);
      *sub<be<u8>>(a, 0x8d6) = 0;
      put(gabi::ea(a) + 0x8dc, 0x1002AA80);
      status = word(gabi::ea(a) + 0x2e4);
      *sub<be<u8>>(a, 0x8d7) = 0;
    }
    put(gabi::ea(a) + 0x2e4, status | 8);
  }
  s32 sw = gabi::call<s32>(0x02359AB0, a, 8, 24);
  if (sw != 255 &&
      gabi::call<s32>(0x025BA0C0, at<void>(word(0x101F84DC) + 0x20), sw,
                      (s8)*sub<be<u8>>(a, 0x2fe)))
    return 5;
  s32 phase =
      gabi::call<s32>(0x02520460, sub<void>(a, 0x850), at<void>(0x1002AC30));
  a->type = gabi::call<u32>(0x02359AB0, a, 4, 8) & 1;
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, a, at<void>(0x02358280), 0))
    return 5;
  u32 model = gabi::ea((void *)a->model);
  put(gabi::ea(a) + 0x348, model ? model + 0xc8 : 0);
  u32 p = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x024EEA6C, at<void>(p + 0x12a0), (void *)a->background, a);
  put(gabi::ea((void *)a->background) + 0xa8, 0);
  u32 cull = 0x101CA06C + (u32)(s32)a->type * 24;
  gabi::call<void>(0x025D674C, a, (f32)*at<be<f32>>(cull),
                   (f32)*at<be<f32>>(cull + 4), (f32)*at<be<f32>>(cull + 8),
                   (f32)*at<be<f32>>(cull + 12), (f32)*at<be<f32>>(cull + 16),
                   (f32)*at<be<f32>>(cull + 20));
  gabi::call<void>(0x02515F14, sub<void>(a, 0x3c8), 0, 255, a);
  u32 sph = (s32)a->type == 0 ? 0x1002AC78 : 0x1002AC38;
  gabi::call<void>(0x0251677C, sub<void>(a, 0x404), at<void>(sph));
  put(gabi::ea(a) + 0x448, gabi::ea(a) + 0x3c8);
  u32 zeroX = word(0x101FFBA8), flags = word(gabi::ea(a) + 0x498);
  put(gabi::ea(a) + 0x4b8, zeroX);
  put(gabi::ea(a) + 0x4bc, word(0x101FFBAC));
  u32 zeroZ = word(0x101FFBB0);
  put(gabi::ea(a) + 0x498, flags | 4);
  put(gabi::ea(a) + 0x4c0, zeroZ);
  for (u32 i = 0; i < 2; i++) {
    u32 st = gabi::ea(a) + 0x530 + i * 0x3c,
        cap = gabi::ea(a) + 0x5a8 + i * 0x138,
        ends = gabi::ea(a) + 0x818 + i * 28;
    gabi::call<void>(0x02515F14, at<void>(st), 100, 255, a);
    u32 src = (s32)a->type == 0 ? 0x1002ACB8 : 0x1002AD04;
    gabi::call<void>(0x025164C0, at<void>(cap), at<void>(src));
    put(cap + 0x44, st);
    for (u32 j = 0; j < 3; j++)
      put(ends + j * 4, word(gabi::ea(a) + 0x314 + j * 4));
    for (u32 j = 0; j < 3; j++)
      put(ends + 12 + j * 4, word(gabi::ea(a) + 0x314 + j * 4));
    *at<be<f32>>(ends + 24) =
        *at<be<f32>>(0x101CA064 + ((u32)(s32)a->type & 1) * 4);
  }
  gabi::call<s32>(0x02358284, a, 0);
  if (gabi::call<s32>(0x02359AB0, a, 4, 18) != 15)
    *sub<be<u8>>(a, 0x2da) = 2;
  p = gabi::call<u32>(0x025200D4);
  a->eventIndex = (s16)gabi::call<s32>(0x02543F10, at<void>(p + 0x52c4),
                                       at<void>(0x1002AB18), 255);
  a->hookshotID = 0xffffffff;
  return phase;
}
VERIFY(0x0235833C, create);
void adjustHookshot(daObjHomen_c *a) {
  WWHD_FUNC(0x02358820, void, a);
  u32 p = gabi::call<u32>(0x025200D4), player = word(p + 0x5b2c);
  if (word(player + 0x3b8) & 0x02000000)
    return;
  u32 id = a->hookshotID;
  if (id == 0xffffffff)
    return;
  Local<be<u32>> lookup;
  *lookup = id;
  if (!gabi::call<u32>(0x025D5218, at<void>(0x025E1234), lookup.get()))
    return;
  Local<cXyz> offset, result;
  Local<be<f32>[12]> transform;
  gabi::call<void>(0x0235874C, a, offset.get(), 1);
  gabi::call<void>(0x025F1884, at<void>(matrix), (s16)a->shape_angle.y);
  gabi::call<void>(0x028E90D4, at<void>(matrix), transform.get());
  gabi::call<void>(0x028E8F64, transform.get(), offset.get(), result.get());
  u32 target = word(word(player + 0xb4) + 0x10c);
  gabi::call<void>(target, at<void>(player), word(gabi::ea(a) + 4),
                   result.get());
  a->hookshotID = 0xffffffff;
}
VERIFY(0x02358820, adjustHookshot);
void executeEvent(daObjHomen_c *a) {
  WWHD_FUNC(0x023588F4, void, a);
  s16 state = a->eventState;
  if (state == 1) {
    if ((u16)*sub<be<u16>>(a, 0xf8) == 2) {
      a->eventState = 2;
      return;
    }
    gabi::call<void>(0x025D7A58, a, (s16)a->eventOrder, 255, 65535, 0, 1);
    *sub<be<u16>>(a, 0xfa) = (u16)*sub<be<u16>>(a, 0xfa) | 2;
  } else if (state == 2) {
    s16 event = a->eventOrder;
    u32 p = gabi::call<u32>(0x025200D4);
    if (gabi::call<s32>(0x025440C8, at<void>(p + 0x52c4), event)) {
      p = gabi::call<u32>(0x025200D4);
      *at<be<u16>>(p + 0x52b8) = (u16)*at<be<u16>>(p + 0x52b8) | 8;
      a->eventOrder = -1;
      a->eventState = 0;
    }
  }
}
VERIFY(0x023588F4, executeEvent);
void setAtCollision(daObjHomen_c *a) {
  WWHD_FUNC(0x02358E0C, void, a);
  constexpr u32 table = 0x10469F3C;
  if (!word(0x10469FA4)) {
    *at<be<f32>>(table + 0x18) = 120;
    *at<be<f32>>(table + 0x20) = 140;
    *at<be<f32>>(table + 8) = 140;
    *at<be<f32>>(table + 0x14) = 140;
    *at<be<f32>>(table + 0x2c) = 140;
    *at<be<f32>>(table + 0x10) = 550;
    *at<be<f32>>(table + 0x1c) = 30;
    *at<be<f32>>(table + 0x24) = 120;
    *at<be<f32>>(table + 0x4c) = 20;
    *at<be<f32>>(table + 0x34) = 20;
    *at<be<f32>>(table + 0x28) = 550;
    *at<be<f32>>(table + 0x30) = -45;
    *at<be<f32>>(table + 0x3c) = -45;
    *at<be<f32>>(table + 0x38) = 80;
    *at<be<f32>>(table + 0x44) = 80;
    put(0x10469FA4, 1);
    *at<be<f32>>(table + 0x40) = 190;
    *at<be<f32>>(table) = -120;
    *at<be<f32>>(table + 4) = 30;
    *at<be<f32>>(table + 0x48) = 45;
    *at<be<f32>>(table + 0xc) = -120;
    *at<be<f32>>(table + 0x50) = 80;
    *at<be<f32>>(table + 0x54) = 45;
    *at<be<f32>>(table + 0x58) = 190;
    *at<be<f32>>(table + 0x5c) = 80;
  }
  Local<cXyz> input, result;
  for (u32 i = 0; i < 2; i++) {
    u32 index = ((u32)(s32)a->type & 1) * 2 + i,
        ends = gabi::ea(a) + 0x818 + i * 28,
        cap = gabi::ea(a) + 0x5a8 + i * 0x138;
    u32 data = table + index * 24;
    put(gabi::ea(input.get()) + 4, word(data + 4));
    put(gabi::ea(input.get()), word(data));
    put(gabi::ea(input.get()) + 8, word(data + 8));
    gabi::call<void>(0x028E8F64, a->matrix, input.get(), result.get());
    u32 x = word(gabi::ea(result.get())), y = word(gabi::ea(result.get()) + 4);
    put(ends, x);
    u32 z = word(gabi::ea(result.get()) + 8);
    put(ends + 4, y);
    put(ends + 8, z);
    index = ((u32)(s32)a->type & 1) * 2 + i;
    data = table + 12 + index * 24;
    put(gabi::ea(input.get()) + 4, word(data + 4));
    put(gabi::ea(input.get()), word(data));
    put(gabi::ea(input.get()) + 8, word(data + 8));
    gabi::call<void>(0x028E8F64, a->matrix, input.get(), result.get());
    y = word(gabi::ea(result.get()) + 4);
    x = word(gabi::ea(result.get()));
    z = word(gabi::ea(result.get()) + 8);
    put(ends + 12, x);
    put(ends + 20, z);
    put(ends + 16, y);
    gabi::call<void>(0x020181FC, at<void>(cap + 0x118), at<void>(ends));
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x0200E240, at<void>(p + 0x26a4), at<void>(cap));
  }
}
VERIFY(0x02358E0C, setAtCollision);
void setCoCollision(daObjHomen_c *a) {
  WWHD_FUNC(0x02358FEC, void, a);
  u32 sph = gabi::ea(a) + 0x404;
  if (gabi::call<s32>(0x025162A4, at<void>(sph))) {
    u32 hit = gabi::call<u32>(0x02516300, at<void>(sph)),
        p = gabi::call<u32>(0x025200D4), player = word(p + 0x5b2c);
    if (hit && (word(hit + 0x10) & 0x8000)) {
      a->yaw = *at<be<s16>>(player + 0x32a);
      u32 actor = gabi::call<u32>(0x02515BBC, sub<void>(a, 0x498));
      u32 id = 0xffffffff;
      s32 type = a->type;
      if (actor)
        id = word(actor + 4);
      a->hookshotID = id;
      if (type == 0)
        gabi::call<void>(0x025B8B68, at<void>(word(0x101F84DC) + 0x644),
                         0x3880);
    }
    gabi::call<void>(0x0251621C, sub<void>(a, 0x404));
    return;
  }
  Local<be<f32>[2]> radii;
  Local<cXyz> center;
  *at<be<f32>>(gabi::ea(radii.get()) + 0) = *at<be<f32>>(0x1002AC0C);
  *at<be<f32>>(gabi::ea(radii.get()) + 4) = *at<be<f32>>(0x1002AC10);
  gabi::call<void>(0x02358DB8, a, center.get(), 0);
  gabi::call<void>(0x02018D40, sub<void>(a, 0x51c), center.get());
  u32 index = (u32)(s32)a->type & 1;
  gabi::call<void>(0x02018C8C, sub<void>(a, 0x51c),
                   (f32)*at<be<f32>>(gabi::ea(radii.get()) + index * 4));
  u32 p = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x0200E240, at<void>(p + 0x26a4), at<void>(sph));
}
VERIFY(0x02358FEC, setCoCollision);
void freeMain(daObjHomen_c *a) {
  WWHD_FUNC(0x02359124, void, a);
  u32 p = gabi::call<u32>(0x025200D4), player = word(p + 0x5b2c);
  u32 boots = word(player + 0x3b8), flags = word(gabi::ea(a) + 0x2e0);
  if (boots & 0x02000000)
    flags = (flags | 0x80000) & ~0x200000u;
  else
    flags = (flags | 0x200000) & ~0x80000u;
  put(gabi::ea(a) + 0x2e0, flags);
  if (flags & 0x100000)
    gabi::call<s32>(0x02358284, a, 1);
  gabi::call<void>(0x02358FEC, a);
}
VERIFY(0x02359124, freeMain);
BOOL waitFallInit(daObjHomen_c *a) {
  WWHD_FUNC(0x023591A4, BOOL, a);
  u32 flags = word(gabi::ea(a) + 0x2e0);
  a->enemyTimer = 6;
  a->followCount = 0;
  put(gabi::ea(a) + 0x2e0, flags & ~0x100000u);
  a->itemTimer = 20;
  u32 p = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x020087EC, at<void>(p + 0x12a0), (void *)a->background);
  a->background = nullptr;
  p = gabi::call<u32>(0x025200D4);
  u32 type = (s32)a->type;
  Local<cXyz> up;
  up->z = 0;
  up->x = 0;
  up->y = 1;
  gabi::call<void>(0x025CB374, at<void>(p + 0x599c),
                   word(0x101CA09C + (type & 1) * 4), -17, up.get());
  if ((s32)a->type != 0) {
    u32 z = word(gabi::ea(a) + 0x31c), y = word(gabi::ea(a) + 0x318),
        x = word(gabi::ea(a) + 0x314), id = a->hookshotID;
    put(gabi::ea(a) + 0x914, x);
    put(gabi::ea(a) + 0x918, y);
    put(gabi::ea(a) + 0x91c, z);
    if (id != 0xffffffff) {
      Local<be<u32>> lookup;
      *lookup = id;
      u32 hook =
          gabi::call<u32>(0x025D5218, at<void>(0x025E1234), lookup.get());
      if (hook) {
        Local<cXyz> offset, result, difference;
        Local<be<f32>[12]> transform;
        gabi::call<void>(0x0235874C, a, offset.get(), 1);
        gabi::call<void>(0x025F1884, at<void>(matrix), (s16)a->shape_angle.y);
        gabi::call<void>(0x028E90D4, at<void>(matrix), transform.get());
        gabi::call<void>(0x028E8F64, transform.get(), offset.get(),
                         result.get());
        gabi::call<void>(0x0201ADE0, at<cXyz>(hook + 0x314), difference.get(),
                         result.get());
        x = word(gabi::ea(difference.get()));
        z = word(gabi::ea(difference.get()) + 8);
        put(gabi::ea(a) + 0x914, x);
        y = word(gabi::ea(difference.get()) + 4);
        put(gabi::ea(a) + 0x91c, z);
        put(gabi::ea(a) + 0x918, y);
      }
    }
    if ((s32)a->type != 0)
      return 1;
  }
  gabi::call<void>(0x025B8B68, at<void>(word(0x101F84DC) + 0x644), 0x3410);
  return 1;
}
VERIFY(0x023591A4, waitFallInit);
void waitFallMain(daObjHomen_c *a) {
  WWHD_FUNC(0x02359328, void, a);
  if ((s16)a->followCount < 3) {
    gabi::call<void>(0x0200ED84, &a->current.pos.x, (f32)a->target.x, 0.5f,
                     20.0f);
    gabi::call<void>(0x0200ED84, &a->current.pos.y, (f32)a->target.y, 0.5f,
                     20.0f);
    gabi::call<void>(0x0200ED84, &a->current.pos.z, (f32)a->target.z, 0.5f,
                     20.0f);
    a->followCount = (s16)((s16)a->followCount + 1);
  } else
    gabi::call<s32>(0x02358284, a, 2);
  gabi::call<void>(0x02358FEC, a);
}
VERIFY(0x02359328, waitFallMain);
BOOL fallInit(daObjHomen_c *a) {
  WWHD_FUNC(0x02359400, BOOL, a);
  u32 id = word(gabi::ea(a) + 4);
  *sub<be<f32>>(a, 0x374) = -2;
  *sub<be<f32>>(a, 0x378) = -36;
  f32 y = (f32)a->current.pos.y + 100.0f, x = a->current.pos.x,
      z = a->current.pos.z;
  *sub<be<f32>>(a, 0x8b4) = x;
  *sub<be<f32>>(a, 0x8b8) = y;
  put(gabi::ea(a) + 0x898, id);
  *sub<be<f32>>(a, 0x8bc) = z;
  u32 p = gabi::call<u32>(0x025200D4);
  f32 ground =
      gabi::call<f32>(0x02008974, at<void>(p + 0x12a0), sub<void>(a, 0x890));
  s32 type = a->type;
  a->groundY = ground;
  a->spinZ = type == 0 ? 90 : 120;
  f32 random = gabi::call<f32>(0x02019918, 3072.0f);
  a->spinTarget = (s16)gabi::ftoi(random);
  return 1;
}
VERIFY(0x02359400, fallInit);
void fallMain(daObjHomen_c *a) {
  WWHD_FUNC(0x023594C8, void, a);
  gabi::call<void>(0x02358E0C, a);
  Local<cXyz> point;
  gabi::call<void>(0x02358DB8, a, point.get(), 0);
  f32 ground;
  if ((s32)a->type == 0 && (s16)a->pivoted == 0) {
    Local<cXyz> zero, result;
    zero->x = 0;
    zero->y = 0;
    zero->z = 0;
    gabi::call<void>(0x028E8F64, a->matrix, zero.get(), result.get());
    ground = a->groundY;
    if (!((f32)result->y - 1.0f > ground)) {
      *sub<be<f32>>(a, 0x370) = 0;
      a->pivoted = 1;
      ground = a->groundY;
      *sub<be<f32>>(a, 0x374) = 0;
      *sub<be<f32>>(a, 0x33c) = 0;
      *sub<be<f32>>(a, 0x344) = 0;
      *sub<be<f32>>(a, 0x340) = 0;
    }
  } else {
    ground = a->groundY;
    if ((f32)a->current.pos.y < ground) {
      *sub<be<f32>>(a, 0x340) = 0;
      *sub<be<f32>>(a, 0x370) = 0;
      *sub<be<f32>>(a, 0x374) = 0;
      *sub<be<f32>>(a, 0x344) = 0;
      ground = a->groundY;
      *sub<be<f32>>(a, 0x33c) = 0;
    }
  }
  s16 pivot = a->pivoted;
  if ((f32)point->y < ground) {
    u32 params = ((u32)(s32)pivot << 1) | (u32)(s32)a->type;
    u32 smoke = gabi::call<u32>(0x025D5A20, 0x82, word(gabi::ea(a) + 4), params,
                                &a->current.pos, (s8)*sub<be<u8>>(a, 0x326),
                                &a->shape_angle, 0, -1, 0);
    a->smokeID = smoke;
    if (smoke != 0xffffffff) {
      u32 p = gabi::call<u32>(0x025200D4);
      Local<cXyz> up;
      up->x = 0;
      up->y = 1;
      up->z = 0;
      gabi::call<void>(0x025CB374, at<void>(p + 0x599c),
                       word(0x101CA0A4 + ((u32)(s32)a->type & 1) * 4), -17,
                       up.get());
      *sub<be<f32>>(a, 0x33c) = 0;
      *sub<be<f32>>(a, 0x344) = 0;
      a->spinX = 0;
      *sub<be<f32>>(a, 0x370) = 0;
      a->spinZ = 0;
      *sub<be<f32>>(a, 0x340) = 0;
      *sub<be<f32>>(a, 0x374) = 0;
      gabi::call<s32>(0x02358284, a, 3);
    }
  } else if (!pivot) {
    gabi::call<void>(0x0200F428, &a->shape_angle.z, (s16)a->spinTarget, 20,
                     4096);
    gabi::call<void>(0x0200F428, &a->shape_angle.y, (s16)((s16)a->yaw + 0x8000),
                     10, 4096);
  }
  if ((f32)*sub<be<f32>>(a, 0x374) != 0.0f)
    *sub<be<f32>>(a, 0x370) =
        *at<be<f32>>(0x101CA0AC + ((u32)(s32)a->type & 1) * 4);
}
VERIFY(0x023594C8, fallMain);
BOOL waitInit(daObjHomen_c *a) {
  WWHD_FUNC(0x023596F8, BOOL, a);
  s32 sw = gabi::call<s32>(0x02359AB0, a, 8, 24);
  gabi::call<void>(0x025B9E38, at<void>(word(0x101F84DC) + 0x20), sw,
                   (s8)*sub<be<u8>>(a, 0x2fe));
  s32 type = a->type;
  a->extra = 1;
  gabi::call<void>(0x025E19CC, type == 0 ? 0x69ff : 0x6a00, &a->current.pos);
  return 1;
}
VERIFY(0x023596F8, waitInit);
void waitMain(daObjHomen_c *a) {
  WWHD_FUNC(0x02359770, void, a);
  s16 timer = a->extra;
  if (timer <= 0)
    return;
  timer = (s16)(timer - 1);
  a->extra = timer;
  if (!timer)
    gabi::call<s32>(0x02358284, a, 4);
}
VERIFY(0x02359770, waitMain);
BOOL noneInit(daObjHomen_c *a) {
  WWHD_FUNC(0x02359794, BOOL, a);
  if ((s32)a->type == 0)
    gabi::call<void>(0x025E1988, 0x806);
  return 1;
}
VERIFY(0x02359794, noneInit);
void noneMain(daObjHomen_c *a) {
  WWHD_FUNC(0x023597C8, void, a);
  u32 id = a->enemyID;
  if (id == 0xffffffff)
    return;
  Local<be<u32>> found;
  if (!gabi::call<s32>(0x025D54C4, id, found.get())) {
    a->enemyID = 0xffffffff;
    gabi::call<void>(0x025D57E0, a);
  }
}
VERIFY(0x023597C8, noneMain);
void staticInit() {
  WWHD_FUNC(0x0235982C, void);
  put(0x10469F34, 0);
  put(0x10469F2C, 0);
  put(0x10469F38, 0);
  put(0x10469F30, 0);
  gabi::call<void>(0x028F026C, at<void>(0x101CA0B4));
  *at<be<f32>>(0x10469EF0) = -3.1415927410125732f;
  *at<be<f32>>(0x10469EF4) = 3.1415927410125732f;
  gabi::call<void>(0x028ED6F8, at<void>(0x10469EF8));
  gabi::call<void>(0x028F026C, at<void>(0x101CA0C0));
  gabi::call<void>(0x028EAB2C, at<void>(0x10469EF9));
  gabi::call<void>(0x028F026C, at<void>(0x101CA0CC));
}
VERIFY(0x0235982C, staticInit);
void staticDestructor(void *self, s32 flags) {
  WWHD_FUNC(0x023598C0, void, self, flags);
  if (self && (flags & 1))
    gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x023598C0, staticDestructor);
void *statusConstruct(void *self) {
  WWHD_FUNC(0x023598D4, void *, self);
  u32 p = gabi::ea(self);
  if (!p)
    p = gabi::call<u32>(0x0273AD10, 0x3c);
  if (p) {
    gabi::call<void>(0x0200BD2C, at<void>(p));
    gabi::call<void>(0x02515DA0, at<void>(p + 0x1c));
    put(p + 0x18, 0x1004AE88);
    put(p + 0x1c, 0x1004AEC0);
  }
  return at<void>(p);
}
VERIFY(0x023598D4, statusConstruct);
void *capsuleConstruct(void *self) {
  WWHD_FUNC(0x0235993C, void *, self);
  u32 p = gabi::ea(self);
  if (!p)
    p = gabi::call<u32>(0x0273AD10, 0x138);
  if (p) {
    gabi::call<void>(0x02515FB8, at<void>(p));
    put(p + 0x114, 0x100015A8);
    put(p + 0x110, 0x1002AA00);
    gabi::call<void>(0x02018150, at<void>(p + 0x118));
    put(p + 0x3c, 0x1004AF18);
    put(p + 0x130, 0x1004AF60);
    put(p + 0x114, 0x1004AF70);
  }
  return at<void>(p);
}
VERIFY(0x0235993C, capsuleConstruct);
void emptyVirtual(void *self) { WWHD_FUNC(0x023599C8, void, self); }
VERIFY(0x023599C8, emptyVirtual);
void actorDestructor(daObjHomen_c *a, s32 flags) {
  WWHD_FUNC(0x023599CC, void, a, flags);
  if (!a)
    return;
  put(gabi::ea(a) + 0x8b0, 0x1002AA30);
  put(gabi::ea(a) + 0x8d0, 0x1002AA50);
  put(gabi::ea(a) + 0x8dc, 0x1002AA10);
  gabi::call<void>(0x02008DAC, sub<void>(a, 0x890), 0);
  gabi::call<void>(0x028F0164, sub<void>(a, 0x5a8), 2, 0x138,
                   at<void>(0x02515980), 0, 0);
  gabi::call<void>(0x028F0164, sub<void>(a, 0x530), 2, 0x3c,
                   at<void>(0x02515860), 0, 0);
  gabi::call<void>(0x02515AE8, sub<void>(a, 0x404), 2);
  gabi::call<void>(0x02515860, sub<void>(a, 0x3c8), 2);
  gabi::call<void>(0x025D50BC, a, 0);
  if (flags & 1)
    gabi::call<void>(0x0273AF40, a);
}
VERIFY(0x023599CC, actorDestructor);
} // namespace Homen
