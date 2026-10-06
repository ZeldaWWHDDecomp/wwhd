/* WWHD Dragonhead. Ported from zeldaret/tww with audited HD behavior.
 */
#include "d/actor/d_a_obj_dragonhead.h"
namespace daObjDragonhead {
static void *resource(s32 index) {
  gabi::Local<SafeString> name;
  name->mStringTop = 0x100276AC;
  name->__vtbl = 0x10027684;
  return gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), index);
}
BOOL Act_c::CreateHeap() {
  WWHD_FUNC(0x0233780C, BOOL, this);
  void *data = resource(4);
  if (!data)
    gabi::call(0x0273AA24, STR(0x100276C4), 0xA0, STR(0x100276B4));
  mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
  if (!mModel)
    return FALSE;
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
  gabi::call(0x028E93CC, matrix, x, y, z);
  gabi::call(0x025F1C28, matrix, (s16)shape_angle.y);
  x = scale.x;
  y = scale.y;
  z = scale.z;
  gabi::call(0x025F2518, x, y, z);
  gabi::call(0x028E90D4, matrix, &mMatrix);
  mBgW = gabi::call<dBgW *>(0x024F23F4, nullptr);
  if (!mBgW)
    return FALSE;
  data = resource(7);
  if (gabi::call<s32>(0x0200A030, (dBgW *)mBgW, data, 1, &mMatrix))
    return FALSE;
  return mModel != nullptr;
}
VERIFY(0x0233780C, &Act_c::CreateHeap);
BOOL CheckCreateHeap(Act_c *actor) {
  WWHD_FUNC(0x0233794C, BOOL, actor);
  return actor->CreateHeap();
}
VERIFY(0x0233794C, CheckCreateHeap);
void Act_c::set_mtx() {
  WWHD_FUNC(0x02337950, void, this);
  f32 x = scale.x, y = scale.y, z = scale.z;
  u32 model = gabi::ea((J3DModel *)mModel);
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  x = current.pos.x;
  y = current.pos.y;
  z = current.pos.z;
  gabi::call(0x028E93CC, matrix, x, y, z);
  gabi::call(0x025F1C28, matrix, (s16)current.angle.y);
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = gabi::load<f32>(0x1048D0CC + 4 * i);
  model = gabi::ea((J3DModel *)mModel);
  for (u32 i = 0; i < 12; ++i)
    gabi::store<f32>(model + 0xC8 + 4 * i, values[i]);
}
VERIFY(0x02337950, &Act_c::set_mtx);
void Act_c::CreateInit() {
  WWHD_FUNC(0x02337A28, void, this);
  u32 base = gabi::ea(this), model = gabi::ea((J3DModel *)mModel);
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  gabi::store<f32>(base + 0x364, 1.0f);
  set_mtx();
  mRegistered = 0;
  s8 room = gabi::load<s8>(base + 0x2FE);
  u32 save = gabi::load<u32>(0x101F84DC), sw = (mParameters >> 8) & 255;
  if (gabi::call<s32>(0x025BA0C0, gabi::at<u8>(save + 0x20), sw, room)) {
    mAlpha = 0;
    mSwitchOn = 1;
  } else {
    u8 registered = mRegistered;
    mSwitchOn = 0;
    mAlpha = 255;
    if (!registered) {
      u32 play = gabi::call<u32>(0x025200D4);
      if (!gabi::call<s32>(0x024EEA6C, gabi::at<u8>(play + 0x12A0),
                           (dBgW *)mBgW, this))
        mRegistered = 1;
    }
  }
  gabi::call(0x02515F14, mCollisionStatus, 255, 255, this);
  gabi::call(0x0251677C, mSphere, gabi::at<u8>(0x101C8D28));
  mSphereCenter.y = 950.0f;
  mSphereCenter.z = 220000.0f;
  mSphereCenter.x = 79338.0f;
  gabi::call(0x02018D40, gabi::at<u8>(base + 0x50C), &mSphereCenter);
  gabi::store<u32>(base + 0x438, base + 0x3B8);
}
VERIFY(0x02337A28, &Act_c::CreateInit);
s32 Create(Act_c *actor) {
  WWHD_FUNC(0x02337BB4, s32, actor);
  u32 base = gabi::ea(actor), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (actor) {
      gabi::call(0x025D4ED0, actor);
      gabi::store<u32>(base + 0xB4, 0x1002769C);
      gabi::call(0x0200BD2C, actor->mCollisionStatus);
      gabi::call(0x02515DA0, gabi::at<u8>(base + 0x3D4));
      gabi::store<u32>(base + 0x3D0, 0x1004AE88);
      gabi::store<u32>(base + 0x3D4, 0x1004AEC0);
      gabi::call(0x025166F0, actor->mSphere);
      flags = gabi::load<u32>(base + 0x2E4);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &actor->mPhs, STR(0x1002767C));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, actor, 0x0233794C, 0x10500))
      return 5;
    actor->CreateInit();
  }
  return phase;
}
VERIFY(0x02337BB4, Create);
BOOL Delete(Act_c *actor) {
  WWHD_FUNC(0x02337CAC, BOOL, actor);
  if (gabi::load<u32>(gabi::ea(actor) + 0xF4) && actor->mRegistered) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, gabi::at<u8>(play + 0x12A0), (dBgW *)actor->mBgW);
  }
  gabi::call(0x025E1B34, &actor->mSphereCenter);
  gabi::call(0x025204C8, &actor->mPhs, STR(0x100276EC));
  return TRUE;
}
VERIFY(0x02337CAC, Delete);
struct FloatColor {
  be<f32> r, g, b, a;
};
void colorToFloat(FloatColor *out, u8 *in) {
  WWHD_FUNC(0x02337D18, void, out, in);
  u32 p = gabi::ea(in);
  f32 r = gabi::load<u8>(p), g = gabi::load<u8>(p + 1),
      b = gabi::load<u8>(p + 2), a = gabi::load<u8>(p + 3);
  out->r = r / 255.0f;
  out->g = g / 255.0f;
  out->b = b / 255.0f;
  out->a = a / 255.0f;
}
VERIFY(0x02337D18, colorToFloat);
BOOL Draw(Act_c *actor) {
  WWHD_FUNC(0x02337DCC, BOOL, actor);
  u32 base = gabi::ea(actor), light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 1, &actor->current.pos,
             gabi::at<u8>(base + 0x110));
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(light), (J3DModel *)actor->mModel,
             gabi::at<u8>(base + 0x110));
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  u32 model = gabi::ea((J3DModel *)actor->mModel),
      data = gabi::load<u32>(model + 0xAC);
  u32 resource = gabi::call<u32>(0x027F3F8C, gabi::at<u8>(data));
  u32 count = gabi::load<u16>(resource + 0x24);
  u16 index = 0;
  while (count) {
    u32 material = gabi::load<u32>(data + 0x10),
        n = gabi::load<u32>(data + 0xC);
    if (index < n)
      material += (u32)index * 0x39C;
    u32 tev = gabi::load<u32>(material + 0x18), vt = gabi::load<u32>(tev + 4);
    u32 color =
        gabi::call<u32>(gabi::load<u32>(vt + 0x4C), gabi::at<u8>(tev), 3);
    gabi::store<u8>(color + 3, actor->mAlpha);
    tev = gabi::load<u32>(material + 0x18);
    vt = gabi::load<u32>(tev + 4);
    color = gabi::call<u32>(gabi::load<u32>(vt + 0x4C), gabi::at<u8>(tev), 3);
    tev = gabi::load<u32>(material + 0x18);
    vt = gabi::load<u32>(tev + 4);
    gabi::call(gabi::load<u32>(vt + 0x3C), gabi::at<u8>(tev), 3,
               gabi::at<u8>(color));
    gabi::Local<FloatColor> input, output;
    colorToFloat(input.get(), gabi::at<u8>(color));
    gabi::call(0x0274D458, output.get(), input.get(), 1.0f);
    u32 flags = gabi::load<u32>(material + 0xA0);
    gabi::store<u32>(material + 0xA0, flags | 0x400);
    u32 dest = gabi::call<u32>(0x027F9F0C, gabi::at<u8>(material + 0xA0), 10);
    f32 alpha = gabi::load<u8>(color + 3), r = output->r, g = output->g,
        b = output->b;
    gabi::store<f32>(dest + 4, g);
    gabi::store<f32>(dest + 8, b);
    gabi::store<f32>(dest, r);
    gabi::store<f32>(dest + 12, alpha / 255.0f);
    ++index;
    --count;
  }
  gabi::call(0x025E2DE0, (J3DModel *)actor->mModel, 0);
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
  return TRUE;
}
VERIFY(0x02337DCC, Draw);
BOOL Execute(Act_c *actor) {
  WWHD_FUNC(0x02337FA0, BOOL, actor);
  u32 base = gabi::ea(actor), play = gabi::call<u32>(0x025200D4);
  gabi::call(0x0200E240, gabi::at<u8>(play + 0x26A4), actor->mSphere);
  if (!actor->mSwitchOn) {
    s32 reverb = gabi::call<s32>(0x02520540, (s8)actor->current.roomNo);
    gabi::call(0x025E1A40, 0x61CC, &actor->mSphereCenter, 0, reverb);
    if (gabi::call<s32>(0x025162A4, actor->mSphere)) {
      u32 hit = gabi::call<u32>(0x02516300, actor->mSphere);
      if (hit && (gabi::load<u32>(hit + 0x10) & 0x40000)) {
        u32 parameters = actor->mParameters, save = gabi::load<u32>(0x101F84DC);
        s8 room = gabi::load<s8>(base + 0x2FE);
        gabi::call(0x025B9E38, gabi::at<u8>(save + 0x20),
                   (parameters >> 8) & 255, room);
        actor->mSwitchOn = 1;
      }
    }
  }
  u32 parameters = actor->mParameters, save = gabi::load<u32>(0x101F84DC);
  s8 room = gabi::load<s8>(base + 0x2FE);
  if (gabi::call<s32>(0x025BA0C0, gabi::at<u8>(save + 0x20),
                      (parameters >> 8) & 255, room)) {
    if (actor->mRegistered) {
      play = gabi::call<u32>(0x025200D4);
      s32 ret = gabi::call<s32>(0x020087EC, gabi::at<u8>(play + 0x12A0),
                                (dBgW *)actor->mBgW);
      s8 soundRoom = actor->current.roomNo;
      if (!ret)
        actor->mRegistered = 0;
      s32 reverb = gabi::call<s32>(0x02520540, soundRoom);
      gabi::call(0x025E1A40, 0x69D0, &actor->eyePos, 0, reverb);
    }
    u8 alpha = actor->mAlpha;
    if (alpha)
      actor->mAlpha = alpha >= 2 ? alpha - 2 : 0;
  } else {
    u8 registered = actor->mRegistered;
    actor->mSwitchOn = 0;
    if (!registered) {
      play = gabi::call<u32>(0x025200D4);
      if (!gabi::call<s32>(0x024EEA6C, gabi::at<u8>(play + 0x12A0),
                           (dBgW *)actor->mBgW, actor))
        actor->mRegistered = 1;
    }
    u8 alpha = actor->mAlpha;
    if (alpha < 255)
      actor->mAlpha = alpha <= 253 ? alpha + 2 : 255;
  }
  actor->set_mtx();
  return TRUE;
}
VERIFY(0x02337FA0, Execute);
void staticInitialize() {
  WWHD_FUNC(0x0233818C, void);
  gabi::store<u32>(0x1046968C, 0);
  gabi::store<u32>(0x10469684, 0);
  gabi::store<u32>(0x10469690, 0);
  gabi::store<u32>(0x10469688, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8D88));
  gabi::store<f32>(0x10469678, -3.1415927410125732f);
  gabi::store<f32>(0x1046967C, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469680));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8D94));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469681));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8DA0));
}
VERIFY(0x0233818C, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x02338220, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x02338220, deleteStatic);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x02338234, BOOL, actor);
  return TRUE;
}
VERIFY(0x02338234, IsDelete);
void destruct(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x0233823C, void, actor, flags);
  if (actor) {
    gabi::call(0x02515AE8, actor->mSphere, 2);
    gabi::call(0x02515860, actor->mCollisionStatus, 2);
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x0233823C, destruct);
void emptyVirtual() { WWHD_FUNC(0x023382A8, void); }
VERIFY(0x023382A8, emptyVirtual);
} // namespace daObjDragonhead
