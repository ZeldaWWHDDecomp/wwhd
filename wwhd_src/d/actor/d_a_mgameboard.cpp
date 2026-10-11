/** Squid-Hunt board. Verified against the HD original. */
#include "d/actor/d_a_mgameboard.h"
#include "gc_api/gc_api.h"

const gc_tu_data gc_tu = {.safeString = 0x10014C14};
static MgBoardArchiveHD l_arcName;
static MgBoardArchiveHD l_arcSource;
static MgBoardHeaderStaticsHD l_headerStatics;
static MgBoardHeaderStaticsHD l_staticHeaderStatics;
static MgBoardRegistrationTableHD l_registration;
static MgBoardRegistrationTableHD l_staticRegistration;
static MgBoardGridHD l_boardPosition;
static MgBoardBulletGridHD l_bulletPosition;
static MgBoardShipGridHD l_shipPosition;
using MgPadPtr = gptr<MgBoardPadHD>;
using MgHudPtr = gptr<MgBoardHudManagerHD>;
using MgResourcePtr = gptr<void>;
static MgPadPtr l_pad;
static MgHudPtr l_hud;
static MgResourcePtr l_resourceControl;
static MgBoardHudCommandHD l_startCommand;
static MgBoardHudCommandHD l_hideCommand;

// HD: the native iterator requires its original judge callback address.
static constexpr u32 l_actorJudge = 0x025E121C;
using MgBoardHeapCallback = BOOL (*)(daMgBoard_c *);

J3DModelData *mgBoard_getResource(void *control, MgBoardSafeStringHD *name,
                                  s32 index);
void mgBoard_assert(const char *file, s32 line, const char *assertion);
void mgBoard_setModelMode(J3DModel *model, u32 mode);
s32 mgBoard_initSeaFight(dSeaFightGame_info_c *game, s32 bullets, u32 scenario);
fopAc_ac_c *mgBoard_findActor(u32 judge, be<s16> *type);
s32 mgBoard_getEventIdx(void *manager, const char *name, u32 priority);
void mgBoard_setStickControl(MgBoardStickControlHD *control, s32 a, s32 b,
                             s32 c, s32 d, s32 e, s32 angle, f32 threshold,
                             f32 release);
void mgBoard_initStickControl(MgBoardStickControlHD *control);
s32 mgBoard_resLoad(request_of_phase_process_class *phase,
                    MgBoardArchiveHD *archive);
s32 mgBoard_entryHeap(daMgBoard_c *actor, MgBoardHeapCallback callback,
                      u32 size);
void mgBoard_resDelete(request_of_phase_process_class *phase,
                       MgBoardArchiveHD *archive);
void mgBoard_startInput(MgBoardPadHD *pad, s32 repeat, s32 delay);
u32 mgBoard_playCursorSound(u32 sound);
s32 mgBoard_checkSound(u32 sound);
s32 mgBoard_checkFire(s32 player);
s32 mgBoard_attack(dSeaFightGame_info_c *game, u32 x, u32 y);
void mgBoard_playSound(u32 sound, cXyz *position, u32 parameter);
void mgBoard_startShock(void *vibration, s32 power, s32 flags, cXyz *direction);
void mgBoard_updateHud(MgBoardHudHD *screen, u32 total, u32 score);
void mgBoard_endInput(MgBoardPadHD *pad);
u8 mgBoard_decTimer(be<u8> *timer);
void mgBoard_registerObject(MgBoardRegistrationHD *registration);
void mgBoard_initRandom(be<u8> *state);
void mgBoard_initMath(be<u8> *state);
void mgBoard_deleteStickControl(MgBoardStickControlHD *control, u32 flags);
void mgBoard_deleteActor(fopAc_ac_c *actor, u32 flags);
void mgBoard_setHudCommand(MgBoardHudCommandHD *command,
                           MgBoardHudCommandHD *value);
void mgBoard_ZrotM(Mtx34 *matrix, s32 rotation);

