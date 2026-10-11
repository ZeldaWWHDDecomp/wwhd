/**
 * d_a_npc_ji1.cpp (WWHD)
 * NPC - Orca
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout
 * and verified against the WWHD code (cking.rpx). The large action functions
 * are in d_a_npc_ji1_<part>.cpp; functions not decompiled yet are weak guest
 * calls in d_a_npc_ji1_pending.cpp.
 */
#define DA_NPC_JI1_USE_SHARED_GAME_API
#include "d/actor/d_a_npc_ji1.h"
#undef DA_NPC_JI1_USE_SHARED_GAME_API

// HD: native bindings retain this unit's argument widths and call order.
void ji1_swordFlushNative(cXyz *, s32);
void ji1_smokeEndNative(dPa_smokeEcallBack_l *);
void ji1_addCalcPosXZNative(cXyz *, cXyz *, f32, f32, f32);
static cXyz out_chk_pos;
static u32 out_chk_pos_guard;
using namespace ji1Event;

enum { fpcNm_PLAYER_e = 0xA8 };
enum { dItemNo_KNIGHTS_CREST_e = 0x48 };
enum { dActStts_OPEN_e = 0xB };
enum {
  JA_SE_CV_JI_ATTACK = 0x4843,
  JA_SE_CV_JI_DEFENCE = 0x4833,
  JA_SE_OBJ_COL_SWS_NMTLP = 0x6817,
};
enum { dPa_ID_AK_JN_NG = 0xC };

/* 02248C64 */
// HD: callbacks are exported to the split unit's weak native adapters.
s16 daNpc_Ji1_XyCheckCB(void *p, int i_itemBtn) {
  return dComIfGp_getSelectItem(i_itemBtn) == dItemNo_KNIGHTS_CREST_e ? TRUE
                                                                      : FALSE;
}

/* 02248CA4 */
void daJi1_CoHitCallback(fopAc_ac_c *i_this, dCcD_GObjInf *p2,
                         fopAc_ac_c *actor, dCcD_GObjInf *p4) {
  if (actor == dComIfGp_getPlayer(0)) {
    static_cast<daNpc_Ji1_c *>(i_this)->field_0xC28 = 1;
  }
}

/* 02248CF0 */
void daJi1_TgHitCallback(fopAc_ac_c *i_this, dCcD_GObjInf *p2,
                         fopAc_ac_c *actor, dCcD_GObjInf *p4) {
  auto *player = reinterpret_cast<Ji1PlayerHD *>(daPy_getPlayerActorClass());
  u32 type = player->getCutType();
  if (actor) {
    static_cast<daNpc_Ji1_c *>(i_this)->field_0xC24 = type;
  }
}

// HD: a hit counts once per ten-frame cooldown window.
/* 02248EA0 */
void daJi1_AtHitCallback(fopAc_ac_c *i_this, dCcD_GObjInf *param_2,
                         fopAc_ac_c *actor, dCcD_GObjInf *p4) {
  if (fopAc_IsActor(actor) && actor != nullptr) {
    if (fpcM_GetName(actor) == fpcNm_PLAYER_e) {
      daNpc_Ji1_c *a = static_cast<daNpc_Ji1_c *>(i_this);
      if (a->mHitCooldownHD == 0) {
        a->mHitCooldownHD = 10;
        /* ChkAtShieldHit: mGObjAt.mRPrm bit 0 */
        if (!(param_2->mGObjAt.mRPrm & 1)) {
          a->field_0xC3C += 1;
        } else {
          a->battleSubActionNockBackInit(0);
        }
      }
    }
  }
}

/* 0224AB7C */
BOOL daNpc_Ji1_c::isGuardAnim() {
  if ((mAnimation >= Ji1_ANM_TATEGUARD && mAnimation <= Ji1_ANM_JPGUARD) ||
      mAnimation == Ji1_ANM_GUARD) {
    return true;
  }
  return false;
}

/* 0224D4F8 */
BOOL daNpc_Ji1_c::isAttackAnim() {
  if (mAnimation == Ji1_ANM_TATEATTACK || mAnimation == Ji1_ANM_YOKOATTACK ||
      mAnimation == Ji1_ANM_INASI) {
    return true;
  }
  return false;
}

