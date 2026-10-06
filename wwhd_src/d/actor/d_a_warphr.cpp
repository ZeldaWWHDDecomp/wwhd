/**
 * d_a_warphr.cpp (WWHD)
 * Warphr - warp portal to Hyrule (Ghrwp, daWarphr_c)
 *
 * Written from the WWHD code with the GameCube decompilation (zeldaret/tww src/d/actor/d_a_warphr.cpp) as
 * reference, verified against cking.rpx.
 */
#include "d/actor/d_a_warphr.h"
namespace {
struct SafeName {
  be<u32> text, vt;
};
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
static u32 eventmanager() { return play() + 0x52C4; }
static u32 saveevent() { return gabi::load<u32>(0x101F84DC) + 0x644; }
static void invalidate(u32 emitter) {
  u32 flags = gabi::load<u32>(emitter + 0x254);
  gabi::store<s32>(emitter + 0x5C, -1);
  gabi::store<u32>(emitter + 0x254, flags | 1);
}
static void matrix(daWarphr_c *actor) {
  f32 sx = actor->scale.x, sy = actor->scale.y, sz = actor->scale.z;
  u32 model = gabi::ea((J3DModel *)actor->mpModel1);
  gabi::store<f32>(model + 0xC0, sy);
  gabi::store<f32>(model + 0xBC, sx);
  gabi::store<f32>(model + 0xC4, sz);
  f32 x = actor->current.pos.x, y = actor->current.pos.y,
      z = actor->current.pos.z;
  auto *stack = gabi::at<Mtx34>(0x1048D0CC);
  gabi::call(0x028E93CC, stack, x, y, z);
  J3DModel_setBaseTRMtx(actor->mpModel1, stack);
  sx = actor->scale.x;
  sy = actor->scale.y;
  sz = actor->scale.z;
  model = gabi::ea((J3DModel *)actor->mpModel2);
  gabi::store<f32>(model + 0xC0, sy);
  gabi::store<f32>(model + 0xC4, sz);
  gabi::store<f32>(model + 0xBC, sx);
  J3DModel_setBaseTRMtx(actor->mpModel2, stack);
}
static u32 particle(daWarphr_c *actor, u32 id, u32 group = 0) {
  u32 control = gabi::load<u32>(play() + 0x5AB0);
  return gabi::call<u32>(0x025A847C, ptr(control), group, id,
                         &actor->current.pos, 0, 0, 255, 0, -1, 0, 0, 0);
}
static s32 heap_callback(daWarphr_c *actor) {
  WWHD_FUNC(0x024DA6D8, s32, actor);
  return actor->CreateHeap();
}
VERIFY(0x024DA6D8, heap_callback);
} // namespace
s32 daWarphr_c::CreateHeap() {
  WWHD_FUNC(0x024DA3FC, s32, this);
  gabi::Local<SafeName> modelA, textureA, modelB, textureB, colors;
  modelA->text = 0x1004279C;
  modelA->vt = 0x10042648;
  u32 resource = gabi::load<u32>(0x101F4F28);
  auto *data =
      gabi::call<J3DModelData *>(0x026066C4, ptr(resource), modelA.get(), 5);
  if (!data)
    JUT_ASSERT_fail(STR(0x100426AC), 0xDD, STR(0x100426BC));
  mpModel1 = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
  if (!(J3DModel *)mpModel1)
    return 0;
  textureA->vt = 0x10042648;
  mpBtkAnm1 = 0;
  resource = gabi::load<u32>(0x101F4F28);
  textureA->text = 0x1004279C;
  auto *tex = gabi::call<u8 *>(0x026066C4, ptr(resource), textureA.get(), 12);
  if (!tex)
    JUT_ASSERT_fail(STR(0x100426AC), 0xEF, STR(0x10042694));
  u32 obj = gabi::call<u32>(0x025E7C6C, 0);
  mpBtkAnm1 = obj;
  if (!obj)
    return 0;
  if (!gabi::call<s32>(0x025E7CE0, ptr(obj), data, tex, 1, 2, 1.0f, 0, -1, 0,
                       0))
    return 0;
  obj = mpBtkAnm1;
  gabi::store<f32>(obj, 1.0f);
  resource = gabi::load<u32>(0x101F4F28);
  modelB->vt = 0x10042648;
  modelB->text = 0x1004279C;
  data = gabi::call<J3DModelData *>(0x026066C4, ptr(resource), modelB.get(), 6);
  if (!data)
    JUT_ASSERT_fail(STR(0x100426AC), 0x100, STR(0x100426BC));
  mpModel2 = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
  if (!(J3DModel *)mpModel2)
    return 0;
  textureB->vt = 0x10042648;
  mpBtkAnm2 = 0;
  resource = gabi::load<u32>(0x101F4F28);
  textureB->text = 0x1004279C;
  tex = gabi::call<u8 *>(0x026066C4, ptr(resource), textureB.get(), 13);
  if (!tex)
    JUT_ASSERT_fail(STR(0x100426AC), 0x112, STR(0x10042694));
  obj = gabi::call<u32>(0x025E7C6C, 0);
  mpBtkAnm2 = obj;
  if (!obj)
    return 0;
  if (!gabi::call<s32>(0x025E7CE0, ptr(obj), data, tex, 1, 0, 1.0f, 0, -1, 0,
                       0))
    return 0;
  obj = mpBtkAnm2;
  gabi::store<f32>(obj, 0.0f);
  mpBrkAnm = 0;
  resource = gabi::load<u32>(0x101F4F28);
  colors->vt = 0x10042648;
  colors->text = 0x1004279C;
  tex = gabi::call<u8 *>(0x026066C4, ptr(resource), colors.get(), 9);
  if (!tex)
    JUT_ASSERT_fail(STR(0x100426AC), 0x127, STR(0x100426A0));
  obj = gabi::call<u32>(0x025E80D0, 0);
  mpBrkAnm = obj;
  if (!obj)
    return 0;
  if (!gabi::call<s32>(0x025E8154, ptr(obj), data, tex, 1, 0, 1.0f, 0, -1, 0,
                       0))
    return 0;
  obj = mpBrkAnm;
  gabi::store<f32>(obj, 0.0f);
  return 1;
}
VERIFY(0x024DA3FC, &daWarphr_c::CreateHeap);
void daWarphr_c::CreateInit() {
  WWHD_FUNC(0x024DA6DC, void, this);
  u32 model = gabi::ea((J3DModel *)mpModel1);
  cullMtx = model ? model + 0xC8 : 0;
  fopAcM_setCullSizeBox(this, -300, 0, -300, 300, 5000, 300);
  cullSizeFar = 1.0f;
  matrix(this);
  u32 event = gabi::call<u32>(0x025B8B94, ptr(saveevent()), 0x2D08);
  u32 manager = eventmanager();
  mEventIdx = gabi::call<s32>(0x02543F10, ptr(manager),
                              STR(event ? 0x100426DC : 0x100426EC), 255);
}
VERIFY(0x024DA6DC, &daWarphr_c::CreateInit);
s32 daWarphr_c::create() {
  WWHD_FUNC(0x024DA8C8, s32, this);
  if (!(actor_condition & 8)) {
    if (gabi::ea(this) != 0) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(gabi::ea(this) + 0xB4, 0x10042660);
    }
    actor_condition = (u32)actor_condition | 8;
  }
  mType = (u32)mParameters >> 28;
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x1004279C));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, 0x024DA6D8, 0x3000))
      return 5;
    CreateInit();
  }
  return phase;
}
VERIFY(0x024DA8C8, &daWarphr_c::create);
bool daWarphr_c::remove() {
  WWHD_FUNC(0x024DA99C, bool, this);
  u32 e = mWarpEmitter;
  if (e) {
    invalidate(e);
    mWarpEmitter = 0;
  }
  e = mProjectionEmitter;
  if (e) {
    invalidate(e);
    mProjectionEmitter = 0;
  }
  gabi::call(0x025204C8, &mPhase, STR(0x1004279C));
  return true;
}
VERIFY(0x024DA99C, &daWarphr_c::remove);
s32 daWarphr_c::get_return_count() {
  WWHD_FUNC(0x024DAC5C, s32, this);
  if (!gabi::call<u32>(0x02520A84, 2))
    return 0;
  return gabi::call<u32>(0x025B8B94, ptr(saveevent()), 0x1001) ? 2 : 1;
}
VERIFY(0x024DAC5C, &daWarphr_c::get_return_count);
s32 daWarphr_c::check_warp() {
  WWHD_FUNC(0x024DACC0, s32, this);
  gabi::Local<cXyz> delta, flat, shipdelta, shipflat;
  u32 player = gabi::load<u32>(play() + 0x5B2C);
  gabi::call(0x0201ADE0, gabi::at<cXyz>(player + 0x314), delta.get(),
             &current.pos);
  flat->y = 0.0f;
  flat->x = delta->x;
  flat->z = delta->z;
  f32 sq = gabi::call<f32>(0x028E8DD0, flat.get());
  gabi::call<f32>(0x028F4384, sq);
  if (!(gabi::load<u32>(play() + 0x5CD8) & 0x10000))
    return 0;
  if (!gabi::load<u32>(play() + 0x5B3C))
    return 0;
  u32 ship = gabi::load<u32>(play() + 0x5B3C);
  gabi::call(0x0201ADE0, gabi::at<cXyz>(ship + 0x314), shipdelta.get(),
             &current.pos);
  shipflat->x = shipdelta->x;
  shipflat->y = 0.0f;
  shipflat->z = shipdelta->z;
  sq = gabi::call<f32>(0x028E8DD0, shipflat.get());
  f32 distance = gabi::call<f32>(0x028F4384, sq);
  return distance < 500.0f;
}
VERIFY(0x024DACC0, &daWarphr_c::check_warp);
void daWarphr_c::anim_play(s32 mode) {
  WWHD_FUNC(0x024DADD0, void, this, mode);
  if (mode == 0 || mode == 1) {
    u32 anm = mpBtkAnm2;
    if (anm) {
      gabi::store<f32>(anm, 1.0f);
      gabi::call(0x025E742C, ptr(mpBtkAnm2));
    }
    anm = mpBrkAnm;
    if (anm) {
      gabi::store<f32>(anm, mode == 0 ? 1.0f : -2.0f);
      gabi::call(0x025E742C, ptr(mpBrkAnm));
    }
  } else if (mode == 2) {
    u32 anm = mpBrkAnm;
    if (anm) {
      gabi::store<f32>(anm, -2.0f);
      gabi::call(0x025E742C, ptr(mpBrkAnm));
    }
  }
  u32 anm = mpBtkAnm1;
  if (anm) {
    gabi::store<f32>(anm, 1.0f);
    gabi::call(0x025E742C, ptr(mpBtkAnm1));
  }
}
VERIFY(0x024DADD0, &daWarphr_c::anim_play);
bool daWarphr_c::normal_execute() {
  WWHD_FUNC(0x024DAEE4, bool, this);
  if (check_warp()) {
    if (!get_return_count())
      gabi::call(0x025B8B68, ptr(saveevent()), 0x3810);
    mEventState = 2;
  }
  anim_play(2);
  return true;
}
VERIFY(0x024DAEE4, &daWarphr_c::normal_execute);
void daWarphr_c::checkOrder() {
  WWHD_FUNC(0x024DAF54, void, this);
  if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2) {
    s32 idx = mEventIdx;
    u32 manager = eventmanager();
    u32 started = gabi::call<u32>(0x0254407C, ptr(manager), idx);
    idx = mEventIdx;
    if (started && mEventState != 0)
      mEventState = 0;
    manager = eventmanager();
    if (gabi::call<u32>(0x025440C8, ptr(manager), idx)) {
      u32 p = play();
      u16 flags = gabi::load<u16>(p + 0x52B8);
      gabi::store<u16>(p + 0x52B8, flags | 8);
    }
  } else if (mEventState == 0) {
    if (!gabi::load<u8>(play() + 0x5292))
      normal_execute();
  }
}
VERIFY(0x024DAF54, &daWarphr_c::checkOrder);
void daWarphr_c::eventOrder() {
  WWHD_FUNC(0x024DB1EC, void, this);
  if (mEventState == 2) {
    s32 idx = mEventIdx;
    gabi::call(0x025D7A58, this, idx, 255, 65535, 0, 1);
    gabi::store<u16>(gabi::ea(this) + 0xFA,
                     gabi::load<u16>(gabi::ea(this) + 0xFA) | 2);
  }
}
VERIFY(0x024DB1EC, &daWarphr_c::eventOrder);
bool daWarphr_c::demo_execute() {
  WWHD_FUNC(0x024DB24C, bool, this);
  u32 id = demoActorID;
  if (id == 0 || id > 32)
    return true;
  u32 obj = gabi::load<u32>(0x101D5FFC);
  if (!obj) {
    JUT_ASSERT_fail(STR(0x10042680), 0x23A, STR(0x10042670));
    obj = gabi::load<u32>(0x101D5FFC);
  }
  u32 actor = gabi::call<u32>(0x02526E70, ptr(obj), id);
  if (actor) {
    u32 shape = gabi::load<u32>(actor + 0x28);
    mShapeId = shape;
    if (shape == 0) {
      anim_play(0);
      return true;
    }
    if (shape == 1)
      anim_play(1);
  }
  return true;
}
VERIFY(0x024DB24C, &daWarphr_c::demo_execute);
void daWarphr_c::set_end_anim() {
  WWHD_FUNC(0x024DB674, void, this);
  u32 anm = mpBtkAnm2;
  if (anm) {
    s16 end = gabi::load<s16>(anm + 10);
    gabi::store<f32>(anm + 4, (f32)end);
  }
  anm = mpBrkAnm;
  if (anm) {
    s16 end = gabi::load<s16>(anm + 10);
    gabi::store<f32>(anm + 4, (f32)end);
  }
}
VERIFY(0x024DB674, &daWarphr_c::set_end_anim);
bool daWarphr_c::actWait(s32 staff) {
  WWHD_FUNC(0x024DB6F0, bool, this, staff);
  anim_play(0);
  return true;
}
VERIFY(0x024DB6F0, &daWarphr_c::actWait);
void daWarphr_c::initWarp(s32 staff) {
  WWHD_FUNC(0x024DB718, void, this, staff);
  particle(this, 0x8291);
  mWarpEmitter = particle(this, 0x8292);
  gabi::call(0x025E1988, 0x288F);
}
VERIFY(0x024DB718, &daWarphr_c::initWarp);
bool daWarphr_c::actWarp(s32 staff) {
  WWHD_FUNC(0x024DB7E0, bool, this, staff);
  anim_play(0);
  return true;
}
VERIFY(0x024DB7E0, &daWarphr_c::actWarp);
void daWarphr_c::initWarpArrive(s32 staff) {
  WWHD_FUNC(0x024DB808, void, this, staff);
  particle(this, 0x8291);
  mWarpEmitter = particle(this, 0x8292);
  set_end_anim();
  if (!gabi::load<u32>(0x1046EB8C)) {
    gabi::store<u32>(0x1046EB8C, 1);
    gabi::store<f32>(0x1046EB88, 650);
    gabi::store<f32>(0x1046EB80, -500);
    gabi::store<f32>(0x1046EB84, 0);
  }
  u32 manager = eventmanager();
  gabi::call(0x02543714, ptr(manager), gabi::at<cXyz>(0x1046EB80));
  gabi::call(0x025E1988, 0x288E);
}
VERIFY(0x024DB808, &daWarphr_c::initWarpArrive);
void daWarphr_c::initWarpArriveEnd(s32 staff) {
  WWHD_FUNC(0x024DB930, void, this, staff);
  u32 e = mWarpEmitter;
  if (e) {
    invalidate(e);
    mWarpEmitter = 0;
  }
}
VERIFY(0x024DB930, &daWarphr_c::initWarpArriveEnd);
bool daWarphr_c::actWarpArriveEnd(s32 staff) {
  WWHD_FUNC(0x024DB95C, bool, this, staff);
  anim_play(1);
  u32 anm = mpBrkAnm;
  return anm && gabi::load<f32>(anm + 4) < 0.5f;
}
VERIFY(0x024DB95C, &daWarphr_c::actWarpArriveEnd);
void daWarphr_c::initStartWarp(s32 staff) {
  WWHD_FUNC(0x024DB9C0, void, this, staff);
  u32 manager = eventmanager();
  gabi::call(0x02543714, ptr(manager), &current.pos);
}
VERIFY(0x024DB9C0, &daWarphr_c::initStartWarp);
bool daWarphr_c::actStartWarp(s32 staff) {
  WWHD_FUNC(0x024DB9F8, bool, this, staff);
  anim_play(-1);
  return true;
}
VERIFY(0x024DB9F8, &daWarphr_c::actStartWarp);
namespace {
static void static_init() {
  WWHD_FUNC(0x024DBA20, void);
  gabi::store<u32>(0x1046EB78, 0);
  gabi::store<u32>(0x1046EB70, 0);
  gabi::store<u32>(0x1046EB7C, 0);
  gabi::store<u32>(0x1046EB74, 0);
  gabi::call(0x028F026C, ptr(0x101D318C));
  gabi::store<f32>(0x1046EB64, -3.1415927410125732f);
  gabi::store<f32>(0x1046EB68, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046EB6C));
  gabi::call(0x028F026C, ptr(0x101D3198));
  gabi::call(0x028EAB2C, ptr(0x1046EB6D));
  gabi::call(0x028F026C, ptr(0x101D31A4));
}
VERIFY(0x024DBA20, static_init);
static void deleting_destructor(u8 *object, s32 flags) {
  WWHD_FUNC(0x024DBAB4, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x024DBAB4, deleting_destructor);
static bool isDelete(daWarphr_c *actor) {
  WWHD_FUNC(0x024DBAC8, bool, actor);
  return true;
}
VERIFY(0x024DBAC8, isDelete);
static s32 create_wrapper(daWarphr_c *actor) {
  WWHD_FUNC(0x024DA998, s32, actor);
  return actor->create();
}
VERIFY(0x024DA998, create_wrapper);
static bool delete_wrapper(daWarphr_c *actor) {
  WWHD_FUNC(0x024DAA18, bool, actor);
  return actor->remove();
}
VERIFY(0x024DAA18, delete_wrapper);
static bool draw_wrapper(daWarphr_c *actor) {
  WWHD_FUNC(0x024DAC58, bool, actor);
  return actor->draw();
}
VERIFY(0x024DAC58, draw_wrapper);
static bool execute_wrapper(daWarphr_c *actor) {
  WWHD_FUNC(0x024DB670, bool, actor);
  return actor->execute();
}
VERIFY(0x024DB670, execute_wrapper);
} // namespace
namespace {
static s32 invoke_action(daWarphr_c *actor, u32 descriptor) {
  s16 slot = gabi::load<s16>(descriptor + 2),
      delta = gabi::load<s16>(descriptor);
  s32 staff = actor->mStaffId;
  u32 object = gabi::ea(actor) + (s32)delta, target;
  if (slot < 0)
    target = gabi::load<u32>(descriptor + 4);
  else {
    s16 offset = gabi::load<s16>(descriptor + 6);
    u32 vt = gabi::load<u32>(object + (s32)offset);
    target = gabi::load<u32>(vt + (u32)(s32)slot * 8 + 4);
  }
  return gabi::call_ptr<s32>(target, ptr(object), staff);
}
} // namespace
void daWarphr_c::demo_proc() {
  WWHD_FUNC(0x024DB028, void, this);
  u32 manager = eventmanager();
  mStaffId = gabi::call<s32>(0x02542D88, ptr(manager), STR(0x10042720), 0, 0);
  if (!gabi::load<u8>(play() + 0x5292))
    return;
  if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1)
    return;
  s32 staff = mStaffId;
  if (staff == -1)
    return;
  manager = eventmanager();
  s32 action = gabi::call<s32>(0x02542EDC, ptr(manager), staff, ptr(0x101D3178),
                               5, 0, 0);
  staff = mStaffId;
  if (action == -1) {
    manager = eventmanager();
    gabi::call(0x02543280, ptr(manager), staff);
    return;
  }
  manager = eventmanager();
  u32 advance = gabi::call<u32>(0x025447C8, ptr(manager), staff);
  u32 offset = (u32)action * 8;
  if (advance)
    invoke_action(this, 0x101D3108 + offset);
  if (invoke_action(this, 0x101D3130 + offset)) {
    staff = mStaffId;
    manager = eventmanager();
    gabi::call(0x02543280, ptr(manager), staff);
  }
}
VERIFY(0x024DB028, &daWarphr_c::demo_proc);
bool daWarphr_c::draw() {
  WWHD_FUNC(0x024DAA1C, bool, this);
  gabi::Local<cXyz> eye, target;
  gabi::Local<csXyz> angle;
  play();
  u32 object = gabi::load<u32>(0x101D5FFC);
  if (!object) {
    JUT_ASSERT_fail(STR(0x1004270C), 0x23E, STR(0x100426FC));
    object = gabi::load<u32>(0x101D5FFC);
  }
  u32 camera = gabi::call<u32>(0x025283F8, ptr(object));
  s16 yaw;
  if (camera) {
    u32 vt = gabi::load<u32>(camera);
    gabi::call_ptr(gabi::load<u32>(vt + 0xDC), ptr(camera), eye.get());
    vt = gabi::load<u32>(camera);
    gabi::call_ptr(gabi::load<u32>(vt + 0xFC), ptr(camera), target.get());
    yaw = gabi::call<s16>(0x0200F93C, eye.get(), target.get());
  } else
    yaw = angle->y;
  /* NOTE (game test 2026-10-04): on the camera == NULL path the ORIGINAL reads uninitialised stack
   * (eye at SP+8..0x10, angle.y at SP+0x22 are never written; undefined behaviour, already in the
   * GameCube source: sp1C/sp08 stay uninitialised when dComIfGp_demo_getCamera() is NULL). The
   * candidate's gabi::Locals start zeroed, so it yields zeros (identity translation, yaw 0) where the
   * game builds mProjectionMatrix (+0x3D0) from stale stack bytes. Visual only (projection emitter). */
  f32 x = eye->x, y = eye->y, z = eye->z;
  auto *stack = gabi::at<Mtx34>(0x1048D0CC);
  gabi::call(0x028E93CC, stack, x, y, z);
  gabi::call(0x025F1C28, stack, yaw);
  gabi::call(0x028E90D4, stack, &mProjectionMatrix);
  u32 emitter = mProjectionEmitter;
  if (emitter)
    gabi::call(0x028249B0, &mProjectionMatrix, ptr(emitter + 0x1F0),
               ptr(emitter + 0x22C));
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 0, &current.pos, &tevStr);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (J3DModel *)mpModel1, &tevStr);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (J3DModel *)mpModel2, &tevStr);
  u32 anm = mpBtkAnm1;
  if (anm) {
    u32 model = gabi::ea((J3DModel *)mpModel1);
    f32 frame = gabi::load<f32>(anm + 4);
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E7FC4, ptr(anm), ptr(data), frame);
  }
  anm = mpBtkAnm2;
  if (anm) {
    u32 model = gabi::ea((J3DModel *)mpModel2);
    f32 frame = gabi::load<f32>(anm + 4);
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E7FC4, ptr(anm), ptr(data), frame);
  }
  anm = mpBrkAnm;
  if (anm) {
    u32 model = gabi::ea((J3DModel *)mpModel2);
    f32 frame = gabi::load<f32>(anm + 4);
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E83FC, ptr(anm), ptr(data), frame);
  }
  if (gabi::load<u8>(0x101F4829)) {
    u32 p = play();
    u32 list = gabi::load<u32>(p + 0x5D58);
    gabi::store<u32>(0x104B4634, list);
    p = play();
    list = gabi::load<u32>(p + 0x5D60);
    gabi::store<u32>(0x104B4638, list);
  }
  gabi::call(0x025E2DE0, (J3DModel *)mpModel1, 0);
  gabi::call(0x025E2DE0, (J3DModel *)mpModel2, 0);
  if (gabi::load<u8>(0x101F4829)) {
    u32 p = play();
    u32 list = gabi::load<u32>(p + 0x5D78);
    gabi::store<u32>(0x104B4634, list);
    p = play();
    list = gabi::load<u32>(p + 0x5D7C);
    gabi::store<u32>(0x104B4638, list);
  }
  return true;
}
VERIFY(0x024DAA1C, &daWarphr_c::draw);
bool daWarphr_c::execute() {
  WWHD_FUNC(0x024DB310, bool, this);
  if (demoActorID == 0) {
    checkOrder();
    demo_proc();
    eventOrder();
  } else {
    if (gabi::load<u8>(play() + 0x5292)) {
      u32 manager = eventmanager();
      u32 started = gabi::call<u32>(0x025445B8, ptr(manager), STR(0x10042768));
      if (started) {
        u32 emitter = mProjectionEmitter;
        if (emitter) {
          if (gabi::load<u32>(0x101D6008) >= 0x225 && !mMonotoneStarted) {
            u32 flags = gabi::load<u32>(emitter + 0x254);
            gabi::store<u32>(emitter + 0x254, flags | 1);
            gabi::call(0x025F0820);
            gabi::Local<SafeName> hyrule, stage;
            hyrule->text = 0x10042760;
            hyrule->vt = 0x10042648;
            u32 p = play();
            stage->text = p + 0x5134;
            stage->vt = 0x10042648;
            auto invoke = [](SafeName *s) {
              u32 vt = s->vt;
              gabi::call_ptr(gabi::load<u32>(vt + 0x14), s);
            };
            invoke(hyrule.get());
            invoke(hyrule.get());
            u32 left = hyrule->text;
            invoke(stage.get());
            u32 right = stage->text;
            bool equal = left == right;
            if (!equal) {
              equal = true;
              for (u32 n = 0; n < 0x40001; n++) {
                u8 a = gabi::load<u8>(left + n), b = gabi::load<u8>(right + n);
                if (a != b) {
                  equal = false;
                  break;
                }
                if (a == 0)
                  break;
                if (n == 0x40000)
                  equal = false;
              }
            }
            s16 rate = 400;
            if (equal) {
              p = play();
              if (gabi::load<s8>(p + 0x513F) == 8)
                rate = -600;
            }
            gabi::store<s16>(0x101F4820, rate);
            gabi::store<s16>(0x101F4822, 0);
            mMonotoneStarted = 1;
          }
        } else {
          mProjectionEmitter = particle(this, 0xC2B9, 4);
          gabi::call(0x025F0830);
        }
      }
    }
    mEventState = 0;
    demo_execute();
  }
  s32 room = current.roomNo;
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, 0x1088, &eyePos, 0, reverb);
  matrix(this);
  return true;
}
VERIFY(0x024DB310, &daWarphr_c::execute);
namespace {
static void init_wait(daWarphr_c *actor, s32 staff) {
  WWHD_FUNC(0x024DBAD0, void, actor, staff);
}
VERIFY(0x024DBAD0, init_wait);
static bool act_warp_arrive(daWarphr_c *actor, s32 staff) {
  WWHD_FUNC(0x024DBAD4, bool, actor, staff);
  return true;
}
VERIFY(0x024DBAD4, act_warp_arrive);
} // namespace
