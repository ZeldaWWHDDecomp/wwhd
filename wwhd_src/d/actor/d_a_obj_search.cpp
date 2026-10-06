#include "d/actor/d_a_obj_search.h"
#include <cmath>
namespace {
// Keep live payloads above the outgoing linkage area. Real guest callees
// save their return address at caller SP+4 before consuming these objects.
struct SearchCallerLinkage {
  u8 bytes[32];
};
template <class T> class SearchLocal {
  gabi::Local<T> payload;
  gabi::Local<SearchCallerLinkage> linkage;
public:
  T *get() const { return payload.get(); }
  T *operator->() const { return get(); }
  T &operator*() const { return *get(); }
  operator T *() const { return get(); }
};
} // namespace
static void vec_copy(cXyz *d, const cXyz *s);
using namespace daObj_Search;
using gabi::load;
using gabi::store;
static u32 address(const void *p) { return gabi::ea(p); }
s32 searchCreateHeap(Act_c *a) {
  WWHD_FUNC(0x02388F20, s32, a);
  SearchLocal<SafeString> name;
  name->mStringTop = 0x1002F2AC;
  name->__vtbl = 0x1002EE6C;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<void>(load<u32>(0x101F4F28)), name.get(), 5);
  if (!data)
    gabi::call<void>(0x0273AA24, STR(0x1002EFC4), 0x4A, STR(0x1002EFD8));
  void *model = gabi::call<void *>(0x025E38E0, data, 0x80000, 0x11000022);
  a->mpSearchModel = model;
  if (!model)
    return 0;
  void *bg = gabi::call<void *>(0x024F23F4, (void *)nullptr);
  a->mpBaseBackground = bg;
  if (!bg)
    return 0;
  SearchLocal<SafeString> collision;
  collision->__vtbl = 0x1002EE6C;
  collision->mStringTop = 0x1002F2AC;
  data = gabi::call<void *>(0x026066C4, gabi::at<void>(load<u32>(0x101F4F28)),
                            collision.get(), 8);
  return gabi::call<s32>(0x0200A030, a->mpBaseBackground.get(), data, 1,
                         &a->mBaseMatrix) == 0;
}
VERIFY(0x02388F20, searchCreateHeap);
s32 beamCreateHeap(Act_c *a, s32 index) {
  WWHD_FUNC(0x02389018, s32, a, index);
  SearchLocal<SafeString> name;
  name->__vtbl = 0x1002EE6C;
  name->mStringTop = 0x1002F2AC;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<void>(load<u32>(0x101F4F28)), name.get(), 4);
  if (!data)
    return 0;
  void *model = gabi::call<void *>(0x025E38E0, data, 0x80000, 0x11000022);
  u32 slot = address(a) + u32(index) * 4;
  store<u32>(slot + 0x714, address(model));
  if (!model)
    return 0;
  void *bg = gabi::call<void *>(0x024F23F4, (void *)nullptr);
  store<u32>(slot + 0x784, address(bg));
  if (!bg)
    return 0;
  u32 resources = load<u32>(0x101F4F28),
      resourceIndex = load<u32>(0x101CCA04 + u32(index) * 4);
  SearchLocal<SafeString> collision;
  collision->mStringTop = 0x1002F2AC;
  collision->__vtbl = 0x1002EE6C;
  data = gabi::call<void *>(0x026066C4, gabi::at<void>(resources),
                            collision.get(), resourceIndex);
  return gabi::call<s32>(
             0x0200A030, gabi::at<void>(load<u32>(slot + 0x784)), data, 1,
             gabi::at<void>(address(a) + 0x790 + u32(index) * 48)) == 0;
}
VERIFY(0x02389018, beamCreateHeap);
s32 createHeap(Act_c *a) {
  WWHD_FUNC(0x02389110, s32, a);
  if (!searchCreateHeap(a) || !beamCreateHeap(a, 0))
    return 0;
  return beamCreateHeap(a, 1) != 0;
}
VERIFY(0x02389110, createHeap);
s32 createHeap_CB(Act_c *a) {
  WWHD_FUNC(0x02389188, s32, a);
  return createHeap(a);
}
VERIFY(0x02389188, createHeap_CB);
void SetArgData(Act_c *a) {
  WWHD_FUNC(0x023897D0, void, a);
  u32 p = load<u32>(address(a) + 0xB0);
  store<u8>(address(a) + 0x902, p >> 24);
  store<u8>(address(a) + 0x957, p >> 16);
  u32 control = (p >> 8) & 255;
  store<u8>(address(a) + 0x956, p);
  store<u8>(address(a) + 0x9F0, control == 255 ? 0 : control);
  u8 type = load<u8>(address(a) + 0x956);
  if (type == 255) {
    store<f32>(address(a) + 0x330, 1);
    store<f32>(address(a) + 0x338, 1);
    store<f32>(address(a) + 0x334, 1);
  } else if (type == 1) {
    store<f32>(address(a) + 0x330, load<f32>(0x1002F00C));
    store<f32>(address(a) + 0x334, load<f32>(0x1002F00C));
    store<f32>(address(a) + 0x338, load<f32>(0x1002F00C));
  }
  store<u32>(address(a) + 0x97C,
             (u32(load<s16>(address(a) + 0x2F8)) >> 8) & 255);
}
VERIFY(0x023897D0, SetArgData);
s32 set_path_info(Act_c *a) {
  WWHD_FUNC(0x02389860, s32, a);
  u8 path = load<u8>(address(a) + 0x957);
  return path == 255
             ? 0
             : gabi::call<s32>(0x02587CA4, gabi::at<void>(address(a) + 0x968),
                               path);
}
VERIFY(0x02389860, set_path_info);
bool is_path_info(Act_c *a) {
  WWHD_FUNC(0x023899D8, bool, a);
  return load<u8>(address(a) + 0x957) != 255;
}
VERIFY(0x023899D8, is_path_info);
bool isSecond(Act_c *a, u32 mode) {
  WWHD_FUNC(0x023899EC, bool, a, mode);
  u32 expected = load<s32>(address(a) + 0x97C) == 255 ? 10 : 103;
  return mode == expected || a->mMode == 7;
}
VERIFY(0x023899EC, isSecond);
void modeStopInit(Act_c *a) {
  WWHD_FUNC(0x0238B6DC, void, a);
  store<s16>(address(a) + 0x822, 0x2300);
  store<s16>(address(a) + 0x828, -0x2300);
}
VERIFY(0x0238B6DC, modeStopInit);
void modeProc(Act_c *a, s32 operation, s32 mode) {
  WWHD_FUNC(0x0238A540, void, a, operation, mode);
  if (operation != 0 && operation != 1)
    return;
  u32 entry = 0x1002F070;
  if (operation == 0) {
    entry += u32(mode) * 20;
    a->mMode = mode;
  } else {
    entry += u32(s32(a->mMode)) * 20;
    entry += 8;
  }
  s16 vi = load<s16>(entry + 2), adjustment = load<s16>(entry);
  u32 receiver = address(a) + s32(adjustment), target;
  if (vi < 0)
    target = load<u32>(entry + 4);
  else {
    if (operation == 0)
      vi = load<s16>(entry + 2);
    s16 vo = load<s16>(entry + 6);
    u32 table = load<u32>(receiver + s32(vo));
    target = load<u32>(table + u32(s32(vi)) * 8 + 4);
  }
  gabi::call<void>(target, gabi::at<Act_c>(receiver));
}
VERIFY(0x0238A540, modeProc);
void modeSearchPathInit(Act_c *a) {
  WWHD_FUNC(0x0238B6A0, void, a);
  if (!is_path_info(a))
    modeProc(a, 0, 0);
}
VERIFY(0x0238B6A0, modeSearchPathInit);
s32 initialMode(Act_c *a, u32 kind) {
  WWHD_FUNC(0x0238A5EC, s32, a, kind);
  if (kind == 10 || kind == 102) {
    u32 save = load<u32>(0x101F84DC);
    if (gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), 0x2A, 1) &&
        load<s32>(address(a) + 0x97C) != 255)
      return 3;
    return 1;
  }
  if (kind == 11)
    return 2;
  if (kind == 103)
    return 4;
  return 0;
}
VERIFY(0x0238A5EC, initialMode);
void check_bk_control(Act_c *a) {
  WWHD_FUNC(0x0238A684, void, a);
  SearchLocal<be<u32>> id;
  u32 child = load<u32>(address(a) + 0x96C);
  *id = child;
  void *controller = nullptr;
  if (child != 0xFFFFFFFF)
    controller =
        gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E1234), id.get());
  if (!controller)
    return;
  s16 angle = load<s16>(address(a) + 0x8CC);
  store<u8>(address(a) + 0x980, 0);
  store<s16>(address(controller) + 0x1384, angle);
  u8 control = a->mBkControl;
  s32 mode = a->mMode;
  if (control) {
    if (mode == 4 || mode == 2)
      modeProc(a, 0, 3);
  } else if (mode != 4 && mode != 2)
    modeProc(a, 0, 4);
}
VERIFY(0x0238A684, check_bk_control);
void bg_check(Act_c *a) {
  WWHD_FUNC(0x0238A73C, void, a);
  u32 play = address(gabi::call<void *>(0x025200D4));
  void *player = gabi::at<void>(load<u32>(play + 0x5B2C));
  f32 distance = gabi::call<f32>(0x025D68EC, a, player);
  if (!(distance > load<f32>(0x1002F18C)) &&
      load<u8>(address(a) + 0x8D8) == 0) {
    gabi::call<void>(0x0238A0B8, a);
    gabi::call<void>(0x0238A154, a);
  }
}
VERIFY(0x0238A73C, bg_check);
void modeToStopInit(Act_c *a) {
  WWHD_FUNC(0x0238B6F0, void, a);
  store<s16>(address(a) + 0x8D0, load<s16>(address(a) + 0x824));
  u32 save = load<u32>(0x101F84DC);
  if (gabi::call<s32>(0x025B8B94, gabi::at<void>(save + 0x644), 0x201)) {
    s8 room = load<s8>(address(a) + 0x326);
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call<void>(0x025E1A40, 0x6939, gabi::at<void>(address(a) + 0x37C), 0,
                     reverb);
  }
}
VERIFY(0x0238B6F0, modeToStopInit);
void modeFindInit(Act_c *a) {
  WWHD_FUNC(0x0238B75C, void, a);
  if (load<s32>(address(a) + 0x97C) != 255) {
    modeProc(a, 0, 6);
    return;
  }
  if (load<u8>(address(a) + 0x9F0)) {
    u32 save = load<u32>(0x101F84DC);
    gabi::call<void>(0x025B8B68, gabi::at<void>(save + 0x644), 0x340);
  }
  if (load<u16>(address(a) + 0xF8) != 2)
    gabi::call<void>(0x025E1988, 0x834);
  s16 y = load<s16>(address(a) + 0x824), z = load<s16>(address(a) + 0x828);
  store<s16>(address(a) + 0x8D0, y);
  s16 x = load<s16>(address(a) + 0x822);
  store<s16>(address(a) + 0x8D4, z);
  store<s16>(address(a) + 0x8D2, x);
}
VERIFY(0x0238B75C, modeFindInit);
void modeFind2ndInit(Act_c *a) {
  WWHD_FUNC(0x0238B800, void, a);
  s16 z = load<s16>(address(a) + 0x828), y = load<s16>(address(a) + 0x824);
  store<s16>(address(a) + 0x8D4, z);
  s16 x = load<s16>(address(a) + 0x822);
  store<s16>(address(a) + 0x8D0, y);
  store<s16>(address(a) + 0x8D2, x);
  u32 save = load<u32>(0x101F84DC);
  s8 room = load<s8>(0x1047E6C8);
  u32 sw = load<u32>(address(a) + 0x97C);
  gabi::call<void>(0x025B9E38, gabi::at<void>(save + 0x20), sw, room);
}
VERIFY(0x0238B800, modeFind2ndInit);

s32 search_Create(Act_c *a) {
  WWHD_FUNC(0x0238DB3C, s32, a);
  return gabi::call<s32>(0x0238AAA0, a);
}
VERIFY(0x0238DB3C, search_Create);

s32 search_Delete(Act_c *a) {
  WWHD_FUNC(0x0238DB40, s32, a);
  return gabi::call<s32>(0x0238B16C, a);
}
VERIFY(0x0238DB40, search_Delete);

s32 search_Execute(Act_c *a) {
  WWHD_FUNC(0x0238DB44, s32, a);
  return gabi::call<s32>(0x0238A7A4, a);
}
VERIFY(0x0238DB44, search_Execute);

s32 search_Draw(Act_c *a) {
  WWHD_FUNC(0x0238DB48, s32, a);
  return gabi::call<s32>(0x0238B258, a);
}
VERIFY(0x0238DB48, search_Draw);