/* 0224D524 */
int daNpc_Ji1_c::isAttackFrame() {
  f32 frame = mpOrcaMorf->getFrame();
  f32 temp2 = 0.0f;
  f32 temp3 = temp2;
  if (mAnimation == Ji1_ANM_TATEATTACK) {
    temp2 = 28.0f;
    temp3 = 35.0f;
  } else if (mAnimation == Ji1_ANM_YOKOATTACK) {
    temp2 = 25.0f;
    temp3 = 35.0f;
  } else {
    if (mAnimation == Ji1_ANM_INASI) {
      temp2 = 12.0f;
      temp3 = 28.0f;
    }
  }
  if (temp2 > frame) {
    return -1;
  }
  return temp3 < frame;
}

/* 0224A9F0 */
BOOL daNpc_Ji1_c::isItemWaitAnim() {
  if (mAnimation == Ji1_ANM_WAIT01 || mAnimation == Ji1_ANM_REI ||
      mAnimation == Ji1_ANM_WARAI || mAnimation == Ji1_ANM_NUGUI) {
    return true;
  }
  return false;
}

// HD: level indexes the four-entry record table modulo four.
/* 0224D5BC */
BOOL daNpc_Ji1_c::isClearRecord(s16 param_1) {
  if (dComIfGs_isEventBit(UNK_0F20)) {
    return false;
  }
  u8 level = dComIfGs_getEventReg(UNK_D003);
  if (param_1 >= l_HIO().field_0x60[level & 3]) {
    return TRUE;
  }
  return FALSE;
}

/* 0224D654 */
void daNpc_Ji1_c::setClearRecord(s16 hits) {
  u8 oldLevel = dComIfGs_getEventReg(UNK_D003);
  u8 newLevel = 0;
  while (hits >= l_HIO().field_0x60[newLevel & 3] && newLevel <= 3) {
    newLevel++;
  }
  if (oldLevel < newLevel) {
    dComIfGs_setEventReg(UNK_D003, newLevel);
    if (hits >= l_HIO().field_0x60[3]) {
      dComIfGs_onEventBit(UNK_0F20);
    }
  }
}

/* 02248F58 */
BOOL daNpc_Ji1_plRoomOutCheck() {
  fopAc_ac_c *player = daPy_getPlayerActorClass();
  cXyz plyrPos;
  plyrPos.x = (f32)player->current.pos.x;
  plyrPos.y = (f32)player->current.pos.y;
  plyrPos.z = (f32)player->current.pos.z;
  // HD: the GHS guard is stored between the y and x/z initializer writes.
  if (out_chk_pos_guard == 0) {
    out_chk_pos.y = 0.0f;
    out_chk_pos_guard = 1;
    out_chk_pos.x = 0.0f;
    out_chk_pos.z = 500.0f;
  }
  cXyz posDiff;
  cXyz_mi(&plyrPos, &posDiff, &out_chk_pos);
  cXyz xz;
  xz.x = (f32)posDiff.x;
  xz.y = 0.0f;
  xz.z = (f32)posDiff.z;
  // HD: keep the rounded f32 magnitude; the shared cXyz helper returns f64.
  if (std_sqrtf(PSVECSquareMag(&xz)) < 100.0f &&
      dComIfGp_getDoStatus() == dActStts_OPEN_e) {
    return true;
  }
  return false;
}

/* 0224DA20 */
u32 daNpc_Ji1_c::getMsg() {
  if (dComIfGs_isEventBit(UNK_0520)) {
    return getMsg2ndType();
  }
  return getMsg1stType();
}

/* 0224FB98 */
void daNpc_Ji1_c::dtParticle() {
  if (mSmokeCb.mpEmitter.get()) {
    ji1_smokeEndNative(&mSmokeCb);
  }
}

/* 0225AEB4 */
void daNpc_Ji1_c::dtParticleAT() {
  if (mSmokeCbAT.mpEmitter.get()) {
    ji1_smokeEndNative(&mSmokeCbAT);
  }
}