// HD: input and HUD updates use native managers instead of the GC 2D display
// lists.
enum BoardSound {
  SE_FIRE = 0x8A8,
  SE_CURSOR,
  SE_SHIP_SUNK,
  SE_CLEAR,
  SE_FAILURE,
  SE_HIT = 0x69A2,
  SE_MISS
};

/* inlined into CreateHeap */
static J3DModelData *resource(s32 index, s32 assertionLine) {
  MgBoardSafeStringHD name;
  name.mpText = &l_arcName;
  name.mVtable = gc_tu.safeString;
  J3DModelData *data = mgBoard_getResource(l_resourceControl, &name, index);
  if (!data)
    mgBoard_assert("d_a_mgameboard.cpp", assertionLine, "modelData != (0)");
  return data;
}
static void transform(daMgBoard_c *board, J3DModel *model, const cXyz *grid) {
  f32 x = board->current.pos.x, y = board->current.pos.y,
      z = board->current.pos.z;
  if (grid) {
    x = x + grid->x;
    y = y + grid->y;
    z = z + grid->z;
  }
  mDoMtx_stack_c::transS(x, y, z);
  mDoMtx_stack_c::YrotM(board->current.angle.y);
  model->setBaseTRMtx(mDoMtx_stack_c::get());
}
static void markersAndShips(daMgBoard_c *board) {
  board->mMissModelCount = 0;
  board->mHitModelCount = 0;
  for (u32 x = 0; x < 8; ++x)
    for (u32 y = 0; y < 8; ++y) {
      u8 cell = board->mSeaFightGame.mGrid[x][y];
      J3DModel *model = NULL;
      if (cell == SEA_HIT) {
        s32 i = board->mHitModelCount;
        model = board->mpHitModel[i];
        board->mHitModelCount = i + 1;
      } else if (cell == SEA_MISS) {
        s32 i = board->mMissModelCount;
        model = board->mpMissModel[i];
        board->mMissModelCount = i + 1;
      }
      if (model)
        transform(board, model, &l_boardPosition.mPositions[y][x]);
    }
  u32 count = board->mSeaFightGame.mAliveShipNum;
  for (u32 i = 0; i < count; ++i) {
    if (i >= 4)
      continue;
    MgBoardShipHD *ship =
        reinterpret_cast<MgBoardShipHD *>(&board->mSeaFightGame.mShips[i]);
    u8 length = ship->mLength;
    J3DModel *model = length == 2   ? board->mpShip2Model[0].get()
                      : length == 3 ? board->mpShip3Model[0].get()
                      : length == 4 ? board->mpShip4Model[0].get()
                                    : NULL;
    if (!model)
      continue;
    u8 y = ship->mY, x = ship->mX;
    if (y >= 8 || x >= 8)
      continue;
    f32 px = board->current.pos.x + l_boardPosition.mPositions[y][x].x;
    f32 py = board->current.pos.y + l_boardPosition.mPositions[y][x].y;
    f32 pz = board->current.pos.z + l_boardPosition.mPositions[y][x].z;
    mDoMtx_stack_c::transS(px, py, pz);
    mDoMtx_stack_c::YrotM(board->current.angle.y);
    // HD: retain the full signed rotation argument at this call site.
    s32 rotation = (s8)ship->mVertical ? -0x8000 : 0x4000;
    mgBoard_ZrotM(mDoMtx_stack_c::get(), rotation);
    model->setBaseTRMtx(mDoMtx_stack_c::get());
  }
}
/* inlined into MiniGameInit */
void daMgBoard_c::set_mtx() {
  J3DModel *model = mpBoardModel;
  transform(this, model, NULL);
  const cXyz *grid =
      &l_boardPosition.mPositions[(s32)mBoardPosY][(s32)mBoardPosX];
  model = mpCursorModel;
  transform(this, model, grid);
  markersAndShips(this);
}
static void lightModel(daMgBoard_c *board, J3DModel *model) {
  auto light = dKy_getEnvlight();
  setLightTevColorType(light, model, &board->tevStr);
}

