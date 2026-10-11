/** Metronome timing and melody guide, retained WWHD screen controller.
 * HD: animation/pane state machines replace the GameCube draw-list action controller.
 * The score, timing and particle operations retain the GameCube terminology.
 * Verified against cking.rpx.
 */
#include "d/d_metronome.h"
using namespace Metronome;
// Retained metronome controllers, construction, pane setup and generated inline companions.
// HD: pane factories and the preceding initializer belong to neighboring units.
static u32 animation(u32 p) {
  return view<Screen>(controller(p)->screen)->animation;
}
/* 0261D93C */
void dMetronome_c::closeFlag() { mTimerActive = 0; }
/* 0261D948 */
void dMetronome_c::open() {
  const u32 p = ea(this);
  if ((u8)tactOpen())
    sdk::changeState<void>(p + 0x18, OpenState);
}
/* 0261D968 */
void dMetronome_c::animationPlay(u32 i) {
  const u32 p = ea(this);
  const f32 f = (f32)one();
  const u32 a = animation(p), v = (u32)animationIndices()[i];
  sdk::playAnimation<void>(a, i, v, f);
}
/* 0261D98C */
void dMetronome_c::animationFrame(u32 i, u32 frame) {
  const u32 p = ea(this);
  const u32 a = animation(p), v = (u32)animationIndices()[i];
  sdk::setAnimationFrame<void>(a, i, v, f32(frame));
}
/* 0261D9E4 */
void dMetronome_c::beatPlay() {
  const u32 p = ea(this);
  u32 b = (u8)mBeat;
  if (b == 3 || b == 4 || b == 6) {
    sdk::invokeAnimationPlay<void>(p, b == 3   ? AnmBeat3
                                      : b == 4 ? AnmBeat4
                                               : AnmBeat6);
    b = (u8)mBeat;
  }
  mAnimationBeat = b;
}
/* 0261DA70 */
void dMetronome_c::melodyHide() {
  const u32 p = ea(this);
  const u32 b = (u8)mBeat;
  auto *panes = notePanes(p, b);
  if (b != 3 && b != 4 && b != 6)
    return;
  for (u32 i = 0; i < b; i++)
    view<Pane>((u32)panes[i])->visible = 0;
}
/* 0261DB68 */
void dMetronome_c::openInit() {
  const u32 p = ea(this);
  u32 b = sdk::getBeat<u32>();
  mBeat = b;
  mPrevBeat = b;
  mNoteIndex = 0;
  mpEmitter = 0;
  mInputActive = 0;
  sdk::invokeAnimationPlay<void>(p, AnmOpen);
  for (u32 i = 1; i <= 3; i++)
    sdk::invokeAnimationFrame<void>(p, i, 0u);
  sdk::invokeBeatPlay<void>(p);
  sdk::invokeHide<void>(p);
  for (u32 i = 0; i < 21; i++)
    sdk::resetPane<void>((u32)pane_timing[i]);
  mLastInput = 0;
  mbOpen = 1;
}
/* 0261E5A8 */
void dMetronome_c::shrinkPlay() {
  const u32 p = ea(this);
  u32 b = (u8)mPrevBeat;
  if (b == 3 || b == 4 || b == 6) {
    sdk::invokeAnimationPlay<void>(p, b == 3   ? AnmShrink3
                                      : b == 4 ? AnmShrink4
                                               : AnmShrink6);
    b = (u8)mPrevBeat;
  }
  mAnimationBeat = b;
}
/* 0261E634 */
void dMetronome_c::shrinkInit() {
  const u32 p = ea(this);
  sdk::invokeShrinkPlay<void>(p);
}
/* 0261E638 */
u32 dMetronome_c::isShrinkEnd() {
  const u32 p = ea(this);
  const u32 b = (u8)mAnimationBeat;
  if (b != 3 && b != 4 && b != 6)
    return 1;
  return sdk::isAnimationEnd<u32>(animation(p), b == 3   ? AnmShrink3
                                                : b == 4 ? AnmShrink4
                                                         : AnmShrink6);
}
/* 0261E760 */
void dMetronome_c::timerInit() { mWaitTimer = 10; }
/* 0261E808 */
void dMetronome_c::demoInit() {
  const u32 p = ea(this);
  const u32 q = sdk::invokeHide<u32>(p);
  controller(q)->mDemoTimer = 0;
  controller(q)->mDemoIndex = 0;
}
/* 0261EA3C */
void dMetronome_c::endInit() {
  const u32 p = ea(this);
  sdk::invokeAnimationPlay<void>(p, AnmEnd);
}
/* 0261EABC */
void dMetronome_c::closeInit() { mbOpen = 0; }

