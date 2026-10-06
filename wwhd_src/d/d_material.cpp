// HD replacement material setup and render wrappers,02590FB8..0259169B.
#include "bindings.h"
namespace d_material_cpp {
struct StringPair { be<u32> text, vtable; };
static void transfer(u32 dst, u32 src) {
 bool same = true;
 for (u32 off : {4u,8u,12u,16u,20u,24u,56u,52u,28u}) {
  u32 a = gabi::load<u32>(dst + off);
  u32 b = gabi::load<u32>(src + off);
  if (a != b) { same = false; break; }
 }
 if (!same) gabi::call<void>(0x027BDEB4, dst, src);
 else {
  u32 a = gabi::load<u32>(src + 40);
  u32 b = gabi::load<u32>(src + 48);
  gabi::store<u32>(dst + 40, a);
  gabi::store<u32>(dst + 220, b);
  gabi::store<u32>(dst + 212, a);
  gabi::store<u32>(dst + 48, b);
 }
}
static void Setup(u32 self) {
 WWHD_FUNC(0x02590FB8, void, self);
 gabi::Local<StringPair> first, second;
 first->text = 0x10050FE4; first->vtable = 0x10050FCC;
 u32 resources = gabi::load<u32>(0x101F4F28);
 u32 a = gabi::call<u32>(0x026066C4, resources, first.get(), 0x92u);
 second->vtable = 0x10050FCC;
 resources = gabi::load<u32>(0x101F4F28);
 second->text = 0x10050FE4;
 u32 b = gabi::call<u32>(0x026066C4, resources, second.get(), 0x93u);
 if (a == 0 || b == 0) return;
 gabi::call<void>(0x02773798, self, gabi::load<u32>(a + 32));
 gabi::call<void>(0x02773798, self + 0x90, gabi::load<u32>(b + 32));
 transfer(self + 0x120, self);
 transfer(self + 0x2B8, self + 0x90);
 gabi::store<u32>(0x104B45C4, self + 0x2B8);
 gabi::store<u32>(0x104B45C0, self + 0x120);
}
VERIFY(0x02590FB8, Setup);
static void UpdateModel(u32 model, s8 unusedBackup, u32 invisible) {
 WWHD_FUNC(0x02591200, void, model, unusedBackup, invisible);
 u32 old0 = gabi::load<u32>(0x104B4634);
 u32 old1 = gabi::load<u32>(0x104B4638);
 u32 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4634, gabi::load<u32>(game + 0x5D78));
 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4638, gabi::load<u32>(game + 0x5D7C));
 u32 actual = model;
 gabi::store<u8>(actual + 0x78, 1);
 gabi::call<void>(0x025E2DE0, model, 0u);
 actual = model;
 gabi::store<u8>(actual + 0x78, 0);
 if (invisible != 0) gabi::call<void>(0x025E8BC4, invisible);
 gabi::store<u32>(0x104B4634, old0);
 gabi::store<u32>(0x104B4638, old1);
}
VERIFY(0x02591200, UpdateModel);
static void UpdateMorf(u32 model, s8 unusedBackup, u32 invisible) {
 WWHD_FUNC(0x02591284, void, model, unusedBackup, invisible);
 u32 old0 = gabi::load<u32>(0x104B4634);
 u32 old1 = gabi::load<u32>(0x104B4638);
 u32 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4634, gabi::load<u32>(game + 0x5D78));
 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4638, gabi::load<u32>(game + 0x5D7C));
 u32 actual = gabi::load<u32>(model + 0x90);
 gabi::store<u8>(actual + 0x78, 1);
 gabi::call<void>(0x025E54D8, model);
 actual = gabi::load<u32>(model + 0x90);
 gabi::store<u8>(actual + 0x78, 0);
 if (invisible != 0) gabi::call<void>(0x025E8BC4, invisible);
 gabi::store<u32>(0x104B4634, old0);
 gabi::store<u32>(0x104B4638, old1);
}
VERIFY(0x02591284, UpdateMorf);
static void EntryModel(u32 model, s8 unusedBackup, u32 invisible) {
 WWHD_FUNC(0x0259130C, void, model, unusedBackup, invisible);
 u32 old0 = gabi::load<u32>(0x104B4634);
 u32 old1 = gabi::load<u32>(0x104B4638);
 u32 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4634, gabi::load<u32>(game + 0x5D78));
 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4638, gabi::load<u32>(game + 0x5D7C));
 u32 actual = model;
 gabi::store<u8>(actual + 0x78, 1);
 gabi::call<void>(0x025E2E5C, model);
 actual = model;
 gabi::store<u8>(actual + 0x78, 0);
 if (invisible != 0) gabi::call<void>(0x025E8BC4, invisible);
 gabi::store<u32>(0x104B4634, old0);
 gabi::store<u32>(0x104B4638, old1);
}
VERIFY(0x0259130C, EntryModel);
static void EntryMorf(u32 model, s8 unusedBackup, u32 invisible) {
 WWHD_FUNC(0x0259138C, void, model, unusedBackup, invisible);
 u32 old0 = gabi::load<u32>(0x104B4634);
 u32 old1 = gabi::load<u32>(0x104B4638);
 u32 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4634, gabi::load<u32>(game + 0x5D78));
 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4638, gabi::load<u32>(game + 0x5D7C));
 u32 actual = gabi::load<u32>(model + 0x90);
 gabi::store<u8>(actual + 0x78, 1);
 gabi::call<void>(0x025E5590, model);
 actual = gabi::load<u32>(model + 0x90);
 gabi::store<u8>(actual + 0x78, 0);
 if (invisible != 0) gabi::call<void>(0x025E8BC4, invisible);
 gabi::store<u32>(0x104B4634, old0);
 gabi::store<u32>(0x104B4638, old1);
}
VERIFY(0x0259138C, EntryMorf);
static void EntryMorfAlternate(u32 model, s8 unusedBackup, u32 invisible) {
 WWHD_FUNC(0x02591414, void, model, unusedBackup, invisible);
 u32 old0 = gabi::load<u32>(0x104B4634);
 u32 old1 = gabi::load<u32>(0x104B4638);
 u32 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4634, gabi::load<u32>(game + 0x5D78));
 game = gabi::call<u32>(0x025200D4);
 gabi::store<u32>(0x104B4638, gabi::load<u32>(game + 0x5D7C));
 u32 actual = gabi::load<u32>(model + 0x90);
 gabi::store<u8>(actual + 0x78, 1);
 gabi::call<void>(0x025E5590, model);
 actual = gabi::load<u32>(model + 0x90);
 gabi::store<u8>(actual + 0x78, 0);
 if (invisible != 0) gabi::call<void>(0x025E8EC0, invisible);
 gabi::store<u32>(0x104B4634, old0);
 gabi::store<u32>(0x104B4638, old1);
}
VERIFY(0x02591414, EntryMorfAlternate);
static void Create(u32 parentHeap) {
 WWHD_FUNC(0x0259149C, void, parentHeap);
 gabi::Local<StringPair> name;
 name->vtable = 0x10050FCC; name->text = 0x10050FEC;
 u32 heap = gabi::call<u32>(0x02754B38, 0x100000u, name.get(), parentHeap, 1u, 0u);
 gabi::store<u32>(0x101EA028, heap);
 u32 obj = gabi::call<u32>(0x0273B050, 0x450u, heap, 4u);
 if (obj != 0) {
  gabi::call<void>(0x027BE6B8, obj);
  gabi::call<void>(0x027BE6B8, obj + 0x90);
  gabi::call<void>(0x027BDF7C, obj + 0x120);
  gabi::call<void>(0x027BDF7C, obj + 0x2B8);
 }
 gabi::store<u32>(0x101EA024, obj);
 if (obj == 0) {
  gabi::call<void>(0x0273AA24, 0x10051008u, 0x140u, 0x10050FFCu);
  obj = gabi::load<u32>(0x101EA024);
 }
 Setup(obj);
 heap = gabi::load<u32>(0x101EA028);
 u32 vt = gabi::load<u32>(heap + 12);
 u32 target = gabi::load<u32>(vt + 44);
 gabi::call_ptr<void>(target, heap);
}
VERIFY(0x0259149C, Create);
static void Remove() {
 WWHD_FUNC(0x02591588, void);
 gabi::store<u32>(0x104B45C0, 0);
 u32 heap = gabi::load<u32>(0x101EA028);
 gabi::store<u32>(0x104B45C4, 0);
 gabi::store<u32>(0x101EA024, 0);
 if (heap != 0) {
  u32 vt = gabi::load<u32>(heap + 12);
  u32 target = gabi::load<u32>(vt + 36);
  gabi::call_ptr<void>(target, heap);
  gabi::store<u32>(0x101EA028, 0);
 }
}
VERIFY(0x02591588, Remove);

