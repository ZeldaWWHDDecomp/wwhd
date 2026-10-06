// Light-activated moon/sun switch. WWHD verified source.
#include "d/actor/d_a_obj_swlight.h"
using daObjSwlight::Act_c;
static BOOL solidHeapCB(Act_c *a) {
  WWHD_FUNC(0x02395560, BOOL, a);
  return gabi::call<BOOL>(0x02395298, a);
}
VERIFY(0x02395560, solidHeapCB);
static void sound(Act_c *a, u32 id) {
  s32 rev = gabi::call<s32>(0x02520540, (s8)a->current.roomNo);
  gabi::call(0x025E1A40, id, &a->eyePos, 0, rev);
}
static s32 prm(Act_c *a, s32 width, s32 shift) {
  return gabi::call<s32>(0x023969A0, a, width, shift);
}
static bool sw(Act_c *a) {
  s32 n = prm(a, 8, 0);
  u32 save = gabi::load<u32>(0x101F84DC);
  s8 room = a->home.roomNo;
  return gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), n, room) != 0;
}
static void switch_write(Act_c *a, bool on) {
  s32 n = prm(a, 8, 0);
  u32 save = gabi::load<u32>(0x101F84DC);
  s8 room = a->home.roomNo;
  gabi::call(on ? 0x025B9E38 : 0x025B9F7C, gabi::at<void>(save + 0x20), n,
             room);
}

