/* Wind Temple Iron Boots springboard. Verified HD semantics, with TWW
 * layout/names. */
#include "d/actor/d_a_obj_jump.h"
using namespace daObjJump;
static s32 jnodeCB_lower(J3DNode *, s32);
static void rideCB(void *, Act_c *, fopAc_ac_c *);

// HD: native constructor vtable and SafeString table are translation-unit
// specific.
static constexpr u32 kActorVtableHD = 0x1002BAE8;
static constexpr u32 kSafeStringVtableHD = 0x1002B968;
static constexpr u32 kMoveBgProcHD = 0x024EE658;
enum {
  ActorConstructedHD = 8,
  fpcNm_PLAYER_e = 0xA8,
  HeavyBootsHD = 0x02000000,
  PlayerHangHD = 0x100,
  ForceVomitJumpHD = 0x10,
  JA_SE_OBJ_JUMP_SPR_CLOSE = 0x69B9,
  JA_SE_OBJ_JUMP_SPR_OPEN = 0x69BA
};
static const char M_arcname[] = "Hjump";
static const char l_createFile[] = "d_a_obj_jump.cpp";
static const char l_heapFile[] = "d_a_obj_jump.cpp";
static const char l_phaseAssert[] =
    "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)";
static const char l_modelAssert[] = "model_data != 0";
static JumpAttributesHD l_attributes;
static JumpModeTableHD l_modeProc;
static Mtx34 l_matrixStack;
static Mtx34 M_tmp_mtx;
using JumpResourcePointerHD = gptr<void>;
static JumpResourcePointerHD l_resources;
static JumpDrawListsHD l_drawLists;
static JumpHeaderStaticsHD l_headerStatics;
static JumpHeaderConstantsHD l_headerConstants;
static JumpRegistrationsHD l_registration;
const Attr_c &Act_c::attr() const { return l_attributes.mEntries[s32(mType)]; }
void jumpMoveBgConstructHD(Act_c *actor);
s32 jumpResLoadHD(request_of_phase_process_class *phase, const char *name);
s32 jumpGetTypeHD(Act_c *actor, s32 width, s32 shift);
s32 jumpMoveBgCreateHD(Act_c *actor, const char *name, s16 dzb, void *callback,
                       u32 heap);
void jumpAssertHD(const char *file, s32 line, const char *expression);
s32 jumpMoveBgDeleteHD(Act_c *actor);
void jumpResDeleteHD(request_of_phase_process_class *phase, const char *name);
void jumpMtxCopyHD(const Mtx34 *source, Mtx34 *target);
void jumpScaleMHD(f32 x, f32 y, f32 z);
J3DModelData *jumpGetResourceHD(void *resources, SafeString *name, s16 index);
J3DModel *jumpCreateModelHD(J3DModelData *data, u32 flags, u32 materialFlags);
void jumpTransSHD(Mtx34 *matrix, f32 x, f32 y, f32 z);
void jumpRotateZXYHD(Mtx34 *matrix, s16 x, s16 y, s16 z);
void jumpTransMHD(f32 x, f32 y, f32 z);
void jumpCullBoxHD(Act_c *actor, f32 minX, f32 minY, f32 minZ, f32 maxX,
                   f32 maxY, f32 maxZ);
JumpPlayHD *jumpGetPlayHD();
void *jumpGetEnvLightHD();
void jumpSetTevHD(void *env, s32 type, cXyz *position, dKy_tevstr_c *tev);
void jumpLightTevHD(void *env, J3DModel *model, dKy_tevstr_c *tev);
void jumpModelUpdateHD(J3DModel *model, s32 flags);
s32 jumpReverbHD(s8 room);
void jumpSoundHD(u32 sound, cXyz *position, s32 flags, s32 reverb);
s32 jumpMoveBgExecuteHD(Act_c *actor);
void jumpRegisterHD(JumpRegistrationHD *registration);
void jumpTrigInitHD(be<u8> *state);
void jumpRandomInitHD(be<u8> *state);
void jumpDeleteHD(void *object);
void jumpActorDestroyHD(Act_c *actor, s32 flags);

