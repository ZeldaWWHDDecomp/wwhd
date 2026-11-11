/**
 * d_a_npc_ji1_exec.cpp (WWHD)
 * NPC - Orca: _create (with the inlined constructor), CreateInit, _execute
 * (with daNpc_Ji1_setHairAngle inlined), _delete, the HIO constructor, __sinit
 * and the deleting destructors.
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout
 * and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_exec.h"

static daNpc_Ji1_HIO_c l_hio;
static cXyz l_head_front;
static cXyz l_head_top;
static Ji1ExecByteHD l_replyFlag;
static Ji1ExecHeaderWordsHD l_headerWords;
static Ji1ExecFloatHD l_angleMin;
static Ji1ExecFloatHD l_angleMax;
static Ji1ExecByteHD l_headerByte;
static Ji1ExecShortHD l_headerShort;
static Ji1ExecGlobalRecordHD l_headerRecord;
static Ji1ExecGlobalRecordHD l_byteRecord;
static Ji1ExecGlobalRecordHD l_shortRecord;
static dCcD_SrcCyl l_cyl_src;
static dCcD_SrcCyl l_cyl2_src;
static dCcD_SrcCyl l_cylAt_src;
static Ji1ExecByteHD l_cpsAt_src;
static Ji1ExecWordHD l_actorVtable;
static Ji1ExecWordHD l_hioVtable;
static Ji1ExecWordHD l_objAcchVtable10;
static Ji1ExecWordHD l_objAcchVtable14;
static Ji1ExecWordHD l_objAcchVtable20;
static Ji1ExecWordHD l_aabVtable;
static Ji1ExecWordHD l_shapeVtable;
static Ji1ExecWordHD l_capsuleVtable;
static Ji1ExecWordHD l_hitInfoVtable;
static Ji1ExecWordHD l_capsuleAttrVtable;
static cXyz l_itemAimOffset;
static csXyz l_itemAimAngle;
static cXyz l_normalAimOffset;
static csXyz l_normalAimAngle;
static Ji1ExecWordHD l_itemAimOffsetGuard;
static Ji1ExecWordHD l_itemAimAngleGuard;
static Ji1ExecWordHD l_normalAimOffsetGuard;
static Ji1ExecWordHD l_normalAimAngleGuard;

/* 0224C550 */
BOOL daNpc_Ji1_c::_delete() {

  if (dComIfGp_getMiniGameType() == 2) {
    dComIfGp_endMiniGame(2);
  }
  if (dComIfGp_getMiniGameType() == 6) {
    dComIfGp_endMiniGame(0x20);
  }
  dComIfG_resDelete(&mPhs, "Ji");
  if (heap.get() && mpOrcaMorf.get()) {
    mpOrcaMorf->stopZelAnime();
  }
  mDoAud_seDeleteObject(&mSpearTipPos);
  mDoAud_seDeleteObject(&mSpearButtPos);
  dComIfGp_att_revivalAleart();
  reinterpret_cast<Ji1ExecCallbackHD *>(&mSmokeCb)->remove();
  reinterpret_cast<Ji1ExecCallbackHD *>(&mSmokeCbAT)->remove();
  return true;
}

// HD: the actor constructor is inlined here, including per-TU vtables.
/* 0224D280 */
cPhs_State daNpc_Ji1_c::_create() {

  u32 cond = actor_condition;
  if (!(cond & fopAcCnd_INIT_e)) {
    if (this != nullptr) {
      fopAc_ac_c_ct(this);
      __vtbl = gabi::ea(&l_actorVtable);
      ji1ExecSmokeConstructHD(&mSmokeCb, 1); /* dPa_smokeEcallBack(u8) */
      ji1ExecSmokeConstructHD(&mSmokeCbAT, 1);
      ji1ExecCutConstructHD(&mEventCut);  /* dNpc_EventCut_c */
      ji1ExecJointConstructHD(&m_jnt);    /* dNpc_JntCtrl_c */
      ji1ExecBtpConstructHD(&mBlinkAnim); /* mDoExt_btpAnm */
      ji1ExecBrkConstructHD(&mCryBrk);    /* mDoExt_brkAnm */
      ji1ExecBtkConstructHD(&mCryBtk);    /* mDoExt_btkAnm */
      ji1ExecConstructAcchHD(&mAcch, gabi::ea(&l_objAcchVtable10),
                             gabi::ea(&l_objAcchVtable14),
                             gabi::ea(&l_objAcchVtable20));
      dBgS_AcchCir_ct(&mAcchCir);
      dCcD_Stts_ct(&mStts);
      dCcD_Stts_ct(&mGuardStts);
      dCcD_Cyl_ct(&mCyl, gabi::ea(&l_aabVtable));
      dCcD_Cyl_ct(&mGuardCyl, gabi::ea(&l_aabVtable));
      dCcD_Cyl_ct(&mAtCyl, gabi::ea(&l_aabVtable));
      /* dCcD_Cps: GObjInf, shape attr, cM3dGAab (per TU), cM3dGCps */
      auto *capsule = ji1ExecCapsuleHD(&mAtCps);
      ji1ExecHitInfoConstructHD(&mAtCps);
      capsule->mAabVtable = gabi::ea(&l_shapeVtable);
      capsule->mShapeVtable = gabi::ea(&l_aabVtable);
      ji1ExecLineConstructHD(&capsule->mCps);
      capsule->mCps.__vtbl = gabi::ea(&l_capsuleVtable);
      mAtCps.__vtbl_hitinf = gabi::ea(&l_hitInfoVtable);
      capsule->mAabVtable = gabi::ea(&l_capsuleAttrVtable);
      auto *head = reinterpret_cast<dNpc_HeadAnm_c *>(mHeadAnm);
      head->mSwingAngle.x = 0;
      head->mSwingAngle.y = 0;
      head->mSwingAngle.z = 0;
      head->mVerticalAmplitude = 0.0f;
      head->mHorizontalAmplitude = 0.0f;
      head->mSwingPhase = 0;
      head->mSwingPhaseStep = 0;
      head->mSwingCount = 0;
      cond = actor_condition;
    }
    actor_condition = cond | fopAcCnd_INIT_e;
  }

  cPhs_State state = dComIfG_resLoad(&mPhs, "Ji");
  if (state == cPhs_COMPLEATE_e) {
    if (!fopAcM_entrySolidHeap(this, ji1ExecHeapCallbackHD, 0x16840)) {
      return cPhs_ERROR_e;
    }
    CreateInit();
  }
  return state;
}