static void set_mtx(Act_c *a) {
  WWHD_FUNC(0x02395564, void, a);
  gabi::call(0x028E93CC, mDoMtx_stack_c::get(), (f32)a->current.pos.x,
             (f32)a->current.pos.y, (f32)a->current.pos.z);
  gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s16)a->shape_angle.x,
             (s16)a->shape_angle.y, (s16)a->shape_angle.z);
  Mtx34 *m = gabi::at<Mtx34>(gabi::ea((J3DModel *)a->mModel) + 0xC8);
  mtx_copy(m, mDoMtx_stack_c::get());
}
VERIFY(0x02395564, set_mtx);
static void init_mtx(Act_c *a) {
  WWHD_FUNC(0x02395628, void, a);
  u32 m = gabi::ea((J3DModel *)a->mModel);
  f32 y = a->scale.y, x = a->scale.x, z = a->scale.z;
  gabi::store<f32>(m + 0xBC, x);
  gabi::store<f32>(m + 0xC0, y);
  gabi::store<f32>(m + 0xC4, z);
  gabi::call(0x02395564, a);
}
VERIFY(0x02395628, init_mtx);
static void init_cc(Act_c *a) {
  WWHD_FUNC(0x02395758, void, a);
  for (int i = 0; i < 8; i++) {
    gabi::call(0x02515F14, &a->mStts[i], 0xFF, 0xFF, a);
    gabi::call(0x0251650C, &a->mTri[i], gabi::at<void>(0x1002FF10));
    u32 t = gabi::ea(&a->mTri[i]);
    u32 flag = gabi::load<u32>(t + 0x94);
    gabi::store<u32>(t + 0x44, gabi::ea(&a->mStts[i]));
    gabi::store<u32>(t + 0x94, flag | 4);
  }
  gabi::call(0x02395648, a);
}
VERIFY(0x02395758, init_cc);
static void init_eye_pos(Act_c *a) {
  WWHD_FUNC(0x023957D8, void, a);
  if (gabi::load<u32>(0x1046C04C) == 0) {
    gabi::store<f32>(0x1046C040, 0.f);
    gabi::store<f32>(0x1046C048, 40.f);
    gabi::store<f32>(0x1046C044, 0.f);
    gabi::store<u32>(0x1046C04C, 1);
  }
  gabi::call(0x028E8F64, J3DModel_getBaseTRMtx(a->mModel),
             gabi::at<cXyz>(0x1046C040), &a->eyePos);
}
VERIFY(0x023957D8, init_eye_pos);
static void mode_norm_sun_init(Act_c *a) {
  WWHD_FUNC(0x02395830, void, a);
  a->mMode = 1;
}
VERIFY(0x02395830, mode_norm_sun_init);
static void mode_norm_moon_init(Act_c *a) {
  WWHD_FUNC(0x0239583C, void, a);
  a->mMode = 0;
  a->mTimer = 20;
}
VERIFY(0x0239583C, mode_norm_moon_init);
static bool is_switch2(Act_c *a) {
  WWHD_FUNC(0x02395850, bool, a);
  s32 n = prm(a, 8, 8);
  if (n == 0xFF)
    return false;
  u32 save = gabi::load<u32>(0x101F84DC);
  s8 room = a->home.roomNo;
  return gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), n, room) != 0;
}
VERIFY(0x02395850, is_switch2);
static void mode_active_sun_init(Act_c *a) {
  WWHD_FUNC(0x023958C8, void, a);
  a->mMode = 3;
}
VERIFY(0x023958C8, mode_active_sun_init);
static void mode_active_moon_init(Act_c *a) {
  WWHD_FUNC(0x023958D4, void, a);
  a->mMode = 2;
  a->mTimer = 20;
}
VERIFY(0x023958D4, mode_active_moon_init);
static void set_cc(Act_c *a) {
  WWHD_FUNC(0x0239620C, void, a);
  for (int i = 0; i < 8; i++) {
    u32 play = dComIfGp_ea();
    gabi::call(0x0200E240, gabi::at<void>(play + 0x26A4), &a->mTri[i]);
  }
}
VERIFY(0x0239620C, set_cc);
static bool chk_light(Act_c *a) {
  WWHD_FUNC(0x0239625C, bool, a);
  u32 play = dComIfGp_ea();
  if (gabi::call<s32>(0x0252A038, gabi::at<void>(play + 0x5A20), &a->eyePos))
    return true;
  bool hit = false;
  for (int i = 0; i < 8; i++) {
    if (gabi::call<s32>(0x025162A4, &a->mTri[i])) {
      gabi::call(0x0251621C, &a->mTri[i]);
      hit = true;
    }
  }
  return hit;
}
VERIFY(0x0239625C, chk_light);
static bool power_up(Act_c *a) {
  WWHD_FUNC(0x023962F8, bool, a);
  return gabi::call<s32>(0x0200F5C8, &a->mPower, 1.f, 0.033333335f) != 0;
}
VERIFY(0x023962F8, power_up);
static bool power_down(Act_c *a) {
  WWHD_FUNC(0x02396334, bool, a);
  return gabi::call<s32>(0x0200F5C8, &a->mPower, 0.f, 0.033333335f) != 0;
}
VERIFY(0x02396334, power_down);
static void mode_norm_moon(Act_c *a) {
  WWHD_FUNC(0x02396370, void, a);
  bool lit = gabi::call<bool>(0x0239625C, a);
  if (lit) {
    if (a->mTimer > 0)
      a->mTimer = (s16)a->mTimer - 1;
  } else
    a->mTimer = 20;
  if (sw(a) || a->mTimer == 0) {
    if (gabi::call<bool>(0x023962F8, a)) {
      sound(a, 0x6962);
      switch_write(a, true);
      gabi::call(0x02395830, a);
      return;
    }
  } else
    gabi::call(0x02396334, a);
  gabi::call(0x0239620C, a);
}
VERIFY(0x02396370, mode_norm_moon);
static void mode_norm_sun(Act_c *a) {
  WWHD_FUNC(0x023964D4, void, a);
  if (sw(a)) {
    gabi::call(0x023962F8, a);
    sound(a, 0x7029);
  } else if (gabi::call<bool>(0x02396334, a))
    gabi::call(0x0239583C, a);
}
VERIFY(0x023964D4, mode_norm_sun);
static void mode_active_moon(Act_c *a) {
  WWHD_FUNC(0x0239657C, void, a);
  bool light = gabi::call<bool>(0x0239625C, a);
  s16 timer;
  if (light) {
    timer = a->mTimer;
    if (timer > 0) {
      timer--;
      a->mTimer = timer;
    }
  } else {
    a->mTimer = 20;
    timer = 20;
  }
  if (timer == 0 && gabi::call<bool>(0x023962F8, a)) {
    sound(a, 0x6962);
    switch_write(a, true);
    gabi::call(0x023958C8, a);
  } else if (timer != 0)
    gabi::call(0x02396334, a);
  gabi::call(0x0239620C, a);
}
VERIFY(0x0239657C, mode_active_moon);
static void mode_active_sun(Act_c *a) {
  WWHD_FUNC(0x0239666C, void, a);
  if ((f32)a->mPower > 0.5f)
    sound(a, 0x7029);
  if (gabi::call<bool>(0x02395850, a) || gabi::call<bool>(0x0239625C, a))
    gabi::call(0x023962F8, a);
  else if (gabi::call<bool>(0x02396334, a)) {
    switch_write(a, false);
    gabi::call(0x023958D4, a);
  }
  gabi::call(0x0239620C, a);
}
VERIFY(0x0239666C, mode_active_sun);
static void set_cc_pos(Act_c *a) {
  WWHD_FUNC(0x02395648, void, a);
  gabi::Local<cXyz> p0, p1, p2, o0, o1, o2;
  f32 zero = gabi::load<f32>(0x1002FEC4);
  p0->set(zero, zero, gabi::load<f32>(0x1002FEC8));
  f32 size = gabi::load<f32>(0x1002FECC);
  f32 x = size * gabi::load<f32>(0x104A64F8);
  f32 y = size * gabi::load<f32>(0x104A64FC);
  p1->set(zero, size, zero);
  p2->set(x, y, zero);
  for (int i = 0; i < 8; i++) {
    gabi::call(0x028E90D4, J3DModel_getBaseTRMtx(a->mModel),
               mDoMtx_stack_c::get());
    gabi::call(0x025F1C5C, mDoMtx_stack_c::get(), (s16)(i * 8192));
    gabi::call(0x028E8F64, mDoMtx_stack_c::get(), p0.get(), o0.get());
    gabi::call(0x028E8F64, mDoMtx_stack_c::get(), p1.get(), o1.get());
    gabi::call(0x028E8F64, mDoMtx_stack_c::get(), p2.get(), o2.get());
    gabi::call(0x0201924C, gabi::at<void>(gabi::ea(&a->mTri[i]) + 0x118),
               o0.get(), o1.get(), o2.get());
  }
}
VERIFY(0x02395648, set_cc_pos);
static bool create_heap(Act_c *a) {
  WWHD_FUNC(0x02395298, bool, a);
  J3DModelData *mdl =
      (J3DModelData *)dComIfG_getObjectRes(STR(0x1002FF08), 10, 0x1002FE18);
  if (!mdl)
    gabi::call(0x0273AA24, STR(0x1002FE78), 0x13D, STR(0x1002FE9C));
  a->mModel = mDoExt_J3DModel__create(mdl, 0x80000, 0x31000202);
  if (a->mModel) {
    for (u32 i = 1; i <= 5; i++) {
      u32 base = gabi::ea(mdl);
      u32 count = gabi::load<u32>(base + 4);
      u32 joints = gabi::load<u32>(base + 8);
      if (count > i)
        joints += i * 0x1C;
      gabi::store<u32>(joints + 8, 0x02395290);
    }
    gabi::store<u32>(gabi::ea((J3DModel *)a->mModel) + 0xB8, gabi::ea(a));
  }
  void *btk = dComIfG_getObjectRes(STR(0x1002FF08), 15, 0x1002FE18);
  if (!btk)
    gabi::call(0x0273AA24, STR(0x1002FE78), 0x158, STR(0x1002FEAC));
  s32 okbtk =
      gabi::call<s32>(0x025E7CE0, a->mBtk, mdl, btk, 1, 2, 1.f, 0, -1, 0, 0);
  void *bck = dComIfG_getObjectRes(STR(0x1002FF08), 6, 0x1002FE18);
  if (!bck)
    gabi::call(0x0273AA24, STR(0x1002FE78), 0x15F, STR(0x1002FEB8));
  s32 okbck =
      gabi::call<s32>(0x025E8508, a->mBck, mdl, bck, 1, 2, 1.f, 0, -1, 0);
  bool okbg = false;
  a->mBg = gabi::call<dBgW *>(0x024F23F4, 0);
  if (a->mBg) {
    void *bgdata = dComIfG_getObjectRes(STR(0x1002FF08), 20, 0x1002FE18);
    if (!bgdata)
      gabi::call(0x0273AA24, STR(0x1002FE78), 0x169, STR(0x1002FE8C));
    okbg =
        gabi::call<s32>(0x0200A030, (dBgW *)a->mBg, bgdata, 1, &a->mBgMtx) == 0;
  }
  bool ok = a->mModel != nullptr && okbtk && okbck && okbg;
  if (!ok)
    a->mBg = nullptr;
  return ok;
}
VERIFY(0x02395298, create_heap);
static bool actor_delete(Act_c *a) {
  WWHD_FUNC(0x02395C40, bool, a);
  dBgW *bg = a->mBg;
  if (bg && gabi::load<u32>(gabi::ea(bg)) < 0x100) {
    u32 play = dComIfGp_ea();
    gabi::call(0x020087EC, gabi::at<void>(play + 0x12A0), (dBgW *)a->mBg);
  }
  gabi::call(0x025204C8, &a->mPhase, STR(0x1002FF08));
  return true;
}
VERIFY(0x02395C40, actor_delete);
static bool actor_execute(Act_c *a) {
  WWHD_FUNC(0x02395CA4, bool, a);
  u32 ent = 0x1002FED4 + (u32)a->mMode * 8;
  s16 index = gabi::load<s16>(ent + 2), delta = gabi::load<s16>(ent);
  u32 self = gabi::ea(a) + (s32)delta;
  u32 target;
  if (index < 0)
    target = gabi::load<u32>(ent + 4);
  else {
    s16 off = gabi::load<s16>(ent + 6);
    u32 vt = gabi::load<u32>(self + (s32)off);
    target = gabi::load<u32>(vt + (s32)index * 8 + 4);
  }
  gabi::call_ptr<void>(target, gabi::at<void>(self));
  gabi::call(0x025E742C, a->mBtk);
  gabi::call(0x025E742C, a->mBck);
  gabi::call(0x02395564, a);
  return true;
}
VERIFY(0x02395CA4, actor_execute);
static s32 actor_create(Act_c *a) {
  WWHD_FUNC(0x023958E8, s32, a);
  u32 b = gabi::ea(a), condition = gabi::load<u32>(b + 0x2E4);
  if (!(condition & 8)) {
    if (a) {
      fopAc_ac_c_ct(a);
      gabi::store<u32>(b + 0xB4, 0x1002FE68);
      gabi::call(0x025E7C6C, a->mBtk);
      gabi::call(0x027F2BC0, a->mBck, 0);
      gabi::store<u32>(b + 0x43C, 0x1016E54C);
      gabi::call(0x027DA984, gabi::at<void>(b + 0x440));
      gabi::store<u32>(b + 0x484, 0);
      gabi::store<u32>(b + 0x4B4, 0);
      gabi::store<u32>(b + 0x4B0, 0);
      gabi::store<u32>(b + 0x4AC, 0);
      gabi::store<u32>(b + 0x474, 0x1016D820);
      gabi::store<u32>(b + 0x4A8, 0);
      gabi::store<u32>(b + 0x43C, 0x1002FE40);
      gabi::call(0x028EFFD0, a->mTri, 8, 0x150, 0x02396868);
      gabi::call(0x028EFFD0, a->mStts, 8, 0x3C, 0x02396800);
      condition = gabi::load<u32>(b + 0x2E4);
    }
    gabi::store<u32>(b + 0x2E4, condition | 8);
  }
  a->mType = prm(a, 1, 16);
  s32 phase = gabi::call<s32>(0x02520460, &a->mPhase, STR(0x1002FF08));
  if (phase == 4) {
    gabi::call(0x028E93CC, mDoMtx_stack_c::get(), (f32)a->current.pos.x,
               (f32)a->current.pos.y, (f32)a->current.pos.z);
    gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s16)a->shape_angle.x,
               (s16)a->shape_angle.y, (s16)a->shape_angle.z);
    gabi::call(0x025F2518, (f32)a->scale.x, (f32)a->scale.y, (f32)a->scale.z);
    gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &a->mBgMtx);
    if (!gabi::call<BOOL>(0x025D63E8, a, 0x02395560, 0x1F40))
      return 5;
    u32 play = dComIfGp_ea();
    gabi::call(0x024EEA6C, gabi::at<void>(play + 0x12A0), (dBgW *)a->mBg, a);
    gabi::store<u32>(gabi::ea((dBgW *)a->mBg) + 0xA8, 0);
    gabi::store<u32>(b + 0x348, gabi::ea(J3DModel_getBaseTRMtx(a->mModel)));
    gabi::call(0x02395628, a);
    gabi::call(0x025D6768, a, 0.f, 0.f, 0.f, 240.f);
    gabi::call(0x02395758, a);
    gabi::call(0x023957D8, a);
    if (a->mType == 0) {
      if (sw(a)) {
        a->mPower = 1.f;
        gabi::call(0x02395830, a);
      } else {
        a->mPower = 0.f;
        gabi::call(0x0239583C, a);
      }
    } else {
      bool active = false;
      if (gabi::call<bool>(0x02395850, a))
        active = sw(a);
      if (active) {
        a->mPower = 1.f;
        gabi::call(0x023958C8, a);
      } else {
        if (sw(a))
          switch_write(a, false);
        a->mPower = 0.f;
        gabi::call(0x023958D4, a);
      }
    }
  }
  return phase;
}
VERIFY(0x023958E8, actor_create);
static void set_material_color(u32 mat, u32 alpha, bool opaque) {
  u32 shape = gabi::load<u32>(mat + 8);
  gabi::store<u8>(shape + 4, 1);
  u32 data = gabi::load<u32>(mat), rel = data + 0x20;
  u32 offset = gabi::load<u32>(rel);
  gabi::call(0x027E212C, gabi::at<void>(offset ? rel + offset : 0),
             opaque ? 1 : 3);
  u32 block = gabi::load<u32>(mat + 0x18);
  u32 vt = gabi::load<u32>(block + 4);
  u32 color = gabi::ea(gabi::call_ptr<void *>(gabi::load<u32>(vt + 0x4C),
                                              gabi::at<void>(block), 3));
  gabi::store<u8>(color + 3, (u8)alpha);
  block = gabi::load<u32>(mat + 0x18);
  vt = gabi::load<u32>(block + 4);
  color = gabi::ea(gabi::call_ptr<void *>(gabi::load<u32>(vt + 0x4C),
                                          gabi::at<void>(block), 3));
  block = gabi::load<u32>(mat + 0x18);
  vt = gabi::load<u32>(block + 4);
  gabi::call_ptr<void>(gabi::load<u32>(vt + 0x3C), gabi::at<void>(block), 3,
                       gabi::at<void>(color));
  gabi::Local<be<f32>[4]> rgba, linear;
  gabi::call(0x023951DC, rgba.get(), gabi::at<void>(color));
  gabi::call(0x0274D458, linear.get(), rgba.get(), 1.f);
  u32 flags = gabi::load<u32>(mat + 0xA0);
  gabi::store<u32>(mat + 0xA0, flags | 0x400);
  u32 param =
      gabi::ea(gabi::call<void *>(0x027F9F0C, gabi::at<void>(mat + 0xA0), 10));
  f32 a = (f32)gabi::load<u8>(color + 3) / 255.f;
  f32 z = (*linear.get())[2], y = (*linear.get())[1], x = (*linear.get())[0];
  gabi::store<f32>(param + 4, y);
  gabi::store<f32>(param + 8, z);
  gabi::store<f32>(param, x);
  gabi::store<f32>(param + 0xC, a);
}
static void setMaterial(void *material, u32 alpha) {
  WWHD_FUNC(0x02395D54, void, material, alpha);
  u32 mat = gabi::ea(material);
  while (mat) {
    if (alpha == 0) {
      u32 shape = gabi::load<u32>(mat + 8);
      gabi::store<u8>(shape + 4, 0);
    } else
      set_material_color(mat, alpha, alpha == 255);
    mat = gabi::load<u32>(mat + 4);
  }
}
VERIFY(0x02395D54, setMaterial);
static u32 joint_mesh(u32 modeldata, u32 index) {
  u32 count = gabi::load<u32>(modeldata + 4);
  u32 joints = gabi::load<u32>(modeldata + 8);
  if (index < count)
    joints += index * 0x1C;
  return gabi::load<u32>(joints + 0x10);
}
static bool actor_draw(Act_c *a) {
  WWHD_FUNC(0x02395ED4, bool, a);
  u32 env = gabi::ea(gabi::call<void *>(0x02555D0C));
  gabi::call(0x025626A4, gabi::at<void>(env), 0, &a->current.pos, &a->tevStr);
  env = gabi::ea(gabi::call<void *>(0x02555D0C));
  gabi::call(0x02562F5C, gabi::at<void>(env), (J3DModel *)a->mModel,
             &a->tevStr);
  J3DModel *model = a->mModel;
  f32 frame = gabi::load<f32>(gabi::ea(a) + 0x3BC);
  gabi::call(0x025E7FC4, a->mBtk, J3DModel_getModelData(model), frame);
  model = a->mModel;
  frame = gabi::load<f32>(gabi::ea(a) + 0x430);
  gabi::call(0x025E86B8, a->mBck, J3DModel_getModelData(model), frame);
  u32 play = dComIfGp_ea();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
  play = dComIfGp_ea();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  u8 alpha = (u8)gabi::ftoi(255.f * (f32)a->mPower);
  u32 data = gabi::ea(J3DModel_getModelData(a->mModel));
  for (u32 i = 1; i <= 3; i++) {
    u32 material = joint_mesh(data, i);
    gabi::call(0x02395D54, gabi::at<void>(material), (u32)alpha);
    gabi::call(0x027F591C, (J3DModel *)a->mModel, gabi::at<void>(material),
               alpha < 255 ? 1 : 0);
  }
  alpha = (u8)(255 - alpha);
  u32 material = joint_mesh(data, 4);
  gabi::call(0x02395D54, gabi::at<void>(material), (u32)alpha);
  gabi::call(0x027F591C, (J3DModel *)a->mModel, gabi::at<void>(material),
             alpha < 255 ? 1 : 0);
  material = joint_mesh(data, 5);
  u32 mat = material;
  if (alpha) {
    while (mat) {
      set_material_color(mat, alpha, false);
      mat = gabi::load<u32>(mat + 4);
    }
  } else {
    while (mat) {
      u32 shape = gabi::load<u32>(mat + 8);
      gabi::store<u8>(shape + 4, 0);
      mat = gabi::load<u32>(mat + 4);
    }
  }
  gabi::call(0x027F591C, (J3DModel *)a->mModel, gabi::at<void>(material), 1);
  gabi::call(0x025E2DE0, (J3DModel *)a->mModel, 0);
  play = dComIfGp_ea();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = dComIfGp_ea();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
  return true;
}
VERIFY(0x02395ED4, actor_draw);
static BOOL jnodeCB_moon(void *node, s32 stage) {
  WWHD_FUNC(0x02395290, BOOL, node, stage);
  return TRUE;
}
VERIFY(0x02395290, jnodeCB_moon);
static s32 Mthd_Create(Act_c *a) {
  WWHD_FUNC(0x02396748, s32, a);
  return gabi::call<s32>(0x023958E8, a);
}
VERIFY(0x02396748, Mthd_Create);
static BOOL Mthd_Delete(Act_c *a) {
  WWHD_FUNC(0x0239674C, BOOL, a);
  return gabi::call<BOOL>(0x02395C40, a);
}
VERIFY(0x0239674C, Mthd_Delete);
static BOOL Mthd_Execute(Act_c *a) {
  WWHD_FUNC(0x02396750, BOOL, a);
  return gabi::call<BOOL>(0x02395CA4, a);
}
VERIFY(0x02396750, Mthd_Execute);
static BOOL Mthd_Draw(Act_c *a) {
  WWHD_FUNC(0x02396754, BOOL, a);
  return gabi::call<BOOL>(0x02395ED4, a);
}
VERIFY(0x02396754, Mthd_Draw);
static void static_init() {
  WWHD_FUNC(0x02396758, void);
  gabi::store<u32>(0x1046C038, 0);
  gabi::store<u32>(0x1046C030, 0);
  gabi::store<u32>(0x1046C03C, 0);
  gabi::store<u32>(0x1046C034, 0);
  gabi::call(0x028F026C, gabi::at<void>(0x101CCF9C));
  gabi::store<f32>(0x1046C024, -3.1415927410125732f);
  gabi::store<f32>(0x1046C028, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<void>(0x1046C02C));
  gabi::call(0x028F026C, gabi::at<void>(0x101CCFA8));
  gabi::call(0x028EAB2C, gabi::at<void>(0x1046C02D));
  gabi::call(0x028F026C, gabi::at<void>(0x101CCFB4));
}
VERIFY(0x02396758, static_init);
static void trivial_dt(void *self, s32 flags) {
  WWHD_FUNC(0x023967EC, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x023967EC, trivial_dt);
static void *stts_ct(void *self) {
  WWHD_FUNC(0x02396800, void *, self);
  if (!self)
    self = gabi::call<void *>(0x0273AD10, 0x3C);
  if (self) {
    u32 b = gabi::ea(self);
    gabi::call(0x0200BD2C, self);
    gabi::call(0x02515DA0, gabi::at<void>(b + 0x1C));
    gabi::store<u32>(b + 0x18, 0x1004AE88);
    gabi::store<u32>(b + 0x1C, 0x1004AEC0);
  }
  return self;
}
VERIFY(0x02396800, stts_ct);
static void *tri_ct(void *self) {
  WWHD_FUNC(0x02396868, void *, self);
  if (!self)
    self = gabi::call<void *>(0x0273AD10, 0x150);
  if (self) {
    u32 b = gabi::ea(self);
    gabi::call(0x02515FB8, self);
    gabi::store<u32>(b + 0x114, 0x100015A8);
    gabi::store<u32>(b + 0x110, 0x1002FE30);
    gabi::call(0x02019040, gabi::at<void>(b + 0x118));
    gabi::store<u32>(b + 0x3C, 0x1004B010);
    gabi::store<u32>(b + 0x128, 0x1004B058);
    gabi::store<u32>(b + 0x114, 0x1004B068);
  }
  return self;
}
VERIFY(0x02396868, tri_ct);
static void empty_virtual(void *self) { WWHD_FUNC(0x023968F4, void, self); }
VERIFY(0x023968F4, empty_virtual);
static void actor_dt(Act_c *a, s32 flags) {
  WWHD_FUNC(0x023968F8, void, a, flags);
  if (a) {
    gabi::call(0x028F0164, a->mStts, 8, 0x3C, 0x02515860, 0, 0);
    gabi::call(0x028F0164, a->mTri, 8, 0x150, 0x025159F8, 0, 0);
    gabi::call(0x027F3628, gabi::at<void>(gabi::ea(a) + 0x43C), 0);
    gabi::call(0x025D50BC, a, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, a);
  }
}
VERIFY(0x023968F8, actor_dt);
static BOOL Mthd_IsDelete(void *a) {
  WWHD_FUNC(0x02396998, BOOL, a);
  return TRUE;
}
VERIFY(0x02396998, Mthd_IsDelete);
static u32 PrmAbstract(fopAc_ac_c *a, u32 width, u32 shift) {
  WWHD_FUNC(0x023969A0, u32, a, width, shift);
  u32 p = fopAcM_GetParam(a);
  u32 mask = (width & 32) ? 0 : (1u << (width & 31));
  u32 value = (shift & 32) ? 0 : (p >> (shift & 31));
  return value & (mask - 1);
}
VERIFY(0x023969A0, PrmAbstract);
