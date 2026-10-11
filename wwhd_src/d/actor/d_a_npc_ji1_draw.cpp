/**
 * d_a_npc_ji1_draw.cpp (WWHD)
 * NPC - Orca: joint callbacks, draw, heap, matrices, look-back, harpoon.
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout
 * and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_draw.h"

static daNpc_Ji1_HIO_c l_hio;
static Ji1DrawQuaternionHD l_zeroQuat;
static cXyz l_baseY;

/* 02249084 */
BOOL nodeCallBack1(J3DNode *node, int calcTiming) {

  if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
    J3DModel *model = j3dSys.getModel();
    daNpc_Ji1_c *i_this = static_cast<daNpc_Ji1_c *>(model->getUserArea());
    J3DJoint *joint = J3DNode_toJoint(node);
    u32 jntNo = reinterpret_cast<Ji1DrawJointHD *>(joint)->mJntNo;
    if (i_this) {
      Mtx34 *now = mDoMtx_stack_c::get();
      PSMTXCopy(model->getAnmMtx(jntNo), now);
      if (jntNo == (u32)(s32)i_this->m_jnt.mHeadJntNum) {
        csXyz *headAnm = reinterpret_cast<csXyz *>(i_this->mHeadAnm);
        mDoMtx_XrotM(now, (s16)(headAnm->y + i_this->m_jnt.mAngles[0][1]));
        mDoMtx_ZrotM(now, (s16)(headAnm->x - i_this->m_jnt.mAngles[0][0]));
      }
      if (jntNo == (u32)(s32)i_this->m_jnt.mBackboneJntNum) {
        mDoMtx_XrotM(now, i_this->m_jnt.mAngles[1][1]);
        mDoMtx_ZrotM(now, (s16)-i_this->m_jnt.mAngles[1][0]);
        for (int i = 0; i < 3; i++) {
          ji1DrawMultVecHD(now, &l_hio.field_0xC4[i], &i_this->field_0xBD8[i]);
        }
      }
      mtx_copy(model->getAnmMtx(jntNo), now); /* setAnmMtx */
      PSMTXCopy(now, J3DSys::mCurrentMtx);
    }
  }
  return true;
}

/* 02249224 */
BOOL nodeCallBack2(J3DNode *node, int calcTiming) {

  if (calcTiming == 0) {
    J3DModel *model = j3dSys.getModel();
    daNpc_Ji1_c *i_this = static_cast<daNpc_Ji1_c *>(model->getUserArea());
    J3DJoint *joint = J3DNode_toJoint(node);
    u32 jntNo = reinterpret_cast<Ji1DrawJointHD *>(joint)->mJntNo;
    if (i_this) {
      PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
      if (jntNo == (u32)(s32)i_this->hair1JointNo) {
        cMtx_ZrotM(calc_mtx(),
                   (s16)(i_this->field_0xBAA + i_this->field_0xBD2));
        cMtx_YrotM(calc_mtx(), i_this->field_0xBAC);
      } else if (jntNo == (u32)(s32)i_this->hair2JointNo) {
        cMtx_ZrotM(calc_mtx(),
                   (s16)(i_this->field_0xBAE + i_this->field_0xBD4));
        cMtx_YrotM(calc_mtx(), i_this->field_0xBB0);
      } else {
        cMtx_ZrotM(calc_mtx(),
                   (s16)(i_this->field_0xBB2 + i_this->field_0xBD6));
        cMtx_YrotM(calc_mtx(), i_this->field_0xBB4);
      }
      mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
      PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx);
    }
  }
  return true;
}

/* 022494C8 */
BOOL nodeCallBack3(J3DNode *node, int calcTiming) {

  if (calcTiming != 0) {
    return true;
  } else {
    J3DModel *model = j3dSys.getModel();
    daNpc_Ji1_c *i_this = static_cast<daNpc_Ji1_c *>(model->getUserArea());
    J3DJoint *joint = J3DNode_toJoint(node);
    u32 jntNo = reinterpret_cast<Ji1DrawJointHD *>(joint)->mJntNo;
    Mtx34 *now = mDoMtx_stack_c::get();
    PSMTXCopy(model->getAnmMtx(jntNo), now);
    s16 temp = (s16)gabi::ftoi((f32)(s16)i_this->field_0xBA2 *
                               cM_ssin((u16)i_this->field_0xBA4));
    if (jntNo == (u32)(s32)i_this->armLJointNo) {
      mDoMtx_XrotM(now, temp);
    } else if (jntNo == (u32)(s32)i_this->armRJointNo) {
      mDoMtx_XrotM(now, temp);
    }
    mtx_copy(model->getAnmMtx(jntNo), now);
    PSMTXCopy(now, J3DSys::mCurrentMtx);
    return true;
  }
}