s32 search_IsDelete(Act_c *a) {
  WWHD_FUNC(0x0238DB4C, s32, a);
  return 1;
}
VERIFY(0x0238DB4C, search_IsDelete);

void search_SafeString_assureTermination(void *p) {
  WWHD_FUNC(0x0238DCF0, void, p);
}
VERIFY(0x0238DCF0, search_SafeString_assureTermination);

void modeSearchRndInit(void *p) { WWHD_FUNC(0x0238DCF4, void, p); }
VERIFY(0x0238DCF4, modeSearchRndInit);

void modeToSearchInit(void *p) { WWHD_FUNC(0x0238DCF8, void, p); }
VERIFY(0x0238DCF8, modeToSearchInit);

void modeSearchBdkInit(void *p) { WWHD_FUNC(0x0238DCFC, void, p); }
VERIFY(0x0238DCFC, modeSearchBdkInit);

void modeStop(void *p) { WWHD_FUNC(0x0238DD00, void, p); }
VERIFY(0x0238DD00, modeStop);

void search_static_dtor(void *p, u32 flags) {
  WWHD_FUNC(0x0238DBE8, void, p, flags);
  if (p && (flags & 1))
    gabi::call<void>(0x0273AF40, p);
}
VERIFY(0x0238DBE8, search_static_dtor);

void *search_status_ctor(void *p) {
  WWHD_FUNC(0x0238DBFC, void *, p);
  if (!p) {
    p = gabi::call<void *>(0x0273AD10, 0x3C);
    if (!p)
      return p;
  }
  gabi::call<void>(0x0200BD2C, p);
  gabi::call<void>(0x02515DA0, gabi::at<void>(address(p) + 0x1C));
  store<u32>(address(p) + 0x18, 0x1004AE88);
  store<u32>(address(p) + 0x1C, 0x1004AEC0);
  return p;
}
VERIFY(0x0238DBFC, search_status_ctor);

void *search_capsule_ctor(void *p) {
  WWHD_FUNC(0x0238DC64, void *, p);
  if (!p) {
    p = gabi::call<void *>(0x0273AD10, 0x138);
    if (!p)
      return p;
  }
  gabi::call<void>(0x02515FB8, p);
  store<u32>(address(p) + 0x114, 0x100015A8);
  store<u32>(address(p) + 0x110, 0x1002EE84);
  gabi::call<void>(0x02018150, gabi::at<void>(address(p) + 0x118));
  store<u32>(address(p) + 0x3C, 0x1004AF18);
  store<u32>(address(p) + 0x130, 0x1004AF60);
  store<u32>(address(p) + 0x114, 0x1004AF70);
  return p;
}
VERIFY(0x0238DC64, search_capsule_ctor);

Bgc_c *Bgc_ctor(Bgc_c *p) {
  WWHD_FUNC(0x02389524, Bgc_c *, p);
  if (!p) {
    p = gabi::call<Bgc_c *>(0x0273AD10, 0x78);
    if (!p)
      return p;
  }
  gabi::call<void>(0x02008FEC, p);
  store<u8>(address(p) + 0x61, 0);
  store<u8>(address(p) + 0x60, 0);
  store<u32>(address(p) + 0x10, 0x1002EEB4);
  store<u8>(address(p) + 0x5C, 0);
  store<u32>(address(p) + 4, address(p) + 0x64);
  store<u32>(address(p) + 0x64, 0x1002EED4);
  store<u32>(address(p), address(p) + 0x58);
  store<u8>(address(p) + 0x5E, 0);
  store<u32>(address(p) + 0x58, 0x1002EEE4);
  store<u8>(address(p) + 0x5F, 0);
  store<u32>(address(p) + 0x20, 0x1002EEC4);
  store<u32>(address(p) + 0x68, 1);
  store<u8>(address(p) + 0x62, 0);
  store<u8>(address(p) + 0x5D, 0);
  return p;
}
VERIFY(0x02389524, Bgc_ctor);

Act_c *Act_ctor(Act_c *p) {
  WWHD_FUNC(0x023896BC, Act_c *, p);
  if (!p) {
    p = gabi::call<Act_c *>(0x0273AD10, 0xA28);
    if (!p)
      return p;
  }
  gabi::call<void>(0x025D4ED0, p);
  store<u32>(address(p) + 0xB4, 0x1002EFB4);
  Bgc_ctor(gabi::at<Bgc_c>(address(p) + 0x3B0));
  gabi::call<void>(0x028EFFD0, gabi::at<void>(address(p) + 0x428), 2, 0x138,
                   gabi::at<void>(0x0238DC64));
  gabi::call<void>(0x028EFFD0, gabi::at<void>(address(p) + 0x698), 2, 0x3C,
                   gabi::at<void>(0x0238DBFC));
  store<f32>(address(p) + 0x8BC, load<f32>(0x1002F008));
  gabi::call<void>(0x025A5B18, gabi::at<void>(address(p) + 0x928), 1);
  gabi::call<void>(0x02008FEC, gabi::at<void>(address(p) + 0x984));
  store<u32>(address(p) + 0x984, address(p) + 0x9DC);
  store<u8>(address(p) + 0x9E2, 0);
  store<u8>(address(p) + 0x9E4, 0);
  store<u32>(address(p) + 0x9DC, 0x1002EFA4);
  store<u32>(address(p) + 0x994, 0x1002EF74);
  store<u32>(address(p) + 0x9EC, 1);
  store<u8>(address(p) + 0x9E0, 1);
  store<u8>(address(p) + 0x9E3, 0);
  store<u8>(address(p) + 0x9E1, 0);
  store<u32>(address(p) + 0x988, address(p) + 0x9E8);
  store<u32>(address(p) + 0x9A4, 0x1002EF84);
  store<u32>(address(p) + 0x9E8, 0x1002EF94);
  store<u8>(address(p) + 0x9E6, 0);
  store<u8>(address(p) + 0x9E5, 0);
  return p;
}
VERIFY(0x023896BC, Act_ctor);

void search_sinit() {
  WWHD_FUNC(0x0238DB54, void, );
  store<u32>(0x1046BDB8, 0);
  store<u32>(0x1046BDB0, 0);
  store<u32>(0x1046BDBC, 0);
  store<u32>(0x1046BDB4, 0);
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101CCA0C));
  store<f32>(0x1046BDA4, load<f32>(0x1002F2A0));
  store<f32>(0x1046BDA8, load<f32>(0x1002F2A4));
  gabi::call<void>(0x028ED6F8, gabi::at<void>(0x1046BDAC));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101CCA18));
  gabi::call<void>(0x028EAB2C, gabi::at<void>(0x1046BDAD));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101CCA24));
}
VERIFY(0x0238DB54, search_sinit);
void set_mtx_base(Act_c *a) {
  WWHD_FUNC(0x02389954, void, a);
  u32 model = address(a->mpSearchModel.get());
  f32 y = load<f32>(address(a) + 0x334), x = load<f32>(address(a) + 0x330),
      z = load<f32>(address(a) + 0x338);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  store<f32>(model + 0xC4, z);
  x = load<f32>(address(a) + 0x314);
  y = load<f32>(address(a) + 0x318);
  z = load<f32>(address(a) + 0x31C);
  gabi::call<void>(0x028E93CC, gabi::at<void>(0x1048D0CC), x, y, z);
  s16 angle = load<s16>(address(a) + 0x322);
  gabi::call<void>(0x025F1C28, gabi::at<void>(0x1048D0CC), angle);
  gabi::call<void>(0x028E90D4, gabi::at<void>(0x1048D0CC), &a->mBaseMatrix);
  gabi::call<void>(0x024F43DC, a->mpBaseBackground.get());
}
VERIFY(0x02389954, set_mtx_base);
void set_model_mtx_base(Act_c *a) {
  WWHD_FUNC(0x0238987C, void, a);
  f32 x = load<f32>(address(a) + 0x330), y = load<f32>(address(a) + 0x334);
  u32 model = address(a->mpSearchModel.get());
  f32 z = load<f32>(address(a) + 0x338);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  store<f32>(model + 0xC4, z);
  x = load<f32>(address(a) + 0x314);
  y = load<f32>(address(a) + 0x318);
  z = load<f32>(address(a) + 0x31C);
  gabi::call<void>(0x028E93CC, gabi::at<void>(0x1048D0CC), x, y, z);
  s16 angle = load<s16>(address(a) + 0x322);
  gabi::call<void>(0x025F1C28, gabi::at<void>(0x1048D0CC), angle);
  mtx_copy(gabi::at<Mtx34>(address(a->mpSearchModel.get()) + 0xC8),
           gabi::at<Mtx34>(0x1048D0CC));
}
VERIFY(0x0238987C, set_model_mtx_base);

void set_moveBG_mtx_light_A(Act_c *a) {
  WWHD_FUNC(0x0238A0B8, void, a);
  u32 model = address(a->mpSearchModel.get()), block = load<u32>(model + 0x2C);
  u16 flags = load<u16>(block + 4);
  u32 matrices = load<u32>(block + 0x10);
  store<u16>(block + 4, flags | 0x10);
  gabi::call<void>(0x028E90D4, gabi::at<void>(matrices + 240),
                   gabi::at<void>(0x1048D0CC));
  gabi::call<void>(0x025F19F8, gabi::at<void>(0x1048D0CC), 0, 0, -0x4000);
  f32 z = load<f32>(268628048), x = load<f32>(268628040),
      y = load<f32>(0x1002F04C);
  gabi::call<void>(0x025F24E0, x, y, z);
  gabi::call<void>(0x028E90D4, gabi::at<void>(0x1048D0CC), &a->mBeamMatrix[0]);
  gabi::call<void>(0x024F43DC, a->mpBeamBackground[0].get());
}
VERIFY(0x0238A0B8, set_moveBG_mtx_light_A);