/* 021C41B0 */
BOOL daMgBoard_c::CreateHeap() {
  J3DModelData *data = resource(BDL_BOARD, 0xC6);
  mpBoardModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
  if (!mpBoardModel)
    return 0;
  data = resource(BDL_CURSOR, 0xD6);
  mpCursorModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
  if (!mpCursorModel)
    return 0;
  data = resource(BDL_HIT, 0xE6);
  for (u32 i = 0; i < 20; ++i) {
    J3DModel *model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    mpHitModel[i] = model;
    if (!model)
      return 0;
  }
  data = resource(BDL_MISS, 0xF8);
  for (u32 i = 0; i < 32; ++i) {
    J3DModel *model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    mpMissModel[i] = model;
    if (!model)
      return 0;
  }
  for (u32 group = 0; group < 3; ++group) {
    data = resource((s32)group + BDL_SHIP2, group == 0   ? 0x10A
                                            : group == 1 ? 0x11F
                                                         : 0x134);
    for (u32 i = 0; i < 2; ++i) {
      J3DModel *model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
      shipModel(group, i) = model;
      if (!model)
        return 0;
      mgBoard_setModelMode(model, 1);
    }
  }
  return 1;
}

/* 021C44E0 */
static BOOL CheckCreateHeap(daMgBoard_c *board) { return board->CreateHeap(); }

/* 021C44E4 */
void daMgBoard_c::MiniGameInit() {
  mBoardPosY = 0;
  mLastFirePosX = -100;
  mLastFirePosY = -100;
  mBoardPosX = 0;
  mGameFlags = 0;
  mGameResult = 0;
  mgBoard_initSeaFight(&mSeaFightGame, 24, 3);
  MgBoardJudgeTypeHD npcType;
  npcType.mType = 0x16A;
  fopAc_ac_c *npc = mgBoard_findActor(l_actorJudge, &npcType.mType);
  if (npc) {
    // GHS copies each position word before loading the next one.
    auto *source = reinterpret_cast<be<u32> *>(&npc->current.pos);
    auto *destination = reinterpret_cast<be<u32> *>(&mNPCPos);
    for (u32 i = 0; i < 3; ++i)
      destination[i] = source[i];
  }
  set_mtx();
}

/* 021C4958 */
void daMgBoard_c::CreateInit() {
  J3DModel *model = mpBoardModel;
  cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
  fopAcM_setCullSizeBox(this, -600.f, -300.f, -500.f, 600.f, 300.f, 100.f);
  mState = STATE_INIT;
  dComIfG_inf_c *play = (dComIfG_inf_c *)dComIfGp_get();
  mMinigameStartIdx =
      mgBoard_getEventIdx(&play->play.mEvtManager, "MINIGAME_START", 255);
  play = (dComIfG_inf_c *)dComIfGp_get();
  mMinigameEndIdx =
      mgBoard_getEventIdx(&play->play.mEvtManager, "MINIGAME_END", 255);
  mgBoard_setStickControl(&mStickControl, 5, 2, 3, 2, 0, 0x800,
                          0.8999999761581421f, 0.5f);
  MiniGameInit();
}

/* 021C4A4C */
s32 daMgBoard_c::_create() {
  u32 condition = actor_condition;
  if (!(condition & 8)) {
    if (gabi::ea(this)) {
      fopAc_ac_c_ct(this);
      mStickControl.mVtable = 0x10050788;
      __vtbl = 0x10014C2C;
      mgBoard_setStickControl(&mStickControl, 15, 15, 0, 0, 0, 0x2000,
                              0.8999999761581421f, 0.5f);
      mgBoard_initStickControl(&mStickControl);
      condition = actor_condition;
    }
    actor_condition = condition | 8;
  }
  l_arcName.mWords[0] = l_arcSource.mWords[0];
  l_arcName.mWords[1] = l_arcSource.mWords[1];
  l_arcName.mTerminator = l_arcSource.mTerminator;
  s32 phase = mgBoard_resLoad(&mPhase, &l_arcName);
  if (phase == cPhs_COMPLEATE_e) {
    if (!mgBoard_entryHeap(this, CheckCreateHeap, 0x4E000))
      return cPhs_ERROR_e;
    CreateInit();
  }
  return phase;
}

