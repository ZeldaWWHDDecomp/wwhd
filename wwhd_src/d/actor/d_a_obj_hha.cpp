/** Tower of the Gods entrance waterfall. HD behavior preserved by unit verification.
 * GC part, splash, gush and actor methods use asserted HD layouts. */
#include "d/actor/d_a_obj_hha.h"
#include "gabi.h"
using Actor = daObjHha_c;
enum HhaResource {
    dRes_INDEX_HHA_BCK_YGSTP00_e = 6,
    dRes_INDEX_HHA_BDL_HHA1_e = 9, dRes_INDEX_HHA_BDL_HHA2_e = 10,
    dRes_INDEX_HHA_BDL_YGSTP00_e = 11, dRes_INDEX_HHA_BDL_YSWTR00_e = 12,
    dRes_INDEX_HHA_BTK_YGSTP00_e = 15,
    dRes_INDEX_HHA_BTK_YSWTR00_01_e = 16, dRes_INDEX_HHA_BTK_YSWTR00_02_e = 17,
    dRes_INDEX_HHA_DZB_HHA1_e = 20, dRes_INDEX_HHA_DZB_HHA2_e = 21,
};
static const HhaTU gc_tu = {
    0x1002A348, // mSafeStringVtable
    0x1002A428, // mPartAssertFile
    0x1002A438, // mPartModelAssert
    0x1002A44C, // mGushAssertFile
    0x1002A45C, // mGushModelDataAssert
    0x1002A48C, // mGushModelAssert
    0x1002A46C, // mBtkAssert
    0x1002A47C, // mBckAssert
    0x1002A370, // mBckVtable
    0x1016E54C, // mFrameCtrlVtable
    0x1016D820, // mControllerVtable
    0x1016E414, // mDefaultLight
    0x1002A578, // mArchiveName
    0x1002A4A4, // mActorAssertFile
    0x1002A4B4, // mActorModelAssert
    0x1002A4C4, // mActorBtkAssert
    0x1002A3A8, // mWaterVtable
    0x1002A3C8, // mWaterPolyVtable
    0x1002A398, // mWaterGroupVtable
    0x1002A5BC, // mCylinderData
    0x1002A57C, // mSphereData
    0x1002A408, // mActorVtable
    0x1004AEC0, // mStatusGameVtable
    0x1004AE88, // mStatusBaseVtable
    0x100015A8, // mShapeBaseVtable
    0x1002A360, // mBoundsVtable
    0x1004B108, // mCylinderVtable
    0x1004B150, // mCylinderGeometryVtable
    0x1004B160, // mCylinderShapeVtable
    0x1002A540, // mEventName
    0x101C9EE4, // mHeaderLink0
    0x101C9EF0, // mHeaderLink1
    0x101C9EFC, // mHeaderLink2
    0x025E7C6C, // mBtkConstructor
    0x02355F70, // mSplashConstructor
    0x02354B3C, // mHeapCallback
    0x02355FC0, // mSplashDestructor
    0x02353D00, // mPartNormal
    0x02353D9C, // mPartMove
    0x02353EC0, // mPartDraw
};

// HD: checked table initializers remain numeric for autoglue; HhaResource names define their indices.
static u32 l_daObjHha_bdl_idx_table[2] = {9, 10};
static u32 l_daObjHha_dzb_idx_table[2] = {20, 21};
static u32 l_daObjHha_btk_idx_table[2] = {16, 17};
static u32 l_daObjHha_btk_mode_table[2] = {2, 0};
static u16 l_daObjHha_splash_id_table[2] = {33037, 33038};
static f32 splash_y[2] = {-1400.0f, -1450.0f};
static f32 splash_z[2] = {0.0f, 100.0f};
static u16 move_frame[2] = {55, 30};
static f32 pos_y[4] = {150.0f, -150.0f, 0.0f, 0.0f};
static f32 tar_y[4] = {0.0f, 0.0f, 150.0f, -150.0f};
static f32 l_headerFloats[2] = {-3.1415927410125732f, 3.1415927410125732f};
static u32 l_resourceControl = 0;
static u32 l_saveInfo = 0;
static f32 l_waterLevel = 0;
static cXyz l_baseZ = {0.0f, 0.0f, 1.0f};
static f32 l_matrix[12] = {};
static u32 l_headerWords[4] = {};
static f32 l_headerValues[2] = {};
static u8 l_headerGuard = 0;
static u8 l_headerByte = 0;
// HD: preserve the original word copies and float-copy load/store order.
static void copyWords(cXyz* src, cXyz* dst) {
  for (u32 i = 0; i < 3; ++i) Hha_word(*dst, i) = (u32)Hha_word(*src, i);
}
static void copyVector(cXyz* src, cXyz* dst) {
  f32 x = src->x, y = src->y, z = src->z;
  dst->x = x; dst->y = y; dst->z = z;
}
static void copyAngle(csXyz* src, csXyz* dst) {
  dst->x = (s16)src->x; dst->y = (s16)src->y; dst->z = (s16)src->z;
}
static void copyMatrix(be<f32>* src, be<f32>* dst) {
  f32 m[12];
  for (u32 i = 0; i < 12; ++i) m[i] = src[i];
  for (u32 i = 0; i < 12; ++i) dst[i] = m[i];
}
static u32 play() { return Hha_getPlay(); }
static u32 resource(const char* name, s32 index) {
  gabi::Local<HhaSafeString> s;
  s->mText = gabi::guest_addr(name);
  s->mVtable = gc_tu.mSafeStringVtable;
  u32 ctl = l_resourceControl;
  return Hha_getObjectRes(Hha_guest<void>(ctl), s.get(), index);
}
static void assertion(u32 line, u32 condition) {
  Hha_assert(Hha_guest<void>(gc_tu.mPartAssertFile), line, Hha_guest<void>(condition));
}
static BOOL switch_on(u32 self) {
  u32 save = l_saveInfo;
  return Hha_isSwitch(&Hha_guest<HhaSave>(save)->mZone, Hha_guest<Actor>(self)->mSwitchNo, (s8)Hha_guest<Actor>(self)->home.roomNo);
}

/* 02353A00 */
void daObjHhaPart_c::init_data(f32 initial, f32 target, u16 frames, u8 index,
               u8 middle) {
  mPos.x = 0.f;
  mPos.y = initial;
  mPos.z = 0.f;
  mPosTarget.x = 0.f;
  mPosTarget.y = target;
  mPartIdx = index;
  mPosTarget.z = 0.f;
  mDeltaY = (target - initial) / (f32)frames;
  gabi::Local<cXyz> diff;
  Hha_subtractVector(&mPosTarget.x, diff.get(), &mPos);
  copyWords(diff.get(), reinterpret_cast<cXyz*>(&mPosDeltaDir.x));
  Hha_normalizeRS(&mPosDeltaDir.x);
  mbMid = middle;
  mCbExec.d = 0;
  mCbExec.i = -1;
  mCbDraw.d = 0;
  mCbDraw.i = -1;
  mCbExec.f = 0x02353D00;
  mCbDraw.f = 0x02353EC0;
}

