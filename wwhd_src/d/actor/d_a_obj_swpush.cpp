/** Yellow floor switch. WWHD port; */
#include "d/actor/d_a_obj_swpush.h"
using daObjSwpush::Act_c;
static u32 PrmAbstract(fopAc_ac_c *a, s32 width, s32 shift) {
  WWHD_FUNC(0x02399090, u32, a, width, shift);
  u32 w = (u32)width & 63, s = (u32)shift & 63;
  return (s < 32 ? (u32)a->mParameters >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
VERIFY(0x02399090, PrmAbstract);
static bool is_switch(Act_c *a) {
  s32 sw = PrmAbstract(a, 8, 8);
  return fopAcM_isSwitch(a, sw) != 0;
}
static void change_switch(Act_c *a, u32 fn) {
  s32 sw = PrmAbstract(a, 8, 8);
  u32 save = gabi::load<u32>(0x101f84dc);
  s32 room = a->home.roomNo;
  gabi::call(fn, gabi::at<void>(save + 0x20), sw, room);
}
void Act_c::prmZ_init() {
  WWHD_FUNC(0x02396B98, void, this);
  if (mPrmZInit)
    return;
  s16 z = home.angle.z;
  mPrmZInit = 1;
  home.angle.z = 0;
  mPrmZ = z;
  current.angle.z = 0;
  shape_angle.z = 0;
}
VERIFY(0x02396B98, &Act_c::prmZ_init);
bool Act_c::is_switch2() {
  WWHD_FUNC(0x02396B14, bool, this);
  s32 ver = PrmAbstract(this, 1, 30);
  s32 sw = ver > 0 ? (u8)mPrmZ : 255;
  if (sw == 255)
    return false;
  return fopAcM_isSwitch(this, sw) != 0;
}
VERIFY(0x02396B14, &Act_c::is_switch2);
void Act_c::set_mtx() {
  WWHD_FUNC(0x023973C8, void, this);
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y,
                 shape_angle.z);
  J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x023973C8, &Act_c::set_mtx);
void Act_c::init_mtx() {
  WWHD_FUNC(0x0239748C, void, this);
  J3DModel *model = mpModel;
  f32 y = scale.y, x = scale.x, z = scale.z;
  gabi::store<f32>(gabi::ea(model) + 0xBC, x);
  gabi::store<f32>(gabi::ea(model) + 0xC0, y);
  gabi::store<f32>(gabi::ea(model) + 0xC4, z);
  set_mtx();
}
VERIFY(0x0239748C, &Act_c::init_mtx);
void Act_c::set_btp_frame() {
  WWHD_FUNC(0x023974AC, void, this);
  if (attr(0x100300b0, 0x100300c4).mBtpArcName) {
    bool sw = is_switch(this);
    gabi::store<f32>(gabi::ea(this) + 0x3fc, sw ? 1.0f : 0.0f);
  }
}
VERIFY(0x023974AC, &Act_c::set_btp_frame);
void Act_c::top_bg_aim_req(f32 h, s16 timer) {
  WWHD_FUNC(0x0239833C, void, this, h, timer);
  m328 = h;
  m324 = timer;
}
VERIFY(0x0239833C, &Act_c::top_bg_aim_req);
void Act_c::mode_upper_init() {
  WWHD_FUNC(0x02397564, void, this);
  mDebounceTimer = 0;
  mTargetHFrac = 1.0f;
  mMode = 0;
  mChangingState = 0;
}
VERIFY(0x02397564, &Act_c::mode_upper_init);
void Act_c::mode_lower_init() {
  WWHD_FUNC(0x02397584, void, this);
  mChangingState = 0;
  mMode = 2;
  mTargetHFrac = 0.0f;
}
VERIFY(0x02397584, &Act_c::mode_lower_init);
void Act_c::mode_u_l_init() {
  WWHD_FUNC(0x02398348, void, this);
  mMode = 1;
  mTargetHFrac = 0.0f;
  mSpeed = attr(0x10030288, 0x1003029c).mPushSpeed0;
  s16 delay = attr(0x10030288, 0x1003029c).m38;
  top_bg_aim_req(0.0f, delay);
}
VERIFY(0x02398348, &Act_c::mode_u_l_init);
void Act_c::mode_l_u_init() {
  WWHD_FUNC(0x02398A98, void, this);
  mMode = 3;
  mTargetHFrac = 1.0f;
  top_bg_aim_req(1.0f, 1);
}
VERIFY(0x02398A98, &Act_c::mode_l_u_init);
void Act_c::demo_non_init() {
  WWHD_FUNC(0x023975A4, void, this);
  mDemoMode = 0;
}
VERIFY(0x023975A4, &Act_c::demo_non_init);
void Act_c::demo_non() { WWHD_FUNC(0x0239902C, void, this); }
VERIFY(0x0239902C, &Act_c::demo_non);
void Act_c::demo_runSw_init() {
  WWHD_FUNC(0x02398E80, void, this);
  mDemoMode = 4;
}
VERIFY(0x02398E80, &Act_c::demo_runSw_init);
void Act_c::demo_runPause_init() {
  WWHD_FUNC(0x0239842C, void, this);
  mDemoMode = 2;
  mPauseTimer = attr(0x100302c0, 0x100302d4).mPauseDuration;
}
VERIFY(0x0239842C, &Act_c::demo_runPause_init);
void Act_c::demo_reqPause() {
  WWHD_FUNC(0x02398498, void, this);
  if (eventInfo_checkCommandDemoAccrpt(this))
    demo_runPause_init();
  else
    demo_non_init();
}
VERIFY(0x02398498, &Act_c::demo_reqPause);
void Act_c::demo_reqPause_init() {
  WWHD_FUNC(0x023985AC, void, this);
  if (mDemoMode == 0) {
    mDemoMode = 1;
    gabi::call(0x025d7b24, this, 2, 0, 0);
    eventInfo_onCondition(this, 2);
  }
}
VERIFY(0x023985AC, &Act_c::demo_reqPause_init);
void Act_c::demo_stop_puase() {
  WWHD_FUNC(0x023984AC, void, this);
  if (mDemoMode == 1)
    demo_reqPause();
  if (mDemoMode == 2)
    dComIfGp_event_reset();
}
VERIFY(0x023984AC, &Act_c::demo_stop_puase);
void Act_c::demo_runPause() {
  WWHD_FUNC(0x02398E2C, void, this);
  s16 timer = (s16)((s16)mPauseTimer - 1);
  mPauseTimer = timer;
  if (timer <= 0) {
    dComIfGp_event_reset();
    demo_non_init();
  }
}
VERIFY(0x02398E2C, &Act_c::demo_runPause);
void Act_c::demo_reqSw_init() {
  WWHD_FUNC(0x02398504, void, this);
  if (dComIfGp_evmng_existence(mEventID) && (u32)mDemoMode <= 2) {
    demo_stop_puase();
    mDemoMode = 3;
    u8 ev = PrmAbstract(this, 8, 0);
    fopAcM_orderOtherEventId(this, mEventID, ev, 0xffff, 0, 1);
    eventInfo_onCondition(this, 2);
  }
}
VERIFY(0x02398504, &Act_c::demo_reqSw_init);
void Act_c::demo_reqSw() {
  WWHD_FUNC(0x02398E8C, void, this);
  if (eventInfo_checkCommandDemoAccrpt(this))
    demo_runSw_init();
  else {
    u8 ev = PrmAbstract(this, 8, 0);
    fopAcM_orderOtherEventId(this, mEventID, ev, 0xffff, 0, 1);
    eventInfo_onCondition(this, 2);
  }
}
VERIFY(0x02398E8C, &Act_c::demo_reqSw);
void Act_c::demo_runSw() {
  WWHD_FUNC(0x02398F10, void, this);
  if (dComIfGp_evmng_endCheck(mEventID)) {
    dComIfGp_event_reset();
    demo_non_init();
  }
}
VERIFY(0x02398F10, &Act_c::demo_runSw);
void Act_c::mode_upper() {
  WWHD_FUNC(0x02398608, void, this);
  bool pause = false, pressing = false;
  f32 height = 1.0f;
  if (mMiniPushFlg) {
    if ((u32)attr(0x100302fc, 0x10030310).mFlags & 8) {
      if (mPushFlg)
        pressing = true;
      else
        height = 0.9f;
    } else
      pressing = true;
  }
  auto &a = attr(0x100302fc, 0x10030310);
  if (mDebounceTimer <= 0) {
    if (!((u32)a.mFlags & 8) && mRidingMode && !mPrevRiding) {
      mSpeed = attr(0x100302fc, 0x10030310).m34;
      mDebounceTimer = 30;
      pause = true;
    }
  } else
    mDebounceTimer = (s16)((s16)mDebounceTimer - 1);
  bool sw = is_switch(this), lower = pressing;
  if (!pressing && ((u32)attr(0x100302fc, 0x10030310).mFlags & 1)) {
    bool onUp = (u32)attr(0x100302fc, 0x10030310).mFlags & 16;
    lower = sw ? !onUp : onUp;
  }
  if (lower) {
    if (pressing)
      mChangingState = 1;
    mode_u_l_init();
    demo_reqSw_init();
  } else {
    mTargetHFrac = height;
    top_bg_aim_req(height, 1);
    if (pause)
      demo_reqPause_init();
  }
}
VERIFY(0x02398608, &Act_c::mode_upper);
void Act_c::mode_lower() {
  WWHD_FUNC(0x02398AB4, void, this);
  bool pressing = false;
  if (mMiniPushFlg)
    pressing = !((u32)attr(0x1003036c, 0x10030380).mFlags & 8) || mPushFlg;
  bool sw = is_switch(this);
  bool sw2 = is_switch2();
  bool obey = (u32)attr(0x1003036c, 0x10030380).mFlags & 1;
  bool stay = (u32)attr(0x1003036c, 0x10030380).mFlags & 2;
  bool toggle = (u32)attr(0x1003036c, 0x10030380).mFlags & 4;
  bool onUp = (u32)attr(0x1003036c, 0x10030380).mFlags & 16;
  bool lock = (u32)attr(0x1003036c, 0x10030380).mFlags & 32;
  bool pop = !stay && !pressing,
       match = obey && (sw ? onUp : !onUp) && !pressing;
  if ((pop || match) && !(lock && sw2)) {
    if (pop && !toggle && !pressing)
      mChangingState = 1;
    demo_reqSw_init();
    mode_l_u_init();
  }
}
VERIFY(0x02398AB4, &Act_c::mode_lower);
void Act_c::mode_u_l() {
  WWHD_FUNC(0x023988E8, void, this);
  if (!(mCurHFrac > 0.0f)) {
    if (mChangingState) {
      if ((u32)attr(0x10030334, 0x10030348).mFlags & 4)
        change_switch(this, 0x025ba20c);
      else {
        bool onUp = (u32)attr(0x10030334, 0x10030348).mFlags & 16;
        change_switch(this, onUp ? 0x025b9f7c : 0x025b9e38);
      }
      if (mVibTimer) {
        gabi::Local<cXyz> dir;
        dir->set(0.0f, 1.0f, 0.0f);
        dComIfGp_getVibration_StartShock(4, -0x21, dir);
      }
    }
    fopAcM_seStart(this, 0x6930, 0);
    mode_lower_init();
  }
}
VERIFY(0x023988E8, &Act_c::mode_u_l);
void Act_c::mode_l_u() {
  WWHD_FUNC(0x02398D54, void, this);
  if (!(mCurHFrac < 1.0f)) {
    if (mChangingState) {
      bool onUp = (u32)attr(0x100303a4, 0x100303b8).mFlags & 16;
      change_switch(this, onUp ? 0x025b9e38 : 0x025b9f7c);
    }
    mode_upper_init();
  }
}
VERIFY(0x02398D54, &Act_c::mode_l_u);
void Act_c::calc_top_pos() {
  WWHD_FUNC(0x02397E70, void, this);
  f32 diff = (f32)mCurHFrac - (f32)mTargetHFrac;
  f32 decay = attr(0x100301a8, 0x100301bc).mSpeedDecay;
  f32 spring = attr(0x100301a8, 0x100301bc).mSpring;
  f32 v = gabi::fnmsubs(diff, spring, mSpeed);
  v = gabi::fnmsubs(v, decay, v);
  f32 h = (f32)mCurHFrac + v;
  mCurHFrac = h;
  mSpeed = v;
  f32 y = (1.0f - h) * -35.5f;
  if (y < -36.5f)
    y = -36.5f;
  else
    y = (y - 1.0f) >= 0 ? 1.0f : y;
  m31C = y;
  if (m324 > 0) {
    s16 timer = (s16)((s16)m324 - 1);
    m324 = timer;
    if (timer == 0)
      m320 = (f32)m328;
  }
  f32 top = mMode == 0 ? (f32)mCurHFrac : (f32)m320;
  m32C = top;
  f32 ty = (1.0f - top) * -35.5f;
  mTopPos = ty;
  if (ty < (f32)m31C)
    mTopPos = (f32)m31C;
}
VERIFY(0x02397E70, &Act_c::calc_top_pos);
void Act_c::set_push_flag() {
  WWHD_FUNC(0x02397BFC, void, this);
  if (mVibTimer)
    mVibTimer = (u8)((u8)mVibTimer - 1);
  if (mMiniPushFlg) {
    if (mRidingMode) {
      if (mRidingMode == 2)
        mMiniPushTimer = attr(0x1003016c, 0x10030180).mMiniPushDelay2;
      else
        mMiniPushTimer = attr(0x1003016c, 0x10030180).mMiniPushDelay1;
    } else {
      s16 t = (s16)((s16)mMiniPushTimer - 1);
      mMiniPushTimer = t;
      if (t <= 0)
        mMiniPushFlg = 0;
    }
  } else {
    if (mRidingMode) {
      auto &a = attr(0x1003016c, 0x10030180);
      s16 t = (s16)((s16)mMiniPushTimer + 1);
      mMiniPushTimer = t;
      if (t >= (s16)a.mMiniPushDelay1)
        mMiniPushFlg = 1;
    } else
      mMiniPushTimer = 0;
  }
  if (mPushFlg) {
    if (mHeavyRiding)
      mPushTimer = attr(0x1003016c, 0x10030180).mPushDelay;
    else {
      s16 t = (s16)((s16)mPushTimer - 1);
      mPushTimer = t;
      if (t <= 0)
        mPushFlg = 0;
    }
  } else {
    if (mHeavyRiding) {
      auto &a = attr(0x1003016c, 0x10030180);
      s16 t = (s16)((s16)mPushTimer + 1);
      mPushTimer = t;
      if (t >= (s16)a.mPushDelay)
        mPushFlg = 1;
    } else
      mPushTimer = 0;
  }
}
VERIFY(0x02397BFC, &Act_c::set_push_flag);
s32 Act_c::create_res_load() {
  WWHD_FUNC(0x023969BC, s32, this);
  if (attr(0x1002FFAC, 0x1002FFC0).mKbotaResName) {
    const char *name = STR(attr(0x1002FFAC, 0x1002FFC0).mKbotaResName);
    s32 result = dComIfG_resLoad(&mKbotaPhs, name);
    if (result != 4)
      return result;
  }
  if (attr(0x1002FFAC, 0x1002FFC0).mHhbotResName) {
    const char *name = STR(attr(0x1002FFAC, 0x1002FFC0).mHhbotResName);
    s32 result = dComIfG_resLoad(&mHhbotPhs, name);
    if (result != 4)
      return result;
  }
  return 4;
}
VERIFY(0x023969BC, &Act_c::create_res_load);
s32 Act_c::Mthd_Delete() {
  WWHD_FUNC(0x02397AD0, s32, this);
  u32 play = dComIfGp_ea();
  u32 bg = mpBgW;
  gabi::call<s32>(0x020087EC, gabi::at<void>(play + 0x12A0),
                  gabi::at<void>(bg));
  if (attr(0x10030134, 0x10030148).mKbotaResName) {
    const char *name = STR(attr(0x10030134, 0x10030148).mKbotaResName);
    dComIfG_resDelete(&mKbotaPhs, name);
  }
  if (attr(0x10030134, 0x10030148).mHhbotResName) {
    const char *name = STR(attr(0x10030134, 0x10030148).mHhbotResName);
    dComIfG_resDelete(&mHhbotPhs, name);
  }
  return TRUE;
}
VERIFY(0x02397AD0, &Act_c::Mthd_Delete);
static BOOL Delete_wrapper(Act_c *a) {
  WWHD_FUNC(0x02398F78, BOOL, a);
  return a->Mthd_Delete();
}
VERIFY(0x02398F78, Delete_wrapper);
static void trivial_dt(void *p, s32 flags) {
  WWHD_FUNC(0x02399018, void, p, flags);
  if (p && (flags & 1))
    operator_delete(p);
}
VERIFY(0x02399018, trivial_dt);
static void actor_dt(void *p, s32 flags) {
  WWHD_FUNC(0x02399034, void, p, flags);
  if (p) {
    gabi::call(0x025D50BC, p, 0);
    if (flags & 1)
      operator_delete(p);
  }
}
VERIFY(0x02399034, actor_dt);
static void empty2(void *p) { WWHD_FUNC(0x02399030, void, p); }
VERIFY(0x02399030, empty2);
static BOOL isDelete(void *p) {
  WWHD_FUNC(0x02399088, BOOL, p);
  return TRUE;
}
VERIFY(0x02399088, isDelete);
static void sinit() {
  WWHD_FUNC(0x02398F84, void, (u32)0);
  gabi::store<u32>(0x1046C094, 0);
  gabi::store<u32>(0x1046C08C, 0);
  gabi::store<u32>(0x1046C098, 0);
  gabi::store<u32>(0x1046C090, 0);
  gabi::call(0x028F026C, gabi::at<void>(0x101CD010));
  gabi::store<f32>(0x1046C050, -3.1415927410125732f);
  gabi::store<f32>(0x1046C054, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<void>(0x1046C058));
  gabi::call(0x028F026C, gabi::at<void>(0x101CD01C));
  gabi::call(0x028EAB2C, gabi::at<void>(0x1046C059));
  gabi::call(0x028F026C, gabi::at<void>(0x101CD028));
}
VERIFY(0x02398F84, sinit);
s32 Act_c::Mthd_Draw() {
  WWHD_FUNC(0x02398240, s32, this);
  settingTevStruct(dKy_getEnvlight(), 1, &current.pos, &tevStr);
  setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
  if (attr(0x10030250, 0x10030264).mBtpArcName) {
    s16 frame = (s16)gabi::ftoi(gabi::load<f32>(gabi::ea(this) + 0x3FC));
    u32 model = gabi::ea((J3DModel *)mpModel);
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::call(0x025E7B3C, gabi::at<void>(gabi::ea(this) + 0x3F8),
               gabi::at<void>(data), frame);
  }
  dComIfGd_setListBG();
  mDoExt_modelUpdateDL(mpModel);
  dComIfGd_setList();
  return TRUE;
}
VERIFY(0x02398240, &Act_c::Mthd_Draw);
static s32 Draw_wrapper(Act_c *a) {
  WWHD_FUNC(0x02398F80, s32, a);
  return a->Mthd_Draw();
}
VERIFY(0x02398F80, Draw_wrapper);
static void member_dispatch(Act_c *a, u32 entry) {
  u32 base = gabi::ea(a) + (s16)gabi::load<s16>(entry);
  s16 index = gabi::load<s16>(entry + 2);
  u32 target;
  if (index < 0)
    target = gabi::load<u32>(entry + 4);
  else {
    s16 offset = gabi::load<s16>(entry + 6);
    u32 vt = gabi::load<u32>(base + offset);
    target = gabi::load<u32>(vt + (u32)index * 8 + 4);
  }
  gabi::call(target, gabi::at<void>(base));
}
s32 Act_c::Mthd_Execute() {
  WWHD_FUNC(0x02398014, s32, this);
  member_dispatch(this, 0x100301E0 + (u32)mDemoMode * 8);
  set_push_flag();
  member_dispatch(this, 0x10030208 + (u32)mMode * 8);
  calc_top_pos();
  u8 riding = mRidingMode;
  u8 heavy = mHeavyRiding;
  mHeavyRiding = 0;
  mRidingMode = 0;
  mPrevHeavyRiding = heavy;
  mPrevRiding = riding != 0;
  set_mtx();
  gabi::call(0x024F5B08, gabi::at<void>(mpBgW));
  u32 bg = mpBgW;
  u32 countPtr = gabi::load<u32>(bg + 0x94);
  u32 vertices = gabi::load<u32>(bg + 0x90);
  s32 count = gabi::load<s32>(countPtr);
  for (u32 i = 0; i < 4; i++) {
    u32 vertex = gabi::load<u8>(0x100303E8 + i);
    if ((s32)vertex >= count)
      JUT_ASSERT_fail(STR(0x1003023C), 0x57B, STR(0x10030228));
    f32 y = (f32)m2D4 + (f32)mTopPos;
    gabi::store<f32>(vertices + vertex * 12 + 4, y);
  }
  gabi::call(0x024F43DC, gabi::at<void>(mpBgW));
  f32 y = (f32)current.pos.y + (f32)m31C - -35.5f;
  f32 x = current.pos.x, z = current.pos.z;
  eyePos.x = x;
  eyePos.z = z;
  eyePos.y = y;
  set_btp_frame();
  return TRUE;
}
VERIFY(0x02398014, &Act_c::Mthd_Execute);
static s32 Execute_wrapper(Act_c *a) {
  WWHD_FUNC(0x02398F7C, s32, a);
  return a->Mthd_Execute();
}
VERIFY(0x02398F7C, Execute_wrapper);
bool Act_c::create_heap() {
  WWHD_FUNC(0x02396CF0, bool, this);
  u32 modelIndex = PrmAbstract(this, 1, 16);
  if (modelIndex >= 2)
    JUT_ASSERT_fail(STR(0x1003002C), 0x203, STR(0x1002FFF8));
  auto &modelAttr = attr(0x1003002C, 0x10030040);
  s16 resource = gabi::load<s16>(gabi::ea(&modelAttr) + 0x22 + modelIndex * 2);
  const char *name = STR(attr(0x1003002C, 0x10030040).mModelArcName);
  auto *data = (J3DModelData *)dComIfG_getObjectRes(name, resource, 0x1002FF84);
  if (!data)
    JUT_ASSERT_fail(STR(0x1003002C), 0x209, STR(0x1002FFE8));
  u32 flags =
      attr(0x1003002C, 0x10030040).mBtpArcName ? 0x11020022 : 0x11000022;
  J3DModel *model = mDoExt_J3DModel__create(data, 0x80000, flags);
  mpModel = model;
  if (model) {
    u32 d = gabi::ea(data);
    u32 count = gabi::load<u32>(d + 4);
    u32 joint = gabi::load<u32>(d + 8);
    if (count > 1)
      joint += 0x1C;
    gabi::store<u32>(joint + 8, 0x02396BC8);
    gabi::store<u32>(gabi::ea((J3DModel *)mpModel) + 0xB8, gabi::ea(this));
  }
  s32 btp = 1;
  if (attr(0x1003002C, 0x10030040).mBtpArcName) {
    const char *btpName = STR(attr(0x1003002C, 0x10030040).mBtpArcName);
    s16 index = attr(0x1003002C, 0x10030040).mBtpResIndex;
    void *animation = dComIfG_getObjectRes(btpName, index, 0x1002FF84);
    if (!animation)
      JUT_ASSERT_fail(STR(0x1003002C), 0x222, STR(0x1003001C));
    btp = gabi::call<s32>(0x025E789C, gabi::at<void>(gabi::ea(this) + 0x3F8),
                          data, animation, 1, 0, 0, -1, 0, 0, 1.0f);
  }
  const char *bgName = STR(attr(0x1003002C, 0x10030040).mBgArcName);
  s16 bgIndex = attr(0x1003002C, 0x10030040).mBgResIndex;
  void *bgData = dComIfG_getObjectRes(bgName, bgIndex, 0x1002FF84);
  if (!bgData)
    JUT_ASSERT_fail(STR(0x1003002C), 0x22F, STR(0x10030064));
  void *bg = gabi::call<void *>(0x024F5A18, (u32)0);
  mpBgW = gabi::ea(bg);
  bool bgOk = false;
  if (bg)
    bgOk = gabi::call<s32>(0x024F5A78, bg, bgData, 0) == 0;
  bool result = mpModel != nullptr && btp && (u32)mpBgW && bgOk;
  if (!result)
    mpBgW = 0;
  return result;
}
VERIFY(0x02396CF0, &Act_c::create_heap);
static bool heap_wrapper(Act_c *a) {
  WWHD_FUNC(0x02397058, bool, a);
  return a->create_heap();
}
VERIFY(0x02397058, heap_wrapper);
s32 Act_c::Mthd_Create() {
  WWHD_FUNC(0x023975B0, s32, this);
  if (!((u32)actor_condition & 8)) {
    if (gabi::ea(this)) {
      fopAc_ac_c_ct(this);
      __vtbl = 0x1002FF9C;
      gabi::call(0x025E7820, gabi::at<void>(gabi::ea(this) + 0x3F8));
    }
    actor_condition = (u32)actor_condition | 8;
  }
  prmZ_init();
  mType = PrmAbstract(this, 3, 24);
  s32 phase = create_res_load();
  if (phase != 4)
    return phase;
  f32 scaleX = attr(0x100300FC, 0x10030110).mScale;
  scale.x = (f32)scale.x * scaleX;
  f32 scaleZ = attr(0x100300FC, 0x10030110).mScale;
  scale.z = (f32)scale.z * scaleZ;
  u32 heap = attr(0x100300FC, 0x10030110).mHeapSize;
  if (!gabi::call<s32>(0x025D63E8, this, (u32)0x02397058, heap))
    return 5;
  u32 play = dComIfGp_ea();
  u32 bg = mpBgW;
  if (gabi::call<s32>(0x024EEA6C, gabi::at<void>(play + 0x12A0),
                      gabi::at<void>(bg), this))
    return 5;
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y,
                 shape_angle.z);
  mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
  PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
  bg = mpBgW;
  gabi::store<u8>(bg + 0x6C, gabi::load<u8>(bg + 0x6C) & 0xFD);
  bg = mpBgW;
  gabi::store<u32>(bg + 8, gabi::ea(&mMtx));
  gabi::call(0x020095BC, gabi::at<void>(mpBgW));
  bg = mpBgW;
  gabi::store<u32>(bg + 8, 0);
  bg = mpBgW;
  gabi::store<u8>(bg + 0x6C, gabi::load<u8>(bg + 0x6C) | 2);
  bg = mpBgW;
  u32 vertices = gabi::load<u32>(bg + 0x90);
  m2D4 = gabi::load<f32>(vertices + 0x4C);
  gabi::store<u32>(bg + 0xB0, 0x0239705C);
  cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
  init_mtx();
  auto *a = &attr(0x100300FC, 0x10030110);
  auto *b = &attr(0x100300FC, 0x10030110);
  auto *c = &attr(0x100300FC, 0x10030110);
  auto *d = &attr(0x100300FC, 0x10030110);
  f32 x0 = -60 * (f32)a->mScale, z0 = -60 * (f32)b->mScale,
      x1 = 60 * (f32)c->mScale, z1 = 60 * (f32)d->mScale;
  fopAcM_setCullSizeBox(this, x0, -2, z0, x1, 39, z1);
  mPushTimer = 0;
  mHeavyRiding = 0;
  mPrevRiding = 0;
  mSpeed = 0;
  mMiniPushTimer = 0;
  mPrevHeavyRiding = 0;
  m324 = 0;
  mPushFlg = 0;
  mMiniPushFlg = 0;
  mVibTimer = 0;
  mRidingMode = 0;
  m328 = 0;
  set_btp_frame();
  bool toggle = (u32)attr(0x100300FC, 0x10030110).mFlags & 4;
  bool onUp = (u32)attr(0x100300FC, 0x10030110).mFlags & 16;
  bool second = (u32)attr(0x100300FC, 0x10030110).mFlags & 32;
  bool on = is_switch(this);
  bool on2 = is_switch2();
  if ((toggle || (on == onUp)) && !(second && on2)) {
    mTopPos = 0;
    m31C = 0;
    mCurHFrac = 1;
    m320 = 1;
    m32C = 1;
    mTargetHFrac = 1;
    mode_upper_init();
  } else {
    mCurHFrac = 0;
    m32C = 0;
    m320 = 0;
    m31C = -35.5f;
    mTargetHFrac = 0;
    mTopPos = -35.5f;
    mode_lower_init();
  }
  u8 event = PrmAbstract(this, 8, 0);
  u32 evPlay = dComIfGp_ea();
  mEventID = gabi::call<s16>(0x02543F10, gabi::at<void>(evPlay + 0x52C4),
                             (u32)0, event);
  demo_non_init();
  return phase;
}
VERIFY(0x023975B0, &Act_c::Mthd_Create);
static s32 Create_wrapper(Act_c *a) {
  WWHD_FUNC(0x02398F74, s32, a);
  return a->Mthd_Create();
}
VERIFY(0x02398F74, Create_wrapper);
static BOOL jnodeCB(void *node, s32 timing) {
  WWHD_FUNC(0x02396BC8, BOOL, node, timing);
  if (timing == 0) {
    u32 model = gabi::load<u32>(0x104B462C);
    u32 actor = gabi::load<u32>(model + 0xB8);
    void *joint = gabi::call<void *>(0x027F7878, node);
    u16 index = gabi::load<u16>(gabi::ea(joint) + 4);
    u32 block = gabi::load<u32>(model + 0x2C);
    u32 matrices = gabi::load<u32>(block + 0x10);
    gabi::store<u16>(block + 4, gabi::load<u16>(block + 4) | 16);
    PSMTXCopy(gabi::at<Mtx34>(matrices + index * 0x30), mDoMtx_stack_c::get());
    gabi::call(0x025F24E0, gabi::load<f32>(actor + 0x498), 0.0f, 0.0f);
    block = gabi::load<u32>(model + 0x2C);
    matrices = gabi::load<u32>(block + 0x10);
    gabi::store<u16>(block + 4, gabi::load<u16>(block + 4) | 16);
    mtx_copy(gabi::at<Mtx34>(matrices + index * 0x30), mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), gabi::at<Mtx34>(0x104B4868));
  }
  return TRUE;
}
VERIFY(0x02396BC8, jnodeCB);
struct RideTri_l {
  u8 data[0x38];
};
struct RideVectors_l {
  cXyz point[4];
};
static void rideCB(void *bg, Act_c *a, fopAc_ac_c *rider) {
  WWHD_FUNC(0x0239705C, void, bg, a, rider);
  if (!((u32)rider->actor_status & 0x400))
    return;
  if (!gabi::load<u32>(0x1046C09C)) {
    gabi::store<u32>(0x1046C09C, 1);
    const f32 xyz[12] = {-1, 0, -1, -1, 0, 1, 1, 0, 1, 1, 0, -1};
    for (u32 i = 0; i < 12; i++)
      gabi::store<f32>(0x1046C05C + i * 4, xyz[i]);
  }
  u32 vertices = gabi::load<u32>((u32)a->mpBgW + 0x90);
  gabi::Local<RideTri_l> tri;
  gabi::call(0x02019040, (RideTri_l *)tri);
  bool previous = a->mPrevRiding;
  auto &attr = a->attr(0x10030078, 0x1003008C);
  f32 scale = previous ? (f32)attr.m44 : (f32)attr.m40;
  gabi::call(0x025F23EC);
  gabi::call(0x025F1884, mDoMtx_stack_c::get(), (s16)a->shape_angle.y);
  mDoMtx_stack_c::scaleM(scale, scale, scale);
  gabi::Local<RideVectors_l> points;
  gabi::Local<cXyz> out;
  for (u32 i = 0; i < 4; i++) {
    u32 vertex = gabi::load<u8>(0x100303E8 + i);
    u32 source = vertices + vertex * 12;
    points->point[i].x = gabi::load<f32>(source);
    points->point[i].y = gabi::load<f32>(source + 4);
    points->point[i].z = gabi::load<f32>(source + 8);
    gabi::call(0x028E8F64, mDoMtx_stack_c::get(),
               gabi::at<cXyz>(0x1046C05C + i * 12), (cXyz *)out);
    PSVECAdd(&points->point[i], (cXyz *)out, &points->point[i]);
  }
  gabi::call(0x025F2468);
  for (u32 i = 0; i < 2; i++) {
    s32 v0 = gabi::load<s8>(0x1002FF7C + i * 3),
        v1 = gabi::load<s8>(0x1002FF7D + i * 3),
        v2 = gabi::load<s8>(0x1002FF7E + i * 3);
    gabi::call(0x0201924C, (RideTri_l *)tri, &points->point[v0],
               &points->point[v1], &points->point[v2]);
    if (gabi::call<s32>(0x0201204C, (RideTri_l *)tri, &rider->current.pos)) {
      a->mRidingMode = ((u32)rider->actor_status & 0x08000000) ? 2 : 1;
      if (rider && gabi::load<s16>(gabi::ea(rider) + 0xE) == 0xA8) {
        a->mVibTimer = 4;
        if (gabi::load<u32>(gabi::ea(rider) + 0x3B8) & 0x02000000)
          a->mHeavyRiding = 1;
      }
      break;
    }
  }
}
VERIFY(0x0239705C, rideCB);