/* 0261DCD0 */
s32 dMetronome_c::getMetronomeTimer() {
  u32 env = sdk::getPlay<u32>();
  f32 rate = sdk::getTactMetronomeRate<f32>((u32)view<Play>(env)->link);
  s32 t = ftoi(fmuls_ppc((f32)timerScale(), rate));
  const u32 side = (u8)mTimerSide;
  if (t > 20)
    t = 20;
  else if (t < 0)
    t = 0;
  if (side) {
    if ((f32)mCurRate > rate)
      mTimerSide = 0;
    return t <= 10 ? 10 - t : t - 10;
  }
  env = sdk::getPlay<u32>();
  rate = sdk::getTactMetronomeRate<f32>((u32)view<Play>(env)->link);
  if ((f32)mCurRate > rate)
    mTimerSide = 1;
  return t <= 10 ? t + 10 : 30 - t;
}
/* 0261DDBC */
void dMetronome_c::metronomeInit() {
  const u32 p = ea(this);
  const u32 env = sdk::getPlay<u32>();
  const f32 rate = sdk::getTactMetronomeRate<f32>((u32)view<Play>(env)->link);
  mCurRate = rate;
  mTimerSide = 0;
  const u32 t = sdk::invokeTimer<u32>(p);
  mInputWait = 0;
  mTimerActive = 1;
  mCurTimer = t;
}
/* 0261DE1C */
void dMetronome_c::waitInit() {
  const u32 p = ea(this);
  sdk::invokeTimerReset<void>(p);
}
/* 0261DE20 */
void dMetronome_c::metronomeMove() {
  const u32 p = ea(this);
  const u32 t = sdk::invokeTimer<u32>(p);
  if (t != (u32)mCurTimer) {
    u32 q = ea(pane_timing);
    if (t < 21)
      q += 4 * t;
    view<Pane>(*view<be<u32>>(q))->active = 1;
  }
  const u32 env = sdk::getPlay<u32>();
  const f32 rate = sdk::getTactMetronomeRate<f32>((u32)view<Play>(env)->link);
  mCurRate = rate;
  mCurTimer = t;
}
static void metroVcall(u32 p, u32 offset) {
  call_ptr<void>(controllerTarget(p, offset), p);
}
/* 0261DC58 */
void dMetronome_c::openMove() {
  const u32 p = ea(this);
  if (sdk::isAnimationEnd<u32>(animation(p), AnmOpen))
    sdk::changeState<void>(p + 0x18, WaitState);
  metroVcall(p, UpdatePanes);
  metroVcall(p, UpdateBase);
}
/* 0261EA44 */
void dMetronome_c::closeMove() {
  const u32 p = ea(this);
  if (sdk::isAnimationEnd<u32>(animation(p), AnmEnd))
    sdk::changeState<void>(p + 0x18, CloseState);
  metroVcall(p, UpdatePanes);
  metroVcall(p, UpdateBase);
}
/* 0261F290 */
void dMetronome_c::_delete() {
  const u32 p = ea(this);
  metroVcall(p, DeletePanes);
  metroVcall(p, DeleteBase);
  sdk::deleteScreen<void>(p);
  const u32 e = (u32)mpEmitter;
  if (e) {
    const u32 flags = (u32)view<Emitter>(e)->mStatus;
    view<Emitter>(e)->mMaxFrame = 0xFFFFFFFF;
    view<Emitter>(e)->mStatus = flags | EmitterStop;
    const u32 q = (u32)mpEmitter;
    view<Emitter>(q)->mStatus =
        (u32)view<Emitter>(q)->mStatus & ~u32(EmitterImmortal);
  }
}
/* 0261F314 */
void dMetronome_c::_move() {
  const u32 p = ea(this);
  if (!sdk::isMessageBusy<u32>(p))
    sdk::moveState<void>(p + 0x18);
}

/* 0261F350 */
void dMetronome_c::_draw(u32 port) {
  const u32 p = ea(this);
  if ((u8)mbOpen) {
    call_ptr<void>(controllerTarget(p, DrawBase), p, port);
    call_ptr<void>(controllerTarget(p, DrawPanes), p, port);
  }
}
/* 0261E4A0 */
void dMetronome_c::waitMove() {
  const u32 p = ea(this);
  if (!(u8)tactOpen())
    sdk::changeState<void>(p + 0x18, EndState);
  else {
    bool changed = false;
    if (!(u8)mLastInput) {
      const u32 current = sdk::getBeat<u32>();
      if ((u8)mBeat != current) {
        mPrevBeat = (u8)mBeat;
        mBeat = sdk::getBeat<u32>();
        mNoteIndex = 0;
        sdk::invokeHide<void>(p);
        changed = true;
      }
    }
    sdk::invokeTimerMove<void>(p);
    sdk::invokeMelodyShow<void>(p);
    if (changed)
      sdk::changeState<void>(p + 0x18, ShrinkState);
    else if ((u8)mLastInput)
      sdk::changeState<void>(p + 0x18, TimerState);
  }
  metroVcall(p, UpdatePanes);
  metroVcall(p, UpdateBase);
}
/* 0261E68C */
void dMetronome_c::shrinkMove() {
  const u32 p = ea(this);
  if (!(u8)tactOpen())
    sdk::changeState<void>(p + 0x18, EndState);
  else {
    sdk::invokeTimerMove<void>(p);
    sdk::invokeMelodyShow<void>(p);
    if ((u8)mLastInput)
      sdk::changeState<void>(p + 0x18, TimerState);
    else if (sdk::invokeShrinkDone<u32>(p)) {
      mInputActive = 0;
      sdk::invokeBeatPlay<void>(p);
      sdk::changeState<void>(p + 0x18, WaitState);
    }
  }
  metroVcall(p, UpdatePanes);
  metroVcall(p, UpdateBase);
}
/* 0261E76C */
void dMetronome_c::timerMove() {
  const u32 p = ea(this);
  if ((u8)tactOpen()) {
    sdk::invokeTimerMove<void>(p);
    u32 t = (u32)mWaitTimer - 1;
    mWaitTimer = t;
    if (!t)
      sdk::changeState<void>(p + 0x18, DemoState);
  } else
    sdk::changeState<void>(p + 0x18, EndState);
  metroVcall(p, UpdatePanes);
  metroVcall(p, UpdateBase);
}
/* 0261E05C */
void dMetronome_c::melodyFlash() {
  const u32 p = ea(this);
  const u32 b = (u8)mBeat;
  if (b != 3 && b != 4 && b != 6)
    return;
  auto *panes = notePanes(p, b);
  for (u32 i = 0; i < b; i++)
    sdk::showBeatEffect<void>((u32)panes[i]);
}
using MetroVec = cXyz;
/* 0261DE98 */
void dMetronome_c::melodyGuideShow(u32 note, u32 index) {
  const u32 p = ea(this);
  const u32 b = (u8)mBeat;
  if (b != 3 && b != 4 && b != 6)
    return;
  auto *panes = notePanes(p, b);
  const u32 pos = index < b ? index : 0;
  const u32 pane = panes[pos];
  view<Pane>(pane)->note = note;
  view<Pane>(pane)->visible = 1;
  const u32 target = targetPanes(p, b)[pos];
  if (!target)
    return;
  Local<MetroVec> coords, result;
  sdk::getPaneCenter<void>(coords.a, target);
  result->x = fadds_ppc(coords->x, mGuideOffset.x);
  result->y = fadds_ppc(coords->y, mGuideOffset.y);
  result->z = (f32)coords->z;
  const u32 env = sdk::getPlay<u32>();
  const u32 e = sdk::createParticle<u32>((u32)view<Play>(env)->particleControl,
                                         7u, BatonGuideParticle, result.a, 0u,
                                         0u, 255u, 0u, 0xFFFFFFFFu, 0u, 0u, 0u);
  const f32 scale = (f32)mGuideScale;
  view<Emitter>(e)->mParticleScale.x = scale;
  view<Emitter>(e)->mParticleScale.y = scale;
  view<Emitter>(e)->mParticleScale.z = scale;
  const f32 wide = (f32)mGuideParticleSize;
  view<Emitter>(e)->mGlobalScale.x = wide;
  view<Emitter>(e)->mGlobalScale.z = wide;
  view<Emitter>(e)->mGlobalScale.y = wide;
}