/* 02353AE8 */
BOOL daObjHhaPart_c::set_mdl_area(const char *name, s32 index) {
  u32 data = resource(name, index);
  if (!data) {
    assertion(0x1D9, gc_tu.mPartModelAssert);
    return 0;
  }
  u32 model = Hha_modelCreate(Hha_guest<void>(data), 0, 0x11020203u);
  mpModel = model;
  return 1;
}

/* 02353B7C */
BOOL daObjHhaPart_c::set_bgw(const char *name, s32 index) {
  u32 data = resource(name, index), model = mpModel;
  u32 bg = Hha_bgNewSet(Hha_guest<void>(data), 1, Hha_guest<void>(model ? gabi::ea(&Hha_guest<HhaModel>(model)->mBaseTransformMtx) : 0));
  mpBgw = bg;
  return bg != 0;
}

/* 02353BFC */
void daObjHhaPart_c::init_mtx(cXyz *position, csXyz *angle, cXyz *scale) {
  u32 pos = gabi::ea(position), ang = gabi::ea(angle),
      s = gabi::ea(scale), model = mpModel;
  copyVector(Hha_guest<cXyz>(s), reinterpret_cast<cXyz*>(&Hha_guest<HhaModel>(model)->mBaseScale));
  Hha_transS(l_matrix, Hha_guest<cXyz>(pos)->x, Hha_guest<cXyz>(pos)->y, Hha_guest<cXyz>(pos)->z);
  Hha_ZXYrotM(l_matrix, Hha_guest<csXyz>(ang)->x, Hha_guest<csXyz>(ang)->y, Hha_guest<csXyz>(ang)->z);
  Hha_transM(mPos.x, mPos.y, mPos.z);
  model = mpModel;
  copyMatrix(l_matrix, Hha_guest<be<f32>>(gabi::ea(&Hha_guest<HhaModel>(model)->mBaseTransformMtx)));
  Hha_modelCalc(Hha_guest<void>(mpModel));
}
static void part_parent_matrix(u32 part, u32 parent) {
  gabi::Local<cXyz> pos, scale;
  gabi::Local<csXyz> angle;
  copyVector(&Hha_guest<Actor>(parent)->current.pos, pos.get());
  copyVector(&Hha_guest<Actor>(parent)->scale, scale.get());
  copyAngle(&Hha_guest<Actor>(parent)->shape_angle, angle.get());
  Hha_guest<daObjHhaPart_c>(part)->init_mtx(pos.get(), angle.get(), scale.get());
}

/* 02353D00 */
void daObjHhaPart_c::exe_normal(Actor *parent) {
  u32 p = gabi::ea(this);
  part_parent_matrix(p, gabi::ea(parent));
  u32 bg = mpBgw;
  if (bg && Hha_guest<be<u32>>(bg)[0] < 0x100)
    Hha_bgMove(Hha_guest<void>(bg));
}

/* 02353D9C */
void daObjHhaPart_c::exe_move(Actor *parent) {
  mPos.y = mPos.y + mDeltaY;
  gabi::Local<cXyz> raw, diff;
  Hha_subtractVector(&mPosTarget.x, raw.get(), &mPos);
  copyWords(raw.get(), diff.get());
  f32 dot = Hha_dotProduct(&mPosDeltaDir.x, diff.get());
  // HD: unordered values take the completion branch.
  if (!(dot > 0.f)) {
    u32 y = Hha_bits(mPosTarget.y), z = Hha_bits(mPosTarget.z);
    u8 middle = mbMid;
    mCbExec.d = 0;
    mCbExec.f = 0x02353D00;
    mCbExec.i = -1;
    Hha_bits(mPos.z) = z;
    u32 x = Hha_bits(mPosTarget.x);
    Hha_bits(mPos.y) = y;
    Hha_bits(mPos.x) = x;
    if (!middle && !mPartIdx) {
      u32 pl = play();
      gabi::Local<cXyz> direction;
      direction->set(0.f, 1.f, 0.f);
      Hha_startShock(reinterpret_cast<void*>(&Hha_guest<HhaPlay>(pl)->mVibration), 4, -0x21, direction.get());
    }
  }
  exe_normal(parent);
}

/* 02353EC0 */
void daObjHhaPart_c::draw_normal(Actor *parent) {
  u32 env = Hha_getEnvLight();
  Hha_setLightTevColorType(Hha_guest<void>(env), Hha_guest<void>(mpModel), &parent->tevStr);
  Hha_modelUpdateDL(Hha_guest<void>(mpModel), 0);
}

/* 02353F10 */
void daObjHhaSplash_c::create_s(u16 id, cXyz *position, f32 yOffset,
                   f32 zOffset, csXyz *angle) {
  u32 p = gabi::ea(this), pos = gabi::ea(position), ang = gabi::ea(angle);
  f32 x = Hha_guest<cXyz>(pos)->x, y = Hha_guest<cXyz>(pos)->y, z = Hha_guest<cXyz>(pos)->z;
  mPos.x = x;
  mPos.z = z;
  mPos.y = y + yOffset;
  Hha_YrotS(l_matrix, Hha_guest<csXyz>(ang)->y);
  gabi::Local<cXyz> vec;
  Hha_multVec(l_matrix, &l_baseZ, vec.get());
  Hha_scaleVec(vec.get(), vec.get(), zOffset);
  Hha_addVector(&mPos, vec.get(), &mPos);
  u32 vx = Hha_bits(mPos.x), vz = Hha_bits(mPos.z);
  Hha_bits(mBasePos.x) = vx;
  u32 vy = Hha_bits(mPos.y);
  Hha_bits(mBasePos.z) = vz;
  Hha_bits(mBasePos.y) = vy;
  copyAngle(Hha_guest<csXyz>(ang), reinterpret_cast<csXyz*>(&mAngle.x));
  u32 pl = play(), ctl = Hha_guest<HhaPlay>(pl)->mpParticle;
  Hha_setParticle(Hha_guest<void>(ctl), 0, id, &mPos, &mAngle.x, 0, Hha_guest<void>(0xFF), p, -1, Hha_guest<void>(0), Hha_guest<void>(0), Hha_guest<void>(0));
  mbIsActive = 1;
}
static void gush_assert(u32 line, u32 condition) {
  Hha_assert(Hha_guest<void>(gc_tu.mGushAssertFile), line, Hha_guest<void>(condition));
}

