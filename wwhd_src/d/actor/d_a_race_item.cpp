/* WWHD barrel items. Derived from zeldaret/tww; */
#include "d/actor/d_a_race_item.h"

void daRaceItem_c::set_mtx() {
  WWHD_FUNC(0x02459158, void, this);
  f32 sx = scale.x, sy = scale.y, sz = scale.z;
  J3DModel *model = mpModel;
  gabi::store<f32>(gabi::ea(model) + 0xBC, sx);
  gabi::store<f32>(gabi::ea(model) + 0xC0, sy);
  gabi::store<f32>(gabi::ea(model) + 0xC4, sz);
  f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
  mDoMtx_stack_c::transS(x, y, z);
  mDoMtx_stack_c::YrotM(current.angle.y);
  J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x02459158,
       static_cast<void (daRaceItem_c::*)()>(&daRaceItem_c::set_mtx));

void daRaceItem_c::checkGet() {
  WWHD_FUNC(0x0245910C, void, this);
  if (mGetType == 1 && mCyl.ChkCoHit())
    normalItemGet();
}
VERIFY(0x0245910C, &daRaceItem_c::checkGet);

BOOL daRaceItem_c::Delete() {
  WWHD_FUNC(0x0245979C, BOOL, this);
  u32 arc = gabi::load<u32>(0x101E6A74 + u32(m_itemNo) * 0x1C);
  return gabi::call<BOOL>(0x02183788, this, arc);
}
VERIFY(0x0245979C, &daRaceItem_c::Delete);

BOOL daRaceItem_c::CreateInit() {
  WWHD_FUNC(0x024597B8, BOOL, this);
  set_mtx();
  u32 model = gabi::ea(mpModel.get());
  cullMtx = model ? model + 0xC8 : 0;
  mStts.Init(0, 255, this);
  mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101CF2B4));
  mCyl.SetStts(&mStts);
  f32 height = f32(gabi::call<u32>(0x02184288, this));
  f32 radius = f32(gabi::call<u32>(0x021842A0, this));
  f32 sx = scale.x;
  if (sx > 1.0f) {
    radius *= sx;
    height *= sx;
  }
  mCyl.SetR(radius);
  mCyl.SetH(height);
  mAcchCir.SetWall(30.0f, 30.0f);
  mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
  u32 flags = mAcch.m_flags;
  mState = 0;
  u32 params = mParameters;
  mAcch.m_flags = flags & ~0x408u;
  mGetType = (params >> 15) & 15;
  return true;
}
VERIFY(0x024597B8, &daRaceItem_c::CreateInit);

cPhs_State daRaceItem_c::create() {
  WWHD_FUNC(0x02459924, cPhs_State, this);
  if (!(actor_condition & 8)) {
    if (this) {
      fopAc_ac_c_ct(this);
      __vtbl = 0x10012158;
      gabi::call(0x024F0474, &mAcch);
      gabi::store<u32>(gabi::ea(&mAcch) + 0x10, 0x10038D9C);
      gabi::store<u32>(gabi::ea(&mAcch) + 0x20, 0x10038DAC);
      gabi::store<u32>(gabi::ea(&mAcch) + 0x14, 0x10038DBC);
      gabi::store<u8>(gabi::ea(&mAcch) + 0x18, 1);
      gabi::call(0x024EFE94, &mAcchCir);
      dCcD_Stts_ct(&mStts);
      dCcD_Cyl_ct(&mCyl, 0x10038D8C);
      __vtbl = 0x10038DCC;
    }
    actor_condition |= 8;
  }
  u32 params = mParameters;
  s32 bit = (params >> 8) & 127;
  s8 room = home.roomNo;
  m_itemNo = params & 255;
  mItemBitNo = bit;
  if (dComIfGs_isItem(bit, room) && mItemBitNo != 127) {
    gabi::call(0x02184350, this);
    return cPhs_ERROR_e;
  }
  u32 arc = gabi::load<u32>(0x101E6A74 + u32(m_itemNo) * 28);
  cPhs_State phase = gabi::call<cPhs_State>(0x02520460, &mPhs, arc);
  if (phase == 4) {
    u32 heap = gabi::load<u16>(0x101E4694 + u32(m_itemNo) * 36);
    if (!gabi::call<BOOL>(0x025D63E8, this, 0x0218422Cu, heap))
      return cPhs_ERROR_e;
    CreateInit();
  }
  return phase;
}
VERIFY(0x02459924, &daRaceItem_c::create);