// HD: no blob shadow; figurine registration takes the position by reference.
/* 02249638 */
BOOL daNpc_Ji1_c::_draw() {

  if (mHide) {
    return true;
  }
  J3DModel *pOrcaModel = mpOrcaMorf->getModel();
  J3DModelData *pOrcaModelData = J3DModel_getModelData(pOrcaModel);
  J3DModel *pSpearModel = mpSpearMorf->getModel();
  settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
  setLightTevColorType(dKy_getEnvlight(), pOrcaModel, &tevStr);
  setLightTevColorType(dKy_getEnvlight(), pSpearModel, &tevStr);
  mDoExt_btpAnm_entry((mDoExt_btpAnm *)&mBlinkAnim, pOrcaModelData,
                      (u8)mBlinkFrame);
  mpOrcaMorf->entryDL();
  if (field_0xD84 == Ji1Draw_HARPOON_HELD) {
    mpSpearMorf->entryDL();
  } else if (field_0xD84 == Ji1Draw_HARPOON_RELEASED) {
    mpSpearMorf->updateDL();
  }
  {
    f32 f = (f32)(s16)gabi::ftoi(mCryBtkFrame);
    mDoExt_btkAnm_entry((mDoExt_btkAnm *)&mCryBtk,
                        J3DModel_getModelData(mpTearsModel), f);
  }
  {
    f32 f = (f32)(s16)gabi::ftoi(mCryBrkFrame);
    mDoExt_brkAnm_entry((mDoExt_brkAnm *)&mCryBrk,
                        J3DModel_getModelData(mpTearsModel), f);
  }
  mDoExt_modelUpdateDL(mpTearsModel);
  ji1DrawRegistFigHD(DSNAP_TYPE_NPC_JI1, this, &current.pos, current.angle.y,
                     1.0f, 1.0f, 1.0f);
  return true;
}