/* 02354050 */
BOOL daObjHhaYgush_c::create_area(const char *name) {
  const char* n = name;
  u32 data = resource(n, dRes_INDEX_HHA_BDL_YGSTP00_e);
  if (!data) {
    gush_assert(0x280, gc_tu.mGushModelDataAssert);
    return 0;
  }
  u32 model = Hha_modelCreate(Hha_guest<void>(data), 0x80000u, 0x11000222u);
  M_mdl = model;
  if (!model) {
    gush_assert(0x289, gc_tu.mGushModelAssert);
    if (!M_mdl)
      return 0;
  }
  u32 btk = resource(n, dRes_INDEX_HHA_BTK_YGSTP00_e);
  if (!btk)
    gush_assert(0x290, gc_tu.mBtkAssert);
  if (!Hha_btkInit(&mBtk, Hha_guest<void>(Hha_guest<HhaModel>(M_mdl)->mModelData), Hha_guest<void>(btk), 1, 2, 0, -1, 0, 1.f, 0))
    return 0;
  u32 bck = resource(n, dRes_INDEX_HHA_BCK_YGSTP00_e);
  if (!bck)
    gush_assert(0x295, gc_tu.mBckAssert);
  return Hha_bckInit(&mBck, Hha_guest<void>(Hha_guest<HhaModel>(M_mdl)->mModelData), Hha_guest<void>(bck), 1, 2, 0, -1, 0, 1.f) != 0;
}
// Tev state is a sparse field copy: HD intentionally leaves the matrix/padding
// blocks alone.
// HD: light copies read and store one field at a time, never the matrix tail.
static void copyLight(HhaLight* src, HhaLight* dst) {
  dst->mPosition.x = (f32)src->mPosition.x;
  dst->mPosition.y = (f32)src->mPosition.y;
  dst->mPosition.z = (f32)src->mPosition.z;
  dst->mDirection.x = (f32)src->mDirection.x;
  dst->mDirection.y = (f32)src->mDirection.y;
  dst->mDirection.z = (f32)src->mDirection.z;
  for (u32 i=0; i<4; ++i) dst->mColor[i] = (u8)src->mColor[i];
  for (u32 i=0; i<4; ++i) dst->mAngle[i] = (s16)src->mAngle[i];
  for (u32 i=0; i<8; ++i) dst->mAttenuation[i] = (f32)src->mAttenuation[i];
}
// HD: sparse TEV assignment leaves all matrix and padding bytes untouched.
static void tev_copy(u32 source, u32 destination) {
  HhaTev* src = Hha_guest<HhaTev>(source);
  HhaTev* dst = Hha_guest<HhaTev>(destination);
  copyLight(&src->mLightObj, &dst->mLightObj);
  for (u32 i=0; i<3; ++i) dst->mLightPosWorld[i] = (u32)src->mLightPosWorld[i];
  for (u32 i=0; i<4; ++i) dst->mColorC0[i] = (u16)src->mColorC0[i];
  dst->mColorK0 = (u32)src->mColorK0;
  dst->mColorK1 = (u32)src->mColorK1;
  for (u32 i=0; i<4; ++i) dst->mFogColor[i] = (u16)src->mFogColor[i];
  dst->mFogStartZ = (f32)src->mFogStartZ;
  dst->mFogEndZ = (f32)src->mFogEndZ;
  dst->mColpatBlend = (f32)src->mColpatBlend;
  dst->mInitTimer = (u8)src->mInitTimer;
  dst->mEnvrIdxCurr = (u8)src->mEnvrIdxCurr;
  dst->mEnvrIdxPrev = (u8)src->mEnvrIdxPrev;
  dst->mColpatCurr = (u8)src->mColpatCurr;
  dst->mColpatPrev = (u8)src->mColpatPrev;
  dst->mRoomNo = (u8)src->mRoomNo;
  dst->mEnvrIdxOverride = (u8)src->mEnvrIdxOverride;
  dst->mLightMode = (u8)src->mLightMode;
  dst->mInitType = (u8)src->mInitType;
  copyLight(&src->mHdLightParam, &dst->mHdLightParam);
  copyLight(&src->mHdLightParamExtra, &dst->mHdLightParamExtra);
}

/* 02354200 */
void daObjHhaYgush_c::init_data(cXyz *position, f32 distance, csXyz *angle,
               cXyz *scale, void *tev, u8 visible) {
  u32 ang = gabi::ea(angle);
  Hha_YrotS(l_matrix, Hha_guest<csXyz>(ang)->y);
  gabi::Local<cXyz> vec, sum;
  Hha_multVec(l_matrix, &l_baseZ, vec.get());
  Hha_scaleVec(vec.get(), vec.get(), distance);
  Hha_sumVectorResult(position, sum.get(), vec.get());
  u32 x = Hha_bits(sum->x), y = Hha_bits(sum->y),
      z = Hha_bits(sum->z);
  Hha_bits(mBasePos.z) = z;
  Hha_bits(mBasePos.x) = x;
  Hha_bits(mPos.x) = x;
  Hha_bits(mBasePos.y) = y;
  Hha_bits(mPos.y) = y;
  Hha_bits(mPos.z) = z;
  copyAngle(Hha_guest<csXyz>(ang), reinterpret_cast<csXyz*>(&mRot));
  copyWords(reinterpret_cast<cXyz*>(scale), reinterpret_cast<cXyz*>(&mScale));
  tev_copy(gabi::ea(tev), gabi::ea(&mTev));
  mbVisible = visible;
}

/* 023545DC */
void daObjHhaYgush_c::init_mtx() {
  u32 model = M_mdl;
  if (model) {
    copyVector(reinterpret_cast<cXyz*>(&mScale), reinterpret_cast<cXyz*>(&Hha_guest<HhaModel>(model)->mBaseScale));
    Hha_transS(l_matrix, mPos.x, mPos.y, mPos.z);
    Hha_ZXYrotM(l_matrix, mRot.x, mRot.y, mRot.z);
    Hha_scaleM(13.f, 1.f, 11.f);
    model = M_mdl;
    copyMatrix(l_matrix, Hha_guest<be<f32>>(gabi::ea(&Hha_guest<HhaModel>(model)->mBaseTransformMtx)));
  }
}

/* 023546E0 */
void daObjHhaYgush_c::draw() {
  if (mbVisible && M_mdl) {
    u32 env = Hha_getEnvLight();
    Hha_settingTevStruct(Hha_guest<void>(env), 2, &mPos, &mTev);
    env = Hha_getEnvLight();
    Hha_setLightTevColorType(Hha_guest<void>(env), Hha_guest<void>(M_mdl), &mTev);
    Hha_btkEntry(&mBtk, Hha_guest<void>(Hha_guest<HhaModel>(M_mdl)->mModelData), mBtk.mFrameCtrl.mFrame);
    Hha_bckEntry(&mBck, Hha_guest<void>(Hha_guest<HhaModel>(M_mdl)->mModelData), mBck.mFrameCtrl.mFrame);
    Hha_modelUpdateDL(Hha_guest<void>(M_mdl), 0);
  }
}

