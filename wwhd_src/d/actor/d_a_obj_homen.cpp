/** d_a_obj_homen.cpp - Wind Temple stone Hookshot target.
 * GameCube naming/layout, with audited HD call, float-payload and guest-temporary ordering. */
#include "d/actor/d_a_obj_homen.h"
using namespace daObjHomen;

static const HomenTUData homen_tu = {
    .mSafeStringVtable = 0x1002A9E8,
    .mArchiveName = 0x1002AC30,
    .mHeapAssertFile = 0x1002AABC,
    .mModelAssertText = 0x1002AAD0,
    .mCollisionAssertText = 0x1002AAE0,
    .mEnemyAssertFile = 0x1002AB48,
    .mEnemyAssertText = 0x1002AB5C,
    .mActorVtable = 0x1002AAA0,
    .mStatusBaseVtable = 0x1004AE88,
    .mStatusGameVtable = 0x1004AEC0,
    .mGroundBaseVtable = 0x1002AA60,
    .mGroundPolyVtable = 0x1002AA90,
    .mGroundVtable = 0x1002AA70,
    .mGroundGroupVtable = 0x1002AA80,
    .mEventName = 0x1002AB18,
    .mGroundDeletingVtable = 0x1002AA30,
    .mGroundDeletingPolyVtable = 0x1002AA50,
    .mGroundDeletingGroupVtable = 0x1002AA10,
    .mGeometryBaseVtable = 0x100015A8,
    .mCapsuleVtable = 0x1002AA00,
    .mCollisionObjectVtable = 0x1004AF18,
    .mCapsuleShapeVtable = 0x1004AF60,
    .mCapsuleGeometryVtable = 0x1004AF70,
    .mHeaderDtorLink0 = 0x101CA0B4,
    .mHeaderDtorLink1 = 0x101CA0C0,
    .mHeaderDtorLink2 = 0x101CA0CC,
    .mFindByIDCallback = 0x025E1234,
    .mCapsuleDestructor = 0x02515980,
    .mStatusDestructor = 0x02515860,
};

// Guest callees save LR at caller SP+4; temporaries must remain above linkage.
template <class T> struct Local : gabi::Local<T> {
  gabi::Local<be<u8>[16]> linkage;
};
static const u32 bdl_idx[2] = {4, 5};
static const u32 dzb_idx[2] = {8, 9};
static const f32 at_col_radius[2] = {135.0f, 45.0f};
static const f32 culling_dat[12] = {-300.0f, -30.0f, -100.0f, 300.0f, 600.0f, 300.0f, -100.0f, -30.0f, -100.0f, 100.0f, 250.0f, 120.0f};
static const u32 vibe_level_1st[2] = {2, 1};
static const u32 vibe_level_2nd[2] = {6, 4};
static const f32 speed_table[2] = {20.0f, 12.0f};
static const f32 co_radius[2] = {105.0f, 57.0f};
static const u32 offset_table[2] = {273063676, 273063700};
static const u32 M_sph_srcS[16] = {0, 0, 0, 0, 32768, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1108082688};
static const u32 M_sph_srcL[16] = {0, 0, 0, 0, 32768, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1112014848};
static const u32 M_cps_srcL[19] = {0, 2048, 16777216, 5, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1124532224};
static const u32 M_cps_srcS[19] = {0, 2048, 16777216, 5, 0, 0, 0, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1110704128};
static const u32 make_enemy_dat[22] = {12386304, 4294901760, 12386304, 4294901792, 12386304, 4294901824, 12386304, 4294901856, 12386304, 4294901888, 12386304, 4294901920, 13500416, 4278190080, 13500416, 4278190336, 13500416, 4278190592, 13500416, 4278190848, 13500416, 4278191104};
static u32 l_resourceManager;
static u32 l_save;
static u32 l_offset0Guard;
static u32 l_offset1Guard;
static u32 l_atOffsetGuard;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_headerGuard;
static u8 l_headerByte;
static f32 offset_data0[6] = {};
static f32 offset_data1[6] = {};
static f32 at_offset[24] = {};
static u32 l_zero[3] = {};
static u32 l_headerDefaults[4] = {};
static f32 l_calcMtx[12] = {};
static HomenInitEntry init_table[] = {{&Act_c::process_free_init}, {&Act_c::process_wait_falldown_init}, {&Act_c::process_falldown_init}, {&Act_c::process_wait_init}, {&Act_c::process_none_init}};
static HomenMainEntry main_table[] = {{&Act_c::process_free_main}, {&Act_c::process_wait_falldown_main}, {&Act_c::process_falldown_main}, {&Act_c::process_wait_main}, {&Act_c::process_none_main}};

/* 02358280 */
BOOL solidHeapCB(daObjHomen_c *a) {
  return a->create_heap();
}

/* 02358100 */
BOOL Act_c::create_heap() {
  Local<HomenSafeString> modelName, collisionName;
  modelName->mVtable = homen_tu.mSafeStringVtable;
  modelName->mText = homen_tu.mArchiveName;
  u32 type = (s32)this->mType;
  u32 data = Homen_getObjectRes(Homen_guest<void>(l_resourceManager),
                             modelName.get(), bdl_idx[(type*4) / 4]);
  if (!data)
    Homen_assert(Homen_guest<void>(homen_tu.mHeapAssertFile), 0x267,
                     Homen_guest<void>(homen_tu.mModelAssertText));
  else
    this->mpModel =
        Homen_guest<void>(Homen_modelCreate(Homen_guest<void>(data), 0, 0x11000002));
  this->init_mtx();
  u32 index = dzb_idx[((u32)(s32)this->mType*4) / 4];
  collisionName->mText = homen_tu.mArchiveName;
  collisionName->mVtable = homen_tu.mSafeStringVtable;
  u32 collision = Homen_getObjectRes(Homen_guest<void>(l_resourceManager),
                                  collisionName.get(), index);
  if (!collision)
    Homen_assert(Homen_guest<void>(homen_tu.mHeapAssertFile), 0x274,
                     Homen_guest<void>(homen_tu.mCollisionAssertText));
  else {
    u32 bg = Homen_bgNew((void *)nullptr);
    this->mpBgW = Homen_guest<void>(bg);
    if (bg && Homen_bgSet(Homen_guest<void>(bg), Homen_guest<void>(collision), 1,
                              this->mMtx))
      return 0;
  }
  return data && (void *)this->mpModel && (void *)this->mpBgW;
}