/* 021C4B70 */
BOOL daMgBoard_c::_delete() {
  mgBoard_resDelete(&mPhase, &l_arcName);
  mDoAud_seDeleteObject(&mNPCPos);
  return 1;
}

/* 021C4BB4 */
BOOL daMgBoard_c::_draw() {
  auto light = dKy_getEnvlight();
  settingTevStruct(light, 0, &current.pos, &tevStr);
  light = dKy_getEnvlight();
  setLightTevColorType(light, mpBoardModel.get(), &tevStr);
  mDoExt_modelUpdateDL(mpBoardModel.get(), 0);
  if (!mbDraw)
    return 1;
  light = dKy_getEnvlight();
  settingTevStruct(light, 0, &current.pos, &tevStr);
  light = dKy_getEnvlight();
  setLightTevColorType(light, mpCursorModel.get(), &tevStr);
  dComIfGd_setListMaskOff();
  mDoExt_modelUpdateDL(mpCursorModel.get(), 0);
  dComIfGd_setList();
  dComIfGd_setListMaskOff();

  for (s32 i = 0; i < (s32)mHitModelCount; ++i) {
    light = dKy_getEnvlight();
    J3DModel *model = mpHitModel[i];
    setLightTevColorType(light, model, &tevStr);
    model = mpHitModel[i];
    mDoExt_modelUpdateDL(model, 0);
  }
  for (s32 i = 0; i < (s32)mMissModelCount; ++i) {
    light = dKy_getEnvlight();
    J3DModel *model = mpMissModel[i];
    setLightTevColorType(light, model, &tevStr);
    model = mpMissModel[i];
    mDoExt_modelUpdateDL(model, 0);
  }
  dComIfGd_setList();
  for (u32 i = 0; i < 3; ++i) {
    u8 remaining = mSeaFightGame.mAliveShipNum,
       bombs = mSeaFightGame.mBulletNum,
       length =
           reinterpret_cast<MgBoardShipHD *>(&mSeaFightGame.mShips[i])->mLength;
    if (remaining && bombs)
      continue;
    J3DModel *model = length == 2   ? mpShip2Model[0].get()
                      : length == 3 ? mpShip3Model[0].get()
                      : length == 4 ? mpShip4Model[0].get()
                                    : NULL;
    if (!model)
      continue;
    lightModel(this, model);
    dComIfGd_setListMaskOff();
    mDoExt_modelUpdateDL(model, 0);
    dComIfGd_setList();
  }
  return 1;
}

/* 021C4DFC */
void daMgBoard_c::ResetGame() { MiniGameInit(); }

/* 021C4E00 */
void daMgBoard_c::StartInput() { mgBoard_startInput(l_pad.get(), 5, 5); }

/* 021C4E14 */
u32 daMgBoard_c::CursorMove() {
  MgBoardPadHD *pad = l_pad.get();
  s8 x = mBoardPosX, y = mBoardPosY, oldX = x, oldY = y;
  u32 buttons = pad->mHeldButtons | pad->mRepeatButtons;
  if (pad->mDpadButtons & 0x00F00000)
    buttons &= 0x00F00000;
  if (buttons & 0x00440000) {
    x = (s8)(x - 1);
    mBoardPosX = x;
  } else if (buttons & 0x00880000) {
    x = (s8)(x + 1);
    mBoardPosX = x;
  }
  if (buttons & 0x00110000) {
    y = (s8)((s8)mBoardPosY + 1);
    mBoardPosY = y;
    x = mBoardPosX;
  } else if (buttons & 0x00220000) {
    y = (s8)((s8)mBoardPosY - 1);
    mBoardPosY = y;
    x = mBoardPosX;
  }
  if (x > 7) {
    x = 7;
    mBoardPosX = x;
  } else if (x < 0) {
    x = 0;
    mBoardPosX = x;
  }
  y = mBoardPosY;
  if (y > 7) {
    y = 7;
    mBoardPosY = y;
    x = mBoardPosX;
  } else if (y < 0) {
    y = 0;
    x = mBoardPosX;
    mBoardPosY = y;
  }
  if (x != oldX || y != oldY)
    return mgBoard_playCursorSound(SE_CURSOR);
  // HD: the unchanged cursor returns the this address still held in r3.
  return gabi::ea(this);
}

