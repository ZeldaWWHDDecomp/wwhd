/**
 * d_a_npc_ji1_battle.cpp (WWHD)
 * NPC - Orca: the battle (sword-training game) functions
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout
 * and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_battle.h"

static daNpc_Ji1_HIO_c l_hio;
using Ji1BattleLifeHD = be<u8>;
static Ji1BattleLifeHD l_gameLife;

/* ---- local bindings (SHARED-CANDIDATE) ---- */
// HD: retain the single-precision magnitude used by these native vector
// operations.
static inline f32 vabs(cXyz *v) { return std_sqrtf(PSVECSquareMag(v)); }
static inline f32 vabsXZ(cXyz *v) {
  gabi::Local<cXyz> t;
  f32 x = v->x, z = v->z;
  t->x = x;
  t->y = 0.0f;
  t->z = z;
  return std_sqrtf(PSVECSquareMag(t));
}
static inline BOOL morf_checkEnd(mDoExt_McaMorf *m, f32 d) {
  return m->checkFrame(m->getEndFrame() - d);
}

enum { JA_SE_CV_JI_ATTACK = 0x4843 };

// HD: counts beyond the last level assert, then use the final level values.
/* 02259A6C */
void daNpc_Ji1_c::battleGameSetTimer() {

  f32 temp1;
  f32 temp2;
  daNpc_Ji1_HIO_c &h = l_hio;
  s32 n = field_0xD70;
  if (n < h.field_0x60[0]) {
    f32 temp = (f32)n / (f32)(s16)h.field_0x60[0];
    f32 r = 1.0f - temp;
    temp2 =
        gabi::fmadds(temp, (f32)(s16)h.field_0x8E, r * (f32)(s16)h.field_0x8A);
    temp1 =
        gabi::fmadds(temp, (f32)(s16)h.field_0x9A, r * (f32)(s16)h.field_0x96);
  } else if (n < h.field_0x60[1]) {
    s32 a = h.field_0x60[0];
    f32 temp = (f32)(n - a) / (f32)(h.field_0x60[1] - a);
    f32 r = 1.0f - temp;
    temp2 =
        gabi::fmadds(temp, (f32)(s16)h.field_0x90, r * (f32)(s16)h.field_0x8C);
    temp1 =
        gabi::fmadds(temp, (f32)(s16)h.field_0x9C, r * (f32)(s16)h.field_0x98);
  } else if (n < h.field_0x60[2]) {
    s32 a = h.field_0x60[1];
    f32 temp = (f32)(n - a) / (f32)(h.field_0x60[2] - a);
    f32 r = 1.0f - temp;
    temp2 =
        gabi::fmadds(temp, (f32)(s16)h.field_0x92, r * (f32)(s16)h.field_0x8E);
    temp1 =
        gabi::fmadds(temp, (f32)(s16)h.field_0x9E, r * (f32)(s16)h.field_0x9A);
  } else if (n < h.field_0x60[3]) {
    s32 a = h.field_0x60[2];
    f32 temp = (f32)(n - a) / (f32)(h.field_0x60[3] - a);
    f32 r = 1.0f - temp;
    temp2 =
        gabi::fmadds(temp, (f32)(s16)h.field_0x94, r * (f32)(s16)h.field_0x8C);
    temp1 =
        gabi::fmadds(temp, (f32)(s16)h.field_0xA0, r * (f32)(s16)h.field_0x98);
  } else {
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0xFB0, "0");
    temp2 = (f32)(s16)h.field_0x94;
    temp1 = (f32)(s16)h.field_0xA0;
  }
  field_0xC30 = gabi::ftoi(temp2 + cM_rndF(temp1));
}

