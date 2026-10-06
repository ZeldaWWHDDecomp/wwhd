/* HD demo point lights, 02563ABC..02563D44 (entry/delete/execute/init).
 * HD adds an initial update delay,
 * brightness smoothing and a lazy environment accessor before each operation.
 */
#include "gabi.h"
using namespace gabi;

// Native 02555D0C is the lazy environment accessor; the matched map name is
// wrong.
static void environment_access() { call<void>(0x02555D0C); }

void dKydm_demo_plight_entry_hd(void *light, void *pos, s32 type, u32 group,
                                u32 delay) {
  WWHD_FUNC(0x02563ABC, void, light, pos, type, group, delay);
  environment_access();
  if (type < 0)
    return;
  u32 a = ea(light), p = ea(pos);
  store<u32>(a + 8, load<u32>(p));
  store<u32>(a + 12, load<u32>(p + 4));
  u32 combined = (u32)type + group * 100u;
  store<u32>(a + 4, combined);
  store<u32>(a + 16, load<u32>(p + 8));
  store<f32>(a + 0x1c, load<f32>(0x1004F370));
  if ((s32)combined < 100) {
    store<u16>(a + 0x18, 128);
    store<u16>(a + 0x14, 255);
    store<f32>(a + 0x2c, load<f32>(0x1004F378));
    store<u16>(a + 0x16, 255);
    store<f32>(a + 0x20, load<f32>(0x1004F374));
    call<void>(0x025564B4, at<void>(a + 8));
    store<u32>(a + 0x30, delay);
    store<u32>(a, 0);
    store<s32>(a + 4, type);
  } else {
    store<u16>(a + 0x14, 255);
    store<u16>(a + 0x18, 120);
    store<u16>(a + 0x16, 255);
    if (combined != 103) {
      store<f32>(a + 0x20, load<f32>(0x1004F37C));
      store<f32>(a + 0x2c, load<f32>(0x1004F380));
    } else {
      store<f32>(a + 0x20, load<f32>(0x1004F384));
      store<f32>(a + 0x2c, load<f32>(0x1004F388));
    }
    call<void>(0x0255B9C8, at<void>(a + 8));
    store<u32>(a + 0x30, delay);
    store<s32>(a + 4, type);
    store<u32>(a, 0);
  }
}
VERIFY(0x02563ABC, dKydm_demo_plight_entry_hd);

void dKydm_demo_plight_delete_hd(void *light) {
  WWHD_FUNC(0x02563BF0, void, light);
  environment_access();
  if (!light)
    return;
  u32 a = ea(light);
  if (load<s32>(a + 4) < 100)
    call<void>(0x0255A374, at<void>(a + 8));
  else
    call<void>(0x0255BA9C, at<void>(a + 8));
}
VERIFY(0x02563BF0, dKydm_demo_plight_delete_hd);

void dKydm_demo_plight_execute_hd(void *light, void *pos) {
  WWHD_FUNC(0x02563C50, void, light, pos);
  environment_access();
  if (!light)
    return;
  u32 a = ea(light), p = ea(pos), delay = load<u32>(a + 0x30);
  if (delay) {
    store<u32>(a + 0x30, delay - 1);
    return;
  }
  store<u32>(a + 8, load<u32>(p));
  f32 fluctuation = load<f32>(a + 0x2c);
  store<u32>(a + 12, load<u32>(p + 4));
  f32 scale = load<f32>(0x1004F38C);
  store<u32>(a + 16, load<u32>(p + 8));
  f32 random = call<f32>(0x020198D8, fmuls_ppc(fluctuation, scale));
  call<void>(0x0200ECD4, at<void>(a + 0x1c), fadds_ppc(fluctuation, random),
             scale, load<f32>(0x1004F390), scale);
}
VERIFY(0x02563C50, dKydm_demo_plight_execute_hd);

void d_kankyo_demo_sinit_hd() {
  WWHD_FUNC(0x02563D44, void);
  store<u32>(0x10477408, 0);
  store<u32>(0x10477400, 0);
  store<u32>(0x1047740C, 0);
  store<u32>(0x10477404, 0);
  call<void>(0x028F026C, at<void>(0x101E99CC));
  f32 a = load<f32>(0x1004F398), b = load<f32>(0x1004F39C);
  store<f32>(0x104773F4, a);
  store<f32>(0x104773F8, b);
  call<void>(0x028ED6F8, at<void>(0x104773FC));
  call<void>(0x028F026C, at<void>(0x101E99D8));
  call<void>(0x028EAB2C, at<void>(0x104773FD));
  call<void>(0x028F026C, at<void>(0x101E99E4));
}
VERIFY(0x02563D44, d_kankyo_demo_sinit_hd);