/* 02249884 */
BOOL daNpc_Ji1_c::CreateHeap() {

  J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(
      "Ji", dRes_INDEX_JI_BDL_JI_e, SAFESTRING_VTBL);
  void *anm =
      dComIfG_getObjectRes("Ji", dRes_INDEX_JI_BCK_WAIT01_e, SAFESTRING_VTBL);
  void *bas =
      dComIfG_getObjectRes("Ji", dRes_INDEX_JI_BAS_WAIT01_e, SAFESTRING_VTBL);
  mpOrcaMorf = mDoExt_McaMorf::create(
      nullptr, modelData, nullptr, nullptr, (J3DAnmTransform *)anm,
      J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1, bas, 0, 0x11020203);
  if (mpOrcaMorf.get() == nullptr || mpOrcaMorf->getModel() == nullptr) {
    return false;
  }
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  mDoMtx_stack_c::YrotM(current.angle.y);
  J3DModel_setBaseTRMtx(mpOrcaMorf->getModel(), mDoMtx_stack_c::get());

  s8 n;
  n = (s8)modelData->getJointName()->getIndex("head");
  m_jnt.mHeadJntNum = n;
  if (n < 0)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1629, "m_jnt.getHeadJntNum() >= 0");
  n = (s8)modelData->getJointName()->getIndex("backbone1");
  m_jnt.mBackboneJntNum = n;
  if (n < 0)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x162E,
                    "m_jnt.getBackboneJntNum() >= 0");
  n = (s8)modelData->getJointName()->getIndex("armL1");
  armLJointNo = n;
  if (n < 0)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1632, "armLJointNo >= 0");
  n = (s8)modelData->getJointName()->getIndex("armR1");
  armRJointNo = n;
  if (n < 0)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1635, "armRJointNo >= 0");
  n = (s8)modelData->getJointName()->getIndex("handR");
  handRJointNo = n;
  if (n < 0)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1638, "handRJointNo >= 0");

  J3DModelData *yariData = (J3DModelData *)dComIfG_getObjectRes(
      "Ji", dRes_INDEX_JI_BDL_JI_YARI_e, SAFESTRING_VTBL);
  void *yariAnm = dComIfG_getObjectRes(
      "Ji", dRes_INDEX_JI_BCK_JIYARI_TATEATTACK_e, SAFESTRING_VTBL);
  mpSpearMorf = mDoExt_McaMorf::create(
      nullptr, yariData, nullptr, nullptr, (J3DAnmTransform *)yariAnm,
      J3DFrameCtrl::EMode_NONE, 0.0f, 0, -1, 1, nullptr, 0, 0x11020203);
  if (mpSpearMorf.get() == nullptr || mpSpearMorf->getModel() == nullptr) {
    return false;
  }

  J3DModelData *modelData2 = (J3DModelData *)dComIfG_getObjectRes(
      "Ji", dRes_INDEX_JI_BDL_YJITR00_e, SAFESTRING_VTBL);
  mpTearsModel = mDoExt_J3DModel__create(modelData2, 0, 0x11020203);
  void *a_brk =
      dComIfG_getObjectRes("Ji", dRes_INDEX_JI_BRK_YJITR00_e, SAFESTRING_VTBL);
  if (a_brk == nullptr)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x165E, "a_brk != (0)");
  void *a_btk =
      dComIfG_getObjectRes("Ji", dRes_INDEX_JI_BTK_YJITR00_e, SAFESTRING_VTBL);
  if (a_btk == nullptr)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1661, "a_btk != (0)");
  BOOL temp1 =
      ji1DrawInitBrkHD(&mCryBrk, modelData2, a_brk, false,
                       J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
  BOOL temp2 = mCryBtk.init(modelData2, (J3DAnmTextureSRTKey *)a_btk, false,
                            J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0);
  if (mpTearsModel.get() == nullptr || temp1 == 0 || temp2 == 0) {
    return false;
  }

  headTexPattern = (J3DAnmTexPattern *)dComIfG_getObjectRes(
      "Ji", dRes_INDEX_JI_BTP_JI_e, SAFESTRING_VTBL);
  if (headTexPattern.get() == nullptr)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1669, "headTexPattern != (0)");
  if (ji1DrawInitBtpHD(&mBlinkAnim, modelData, headTexPattern.get(), TRUE,
                       J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0) == 0) {
    return false;
  }

  n = (s8)modelData->getJointName()->getIndex("hair1");
  hair1JointNo = n;
  if (n < 0)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1671, "hair1JointNo >= 0");
  n = (s8)modelData->getJointName()->getIndex("hair2");
  hair2JointNo = n;
  if (n < 0)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1673, "hair2JointNo >= 0");
  n = (s8)modelData->getJointName()->getIndex("hair3");
  hair3JointNo = n;
  if (n < 0)
    JUT_ASSERT_fail("d_a_npc_ji1.cpp", 0x1675, "hair3JointNo >= 0");

  for (u16 i = 0; i < modelData->getJointNum(); i++) {
    if (i == hair1JointNo || i == hair2JointNo || i == hair3JointNo) {
      modelData->getJointNodePointer(i)->setCallBack(nodeCallBack2);
    }
  }
  for (u16 i = 0; i < modelData->getJointNum(); i++) {
    if (i == m_jnt.mHeadJntNum || i == m_jnt.mBackboneJntNum) {
      modelData->getJointNodePointer(i)->setCallBack(nodeCallBack1);
    } else if (i == armLJointNo || i == armRJointNo) {
      modelData->getJointNodePointer(i)->setCallBack(nodeCallBack3);
    }
  }

  mpOrcaMorf->getModel()->setUserArea(reinterpret_cast<uintptr_t>(this));
  mAcchCir.SetWall(60.0f, 50.0f);
  mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
  mAcch.OnLineCheck();

  mEventIdx[0] = dComIfGp_evmng_getEventIdx("Ji1_StartSpeak", 0xFF);
  mEventIdx[1] = dComIfGp_evmng_getEventIdx("Ji1_Speak", 0xFF);
  mEventIdx[2] = dComIfGp_evmng_getEventIdx("Ji1_EndSpeak", 0xFF);
  mEventIdx[3] = dComIfGp_evmng_getEventIdx("Ji1_BadSpeak", 0xFF);
  mEventIdx[4] = dComIfGp_evmng_getEventIdx("Ji1_linkmove", 0xFF);
  mEventIdx[5] = dComIfGp_evmng_getEventIdx("Ji1_kaiten", 0xFF);
  mEventIdx[6] = dComIfGp_evmng_getEventIdx("Ji1_kaiten_exp", 0xFF);
  mEventIdx[7] = dComIfGp_evmng_getEventIdx("Ji1_ItemGetEnd", 0xFF);
  mEventIdx[8] = dComIfGp_evmng_getEventIdx("Ji1_ItemGetTalkEnd", 0xFF);
  mEventIdx[9] = dComIfGp_evmng_getEventIdx("Ji1_pl_backroom", 0xFF);
  mEventIdx[10] = dComIfGp_evmng_getEventIdx("Ji1_TeachSpRollCut", 0xFF);
  mEventIdx[11] = dComIfGp_evmng_getEventIdx("Ji1_SpRollFail", 0xFF);
  mEventIdx[12] = dComIfGp_evmng_getEventIdx("Ji1_NormAng", 0xFF);
  mEventIdx[13] = dComIfGp_evmng_getEventIdx("Ji1_NormAng2", 0xFF);
  mEventIdx[14] = dComIfGp_evmng_getEventIdx("Ji1_NormAngFin", 0xFF);
  mEventIdx[15] = dComIfGp_evmng_getEventIdx("Ji1_SwordGetTalkEnd", 0xFF);
  mEventIdx[16] = dComIfGp_evmng_getEventIdx("Ji1_KmonTalk", 0xFF);
  mEventIdx[17] = dComIfGp_evmng_getEventIdx("Ji1_EquipTalk", 0xFF);

  if (dComIfGs_isEventBit(0x0001) && !dComIfGs_isEventBit(0x0108)) {
    eventInfo.setEventId(mEventIdx[0x11]);
  }
  return true;
}