static void seStart(Act_c *actor, u32 id) {
  s8 room = actor->current.roomNo;
  s32 reverb = jumpReverbHD(room);
  jumpSoundHD(id, &actor->eyePos, 0, reverb);
}
/* 02365F68 */
s32 Act_c::CreateHeap() {
  const Attr_c &table = attr();
  void *resources = l_resources.get();
  gabi::Local<SafeString> name;
  name->mStringTop = gabi::guest_addr(M_arcname);
  name->__vtbl = kSafeStringVtableHD;
  s16 index = table.bdlResIndex;
  J3DModelData *data = jumpGetResourceHD(resources, name.get(), index);
  if (!data)
    jumpAssertHD(l_heapFile, 0x11A, l_modelAssert);
  mModel = jumpCreateModelHD(data, 0x80000, 0x11000022);
  if (mModel.get()) {
    u16 spring = u16(attr().springJntNum);
    data->getJointNodePointer(spring)->setCallBack(jnodeCB_lower);
    spring = u16(attr().springJntNum);
    // HD: the second spring joint also compensates its inherited X scale.
    data->getJointNodePointer(u16(spring + 1))->setCallBack(jnodeCB_lower);
    mModel->setUserArea(reinterpret_cast<uintptr_t>(this));
  }
  return mModel.get() != nullptr;
}

/* 023662E4 */
s32 Act_c::Create() {
  J3DModel *model = mModel.get();
  field_0x338 = 1.0f;
  field_0x33C = 1.0f;
  cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
  init_mtx();
  const Attr_c &table = attr();
  f32 minx = s16(table.cullSizeBoxMin.x), miny = s16(table.cullSizeBoxMin.y),
      minz = s16(table.cullSizeBoxMin.z);
  f32 maxx = s16(table.cullSizeBoxMax.x), maxy = s16(table.cullSizeBoxMax.y),
      maxz = s16(table.cullSizeBoxMax.z);
  jumpCullBoxHD(this, minx, miny, minz, maxx, maxy, maxz);
  if (s32(mType) == Type_SPRING_ON_BOX_e) {
    u32 status = actor_status;
    gbaName = 0;
    actor_status = status & ~63u;
  }
  reinterpret_cast<JumpBackgroundHD *>(mpBgW.get())->setRideCallback(rideCB);
  // HD: the GameCube ground-check and simple-shadow path is omitted.
  mode_wait_init();
  return 1;
}

/* 02365C98 */
s32 Act_c::Mthd_Create() {
  u32 condition = actor_condition;
  if (!(condition & ActorConstructedHD)) {
    if (gabi::ea(this)) {
      jumpMoveBgConstructHD(this);
      condition = actor_condition;
      __vtbl = kActorVtableHD;
    }
    actor_condition = condition | ActorConstructedHD;
  }
  s32 phase = jumpResLoadHD(&mPhase, M_arcname);
  if (phase == cPhs_COMPLEATE_e) {
    mType = jumpGetTypeHD(this, PRM_TYPE_W, PRM_TYPE_S);
    const Attr_c &table = attr();
    u32 heap = table.heapSize;
    s16 dzb = table.dzbResIndex;
    phase = jumpMoveBgCreateHD(this, M_arcname, dzb,
                               gabi::at<void>(kMoveBgProcHD), heap);
    // HD: the phase assertion uses line 390 in the native source.
    if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
      jumpAssertHD(l_createFile, 0x186, l_phaseAssert);
  }
  return phase;
}

/* 02366C88 */
s32 Act_c::Delete() { return 1; }

/* 02365D8C */
s32 Act_c::Mthd_Delete() {
  s32 result = jumpMoveBgDeleteHD(this);
  jumpResDeleteHD(&mPhase, M_arcname);
  return result;
}