static void metroProject(u32 p, u32 pos, u32 out) {
  sdk::project<void>(pos, out);
  view<cXyz>(out)->x =
      fadds_ppc(view<cXyz>(out)->x, controller(p)->mProjectOffset.x);
  view<cXyz>(out)->y =
      fadds_ppc(view<cXyz>(out)->y, controller(p)->mProjectOffset.y);
}
static void metroParticleSize(u32 p, u32 e) {
  f32 x = (f32)controller(p)->mTactScale;
  view<Emitter>(e)->mParticleScale.x = x;
  view<Emitter>(e)->mParticleScale.y = x;
  view<Emitter>(e)->mParticleScale.z = x;
  x = (f32)controller(p)->mTactParticleSize;
  e = (u32)controller(p)->mpEmitter;
  view<Emitter>(e)->mGlobalScale.x = x;
  view<Emitter>(e)->mGlobalScale.z = x;
  view<Emitter>(e)->mGlobalScale.y = x;
}
/* 0261E1A0 */
void dMetronome_c::melodyShow() {
  const u32 p = ea(this);
  Local<MetroVec> tact, projected;
  u32 env = sdk::getPlay<u32>();
  const bool input =
      (u32)view<Player>(view<Play>(env)->player)->flags & TactInput;
  if (input) {
    if (!(u8)mInputActive) {
      u32 index = (u32)mNoteIndex;
      if (index >= (u8)mBeat) {
        mNoteIndex = 0;
        sdk::invokeHide<void>(p);
        index = (u32)mNoteIndex;
      }
      if (!index)
        view<Pane>(pane_timing[10])->active = 1;
      mInputActive = 1;
      env = sdk::getPlay<u32>();
      const u32 note =
          u32(s32((s16)view<Player>(view<Play>(env)->link)->tactNote));
      index = (u32)mNoteIndex;
      mNote[index < 6 ? index : 0] = note;
      sdk::invokeMelodyGuideShow<void>(p, note, u32((u8)mNoteIndexBytes[3]));
      mNoteIndex = (u32)mNoteIndex + 1;
      env = sdk::getPlay<u32>();
      if (sdk::getTactTopPos<u32>((u32)view<Play>(env)->link, tact.a)) {
        metroProject(p, tact.a, projected.a);
        u32 e = (u32)mpEmitter;
        if (e) {
          view<Emitter>(e)->mStatus =
              (u32)view<Emitter>(e)->mStatus & ~u32(EmitterImmortal);
          mpEmitter = 0;
        }
        env = sdk::getPlay<u32>();
        e = sdk::createParticle<u32>((u32)view<Play>(env)->particleControl, 7u,
                                     BatonTactParticle, projected.a, 0u, 0u,
                                     255u, 0u, 0xFFFFFFFFu, 0u, 0u, 0u);
        mpEmitter = e;
        if (e) {
          view<Emitter>(e)->mStatus =
              (u32)view<Emitter>(e)->mStatus | EmitterImmortal;
          metroParticleSize(p, (u32)mpEmitter);
        }
      }
      env = sdk::getPlay<u32>();
      if (sdk::checkTactLastInput<u32>((u32)view<Play>(env)->link)) {
        mLastInput = 1;
        sdk::invokeMelodyFlash<void>(p);
        sdk::showTimingEcho<void>((u32)pane_timing[10]);
      }
    }
    mInputWait = 0;
  } else {
    if ((u32)mNoteIndex >= (u8)mBeat) {
      u32 t = (u32)mInputWait + 1;
      mInputWait = t;
      if (t > 30) {
        mNoteIndex = 0;
        sdk::invokeHide<void>(p);
      }
    }
    mInputActive = 0;
  }
  u32 e = (u32)mpEmitter;
  if (!e)
    return;
  const u32 flags = (u32)view<Emitter>(e)->mStatus;
  if ((flags & EmitterDeleteEnabled) &&
      (u32)view<Emitter>(e)->particleCount +
              (u32)view<Emitter>(e)->childParticleCount ==
          0) {
    view<Emitter>(e)->mStatus =
        (u32)view<Emitter>(e)->mStatus & ~u32(EmitterImmortal);
    mpEmitter = 0;
    return;
  }
  env = sdk::getPlay<u32>();
  if (sdk::getTactTopPos<u32>((u32)view<Play>(env)->link, tact.a)) {
    metroProject(p, tact.a, projected.a);
    e = (u32)mpEmitter;
    f32 y = (f32)projected->y;
    if ((u8)view<Emitter>(e)->mDrawGroup >= 7)
      y = -y;
    view<Emitter>(e)->mGlobalTranslation.y = y;
    view<Emitter>(e)->mGlobalTranslation.x = (f32)projected->x;
    view<Emitter>(e)->mGlobalTranslation.z = (f32)projected->z;
    metroParticleSize(p, (u32)mpEmitter);
  }
}

/* 0261E834 */
void dMetronome_c::melodyDemo() {
  const u32 p = ea(this);
  const u32 ticks = (u32)mDemoTimer;
  const u32 env = sdk::getPlay<u32>();
  u32 player = (u32)view<Play>(env)->link;
  if (!ticks) {
    if (sdk::checkTactPlayMelody<u32>(player))
      mDemoTimer = 1;
    return;
  }
  const u32 melody = call_ptr<u32>(
      virtualTarget(view<Player>(player)->vtable, GetMelody), player);
  Local<be<s16>[7]> frames;
  Local<be<s32>[6]> notes;
  (*frames)[0] = 1;
  u32 count = (u8)mBeat;
  for (u32 i = 0; i < count; i++) {
    const f32 f = sdk::getMelodyPattern<f32>(melody, i, ea(&(*notes)[i]));
    (*frames)[i + 1] = s16(ftoi(fadds_ppc(f, f32((*frames)[i]))));
    count = (u8)mBeat;
  }
  const u32 index = (u32)mDemoIndex;
  if (count > index) {
    u32 t = (u32)mDemoTimer;
    if (u32(s32((s16)(*frames)[index])) == t) {
      const u32 note = (u32)mNote[index < 6 ? index : 0];
      sdk::invokeMelodyGuideShow<void>(p, note, index & 255u);
      mDemoIndex = (u32)mDemoIndex + 1;
      t = (u32)mDemoTimer;
    }
    mDemoTimer = t + 1;
  }
}