/* 02259EC4 */
BOOL daNpc_Ji1_c::battleMove(f32 param_1) {

  fopAc_ac_c *player = daPy_getPlayerActorClass();
  gabi::Local<cXyz> temp1;
  cXyz_mi(&home.pos, temp1, &current.pos);
  gabi::Local<cXyz> temp2;
  cXyz_mi(&player->current.pos, temp2, &current.pos);
  vabs(temp1);
  f32 temp3 = vabs(temp2);
  gabi::Local<cXyz> d;
  cXyz_mi(&player->current.pos, d, &home.pos);
  vabs(d);
  cM_atan2s(temp1->x, temp1->z);
  s16 temp5 = cM_atan2s(temp2->x, temp2->z);
  BOOL temp13 = FALSE;
  BOOL temp20 = FALSE;
  gabi::Local<cXyz> a;
  cXyz_ml(&home.pos, a, 0.3f);
  gabi::Local<cXyz> b;
  cXyz_ml(&player->current.pos, b, 0.7f);
  gabi::Local<cXyz> temp8;
  cXyz_pl(a, temp8, b);
  temp8->y = 0.0f;

  if (ptmf_eq(mAction, ACT_speakBadAction)) {
    temp3 = 10.0f;
  } else {
    if (temp3 > 150.0f) {
      temp3 = gabi::fmadds(5.0f, temp3 / 150.0f - 1.0f, 0.5f);
    } else {
      f32 px = player->current.pos.x;
      f32 py = player->current.pos.y;
      f32 pz = player->current.pos.z;
      f32 s = cM_ssin(temp5);
      f32 c = cM_scos(temp5);
      temp8->x = gabi::fnmsubs(150.0f, s, px);
      temp8->y = py;
      temp8->z = gabi::fnmsubs(150.0f, c, pz);
      temp3 = 2.5f;
    }
  }

  gabi::Local<cXyz> diff;
  cXyz_mi(temp8, diff, &current.pos);
  if (vabsXZ(diff) > 25.0f) {
    cLib_addCalcPos2(&current.pos, *temp8, 0.1f, temp3 * param_1);
    if (temp3 > 1.0f) {
      temp13 = TRUE;
    }
  }

  s16 dy = (s16)(temp5 - current.angle.y);
  if ((dy < 0 ? -dy : dy) > 0x1000) {
    temp20 = TRUE;
  }
  cLib_addCalcAngleS2(&current.angle.y, temp5, 8, 0x1000);

  if (temp20 == TRUE || temp13 == TRUE) {
    if (mAnimation == Ji1Battle_BCK_KAMAE || morf_checkEnd(mpOrcaMorf, 2.0f)) {
      setAnm(Ji1Battle_BCK_KROT, 4.0f, 0);
    }
    if (mAnimation == Ji1Battle_BCK_KROT) {
      mpOrcaMorf->setPlaySpeed(l_hio.field_0x48 * param_1);
    }
    return TRUE;
  }
  return FALSE;
}

/* 0225B344 */
BOOL daNpc_Ji1_c::battleSubActionWait() {

  if (--field_0xC30 < 0) {
    f32 dist = fopAcM_searchPlayerDistanceXZ(this);
    f32 rnd = cM_rndF(10.0f);
    if (rnd > 3.3f || dist > 100.0f) {
      if (rnd > 6.6f || dist < 150.0f) {
        battleSubActionYokoAttackInit();
      } else {
        battleSubActionTateAttackInit();
      }
    } else {
      battleSubActionAttackInit();
    }
    battleGameSetTimer();
  }
  if (battleMove(1.5f) == FALSE && morf_checkEnd(mpOrcaMorf, 2.0f)) {
    setAnm(Ji1Battle_BCK_KAMAE, 4.0f, 0);
  }
  return TRUE;
}

/* 0225C208 */
BOOL daNpc_Ji1_c::battleSubActionNockBack() {

  s16 temp =
      cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
  if (mpOrcaMorf->checkFrame(1.0f)) {
    setAnm(Ji1Battle_BCK_KAMAE, 4.0f, 0);
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);
    field_0xD6C = 0;
    battleSubActionWaitInit();
    return FALSE;
  }
  cLib_addCalc2(&field_0xC9C, 50.0f, 0.2f, 10.0f);
  gabi::Local<cXyz> temp2;
  f32 c9c = field_0xC9C;
  f32 x = field_0xD38.x;
  f32 y = field_0xD38.y;
  f32 s = cM_ssin(temp);
  f32 c = cM_scos(temp);
  f32 z = field_0xD38.z;
  temp2->x = gabi::fnmsubs(c9c, s, x);
  temp2->y = y;
  temp2->z = gabi::fnmsubs(c9c, c, z);
  ji1BattleAddCalcPosXZ2HD(&current.pos, temp2, 0.25f, 5.0f);
  return TRUE;
}

