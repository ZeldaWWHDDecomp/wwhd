/* Hami2 hinged gate. Derived from WWHD disassembly;
 */
#include "d/actor/d_a_obj_hami2.h"
using daObjHami2::Act_c;
enum Mode { MODE_CLOSED, MODE_WAIT_OPEN, MODE_OPENING, MODE_OPEN, MODE_WAIT_CLOSE, MODE_CLOSING };
// HD: retain float loads before stores for overlapping matrix buffers.
static Mtx34 sMatrixStack;
static Mtx34 sBgMatrix;
static Mtx34 sCurrentMtx;
static Mtx34* sMatrixStackPtr;
static J3DModel* sCurrentModel;
static Hami2DrawBuffers sDrawBuffers;
static void* sResourceManager;
static Hami2SaveData* sSaveData;
static Hami2HeaderStatics sHeaderStatics;
static Hami2DestructorRecords sDestructorRecords;
static void copyMatrix(const Mtx34* src, Mtx34* dst) {
  f32 v[12];
  for (int i = 0; i < 12; ++i)
    v[i] = src->m[i/4][i%4];
  for (int i = 0; i < 12; ++i)
    dst->m[i/4][i%4]=v[i];
}
/* 0234EC5C */
u32 daObjHami2::Act_c::prm_get(s32 width, s32 shift) {
  u32 w = (u32)width & 63, s = (u32)shift & 63;
  return (s < 32 ? this->mParameters >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
/* 0234E09C */
s32 daObjHami2::Act_c::Mthd_Create() {
  u32 flags = this->actor_condition;
  if (!(flags & 8)) {
    if (this) {
      hami2ConstructMoveBg(this);
      flags = this->actor_condition;
      this->__vtbl=0x10029C80;
    }
    this->actor_condition=flags | 8;
  }
  s32 phase = hami2LoadResource(&this->mPhase, "Hami2");
  if (phase == 4) {
    phase = hami2CreateMoveBg(this, "Hami2", 8, 0x024EE708u, 0x34E0);
    Hami2GameInfo* p = hami2GetGameInfo();
    dBgW* bg = this->mpMovingBg;
    hami2RegisterBackground(&p->mBgS, bg, this);
    if (phase != 4 && phase != 5)
      hami2Assert("d_a_obj_hami2.cpp", 0xD0, "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
  }
  return phase;
}
/* 0234E184 */
s32 daObjHami2::Act_c::Mthd_Delete() {
  if (this->heap) {
    dBgW* bg = this->mpMovingBg;
    if (bg && bg->m_id < 256) {
      Hami2GameInfo* p = hami2GetGameInfo();
      hami2ReleaseBackground(&p->mBgS, this->mpMovingBg);
    }
  }
  s32 r = hami2DeleteMoveBg(this);
  hami2DeleteResource(&this->mPhase, "Hami2");
  return r;
}
/* 0234E208 */
s32 daObjHami2::Act_c::CreateHeap() {
  gabi::Local<SafeString> name;
  name->mStringTop=gabi::guest_addr("Hami2");
  name->__vtbl=0x10029B9C;
  void* data = hami2GetResource(sResourceManager.get(), name.get(), 4);
  if (!data)
    hami2Assert("d_a_obj_hami2.cpp", 0x68, "modelData != (0)");
  J3DModel* model = hami2CreateModel(data, 0, 0x11020203u);
  this->mpModel=model;
  if (!model)
    return 0;
  s32 idx=model->getModelData()->getJointName()->getIndex("mono1");
  if (idx>=0) {
    J3DModel* currentModel=mpModel;
    currentModel->getModelData()->getJointNodePointer((u16)idx)->setCallBack(nodeCallBack);
  }
  mpModel->mUserArea=gabi::ea(this);
  f32 x = this->current.pos.x, y = this->current.pos.y,
      z = this->current.pos.z;
  hami2TranslateMatrix(&sMatrixStack, x, y, z);
  hami2RotateY(&sMatrixStack, this->shape_angle.y);
  x = this->scale.x;
  y = this->scale.y;
  z = this->scale.z;
  hami2ScaleMatrix(x, y, z);
  hami2CopyMatrix(&sMatrixStack, &this->mInitialBgMtx);
  dBgW* bg = hami2ConstructBgW(0);
  this->mpMovingBg=bg;
  if (!bg)
    return 0;
  gabi::Local<SafeString> name2;
  name2->mStringTop=gabi::guest_addr("Hami2");
  name2->__vtbl=0x10029B9C;
  data = hami2GetResource(sResourceManager.get(), name2.get(), 7);
  s32 r = hami2SetBackground(this->mpMovingBg, data, 1, &this->mInitialBgMtx);
  return r == 0;
}
/* 0234E3B0 */
void daObjHami2::Act_c::set_mtx() {
  f32 x = this->current.pos.x, y = this->current.pos.y,
      z = this->current.pos.z;
  hami2TranslateMatrix(&sMatrixStack, x, y, z);
  s16 rx = this->shape_angle.x, ry = this->shape_angle.y,
      rz = this->shape_angle.z;
  hami2RotateZXY(&sMatrixStack, rx, ry, rz);
  copyMatrix(&sMatrixStack, &mpModel->mBaseTransformMtx);
  hami2RotateY(&sMatrixStack, this->mRotation);
  hami2CopyMatrix(&sMatrixStack, &sBgMatrix);
}
/* 0234E490 */
void daObjHami2::Act_c::init_mtx() {
  J3DModel* model = this->mpModel;
  f32 x = this->scale.x, y = this->scale.y,
      z = this->scale.z;
  model->mBaseScale.x=x;
  model->mBaseScale.y=y;
  model->mBaseScale.z=z;
  this->set_mtx();
}
static s32 switchState(Act_c* a) {
  u32 sw = a->prm_get(8, 0);
  Hami2SaveData* save=sSaveData;
  return hami2CheckSwitch(&save->mSwitch, sw, (s8)a->home.roomNo);
}
/* 0234E4B0 */
s32 daObjHami2::Act_c::Create() {
  J3DModel* m = this->mpModel;
  cullMtx=m ? gabi::ea(&m->mBaseTransformMtx) : 0;
  bool open = switchState(this) != 0;
  this->mMode=open ? MODE_OPEN : MODE_CLOSED;
  this->mRotation=open ? 0x4000 : 0;
  this->init_mtx();
  hami2SetCullBox(this, -1200.0f, -40000.0f, -1200.0f, 1200.0f, 40000.0f,
             1200.0f);
  Hami2GameInfo* p = hami2GetGameInfo();
  s32 e = hami2GetEventIndex(&p->mEvtManager, "AMI2_OPEN", 255);
  this->mOpenEventIdx=e;
  p = hami2GetGameInfo();
  e = hami2GetEventIndex(&p->mEvtManager, "AMI2_CLOSE", 255);
  this->mCloseEventIdx=e;
  return 1;
}
static void order(Act_c* a, s16 event) {
  hami2OrderEvent(a, event, 255, 65535, 0, 1);
}
/* 0234E628 */
void daObjHami2::Act_c::daObjHami2_close_stop() {
  if (switchState(this)) {
    order(this, mOpenEventIdx);
    this->mMode=MODE_WAIT_OPEN;
  }
}
/* 0234E7D8 */
void daObjHami2::Act_c::daObjHami2_open_stop() {
  if (!switchState(this)) {
    order(this, mCloseEventIdx);
    this->mMode=MODE_WAIT_CLOSE;
  }
}
/* 0234E6A8 */
void daObjHami2::Act_c::daObjHami2_open_demo_wait() {
  if (this->eventInfo.mCommand == 2) {
    s32 room = (s8)this->current.roomNo;
    this->mMode=MODE_OPENING;
    s32 reverb = hami2GetReverb(room);
    hami2StartSound(0x69C4, &this->current.pos, 0, reverb);
    hami2StartSystemSound(0x806);
  } else
    order(this, mOpenEventIdx);
}
/* 0234E858 */
void daObjHami2::Act_c::daObjHami2_close_demo_wait() {
  if (this->eventInfo.mCommand == 2)
    this->mMode=MODE_CLOSING;
  else
    order(this, mCloseEventIdx);
}
static void shock() {
  Hami2GameInfo* p = hami2GetGameInfo();
  gabi::Local<cXyz> direction;
  direction->x=0;
  direction->y=1;
  direction->z=0;
  hami2StartShock(&p->mVibration, 4, -33, direction.get());
  p = hami2GetGameInfo();
  p->mEventFlag=(u16)p->mEventFlag | 8;
}
/* 0234E740 */
void daObjHami2::Act_c::daObjHami2_open_demo() {
  s16 rot = (s16)(this->mRotation + 0x100);
  if (rot < 0x4000)
    this->mRotation=rot;
  else {
    this->mRotation=0x4000;
    this->mMode=MODE_OPEN;
    shock();
  }
}
/* 0234E88C */
void daObjHami2::Act_c::daObjHami2_close_demo() {
  s16 rot = (s16)(this->mRotation - 0x100);
  if (rot > 0)
    this->mRotation=rot;
  else {
    this->mRotation=0;
    shock();
    this->mMode=MODE_CLOSED;
  }
}
/* 0234E938 */
s32 daObjHami2::Act_c::Execute(be<u32>* out) {
  switch (this->mMode) {
  case MODE_CLOSED:
    this->daObjHami2_close_stop();
    break;
  case MODE_WAIT_OPEN:
    this->daObjHami2_open_demo_wait();
    break;
  case MODE_OPENING:
    this->daObjHami2_open_demo();
    break;
  case MODE_OPEN:
    this->daObjHami2_open_stop();
    break;
  case MODE_WAIT_CLOSE:
    this->daObjHami2_close_demo_wait();
    break;
  case MODE_CLOSING:
    this->daObjHami2_close_demo();
    break;
  }
  this->set_mtx();
  *out=gabi::ea(&sBgMatrix);
  return 1;
}
/* 0234EA88 */
s32 daObjHami2::Act_c::Draw() {
  void* env = hami2GetEnvironment();
  hami2SetLight(env, 1, &this->current.pos, &this->tevStr);
  env = hami2GetEnvironment();
  hami2ApplyLight(env, this->mpModel, &this->tevStr);
  Hami2GameInfo* p = hami2GetGameInfo();
  sDrawBuffers.mOpaque=p->mOpaqueCurrent;
  p = hami2GetGameInfo();
  sDrawBuffers.mTranslucent=p->mTranslucentCurrent;
  hami2UpdateModel(this->mpModel, 0);
  p = hami2GetGameInfo();
  sDrawBuffers.mOpaque=p->mOpaqueNormal;
  p = hami2GetGameInfo();
  sDrawBuffers.mTranslucent=p->mTranslucentNormal;
  return 1;
}
/* 0234EC00 */
s32 daObjHami2::Act_c::Delete() {
  return 1;
}
/* 0234DF78 */
s32 nodeCallBack(J3DNode* joint, s32 phase) {
  if (phase==0) {
    Hami2JointData* data=hami2GetJointData(joint);
    J3DModel* model=sCurrentModel;
    Act_c* actor=gabi::at<Act_c>(model->mUserArea);
    u32 idx=data->mJointNo;
    if(actor) {
      Hami2MtxBuffer* block=reinterpret_cast<Hami2MtxBuffer*>(model->mpMtxBuffer.get());
      u32 flags=block->mFlags;
      Mtx34* matrices=block->mpAnmMtx;
      block->mFlags=flags|16;
      Mtx34* stack=sMatrixStackPtr;
      hami2CopyMatrix(&matrices[idx],stack);
      stack=sMatrixStackPtr;
      hami2RotateY(stack,actor->mRotation);
      block=reinterpret_cast<Hami2MtxBuffer*>(model->mpMtxBuffer.get());
      flags=block->mFlags;
      stack=sMatrixStackPtr;
      matrices=block->mpAnmMtx;
      block->mFlags=flags|16;
      copyMatrix(stack,&matrices[idx]);
      hami2CopyMatrix(sMatrixStackPtr,&sCurrentMtx);
    }
  }
  return 1;
}
/* 0234EB20 */
s32 wrapperCreate(Act_c* a) {
  return a->Mthd_Create();
}
/* 0234EB24 */
s32 wrapperDelete(Act_c* a) {
  return a->Mthd_Delete();
}
/* 0234EB28 */
s32 wrapperExecute(Act_c* a) {
  return hami2ExecuteMoveBg(a);
}
/* 0234EB2C */
s32 daObjHami2_Draw(Act_c* a) {
  return a->Draw_v();
}
/* 0234EB3C */
s32 daObjHami2_IsDelete(Act_c* a) {
  return a->IsDelete_v();
}
/* 0234EB4C */
void staticInit() {
  sHeaderStatics.mZero[2]=0;
  sHeaderStatics.mZero[0]=0;
  sHeaderStatics.mZero[3]=0;
  sHeaderStatics.mZero[1]=0;
  hami2RegisterGlobal(&sDestructorRecords.mRecords[0]);
  sHeaderStatics.mMinAngle=-3.1415927410125732f;
  sHeaderStatics.mMaxAngle=3.1415927410125732f;
  hami2InitializeHeaderByte(&sHeaderStatics.mHeaderByte);
  hami2RegisterGlobal(&sDestructorRecords.mRecords[1]);
  hami2InitializeHeaderFlag(&sHeaderStatics.mHeaderFlag);
  hami2RegisterGlobal(&sDestructorRecords.mRecords[2]);
}
/* 0234EBE0 */
void staticDtor(void* a, s32 flags) {
  if (a && (flags & 1))
    hami2DeleteObject(a);
}
/* 0234EC08 */
void actorDtor(Act_c* a, s32 flags) {
  if (a) {
    hami2DestroyActor(a, 0);
    if (flags & 1)
      hami2DeleteObject(a);
  }
}


/* ---- leftover functions of the translation unit ---- */

/* 0234EBF4 dBgS_MoveBgActor::IsDelete (out-of-line copy; daObjHami2::Act_c vtable slot 10029CBC) */
/* 0234EBF4 */
static BOOL hami2_MoveBgActor_IsDelete(void* p) {
    return TRUE;
}

/* 0234EBFC sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10029BB0, after the destructor 0234EBE0 */
/* 0234EBFC */
static void hami2_SafeString_assureTermination(void* p) {
}

/* WWHD glue
data sMatrixStack 1048D0CC
data sBgMatrix 10469D1C
data sCurrentMtx 104B4868
data sMatrixStackPtr 1018C7B0
data sCurrentModel 104B462C
data sDrawBuffers 104B4634
data sResourceManager 101F4F28
data sSaveData 101F84DC
data sHeaderStatics 10469D00
data sDestructorRecords 101C9B54
ext 024F1D40 void hami2ConstructMoveBg(Act_c*)
ext 02520460 s32 hami2LoadResource(request_of_phase_process_class*, const char*)
ext 024F1D9C s32 hami2CreateMoveBg(Act_c*, const char*, s32, u32, u32)
ext 024EEA6C void hami2RegisterBackground(void*, dBgW*, Act_c*)
ext 020087EC void hami2ReleaseBackground(void*, dBgW*)
ext 024F1F64 s32 hami2DeleteMoveBg(Act_c*)
ext 025204C8 void hami2DeleteResource(request_of_phase_process_class*, const char*)
ext 0273AA24 void hami2Assert(const char*, s32, const char*)
ext 026066C4 void* hami2GetResource(void*, SafeString*, s32)
ext 025E38E0 J3DModel* hami2CreateModel(void*, u32, u32)
ext 028E93CC void hami2TranslateMatrix(Mtx34*, f32, f32, f32)
ext 025F1C28 void hami2RotateY(Mtx34*, s16)
ext 025F2518 void hami2ScaleMatrix(f32, f32, f32)
ext 028E90D4 void hami2CopyMatrix(const Mtx34*, Mtx34*)
ext 024F23F4 dBgW* hami2ConstructBgW(u32)
ext 0200A030 s32 hami2SetBackground(dBgW*, void*, u32, Mtx34*)
ext 025F1B48 void hami2RotateZXY(Mtx34*, s16, s16, s16)
ext 025BA0C0 s32 hami2CheckSwitch(void*, u32, s8)
ext 025D674C void hami2SetCullBox(Act_c*, f32, f32, f32, f32, f32, f32)
ext 02543F10 s32 hami2GetEventIndex(void*, const char*, u32)
ext 025D7A58 void hami2OrderEvent(Act_c*, s16, u32, u32, u32, u32)
ext 02520540 s32 hami2GetReverb(s32)
ext 025E1A40 void hami2StartSound(u32, cXyz*, u32, s32)
ext 025E1988 void hami2StartSystemSound(u32)
ext 025CB374 void hami2StartShock(void*, u32, s32, cXyz*)
ext 02555D0C void* hami2GetEnvironment()
ext 025626A4 void hami2SetLight(void*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void hami2ApplyLight(void*, J3DModel*, dKy_tevstr_c*)
ext 025E2DE0 void hami2UpdateModel(J3DModel*, u32)
ext 024F1E9C s32 hami2ExecuteMoveBg(Act_c*)
ext 028F026C void hami2RegisterGlobal(void*)
ext 028ED6F8 void hami2InitializeHeaderByte(void*)
ext 028EAB2C void hami2InitializeHeaderFlag(void*)
ext 0273AF40 void hami2DeleteObject(void*)
ext 025D50BC void hami2DestroyActor(Act_c*, s32)
ext 025200D4 Hami2GameInfo* hami2GetGameInfo()
ext 027F7878 Hami2JointData* hami2GetJointData(J3DNode*)
str 10029C78
str 10029BBC in 0234E09C
str 10029BD0
str 10029C1C in 0234E208
str 10029C30
str 10029C14
str 10029C54
str 10029C60
*/