/* 0235833C */
BOOL Act_c::_create() {
  u32 status = this->actor_condition;
  if (!(status & 8)) {
    if (gabi::ea(this)) {
      Homen_actorCtor(this);
      this->__vtbl = homen_tu.mActorVtable;
      Homen_statusBaseCtor(&this->mSphStts);
      Homen_statusCtor(&this->mSphStts.mGameVtable);
      this->mSphStts.mBaseVtable = homen_tu.mStatusBaseVtable;
      this->mSphStts.mGameVtable = homen_tu.mStatusGameVtable;
      Homen_sphereCtor(&this->mSph);
      Homen_constructArray(this->mCpsStts, 2, 0x3c,
                       statusConstruct);
      Homen_constructArray(this->mCps, 2, 0x138,
                       capsuleConstruct);
      Homen_groundCtor(&this->mGndChk);
      this->mGndChk.mCamera = 0;
      this->mGndChk.mRope = 0;
      this->mGndChk.mPolyPass = gabi::ea(&this->mGndChk.mPolyVtable);
      this->mGndChk.mObject = 1;
      this->mGndChk.mBomb = 0;
      this->mGndChk.mGroupPass = gabi::ea(&this->mGndChk.mGroupVtable);
      this->mGndChk.mGroupMask = 1;
      this->mGndChk.mBaseVtable = homen_tu.mGroundBaseVtable;
      this->mGndChk.mPolyVtable = homen_tu.mGroundPolyVtable;
      this->mGndChk.mBoomerang = 0;
      this->mGndChk.mVtable = homen_tu.mGroundVtable;
      this->mGndChk.mLink = 0;
      this->mGndChk.mGroupVtable = homen_tu.mGroundGroupVtable;
      status = this->actor_condition;
      this->mGndChk.mArrow = 0;
    }
    this->actor_condition = status | 8;
  }
  s32 sw = PrmAbstract(this, PRM_SWITCH_W, PRM_SWITCH_S);
  if (sw != 255 &&
      Homen_isSwitch(Homen_guest<HomenSave>(l_save)->mZone, sw,
                      (s8)this->home.roomNo))
    return HOMEN_PHASE_ERROR;
  s32 phase =
      Homen_resLoad(&this->mPhase, Homen_guest<void>(homen_tu.mArchiveName));
  this->mType = PrmAbstract(this, PRM_ARG0_W, PRM_ARG0_S) & 1;
  if (phase != HOMEN_PHASE_COMPLETE)
    return phase;
  if (!Homen_entrySolidHeap(this, solidHeapCB, 0))
    return HOMEN_PHASE_ERROR;
  u32 model = gabi::ea((void *)this->mpModel);
  this->cullMtx = model ? model + 0xc8 : 0;
  u32 p = Homen_getPlay();
  Homen_bgRegist(Homen_guest<HomenPlay>(p)->mBgS, (void *)this->mpBgW, this);
  Homen_guest<HomenBgW>(gabi::ea((void*)this->mpBgW))->mCorrection = 0;
  const HomenCullBox* cull = reinterpret_cast<const HomenCullBox*>(&culling_dat[(u32)(s32)this->mType * 6]);
  Homen_setCullBox(this, (f32)cull->mMinX,
                   (f32)cull->mMinY, (f32)cull->mMinZ,
                   (f32)cull->mMaxX, (f32)cull->mMaxY,
                   (f32)cull->mMaxZ);
  Homen_statusInit(&this->mSphStts, 0, 255, this);
  u32 sph = (s32)this->mType == 0 ? gabi::ea(M_sph_srcL) : gabi::ea(M_sph_srcS);
  Homen_setSphere(&this->mSph, Homen_guest<void>(sph));
  this->mSph.mStatus = gabi::ea(&this->mSphStts);
  u32 zeroX = l_zero[0], flags = this->mSph.mTargetFlags;
  this->mSph.mTargetVector[0] = zeroX;
  this->mSph.mTargetVector[1] = l_zero[1];
  u32 zeroZ = l_zero[2];
  this->mSph.mTargetFlags = flags | 4;
  this->mSph.mTargetVector[2] = zeroZ;
  for (u32 i = 0; i < 2; i++) {
    u32 st = gabi::ea(&this->mCpsStts[i]),
        cap = gabi::ea(&this->mCps[i]),
        ends = gabi::ea(&this->mCpsS[i]);
    Homen_statusInit(Homen_guest<void>(st), 100, 255, this);
    u32 src = (s32)this->mType == 0 ? gabi::ea(M_cps_srcL) : gabi::ea(M_cps_srcS);
    Homen_setCapsule(Homen_guest<void>(cap), Homen_guest<void>(src));
    Homen_guest<HomenCapsule>(cap)->mStatus = st;
    for (u32 j = 0; j < 3; j++)
      Homen_guest<HomenCapsuleShape>(ends)->mStart[j] = Homen_component(this->current.pos, j);
    for (u32 j = 0; j < 3; j++)
      Homen_guest<HomenCapsuleShape>(ends)->mEnd[j] = Homen_component(this->current.pos, j);
    Homen_guest<HomenCapsuleShape>(ends)->mRadius =
        at_col_radius[(((u32)(s32)this->mType&1)*4) / 4];
  }
  this->process_init(HOMEN_FREE);
  if (PrmAbstract(this, PRM_ENEMY_W, PRM_ENEMY_S) != 15)
    this->group = 2;
  p = Homen_getPlay();
  this->mEventIdx = (s16)Homen_getEventIdx(Homen_guest<HomenPlay>(p)->mEventManager,
                                       Homen_guest<void>(homen_tu.mEventName), 255);
  this->mHookshotID = 0xffffffff;
  return phase;
}

