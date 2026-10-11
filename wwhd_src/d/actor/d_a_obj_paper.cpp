/* Notice-board/message actor. Restored from tww onto the verified WWHD
 * behavior. */
#include "d/actor/d_a_obj_paper.h"
using namespace daObjPaper;

const gc_tu_data gc_tu = {.safeString = 0x1002E420};
// HD: all three variants use the retail 0x4C0 heap; resources retain model
// index 3. Numeric model fields below are the Resource_e values, checked
// against the image.
static Attr_c L_attr[3] = {
    {"Opaper", 0x4C0, 3, 0, 40, 40, 0, 29, 30, 1, 0, 0, 0, {0, 0}},
    {"Ppos", 0x4C0, 3, 0, 50, 60, 0, 31, 32, 0, 0, 0, 0, {0, 0}},
    {"Piwa", 0x4C0, 3, 60, 130, 80, 60, 29, 30, 0, 0, 55, 115, {0, 0}}};
static inline const Attr_c &attr(s32 type) { return L_attr[type]; }
static PaperCylinderSourceHD L_cyl_src = {
    {0, 0, 0, 0, 0xFF1DFEFF, 0xF, 0x79, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0}};
static PaperModeEntryHD mode_proc[4] = {{&Act_c::mode_wait},
                                        {&Act_c::mode_talk0},
                                        {&Act_c::mode_talk1},
                                        {&Act_c::mode_talk2}};
using PaperMessagesHD = gptr<PaperMessageManagerHD>;
static PaperMessagesHD sMessages;
static PaperHeaderZerosHD sHeaderZeros;
static PaperRegistrationHD sRegistration0;
static PaperRegistrationHD sRegistration1;
static PaperRegistrationHD sRegistration2;
static f32 sMinAngle;
static f32 sMaxAngle;
static u8 sAngleInit;
static u8 sLimitInit;
static const f32 l_minAngle = -3.1415927f;
static const f32 l_maxAngle = 3.1415927f;

void *Paper_getObjectRes(dRes_control_c *, SafeString *, s32);
void Paper_hitSeStart(cXyz *, s8, dCcD_Cyl *, s32);
void Paper_soundSet(cXyz *, s32, u32, s32);
void Paper_hitEffHibana(fopAc_ac_c *, dCcD_Cyl *);
void fopAcM_setCullSizeSphere(fopAc_ac_c *, f32, f32, f32, f32);
void Paper_registerGlobal(PaperRegistrationHD *);
void Paper_initAngle(be<u8> *);
void Paper_initLimit(be<u8> *);
BOOL solidHeapCB(fopAc_ac_c *i_this);

// HD: construction keeps this translation unit's collider and actor virtual
// tables.
Act_c::Act_c() {
  fopAc_ac_c_ct(this);
  __vtbl = 0x1002E448;
  Paper_constructGObjInf(&mCylinderCol);
  PaperShapeAttrHD &shape =
      *reinterpret_cast<PaperShapeAttrHD *>(mCylinderCol.mShapeAttr);
  shape.mShapeVtable = 0x100015A8;
  shape.mCylinderVtable = 0x1002E438;
  Paper_constructCylinder(&mCylinderCol.mCyl);
  mCylinderCol.__vtbl_hitinf = 0x1004B108;
  mCylinderCol.mCyl.__vtbl = 0x1004B150;
  shape.mShapeVtable = 0x1004B160;
  Paper_constructStts(&mColStatus);
  Paper_constructGStts(&mColStatus.__vtbl_gstts);
  mColStatus.__vtbl = 0x1004AE88;
  mColStatus.__vtbl_gstts = 0x1004AEC0;
}

static u32 PrmAbstract(Act_c *i_this, s32 width, s32 shift);

/* 02381560 */
BOOL solidHeapCB(fopAc_ac_c *i_this) {
  return static_cast<Act_c *>(i_this)->create_heap();
}