/* ---- attack sub-actions (common inline parts) ---- */
enum { JA_SE_CM_JI_SWING = 0x594C };
/* setBtAttackData(start, end, maxDis, type), setBtNowFrame(frame) */
static inline void setBtAttackData(daNpc_Ji1_c *a, f32 start, f32 end,
                                   f32 maxDis, u8 type) {
  a->mBtStartFrame = start;
  a->mBtEndFrame = end;
  a->mBtMaxDis = maxDis;
  a->mBtAttackType = type;
}
/* dCcD_GObjInf: OnTgShield / ChkTgShieldHit on the dCcD_GObjTg SPrm (+0x94) /
 * RPrm (+0x98) */
static inline void OnTgShield(dCcD_GObjInf *c) { c->mGObjTg.mSPrm |= 1; }
static inline bool ChkTgShieldHit(dCcD_GObjInf *c) {
  return (c->mGObjTg.mRPrm & 2) != 0;
}
/* the attack prologue; returns TRUE when the attack ended (back to wait) */
static inline BOOL attack_begin(daNpc_Ji1_c *a, f32 frame, f32 start, f32 end,
                                u8 type) {
  setBtAttackData(a, start, end, 500.0f, type);
  a->mBtNowFrame = frame;
  if (a->isAttackFrame() <= 0) {
    OnTgShield(&a->field_0x7E0);
  }
  if (morf_checkEnd(a->mpOrcaMorf, 1.0f)) {
    a->setAnm(Ji1Battle_BCK_KAMAE, 4.0f, 0);
    a->battleSubActionWaitInit();
    return TRUE;
  }
  return FALSE;
}
/* the play speed (guard knock-back timer, then fast/slow by frame) */
static inline void attack_speed(daNpc_Ji1_c *a, f32 limit, be<f32> &fast,
                                be<f32> &slow) {
  if (ji1BattleCalcTimerHD(&a->field_0xC34) > 0) {
    a->mpOrcaMorf->setPlaySpeed(l_hio.field_0x84);
    s32 t = a->field_0xC34;
    a->field_0xBA4 = (s16)(t * 0x2800);
    a->field_0xBA2 = (s16)(t * 0x100);
  } else {
    if (a->mpOrcaMorf->getFrame() > limit) {
      a->mpOrcaMorf->setPlaySpeed(fast);
    } else {
      a->mpOrcaMorf->setPlaySpeed(slow);
      if (ChkTgShieldHit(&a->field_0x7E0)) {
        a->setGuardParticle();
        a->field_0xC34 = (s16)l_hio.field_0x88;
        a->mpOrcaMorf->setPlaySpeed(l_hio.field_0x84);
      }
    }
  }
}

/* 0225BF44 */
BOOL daNpc_Ji1_c::battleSubActionAttack() {

  s16 temp =
      cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
  f32 frame = mpOrcaMorf->getFrame();
  if (attack_begin(this, frame, 0.0f, 12.0f, 2)) {
    return FALSE;
  }
  attack_speed(this, 10.0f, l_hio.field_0x70, l_hio.field_0x6C);
  if (frame > 12.0f) {
    if (frame < 28.0f) {
      gabi::Local<cXyz> temp2;
      f32 s = cM_ssin(current.angle.y);
      f32 c = cM_scos(current.angle.y);
      temp2->x = gabi::fmadds(100.0f, s, field_0xD38.x);
      temp2->y = (f32)field_0xD38.y;
      temp2->z = gabi::fmadds(100.0f, c, field_0xD38.z);
      battleAtSet();
      cLib_addCalcPos2(&current.pos, *temp2, 0.25f, 20.0f);
    }
  } else {
    cLib_addCalcAngleS2(&current.angle.y, temp, 4, 0x800);
    field_0xD38.copy(current.pos);
  }
  return TRUE;
}