/* 023586DC */
BOOL Act_c::_delete() {
  Homen_resDelete(&this->mPhase, Homen_guest<void>(homen_tu.mArchiveName));
  if (this->heap.v) {
    u32 bg = gabi::ea((void *)this->mpBgW);
    if (bg && Homen_guest<HomenBgW>(bg)->mUseFlags < 256) {
      u32 p = Homen_getPlay();
      Homen_bgRelease(Homen_guest<HomenPlay>(p)->mBgS, (void *)this->mpBgW);
    }
  }
  return 1;
}

/* 02357EC4 */
void Act_c::set_mtx() {
  if ((s32)this->mType == 0 && (s16)this->pivoted == 1) {
    Homen_transS(l_calcMtx, (f32)this->current.pos.x,
                     (f32)this->current.pos.y, (f32)this->current.pos.z);
    Homen_transM(0.0f, 0.0f, 200.0f);
    Homen_ZrotM(l_calcMtx, (s16)this->shape_angle.z);
    Homen_YrotM(l_calcMtx, (s16)this->shape_angle.y);
    Homen_XrotM(l_calcMtx, (s16)this->shape_angle.x);
    Homen_transM(0.0f, 0.0f, -200.0f);
  } else {
    Homen_transS(l_calcMtx, (f32)this->current.pos.x,
                     (f32)this->current.pos.y, (f32)this->current.pos.z);
    Homen_ZrotM(l_calcMtx, (s16)this->shape_angle.z);
    Homen_YrotM(l_calcMtx, (s16)this->shape_angle.y);
    Homen_XrotM(l_calcMtx, (s16)this->shape_angle.x);
  }
  // HD: load the whole model matrix before publishing any component.
  f32 values[12];
  for (int i = 0; i < 12; i++)
    values[i] = l_calcMtx[(i * 4) / 4];
  u32 model = gabi::ea((void *)this->mpModel);
  for (int i = 0; i < 12; i++)
    Homen_guest<HomenModel>(model)->mBaseMtx[i] = values[i];
  Homen_mtxCopy(l_calcMtx, this->mMtx);
}

/* 023580B0 */
void Act_c::init_mtx() {
  // HD: these three plain scale copies preserve the stored float payload bits.
  u32 y = Homen_bits(this->scale.y), x = Homen_bits(this->scale.x);
  u32 model = gabi::ea((void *)this->mpModel);
  u32 z = Homen_bits(this->scale.z);
  Homen_guest<HomenModel>(model)->mScaleBits[1] = y;
  Homen_guest<HomenModel>(model)->mScaleBits[0] = x;
  Homen_guest<HomenModel>(model)->mScaleBits[2] = z;
  this->set_mtx();
  Homen_modelCalc((void *)this->mpModel);
}

/* 023588F4 */
void Act_c::exe_event() {
  s16 state = this->eventState;
  if (state == 1) {
    if ((u16)this->eventInfo.mCommand == 2) {
      this->eventState = 2;
      return;
    }
    Homen_orderEvent(this, (s16)this->eventOrder, 255, 65535, 0, 1);
    this->eventInfo.mCondition = (u16)this->eventInfo.mCondition | 2;
  } else if (state == 2) {
    s16 event = this->eventOrder;
    u32 p = Homen_getPlay();
    if (Homen_eventEndCheck(Homen_guest<HomenPlay>(p)->mEventManager, event)) {
      p = Homen_getPlay();
      Homen_guest<HomenPlay>(p)->mEventFlags = (u16)Homen_guest<HomenPlay>(p)->mEventFlags | 8;
      this->eventOrder = -1;
      this->eventState = 0;
    }
  }
}

/* 02358AC8 */
void Act_c::make_enemy() {
  s32 enemy = PrmAbstract(this, PRM_ENEMY_W, PRM_ENEMY_S);
  if (enemy == 15 || (u32)enemy >= 11)
    return;
  if (enemy < 0)
    Homen_assert(Homen_guest<void>(homen_tu.mEnemyAssertFile), 0x3a9,
                     Homen_guest<void>(homen_tu.mEnemyAssertText));
  Local<be<f32>[6]> offsets;
  Local<cXyz> position;
  (*offsets)[(8) / 4] = 350;
  (*offsets)[(4) / 4] = 300;
  (*offsets)[(12) / 4] = 0;
  (*offsets)[(0) / 4] = 0;
  (*offsets)[(20) / 4] = 25;
  (*offsets)[(16) / 4] = 0;
  u32 index = (u32)(s32)this->mType & 1;
  Homen_multVec(this->mMtx,
                   &(*offsets)[index * 3],
                   position.get());
  u32 dat = gabi::ea(&make_enemy_dat[(u32)enemy * 2]);
  s16 proc = Homen_guest<HomenEnemyData>(dat)->mProcName;
  u32 params = Homen_guest<HomenEnemyData>(dat)->mParameters;
  u32 id = Homen_createActor(proc, params, position.get(),
                           (s8)this->current.roomNo, 0, 0, -1, 0);
  this->mEnemyID = id;
  if (id == 0xffffffff)
    this->group = 0;
}

/* 023589FC */
void Act_c::make_item() {
  Local<csXyz> angles;
  Local<cXyz> scale;
  Homen_anglesCtor(angles.get(), 0, 0, 0);
  scale->z = 1;
  scale->x = 1;
  scale->y = 1;
  s32 item = PrmAbstract(this, PRM_ITEM_W, PRM_ITEM_S);
  if (item != 63) {
    s32 bit = PrmAbstract(this, PRM_ITEMBIT_W, PRM_ITEMBIT_S);
    Homen_createItem(&this->current.pos, item, bit,
                     (s8)this->current.roomNo, 0, angles.get(), 1,
                     scale.get());
  }
}

/* 02358AA8 */
void Act_c::manage_item_timer() {
  s16 timer = this->mItemTimer;
  if (timer <= 0)
    return;
  timer = (s16)(timer - 1);
  this->mItemTimer = timer;
  if (!timer)
    this->make_item();
}

