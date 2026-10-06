/* Retained HD environment data APIs. Tables live in the reference image;
 * this unit verifies actual accessor/copy code, not fabricated data functions.
 * Neighboring02563940 has a separate registration cluster and remains
 * unattributed/excluded; the admitted executable scope has9 entries.
 */
#include "gabi.h"
using namespace gabi;

void dKyd_xfog_table_set_hd(u32 index) {
  WWHD_FUNC(0x025638D8, void, index);
  u32 table = 0x101E8F50 + index * 20u;
  for (u32 offset = 0; offset < 20; offset += 2) {
    // Native lazy environment accessor, called once for every copied entry.
    u32 environment = call<u32>(0x02555D0C);
    u16 value = load<u16>(table + offset);
    store<u16>(environment + offset + 0xB50, value);
  }
}
VERIFY(0x025638D8, dKyd_xfog_table_set_hd);

void *dKyd_dmpalet_getp_hd() {
  WWHD_FUNC(0x025639D4, void *);
  return at<void>(0x101E9104);
}
VERIFY(0x025639D4, dKyd_dmpalet_getp_hd);
void *dKyd_dmpselect_getp_hd() {
  WWHD_FUNC(0x025639E0, void *);
  return at<void>(0x101E8F28);
}
VERIFY(0x025639E0, dKyd_dmpselect_getp_hd);
void *dKyd_dmenvr_getp_hd() {
  WWHD_FUNC(0x025639EC, void *);
  return at<void>(0x101E8F40);
}
VERIFY(0x025639EC, dKyd_dmenvr_getp_hd);
void *dKyd_dmvrbox_getp_hd() {
  WWHD_FUNC(0x025639F8, void *);
  return at<void>(0x101E97C4);
}
VERIFY(0x025639F8, dKyd_dmvrbox_getp_hd);
void *dKyd_schejule_getp_hd() {
  WWHD_FUNC(0x02563A04, void *);
  return at<void>(0x101E8F78);
}
VERIFY(0x02563A04, dKyd_schejule_getp_hd);
void *dKyd_schejule_boss_getp_hd() {
  WWHD_FUNC(0x02563A10, void *);
  return at<void>(0x101E8FFC);
}
VERIFY(0x02563A10, dKyd_schejule_boss_getp_hd);
void *dKyd_schejule_menu_getp_hd() {
  WWHD_FUNC(0x02563A1C, void *);
  return at<void>(0x101E9080);
}
VERIFY(0x02563A1C, dKyd_schejule_menu_getp_hd);

void d_kankyo_data_sinit_hd() {
  WWHD_FUNC(0x02563A28, void);
  store<u32>(0x104773EC, 0);
  store<u32>(0x104773E4, 0);
  store<u32>(0x104773F0, 0);
  store<u32>(0x104773E8, 0);
  call<void>(0x028F026C, at<void>(0x101E99A8));
  f32 a = load<f32>(0x1004F368), b = load<f32>(0x1004F36C);
  store<f32>(0x104773D8, a);
  store<f32>(0x104773DC, b);
  call<void>(0x028ED6F8, at<void>(0x104773E0));
  call<void>(0x028F026C, at<void>(0x101E99B4));
  call<void>(0x028EAB2C, at<void>(0x104773E1));
  call<void>(0x028F026C, at<void>(0x101E99C0));
}
VERIFY(0x02563A28, d_kankyo_data_sinit_hd);

/* 02563940 __sinit: header-statics initializer of the TU that ends with dKyd_xfog_table_set 025638D8 (the data accessors 025639D4.. and their own __sinit 02563A28 follow). The header statics block at
 * 104773BC (zeroed 16-byte object at +0xC, {-pi, pi} from 1004F35C, two one-byte objects at +8/+9),
 * each registered for destruction (records 101E9984, +0xC, +0x18). */
static void __sinit_d_kankyo_data_xfog_tu() {
    WWHD_FUNC(0x02563940, void);
    const u32 bss = 0x104773BC, rec = 0x101E9984, ro = 0x1004F35C;
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
VERIFY(0x02563940, __sinit_d_kankyo_data_xfog_tu);
