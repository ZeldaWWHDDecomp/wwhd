/* Collectable Deku Leaf; derived from WWHD disassembly. */
#include "d/actor/d_a_deku_item.h"
using namespace DekuItem;
static u32 resource(gabi::Local<ResourceLocal> &local, u32 id) {
  local->name.text = ArcName;
  local->name.vtable = SafeStringVtable;
  return sdk::getObjectResource<u32>(view<void>(resourceController()),
                                     &local->name, id);
}
static void assertion(u32 line, u32 condition) {
  sdk::assertion(view<void>(AssertFile), line, view<void>(condition));
}
/* 0211E6F0 */
BOOL daDekuItem_c::CreateHeap() {
  gabi::Local<ResourceLocal> modelName, firstName, secondName;
  u32 data = resource(modelName, LeafModel);
  if (!data)
    assertion(0xf4, ModelAssertion);
  u32 model = sdk::createModel<u32>(view<void>(data), 0, 0x11020203);
  mpModel = modelView(model);
  if (!model)
    return 0;
  u32 animation = resource(firstName, LeafOpenAnimation);
  if (!animation)
    assertion(0x103, AnimationAssertion);
  if (!sdk::initBck<s32>(&mBck1, view<void>(data), view<void>(animation), 1, 2,
                         1.0f, 0, -1, 0))
    return 0;
  animation = resource(secondName, LeafIdleAnimation);
  if (!animation)
    assertion(0x110, AnimationAssertion);
  return sdk::initBck<s32>(&mBck2, view<void>(data), view<void>(animation), 1,
                           0, 1.0f, 0, -1, 0) != 0;
}
/* 0211E890 */
static BOOL CheckCreateHeap(daDekuItem_c *a) { return a->CreateHeap(); }
/* 0211E894 */
void daDekuItem_c::set_mtx() {
  f32 x = scale.x, y = scale.y;
  u32 model = gabi::ea((void *)mpModel);
  f32 z = scale.z;
  modelView(model)->scale.x = x;
  modelView(model)->scale.y = y;
  modelView(model)->scale.z = z;
  x = current.pos.x;
  y = current.pos.y;
  z = current.pos.z;
  sdk::translateMatrix(matrixStack(), x, y, z);
  sdk::rotateYMatrix(matrixStack(), (s16)current.angle.y);
  f32 matrix[12];
  for (u32 i = 0; i < 12; ++i)
    matrix[i] = matrixStack()[i];
  model = gabi::ea((void *)mpModel);
  for (u32 i = 0; i < 12; ++i)
    modelView(model)->matrix[i] = matrix[i];
}
/* 0211E96C */
void daDekuItem_c::CreateInit() {
  u32 model = gabi::ea((void *)mpModel);
  cullMtx = model ? gabi::ea(&modelView(model)->matrix) : 0;
  sdk::setCullBox(this, -50.0f, 0.0f, -50.0f, 50.0f, 100.0f, 50.0f);
  sdk::initStatus(&mStts, 0xff, 0xff, this);
  sdk::setCylinder(&mCyl, view<void>(CylinderSource));
  mCyl.status = &mStts;
  sdk::setWall(&mAcchCir, 50.0f, 50.0f);
  sdk::setAcch(&mAcch, &current.pos, &old.pos, this, 1, &mAcchCir, &speed,
               (void *)nullptr, (void *)nullptr);
  set_mtx();
  mMode = Mode_WAIT_e;
  gravity = -3.0f;
}
static void constructAnimation(BckAnm *animation) {
  sdk::initFrameCtrl(animation, 0);
  animation->transformVtable = TransformVtable;
  sdk::constructTransform(animation->transform);
  animation->materialVtable = MaterialVtable;
  animation->weight = 0;
  animation->oldFrame = 0;
  animation->callback = 0;
  animation->transformVtable = BckVtable;
  animation->model = 0;
  animation->animation = 0;
}
/* 0211EA7C */
s32 daDekuItem_c::_create() {
  u32 condition = actor_condition;
  if (!(condition & 8)) {
    if (gabi::ea(this)) {
      sdk::constructActor(this);
      __vtbl = ActorVtable;
      constructAnimation(&mBck1);
      constructAnimation(&mBck2);
      sdk::constructAcch(&mAcch);
      mAcch.vtable = AcchVtable;
      mAcch.polygonVtable = PolygonVtable;
      mAcch.checkVtable = CheckVtable;
      mAcch.object = 1;
      sdk::constructAcchCir(&mAcchCir);
      sdk::constructStatus(&mStts);
      sdk::constructGlobalStatus(&mStts.globalVtable);
      mStts.vtable = StatusVtable;
      mStts.globalVtable = GlobalStatusVtable;
      sdk::constructCylinder(&mCyl);
      mCyl.shapeVtable = ShapeBaseVtable;
      mCyl.boxVtable = BoxVtable;
      sdk::constructGeometry(&mCyl.center);
      condition = actor_condition;
      mCyl.shapeVtable = CylinderShapeVtable;
      mCyl.hitVtable = CylinderHitVtable;
      mCyl.geometryVtable = CylinderGeometryVtable;
    }
    actor_condition = condition | 8;
  }
  u32 bit = (u8)(u32)mParameters;
  s32 room = home.roomNo;
  mItemBitNo = bit;
  if (sdk::isItem<s32>(saveView()->memory, bit, room))
    return 5;
  if (!sdk::isEventBit<s32>(saveView()->event, 0x1801))
    return 5;
  s32 phase = sdk::loadResource<s32>(&mPhs, view<void>(ArcName));
  if (phase == 4) {
    if (!sdk::entrySolidHeap<s32>(this, view<void>(HeapCallback), 0x5000))
      return 5;
    CreateInit();
  }
  return phase;
}
/* 0211ECA8 */
static s32 daDekuItem_Create(daDekuItem_c *a) { return a->_create(); }
static void invalidateEmitter(daDekuItem_c *a) {
  u32 emitter = gabi::ea((void *)a->mpEmitter);
  if (emitter) {
    u32 flags = view<Emitter>(emitter)->flags;
    view<Emitter>(emitter)->userWork = 0xffffffff;
    view<Emitter>(emitter)->flags = flags | 1;
    a->mpEmitter = nullptr;
  }
}
/* 0211ECAC */
BOOL daDekuItem_c::_delete() {
  invalidateEmitter(this);
  sdk::deleteResource(&mPhs, view<void>(ArcName));
  return 1;
}
/* 0211ED04 */
static BOOL daDekuItem_Delete(daDekuItem_c *a) { return a->_delete(); }
/* 0211ED08 */
BOOL daDekuItem_c::_draw() {
  u32 env = sdk::environment<u32>();
  sdk::settingTevStruct(view<void>(env), 0, &current.pos, &tevStr);
  env = sdk::environment<u32>();
  u32 model = gabi::ea((void *)mpModel);
  sdk::setLightTevColorType(view<void>(env), view<void>(model), &tevStr);
  model = gabi::ea((void *)mpModel);
  f32 frame = mBck1.mFrameCtrl.mFrame;
  u32 data = modelView(model)->data;
  sdk::entryBck(&mBck1, view<void>(data), frame);
  sdk::modelUpdateDL((void *)mpModel, 0);
  return 1;
}
/* 0211ED78 */
static BOOL daDekuItem_Draw(daDekuItem_c *a) { return a->_draw(); }
/* 0211ED7C */
void daDekuItem_c::checkOrder() {
  if (eventInfo.mCommand != dEvtCmd_INDEMO_e)
    return;
  u32 play = sdk::play<u32>();
  if (sdk::eventStartCheck<s32>(playView(play)->eventManager,
                                view<void>(EventCheckName)) &&
      unk630)
    unk630 = 0;
  play = sdk::play<u32>();
  if (sdk::eventEndCheck<s32>(playView(play)->eventManager,
                              view<void>(EventCheckName))) {
    play = sdk::play<u32>();
    playView(play)->eventFlag = (u16)(playView(play)->eventFlag | 8);
    sdk::deleteActor(this);
    s32 room = home.roomNo;
    Save *save = saveView();
    u32 bit = mItemBitNo;
    sdk::onItem(save->memory, bit, room);
  }
}
/* 0211EE34 */
void daDekuItem_c::mode_proc_call() {
  ModeProc *entry = &modeTable()[(u32)mMode];
  s16 slot = entry->slot, adjust = entry->adjustment;
  void *object = view<void>(gabi::ea(this) + (s32)adjust);
  u32 target;
  if (slot < 0)
    target = entry->function;
  else
    target =
        tableFunction(gabi::ea(object), entry->virtualCall.vtableOffset, slot);
  invokeMode(target, object);
}
/* 0211EE80 */
void daDekuItem_c::eventOrder() {
  if (unk630 == 1) {
    sdk::orderOtherEvent(this, view<void>(EventOrderName), 1, 0xffff);
    eventInfo.mCondition = (u16)(eventInfo.mCondition | dEvtCnd_UNK2_e);
  }
}
/* 0211EEDC */
BOOL daDekuItem_c::_execute() {
  eyePos.copy(current.pos);
  sdk::moveActor(this, &mStts);
  u32 play = sdk::play<u32>();
  sdk::correctPosition(&mAcch, playView(play)->background);
  checkOrder();
  mode_proc_call();
  eventOrder();
  set_mtx();
  mBck1.mFrameCtrl.mFrame = (f32)(s16)mBck1.mFrameCtrl.mEnd;
  return 1;
}
/* 0211EF84 */
static BOOL daDekuItem_Execute(daDekuItem_c *a) { return a->_execute(); }
/* 0211EF88 */
void daDekuItem_c::mode_wait() {
  s32 hit = sdk::checkCoHit<s32>(&mCyl);
  u32 emitter = gabi::ea((void *)mpEmitter);
  if (hit)
    mMode = Mode_GETDEMO_INIT_e;
  if (!emitter) {
    u32 play = sdk::play<u32>();
    u32 particles = playView(play)->particles;
    emitter = sdk::setParticle<u32>(
        view<void>(particles), 0, LeafShine, &current.pos, (void *)nullptr,
        (void *)nullptr, 0xff, (void *)nullptr, -1, (void *)nullptr,
        (void *)nullptr, (void *)nullptr);
    mpEmitter = view<Emitter>(emitter);
  } else {
    f32 y = current.pos.y;
    u8 kind = view<Emitter>(emitter)->kind;
    f32 x = current.pos.x, z = current.pos.z;
    view<Emitter>(emitter)->translation.x = x;
    view<Emitter>(emitter)->translation.y = y;
    view<Emitter>(emitter)->translation.z = z;
    // HD: emitter kinds seven and above use an inverted vertical coordinate.
    if (kind >= 7)
      view<Emitter>(emitter)->translation.y =
          -(f32)view<Emitter>(emitter)->translation.y;
  }
  s32 reverb = sdk::getReverb<s32>((s32)current.roomNo);
  sdk::startSound(LeafSound, &eyePos, 0, reverb);
  sdk::setCylinderCenter(&mCyl.center, &current.pos);
  u32 play = sdk::play<u32>();
  sdk::registerCollision(playView(play)->collision, &mCyl);
}
/* 0211F0CC */
void daDekuItem_c::mode_getdemo_init() {
  sdk::offDraw(&draw_tag);
  invalidateEmitter(this);
  mMode = Mode_GETDEMO_WAIT_e;
  unk630 = 1;
}
/* 0211F134 */
void daDekuItem_c::mode_getdemo_wait() {
  if (!unk630) {
    s32 room = current.roomNo;
    u32 pid = sdk::createDemoItem<u32>(&current.pos, LeafItem, -1, room,
                                       (void *)nullptr, (void *)nullptr);
    mItemPID = pid;
    if (pid != 0xffffffff) {
      u32 play = sdk::play<u32>();
      playView(play)->itemPartner = pid;
    }
    mMode = Mode_GETDEMO_e;
  }
}
/* 0211F1B0 */
static void sinit() {
  constants()->zeroC = 0;
  constants()->zeroA = 0;
  constants()->zeroD = 0;
  constants()->zeroB = 0;
  sdk::registerDestructor(view<void>(AngleDestructor));
  constants()->minimum = -3.1415927410125732f;
  constants()->maximum = 3.1415927410125732f;
  sdk::constructNullByte(&constants()->nullByte);
  sdk::registerDestructor(view<void>(NullDestructor));
  sdk::constructOneByte(&constants()->oneByte);
  sdk::registerDestructor(view<void>(OneDestructor));
}
/* 0211F244 */
static void static_destructor(void *p, u32 flags) {
  if (p && (flags & 1))
    sdk::deleteObject(p);
}
/* 0211F258 */
static BOOL daDekuItem_IsDelete(daDekuItem_c *a) { return 1; }
/* 0211F260 */
void daDekuItem_c::mode_getdemo() {}
/* 0211F264 */
void daDekuItem_c::destroy(u32 flags) {
  if (gabi::ea(this)) {
    sdk::destroyCylinder(&mCyl, 2);
    sdk::destroyStatus(&mStts, 2);
    sdk::destroyCircle(&mAcchCir.circle, 2);
    mAcch.polygonVtable = PolygonVtable;
    mAcch.checkVtable = CheckVtable;
    sdk::destroyAcch(&mAcch, 0);
    sdk::destroyTransform(&mBck2.transformVtable, 0);
    sdk::destroyTransform(&mBck1.transformVtable, 0);
    sdk::destroyActor(this, 0);
    if (flags & 1)
      sdk::deleteObject(this);
  }
}
// This TU's SafeString virtual at vtable1000D574+14; actualresourcecallee invokes it.
/* 0211F318 */
static void SafeString_assureTermination(void *name) {}

/* WWHD glue (tools/verify/autoglue.py): unit-local SDK/data views live in its header.
*/