/* 023814A0 */
bool Act_c::create_heap() {
  const Attr_c &attributes = attr(mType);
  dRes_control_c *control = dComIfG_resControl();
  SafeString name;
  name.mStringTop = gabi::guest_addr(attributes.mResName.get());
  name.__vtbl = gc_tu.safeString;
  s32 index = attributes.mModelId;
  J3DModelData *mdl_data =
      static_cast<J3DModelData *>(Paper_getObjectRes(control, &name, index));
  JUT_ASSERT(0x141, mdl_data != NULL);
  mpModel = mDoExt_J3DModel__create(mdl_data, 0x80000, 0x11000022);
  return mpModel != nullptr;
}

/* 02381660 */
s32 Act_c::_create() {
  fopAcM_ct(this, Act_c);
  s32 type = PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
  mType = type;
  s32 phase = dComIfG_resLoad(&mPhs, attr(type).mResName);
  if (phase != cPhs_COMPLEATE_e)
    return phase;
  u32 heap = attr(mType).mHeapSize;
  if (!fopAcM_entrySolidHeap(this, solidHeapCB, heap))
    return cPhs_ERROR_e;
  type = mType;
  const Attr_c &attributes = attr(type);
  f32 eyeOffset = f32(attributes.mEyeOffset);
  eyePos.y = eyePos.y + eyeOffset;
  f32 attention = f32(attributes.mAttentionOffset);
  attention_info.position.y = attention_info.position.y + attention;
  u8 talk = attributes.mAttentionDist1;
  u32 attentionFlags = attention_info.flags;
  attention_info.distances[fopAc_Attn_TYPE_TALK_e] = talk;
  u8 speak = attributes.mAttentionDist2;
  mMsgId = -1;
  attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = speak;
  attention_info.flags = attentionFlags | fopAc_Attn_LOCKON_TALK_e |
                         fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_TALKFLAG_READ_e;
  if (type == Piwa_e) {
    u32 status = actor_status;
    type = mType;
    actor_status = (status & ~u32(PAPER_MAP_ICON_MASK_e)) | PAPER_MAP_ICON_e;
  }
  if (attr(type).mColCylinderRadius != 0) {
    mbHasCc = 1;
    mColStatus.Init(255, 255, this);
    mCylinderCol.Set(reinterpret_cast<const dCcD_SrcCyl *>(&L_cyl_src));
    const Attr_c &cylinderAttributes = attr(mType);
    mCylinderCol.SetStts(&mColStatus);
    f32 radius = f32(cylinderAttributes.mColCylinderRadius);
    mCylinderCol.SetR(radius);
    f32 height = f32(attr(mType).mColCylinderHeight);
    mCylinderCol.SetH(height);
  } else
    mbHasCc = 0;
  const Attr_c &cullAttributes = attr(mType);
  f32 y = f32(cullAttributes.mCullSphereYOffset),
      radius = f32(cullAttributes.mCullSphereRadius);
  fopAcM_setCullSizeSphere(this, 0.0f, y, 0.0f, radius);
  fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
  init_mtx();
  mode_wait_init();
  return phase;
}

/* 02381A18 */
BOOL Act_c::_delete() {
  dComIfG_resDelete(&mPhs, attr(mType).mResName);
  return TRUE;
}

/* 02381648 */
void Act_c::mode_wait_init() {
  u32 status = actor_status;
  mMode = ActMode_WAIT_e;
  actor_status = status | fopAcStts_NOCULLEXEC_e;
}

/* 02381C5C */
void Act_c::mode_wait() {
  if (eventInfo.checkCommandTalk())
    mode_talk0_init();
  else
    eventInfo.onCondition(dEvtCnd_CANTALK_e);
}

/* 02381C3C */
void Act_c::mode_talk0_init() {
  u32 status = actor_status;
  mMsgId = -1;
  mMode = ActMode_TALKBEGIN_e;
  actor_status = status & ~fopAcStts_NOCULLEXEC_e;
}