/* 025915F0 __sinit: the TU's header-statics initializer. The header statics block at
 * 1047A970 (zeroed 16-byte object at +0xC, {-pi, pi} from 1005101C, two one-byte objects at +8/+9),
 * each registered for destruction (records 101EA000, +0xC, +0x18). */
static void __sinit_d_material_cpp() {
    WWHD_FUNC(0x025915F0, void);
    const u32 bss = 0x1047A970, rec = 0x101EA000, ro = 0x1005101C;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025915F0, __sinit_d_material_cpp);

/* 02591684 this TU's sead::SafeString copy (vtable 10050FCC): deleting destructor of a trivially
 * destructible class: operator delete when this != NULL and bit 0 of the flags is set */
static void SafeString_deletingDtor_d_material(u32 p, u32 flags) {
    WWHD_FUNC(0x02591684, void, p, flags);
    if (p == 0) return;
    if (flags & 1) gabi::call(0x0273AF40, p);
}
VERIFY(0x02591684, SafeString_deletingDtor_d_material);

/* 02591698 this TU's sead::SafeString assureTerminationImpl_ (vtable 10050FCC): empty function */
static void SafeString_assureTerminationImpl_d_material(u32 p) {
    WWHD_FUNC(0x02591698, void, p);
}
VERIFY(0x02591698, SafeString_assureTerminationImpl_d_material);

}