/* 023660A4 */
void Act_c::set_mtx() {
  Mtx34 *matrix = &l_matrixStack;
  f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
  jumpTransSHD(matrix, x, y, z);
  s16 rx = shape_angle.x, ry = shape_angle.y, rz = shape_angle.z;
  jumpRotateZXYHD(matrix, rx, ry, rz);
  mtx_copy(&mModel->mBaseTransformMtx, matrix);
  const Attr_c &table = attr();
  f32 scale = field_0x33C, offset = f32(table.field_0x024);
  f32 low = f32(table.field_0x018), high = f32(table.field_0x01C),
      span = f32(table.field_0x020) - offset;
  f32 ratio = gabi::fmadds(high - low, scale - 1.0f, span) / span;
  jumpTransMHD(0.0f, offset, 0.0f);
  jumpScaleMHD(1.0f, ratio, 1.0f);
  offset = f32(attr().field_0x024);
  jumpTransMHD(0.0f, -offset, 0.0f);
  jumpMtxCopyHD(matrix, &M_tmp_mtx);
}

/* 02366248 */
void Act_c::init_mtx() {
  J3DModel *model = mModel.get();
  f32 x = scale.x, y = scale.y, z = scale.z;
  model->mBaseScale.x = x;
  model->mBaseScale.y = y;
  model->mBaseScale.z = z;
  set_mtx();
}

/* 0236642C */
void Act_c::set_push_flag() {
  field_0x345 = 0;
  if (mIsRide) {
    u8 count = field_0x347;
    if (count < 255)
      field_0x347 = ++count;
    if (count == u8(attr().field_0x043))
      field_0x345 = 1;
  } else
    field_0x347 = 0;
  field_0x346 = 0;
  if (mIsRide)
    field_0x348 = 0;
  else {
    u8 count = field_0x348;
    if (count < 255)
      field_0x348 = ++count;
    if (count == u8(attr().field_0x044))
      field_0x346 = 1;
  }
  field_0x34a = 0;
  bool player = mIsPlayerRide != 0;
  if (player) {
    JumpPlayHD *play = jumpGetPlayHD();
    player = (u32(play->mPlayerStatus0) & PlayerHangHD) != 0;
  }
  if (player) {
    u8 count = field_0x34b;
    if (count < 255)
      field_0x34b = ++count;
    if (count == u8(attr().field_0x045))
      field_0x34a = 1;
  } else
    field_0x34b = 0;
  field_0x34d = 0;
  if (mIsHeavyRide) {
    u8 count = field_0x34f;
    if (count < 255)
      field_0x34f = ++count;
    if (count == u8(attr().field_0x046))
      field_0x34d = 1;
  } else
    field_0x34f = 0;
  field_0x34e = 0;
  if (mIsHeavyRide)
    field_0x350 = 0;
  else {
    u8 count = field_0x350;
    if (count < 255)
      field_0x350 = ++count;
    if (count == u8(attr().field_0x047))
      field_0x34e = 1;
  }
}

/* 023666B0 */
void Act_c::clear_push_flag() {
  mIsRide = 0;
  field_0x354 = 0;
  mIsHeavyRide = 0;
  mIsPlayerRide = 0;
}

/* 02366668 */
void Act_c::calc_vib_pos() {
  const Attr_c &table = attr();
  f32 target = field_0x338, scale = field_0x33C, velocity = field_0x340;
  velocity = gabi::fnmsubs(scale - target, f32(table.field_0x02C), velocity);
  field_0x340 = velocity;
  velocity = gabi::fnmsubs(velocity, f32(table.field_0x028), velocity);
  field_0x340 = velocity;
  field_0x33C = scale + velocity;
}

/* 02366268 */
static void rideCB(void *bg, Act_c *actor, fopAc_ac_c *rider) {
  actor->mIsRide = 1;
  if (!rider || rider->mProfName != fpcNm_PLAYER_e)
    return;
  actor->mIsPlayerRide = 1;
  u32 flags = static_cast<JumpPlayerHD *>(rider)->mNoResetFlg0;
  if (flags & HeavyBootsHD)
    actor->mIsHeavyRide = 1;
  f32 speed = rider->speedF, threshold = f32(actor->attr().field_0x04C);
  // HD: this unordered compare also starts wobbling for a NaN speed.
  if (!(speed <= threshold))
    actor->field_0x354 = 1;
}

