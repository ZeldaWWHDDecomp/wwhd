/**
 * d_a_warpgn.cpp (WWHD)
 * Warpgn - warp portal to Forsaken Fortress (Gmjwp, daWarpgn_c)
 *
 * Written from the WWHD
 * code with the GameCube decompilation (zeldaret/tww src/d/actor/d_a_warpgn.cpp) as reference,
 * verified against cking.rpx.
 */
#include "d/actor/d_a_warpgn.h"
#include "bindings.h"
static const u32 action_table[6] = {0x100425E0, 0x100425E8, 0x100425F0, 0x100425FC, 0x1004260C, 0x10042618};
static WarpgnEventMethod event_init_tbl[6] = {};
static WarpgnEventMethod event_action_tbl[6] = {};
static u32 l_one = 0x3F800000;
static u32 l_zero = 0x00000000;
static u32 l_colorDivisor = 0x437F0000;
static u32 l_reverse = 0xBF800000;
static u32 l_warpDistance = 0x43480000;
static u32 l_arriveFrame = 0x3F000000;
static u32 l_cullMin = 0xC3C80000;
static u32 l_cullMax = 0x43C80000;
static u32 l_cullHeight = 0x44FA0000;
static u32 l_headerLow = 0xC0490FDB;
static u32 l_headerHigh = 0x40490FDB;
static u32 l_saveInfo = 0;
static u32 l_resControl = 0;
static u32 l_demoObject = 0;
static u32 l_headerWord2 = 0;
static u32 l_headerWord0 = 0;
static u32 l_headerWord3 = 0;
static u32 l_headerWord1 = 0;
static u32 l_headerValueLow = 0;
static u32 l_headerValueHigh = 0;
static f32 l_matrix[12] = {};
static const WarpgnDefaultLight l_defaultLight = {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 255, 255, 255, 255, 255, 255, 255, 255, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
static const WarpgnTU gc_tu = {0x10042570, 0x10042580, 0x1004254C, 0x10042558, 0x10042564, 0x10042538, 0x10042528, 0x024D8BB0, 0x10042518, 0x100424D8, 0x1016E54C, 0x1016D820, 0x100424F0, 0x101D30B4, 0x101D30C0, 0x101D30CC, 0x1046EB50, 0x1046EB51};

static void resumeEmitter(u32 emitter) {
  if (emitter)
    Warpgn_view<WarpgnEmitter>(emitter)->mFlags = u32(Warpgn_view<WarpgnEmitter>(emitter)->mFlags) & ~1u;
}

static void playAnimation(be<u32>& animation, u32 speed) {
  u32 p = animation;
  if (p) {
    Warpgn_view<WarpgnAnimation>(p)->mRate = speed;
    p = animation;
    Warpgn_animationPlay(Warpgn_view<void>(p));
  }
}
static void endAnimation(u32 p) {
  if (p)
    Warpgn_view<WarpgnAnimation>(p)->mFrame = f32(s16(Warpgn_view<WarpgnAnimation>(p)->mEnd));
}

static void* eventManager() {
  return &Warpgn_view<WarpgnPlay>(Warpgn_getPlay())->mEventManager;
}

static u32 materialColor(u32 material) {
  u32 block = u32(Warpgn_view<WarpgnMaterial>(material)->mpTevBlock), vtable = u32(Warpgn_view<WarpgnTevBlock>(block)->mVtable);
  return gabi::call_ptr<u32>(u32(Warpgn_view<WarpgnTevVtable>(vtable)->mGetColor), Warpgn_view<void>(block), 1);
}

/* 024D915C */
s32 daWarpgn_c::_delete() {
  u32 p = this->mpEmitter1;
  if (p) {
    u32 flags = u32(Warpgn_view<WarpgnEmitter>(p)->mFlags);
    Warpgn_view<WarpgnEmitter>(p)->mMaxFrame = 0xFFFFFFFF;
    Warpgn_view<WarpgnEmitter>(p)->mFlags = flags | 1;
    this->mpEmitter1 = 0;
  }
  p = this->mpEmitter2;
  if (p) {
    u32 flags = u32(Warpgn_view<WarpgnEmitter>(p)->mFlags);
    Warpgn_view<WarpgnEmitter>(p)->mMaxFrame = 0xFFFFFFFF;
    Warpgn_view<WarpgnEmitter>(p)->mFlags = flags | 1;
    this->mpEmitter2 = 0;
  }
  p = this->mpEmitter3;
  if (p) {
    u32 flags = u32(Warpgn_view<WarpgnEmitter>(p)->mFlags);
    Warpgn_view<WarpgnEmitter>(p)->mMaxFrame = 0xFFFFFFFF;
    Warpgn_view<WarpgnEmitter>(p)->mFlags = flags | 1;
    this->mpEmitter3 = 0;
  }
  Warpgn_resDelete(&this->mPhs, "Gmjwp");
  return 1;
}

/* 024D8BB0 */
s32 CheckCreateHeap(daWarpgn_c *actor) {
  return actor->CreateHeap();
}

/* 024D885C */
s32 daWarpgn_c::CreateHeap() {
  struct SafeName {
    be<u32> text, vtable;
  };
  struct Names {
    u8 prefix[0x14];
    SafeName model, btk1, btk2, brk, bck;
    u8 tail[0x44];
  };
  static_assert(sizeof(Names) == 0x80);
  static_assert(offsetof(Names, model) == 0x14);
  gabi::Local<Names> names;
  gabi::Local<be<u32>[4]> outgoing;
  names->model.vtable = gc_tu.mSafeStringVtable;
  names->model.text = gabi::guest_addr("Gmjwp");
  u32 data = Warpgn_getResource(Warpgn_view<void>(l_resControl), &names->model, dRes_INDEX_GMJWP_BDL_GMJWP00_e);
  if (!data)
    Warpgn_assertion(Warpgn_view<void>(gc_tu.mSourceName), 0xCF, Warpgn_view<void>(gc_tu.mModelAssertion));
  u32 model = Warpgn_modelCreate(Warpgn_view<void>(data), 0x80000, 0x11000222);
  this->mpModel = model;
  if (!model)
    return 0;
  names->btk1.vtable = gc_tu.mSafeStringVtable;
  names->btk1.text = gabi::guest_addr("Gmjwp");
  u32 resource =
      Warpgn_getResource(Warpgn_view<void>(l_resControl), &names->btk1, dRes_INDEX_GMJWP_BTK_GMJWP00_e);
  if (!resource)
    Warpgn_assertion(Warpgn_view<void>(gc_tu.mSourceName), 0xDE, Warpgn_view<void>(gc_tu.mBtkAssertion));
  u32 animation = Warpgn_btkConstruct(0);
  this->mpBtkAnm1 = animation;
  if (!animation)
    return 0;
  f32 speed = f32(Warpgn_float(&l_one));
  if (!Warpgn_btkInit(Warpgn_view<void>(animation), Warpgn_view<void>(data),
                       Warpgn_view<void>(resource), 1, 2, 0, -1, 0, speed, 0))
    return 0;
  Warpgn_floatRate(this->mpBtkAnm1) = speed;
  names->btk2.vtable = gc_tu.mSafeStringVtable;
  names->btk2.text = gabi::guest_addr("Gmjwp");
  resource = Warpgn_getResource(Warpgn_view<void>(l_resControl), &names->btk2, dRes_INDEX_GMJWP_BTK_GMJWP02_e);
  if (!resource)
    Warpgn_assertion(Warpgn_view<void>(gc_tu.mSourceName), 0xED, Warpgn_view<void>(gc_tu.mBtkAssertion));
  animation = Warpgn_btkConstruct(0);
  this->mpBtkAnm2 = animation;
  if (!animation)
    return 0;
  if (!Warpgn_btkInit(Warpgn_view<void>(animation), Warpgn_view<void>(data),
                       Warpgn_view<void>(resource), 1, 0, 0, -1, 0, speed, 0))
    return 0;
  u32 zero = l_zero;
  Warpgn_rate(this->mpBtkAnm2) = zero;
  names->brk.vtable = gc_tu.mSafeStringVtable;
  names->brk.text = gabi::guest_addr("Gmjwp");
  resource = Warpgn_getResource(Warpgn_view<void>(l_resControl), &names->brk, dRes_INDEX_GMJWP_BRK_GMJWP01_e);
  if (!resource)
    Warpgn_assertion(Warpgn_view<void>(gc_tu.mSourceName), 0xFF, Warpgn_view<void>(gc_tu.mBrkAssertion));
  animation = Warpgn_brkConstruct(0);
  this->mpBrkAnm = animation;
  if (!animation)
    return 0;
  if (!Warpgn_brkInit(Warpgn_view<void>(animation), Warpgn_view<void>(data),
                       Warpgn_view<void>(resource), 1, 0, 0, -1, 0, speed, 0))
    return 0;
  Warpgn_rate(this->mpBrkAnm) = zero;
  names->bck.vtable = gc_tu.mSafeStringVtable;
  names->bck.text = gabi::guest_addr("Gmjwp");
  resource = Warpgn_getResource(Warpgn_view<void>(l_resControl), &names->bck, dRes_INDEX_GMJWP_BCK_GMJWP01_e);
  if (!resource)
    Warpgn_assertion(Warpgn_view<void>(gc_tu.mSourceName), 0x10F, Warpgn_view<void>(gc_tu.mBckAssertion));
  animation = Warpgn_allocate(0x8C);
  if (animation) {
    Warpgn_frameConstruct(Warpgn_view<void>(animation), 0);
    Warpgn_view<WarpgnBckAnimation>(animation)->mVtable = gc_tu.mBaseAnimationVtable;
    Warpgn_bckMatrixConstruct(&Warpgn_view<WarpgnBckAnimation>(animation)->mMatrix);
    Warpgn_view<WarpgnBckAnimation>(animation)->mModel = 0;
    Warpgn_view<WarpgnBckAnimation>(animation)->mFrameData = 0;
    Warpgn_view<WarpgnBckAnimation>(animation)->mMatrixVtable = gc_tu.mMatrixVtable;
    Warpgn_view<WarpgnBckAnimation>(animation)->mMatrixAnm = 0;
    Warpgn_view<WarpgnBckAnimation>(animation)->mBlend = 0;
    Warpgn_view<WarpgnBckAnimation>(animation)->mVtable = gc_tu.mAnimationVtable;
    Warpgn_view<WarpgnBckAnimation>(animation)->mJoint = 0;
  }
  this->mpBckAnm = animation;
  if (!animation)
    return 0;
  if (!Warpgn_bckInit(Warpgn_view<void>(animation), Warpgn_view<void>(data),
                       Warpgn_view<void>(resource), 1, 0, 0, -1, 0, speed))
    return 0;
  Warpgn_rate(this->mpBckAnm) = zero;
  return 1;
}

/* 024D8BB4 */
daWarpgn_c *daWarpgnConstruct(daWarpgn_c *actor) {
  if (!actor) {
    actor = Warpgn_view<daWarpgn_c>(Warpgn_allocate(0x5C0));
    if (!actor)
      return nullptr;
  }
  Warpgn_actorConstruct(actor);
  actor->__vtbl = gc_tu.mActorVtable;
  // HD: cache each default-lighting component at its original load point, using lfs conversion for
  // floating components.
  const f32 lighting0 = f32(l_defaultLight.mPositionX);
  Warpgn_tev(&actor->mTevStr)->mLight.mPosition.x = lighting0;
  const f32 lighting1 = f32(l_defaultLight.mPositionY);
  Warpgn_tev(&actor->mTevStr)->mLight.mPosition.y = lighting1;
  const f32 lighting2 = f32(l_defaultLight.mPositionZ);
  Warpgn_tev(&actor->mTevStr)->mLight.mPosition.z = lighting2;
  const f32 lighting3 = f32(l_defaultLight.mDirectionX);
  Warpgn_tev(&actor->mTevStr)->mLight.mDirection.x = lighting3;
  const f32 lighting4 = f32(l_defaultLight.mDirectionY);
  Warpgn_tev(&actor->mTevStr)->mLight.mDirection.y = lighting4;
  const f32 lighting5 = f32(l_defaultLight.mDirectionZ);
  Warpgn_tev(&actor->mTevStr)->mLight.mDirection.z = lighting5;
  const u8 lighting6 = u8(l_defaultLight.mColorR);
  Warpgn_tev(&actor->mTevStr)->mLight.mColor.r = lighting6;
  const u8 lighting7 = u8(l_defaultLight.mColorG);
  Warpgn_tev(&actor->mTevStr)->mLight.mColor.g = lighting7;
  const u8 lighting8 = u8(l_defaultLight.mColorB);
  Warpgn_tev(&actor->mTevStr)->mLight.mColor.b = lighting8;
  const u8 lighting9 = u8(l_defaultLight.mColorA);
  Warpgn_tev(&actor->mTevStr)->mLight.mColor.a = lighting9;
  const s16 lighting10 = s16(l_defaultLight.mAngle0);
  Warpgn_tev(&actor->mTevStr)->mLight.mAngle[0] = lighting10;
  const s16 lighting11 = s16(l_defaultLight.mAngle1);
  Warpgn_tev(&actor->mTevStr)->mLight.mAngle[1] = lighting11;
  const s16 lighting12 = s16(l_defaultLight.mAngle2);
  Warpgn_tev(&actor->mTevStr)->mLight.mAngle[2] = lighting12;
  const s16 lighting13 = s16(l_defaultLight.mAngle3);
  Warpgn_tev(&actor->mTevStr)->mLight.mAngle[3] = lighting13;
  const f32 lighting14 = f32(l_defaultLight.mAttenuation0);
  Warpgn_tev(&actor->mTevStr)->mLight.mAttenuation[0] = lighting14;
  const f32 lighting15 = f32(l_defaultLight.mAttenuation1);
  Warpgn_tev(&actor->mTevStr)->mLight.mAttenuation[1] = lighting15;
  const f32 lighting16 = f32(l_defaultLight.mAttenuation2);
  Warpgn_tev(&actor->mTevStr)->mLight.mAttenuation[2] = lighting16;
  const f32 lighting17 = f32(l_defaultLight.mAttenuation3);
  Warpgn_tev(&actor->mTevStr)->mLight.mAttenuation[3] = lighting17;
  const f32 lighting18 = f32(l_defaultLight.mAttenuation4);
  Warpgn_tev(&actor->mTevStr)->mLight.mAttenuation[4] = lighting18;
  const f32 lighting19 = f32(l_defaultLight.mAttenuation5);
  Warpgn_tev(&actor->mTevStr)->mLight.mAttenuation[5] = lighting19;
  const f32 lighting20 = f32(l_defaultLight.mAttenuation6);
  Warpgn_tev(&actor->mTevStr)->mLight.mAttenuation[6] = lighting20;
  const f32 lighting21 = f32(l_defaultLight.mAttenuation7);
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAttenuation[4] = lighting18;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAttenuation[7] = lighting21;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAngle[1] = lighting11;
  Warpgn_tev(&actor->mTevStr)->mLight.mAttenuation[7] = lighting21;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mDirection.x = lighting3;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAngle[1] = lighting11;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mPosition.y = lighting1;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mPosition.x = lighting0;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAttenuation[3] = lighting17;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mDirection.y = lighting4;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAngle[2] = lighting12;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mColor.r = lighting6;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAttenuation[3] = lighting17;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mPosition.y = lighting1;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAttenuation[1] = lighting15;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAttenuation[6] = lighting20;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mColor.b = lighting8;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mDirection.z = lighting5;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAngle[3] = lighting13;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mColor.a = lighting9;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mPosition.z = lighting2;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAttenuation[1] = lighting15;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAttenuation[5] = lighting19;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mPosition.x = lighting0;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAngle[0] = lighting10;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mColor.r = lighting6;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mColor.b = lighting8;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mPosition.z = lighting2;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mDirection.y = lighting4;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAttenuation[0] = lighting14;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAttenuation[7] = lighting21;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAttenuation[5] = lighting19;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAngle[3] = lighting13;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mDirection.x = lighting3;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mColor.a = lighting9;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAngle[0] = lighting10;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAngle[2] = lighting12;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAttenuation[2] = lighting16;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mColor.g = lighting7;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAttenuation[4] = lighting18;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mAttenuation[2] = lighting16;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAttenuation[6] = lighting20;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mColor.g = lighting7;
  Warpgn_tev(&actor->mTevStr)->mHdLight[0].mAttenuation[0] = lighting14;
  Warpgn_tev(&actor->mTevStr)->mHdLight2.mDirection.z = lighting5;
  return actor;
}

/* 024D8DE8 */
void daWarpgn_c::CreateInit() {
  u32 model = this->mpModel;
  this->cullMtx = model ? model + 0xC8 : 0;
  Warpgn_cullBox(this, f32(Warpgn_float(&l_cullMin)), f32(Warpgn_float(&l_zero)),
                   f32(Warpgn_float(&l_cullMin)), f32(Warpgn_float(&l_cullMax)), f32(Warpgn_float(&l_cullHeight)),
                   f32(Warpgn_float(&l_cullMax)));
  model = this->mpModel;
  f32 scaleZ = f32(this->scale.z);
  Warpgn_word(&this->cullSizeFar) = l_one;
  f32 scaleX = f32(this->scale.x), scaleY = f32(this->scale.y);
  Warpgn_view<WarpgnModel>(model)->mBaseScale.z = scaleZ;
  Warpgn_view<WarpgnModel>(model)->mBaseScale.x = scaleX;
  Warpgn_view<WarpgnModel>(model)->mBaseScale.y = scaleY;
  Warpgn_translate(&l_matrix, f32(this->current.pos.x),
                   f32(this->current.pos.y), f32(this->current.pos.z));
  f32 m24 = f32(l_matrix[9]), m2C = f32(l_matrix[11]), m20 = f32(l_matrix[8]),
      m04 = f32(l_matrix[1]), m1C = f32(l_matrix[7]), m00 = f32(l_matrix[0]),
      m10 = f32(l_matrix[4]), m0C = f32(l_matrix[3]), m28 = f32(l_matrix[10]),
      m14 = f32(l_matrix[5]), m18 = f32(l_matrix[6]);
  model = this->mpModel;
  f32 m08 = f32(l_matrix[2]);
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[7] = m1C;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[5] = m14;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[6] = m18;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[3] = m0C;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[11] = m2C;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[10] = m28;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[8] = m20;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[0] = m00;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[9] = m24;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[2] = m08;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[4] = m10;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[1] = m04;
  this->mpEmitter1 =
      Warpgn_particleSet(Warpgn_view<void>(u32(Warpgn_view<WarpgnPlay>(Warpgn_getPlay())->mpParticle)),
                      0, 0x83FD, &this->current.pos, 0, 0, 0xFF, 0, -1, 0, 0, 0);
  this->mpEmitter2 =
      Warpgn_particleSet(Warpgn_view<void>(u32(Warpgn_view<WarpgnPlay>(Warpgn_getPlay())->mpParticle)),
                      0, 0x83FE, &this->current.pos, 0, 0, 0xFF, 0, -1, 0, 0, 0);
  this->mpEmitter3 =
      Warpgn_particleSet(Warpgn_view<void>(u32(Warpgn_view<WarpgnPlay>(Warpgn_getPlay())->mpParticle)),
                      4, 0xC3FC, &this->current.pos, 0, 0, 0xFF, 0, -1, 0, 0, 0);
  u32 p = this->mpEmitter1;
  if (p)
    Warpgn_view<WarpgnEmitter>(p)->mFlags = u32(Warpgn_view<WarpgnEmitter>(p)->mFlags) | 1;
  p = this->mpEmitter2;
  if (p)
    Warpgn_view<WarpgnEmitter>(p)->mFlags = u32(Warpgn_view<WarpgnEmitter>(p)->mFlags) | 1;
  p = this->mpEmitter3;
  if (p)
    Warpgn_view<WarpgnEmitter>(p)->mFlags = u32(Warpgn_view<WarpgnEmitter>(p)->mFlags) | 1;
  this->mEvtToMajyuuWarpIdx =
      Warpgn_getEventIndex(&Warpgn_view<WarpgnPlay>(Warpgn_getPlay())->mEventManager,
                      Warpgn_view<void>(gabi::guest_addr("TO_MAJYUU_WARP")), 0xFF);
  u32 appear = Warpgn_getEventIndex(&Warpgn_view<WarpgnPlay>(Warpgn_getPlay())->mEventManager,
                               Warpgn_view<void>(gabi::guest_addr("APPEAR_WARP")), 0xFF);
  u32 parameters = u32(this->mParameters);
  this->mEvtAppearWarpIdx = appear;
  this->mSwitchNo = (parameters >> 8) & 0xFF;
  s32 valid = this->checkValidWarp();
  s8 room = s8(this->current.roomNo);
  u8 scene = u8(this->mParameters);
  if (valid) {
    this->mIsSwitch = 1;
    this->mEvtAppearWarpIdx = -1;
  }
  this->mSceneNo = scene;
  Warpgn_tevInit(&this->mTevStr, room, 0xFF);
}

/* 024D90A0 */
s32 daWarpgn_c::_create() {
  u32 condition = this->actor_condition;
  if (!(condition & fopAcCnd_INIT_e)) {
    if (gabi::ea(this)) {
      daWarpgnConstruct(this);
      condition = this->actor_condition;
    }
    this->actor_condition = condition | fopAcCnd_INIT_e;
  }
  s32 phase = Warpgn_resLoad(&this->mPhs, "Gmjwp");
  if (phase == 4) {
    if (!Warpgn_entryHeap(this, Warpgn_view<void>(gc_tu.mHeapCallback), 0x3000))
      return 5;
    this->CreateInit();
  }
  return phase;
}

/* 024D9E00 */
s32 daWarpgn_c::_execute() {
  if (!this->demoActorID) {
    this->checkOrder();
    this->demo_proc();
    this->eventOrder();
  } else {
    this->m2C4 = WarpgnOrder_NONE;
    this->demo_execute();
  }
  bool sounding = this->m388 != 0;
  if (!sounding) {
    u32 save = l_saveInfo;
    sounding = Warpgn_isEvent(&Warpgn_view<WarpgnSave>(save)->mEvent, 0x3D02) != 0;
  }
  if (sounding) {
    s32 reverb = Warpgn_getReverb(s8(this->current.roomNo));
    Warpgn_soundLevel(0x1089, &this->eyePos, 0, reverb);
  }
  u32 model = this->mpModel;
  f32 z = f32(this->scale.z), x = f32(this->scale.x), y = f32(this->scale.y);
  Warpgn_view<WarpgnModel>(model)->mBaseScale.x = x;
  Warpgn_view<WarpgnModel>(model)->mBaseScale.z = z;
  Warpgn_view<WarpgnModel>(model)->mBaseScale.y = y;
  f32 px = this->current.pos.x, py = this->current.pos.y, pz = this->current.pos.z;
  Warpgn_translate(&l_matrix, px, py, pz);
  f32 a3 = f32(l_matrix[3]), a1 = f32(l_matrix[1]), a4 = f32(l_matrix[4]), a7 = f32(l_matrix[7]),
      a10 = f32(l_matrix[10]), a5 = f32(l_matrix[5]), a2 = f32(l_matrix[2]),
      a8 = f32(l_matrix[8]), a0 = f32(l_matrix[0]);
  model = this->mpModel;
  f32 a9 = f32(l_matrix[9]), a11 = f32(l_matrix[11]), a6 = f32(l_matrix[6]);
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[11] = a11;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[8] = a8;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[0] = a0;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[5] = a5;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[1] = a1;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[4] = a4;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[6] = a6;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[9] = a9;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[10] = a10;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[3] = a3;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[7] = a7;
  Warpgn_view<WarpgnModel>(model)->mBaseTRMtx[2] = a2;
  return 1;
}

/* 024D9880 */
s32 daWarpgn_c::normal_execute() {
  s8 room = this->home.roomNo;
  u32 save = l_saveInfo, bit = this->mSwitchNo;
  s32 switched = Warpgn_isSwitch(&Warpgn_view<WarpgnSave>(save)->mZone, bit, room);
  save = l_saveInfo;
  if (!Warpgn_isEvent(&Warpgn_view<WarpgnSave>(save)->mEvent, 0x3D02)) {
    if (switched && !this->mIsSwitch)
      this->m2C4 = WarpgnOrder_APPEAR;
  } else {
    this->anim_play(WarpgnAnim_OPEN);
    resumeEmitter(this->mpEmitter1);
    resumeEmitter(this->mpEmitter2);
    resumeEmitter(this->mpEmitter3);
  }
  if (this->check_warp()) {
    u32 play = Warpgn_getPlay();
    if (!u8(Warpgn_view<WarpgnPlay>(play)->mEventRunning))
      this->m2C4 = WarpgnOrder_WARP;
  }
  this->mIsSwitch = u8(switched);
  return 1;
}

/* 024D9D3C */
s32 daWarpgn_c::demo_execute() {
  u8 id = this->demoActorID;
  if (id && id <= 0x20) {
    u32 object = l_demoObject;
    if (!object) {
      Warpgn_assertion(Warpgn_view<const char>(gc_tu.mDemoSourceName), 0x23A, Warpgn_view<const char>(gc_tu.mDemoAssertion));
      object = l_demoObject;
    }
    u32 demo = Warpgn_getDemoActor(Warpgn_view<void>(object), id);
    if (demo) {
      u32 shape = u32(Warpgn_view<WarpgnDemoActor>(demo)->mShapeId);
      this->mDemoShape = shape;
      if (shape == 0)
        this->anim_play(WarpgnAnim_WAIT);
      else if (shape == 1)
        this->anim_play(WarpgnAnim_APPEAR);
    }
  }
  return 1;
}

/* 024D9AE0 */
void daWarpgn_c::demo_proc() {
  void* manager = eventManager();
  this->mStaffId = Warpgn_getStaffId(manager, "Warpgn", 0, 0);
  u32 play = Warpgn_getPlay();
  if (!u8(Warpgn_view<WarpgnPlay>(play)->mEventRunning) || u16(this->eventInfo.mCommand) == 1)
    return;
  s32 staff = this->mStaffId;
  if (staff == -1)
    return;
  manager = eventManager();
  s32 action = Warpgn_getAction(manager, staff,
                               &action_table, 6, 0, 0);
  staff = this->mStaffId;
  if (action == -1) {
    manager = eventManager();
    Warpgn_cutEnd(manager, staff);
    return;
  }
  manager = eventManager();
  if (Warpgn_getIsAdvance(manager, staff))
    Warpgn_eventMethod(this, event_init_tbl[u32(action)], this->mStaffId);
  if (Warpgn_eventMethod(this, event_action_tbl[u32(action)], this->mStaffId)) {
    staff = this->mStaffId;
    manager = eventManager();
    Warpgn_cutEnd(manager, staff);
  }
}

/* 024DA00C */
s32 daWarpgn_c::actWait() {
  this->anim_play(WarpgnAnim_WAIT);
  return 1;
}

/* 024DA160 */
void daWarpgn_c::initStartWarp() {
  u32 play = Warpgn_getPlay();
  Warpgn_setGoal(&Warpgn_view<WarpgnPlay>(play)->mEventManager, &this->current.pos);
  u32 zero = l_zero, p = this->mpBtkAnm1;
  if (p)
    Warpgn_view<WarpgnAnimation>(p)->mFrame = zero;
  p = this->mpBtkAnm2;
  if (p)
    Warpgn_view<WarpgnAnimation>(p)->mFrame = zero;
  resumeEmitter(this->mpEmitter1);
  resumeEmitter(this->mpEmitter2);
  resumeEmitter(this->mpEmitter3);
}

/* 024DA208 */
s32 daWarpgn_c::actStartWarp() {
  this->anim_play(WarpgnAnim_WAIT);
  return 1;
}

// HD: weak sound stub has no receiver or staff arguments in the verified ABI.
/* 024DA034 */
u32 daWarpgnInitWarp() {
  return Warpgn_soundStart(0x2893);
}

/* 024DA03C */
s32 daWarpgn_c::actWarp() {
  this->anim_play(WarpgnAnim_WAIT);
  return 1;
}

/* 024DA064 */
void daWarpgn_c::initWarpArrive() {
  this->set_end_anim();
  resumeEmitter(this->mpEmitter1);
  resumeEmitter(this->mpEmitter2);
  resumeEmitter(this->mpEmitter3);
  Warpgn_soundStart(0x2892);
}

/* 024DA0D4 */
s32 daWarpgn_c::actWarpArrive() {
  this->anim_play(WarpgnAnim_WAIT);
  return 1;
}

/* 024DA0FC */
s32 daWarpgn_c::actWarpArriveEnd() {
  this->anim_play(WarpgnAnim_REVERSE);
  u32 animation = this->mpBtkAnm2;
  if (!animation)
    return 0;
  return f32(Warpgn_view<WarpgnAnimation>(animation)->mFrame) < f32(Warpgn_float(&l_arriveFrame)) ? 1 : 0;
}

/* 024DA230 */
void daWarpgn_c::initAppear() {
  this->mTimer = 100;
}

/* 024DA23C */
s32 daWarpgn_c::actAppear() {
  s32 done = 0;
  if (Warpgn_calcTimer(&this->mTimer) == 0) {
    resumeEmitter(this->mpEmitter1);
    resumeEmitter(this->mpEmitter2);
    resumeEmitter(this->mpEmitter3);
    this->mTimer = 100;
    done = 1;
  }
  this->anim_play(WarpgnAnim_APPEAR);
  this->m388 = 1;
  return done;
}

/* 024D9CA4 */
void daWarpgn_c::eventOrder() {
  s32 state = this->m2C4;
  if (state == WarpgnOrder_WARP || state == WarpgnOrder_APPEAR) {
    s16 event = state == WarpgnOrder_WARP ? s16(this->mEvtToMajyuuWarpIdx) : s16(this->mEvtAppearWarpIdx);
    Warpgn_orderEvent(this, event, 0xFF, 0xFFFF, 0, 1);
    this->eventInfo.mCondition = u16(this->eventInfo.mCondition) | dEvtCnd_UNK2_e;
  }
}

/* 024D999C */
void daWarpgn_c::checkOrder() {
  if (u16(this->eventInfo.mCommand) == 2) {
    s16 event = this->mEvtToMajyuuWarpIdx;
    void* manager = eventManager();
    if (Warpgn_startCheck(manager, event) && this->m2C4)
      this->m2C4 = WarpgnOrder_NONE;
    event = this->mEvtAppearWarpIdx;
    manager = eventManager();
    if (Warpgn_startCheck(manager, event) && this->m2C4)
      this->m2C4 = WarpgnOrder_NONE;
    event = this->mEvtToMajyuuWarpIdx;
    manager = eventManager();
    if (Warpgn_endCheck(manager, event)) {
      s8 room = this->current.roomNo;
      u8 scene = u8(this->mSceneNo);
      Warpgn_nextStage(scene, room);
    }
    event = this->mEvtAppearWarpIdx;
    manager = eventManager();
    if (Warpgn_endCheck(manager, event)) {
      u32 save = l_saveInfo;
      Warpgn_onEvent(&Warpgn_view<WarpgnSave>(save)->mEvent, 0x3D02);
      this->mEvtAppearWarpIdx = -1;
      u32 play = Warpgn_getPlay();
      Warpgn_view<WarpgnPlay>(play)->mEventFlags = u16(Warpgn_view<WarpgnPlay>(play)->mEventFlags) | 8;
    }
  } else if (!this->m2C4)
    this->normal_execute();
}

/* 024D9520 */
void daWarpgn_c::anim_play(s32 mode) {
  u32 speed = l_one;
  if (mode == WarpgnAnim_WAIT) {
    playAnimation(this->mpBtkAnm1, speed);
    playAnimation(this->mpBtkAnm2, speed);
  } else if (mode == WarpgnAnim_APPEAR) {
    playAnimation(this->mpBtkAnm1, speed);
    playAnimation(this->mpBckAnm, speed);
    playAnimation(this->mpBrkAnm, speed);
  } else if (mode == WarpgnAnim_OPEN) {
    playAnimation(this->mpBtkAnm1, speed);
    endAnimation(this->mpBrkAnm);
    endAnimation(this->mpBckAnm);
  } else if (mode == WarpgnAnim_FIRST)
    playAnimation(this->mpBtkAnm1, speed);
  else if (mode == WarpgnAnim_REVERSE) {
    playAnimation(this->mpBtkAnm1, speed);
    u32 p = this->mpBtkAnm2;
    if (p) {
      Warpgn_view<WarpgnAnimation>(p)->mRate = l_reverse;
      p = this->mpBtkAnm2;
      Warpgn_animationPlay(Warpgn_view<void>(p));
    }
  }
}

/* 024D9F58 */
void daWarpgn_c::set_end_anim() {
  endAnimation(this->mpBtkAnm2);
  endAnimation(this->mpBrkAnm);
  endAnimation(this->mpBckAnm);
}

/* 024D9760 */
s32 daWarpgn_c::check_warp() {
  struct Frame {
    u8 prefix[8];
    cXyz playerXZ, shipXZ, playerDelta, shipDelta;
    u8 saved[24];
  };
  static_assert(sizeof(Frame) == 0x50);
  gabi::Local<Frame> frame;
  gabi::Local<be<u32>[4]> linkage;
  u32 play = Warpgn_getPlay(), player = u32(Warpgn_view<WarpgnPlay>(play)->mpPlayer);
  Warpgn_vecSubtract(&Warpgn_view<fopAc_ac_c>(player)->current.pos, &frame->playerDelta,
                   &this->current.pos);
  f32 zero = f32(Warpgn_float(&l_zero));
  frame->playerXZ.x = f32(frame->playerDelta.x);
  frame->playerXZ.y = zero;
  frame->playerXZ.z = f32(frame->playerDelta.z);
  f32 length = Warpgn_squareMagnitude(&frame->playerXZ);
  Warpgn_squareRoot(length);
  if (!this->checkValidWarp())
    return 0;
  play = Warpgn_getPlay();
  if (!(u32(Warpgn_view<WarpgnPlay>(play)->mPlayerStatus0) & 0x10000))
    return 0;
  play = Warpgn_getPlay();
  if (!u32(Warpgn_view<WarpgnPlay>(play)->mpShip))
    return 0;
  play = Warpgn_getPlay();
  u32 ship = u32(Warpgn_view<WarpgnPlay>(play)->mpShip);
  Warpgn_vecSubtract(&Warpgn_view<fopAc_ac_c>(ship)->current.pos, &frame->shipDelta,
                   &this->current.pos);
  f32 x = f32(frame->shipDelta.x), z = f32(frame->shipDelta.z);
  frame->shipXZ.x = x;
  frame->shipXZ.y = zero;
  frame->shipXZ.z = z;
  length = Warpgn_squareMagnitude(&frame->shipXZ);
  f32 distance = Warpgn_squareRoot(length);
  return distance < f32(Warpgn_float(&l_warpDistance)) ? 1 : 0;
}

/* 024D8D6C */
s32 daWarpgn_c::checkValidWarp() {
  s8 room = this->home.roomNo;
  u32 save = l_saveInfo, bit = this->mSwitchNo;
  if (Warpgn_isSwitch(&Warpgn_view<WarpgnSave>(save)->mZone, bit, room))
    return 1;
  save = l_saveInfo;
  return Warpgn_isEvent(&Warpgn_view<WarpgnSave>(save)->mEvent, 0x3D02) != 0 ? 1 : 0;
}

/* 024D92B4 */
s32 daWarpgn_c::_draw() {
  struct ColorFrame {
    u8 prefix[0x18];
    be<u32> rgb[4];
    be<f32> rgba[4];
    u8 tail[0x28];
  };
  static_assert(sizeof(ColorFrame) == 0x60);
  static_assert(offsetof(ColorFrame, rgb) == 0x18);
  static_assert(offsetof(ColorFrame, rgba) == 0x28);
  // HD: output RGB is kept as words and reloaded as floats at the original points.
  gabi::Local<ColorFrame> frame;
  gabi::Local<be<u32>[4]> outgoing;
  u32 environment = Warpgn_getEnvLight();
  Warpgn_settingTev(Warpgn_view<void>(environment), 0, &this->current.pos,
                   &this->tevStr);
  environment = Warpgn_getEnvLight();
  u32 model = this->mpModel;
  Warpgn_setLightTev(Warpgn_view<void>(environment), Warpgn_view<void>(model),
                   &this->tevStr);
  environment = Warpgn_getEnvLight();
  Warpgn_settingTev(Warpgn_view<void>(environment), 2, &this->current.pos,
                   &this->mTevStr);
  model = this->mpModel;
  u32 data = u32(Warpgn_view<WarpgnModel>(model)->mpModelData), table = Warpgn_materialTable(Warpgn_view<void>(data));
  u16 index = 0;
  if (index < u16(Warpgn_view<WarpgnMaterialTable>(table)->mMaterialNum)) {
    f32 intensity = f32(Warpgn_float(&l_one));
    do {
      u32 count = u32(Warpgn_view<WarpgnModelData>(data)->mMaterialCount), material = u32(Warpgn_view<WarpgnModelData>(data)->mpMaterials);
      if (index < count)
        material += u32(index) * 0x39C;
      u32 color = materialColor(material);
      Warpgn_view<WarpgnColor>(color)->r = u8(this->mTevStr.mColorK0.r);
      color = materialColor(material);
      Warpgn_view<WarpgnColor>(color)->g = u8(this->mTevStr.mColorK0.g);
      color = materialColor(material);
      Warpgn_view<WarpgnColor>(color)->b = u8(this->mTevStr.mColorK0.b);
      color = materialColor(material);
      u32 block = u32(Warpgn_view<WarpgnMaterial>(material)->mpTevBlock), vtable = u32(Warpgn_view<WarpgnTevBlock>(block)->mVtable);
      gabi::call_ptr<void>(u32(Warpgn_view<WarpgnTevVtable>(vtable)->mSetColor), Warpgn_view<void>(block), 1,
                           Warpgn_view<void>(color));
      // HD: propagate the colour into the extra floating material attribute.
      Warpgn_colorConvert(&frame->rgba, Warpgn_view<void>(color));
      Warpgn_scaleColor(&frame->rgb, &frame->rgba, intensity);
      Warpgn_view<WarpgnMaterial>(material)->mAttribute = u32(Warpgn_view<WarpgnMaterial>(material)->mAttribute) | 0x100;
      u32 output = Warpgn_materialAttribute(&Warpgn_view<WarpgnMaterial>(material)->mAttribute, 8);
      f32 alpha = f32(u8(Warpgn_view<WarpgnColor>(color)->a)) / f32(Warpgn_float(&l_colorDivisor));
      f32 red = f32(Warpgn_float(&frame->rgb[0])), green = f32(Warpgn_float(&frame->rgb[1])),
          blue = f32(Warpgn_float(&frame->rgb[2]));
      Warpgn_view<WarpgnColor4f>(output)->g = green;
      Warpgn_view<WarpgnColor4f>(output)->b = blue;
      Warpgn_view<WarpgnColor4f>(output)->r = red;
      index = u16(index + 1);
      Warpgn_view<WarpgnColor4f>(output)->a = alpha;
      table = Warpgn_materialTable(Warpgn_view<void>(data));
    } while (index < u16(Warpgn_view<WarpgnMaterialTable>(table)->mMaterialNum));
  }
  u32 animation = this->mpBtkAnm1;
  if (animation) {
    f32 time = f32(Warpgn_view<WarpgnAnimation>(animation)->mFrame);
    model = this->mpModel;
    Warpgn_btkEntry(Warpgn_view<void>(animation), Warpgn_view<void>(model), time);
  }
  animation = this->mpBtkAnm2;
  if (animation) {
    f32 time = f32(Warpgn_view<WarpgnAnimation>(animation)->mFrame);
    model = this->mpModel;
    Warpgn_btkEntry(Warpgn_view<void>(animation), Warpgn_view<void>(model), time);
  }
  animation = this->mpBrkAnm;
  if (animation) {
    model = this->mpModel;
    f32 time = f32(Warpgn_view<WarpgnAnimation>(animation)->mFrame);
    Warpgn_brkEntry(Warpgn_view<void>(animation), Warpgn_view<void>(u32(Warpgn_view<WarpgnModel>(model)->mpModelData)),
                     time);
  }
  animation = this->mpBckAnm;
  if (animation) {
    model = this->mpModel;
    f32 time = f32(Warpgn_view<WarpgnAnimation>(animation)->mFrame);
    Warpgn_bckEntry(Warpgn_view<void>(animation), Warpgn_view<void>(u32(Warpgn_view<WarpgnModel>(model)->mpModelData)),
                     time);
  }
  Warpgn_modelUpdate(Warpgn_view<void>(u32(this->mpModel)), 0);
  return 1;
}

/* 024D9158 */
s32 daWarpgn_Create(daWarpgn_c *actor) {
  return actor->_create();
}

/* 024D91FC */
s32 daWarpgn_Delete(daWarpgn_c *actor) {
  return actor->_delete();
}

/* 024D951C */
s32 daWarpgn_Draw(daWarpgn_c *actor) {
  return actor->_draw();
}

/* 024D9F54 */
s32 daWarpgn_Execute(daWarpgn_c *actor) {
  return actor->_execute();
}

/* 024DA394 */
s32 daWarpgn_IsDelete() {
  return 1;
}

/* 024DA39C */
void Warpgn_globalDestructor1() {
}

/* 024DA3A0 */
void Warpgn_globalDestructor2() {
}

/* 024DA3F8 */
void Warpgn_emptyVirtual() {
}

/* 024DA3A4 */
void Warpgn_actorDestructor(daWarpgn_c *actor, s32 flags) {
  if (actor) {
    Warpgn_actorDestruct(actor, 0);
    if (flags & 1)
      Warpgn_deallocate(actor);
  }
}

/* 024DA380 */
void Warpgn_emptyDestructor(void *object, s32 flags) {
  if (object && (flags & 1))
    Warpgn_deallocate(object);
}

/* 024DA2EC */
void Warpgn_staticInit() {
  l_headerWord2 = 0;
  l_headerWord0 = 0;
  l_headerWord3 = 0;
  l_headerWord1 = 0;
  Warpgn_registerGlobal(Warpgn_view<void>(gc_tu.mHeaderRegistration0));
  u32 lo = l_headerLow, hi = l_headerHigh;
  l_headerValueLow = lo;
  l_headerValueHigh = hi;
  Warpgn_constructHeader(Warpgn_view<void>(gc_tu.mHeaderGuard));
  Warpgn_registerGlobal(Warpgn_view<void>(gc_tu.mHeaderRegistration1));
  Warpgn_constructByte(Warpgn_view<void>(gc_tu.mHeaderByte));
  Warpgn_registerGlobal(Warpgn_view<void>(gc_tu.mHeaderRegistration2));
}

// HD: normalize byte colours for the added floating material-colour path.
/* 024D9200 */
void Warpgn_colorConvert(void *destination, void *source) {
  u32 src = gabi::ea(source), dst = gabi::ea(destination);
  u8 r = u8(Warpgn_view<WarpgnColor>(src)->r), g = u8(Warpgn_view<WarpgnColor>(src)->g), b = u8(Warpgn_view<WarpgnColor>(src)->b), a = u8(Warpgn_view<WarpgnColor>(src)->a);
  f32 divisor = f32(Warpgn_float(&l_colorDivisor));
  f32 rf = f32(r) / divisor, gf = f32(g) / divisor, bf = f32(b) / divisor, af = f32(a) / divisor;
  Warpgn_view<WarpgnColor4f>(dst)->r = rf;
  Warpgn_view<WarpgnColor4f>(dst)->g = gf;
  Warpgn_view<WarpgnColor4f>(dst)->b = bf;
  Warpgn_view<WarpgnColor4f>(dst)->a = af;
}

/* WWHD glue
data action_table 101D309C
data event_init_tbl 101D301C nocheck
data event_action_tbl 101D304C nocheck
data l_one 10042544
data l_zero 10042548
data l_colorDivisor 100425BC
data l_reverse 100425D0
data l_warpDistance 100425D4
data l_arriveFrame 10042620
data l_cullMin 10042594
data l_cullMax 10042598
data l_cullHeight 1004259C
data l_headerLow 10042628
data l_headerHigh 1004262C
data l_saveInfo 101F84DC nocheck
data l_resControl 101F4F28 nocheck
data l_demoObject 101D5FFC nocheck
data l_headerWord2 1046EB5C nocheck
data l_headerWord0 1046EB54 nocheck
data l_headerWord3 1046EB60 nocheck
data l_headerWord1 1046EB58 nocheck
data l_headerValueLow 1046EB48 nocheck
data l_headerValueHigh 1046EB4C nocheck
data l_matrix 1048D0CC nocheck
data l_defaultLight 1016E414
str 10042634 "Gmjwp"
str 100425D8 "Warpgn"
str 100425AC "TO_MAJYUU_WARP"
str 100425A0 "APPEAR_WARP"
ext 0201ADE0 void Warpgn_vecSubtract(const void*,void*,const void*)
ext 0211D2F8 s32 Warpgn_calcTimer(void*)
ext 025200D4 u32 Warpgn_getPlay()
ext 02520460 s32 Warpgn_resLoad(void*,const char*)
ext 025204C8 void Warpgn_resDelete(void*,const char*)
ext 02520540 s32 Warpgn_getReverb(s32)
ext 02526E70 u32 Warpgn_getDemoActor(void*,u32)
ext 02542D88 s32 Warpgn_getStaffId(void*,const char*,s32,s32)
ext 02542EDC s32 Warpgn_getAction(void*,s32,const void*,s32,s32,s32)
ext 02543280 void Warpgn_cutEnd(void*,s32)
ext 02543714 void Warpgn_setGoal(void*,const void*)
ext 02543F10 s32 Warpgn_getEventIndex(void*,void*,s32)
ext 0254407C s32 Warpgn_startCheck(void*,s32)
ext 025440C8 s32 Warpgn_endCheck(void*,s32)
ext 025447C8 s32 Warpgn_getIsAdvance(void*,s32)
ext 02555D0C u32 Warpgn_getEnvLight()
ext 0255FFF4 void Warpgn_tevInit(void*,s32,s32)
ext 025626A4 void Warpgn_settingTev(void*,s32,const void*,void*)
ext 02562F5C void Warpgn_setLightTev(void*,void*,void*)
ext 02587EFC void Warpgn_nextStage(u32,s32)
ext 025A847C u32 Warpgn_particleSet(void*,s32,u32,const void*,s32,s32,s32,s32,s32,s32,s32,s32)
ext 025B8B68 void Warpgn_onEvent(void*,u32)
ext 025B8B94 s32 Warpgn_isEvent(void*,u32)
ext 025BA0C0 s32 Warpgn_isSwitch(void*,u32,s32)
ext 025D4ED0 void Warpgn_actorConstruct(void*)
ext 025D50BC void Warpgn_actorDestruct(void*,s32)
ext 025D63E8 s32 Warpgn_entryHeap(void*,void*,u32)
ext 025D674C void Warpgn_cullBox(void*,f32,f32,f32,f32,f32,f32)
ext 025D7A58 s32 Warpgn_orderEvent(void*,s32,s32,s32,s32,s32)
ext 025E1988 u32 Warpgn_soundStart(u32)
ext 025E1A40 void Warpgn_soundLevel(u32,const void*,s32,s32)
ext 025E2DE0 void Warpgn_modelUpdate(void*,s32)
ext 025E38E0 u32 Warpgn_modelCreate(void*,u32,u32)
ext 025E742C s32 Warpgn_animationPlay(void*)
ext 025E7C6C u32 Warpgn_btkConstruct(s32)
ext 025E7CE0 s32 Warpgn_btkInit(void*,void*,void*,s32,s32,s32,s32,s32,f32,s32)
ext 025E8048 void Warpgn_btkEntry(void*,void*,f32)
ext 025E80D0 u32 Warpgn_brkConstruct(s32)
ext 025E8154 s32 Warpgn_brkInit(void*,void*,void*,s32,s32,s32,s32,s32,f32,s32)
ext 025E83FC void Warpgn_brkEntry(void*,void*,f32)
ext 025E8508 s32 Warpgn_bckInit(void*,void*,void*,s32,s32,s32,s32,s32,f32)
ext 025E86B8 void Warpgn_bckEntry(void*,void*,f32)
ext 026066C4 u32 Warpgn_getResource(void*,void*,s32)
ext 0273AA24 void Warpgn_assertion(const void*,s32,const void*)
ext 0273AD10 u32 Warpgn_allocate(u32)
ext 0273AF40 void Warpgn_deallocate(void*)
ext 0274D458 void Warpgn_scaleColor(void*,const void*,f32)
ext 027DA984 void Warpgn_bckMatrixConstruct(void*)
ext 027F2BC0 void Warpgn_frameConstruct(void*,s32)
ext 027F3F8C u32 Warpgn_materialTable(void*)
ext 027F9F0C u32 Warpgn_materialAttribute(void*,s32)
ext 028E8DD0 f32 Warpgn_squareMagnitude(const void*)
ext 028E93CC void Warpgn_translate(void*,f32,f32,f32)
ext 028EAB2C void Warpgn_constructByte(void*)
ext 028ED6F8 void Warpgn_constructHeader(void*)
ext 028F026C void Warpgn_registerGlobal(void*)
ext 028F4384 f32 Warpgn_squareRoot(f32)
*/