/* 021C4F34 */
BOOL daMgBoard_c::MinigameMain() {
  if (mgBoard_checkSound(SE_FIRE))
    return 1;
  CursorMove();
  u8 bombs = mSeaFightGame.mBulletNum,
     oldRemaining = mSeaFightGame.mAliveShipNum;
  s32 fire = mgBoard_checkFire(0);
  if (fire && bombs > 0) {
    u8 x = (u8)(s8)mBoardPosX, y = (u8)(s8)mBoardPosY;
    s32 result = mgBoard_attack(&mSeaFightGame, x, y);
    s8 fireY = mBoardPosY, fireX = mBoardPosX;
    u8 remaining = mSeaFightGame.mAliveShipNum;
    mLastFirePosY = fireY;
    u32 score = (u32)mSeaFightGame.mDeadShipNum;
    u8 total = mSeaFightGame.mScore;
    mLastFirePosX = fireX;
    if (result >= 0) {
      mgBoard_playSound(SE_HIT, &mNPCPos, 0);
      if (oldRemaining != remaining && remaining)
        mgBoard_playSound(SE_SHIP_SUNK, &mNPCPos, 0);
      dComIfG_inf_c *play = (dComIfG_inf_c *)dComIfGp_get();
      cXyz direction;
      direction.x = 0.f;
      direction.y = 1.f;
      direction.z = 0.f;
      mgBoard_startShock(&play->play.mVibration, 7, -33, &direction);
    } else if (result == -1)
      mgBoard_playSound(SE_MISS, &mNPCPos, 0);
    MgBoardHudHD *screen = l_hud->mpScreen;
    mgBoard_updateHud(screen, total, score);
  }
  set_mtx();
  return 1;
}

/* 021C546C */
BOOL daMgBoard_c::execGameMain() {
  MinigameMain();

  if (!mSeaFightGame.mAliveShipNum || !mSeaFightGame.mBulletNum) {
    mbEndGame = 1;
    return 1;
  }
  return 0;
}

/* 021C54D4 */
void daMgBoard_c::EndInput() { mgBoard_endInput(l_pad.get()); }

/* 021C54E0 */
BOOL daMgBoard_c::_execute() {
  dComIfGs_getEventRegHD(0xBEFF);
  if (mbForceEnd) {
    ResetGame();
    mbForceEnd = 0;
    mbEndGame = 0;
  }
  switch ((u32)(s32)mState) {
  case STATE_INIT:
    mbStartGame = 0;
    mState = STATE_WAIT;
    break;
  case STATE_WAIT:
    if (mbStartGame)
      mState = STATE_START;
    break;
  case STATE_START:
    StartInput();
    mState = STATE_PLAY;
    [[fallthrough]];
  case STATE_PLAY:
    execGameMain();
    if (mbEndGame) {
      EndInput();
      mTimer = 30;
      mState = STATE_END;
    }
    break;
  case STATE_END:
    if (!mgBoard_decTimer(&mTimer)) {
      u8 remaining = mSeaFightGame.mAliveShipNum;
      mState = STATE_INIT;
      mgBoard_playSound(remaining ? SE_FAILURE : SE_CLEAR, &mNPCPos, 0);
    }
    break;
  }
  return 1;
}

/* 021C5634 */
static BOOL daMgBoard_Execute(daMgBoard_c *board) { return board->_execute(); }

/* ---------------- HD: compiler-generated code of this translation unit
 * ---------------- */