/* 0261E990 */
void dMetronome_c::demoMove() {
  const u32 p = ea(this);
  if ((u8)tactOpen()) {
    sdk::invokeTimerMove<void>(p);
    sdk::invokeMelodyDemo<void>(p);
  } else
    sdk::changeState<void>(p + 0x18, EndState);
  metroVcall(p, UpdatePanes);
  metroVcall(p, UpdateBase);
}
/* 0261EAC8 */
u32 metroCtor(u32 p) {
  if (!p)
    p = sdk::allocate<u32>(0x16Cu);
  if (!p)
    return 0;
  sdk::constructScreenController<void>(p);
  controller(p)->vtable = ControllerVtable;
  controller(p)->descriptorVtable = DescriptorVtable;
  controller(p)->descriptor = ControllerDescriptor;
  auto *self = controller(p);
  const u32 arrays[] = {ea(self->pane_timing),  ea(self->pane_note3),
                        ea(self->pane_note4),   ea(self->pane_note6),
                        ea(self->pane_target3), ea(self->pane_target4),
                        ea(self->pane_target6), ea(self->mNote)};
  const u32 sizes[] = {84, 12, 16, 24, 12, 16, 24, 24};
  for (u32 i = 0; i < 8; i++)
    if (!arrays[i])
      sdk::allocate<void>(sizes[i]);
  const u32 zero = (u32)zeroBits();
  controller(p)->mpEmitter = 0;
  u32 q = ea(&controller(p)->mGuideOffset);
  if (!q)
    q = sdk::allocate<u32>(8u);
  if (q) {
    view<WordPair>(q)->y = zero;
    view<WordPair>(q)->x = zero;
  }
  q = ea(&controller(p)->mProjectOffset);
  if (!q)
    q = sdk::allocate<u32>(8u);
  if (q) {
    view<WordPair>(q)->y = zero;
    view<WordPair>(q)->x = zero;
  }
  controller(p)->mBeat = 0;
  controller(p)->mAnimationBeat = 0;
  controller(p)->mTimerSide = 0;
  const u32 one = (u32)oneBits();
  *view<be<u32>>(ea(&controller(p)->mTactParticleSize)) = one;
  *view<be<u32>>(ea(&controller(p)->mTactScale)) = one;
  *view<be<u32>>(ea(&controller(p)->mGuideScale)) =
      (u32)constantBits(GuideScale);
  controller(p)->mLastInput = 0;
  controller(p)->mNoteIndex = 0;
  controller(p)->mTimerActive = 0;
  controller(p)->mDemoTimer = 0;
  *view<be<u32>>(ea(&controller(p)->mGuideParticleSize)) =
      (u32)constantBits(GuideParticleSize);
  controller(p)->mbOpen = 0;
  controller(p)->mInputWait = 0;
  controller(p)->mWaitTimer = 0;
  controller(p)->mCurTimer = 0;
  controller(p)->mDemoIndex = 0;
  controller(p)->mInputActive = 0;
  *view<be<u32>>(ea(&controller(p)->mCurRate)) = zero;
  for (u32 i = 0; i < 21; i++)
    controller(p)->pane_timing[i] = 0;
  for (u32 i = 0; i < 3; i++) {
    controller(p)->pane_note3[i] = 0;
    controller(p)->pane_target3[i] = 0;
  }
  for (u32 i = 0; i < 4; i++) {
    controller(p)->pane_note4[i] = 0;
    controller(p)->pane_target4[i] = 0;
  }
  for (u32 i = 0; i < 6; i++) {
    controller(p)->pane_note6[i] = 0;
    controller(p)->pane_target6[i] = 0;
    controller(p)->mNote[i] = 0;
  }
  return p;
}
/* 0261D8D0 */
void metroDtor(u32 p, u32 flags) {
  if (p) {
    controller(p)->descriptor = ControllerDescriptor;
    controller(p)->descriptorVtable = DescriptorVtable;
    sdk::destroyScreenController<void>(p, 0u);
    if (flags & 1)
      sdk::release<void>(p);
  }
}