/* 02358BDC */
void Act_c::manage_enemy_timer() {
  s16 timer = this->mEnemyTimer;
  if (timer <= 0)
    return;
  timer = (s16)(timer - 1);
  this->mEnemyTimer = timer;
  if (!timer)
    this->make_enemy();
}

/* 0235874C */
void Act_c::get_norse_offset(cXyz *out, s32 which) {
  if (!l_offset0Guard) {
    l_offset0Guard = 1;
    offset_data0[0] = 0;
    offset_data0[1] = 453;
    offset_data0[2] = 140;
    offset_data0[3] = 0;
    offset_data0[4] = 181.5f;
    offset_data0[5] = 48;
  }
  if (!l_offset1Guard) {
    l_offset1Guard = 1;
    offset_data1[0] = 0;
    offset_data1[1] = 453;
    offset_data1[2] = 190;
    offset_data1[3] = 0;
    offset_data1[4] = 181.5f;
    offset_data1[5] = 68;
  }
  u32 data = offset_table[(((u32)which&1)*4) / 4] + (u32)(s32)this->mType * 12;
  Homen_component(*out, (0) / 4) = Homen_guest<HomenWordVector>(data)->x;
  Homen_component(*out, (4) / 4) = Homen_guest<HomenWordVector>(data)->y;
  Homen_component(*out, (8) / 4) = Homen_guest<HomenWordVector>(data)->z;
}

/* 02358DB8 */
void Act_c::get_norse_point(cXyz *out, s32 which) {
  Local<cXyz> offset, result;
  this->get_norse_offset(offset.get(), which);
  Homen_multVec(this->mMtx, offset.get(), result.get());
  Homen_component(*out, (0) / 4) = Homen_component(*result, (0) / 4);
  Homen_component(*out, (4) / 4) = Homen_component(*result, (4) / 4);
  Homen_component(*out, (8) / 4) = Homen_component(*result, (8) / 4);
}

/* 02359114 */
BOOL Act_c::process_free_init() {
  this->mEnemyID = 0xffffffff;
  return 1;
}

/* 02359124 */
void Act_c::process_free_main() {
  u32 p = Homen_getPlay(), player = Homen_guest<HomenPlay>(p)->mpPlayer;
  u32 boots = Homen_guest<HomenPlayer>(player)->mEquipmentFlags, flags = this->actor_status;
  if (boots & 0x02000000)
    flags = (flags | 0x80000) & ~0x200000u;
  else
    flags = (flags | 0x200000) & ~0x80000u;
  this->actor_status = flags;
  if (flags & 0x100000)
    this->process_init(HOMEN_WAIT_FALLDOWN);
  this->set_co_collision();
}

/* 023591A4 */
BOOL Act_c::process_wait_falldown_init() {
  u32 flags = this->actor_status;
  this->mEnemyTimer = 6;
  this->followCount = 0;
  this->actor_status = flags & ~0x100000u;
  this->mItemTimer = 20;
  u32 p = Homen_getPlay();
  Homen_bgRelease(Homen_guest<HomenPlay>(p)->mBgS, (void *)this->mpBgW);
  this->mpBgW = nullptr;
  p = Homen_getPlay();
  u32 type = (s32)this->mType;
  Local<cXyz> up;
  up->z = 0;
  up->x = 0;
  up->y = 1;
  Homen_startShock(Homen_guest<HomenPlay>(p)->mVibration,
                   vibe_level_1st[((type&1)*4) / 4], -17, up.get());
  if ((s32)this->mType != 0) {
    u32 z = Homen_bits(this->current.pos.z), y = Homen_bits(this->current.pos.y),
        x = Homen_bits(this->current.pos.x), id = this->mHookshotID;
    Homen_bits(this->target.x) = x;
    Homen_bits(this->target.y) = y;
    Homen_bits(this->target.z) = z;
    if (id != 0xffffffff) {
      Local<be<u32>> lookup;
      *lookup = id;
      u32 hook =
          Homen_searchActor(Homen_guest<void>(homen_tu.mFindByIDCallback), lookup.get());
      if (hook) {
        Local<cXyz> offset, result, difference;
        Local<be<f32>[12]> transform;
        this->get_norse_offset(offset.get(), 1);
        Homen_YrotS(l_calcMtx, (s16)this->shape_angle.y);
        Homen_mtxCopy(l_calcMtx, transform.get());
        Homen_multVec(transform.get(), offset.get(),
                         result.get());
        Homen_vectorSubtract(&Homen_guest<fopAc_ac_c>(hook)->current.pos, difference.get(),
                         result.get());
        x = Homen_component(*difference, (0) / 4);
        z = Homen_component(*difference, (8) / 4);
        Homen_bits(this->target.x) = x;
        y = Homen_component(*difference, (4) / 4);
        Homen_bits(this->target.z) = z;
        Homen_bits(this->target.y) = y;
      }
    }
    if ((s32)this->mType != 0)
      return 1;
  }
  Homen_onEventBit(Homen_guest<HomenSave>(l_save)->mEvent, 0x3410);
  return 1;
}

/* 02359328 */
void Act_c::process_wait_falldown_main() {
  if ((s16)this->followCount < 3) {
    Homen_addCalc2(&this->current.pos.x, (f32)this->target.x, 0.5f,
                     20.0f);
    Homen_addCalc2(&this->current.pos.y, (f32)this->target.y, 0.5f,
                     20.0f);
    Homen_addCalc2(&this->current.pos.z, (f32)this->target.z, 0.5f,
                     20.0f);
    this->followCount = (s16)((s16)this->followCount + 1);
  } else
    this->process_init(HOMEN_FALLDOWN);
  this->set_co_collision();
}

/* 02359400 */
BOOL Act_c::process_falldown_init() {
  // HD: copy the ground position component stores around SetActorPid exactly as verified.
  u32 id = this->mBsPcId;
  this->gravity = -2;
  this->maxFallSpeed = -36;
  f32 y = (f32)this->current.pos.y + 100.0f, x = this->current.pos.x,
      z = this->current.pos.z;
  this->mGndChk.mPosX = x;
  this->mGndChk.mPosY = y;
  this->mGndChk.mActorID = id;
  this->mGndChk.mPosZ = z;
  u32 p = Homen_getPlay();
  f32 ground =
      Homen_groundCross(Homen_guest<HomenPlay>(p)->mBgS, &this->mGndChk);
  s32 type = this->mType;
  this->mGroundY = ground;
  this->spinZ = type == 0 ? 90 : 120;
  f32 random = Homen_rndFX(3072.0f);
  this->spinTarget = (s16)gabi::ftoi(random);
  return 1;
}

