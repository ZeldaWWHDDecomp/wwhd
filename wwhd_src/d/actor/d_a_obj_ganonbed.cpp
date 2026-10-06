/* Puppet Ganon intro bed. Full actual HD TU02349E18..0234A2EF,
 * including the unnamed heap callback, static initialization and cleanup.
 */
#include "d/actor/d_a_obj_ganonbed.h"
#include "bindings.h"
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
struct GbedArchive {
  be<u32> name, vt;
};
u32 resource(s32 index) {
  gabi::Local<GbedArchive> archive;
  archive->name = 0x1002955C;
  archive->vt = 0x10029564;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(),
                         index);
}
u32 play() { return gabi::call<u32>(0x025200D4); }
} // namespace
BOOL gbed_createHeap(daObjGbed_c *bed) {
  WWHD_FUNC(0x02349E18, BOOL, bed);
  u32 data = resource(4);
  if (!data) {
    gabi::call(0x0273AA24, ptr(0x10029590), 177, ptr(0x1002958C));
    return 0;
  }
  bed->model = gabi::call<u32>(0x025E38E0, ptr(data), 0x80000, 0x11000022);
  if (!bed->model)
    return 0;
  u32 dzb = resource(7), model = bed->model;
  bed->background =
      gabi::call<u32>(0x024F2478, ptr(dzb), 1, ptr(model ? model + 0xC8 : 0));
  return bed->model && bed->background;
}
VERIFY(0x02349E18, gbed_createHeap);
BOOL gbed_heapCallback(daObjGbed_c *bed) {
  WWHD_FUNC(0x02349F04, BOOL, bed);
  return gabi::call<BOOL>(0x02349E18, bed);
}
VERIFY(0x02349F04, gbed_heapCallback);
void gbed_initMatrix(daObjGbed_c *bed) {
  WWHD_FUNC(0x02349F08, void, bed);
  J3DModel_setBaseScale(gabi::at<J3DModel>(bed->model), &bed->scale);
  mDoMtx_stack_c::transS(bed->current.pos.x, bed->current.pos.y,
                         bed->current.pos.z);
  gabi::call(0x025F19F8, mDoMtx_stack_c::get(), (s32)(s16)bed->shape_angle.x,
             (s32)(s16)bed->shape_angle.y, (s32)(s16)bed->shape_angle.z);
  J3DModel_setBaseTRMtx(gabi::at<J3DModel>(bed->model), mDoMtx_stack_c::get());
}
VERIFY(0x02349F08, gbed_initMatrix);
s32 gbed_create(daObjGbed_c *bed) {
  WWHD_FUNC(0x02349FE8, s32, bed);
  u32 a = gabi::ea(bed);
  if (!(read<u32>(a, 0x2E4) & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, bed);
      write<u32>(a, 0xB4, 0x1002957C);
    }
    write<u32>(a, 0x2E4, read<u32>(a, 0x2E4) | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &bed->phase, ptr(0x1002955C));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, bed, ptr(0x02349F04), 0x13E0))
      return 5;
    u32 game = play();
    if (gabi::call<s32>(0x024EEA6C, ptr(game, 0x12A0), ptr(bed->background),
                        bed))
      return 5;
    u32 model = bed->model;
    write<u32>(a, 0x348, model ? model + 0xC8 : 0);
    gabi::call(0x02349F08, bed);
  }
  return phase;
}
VERIFY(0x02349FE8, gbed_create);
s32 gbed_Create(daObjGbed_c *bed) {
  WWHD_FUNC(0x0234A0C8, s32, bed);
  return gabi::call<s32>(0x02349FE8, bed);
}
VERIFY(0x0234A0C8, gbed_Create);
BOOL gbed_delete(daObjGbed_c *bed) {
  WWHD_FUNC(0x0234A0CC, BOOL, bed);
  gabi::call(0x025204C8, &bed->phase, ptr(0x1002955C));
  if (read<u32>(gabi::ea(bed), 0xF4) && bed->background) {
    if (read<u32>(bed->background) < 256) {
      u32 game = play();
      gabi::call(0x020087EC, ptr(game, 0x12A0), ptr(bed->background));
    }
    bed->background = 0;
  }
  return 1;
}
VERIFY(0x0234A0CC, gbed_delete);
BOOL gbed_Delete(daObjGbed_c *bed) {
  WWHD_FUNC(0x0234A144, BOOL, bed);
  return gabi::call<BOOL>(0x0234A0CC, bed);
}
VERIFY(0x0234A144, gbed_Delete);
BOOL gbed_execute(daObjGbed_c *bed) {
  WWHD_FUNC(0x0234A148, BOOL, bed);
  u32 background = bed->background;
  if (background && read<u32>(background) < 256)
    gabi::call(0x024F43DC, ptr(background));
  return 1;
}
VERIFY(0x0234A148, gbed_execute);
BOOL gbed_Execute(daObjGbed_c *bed) {
  WWHD_FUNC(0x0234A184, BOOL, bed);
  return gabi::call<BOOL>(0x0234A148, bed);
}
VERIFY(0x0234A184, gbed_Execute);
BOOL gbed_draw(daObjGbed_c *bed) {
  WWHD_FUNC(0x0234A188, BOOL, bed);
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(light), 0, &bed->current.pos, &bed->tevStr);
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(light), ptr(bed->model), &bed->tevStr);
  gabi::call(0x025E2DE0, ptr(bed->model), 0);
  return 1;
}
VERIFY(0x0234A188, gbed_draw);
BOOL gbed_Draw(daObjGbed_c *bed) {
  WWHD_FUNC(0x0234A1E4, BOOL, bed);
  return gabi::call<BOOL>(0x0234A188, bed);
}
VERIFY(0x0234A1E4, gbed_Draw);
BOOL gbed_IsDelete() {
  WWHD_FUNC(0x0234A1E8, BOOL);
  return 1;
}
VERIFY(0x0234A1E8, gbed_IsDelete);
void gbed_staticInit() {
  WWHD_FUNC(0x0234A1F0, void);
  write<u32>(0x10469BE4, 8, 0);
  write<u32>(0x10469BE4, 0, 0);
  write<u32>(0x10469BE4, 12, 0);
  write<u32>(0x10469BE4, 4, 0);
  gabi::call(0x028F026C, ptr(0x101C971C));
  write<f32>(0x10469BD8, 0, read<f32>(0x100295AC));
  write<f32>(0x10469BDC, 0, read<f32>(0x100295B0));
  gabi::call(0x028ED6F8, ptr(0x10469BE0));
  gabi::call(0x028F026C, ptr(0x101C9728));
  gabi::call(0x028EAB2C, ptr(0x10469BE1));
  gabi::call(0x028F026C, ptr(0x101C9734));
}
VERIFY(0x0234A1F0, gbed_staticInit);
void gbed_trivialDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x0234A284, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x0234A284, gbed_trivialDestructor);
void gbed_actorDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x0234A298, void, object, flags);
  if (object) {
    gabi::call(0x025D50BC, object, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, object);
  }
}
VERIFY(0x0234A298, gbed_actorDestructor);
void gbed_noop(void *object) { WWHD_FUNC(0x0234A2EC, void, object); }
VERIFY(0x0234A2EC, gbed_noop);