/* 0224AA24 */
BOOL daNpc_Ji1_c::chkAttention(cXyz *param_1, s16 param_2) {

  fopAc_ac_c *player = daPy_getPlayerActorClass();
  f32 temp = 800.0f;
  s32 temp2 = l_hio.field_0x10 + l_hio.field_0x12;
  f32 x = player->current.pos.x - param_1->x;
  f32 z = player->current.pos.z - param_1->z;
  f32 dist = std_sqrtf(gabi::fmadds(x, x, z * z));
  s16 angle = cM_atan2s(x, z);
  if (field_0xD7A != 0) {
    temp += 40.0f;
    temp2 += 0x71C;
  }
  angle -= param_2;
  s32 a = angle < 0 ? -angle : angle;
  // HD: the unordered comparison also accepts a NaN distance.
  field_0xD7A = temp2 > a && !(temp <= dist);
  return temp2 > a && !(temp <= dist);
}

/* 0224ABA4 */
BOOL daNpc_Ji1_c::lookBack() {

  BOOL ret = false;
  fopAc_ac_c *player = dComIfGp_getPlayer(0);
  gabi::Local<cXyz> temp;
  cXyz_mi(&player->current.pos, temp, &current.pos);
  gabi::Local<cXyz> xz;
  xz->x = (f32)temp->x;
  xz->y = 0.0f;
  xz->z = (f32)temp->z;
  f32 dist = std_sqrtf(PSVECSquareMag(xz));
  bool temp2 = true;
  {
    gabi::Local<cXyz> pos;
    pos->x = (f32)current.pos.x;
    pos->y = (f32)current.pos.y;
    pos->z = (f32)current.pos.z;
    chkAttention(pos, current.angle.y);
  }
  gabi::Local<cXyz> attnPos;
  if (mEventCut.mbAttention) {
    attnPos->x = (f32)mEventCut.mPos.x;
    attnPos->y = (f32)mEventCut.mPos.y;
    attnPos->z = (f32)mEventCut.mPos.z;
  }
  cXyz *dstPos;
  if (field_0xD7E) {
    /* daPy_py_c::getLeftHandPos(): the player's cXyz at +0x3F0 */
    cXyz *lh = &reinterpret_cast<Ji1DrawPlayerHD *>(player)->mLeftHandPos;
    attnPos->x = (f32)lh->x;
    attnPos->y = (f32)lh->y;
    attnPos->z = (f32)lh->z;
    dstPos = attnPos;
  } else {
    if ((field_0xD7A != 0 || isGuardAnim()) ||
        (ptmf_eq(mAction, ACT_kaitenwaitAction) &&
         !dComIfGs_isEventBit(0x0501)) ||
        (!ptmf_eq(mAction, ACT_normalAction) && dist < 600.0f)) {
      if (mAnimation == Ji1Draw_BCK_WARAI) {
        dstPos = nullptr;
      } else {
        gabi::Local<cXyz> eye;
        ji1DrawPlayerEyePosHD(eye, 0.0f);
        attnPos->copy(*eye);
        dstPos = attnPos;
      }
    } else {
      dstPos = nullptr;
    }
  }
  s16 maxHeadRot = l_hio.field_0x10;
  s16 maxSpineRot = l_hio.field_0x12;
  if (mAnimation == Ji1Draw_BCK_BTKAMASI || mAnimation == Ji1Draw_BCK_BTKAMAE) {
    maxHeadRot = 0x3E80;
    maxSpineRot = 0x1F40;
  }
  m_jnt.setParam(l_hio.field_0x0E, maxSpineRot, -l_hio.field_0x0E, -maxHeadRot,
                 l_hio.field_0x0C, maxHeadRot, -l_hio.field_0x0C, -maxHeadRot,
                 l_hio.field_0x14);
  if (m_jnt.mbTrn) {
    cLib_addCalcAngleS2(&field_0xC88, l_hio.field_0x18, l_hio.field_0x16,
                        0x800);
    temp2 = false;
    ret = true;
  } else {
    field_0xC88 = 0;
  }
  gabi::Local<cXyz> eye;
  eye->x = (f32)eyePos.x;
  eye->y = (f32)eyePos.y;
  eye->z = (f32)eyePos.z;
  ji1DrawLookAtTargetHD(&m_jnt, &current.angle.y, dstPos, eye, current.angle.y,
                        field_0xC88, temp2);
  return ret;
}