/* 0225B85C */
BOOL daNpc_Ji1_c::battleSubActionTateAttack() {

  s16 temp =
      cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
  f32 frame = mpOrcaMorf->getFrame();
  if (attack_begin(this, frame, 5.0f, 20.0f, 2)) {
    return FALSE;
  }
  attack_speed(this, 26.0f, l_hio.field_0x74, l_hio.field_0x78);
  if (frame > 28.0f && frame < 35.0f) {
    battleAtSet();
    field_0x320.copy(field_0xB78);
    field_0x320.y = 0.0f;
  } else if (frame < 23.0f) {
    cLib_addCalcAngleS2(&current.angle.y, temp, 4, 0x400);
    field_0xD38.copy(current.pos);
  }
  if (mpOrcaMorf->checkFrame(35.0f)) {
    setParticleAT(1, 10.0f, 0.5f);
    ji1_seStart(this, JA_SE_CM_JI_SWING, 0);
  } else if (mpOrcaMorf->checkFrame(40.0f)) {
    dtParticleAT();
  }
  return TRUE;
}

/* 0225BBA4 */
BOOL daNpc_Ji1_c::battleSubActionYokoAttack() {

  s16 temp =
      cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
  f32 frame = mpOrcaMorf->getFrame();
  if (attack_begin(this, frame, 0.0f, 20.0f, 1)) {
    return FALSE;
  }
  attack_speed(this, 25.0f, l_hio.field_0x74, l_hio.field_0x78);
  if (frame > 25.0f && frame < 35.0f) {
    battleAtSet();
    f32 x = current.pos.x;
    f32 y = current.pos.y;
    f32 z = current.pos.z;
    s16 ang = (s16)(field_0x32C - 0x1400);
    field_0x320.x = x;
    field_0x320.y = y;
    field_0x320.z = z;
    field_0x32C = ang;
    field_0x320.x = gabi::fmadds(200.0f, cM_ssin(ang), x);
    field_0x320.z = gabi::fmadds(200.0f, cM_scos(ang), z);
  } else if (frame < 15.0f) {
    cLib_addCalcAngleS2(&current.angle.y, temp, 4, 0x600);
    field_0xD38.copy(current.pos);
  }
  if (mpOrcaMorf->checkFrame(25.0f)) {
    field_0x320.copy(field_0xB78);
    field_0x320.y = 0.0f;
    field_0x32C = cLib_targetAngleY(&current.pos, &field_0x320);
    ji1_seStart(this, JA_SE_CM_JI_SWING, 0);
    setParticleAT(0xF, 10.0f, 0.5f);
  } else if (mpOrcaMorf->checkFrame(40.0f)) {
    dtParticleAT();
  }
  return TRUE;
}

/* ---- moves back along the facing direction ---- */
/* temp = field_0xD38; temp.x -= field_0xC9C * sin(angle.y); temp.z -=
 * field_0xC9C * cos(angle.y) */
static inline void back_pos(daNpc_Ji1_c *a, cXyz *temp) {
  s16 ang = a->current.angle.y;
  f32 c9c = a->field_0xC9C;
  f32 x = a->field_0xD38.x;
  f32 y = a->field_0xD38.y;
  f32 z = a->field_0xD38.z;
  temp->x = gabi::fnmsubs(c9c, cM_ssin(ang), x);
  temp->y = y;
  temp->z = gabi::fnmsubs(c9c, cM_scos(ang), z);
}
/* end of a sub-action: back to waiting */
static inline void back_to_wait(daNpc_Ji1_c *a) {
  a->field_0xD6C = 0;
  a->field_0xC9C = 0.0f;
  a->field_0xD38.copy(a->current.pos);
  a->setAnm(Ji1Battle_BCK_KAMAE, 4.0f, 0);
  a->battleSubActionWaitInit();
}
enum { JA_SE_CM_JI_SLIP = 0x5021 };