/* 02365DD8 */
static s32 jnodeCB_lower(J3DNode *node, s32 timing) {
  if (timing == 0) {
    J3DModel *model = j3dSys.getModel();
    Act_c *i_this = static_cast<Act_c *>(model->getUserArea());
    u16 index = reinterpret_cast<J3DJoint *>(node)->getJntNo();
    Mtx34 *matrix = model->getAnmMtx(index);
    jumpMtxCopyHD(matrix, &l_matrixStack);
    // HD: both spring joints have callbacks; the second cancels the X stretch.
    if (index == 1)
      jumpScaleMHD(i_this->field_0x33C, 1.0f, 1.0f);
    else if (index == 2)
      jumpScaleMHD(1.0f / f32(i_this->field_0x33C), 1.0f, 1.0f);
    matrix = model->getAnmMtx(index);
    mtx_copy(matrix, &l_matrixStack);
    const Attr_c &table = i_this->attr();
    f32 scale = i_this->field_0x33C, low = table.field_0x018,
        high = table.field_0x01C;
    J3DSys::mCurrentMtx->m[1][3] =
        gabi::fmadds(high - low, scale - 1.0f, J3DSys::mCurrentMtx->m[1][3]);
  }
  return 1;
}

/* 023662C8 */
void Act_c::mode_wait_init() {
  mMode = Mode_WAIT_e;
  field_0x338 = 1.0f;
  field_0x352 = 0;
}

/* 0236687C */
void Act_c::mode_wait() {
  s16 timer = field_0x352;
  if (timer <= 0 &&
      (field_0x345 || field_0x346 || field_0x34a || field_0x34d)) {
    field_0x340 = f32(field_0x340) + f32(attr().field_0x030);
    field_0x352 = u8(attr().field_0x048);
  } else if (timer > 0)
    field_0x352 = s16(timer - 1);
  if (u8(field_0x34f) > u8(attr().field_0x040)) {
    seStart(this, JA_SE_OBJ_JUMP_SPR_CLOSE);
    mode_w_l_init();
  }
}

/* 0236685C */
void Act_c::mode_w_l_init() {
  mMode = Mode_WAIT_LOWER_e;
  field_0x338 = f32(attr().field_0x038);
}

/* 023669AC */
void Act_c::mode_w_l() {
  if (!(f32(field_0x33C) > f32(field_0x338)))
    mode_lower_init();
}

/* 02366998 */
void Act_c::mode_lower_init() {
  mMode = Mode_LOWER_e;
  field_0x352 = 0;
}

/* 023669E0 */
void Act_c::mode_lower() {
  s16 timer = field_0x352;
  if (timer <= 0 && field_0x34e) {
    field_0x340 = f32(field_0x340) + f32(attr().field_0x030);
    field_0x352 = u8(attr().field_0x048);
  } else if (timer > 0)
    field_0x352 = s16(timer - 1);
  if (u8(field_0x350) > u8(attr().field_0x041)) {
    seStart(this, JA_SE_OBJ_JUMP_SPR_OPEN);
    mode_l_u_init();
  }
}

/* 023669C0 */
void Act_c::mode_l_u_init() {
  mMode = Mode_LOWER_UPPER_e;
  field_0x338 = f32(attr().field_0x03C);
}

/* 02366AF4 */
void Act_c::mode_l_u() {
  f32 scale = field_0x33C, target = field_0x338;
  if (!(scale < target)) {
    bool player = mIsPlayerRide != 0;
    field_0x33C = target;
    field_0x340 = 0.0f;
    if (player) {
      JumpPlayHD *play = jumpGetPlayHD();
      JumpPlayerHD *actor = play->mPlayer;
      actor->mNoResetFlg1 |= ForceVomitJumpHD;
    }
    mode_upper_init();
  }
}

/* 02366AD4 */
void Act_c::mode_upper_init() {
  mMode = Mode_UPPER_e;
  mTimer = u8(attr().field_0x042);
}

