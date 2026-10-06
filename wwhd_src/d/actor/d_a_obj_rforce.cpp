// Actual HD TU 02388388..02388907, including all unnamed wrappers and cleanup.
#include "d/actor/d_a_obj_rforce.h"
#include "bindings.h"
using Force = daObjRforce::Act_c;
namespace {
template <class T> T read(u32 address, u32 offset = 0) {
  return *gabi::at<be<T>>(address + offset);
}
template <class T> void write(u32 address, u32 offset, T value) {
  *gabi::at<be<T>>(address + offset) = value;
}
void *ptr(u32 address, u32 offset = 0) {
  return gabi::at<void>(address + offset);
}
struct ForceArchive {
  be<u32> name, vt;
};
u32 resource(s32 index) {
  gabi::Local<ForceArchive> archive;
  archive->name = 0x1002EDD8;
  archive->vt = 0x1002ED70;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(),
                         index);
}
u32 play() { return gabi::call<u32>(0x025200D4); }
} // namespace
void force_setMatrix(Force *force) {
  WWHD_FUNC(0x02388388, void, force);
  J3DModel_setBaseScale(gabi::at<J3DModel>(force->model), &force->scale);
  mDoMtx_stack_c::transS(force->current.pos.x, force->current.pos.y,
                         force->current.pos.z);
  gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s32)(s16)force->shape_angle.x,
             (s32)(s16)force->shape_angle.y, (s32)(s16)force->shape_angle.z);
  J3DModel_setBaseTRMtx(gabi::at<J3DModel>(force->model),
                        mDoMtx_stack_c::get());
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &force->matrix);
  gabi::call(0x027F4D5C, ptr(force->model));
}
VERIFY(0x02388388, force_setMatrix);
BOOL force_createHeap(Force *force) {
  WWHD_FUNC(0x0238847C, BOOL, force);
  u32 data = resource(4);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x1002EDB8), 87, ptr(0x1002ED98));
  force->model = gabi::call<u32>(0x025E38E0, ptr(data), 0, 0x11000002);
  gabi::call(0x02388388, force);
  u32 dzb = resource(7);
  if (!dzb)
    gabi::call(0x0273AA24, ptr(0x1002EDB8), 100, ptr(0x1002EDA8));
  else {
    force->background = gabi::call<u32>(0x024F23F4, ptr(0));
    if (force->background && gabi::call<s32>(0x0200A030, ptr(force->background),
                                             ptr(dzb), 1, &force->matrix))
      return 0;
  }
  return data && force->model && dzb && force->background;
}
VERIFY(0x0238847C, force_createHeap);
BOOL force_heapCallback(Force *force) {
  WWHD_FUNC(0x023885CC, BOOL, force);
  return gabi::call<BOOL>(0x0238847C, force);
}
VERIFY(0x023885CC, force_heapCallback);
s32 force_create(Force *force) {
  WWHD_FUNC(0x023885D0, s32, force);
  u32 a = gabi::ea(force);
  if (!(read<u32>(a, 0x2E4) & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, force);
      write<u32>(a, 0xB4, 0x1002ED88);
    }
    write<u32>(a, 0x2E4, read<u32>(a, 0x2E4) | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &force->phase, ptr(0x1002EDD8));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, force, ptr(0x023885CC), 0))
      return 5;
    u32 model = force->model;
    write<u32>(a, 0x348, model ? model + 0xC8 : 0);
    gabi::call(0x024EEA6C, ptr(play(), 0x12A0), ptr(force->background), force);
    write<u32>(force->background, 0xA8, 0x024EE658);
  }
  return phase;
}
VERIFY(0x023885D0, force_create);
BOOL force_delete(Force *force) {
  WWHD_FUNC(0x023886B0, BOOL, force);
  if (read<u32>(gabi::ea(force), 0xF4) && force->background &&
      read<u32>(force->background) < 256)
    gabi::call(0x020087EC, ptr(play(), 0x12A0), ptr(force->background));
  gabi::call(0x025204C8, &force->phase, ptr(0x1002EDD8));
  return 1;
}
VERIFY(0x023886B0, force_delete);
BOOL force_execute(Force *force) {
  WWHD_FUNC(0x02388720, BOOL, force);
  gabi::call(0x02388388, force);
  gabi::call(0x024F43DC, ptr(force->background));
  return 1;
}
VERIFY(0x02388720, force_execute);
BOOL force_draw(Force *force) {
  WWHD_FUNC(0x02388758, BOOL, force);
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(light), 1, &force->current.pos, &force->tevStr);
  write<u32>(0x104B4634, 0, read<u32>(play(), 0x5D70));
  write<u32>(0x104B4634, 4, read<u32>(play(), 0x5D74));
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(light), ptr(force->model), &force->tevStr);
  gabi::call(0x025E2DE0, ptr(force->model), 0);
  write<u32>(0x104B4634, 0, read<u32>(play(), 0x5D78));
  write<u32>(0x104B4634, 4, read<u32>(play(), 0x5D7C));
  return 1;
}
VERIFY(0x02388758, force_draw);
s32 force_Create(Force *force) {
  WWHD_FUNC(0x023887F0, s32, force);
  return gabi::call<s32>(0x023885D0, force);
}
VERIFY(0x023887F0, force_Create);
BOOL force_Delete(Force *force) {
  WWHD_FUNC(0x023887F4, BOOL, force);
  return gabi::call<BOOL>(0x023886B0, force);
}
VERIFY(0x023887F4, force_Delete);
BOOL force_Execute(Force *force) {
  WWHD_FUNC(0x023887F8, BOOL, force);
  return gabi::call<BOOL>(0x02388720, force);
}
VERIFY(0x023887F8, force_Execute);
BOOL force_Draw(Force *force) {
  WWHD_FUNC(0x023887FC, BOOL, force);
  return gabi::call<BOOL>(0x02388758, force);
}
VERIFY(0x023887FC, force_Draw);
void force_staticInit() {
  WWHD_FUNC(0x02388800, void);
  for (s32 i = 3; i >= 0; i--)
    write<u32>(0x1046BD68, i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CC8B8));
  write<f32>(0x1046BD5C, 0, read<f32>(0x1002EDCC));
  write<f32>(0x1046BD60, 0, read<f32>(0x1002EDD0));
  gabi::call(0x028ED6F8, ptr(0x1046BD64));
  gabi::call(0x028F026C, ptr(0x101CC8C4));
  gabi::call(0x028EAB2C, ptr(0x1046BD65));
  gabi::call(0x028F026C, ptr(0x101CC8D0));
}
VERIFY(0x02388800, force_staticInit);
void force_staticDestructor(void *object, s32 flags) {
  WWHD_FUNC(0x02388894, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x02388894, force_staticDestructor);
void force_noop() { WWHD_FUNC(0x023888A8, void); }
VERIFY(0x023888A8, force_noop);
void force_destructor(Force *force, s32 flags) {
  WWHD_FUNC(0x023888AC, void, force, flags);
  if (force) {
    gabi::call(0x025D50BC, force, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, force);
  }
}
VERIFY(0x023888AC, force_destructor);
BOOL force_IsDelete(Force *force) {
  WWHD_FUNC(0x02388900, BOOL, force);
  return 1;
}
VERIFY(0x02388900, force_IsDelete);