/* 023594C8 */
void Act_c::process_falldown_main() {
  this->set_at_collision();
  Local<cXyz> point;
  this->get_norse_point(point.get(), 0);
  f32 ground;
  if ((s32)this->mType == 0 && (s16)this->pivoted == 0) {
    Local<cXyz> zero, result;
    zero->x = 0;
    zero->y = 0;
    zero->z = 0;
    Homen_multVec(this->mMtx, zero.get(), result.get());
    ground = this->mGroundY;
    if (!((f32)result->y - 1.0f > ground)) {
      this->speedF = 0;
      this->pivoted = 1;
      ground = this->mGroundY;
      this->gravity = 0;
      this->speed.x = 0;
      this->speed.z = 0;
      this->speed.y = 0;
    }
  } else {
    ground = this->mGroundY;
    if ((f32)this->current.pos.y < ground) {
      this->speed.y = 0;
      this->speedF = 0;
      this->gravity = 0;
      this->speed.z = 0;
      ground = this->mGroundY;
      this->speed.x = 0;
    }
  }
  s16 pivot = this->pivoted;
  if ((f32)point->y < ground) {
    u32 params = ((u32)(s32)pivot << 1) | (u32)(s32)this->mType;
    u32 smoke = Homen_createChild(HOMEN_SMOKE_PROCESS, this->mBsPcId, params,
                                &this->current.pos, (s8)this->current.roomNo,
                                &this->shape_angle, 0, -1, 0);
    this->mSmokeID = smoke;
    if (smoke != 0xffffffff) {
      u32 p = Homen_getPlay();
      Local<cXyz> up;
      up->x = 0;
      up->y = 1;
      up->z = 0;
      Homen_startShock(Homen_guest<HomenPlay>(p)->mVibration,
                       vibe_level_2nd[(((u32)(s32)this->mType&1)*4) / 4], -17,
                       up.get());
      this->speed.x = 0;
      this->speed.z = 0;
      this->spinX = 0;
      this->speedF = 0;
      this->spinZ = 0;
      this->speed.y = 0;
      this->gravity = 0;
      this->process_init(HOMEN_WAIT);
    }
  } else if (!pivot) {
    Homen_addCalcAngleS2(&this->shape_angle.z, (s16)this->spinTarget, 20,
                     4096);
    Homen_addCalcAngleS2(&this->shape_angle.y, (s16)((s16)this->yaw + 0x8000),
                     10, 4096);
  }
  if ((f32)this->gravity != 0.0f)
    this->speedF =
        speed_table[(((u32)(s32)this->mType&1)*4) / 4];
}

/* 023596F8 */
BOOL Act_c::process_wait_init() {
  s32 sw = PrmAbstract(this, PRM_SWITCH_W, PRM_SWITCH_S);
  Homen_onSwitch(Homen_guest<HomenSave>(l_save)->mZone, sw,
                   (s8)this->home.roomNo);
  s32 type = this->mType;
  this->extra = 1;
  Homen_seStartAt(type == 0 ? 0x69ff : 0x6a00, &this->current.pos);
  return 1;
}

/* 02359770 */
void Act_c::process_wait_main() {
  s16 timer = this->extra;
  if (timer <= 0)
    return;
  timer = (s16)(timer - 1);
  this->extra = timer;
  if (!timer)
    this->process_init(HOMEN_NONE);
}

/* 02359794 */
BOOL Act_c::process_none_init() {
  if ((s32)this->mType == 0)
    Homen_seStartGlobal(0x806);
  return 1;
}

/* 023597C8 */
void Act_c::process_none_main() {
  u32 id = this->mEnemyID;
  if (id == 0xffffffff)
    return;
  Local<be<u32>> found;
  if (!Homen_searchByID(id, found.get())) {
    this->mEnemyID = 0xffffffff;
    Homen_deleteActor(this);
  }
}

/* 02358284 */
BOOL Act_c::process_init(s16 state) {
  if ((u32)(s32)state >= 5) return 0;
  if (!Homen_dispatch<BOOL>(this, init_table[state].proc)) return 0;
  this->mState = state;
  return 1;
}

/* 02358BFC */
void Act_c::process_main() {
  s16 state = this->mState;
  if ((u32)(s32)state >= 5) return;
  Homen_dispatch<void>(this, main_table[state].proc);
}

/* 02358FEC */
void Act_c::set_co_collision() {
  u32 sph = gabi::ea(&this->mSph);
  if (Homen_checkTargetHit(Homen_guest<void>(sph))) {
    u32 hit = Homen_getTargetHit(Homen_guest<void>(sph)),
        p = Homen_getPlay(), player = Homen_guest<HomenPlay>(p)->mpPlayer;
    if (hit && (Homen_guest<HomenAttack>(hit)->mType & 0x8000)) {
      this->yaw = Homen_guest<HomenPlayer>(player)->mShapeYaw;
      u32 actor = Homen_getTargetHitActor(&this->mSph.mTargetFlags);
      u32 id = 0xffffffff;
      s32 type = this->mType;
      if (actor)
        id = Homen_guest<fopAc_ac_c>(actor)->mBsPcId;
      this->mHookshotID = id;
      if (type == 0)
        Homen_onEventBit(Homen_guest<HomenSave>(l_save)->mEvent,
                         0x3880);
    }
    Homen_clearTargetHit(&this->mSph);
    return;
  }
  Local<be<f32>[2]> radii;
  Local<cXyz> center;
  (*radii)[(0) / 4] = co_radius[0];
  (*radii)[(4) / 4] = co_radius[1];
  this->get_norse_point(center.get(), 0);
  Homen_sphereSetC(this->mSph.mGeometry, center.get());
  u32 index = (u32)(s32)this->mType & 1;
  Homen_sphereSetR(this->mSph.mGeometry,
                   (f32)(*radii)[(index*4) / 4]);
  u32 p = Homen_getPlay();
  Homen_collisionSet(Homen_guest<HomenPlay>(p)->mCcS, Homen_guest<void>(sph));
}