/* 02354974 */
BOOL daObjHha_c::create_heap() {
  for (u32 i = 0; i < 2; ++i) {
    if (!mPartA[i].set_mdl_area("Hha", l_daObjHha_bdl_idx_table[i]))
      return 0;
    if (!mPartA[i].set_bgw("Hha", l_daObjHha_dzb_idx_table[i]))
      return 0;
  }
  u32 data = resource("Hha", dRes_INDEX_HHA_BDL_YSWTR00_e);
  // HD: missing waterfall model data still creates the gush.
  if (!data) {
    Hha_assert(Hha_guest<void>(gc_tu.mActorAssertFile), 0x324, Hha_guest<void>(gc_tu.mActorModelAssert));
    return mYgush.create_area("Hha");
  }
  u32 model = Hha_modelCreate(Hha_guest<void>(data), 0x80000u, 0x11000222u);
  mpModel = model;
  for (u32 i = 0; i < 2; ++i) {
    u32 animation = resource("Hha", l_daObjHha_btk_idx_table[i]);
    if (!animation)
      Hha_assert(Hha_guest<void>(gc_tu.mActorAssertFile), 0x32F, Hha_guest<void>(gc_tu.mActorBtkAssert));
    if (!Hha_btkInit(&mBtkA[i], Hha_guest<void>(data), Hha_guest<void>(animation), 1, l_daObjHha_btk_mode_table[i], 0, -1, 0, 1.f, 0))
      return 0;
  }
  return mYgush.create_area("Hha");
}

/* 02354B3C */
BOOL solidHeapCB(Actor *actor) {
  return actor->create_heap();
}

/* 02354B40 */
void daObjHha_c::set_tex(f32 frame, f32 speed, s32 index) {
  mBtkA[index].mFrameCtrl.mFrame = frame;
  mBtkA[index].mFrameCtrl.mRate = speed;
}

/* 02354B54 */
void daObjHha_c::init_mtx() {
  u32 model = mpModel;
  copyVector(reinterpret_cast<cXyz*>(&scale), reinterpret_cast<cXyz*>(&Hha_guest<HhaModel>(model)->mBaseScale));
  Hha_transS(l_matrix, current.pos.x, current.pos.y, current.pos.z);
  Hha_ZXYrotM(l_matrix, shape_angle.x, shape_angle.y, shape_angle.z);
  Hha_transM(mPosOffset.x, mPosOffset.y, mPosOffset.z);
  Hha_scaleM(1.f, 1.f, mWtrScale);
  model = mpModel;
  copyMatrix(l_matrix, Hha_guest<be<f32>>(gabi::ea(&Hha_guest<HhaModel>(model)->mBaseTransformMtx)));
  Hha_modelCalc(Hha_guest<void>(mpModel));
}

/* 02354C60 */
f32 daObjHha_c::get_water_h() {
  gabi::Local<HhaWaterCheck> check;
  gabi::Local<cXyz> position, vec;
  Hha_waterCheckCtor(check.get());
  copyVector(reinterpret_cast<cXyz*>(&current.pos), position.get());
  s16 yaw = current.angle.y;
  f32 height = current.pos.y;
  copyVector(&l_baseZ, vec.get());
  Hha_YrotS(l_matrix, yaw);
  Hha_multVec(l_matrix, &l_baseZ, vec.get());
  Hha_scaleVec(vec.get(), vec.get(), 400.f);
  Hha_addVector(position.get(), vec.get(), position.get());
  if (Hha_waterCross(position.get(), check.get(), 1.f))
    height = (f32)check->mHeight;
  check->mVtable = gc_tu.mWaterVtable;
  check->mPolyVtable = gc_tu.mWaterPolyVtable;
  check->mGroupVtable = gc_tu.mWaterGroupVtable;
  Hha_polyInfoDtor(&check->mBaseVtable, 0);
  return height;
}

/* 02354D7C */
void daObjHha_c::init_co() {
  u32 p = gabi::ea(this);
  gabi::Local<cXyz> center;
  Hha_YrotS(l_matrix, current.angle.y);
  Hha_multVec(l_matrix, &l_baseZ, center.get());
  Hha_scaleVec(center.get(), center.get(), 460.f);
  f32 height = get_water_h();
  center->y = height;
  Hha_addVector(center.get(), &current.pos, center.get());
  Hha_statusInit(&mCylStts, 0xFF, 0xFF, this);
  mCyl.mStts = &mCylStts;
  Hha_setCylinder(&mCyl, Hha_guest<void>(gc_tu.mCylinderData));
  Hha_cylinderSetC(&mCyl.mCyl, center.get());
  if (!switch_on(p) && !mIsMiddle) {
    Hha_statusInit(&mSphStts, 0xFF, 0xFF, this);
    mSph.mStts = &mSphStts;
    Hha_setSphere(&mSph, Hha_guest<void>(gc_tu.mSphereData));
    f32 y = current.pos.y, z = current.pos.z, x = current.pos.x;
    center->set(x, y - 400.f, z - 110.f);
    Hha_sphereSetC(&mSph.mSph, center.get());
    Hha_sphereSetR(&mSph.mSph, 220.f);
  }
}

static void splash_stop(u32 splash) {
  u32 emitter = Hha_guest<daObjHhaSplash_c>(splash)->mSplashCb.mpEmitter;
  if (emitter) {
    Hha_guest<HhaEmitter>(emitter)->mFlags = Hha_guest<HhaEmitter>(emitter)->mFlags | 1;
    Hha_guest<daObjHhaSplash_c>(splash)->mbIsActive = 0;
  }
}
static void splash_play(u32 splash) {
  u32 emitter = Hha_guest<daObjHhaSplash_c>(splash)->mSplashCb.mpEmitter;
  if (emitter) {
    Hha_guest<HhaEmitter>(emitter)->mFlags = Hha_guest<HhaEmitter>(emitter)->mFlags & ~1u;
    Hha_guest<daObjHhaSplash_c>(splash)->mbIsActive = 1;
  }
}

