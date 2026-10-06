/* d_kyeff weather process, native 02585AC0..02586BA7.
 * Own inventory: Draw, signed/float ratios, Execute body/wrapper, IsDelete,
 * Delete, Create, and closing static initializer. Previous AA0/AA8/ABC belong
 * to the thunder registration/vtable cluster and are excluded. String-wrapper
 * vtable100504D0 uses the following TU's no-op02586BBC; wrappers remain8bytes.
 * The GC get_parcent is inlined in Execute; no separate native entry is
 * claimed.
 */
#include "gabi.h"
using namespace gabi;
namespace d_kyeff_cpp {
namespace {
u32 environment() { return call<u32>(0x02555D0C); }
u32 game() { return call<u32>(0x025200D4); }
f32 constant(u32 off) { return load<f32>(0x10050000 + off); }
struct StageString {
  be<u32> text, vtable;
};
static_assert(sizeof(StageString) == 8);
// Native string wrappers contain both the text pointer and their real vtable.
bool stageEquals(u32 text) {
  Local<StageString> left, right;
  left->text = text;
  left->vtable = 0x100504D0;
  right->text = game() + 0x5134;
  right->vtable = 0x100504D0;
  call_ptr<void>(load<u32>((u32)left->vtable + 0x14), left.get());
  call_ptr<void>(load<u32>((u32)left->vtable + 0x14), left.get());
  u32 a = left->text;
  call_ptr<void>(load<u32>((u32)right->vtable + 0x14), right.get());
  u32 b = right->text;
  if (a == b)
    return true;
  for (u32 i = 0; i < 0x40001; ++i) {
    u8 x = load<u8>(a + i), y = load<u8>(b + i);
    if (x != y)
      return false;
    if (!x)
      return true;
  }
  return false;
}
} // namespace
u32 weatherDraw() {
  WWHD_FUNC(0x02585AC0, u32);
  call<void>(0x0257CE00);
  return 1;
}
VERIFY(0x02585AC0, weatherDraw);
s16 signedColorRatio(s32 a, s32 b, f32 blend) {
  WWHD_FUNC(0x02585AE4, s16, a, b, blend);
  s32 delta = (s32)((u32)b - (u32)a);
  s16 step = (s16)ftoi(fmuls_ppc((f32)delta, blend));
  return (s16)((u32)a + (s32)step);
}
VERIFY(0x02585AE4, signedColorRatio);
f32 floatRatio(f32 a, f32 b, f32 blend) {
  WWHD_FUNC(0x02585B34, f32, a, b, blend);
  return fmadds(fsubs_ppc(b, a), blend, a);
}
VERIFY(0x02585B34, floatRatio);
u32 weatherExecute() {
  WWHD_FUNC(0x02585B40, u32);
  if (!stageEquals(0x10050540)) {
    call<void>(0x0257836C);
    call<void>(0x0257ACAC);
    return 1;
  }
  u32 camera = load<u32>(game() + 0x5AF8);
  const u32 cameraConstants[] = {0x504, 0x508, 0x50C, 0x510, 0x514, 0x518};
  for (u32 i = 0; i < 6; ++i)
    store<f32>(camera + 0xDC + i * 4, constant(cameraConstants[i]));
  f32 zero = constant(0x508), one = constant(0x53C);
  store<f32>(environment() + 0xA18, constant(0x51C));
  f32 time = load<f32>(environment() + 0x1020);
  u32 env = environment();
  store<f32>(env + 0x1020, fadds_ppc(load<f32>(env + 0x1020), constant(0x520)));
  f32 now = load<f32>(environment() + 0x1020), threshold = constant(0x524);
  u32 integer = now < threshold
                    ? (u32)ftoi(now)
                    : (u32)ftoi(fsubs_ppc(now, threshold)) + 0x80000000u;
  if (!((f32)integer < constant(0x530)))
    store<f32>(environment() + 0x1020, zero);
  call<void>(0x025E1F80, ftoi((f32)((f64)time / constant(0x534))));
  s32 scaled = ftoi(fmuls_ppc(time, constant(0x538)));
  s32 remainder = scaled % 15000000;
  call<void>(0x025E1F90, ftoi((f32)((f64)(f32)remainder / constant(0x538))));
  env = environment();
  store<u32>(env + 0x10, call<u32>(0x02563A1C));
  u32 a = 0, b = 0;
  f32 blend = zero;
  bool found = false;
  for (u32 off = 0; off < 132; off += 12) {
    f32 start = load<f32>(load<u32>(environment() + 0x10) + off);
    if (time < start)
      continue;
    f32 end = load<f32>(load<u32>(environment() + 0x10) + off + 4);
    if (time > end)
      continue;
    a = load<u8>(load<u32>(environment() + 0x10) + off + 8);
    b = load<u8>(load<u32>(environment() + 0x10) + off + 9);
    end = load<f32>(load<u32>(environment() + 0x10) + off + 4);
    start = load<f32>(load<u32>(environment() + 0x10) + off);
    f32 width = fsubs_ppc(end, start);
    blend = one;
    if (width != zero) {
      blend = fsubs_ppc(one, (f32)((f64)fsubs_ppc(end, time) / width));
      if (!(blend < one))
        blend = one;
    }
    found = true;
    break;
  }
  if (!found)
    call<void>(0x0273AA24, at<void>(0x100504F8), 0xF1, at<void>(0x100504CC));
  env = environment();
  store<u32>(env + 0xC, call<u32>(0x025639F8));
  u32 palA = load<u32>(environment() + 0xC) + a * 56;
  u32 palB = load<u32>(environment() + 0xC) + b * 56;
  auto byteColor = [&](u32 src, u32 dest) {
    u32 out = environment();
    s32 x = load<u8>(palA + src), y = load<u8>(palB + src);
    store<u8>(out + dest, call<s16>(0x02585AE4, x, y, blend));
  };
  for (u32 i = 0; i < 3; ++i)
    byteColor(0x18 + i, 0xB90 + i);
  store<u8>(environment() + 0xB93, 255);
  for (u32 i = 0; i < 3; ++i)
    byteColor(0x1B + i, 0xB94 + i);
  env = environment();
  s16 alphaA = (s16)((s16)ftoi(load<f32>(palA + 0x34)) * 255);
  s16 alphaB = (s16)((s16)ftoi(load<f32>(palB + 0x34)) * 255);
  store<u8>(env + 0xB97,
            call<s16>(0x02585AE4, (s32)alphaA, (s32)alphaB, blend));
  for (u32 i = 0; i < 7; ++i)
    byteColor(0x10 + i, 0xB98 + i);
  for (u32 i = 0; i < 3; ++i)
    byteColor(0x1E + i, 0xBA0 + i);
  store<u8>(environment() + 0xBA3, 255);
  const u32 sourceFloat[] = {0x2C, 0x24, 0x28, 0x30};
  const u32 destFloat[] = {0x10D4, 0x10DC, 0x10E0, 0x10E4};
  for (u32 i = 0; i < 4; ++i) {
    env = environment();
    f32 x = load<f32>(palA + sourceFloat[i]),
        y = load<f32>(palB + sourceFloat[i]);
    store<f32>(env + destFloat[i], call<f32>(0x02585B34, x, y, blend));
  }
  for (u32 i = 0; i < 3; ++i)
    byteColor(0x1E + i, 0xB8C + i);
  call<void>(0x0257ACAC);
  return 1;
}
VERIFY(0x02585B40, weatherExecute);
u32 weatherExecuteWrapper() {
  WWHD_FUNC(0x02586254, u32);
  return call<u32>(0x02585B40);
}
VERIFY(0x02586254, weatherExecuteWrapper);
u32 weatherIsDelete() {
  WWHD_FUNC(0x02586258, u32);
  return 1;
}
VERIFY(0x02586258, weatherIsDelete);
u32 weatherDelete(void *self) {
  WWHD_FUNC(0x02586260, u32, self);
  if (self)
    call<void>(0x025DD630, self, 0);
  call<void>(0x0257802C);
  return 1;
}
VERIFY(0x02586260, weatherDelete);
u32 weatherCreate(void *self) {
  WWHD_FUNC(0x02586294, u32, self);
  if (self) {
    call<void>(0x025DD5F0, self);
    store<u32>(ea(self) + 0xB4, 0x100504E8);
  }
  call<void>(0x02577CB0);
  if (stageEquals(0x1005056C)) {
    store<f32>(environment() + 0x9FC, constant(0x53C));
    store<f32>(environment() + 0xA00, constant(0x508));
    store<f32>(environment() + 0xA04, constant(0x508));
    store<f32>(environment() + 0xA18, constant(0x51C));
    store<f32>(environment() + 0x1020, constant(0x548));
    return 4;
  }
  if (!call<u32>(0x02556BC0))
    return 4;
  u32 stage = game() + 0x5150;
  u32 info =
      call_ptr<u32>(load<u32>(load<u32>(stage) + 0x15C), at<void>(stage));
  u32 type = (load<u32>(info + 0xC) >> 16) & 7;
  if (type == 0 || type == 7) {
    call<void>(0x0257E7C0, 250);
    store<u32>(environment() + 0xAB4, 1);
    return 4;
  }
  if (type != 2)
    return 4;
  const u32 names[] = {0x574, 0x564, 0x57C, 0x54C, 0x584,
                       0x554, 0x55C, 0x58C, 0x594, 0x59C};
  for (u32 name : names)
    if (stageEquals(0x10050000 + name)) {
      call<void>(0x0257E7C0, 250);
      store<u32>(environment() + 0xAB4, 10);
      break;
    }
  return 4;
}
VERIFY(0x02586294, weatherCreate);
void weatherStaticInit() {
  WWHD_FUNC(0x02586B14, void);
  store<u32>(0x104775EC, 0);
  store<u32>(0x104775E4, 0);
  store<u32>(0x104775F0, 0);
  store<u32>(0x104775E8, 0);
  call<void>(0x028F026C, at<void>(0x101E9AE4));
  store<f32>(0x104775D8, constant(0x5A8));
  store<f32>(0x104775DC, constant(0x5AC));
  call<void>(0x028ED6F8, at<void>(0x104775E0));
  call<void>(0x028F026C, at<void>(0x101E9AF0));
  call<void>(0x028EAB2C, at<void>(0x104775E1));
  call<void>(0x028F026C, at<void>(0x101E9AFC));
}
VERIFY(0x02586B14, weatherStaticInit);


/* 02586BA8 this TU's sead::SafeString copy (vtable 100504D0): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void SafeString_deletingDtor_d_kyeff(u32 p, u32 flags) {
    WWHD_FUNC(0x02586BA8, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x02586BA8, SafeString_deletingDtor_d_kyeff);

/* 02586BBC this TU's sead::SafeString copy assureTerminationImpl_ (vtable 100504D0): empty function */
static void SafeString_assureTerminationImpl_d_kyeff(u32 p) {
    WWHD_FUNC(0x02586BBC, void, p);
}
VERIFY(0x02586BBC, SafeString_assureTerminationImpl_d_kyeff);

} // namespace d_kyeff_cpp