/* 02358E0C */
void Act_c::set_at_collision() {
  // HD: preserve the interleaved GHS guarded-table store order, including its early guard write.
  if (!l_atOffsetGuard) {
    at_offset[6] = 120;
    at_offset[8] = 140;
    at_offset[2] = 140;
    at_offset[5] = 140;
    at_offset[11] = 140;
    at_offset[4] = 550;
    at_offset[7] = 30;
    at_offset[9] = 120;
    at_offset[19] = 20;
    at_offset[13] = 20;
    at_offset[10] = 550;
    at_offset[12] = -45;
    at_offset[15] = -45;
    at_offset[14] = 80;
    at_offset[17] = 80;
    l_atOffsetGuard = 1;
    at_offset[16] = 190;
    at_offset[0] = -120;
    at_offset[1] = 30;
    at_offset[18] = 45;
    at_offset[3] = -120;
    at_offset[20] = 80;
    at_offset[21] = 45;
    at_offset[22] = 190;
    at_offset[23] = 80;
  }
  Local<cXyz> input, result;
  for (u32 i = 0; i < 2; i++) {
    u32 index = ((u32)(s32)this->mType & 1) * 2 + i,
        ends = gabi::ea(&this->mCpsS[i]),
        cap = gabi::ea(&this->mCps[i]);
    u32 data = gabi::ea(&at_offset[index * 6]);
    Homen_component(*input, (4) / 4) = Homen_guest<HomenWordVector>(data)->y;
    Homen_component(*input, (0) / 4) = Homen_guest<HomenWordVector>(data)->x;
    Homen_component(*input, (8) / 4) = Homen_guest<HomenWordVector>(data)->z;
    Homen_multVec(this->mMtx, input.get(), result.get());
    u32 x = Homen_component(*result, (0) / 4), y = Homen_component(*result, (4) / 4);
    Homen_guest<HomenCapsuleShape>(ends)->mStart[0] = x;
    u32 z = Homen_component(*result, (8) / 4);
    Homen_guest<HomenCapsuleShape>(ends)->mStart[1] = y;
    Homen_guest<HomenCapsuleShape>(ends)->mStart[2] = z;
    index = ((u32)(s32)this->mType & 1) * 2 + i;
    data = gabi::ea(&at_offset[3 + index * 6]);
    Homen_component(*input, (4) / 4) = Homen_guest<HomenWordVector>(data)->y;
    Homen_component(*input, (0) / 4) = Homen_guest<HomenWordVector>(data)->x;
    Homen_component(*input, (8) / 4) = Homen_guest<HomenWordVector>(data)->z;
    Homen_multVec(this->mMtx, input.get(), result.get());
    y = Homen_component(*result, (4) / 4);
    x = Homen_component(*result, (0) / 4);
    z = Homen_component(*result, (8) / 4);
    Homen_guest<HomenCapsuleShape>(ends)->mEnd[0] = x;
    Homen_guest<HomenCapsuleShape>(ends)->mEnd[2] = z;
    Homen_guest<HomenCapsuleShape>(ends)->mEnd[1] = y;
    Homen_capsuleSet(Homen_guest<HomenCapsule>(cap)->mGeometry, Homen_guest<void>(ends));
    u32 p = Homen_getPlay();
    Homen_collisionSet(Homen_guest<HomenPlay>(p)->mCcS, Homen_guest<void>(cap));
  }
}

/* 02358820 */
void Act_c::adjust_hookshot() {
  u32 p = Homen_getPlay(), player = Homen_guest<HomenPlay>(p)->mpPlayer;
  if (Homen_guest<HomenPlayer>(player)->mEquipmentFlags & 0x02000000)
    return;
  u32 id = this->mHookshotID;
  if (id == 0xffffffff)
    return;
  Local<be<u32>> lookup;
  *lookup = id;
  if (!Homen_searchActor(Homen_guest<void>(homen_tu.mFindByIDCallback), lookup.get()))
    return;
  Local<cXyz> offset, result;
  Local<be<f32>[12]> transform;
  this->get_norse_offset(offset.get(), 1);
  Homen_YrotS(l_calcMtx, (s16)this->shape_angle.y);
  Homen_mtxCopy(l_calcMtx, transform.get());
  Homen_multVec(transform.get(), offset.get(), result.get());
  u32 target = Homen_guest<HomenPlayerVtable>(Homen_guest<HomenPlayer>(player)->mVtable)->mSetHookshotCarryOffset;
  gabi::call<void>(target, Homen_guest<void>(player), u32(this->mBsPcId),
                   result.get());
  this->mHookshotID = 0xffffffff;
}

/* 02358C54 */
BOOL Act_c::_execute() {
  this->adjust_hookshot();
  this->exe_event();
  this->manage_item_timer();
  this->manage_enemy_timer();
  this->process_main();
  u32 bg = gabi::ea((void *)this->mpBgW);
  if (bg)
    Homen_bgMove(Homen_guest<void>(bg));
  s16 increment = this->spinZ, spin = this->spinX;
  spin = (s16)(spin + increment);
  if (spin > 2000)
    spin = 2000;
  s16 angle = (s16)((s16)this->shape_angle.x + spin);
  this->spinX = spin;
  if (angle >= 0x5000) {
    this->shape_angle.x = 0x5000;
    this->spinX = 0;
    this->spinZ = 0;
  } else
    this->shape_angle.x = angle;
  Homen_calcSpeed(this);
  Homen_posMove(this, 0);
  this->set_mtx();
  return 1;
}