/* 02354EE0 */
s32 daObjHha_c::_create() {
  u32 p = gabi::ea(this), flags = actor_condition;
  if (!(flags & 8)) {
    if (p) {
      Hha_actorCtor(this);
      __vtbl = gc_tu.mActorVtable;
      Hha_constructArray(&mBtkA[0], 2, 0x74, Hha_guest<void>(0x025E7C6Cu));
      Hha_statusBaseCtor(&mCylStts);
      Hha_statusGameCtor(&mCylStts.__vtbl_gstts);
      mCylStts.__vtbl_gstts = gc_tu.mStatusGameVtable;
      mCylStts.__vtbl = gc_tu.mStatusBaseVtable;
      Hha_collisionObjectCtor(&mCyl);
      Hha_guest<HhaShapeAttribute>(gabi::ea(mCyl.mShapeAttr))->mVtable = gc_tu.mShapeBaseVtable;
      Hha_guest<HhaShapeAttribute>(gabi::ea(mCyl.mShapeAttr))->mBoundsVtable = gc_tu.mBoundsVtable;
      Hha_cylinderGeometryCtor(&mCyl.mCyl);
      mCyl.__vtbl_hitinf = gc_tu.mCylinderVtable;
      mCyl.mCyl.__vtbl = gc_tu.mCylinderGeometryVtable;
      Hha_guest<HhaShapeAttribute>(gabi::ea(mCyl.mShapeAttr))->mVtable = gc_tu.mCylinderShapeVtable;
      Hha_statusBaseCtor(&mSphStts);
      Hha_statusGameCtor(&mSphStts.__vtbl_gstts);
      mSphStts.__vtbl = gc_tu.mStatusBaseVtable;
      mSphStts.__vtbl_gstts = gc_tu.mStatusGameVtable;
      Hha_sphereCtor(&mSph);
      gushConstructor(&mYgush);
      Hha_constructArray(&mSplashA[0].mSplashCb.mVtable, 2, 0x34, Hha_guest<void>(0x02355F70u));
      flags = actor_condition;
    }
    actor_condition = flags | 8;
  }
  s32 phase = Hha_resLoad(&mPhs, "Hha");
  if (phase != HHA_PHASE_COMPLETE)
    return phase;
  if (!Hha_entrySolidHeap(this, Hha_guest<void>(0x02354B3Cu), 0x3C80))
    return HHA_PHASE_ERROR;
  u32 params = mParameters;
  mSwitchNo = params & 0xFF;
  mIsMiddle = (params >> 8) & 0xFF;
  u32 pl = play();
  s32 event = Hha_getEventIdx(reinterpret_cast<void*>(&Hha_guest<HhaPlay>(pl)->mEventManager), "hha_close", 0xFF);
  u8 middle = mIsMiddle;
  mEventIdx = event;
  if (middle == 15)
    mIsMiddle = 0;
  if (switch_on(p)) {
    middle = mIsMiddle;
    mState = State_CLOSED;
    mIsMiddle = (middle + 1) & 1;
  }
  mPartTimer = -1;
  middle = mIsMiddle;
  u32 posIndex = (middle & 1) * 8;
  for (u32 i = 0; i < 2; ++i) {
    u32 part = gabi::ea(&mPartA[i]);
    Hha_guest<daObjHhaPart_c>(part)->init_data(pos_y[posIndex / 4], tar_y[posIndex / 4], move_frame[middle], (u8)i, middle);
    pl = play();
    Hha_bgRegist(reinterpret_cast<void*>(&Hha_guest<HhaPlay>(pl)->mBgS), Hha_guest<void>(Hha_guest<daObjHhaPart_c>(part)->mpBgw), this);
    part_parent_matrix(part, p);
    posIndex += 4;
    if (i == 0)
      middle = mIsMiddle;
  }
  mWtrScale = 1.f;
  mPosOffset.z = 50.f;
  mPosOffset.x = 0.f;
  mPosOffset.y = -1400.f;
  for (u32 i = 0; i < 2; ++i)
    mSplashA[i].create_s(l_daObjHha_splash_id_table[i], &current.pos, splash_y[i], splash_z[i], &current.angle);
  if (!mIsMiddle) {
    set_tex(36.f, 0.f, 1);
    mHitboxActive = 1;
    mYgush.init_data(&current.pos, 600.f, &current.angle, &scale, &tevStr, 1);
    mWaterSound = 1;
  } else {
    set_tex(0.f, 0.f, 1);
    splash_stop(gabi::ea(&mSplashA[0].mSplashCb.mVtable));
    splash_stop(gabi::ea(&mSplashA[1].mSplashCb.mVtable));
    mYgush.init_data(&current.pos, 600.f, &current.angle, &scale, &tevStr, 0);
    mWaterSound = 0;
  }
  mYgush.init_mtx();
  init_mtx();
  init_co();
  u32 model = mPartA[0].mpModel;
  cullMtx = model ? gabi::ea(&Hha_guest<HhaModel>(model)->mBaseTransformMtx) : 0;
  Hha_setCullBox(this, -700.f, -1600.f, -400.f, 700.f, 120.f, 1000.f);
  return HHA_PHASE_COMPLETE;
}

/* 02355394 */
u8 daObjHha_c::_delete() {
  if (heap)
    for (u32 i = 0; i < 2; ++i) {
      u32 part = gabi::ea(&mPartA[i]), bg = Hha_guest<daObjHhaPart_c>(part)->mpBgw;
      if (bg && Hha_guest<be<u32>>(bg)[0] < 0x100) {
        u32 pl = play();
        Hha_bgRelease(reinterpret_cast<void*>(&Hha_guest<HhaPlay>(pl)->mBgS), Hha_guest<void>(Hha_guest<daObjHhaPart_c>(part)->mpBgw));
        Hha_guest<daObjHhaPart_c>(part)->mpBgw = 0;
      }
    }
  for (u32 i = 0; i < 2; ++i) {
    u32 splash = gabi::ea(&mSplashA[i]);
    if (Hha_guest<daObjHhaSplash_c>(splash)->mSplashCb.mpEmitter) {
      Hha_removeSplash(Hha_guest<daObjHhaSplash_c>(splash));
      Hha_guest<daObjHhaSplash_c>(splash)->mbIsActive = 0;
    }
  }
  Hha_resDelete(&mPhs, "Hha");
  return 1;
}

/* 02355464 */
void daObjHha_c::part_manager() {
  s32 timer = mPartTimer;
  if (timer >= 0) {
    if (timer == 0) {
      u8 middle = mIsMiddle;
      mPartA[0].mCbExec.d = 0;
      mPartTimer = -1;
      mPartA[1].mCbExec.f = 0x02353D9C;
      mPartA[1].mCbExec.d = 0;
      mPartA[0].mCbExec.f = 0x02353D9C;
      mPartA[0].mCbExec.i = -1;
      mPartA[1].mCbExec.i = -1;
      if (!middle) {
        s32 reverb = Hha_getReverb((s8)current.roomNo);
        Hha_seStartCurrent(JA_SE_OBJ_F_WATERGATE_CL, &current.pos, 0, reverb);
      }
    } else
      mPartTimer = timer - 1;
  }
  for (u32 i = 0; i < 2; ++i)
    Hha_dispatch(&mPartA[i], this, mPartA[i].mCbExec);
}

/* 0235557C */
void daObjHha_c::set_splash_bottom_h() {
  f32 y = get_water_h();
  mSplashA[0].mPos.y = y;
}

/* 02353924 */
void daObjHha_get_r(void *destination, u32 yaw) {
  u32 dest = gabi::ea(destination);
  f32 scale = l_waterLevel * -190.f;
  gabi::Local<cXyz> vec;
  copyVector(&l_baseZ, vec.get());
  Hha_YrotS(l_matrix, yaw);
  Hha_multVec(l_matrix, &l_baseZ, vec.get());
  Hha_scaleVec(vec.get(), vec.get(), scale);
  if (!dest)
    dest = Hha_new(12);
  if (dest)
    copyVector(vec.get(), Hha_guest<cXyz>(dest));
}

