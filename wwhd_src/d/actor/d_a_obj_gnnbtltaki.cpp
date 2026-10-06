/* Full HD Ganondorf battle waterfall actor, derived from zeldaret/tww. */
#include "d/actor/d_a_obj_gnnbtltaki.h"
BOOL daObjGnnbtaki_c::create_heap() {
  WWHD_FUNC(0x0234AEBC, BOOL, this);
  gabi::Local<SafeString> name;
  name->mStringTop = 0x10029730;
  name->__vtbl = 0x100296A4;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 4);
  if (!data) {
    gabi::call(0x0273AA24, STR(0x100296F0), 90, STR(0x100296D0));
    return FALSE;
  }
  mpModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
  if (!mpModel)
    return FALSE;
  gabi::Local<SafeString> animationName;
  animationName->mStringTop = 0x10029730;
  animationName->__vtbl = 0x100296A4;
  void *animation =
      gabi::call<void *>(0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)),
                         animationName.get(), 7);
  if (!animation) {
    gabi::call(0x0273AA24, STR(0x100296F0), 97, STR(0x100296E0));
    return FALSE;
  }
  return gabi::call<s32>(0x025E7CE0, &mBtk, data, animation, 1, 2, 1.0f, 0, -1,
                         0, 0) != 0;
}
VERIFY(0x0234AEBC, &daObjGnnbtaki_c::create_heap);
BOOL solidHeapCB(daObjGnnbtaki_c *actor) {
  WWHD_FUNC(0x0234AFD4, BOOL, actor);
  return actor->create_heap();
}
VERIFY(0x0234AFD4, solidHeapCB);
void daObjGnnbtaki_c::init_mtx() {
  WWHD_FUNC(0x0234AFD8, void, this);
  u32 model = gabi::ea((J3DModel *)mpModel);
  f32 z = scale.z, x = scale.x, y = scale.y;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC4, z);
  gabi::store<f32>(model + 0xC0, y);
}
VERIFY(0x0234AFD8, &daObjGnnbtaki_c::init_mtx);
s32 daObjGnnbtaki_c::_create() {
  WWHD_FUNC(0x0234AFF8, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(base + 0xB4, 0x100296BC);
      gabi::call(0x025E7C6C, &mBtk);
      flags = gabi::load<u32>(base + 0x2E4);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10029730));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, this, 0x0234AFD4, 0))
    return 5;
  u8 type = gabi::load<u32>(base + 0xB0) & 15;
  u32 model = gabi::ea((J3DModel *)mpModel);
  mType = type == 15 ? 0 : type;
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  init_mtx();
  mBtk.frame = (f32)(s16)mBtk.startFrame;
  if (mType == 1) {
    mPlaying = 0;
    mBtk.speed = 0.0f;
  } else {
    mPlaying = 1;
    mBtk.speed = 1.0f;
  }
  return 4;
}
VERIFY(0x0234AFF8, &daObjGnnbtaki_c::_create);
BOOL daObjGnnbtaki_c::_delete() {
  WWHD_FUNC(0x0234B154, BOOL, this);
  gabi::call(0x025204C8, &mPhase, STR(0x10029730));
  return TRUE;
}
VERIFY(0x0234B154, &daObjGnnbtaki_c::_delete);
BOOL daObjGnnbtaki_c::_execute() {
  WWHD_FUNC(0x0234B184, BOOL, this);
  bool tower = false;
  if (mType == 1) {
    gabi::Local<SafeString> requested, currentStage;
    requested->mStringTop = 0x10029714;
    requested->__vtbl = 0x100296A4;
    u32 play = gabi::call<u32>(0x025200D4);
    currentStage->mStringTop = play + 0x5134;
    currentStage->__vtbl = 0x100296A4;
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
                          STR(0x1002971C)) &&
          !mPlaying && gabi::load<u32>(0x101D600C) >= 0x126F) {
        mPlaying = 1;
        mBtk.frame = (f32)(s16)mBtk.startFrame;
        mBtk.speed = 1.0f;
      }
    }
  }
  gabi::call(0x025E742C, &mBtk);
  return TRUE;
}
VERIFY(0x0234B184, &daObjGnnbtaki_c::_execute);
BOOL daObjGnnbtaki_c::_draw() {
  WWHD_FUNC(0x0234B32C, BOOL, this);
  if (mPlaying) {
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
VERIFY(0x0234B32C, &daObjGnnbtaki_c::_draw);
s32 createWrapper(daObjGnnbtaki_c *p) {
  WWHD_FUNC(0x0234B3A8, s32, p);
  return p->_create();
}
VERIFY(0x0234B3A8, createWrapper);
BOOL deleteWrapper(daObjGnnbtaki_c *p) {
  WWHD_FUNC(0x0234B3AC, BOOL, p);
  return p->_delete();
}
VERIFY(0x0234B3AC, deleteWrapper);
BOOL executeWrapper(daObjGnnbtaki_c *p) {
  WWHD_FUNC(0x0234B3B0, BOOL, p);
  return p->_execute();
}
VERIFY(0x0234B3B0, executeWrapper);
BOOL drawWrapper(daObjGnnbtaki_c *p) {
  WWHD_FUNC(0x0234B3B4, BOOL, p);
  return p->_draw();
}
VERIFY(0x0234B3B4, drawWrapper);
void staticInitialize() {
  WWHD_FUNC(0x0234B3B8, void);
  gabi::store<u32>(0x10469C6C, 0);
  gabi::store<u32>(0x10469C64, 0);
  gabi::store<u32>(0x10469C70, 0);
  gabi::store<u32>(0x10469C68, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C9858));
  gabi::store<f32>(0x10469C58, -3.1415927410125732f);
  gabi::store<f32>(0x10469C5C, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469C60));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C9864));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469C61));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C9870));
}
VERIFY(0x0234B3B8, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x0234B44C, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x0234B44C, deleteStatic);
void destruct(daObjGnnbtaki_c *p, s32 flags) {
  WWHD_FUNC(0x0234B460, void, p, flags);
  if (p) {
    gabi::call(0x025D50BC, p, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, p);
  }
}
VERIFY(0x0234B460, destruct);
void emptyVirtual() { WWHD_FUNC(0x0234B4B4, void); }
VERIFY(0x0234B4B4, emptyVirtual);
BOOL IsDelete(daObjGnnbtaki_c *p) {
  WWHD_FUNC(0x0234B4B8, BOOL, p);
  return TRUE;
}
VERIFY(0x0234B4B8, IsDelete);