/* 021C5638 */
static void __sinit_d_a_mgameboard_cpp() {
  for (u32 i = 0; i < 4; ++i)
    l_headerStatics.mObject[i] = 0;
  mgBoard_registerObject(&l_registration.mEntries[0]);
  l_headerStatics.mMinAngle = -3.1415927410125732f;
  l_headerStatics.mMaxAngle = 3.1415927410125732f;
  mgBoard_initRandom(&l_headerStatics.mRandomState);
  mgBoard_registerObject(&l_registration.mEntries[1]);
  mgBoard_initMath(&l_headerStatics.mMathState);
  mgBoard_registerObject(&l_registration.mEntries[2]);
  l_headerStatics.mNearLimit = 10000.f;
  for (u32 y = 0; y < 8; ++y)
    for (u32 x = 0; x < 8; ++x) {
      l_boardPosition.mPositions[y][x].x = -87.5f + (f32)x * 25.f;
      l_boardPosition.mPositions[y][x].y = -87.5f + (f32)y * 25.f;
      l_boardPosition.mPositions[y][x].z = 0.f;
    }
  l_headerStatics.mFarDistance = 50000.f;
  l_headerStatics.mNearDistance = 10000.f;
  l_headerStatics.mFarLimit = 50000.f;
  for (u32 column = 0; column < 3; ++column)
    for (u32 row = 0; row < 8; ++row) {
      l_bulletPosition.mPositions[column][row].x = -150.f - (f32)column * 20.f;
      l_bulletPosition.mPositions[column][row].y = 90.f - (f32)row * 20.f;
      l_bulletPosition.mPositions[column][row].z = 0.f;
    }
  for (u32 i = 0; i < 3; ++i) {
    l_shipPosition.mPositions[i].x = 162.5f;
    l_shipPosition.mPositions[i].y = 87.5f - (f32)i * 25.f;
    l_shipPosition.mPositions[i].z = 0.f;
  }
}

/* 021C5BF4 */
static void SafeString_dtor(void *object, u32 flags) {
  if (object && (flags & 1))
    operator_delete(object);
}

/* 021C5C08 */
static BOOL daMgBoard_IsDelete(daMgBoard_c *board) { return 1; }

/* 021C5C10 */
static void daMgBoard_c_dtor(daMgBoard_c *board, u32 flags) {

  if (board) {
    mgBoard_deleteStickControl(&board->mStickControl, 2);
    mgBoard_deleteActor(board, 0);
    if (flags & 1)
      operator_delete(board);
  }
}

/* 021C5C70 */
static void SafeString_assureTerminationImpl(void *object) {}

/* 021C5C74 */
bool daMgBoard_c::checkClearGame() { return mSeaFightGame.mAliveShipNum == 0; }

/* 021C5C84 */
u8 daMgBoard_c::getScore() { return mSeaFightGame.mScore; }

/* 021C5C8C */
void daMgBoard_c::reqStartGame() {
  mbEndGame = 0;
  mbStartGame = 1;
  MgBoardHudHD *screen = l_hud->mpScreen;
  mgBoard_setHudCommand(&screen->mCommand, &l_startCommand);
}

/* 021C5CB8 */
bool daMgBoard_c::checkEndGame() { return mbEndGame != 0; }

/* 021C5CC8 */
void daMgBoard_c::setGInfoDraw() { mbDraw = 1; }

/* 021C5CD4 */
void daMgBoard_c::clrGInfoDraw() {
  mbDraw = 0;
  MgBoardHudHD *screen = l_hud->mpScreen;
  mgBoard_setHudCommand(&screen->mCommand, &l_hideCommand);
}