/* 023555AC */
void daObjHha_c::set_splash_bottom_r() {
  gabi::Local<cXyz> result, base;
  copyVector(&l_baseZ, result.get());
  daObjHha_get_r(base.get(), current.angle.y);
  u32 x = Hha_bits(base->x);
  base->x = mSplashA[0].mBasePos.x;
  u32 y = Hha_bits(base->y);
  base->y = mSplashA[0].mBasePos.y;
  Hha_bits(result->x) = x;
  Hha_bits(result->y) = y;
  u32 z = Hha_bits(base->z);
  Hha_bits(result->z) = z;
  base->z = mSplashA[0].mBasePos.z;
  Hha_addVector(result.get(), base.get(), result.get());
  f32 vx = result->x, vz = result->z;
  mSplashA[0].mPos.x = vx;
  mSplashA[0].mPos.z = vz;
}

/* 02355650 */
void daObjHha_c::set_splash_bottom_stop_r() {
  f32 distance = (1.f - (f32)((s32)mWtrTimer - 15) / 75.f) * -300.f;
  gabi::Local<cXyz> vec, base;
  copyVector(&l_baseZ, vec.get());
  Hha_YrotS(l_matrix, current.angle.y);
  Hha_multVec(l_matrix, &l_baseZ, vec.get());
  Hha_scaleVec(vec.get(), vec.get(), distance);
  copyVector(reinterpret_cast<cXyz*>(&mSplashA[0].mPos.x), base.get());
  Hha_addVector(vec.get(), base.get(), vec.get());
  f32 x = vec->x, z = vec->z;
  mSplashA[0].mPos.x = x;
  mSplashA[0].mPos.z = z;
}

static void splash_remove(u32 splash) {
  Hha_removeSplash(Hha_guest<daObjHhaSplash_c>(splash));
  Hha_guest<daObjHhaSplash_c>(splash)->mbIsActive = 0;
}

/* 02355774 */
void daObjHha_c::water_manager() {
  set_splash_bottom_h();
  set_splash_bottom_r();
  u8 state = mWtrState;
  switch (state) {
  case Water_STOPPING: {
    f32 scale = mWtrScale - 0.0061111110262572765f, minimum = mWtrScaleMin;
    mWtrScale = scale;
    u32 timer = mWtrTimer;
    if (scale < minimum)
      mWtrScale = minimum;
    if (timer == 34) {
      set_tex(37.f, 1.f, 1);
      timer = mWtrTimer;
    }
    if (timer >= 15) {
      if (timer == 15) {
        u32 emitter = mSplashA[1].mSplashCb.mpEmitter;
        if (emitter) {
          Hha_guest<HhaEmitter>(emitter)->mFlags = Hha_guest<HhaEmitter>(emitter)->mFlags | 1;
          u32 reloaded = mSplashA[1].mSplashCb.mpEmitter;
          mSplashA[1].mbIsActive = 0;
          if (reloaded) {
            splash_remove(gabi::ea(&mSplashA[1].mSplashCb.mVtable));
            timer = mWtrTimer;
          } else
            timer = mWtrTimer;
        }
      } else {
        set_splash_bottom_stop_r();
        timer = mWtrTimer;
      }
    }
    timer = (timer - 1) & 0xFFFF;
    mWtrTimer = timer;
    if (!timer) {
      mWtrState = Water_IDLE;
      u32 emitter = mSplashA[0].mSplashCb.mpEmitter;
      if (emitter) {
        Hha_guest<HhaEmitter>(emitter)->mFlags = Hha_guest<HhaEmitter>(emitter)->mFlags | 1;
        u32 reloaded = mSplashA[0].mSplashCb.mpEmitter;
        mSplashA[0].mbIsActive = 0;
        if (reloaded)
          splash_remove(gabi::ea(&mSplashA[0].mSplashCb.mVtable));
      }
      mHitboxActive = 0;
      mYgush.mbVisible = 0;
      mWaterSound = 0;
    }
    break;
  }
  case Water_STARTING: {
    u32 timer = mWtrTimer;
    if (timer == 35) {
      splash_play(gabi::ea(&mSplashA[1].mSplashCb.mVtable));
      set_tex(0.f, 1.f, 1);
      timer = mWtrTimer;
    }
    u8 active = mSplashA[0].mbIsActive;
    timer = (timer - 1) & 0xFFFF;
    mWtrTimer = timer;
    if (!active) {
      // HD: retain the fused threshold and unordered comparison.
      f32 threshold = gabi::fmadds(5.f, l_waterLevel, 10.f);
      if (!((f32)timer > threshold)) {
        splash_play(gabi::ea(&mSplashA[0].mSplashCb.mVtable));
        timer = mWtrTimer;
        mWaterSound = 1;
        mYgush.mbVisible = 1;
      }
    }
    if (!timer) {
      set_tex(36.f, 0.f, 1);
      mWtrState = Water_IDLE;
      mHitboxActive = 1;
    }
  }
    break;
  default: break;
  }
  for (u32 i = 0; i < 2; ++i)
    Hha_animationPlay(&mBtkA[i]);
  init_mtx();
}

/* 02355A18 */
void daObjHha_c::ygush_manager() {
  if (mYgush.mbVisible) {
    gabi::Local<cXyz> base, result;
    daObjHha_get_r(base.get(), current.angle.y);
    u32 x = Hha_bits(base->x);
    base->x = mYgush.mBasePos.x;
    u32 y = Hha_bits(base->y);
    base->y = mYgush.mBasePos.y;
    Hha_bits(result->x) = x;
    Hha_bits(result->y) = y;
    u32 z = Hha_bits(base->z);
    Hha_bits(result->z) = z;
    base->z = mYgush.mBasePos.z;
    Hha_addVector(result.get(), base.get(), result.get());
    f32 height = get_water_h(),
        vz = result->z, vx = result->x;
    mYgush.mPos.z = vz;
    mYgush.mPos.x = vx;
    mYgush.mPos.y = height;
    result->y = height;
    base->z = vz;
    base->y = height;
    base->x = vx;
    Hha_animationPlay(&mYgush.mBtk);
    Hha_animationPlay(&mYgush.mBck);
    mYgush.init_mtx();
  }
}

