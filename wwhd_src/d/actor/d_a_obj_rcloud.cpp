/* Dragon Roost clouds. GC reconstruction corrected against WWHD.
 */
#include "d/actor/d_a_obj_rcloud.h"
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
static u8 *resource(s32 index) {
  gabi::Local<be<u32>[2]> name;
  (*name)[0] = 0x1002EC44;
  (*name)[1] = 0x1002EC5C;
  return gabi::call<u8 *>(0x026066C4, ptr(gabi::load<u32>(0x101F4F28)),
                          name.get(), index);
}
BOOL daObjRcloud_c::create_heap() {
  WWHD_FUNC(0x0238724C, BOOL, this);
  u8 *modelData = resource(4), *animation = resource(7);
  if (!modelData || !animation) {
    gabi::call(0x0273AA24, ptr(0x1002EC8C), 0xDE, ptr(0x1002EC88));
    return 0;
  }
  mpModel = gabi::call<u8 *>(0x025E38E0, modelData, 0x80000, 0x11000222);
  s32 result = gabi::call<s32>(0x025E7CE0, mBtk, modelData, animation, 1, 2,
                               gabi::load<f32>(0x1002EC84), 0, -1, 0, 0);
  return mpModel != nullptr && result != 0;
}
VERIFY(0x0238724C, &daObjRcloud_c::create_heap);
BOOL rcloudHeap(daObjRcloud_c *self) {
  WWHD_FUNC(0x02387358, BOOL, self);
  return self->create_heap();
}
VERIFY(0x02387358, rcloudHeap);
void daObjRcloud_c::init_mtx() {
  WWHD_FUNC(0x0238735C, void, this);
  f32 x = scale.x, y = scale.y;
  u32 model = gabi::ea((u8 *)mpModel);
  f32 z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  model = gabi::ea((u8 *)mpModel);
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
}
VERIFY(0x0238735C, &daObjRcloud_c::init_mtx);
void daObjRcloud_c::setup_action(s32 action) {
  WWHD_FUNC(0x02387428, void, this, action);
  u32 table = 0x101CC78C + (u32)action * 8;
  mActDescriptor[0] = gabi::load<u32>(table);
  u32 second = gabi::load<u32>(table + 4);
  mAction = action;
  mActDescriptor[1] = second;
}
VERIFY(0x02387428, &daObjRcloud_c::setup_action);
static u32 rcloudParam(daObjRcloud_c *self, u32 width, u32 shift);
s32 daObjRcloud_c::_create() {
  WWHD_FUNC(0x02387448, s32, this);
  u32 a = gabi::ea(this), flags = gabi::load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(a + 0xB4, 0x1002EC74);
      gabi::call(0x025E7C6C, mBtk);
      flags = gabi::load<u32>(a + 0x2E4);
    }
    gabi::store<u32>(a + 0x2E4, flags | 8);
  }
  s32 phase = 5;
  u32 name;
  if (!gabi::load<u8>(a + 0xC)) {
    name = rcloudParam(this, 1, 0);
    mDemoName = name;
  } else
    name = mDemoName;
  if (name > 1)
    return phase;
  u32 save = gabi::load<u32>(0x101F84DC);
  s32 done = name ? gabi::call<s32>(0x025B7D90, ptr(save + 0xD4), 0)
                  : gabi::call<s32>(0x025B8B94, ptr(save + 0x644), 0x3908);
  if (!done) {
    phase = gabi::call<s32>(0x02520460, mPhase, ptr(0x1002EC44));
    mLoaded = 1;
    if (phase == 4) {
      if (gabi::call<s32>(0x025D63E8, this, ptr(0x02387358), 0x5A0)) {
        u32 model = gabi::ea((u8 *)mpModel);
        gabi::store<u32>(a + 0x348, model ? model + 0xC8 : 0);
        init_mtx();
        setup_action(0);
      } else
        phase = 5;
    }
  }
  return phase;
}
VERIFY(0x02387448, &daObjRcloud_c::_create);
s32 rcloudCreate(daObjRcloud_c *self) {
  WWHD_FUNC(0x023875BC, s32, self);
  return self->_create();
}
VERIFY(0x023875BC, rcloudCreate);
BOOL daObjRcloud_c::_delete() {
  WWHD_FUNC(0x023875C0, BOOL, this);
  if (mLoaded)
    gabi::call(0x025204C8, mPhase, ptr(0x1002EC44));
  return 1;
}
VERIFY(0x023875C0, &daObjRcloud_c::_delete);
BOOL rcloudDelete(daObjRcloud_c *self) {
  WWHD_FUNC(0x023875FC, BOOL, self);
  return self->_delete();
}
VERIFY(0x023875FC, rcloudDelete);
BOOL daObjRcloud_c::_execute() {
  WWHD_FUNC(0x02387600, BOOL, this);
  gabi::call(0x025E742C, mBtk);
  u32 a = gabi::ea(this);
  s16 index = gabi::load<s16>(a + 0x42E), delta = gabi::load<s16>(a + 0x42C);
  u32 self = a + (s32)delta, target;
  if (index < 0)
    target = gabi::load<u32>(a + 0x430);
  else {
    s16 offset = gabi::load<s16>(a + 0x432);
    u32 vtable = gabi::load<u32>(self + (s32)offset);
    target = gabi::load<u32>(vtable + (s32)index * 8 + 4);
  }
  gabi::call(target, ptr(self));
  return 1;
}
VERIFY(0x02387600, &daObjRcloud_c::_execute);
BOOL rcloudExecute(daObjRcloud_c *self) {
  WWHD_FUNC(0x02387678, BOOL, self);
  return self->_execute();
}
VERIFY(0x02387678, rcloudExecute);
void daObjRcloud_c::setTexMtx() {
  WWHD_FUNC(0x0238767C, void, this);
  u32 model = gabi::ea((u8 *)mpModel), data = gabi::load<u32>(model + 0xAC);
  u16 i = 0;
  u32 header = gabi::call<u32>(0x027F3F8C, ptr(data));
  u16 count = gabi::load<u16>(header + 0x24);
  while (i < count) {
    u32 materialCount = gabi::load<u32>(data + 0xC),
        material = gabi::load<u32>(data + 0x10);
    if ((u32)i < materialCount)
      material += (u32)i * 0x39C;
    u32 materialData = gabi::load<u32>(material);
    u16 materialIndex = gabi::load<u16>(materialData + 0xC);
    model = gabi::ea((u8 *)mpModel);
    u32 packets = gabi::load<u32>(model + 0x34),
        packet = packets + (u32)materialIndex * 0x3C;
    for (u32 tex = 0; tex < 8; tex++) {
      u32 matrix = gabi::call<u32>(0x027FA7F8, ptr(packet), tex);
      if (matrix) {
        f32 x = gabi::load<f32>(matrix + 0x10);
        gabi::store<f32>(matrix + 0x10, x + x);
        if (mAction == 2) {
          f32 y = gabi::load<f32>(matrix + 0x14), progress = mProgress;
          gabi::store<f32>(matrix + 0x14, y + progress);
        }
      }
    }
    i = (u16)(i + 1);
    header = gabi::call<u32>(0x027F3F8C, ptr(data));
    count = gabi::load<u16>(header + 0x24);
  }
}
VERIFY(0x0238767C, &daObjRcloud_c::setTexMtx);
BOOL daObjRcloud_c::_draw() {
  WWHD_FUNC(0x02387764, BOOL, this);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 1, &current.pos,
             ptr(gabi::ea(this) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (u8 *)mpModel, ptr(gabi::ea(this) + 0x110));
  gabi::call(0x025E8048, mBtk, (u8 *)mpModel,
             gabi::load<f32>(gabi::ea(this) + 0x3BC));
  setTexMtx();
  gabi::call(0x025E2DE0, (u8 *)mpModel, 0);
  return 1;
}
VERIFY(0x02387764, &daObjRcloud_c::_draw);
BOOL rcloudDraw(daObjRcloud_c *self) {
  WWHD_FUNC(0x023877D8, BOOL, self);
  return self->_draw();
}
VERIFY(0x023877D8, rcloudDraw);
BOOL rcloudIsDelete(daObjRcloud_c *self) {
  WWHD_FUNC(0x023877DC, BOOL, self);
  return 1;
}
VERIFY(0x023877DC, rcloudIsDelete);
void daObjRcloud_c::wait_act_proc() {
  WWHD_FUNC(0x023877E4, void, this);
  u32 play = gabi::call<u32>(0x025200D4);
  if (gabi::load<u8>(play + 0x5292)) {
    u32 name = gabi::load<u32>(0x101CC7C8 + (u32)mDemoName * 4);
    play = gabi::call<u32>(0x025200D4);
    if (gabi::call<s32>(0x025445B8, ptr(play + 0x52C4), ptr(name)))
      setup_action(1);
  }
}
VERIFY(0x023877E4, &daObjRcloud_c::wait_act_proc);
void daObjRcloud_c::clouds_lift_start_wait_act_proc() {
  WWHD_FUNC(0x0238785C, void, this);
  f32 frame = (f32)gabi::load<u32>(0x101D6008);
  if (frame > gabi::load<f32>(0x1002ECA8))
    setup_action(2);
}
VERIFY(0x0238785C, &daObjRcloud_c::clouds_lift_start_wait_act_proc);
void daObjRcloud_c::clouds_lift_act_proc() {
  WWHD_FUNC(0x023878B8, void, this);
  f32 progress = (f32)mProgress + gabi::load<f32>(0x1046BD28),
      end = gabi::load<f32>(0x1002ECAC);
  if (progress < end) {
    mProgress = end;
    u32 save = gabi::load<u32>(0x101F84DC);
    gabi::call(0x025B8B68, ptr(save + 0x644), 0x3908);
    gabi::call(0x025D57E0, this);
  } else
    mProgress = progress;
}
VERIFY(0x023878B8, &daObjRcloud_c::clouds_lift_act_proc);
static void rcloudStaticInit() {
  WWHD_FUNC(0x02387938, void);
  for (u32 i = 0; i < 4; i++)
    gabi::store<u32>(0x1046BD30 + i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CC7A4));
  gabi::store<f32>(0x1046BD1C, gabi::load<f32>(0x1002ECB0));
  gabi::store<f32>(0x1046BD20, gabi::load<f32>(0x1002ECB4));
  gabi::call(0x028ED6F8, ptr(0x1046BD2C));
  gabi::call(0x028F026C, ptr(0x101CC7B0));
  gabi::call(0x028EAB2C, ptr(0x1046BD2D));
  gabi::call(0x028F026C, ptr(0x101CC7BC));
  gabi::store<f32>(0x1046BD24, gabi::load<f32>(0x1002ECB8));
  gabi::store<f32>(0x1046BD28, gabi::load<f32>(0x1002ECBC));
}
VERIFY(0x02387938, rcloudStaticInit);
static void rcloudSimpleDelete(u8 *self, s32 flags) {
  WWHD_FUNC(0x023879EC, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x023879EC, rcloudSimpleDelete);
static void rcloudDestructor(daObjRcloud_c *self, s32 flags) {
  WWHD_FUNC(0x02387A00, void, self, flags);
  if (self) {
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x02387A00, rcloudDestructor);
static void rcloudEmpty() { WWHD_FUNC(0x02387A54, void); }
VERIFY(0x02387A54, rcloudEmpty);
static u32 rcloudParam(daObjRcloud_c *self, u32 width, u32 shift) {
  WWHD_FUNC(0x02387A58, u32, self, width, shift);
  u32 p = gabi::load<u32>(gabi::ea(self) + 0xB0), bit = (width & 32) ? 0 : 1u << (width & 31),
      mask = bit - 1;
  u32 shifted = shift & 32 ? 0 : p >> (shift & 31);
  return shifted & mask;
}
VERIFY(0x02387A58, rcloudParam);