/* the common tail of CreateInit's branches */
static inline BOOL CreateInit_tail(daNpc_Ji1_c *i_this) {
  dComIfGp_att_offAleart();
  ji1ExecSetActorInfoHD(&i_this->mEventCut, "Ji1", i_this);
  i_this->mEventCut.mpJntCtrl = &i_this->m_jnt;
  i_this->mEventNo = 0x12;
  i_this->field_0xD7B = 0;
  i_this->field_0xD7C = 0;
  i_this->field_0xD7E = 0;
  i_this->attention_info.flags |= fopAc_Attn_LOCKON_MISC_e;
  i_this->attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA9;
  i_this->attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA9;
  i_this->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0xB5;
  i_this->mCryBrkFrame = 0.0f;
  i_this->mCryBtkFrame = 0.0f;
  i_this->mpTearEmitter = nullptr;
  i_this->set_mtx();
  // HD: creation also clears the shared reply flag.
  l_replyFlag = 0;
  return true;
}

/* 0224C650 */
BOOL daNpc_Ji1_c::CreateInit() {

  gabi::Local<cXyz> temp;
  temp->x = 190.0f;
  temp->y = 230.0f;
  temp->z = -1100.0f;
  gabi::Local<csXyz> temp2;
  csXyz_ct(temp2, 0, 0, 0);
  ji1ExecCreateChildHD("Kmon", mBsPcId, 0, temp, fopAcM_GetRoomNo(this), temp2,
                       nullptr, 0);
  cullMtx = gabi::ea(
      J3DModel_getBaseTRMtx(mpOrcaMorf->getModel())); /* fopAcM_SetMtx */
  fopAcM_setCullSizeBox(this, -70.0f, 0.0f, -70.0f, 70.0f, 200.0f, 70.0f);
  gravity = -30.0f;
  attention_info.flags = fopAc_Attn_LOCKON_MISC_e | fopAc_Attn_LOCKON_TALK_e |
                         fopAc_Attn_ACTION_SPEAK_e;

  mStts.Init(0xFF, 0xFF, this);
  mGuardStts.Init(0xFF, 0xFF, this);
  mCyl.Set(&l_cyl_src);
  mCyl.SetStts(&mStts);
  mGuardCyl.Set(&l_cyl2_src);
  mGuardCyl.SetStts(&mGuardStts);
  mGuardCyl.mGObjTg.mHitCallback =
      ji1ExecTgHitCallbackHD; /* daJi1_TgHitCallback */
  mGuardCyl.mGObjCo.mHitCallback =
      ji1ExecCoHitCallbackHD;       /* daJi1_CoHitCallback */
  mCyl.mGObjTg.mSPrm &= ~1u; /* OffTgShield */
  mGuardCyl.mGObjTg.mSPrm |= 1u;  /* OnTgShield */
  mAtCyl.Set(&l_cylAt_src);
  mAtCyl.SetStts(&mStts);
  ji1ExecSetCapsuleHD(&mAtCps, &l_cpsAt_src);
  mAtCps.SetStts(&mStts);
  ji1ExecSetStartEndHD(&ji1ExecCapsuleHD(&mAtCps)->mCps, &mSpearButtPos,
                       &mSpearTipPos);
  ji1ExecCapsuleHD(&mAtCps)->mCps.mRadius = 50.0f;
  mAtCps.mGObjAt.mHitCallback =
      ji1ExecAtHitCallbackHD; /* daJi1_AtHitCallback */

  mBtHeight = 240.0f; /* initBt */
  mBtBodyR = 100.0f;
  mAnimation = -1;
  field_0xD68 = 0;
  mSubActionPhase = 0;
  mTeachStep = 0;
  mGuardCount = 0;
  mFirstMsgDone = 0;
  field_0xD79 = 0;
  field_0xC94 = 0;
  mTeachResult = 0;
  mTeachHitsLeft = l_hio.mTeachHitCount[mTeachStep];
  mCoHit = 0;
  mAttackEndWait = 0;
  mCoCorrectSum = 0.0f;
  mBgCorrectSum = 0.0f;
  mBackSlideDist = 0.0f;
  mBackSlideStartPos.copy(current.pos);

  if (dComIfGs_isEventBit(0x0520)) {
    if (!dComIfGs_isEventBit(0x0002)) {
      mHarpoonState = Ji1Exec_HARPOON_HIDDEN;
      mHeldItemOffset.set(-13.0f, -4.0f, 10.0f);
      mHeldItemAngle.x = 0x4000;
      mHeldItemAngle.y = 0;
      mHeldItemAngle.z = 0x7FFF;
      current.pos.x = 0.0f;
      current.pos.y = 0.0f;
      current.pos.z = -850.0f;
      current.angle.y = -0x8000;
      ji1_setAction(this, ACT_kaitenExpAction, nullptr);
    } else {
      mHarpoonState = Ji1Exec_HARPOON_HELD;
      mHeldItemOffset.set(-13.0f, -4.0f, 10.0f);
      mHeldItemAngle.x = 0x4000;
      mHeldItemAngle.y = 0;
      mHeldItemAngle.z = 0x7FFF;
      ji1_setAction(this, ACT_normalAction, nullptr);
    }
    mInitPos.copy(home.pos);
  } else {
    if (dComIfGs_isEventBit(0x0501) || dComIfGs_isEventBit(0x0001)) {
      mHarpoonState = Ji1Exec_HARPOON_HELD;
      mHeldItemOffset.set(-13.0f, -4.0f, 10.0f);
      mHeldItemAngle.x = 0x4000;
      mHeldItemAngle.y = 0;
      mHeldItemAngle.z = 0x7FFF;
      mInitPos.copy(home.pos);
      ji1_setAction(this, ACT_normalAction, nullptr);
    } else {
      mHarpoonState = Ji1Exec_HARPOON_HIDDEN;
      if (!dComIfGs_isEventBit(0x0640) || l_hio.mbRepeatKaiten) {
        field_0xD68 = 0;
        ji1_setAction(this, ACT_kaitenExpAction, nullptr);
      } else {
        ji1_setAction(this, ACT_kaitenwaitAction, nullptr);
      }
      current.pos.x = 0.0f;
      current.pos.y = 0.0f;
      current.pos.z = -850.0f;
      current.angle.y = -0x8000;
      mInitPos.copy(current.pos);
      home.angle.y = -0x8000;
    }
  }
  return CreateInit_tail(this);
}