/* 02355AE8 */
u8 daObjHha_c::_execute() {
  u32 p = gabi::ea(this);
  u8 state = mState;
  switch (state) {
  case State_OPEN: {
    if (switch_on(p)) {
      if (!mIsMiddle) {
        if (eventInfo.mCommand != dEvtCmd_INDEMO_e) {
          Hha_orderOtherEventId(this, mEventIdx, 0xFF, 0xFFFF, 0, 1);
          eventInfo.mCondition = eventInfo.mCondition | 2;
        } else {
          Hha_seStartGlobal(JA_SE_READ_RIDDLE_1);
          mState = State_SWITCHING;
          mWtrState = Water_STOPPING;
          mWtrTimer = 90;
          mPartTimer = 65;
        }
      } else {
        s32 event = mEventIdx;
        u32 pl = play();
        if (Hha_eventStartCheck(reinterpret_cast<void*>(&Hha_guest<HhaPlay>(pl)->mEventManager), event)) {
          mState = State_CLOSED;
          mWtrState = Water_STARTING;
          mWtrTimer = 35;
          mPartTimer = 0;
        }
      }
    }
    break;
  }
  case State_SWITCHING: {
    s32 event = mEventIdx;
    u32 pl = play();
    if (Hha_eventEndCheck(reinterpret_cast<void*>(&Hha_guest<HhaPlay>(pl)->mEventManager), event)) {
      pl = play();
      Hha_guest<HhaPlay>(pl)->mEventFlags = Hha_guest<HhaPlay>(pl)->mEventFlags | 8;
      mState = State_CLOSED;
    }
  }
    break;
  default: break;
  }
  part_manager();
  water_manager();
  ygush_manager();
  if (mWaterSound) {
    s32 reverb = Hha_getReverb((s8)current.roomNo);
    Hha_seStartCurrent(JA_SE_ATM_WATER_GATE, &current.pos, 0, reverb);
  }
  if (mHitboxActive) {
    gabi::Local<cXyz> center;
    copyVector(&mCyl.mCyl.mCenter, center.get());
    f32 height = get_water_h();
    center->y = height;
    Hha_cylinderSetC(&mCyl.mCyl, center.get());
    u32 pl = play();
    Hha_collisionSet(reinterpret_cast<void*>(&Hha_guest<HhaPlay>(pl)->mCcS), &mCyl);
    if (!switch_on(p) && !mIsMiddle) {
      pl = play();
      Hha_collisionSet(reinterpret_cast<void*>(&Hha_guest<HhaPlay>(pl)->mCcS), &mSph);
    }
  }
  return 1;
}

/* 02355D9C */
u8 daObjHha_c::_draw() {
  u32 env = Hha_getEnvLight();
  Hha_settingTevStruct(Hha_guest<void>(env), 1, &current.pos, &tevStr);
  for (u32 i = 0; i < 2; ++i)
    Hha_dispatch(&mPartA[i], this, mPartA[i].mCbDraw);
  if (mpModel) {
    env = Hha_getEnvLight();
    Hha_settingTevStruct(Hha_guest<void>(env), 0, &current.pos, &tevStr);
    env = Hha_getEnvLight();
    Hha_setLightTevColorType(Hha_guest<void>(env), Hha_guest<void>(mpModel), &tevStr);
    for (u32 i = 0; i < 2; ++i)
      Hha_btkEntryModel(&mBtkA[i], Hha_guest<void>(mpModel), (f32)mBtkA[i].mFrameCtrl.mFrame);
    Hha_modelUpdateDL(Hha_guest<void>(mpModel), 0);
  }
  mYgush.draw();
  return 1;
}

/* 02355EB8 */
s32 Mthd_Create(Actor *actor) {
  return actor->_create();
}

/* 02355EBC */
u8 Mthd_Delete(Actor *actor) {
  return actor->_delete();
}

/* 02355EC0 */
u8 Mthd_Execute(Actor *actor) {
  return actor->_execute();
}

/* 02355EC4 */
u8 Mthd_Draw(Actor *actor) {
  return actor->_draw();
}
// HD: compiler-generated and template code of this translation unit.
/* 02354778 */
void *gushConstructor(void *gush) {
  u32 p = gabi::ea(gush);
  if (!p)
    p = Hha_new(0x2F8);
  if (p) {
    Hha_btkCtor(&Hha_guest<daObjHhaYgush_c>(p)->mBtk);
    Hha_frameCtrlCtor(&Hha_guest<daObjHhaYgush_c>(p)->mBck, 0);
    Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mVtable = gc_tu.mFrameCtrlVtable;
    Hha_bckControllerCtor(&Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mPadding14);
    Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mState = 0;
    Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mCount = 0;
    Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mVtable = gc_tu.mBckVtable;
    Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mResource = 0;
    Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mControllerVtable = gc_tu.mControllerVtable;
    Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mFlags = 0;
    Hha_guest<HhaAnimationTail>(gabi::ea(&Hha_guest<daObjHhaYgush_c>(p)->mBck))->mBuffer = 0;
    copyLight(Hha_guest<HhaLight>(gc_tu.mDefaultLight), &Hha_guest<daObjHhaYgush_c>(p)->mTev.mLightObj);
    copyLight(Hha_guest<HhaLight>(gc_tu.mDefaultLight), &Hha_guest<daObjHhaYgush_c>(p)->mTev.mHdLightParam);
    copyLight(Hha_guest<HhaLight>(gc_tu.mDefaultLight), &Hha_guest<daObjHhaYgush_c>(p)->mTev.mHdLightParamExtra);
  }
  return Hha_guest<void>(p);
}

/* 02355EC8 */
void __sinit_d_a_obj_hha_cpp() {
  l_headerWords[2] = 0;
  l_headerWords[0] = 0;
  l_headerWords[3] = 0;
  l_headerWords[1] = 0;
  Hha_registerDestructor(Hha_guest<void>(gc_tu.mHeaderLink0));
  l_headerValues[0] = l_headerFloats[0];
  l_headerValues[1] = l_headerFloats[1];
  Hha_headerGuardCtor(&l_headerGuard);
  Hha_registerDestructor(Hha_guest<void>(gc_tu.mHeaderLink1));
  Hha_headerByteCtor(&l_headerByte);
  Hha_registerDestructor(Hha_guest<void>(gc_tu.mHeaderLink2));
}

/* 02355F5C */
void SafeString_destructor(void *object, s32 mode) {
  if (object && (mode & 1))
    Hha_delete(object);
}

/* 02355F70 */
void *splashConstructor(void *splash) {
  u32 p = gabi::ea(splash);
  if (!p)
    p = Hha_new(0x34);
  if (p)
    Hha_followCallbackCtor(Hha_guest<void>(p), 0, 0);
  return Hha_guest<void>(p);
}

/* 02355FC0 */
void splashDestructor(void *object, s32 mode) {
  if (object && (mode & 1))
    Hha_delete(object);
}

/* 02355FD4 */
void actorDestructor(Actor *actor, s32 mode) {
  u32 p = gabi::ea(actor);
  if (p) {
    Hha_destroyArray(&Hha_guest<daObjHha_c>(p)->mSplashA[0].mSplashCb.mVtable, 2, 0x34, Hha_guest<void>(0x02355FC0u), 0, 0);
    Hha_bckControllerDtor(&reinterpret_cast<HhaAnimationTail*>(&Hha_guest<daObjHha_c>(p)->mYgush.mBck)->mVtable, 0);
    Hha_sphereDtor(&Hha_guest<daObjHha_c>(p)->mSph, 2);
    Hha_statusDtor(&Hha_guest<daObjHha_c>(p)->mSphStts, 2);
    Hha_cylinderDtor(&Hha_guest<daObjHha_c>(p)->mCyl, 2);
    Hha_statusDtor(&Hha_guest<daObjHha_c>(p)->mCylStts, 2);
    Hha_actorDtor(actor, 0);
    if (mode & 1)
      Hha_delete(actor);
  }
}

