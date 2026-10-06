/* Collectable Deku Leaf; derived from WWHD disassembly. */
#include "d/actor/d_a_deku_item.h"
namespace DekuItem {
template <class T> T *at(u32 p, u32 off = 0) { return gabi::at<T>(p + off); }
template <class T> T *sub(daDekuItem_c *a, u32 off) {
  return at<T>(gabi::ea(a), off);
}
u32 word(u32 p) { return *at<be<u32>>(p); }
void put(u32 p, u32 v) { *at<be<u32>>(p) = v; }
struct SafeString {
  be<u32> text, vtable;
};
// Leave linkage space below the payload when an actual nonleaf callee runs.
struct ResourceLocal {
  be<u32> linkage[2];
  SafeString name;
};
u32 resource(gabi::Local<ResourceLocal> &local, u32 id) {
  local->name.text = 0x1000d6a0;
  local->name.vtable = 0x1000d574;
  return gabi::call<u32>(0x026066C4, at<void>(word(0x101f4f28)), &local->name,
                         id);
}
void assertion(u32 line, u32 condition) {
  gabi::call(0x0273AA24, at<void>(0x1000d614), line, at<void>(condition));
}
BOOL CreateHeap(daDekuItem_c *a) {
  WWHD_FUNC(0x0211E6F0, BOOL, a);
  gabi::Local<ResourceLocal> modelName, firstName, secondName;
  u32 data = resource(modelName, 8);
  if (!data)
    assertion(0xf4, 0x1000d628);
  u32 model = gabi::call<u32>(0x025E38E0, at<void>(data), 0, 0x11020203);
  a->model = at<void>(model);
  if (!model)
    return 0;
  u32 animation = resource(firstName, 4);
  if (!animation)
    assertion(0x103, 0x1000d608);
  if (!gabi::call<s32>(0x025E8508, sub<void>(a, 0x3b8), at<void>(data),
                       at<void>(animation), 1, 2, 1.0f, 0, -1, 0))
    return 0;
  animation = resource(secondName, 5);
  if (!animation)
    assertion(0x110, 0x1000d608);
  return gabi::call<s32>(0x025E8508, sub<void>(a, 0x444), at<void>(data),
                         at<void>(animation), 1, 0, 1.0f, 0, -1, 0) != 0;
}
VERIFY(0x0211E6F0, CreateHeap);
BOOL CheckCreateHeap(daDekuItem_c *a) {
  WWHD_FUNC(0x0211E890, BOOL, a);
  return CreateHeap(a);
}
VERIFY(0x0211E890, CheckCreateHeap);
void set_mtx(daDekuItem_c *a) {
  WWHD_FUNC(0x0211E894, void, a);
  f32 x = a->scale.x, y = a->scale.y;
  u32 model = gabi::ea((void *)a->model);
  f32 z = a->scale.z;
  *at<be<f32>>(model, 0xbc) = x;
  *at<be<f32>>(model, 0xc0) = y;
  *at<be<f32>>(model, 0xc4) = z;
  x = a->current.pos.x;
  y = a->current.pos.y;
  z = a->current.pos.z;
  gabi::call(0x028E93CC, at<void>(0x1048d0cc), x, y, z);
  gabi::call(0x025F1C28, at<void>(0x1048d0cc), (s16)a->current.angle.y);
  f32 matrix[12];
  for (u32 i = 0; i < 12; ++i)
    matrix[i] = *at<be<f32>>(0x1048d0cc, i * 4);
  model = gabi::ea((void *)a->model);
  for (u32 i = 0; i < 12; ++i)
    *at<be<f32>>(model, 0xc8 + i * 4) = matrix[i];
}
VERIFY(0x0211E894, set_mtx);
void CreateInit(daDekuItem_c *a) {
  WWHD_FUNC(0x0211E96C, void, a);
  u32 model = gabi::ea((void *)a->model);
  a->cullMtx = model ? model + 0xc8 : 0;
  gabi::call(0x025D674C, a, -50.0f, 0.0f, -50.0f, 50.0f, 100.0f, 50.0f);
  gabi::call(0x02515F14, sub<void>(a, 0x6d4), 0xff, 0xff, a);
  gabi::call(0x02516518, sub<void>(a, 0x710), at<void>(0x101b4228));
  *sub<be<u32>>(a, 0x754) = gabi::ea(a) + 0x6d4;
  gabi::call(0x024EFF44, sub<void>(a, 0x694), 50.0f, 50.0f);
  gabi::call(0x024F06B4, sub<void>(a, 0x4d0), &a->current.pos, &a->old.pos, a,
             1, sub<void>(a, 0x694), &a->speed, (void *)nullptr,
             (void *)nullptr);
  set_mtx(a);
  a->mode = 0;
  a->gravity = -3.0f;
}
VERIFY(0x0211E96C, CreateInit);
void constructAnimation(daDekuItem_c *a, u32 off) {
  gabi::call(0x027F2BC0, sub<void>(a, off), 0);
  *sub<be<u32>>(a, off + 0x10) = 0x1016e54c;
  gabi::call(0x027DA984, sub<void>(a, off + 0x14));
  *sub<be<u32>>(a, off + 0x48) = 0x1016d820;
  *sub<be<u32>>(a, off + 0x88) = 0;
  *sub<be<u32>>(a, off + 0x7c) = 0;
  *sub<be<u32>>(a, off + 0x84) = 0;
  *sub<be<u32>>(a, off + 0x10) = 0x1000d59c;
  *sub<be<u32>>(a, off + 0x80) = 0;
  *sub<be<u32>>(a, off + 0x58) = 0;
}
s32 create(daDekuItem_c *a) {
  WWHD_FUNC(0x0211EA7C, s32, a);
  u32 condition = a->actor_condition;
  if (!(condition & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, a);
      a->__vtbl = 0x1000d5f4;
      constructAnimation(a, 0x3b8);
      constructAnimation(a, 0x444);
      gabi::call(0x024F0474, sub<void>(a, 0x4d0));
      *sub<be<u32>>(a, 0x4e0) = 0x1000d5c4;
      *sub<be<u32>>(a, 0x4f0) = 0x1000d5d4;
      *sub<be<u32>>(a, 0x4e4) = 0x1000d5e4;
      *sub<be<u8>>(a, 0x4e8) = 1;
      gabi::call(0x024EFE94, sub<void>(a, 0x694));
      gabi::call(0x0200BD2C, sub<void>(a, 0x6d4));
      gabi::call(0x02515DA0, sub<void>(a, 0x6f0));
      *sub<be<u32>>(a, 0x6ec) = 0x1004ae88;
      *sub<be<u32>>(a, 0x6f0) = 0x1004aec0;
      gabi::call(0x02515FB8, sub<void>(a, 0x710));
      *sub<be<u32>>(a, 0x824) = 0x100015a8;
      *sub<be<u32>>(a, 0x820) = 0x1000d58c;
      gabi::call(0x02018590, sub<void>(a, 0x828));
      condition = a->actor_condition;
      *sub<be<u32>>(a, 0x824) = 0x1004b160;
      *sub<be<u32>>(a, 0x74c) = 0x1004b108;
      *sub<be<u32>>(a, 0x83c) = 0x1004b150;
    }
    a->actor_condition = condition | 8;
  }
  u32 bit = *sub<be<u8>>(a, 0xb3);
  s32 room = *sub<be<s8>>(a, 0x2fe);
  a->itemBit = bit;
  if (gabi::call<s32>(0x025BA494, at<void>(word(0x101f84dc), 0x20), bit, room))
    return 5;
  if (!gabi::call<s32>(0x025B8B94, at<void>(word(0x101f84dc), 0x644), 0x1801))
    return 5;
  s32 phase = gabi::call<s32>(0x02520460, &a->phase, at<void>(0x1000d6a0));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, a, at<void>(0x0211e890), 0x5000))
      return 5;
    CreateInit(a);
  }
  return phase;
}
VERIFY(0x0211EA7C, create);
s32 daDekuItem_Create(daDekuItem_c *a) {
  WWHD_FUNC(0x0211ECA8, s32, a);
  return create(a);
}
VERIFY(0x0211ECA8, daDekuItem_Create);
void invalidateEmitter(daDekuItem_c *a) {
  u32 emitter = gabi::ea((void *)a->emitter);
  if (emitter) {
    u32 flags = word(emitter + 0x254);
    put(emitter + 0x5c, 0xffffffff);
    put(emitter + 0x254, flags | 1);
    a->emitter = (void *)nullptr;
  }
}
BOOL remove(daDekuItem_c *a) {
  WWHD_FUNC(0x0211ECAC, BOOL, a);
  invalidateEmitter(a);
  gabi::call(0x025204C8, &a->phase, at<void>(0x1000d6a0));
  return 1;
}
VERIFY(0x0211ECAC, remove);
BOOL daDekuItem_Delete(daDekuItem_c *a) {
  WWHD_FUNC(0x0211ED04, BOOL, a);
  return remove(a);
}
VERIFY(0x0211ED04, daDekuItem_Delete);
BOOL draw(daDekuItem_c *a) {
  WWHD_FUNC(0x0211ED08, BOOL, a);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, at<void>(env), 0, &a->current.pos, &a->tevStr);
  env = gabi::call<u32>(0x02555D0C);
  u32 model = gabi::ea((void *)a->model);
  gabi::call(0x02562F5C, at<void>(env), at<void>(model), &a->tevStr);
  model = gabi::ea((void *)a->model);
  f32 frame = *sub<be<f32>>(a, 0x3bc);
  u32 data = word(model + 0xac);
  gabi::call(0x025E86B8, sub<void>(a, 0x3b8), at<void>(data), frame);
  gabi::call(0x025E2DE0, (void *)a->model, 0);
  return 1;
}
VERIFY(0x0211ED08, draw);
BOOL daDekuItem_Draw(daDekuItem_c *a) {
  WWHD_FUNC(0x0211ED78, BOOL, a);
  return draw(a);
}
VERIFY(0x0211ED78, daDekuItem_Draw);
void checkOrder(daDekuItem_c *a) {
  WWHD_FUNC(0x0211ED7C, void, a);
  if (*sub<be<u16>>(a, 0xf8) != 2)
    return;
  u32 play = gabi::call<u32>(0x025200D4);
  if (gabi::call<s32>(0x025445B8, at<void>(play, 0x52c4),
                      at<void>(0x1000d650)) &&
      a->eventPending)
    a->eventPending = 0;
  play = gabi::call<u32>(0x025200D4);
  if (gabi::call<s32>(0x0254457C, at<void>(play, 0x52c4),
                      at<void>(0x1000d650))) {
    play = gabi::call<u32>(0x025200D4);
    *at<be<u16>>(play, 0x52b8) = (u16)(*at<be<u16>>(play, 0x52b8) | 8);
    gabi::call(0x025D57E0, a);
    s32 room = *sub<be<s8>>(a, 0x2fe);
    u32 save = word(0x101f84dc);
    u32 bit = a->itemBit;
    gabi::call(0x025BA384, at<void>(save, 0x20), bit, room);
  }
}
VERIFY(0x0211ED7C, checkOrder);
void mode_proc_call(daDekuItem_c *a) {
  WWHD_FUNC(0x0211EE34, void, a);
  u32 entry = 0x1000d660 + (u32)a->mode * 8;
  s16 slot = *at<be<s16>>(entry, 2), adjust = *at<be<s16>>(entry);
  void *object = at<void>(gabi::ea(a) + (s32)adjust);
  u32 target;
  if (slot < 0)
    target = word(entry + 4);
  else {
    s16 offset = *at<be<s16>>(entry, 6);
    u32 table = word(gabi::ea(object) + (s32)offset);
    target = word(table + (u32)(s32)slot * 8 + 4);
  }
  gabi::call_ptr(target, object);
}
VERIFY(0x0211EE34, mode_proc_call);
void eventOrder(daDekuItem_c *a) {
  WWHD_FUNC(0x0211EE80, void, a);
  if (a->eventPending == 1) {
    gabi::call(0x025D77DC, a, at<void>(0x1000d680), 1, 0xffff);
    *sub<be<u16>>(a, 0xfa) = (u16)(*sub<be<u16>>(a, 0xfa) | 2);
  }
}
VERIFY(0x0211EE80, eventOrder);
BOOL execute(daDekuItem_c *a) {
  WWHD_FUNC(0x0211EEDC, BOOL, a);
  a->eyePos.copy(a->current.pos);
  gabi::call(0x025D6870, a, sub<void>(a, 0x6d4));
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024F08A8, sub<void>(a, 0x4d0), at<void>(play, 0x12a0));
  checkOrder(a);
  mode_proc_call(a);
  eventOrder(a);
  set_mtx(a);
  *sub<be<f32>>(a, 0x3bc) = (f32)(s16)*sub<be<s16>>(a, 0x3c2);
  return 1;
}
VERIFY(0x0211EEDC, execute);
BOOL daDekuItem_Execute(daDekuItem_c *a) {
  WWHD_FUNC(0x0211EF84, BOOL, a);
  return execute(a);
}
VERIFY(0x0211EF84, daDekuItem_Execute);
void mode_wait(daDekuItem_c *a) {
  WWHD_FUNC(0x0211EF88, void, a);
  s32 hit = gabi::call<s32>(0x02516464, sub<void>(a, 0x710));
  u32 emitter = gabi::ea((void *)a->emitter);
  if (hit)
    a->mode = 1;
  if (!emitter) {
    u32 play = gabi::call<u32>(0x025200D4);
    u32 particles = word(play + 0x5ab0);
    emitter = gabi::call<u32>(0x025A847C, at<void>(particles), 0, 0x820f,
                              &a->current.pos, (void *)nullptr, (void *)nullptr,
                              0xff, (void *)nullptr, -1, (void *)nullptr,
                              (void *)nullptr, (void *)nullptr);
    a->emitter = at<void>(emitter);
  } else {
    f32 y = a->current.pos.y;
    u8 kind = *at<be<u8>>(emitter, 0x262);
    f32 x = a->current.pos.x, z = a->current.pos.z;
    *at<be<f32>>(emitter, 0x22c) = x;
    *at<be<f32>>(emitter, 0x230) = y;
    *at<be<f32>>(emitter, 0x234) = z;
    if (kind >= 7)
      *at<be<f32>>(emitter, 0x230) = -(f32)*at<be<f32>>(emitter, 0x230);
  }
  s32 reverb = gabi::call<s32>(0x02520540, (s32)a->current.roomNo);
  gabi::call(0x025E1A40, 0x7052, &a->eyePos, 0, reverb);
  gabi::call(0x020182E0, sub<void>(a, 0x828), &a->current.pos);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x0200E240, at<void>(play, 0x26a4), sub<void>(a, 0x710));
}
VERIFY(0x0211EF88, mode_wait);
void mode_getdemo_init(daDekuItem_c *a) {
  WWHD_FUNC(0x0211F0CC, void, a);
  gabi::call(0x025DA884, sub<void>(a, 0xdc));
  invalidateEmitter(a);
  a->mode = 2;
  a->eventPending = 1;
}
VERIFY(0x0211F0CC, mode_getdemo_init);
void mode_getdemo_wait(daDekuItem_c *a) {
  WWHD_FUNC(0x0211F134, void, a);
  if (!a->eventPending) {
    s32 room = a->current.roomNo;
    u32 pid = gabi::call<u32>(0x025D7E88, &a->current.pos, 0x34, -1, room,
                              (void *)nullptr, (void *)nullptr);
    a->itemPID = pid;
    if (pid != 0xffffffff) {
      u32 play = gabi::call<u32>(0x025200D4);
      put(play + 0x52a0, pid);
    }
    a->mode = 3;
  }
}
VERIFY(0x0211F134, mode_getdemo_wait);
void sinit() {
  WWHD_FUNC(0x0211F1B0, void);
  put(0x10463ad8, 0);
  put(0x10463ad0, 0);
  put(0x10463adc, 0);
  put(0x10463ad4, 0);
  gabi::call(0x028F026C, at<void>(0x101b426c));
  *at<be<f32>>(0x10463ac4) = -3.1415927410125732f;
  *at<be<f32>>(0x10463ac8) = 3.1415927410125732f;
  gabi::call(0x028ED6F8, at<void>(0x10463acc));
  gabi::call(0x028F026C, at<void>(0x101b4278));
  gabi::call(0x028EAB2C, at<void>(0x10463acd));
  gabi::call(0x028F026C, at<void>(0x101b4284));
}
VERIFY(0x0211F1B0, sinit);
void static_destructor(void *p, u32 flags) {
  WWHD_FUNC(0x0211F244, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x0211F244, static_destructor);
BOOL daDekuItem_IsDelete(daDekuItem_c *a) {
  WWHD_FUNC(0x0211F258, BOOL, a);
  return 1;
}
VERIFY(0x0211F258, daDekuItem_IsDelete);
void mode_getdemo(daDekuItem_c *a) { WWHD_FUNC(0x0211F260, void, a); }
VERIFY(0x0211F260, mode_getdemo);
void destructor(daDekuItem_c *a, u32 flags) {
  WWHD_FUNC(0x0211F264, void, a, flags);
  if (a) {
    gabi::call(0x02515A70, sub<void>(a, 0x710), 2);
    gabi::call(0x02515860, sub<void>(a, 0x6d4), 2);
    gabi::call(0x02018034, sub<void>(a, 0x6a8), 2);
    *sub<be<u32>>(a, 0x4f0) = 0x1000d5d4;
    *sub<be<u32>>(a, 0x4e4) = 0x1000d5e4;
    gabi::call(0x024EFD9C, sub<void>(a, 0x4d0), 0);
    gabi::call(0x027F3628, sub<void>(a, 0x454), 0);
    gabi::call(0x027F3628, sub<void>(a, 0x3c8), 0);
    gabi::call(0x025D50BC, a, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, a);
  }
}
VERIFY(0x0211F264, destructor);
// This TU's SafeString virtual at vtable1000D574+14; actualresourcecallee invokes it.
void SafeString_assureTermination(void *name) {
  WWHD_FUNC(0x0211F318, void, name);
}
VERIFY(0x0211F318, SafeString_assureTermination);
} // namespace DekuItem