/* (s16)(f32 * 0.2f) of an s16 difference */
static inline s16 tenth2(s32 d) { return (s16)gabi::ftoi((f32)(s16)d * 0.2f); }
/* cLib_minMaxLimit<s16> on int-extended limits */
static inline s16 minMax(s32 v, s32 lo, s32 hi) {
  if (v < lo)
    return (s16)lo;
  if (hi < v)
    return (s16)hi;
  return (s16)v;
}

/* daNpc_Ji1_setHairAngle (inlined in _execute) */
static inline void daNpc_Ji1_setHairAngle(daNpc_Ji1_c *i_this) {
  be<f32> *wp1 = ji1ExecWindPowerHD();
  be<f32> *wp2 = ji1ExecWindPowerHD();
  f32 w2 = *wp2;
  f32 w1 = *wp1;
  f32 wind = w1 * w2 * 25.0f;
  cXyz *windVec = ji1ExecWindVectorHD();

  J3DModel *pModel = i_this->mpOrcaMorf->getModel();
  {
    pModel->mpMtxBuffer->mFlags |= 0x10;
  }
  gabi::Local<cXyz> temp2;
  gabi::Local<cXyz> temp;
  {
    s8 head = i_this->m_jnt.mHeadJntNum;
    Mtx34 *m = ji1ExecGetAnmMtxHD(i_this->mpOrcaMorf->getModel(), head);
    PSMTXMultVecSR(m, &l_head_front, temp2);
  }
  {
    s8 head = i_this->m_jnt.mHeadJntNum;
    Mtx34 *m = ji1ExecGetAnmMtxHD(i_this->mpOrcaMorf->getModel(), head);
    PSMTXMultVecSR(m, &l_head_top, temp);
  }
  s16 temp3 = i_this->mHeadPitch;
  s16 temp4 = i_this->mHeadYaw;
  gabi::Local<cXyz> xz;
  xz->x = (f32)temp2->x;
  xz->y = 0.0f;
  xz->z = (f32)temp2->z;
  if (temp->y < 0.0f) {
    f32 d = std_sqrtf(PSVECSquareMag(xz));
    i_this->mHeadPitch = cM_atan2s(temp2->y, -d);
  } else {
    f32 d = std_sqrtf(PSVECSquareMag(xz));
    i_this->mHeadPitch = cM_atan2s(temp2->y, d);
  }
  s16 ba8 = cM_atan2s(temp2->x, temp2->z);
  i_this->mHeadYaw = ba8;
  s16 temp3_2 = (s16)(i_this->mHeadPitch - temp3) >> 1;
  s16 temp4_2 = (s16)(ba8 - temp4) >> 1;

  Mtx34 *pMtx = ji1ExecGetAnmMtxHD(pModel, i_this->hair1JointNo);
  gabi::Local<cXyz> sp48;
  sp48->x = (f32)pMtx->m[0][3];
  sp48->y = (f32)pMtx->m[1][3];
  sp48->z = (f32)pMtx->m[2][3];
  gabi::Local<cXyz> sp3C;
  sp3C->x = i_this->mHair1LastPos.x - sp48->x;
  sp3C->y = i_this->mHair1LastPos.y - sp48->y - 8.5f;
  sp3C->z = i_this->mHair1LastPos.z - sp48->z;
  gabi::Local<cXyz> wv;
  cXyz_ml(windVec, wv, wind);
  PSVECAdd(sp3C, wv, sp3C);

  f32 x = sp3C->x;
  if (std::fabs(x) < 0.01f) {
    x = 0.0f;
    sp3C->x = x;
  }
  f32 z = sp3C->z;
  if (std::fabs(z) < 0.01f) {
    z = 0.0f;
    sp3C->z = z;
  }

  s16 r4 = i_this->current.angle.y + i_this->m_jnt.mAngles[0][1];
  f32 temp10 = gabi::fmadds(z, cM_scos(r4), x * cM_ssin(r4));
  f32 y = sp3C->y;
  s16 r25 = i_this->mHair1AngleY;
  s16 r26 = i_this->mHair1AngleZ;
  s16 temp9 = cM_atan2s(-temp10, -y);
  if (temp9 < 0 && temp9 > -0x6000) {
    temp9 = 0;
  } else if (temp9 > 0x7C00 || temp9 <= -0x6000) {
    temp9 = 0x7C00;
  }
  temp9 += i_this->mHeadPitch;
  cLib_addCalcAngleS2(&i_this->mHair1AngleZ, temp9, 5, 0x400);
  {
    s32 add = i_this->mHair1SpeedZ + temp3_2;
    i_this->mHair1AngleZ = (s16)(i_this->mHair1AngleZ + add + add);
  }
  i_this->mHair1AngleZ =
      minMax(i_this->mHair1AngleZ, l_hio.mHair1MinZ, l_hio.mHair1MaxZ);

  {
    s16 a = i_this->current.angle.y + i_this->m_jnt.mAngles[0][1];
    f32 yy = sp3C->y;
    f32 f1 = gabi::fmadds(temp10, temp10, yy * yy);
    f32 f5 = cM_ssin(a);
    f32 f6 = cM_scos(a);
    f32 sq = std_sqrtf(f1);
    f32 v = gabi::fmsubs((f32)sp3C->z, f5, (f32)sp3C->x * f6);
    cLib_addCalcAngleS2(&i_this->mHair1AngleY, cM_atan2s(v, sq), 5, 0x400);
  }
  i_this->mHair1AngleY =
      (s16)(i_this->mHair1AngleY + (s16)(i_this->mHair1SpeedY - temp4_2));
  s16 bac = minMax(i_this->mHair1AngleY, l_hio.mHair1MinY, l_hio.mHair1MaxY);
  s16 baa = i_this->mHair1AngleZ;
  i_this->mHair1AngleY = bac;

  s32 d26 = baa - r26;
  s32 d25 = bac - r25;
  i_this->mHair1SpeedZ = tenth2(d26);
  i_this->mHair1SpeedY = tenth2(d25);
  i_this->mHair2AngleZ = (s16)(i_this->mHair2AngleZ - d26);
  i_this->mHair2AngleY = (s16)(i_this->mHair2AngleY - d25);
  r26 = i_this->mHair2AngleZ;
  r25 = i_this->mHair2AngleY;
  cLib_addCalcAngleS2(&i_this->mHair2AngleZ, 0, 5, 0x400);
  cLib_addCalcAngleS2(&i_this->mHair2AngleY, 0, 5, 0x400);

  i_this->mHair2AngleZ =
      (s16)(i_this->mHair2AngleZ + (s16)(i_this->mHair2SpeedZ + temp3_2));
  i_this->mHair2AngleZ =
      minMax(i_this->mHair2AngleZ, (s16)(l_hio.mHair2MinZ - i_this->mHair1AngleZ),
             (s16)(l_hio.mHair2MaxZ - i_this->mHair1AngleZ));
  i_this->mHair2AngleY =
      (s16)(i_this->mHair2AngleY + (s16)(i_this->mHair2SpeedY - temp4_2));
  i_this->mHair2AngleY =
      minMax(i_this->mHair2AngleY, (s16)(l_hio.mHair2MinY - i_this->mHair1AngleY),
             (s16)(l_hio.mHair2MaxY - i_this->mHair1AngleY));

  d26 = i_this->mHair2AngleZ - r26;
  d25 = i_this->mHair2AngleY - r25;
  i_this->mHair2SpeedZ = tenth2(d26);
  i_this->mHair2SpeedY = tenth2(d25);
  i_this->mHair3AngleZ = (s16)(i_this->mHair3AngleZ - d26);
  i_this->mHair3AngleY = (s16)(i_this->mHair3AngleY - d25);
  r26 = i_this->mHair3AngleZ;
  r25 = i_this->mHair3AngleY;
  cLib_addCalcAngleS2(&i_this->mHair3AngleZ, 0, 5, 0x400);
  cLib_addCalcAngleS2(&i_this->mHair3AngleY, 0, 5, 0x400);

  i_this->mHair3AngleZ =
      (s16)(i_this->mHair3AngleZ + (s16)(i_this->mHair3SpeedZ + temp3_2));
  i_this->mHair3AngleZ = minMax(
      i_this->mHair3AngleZ,
      (s16)(l_hio.mHair3MinZ - i_this->mHair1AngleZ - i_this->mHair2AngleZ),
      (s16)(l_hio.mHair3MaxZ - i_this->mHair1AngleZ - i_this->mHair2AngleZ));
  i_this->mHair3AngleY =
      (s16)(i_this->mHair3AngleY + (s16)(i_this->mHair3SpeedY - temp4_2));
  i_this->mHair3AngleY = minMax(
      i_this->mHair3AngleY,
      (s16)(l_hio.mHair3MinY - i_this->mHair2AngleY - i_this->mHair1AngleY),
      (s16)(l_hio.mHair3MaxY - i_this->mHair2AngleY - i_this->mHair1AngleY));
  i_this->mHair3SpeedZ = tenth2(i_this->mHair3AngleZ - r26);
  i_this->mHair3SpeedY = tenth2(i_this->mHair3AngleY - r25);

  wind = gabi::fmadds(
             std_sqrtf(ji1ExecSquareDistanceHD(&i_this->mHair1LastPos, sp48)),
             0.65f, wind) /
         30.0f;
  if (wind > 1.0f) {
    wind = 1.0f;
  }
  s16 temp14 = (s16)gabi::ftoi(gabi::fmadds(wind, 4600.0f, 1500.0f));
  s16 bd0 = (s16)(i_this->mHairWindPhase + temp14);
  i_this->mHairWindPhase = bd0;
  i_this->mHair1WindAngle = (s16)gabi::ftoi(wind * 2280.0f * cM_scos(bd0));
  f32 f5 = (f32)temp14;
  f32 f8 = (f32)bd0;
  i_this->mHair2WindAngle = (s16)gabi::ftoi(
      wind * 3908.0f * cM_scos(gabi::ftoi(gabi::fnmsubs(3.0f, f5, f8))));
  i_this->mHair3WindAngle = (s16)gabi::ftoi(
      wind * 7568.0f * cM_scos(gabi::ftoi(gabi::fnmsubs(6.0f, f5, f8))));
  i_this->mHair1LastPos.copy(*sp48.get());
}