/* 0224AEF0 */
void daNpc_Ji1_c::set_mtx() {

  J3DModel *pModel = mpOrcaMorf->getModel();
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  mDoMtx_stack_c::YrotM(current.angle.y);
  J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
  mpOrcaMorf->calc();
  {
    Mtx34 *src = pModel->getAnmMtx(m_jnt.mHeadJntNum);
    J3DModel_setBaseTRMtx(mpTearsModel, src);
  }
  JPABaseEmitter *e = field_0x430;
  if (e) {
    Mtx34 *m = pModel->getAnmMtx(m_jnt.mHeadJntNum);
    auto *emitter = reinterpret_cast<Ji1DrawEmitterHD *>(e);
    ji1DrawSetEmitterMatrixHD(m, emitter->mGlobalRotation,
                              &emitter->mGlobalTranslation);
  }
  if (field_0xD84 == Ji1Draw_HARPOON_HELD) {
    PSMTXCopy(pModel->getAnmMtx(handRJointNo), mDoMtx_stack_c::get());
    ji1DrawXYZrotMHD(mDoMtx_stack_c::get(), field_0xD5C.x, field_0xD5C.y,
                     field_0xD5C.z);
    mDoMtx_stack_c::transM(field_0xD50.x, field_0xD50.y, field_0xD50.z);
    J3DModel_setBaseTRMtx(mpSpearMorf->getModel(), mDoMtx_stack_c::get());
  }
  mpSpearMorf->calc();
  if (field_0xD84) {
    gabi::Local<cXyz> temp;
    gabi::Local<cXyz> temp2;
    temp->set(0.0f, 0.0f, 100.0f);
    temp2->set(0.0f, 0.0f, -100.0f);
    if (mAnimation == Ji1Draw_BCK_YOKOATTACK) {
      temp->z = 70.0f;
    }
    field_0xB90.copy(field_0xB78);
    PSMTXCopy(mpSpearMorf->getModel()->getAnmMtx(JI_YARI_JNT_JI_YARI_e),
              mDoMtx_stack_c::get());
    ji1DrawMultVecHD(mDoMtx_stack_c::get(), temp, &field_0xB78);
    ji1DrawMultVecHD(mDoMtx_stack_c::get(), temp2, &field_0xB84);
  }
}

