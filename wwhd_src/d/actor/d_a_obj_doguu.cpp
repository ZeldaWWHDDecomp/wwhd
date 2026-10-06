/* WWHD Goddess Statues. HD behavior verified against cking.rpx. */
#include "d/actor/d_a_obj_doguu.h"
namespace daObjDoguu {
static void *saveEvents() {
  return gabi::at<u8>(gabi::load<u32>(0x101F84DC) + 0x644);
}
s32 Act_c::getFinishEventCount() {
  WWHD_FUNC(0x02336824, s32, this);
  s32 count = 0;
  if (gabi::call<s32>(0x025B8B94, saveEvents(), 0x1480))
    count = 1;
  if (gabi::call<s32>(0x025B8B94, saveEvents(), 0x1440))
    ++count;
  if (gabi::call<s32>(0x025B8B94, saveEvents(), 0x1410))
    ++count;
  return count;
}
VERIFY(0x02336824, &Act_c::getFinishEventCount);
void Act_c::setFinishMyEvent() {
  WWHD_FUNC(0x023368B0, void, this);
  s32 variant = mVariant;
  s32 event = variant == 0 ? 0x1480 : variant == 1 ? 0x1440 : 0x1410;
  gabi::call(0x025B8B68, saveEvents(), event);
}
VERIFY(0x023368B0, &Act_c::setFinishMyEvent);
u32 Act_c::getMsg() {
  WWHD_FUNC(0x02336F84, u32, this);
  if (mAction == 4)
    return 3821;
  s32 variant = mVariant;
  return variant == 0 ? 3822 : variant == 1 ? 3823 : 3824;
}
VERIFY(0x02336F84, &Act_c::getMsg);
u16 Act_c::next_msgStatus(be<u32> *message) {
  WWHD_FUNC(0x02336FC4, u16, this, message);
  u32 id = *message;
  if (id >= 3822 && id <= 3824) {
    if (getFinishEventCount() == 0) {
      *message = 3825;
      return 15;
    }
    if (getFinishEventCount() == 1) {
      *message = 3826;
      return 15;
    }
  }
  return 16;
}
VERIFY(0x02336FC4, &Act_c::next_msgStatus);
void Act_c::setJDemo(s32 staff) {
  WWHD_FUNC(0x023363DC, void, this, staff);
  // HD's stage helper adds wipe and transition flags.
  s16 room = gabi::load<s16>(gabi::ea(this) + 0xB82);
  gabi::call(0x0252012C, STR(0x10027500), room, 0, 8, 0, 1, 0, 0.0f);
}
VERIFY(0x023363DC, &Act_c::setJDemo);
void Act_c::setPlayerAngle(s32 staff) {
  WWHD_FUNC(0x02336294, void, this, staff);
  u32 play = gabi::call<u32>(0x025200D4);
  u32 value = gabi::call<u32>(0x0254487C, gabi::at<u8>(play + 0x52C4), staff,
                              STR(0x100274C4), 3);
  s16 offset = value ? gabi::load<s16>(value + 2) : 0;
  play = gabi::call<u32>(0x025200D4);
  s16 angle = current.angle.y;
  u32 player = gabi::load<u32>(play + 0x5B2C);
  u32 vt = gabi::load<u32>(player + 0xB4);
  u32 fn = gabi::load<u32>(vt + 0x114);
  gabi::call(fn, gabi::at<u8>(player), gabi::at<cXyz>(player + 0x314),
             (s16)(angle + offset));
}
VERIFY(0x02336294, &Act_c::setPlayerAngle);
void Act_c::setQuake(s32 staff) {
  WWHD_FUNC(0x0233631C, void, this, staff);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::Local<cXyz> direction;
  direction->x = 0;
  direction->y = 1;
  direction->z = 0;
  gabi::call(0x025CB4E0, gabi::at<u8>(play + 0x599C), gabi::at<u8>(0x100274CC),
             0, 63, direction.get());
  play = gabi::call<u32>(0x025200D4);
  u32 value = gabi::call<u32>(0x0254487C, gabi::at<u8>(play + 0x52C4), staff,
                              STR(0x100274D4), 3);
  if (!value) {
    gabi::call(0x0273AA24, STR(0x100274DC), 0x2F6, STR(0x100274F0));
    mTimer = 1;
  } else
    mTimer = gabi::load<s32>(value);
}
VERIFY(0x0233631C, &Act_c::setQuake);
void Act_c::setGoal(s32 staff) {
  WWHD_FUNC(0x02336194, void, this, staff);
  u32 play = gabi::call<u32>(0x025200D4);
  u32 value = gabi::call<u32>(0x0254487C, gabi::at<u8>(play + 0x52C4), staff,
                              STR(0x100274BC), 1);
  f32 y = current.pos.y, localY = gabi::load<f32>(value + 4);
  f32 x = current.pos.x, localZ = gabi::load<f32>(value + 8), z = current.pos.z,
      localX = gabi::load<f32>(value);
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  gabi::call(0x028E93CC, matrix, x, y, z);
  gabi::call(0x025F1C28, matrix, (s16)current.angle.y);
  gabi::call(0x025F24E0, localX, localY, localZ);
  f32 gx = gabi::load<f32>(0x1048D0D8), gy = gabi::load<f32>(0x1048D0E8),
      gz = gabi::load<f32>(0x1048D0F8);
  mGoal.y = gy;
  mGoal.x = gx;
  mGoal.z = gz;
  play = gabi::call<u32>(0x025200D4);
  gabi::call(0x02543714, gabi::at<u8>(play + 0x52C4), &mGoal);
}
VERIFY(0x02336194, &Act_c::setGoal);
BOOL Delete(Act_c *actor) {
  WWHD_FUNC(0x02335C60, BOOL, actor);
  gabi::call(0x0255A374, actor->mLightInfluence);
  gabi::call(0x025204C8, gabi::at<u8>(gabi::ea(actor) + 0x7DC),
             STR(0x100274B0));
  return TRUE;
}
VERIFY(0x02335C60, Delete);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x02337104, BOOL, actor);
  return TRUE;
}
VERIFY(0x02337104, IsDelete);
} // namespace daObjDoguu
namespace daObjDoguu {
void Act_c::setPointLight() {
  WWHD_FUNC(0x0233516C, void, this);
  s32 variant = mVariant;
  u32 px = gabi::load<u32>(gabi::ea(&mLightPos)),
      py = gabi::load<u32>(gabi::ea(&mLightPos) + 4),
      pz = gabi::load<u32>(gabi::ea(&mLightPos) + 8);
  u32 light = gabi::ea(mLightInfluence);
  gabi::store<u32>(light, px);
  gabi::store<u32>(light + 4, py);
  gabi::store<u32>(light + 8, pz);
  s16 red = gabi::load<s16>(0x10027450 + 6u * (u32)variant);
  f32 power = mLightPower;
  gabi::store<s16>(light + 12, (s16)gabi::ftoi((f32)red * power));
  s16 green = gabi::load<s16>(0x10027452 + 6u * (u32)variant);
  gabi::store<s16>(light + 14, (s16)gabi::ftoi((f32)green * power));
  s16 blue = gabi::load<s16>(0x10027454 + 6u * (u32)variant);
  gabi::store<s16>(light + 16, (s16)gabi::ftoi((f32)blue * power));
  gabi::store<f32>(light + 20, (f32)(s16)gabi::ftoi(300.0f * power));
  gabi::store<f32>(light + 24, 100.0f);
}
VERIFY(0x0233516C, &Act_c::setPointLight);
void Act_c::set_mtx() {
  WWHD_FUNC(0x0233528C, void, this);
  for (u32 offset : {0x7E4u, 0x7ECu, 0x7F0u, 0x7E8u}) {
    f32 x = scale.x, y = scale.y, z = scale.z;
    u32 model = gabi::load<u32>(gabi::ea(this) + offset);
    gabi::store<f32>(model + 0xBC, x);
    gabi::store<f32>(model + 0xC0, y);
    gabi::store<f32>(model + 0xC4, z);
  }
  for (u32 offset : {0x7E4u, 0x7ECu, 0x7F0u}) {
    auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    gabi::call(0x028E93CC, matrix, x, y, z);
    gabi::call(0x025F1C28, matrix, (s16)current.angle.y);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
      values[i] = gabi::load<f32>(0x1048D0CC + 4 * i);
    u32 model = gabi::load<u32>(gabi::ea(this) + offset);
    for (u32 i = 0; i < 12; ++i)
      gabi::store<f32>(model + 0xC8 + 4 * i, values[i]);
  }
  u32 body = gabi::ea((J3DModel *)mBody), joint = mHeadJoint;
  u32 block = gabi::load<u32>(body + 0x2C);
  u16 flags = gabi::load<u16>(block + 4);
  u32 matrices = gabi::load<u32>(block + 0x10);
  u32 head = gabi::ea((J3DModel *)mHead);
  gabi::store<u16>(block + 4, flags | 0x10);
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = gabi::load<f32>(matrices + joint * 0x30 + 4 * i);
  for (u32 i = 0; i < 12; ++i)
    gabi::store<f32>(head + 0xC8 + 4 * i, values[i]);
}
VERIFY(0x0233528C, &Act_c::set_mtx);
static u32 doguuResource(s32 index) {
  gabi::Local<SafeString> name;
  name->mStringTop = 0x100273C4;
  name->__vtbl = 0x100272EC;
  u32 controller = gabi::load<u32>(0x101F4F28);
  return gabi::call<u32>(0x026066C4, gabi::at<u8>(controller), name.get(),
                         index);
}
static void assertResource(u32 resource, s32 line, u32 message) {
  if (!resource)
    gabi::call(0x0273AA24, STR(0x100273D4), line, STR(message));
}
BOOL Act_c::CreateHeap() {
  WWHD_FUNC(0x02334BE8, BOOL, this);
  auto index = [&](u32 offset) {
    return gabi::load<s32>(0x10027378 + offset + 4u * (u32)mVariant);
  };
  u32 data = doguuResource(index(0));
  assertResource(data, 0x162, 0x10027408);
  auto *main = gabi::call<J3DModel *>(0x025E38E0, gabi::at<u8>(data), 0x80000,
                                      0x15220202);
  mMain = main;
  if (!main)
    return FALSE;
  u32 animation = doguuResource(index(0x24));
  assertResource(animation, 0x178, 0x1002741C);
  f32 rate = 1.0f;
  if (!gabi::call<s32>(0x025E8154, mBrk, gabi::at<u8>(data),
                       gabi::at<u8>(animation), 1, 2, 0, -1, 0, rate, 0))
    return FALSE;
  data = doguuResource(index(0x30));
  assertResource(data, 0x17F, 0x10027408);
  auto *head =
      gabi::call<J3DModel *>(0x025E38E0, gabi::at<u8>(data), 0, 0x11020203);
  mHead = head;
  if (!head)
    return FALSE;
  animation = doguuResource(index(0x18));
  assertResource(animation, 0x185, 0x100273E8);
  if (!gabi::call<s32>(0x025E8508, mHeadBck, gabi::at<u8>(data),
                       gabi::at<u8>(animation), 1, 0, 0, -1, 0, rate))
    return FALSE;
  data = doguuResource(index(0xC));
  assertResource(data, 0x19A, 0x10027408);
  auto *body = gabi::call<J3DModel *>(0x025E38E0, gabi::at<u8>(data), 0x80000,
                                      0x15220202);
  mBody = body;
  if (!body)
    return FALSE;
  u32 modelData = gabi::load<u32>(gabi::ea(body) + 0xAC);
  u32 names = gabi::call<u32>(0x027F68FC, gabi::at<u8>(modelData));
  u32 relative = gabi::load<u32>(names + 0x10);
  s32 joint = gabi::call<s32>(
      0x027DF9B0, gabi::at<u8>(relative ? names + 0x10 + relative : 0),
      STR(0x100273CC));
  if (joint >= 0)
    mHeadJoint = joint;
  animation = doguuResource(6);
  assertResource(animation, 0x1C5, 0x100273F8);
  if (!gabi::call<s32>(0x025E8508, mBodyBck, gabi::at<u8>(data),
                       gabi::at<u8>(animation), 1, 0, 0, -1, 0, rate))
    return FALSE;
  data = doguuResource(index(0x3C));
  assertResource(data, 0x1D9, 0x10027408);
  auto *crystal =
      gabi::call<J3DModel *>(0x025E38E0, gabi::at<u8>(data), 0, 0x11020203);
  mCrystal = crystal;
  if (!crystal)
    return FALSE;
  animation = doguuResource(10);
  assertResource(animation, 0x1DF, 0x1002742C);
  if (!gabi::call<s32>(0x025E8508, mCrystalBck, gabi::at<u8>(data),
                       gabi::at<u8>(animation), 1, 0, 0, -1, 0, rate))
    return FALSE;
  mCrystalJoint = 1;
  return TRUE;
}
VERIFY(0x02334BE8, &Act_c::CreateHeap);
BOOL CheckCreateHeap(Act_c *actor) {
  WWHD_FUNC(0x02334FD4, BOOL, actor);
  return actor->CreateHeap();
}
VERIFY(0x02334FD4, CheckCreateHeap);
} // namespace daObjDoguu
namespace daObjDoguu {
Act_c *construct(Act_c *actor) {
  WWHD_FUNC(0x02334FD8, Act_c *, actor);
  if (!actor)
    actor = gabi::call<Act_c *>(0x0273AD10, 0xBF0);
  if (!actor)
    return nullptr;
  u32 base = gabi::ea(actor);
  auto part = [&](u32 offset) { return gabi::at<u8>(base + offset); };
  auto word = [&](u32 offset, u32 value) {
    gabi::store<u32>(base + offset, value);
  };
  gabi::call(0x025A1458, actor);
  word(0xB4, 0x100275A0);
  gabi::call(0x025E80D0, part(0x7F4));
  for (u32 ctrl : {0x86Cu, 0x8F8u, 0x984u}) {
    gabi::call(0x027F2BC0, part(ctrl), 0);
    word(ctrl + 0x10, 0x1016E54C);
    gabi::call(0x027DA984, part(ctrl + 0x14));
    word(ctrl + 0x80, 0);
    word(ctrl + 0x58, 0);
    word(ctrl + 0x84, 0);
    word(ctrl + 0x7C, 0);
    word(ctrl + 0x48, 0x1016D820);
    word(ctrl + 0x10, 0x10027314);
    word(ctrl + 0x88, 0);
  }
  gabi::call(0x0200BD2C, part(0xA10));
  gabi::call(0x02515DA0, part(0xA2C));
  word(0xA28, 0x1004AE88);
  word(0xA2C, 0x1004AEC0);
  gabi::call(0x02515FB8, part(0xA4C));
  word(0xB60, 0x100015A8);
  word(0xB5C, 0x10027304);
  gabi::call(0x02018590, part(0xB64));
  gabi::store<f32>(base + 0xBE8, 1.0f);
  word(0xB78, 0x1004B150);
  word(0xA88, 0x1004B108);
  word(0xB60, 0x1004B160);
  return actor;
}
VERIFY(0x02334FD8, construct);
void staticInitialize() {
  WWHD_FUNC(0x0233705C, void);
  gabi::store<u32>(0x10469654, 0);
  gabi::store<u32>(0x1046964C, 0);
  gabi::store<u32>(0x10469658, 0);
  gabi::store<u32>(0x10469650, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8C60));
  gabi::store<f32>(0x10469640, -3.1415927410125732f);
  gabi::store<f32>(0x10469644, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469648));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8C6C));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469649));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8C78));
}
VERIFY(0x0233705C, staticInitialize);
void deleteStatic(void *object, u32 flags) {
  WWHD_FUNC(0x023370F0, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x023370F0, deleteStatic);
void destruct(Act_c *actor, u32 flags) {
  WWHD_FUNC(0x0233710C, void, actor, flags);
  if (!actor)
    return;
  u32 base = gabi::ea(actor);
  auto part = [&](u32 offset) { return gabi::at<u8>(base + offset); };
  gabi::call(0x02515A70, part(0xA4C), 2);
  gabi::call(0x02515860, part(0xA10), 2);
  gabi::call(0x027F3628, part(0x994), 0);
  gabi::call(0x027F3628, part(0x908), 0);
  gabi::call(0x027F3628, part(0x87C), 0);
  gabi::call(0x02515A70, part(0x690), 2);
  gabi::call(0x02515860, part(0x654), 2);
  gabi::call(0x02018034, part(0x628), 2);
  gabi::store<u32>(base + 0x470, 0x1002733C);
  gabi::store<u32>(base + 0x464, 0x1002734C);
  gabi::call(0x024EFD9C, part(0x450), 0);
  gabi::call(0x025D50BC, actor, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, actor);
}
VERIFY(0x0233710C, destruct);
void emptyVirtual() { WWHD_FUNC(0x023371E4, void); }
VERIFY(0x023371E4, emptyVirtual);
} // namespace daObjDoguu
namespace daObjDoguu {
void privateCut(Act_c *actor) {
  WWHD_FUNC(0x02336408, void, actor);
  auto event = []() {
    return gabi::at<u8>(gabi::call<u32>(0x025200D4) + 0x52C4);
  };
  s32 staff = gabi::call<s32>(0x02542D88, event(), STR(0x10027510), 0, 0);
  if (staff == -1)
    return;
  s8 action = (s8)gabi::call<s32>(0x02542EDC, event(), staff,
                                  gabi::at<u8>(0x101C8C3C), 9, 1, 0);
  actor->mAction = action;
  if (action == -1) {
    gabi::call(0x02543280, event(), staff);
    return;
  }
  bool end = false;
  if (gabi::call<s32>(0x025447C8, event(), staff)) {
    action = actor->mAction;
    switch (action) {
    case 0:
      actor->setGoal(staff);
      break;
    case 1: {
      s8 room = actor->current.roomNo;
      actor->mHasPearl = 1;
      s32 reverb = gabi::call<s32>(0x02520540, room);
      gabi::call(0x025E1A40, 0x288C, &actor->current.pos, 0, reverb);
      actor->mLightActive = 1;
      break;
    }
    case 2:
      actor->setPlayerAngle(staff);
      break;
    case 3:
      actor->mEyeFlash = 1;
      break;
    case 5:
      gabi::call(0x025E1918, 0x8000004Fu);
      break;
    case 6:
      actor->setQuake(staff);
      break;
    case 7:
      actor->setJDemo(staff);
      break;
    case 8:
      actor->mLightTimer = 0;
      break;
    }
  }
  action = actor->mAction;
  switch (action) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 7:
    end = true;
    break;
  case 4:
  case 5:
    end = gabi::call<s32>(0x025A11EC, actor, 1) == 18;
    break;
  case 6: {
    s32 timer = (u32)(s32)actor->mTimer - 1;
    actor->mTimer = timer;
    if (timer == 0) {
      gabi::call(0x025CB610, gabi::at<u8>(gabi::call<u32>(0x025200D4) + 0x599C),
                 63);
      end = true;
    }
    break;
  }
  case 8: {
    s32 timer = actor->mLightTimer;
    if (timer >= 200) {
      actor->mColorRatio = 1.0f;
      end = true;
    } else {
      timer = (u32)timer + 1;
      actor->mLightTimer = timer;
    }
    timer = actor->mLightTimer;
    gabi::call(0x0200ED84, &actor->mColorRatio, timer < 160 ? 0.1f : 1.0f, 0.1f,
               0.025f);
    timer = actor->mLightTimer;
    if (timer == 40) {
      actor->mLightPos.y = 0;
      actor->mLightPos.x = 0;
      actor->mLightPos.z = 0;
      u32 crystal = gabi::ea((J3DModel *)actor->mCrystal),
          joint = actor->mCrystalJoint;
      u32 block = gabi::load<u32>(crystal + 0x2C),
          matrices = gabi::load<u32>(block + 0x10);
      gabi::store<u16>(block + 4, gabi::load<u16>(block + 4) | 0x10);
      gabi::call(0x028E90D4, gabi::at<u8>(matrices + joint * 0x30),
                 gabi::at<Mtx34>(0x1048D0CC));
      actor->mLightPos.x = gabi::load<f32>(0x1048D0D8);
      actor->mLightPos.y = gabi::load<f32>(0x1048D0E8);
      s32 variant = actor->mVariant;
      actor->mLightPos.z = gabi::load<f32>(0x1048D0F8);
      u32 play = gabi::call<u32>(0x025200D4),
          particles = gabi::load<u32>(play + 0x5AB0);
      u32 id = variant == 0 ? 0x8434 : variant == 1 ? 0x8436 : 0x8435;
      gabi::call(0x025A847C, gabi::at<u8>(particles), 0, id, &actor->mLightPos,
                 &actor->current.angle, 0, 255, 0, -1, 0, 0, 0);
    } else if (timer == 80)
      actor->mLightActive = 2;
    break;
  }
  default:
    end = true;
    break;
  }
  if (end)
    gabi::call(0x02543280, event(), staff);
}
VERIFY(0x02336408, privateCut);
} // namespace daObjDoguu
namespace daObjDoguu {
void CreateInit(Act_c *actor) {
  WWHD_FUNC(0x02335548, void, actor);
  u32 base = gabi::ea(actor);
  auto part = [&](u32 offset) { return gabi::at<u8>(base + offset); };
  u32 model = gabi::ea((J3DModel *)actor->mMain);
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, actor, -600.0f, -0.0f, -600.0f, 600.0f, 400.0f,
             600.0f);
  s32 variant = actor->mVariant;
  gabi::store<f32>(base + 0x364, 1.0f);
  gabi::store<u8>(base + 0xB7C, variant == 0   ? 0x6A
                                : variant == 1 ? 0x6B
                                               : 0x69);
  gabi::call(0x02515F14, part(0xA10), 255, 255, actor);
  gabi::call(0x02516518, part(0xA4C), gabi::at<u8>(0x101C8BF8));
  if (!gabi::call<s32>(0x0254DA50, gabi::load<u8>(base + 0xB7C), 1))
    gabi::call(0x020184DC, part(0xB64), 50.0f);
  gabi::store<u32>(base + 0xA90, base + 0xA10);
  actor->mLightPower = 0;
  actor->setPointLight();
  gabi::call(0x025564B4, actor->mLightInfluence);
  if (gabi::load<s8>(base + 0x2DD) > 0) {
    f32 head = (f32)gabi::load<s16>(base + 0x874),
        body = (f32)gabi::load<s16>(base + 0x900),
        crystal = (f32)gabi::load<s16>(base + 0x98C);
    actor->mHasPearl = 1;
    gabi::store<f32>(base + 0x870, head);
    gabi::store<f32>(base + 0x8FC, body);
    actor->mUseBody = 0;
    actor->mEyeFlash = 1;
    gabi::store<f32>(base + 0x988, crystal);
    actor->mState = 14;
  } else if (gabi::call<s32>(0x025B8B94, saveEvents(), 0x1480) &&
             gabi::call<s32>(0x025B8B94, saveEvents(), 0x1440) &&
             gabi::call<s32>(0x025B8B94, saveEvents(), 0x1410)) {
    f32 head = (f32)gabi::load<s16>(base + 0x876),
        body = (f32)gabi::load<s16>(base + 0x902),
        crystal = (f32)gabi::load<s16>(base + 0x98E);
    actor->mHasPearl = 1;
    actor->mUseBody = 1;
    gabi::store<f32>(base + 0x870, head);
    actor->mState = 14;
    actor->mEyeFlash = 0;
    gabi::store<f32>(base + 0x8FC, body);
    gabi::store<f32>(base + 0x988, crystal);
  } else {
    variant = actor->mVariant;
    actor->mUseBody = 0;
    bool placed = false;
    if (variant == 0) {
      placed = gabi::call<s32>(0x025B8B94, saveEvents(), 0x1480) != 0;
      if (!placed)
        variant = actor->mVariant;
    }
    if (!placed && variant == 1) {
      placed = gabi::call<s32>(0x025B8B94, saveEvents(), 0x1440) != 0;
      if (!placed)
        variant = actor->mVariant;
    }
    if (!placed && variant == 2)
      placed = gabi::call<s32>(0x025B8B94, saveEvents(), 0x1410) != 0;
    actor->mEyeFlash = placed;
    actor->mHasPearl = placed;
    actor->mState = placed ? 10 : 0;
  }
  actor->eyePos.y = (f32)actor->eyePos.y + 125.0f;
  actor->set_mtx();
  gabi::call(0x027F4D5C, (J3DModel *)actor->mBody);
  actor->set_mtx();
  auto event = []() {
    return gabi::at<u8>(gabi::call<u32>(0x025200D4) + 0x52C4);
  };
  actor->mDemo1 = gabi::call<s32>(0x02543F10, event(), STR(0x10027480), 255);
  actor->mDemo2 = gabi::call<s32>(0x02543F10, event(), STR(0x1002748C), 255);
  actor->mDemo3 = gabi::call<s32>(0x02543F10, event(), STR(0x10027498), 255);
  s16 megami = gabi::call<s32>(0x02543F10, event(), STR(0x100274A4), 255);
  bool pearl = actor->mHasPearl != 0;
  actor->mMegami = megami;
  actor->mLightActive = pearl ? 2 : 0;
  actor->mColorRatio = 1;
}
VERIFY(0x02335548, CreateInit);
s32 Create(Act_c *actor) {
  WWHD_FUNC(0x02335B6C, s32, actor);
  u32 base = gabi::ea(actor), status = gabi::load<u32>(base + 0x2E4);
  if (!(status & 8)) {
    if (actor) {
      construct(actor);
      status = gabi::load<u32>(base + 0x2E4);
    }
    gabi::store<u32>(base + 0x2E4, status | 8);
  }
  s8 argument = gabi::load<s8>(base + 0x2DD);
  actor->mVariant = argument > 0 ? argument - 1 : gabi::load<u8>(base + 0xB3);
  s32 phase = gabi::call<s32>(0x02520460, &actor->mPhs, STR(0x100272E4));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, actor, gabi::at<u8>(0x02334FD4), 0))
      return 5;
    CreateInit(actor);
  }
  return phase;
}
VERIFY(0x02335B6C, Create);
} // namespace daObjDoguu
namespace daObjDoguu {
BOOL Execute(Act_c *actor) {
  WWHD_FUNC(0x023368F0, BOOL, actor);
  u32 base = gabi::ea(actor);
  auto part = [&](u32 offset) { return gabi::at<u8>(base + offset); };
  if (actor->mLightActive) {
    actor->mLightPos.y = 0;
    actor->mLightPos.x = 0;
    actor->mLightPos.z = 0;
    u32 crystal = gabi::ea((J3DModel *)actor->mCrystal),
        joint = actor->mCrystalJoint;
    u32 block = gabi::load<u32>(crystal + 0x2C),
        matrices = gabi::load<u32>(block + 0x10);
    gabi::store<u16>(block + 4, gabi::load<u16>(block + 4) | 0x10);
    gabi::call(0x028E90D4, gabi::at<u8>(matrices + joint * 0x30),
               gabi::at<Mtx34>(0x1048D0CC));
    actor->mLightPos.x = gabi::load<f32>(0x1048D0D8);
    f32 y = gabi::load<f32>(0x1048D0E8);
    u8 active = actor->mLightActive;
    actor->mLightPos.y = y;
    actor->mLightPos.z = gabi::load<f32>(0x1048D0F8);
    f32 random = gabi::call<f32>(0x02019918, active == 1 ? 0.08f : 0.2f);
    gabi::call(0x0200ED84, &actor->mLightPower,
               random + (active == 1 ? 0.15f : 0.8f), 0.25f,
               active == 1 ? 0.1f : 0.25f);
    actor->setPointLight();
  }
  gabi::call(0x020182E0, part(0xB64), &actor->current.pos);
  gabi::call(0x0200E240, gabi::at<u8>(gabi::call<u32>(0x025200D4) + 0x26A4),
             part(0xA4C));
  f32 ratio = actor->mColorRatio;
  if (ratio != 1.0f)
    gabi::call(0x02560444, ratio);
  auto event = []() {
    return gabi::at<u8>(gabi::call<u32>(0x025200D4) + 0x52C4);
  };
  auto orderOther = [&](s16 id) {
    gabi::call(0x025D7A58, actor, id, 255, 65535, 0, 1);
  };
  auto orderChange = [&](s16 id) {
    gabi::call(0x025D7874, actor, id, 0, 65535);
  };
  auto accepted = [&]() { return gabi::load<u16>(base + 0xF8) == 2; };
  switch ((s32)actor->mState) {
  case 0:
    if (gabi::call<s32>(0x0254DA50, gabi::load<u8>(base + 0xB7C), 1) &&
        gabi::call<s32>(0x02516464, part(0xA4C))) {
      u32 object = gabi::call<u32>(0x025163BC, part(0xA4C));
      if (object) {
        u32 status = gabi::load<u32>(object + 0x44);
        if (status) {
          u32 other = gabi::load<u32>(status + 0xC);
          if (other && gabi::load<s16>(other + 8) == 0xA8) {
            gabi::call(0x020184DC, part(0xB64), 50.0f);
            orderOther(actor->mDemo1);
            actor->mState = 1;
          }
        }
      }
    }
    break;
  case 1:
    if (accepted())
      actor->mState = 2;
    else
      orderOther(actor->mDemo1);
    break;
  case 2: {
    privateCut(actor);
    s16 id = actor->mDemo1;
    if (gabi::call<s32>(0x025440C8, event(), id))
      actor->mState = 3;
    break;
  }
  case 3:
    if (actor->getFinishEventCount() == 0) {
      orderChange(actor->mDemo2);
      actor->mState = 4;
    } else {
      orderChange(actor->mDemo3);
      actor->mState = 7;
    }
    break;
  case 4:
    if (accepted())
      actor->mState = 5;
    break;
  case 5: {
    privateCut(actor);
    s16 id = actor->mDemo2;
    if (gabi::call<s32>(0x025440C8, event(), id))
      actor->mState = 6;
    break;
  }
  case 6:
    orderChange(actor->mDemo3);
    actor->mState = 7;
    break;
  case 7:
    if (accepted())
      actor->mState = 8;
    break;
  case 8: {
    privateCut(actor);
    s16 id = actor->mDemo3;
    if (gabi::call<s32>(0x025440C8, event(), id))
      actor->mState = 9;
    break;
  }
  case 9:
    actor->setFinishMyEvent();
    if (actor->getFinishEventCount() >= 3) {
      gabi::call(0x025B8B68, saveEvents(), 0x1E40);
      orderChange(actor->mMegami);
      actor->mState = 11;
    } else {
      actor->mState = 10;
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::store<u16>(play + 0x52B8, gabi::load<u16>(play + 0x52B8) | 8);
    }
    break;
  case 10:
  case 14:
    gabi::call(0x020184DC, part(0xB64), 30.0f);
    break;
  case 11:
    if (accepted())
      actor->mState = 12;
    break;
  case 12: {
    privateCut(actor);
    s16 id = actor->mMegami;
    if (gabi::call<s32>(0x025440C8, event(), id))
      actor->mState = 13;
    break;
  }
  }
  if (!actor->mUseBody && actor->mEyeFlash) {
    f32 frame = gabi::load<f32>(base + 0x7F8);
    if (frame < 1.0f) {
      s32 reverb = gabi::call<s32>(0x02520540, (s8)actor->current.roomNo);
      gabi::call(0x025E1A40, 0x6A31, &actor->current.pos, 0, reverb);
    }
    gabi::call(0x025E742C, actor->mBrk);
  }
  u8 demo = gabi::load<u8>(base + 0x2DC);
  if (demo) {
    if (demo <= 32) {
      u32 object = gabi::load<u32>(0x101D5FFC);
      if (!object) {
        gabi::call(0x0273AA24, STR(0x1002736C), 0x23A, STR(0x1002735C));
        object = gabi::load<u32>(0x101D5FFC);
      }
      u32 playback = gabi::call<u32>(0x02526E70, gabi::at<u8>(object), demo);
      if (playback) {
        if (gabi::load<u16>(playback + 4) & 16)
          actor->mShape = gabi::load<u32>(playback + 0x28);
        if (gabi::load<u16>(playback + 4) & 8) {
          actor->current.angle.x = gabi::load<u16>(playback + 0x20);
          actor->current.angle.y = gabi::load<u16>(playback + 0x22);
          actor->current.angle.z = gabi::load<u16>(playback + 0x24);
        }
        if (gabi::load<u16>(playback + 4) & 2) {
          actor->current.pos.x = gabi::load<f32>(playback + 8);
          actor->current.pos.y = gabi::load<f32>(playback + 12);
          actor->current.pos.z = gabi::load<f32>(playback + 16);
        }
        if (gabi::load<u16>(playback + 4) & 64) {
          f32 frame = gabi::load<f32>(playback + 0x30);
          gabi::store<f32>(base + 0x870, frame);
          gabi::store<f32>(base + 0x988, frame);
          gabi::store<f32>(base + 0x8FC, frame);
        }
      }
    }
    actor->mUseBody = actor->mShape != 0;
  }
  actor->set_mtx();
  return TRUE;
}
VERIFY(0x023368F0, Execute);
} // namespace daObjDoguu
namespace daObjDoguu {
// HD materials keep per-model texture-matrix dirty bits and a relative resource
// table.
static void updateTexMatrices(u32 model) {
  u16 count = gabi::load<u16>(model + 0x2A);
  for (u16 i = 0; i < count; ++i) {
    u32 material = gabi::load<u32>(model + 0x34) + 0x3C * i;
    for (s32 slot = 0; slot < 8; ++slot) {
      gabi::Local<be<s32>> index;
      *index = -1;
      u32 info = gabi::call<u32>(0x027FA974, gabi::at<u8>(material), slot,
                                 index.get());
      if (!info || gabi::load<u32>(info) != 10)
        continue;
      u32 data = gabi::load<u32>(material),
          relative = gabi::load<u32>(data + 0x34);
      s32 matrix = *index;
      u32 entry = (relative ? data + 0x34 + relative : 0) + (u32)matrix * 0x14;
      auto dirty = [&](s32 which) {
        u16 flags = gabi::load<u16>(material + 4);
        u32 bits = gabi::load<u32>(material + 0xC);
        gabi::store<u16>(material + 4, flags | 4);
        u32 word = bits + 4 * (which >> 5);
        gabi::store<u32>(word, gabi::load<u32>(word) | (1u << (which & 31)));
      };
      if (gabi::load<s32>(entry + 4) >= 0) {
        dirty(matrix);
        data = gabi::load<u32>(material);
      }
      relative = gabi::load<u32>(data + 0x34);
      u16 parent = gabi::load<u16>(entry + 0xC);
      u32 parentEntry = (relative ? data + 0x34 + relative : 0) + parent * 0x14;
      if (gabi::load<s32>(parentEntry + 4) >= 0)
        dirty(parent);
      u32 effect = gabi::call<u32>(0x027FA678, gabi::at<u8>(material), slot);
      if (effect)
        gabi::store<u32>(effect, model + 0xF8);
    }
  }
}
BOOL Draw(Act_c *actor) {
  WWHD_FUNC(0x02335CA4, BOOL, actor);
  u32 base = gabi::ea(actor);
  auto part = [&](u32 offset) { return gabi::at<u8>(base + offset); };
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 0, &actor->current.pos,
             part(0x110));
  for (u32 offset : {0x7E4u, 0x7E8u, 0x7ECu, 0x7F0u}) {
    light = gabi::call<u32>(0x02555D0C);
    u32 model = gabi::load<u32>(base + offset);
    gabi::call(0x02562F5C, gabi::at<u8>(light), gabi::at<u8>(model),
               part(0x110));
  }
  if (!gabi::load<u32>(0x101FDB9C)) {
    gabi::store<u32>(0x101FDB9C, 1);
    memcpy_g(gabi::at<u8>(0x101FDBA0), gabi::at<u8>(0x101C8BA8), 0x30);
  }
  u32 camera = gabi::call<u32>(0x024F8020);
  gabi::Local<cXyz> look, lightDirection, reflection;
  gabi::call(0x0201ADE0, &actor->eyePos, look.get(),
             gabi::at<cXyz>(camera + 0xDC));
  gabi::call(0x02563F64, part(0x194), &actor->eyePos, lightDirection.get());
  gabi::call(0x028E9E88, look.get(), lightDirection.get(), reflection.get());
  gabi::Local<Mtx34> reflectionMatrix;
  gabi::call(0x028E9684, reflectionMatrix.get(), gabi::at<cXyz>(0x101FFBA8),
             gabi::at<cXyz>(0x101FFBC0), reflection.get());
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  gabi::call(0x028E945C, matrix, 2.0f, 2.0f, 1.0f);
  gabi::call(0x028E9108, matrix, gabi::at<Mtx34>(0x101FDBA0), matrix);
  gabi::call(0x028E9108, matrix, reflectionMatrix.get(), matrix);
  gabi::store<f32>(0x1048D0D8, 0);
  gabi::store<f32>(0x1048D0E8, 0);
  gabi::store<f32>(0x1048D0F8, 0);
  u32 head = gabi::ea((J3DModel *)actor->mHead);
  gabi::call(0x028E90D4, matrix, gabi::at<Mtx34>(head + 0xF8));
  u32 body = gabi::ea((J3DModel *)actor->mBody);
  gabi::call(0x028E90D4, matrix, gabi::at<Mtx34>(body + 0xF8));
  updateTexMatrices(gabi::ea((J3DModel *)actor->mHead));
  updateTexMatrices(gabi::ea((J3DModel *)actor->mBody));
  auto entryBck = [&](u32 offset, u32 animation, u32 frame) {
    u32 model = gabi::load<u32>(base + offset);
    f32 value = gabi::load<f32>(base + frame);
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E86B8, part(animation), gabi::at<u8>(data), value);
    gabi::call(0x025E2DE0, gabi::at<u8>(gabi::load<u32>(base + offset)), 0);
    model = gabi::load<u32>(base + offset);
    data = gabi::load<u32>(model + 0xAC);
    u32 joint = gabi::load<u32>(data + 8);
    gabi::store<u32>(joint + 0x14, 0);
  };
  if (actor->mUseBody) {
    entryBck(0x7E8, 0x86C, 0x870);
    entryBck(0x7EC, 0x8F8, 0x8FC);
  } else {
    u32 model = gabi::ea((J3DModel *)actor->mMain);
    f32 frame = gabi::load<f32>(base + 0x7F8);
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E83FC, actor->mBrk, gabi::at<u8>(data), frame);
    gabi::call(0x025E2DE0, (J3DModel *)actor->mMain, 0);
    model = gabi::ea((J3DModel *)actor->mMain);
    data = gabi::load<u32>(model + 0xAC);
    gabi::store<u32>(data + 0x48, 0);
  }
  if (actor->mHasPearl)
    entryBck(0x7F0, 0x984, 0x988);
  return TRUE;
}
VERIFY(0x02335CA4, Draw);
} // namespace daObjDoguu