/* 0225C358 */
BOOL daNpc_Ji1_c::battleSubActionJump() {

  gabi::Local<cXyz> temp;
  cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, temp, &current.pos);
  s16 temp2 = cM_atan2s(temp->x, temp->z);
  if (mAnimation == Ji1Battle_BCK_JPGUARD) {
    cLib_addCalc2(&field_0xC9C, 50.0f, 0.25f, 50.0f);
    cLib_addCalcAngleS2(&current.angle.y, temp2, 4, 0x1000);
    gabi::Local<cXyz> temp3;
    back_pos(this, temp3);
    ji1BattleAddCalcPosXZHD(&current.pos, temp3, 0.8f, 10.0f, 1.0f);
    if (morf_checkEnd(mpOrcaMorf, 1.0f)) {
      back_to_wait(this);
    }
  } else {
    cLib_addCalc2(&field_0xC9C, 180.0f, 0.25f, 50.0f);
    cLib_addCalcAngleS2(&current.angle.y, temp2, 4, 0x1000);
    gabi::Local<cXyz> temp3;
    back_pos(this, temp3);
    f32 temp4 = 25.0f;
    if (mAcch.ChkWallHit()) {
      temp4 = 10.0f;
    }
    ji1BattleAddCalcPosXZHD(&current.pos, temp3, 0.8f, temp4, 1.0f);
    if (morf_checkEnd(mpOrcaMorf, 10.0f)) {
      field_0xC9C = 0.0f;
      field_0xD38.copy(current.pos);
      setAnm(Ji1Battle_BCK_JPGUARD, 0.0f, 0);
      // HD: restarting the jump animation also resets its current frame.
      mpOrcaMorf->mFrameCtrl.mStart = 0;
      mpOrcaMorf->mFrameCtrl.mFrame = 0.0f;
      mpOrcaMorf->mFrameCtrl.mEnd = 20;
      setParticle(3, 3.0f, 0.75f);
    }
  }
  return TRUE;
}

/* 0225CA04 */
BOOL daNpc_Ji1_c::battleSubActionDamage() {

  gabi::Local<cXyz> temp;
  cXyz_mi(&dComIfGp_getPlayer(0)->current.pos, temp, &current.pos);
  s16 temp2 = cM_atan2s(temp->x, temp->z);
  cLib_addCalc2(&field_0xC9C, 180.0f, 0.25f, 50.0f);
  gabi::Local<cXyz> temp3;
  back_pos(this, temp3);
  f32 temp4 = 25.0f;
  if (mAcch.ChkWallHit()) {
    temp4 = 10.0f;
  }
  ji1BattleAddCalcPosXZHD(&current.pos, temp3, 0.8f, temp4, 1.0f);
  if (morf_checkEnd(mpOrcaMorf, 20.0f)) {
    setParticle(3, 3.0f, 0.75f);
  }
  if (mpOrcaMorf->getFrame() < 15.0f) {
    cLib_addCalcAngleS2(&current.angle.y, temp2, 4, 0x1800);
  }
  if (!playerCutAtCheck()) {
    field_0xD6C = 0;
  }
  if (morf_checkEnd(mpOrcaMorf, 1.0f)) {
    back_to_wait(this);
  }
  return TRUE;
}

/* 0225C660 */
BOOL daNpc_Ji1_c::battleSubActionJpGuard() {

  cLib_addCalc2(&field_0xC9C, l_hio.field_0x34, 0.25f, l_hio.field_0x38);
  gabi::Local<cXyz> temp;
  back_pos(this, temp);
  f32 temp2 = 25.0f;
  if (mAcch.ChkWallHit()) {
    temp2 = 10.0f;
  }
  cLib_addCalcPos2(&current.pos, *temp, 0.8f, temp2);
  ji1_seStart(this, JA_SE_CM_JI_SLIP, 0);
  if (playerCutAtCheck() == FALSE) {
    field_0xD6C = 0;
  }
  if (morf_checkEnd(mpOrcaMorf, 1.0f)) {
    back_to_wait(this);
  }
  return TRUE;
}