/* (u32)f for f >= 0 (GHS: through 2^31) */
static inline u32 f2u(f32 f) {
  if (f < 2147483648.0f)
    return (u32)gabi::ftoi(f);
  return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}
static inline void quat_copy_neg(Ji1DrawQuaternionHD *dst,
                                 Ji1DrawQuaternionHD *src) {
  dst->x = -src->x;
  dst->y = (f32)src->y;
  dst->z = -src->z;
  dst->w = (f32)src->w;
}

/* 0224DE3C */
void daNpc_Ji1_c::harpoonRelease(cXyz *param_1) {

  PSMTXCopy(mpSpearMorf->getModel()->getAnmMtx(JI_YARI_JNT_JI_YARI_e),
            &field_0xCA0);
  mpSpearMorf->setFrame(0.0f);
  field_0xCD0.x = field_0xCA0.m[0][3];
  field_0xCD0.y = field_0xCA0.m[1][3];
  field_0xCD0.z = field_0xCA0.m[2][3];
  field_0xCA0.m[0][3] = 0.0f;
  field_0xCA0.m[1][3] = 0.0f;
  field_0xCA0.m[2][3] = 0.0f;
  if (param_1) {
    field_0xCDC.copy(*param_1);
  } else {
    field_0xCDC.set(0.0f, 5.0f, 0.0f);
  }
  field_0xD14 = 0x400;
  field_0xD04.x = cM_ssin((u16)field_0xD14) * cM_scos((u16)current.angle.y);
  field_0xD04.y = 0.0f;
  field_0xD04.z = cM_ssin((u16)field_0xD14) * cM_ssin((u16)current.angle.y);
  field_0xD04.w = cM_scos((u16)field_0xD14);
  ji1DrawCopyQuaternionHD(&field_0xCF4, &l_zeroQuat);
  field_0xD84 = Ji1Draw_HARPOON_RELEASED;
}

