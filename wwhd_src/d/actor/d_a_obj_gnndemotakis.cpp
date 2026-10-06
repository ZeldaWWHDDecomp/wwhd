/* Full HD Ganondorf pre-battle waterfall actor, derived from zeldaret/tww. */
#include "d/actor/d_a_obj_gnndemotakis.h"
BOOL daObjGnntakis_c::create_heap() {
  WWHD_FUNC(0x0234B8E0, BOOL, this);
  gabi::Local<SafeString> name;
  name->mStringTop = 0x10029868;
  name->__vtbl = 0x100297D4;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 4);
  if (!data) {
    gabi::call(0x0273AA24, STR(0x10029820), 156, STR(0x10029800));
    return FALSE;
  }
  mpModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
  if (!mpModel)
    return FALSE;
  gabi::Local<SafeString> animationName;
  animationName->mStringTop = 0x10029868;
  animationName->__vtbl = 0x100297D4;
  void *animation =
      gabi::call<void *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         animationName.get(), 7);
  BOOL success = FALSE;
  if (!animation) {
    gabi::call(0x0273AA24, STR(0x10029820), 163, STR(0x10029810));
  } else {
    success = gabi::call<s32>(0x025E7CE0, &mBtk, data, animation, 1, 0, 1.0f, 0,
                              -1, 0, 0) != 0;
  }
  u32 model = gabi::ea((J3DModel *)mpModel);
  u32 flags = gabi::load<u32>(model + 0x74);
  flags &= ~1u;
  gabi::store<u32>(model + 0x74, flags);
  gabi::call(0x027F596C, gabi::at<J3DModel>(model), flags);
  return success;
}
VERIFY(0x0234B8E0, &daObjGnntakis_c::create_heap);
BOOL solidHeapCB(daObjGnntakis_c *actor) {
  WWHD_FUNC(0x0234BA18, BOOL, actor);
  return actor->create_heap();
}
VERIFY(0x0234BA18, solidHeapCB);
void daObjGnntakis_c::init_mtx() {
  WWHD_FUNC(0x0234BA1C, void, this);
  u32 model = gabi::ea((J3DModel *)mpModel);
  f32 z = scale.z, x = scale.x, y = scale.y;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC4, z);
  gabi::store<f32>(model + 0xC0, y);
}
VERIFY(0x0234BA1C, &daObjGnntakis_c::init_mtx);
s32 daObjGnntakis_c::_create() {
  WWHD_FUNC(0x0234BA3C, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(base + 0xB4, 0x100297EC);
      gabi::call(0x025E7C6C, &mBtk);
      flags = gabi::load<u32>(base + 0x2E4);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10029868));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, this, 0x0234BA18, 0))
    return 5;
  u32 model = gabi::ea((J3DModel *)mpModel);
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  init_mtx();
  s16 start = mBtk.startFrame;
  mPlaying = 0;
  mBtk.frame = (f32)start;
  mBtk.speed = 0.0f;
  return 4;
}
VERIFY(0x0234BA3C, &daObjGnntakis_c::_create);
BOOL daObjGnntakis_c::_delete() {
  WWHD_FUNC(0x0234BB48, BOOL, this);
  gabi::call(0x025204C8, &mPhase, STR(0x10029868));
  return TRUE;
}
VERIFY(0x0234BB48, &daObjGnntakis_c::_delete);
BOOL daObjGnntakis_c::_execute() {
  WWHD_FUNC(0x0234BB78, BOOL, this);
  bool tower = false;
  {
    gabi::Local<SafeString> requested, currentStage;
    requested->mStringTop = 0x1002984C;
    requested->__vtbl = 0x100297D4;
    u32 play = gabi::call<u32>(0x025200D4);
    currentStage->mStringTop = play + 0x5134;
    currentStage->__vtbl = 0x100297D4;
    gabi::call(gabi::load<u32>((u32)requested->__vtbl + 0x14), requested.get());
    gabi::call(gabi::load<u32>((u32)requested->__vtbl + 0x14), requested.get());
    u32 first = requested->mStringTop;
    gabi::call(gabi::load<u32>((u32)currentStage->__vtbl + 0x14),
               currentStage.get());
    u32 second = currentStage->mStringTop;
    if (first == second)
      tower = true;
    else {
      first = requested->mStringTop;
      second = currentStage->mStringTop;
      for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = gabi::load<u8>(first + i), b = gabi::load<u8>(second + i);
        if (a != b)
          break;
        if (!a) {
          tower = true;
          break;
        }
      }
    }
  }
  if (tower) {
    u32 play = gabi::call<u32>(0x025200D4);
    if (gabi::load<u8>(play + 0x5292)) {
      play = gabi::call<u32>(0x025200D4);
      if (gabi::call<s32>(0x025445B8, gabi::at<u8>(play + 0x52C4),
                          STR(0x10029854))) {
        s32 playing = mPlaying;
        u32 demoFrame = gabi::load<u32>(0x101D600C);
        if (!playing) {
          if (demoFrame >= 0x11D9) {
            s16 start = mBtk.startFrame;
            mPlaying = 1;
            mBtk.frame = (f32)start;
            mBtk.speed = 1.0f;
          }
        } else if (demoFrame >= 0x126F) {
          gabi::call(0x025D57E0, this);
        }
      }
    }
  }
  gabi::call(0x025E742C, &mBtk);
  return TRUE;
}
VERIFY(0x0234BB78, &daObjGnntakis_c::_execute);
BOOL daObjGnntakis_c::_draw() {
  WWHD_FUNC(0x0234BD30, BOOL, this);
  {
    u32 light = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, gabi::at<u8>(light), 4, &current.pos,
               gabi::at<u8>(gabi::ea(this) + 0x110));
    light = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, gabi::at<u8>(light), (J3DModel *)mpModel,
               gabi::at<u8>(gabi::ea(this) + 0x110));
    u32 model = gabi::ea((J3DModel *)mpModel);
    f32 frame = mBtk.frame;
    void *data = gabi::at<u8>(gabi::load<u32>(model + 0xAC));
    gabi::call(0x025E7FC4, &mBtk, data, frame);
    gabi::call(0x025E2DE0, (J3DModel *)mpModel, 0);
  }
  return TRUE;
}
VERIFY(0x0234BD30, &daObjGnntakis_c::_draw);
s32 createWrapper(daObjGnntakis_c *p) {
  WWHD_FUNC(0x0234BDA0, s32, p);
  return p->_create();
}
VERIFY(0x0234BDA0, createWrapper);
BOOL deleteWrapper(daObjGnntakis_c *p) {
  WWHD_FUNC(0x0234BDA4, BOOL, p);
  return p->_delete();
}
VERIFY(0x0234BDA4, deleteWrapper);
BOOL executeWrapper(daObjGnntakis_c *p) {
  WWHD_FUNC(0x0234BDA8, BOOL, p);
  return p->_execute();
}
VERIFY(0x0234BDA8, executeWrapper);
BOOL drawWrapper(daObjGnntakis_c *p) {
  WWHD_FUNC(0x0234BDAC, BOOL, p);
  return p->_draw();
}
VERIFY(0x0234BDAC, drawWrapper);
void staticInitialize() {
  WWHD_FUNC(0x0234BDB0, void);
  gabi::store<u32>(0x10469CA4, 0);
  gabi::store<u32>(0x10469C9C, 0);
  gabi::store<u32>(0x10469CA8, 0);
  gabi::store<u32>(0x10469CA0, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C9940));
  gabi::store<f32>(0x10469C90, -3.1415927410125732f);
  gabi::store<f32>(0x10469C94, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469C98));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C994C));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469C99));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C9958));
}
VERIFY(0x0234BDB0, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x0234BE44, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x0234BE44, deleteStatic);
void destruct(daObjGnntakis_c *p, s32 flags) {
  WWHD_FUNC(0x0234BE58, void, p, flags);
  if (p) {
    gabi::call(0x025D50BC, p, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, p);
  }
}
VERIFY(0x0234BE58, destruct);
void emptyVirtual() { WWHD_FUNC(0x0234BEAC, void); }
VERIFY(0x0234BEAC, emptyVirtual);
BOOL IsDelete(daObjGnntakis_c *p) {
  WWHD_FUNC(0x0234BEB0, BOOL, p);
  return TRUE;
}
VERIFY(0x0234BEB0, IsDelete);