/* 0224B268 */
BOOL daNpc_Ji1_c::_execute() {

  dComIfGp_get(); /* daPy_getPlayerActorClass(), unused */
  // HD: retain the unused play-object access before decrementing the hit
  // window.
  u8 e64 = ji1ExecHitCooldownHD(this);
  u8 d84 = mHarpoonState;
  if (e64 != 0) {
    ji1ExecHitCooldownHD(this) = (u8)(e64 - 1);
  }
  if (d84 == Ji1Exec_HARPOON_RELEASED) {
    harpoonMove();
  }

  if (cLib_calcTimer(&mBlinkTimer) == 0) {
    u8 f = (u8)(mBlinkFrame + 1);
    mBlinkFrame = f;
    /* headTexPattern->getFrameMax(): virtual (vtable at +4, slot 0x14) */
    s32 max = reinterpret_cast<Ji1ExecPatternHD *>(headTexPattern.get())
                  ->getFrameMax();
    if ((s32)f >= max) {
      s32 max2 = reinterpret_cast<Ji1ExecPatternHD *>(headTexPattern.get())
                     ->getFrameMax();
      mBlinkFrame = (u8)(mBlinkFrame - max2);
      mBlinkTimer = (s16)((s16)gabi::ftoi(cM_rndF(100.0f)) + 30);
    }
  }

  u32 sound;
  if (mAcch.ChkGroundHit()) {
    sound = dBgS_GetMtrlSndId(dComIfG_Bgsp(),
                              static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd));
  } else {
    sound = 0;
  }
  s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
  ji1ExecMorfPlayHD(mpOrcaMorf, &current.pos, sound, reverb);
  mpOrcaMorf->calc();

  if (isItemWaitAnim()) {
    // HD: each GHS local-static guard is stored before its value is
    // initialized.
    cXyz *aim_offset = &l_itemAimOffset;
    if (l_itemAimOffsetGuard == 0) {
      l_itemAimOffsetGuard = 1;
      aim_offset->x = -13.0f;
      aim_offset->y = -4.0f;
      aim_offset->z = 10.0f;
    }
    csXyz *aim_angle = &l_itemAimAngle;
    if (l_itemAimAngleGuard == 0) {
      l_itemAimAngleGuard = 1;
      csXyz_ct(aim_angle, 0x4000, 0, 0x7FFF);
    }
    mHeldItemAngle.x = (s16)aim_angle->x;
    mHeldItemAngle.y = (s16)aim_angle->y;
    mHeldItemAngle.z = (s16)aim_angle->z;
    cLib_addCalcPos2(&mHeldItemOffset, *aim_offset, 0.25f, 5.0f);
  } else {
    /* static cXyz aim_offset(-13.06f, -4.0f, 44.35f); static csXyz
     * aim_angle(0x3E94, 0, 0x7E93) */
    cXyz *aim_offset = &l_normalAimOffset;
    if (l_normalAimOffsetGuard == 0) {
      l_normalAimOffsetGuard = 1;
      aim_offset->x = -13.06f;
      aim_offset->y = -4.0f;
      aim_offset->z = 44.35f;
    }
    csXyz *aim_angle = &l_normalAimAngle;
    if (l_normalAimAngleGuard == 0) {
      l_normalAimAngleGuard = 1;
      csXyz_ct(aim_angle, 0x3E94, 0, 0x7E93);
    }
    mHeldItemAngle.x = (s16)aim_angle->x;
    mHeldItemAngle.y = (s16)aim_angle->y;
    mHeldItemAngle.z = (s16)aim_angle->z;
    cLib_addCalcPos2(&mHeldItemOffset, *aim_offset, 0.25f, 5.0f);
  }

  if (mHarpoonState == Ji1Exec_HARPOON_HELD &&
      (mAnimation == Ji1Exec_BCK_TATEATTACK ||
       mAnimation == Ji1Exec_BCK_YOKOATTACK)) {
    mpSpearMorf->setFrame(mpOrcaMorf->getFrame());
  } else {
    mpSpearMorf->mFrameCtrl.mFrame = 0.0f;
    mpSpearMorf->setPlaySpeed(0.0f);
  }
  mpSpearMorf->calc();
  ji1ExecHeadMoveHD(mHeadAnm);

  if (mAnimation == Ji1Exec_BCK_NAKU) {
    f32 v = mCryBtkFrame + 1.0f;
    if (!(v < 60.0f)) { // HD: NaN also resets the tear animation to frame 30.
      mCryBtkFrame = 30.0f;
    } else {
      mCryBtkFrame = v;
    }
  } else {
    if (mAnimation != Ji1Exec_BCK_NUGUI && mpTearEmitter.get() != nullptr) {
      auto *emitter = reinterpret_cast<Ji1ExecEmitterHD *>(mpTearEmitter.get());
      u32 status = emitter->mStatus;
      emitter->mMaxFrame = -1;
      emitter->mStatus = status | 1;
      mpTearEmitter = nullptr;
    }
    if (mCryBtkFrame > 0.0f) {
      f32 v = mCryBrkFrame + 1.0f;
      mCryBrkFrame = v;
      if (!(v < 30.0f)) {
        mCryBrkFrame = 0.0f;
        mCryBtkFrame = 0.0f;
      }
    }
  }

  ptmf_invoke(mAction, this, (u32)0);
  lookBack();
  daNpc_Ji1_setHairAngle(this);

  f32 px = current.pos.x;
  f32 py = current.pos.y;
  f32 pz = current.pos.z;
  attention_info.position.x = px; /* attention_info.position */
  attention_info.position.y = py + 190.0f;
  attention_info.position.z = pz;
  shape_angle.x = (s16)current.angle.x;
  shape_angle.y = (s16)current.angle.y;
  shape_angle.z = (s16)current.angle.z;
  eyePos.x = px;
  eyePos.y = py + 150.0f;
  eyePos.z = pz;
  fopAcM_posMoveF(this, &mStts.m_cc_move);
  gabi::Local<cXyz> temp;
  temp->x = (f32)current.pos.x;
  temp->y = (f32)current.pos.y;
  temp->z = (f32)current.pos.z;
  mAcch.CrrPos(dComIfG_Bgsp());
  gabi::Local<cXyz> diff;
  cXyz_mi(&current.pos, diff, temp);
  mBgCorrectVec.copy(*diff.get());
  tevStr.mRoomNo = (s8)ji1ExecRoomIdHD(
      dComIfG_Bgsp(), static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd));
  s32 col = ji1ExecPolyColorHD(dComIfG_Bgsp(),
                               static_cast<cBgS_PolyInfo *>(&mAcch.m_gnd));
  tevStr.mEnvrIdxOverride = (u8)col;
  set_mtx();
  mCyl.SetC(&current.pos);
  dComIfG_Ccsp_Set(&mCyl);
  mGuardCyl.SetC(&current.pos);
  dComIfG_Ccsp_Set(&mGuardCyl);
  mCoHit = 0;
  return true;
}

