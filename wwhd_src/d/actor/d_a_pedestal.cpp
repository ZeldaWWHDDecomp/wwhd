/* WWHD tower pedestal. */
#include "d/actor/d_a_pedestal.h"
namespace daPedestal {
namespace {
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 addr(const void *p) { return gabi::ea(p); }
u32 invoke(u32 object, u32 descriptor, u32 argument) {
  s16 delta = gabi::load<s16>(descriptor),
      slot = gabi::load<s16>(descriptor + 2);
  u32 self = object + delta, target;
  if (slot < 0)
    target = gabi::load<u32>(descriptor + 4);
  else {
    s16 offset = gabi::load<s16>(descriptor + 6);
    target =
        gabi::load<u32>(gabi::load<u32>(self + offset) + u32(slot) * 8 + 4);
  }
  return gabi::call_ptr<u32>(target, gabi::at<u8>(self), argument);
}
struct Name {
  be<u32> text, vt;
};
u32 resource(Name *name, s32 index) {
  return gabi::call<u32>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         name, index);
}
void sound(daPds_c *a, u32 id) {
  s32 room = gabi::load<s8>(addr(a) + 0x326);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, id, &a->current.pos, 0, reverb);
}
} // namespace
void Callback::execute(u8 *e) {
  WWHD_FUNC(0x023C84E4, void, this, e);
  if (pos) {
    u32 p = addr((cXyz *)pos), q = addr(e);
    f32 x = gabi::load<f32>(p), z = gabi::load<f32>(p + 8),
        y = gabi::load<f32>(p + 4);
    if (gabi::load<u8>(q + 0x262) >= 7)
      y = -y;
    gabi::store<f32>(q + 0x230, y);
    gabi::store<f32>(q + 0x22C, x);
    gabi::store<f32>(q + 0x234, z);
  }
  if (angle) {
    u32 p = addr((csXyz *)angle);
    s16 x = gabi::load<s16>(p), y = gabi::load<s16>(p + 2),
        z = gabi::load<s16>(p + 4);
    gabi::call(0x028245AC, x, y, z, gabi::at<u8>(addr(e) + 0x1F0));
  }
}
VERIFY(0x023C84E4, &Callback::execute);
void Callback::end() {
  WWHD_FUNC(0x023C853C, void, this);
  if (emitter) {
    u32 e = addr((u8 *)emitter), flags = gabi::load<u32>(e + 0x254);
    gabi::store<u32>(e + 0x5C, 0xFFFFFFFF);
    gabi::store<u32>(e + 0x254, flags | 1);
    gabi::store<u32>(addr((u8 *)emitter) + 0x1E4, 0);
    emitter = nullptr;
    pos = nullptr;
    angle = nullptr;
  }
}
VERIFY(0x023C853C, &Callback::end);
void Callback::makeEmitter(u16 id, const cXyz *p, const csXyz *a,
                           const cXyz *scale) {
  WWHD_FUNC(0x023C8578, void, this, id, p, a, scale);
  end();
  u32 game = play();
  gabi::call(0x025A847C, gabi::at<u8>(gabi::load<u32>(game + 0x5AB0)), 0, id, p,
             a, scale, 255, this, -1, 0, 0, 0);
  angle = gabi::at<csXyz>(addr(a));
  pos = gabi::at<cXyz>(addr(p));
}
VERIFY(0x023C8578, &Callback::makeEmitter);
BOOL daPds_c::initBrkAnm(u8 index, BOOL force) {
  WWHD_FUNC(0x023C85F8, BOOL, this, index, force);
  u32 data = gabi::load<u32>(addr((J3DModel *)mpModel) + 0xAC);
  gabi::Local<Name> name;
  // A real GHS callee may save LR in its caller linkage area at r1+4.
  gabi::Local<be<u32>> linkage;
  name->text = 0x100343F0;
  name->vt = 0x10034318;
  u32 animation = resource(name.get(), 8);
  if (!animation)
    gabi::call(0x0273AA24, STR(0x10034348), 0x28C, STR(0x1003435C));
  u32 table = 0x101CE5DC + u32(index) * 12;
  s32 loop = gabi::load<s32>(table);
  f32 speed = gabi::load<f32>(table + 4);
  BOOL ok = gabi::call<BOOL>(0x025E8154, &mBrk, gabi::at<u8>(data),
                             gabi::at<u8>(animation), 1, loop, speed, 0, -1,
                             force, 0);
  if (!ok)
    return false;
  mAnmIndex = index;
  if (gabi::load<s32>(table + 8) < 0)
    gabi::store<f32>(addr(this) + 0x3BC,
                     f32(gabi::load<s16>(addr(this) + 0x3C2)));
  mFrame = gabi::load<f32>(addr(this) + 0x3BC);
  return true;
}
VERIFY(0x023C85F8, &daPds_c::initBrkAnm);
BOOL daPds_c::CreateHeap() {
  WWHD_FUNC(0x023C8718, BOOL, this);
  gabi::Local<Name> name;
  // A real GHS callee may save LR in its caller linkage area at r1+4.
  gabi::Local<be<u32>> linkage;
  name->text = 0x100343F0;
  name->vt = 0x10034318;
  u32 data = resource(name.get(), 5);
  if (!data)
    gabi::call(0x0273AA24, STR(0x1003436C), 0xC1, STR(0x10034380));
  mpModel = gabi::at<J3DModel>(
      gabi::call<u32>(0x025E38E0, gabi::at<u8>(data), 0, 0x11020203));
  if (!mpModel || !initBrkAnm(3, false))
    return false;
  mpBgW = gabi::at<u8>(gabi::call<u32>(0x024F23F4, 0));
  if (!mpBgW)
    return false;
  gabi::Local<Name> collision;
  gabi::Local<be<u32>> collisionLinkage;
  collision->text = 0x100343F0;
  collision->vt = 0x10034318;
  u32 dzb = resource(collision.get(), 11);
  return gabi::call<s32>(0x0200A030, (u8 *)mpBgW, gabi::at<u8>(dzb), 1,
                         &mMtx) == 0;
}
VERIFY(0x023C8718, &daPds_c::CreateHeap);
static BOOL heapCB(daPds_c *a) {
  WWHD_FUNC(0x023C8828, BOOL, a);
  return a->CreateHeap();
}
VERIFY(0x023C8828, heapCB);
BOOL daPds_c::wakeupCheck() {
  WWHD_FUNC(0x023C882C, BOOL, this);
  s8 n = gabi::load<s8>(addr(this) + 0x2DD);
  u16 bit;
  switch (n) {
  case 0:
    bit = 0x1780;
    break;
  case 1:
    bit = 0x1740;
    break;
  case 2:
    bit = 0x1720;
    break;
  default:
    return false;
  }
  return gabi::call<s32>(0x025B8B94,
                         gabi::at<u8>(gabi::load<u32>(0x101F84DC) + 0x644),
                         bit) != 0;
}
VERIFY(0x023C882C, &daPds_c::wakeupCheck);
BOOL daPds_c::finishCheck() {
  WWHD_FUNC(0x023C88CC, BOOL, this);
  s8 n = gabi::load<s8>(addr(this) + 0x2DD);
  u16 bit;
  switch (n) {
  case 0:
    bit = 0x1710;
    break;
  case 1:
    bit = 0x1704;
    break;
  case 2:
    bit = 0x1B01;
    break;
  default:
    return false;
  }
  return gabi::call<s32>(0x025B8B94,
                         gabi::at<u8>(gabi::load<u32>(0x101F84DC) + 0x644),
                         bit) != 0;
}
VERIFY(0x023C88CC, &daPds_c::finishCheck);
void daPds_c::set_mtx() {
  WWHD_FUNC(0x023C896C, void, this);
  f32 x = scale.x, y = scale.y, z = scale.z;
  u32 model = addr((J3DModel *)mpModel);
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  gabi::call(0x028E93CC, matrix, (f32)current.pos.x, (f32)current.pos.y,
             (f32)current.pos.z);
  gabi::call(0x025F1C28, matrix, (s16)current.angle.y);
  f32 values[12];
  for (u32 i = 0; i < 12; i++)
    values[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  model = addr((J3DModel *)mpModel);
  const u8 order[12] = {4, 10, 7, 11, 5, 3, 6, 1, 9, 8, 2, 0};
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + order[i] * 4, values[order[i]]);
  gabi::call(0x028E90D4, matrix, &mMtx);
}
VERIFY(0x023C896C, &daPds_c::set_mtx);
void daPds_c::CreateInit() {
  WWHD_FUNC(0x023C8A50, void, this);
  u32 model = addr((J3DModel *)mpModel);
  mType = mParameters & 255;
  gabi::store<u32>(addr(this) + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, this, -150.0f, -20.0f, -150.0f, 150.0f, 250.0f,
             150.0f);
  u8 type = mType;
  if (type == 0 && wakeupCheck()) {
    s8 n = gabi::load<s8>(addr(this) + 0x2DD);
    if (n <= 2) {
      u32 name = gabi::load<u32>(0x101CE60C + u32(s32(n)) * 4);
      u32 npc = gabi::call<u32>(0x025D9F38, STR(name), 0, 0);
      if (npc && gabi::call<f32>(0x025D6958, this, gabi::at<u8>(npc)) < 100.0f)
        gabi::store<f32>(npc + 0x318, current.pos.y);
    }
    current.pos.y = (f32)current.pos.y - 240.0f;
  } else if (type == 1 && finishCheck())
    current.pos.y = (f32)current.pos.y + 240.0f;
  set_mtx();
  u32 p = play();
  gabi::call(0x024EEA6C, gabi::at<u8>(p + 0x12A0), (u8 *)mpBgW, this);
  gabi::call(0x024F43DC, (u8 *)mpBgW);
}
VERIFY(0x023C8A50, &daPds_c::CreateInit);
s32 daPds_c::_create() {
  WWHD_FUNC(0x023C8BD8, s32, this);
  u32 flags = actor_condition;
  if (!(flags & 8)) {
    if (addr(this)) {
      gabi::call(0x025D4ED0, this);
      __vtbl = 0x10034330;
      gabi::call(0x025E80D0, &mBrk);
      flags = actor_condition;
      mGlow.vt = 0x100343F8;
    }
    actor_condition = flags | 8;
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x100343F0));
  if (phase == 4) {
    if (!gabi::call<BOOL>(0x025D63E8, this, gabi::at<u8>(0x023C8828), 0x2000)) {
      mpBgW = nullptr;
      return 5;
    }
    CreateInit();
  }
  return phase;
}
VERIFY(0x023C8BD8, &daPds_c::_create);
BOOL daPds_c::_delete() {
  WWHD_FUNC(0x023C8CB8, BOOL, this);
  if (gabi::load<u32>(addr(this) + 0xF4)) {
    u32 p = play();
    gabi::call(0x020087EC, gabi::at<u8>(p + 0x12A0), (u8 *)mpBgW);
  }
  mGlow.end();
  gabi::call(0x025204C8, &mPhase, STR(0x100343F0));
  return true;
}
VERIFY(0x023C8CB8, &daPds_c::_delete);
void daPds_c::playBrkAnm() {
  WWHD_FUNC(0x023C8D18, void, this);
  s32 result = gabi::call<s32>(0x025E742C, &mBrk);
  f32 frame = gabi::load<f32>(addr(this) + 0x3BC);
  mPlayResult = result;
  mFrame = frame;
}
VERIFY(0x023C8D18, &daPds_c::playBrkAnm);
s32 daPds_c::getMyStaffId() {
  WWHD_FUNC(0x023C8D54, s32, this);
  s8 n = gabi::load<s8>(addr(this) + 0x2DD);
  u32 name;
  switch (n) {
  case 0:
    name = 0x100343B8;
    break;
  case 1:
    name = 0x100343C0;
    break;
  case 2:
    name = 0x100343C8;
    break;
  default:
    return -1;
  }
  return gabi::call<s32>(0x02542D88, gabi::at<u8>(play() + 0x52C4), STR(name),
                         0, 0);
}
VERIFY(0x023C8D54, &daPds_c::getMyStaffId);
BOOL daPds_c::eventProc() {
  WWHD_FUNC(0x023C8E14, BOOL, this);
  s32 staff = getMyStaffId();
  u32 p = play();
  if (!gabi::load<u8>(p + 0x5292))
    return false;
  if (staff != -1) {
    s32 index = gabi::call<s32>(0x02542EDC, gabi::at<u8>(play() + 0x52C4),
                                staff, gabi::at<u8>(0x101CE5AC), 4, 1, 0);
    u32 manager = play() + 0x52C4;
    if (index == -1)
      gabi::call(0x02543280, gabi::at<u8>(manager), staff);
    else {
      BOOL advance = gabi::call<BOOL>(0x025447C8, gabi::at<u8>(manager), staff);
      u32 offset = u32(index) * 8;
      if (advance)
        invoke(addr(this), 0x101CE56C + offset, staff);
      if (invoke(addr(this), 0x101CE58C + offset, staff))
        gabi::call(0x02543280, gabi::at<u8>(play() + 0x52C4), staff);
    }
  }
  return true;
}
VERIFY(0x023C8E14, &daPds_c::eventProc);
BOOL daPds_c::setAction(Action *next, u32 argument) {
  WWHD_FUNC(0x023C8FBC, BOOL, this, next, argument);
  if ((s16)mAction.slot != 0) {
    mStatus = -1;
    invoke(addr(this), addr(&mAction), argument);
  }
  u32 first = gabi::load<u32>(addr(next));
  gabi::store<u32>(addr(&mAction), first);
  s16 slot = mAction.slot;
  u32 second = gabi::load<u32>(addr(next) + 4);
  mTimer0 = 0;
  gabi::store<u32>(addr(&mAction) + 4, second);
  mTimer3 = 0;
  mTimer2 = 0;
  mStatus = 0;
  mValue = 0.0f;
  mTimer1 = 0;
  // The newly installed descriptor may alias the actor's own action storage.
  (void)slot;
  invoke(addr(this), addr(&mAction), argument);
  return true;
}
VERIFY(0x023C8FBC, &daPds_c::setAction);
void daPds_c::action(u32 argument) {
  WWHD_FUNC(0x023C90DC, void, this, argument);
  if ((s16)mAction.slot == 0) {
    gabi::Local<Action> initial;
    gabi::Local<be<u32>> actionLinkage;
    gabi::store<u32>(addr(initial.get()), gabi::load<u32>(0x100342EC));
    gabi::store<u32>(addr(initial.get()) + 4, gabi::load<u32>(0x100342F0));
    setAction(initial.get(), 0);
  }
  invoke(addr(this), addr(&mAction), argument);
}
VERIFY(0x023C90DC, &daPds_c::action);
BOOL daPds_c::_execute() {
  WWHD_FUNC(0x023C9188, BOOL, this);
  playBrkAnm();
  if (!eventProc())
    action(0);
  set_mtx();
  gabi::call(0x024F43DC, (u8 *)mpBgW);
  if (mGlow.emitter)
    sound(this, 0x61D7);
  return true;
}
VERIFY(0x023C9188, &daPds_c::_execute);
BOOL daPds_c::_draw() {
  WWHD_FUNC(0x023C9210, BOOL, this);
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 1, &current.pos,
             gabi::at<u8>(addr(this) + 0x110));
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(light), (J3DModel *)mpModel,
             gabi::at<u8>(addr(this) + 0x110));
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play() + 0x5D70));
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play() + 0x5D74));
  u32 data = gabi::load<u32>(addr((J3DModel *)mpModel) + 0xAC);
  gabi::call(0x025E83FC, &mBrk, gabi::at<u8>(data),
             gabi::load<f32>(addr(this) + 0x3BC));
  gabi::call(0x025E2DE0, (J3DModel *)mpModel, 0);
  gabi::store<u32>(data + 0x48, 0);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play() + 0x5D78));
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play() + 0x5D7C));
  return true;
}
VERIFY(0x023C9210, &daPds_c::_draw);
void daPds_c::initialDefault(s32 id) { WWHD_FUNC(0x023C92D0, void, this, id); }
VERIFY(0x023C92D0, &daPds_c::initialDefault);
BOOL daPds_c::actionDefault(s32 id) {
  WWHD_FUNC(0x023C92D4, BOOL, this, id);
  return true;
}
VERIFY(0x023C92D4, &daPds_c::actionDefault);
void daPds_c::initialMoveEvent(s32 id) {
  WWHD_FUNC(0x023C92DC, void, this, id);
  u32 speed = gabi::call<u32>(0x0254487C, gabi::at<u8>(play() + 0x52C4), id,
                              STR(0x100343D4), 0);
  gabi::store<f32>(addr(this) + 0x340, speed ? gabi::load<f32>(speed) : 1.0f);
  u32 distance = gabi::call<u32>(0x0254487C, gabi::at<u8>(play() + 0x52C4), id,
                                 STR(0x100343DC), 0);
  f32 y = current.pos.y;
  mTargetY = distance ? y + gabi::load<f32>(distance) : y;
}
VERIFY(0x023C92DC, &daPds_c::initialMoveEvent);
BOOL daPds_c::actionMoveEvent(s32 id) {
  WWHD_FUNC(0x023C93AC, BOOL, this, id);
  f32 speed = gabi::load<f32>(addr(this) + 0x340),
      y = (f32)current.pos.y + speed, target = mTargetY;
  current.pos.y = y;
  if (speed < 0.0f) {
    if (y > target) {
      sound(this, 0x61D8);
      return false;
    }
  } else if (speed > 0.0f) {
    if (y < target) {
      sound(this, 0x61D8);
      return false;
    }
  } else {
    current.pos.y = target;
    sound(this, 0x69D9);
    return true;
  }
  current.pos.y = target;
  gabi::store<f32>(addr(this) + 0x340, 0.0f);
  sound(this, 0x69D9);
  return true;
}
VERIFY(0x023C93AC, &daPds_c::actionMoveEvent);
void daPds_c::initialEffectSet(s32 id) {
  WWHD_FUNC(0x023C94A4, void, this, id);
  mGlow.makeEmitter(0x826D, &current.pos, &shape_angle, nullptr);
  initBrkAnm(0, true);
}
VERIFY(0x023C94A4, &daPds_c::initialEffectSet);
void daPds_c::initialEffectEnd(s32 id) {
  WWHD_FUNC(0x023C94F8, void, this, id);
  mGlow.end();
  initBrkAnm(2, true);
}
VERIFY(0x023C94F8, &daPds_c::initialEffectEnd);
BOOL daPds_c::waitAction(u32 argument) {
  WWHD_FUNC(0x023C9530, BOOL, this, argument);
  if (mStatus == 0)
    mStatus = 1;
  return true;
}
VERIFY(0x023C9530, &daPds_c::waitAction);
static s32 create(daPds_c *a) {
  WWHD_FUNC(0x023C954C, s32, a);
  return a->_create();
}
VERIFY(0x023C954C, create);
static BOOL destroy(daPds_c *a) {
  WWHD_FUNC(0x023C9550, BOOL, a);
  return a->_delete();
}
VERIFY(0x023C9550, destroy);
static BOOL draw(daPds_c *a) {
  WWHD_FUNC(0x023C9554, BOOL, a);
  return a->_draw();
}
VERIFY(0x023C9554, draw);
static BOOL execute(daPds_c *a) {
  WWHD_FUNC(0x023C9558, BOOL, a);
  return a->_execute();
}
VERIFY(0x023C9558, execute);
static void initialize() {
  WWHD_FUNC(0x023C955C, void);
  gabi::store<u32>(0x1046CC14, 0);
  gabi::store<u32>(0x1046CC0C, 0);
  gabi::store<u32>(0x1046CC18, 0);
  gabi::store<u32>(0x1046CC10, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101CE618));
  f32 min = gabi::load<f32>(0x100343E4), max = gabi::load<f32>(0x100343E8);
  gabi::store<f32>(0x1046CC00, min);
  gabi::store<f32>(0x1046CC04, max);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x1046CC08));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CE624));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x1046CC09));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CE630));
}
VERIFY(0x023C955C, initialize);
static void stringDestroy(u8 *p, u32 flags) {
  WWHD_FUNC(0x023C95F0, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x023C95F0, stringDestroy);
static void callbackHook0(Callback *p) { WWHD_FUNC(0x023C9604, void, p); }
VERIFY(0x023C9604, callbackHook0);
static void callbackHook1(Callback *p) { WWHD_FUNC(0x023C9608, void, p); }
VERIFY(0x023C9608, callbackHook1);
static void callbackHook2(Callback *p) { WWHD_FUNC(0x023C960C, void, p); }
VERIFY(0x023C960C, callbackHook2);
static void stringTerminate(u8 *p) { WWHD_FUNC(0x023C9610, void, p); }
VERIFY(0x023C9610, stringTerminate);
void Callback::setup(u8 *p) {
  WWHD_FUNC(0x023C9614, void, this, p);
  emitter = p;
}
VERIFY(0x023C9614, &Callback::setup);
static void actorDestroy(daPds_c *p, u32 flags) {
  WWHD_FUNC(0x023C961C, void, p, flags);
  if (p) {
    gabi::call(0x025D50BC, p, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, p);
  }
}
VERIFY(0x023C961C, actorDestroy);
static BOOL isDelete(daPds_c *p) {
  WWHD_FUNC(0x023C9670, BOOL, p);
  return true;
}
VERIFY(0x023C9670, isDelete);
} // namespace daPedestal