/* 0225C7EC */
BOOL daNpc_Ji1_c::battleSubActionGuard() {

  cLib_addCalc2(&field_0xC9C, 70.0f, 0.35f, l_hio.field_0x40);
  gabi::Local<cXyz> temp;
  s16 ang = current.angle.y;
  f32 c9c = field_0xC9C;
  f32 x = field_0xD38.x;
  f32 y = field_0xD38.y;
  f32 z = field_0xD38.z;
  temp->y = y;
  if (field_0xC24 != Ji1Battle_CUT_TYPE_BT_JUMPCUT) {
    temp->x = gabi::fnmsubs(c9c, cM_ssin(ang), x);
    temp->z = gabi::fnmsubs(c9c, cM_scos(ang), z);
  } else {
    f32 k = 0.25f * c9c;
    temp->x = gabi::fmadds(k, cM_ssin(ang), x);
    temp->z = gabi::fmadds(k, cM_scos(ang), z);
  }
  cLib_addCalcPos2(&current.pos, *temp, 0.8f, 45.0f);
  fopAc_ac_c *player = dComIfGp_getPlayer(0);
  s16 target = cLib_targetAngleY(&current.pos, &player->current.pos);
  cLib_addCalcAngleS2(&current.angle.y, target, 8, 0x400);
  if (playerCutAtCheck() == FALSE) {
    field_0xD6C = 0;
  }
  if (morf_checkEnd(mpOrcaMorf, 1.0f)) {
    back_to_wait(this);
  }
  if (field_0xC30 > 0) {
    field_0xC30 = field_0xC30 - 1;
  }
  return TRUE;
}

// HD: dCcD_Cps retains an opaque geometry array in the shared header.
static inline void cps_SetR(dCcD_Cps *c, f32 radius) {
  ji1BattleCapsuleShapeHD(c)->mRadius = radius;
}
static inline void cps_SetStartEnd(dCcD_Cps *c, const cXyz *start,
                                   const cXyz *end) {
  ji1BattleSetStartEndHD(ji1BattleCapsuleShapeHD(c), start, end);
}
static inline void cps_CalcVec(dCcD_Cps *c, cXyz *out) {
  Ji1BattleCapsuleHD *shape = ji1BattleCapsuleShapeHD(c);
  ji1BattleVectorSubtractHD(&shape->mEnd, &shape->mStart, out);
}

/* 0225B4AC */
BOOL daNpc_Ji1_c::battleAtSet() {

  gabi::Local<cXyz> temp;
  f32 sin = cM_ssin(current.angle.y);
  f32 cos = cM_scos(current.angle.y);
  f32 vx, vy, vz;
  if (mAnimation == Ji1Battle_BCK_YOKOATTACK) {
    cps_SetR(&field_0xA40, REG_F(10, 9) + 85.0f);
    f32 y = REG_F(10, 10) + 40.0f;
    field_0xB78.y = y;
    field_0xB90.y = y;
    cps_SetStartEnd(&field_0xA40, &field_0xB90, &field_0xB78);
    cps_CalcVec(&field_0xA40, temp);
    if (!ji1BattleNormalizeHD(temp)) {
      temp->set(cos, 0.0f, sin);
    }
    vx = temp->x;
    vy = temp->y;
    vz = temp->z;
  } else if (mAnimation == Ji1Battle_BCK_TATEATTACK) {
    gabi::Local<cXyz> temp2;
    f32 x = field_0xB84.x, y = field_0xB84.y, z = field_0xB84.z;
    temp2->x = x;
    temp2->y = y;
    temp2->z = z;
    gabi::Local<cXyz> temp3;
    cXyz_mi(&field_0xB78, temp3, &field_0xB84);
    PSVECScale(temp3, temp3, 0.3f);
    PSVECAdd(temp2, temp3, temp2);
    temp->set(sin, 0.0f, cos);
    cps_SetR(&field_0xA40, 50.0f);
    cps_SetStartEnd(&field_0xA40, temp2, &field_0xB78);
    vx = temp->x;
    vy = temp->y;
    vz = temp->z;
  } else {
    temp->set(sin, 0.0f, cos);
    cps_SetR(&field_0xA40, 20.0f);
    cps_SetStartEnd(&field_0xA40, &field_0xB84, &field_0xB78);
    vx = temp->x;
    vy = temp->y;
    vz = temp->z;
  }
  /* SetAtVec */
  field_0xA40.mGObjAt.mVec.x = vx;
  field_0xA40.mGObjAt.mVec.y = vy;
  field_0xA40.mGObjAt.mVec.z = vz;
  dComIfG_Ccsp_Set(&field_0xA40);
  return TRUE;
}