/* 0224F528 */
f32 daNpc_Ji1_c::calcCoCorrectValue() {
  cXyz temp;
  temp.x = cM_ssin(current.angle.y);
  temp.y = 0.0f;
  temp.z = cM_scos(current.angle.y);
  f32 d = PSVECDotProduct(&field_0x638.m_cc_move, &temp);
  f32 v = field_0xC4C + d;
  field_0xC4C = v;
  return v;
}

/* 0224F59C */
f32 daNpc_Ji1_c::calcBgCorrectValue() {
  cXyz temp;
  temp.x = cM_ssin(current.angle.y);
  temp.y = 0.0f;
  temp.z = cM_scos(current.angle.y);
  if (mAcch.ChkWallHit()) {
    f32 d = PSVECDotProduct(&field_0xC40, &temp);
    f32 v = field_0xC50 + d;
    field_0xC50 = v;
    return v;
  }
  return field_0xC50;
}

/* 0225589C */
void daNpc_Ji1_c::teachSubActionAttackInit() {
  setAnm(Ji1_ANM_INASI, 0.0f, 0);
  mpOrcaMorf->setPlaySpeed(0.4f);
  ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
}

/* 022552E4 */
void daNpc_Ji1_c::teachSubActionJumpInit() {
  setAnm(Ji1_ANM_TOKAMAE, 0.0f, 1);
  field_0xC9C = 0.0f;
  field_0xD38.copy(current.pos);
}

/* 02249058 */
u32 playerCutAtCheck() {
  auto *player = reinterpret_cast<Ji1PlayerHD *>(daPy_getPlayerActorClass());
  return player->getCutAtFlg();
}

/* 0225A2C8 */
void daNpc_Ji1_c::battleSubActionWaitInit() {
  field_0xC9C = 0.0f;
  ji1_setSubAction(this, SUB_battleSubActionWait);
}

/* 02248E0C */
void daNpc_Ji1_c::battleSubActionNockBackInit(int param_1) {
  if (param_1) {
    setGuardParticle();
  }
  field_0xC9C = 0.0f;
  field_0xD38.copy(current.pos);
  mpOrcaMorf->setPlaySpeed(-1.0f);
  ji1_setSubAction(this, SUB_battleSubActionNockBack);
  mBtNowFrame = 30.0f; /* setBtNowFrame */
}

/* 0225B2B8 */
void daNpc_Ji1_c::battleSubActionAttackInit() {
  setAnm(Ji1_ANM_INASI, 0.0f, 0);
  mpOrcaMorf->setPlaySpeed(l_HIO().field_0x6C);
  ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
  field_0xC34 = 0;
  ji1_setSubAction(this, SUB_battleSubActionAttack);
}

/* 0225B22C */
void daNpc_Ji1_c::battleSubActionTateAttackInit() {
  setAnm(Ji1_ANM_TATEATTACK, 0.0f, 0);
  mpOrcaMorf->setPlaySpeed(l_HIO().field_0x78);
  ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
  field_0xC34 = 0;
  ji1_setSubAction(this, SUB_battleSubActionTateAttack);
}

/* 0225B1A0 */
void daNpc_Ji1_c::battleSubActionYokoAttackInit() {
  setAnm(Ji1_ANM_YOKOATTACK, 0.0f, 0);
  mpOrcaMorf->setPlaySpeed(l_HIO().field_0x80);
  ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
  field_0xC34 = 0;
  ji1_setSubAction(this, SUB_battleSubActionYokoAttack);
}

/* 0225A2F4 */
void daNpc_Ji1_c::battleSubActionJumpInit() {
  setAnm(Ji1_ANM_TOKAMAE, 0.0f, 1);
  field_0xC9C = 0.0f;
  field_0xD38.copy(current.pos);
  ji1_setSubAction(this, SUB_battleSubActionJump);
}