/* 0261F680 */
u32 dMetronome_c::loadScreen(u32 file, u32 archive, u32 arg6, u32 arg7,
                             u32 arg8) {
  const u32 p = ea(this);
  u32 screen = sdk::allocateAligned<u32>(0x110u, currentHeap(), 4u);
  if (screen)
    screen = sdk::constructScreen<u32>(screen);
  this->screen = screen;
  if (!screen)
    return 0;
  const u32 target =
      virtualTarget(view<Screen>(screen)->resourceVtable, LoadResource);
  if (!call_ptr<u32>(target, screen, file, archive, arg6, arg7, 8u, 4u, arg8))
    return 0;
  for (u32 i = 0; i < 8; i++) {
    const u32 a = animation(p), index = (u32)animationIndices()[i];
    sdk::registerAnimation<void>(a, i, AnimationNames + 8 * i,
                                 AnimationResources + 8 * index);
  }
  metroVcall(p, UpdateBase);
  return 1;
}
static void metroStringTouch(u32 p) {
  call_ptr<void>(virtualTarget(view<String>(p)->vtable, StringAssure), p);
}
static bool metroStringEqual(u32 a, u32 b) {
  if (a == b)
    return true;
  for (u32 i = 0; i < StringCompareLimit; i++) {
    const u8 x = (u8)view<be<u8>>(a)[i], y = (u8)view<be<u8>>(b)[i];
    if (x != y)
      return false;
    if (!x)
      return true;
  }
  return false;
}
/* 0261F3C0 */
u32 dMetronome_c::createPane(u32 strings) {
  const u32 p = ea(this);
  Local<String> str;
  u32 creators[3] = {CreateTimingPane, CreateNotePane, CreateBeatPane};
  u32 names[3];
  const u32 holders[3] = {TimingPaneType, NotePaneType, BeatPaneType};
  for (u32 i = 0; i < 3; i++) {
    if (i == 0)
      call_ptr<void>(
          virtualTarget(view<String>(holders[i])->vtable, StringAssure),
          holders[i], strings, CreateBeatPane);
    else
      metroStringTouch(holders[i]);
    names[i] = (u32)view<String>(holders[i])->text;
  }
  const u32 first = ea(&view<PaneStrings>(strings)->name),
            second = ea(&view<PaneStrings>(strings)->type);
  for (u32 i = 0; i < 34; i++) {
    const u32 vt = (u32)view<String>(first)->vtable,
              fn = virtualTarget(vt, StringAssure);
    call_ptr<void>(fn, first, vt, fn);
    metroStringTouch(first);
    const u32 table = PaneNames + 8 * i;
    const u32 name = (u32)view<String>(first)->text;
    metroStringTouch(table);
    const u32 other = (u32)view<String>(table)->text;
    if (name != other && !metroStringEqual((u32)view<String>(first)->text,
                                           (u32)view<String>(table)->text))
      continue;
    for (u32 j = 0; j < 3; j++) {
      str->text = names[j];
      str->vtable = StringVtable;
      metroStringTouch(second);
      metroStringTouch(second);
      const u32 n = (u32)view<String>(second)->text;
      metroStringTouch(str.a);
      const u32 other2 = (u32)view<String>(str.a)->text;
      if (n != other2 && !metroStringEqual((u32)view<String>(second)->text,
                                           (u32)view<String>(str.a)->text))
        continue;
      const u32 count = (u32)paneCount;
      u32 slot = (u32)panes;
      if (i < count)
        slot += 4 * i;
      const u32 result = call_ptr<u32>(creators[j], 0u, 0u, strings);
      *view<be<u32>>(slot) = result;
      slot = (u32)panes;
      if (i < (u32)paneCount)
        slot += 4 * i;
      return (u32)*view<be<u32>>(slot) ? 1u : 0u;
    }
    return sdk::createBasePane<u32>(p, i, strings);
  }
  return 1;
}

/* 0261EDFC */
u32 dMetronome_c::screenSet(u32 archive, u32 a, u32 b) {
  const u32 p = ea(this);
  sdk::initializePaneNames<void>(p, LayoutName, 4u);
  u32 target = controllerTarget(p, ScreenSet);
  if (!call_ptr<u32>(target, p, LayoutFilename, (u32)this->archive, archive, a,
                     b))
    return 0;
  if (!call_ptr<u32>(controllerTarget(p, CreatePanes), p))
    return 0;
  sdk::setAnimationFrame<void>(animation(p), 0u, 0u, (f32)zeroBits());
  const u32 screen =
      (u32)view<ScreenOwnerVtable>(view<Screen>(this->screen)->vtable)->screen;
  for (u32 i = 0; i < 21; i++) {
    const u32 count = (u32)paneCount, array = (u32)panes;
    pane_timing[i] = (u32)view<be<u32>>(array)[i < count ? i : 0];
  }
  const u32 n[3] = {3, 4, 6}, base[3] = {21, 24, 28};
  for (u32 j = 0; j < 3; j++)
    for (u32 i = 0; i < n[j]; i++) {
      const u32 index = base[j] + i, count = (u32)paneCount, array = (u32)panes;
      notePanes(p, n[j])[i] =
          (u32)view<be<u32>>(array)[index < count ? index : 0];
      const u32 str = PaneNames + 8 * index;
      const u32 vtable = (u32)view<PaneScreen>(screen)->vtable;
      metroStringTouch(str);
      const u32 pane = call_ptr<u32>(virtualTarget(vtable, FindPane), screen,
                                     (u32)view<String>(str)->text, 1u);
      targetPanes(p, n[j])[i] = pane;
    }
  const u32 state = this->state;
  const u32 action = call_ptr<u32>(
      virtualTarget(view<StateObject>(state)->vtable, SelectState), state,
      CloseState);
  this->action = action;
  call_ptr<void>(virtualTarget(view<StateObject>(action)->vtable, EnterState),
                 action);
  return 1;
}

