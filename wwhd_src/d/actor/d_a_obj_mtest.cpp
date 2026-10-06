// Eight model/collision test-object variants. Full actual HD
// TU0237DC40..0237E8B3.
#include "d/actor/d_a_obj_mtest.h"
#include "bindings.h"
using Test = daObjMtest::Act_c;
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
u32 play() { return gabi::call<u32>(0x025200D4); }
s32 parameter(Test *test, s32 width, s32 shift) {
  return gabi::call<s32>(0x0237E898, test, width, shift);
}
u32 archiveName(s32 type) { return read<u32>(0x101CBFC8, (u32)type * 4); }
struct TestArchive {
  be<u32> name, vt;
};
u32 resource(s32 type, s32 index) {
  gabi::Local<TestArchive> archive;
  archive->name = archiveName(type);
  archive->vt = 0x1002DC60;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(),
                         index);
}
} // namespace
BOOL mtest_checkAppear(Test *test) {
  WWHD_FUNC(0x0237DC40, BOOL, test);
  s32 save = parameter(test, 8, 8), arg = parameter(test, 4, 16);
  if (arg == 0 || arg == 1) {
    s32 enabled = gabi::call<s32>(0x025BA0C0, ptr(read<u32>(0x101F84DC), 0x20),
                                  save, (s32)(s8)test->home.roomNo);
    return arg == 0 ? enabled != 0 : enabled == 0;
  }
  return 1;
}
VERIFY(0x0237DC40, mtest_checkAppear);
s32 mtest_methodCreate(Test *test) {
  WWHD_FUNC(0x0237DD18, s32, test);
  u32 a = gabi::ea(test);
  if (!(read<u32>(a, 0x2E4) & 8)) {
    if (a) {
      gabi::call(0x024F1D40, test);
      write<u32>(a, 0xB4, 0x1002DD9C);
      gabi::call(0x0200BD2C, &test->status);
      gabi::call(0x02515DA0, ptr(a, 0x438));
      write<u32>(a, 0x434, 0x1004AE88);
      write<u32>(a, 0x438, 0x1004AEC0);
      gabi::call(0x02515FB8, &test->cylinder);
      write<u32>(a, 0x56C, 0x100015A8);
      write<u32>(a, 0x568, 0x1002DC78);
      gabi::call(0x02018590, ptr(a, 0x570));
      write<u32>(a, 0x56C, 0x1004B160);
      write<u32>(a, 0x494, 0x1004B108);
      write<u32>(a, 0x584, 0x1004B150);
    }
    write<u32>(a, 0x2E4, read<u32>(a, 0x2E4) | 8);
  }
  s32 type = parameter(test, 3, 0);
  test->type = type;
  if (type >= 8) {
    gabi::call(0x0273AA24, ptr(0x1002DC88), 335, ptr(0x1002DC9C));
    type = test->type;
  }
  s32 phase = gabi::call<s32>(0x02520460, &test->phase, ptr(archiveName(type)));
  if (phase == 4) {
    s32 arg = parameter(test, 4, 24), row = arg >= 1 && arg <= 4 ? arg : 0;
    test->appear = (u8)gabi::call<s32>(0x0237DC40, test);
    type = test->type;
    u32 mult = 0x1046BA14 + (u32)type * 12;
    test->scale.x = (f32)test->scale.x * read<f32>(mult);
    test->scale.y = (f32)test->scale.y * read<f32>(mult, 4);
    test->scale.z = (f32)test->scale.z * read<f32>(mult, 8);
    u32 index = (u32)row * 8 + (u32)type;
    phase = gabi::call<s32>(0x024F1D9C, test, ptr(archiveName(type)),
                            (s32)read<s16>(0x101CBE74, index * 2), ptr(0),
                            read<u32>(0x101CBEC4, index * 4));
    if (phase != 4 && phase != 5)
      gabi::call(0x0273AA24, ptr(0x1002DC88), 458, ptr(0x1002DCB0));
    else if (phase == 4) {
      write<u8>(test->background, 0x6C,
                read<u8>(test->background, 0x6C) | 0x80);
      if (!test->appear)
        gabi::call(0x020087EC, ptr(play(), 0x12A0), ptr(test->background));
    }
  }
  return phase;
}
VERIFY(0x0237DD18, mtest_methodCreate);
s32 mtest_methodDelete(Test *test) {
  WWHD_FUNC(0x0237E114, s32, test);
  s32 result = gabi::call<s32>(0x024F1F64, test);
  gabi::call(0x025204C8, &test->phase, ptr(archiveName(test->type)));
  return result;
}
VERIFY(0x0237E114, mtest_methodDelete);
BOOL mtest_createHeap(Test *test) {
  WWHD_FUNC(0x0237E16C, BOOL, test);
  s32 index = read<s16>(0x101CBF64, (u32)test->type * 2);
  if (index < 0) {
    test->model = 0;
    return 1;
  }
  u32 data = resource(test->type, index);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x1002DD04), 265, ptr(0x1002DCF4));
  test->model = gabi::call<u32>(0x025E38E0, ptr(data), 0, 0x11020203);
  return test->model != 0;
}
VERIFY(0x0237E16C, mtest_createHeap);
void mtest_setMatrix(Test *test) {
  WWHD_FUNC(0x0237E24C, void, test);
  mDoMtx_stack_c::transS(test->current.pos.x, test->current.pos.y,
                         test->current.pos.z);
  gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s32)(s16)test->shape_angle.x,
             (s32)(s16)test->shape_angle.y, (s32)(s16)test->shape_angle.z);
  if (test->model)
    J3DModel_setBaseTRMtx(gabi::at<J3DModel>(test->model),
                          mDoMtx_stack_c::get());
  mDoMtx_stack_c::scaleM(test->scale.x, test->scale.y, test->scale.z);
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &test->matrix);
}
VERIFY(0x0237E24C, mtest_setMatrix);
void mtest_initMatrix(Test *test) {
  WWHD_FUNC(0x0237E334, void, test);
  if (test->model)
    J3DModel_setBaseScale(gabi::at<J3DModel>(test->model), &test->scale);
  gabi::call(0x0237E24C, test);
}
VERIFY(0x0237E334, mtest_initMatrix);
BOOL mtest_Create(Test *test) {
  WWHD_FUNC(0x0237E35C, BOOL, test);
  u32 a = gabi::ea(test);
  write<u32>(a, 0x348, a + 0x3E0);
  gabi::call(0x0237E334, test);
  if (test->type == 4 || test->type == 5)
    gabi::call(0x025D674C, test, -3000.f, -10.f, -3000.f, 3000.f, 10.f, 3000.f);
  else if (test->type != 6)
    gabi::call(0x025D674C, test, -51.f, -1.f, -51.f, 51.f, 101.f, 51.f);
  if (test->type == 7)
    write<u32>(a, 0x2E0, read<u32>(a, 0x2E0) | 0x80);
  if (parameter(test, 4, 24) == 0) {
    gabi::call(0x02515F14, &test->status, 255, 255, test);
    gabi::call(0x02516518, &test->cylinder, ptr(0x1002DD58));
    write<u32>(a, 0x49C, a + 0x41C);
    f32 x = test->scale.x, z = test->scale.z;
    write<u32>(a, 0x50C, read<u32>(0x101FFBA8));
    write<u32>(a, 0x510, read<u32>(0x101FFBA8, 4));
    write<u32>(a, 0x514, read<u32>(0x101FFBA8, 8));
    write<u32>(a, 0x4EC, read<u32>(a, 0x4EC) | 4);
    f32 smaller = ((f64)z - (f64)x) >= 0. ? x : z;
    gabi::call(0x020184DC, ptr(a, 0x570), 50.f * smaller);
    gabi::call(0x02018428, ptr(a, 0x570), 100.f * (f32)test->scale.y);
  }
  return 1;
}
VERIFY(0x0237E35C, mtest_Create);
BOOL mtest_Execute(Test *test, be<u32> *matrixOut) {
  WWHD_FUNC(0x0237E4D8, BOOL, test, matrixOut);
  s32 appear = gabi::call<s32>(0x0237DC40, test);
  if (gabi::call<s32>(0x025162A4, &test->cylinder))
    gabi::call(0x025D57E0, test);
  else if (appear) {
    if (!test->appear)
      gabi::call(0x024EEA6C, ptr(play(), 0x12A0), ptr(test->background), test);
    if (parameter(test, 4, 24) == 0) {
      gabi::call(0x025165A4, &test->cylinder, &test->current.pos);
      gabi::call(0x0200E240, ptr(play(), 0x26A4), &test->cylinder);
    }
  } else if (test->appear)
    gabi::call(0x020087EC, ptr(play(), 0x12A0), ptr(test->background));
  *matrixOut = gabi::ea(&test->matrix);
  test->appear = (u8)appear;
  return 1;
}
VERIFY(0x0237E4D8, mtest_Execute);
BOOL mtest_Draw(Test *test) {
  WWHD_FUNC(0x0237E5E0, BOOL, test);
  if (test->appear && test->model) {
    s32 arg = parameter(test, 4, 24);
    if (arg != 3 && arg != 4) {
      u32 light = gabi::call<u32>(0x02555D0C);
      gabi::call(0x025626A4, ptr(light), 1, &test->current.pos, &test->tevStr);
      light = gabi::call<u32>(0x02555D0C);
      gabi::call(0x02562F5C, ptr(light), ptr(test->model), &test->tevStr);
      write<u32>(0x104B4634, 0, read<u32>(play(), 0x5D70));
      write<u32>(0x104B4634, 4, read<u32>(play(), 0x5D74));
      gabi::call(0x025E2DE0, ptr(test->model), 0);
      write<u32>(0x104B4634, 0, read<u32>(play(), 0x5D78));
      write<u32>(0x104B4634, 4, read<u32>(play(), 0x5D7C));
    }
  }
  return 1;
}
VERIFY(0x0237E5E0, mtest_Draw);
s32 mtest_createWrapper(Test *test) {
  WWHD_FUNC(0x0237E6B0, s32, test);
  return gabi::call<s32>(0x0237DD18, test);
}
VERIFY(0x0237E6B0, mtest_createWrapper);
s32 mtest_deleteWrapper(Test *test) {
  WWHD_FUNC(0x0237E6B4, s32, test);
  return gabi::call<s32>(0x0237E114, test);
}
VERIFY(0x0237E6B4, mtest_deleteWrapper);
s32 mtest_executeWrapper(Test *test) {
  WWHD_FUNC(0x0237E6B8, s32, test);
  return gabi::call<s32>(0x024F1E9C, test);
}
VERIFY(0x0237E6B8, mtest_executeWrapper);
s32 mtest_drawWrapper(Test *test) {
  WWHD_FUNC(0x0237E6BC, s32, test);
  return gabi::call_ptr<s32>(read<u32>(read<u32>(gabi::ea(test), 0xB4), 0x2C),
                             test);
}
VERIFY(0x0237E6BC, mtest_drawWrapper);
s32 mtest_isDeleteWrapper(Test *test) {
  WWHD_FUNC(0x0237E6CC, s32, test);
  return gabi::call_ptr<s32>(read<u32>(read<u32>(gabi::ea(test), 0xB4), 0x3C),
                             test);
}
VERIFY(0x0237E6CC, mtest_isDeleteWrapper);
void mtest_staticInit() {
  WWHD_FUNC(0x0237E6DC, void);
  for (s32 i = 3; i >= 0; i--)
    write<u32>(0x1046BA04, i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CBF74));
  write<f32>(0x1046B9F8, 0, read<f32>(0x1002DD40));
  write<f32>(0x1046B9FC, 0, read<f32>(0x1002DD44));
  gabi::call(0x028ED6F8, ptr(0x1046BA00));
  gabi::call(0x028F026C, ptr(0x101CBF80));
  gabi::call(0x028EAB2C, ptr(0x1046BA01));
  gabi::call(0x028F026C, ptr(0x101CBF8C));
  const f32 values[24] = {1,   1,   1,   1,   .5f, 1,   10, 10, 10, 10, 5, 10,
                          .2f, .2f, .2f, .2f, .2f, .2f, 1,  1,  1,  40, 1, 40};
  for (u32 i = 0; i < 24; i++)
    write<f32>(0x1046BA14, i * 4, values[i]);
}
VERIFY(0x0237E6DC, mtest_staticInit);
void mtest_staticDestructor(void *object, s32 flags) {
  WWHD_FUNC(0x0237E804, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x0237E804, mtest_staticDestructor);
BOOL mtest_baseIsDelete(Test *test) {
  WWHD_FUNC(0x0237E818, BOOL, test);
  return 1;
}
VERIFY(0x0237E818, mtest_baseIsDelete);
void mtest_baseNoop() { WWHD_FUNC(0x0237E820, void); }
VERIFY(0x0237E820, mtest_baseNoop);
BOOL mtest_Delete(Test *test) {
  WWHD_FUNC(0x0237E824, BOOL, test);
  return 1;
}
VERIFY(0x0237E824, mtest_Delete);
void mtest_destructor(Test *test, s32 flags) {
  WWHD_FUNC(0x0237E82C, void, test, flags);
  if (test) {
    gabi::call(0x02515A70, &test->cylinder, 2);
    gabi::call(0x02515860, &test->status, 2);
    gabi::call(0x025D50BC, test, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, test);
  }
}
VERIFY(0x0237E82C, mtest_destructor);
u32 mtest_parameter(Test *test, u32 width, u32 shift) {
  WWHD_FUNC(0x0237E898, u32, test, width, shift);
  u32 mask = (width & 32) ? 0 : 1u << (width & 31),
      value = (shift & 32) ? 0 : ((u32)test->mParameters >> (shift & 31));
  return value & (mask - 1);
}
VERIFY(0x0237E898, mtest_parameter);