/* 02366B7C */
void Act_c::mode_upper() {
  s16 timer = s16(s16(mTimer) - 1);
  mTimer = timer;
  if (timer <= 0)
    mode_u_w_init();
}

/* 02366B64 */
void Act_c::mode_u_w_init() {
  mMode = Mode_UPPER_WAIT_e;
  field_0x338 = 1.0f;
}

/* 02366B94 */
void Act_c::mode_u_w() {
  if (!(f32(field_0x33C) > f32(field_0x338)))
    mode_wait_init();
}

/* 023666C8 */
s32 Act_c::Execute(gptr<Mtx34> *out) {
  set_push_flag();
  (this->*l_modeProc.mModes[s32(mMode)])();
  s32 mode = mMode;
  if ((mode == Mode_WAIT_e || mode == Mode_LOWER_e) && field_0x354)
    field_0x340 = f32(field_0x340) + f32(attr().field_0x034);
  calc_vib_pos();
  set_mtx();
  // HD: the matrix output slot contains a 32-bit guest pointer.
  *out = &M_tmp_mtx;
  clear_push_flag();
  return 1;
}

/* 023667C4 */
s32 Act_c::Draw() {
  void *env = jumpGetEnvLightHD();
  jumpSetTevHD(env, 0, &current.pos, &tevStr);
  env = jumpGetEnvLightHD();
  J3DModel *model = mModel.get();
  jumpLightTevHD(env, model, &tevStr);
  JumpPlayHD *play = jumpGetPlayHD();
  l_drawLists.mOpaque = play->mBackgroundOpaque;
  play = jumpGetPlayHD();
  l_drawLists.mTranslucent = play->mBackgroundTranslucent;
  model = mModel.get();
  jumpModelUpdateHD(model, 0);
  play = jumpGetPlayHD();
  l_drawLists.mOpaque = play->mOpaque;
  play = jumpGetPlayHD();
  l_drawLists.mTranslucent = play->mTranslucent;
  return 1;
}

/* 02366BA8 */
static s32 Mthd_Create(Act_c *actor) { return actor->Mthd_Create(); }

/* 02366BAC */
static s32 Mthd_Delete(Act_c *actor) { return actor->Mthd_Delete(); }

/* 02366BB0 */
static s32 Mthd_Execute(Act_c *actor) { return jumpMoveBgExecuteHD(actor); }

/* 02366BB4 */
static s32 Mthd_Draw(Act_c *actor) {
  const JumpActorVtableHD *vtable = JumpActorVtableHD::get(actor);
  return vtable->draw(actor);
}

/* 02366BC4 */
static s32 Mthd_IsDelete(Act_c *actor) {
  const JumpActorVtableHD *vtable = JumpActorVtableHD::get(actor);
  return vtable->isDelete(actor);
}
// HD: compiler-generated and template code of this translation unit.

/* 02366BD4 */
static void __sinit_d_a_obj_jump_cpp() {
  l_headerStatics.mZero[2] = 0;
  l_headerStatics.mZero[0] = 0;
  l_headerStatics.mZero[3] = 0;
  l_headerStatics.mZero[1] = 0;
  jumpRegisterHD(&l_registration.mEntries[0]);
  l_headerStatics.mFirst = l_headerConstants.mFirst;
  l_headerStatics.mSecond = l_headerConstants.mSecond;
  jumpTrigInitHD(&l_headerStatics.mTrigState);
  jumpRegisterHD(&l_registration.mEntries[1]);
  jumpRandomInitHD(&l_headerStatics.mRandomState);
  jumpRegisterHD(&l_registration.mEntries[2]);
}

/* 02366C68 */
static void jump_static_dtor(void *object, s32 flags) {
  if (object && (flags & 1))
    jumpDeleteHD(object);
}

/* 02366C7C */
static s32 IsDelete(Act_c *actor) { return 1; }