void set_moveBG_mtx_light_B(Act_c *a) {
  WWHD_FUNC(0x0238A154, void, a);
  u32 model = address(a->mpSearchModel.get()), block = load<u32>(model + 0x2C);
  u16 flags = load<u16>(block + 4);
  u32 matrices = load<u32>(block + 0x10);
  store<u16>(block + 4, flags | 0x10);
  gabi::call<void>(0x028E90D4, gabi::at<void>(matrices + 288),
                   gabi::at<void>(0x1048D0CC));
  gabi::call<void>(0x025F19F8, gabi::at<void>(0x1048D0CC), 0, 0, -0x4000);
  f32 z = load<f32>(268628056), x = load<f32>(268628052),
      y = load<f32>(0x1002F04C);
  gabi::call<void>(0x025F24E0, x, y, z);
  gabi::call<void>(0x028E90D4, gabi::at<void>(0x1048D0CC), &a->mBeamMatrix[1]);
  gabi::call<void>(0x024F43DC, a->mpBeamBackground[1].get());
}
VERIFY(0x0238A154, set_moveBG_mtx_light_B);
void wall_pos(Bgc_c *bg, const Act_c *a, cXyz *start, cXyz *end, bool *hit,
              f32 *dot) {
  WWHD_FUNC(0x023895D4, void, bg, a, start, end, hit, dot);
  gabi::call<void>(0x024F1AFC, bg, start, end, a);
  u32 play = address(gabi::call<void *>(0x025200D4));
  if (gabi::call<s32>(0x02008860, gabi::at<void>(play + 0x12A0), bg)) {
    play = address(gabi::call<void *>(0x025200D4));
    u16 index = load<u16>(address(bg) + 0x14),
        owner = load<u16>(address(bg) + 0x16);
    void *plane = gabi::call<void *>(0x020084C8, gabi::at<void>(play + 0x12A0),
                                     owner, index);
    if (plane) {
      SearchLocal<cXyz> difference, normal;
      gabi::call<void>(0x0201ADE0, end, difference.get(), start);
      gabi::call<void>(0x0201B12C, difference.get(), normal.get());
      f32 d = gabi::call<f32>(0x028E8F44, normal.get(), plane);
      store<f32>(address(dot), d);
      u32 x = load<u32>(address(bg) + 0x30), y = load<u32>(address(bg) + 0x34);
      store<u32>(address(bg) + 0x6C, x);
      u32 z = load<u32>(address(bg) + 0x38);
      store<u32>(address(bg) + 0x70, y);
      store<u32>(address(bg) + 0x74, z);
      store<u32>(address(end), x);
      store<u32>(address(end) + 4, load<u32>(address(bg) + 0x70));
      store<u32>(address(end) + 8, load<u32>(address(bg) + 0x74));
      store<u8>(address(hit), 1);
      return;
    }
  }
  store<u8>(address(hit), 0);
}
VERIFY(0x023895D4, wall_pos);
void Act_dtor(Act_c *a, u32 flags) {
  WWHD_FUNC(0x0238DD04, void, a, flags);
  if (!a)
    return;
  store<u32>(address(a) + 0x9DC, 0x1002EEE4);
  store<u32>(address(a) + 0x9E8, 0x1002EEA4);
  store<u32>(address(a) + 0x9A4, 0x1002EE94);
  gabi::call<void>(0x02008B4C, gabi::at<void>(address(a) + 0x984), 0);
  gabi::call<void>(0x028F0164, gabi::at<void>(address(a) + 0x698), 2, 0x3C,
                   gabi::at<void>(0x02515860), 0, 0);
  gabi::call<void>(0x028F0164, gabi::at<void>(address(a) + 0x428), 2, 0x138,
                   gabi::at<void>(0x02515980), 0, 0);
  store<u32>(address(a) + 0x408, 0x1002EEE4);
  store<u32>(address(a) + 0x414, 0x1002EEA4);
  store<u32>(address(a) + 0x3D0, 0x1002EE94);
  gabi::call<void>(0x02008B4C, gabi::at<void>(address(a) + 0x3B0), 0);
  gabi::call<void>(0x025D50BC, a, 0);
  if (flags & 1)
    gabi::call<void>(0x0273AF40, a);
}
VERIFY(0x0238DD04, Act_dtor);
s32 nodeControl_CB(void *node, s32 timing) {
  WWHD_FUNC(0x023894DC, s32, node, timing);
  if (timing == 0) {
    u32 model = load<u32>(0x104B462C);
    u32 actor = load<u32>(model + 0xB8);
    if (actor)
      gabi::call<void>(0x0238918C, gabi::at<Act_c>(actor), node,
                       gabi::at<void>(model));
  }
  return 1;
}
VERIFY(0x023894DC, nodeControl_CB);
s32 actor_delete(Act_c *a) {
  WWHD_FUNC(0x0238B16C, s32, a);
  gabi::call<void>(0x0255A374, gabi::at<void>(address(a) + 0x89C));
  for (u32 offset : {0x784u, 0x788u, 0x78Cu}) {
    u32 bg = load<u32>(address(a) + offset);
    if (bg && load<u32>(bg) < 0x100) {
      u32 play = address(gabi::call<void *>(0x025200D4));
      bg = load<u32>(address(a) + offset);
      gabi::call<void>(0x020087EC, gabi::at<void>(play + 0x12A0),
                       gabi::at<void>(bg));
    }
  }
  gabi::call<void>(0x025204C8, gabi::at<void>(address(a) + 0x974),
                   STR(0x1002F2AC));
  u32 vtable = load<u32>(address(a) + 0x928), target = load<u32>(vtable + 0x44);
  gabi::call<void>(target, gabi::at<void>(address(a) + 0x928));
  if (gabi::call<s32>(0x025E1B24, 0x834))
    gabi::call<void>(0x025E1AE0, 0x834, 0);
  return 1;
}
VERIFY(0x0238B16C, actor_delete);
void modeSearchRnd(Act_c *a) {
  WWHD_FUNC(0x0238BE60, void, a);
  if (load<u8>(address(a) + 0x954) && gabi::call<s32>(0x0238B838, a))
    modeProc(a, 0, 5);
  s16 y = load<s16>(address(a) + 0x824), x = load<s16>(address(a) + 0x822);
  store<s16>(address(a) + 0x8D2, x);
  s16 z = load<s16>(address(a) + 0x828);
  store<s16>(address(a) + 0x8D4, z);
  f32 next = f32(y) + load<f32>(0x1002F1B4);
  store<s16>(address(a) + 0x822, 0);
  store<s16>(address(a) + 0x8D0, y);
  s16 angle = s16(gabi::ftoi(next));
  store<s16>(address(a) + 0x824, angle);
  store<s16>(address(a) + 0x828, 0);
  store<s16>(address(a) + 0x82A, angle);
}
VERIFY(0x0238BE60, modeSearchRnd);
void modeToSearch(Act_c *a) {
  WWHD_FUNC(0x0238C640, void, a);
  s16 x = load<s16>(address(a) + 0x8D2);
  s32 dx = gabi::call<s32>(0x0200F378, gabi::at<void>(address(a) + 0x822), x,
                           10, 0x200, 0x100);
  s16 z = load<s16>(address(a) + 0x8D4);
  s32 dz = gabi::call<s32>(0x0200F378, gabi::at<void>(address(a) + 0x828), z,
                           10, 0x200, 0x100);
  if (dx < 0x100 && dz < 0x100)
    modeProc(a, 0, 1);
}
VERIFY(0x0238C640, modeToSearch);
void nodeControl(Act_c *a, void *node, void *model) {
  WWHD_FUNC(0x0238918C, void, a, node, model);
  u32 desc = address(gabi::call<void *>(0x027F7878, node));
  u16 index = load<u16>(desc + 4);
  u32 block = load<u32>(address(model) + 0x2C), array = load<u32>(block + 0x10);
  u16 flags = load<u16>(block + 4);
  store<u16>(block + 4, flags | 0x10);
  gabi::call<void>(0x028E90D4, gabi::at<void>(array + u32(index) * 48),
                   gabi::at<void>(0x1048D0CC));
  s16 delta =
      s16(load<s16>(address(a) + 0x824) - load<s16>(address(a) + 0x8D0));
  s16 spin = s16(gabi::ftoi(f32(delta) * load<f32>(0x1002EFF8)));
  store<s16>(address(a) + 0x8CE, 0x2700);
  if (index == 5) {
    s16 angle = load<s16>(address(a) + 0x824);
    gabi::call<void>(0x025F1BF4, gabi::at<void>(0x1048D0CC), angle);
    angle = load<s16>(address(a) + 0x822);
    gabi::call<void>(0x025F1C28, gabi::at<void>(0x1048D0CC), angle);
  } else if (index == 6) {
    s16 angle = load<s16>(address(a) + 0x82A);
    gabi::call<void>(0x025F1BF4, gabi::at<void>(0x1048D0CC), angle);
    angle = load<s16>(address(a) + 0x828);
    gabi::call<void>(0x025F1C28, gabi::at<void>(0x1048D0CC), angle);
  } else if (index == 7)
    gabi::call<void>(0x025F1C28, gabi::at<void>(0x1048D0CC), 0x2700);
  else if (index == 4) {
    gabi::call<void>(0x025F1C28, gabi::at<void>(0x1048D0CC), 0x2700);
    s16 angle = load<s16>(address(a) + 0x8CC);
    gabi::call<void>(0x025F1BF4, gabi::at<void>(0x1048D0CC), angle);
  }
  if (a->mMode != 2) {
    if (index == 3) {
      f32 old = f32(load<s16>(address(a) + 0x8CC));
      s16 angle =
          s16(gabi::ftoi(gabi::fmadds(f32(spin), load<f32>(0x1002EFFC), old)));
      store<s16>(address(a) + 0x8CC, angle);
      gabi::call<void>(0x025F1BF4, gabi::at<void>(0x1048D0CC), angle);
    } else if (index == 2) {
      f32 old = f32(load<s16>(address(a) + 0x8CA));
      s16 angle =
          s16(gabi::ftoi(gabi::fnmsubs(f32(spin), load<f32>(0x1002F000), old)));
      store<s16>(address(a) + 0x8CA, angle);
      gabi::call<void>(0x025F1BF4, gabi::at<void>(0x1048D0CC), angle);
    } else if (index == 1) {
      f32 old = f32(load<s16>(address(a) + 0x8C8));
      s16 angle =
          s16(gabi::ftoi(gabi::fmadds(f32(spin), load<f32>(0x1002F004), old)));
      store<s16>(address(a) + 0x8C8, angle);
      gabi::call<void>(0x025F1BF4, gabi::at<void>(0x1048D0CC), angle);
    }
  }
  gabi::call<void>(0x028E90D4, gabi::at<void>(0x1048D0CC),
                   gabi::at<void>(0x104B4868));
  block = load<u32>(address(model) + 0x2C);
  flags = load<u16>(block + 4);
  array = load<u32>(block + 0x10);
  store<u16>(block + 4, flags | 0x10);
  mtx_copy(gabi::at<Mtx34>(array + u32(index) * 48),
           gabi::at<Mtx34>(0x1048D0CC));
}
VERIFY(0x0238918C, nodeControl);
s32 actor_execute(Act_c *a) {
  WWHD_FUNC(0x0238A7A4, s32, a);
  if (!gabi::call<s32>(0x0211D2F8, gabi::at<void>(address(a) + 0x948)))
    gabi::call<void>(0x025A5F88, gabi::at<void>(address(a) + 0x928));
  bool copied = false;
  s32 angleDelta;
  u8 control = load<u8>(address(a) + 0x9F0);
  if (control != 6 && control != 5) {
    SearchLocal<be<u32>> id;
    u32 child = load<u32>(address(a) + 0x96C);
    *id = child;
    void *controller = nullptr;
    if (child != 0xFFFFFFFF)
      controller =
          gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E1234), id.get());
    if (controller) {
      s16 current = load<s16>(address(a) + 0x824),
          previous = load<s16>(address(a) + 0x8D0);
      u32 x = load<u32>(address(a) + 0x314), y = load<u32>(address(a) + 0x318);
      u8 sw = load<u8>(address(controller) + 0x3D3);
      angleDelta = current - previous;
      store<u32>(address(a) + 0x37C, x);
      store<u32>(address(a) + 0x970, sw);
      u32 z = load<u32>(address(a) + 0x31C);
      store<u32>(address(a) + 0x380, y);
      store<u32>(address(a) + 0x384, z);
      copied = true;
    } else {
      u32 sw = load<u32>(address(a) + 0x970);
      if (sw != 255) {
        u32 save = load<u32>(0x101F84DC);
        s8 room = load<s8>(0x1047E6C8);
        if (!gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), sw,
                             room)) {
          room = load<s8>(0x1047E6C8);
          save = load<u32>(0x101F84DC);
          sw = load<u32>(address(a) + 0x970);
          gabi::call<void>(0x025B9E38, gabi::at<void>(save + 0x20), sw, room);
        }
      }
    }
  }
  if (!copied) {
    s16 current = load<s16>(address(a) + 0x824),
        previous = load<s16>(address(a) + 0x8D0);
    u32 x = load<u32>(address(a) + 0x314), y = load<u32>(address(a) + 0x318);
    angleDelta = current - previous;
    store<u32>(address(a) + 0x37C, x);
    u32 z = load<u32>(address(a) + 0x31C);
    store<u32>(address(a) + 0x380, y);
    store<u32>(address(a) + 0x384, z);
  }
  s16 magnitude = s16(angleDelta < 0 ? -angleDelta : angleDelta);
  if (magnitude && a->mMode != 2) {
    s8 room = load<s8>(address(a) + 0x326);
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call<void>(0x025E1A40, 0x6131, gabi::at<void>(address(a) + 0x37C), 0,
                     reverb);
  }
  gabi::call<void>(0x027F4D5C, a->mpSearchModel.get());
  SearchLocal<cXyz> zero;
  f32 value = load<f32>(0x1002F010);
  zero->y = value;
  zero->z = value;
  zero->x = value;
  u32 block = load<u32>(address(a->mpSearchModel.get()) + 0x2C);
  u16 flags = load<u16>(block + 4);
  u32 array = load<u32>(block + 0x10);
  store<u16>(block + 4, flags | 0x10);
  gabi::call<void>(0x028E90D4, gabi::at<void>(array + 0xF0),
                   gabi::at<void>(0x1048D0CC));
  gabi::call<void>(0x028E8F64, gabi::at<void>(0x1048D0CC), zero.get(),
                   gabi::at<void>(address(a) + 0x71C));
  gabi::call<void>(0x02389A24, a);
  gabi::call<void>(0x02389DC4, a);
  s32 frame = s32(u32(load<s32>(address(a) + 0x8C4)) + 1);
  if (frame >= 4)
    frame = 0;
  store<s32>(address(a) + 0x8C4, frame);
  u32 save = load<u32>(0x101F84DC);
  f32 time = load<f32>(save + 0x44);
  bool active;
  if (time >= load<f32>(0x1002F190))
    active = !load<u8>(address(a) + 0x980);
  else if (time > load<f32>(0x1002F194))
    active = false;
  else
    active = !load<u8>(address(a) + 0x980);
  store<u8>(address(a) + 0x954, active ? 1 : 0);
  s16 speed = s16(load<s16>(0x1047BD48) + 20);
  gabi::call<void>(0x0200F564, gabi::at<void>(address(a) + 0x89A),
                   active ? 255 : 0, speed);
  SearchLocal<be<s16>> profile;
  *profile = 0xF1;
  void *found =
      gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E121C), profile.get());
  if (load<s32>(address(a) + 0x97C) == 255 || !found) {
    if (load<u8>(address(a) + 0x9F0) != 5)
      check_bk_control(a);
  }
  modeProc(a, 1, 8);
  bg_check(a);
  return 1;
}
VERIFY(0x0238A7A4, actor_execute);
void smoke_set(Act_c *a, f32 speed, s32 duration) {
  WWHD_FUNC(0x0238C6E8, void, a, speed, duration);
  u32 y = load<u32>(address(a) + 0x720),
      emitter = load<u32>(address(a) + 0x92C);
  store<u32>(address(a) + 0x918, y);
  u32 z = load<u32>(address(a) + 0x724), x = load<u32>(address(a) + 0x71C);
  store<u32>(address(a) + 0x91C, z);
  store<u32>(address(a) + 0x914, x);
  store<s16>(address(a) + 0x922, 0);
  store<s16>(address(a) + 0x920, 0);
  store<s16>(address(a) + 0x924, 0);
  if (!emitter) {
    s8 room = load<s8>(address(a) + 0x326);
    u32 play = address(gabi::call<void *>(0x025200D4));
    u32 particles = load<u32>(play + 0x5AB0);
    gabi::call<void>(0x025A847C, gabi::at<void>(particles), 2, 0x2022,
                     gabi::at<void>(address(a) + 0x914),
                     gabi::at<void>(address(a) + 0x920), 0, 0xB9,
                     gabi::at<void>(address(a) + 0x928), room, 0, 0, 0);
    emitter = load<u32>(address(a) + 0x92C);
  }
  if (emitter) {
    store<f32>(emitter + 0x34, speed);
    f32 one = load<f32>(0x1002F008), offset = load<f32>(0x1002F1BC);
    emitter = load<u32>(address(a) + 0x92C);
    store<f32>(emitter + 0x58, one);
    f32 scale = load<f32>(0x1047BCF8) + offset;
    emitter = load<u32>(address(a) + 0x92C);
    f32 extra = load<f32>(0x1002F1C0);
    for (u32 o : {0x220u, 0x224u, 0x228u, 0x238u, 0x23Cu, 0x240u})
      store<f32>(emitter + o, scale);
    scale = load<f32>(0x1047BCFC) + extra;
    emitter = load<u32>(address(a) + 0x92C);
    store<f32>(emitter + 0x238, scale);
    store<f32>(emitter + 0x240, scale);
    store<f32>(emitter + 0x23C, scale);
  }
  store<s32>(address(a) + 0x948, duration);
}
VERIFY(0x0238C6E8, smoke_set);
void CreateInit(Act_c *a) {
  WWHD_FUNC(0x0238A1F0, void, a);
  set_path_info(a);
  u32 beam = address(a->mpBeamModel[0].get());
  store<u8>(address(a) + 0x8D6, 0);
  store<u8>(address(a) + 0x8D7, 0);
  store<u8>(address(a) + 0x8D8, 0);
  if (beam)
    beam += 0xC8;
  f32 high = load<f32>(0x1002F060), low = load<f32>(0x1002F05C),
      height = load<f32>(0x1002F064), zero = load<f32>(0x1002F010);
  store<u32>(address(a) + 0x348, beam);
  gabi::call<void>(0x025D674C, a, low, zero, low, high, height, high);
  for (u32 offset : {0x784u, 0x788u, 0x78Cu})
    store<u32>(load<u32>(address(a) + offset) + 0xA8, 0x024EE658);
  for (u32 offset : {0x784u, 0x788u, 0x78Cu}) {
    u32 play = address(gabi::call<void *>(0x025200D4));
    void *bg = gabi::at<void>(load<u32>(address(a) + offset));
    gabi::call<void>(0x024EEA6C, gabi::at<void>(play + 0x12A0), bg, a);
  }
  gabi::call<void>(0x02515F14, gabi::at<void>(address(a) + 0x698), 255, 255, a);
  gabi::call<void>(0x025164C0, gabi::at<void>(address(a) + 0x428),
                   gabi::at<void>(0x101CC9B8));
  store<u32>(address(a) + 0x46C, address(a) + 0x698);
  gabi::call<void>(0x02018808, gabi::at<void>(address(a) + 0x540),
                   gabi::at<void>(address(a) + 0x314),
                   gabi::at<void>(address(a) + 0x314));
  f32 radius = load<f32>(0x1002F068);
  store<f32>(address(a) + 0x55C, radius);
  gabi::call<void>(0x02515F14, gabi::at<void>(address(a) + 0x6D4), 255, 255, a);
  gabi::call<void>(0x025164C0, gabi::at<void>(address(a) + 0x560),
                   gabi::at<void>(0x101CC9B8));
  store<u32>(address(a) + 0x5A4, address(a) + 0x6D4);
  gabi::call<void>(0x02018808, gabi::at<void>(address(a) + 0x678),
                   gabi::at<void>(address(a) + 0x314),
                   gabi::at<void>(address(a) + 0x314));
  store<f32>(address(a) + 0x694, radius);
  set_model_mtx_base(a);
  set_mtx_base(a);
  store<u32>(address(a->mpSearchModel.get()) + 0xB8, address(a));
  u32 data = load<u32>(address(a->mpSearchModel.get()) + 0xAC);
  u16 index = 0;
  u32 table = address(gabi::call<void *>(0x027F3F94, gabi::at<void>(data)));
  while (index < load<u16>(table + 8)) {
    if (index >= 1 && index <= 7) {
      u32 count = load<u32>(data + 4), nodes = load<u32>(data + 8);
      u32 joint = index < count ? nodes + u32(index) * 0x1C : nodes;
      store<u32>(joint + 8, 0x023894DC);
    }
    index = u16(index + 1);
    table = address(gabi::call<void *>(0x027F3F94, gabi::at<void>(data)));
  }
  gabi::call<void>(0x027F4D5C, a->mpSearchModel.get());
  if (is_path_info(a)) {
    s8 room = load<s8>(address(a) + 0x326);
    u8 pathId = load<u8>(address(a) + 0x957);
    void *path = gabi::call<void *>(0x025AAF88, pathId, room);
    store<u32>(address(a) + 0x968, address(path));
    gabi::call<void>(0x02587D24, gabi::at<void>(address(a) + 0x958),
                     gabi::at<void>(address(a) + 0x964), path, 0, 0,
                     load<f32>(0x1002F06C));
    u32 y = load<u32>(address(a) + 0x95C), x = load<u32>(address(a) + 0x958);
    store<u32>(address(a) + 0x72C, y);
    u32 z = load<u32>(address(a) + 0x960);
    store<u32>(address(a) + 0x728, x);
    store<u32>(address(a) + 0x730, z);
    SearchLocal<cXyz> diff;
    gabi::call<void>(0x0201ADE0, gabi::at<void>(address(a) + 0x728), diff.get(),
                     gabi::at<void>(address(a) + 0x740));
    s16 angle = load<s16>(address(a) + 0x824);
    f32 dx = diff->x;
    store<s16>(address(a) + 0x8D0, angle);
    f32 dz = diff->z;
    angle = load<s16>(address(a) + 0x828);
    s16 ax = load<s16>(address(a) + 0x822);
    store<s16>(address(a) + 0x8D4, angle);
    f32 dy = diff->y;
    store<s16>(address(a) + 0x8D2, ax);
    s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
    s16 base = load<s16>(address(a) + 0x322);
    yaw -= base;
    f32 len = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
    s32 pitch = gabi::call<s32>(0x020195B0, dy, len);
    store<s16>(address(a) + 0x82A, yaw);
    store<s16>(address(a) + 0x822, pitch);
    store<s16>(address(a) + 0x824, yaw);
  }
  if (load<u8>(address(a) + 0x9F0) == 6) {
    store<s16>(address(a) + 0x822, 0x4000);
    store<s16>(address(a) + 0x828, -0x4000);
  }
  gabi::call<void>(0x02389A24, a);
  gabi::call<void>(0x02389DC4, a);
  set_moveBG_mtx_light_A(a);
  set_moveBG_mtx_light_B(a);
  store<u32>(address(a) + 0x8C0, 0);
}
VERIFY(0x0238A1F0, CreateInit);
// Both beam variants share the collision extension and matrix scaling
// algorithm.
static void light_matrix(Act_c *a, bool second) {
  u32 start = address(a) + (second ? 0x74C : 0x740),
      end = address(a) + (second ? 0x764 : 0x758),
      hit = address(a) + (second ? 0x899 : 0x898),
      maximum = address(a) + (second ? 0x77C : 0x778),
      lengthField = address(a) + (second ? 0x774 : 0x770);
  SearchLocal<cXyz> origin, far;
  f32 zero = load<f32>(0x1002F010), reach = load<f32>(0x1002F014);
  far->z = reach;
  origin->z = zero;
  origin->x = zero;
  origin->y = zero;
  far->y = zero;
  far->x = zero;
  u32 block = load<u32>(address(a->mpSearchModel.get()) + 0x2C);
  u16 flags = load<u16>(block + 4);
  u32 array = load<u32>(block + 0x10);
  store<u16>(block + 4, flags | 0x10);
  gabi::call<void>(0x028E90D4, gabi::at<void>(array + (second ? 0x120 : 0xF0)),
                   gabi::at<void>(0x1048D0CC));
  f32 x = load<f32>(second ? 0x1002F03C : 0x1002F018),
      y = load<f32>(second ? 0x1002F040 : 0x1002F01C),
      z = load<f32>(second ? 0x1002F044 : 0x1002F020);
  gabi::call<void>(0x025F24E0, x, y, z);
  gabi::call<void>(0x025F19F8, gabi::at<void>(0x1048D0CC), second ? -0x8000 : 0,
                   second ? 0xC8 : -0xC8, 0);
  gabi::call<void>(0x028E8F64, gabi::at<void>(0x1048D0CC), origin.get(),
                   gabi::at<void>(start));
  SearchLocal<be<f32>> dot;
  *dot = zero;
  gabi::call<void>(0x028E8F64, gabi::at<void>(0x1048D0CC), far.get(),
                   gabi::at<void>(end));
  if (a->mMode != 5)
    wall_pos(gabi::at<Bgc_c>(address(a) + 0x3B0), a, gabi::at<cXyz>(start),
             gabi::at<cXyz>(end), gabi::at<bool>(hit),
             gabi::at<f32>(address(dot.get())));
  f32 square =
      gabi::call<f32>(0x028E8DE8, gabi::at<void>(start), gabi::at<void>(end));
  f32 length = gabi::call<f32>(0x028F4384, square);
  if (load<u8>(hit)) {
    f32 factor = f32(*dot) + load<f32>(0x1002F024);
    bool high = second || load<u16>(address(a) + 0x2D8) != 0;
    u8 control = load<u8>(address(a) + 0x9F0);
    f32 extension = factor * load<f32>(high ? 0x1002F030 : 0x1002F028);
    if (control == 6) {
      f32 tweak = load<f32>(0x1047BCD0);
      f32 base = load<f32>(high ? 0x1002F034 : 0x1002F02C);
      extension = -(factor * (high ? base + tweak : base - tweak));
    }
    length += extension;
    SearchLocal<cXyz> difference, normal, offset, sum;
    gabi::call<void>(0x0201ADE0, gabi::at<void>(end), difference.get(),
                     gabi::at<void>(start));
    gabi::call<void>(0x0201B12C, difference.get(), normal.get());
    gabi::call<void>(0x0201AE48, normal.get(), offset.get(), extension);
    gabi::call<void>(0x0201AD78, gabi::at<void>(end), sum.get(), offset.get());
    u32 sx = load<u32>(address(sum.get())),
        sz = load<u32>(address(sum.get()) + 8);
    store<u32>(end, sx);
    u32 sy = load<u32>(address(sum.get()) + 4);
    store<u32>(end + 8, sz);
    store<u32>(end + 4, sy);
  }
  u8 kind = load<u8>(address(a) + 0x902);
  if (isSecond(a, kind) || length > load<f32>(maximum))
    store<f32>(maximum, length);
  store<f32>(lengthField, length);
  gabi::call<void>(0x025F19F8, gabi::at<void>(0x1048D0CC), 0, 0, 0xC80);
  kind = load<u8>(address(a) + 0x902);
  if (!isSecond(a, kind))
    gabi::call<void>(0x025F23EC);
  f32 scaleFactor = load<f32>(0x1002F038), size = load<f32>(maximum);
  x = load<f32>(address(a) + 0x330);
  y = load<f32>(address(a) + 0x334);
  gabi::call<void>(0x025F2518, x, y, size * scaleFactor);
  mtx_copy(
      gabi::at<Mtx34>(address(a->mpBeamModel[second ? 1 : 0].get()) + 0xC8),
      gabi::at<Mtx34>(0x1048D0CC));
  kind = load<u8>(address(a) + 0x902);
  if (!isSecond(a, kind)) {
    gabi::call<void>(0x025F2468);
    size = load<f32>(lengthField);
    x = load<f32>(address(a) + 0x330);
    y = load<f32>(address(a) + 0x334);
    gabi::call<void>(0x025F2518, x, y, size * scaleFactor);
  }
  gabi::call<void>(0x028E90D4, gabi::at<void>(0x1048D0CC),
                   gabi::at<void>(address(a) + (second ? 0x868 : 0x838)));
}
void set_mtx_light_A(Act_c *a) {
  WWHD_FUNC(0x02389A24, void, a);
  light_matrix(a, false);
}
VERIFY(0x02389A24, set_mtx_light_A);
void set_mtx_light_B(Act_c *a) {
  WWHD_FUNC(0x02389DC4, void, a);
  light_matrix(a, true);
}
VERIFY(0x02389DC4, set_mtx_light_B);
void modeFind2nd(Act_c *a) {
  WWHD_FUNC(0x0238D128, void, a);
  u32 play = address(gabi::call<void *>(0x025200D4)),
      player = load<u32>(play + 0x5B2C);
  gabi::call<s32>(0x0238B838, a);
  SearchLocal<cXyz> offset, target, diff;
  if (load<s32>(0x1046BE14)) {
    offset->x = load<f32>(0x1046BDE4);
    offset->y = load<f32>(0x1046BDE8);
    offset->z = load<f32>(0x1046BDEC);
  } else {
    f32 zero = load<f32>(0x1002F010), height = load<f32>(0x1002F1B8);
    store<f32>(0x1046BDE4, zero);
    offset->y = height;
    store<f32>(0x1046BDEC, zero);
    store<s32>(0x1046BE14, 1);
    offset->z = zero;
    offset->x = zero;
    store<f32>(0x1046BDE8, height);
  }
  gabi::call<void>(0x0201AD78, offset.get(), target.get(),
                   gabi::at<void>(player + 0x314));
  u32 beam = load<u32>(address(a) + 0x950);
  gabi::call<void>(0x0201ADE0, target.get(), diff.get(),
                   gabi::at<void>(address(a) + 0x740 + beam * 12));
  s16 current = load<s16>(address(a) + 0x824);
  store<s16>(address(a) + 0x8D0, current);
  offset->y = diff->y;
  f32 dz = diff->z, dx = diff->x;
  offset->z = dz;
  offset->x = dx;
  s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
  dz = offset->z;
  s16 base = load<s16>(address(a) + 0x322);
  dx = offset->x;
  s16 desiredYaw = s16(yaw - base);
  f32 len = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
  s32 pitch = gabi::call<s32>(0x020195B0, f32(offset->y), len);
  s16 lower = s16(load<s16>(0x1047BD4A) - 10000),
      upper = s16(load<s16>(0x1047BD48) + 26000);
  bool above = (u32(upper) - u32(pitch) - 1) >> 31,
       below = (u32(pitch) - u32(lower) - 1) >> 31;
  if (above) {
    if (upper > pitch)
      pitch = upper;
    pitch = s16(pitch);
  }
  if (below) {
    if (lower < pitch)
      pitch = lower;
    pitch = s16(pitch);
  }
  SearchLocal<cXyz> eye;
  eye->x = load<f32>(player + 0x3D8);
  beam = load<u32>(address(a) + 0x950);
  eye->y = load<f32>(player + 0x3DC);
  eye->z = load<f32>(player + 0x3E0);
  gabi::call<void>(0x024F1AFC, gabi::at<void>(address(a) + 0x984),
                   gabi::at<void>(address(a) + 0x740 + beam * 12), eye.get(),
                   a);
  play = address(gabi::call<void *>(0x025200D4));
  if (gabi::call<s32>(0x02008860, gabi::at<void>(play + 0x12A0),
                      gabi::at<void>(address(a) + 0x984)) ||
      above || below)
    modeProc(a, 0, 1);
  beam = load<u32>(address(a) + 0x950);
  u32 rotations = address(a) + 0x822;
  if (beam == 0) {
    store<s16>(rotations + 8, desiredYaw);
    beam = load<u32>(address(a) + 0x950);
    gabi::call<void>(0x0200F428, gabi::at<void>(rotations + beam * 6 + 2),
                     desiredYaw, 10, 0x400);
    beam = load<u32>(address(a) + 0x950);
    gabi::call<void>(0x0200F428, gabi::at<void>(rotations + beam * 6), pitch,
                     10, 0x400);
  } else {
    desiredYaw = s16(desiredYaw - 0x8000);
    store<s16>(rotations + 2, desiredYaw);
    beam = load<u32>(address(a) + 0x950);
    pitch = s16(-pitch);
    gabi::call<void>(0x0200F428, gabi::at<void>(rotations + beam * 6 + 2),
                     desiredYaw, 10, 0x400);
    beam = load<u32>(address(a) + 0x950);
    gabi::call<void>(0x0200F428, gabi::at<void>(rotations + beam * 6), pitch,
                     10, 0x400);
  }
}
VERIFY(0x0238D128, modeFind2nd);
s32 actor_draw(Act_c *a) {
  WWHD_FUNC(0x0238B258, s32, a);
  store<f32>(0x1048D044, load<f32>(0x1002F198));
  gabi::call<void>(0x0283801C, gabi::at<void>(0x1048CFF0));
  u32 matrix = address(a->mpSearchModel.get());
  if (matrix)
    matrix += 0xC8;
  f32 low = load<f32>(0x1002F05C), high = load<f32>(0x1002F060),
      zero = load<f32>(0x1002F010), height = load<f32>(0x1002F064);
  u8 culled = gabi::call<u8>(0x025D6B70, gabi::at<void>(matrix), low, zero, low,
                             high, height, high);
  matrix = address(a->mpBeamModel[0].get());
  store<u8>(address(a) + 0x8D8, culled);
  if (matrix)
    matrix += 0xC8;
  f32 bHigh = load<f32>(0x1002F1A0), bLow = load<f32>(0x1002F19C),
      near = load<f32>(0x1002F1A4), far = load<f32>(0x1002F1A8);
  culled = gabi::call<u8>(0x025D6B70, gabi::at<void>(matrix), bLow, bLow, near,
                          bHigh, bHigh, far);
  matrix = address(a->mpBeamModel[1].get());
  store<u8>(address(a) + 0x8D6, culled);
  if (matrix)
    matrix += 0xC8;
  near = load<f32>(0x1002F1A4);
  far = load<f32>(0x1002F1A8);
  culled = gabi::call<u8>(0x025D6B70, gabi::at<void>(matrix), bLow, bLow, near,
                          bHigh, bHigh, far);
  store<u8>(address(a) + 0x8D7, culled);
  store<f32>(0x1048D044, load<f32>(0x1048D04C));
  gabi::call<void>(0x0283801C, gabi::at<void>(0x1048CFF0));
  if (!load<u8>(address(a) + 0x8D8)) {
    if (load<u8>(address(a) + 0x956) == 1) {
      u32 env = address(gabi::call<void *>(0x02555D0C));
      void *again = gabi::call<void *>(0x02555D0C);
      gabi::call<void>(0x025626A4, again, 1, gabi::at<void>(address(a) + 0x314),
                       gabi::at<void>(address(a) + 0x110));
      for (u32 i = 0; i < 3; i++)
        store<u16>(address(a) + 0x1A0 + i * 2, load<u8>(env + 0xB64 + i));
      for (u32 i = 0; i < 3; i++)
        store<u8>(address(a) + 0x1A8 + i, load<u8>(env + 0xB68 + i));
    } else {
      void *env = gabi::call<void *>(0x02555D0C);
      gabi::call<void>(0x025626A4, env, 0, gabi::at<void>(address(a) + 0x314),
                       gabi::at<void>(address(a) + 0x110));
    }
    void *env = gabi::call<void *>(0x02555D0C);
    gabi::call<void>(0x02562F5C, env, a->mpSearchModel.get(),
                     gabi::at<void>(address(a) + 0x110));
    u32 play = address(gabi::call<void *>(0x025200D4));
    store<u32>(0x104B4634, load<u32>(play + 0x5D70));
    play = address(gabi::call<void *>(0x025200D4));
    store<u32>(0x104B4638, load<u32>(play + 0x5D74));
    gabi::call<void>(0x025E2E5C, a->mpSearchModel.get());
    play = address(gabi::call<void *>(0x025200D4));
    store<u32>(0x104B4634, load<u32>(play + 0x5D78));
    play = address(gabi::call<void *>(0x025200D4));
    store<u32>(0x104B4638, load<u32>(play + 0x5D7C));
  }
  if (!load<u8>(address(a) + 0x8D6) && load<s16>(address(a) + 0x89A) > 0) {
    SearchLocal<cXyz> difference, normal, secondDifference, direction, point,
        position;
    gabi::call<void>(0x0201ADE0, gabi::at<void>(address(a) + 0x758),
                     difference.get(), gabi::at<void>(address(a) + 0x740));
    gabi::call<void>(0x0201B084, difference.get(), normal.get());
    gabi::call<void>(0x0201ADE0, gabi::at<void>(address(a) + 0x758),
                     secondDifference.get(),
                     gabi::at<void>(address(a) + 0x740));
    f32 sq = gabi::call<f32>(0x028E8DD0, secondDifference.get());
    f32 length = gabi::call<f32>(0x028F4384, sq);
    direction->z = normal->z;
    direction->y = normal->y;
    direction->x = normal->x;
    u8 kind = load<u8>(address(a) + 0x902);
    s32 mode = initialMode(a, kind);
    f32 offset = load<f32>(0x101CCA80 + u32(mode) * 4);
    gabi::call<void>(0x028E8E64, direction.get(), direction.get(), offset);
    gabi::call<void>(0x0201AD78, gabi::at<void>(address(a) + 0x740),
                     point.get(), direction.get());
    s16 red = load<s16>(address(a) + 0x8A8),
        green = load<s16>(address(a) + 0x8AA);
    position->z = point->z;
    direction->x = normal->x;
    position->x = point->x;
    direction->y = normal->y;
    s16 blue = load<s16>(address(a) + 0x8AC);
    position->y = point->y;
    direction->z = normal->z;
    struct Color {
      be<f32> r, g, b, a;
    };
    SearchLocal<Color> color;
    f32 full = load<f32>(0x1002F1AC);
    color->r = f32(red) / full;
    color->a = load<f32>(0x1002F008);
    color->g = f32(green) / full;
    color->b = f32(blue) / full;
    kind = load<u8>(address(a) + 0x902);
    mode = initialMode(a, kind);
    f32 width = load<f32>(0x101CCA94 + u32(mode) * 4);
    kind = load<u8>(address(a) + 0x902);
    mode = initialMode(a, kind);
    offset = load<f32>(0x101CCA80 + u32(mode) * 4);
    void *renderer = gabi::at<void>(load<u32>(0x101F8A20));
    f32 radius = length - offset + high;
    zero = load<f32>(0x1002F010);
    f32 opacity = load<f32>(0x101CCAA8);
    gabi::call<void>(0x027386D4, renderer, position.get(), direction.get(),
                     color.get(), 0, gabi::at<void>(0x104A01CC), width, radius,
                     zero, opacity);
    gabi::call<void>(0x025E2DE0, a->mpBeamModel[0].get(), 0);
  }
  if (!load<u8>(address(a) + 0x8D7) && load<s16>(address(a) + 0x89A) > 0)
    gabi::call<void>(0x025E2DE0, a->mpBeamModel[1].get(), 0);
  return 1;
}
VERIFY(0x0238B258, actor_draw);
static bool equal_string(u32 a, u32 b) {
  for (;;) {
    u8 x = load<u8>(a++), y = load<u8>(b++);
    if (x != y)
      return false;
    if (!x)
      return true;
  }
}
static s32 angle_abs(s32 x) { return x < 0 ? s32(0u - u32(x)) : x; }
void modeToStop(Act_c *a) {
  WWHD_FUNC(0x0238C834, void, a);
  u32 save = load<u32>(0x101F84DC);
  s32 event = gabi::call<s32>(0x025B8B94, gabi::at<void>(save + 0x644), 0x201);
  f32 speed = load<f32>(0x1002EFF8);
  if (!event && load<s32>(address(a) + 0x97C) == 255) {
    if (load<u16>(address(a) + 0xF8) != 2) {
      gabi::call<void>(0x025D77DC, a, STR(0x1002F1D4), 1, 0xFFFF);
      return;
    }
    store<s16>(address(a) + 0x89A, 255);
    u32 play = address(gabi::call<void *>(0x025200D4));
    s32 staff = gabi::call<s32>(0x02542D88, gabi::at<void>(play + 0x52C4),
                                STR(0x1002F1CC), 0, 0);
    play = address(gabi::call<void *>(0x025200D4));
    if (gabi::call<s32>(0x0254457C, gabi::at<void>(play + 0x52C4),
                        STR(0x1002F1D4))) {
      play = address(gabi::call<void *>(0x025200D4));
      store<u16>(play + 0x52B8, load<u16>(play + 0x52B8) | 8);
      save = load<u32>(0x101F84DC);
      gabi::call<void>(0x025B8B68, gabi::at<void>(save + 0x644), 0x201);
      modeProc(a, 0, 2);
      return;
    }
    SearchLocal<be<u32>> id;
    u32 child = load<u32>(address(a) + 0x96C);
    *id = child;
    void *controller = nullptr;
    if (child != 0xFFFFFFFF)
      controller =
          gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E1234), id.get());
    if (controller && load<s16>(address(controller) + 0x41A) < 5)
      store<s16>(address(controller) + 0x41A, 5);
    play = address(gabi::call<void *>(0x025200D4));
    u32 cut = address(
        gabi::call<void *>(0x02544830, gabi::at<void>(play + 0x52C4), staff));
    if (!cut) {
      gabi::call<void>(0x0273AA24, STR(0x1002F1F4), 0x279, STR(0x1002F1E4));
      return;
    }
    if (equal_string(cut, 0x1002F1C4)) {
      play = address(gabi::call<void *>(0x025200D4));
      gabi::call<void>(0x02543280, gabi::at<void>(play + 0x52C4), staff);
    }
    if (equal_string(cut, 0x1002F208)) {
      s8 room = load<s8>(address(a) + 0x326);
      s32 reverb = gabi::call<s32>(0x02520540, room);
      gabi::call<void>(0x025E1A40, 0x6939, gabi::at<void>(address(a) + 0x37C),
                       0, reverb);
      play = address(gabi::call<void *>(0x025200D4));
      gabi::call<void>(0x02543280, gabi::at<void>(play + 0x52C4), staff);
    }
    if (equal_string(cut, 0x1002F218)) {
      s32 dx = gabi::call<s32>(0x0200F378, gabi::at<void>(address(a) + 0x822),
                               0x2300, 30, 0x300, 0x10);
      s32 dz = gabi::call<s32>(0x0200F378, gabi::at<void>(address(a) + 0x828),
                               -0x2300, 30, 0x300, 0x10);
      if (angle_abs(dx) < 0x100 && angle_abs(dz) < 0x100) {
        smoke_set(a, speed, 10);
        play = address(gabi::call<void *>(0x025200D4));
        gabi::call<void>(0x02543280, gabi::at<void>(play + 0x52C4), staff);
      }
    }
    return;
  }
  s32 dx = gabi::call<s32>(0x0200F378, gabi::at<void>(address(a) + 0x822),
                           0x2300, 20, 0x200, 0x10);
  s32 dz = gabi::call<s32>(0x0200F378, gabi::at<void>(address(a) + 0x828),
                           -0x2300, 20, 0x200, 0x10);
  if (angle_abs(dx) < 0x100 && angle_abs(dz) < 0x100) {
    smoke_set(a, speed, 10);
    modeProc(a, 0, 2);
  }
}
VERIFY(0x0238C834, modeToStop);
void modeFind(Act_c *a) {
  WWHD_FUNC(0x0238CC18, void, a);
  for (u32 name : {0x1002F234u, 0x1002F248u, 0x1002F268u}) {
    u32 play = address(gabi::call<void *>(0x025200D4));
    gabi::call<s32>(0x02543F10, gabi::at<void>(play + 0x52C4),
                    gabi::at<void>(name), 255);
  }
  u32 play = address(gabi::call<void *>(0x025200D4)),
      player = load<u32>(play + 0x5B2C);
  gabi::call<s32>(0x0238B838, a);
  SearchLocal<cXyz> offset, target, diff;
  if (load<s32>(0x1046BE10)) {
    offset->x = load<f32>(0x1046BDD8);
    offset->y = load<f32>(0x1046BDDC);
    offset->z = load<f32>(0x1046BDE0);
  } else {
    f32 zero = load<f32>(0x1002F010), height = load<f32>(0x1002F1B8);
    store<f32>(0x1046BDD8, zero);
    offset->y = height;
    store<f32>(0x1046BDE0, zero);
    store<s32>(0x1046BE10, 1);
    offset->z = zero;
    offset->x = zero;
    store<f32>(0x1046BDDC, height);
  }
  gabi::call<void>(0x0201AD78, offset.get(), target.get(),
                   gabi::at<void>(player + 0x314));
  u32 beam = load<u32>(address(a) + 0x950);
  gabi::call<void>(0x0201ADE0, target.get(), diff.get(),
                   gabi::at<void>(address(a) + 0x740 + beam * 12));
  s16 current = load<s16>(address(a) + 0x824);
  store<s16>(address(a) + 0x8D0, current);
  offset->y = diff->y;
  f32 dz = diff->z, dx = diff->x;
  offset->z = dz;
  offset->x = dx;
  s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
  dz = offset->z;
  s16 base = load<s16>(address(a) + 0x322);
  dx = offset->x;
  s16 desiredYaw = s16(yaw - base);
  f32 len = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
  s32 pitch = gabi::call<s32>(0x020195B0, f32(offset->y), len);

  beam = load<u32>(address(a) + 0x950);
  u32 rotations = address(a) + 0x822;
  if (beam == 0) {
    store<s16>(rotations + 8, desiredYaw);
    beam = load<u32>(address(a) + 0x950);
    gabi::call<void>(0x0200F428, gabi::at<void>(rotations + beam * 6 + 2),
                     desiredYaw, 10, 0x400);
    beam = load<u32>(address(a) + 0x950);
    gabi::call<void>(0x0200F428, gabi::at<void>(rotations + beam * 6), pitch,
                     10, 0x400);
  } else {
    desiredYaw = s16(desiredYaw - 0x8000);
    pitch = s16(-pitch);
    store<s16>(rotations + 2, desiredYaw);
    beam = load<u32>(address(a) + 0x950);
    gabi::call<void>(0x0200F428, gabi::at<void>(rotations + beam * 6 + 2),
                     desiredYaw, 10, 0x400);
    beam = load<u32>(address(a) + 0x950);
    gabi::call<void>(0x0200F428, gabi::at<void>(rotations + beam * 6), pitch,
                     10, 0x400);
  }
  if (load<u16>(address(a) + 0xF8) == 2) {
    play = address(gabi::call<void *>(0x025200D4));
    s32 staff = gabi::call<s32>(0x02542D88, gabi::at<void>(play + 0x52C4),
                                STR(0x1002F22C), 0, 0);
    bool ended = false;
    for (u32 name : {0x1002F234u, 0x1002F248u, 0x1002F268u}) {
      play = address(gabi::call<void *>(0x025200D4));
      if (gabi::call<s32>(0x0254457C, gabi::at<void>(play + 0x52C4),
                          gabi::at<void>(name))) {
        ended = true;
        break;
      }
    }
    if (ended) {
      gabi::call<void>(0x025E1AE0, 0x834, 20);
      gabi::call<void>(0x0252012C, STR(0x1002F224), 0, 0, -1, 0, 1, 0,
                       load<f32>(0x1002F010));
      return;
    }
    play = address(gabi::call<void *>(0x025200D4));
    bool stop = load<u32>(play + 0x5CD8) & 1;
    if (!stop) {
      play = address(gabi::call<void *>(0x025200D4));
      stop = load<u32>(play + 0x5CD8) & 0x100;
    }
    if (!stop) {
      store<s16>(player + 0x420, 3);
      store<u32>(player + 0x428, 0);
      beam = load<u32>(address(a) + 0x950);
      s16 angle = load<s16>(rotations + beam * 6 + 2),
          base = load<s16>(address(a) + 0x322);
      s16 yaw = s16(angle + base);
      if (beam == 0)
        yaw = s16(yaw - 0x8000);
      gabi::call<void>(0x0200F428, gabi::at<void>(address(a) + 0x900), yaw, 4,
                       0x400);
      u32 vt = load<u32>(player + 0xB4), target = load<u32>(vt + 0x114);
      angle = load<s16>(address(a) + 0x900);
      gabi::call<void>(target, gabi::at<void>(player),
                       gabi::at<void>(player + 0x314), angle);
      angle = load<s16>(address(a) + 0x900);
      if (gabi::call<s32>(0x0200FAAC, yaw, angle) < 0x500) {
        store<s16>(player + 0x420, 2);
        store<u32>(player + 0x430, 1);
      }
    }
    play = address(gabi::call<void *>(0x025200D4));
    gabi::call<void>(0x02543280, gabi::at<void>(play + 0x52C4), staff);
    return;
  }
  u32 vt = load<u32>(player + 0xB4), targetFn = load<u32>(vt + 0x4C);
  if (gabi::call<s32>(targetFn, gabi::at<void>(player)))
    return;
  if (load<f32>(player + 0x3CC) < load<f32>(0x1002F010)) {
    gabi::call<void>(0x025D77DC, a, STR(0x1002F248), 1, 0xFFFF);
    return;
  }
  play = address(gabi::call<void *>(0x025200D4));
  u32 status = load<u32>(play + 0x5CD8);
  gabi::call<void>(0x025D77DC, a, STR(status & 1 ? 0x1002F268 : 0x1002F234), 1,
                   0xFFFF);
}
VERIFY(0x0238CC18, modeFind);
static void vec_copy(cXyz *d, const cXyz *s) { d->copy(*s); }
static f32 project_hit(Act_c *a, u32 other, u32 start, u32 end, bool second) {
  SearchLocal<cXyz> temporary, relative, beam, unit, scaled, point;
  gabi::call<void>(0x0201ADE0, gabi::at<void>(other + 0x314), temporary.get(),
                   gabi::at<void>(start));
  vec_copy(relative.get(), temporary.get());
  gabi::call<void>(0x0201ADE0, gabi::at<void>(end), temporary.get(),
                   gabi::at<void>(start));
  vec_copy(beam.get(), temporary.get());
  gabi::call<void>(0x0201B12C, beam.get(), temporary.get());
  vec_copy(unit.get(), temporary.get());
  f32 dot = gabi::call<f32>(0x028E8F44, unit.get(), relative.get());
  gabi::call<void>(0x0201AE48, unit.get(), scaled.get(), dot);
  gabi::call<void>(0x0201AD78, gabi::at<void>(start), point.get(),
                   scaled.get());
  f32 height;
  if (second) {
    store<f32>(address(a) + 0x8A4, f32(point->z));
    store<f32>(address(a) + 0x89C, f32(point->x));
    height = point->y;
    store<f32>(address(a) + 0x8A0, height);
  } else {
    store<f32>(address(a) + 0x89C, f32(point->x));
    height = point->y;
    store<f32>(address(a) + 0x8A0, height);
    store<f32>(address(a) + 0x8A4, f32(point->z));
  }
  u8 kind = load<u8>(address(a) + 0x902);
  s32 mode = initialMode(a, kind);
  store<f32>(address(a) + 0x8A0,
             height + load<f32>(0x101CCA80 + u32(mode) * 4));
  return height;
}
s32 player_check(Act_c *a) {
  WWHD_FUNC(0x0238B838, s32, a);
  u32 play = address(gabi::call<void *>(0x025200D4)),
      player = load<u32>(play + 0x5B2C);
  SearchLocal<cXyz> temporary, copy;
  gabi::call<void>(0x0201ADE0, gabi::at<void>(player + 0x314), temporary.get(),
                   gabi::at<void>(address(a) + 0x740));
  vec_copy(copy.get(), temporary.get());
  f32 sq = gabi::call<f32>(0x028E8DD0, copy.get());
  f32 distanceA = gabi::call<f32>(0x028F4384, sq);
  gabi::call<void>(0x0201ADE0, gabi::at<void>(player + 0x314), temporary.get(),
                   gabi::at<void>(address(a) + 0x74C));
  vec_copy(copy.get(), temporary.get());
  sq = gabi::call<f32>(0x028E8DD0, copy.get());
  f32 distanceB = gabi::call<f32>(0x028F4384, sq);
  gabi::call<void>(0x02018808, gabi::at<void>(address(a) + 0x540),
                   gabi::at<void>(address(a) + 0x740),
                   gabi::at<void>(address(a) + 0x758));
  f32 radius = load<f32>(0x1002F068);
  store<f32>(address(a) + 0x55C, radius);
  play = address(gabi::call<void *>(0x025200D4));
  gabi::call<void>(0x0200E240, gabi::at<void>(play + 0x26A4),
                   gabi::at<void>(address(a) + 0x428));
  s32 hit = gabi::call<s32>(0x02516464, gabi::at<void>(address(a) + 0x428));
  f32 speed = load<f32>(0x1002F1B0), far = load<f32>(0x1002F18C),
      zero = load<f32>(0x1002F010);
  if (hit) {
    u32 other = address(
        gabi::call<void *>(0x02515BBC, gabi::at<void>(address(a) + 0x504)));
    project_hit(a, other, address(a) + 0x740, address(a) + 0x758, false);
    if (other && load<s16>(other + 8) == 168) {
      if (distanceA < far || distanceB < far)
        return 0;
      if (!(std::fabs(load<f32>(player + 0x370)) < speed) ||
          !(load<f32>(player + 0x3CC) < zero))
        return 1;
    }
  }
  gabi::call<void>(0x02018808, gabi::at<void>(address(a) + 0x678),
                   gabi::at<void>(address(a) + 0x74C),
                   gabi::at<void>(address(a) + 0x764));
  store<f32>(address(a) + 0x694, radius);
  play = address(gabi::call<void *>(0x025200D4));
  gabi::call<void>(0x0200E240, gabi::at<void>(play + 0x26A4),
                   gabi::at<void>(address(a) + 0x560));
  if (gabi::call<s32>(0x02516464, gabi::at<void>(address(a) + 0x560))) {
    u32 other = address(
        gabi::call<void *>(0x02515BBC, gabi::at<void>(address(a) + 0x63C)));
    project_hit(a, other, address(a) + 0x74C, address(a) + 0x764, true);
    if (distanceA < far)
      return 0;
    if (distanceB < far)
      return 0;
    if (!(std::fabs(load<f32>(player + 0x370)) < speed) ||
        !(load<f32>(player + 0x3CC) < zero))
      return 1;
  }
  if (!gabi::call<s32>(0x02516464, gabi::at<void>(address(a) + 0x428)) &&
      !gabi::call<s32>(0x02516464, gabi::at<void>(address(a) + 0x560))) {
    struct Line {
      u8 bytes[0x6C];
    };
    SearchLocal<Line> line;
    u32 p = address(line.get());
    gabi::call<void>(0x02008FEC, line.get());
    store<u32>(p, p + 0x58);
    store<u32>(p + 4, p + 0x64);
    store<u8>(p + 0x60, 0);
    store<u8>(p + 0x5E, 0);
    store<u8>(p + 0x5C, 0);
    store<u32>(p + 0x10, 0x1002EF34);
    store<u8>(p + 0x62, 0);
    store<u8>(p + 0x5F, 0);
    store<u32>(p + 0x58, 0x1002EF64);
    store<u32>(p + 0x64, 0x1002EF54);
    store<u32>(p + 0x20, 0x1002EF44);
    store<u32>(p + 0x68, 3);
    store<u8>(p + 0x61, 0);
    store<u8>(p + 0x5D, 1);
    gabi::call<void>(0x024F1AFC, line.get(), gabi::at<void>(address(a) + 0x740),
                     gabi::at<void>(address(a) + 0x758), 0);
    play = address(gabi::call<void *>(0x025200D4));
    if (gabi::call<s32>(0x02008860, gabi::at<void>(play + 0x12A0),
                        line.get())) {
      store<f32>(address(a) + 0x8A4, load<f32>(p + 0x38));
      f32 height = load<f32>(p + 0x34);
      store<f32>(address(a) + 0x8A0, height);
      store<f32>(address(a) + 0x89C, load<f32>(p + 0x30));
      u8 kind = load<u8>(address(a) + 0x902);
      s32 mode = initialMode(a, kind);
      store<u32>(p + 0x58, 0x1002EEE4);
      store<u32>(p + 0x64, 0x1002EEA4);
      store<u32>(p + 0x20, 0x1002EE94);
      store<f32>(address(a) + 0x8A0,
                 height + load<f32>(0x101CCA80 + u32(mode) * 4));
    } else {
      u32 y = load<u32>(address(a) + 0x744), x = load<u32>(address(a) + 0x740),
          z = load<u32>(address(a) + 0x748);
      store<u32>(p + 0x58, 0x1002EEE4);
      store<u32>(address(a) + 0x89C, x);
      store<u32>(address(a) + 0x8A0, y);
      store<u32>(p + 0x64, 0x1002EEA4);
      store<u32>(p + 0x20, 0x1002EE94);
      store<u32>(address(a) + 0x8A4, z);
    }
    gabi::call<void>(0x02008B4C, line.get(), 0);
  }
  u8 kind = load<u8>(address(a) + 0x902);
  s32 mode = initialMode(a, kind);
  store<f32>(address(a) + 0x8BC, load<f32>(0x101CCAA8 + u32(mode) * 4));
  kind = load<u8>(address(a) + 0x902);
  mode = initialMode(a, kind);
  store<f32>(address(a) + 0x8B0, load<f32>(0x101CCA94 + u32(mode) * 4));
  return 0;
}
VERIFY(0x0238B838, player_check);
// Aim a beam at the persistent offset for a followed actor.
static void follow_actor(Act_c *a, u32 actor, u32 global, u32 guard) {
  SearchLocal<cXyz> offset, target, diff;
  f32 zero = load<f32>(0x1002F010), height = load<f32>(0x1002F1B8);
  if (load<s32>(guard)) {
    offset->x = load<f32>(global);
    offset->z = load<f32>(global + 8);
    offset->y = load<f32>(global + 4);
  } else {
    store<f32>(global, zero);
    store<f32>(global + 4, height);
    store<f32>(global + 8, zero);
    offset->y = height;
    store<u32>(guard, 1);
    offset->x = zero;
    offset->z = zero;
  }
  gabi::call<void>(0x0201AD78, offset.get(), target.get(),
                   gabi::at<void>(actor + 0x314));
  gabi::call<void>(0x0201ADE0, target.get(), diff.get(),
                   gabi::at<void>(address(a) + 0x740));
  offset->y = diff->y;
  store<s16>(address(a) + 0x8D0, load<s16>(address(a) + 0x824));
  f32 dx = diff->x, dz = diff->z;
  offset->x = dx;
  offset->z = dz;
  s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
  dz = offset->z;
  s16 base = load<s16>(address(a) + 0x322);
  dx = offset->x;
  s16 targetYaw = s16(yaw - base);
  f32 len = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
  s32 pitch = gabi::call<s32>(0x020195B0, f32(offset->y), len);
  u32 beam = load<u32>(address(a) + 0x950);
  gabi::call<void>(0x0200F428, gabi::at<void>(address(a) + 0x824 + beam * 6),
                   targetYaw, 10, 0x400);
  beam = load<u32>(address(a) + 0x950);
  gabi::call<void>(0x0200F428, gabi::at<void>(address(a) + 0x822 + beam * 6),
                   pitch, 10, 0x400);
}
void modeSearchPath(Act_c *a) {
  WWHD_FUNC(0x0238BF20, void, a);
  u32 path = load<u32>(address(a) + 0x968);
  gabi::call<void>(0x02587D24, gabi::at<void>(address(a) + 0x958),
                   gabi::at<void>(address(a) + 0x964), gabi::at<void>(path), 0,
                   0, load<f32>(0x1002F06C));
  if (load<u8>(address(a) + 0x954)) {
    SearchLocal<be<s16>> profile;
    *profile = 0xF1;
    u32 found = address(gabi::call<void *>(
        0x025D5218, gabi::at<void>(0x025E121C), profile.get()));
    if (load<s32>(address(a) + 0x97C) != 255 && found) {
      SearchLocal<be<u32>> id;
      u32 child = load<u32>(address(a) + 0x96C);
      *id = child;
      void *controller = nullptr;
      if (child != 0xFFFFFFFF)
        controller = gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E1234),
                                        id.get());
      if (controller)
        store<s16>(address(controller) + 0x1384, load<s16>(address(a) + 0x8CC));
      u8 control = load<u8>(address(a) + 0x9F0);
      if (control == 4) {
        modeProc(a, 0, 4);
        return;
      }
      if (control == 2) {
        u32 play = address(gabi::call<void *>(0x025200D4)),
            player = load<u32>(play + 0x5B2C);
        follow_actor(a, player, 0x1046BDC0, 0x1046BE08);
        return;
      }
      if (control == 3) {
        follow_actor(a, found, 0x1046BDCC, 0x1046BE0C);
        return;
      }
    } else {
      if (gabi::call<s32>(0x0238B838, a)) {
        if (load<s32>(0x101D6010) != 1) {
          s32 count = s32(u32(load<s32>(address(a) + 0x8C0)) + 1);
          store<s32>(address(a) + 0x8C0, count);
          if (count > 14) {
            modeProc(a, 0, 5);
            store<u32>(address(a) + 0x8C0, 0);
          }
        }
      } else
        store<u32>(address(a) + 0x8C0, 0);
    }
  }
  u32 z = load<u32>(address(a) + 0x960), y = load<u32>(address(a) + 0x95C);
  store<u32>(address(a) + 0x730, z);
  u32 x = load<u32>(address(a) + 0x958);
  store<u32>(address(a) + 0x72C, y);
  store<u32>(address(a) + 0x728, x);
  SearchLocal<cXyz> diff;
  gabi::call<void>(0x0201ADE0, gabi::at<void>(address(a) + 0x728), diff.get(),
                   gabi::at<void>(address(a) + 0x740));
  f32 dz = diff->z;
  s16 angle = load<s16>(address(a) + 0x824), ax = load<s16>(address(a) + 0x822);
  store<s16>(address(a) + 0x8D0, angle);
  f32 dx = diff->x;
  store<s16>(address(a) + 0x8D2, ax);
  angle = load<s16>(address(a) + 0x828);
  f32 dy = diff->y;
  store<s16>(address(a) + 0x8D4, angle);
  s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
  s16 base = load<s16>(address(a) + 0x322);
  s16 targetYaw = s16(yaw - base);
  f32 len = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
  s32 pitch = gabi::call<s32>(0x020195B0, dy, len);
  if (load<s16>(0x1047BD50) == 0) {
    gabi::call<void>(0x0200F428, gabi::at<void>(address(a) + 0x824), targetYaw,
                     30, 0x400);
    gabi::call<void>(0x0200F428, gabi::at<void>(address(a) + 0x822), pitch, 30,
                     0x400);
    angle = load<s16>(address(a) + 0x824);
    store<s16>(address(a) + 0x828, 0xE38);
    store<s16>(address(a) + 0x82A, angle);
  } else {
    store<s16>(address(a) + 0x824, targetYaw);
    store<s16>(address(a) + 0x822, pitch);
    store<s16>(address(a) + 0x828, 0xE38);
    store<s16>(address(a) + 0x82A, targetYaw);
  }
}
VERIFY(0x0238BF20, modeSearchPath);
void modeSearchBdk(Act_c *a) {
  WWHD_FUNC(0x0238D444, void, a);
  SearchLocal<be<u32>> found;
  if (!gabi::call<s32>(0x025D5578, 0xEE, found.get()))
    return;
  u32 bird = *found;
  if (!bird)
    return;
  f32 z = load<f32>(bird + 0x31C), y = load<f32>(bird + 0x318),
      x = load<f32>(bird + 0x314), tenThousand = load<f32>(0x1002F284),
      radius = load<f32>(0x1047BCD0), height = load<f32>(0x1047BCD8);
  store<f32>(address(a) + 0x9F4, load<f32>(0x1002F280));
  store<f32>(address(a) + 0x9F8, tenThousand);
  store<f32>(address(a) + 0x9FC, load<f32>(0x1002F288));
  store<f32>(address(a) + 0xA08, load<f32>(0x1047BA90) + load<f32>(0x1002F030));
  store<f32>(address(a) + 0xA04, load<f32>(0x1047BAA4) + load<f32>(0x1002F28C));
  store<s16>(address(a) + 0xA0C, load<s16>(0x1047BB08) + 0x3500);
  store<s16>(address(a) + 0xA00, load<s16>(0x1047BB0A));
  height = y + height;
  SearchLocal<cXyz> difference, horizontal, origin;
  gabi::call<void>(0x0201ADE0, gabi::at<void>(address(a) + 0x9F4),
                   difference.get(), gabi::at<void>(bird + 0x314));
  f32 zero = load<f32>(0x1002F010);
  horizontal->z = difference->z;
  horizontal->x = difference->x;
  horizontal->y = zero;
  f32 sq = gabi::call<f32>(0x028E8DD0, horizontal.get());
  gabi::call<f32>(0x028F4384, sq);
  origin->x = load<f32>(address(a) + 0x9F4);
  origin->y = load<f32>(address(a) + 0x9F8);
  origin->z = load<f32>(address(a) + 0x9FC);
  f32 fanRange = load<f32>(address(a) + 0xA04),
      threeThousand = load<f32>(0x1002F290);
  s16 direction = load<s16>(address(a) + 0xA00),
      angle = load<s16>(address(a) + 0xA0C);
  bird = *found;
  u8 inside = gabi::call<u8>(0x02588230, origin.get(), gabi::at<void>(bird),
                             direction, angle, fanRange, threeThousand);
  origin->y = load<f32>(address(a) + 0x9F8);
  store<u8>(address(a) + 0xA0E, inside);
  origin->z = load<f32>(address(a) + 0x9FC);
  origin->x = load<f32>(address(a) + 0x9F4);
  bird = *found;
  f32 circle = load<f32>(address(a) + 0xA08);
  if (gabi::call<s32>(0x025880B4, origin.get(), gabi::at<void>(bird), circle,
                      tenThousand))
    store<u8>(address(a) + 0xA0E, 0);
  s16 state = load<s16>(bird + 0x3DA);
  f32 hundred = load<f32>(0x1002F1B8);
  bool track = true;
  if (u32(s32(state)) <= 1 || state == 7) {
    if (load<u8>(address(a) + 0xA0E)) {
      height += hundred;
      radius += load<f32>(0x1002F294);
    } else
      track = false;
  } else if (u32(s32(state)) < 7 ||
             (u32(s32(state)) > 7 && u32(s32(state)) < 10)) {
    if (load<u8>(address(a) + 0xA0E)) {
      radius += threeThousand;
      height += load<f32>(0x1002F060);
    } else
      track = false;
  } else if (state == 10) {
    s16 phase = load<s16>(bird + 0x3DC);
    if (phase == 8) {
      if (gabi::call<s32>(0x0211D2F8, gabi::at<void>(address(a) + 0xA24))) {
        x = load<f32>(address(a) + 0xA14);
        height = load<f32>(address(a) + 0xA18);
        z = load<f32>(address(a) + 0xA1C);
      } else
        track = false;
    } else if (phase == 7) {
      f32 random = gabi::call<f32>(0x020198D8, hundred);
      s16 adjustment = load<s16>(0x1047BB10);
      f32 base = load<f32>(0x1002F298);
      height = load<f32>(address(a) + 0xA18);
      z = load<f32>(address(a) + 0xA1C);
      x = load<f32>(address(a) + 0xA14);
      store<s32>(address(a) + 0xA24,
                 gabi::ftoi((random + base) + f32(adjustment)));
    } else if (phase >= 6) {
      store<f32>(address(a) + 0xA18, height);
      store<f32>(address(a) + 0xA1C, z);
      store<f32>(address(a) + 0xA14, x);
    } else if (phase >= 5) {
      radius += hundred;
      height += hundred;
    } else
      track = false;
  } else
    track = false;
  s32 sign = (load<u32>(address(a) + 4) & 1) ? -1 : 1;
  if (track) {
    gabi::call<void>(0x0200ED84, gabi::at<void>(address(a) + 0x8F4), radius,
                     load<f32>(0x1002F1B0), hundred);
    store<f32>(address(a) + 0x8F8,
               load<f32>(0x1047BCD4) + load<f32>(0x1002F29C));
    s16 rate = load<s16>(0x1047BD48);
    store<f32>(address(a) + 0x8DC, x);
    store<f32>(address(a) + 0x8E0, height);
    store<f32>(address(a) + 0x8E4, z);
    store<s16>(address(a) + 0x8FE, (s32(rate) + 0x150) * sign);
    gabi::call<void>(0x02587128, gabi::at<void>(address(a) + 0x8DC));
    SearchLocal<cXyz> offset, target, diff;
    if (load<s32>(0x1046BE18)) {
      offset->x = load<f32>(0x1046BDF0);
      offset->y = load<f32>(0x1046BDF4);
      offset->z = load<f32>(0x1046BDF8);
    } else {
      offset->x = zero;
      store<f32>(0x1046BDF0, zero);
      offset->y = hundred;
      store<f32>(0x1046BDF4, hundred);
      store<u32>(0x1046BE18, 1);
      offset->z = zero;
      store<f32>(0x1046BDF8, zero);
    }
    gabi::call<void>(0x0201AD78, offset.get(), target.get(),
                     gabi::at<void>(address(a) + 0x8E8));
    gabi::call<void>(0x0201ADE0, target.get(), diff.get(),
                     gabi::at<void>(address(a) + 0x740));
    store<s16>(address(a) + 0x8D0, load<s16>(address(a) + 0x824));
    f32 dx = diff->x;
    offset->x = dx;
    offset->y = diff->y;
    f32 dz = diff->z;
    offset->z = dz;
    s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
    dz = offset->z;
    s16 base = load<s16>(address(a) + 0x322);
    dx = offset->x;
    s16 desired = s16(yaw - base);
    f32 len = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
    s32 pitch = gabi::call<s32>(0x020195B0, f32(offset->y), len);
    gabi::call<void>(0x0200F428, gabi::at<void>(address(a) + 0x824), desired,
                     10, 0x100);
    gabi::call<void>(0x0200F428, gabi::at<void>(address(a) + 0x822), pitch, 10,
                     0x100);
    store<s16>(address(a) + 0x82A, load<s16>(address(a) + 0x824));
    store<s16>(address(a) + 0x828, -load<s16>(address(a) + 0x822));
    return;
  }
  if (!load<s32>(0x1046BE1C)) {
    store<u32>(0x1046BE1C, 1);
    store<f32>(0x1046BDFC, zero);
    store<f32>(0x1046BE00, hundred);
    store<f32>(0x1046BE04, zero);
  }
  s16 target = s16(
      load<s16>(load<s32>(address(a) + 0xA20) == 1 ? 0x1047BB16 : 0x1047BB18) +
      (load<s32>(address(a) + 0xA20) == 1 ? 0x3400 : 0x2600));
  if (!gabi::call<s32>(0x0211D2F8, gabi::at<void>(address(a) + 0xA10))) {
    f32 base = f32(s32(load<s16>(0x1047BB0C)) + 60);
    f32 random = gabi::call<f32>(0x020198D8,
                                 load<f32>(0x1047BAA4) + load<f32>(0x1002F194));
    s32 phase = load<s32>(address(a) + 0xA20);
    store<s32>(address(a) + 0xA10, gabi::ftoi(base + random));
    store<s32>(address(a) + 0xA20, phase == 1 ? -1 : 1);
  }
  store<s16>(address(a) + 0x8D0, load<s16>(address(a) + 0x824));
  gabi::call<void>(0x0200F428, gabi::at<void>(address(a) + 0x822), target, 20,
                   0x20);
  store<s16>(address(a) + 0x828, -load<s16>(address(a) + 0x822));
}
VERIFY(0x0238D444, modeSearchBdk);
static void initialize_beam_targets(Act_c *a) {
  for (u32 i = 0; i < 3; i++) {
    store<u32>(address(a) + 0x758 + i * 4,
               load<u32>(address(a) + 0x728 + i * 4));
    store<u32>(address(a) + 0x764 + i * 4,
               load<u32>(address(a) + 0x734 + i * 4));
    store<u32>(address(a) + 0x89C + i * 4,
               load<u32>(address(a) + 0x314 + i * 4));
  }
  store<s16>(address(a) + 0x8A8, 235);
  store<s16>(address(a) + 0x8AA, 125);
  store<s16>(address(a) + 0x8AC, 0);
}
s32 actor_create(Act_c *a) {
  WWHD_FUNC(0x0238AAA0, s32, a);
  u32 condition = load<u32>(address(a) + 0x2E4);
  if (!(condition & 8)) {
    if (a) {
      Act_ctor(a);
      condition = load<u32>(address(a) + 0x2E4);
    }
    store<u32>(address(a) + 0x2E4, condition | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, gabi::at<void>(address(a) + 0x974),
                              STR(0x1002F2AC));
  if (phase != 4)
    return phase;
  SetArgData(a);
  if (!gabi::call<s32>(0x025D63E8, a, gabi::at<void>(0x02389188), 0x4620))
    return 5;
  CreateInit(a);
  u32 x = load<u32>(address(a) + 0x314), z = load<u32>(address(a) + 0x31C);
  store<u32>(address(a) + 0x74C, x);
  u32 y = load<u32>(address(a) + 0x318);
  store<u32>(address(a) + 0x748, z);
  store<u32>(address(a) + 0x750, y);
  u8 control = load<u8>(address(a) + 0x9F0);
  store<u32>(address(a) + 0x754, z);
  store<u32>(address(a) + 0x744, y);
  store<u32>(address(a) + 0x740, x);
  if (control == 6) {
    modeProc(a, 0, 7);
    initialize_beam_targets(a);
  } else if (control == 5) {
    store<u8>(address(a) + 0x980, 1);
    modeProc(a, 0, 2);
    initialize_beam_targets(a);
  } else {
    bool path = is_path_info(a);
    f32 one = load<f32>(0x1002F008);
    SearchLocal<cXyz> unit, result;
    unit->x = one;
    unit->y = one;
    unit->z = one;
    if (path) {
      store<u32>(address(a) + 0x730, load<u32>(address(a) + 0x960));
      store<u32>(address(a) + 0x728, load<u32>(address(a) + 0x958));
      store<u32>(address(a) + 0x72C, load<u32>(address(a) + 0x95C));
      gabi::call<void>(0x0201AD78, gabi::at<void>(address(a) + 0x74C),
                       result.get(), unit.get());
      vec_copy(gabi::at<cXyz>(address(a) + 0x734), result.get());
      s8 room = load<s8>(0x1047E6C8);
      u32 save = load<u32>(0x101F84DC);
      u8 sw = load<u8>(address(a) + 0x902);
      if (gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), sw, room)) {
        a->mBkControl = 0;
        modeProc(a, 0, 2);
        initialize_beam_targets(a);
      } else {
        a->mBkControl = 1;
        modeProc(a, 0, 1);
        set_mtx_light_A(a);
        set_mtx_light_B(a);
        SearchLocal<cXyz> diff;
        gabi::call<void>(0x0201ADE0, gabi::at<void>(address(a) + 0x728),
                         diff.get(), gabi::at<void>(address(a) + 0x740));
        f32 dx = diff->x, dz = diff->z, dy = diff->y;
        s32 yaw = gabi::call<s32>(0x020195B0, dx, dz);
        s16 base = load<s16>(address(a) + 0x322);
        yaw -= base;
        f32 len = gabi::call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
        s32 pitch = gabi::call<s32>(0x020195B0, dy, len);
        store<s16>(address(a) + 0x822, pitch);
        store<s16>(address(a) + 0x824, yaw);
        initialize_beam_targets(a);
        store<s16>(address(a) + 0x82A, yaw);
      }
    } else {
      gabi::call<void>(0x0201AD78, gabi::at<void>(address(a) + 0x740),
                       result.get(), unit.get());
      vec_copy(gabi::at<cXyz>(address(a) + 0x728), result.get());
      gabi::call<void>(0x0201AD78, gabi::at<void>(address(a) + 0x74C),
                       result.get(), unit.get());
      vec_copy(gabi::at<cXyz>(address(a) + 0x734), result.get());
      modeProc(a, 0, 2);
      initialize_beam_targets(a);
    }
  }
  u8 kind = load<u8>(address(a) + 0x902);
  s32 mode = initialMode(a, kind);
  f32 width = load<f32>(0x101CCA94 + u32(mode) * 4),
      zero = load<f32>(0x1002F010);
  store<f32>(address(a) + 0x8B0, width);
  store<f32>(address(a) + 0x8B4, zero);
  kind = load<u8>(address(a) + 0x902);
  mode = initialMode(a, kind);
  control = load<u8>(address(a) + 0x9F0);
  store<f32>(address(a) + 0x8BC, load<f32>(0x101CCAA8 + u32(mode) * 4));
  bool visible = false;
  if (control != 5) {
    u32 save = load<u32>(0x101F84DC);
    f32 time = load<f32>(save + 0x44);
    visible =
        !(time < load<f32>(0x1002F190)) || !(time > load<f32>(0x1002F194));
  }
  store<s16>(address(a) + 0x89A, visible ? 255 : 0);
  store<u8>(address(a) + 0x94D, 125);
  store<u8>(address(a) + 0x94C, 235);
  gabi::call<s32>(0x0238A7A4, a);
  gabi::call<s32>(0x0238A7A4, a);
  return phase;
}
VERIFY(0x0238AAA0, actor_create);