// HD: compiler-generated and template code of this translation unit.
// Static animation names, pane names and state registration descriptors, in native write order.
/* 0261F7AC */
void metroStaticInit() {
  staticData().cleared[3] = 0x00000000u;
  staticData().cleared[2] = 0x00000000u;
  staticData().cleared[1] = 0x00000000u;
  staticData().cleared[0] = 0x00000000u;
  sdk::registerDestructor<void>(StaticDestructor0);
  staticData().position[0] = (u32)constantBits(StaticPositionX);
  staticData().position[1] = (u32)constantBits(StaticPositionY);
  sdk::constructStaticA<void>(StaticA);
  sdk::registerDestructor<void>(StaticDestructor1);
  sdk::constructStaticB<void>(StaticB);
  sdk::registerDestructor<void>(StaticDestructor2);
  staticData().animationResources[0].vtable = StringVtable;
  staticData().animationResources[0].text = MainAnimationResource;
  staticData().animationResources[1].vtable = StringVtable;
  staticData().animationResources[1].text = Beat3AnimationResource;
  staticData().animationResources[2].vtable = StringVtable;
  staticData().animationResources[2].text = Beat4AnimationResource;
  staticData().animationResources[3].vtable = StringVtable;
  staticData().animationResources[3].text = Beat6AnimationResource;
  staticData().animationNames[0].vtable = StringVtable;
  staticData().animationNames[0].text = OpenAnimationName;
  staticData().animationNames[1].vtable = StringVtable;
  staticData().animationNames[1].text = BeatAnimationName;
  staticData().animationNames[2].vtable = StringVtable;
  staticData().animationNames[2].text = BeatAnimationName;
  staticData().animationNames[3].vtable = StringVtable;
  staticData().animationNames[3].text = BeatAnimationName;
  staticData().animationNames[4].vtable = StringVtable;
  staticData().animationNames[4].text = ShrinkAnimationName;
  staticData().animationNames[5].vtable = StringVtable;
  staticData().animationNames[5].text = ShrinkAnimationName;
  staticData().animationNames[6].vtable = StringVtable;
  staticData().paneNames[0].vtable = StringVtable;
  staticData().paneNames[0].text = TimingPaneName01;
  staticData().animationNames[6].text = ShrinkAnimationName;
  staticData().paneNames[1].vtable = StringVtable;
  staticData().paneNames[1].text = TimingPaneName02;
  staticData().paneNames[2].vtable = StringVtable;
  staticData().animationNames[7].vtable = StringVtable;
  staticData().paneNames[2].text = TimingPaneName03;
  staticData().paneNames[3].vtable = StringVtable;
  staticData().paneNames[3].text = TimingPaneName04;
  staticData().paneNames[4].vtable = StringVtable;
  staticData().animationNames[7].text = EndAnimationName;
  staticData().paneNames[4].text = TimingPaneName05;
  staticData().paneNames[5].vtable = StringVtable;
  staticData().paneNames[5].text = TimingPaneName06;
  staticData().paneNames[6].vtable = StringVtable;
  staticData().paneNames[6].text = TimingPaneName07;
  staticData().paneNames[7].vtable = StringVtable;
  staticData().paneNames[7].text = TimingPaneName08;
  staticData().paneNames[8].vtable = StringVtable;
  staticData().paneNames[8].text = TimingPaneName09;
  staticData().paneNames[9].vtable = StringVtable;
  staticData().paneNames[9].text = TimingPaneName10;
  staticData().paneNames[10].vtable = StringVtable;
  staticData().paneNames[10].text = TimingPaneName11;
  staticData().paneNames[11].vtable = StringVtable;
  staticData().paneNames[11].text = TimingPaneName12;
  staticData().paneNames[12].vtable = StringVtable;
  staticData().paneNames[12].text = TimingPaneName13;
  staticData().paneNames[13].vtable = StringVtable;
  staticData().paneNames[13].text = TimingPaneName14;
  staticData().paneNames[14].vtable = StringVtable;
  staticData().paneNames[14].text = TimingPaneName15;
  staticData().paneNames[15].vtable = StringVtable;
  staticData().paneNames[15].text = TimingPaneName16;
  staticData().paneNames[16].vtable = StringVtable;
  staticData().paneNames[16].text = TimingPaneName17;
  staticData().paneNames[17].vtable = StringVtable;
  staticData().paneNames[17].text = TimingPaneName18;
  staticData().paneNames[18].vtable = StringVtable;
  staticData().paneNames[18].text = TimingPaneName19;
  staticData().paneNames[19].vtable = StringVtable;
  staticData().paneNames[19].text = TimingPaneName20;
  staticData().paneNames[20].vtable = StringVtable;
  staticData().paneNames[20].text = TimingPaneName21;
  staticData().paneNames[21].vtable = StringVtable;
  staticData().paneNames[21].text = Beat3PaneName1;
  staticData().paneNames[22].vtable = StringVtable;
  staticData().paneNames[22].text = Beat3PaneName2;
  staticData().paneNames[23].vtable = StringVtable;
  staticData().paneNames[23].text = Beat3PaneName3;
  staticData().paneNames[24].vtable = StringVtable;
  staticData().paneNames[24].text = Beat4PaneName1;
  staticData().paneNames[25].vtable = StringVtable;
  staticData().paneNames[25].text = Beat4PaneName2;
  staticData().paneNames[26].vtable = StringVtable;
  staticData().paneNames[26].text = Beat4PaneName3;
  staticData().paneNames[27].vtable = StringVtable;
  staticData().paneNames[27].text = Beat4PaneName4;
  staticData().paneNames[28].vtable = StringVtable;
  staticData().paneNames[28].text = Beat6PaneName1;
  staticData().paneTypes[1].text = TimingPaneTypeName;
  staticData().paneTypes[4].vtable = StringVtable;
  staticData().paneTypes[5].vtable = StringVtable;
  staticData().paneTypes[1].vtable = StringVtable;
  staticData().paneTypes[5].text = TimingPaneTypeName;
  staticData().paneTypes[3].vtable = StringVtable;
  staticData().paneTypes[2].text = TimingPaneTypeName;
  staticData().paneTypes[0].text = TimingPaneTypeName;
  staticData().paneTypes[0].vtable = StringVtable;
  staticData().paneTypes[11].vtable = StringVtable;
  staticData().paneTypes[9].text = TimingPaneTypeName;
  staticData().paneTypes[9].vtable = StringVtable;
  staticData().paneTypes[4].text = TimingPaneTypeName;
  staticData().paneTypes[8].text = TimingPaneTypeName;
  staticData().paneTypes[3].text = TimingPaneTypeName;
  staticData().paneTypes[8].vtable = StringVtable;
  staticData().paneTypes[13].vtable = StringVtable;
  staticData().paneTypes[12].vtable = StringVtable;
  staticData().paneTypes[6].vtable = StringVtable;
  staticData().paneTypes[6].text = TimingPaneTypeName;
  staticData().paneTypes[18].text = TimingPaneTypeName;
  staticData().paneTypes[18].vtable = StringVtable;
  staticData().paneTypes[19].vtable = StringVtable;
  staticData().paneTypes[17].text = TimingPaneTypeName;
  staticData().paneTypes[11].text = TimingPaneTypeName;
  staticData().paneTypes[10].vtable = StringVtable;
  staticData().paneTypes[19].text = TimingPaneTypeName;
  staticData().paneTypes[17].vtable = StringVtable;
  staticData().paneNames[29].vtable = StringVtable;
  staticData().paneNames[29].text = Beat6PaneName2;
  staticData().paneTypes[7].text = TimingPaneTypeName;
  staticData().paneTypes[13].text = TimingPaneTypeName;
  staticData().paneNames[30].vtable = StringVtable;
  staticData().paneNames[30].text = Beat6PaneName3;
  staticData().paneTypes[20].vtable = StringVtable;
  staticData().paneNames[31].vtable = StringVtable;
  staticData().paneNames[31].text = Beat6PaneName4;
  staticData().paneNames[32].vtable = StringVtable;
  staticData().paneNames[32].text = Beat6PaneName5;
  staticData().paneTypes[31].vtable = StringVtable;
  staticData().paneNames[33].vtable = StringVtable;
  staticData().paneNames[33].text = Beat6PaneName6;
  staticData().paneTypes[16].text = TimingPaneTypeName;
  staticData().paneTypes[12].text = TimingPaneTypeName;
  staticData().paneTypes[14].vtable = StringVtable;
  staticData().paneTypes[16].vtable = StringVtable;
  staticData().paneTypes[20].text = TimingPaneTypeName;
  staticData().paneTypes[14].text = TimingPaneTypeName;
  staticData().paneTypes[21].vtable = StringVtable;
  staticData().paneTypes[15].text = TimingPaneTypeName;
  staticData().paneTypes[2].vtable = StringVtable;
  staticData().paneTypes[10].text = EchoPaneTypeName;
  staticData().paneTypes[15].vtable = StringVtable;
  staticData().layoutNames[0].vtable = StringVtable;
  staticData().layoutNames[0].text = LayoutResourceName;
  staticData().layoutNames[1].vtable = StringVtable;
  staticData().paneTypes[25].text = BeatPaneTypeName;
  staticData().paneTypes[29].vtable = StringVtable;
  staticData().layoutNames[1].text = TimingPaneTypeName;
  staticData().paneTypes[29].text = BeatPaneTypeName;
  const u32 registration = (u32)registered();
  staticData().paneTypes[23].text = BeatPaneTypeName;
  staticData().paneTypes[25].vtable = StringVtable;
  staticData().paneTypes[31].text = BeatPaneTypeName;
  staticData().paneTypes[24].vtable = StringVtable;
  staticData().paneTypes[23].vtable = StringVtable;
  staticData().paneTypes[30].text = BeatPaneTypeName;
  staticData().paneTypes[26].vtable = StringVtable;
  staticData().paneTypes[32].vtable = StringVtable;
  staticData().paneTypes[28].text = BeatPaneTypeName;
  staticData().paneTypes[28].vtable = StringVtable;
  staticData().paneTypes[26].text = BeatPaneTypeName;
  staticData().paneTypes[22].vtable = StringVtable;
  staticData().paneTypes[21].text = BeatPaneTypeName;
  staticData().paneTypes[27].vtable = StringVtable;
  staticData().paneTypes[24].text = BeatPaneTypeName;
  staticData().paneTypes[32].text = BeatPaneTypeName;
  staticData().paneTypes[27].text = BeatPaneTypeName;
  staticData().paneTypes[22].text = BeatPaneTypeName;
  staticData().layoutNames[2].vtable = StringVtable;
  staticData().layoutNames[2].text = BeatPaneTypeName;
  staticData().layoutNames[3].vtable = StringVtable;
  staticData().layoutNames[3].text = EchoPaneTypeName;
  staticData().paneTypes[30].vtable = StringVtable;
  staticData().paneTypes[33].text = BeatPaneTypeName;
  staticData().paneTypes[7].vtable = StringVtable;
  staticData().layoutFilename.vtable = StringVtable;
  staticData().paneTypes[33].vtable = StringVtable;
  staticData().layoutFilename.text = LayoutResourceName;
  u32 nextId;
  if (registration) {
    nextId = (u32)stateId();
  } else {
    nextId = 0;
    registered() = 1;
  }
  staticData().states[0].initialize.adjustment = 0x00000000u;
  staticData().states[0].initialize.slot = 0x00000019u;
  staticData().states[0].execute.adjustment = 0x00000000u;
  staticData().states[0].execute.slot = 0x0000001Au;
  ++nextId;
  staticData().states[1].initialize.adjustment = 0x00000000u;
  staticData().states[0].id = nextId;
  staticData().states[0].name.text = CloseStateName;
  staticData().states[0].draw.adjustment = 0x00000000u;
  staticData().states[0].draw.slot = 0x0000001Bu;
  staticData().states[1].initialize.slot = 0x0000001Cu;
  staticData().states[1].execute.adjustment = 0x00000000u;
  ++nextId;
  staticData().states[0].initialize.function = 0x00000004u;
  staticData().states[1].id = nextId;
  staticData().states[1].name.text = OpenStateName;
  staticData().states[1].initialize.function = 0x00000004u;
  staticData().states[1].execute.function = 0x00000004u;
  staticData().states[1].draw.function = 0x00000004u;
  staticData().states[1].execute.slot = 0x0000001Du;
  staticData().states[0].execute.function = 0x00000004u;
  staticData().states[1].draw.adjustment = 0x00000000u;
  staticData().states[1].draw.slot = 0x0000001Eu;
  staticData().states[1].reserved.adjustment = 0x00000000u;
  staticData().states[0].draw.function = 0x00000004u;
  staticData().states[0].reserved.function = 0x00000000u;
  staticData().states[1].reserved.slot = 0x00000000u;
  staticData().states[0].reserved.adjustment = 0x00000000u;
  staticData().states[0].name.vtable = StateNameVtable;
  ++nextId;
  staticData().states[1].reserved.function = 0x00000000u;
  staticData().states[1].name.vtable = StateNameVtable;
  staticData().states[2].id = nextId;
  staticData().states[0].base = StateBase;
  staticData().states[1].base = StateBase;
  staticData().states[2].initialize.adjustment = 0x00000000u;
  staticData().states[2].initialize.slot = 0x0000001Fu;
  staticData().states[2].name.text = WaitStateName;
  staticData().states[2].initialize.function = 0x00000004u;
  staticData().states[2].execute.function = 0x00000004u;
  staticData().states[3].initialize.adjustment = 0x00000000u;
  staticData().states[3].initialize.slot = 0x00000022u;
  staticData().states[3].execute.adjustment = 0x00000000u;
  staticData().states[0].reserved.slot = 0x00000000u;
  staticData().states[2].draw.function = 0x00000004u;
  staticData().states[2].execute.adjustment = 0x00000000u;
  staticData().states[3].execute.slot = 0x00000023u;
  ++nextId;
  staticData().states[3].draw.adjustment = 0x00000000u;
  staticData().states[3].id = nextId;
  staticData().states[2].reserved.function = 0x00000000u;
  ++nextId;
  staticData().states[3].draw.slot = 0x00000024u;
  staticData().states[2].execute.slot = 0x00000020u;
  staticData().states[3].reserved.adjustment = 0x00000000u;
  staticData().states[5].id = nextId;
  staticData().states[2].draw.adjustment = 0x00000000u;
  staticData().states[2].draw.slot = 0x00000021u;
  staticData().states[2].reserved.adjustment = 0x00000000u;
  staticData().states[3].name.text = ShrinkStateName;
  staticData().states[3].reserved.slot = 0x00000000u;
  staticData().states[5].initialize.adjustment = 0x00000000u;
  staticData().states[5].initialize.slot = 0x00000028u;
  staticData().states[5].execute.adjustment = 0x00000000u;
  staticData().states[5].execute.slot = 0x00000029u;
  staticData().states[2].reserved.slot = 0x00000000u;
  ++nextId;
  staticData().states[5].name.text = DemoStateName;
  staticData().states[4].id = nextId;
  staticData().states[4].name.text = TimerStateName;
  staticData().states[2].name.vtable = StateNameVtable;
  staticData().states[2].base = StateBase;
  staticData().states[3].initialize.function = 0x00000004u;
  staticData().states[5].draw.adjustment = 0x00000000u;
  staticData().states[5].draw.slot = 0x0000002Au;
  staticData().states[5].reserved.adjustment = 0x00000000u;
  staticData().states[5].reserved.slot = 0x00000000u;
  staticData().states[3].execute.function = 0x00000004u;
  staticData().states[3].draw.function = 0x00000004u;
  staticData().states[3].reserved.function = 0x00000000u;
  staticData().states[3].name.vtable = StateNameVtable;
  staticData().states[5].initialize.function = 0x00000004u;
  staticData().states[5].execute.function = 0x00000004u;
  staticData().states[5].draw.function = 0x00000004u;
  staticData().states[5].reserved.function = 0x00000000u;
  staticData().states[5].name.vtable = StateNameVtable;
  staticData().states[5].base = StateBase;
  staticData().states[3].base = StateBase;
  staticData().states[4].initialize.adjustment = 0x00000000u;
  staticData().states[4].initialize.slot = 0x00000025u;
  staticData().states[4].execute.adjustment = 0x00000000u;
  staticData().states[4].execute.slot = 0x00000026u;
  staticData().states[4].draw.adjustment = 0x00000000u;
  staticData().states[4].initialize.function = 0x00000004u;
  ++nextId;
  staticData().states[6].initialize.adjustment = 0x00000000u;
  staticData().states[6].id = nextId;
  staticData().states[6].name.text = EndStateName;
  staticData().states[6].initialize.function = 0x00000004u;
  staticData().states[6].execute.function = 0x00000004u;
  staticData().states[6].draw.function = 0x00000004u;
  staticData().states[6].reserved.function = 0x00000000u;
  staticData().states[6].name.vtable = StateNameVtable;
  staticData().states[4].execute.function = 0x00000004u;
  staticData().states[4].draw.function = 0x00000004u;
  staticData().states[6].initialize.slot = 0x0000002Bu;
  staticData().states[6].execute.adjustment = 0x00000000u;
  staticData().states[4].reserved.function = 0x00000000u;
  staticData().states[6].execute.slot = 0x0000002Cu;
  staticData().states[6].draw.adjustment = 0x00000000u;
  staticData().states[6].draw.slot = 0x0000002Du;
  staticData().states[6].reserved.adjustment = 0x00000000u;
  staticData().states[4].name.vtable = StateNameVtable;
  staticData().states[6].reserved.slot = 0x00000000u;
  staticData().states[4].draw.slot = 0x00000027u;
  stateId() = nextId;
  staticData().states[4].base = StateBase;
  staticData().states[4].reserved.adjustment = 0x00000000u;
  staticData().states[4].reserved.slot = 0x00000000u;
  staticData().states[6].base = StateBase;
}