/* 02381C88 */
void Act_c::mode_talk0() {
  if (mMsgId != -1)
    return;
  s32 camera = dComIfGp_getPlayerCameraID(0);
  if (!dComIfGp_checkCameraAttentionStatus(camera, CAMERA_ATTENTION_TALK_e))
    return;
  PaperMessageManagerHD *messages = sMessages;
  u32 message = PrmAbstract(this, PRM_MSG_NO_W, PRM_MSG_NO_S);
  s32 id = messages->messageSet(message, &eyePos);
  mMsgId = id;
  // HD: failed message creation leaves the actor waiting in this mode.
  if (id != -1)
    mode_talk1_init();
}

/* 02381C7C */
void Act_c::mode_talk1_init() { mMode = ActMode_GETMSG_e; }

/* 02381D30 */
void Act_c::mode_talk1() {
  // HD: messages are manager-owned; no message actor lookup or mpMsg field
  // remains.
  mode_talk2_init();
}

/* 02381D24 */
void Act_c::mode_talk2_init() { mMode = ActMode_TALKWAIT_e; }

/* 02381D34 */
void Act_c::mode_talk2() {
  PaperMessageManagerHD *messages = sMessages;
  if (messages->getStatus() == fopMsgStts_BOX_CLOSED_e) {
    messages->setStatus(MESSAGE_DESTROYED_e);
    mMsgId = -1;
    dComIfGp_event_reset();
    mode_wait_init();
  }
}

/* 02381564 */
void Act_c::set_mtx() {
  mDoMtx_stack_c::transS(current.pos);
  mDoMtx_stack_c::ZXYrotM(shape_angle);
  mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02381628 */
void Act_c::init_mtx() {
  J3DModel *model = mpModel;
  // HD: all three scale loads precede the stores, in Y/X/Z order.
  f32 y = scale.y, x = scale.x, z = scale.z;
  model->mBaseScale.x = x;
  model->mBaseScale.y = y;
  model->mBaseScale.z = z;
  set_mtx();
}

/* 02381A54 */
void Act_c::damage_cc_proc() {
  if (mCylinderCol.ChkTgHit()) {
    Paper_hitSeStart(&eyePos, current.roomNo, &mCylinderCol, 0x0D);
    cXyz position;
    position.x = current.pos.x;
    position.y = current.pos.y;
    position.z = current.pos.z;
    Paper_soundSet(&position, 4, mBsPcId, 100);
    Paper_hitEffHibana(this, &mCylinderCol);
    mCylinderCol.ClrTgHit();
  }
  mColStatus.Move();
}

/* 02381AEC */
BOOL Act_c::_execute() {
  if (mbHasCc)
    damage_cc_proc();
  (this->*mode_proc[mMode].function)();
  set_mtx();
  if (mbHasCc) {
    mCylinderCol.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCylinderCol);
  }
  return TRUE;
}

/* 02381BC4 */
BOOL Act_c::_draw() {
  // HD: the retained caller passes mTevType XOR 1, including non-boolean
  // values.
  u32 tev = attr(mType).mTevType ^ 1u;
  g_env_light.settingTevStruct(tev, &current.pos, &tevStr);
  g_env_light.setLightTevColorType(mpModel, &tevStr);
  mDoExt_modelUpdateDL(mpModel);
  return TRUE;
}

/* 02381DA8 */
s32 Mthd_Create(Act_c *i_this) { return i_this->_create(); }

/* 02381DAC */
BOOL Mthd_Delete(Act_c *i_this) { return i_this->_delete(); }

/* 02381DB0 */
BOOL Mthd_Execute(Act_c *i_this) { return i_this->_execute(); }

/* 02381DB4 */
BOOL Mthd_Draw(Act_c *i_this) { return i_this->_draw(); }