/* 0225A384 */
void daNpc_Ji1_c::battleSubActionDamageInit() {
  setAnm(Ji1_ANM_DAMAGE, 0.0f, 1);
  mpOrcaMorf->setPlaySpeed(1.5f);
  field_0xC9C = 0.0f;
  field_0xD38.copy(current.pos);
  ji1_setSubAction(this, SUB_battleSubActionDamage);
}

/* 0225A4A4 */
void daNpc_Ji1_c::battleSubActionJpGuardInit() {
  setAnm(Ji1_ANM_JPGUARD, 0.0f, 1);
  field_0xC9C = 0.0f;
  field_0xD38.copy(current.pos);
  ji1_setSubAction(this, SUB_battleSubActionJpGuard);
}

/* 0225A424 */
void daNpc_Ji1_c::battleSubActionGuardInit() {
  field_0xC9C = 0.0f;
  field_0xD38.copy(current.pos);
  setParticle(0x10, 1.0f, 0.1f);
  ji1_setSubAction(this, SUB_battleSubActionGuard);
}

/* 02249880 */
static BOOL daNpc_Ji1_Draw(daNpc_Ji1_c *i_this) { return i_this->_draw(); }

/* 0224A2B4 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) {
  return static_cast<daNpc_Ji1_c *>(i_this)->CreateHeap();
}

/* 02248D3C */
void daNpc_Ji1_c::setGuardParticle() {
  dComIfGp_particle_set(dPa_ID_AK_JN_NG, field_0x7E0.GetTgHitPosP());
  ji1_seStart(this, JA_SE_OBJ_COL_SWS_NMTLP, 0);
  ji1_seStart(this, JA_SE_CV_JI_DEFENCE, 0);
  cXyz pos;
  cXyz *hit = field_0x7E0.GetTgHitPosP();
  pos.x = (f32)hit->x;
  pos.y = (f32)hit->y;
  pos.z = (f32)hit->z;
  ji1_swordFlushNative(&pos, 0);
}

/* 0224DE14 */
void daNpc_Ji1_c::BackSlideInit() {
  field_0xC9C = 0.0f;
  field_0xD38.copy(current.pos);
}

/* 022525B4 */
void daNpc_Ji1_c::BackSlide(f32 param_1, f32 param_2) {
  cLib_addCalc2(&field_0xC9C, param_1, 0.25f, 50.0f);
  cXyz temp;
  f32 x = field_0xD38.x;
  f32 c9c = field_0xC9C;
  f32 y = field_0xD38.y;
  f32 s = cM_ssin(current.angle.y);
  f32 c = cM_scos(current.angle.y);
  f32 z = field_0xD38.z;
  temp.x = gabi::fnmsubs(c9c, s, x);
  temp.y = y;
  temp.z = gabi::fnmsubs(c9c, c, z);
  if (mAcch.ChkWallHit()) {
    param_2 *= 0.35f;
  }
  ji1_addCalcPosXZNative(&current.pos, &temp, 0.8f, param_2, 1.0f);
}

/* 0224C524 */
static BOOL daNpc_Ji1_Execute(daNpc_Ji1_c *i_this) {
  i_this->_execute();
  return true;
}

/* 0224C548 */
static BOOL daNpc_Ji1_IsDelete(daNpc_Ji1_c *i_this) { return true; }

/* 0224C64C */
static BOOL daNpc_Ji1_Delete(daNpc_Ji1_c *i_this) { return i_this->_delete(); }

/* 0224D4F4 */
static cPhs_State daNpc_Ji1_Create(fopAc_ac_c *i_this) {
  return static_cast<daNpc_Ji1_c *>(i_this)->_create();
}

// HD: empty HIO message virtual.
/* 0225D0D0 */
void daNpc_Ji1_HIO_c::genMessage(void *ctx) {}
/* WWHD glue
data out_chk_pos 10467258
data out_chk_pos_guard 104673AC
ext 0255F554 void ji1_swordFlushNative(cXyz* position, s32 parameter)
ext 025A5F88 void ji1_smokeEndNative(dPa_smokeEcallBack_l* callback)
ext 0200EF78 void ji1_addCalcPosXZNative(cXyz* position, cXyz* target, f32 scale, f32 maxStep, f32 minStep)
*/