/* 0225CC44 */
static daNpc_Ji1_HIO_c *daNpc_Ji1_HIO_ct(daNpc_Ji1_HIO_c *p) {

  if (p == nullptr) {
    p = (daNpc_Ji1_HIO_c *)operator_new(0x100);
    if (p == nullptr)
      return p;
  }
  p->__vtbl = gabi::ea(&l_hioVtable);
  p->mNo = -1;
  p->field_0x08 = 100.0f;
  p->mMaxHeadX = 7000;
  p->mMaxBackboneX = 4000;
  p->mMaxHeadY = 9000;
  p->mMaxBackboneY = 7000;
  p->mMaxTurnStep = 5000;
  p->mHeadTurnVelScale = 1;
  p->mMaxHeadTurnVel = 2000;
  p->field_0x1C = 0.0f;
  p->field_0x20 = 20.0f;
  p->field_0x24 = 20.0f;
  p->field_0x28 = 0;
  p->mTalkDistXZ = 200.0f;
  p->mbRepeatKaiten = 0;
  p->mJpGuardSlideDist = 150.0f;
  p->mJpGuardSlideStep = 20.0f;
  p->mGuardSlideDist = 90.0f;
  p->mGuardSlideStep = 10.0f;
  p->mInasiAnmSpeed = 2.0f;
  p->mKrotAnmSpeed = 2.0f;
  p->mWaraiAnmSpeed = 1.25f;
  p->field_0x50 = 1.5f;
  p->mTeachHitCount[0] = 9;
  p->mTeachHitCount[1] = 9;
  p->mTeachHitCount[2] = 9;
  p->mTeachHitCount[3] = 2;
  p->mTeachHitCount[4] = 2;
  p->mTeachHitCount[5] = 2;
  p->mLevelHits[0] = 100;
  p->mLevelHits[1] = 300;
  p->mLevelHits[2] = 500;
  p->mLevelHits[3] = 1000;
  p->field_0x68 = 0;
  p->mAttackAnmSpeed = 0.7f;
  p->mAttackSwingAnmSpeed = 1.5f;
  p->mTateAnmSpeed = 1.4f;
  p->mTateYokoSwingAnmSpeed = 1.4f;
  p->mYokoAnmSpeed = 1.4f;
  p->field_0x7C = 1.2f;
  p->mNockBackAnmSpeed = 0.1f;
  p->mNockBackTime = 0xB;
  p->mBattleTimerBase0 = 0x32;
  p->mBattleTimerBase1 = 0x28;
  p->mBattleTimerBase2 = 0x1E;
  p->mBattleTimerBase3 = 0x14;
  p->mBattleTimerBase4 = 0xF;
  p->mBattleTimerBase5 = 10;
  p->mBattleTimerRange0 = 0xB4;
  p->mBattleTimerRange1 = 0x8C;
  p->mBattleTimerRange2 = 100;
  p->mBattleTimerRange3 = 0x50;
  p->mBattleTimerRange4 = 0x3C;
  p->mBattleTimerRange5 = 0x32;
  p->mBibiriAnmMorf = 6.0f;
  p->mOdorokuAnmMorf = 16.0f;
  p->mBibiriSlideDist = 70.0f;
  p->mBibiriSlideStep = 10.0f;
  p->mBibiriSlideFrame = 1.0f;
  p->field_0xB8 = 0;
  p->mHarpoonBounce = 0.25f;
  p->mHarpoonGravity = -5.0f;
  p->mBackboneOffsets[0].set(90.0f, 5.0f, 30.0f);
  p->mBackboneOffsets[1].set(90.0f, 5.0f, -30.0f);
  p->mBackboneOffsets[2].set(-20.0f, -20.0f, 0.0f);
  p->mHair1MinY = (s16)0xC800;
  p->mHair1MaxY = 0x3800;
  p->mHair2MinY = (s16)0xC600;
  p->mHair2MaxY = 0x3A00;
  p->mHair3MinY = (s16)0xC600;
  p->mHair3MaxY = 0x3A00;
  p->mHair1MinZ = (s16)0xFB00;
  p->mHair1MaxZ = 0x3800;
  p->mHair2MinZ = (s16)0xF600;
  p->mHair2MaxZ = 0x3A00;
  p->mHair3MinZ = (s16)0xF100;
  p->mHair3MaxZ = 0x3A00;
  return p;
}