/* 0224A2B8 */
void daNpc_Ji1_c::harpoonMove() {

  gabi::Local<cXyz> temp, temp2, temp3, temp4;
  temp3->x = (f32)field_0xB78.x;
  temp4->z = (f32)field_0xB84.z;
  temp2->z = -100.0f;
  temp->z = 100.0f;
  temp->x = 0.0f;
  temp->y = 0.0f;
  temp2->x = 0.0f;
  temp2->y = 0.0f;
  temp4->x = (f32)field_0xB84.x;
  temp4->y = (f32)field_0xB84.y;
  temp3->z = (f32)field_0xB78.z;
  temp3->y = (f32)field_0xB78.y;
  f32 hio_bc = l_hio.field_0xBC;
  f32 hio_c0 = l_hio.field_0xC0;

  ji1DrawQuatMultiplyHD(&field_0xCF4, &field_0xD04, &field_0xCF4);
  ji1DrawQuatNormalizeHD(&field_0xCF4, &field_0xCF4);
  PSVECAdd(&field_0xCD0, &field_0xCDC, &field_0xCD0);
  field_0xCDC.y = field_0xCDC.y + hio_c0;
  PSVECScale(&field_0xCDC, &field_0xCDC, 0.95f);
  mDoMtx_stack_c::transS(field_0xCD0.x, field_0xCD0.y, field_0xCD0.z);
  ji1DrawQuatMHD(&field_0xCF4);
  ji1DrawMatrixConcatHD(mDoMtx_stack_c::get(), &field_0xCA0,
                        mDoMtx_stack_c::get());
  J3DModel_setBaseTRMtx(mpSpearMorf->getModel(), mDoMtx_stack_c::get());
  ji1DrawMultVecHD(mDoMtx_stack_c::get(), temp, &field_0xB78);
  ji1DrawMultVecHD(mDoMtx_stack_c::get(), temp2, &field_0xB84);

  if (field_0xB78.y < 0.0f || field_0xB84.y < 0.0f) {
    gabi::Local<cXyz> temp5, temp6;
    cXyz_mi(&field_0xB78, temp5, &field_0xCD0);
    cXyz_mi(&field_0xB84, temp6, &field_0xCD0);
    ji1DrawVectorSubtractHD(temp3, &field_0xCD0, temp3);
    ji1DrawVectorSubtractHD(temp4, &field_0xCD0, temp4);
    gabi::Local<Ji1DrawQuaternionHD> temp7, temp8, temp11;
    ji1DrawQuatRotVecHD(temp7, temp3, temp5);
    ji1DrawQuatRotVecHD(temp8, temp4, temp6);

    cXyz *sePos = nullptr;
    gabi::Local<cXyz> temp12;
    // HD: preserve the original ordered tests when either height is NaN.
    bool bound = false;
    if (field_0xB78.y < 0.0f) {
      if (!(field_0xB84.y < 0.0f)) {
        f32 b78y = field_0xB78.y;
        field_0xB84.y = field_0xB84.y - b78y;
        field_0xCD0.y = field_0xCD0.y - b78y;
        field_0xB78.y = 0.0f;
        quat_copy_neg(temp7, &field_0xD04);
        gabi::Local<cXyz> temp10;
        temp10->x = (f32)temp6->x;
        temp10->y = 0.0f;
        temp10->z = (f32)temp6->z;
        ji1DrawQuatRotVecHD(temp11, temp6, temp10);
        ji1DrawQuatSlerpHD(temp8, temp11, temp8, 0.05f);
        ji1DrawQuatSlerpHD(temp7, &l_zeroQuat, temp7, 0.1f);
        ji1DrawQuatSlerpHD(temp8, temp7, &field_0xD04, 0.5f);
        cXyz_mi(temp5, temp12, temp3);
        sePos = &field_0xB78;
        bound = true;
      }
    } else if (field_0xB84.y < 0.0f) {
      f32 b84y = field_0xB84.y;
      field_0xCD0.y = field_0xCD0.y - b84y;
      field_0xB78.y = field_0xB78.y - b84y;
      field_0xB84.y = 0.0f;
      quat_copy_neg(temp8, &field_0xD04);
      gabi::Local<cXyz> temp10;
      temp10->x = (f32)temp5->x;
      temp10->y = 0.0f;
      temp10->z = (f32)temp5->z;
      ji1DrawQuatRotVecHD(temp11, temp5, temp10);
      ji1DrawQuatSlerpHD(temp7, temp11, temp7, 0.1f);
      ji1DrawQuatSlerpHD(temp8, &l_zeroQuat, temp8, 0.1f);
      ji1DrawQuatSlerpHD(temp7, temp8, &field_0xD04, 0.5f);
      cXyz_mi(temp6, temp12, temp4);
      sePos = &field_0xB84;
      bound = true;
    }
    if (!bound) {
      gabi::Local<cXyz> temp10;
      temp10->x = (f32)temp5->x;
      temp10->y = 0.0f;
      temp10->z = (f32)temp5->z;
      ji1DrawQuatRotVecHD(temp7, temp5, temp10);
      field_0xCD0.y =
          gabi::fnmsubs(field_0xB84.y + field_0xB78.y, 0.5f, field_0xCD0.y);
      ji1DrawQuatSlerpHD(&field_0xD04, temp7, &field_0xD04, 0.25f);
      field_0xCDC.y = field_0xCDC.y * hio_bc;
    }
    if (bound) {
      if (temp12->y < -3.0f && ji1DrawNormalizeHD(temp12)) {
        u32 temp13 = f2u(std::fabs(temp12->y * 100.0f));
        u32 temp14 = temp13 > 100 ? 100 : temp13;
        ji1DrawSeStartHD(JA_SE_CM_LANCE_BOUND, sePos, temp14);
        f32 d = PSVECDotProduct(temp12, &l_baseY);
        f32 k = hio_bc * d;
        field_0xCDC.y = field_0xCDC.y * k;
      } else {
        field_0xCDC.y = 0.0f;
      }
    }
    gabi::Local<cXyz> temp9;
    cXyz_mi(&field_0xB78, temp9, &field_0xB84);
    gabi::Local<cXyz> up;
    up->set(0.0f, 1.0f, 0.0f);
    f32 dot = PSVECDotProduct(temp9, up);
    if (dot < 1.0f) {
      gabi::Local<cXyz> temp10;
      temp10->x = (f32)temp5->x;
      temp10->y = 0.0f;
      temp10->z = (f32)temp5->z;
      ji1DrawQuatRotVecHD(temp7, temp5, temp10);
      field_0xCD0.y =
          gabi::fnmsubs(field_0xB84.y + field_0xB78.y, 0.5f, field_0xCD0.y);
      ji1DrawQuatSlerpHD(&field_0xD04, temp7, &field_0xD04, 0.25f);
      field_0xCDC.y = field_0xCDC.y * hio_bc;
    }
  }
}