static BOOL daRaceItem_Draw(daRaceItem_c *p) {
  WWHD_FUNC(0x024590FC, BOOL, p);
  u32 method = gabi::load<u32>(u32(p->__vtbl) + 0x14);
  return gabi::call<BOOL>(method, p);
}
VERIFY(0x024590FC, daRaceItem_Draw);
static BOOL daRaceItem_IsDelete(daRaceItem_c *p) {
  WWHD_FUNC(0x02459794, BOOL, p);
  return true;
}
VERIFY(0x02459794, daRaceItem_IsDelete);
static BOOL daRaceItem_Delete(daRaceItem_c *p) {
  WWHD_FUNC(0x024597B4, BOOL, p);
  return p->Delete();
}
VERIFY(0x024597B4, daRaceItem_Delete);
static cPhs_State daRaceItem_Create(daRaceItem_c *p) {
  WWHD_FUNC(0x02459B18, cPhs_State, p);
  return p->create();
}
VERIFY(0x02459B18, daRaceItem_Create);
static void race_setListStart(daRaceItem_c *p) {
  WWHD_FUNC(0x02459BB0, void, p);
}
VERIFY(0x02459BB0, race_setListStart);
static void race_destructor(daRaceItem_c *p, s32 flags) {
  WWHD_FUNC(0x02459BB4, void, p, flags);
  if (p) {
    gabi::call(0x021832F4, p, 0);
    if (flags & 1)
      operator_delete(p);
  }
}
VERIFY(0x02459BB4, race_destructor);
static void race_sinit() {
  WWHD_FUNC(0x02459B1C, void, (u32)0);
  sinit_header_statics(0x1046D634, 0x101CF2F8);
}
VERIFY(0x02459B1C, race_sinit);