/* 0261FF78 */
void metroSmallDtor(u32 p, u32 flag) {
  if (p && (flag & 1))
    sdk::release<void>(p);
}
/* 0261FFA8 */
u32 metroStringValue(u32 p) { return view<String>(p)->text; }
/* 0261FF8C */
void metroEmptyInitialize() {}
/* 0261FF90 */
void metroEmptyExecute() {}
/* 0261FF94 */
void metroEmptyDraw() {}
/* 0261FF98 */
void metroEmptyDelete() {}
/* 0261FF9C */
void metroEmptyOpen() {}
/* 0261FFA0 */
void metroEmptyClose() {}
/* 0261FFA4 */
void metroEmptySet() {}
/* 0261FFB0 */
void metroEmptyStateInitialize() {}
/* 0261FFB4 */
void metroEmptyStateExecute() {}
/* 0261FFB8 */
void metroEmptyStateDraw() {}

/* 0261FFBC */
u32 metroDispatchInitialize(u32 desc, u32 base, u32 r5, u32 r6, u32 r7, u32 r8,
                            u32 r9, u32 r10) {
  u32 self;
  const u32 target = memberTarget(desc, base, 12, self, r9, r10);
  return call_ptr<u32>(target, self, base, r5, r6, r7, r8, r9, r10);
}
/* 0261FFFC */
u32 metroDispatchExecute(u32 desc, u32 base, u32 r5, u32 r6, u32 r7, u32 r8,
                         u32 r9, u32 r10) {
  u32 self;
  const u32 target = memberTarget(desc, base, 20, self, r9, r10);
  return call_ptr<u32>(target, self, base, r5, r6, r7, r8, r9, r10);
}
/* 0262003C */
u32 metroDispatchDraw(u32 desc, u32 base, u32 r5, u32 r6, u32 r7, u32 r8,
                      u32 r9, u32 r10) {
  u32 self;
  const u32 target = memberTarget(desc, base, 28, self, r9, r10);
  return call_ptr<u32>(target, self, base, r5, r6, r7, r8, r9, r10);
}
/* 0262007C */
u32 metroDescriptorValue(u32 p, u32 r4, u32 r5, u32 r6, u32 r7, u32 r8, u32 r9,
                         u32 r10) {
  const u32 desc = (u32)controller(p)->descriptor;
  if ((s32)view<Descriptor>(desc)->value == -1)
    return controller(p)->base;
  const u32 target =
      virtualTarget(view<Descriptor>(desc)->vtable, StringAssure);
  return call_ptr<u32>(target, desc, r4, r5, r6, r7, r8, r9, r10);
}

/* WWHD glue (tools/verify/autoglue.py)
*/