/* 02381ED0 */
BOOL Mthd_IsDelete(Act_c *i_this) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 02381DB8 */
static void __sinit_d_a_obj_paper_cpp() {
  for (u32 i = 0; i < 4; ++i)
    sHeaderZeros.words[i] = 0;
  Paper_registerGlobal(&sRegistration0);
  sMinAngle = l_minAngle;
  sMaxAngle = l_maxAngle;
  Paper_initAngle(&sAngleInit);
  Paper_registerGlobal(&sRegistration1);
  Paper_initLimit(&sLimitInit);
  Paper_registerGlobal(&sRegistration2);
}

/* 02381E4C */
static void Hio_destructHD(u8 *i_this, s32 flags) {
  if (i_this && (flags & 1))
    operator_delete(i_this);
}

/* 02381E60 */
static void SafeString_assureTerminationImpl_() {}

/* 02381E64 */
static void Act_destructHD(Act_c *i_this, s32 flags) {
  if (i_this) {
    Paper_destructStts(&i_this->mColStatus, 2);
    Paper_destructCylinder(&i_this->mCylinderCol, 2);
    __dt(i_this);
    if (flags & 1)
      operator_delete(i_this);
  }
}

/* 02381ED8 */
static u32 PrmAbstract(Act_c *i_this, s32 width, s32 shift) {
  // HD: retain PPC's zero result for shifts with bit 5 set.
  u32 value = i_this->mParameters;
  u32 left = (u32(width) & 32) ? 0 : 1u << (u32(width) & 31);
  u32 right = (u32(shift) & 32) ? 0 : value >> (u32(shift) & 31);
  return right & (left - 1);
}

/* WWHD glue
str 1002E408 "Opaper"
str 1002E410 "Ppos"
str 1002E418 "Piwa"
str 1002E4AC "d_a_obj_paper.cpp"
str 1002E4C0 "mdl_data != NULL" renamed # HD: mdl_data != 0
 data L_attr 1002E458
 data L_cyl_src 1002E508
 data mode_proc 1002E4DC
 data sMessages 101F4B5C
 data sHeaderZeros 1046BBAC
 data sRegistration0 101CC24C
 data sRegistration1 101CC258
 data sRegistration2 101CC264
 data sMinAngle 1046BBA0
 data sMaxAngle 1046BBA4
 data sAngleInit 1046BBA8
 data sLimitInit 1046BBA9
 data l_minAngle 1002E4FC
 data l_maxAngle 1002E500
ext 026066C4 void* Paper_getObjectRes(dRes_control_c*, SafeString*, s32)
ext 023129C4 void Paper_hitSeStart(cXyz*, s8, dCcD_Cyl*, s32)
ext 0255F458 void Paper_soundSet(cXyz*, s32, u32, s32)
ext 02312E54 void Paper_hitEffHibana(fopAc_ac_c*, dCcD_Cyl*)
ext 025D6768 void fopAcM_setCullSizeSphere(fopAc_ac_c*, f32, f32, f32, f32)
ext 025F7DB0 s32 PaperMessageManagerHD::messageSet(u32,cXyz*)
ext 025F795C s32 PaperMessageManagerHD::getStatus()
ext 025F74D0 void PaperMessageManagerHD::setStatus(s32)
ext 02515FB8 void Paper_constructGObjInf(dCcD_Cyl*)
ext 02018590 void Paper_constructCylinder(cM3dGCyl*)
ext 0200BD2C void Paper_constructStts(dCcD_Stts*)
ext 02515DA0 void Paper_constructGStts(be<u32>*)
ext 02515860 void Paper_destructStts(dCcD_Stts*,s32)
ext 02515A70 void Paper_destructCylinder(dCcD_Cyl*,s32)
ext 028F026C void Paper_registerGlobal(PaperRegistrationHD*)
ext 028ED6F8 void Paper_initAngle(be<u8>*)
ext 028EAB2C void Paper_initLimit(be<u8>*)
*/