/* WWHD glue
ext 028E8F64 void ji1DrawMultVecHD(Mtx34* m, cXyz* a, cXyz* b)
ext 025F19F8 void ji1DrawXYZrotMHD(Mtx34* m, s16 x, s16 y, s16 z)
ext 028249B0 void ji1DrawSetEmitterMatrixHD(Mtx34* m, void* rotation, cXyz* translation)
ext 025BEBB8 void ji1DrawRegistFigHD(s32 type, fopAc_ac_c* actor, cXyz* pos, s16 angle, f32 x, f32 y, f32 z)
ext 025E789C BOOL ji1DrawInitBtpHD(void* anim, J3DModelData* data, void* btp, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry)
ext 025E8154 BOOL ji1DrawInitBrkHD(void* anim, J3DModelData* data, void* brk, s32 play, s32 mode, f32 rate, s16 start, s16 end, bool modify, s32 entry)
ext 0259D54C void ji1DrawPlayerEyePosHD(cXyz* out, f32 offset)
ext 0259DED0 void ji1DrawLookAtTargetHD(dNpc_JntCtrl_c* joint, be<s16>* y, cXyz* target, cXyz* eye, s16 a, s16 b, bool turn)
ext 028E8B48 void ji1DrawQuatMultiplyHD(Ji1DrawQuaternionHD* a, Ji1DrawQuaternionHD* b, Ji1DrawQuaternionHD* out)
ext 028E8C0C void ji1DrawQuatNormalizeHD(Ji1DrawQuaternionHD* a, Ji1DrawQuaternionHD* out)
ext 028E9BC0 void ji1DrawQuatSlerpHD(Ji1DrawQuaternionHD* a, Ji1DrawQuaternionHD* b, Ji1DrawQuaternionHD* out, f32 t)
ext 028E8DAC void ji1DrawVectorSubtractHD(const cXyz* a, const cXyz* b, cXyz* out)
ext 028E9108 void ji1DrawMatrixConcatHD(Mtx34* a, Mtx34* b, Mtx34* out)
ext 025F25CC void ji1DrawQuatMHD(Ji1DrawQuaternionHD* q)
ext 023127F8 void ji1DrawQuatRotVecHD(Ji1DrawQuaternionHD* q, cXyz* a, cXyz* b)
ext 0201B47C BOOL ji1DrawNormalizeHD(cXyz* v)
ext 025E1A04 void ji1DrawSeStartHD(u32 id, cXyz* pos, u32 param)
data l_hio 104672AC
data l_zeroQuat 101E9C38
data l_baseY 101FFBC0
str 1001AEB4
str 1001AEBC
str 1001AEC4
str 1001AECC
str 1001AED4
str 1001AEDC
str 1001AEE4
str 1001AEE8
str 1001AEF0
str 1001AF00
str 1001AF10
str 1001AF1C
str 1001AF30
str 1001AF3C
str 1001AF50
str 1001AF68
str 1001AF7C
str 1001AF90
str 1001AFA4
str 1001AFB0
str 1001AFC0
str 1001AFDC
str 1001AFFC
str 1001B010
str 1001B024
str 1001B034
str 1001B044
str 1001B054
str 1001B064
str 1001B074
str 1001B084
str 1001B090
str 1001B0A0
str 1001B0B0
str 1001B0C4
str 1001B0D8
str 1001B0E8
str 1001B0F8
str 1001B108
*/