// HD: initialize this TU's header statics and register their native
// destructors.
/* 0225CF20 */
static void __sinit_d_a_npc_ji1_cpp() {
  for (int i = 3; i >= 0; i--)
    l_headerWords.mWords[i] = 0;
  __register_global_object(gabi::ea(&l_headerRecord));
  l_angleMin = -3.1415927f;
  l_angleMax = 3.1415927f;
  ji1ExecHeaderByteConstructHD(&l_headerByte);
  __register_global_object(gabi::ea(&l_byteRecord));
  ji1ExecHeaderShortConstructHD(&l_headerShort);
  __register_global_object(gabi::ea(&l_shortRecord));
  daNpc_Ji1_HIO_ct(&l_hio);
  l_head_front.set(0.0f, 1.0f, 0.0f);
  l_head_top.set(1.0f, 0.0f, 0.0f);
}

/* 0225CFF0 */
static void daNpc_Ji1_HIO_dt(daNpc_Ji1_HIO_c *p, s32 flags) {

  if (p != nullptr && (flags & 1)) {
    operator_delete(p);
  }
}

// HD: explicit deleting destructor; flags control the final operator delete.
/* 0225D004 */
static void daNpc_Ji1_dt(daNpc_Ji1_c *p, s32 flags) {

  if (p != nullptr) {
    ji1ExecHitInfoDestructHD(&p->mAtCps,
                             2); /* dCcD_GObjInf::~dCcD_GObjInf (dCcD_Cps) */
    dCcD_Cyl_dt(&p->mAtCyl, 2);
    dCcD_Cyl_dt(&p->mGuardCyl, 2);
    dCcD_Cyl_dt(&p->mCyl, 2);
    dCcD_Stts_dt(&p->mGuardStts, 2);
    dCcD_Stts_dt(&p->mStts, 2);
    ji1ExecCircleDestructHD(&p->mAcchCir.m_cir,
                            2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
    static_cast<dBgS_GrpPassChk &>(p->mAcch).__vtbl =
        gabi::ea(&l_objAcchVtable20); /* dBgS_ObjAcch -> dBgS_Acch vtables */
    static_cast<dBgS_PolyPassChk &>(p->mAcch).__vtbl =
        gabi::ea(&l_objAcchVtable14);
    ji1ExecAcchDestructHD(&p->mAcch, 0); /* dBgS_Acch::~dBgS_Acch */
    ji1ExecActorDestructHD(p, 0);        /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) {
      operator_delete(p);
    }
  }
}