// HD: the linked static translation unit has its own common-header objects.
/* 021C5CF8 */
static void __sinit_d_a_mgameboard_static_cpp() {
  for (int i = 3; i >= 0; --i)
    l_staticHeaderStatics.mObject[i] = 0;
  mgBoard_registerObject(&l_staticRegistration.mEntries[0]);
  l_staticHeaderStatics.mMinAngle = -3.1415927f;
  l_staticHeaderStatics.mMaxAngle = 3.1415927f;
  mgBoard_initRandom(&l_staticHeaderStatics.mRandomState);
  mgBoard_registerObject(&l_staticRegistration.mEntries[1]);
  mgBoard_initMath(&l_staticHeaderStatics.mMathState);
  mgBoard_registerObject(&l_staticRegistration.mEntries[2]);
  l_staticHeaderStatics.mFarDistance = 50000.f;
  l_staticHeaderStatics.mNearDistance = 10000.f;
  l_staticHeaderStatics.mFarLimit = 50000.f;
  l_staticHeaderStatics.mNearLimit = 10000.f;
}
/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and private HD adapters
data l_arcName 101BA5E8
data l_arcSource 10014C3C
data l_headerStatics 10464FE0
data l_staticHeaderStatics 10465450
data l_registration 101BA594
data l_staticRegistration 101BA5F4
data l_boardPosition 1046500C
data l_bulletPosition 1046530C
data l_shipPosition 1046542C
data l_pad 101F5088
data l_hud 101F8344
data l_resourceControl 101F4F28
data l_startCommand 1048ED50
data l_hideCommand 1048ED80
str 10014C48
str 10014C5C
str 10014C90
str 10014CA0
ext 026066C4 J3DModelData* mgBoard_getResource(void* control, MgBoardSafeStringHD* name, s32 index)
ext 0273AA24 void mgBoard_assert(const char* file, s32 line, const char* assertion)
ext 027F58E0 void mgBoard_setModelMode(J3DModel* model, u32 mode)
ext 025BB358 s32 mgBoard_initSeaFight(dSeaFightGame_info_c* game, s32 bullets, u32 scenario)
ext 025D5218 fopAc_ac_c* mgBoard_findActor(u32 judge, be<s16>* type)
ext 02543F10 s32 mgBoard_getEventIdx(void* manager, const char* name, u32 priority)
ext 025885C4 void mgBoard_setStickControl(MgBoardStickControlHD* control, s32 a, s32 b, s32 c, s32 d, s32 e, s32 angle, f32 threshold, f32 release)
ext 025885E8 void mgBoard_initStickControl(MgBoardStickControlHD* control)
ext 02520460 s32 mgBoard_resLoad(request_of_phase_process_class* phase, MgBoardArchiveHD* archive)
ext 025D63E8 s32 mgBoard_entryHeap(daMgBoard_c* actor, MgBoardHeapCallback callback, u32 size)
ext 025204C8 void mgBoard_resDelete(request_of_phase_process_class* phase, MgBoardArchiveHD* archive)
ext 02618760 void mgBoard_startInput(MgBoardPadHD* pad, s32 repeat, s32 delay)
ext 025E1988 u32 mgBoard_playCursorSound(u32 sound)
ext 025E1B24 s32 mgBoard_checkSound(u32 sound)
ext 02007898 s32 mgBoard_checkFire(s32 player)
ext 025BB470 s32 mgBoard_attack(dSeaFightGame_info_c* game, u32 x, u32 y)
ext 025E1A04 void mgBoard_playSound(u32 sound, cXyz* position, u32 parameter)
ext 025CB374 void mgBoard_startShock(void* vibration, s32 power, s32 flags, cXyz* direction)
ext 02624134 void mgBoard_updateHud(MgBoardHudHD* screen, u32 total, u32 score)
ext 02618774 void mgBoard_endInput(MgBoardPadHD* pad)
ext 0207A9A0 u8 mgBoard_decTimer(be<u8>* timer)
ext 028F026C void mgBoard_registerObject(MgBoardRegistrationHD* registration)
ext 028ED6F8 void mgBoard_initRandom(be<u8>* state)
ext 028EAB2C void mgBoard_initMath(be<u8>* state)
ext 025886D8 void mgBoard_deleteStickControl(MgBoardStickControlHD* control, u32 flags)
ext 025D50BC void mgBoard_deleteActor(fopAc_ac_c* actor, u32 flags)
ext 020063C0 void mgBoard_setHudCommand(MgBoardHudCommandHD* command, MgBoardHudCommandHD* value)
ext 025F1C5C void mgBoard_ZrotM(Mtx34* matrix, s32 rotation)
*/