static BOOL daRaceItem_Execute(daRaceItem_c *p) {
  WWHD_FUNC(0x02459230, BOOL, p);
  dComIfGp_ea(); // HD accessor retained even though its result is unused.
  p->m_timer = u32(p->m_timer) + 1;
  gabi::call(0x021837B0, p, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
  f32 y = p->current.pos.y, z = p->current.pos.z, x = p->current.pos.x;
  u32 item = p->m_itemNo;
  p->eyePos.y = y;
  p->eyePos.z = z;
  p->eyePos.x = x;
  f32 h = f32(gabi::load<u8>(0x101E8675 + item * 4));
  u32 state = p->mState;
  p->eyePos.y = gabi::fmadds(h * f32(p->scale.x), 0.5f, y);
  switch (state) {
  case 0: {
    p->checkGet();
    s32 divisor = gabi::load<s16>(0x100121CC);
    s16 spin = divisor ? s16(65535 / divisor) : 0;
    s16 angle = p->current.angle.y;
    gabi::call(0x025D677C, p, s16(angle + spin), spin);
    p->mCyl.SetC(&p->current.pos);
    dComIfG_Ccsp_Set(&p->mCyl);
    break;
  }
  case 1: {
    u32 player = gabi::load<u32>(dComIfGp_ea() + 0x5B34);
    f32 hy = gabi::load<f32>(player + 0x3DC) + 15.0f;
    f32 hx = gabi::load<f32>(player + 0x3D8),
        hz = gabi::load<f32>(player + 0x3E0);
    p->current.pos.x = hx;
    p->current.angle.z = 0;
    p->current.pos.y = hy;
    p->current.angle.x = 0;
    p->current.pos.z = hz;
    p->scale.x = 1.0f;
    p->scale.y = 1.0f;
    p->scale.z = 1.0f;
    p->mGetTimer = 13;
    p->speed.x = 0.0f;
    p->speed.y = 23.0f;
    p->shape_angle.z = 0;
    p->speed.z = 0.0f;
    p->shape_angle.x = 0;
    p->gravity = -6.0f;
    p->mCyl.ClrTgHit();
    gabi::call(0x0251641C, &p->mCyl);
    u32 co = p->mCyl.mObjCo.mSPrm, tg = p->mCyl.mObjTg.mSPrm;
    u8 flags = p->mFlags;
    p->mState = 2;
    p->mCyl.mObjCo.mSPrm = co & ~1u;
    p->mCyl.mObjTg.mSPrm = tg & ~1u;
    p->mFlags = flags & ~1u;
    [[fallthrough]];
  }
  case 2: {
    p->mGetTimer = u32(p->mGetTimer) - 1;
    u32 player = gabi::load<u32>(dComIfGp_ea() + 0x5B34);
    f32 hz = gabi::load<f32>(player + 0x3E0),
        hx = gabi::load<f32>(player + 0x3D8);
    f32 hy = gabi::load<f32>(player + 0x3DC) + 15.0f;
    p->current.pos.x = hx;
    p->current.pos.z = hz;
    gabi::call(0x025D6870, p, (u32)0);
    if (f32(p->current.pos.y) < hy)
      p->current.pos.y = hy;
    if (p->mGetTimer < 0)
      fopAcM_delete(p);
    break;
  }
  case 3:
    p->mState = 4;
    [[fallthrough]];
  case 4: {
    p->checkGet();
    gabi::call(0x025D6870, p, &p->mStts.m_cc_move);
    u32 play = dComIfGp_ea();
    p->mAcch.CrrPos(gabi::at<dBgS>(play + 0x12A0));
    if (!(p->mFlags & 1) && !(p->actor_status & 0x100000)) {
      if (p->mAcch.m_flags & 0x1000) {
        s32 reverb = gabi::call<s32>(0x02520540, s32(p->current.roomNo));
        gabi::call(0x025E1A40, 0x6918, &p->eyePos, 0, reverb);
        fopAcM_delete(p);
      }
      f32 px = p->current.pos.x, pz = p->current.pos.z;
      if (gabi::call<BOOL>(0x0246B6A4, px, pz)) {
        px = p->current.pos.x;
        pz = p->current.pos.z;
        f32 wave = gabi::call<f32>(0x0246BA0C, px, pz);
        if (wave != 0.0f) {
          f32 py = p->current.pos.y;
          if (py - wave < 0.0f)
            fopAcM_delete(p);
          // The compiler retains py if no delete intervenes.
        }
      }
    }
    if (f32(p->current.pos.y) < -5000.0f)
      fopAcM_delete(p);
    p->mCyl.SetC(&p->current.pos);
    dComIfG_Ccsp_Set(&p->mCyl);
    break;
  }
  }
  if (p->mCyl.ChkTgHit() && p->mGetType == 1) {
    void *obj = p->mCyl.GetTgHitObj();
    if (obj) {
      u32 type = gabi::load<u32>(gabi::ea(obj) + 0x10);
      if (type & 0x40)
        p->mFlags = u8(p->mFlags) | 1;
      else if (type & 0x8000) {
        f32 height = f32(gabi::load<u8>(0x101E8675 + u32(p->m_itemNo) * 4));
        gabi::Local<cXyz> offset;
        offset->z = 0.0f;
        offset->x = 0.0f;
        offset->y = height * 0.5f;
        u32 player = gabi::load<u32>(dComIfGp_ea() + 0x5B2C);
        u32 vt = gabi::load<u32>(player + 0xB4),
            method = gabi::load<u32>(vt + 0x10C);
        u32 id = gabi::load<u32>(gabi::ea(p) + 4);
        gabi::call(method, gabi::at<void>(player), id, offset.get());
      }
    }
  }
  if (p->mFlags & 1) {
    gabi::Local<be<s16>> name;
    *name = 0x1B0;
    fopAc_ac_c *boom =
        gabi::call<fopAc_ac_c *>(0x025D5218, 0x025E121Cu, name.get());
    if (boom)
      p->current.pos.copy(boom->current.pos);
    else
      p->mFlags = u8(p->mFlags) & ~1u;
  }
  if (!p->checkOffsetPos()) {
    p->scale.x = 1.0f;
    p->scale.y = 1.0f;
    p->scale.z = 1.0f;
  }
  p->set_mtx();
  return true;
}
VERIFY(0x02459230, daRaceItem_Execute);