/* WWHD glue
ext 025E535C BOOL ji1ExecMorfPlayHD(mDoExt_McaMorf* morph, cXyz* pos, u32 sound, s32 reverb)
ext 0259F67C void ji1ExecHeadMoveHD(void* head)
ext 0257DB04 be<f32>* ji1ExecWindPowerHD()
ext 0257DAA8 cXyz* ji1ExecWindVectorHD()
ext 028E8DE8 f32 ji1ExecSquareDistanceHD(const cXyz* a, const cXyz* b)
ext 024EF130 s32 ji1ExecRoomIdHD(dBgS* bgs, void* polygon)
ext 024EEEB8 s32 ji1ExecPolyColorHD(dBgS* bgs, void* polygon)
ext 025D5AA4 u32 ji1ExecCreateChildHD(const char* name, u32 parent, u32 parameter, cXyz* pos, s32 room, csXyz* angle, cXyz* scale, u32 callback)
ext 0259F7D4 void ji1ExecSetActorInfoHD(dNpc_EventCut_c* cut, const char* name, fopAc_ac_c* actor)
ext 025164C0 void ji1ExecSetCapsuleHD(dCcD_Cps* capsule, void* source)
ext 02018808 void ji1ExecSetStartEndHD(void* capsule, cXyz* start, cXyz* end)
ext 025A5B18 void ji1ExecSmokeConstructHD(void* smoke, u32 mode)
ext 0259F740 void ji1ExecCutConstructHD(void* cut)
ext 0259DAA0 void ji1ExecJointConstructHD(void* joint)
ext 025E7820 void ji1ExecBtpConstructHD(void* anim)
ext 025E80D0 void ji1ExecBrkConstructHD(void* anim)
ext 025E7C6C void ji1ExecBtkConstructHD(void* anim)
ext 02515FB8 void ji1ExecHitInfoConstructHD(void* info)
ext 02018150 void ji1ExecLineConstructHD(void* line)
ext 028ED6F8 void ji1ExecHeaderByteConstructHD(void* byte)
ext 028EAB2C void ji1ExecHeaderShortConstructHD(void* value)
ext 02515980 void ji1ExecHitInfoDestructHD(void* info, s32 flags)
ext 02018034 void ji1ExecCircleDestructHD(void* circle, s32 flags)
ext 024EFD9C void ji1ExecAcchDestructHD(void* acch, s32 flags)
ext 025D50BC void ji1ExecActorDestructHD(void* actor, s32 flags)
data l_hio 104672AC
data l_head_front 10467294
data l_head_top 104672A0
data l_replyFlag 10467244
data l_headerWords 10467248
data l_angleMin 1046722C
data l_angleMax 10467230
data l_headerByte 10467245
data l_headerShort 10467246
data l_headerRecord 101BEBF4
data l_byteRecord 101BEC00
data l_shortRecord 101BEC0C
data l_cyl_src 101BEA8C
data l_cyl2_src 101BEAD0
data l_cylAt_src 101BEB14
data l_cpsAt_src 101BEB58
data l_actorVtable 1001AE78
data l_hioVtable 1001AE88
data l_objAcchVtable10 1001AE48
data l_objAcchVtable14 1001AE68
data l_objAcchVtable20 1001AE58
data l_aabVtable 1001AE38
data l_shapeVtable 100015A8
data l_capsuleVtable 1004AF60
data l_hitInfoVtable 1004AF18
data l_capsuleAttrVtable 1004AF70
data l_itemAimOffset 1046727C
data l_itemAimAngle 10467234
data l_normalAimOffset 10467288
data l_normalAimAngle 1046723C
data l_itemAimOffsetGuard 104673B0
data l_itemAimAngleGuard 104673B4
data l_normalAimOffsetGuard 104673B8
data l_normalAimAngleGuard 104673BC
str 1001B198 "Ji" in 0224C550
str 1001B1C4 "Ji" in 0224D280
str 1001B1B8 "Ji1" in 0224C650
str 1001B1BC "Kmon" in 0224C650
*/