/* 02356084 */
void SafeString_assureTerminationImpl(void *object) { }

/* 02356088 */
BOOL Mthd_IsDelete(Actor *actor) {
  return 1;
}

/* WWHD glue
str 1002A578 "Hha"
str 1002A540 "hha_close"
data l_daObjHha_bdl_idx_table 1002A320
data l_daObjHha_dzb_idx_table 1002A328
data l_daObjHha_btk_idx_table 1002A330
data l_daObjHha_btk_mode_table 1002A338
data l_daObjHha_splash_id_table 1002A344
data splash_y 1002A50C
data splash_z 1002A514
data move_frame 1002A51C
data pos_y 1002A520
data tar_y 1002A530
data l_headerFloats 1002A570
data l_resourceControl 101F4F28 nocheck
data l_saveInfo 101F84DC nocheck
data l_waterLevel 101D5F28 nocheck
data l_baseZ 101FFBCC nocheck
data l_matrix 1048D0CC nocheck
data l_headerWords 10469E64 nocheck
data l_headerValues 10469E58 nocheck
data l_headerGuard 10469E60 nocheck
data l_headerByte 10469E61 nocheck
ext 025200D4 u32 Hha_getPlay()
ext 026066C4 u32 Hha_getObjectRes(void*, void*, u32)
ext 0273AA24 void Hha_assert(void*, u32, void*)
ext 025BA0C0 BOOL Hha_isSwitch(void*, u32, u32)
ext 025F1884 void Hha_YrotS(void*, u32)
ext 028E8F64 void Hha_multVec(void*, void*, void*)
ext 028E8E64 void Hha_scaleVec(void*, void*, f32)
ext 0273AD10 u32 Hha_new(u32)
ext 0201ADE0 void Hha_subtractVector(void*, void*, void*)
ext 0201B47C void Hha_normalizeRS(void*)
ext 025E38E0 u32 Hha_modelCreate(void*, u32, u32)
ext 024F2478 u32 Hha_bgNewSet(void*, u32, void*)
ext 028E93CC void Hha_transS(void*, f32, f32, f32)
ext 025F1B48 void Hha_ZXYrotM(void*, u32, u32, u32)
ext 025F24E0 void Hha_transM(f32, f32, f32)
ext 027F4D5C void Hha_modelCalc(void*)
ext 024F43DC void Hha_bgMove(void*)
ext 028E8F44 f32 Hha_dotProduct(void*, void*)
ext 025CB374 void Hha_startShock(void*, u32, u32, void*)
ext 02555D0C u32 Hha_getEnvLight()
ext 02562F5C void Hha_setLightTevColorType(void*, void*, void*)
ext 025E2DE0 void Hha_modelUpdateDL(void*, u32)
ext 028E8D88 void Hha_addVector(void*, void*, void*)
ext 025A847C void Hha_setParticle(void*, u32, u32, void*, void*, u32, void*, u32, u32, void*, void*, void*)
ext 025E7CE0 BOOL Hha_btkInit(void*, void*, void*, u32, u32, u32, u32, u32, f32, u32)
ext 025E8508 BOOL Hha_bckInit(void*, void*, void*, u32, u32, u32, u32, u32, f32)
ext 0201AD78 void Hha_sumVectorResult(void*, void*, void*)
ext 025F2518 void Hha_scaleM(f32, f32, f32)
ext 025626A4 void Hha_settingTevStruct(void*, u32, void*, void*)
ext 025E7FC4 void Hha_btkEntry(void*, void*, f32)
ext 025E86B8 void Hha_bckEntry(void*, void*, f32)
ext 025E7C6C void Hha_btkCtor(void*)
ext 027F2BC0 void Hha_frameCtrlCtor(void*, u32)
ext 027DA984 void Hha_bckControllerCtor(void*)
ext 024F22DC void Hha_waterCheckCtor(void*)
ext 024F15A4 BOOL Hha_waterCross(void*, void*, f32)
ext 02008B4C void Hha_polyInfoDtor(void*, u32)
ext 02515F14 void Hha_statusInit(void*, u32, u32, void*)
ext 02516518 void Hha_setCylinder(void*, void*)
ext 020182E0 void Hha_cylinderSetC(void*, void*)
ext 0251677C void Hha_setSphere(void*, void*)
ext 02018D40 void Hha_sphereSetC(void*, void*)
ext 02018C8C void Hha_sphereSetR(void*, f32)
ext 025D4ED0 void Hha_actorCtor(void*)
ext 028EFFD0 void Hha_constructArray(void*, u32, u32, void*)
ext 0200BD2C void Hha_statusBaseCtor(void*)
ext 02515DA0 void Hha_statusGameCtor(void*)
ext 02515FB8 void Hha_collisionObjectCtor(void*)
ext 02018590 void Hha_cylinderGeometryCtor(void*)
ext 025166F0 void Hha_sphereCtor(void*)
ext 02520460 s32 Hha_resLoad(void*, const char*)
ext 025D63E8 BOOL Hha_entrySolidHeap(void*, void*, u32)
ext 02543F10 s32 Hha_getEventIdx(void*, const char*, u32)
ext 024EEA6C void Hha_bgRegist(void*, void*, void*)
ext 025D674C void Hha_setCullBox(void*, f32, f32, f32, f32, f32, f32)
ext 020087EC void Hha_bgRelease(void*, void*)
ext 025204C8 void Hha_resDelete(void*, const char*)
ext 02520540 s32 Hha_getReverb(u32)
ext 025E1A40 void Hha_seStartCurrent(u32, void*, u32, u32)
ext 025E742C void Hha_animationPlay(void*)
ext 025D7A58 void Hha_orderOtherEventId(void*, u32, u32, u32, u32, u32)
ext 025E1988 void Hha_seStartGlobal(u32)
ext 0254407C BOOL Hha_eventStartCheck(void*, u32)
ext 025440C8 BOOL Hha_eventEndCheck(void*, u32)
ext 0200E240 void Hha_collisionSet(void*, void*)
ext 025E8048 void Hha_btkEntryModel(void*, void*, f32)
ext 028F026C void Hha_registerDestructor(void*)
ext 028ED6F8 void Hha_headerGuardCtor(void*)
ext 028EAB2C void Hha_headerByteCtor(void*)
ext 0273AF40 void Hha_delete(void*)
ext 025A5894 void Hha_followCallbackCtor(void*, u32, u32)
ext 028F0164 void Hha_destroyArray(void*, u32, u32, void*, u32, u32)
ext 027F3628 void Hha_bckControllerDtor(void*, u32)
ext 02515AE8 void Hha_sphereDtor(void*, u32)
ext 02515860 void Hha_statusDtor(void*, u32)
ext 02515A70 void Hha_cylinderDtor(void*, u32)
ext 025D50BC void Hha_actorDtor(void*, u32)
*/