/* 0225AEC8 */
BOOL daNpc_Ji1_c::battleAction(void *arg) {

  fopAc_ac_c *player = dComIfGp_getPlayer(0);
  if (field_0xC78 == 0) {
    u8 icon;
    switch (dComIfGs_getSelectEquip(0)) {
    case 0x38 /* dItemNo_SWORD_e */:
      icon = 1;
      break;
    default:
      icon = 2;
      break;
    }
    ji1BattleCreateTimerHD(Ji1Battle_TIMER_SWORD_TRAINING, 1000, 2, icon,
                           221.0f, 439.0f, 32.0f, 419.0f);
    dComIfGp_setMiniGameRupee(0);
    field_0xC3C = 0;
    setAnm(Ji1Battle_BCK_KAMAE, 0.0f, 0);
    attention_info.flags |= fopAc_Attn_LOCKON_MISC_e;
    attention_info.flags |= fopAc_Attn_LOCKON_BATTLE_e;
    attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 3;
    field_0xC30 = (s16)gabi::ftoi(cM_rndF(150.0f)) + 30;
    field_0xD70 = 0;
    field_0xD6C = 0;
    battleSubActionWaitInit();
    field_0xC38 = 0;
    field_0xC78 += 1;
  } else if (field_0xC78 == -1) {
    attention_info.flags &= ~u32(fopAc_Attn_LOCKON_BATTLE_e);
    attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0xB5;
  } else {
    gabi::Local<cXyz> temp;
    cXyz_mi(&player->current.pos, temp, &current.pos);
    cM_atan2s(temp->x, temp->z);
    vabsXZ(temp);
    field_0x7E0.mGObjTg.mSPrm &= ~1u; /* OffTgShield */
    battleGuardCheck();
    ptmf_invoke(mSubAction, this);
    if (field_0xD6C != 1) {
      dtParticle();
    }
    if (!isAttackAnim() || ptmf_eq(mSubAction, SUB_battleSubActionNockBack)) {
      dtParticleAT();
    }
    s32 lp = 3 - field_0xC3C;
    if (lp < 0) {
      lp = 0;
    }
    l_gameLife = (u8)lp;
  }
  return true;
}

/* WWHD glue
ext 028E8DAC void ji1BattleVectorSubtractHD(const cXyz* a, const cXyz* b, cXyz* result)
ext 0200F268 void ji1BattleAddCalcPosXZ2HD(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep)
ext 0200EF78 void ji1BattleAddCalcPosXZHD(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep, f32 minStep)
ext 0211D2F8 s32 ji1BattleCalcTimerHD(be<s32>* timer)
ext 02018808 void ji1BattleSetStartEndHD(Ji1BattleCapsuleHD* shape, const cXyz* start, const cXyz* end)
ext 0201B47C bool ji1BattleNormalizeHD(cXyz* vector)
ext 025C60F4 void ji1BattleCreateTimerHD(s32 mode, u16 limit, u8 type, u8 icon, f32 a, f32 b, f32 c, f32 d)
data l_hio 104672AC
data l_gameLife 101D5F44
str 1001B3A8
str 1001B3A4
*/