/* 02358D48 */
BOOL Act_c::_draw() {
  s16 state = this->mState;
  if (state != 4 && state != 3) {
    u32 env = Homen_getEnvLight();
    Homen_settingTevStruct(Homen_guest<void>(env), 1, &this->current.pos,
                     &this->tevStr);
    env = Homen_getEnvLight();
    Homen_setLightTevColorType(Homen_guest<void>(env), (void *)this->mpModel,
                     &this->tevStr);
    Homen_modelUpdateDL((void *)this->mpModel, 0);
  }
  return 1;
}

/* 0235981C */
BOOL Mthd_Create(daObjHomen_c *a) {
  return a->_create();
}

/* 02359820 */
BOOL Mthd_Delete(daObjHomen_c *a) {
  return a->_delete();
}

/* 02359824 */
BOOL Mthd_Execute(daObjHomen_c *a) {
  return a->_execute();
}

/* 02359828 */
BOOL Mthd_Draw(daObjHomen_c *a) {
  return a->_draw();
}

/* 02359AA8 */
BOOL Mthd_IsDelete(daObjHomen_c *a) {
  return 1;
}

/* 02359AB0 */
u32 PrmAbstract(const daObjHomen_c *a, u32 width, u32 shift) {
  u32 value = a->mParameters, n = width & 63, s = shift & 63;
  u32 mask = (n < 32 ? (1u << n) : 0u) - 1u;
  u32 shifted = s < 32 ? value >> s : 0u;
  return shifted & mask;
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0235982C */
void __sinit_d_a_obj_homen_cpp() {
  l_headerDefaults[2] = 0;
  l_headerDefaults[0] = 0;
  l_headerDefaults[3] = 0;
  l_headerDefaults[1] = 0;
  Homen_registerDestructor(Homen_guest<void>(homen_tu.mHeaderDtorLink0));
  l_minAngle = -3.1415927410125732f;
  l_maxAngle = 3.1415927410125732f;
  Homen_headerGuardCtor(Homen_guest<void>(0x10469EF8));
  Homen_registerDestructor(Homen_guest<void>(homen_tu.mHeaderDtorLink1));
  Homen_headerByteCtor(Homen_guest<void>(0x10469EF9));
  Homen_registerDestructor(Homen_guest<void>(homen_tu.mHeaderDtorLink2));
}

/* 023598C0 */
void SafeString_dtor(void *self, s32 flags) {
  if (self && (flags & 1))
    Homen_delete(self);
}

/* 023598D4 */
void* statusConstruct(void *self) {
  u32 p = gabi::ea(self);
  if (!p)
    p = Homen_new(0x3c);
  if (p) {
    Homen_statusBaseCtor(Homen_guest<void>(p));
    Homen_statusCtor(&Homen_guest<HomenStatus>(p)->mGameVtable);
    Homen_guest<HomenStatus>(p)->mBaseVtable = homen_tu.mStatusBaseVtable;
    Homen_guest<HomenStatus>(p)->mGameVtable = homen_tu.mStatusGameVtable;
  }
  return Homen_guest<void>(p);
}

/* 0235993C */
void* capsuleConstruct(void *self) {
  u32 p = gabi::ea(self);
  if (!p)
    p = Homen_new(0x138);
  if (p) {
    Homen_collisionObjectCtor(Homen_guest<void>(p));
    Homen_guest<HomenCapsule>(p)->mGeometryVtable = homen_tu.mGeometryBaseVtable;
    Homen_guest<HomenCapsule>(p)->mCapsuleVtable = homen_tu.mCapsuleVtable;
    Homen_capsuleGeometryCtor(Homen_guest<HomenCapsule>(p)->mGeometry);
    Homen_guest<HomenCapsule>(p)->mObjectVtable = homen_tu.mCollisionObjectVtable;
    Homen_guest<HomenCapsule>(p)->mShapeVtable = homen_tu.mCapsuleShapeVtable;
    Homen_guest<HomenCapsule>(p)->mGeometryVtable = homen_tu.mCapsuleGeometryVtable;
  }
  return Homen_guest<void>(p);
}

/* 023599C8 */
void SafeString_assureTerminationImpl(void *self) { }

/* 023599CC */
void actorDestructor(daObjHomen_c *a, s32 flags) {
  if (!a)
    return;
  a->mGndChk.mVtable = homen_tu.mGroundDeletingVtable;
  a->mGndChk.mPolyVtable = homen_tu.mGroundDeletingPolyVtable;
  a->mGndChk.mGroupVtable = homen_tu.mGroundDeletingGroupVtable;
  Homen_groundDtor(&a->mGndChk, 0);
  Homen_destroyArray(a->mCps, 2, 0x138,
                   Homen_guest<void>(homen_tu.mCapsuleDestructor), 0, 0);
  Homen_destroyArray(a->mCpsStts, 2, 0x3c,
                   Homen_guest<void>(homen_tu.mStatusDestructor), 0, 0);
  Homen_sphereDtor(&a->mSph, 2);
  Homen_statusDtor(&a->mSphStts, 2);
  Homen_actorDtor(a, 0);
  if (flags & 1)
    Homen_delete(a);
}

/* WWHD glue (tools/verify/autoglue.py): private HD ABI bindings
data bdl_idx 101CA054
data dzb_idx 101CA05C
data at_col_radius 101CA064
data culling_dat 101CA06C
data vibe_level_1st 101CA09C
data vibe_level_2nd 101CA0A4
data speed_table 101CA0AC
data co_radius 1002AC0C
data offset_table 101CA04C
data M_sph_srcS 1002AC38
data M_sph_srcL 1002AC78
data M_cps_srcL 1002ACB8
data M_cps_srcS 1002AD04
data make_enemy_dat 1002AB6C
data l_resourceManager 101F4F28
data l_save 101F84DC
data l_offset0Guard 10469F9C
data l_offset1Guard 10469FA0
data l_atOffsetGuard 10469FA4
data l_minAngle 10469EF0
data l_maxAngle 10469EF4
data l_headerGuard 10469EF8
data l_headerByte 10469EF9
data offset_data0 10469EFC nocheck
data offset_data1 10469F14 nocheck
data at_offset 10469F3C nocheck
data l_zero 101FFBA8 nocheck
data l_headerDefaults 10469F2C nocheck
data l_calcMtx 1048D0CC nocheck
data init_table 1002AAF0
data main_table 1002ABC4
ext 020087EC void Homen_bgRelease(void*, void*)
ext 02008974 f32 Homen_groundCross(void*, void*)
ext 02008DAC void Homen_groundDtor(void*, s32)
ext 02008E0C void Homen_groundCtor(void*)
ext 0200A030 s32 Homen_bgSet(void*, void*, s32, void*)
ext 0200BD2C void Homen_statusBaseCtor(void*)
ext 0200E240 void Homen_collisionSet(void*, void*)
ext 0200ED84 void Homen_addCalc2(be<f32>*, f32, f32, f32)
ext 0200F428 void Homen_addCalcAngleS2(be<s16>*, s16, s32, s32)
ext 02018150 void Homen_capsuleGeometryCtor(void*)
ext 020181FC void Homen_capsuleSet(void*, void*)
ext 02018C8C void Homen_sphereSetR(void*, f32)
ext 02018D40 void Homen_sphereSetC(void*, const cXyz*)
ext 02019918 f32 Homen_rndFX(f32)
ext 0201A478 void Homen_anglesCtor(csXyz*, s32, s32, s32)
ext 0201ADE0 void Homen_vectorSubtract(cXyz*, cXyz*, cXyz*)
ext 024EEA6C void Homen_bgRegist(void*, void*, daObjHomen_c*)
ext 024F23F4 u32 Homen_bgNew(void*)
ext 024F43DC void Homen_bgMove(void*)
ext 02515860 void Homen_statusDtor(void*, s32)
ext 02515AE8 void Homen_sphereDtor(void*, s32)
ext 02515BBC u32 Homen_getTargetHitActor(void*)
ext 02515DA0 void Homen_statusCtor(void*)
ext 02515F14 void Homen_statusInit(void*, s32, s32, daObjHomen_c*)
ext 02515FB8 void Homen_collisionObjectCtor(void*)
ext 0251621C void Homen_clearTargetHit(void*)
ext 025162A4 s32 Homen_checkTargetHit(void*)
ext 02516300 u32 Homen_getTargetHit(void*)
ext 025164C0 void Homen_setCapsule(void*, void*)
ext 025166F0 void Homen_sphereCtor(void*)
ext 0251677C void Homen_setSphere(void*, void*)
ext 025200D4 u32 Homen_getPlay()
ext 02520460 s32 Homen_resLoad(void*, void*)
ext 025204C8 void Homen_resDelete(void*, void*)
ext 02543F10 s32 Homen_getEventIdx(void*, void*, s32)
ext 025440C8 s32 Homen_eventEndCheck(void*, s16)
ext 02555D0C u32 Homen_getEnvLight()
ext 025626A4 void Homen_settingTevStruct(void*, s32, const cXyz*, void*)
ext 02562F5C void Homen_setLightTevColorType(void*, void*, void*)
ext 025B8B68 void Homen_onEventBit(void*, s32)
ext 025B9E38 void Homen_onSwitch(void*, s32, s32)
ext 025BA0C0 s32 Homen_isSwitch(void*, s32, s32)
ext 025CB374 void Homen_startShock(void*, u32, s32, const cXyz*)
ext 025D4ED0 void Homen_actorCtor(daObjHomen_c*)
ext 025D50BC void Homen_actorDtor(daObjHomen_c*, s32)
ext 025D5218 u32 Homen_searchActor(void*, void*)
ext 025D54C4 s32 Homen_searchByID(u32, void*)
ext 025D57E0 void Homen_deleteActor(daObjHomen_c*)
ext 025D5834 u32 Homen_createActor(s16, u32, const cXyz*, s32, void*, void*, s32, void*)
ext 025D5A20 u32 Homen_createChild(s32, u32, u32, const cXyz*, s32, const csXyz*, void*, s32, void*)
ext 025D63E8 s32 Homen_entrySolidHeap(daObjHomen_c*, HomenHeapCallback, s32)
ext 025D674C void Homen_setCullBox(daObjHomen_c*, f32, f32, f32, f32, f32, f32)
ext 025D67A8 void Homen_calcSpeed(daObjHomen_c*)
ext 025D6800 void Homen_posMove(daObjHomen_c*, void*)
ext 025D7A58 void Homen_orderEvent(daObjHomen_c*, s16, s32, s32, s32, s32)
ext 025D8120 void Homen_createItem(const cXyz*, s32, s32, s32, s32, const csXyz*, s32, const cXyz*)
ext 025E1988 void Homen_seStartGlobal(s32)
ext 025E19CC void Homen_seStartAt(s32, const cXyz*)
ext 025E2DE0 void Homen_modelUpdateDL(void*, s32)
ext 025E38E0 u32 Homen_modelCreate(void*, s32, u32)
ext 025F1884 void Homen_YrotS(void*, s16)
ext 025F1BF4 void Homen_XrotM(void*, s16)
ext 025F1C28 void Homen_YrotM(void*, s16)
ext 025F1C5C void Homen_ZrotM(void*, s16)
ext 025F24E0 void Homen_transM(f32, f32, f32)
ext 026066C4 u32 Homen_getObjectRes(void*, void*, u32)
ext 0273AA24 void Homen_assert(void*, s32, void*)
ext 0273AD10 u32 Homen_new(s32)
ext 0273AF40 void Homen_delete(void*)
ext 027F4D5C void Homen_modelCalc(void*)
ext 028E8F64 void Homen_multVec(void*, void*, void*)
ext 028E90D4 void Homen_mtxCopy(void*, void*)
ext 028E93CC void Homen_transS(void*, f32, f32, f32)
ext 028EAB2C void Homen_headerByteCtor(void*)
ext 028ED6F8 void Homen_headerGuardCtor(void*)
ext 028EFFD0 void Homen_constructArray(void*, s32, s32, HomenArrayConstructor)
ext 028F0164 void Homen_destroyArray(void*, s32, s32, void*, s32, s32)
ext 028F026C void Homen_registerDestructor(void*)
*/