/* 02366C90 */
static void jump_dtor(Act_c *actor, s32 flags) {
  if (actor) {
    jumpActorDestroyHD(actor, 0);
    if (flags & 1)
      jumpDeleteHD(actor);
  }
}

/* 02366CE4 */
static u32 PrmAbstract(Act_c *actor, u32 width, u32 shift) {
  u32 parameters = u32(actor->mParameters);
  u32 mask = (width & 32) ? 0u : (1u << (width & 31));
  u32 value = (shift & 32) ? 0u : (parameters >> (shift & 31));
  return value & (mask - 1);
}

// HD: SafeString termination hook is empty.

/* 02366C84 */
static void SafeString_assureTerminationImpl_(const SafeString *key) {}
/* WWHD glue
ext 024F1D40 void jumpMoveBgConstructHD(Act_c* actor)
ext 02520460 s32 jumpResLoadHD(request_of_phase_process_class* phase, const char* name)
ext 02366CE4 s32 jumpGetTypeHD(Act_c* actor, s32 width, s32 shift)
ext 024F1D9C s32 jumpMoveBgCreateHD(Act_c* actor, const char* name, s16 dzb, void* callback, u32 heap)
ext 0273AA24 void jumpAssertHD(const char* file, s32 line, const char* expression)
ext 024F1F64 s32 jumpMoveBgDeleteHD(Act_c* actor)
ext 025204C8 void jumpResDeleteHD(request_of_phase_process_class* phase, const char* name)
ext 028E90D4 void jumpMtxCopyHD(const Mtx34* source, Mtx34* target)
ext 025F2518 void jumpScaleMHD(f32 x, f32 y, f32 z)
ext 026066C4 J3DModelData* jumpGetResourceHD(void* resources, SafeString* name, s16 index)
ext 025E38E0 J3DModel* jumpCreateModelHD(J3DModelData* data, u32 flags, u32 materialFlags)
ext 028E93CC void jumpTransSHD(Mtx34* matrix, f32 x, f32 y, f32 z)
ext 025F1B48 void jumpRotateZXYHD(Mtx34* matrix, s16 x, s16 y, s16 z)
ext 025F24E0 void jumpTransMHD(f32 x, f32 y, f32 z)
ext 025D674C void jumpCullBoxHD(Act_c* actor, f32 minX, f32 minY, f32 minZ, f32 maxX, f32 maxY, f32 maxZ)
ext 025200D4 JumpPlayHD* jumpGetPlayHD()
ext 02555D0C void* jumpGetEnvLightHD()
ext 025626A4 void jumpSetTevHD(void* env, s32 type, cXyz* position, dKy_tevstr_c* tev)
ext 02562F5C void jumpLightTevHD(void* env, J3DModel* model, dKy_tevstr_c* tev)
ext 025E2DE0 void jumpModelUpdateHD(J3DModel* model, s32 flags)
ext 02520540 s32 jumpReverbHD(s8 room)
ext 025E1A40 void jumpSoundHD(u32 sound, cXyz* position, s32 flags, s32 reverb)
ext 024F1E9C s32 jumpMoveBgExecuteHD(Act_c* actor)
ext 028F026C void jumpRegisterHD(JumpRegistrationHD* registration)
ext 028ED6F8 void jumpTrigInitHD(be<u8>* state)
ext 028EAB2C void jumpRandomInitHD(be<u8>* state)
ext 0273AF40 void jumpDeleteHD(void* object)
ext 025D50BC void jumpActorDestroyHD(Act_c* actor, s32 flags)
data l_attributes 1002BA48
data l_modeProc 1002BA08
data l_matrixStack 1048D0CC
data M_tmp_mtx 1046A338
data l_resources 101F4F28
data l_drawLists 104B4634
data l_headerStatics 1046A31C
data l_headerConstants 1002BA38
data l_registration 101CA5A4
str 1002BA40 "Hjump"
str 1002B980 "d_a_obj_jump.cpp" in 02365C98
str 1002B9E8 "d_a_obj_jump.cpp" in 02365F68
str 1002B994 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
str 1002B9D8 "model_data != 0"
*/
