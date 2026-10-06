/* WWHD barrel-item static methods. */
#include "d/actor/d_a_race_item.h"
void daRaceItem_c::raceItemGet() {
  WWHD_FUNC(0x02459C08, void, this);
  u32 item = m_itemNo, amount = 0, sound = 0x826;
  switch (item) {
  case 1:
    amount = 1;
    break;
  case 2:
    amount = 5;
    sound = 0x835;
    break;
  case 3:
    amount = 10;
    break;
  case 4:
    amount = 20;
    sound = 0x836;
    break;
  case 5:
    amount = 50;
    break;
  case 6:
    amount = 100;
    break;
  case 15:
    amount = 200;
    break;
  default:
    return;
  }
  gabi::call(0x025E1988, sound);
  u32 play = dComIfGp_ea();
  s32 rupees = gabi::load<s16>(play + 0x5CEC) + amount;
  gabi::store<s16>(play + 0x5CEC, rupees > 0 ? rupees : 0);
}
VERIFY(0x02459C08, &daRaceItem_c::raceItemGet);
void daRaceItem_c::normalItemGet() {
  WWHD_FUNC(0x02459EA0, void, this);
  u32 item = m_itemNo;
  mState = 1;
  gabi::call(0x0254DA38, item);
  s32 bit = mItemBitNo;
  if (bit != 127) {
    s32 room = home.roomNo;
    gabi::call(0x025BA384, dComIfGs_info(), bit, room);
  }
  item = m_itemNo;
  if (item >= 1 && item <= 6)
    gabi::call(0x025E1988, u32(gabi::load<u16>(0x10038E42 + 2 * item)));
  else if (item >= 9 && item <= 18)
    gabi::call(0x025E1988, u32(gabi::load<u16>(0x10038E3E + 2 * item)));
  else if (item == 30)
    gabi::call(0x025E1988, 0x821);
}
VERIFY(0x02459EA0, &daRaceItem_c::normalItemGet);
void daRaceItem_c::raceItemForceGet() {
  WWHD_FUNC(0x02459F6C, void, this);
  u32 type = mGetType;
  mState = 1;
  if (type == 0)
    raceItemGet();
  else if (type == 1)
    normalItemGet();
}
VERIFY(0x02459F6C, &daRaceItem_c::raceItemForceGet);
BOOL daRaceItem_c::startOffsetPos() {
  WWHD_FUNC(0x02459F90, BOOL, this);
  mState = 0;
  return true;
}
VERIFY(0x02459F90, &daRaceItem_c::startOffsetPos);
BOOL daRaceItem_c::endOffsetPos(f32 grav, cXyz *scl, f32 vy, f32 forward,
                                csXyz *angle) {
  WWHD_FUNC(0x02459FA0, BOOL, this, grav, scl, vy, forward, angle);
  if (angle) {
    current.angle.x = angle->x;
    current.angle.y = angle->y;
    current.angle.z = angle->z;
  }
  if (scl) {
    scale.x = scl->x;
    scale.y = scl->y;
    scale.z = scl->z;
  }
  speedF = forward;
  gravity = grav;
  mState = 3;
  speed.y = vy;
  return true;
}
VERIFY(0x02459FA0, &daRaceItem_c::endOffsetPos);
BOOL daRaceItem_c::checkOffsetPos() {
  WWHD_FUNC(0x0245A000, BOOL, this);
  return !(mFlags & 1) && !(actor_status & 0x100000);
}
VERIFY(0x0245A000, &daRaceItem_c::checkOffsetPos);
void daRaceItem_c::set_mtx(cXyz *pos) {
  WWHD_FUNC(0x0245A028, void, this, pos);
  f32 sx = scale.x, sy = scale.y, sz = scale.z;
  J3DModel *model = mpModel;
  gabi::store<f32>(gabi::ea(model) + 0xBC, sx);
  gabi::store<f32>(gabi::ea(model) + 0xC0, sy);
  gabi::store<f32>(gabi::ea(model) + 0xC4, sz);
  f32 x = pos->x, y = pos->y, z = pos->z;
  mDoMtx_stack_c::transS(x, y, z);
  mDoMtx_stack_c::YrotM(current.angle.y);
  J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x0245A028,
       static_cast<void (daRaceItem_c::*)(cXyz *)>(&daRaceItem_c::set_mtx));
static void race_static_sinit() {
  WWHD_FUNC(0x0245A100, void, (u32)0);
  sinit_header_statics(0x1046D650, 0x101CF34C);
}
VERIFY(0x0245A100, race_static_sinit);
