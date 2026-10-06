/* Rat-operated trapdoor. GameCube structure corrected against WWHD.
 */
#include "d/actor/d_a_obj_pfall.h"
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
static u32 eventManager() { return gabi::call<u32>(0x025200D4) + 0x52C4; }
static void cutEnd(s32 staff) {
  gabi::call(0x02543280, ptr(eventManager()), staff);
}
void daObj_Pfall_c::mode_wait_init() {
  WWHD_FUNC(0x023830CC, void, this);
  s16 wait = gabi::load<s16>(0x1046BC10);
  mMode = 0;
  mWaitTimer = wait;
}
VERIFY(0x023830CC, &daObj_Pfall_c::mode_wait_init);
void daObj_Pfall_c::CreateInit() {
  WWHD_FUNC(0x023830E4, void, this);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, ptr(play + 0x12A0), (u8 *)mpBgLeft, this);
  play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, ptr(play + 0x12A0), (u8 *)mpBgRight, this);
  set_mtx();
  mAnm = 1;
  mode_wait_init();
}
VERIFY(0x023830E4, &daObj_Pfall_c::CreateInit);
s32 daObj_Pfall_c::_create() {
  WWHD_FUNC(0x0238314C, s32, this);
  u32 a = gabi::ea(this), flags = gabi::load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(a + 0xB4, 0x1002E674);
      gabi::call(0x025EB82C, mLine);
      flags = gabi::load<u32>(a + 0x2E4);
    }
    gabi::store<u32>(a + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, mPhase, ptr(0x1002E714));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, ptr(0x023830C8), 0x38E0))
      return 5;
    CreateInit();
  }
  return phase;
}
VERIFY(0x0238314C, &daObj_Pfall_c::_create);
s32 pfallCreate(daObj_Pfall_c *self) {
  WWHD_FUNC(0x02383218, s32, self);
  return self->_create();
}
VERIFY(0x02383218, pfallCreate);
BOOL daObj_Pfall_c::_delete() {
  WWHD_FUNC(0x0238321C, BOOL, this);
  gabi::call(0x025204C8, mPhase, ptr(0x1002E71C));
  u32 bg = gabi::ea((u8 *)mpBgLeft);
  if (bg && gabi::load<u32>(bg) < 0x100) {
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(p + 0x12A0), (u8 *)mpBgLeft);
  }
  bg = gabi::ea((u8 *)mpBgRight);
  if (bg && gabi::load<u32>(bg) < 0x100) {
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(p + 0x12A0), (u8 *)mpBgRight);
  }
  return 1;
}
VERIFY(0x0238321C, &daObj_Pfall_c::_delete);
BOOL pfallDelete(daObj_Pfall_c *self) {
  WWHD_FUNC(0x023832A8, BOOL, self);
  return self->_delete();
}
VERIFY(0x023832A8, pfallDelete);
void daObj_Pfall_c::cutWaitStart(s32 staff) {
  WWHD_FUNC(0x023832AC, void, this, staff);
  u8 *timer = gabi::call<u8 *>(0x0254487C, ptr(eventManager()), staff,
                               ptr(0x1002E724), 3);
  mTimer = timer ? gabi::load<s16>(gabi::ea(timer) + 2) : 0;
}
VERIFY(0x023832AC, &daObj_Pfall_c::cutWaitStart);
void daObj_Pfall_c::cutOpenStart(s32 staff) {
  WWHD_FUNC(0x02383310, void, this, staff);
  s8 room = gabi::load<s8>(gabi::ea(this) + 0x326);
  mOpenAngle = 0;
  mOpenStep = 0;
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, 0x699C, ptr(gabi::ea(this) + 0x37C), 0, reverb);
}
VERIFY(0x02383310, &daObj_Pfall_c::cutOpenStart);
void daObj_Pfall_c::cutHikuStart(s32 staff) {
  WWHD_FUNC(0x0238336C, void, this, staff);
  mAnm = 2;
}
VERIFY(0x0238336C, &daObj_Pfall_c::cutHikuStart);
void daObj_Pfall_c::cutWaitProc(s32 staff) {
  WWHD_FUNC(0x02383378, void, this, staff);
  if (!gabi::call<s32>(0x0211D2F8, &mTimer)) {
    if (!mLaughed) {
      s8 room = gabi::load<s8>(gabi::ea(this) + 0x326);
      mLaughed = 1;
      s32 reverb = gabi::call<s32>(0x02520540, room);
      gabi::call(0x025E1AA4, 0x48A4, ptr(gabi::ea(this) + 0x37C),
                 gabi::load<u32>(gabi::ea(this) + 4), 0, reverb);
    }
    cutEnd(staff);
  }
}
VERIFY(0x02383378, &daObj_Pfall_c::cutWaitProc);
void daObj_Pfall_c::cutHikuProc(s32 staff) {
  WWHD_FUNC(0x02383510, void, this, staff);
  if (mAnm == 2) {
    u32 morph = gabi::ea((u8 *)mpMorf);
    if ((gabi::load<u8>(morph + 0xA7) & 1) ||
        gabi::load<f32>(morph + 0x98) == 0.0f) {
      cutEnd(staff);
      mAnm = 1;
    }
  }
}
VERIFY(0x02383510, &daObj_Pfall_c::cutHikuProc);
void daObj_Pfall_c::cutProc() {
  WWHD_FUNC(0x02383590, void, this);
  s32 staff =
      gabi::call<s32>(0x02542D88, ptr(eventManager()), ptr(0x1002E738), 0, 0);
  if (staff == -1)
    return;
  s32 action = gabi::call<s32>(0x02542EDC, ptr(eventManager()), staff,
                               ptr(0x101CC3C8), 3, 0, 0);
  u32 manager = eventManager();
  if (action == -1) {
    gabi::call(0x02543280, ptr(manager), staff);
    return;
  }
  if (gabi::call<s32>(0x025447C8, ptr(manager), staff)) {
    switch (action) {
    case 0:
      cutWaitStart(staff);
      break;
    case 1:
      cutOpenStart(staff);
      break;
    case 2:
      cutHikuStart(staff);
      break;
    }
  }
  switch (action) {
  case 0:
    cutWaitProc(staff);
    break;
  case 1:
    cutOpenProc(staff);
    break;
  case 2:
    cutHikuProc(staff);
    break;
  }
}
VERIFY(0x02383590, &daObj_Pfall_c::cutProc);
void daObj_Pfall_c::mode_proc_call() {
  WWHD_FUNC(0x02383748, void, this);
  u32 table = 0x1002E758 + (u32)mMode * 8;
  s16 index = gabi::load<s16>(table + 2), delta = gabi::load<s16>(table);
  u32 self = gabi::ea(this) + (s32)delta, target;
  if (index < 0)
    target = gabi::load<u32>(table + 4);
  else {
    s16 offset = gabi::load<s16>(table + 6);
    u32 vtable = gabi::load<u32>(self + (s32)offset);
    target = gabi::load<u32>(vtable + (s32)index * 8 + 4);
  }
  gabi::call(target, ptr(self));
}
VERIFY(0x02383748, &daObj_Pfall_c::mode_proc_call);
void daObj_Pfall_c::setAnm() {
  WWHD_FUNC(0x02383794, void, this);
  gabi::call(0x02587A0C, ptr(0x1002E770), (u8 *)mpMorf, &mBckIdx, &mAnm,
             &mOldAnm, ptr(0x1002E768), ptr(0x1002E778), 0);
}
VERIFY(0x02383794, &daObj_Pfall_c::setAnm);
void daObj_Pfall_c::mode_event_init() {
  WWHD_FUNC(0x02383D18, void, this);
  mMode = 1;
}
VERIFY(0x02383D18, &daObj_Pfall_c::mode_event_init);
void daObj_Pfall_c::mode_wait() {
  WWHD_FUNC(0x02383D24, void, this);
  gabi::Local<cXyz> position;
  position->x = current.pos.x;
  position->y = current.pos.y;
  position->z = current.pos.z;
  if (gabi::call<s32>(0x025881A4, position.get(), 100.0f, 100.0f)) {
    u32 play = gabi::call<u32>(0x025200D4),
        player = gabi::load<u32>(play + 0x5B2C);
    if (gabi::load<f32>(player + 0x370) == 0.0f &&
        !gabi::call<s32>(0x0211D2F8, &mWaitTimer))
      mode_event_init();
  }
  gabi::call(0x024F43DC, (u8 *)mpBgLeft);
  gabi::call(0x024F43DC, (u8 *)mpBgRight);
}
VERIFY(0x02383D24, &daObj_Pfall_c::mode_wait);
void daObj_Pfall_c::mode_event() {
  WWHD_FUNC(0x02383DC4, void, this);
  if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2) {
    if (gabi::call<s32>(0x0254457C, ptr(eventManager()), ptr(0x1002E7E4))) {
      if (gabi::load<u8>(0x1046BC0A))
        mode_wait_init();
      else
        gabi::call(0x0252012C, ptr(0x1002E7E0), 15, 11, -1, 0.0f, 0, 1, 0);
    }
  } else
    gabi::call(0x025D77DC, this, ptr(0x1002E7E4), 1, 0xFFFF);
  gabi::call(0x024F43DC, (u8 *)mpBgLeft);
  gabi::call(0x024F43DC, (u8 *)mpBgRight);
}
VERIFY(0x02383DC4, &daObj_Pfall_c::mode_event);
static daObj_PfallHIO_c *pfallHioConstruct(daObj_PfallHIO_c *self) {
  WWHD_FUNC(0x02383EB0, daObj_PfallHIO_c *, self);
  if (!self)
    self = gabi::call<daObj_PfallHIO_c *>(0x0273AD10, 0x10);
  if (self) {
    self->mVtable = 0x1002E684;
    self->mRopeLift = gabi::load<f32>(0x1002E7B4);
    self->mWait = 0;
    self->mRepeat = 0;
    self->mNo = -1;
    self->mDebug = 0;
  }
  return self;
}
VERIFY(0x02383EB0, pfallHioConstruct);
static void pfallSimpleDelete(u8 *self, s32 flags) {
  WWHD_FUNC(0x02383FC0, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x02383FC0, pfallSimpleDelete);
BOOL pfallIsDelete(daObj_Pfall_c *self) {
  WWHD_FUNC(0x02383FD4, BOOL, self);
  return 1;
}
VERIFY(0x02383FD4, pfallIsDelete);
static void pfallDestructor(daObj_Pfall_c *self, s32 flags) {
  WWHD_FUNC(0x02383FDC, void, self, flags);
  if (self) {
    gabi::call(0x025EB8B8, self->mLine, 2);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x02383FDC, pfallDestructor);
static void pfallHioDelete(daObj_PfallHIO_c *self, s32 flags) {
  WWHD_FUNC(0x0238403C, void, self, flags);
  if (self) {
    self->mNo = -1;
    self->mVtable = 0x1002E684;
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x0238403C, pfallHioDelete);
static void pfallEmpty(u8 *self) { WWHD_FUNC(0x02384064, void, self); }
VERIFY(0x02384064, pfallEmpty);
static void modelMatrix(u32 model) {
  f32 m[12];
  for (u32 i = 0; i < 12; i++)
    m[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, m[i]);
}
static void transOffset(u32 a) {
  gabi::call(0x025F24E0, gabi::load<f32>(a), gabi::load<f32>(a + 4),
             gabi::load<f32>(a + 8));
}
void daObj_Pfall_c::set_mtx() {
  WWHD_FUNC(0x0238299C, void, this);
  f32 zero = gabi::load<f32>(0x1002E650);
  if (!gabi::load<u32>(0x1046BC58)) {
    gabi::store<f32>(0x1046BC28, gabi::load<f32>(0x1002E694));
    gabi::store<f32>(0x1046BC30, gabi::load<f32>(0x1002E698));
    gabi::store<u32>(0x1046BC58, 1);
    gabi::store<f32>(0x1046BC2C, zero);
  }
  if (!gabi::load<u32>(0x1046BC5C)) {
    gabi::store<f32>(0x1046BC34, gabi::load<f32>(0x1002E69C));
    gabi::store<f32>(0x1046BC3C, gabi::load<f32>(0x1002E6A0));
    gabi::store<u32>(0x1046BC5C, 1);
    gabi::store<f32>(0x1046BC38, zero);
  }
  u32 model = gabi::ea((u8 *)mpDoorLeft);
  f32 x = scale.x, y = scale.y, z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC4, z);
  gabi::store<f32>(model + 0xC0, y);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  gabi::call(0x025F1C28, ptr(0x1048D0CC),
             gabi::load<s16>(gabi::ea(this) + 0x322));
  transOffset(0x1046BC28);
  f32 down = gabi::load<f32>(0x1002E6A4);
  gabi::call(0x025F24E0, zero, down, zero);
  gabi::call(0x025F1C5C, ptr(0x1048D0CC), (s16)mOpenAngle);
  f32 up = gabi::load<f32>(0x1002E6A8);
  gabi::call(0x025F24E0, zero, up, zero);
  modelMatrix(gabi::ea((u8 *)mpDoorLeft));
  gabi::call(0x028E90D4, ptr(0x1048D0CC), mLeftMatrix);
  x = scale.x;
  y = scale.y;
  model = gabi::ea((u8 *)mpDoorRight);
  z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  gabi::call(0x025F1C28, ptr(0x1048D0CC),
             gabi::load<s16>(gabi::ea(this) + 0x322));
  transOffset(0x1046BC34);
  gabi::call(0x025F1C28, ptr(0x1048D0CC), -0x8000);
  gabi::call(0x025F24E0, zero, down, zero);
  gabi::call(0x025F1C5C, ptr(0x1048D0CC), (s16)mOpenAngle);
  gabi::call(0x025F24E0, zero, up, zero);
  modelMatrix(gabi::ea((u8 *)mpDoorRight));
  gabi::call(0x028E90D4, ptr(0x1048D0CC), mRightMatrix);
  u32 morph = gabi::ea((u8 *)mpMorf);
  model = gabi::load<u32>(morph + 0x90);
  if (!gabi::load<u32>(0x1046BC60)) {
    gabi::store<f32>(0x1046BC44, zero);
    gabi::store<u32>(0x1046BC60, 1);
    gabi::store<f32>(0x1046BC40, zero);
    gabi::store<f32>(0x1046BC48, gabi::load<f32>(0x1002E6AC));
  }
  if (!gabi::load<u32>(0x1046BC64)) {
    f32 half = gabi::load<f32>(0x1002E6B0);
    gabi::store<u32>(0x1046BC64, 1);
    gabi::store<f32>(0x1046BC54, half);
    gabi::store<f32>(0x1046BC4C, half);
    gabi::store<f32>(0x1046BC50, half);
  }
  x = gabi::load<f32>(0x1046BC4C);
  y = gabi::load<f32>(0x1046BC50);
  z = gabi::load<f32>(0x1046BC54);
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC4, z);
  gabi::store<f32>(model + 0xC0, y);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  gabi::call(0x025F1C28, ptr(0x1048D0CC),
             gabi::load<s16>(gabi::ea(this) + 0x322));
  transOffset(0x1046BC40);
  modelMatrix(model);
  f32 lift = gabi::load<f32>(0x1046BC0C);
  f32 offset = mRopeOffset;
  gabi::call(0x025F24E0, zero, offset + lift, zero);
  gabi::call(0x025F1C28, ptr(0x1048D0CC), -0x8000);
  modelMatrix(gabi::ea((u8 *)mpRope));
}
VERIFY(0x0238299C, &daObj_Pfall_c::set_mtx);
static u8 *pfallResource(s32 index) {
  gabi::Local<be<u32>[2]> name;
  (*name)[0] = 0x1002E6B8;
  (*name)[1] = 0x1002E65C;
  return gabi::call<u8 *>(0x026066C4, ptr(gabi::load<u32>(0x101F4F28)),
                          name.get(), index);
}
static void resourceAssert(u8 *data, s32 line, u32 expression) {
  if (!data)
    gabi::call(0x0273AA24, ptr(0x1002E6C0), line, ptr(expression));
}
BOOL daObj_Pfall_c::CreateHeap() {
  WWHD_FUNC(0x02382E64, BOOL, this);
  u8 *data = pfallResource(9);
  resourceAssert(data, 0xD1, 0x1002E6D4);
  mpDoorLeft = gabi::call<u8 *>(0x025E38E0, data, 0, 0x11020203);
  mpDoorRight = gabi::call<u8 *>(0x025E38E0, data, 0, 0x11020203);
  if (mpDoorLeft == nullptr || mpDoorRight == nullptr)
    return 0;
  data = pfallResource(11);
  resourceAssert(data, 0xD9, 0x1002E6E8);
  mpRope = gabi::call<u8 *>(0x025E38E0, data, 0, 0x11020203);
  if (mpRope == nullptr)
    return 0;
  data = pfallResource(10);
  resourceAssert(data, 0xE0, 0x1002E700);
  mpMorf = gabi::call<u8 *>(0x025E4F64, (u8 *)nullptr, data, 0, 0, 0, -1,
                            gabi::load<f32>(0x1002E6B4), 0, -1, 1, 0, 0x80000,
                            0x11000002);
  u32 morph = gabi::ea((u8 *)mpMorf);
  if (!morph || !gabi::load<u32>(morph + 0x90))
    return 0;
  gabi::store<u32>(gabi::load<u32>(morph + 0x90) + 0xB8, gabi::ea(this));
  set_mtx();
  gabi::call(0x025E55A0, (u8 *)mpMorf);
  mpBgLeft = gabi::call<u8 *>(0x024F23F4, (u8 *)nullptr);
  if (mpBgLeft == nullptr)
    return 0;
  data = pfallResource(14);
  if (gabi::call<s32>(0x0200A030, (u8 *)mpBgLeft, data, 1, mLeftMatrix))
    return 0;
  mpBgRight = gabi::call<u8 *>(0x024F23F4, (u8 *)nullptr);
  if (mpBgRight == nullptr)
    return 0;
  data = pfallResource(14);
  return gabi::call<s32>(0x0200A030, (u8 *)mpBgRight, data, 1, mRightMatrix) ==
         0;
}
VERIFY(0x02382E64, &daObj_Pfall_c::CreateHeap);
BOOL pfallHeap(daObj_Pfall_c *self) {
  WWHD_FUNC(0x023830C8, BOOL, self);
  return self->CreateHeap();
}
VERIFY(0x023830C8, pfallHeap);
void daObj_Pfall_c::cutOpenProc(s32 staff) {
  WWHD_FUNC(0x02383404, void, this, staff);
  s32 step = mOpenStep;
  if (step >= 7) {
    cutEnd(staff);
    return;
  }
  gabi::Local<be<s16>[7]> targets;
  (*targets)[0] = -0x3A98;
  (*targets)[1] = -0x2AF8;
  (*targets)[2] = -0x36B0;
  (*targets)[3] = -0x2EE0;
  (*targets)[4] = -0x32C8;
  (*targets)[5] = -0x30D4;
  (*targets)[6] = -0x31CE;
  s16 target = gabi::load<s16>(gabi::ea(targets.get()) + (u32)step * 2);
  s16 difference =
      gabi::call<s16>(0x0200F378, &mOpenAngle, target, 4, 0x2000, 0x100);
  s16 magnitude = (s16)(difference < 0 ? -(s32)difference : (s32)difference);
  if (magnitude <= 0x100)
    mOpenStep = (u32)mOpenStep + 1;
}
VERIFY(0x02383404, &daObj_Pfall_c::cutOpenProc);
BOOL daObj_Pfall_c::_execute() {
  WWHD_FUNC(0x023837E4, BOOL, this);
  u32 a = gabi::ea(this), z = gabi::load<u32>(a + 0x31C),
      y = gabi::load<u32>(a + 0x318), x = gabi::load<u32>(a + 0x314);
  gabi::store<u32>(a + 0x384, z);
  gabi::store<u32>(a + 0x380, y);
  gabi::store<u32>(a + 0x390, x);
  gabi::store<u32>(a + 0x398, z);
  gabi::store<u32>(a + 0x37C, x);
  gabi::store<u32>(a + 0x394, y);
  u32 play = gabi::call<u32>(0x025200D4);
  if (gabi::load<u8>(play + 0x5292))
    cutProc();
  f32 zero = gabi::load<f32>(0x1002E650);
  u32 morph;
  if (mAnm == 2) {
    morph = gabi::ea((u8 *)mpMorf);
    f32 six = gabi::load<f32>(0x1002E7A8);
    if (gabi::load<f32>(morph + 0x9C) == six) {
      s32 reverb = gabi::call<s32>(0x02520540, gabi::load<s8>(a + 0x326));
      gabi::call(0x025E1A40, 0x699B, ptr(a + 0x37C), 0, reverb);
      morph = gabi::ea((u8 *)mpMorf);
    }
    f32 frame = gabi::load<f32>(morph + 0x9C);
    if (!(frame > zero && frame <= six)) {
      f32 ten = gabi::load<f32>(0x1002E6A8);
      if (frame > six && !(frame > ten)) {
        mRopeOffset = (f32)mRopeOffset + gabi::load<f32>(0x1002E6B4);
        morph = gabi::ea((u8 *)mpMorf);
      } else {
        f32 twentyThree = gabi::load<f32>(0x1002E7AC);
        if (!(frame > ten && frame <= twentyThree) && frame > twentyThree &&
            !(frame > gabi::load<f32>(0x1002E7B0))) {
          mRopeOffset = (f32)mRopeOffset - gabi::load<f32>(0x1002E7B4);
          morph = gabi::ea((u8 *)mpMorf);
        }
      }
    }
  } else {
    morph = gabi::ea((u8 *)mpMorf);
    mRopeOffset = zero;
  }
  gabi::call(0x025E535C, ptr(morph), 0, 0, 0);
  gabi::call(0x025D6870, this, 0);
  set_mtx();
  mode_proc_call();
  setAnm();
  return 0;
}
VERIFY(0x023837E4, &daObj_Pfall_c::_execute);
BOOL pfallExecute(daObj_Pfall_c *self) {
  WWHD_FUNC(0x023839B8, BOOL, self);
  return self->_execute();
}
VERIFY(0x023839B8, pfallExecute);
static u32 namedShape(u32 data, u32 name) {
  gabi::Local<be<u32>[2]> string;
  (*string)[0] = name;
  (*string)[1] = 0x1002E65C;
  u32 names = gabi::load<u32>(data);
  pfallEmpty((u8 *)string.get());
  u32 offset = gabi::load<u32>(names + 0x18);
  s32 index = gabi::call<s32>(
      0x027DF9B0, ptr(offset ? names + 0x18 + offset : 0), ptr((*string)[0]));
  u32 material = 0;
  if (index >= 0) {
    u32 count = gabi::load<u32>(data + 0xC);
    material = gabi::load<u32>(data + 0x10);
    if ((u32)index < count)
      material += (u32)index * 0x39C;
  }
  return gabi::load<u32>(material + 8);
}
void daObj_Pfall_c::nz_draw() {
  WWHD_FUNC(0x023839BC, void, this);
  u32 morph = gabi::ea((u8 *)mpMorf), model = gabi::load<u32>(morph + 0x90),
      data = gabi::load<u32>(model + 0xAC), joint = gabi::load<u32>(data + 8);
  u32 shape = namedShape(data, 0x1002E7B8),
      shape2 = namedShape(data, 0x1002E7C4),
      shape3 = namedShape(data, 0x1002E7D0);
  gabi::store<u8>(shape + 4, 0);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 0, &current.pos,
             ptr(gabi::ea(this) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), ptr(model), ptr(gabi::ea(this) + 0x110));
  gabi::call(0x025E54D8, (u8 *)mpMorf);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D84));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D88));
  gabi::store<u8>(shape + 4, 1);
  gabi::store<u8>(shape2 + 4, 0);
  gabi::store<u8>(shape3 + 4, 0);
  gabi::call(0x027F583C, ptr(model), ptr(joint));
  gabi::store<u8>(shape2 + 4, 1);
  gabi::store<u8>(shape3 + 4, 1);
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
}
VERIFY(0x023839BC, &daObj_Pfall_c::nz_draw);
BOOL daObj_Pfall_c::_draw() {
  WWHD_FUNC(0x02383C24, BOOL, this);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 1, &current.pos,
             ptr(gabi::ea(this) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (u8 *)mpDoorLeft,
             ptr(gabi::ea(this) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (u8 *)mpDoorRight,
             ptr(gabi::ea(this) + 0x110));
  gabi::call(0x025E2DE0, (u8 *)mpDoorLeft, 0);
  gabi::call(0x025E2DE0, (u8 *)mpDoorRight, 0);
  nz_draw();
  if (gabi::load<u8>(0x1046BC09) && !gabi::load<u32>(0x101FDA48)) {
    gabi::store<u32>(0x101FDA48, 1);
    gabi::call<void *>(0xC000A848, ptr(0x101FEBEC), ptr(0x1002E654), 4);
  }
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 0, &current.pos,
             ptr(gabi::ea(this) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (u8 *)mpRope, ptr(gabi::ea(this) + 0x110));
  gabi::call(0x025E2DE0, (u8 *)mpRope, 0);
  return 1;
}
VERIFY(0x02383C24, &daObj_Pfall_c::_draw);
BOOL pfallDraw(daObj_Pfall_c *self) {
  WWHD_FUNC(0x02383D14, BOOL, self);
  return self->_draw();
}
VERIFY(0x02383D14, pfallDraw);
static void pfallStaticInit() {
  WWHD_FUNC(0x02383F14, void);
  for (u32 i = 0; i < 4; i++)
    gabi::store<u32>(0x1046BC18 + i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CC3D4));
  gabi::store<f32>(0x1046BBFC, gabi::load<f32>(0x1002E7F0));
  gabi::store<f32>(0x1046BC00, gabi::load<f32>(0x1002E7F4));
  gabi::call(0x028ED6F8, ptr(0x1046BC04));
  gabi::call(0x028F026C, ptr(0x101CC3E0));
  gabi::call(0x028EAB2C, ptr(0x1046BC05));
  gabi::call(0x028F026C, ptr(0x101CC3EC));
  pfallHioConstruct(gabi::at<daObj_PfallHIO_c>(0x1046BC08));
  gabi::call(0x028F026C, ptr(0x101CC3F8));
}
VERIFY(0x02383F14, pfallStaticInit);
