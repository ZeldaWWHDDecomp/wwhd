/* dFile_select_c: save-data selection.
 * HD: layout/state UI replaces the GameCube J2D screen. All verified entry points,
 * callback ordering and GHS stack footprints are preserved. */
#include "d/d_file_select.h"
using namespace FileSelectHD;
#include <bit>
using namespace gabi;
// HD: word transfers and lfs transfers differ for signalling NaNs.
// Preserve the original transfer kind and callback-relative reload ordering.

static SafeString l_savePaneType;
static SafeString l_collectionPaneType;
static SafeString l_namePaneType;
static SafeString l_difficultyPaneType;
static State l_difficultySelectionState;
static State l_nameInputState;
static SafeString l_soundArchive;

static Statics l_fileSelect;
static u32 mInputManager;
static u32 mPad;
static u32 mAllocationManager;
static u32 mSave;
static u32 mSaveManager;
static u32 l_stateIdInitialized;
static u32 l_nextStateId;
static u8 l_saveComplete;
static f32 l_animationRate;
static f32 l_initialSpeedRatio;
static f32 l_copyOutOffset;
static f32 l_nameOutOffset;
static f32 l_acceleration;
static f32 l_positionRatio;
static f32 l_positionStep;
static f32 l_zero;
static f64 l_slotDelayFactor;
static f32 l_slotMaxFrames;
static f32 l_slotOutOffset;
static f32 l_negativePi;
static f32 l_positivePi;
static u32 l_eventPanes[8] = {0x5, 0x6, 0x7, 0x8, 0x0, 0x1, 0x2, 0x3};
static u32 l_eventCodes[8] = {0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C};
static u32 l_animationRootIndices[7] = {0x0, 0x1, 0x1, 0x2, 0x2, 0x3, 0x0};
static f32 fileSelectReadFloat(u32 p) { return at<be<f32>>(p)->get(); }
static f32 fileSelectReadFloatWord(u32 p) { return std::bit_cast<f32>(at<be<u32>>(p)->get()); }
static u32 fileSelectGetPane(u32 p, u32 i) {
    auto* owner = at<dFile_select_c>(p);
    const u32 array = owner->mpPanes;
    const u32 count = owner->mPaneCount;
    return at<be<u32>>(array)[i < count ? i : 0].get();
}
static u32 fileSelectGetAnimation(u32 p) {
    return at<Layout>(at<dFile_select_c>(p)->mpLayout.get())->mpAnimation.get();
}
static void fileSelectVirtualCall(u32 p, u32 off) {
    const u32 table = at<Pane>(p)->mVtable;
    call_ptr<void>(at<be<u32>>(table + off)->get(), p);
}
static void fileSelectDrawScreen(u32 p) {
    fileSelectVirtualCall(p, 0xB4);
    fileSelectVirtualCall(p, 0x8C);
}
// Local descriptor, safe-string and action inline companions referenced by the
// screen tables.

// HD: preserve this adjustment, virtual displacement and outgoing r9/r10 values.
static u32 memberTarget(u32 desc, u32 base, u32 off, u32& self, u32& r9, u32& r10) {
    const auto* member = at<Callback>(desc + off);
    const s32 slot = member->mVirtualSlot;
    r9 = u32(s32(member->mThisAdjust.get()));
    self = base + r9;
    if (slot < 0)
        return member->mTarget;
    const s32 vt = member->mVtableOffset;
    r9 = u32(slot) * 8;
    r10 = at<be<u32>>(self + u32(vt))->get() + r9;
    return at<VirtualEntry>(r10)->mTarget;
}

// Native resource-name and state descriptors, in original store/registration
// order.

static void waitService(u32 p, u32 state) {
    fileSelectDrawScreen(p);
    const u32 ticks = at<dFile_select_c>(p)->mDelayTimer.get();
    if (ticks)
        (at<dFile_select_c>(p)->mDelayTimer = ticks - 1);
    if (fileSelectHD_dFile_select_c_updateTransitions(p) &&
        s32(at<dFile_select_c>(p)->mDelayTimer.get()) <= 0) {
        const u32 status = at<Service>(mSaveManager)->mCommand.get();
        if (status != 2 && status != 3)
            fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)), state);
    }
}

static void busyPosition(u32 p, bool nameMode) {
    const u32 q = at<dFile_select_c>(p)->mpScreenParts[3].get();
    const f32 y =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, y)));
    const u32 flags = at<Pane>(q)->mFlags.get();
    f32 x, z;
    if (nameMode) {
        x = fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                                u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                                u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, x)));
        z = fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                                u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                                u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, z)));
        (at<Pane>(q)->mY = y);
        (at<Pane>(q)->mFlags = flags | PANE_POSITION_DIRTY);
        (at<Pane>(q)->mZ = z);
    } else {
        z = fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                                u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                                u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, z)));
        x = fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                                u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                                u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, x)));
        (at<Pane>(q)->mZ = z);
        (at<Pane>(q)->mY = y);
        (at<Pane>(q)->mFlags = flags | PANE_POSITION_DIRTY);
    }
    (at<Pane>(q)->mX = x);
    const u32 next = at<dFile_select_c>(p)->mpScreenParts[3].get();
    (at<Pane>(next)->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
}

static void motionVector(u32 p, u32 index, u32 mode, u32 out) {
    const u32 item = index < 4 ? index : 0;
    u32 base = 0;
    switch (mode) {
    case MOTION_ORIGIN:
        base = p + u32(offsetof(Motion, mOrigin)) + 12 * item;
        break;
    case MOTION_RIGHT:
        base = p + u32(offsetof(Motion, mRight)) + 12 * item;
        break;
    case MOTION_LEFT:
        base = p + u32(offsetof(Motion, mLeft)) + 12 * item;
        break;
    case MOTION_FIRST:
    case MOTION_FIRST_OUT:
        base = p + u32(offsetof(Motion, mFirstChoice));
        break;
    case MOTION_SECOND:
    case MOTION_SECOND_OUT:
        base = p + u32(offsetof(Motion, mSecondChoice));
        break;
    case MOTION_BUSY_ORIGIN:
    case MOTION_NAME_OUT:
        base = p + u32(offsetof(Motion, mOrigin));
        break;
    case MOTION_BUSY_OUT:
        base = p + u32(offsetof(Motion, mRight));
        break;
    case MOTION_COPY_ORIGIN:
        base = p + u32(offsetof(Motion, mOrigin)) + u32(sizeof(Vector));
        break;
    case MOTION_COPY_OUT:
        base = p + u32(offsetof(Motion, mRight)) + u32(sizeof(Vector));
        break;
    case MOTION_CURRENT:
        base = p + u32(offsetof(Motion, mTransition)) + 48 * item;
        break;
    default:
        return;
    }
    f32 x = fileSelectReadFloatWord(base), y = fileSelectReadFloatWord(base + 4),
        z = fileSelectReadFloatWord(base + 8);
    if (mode == 5 || mode == 6)
        x = fadds_ppc(x, l_copyOutOffset);
    if (mode == 12)
        y = fsubs_ppc(y, l_nameOutOffset);
    (at<Vector>(out)->x = x);
    (at<Vector>(out)->y = y);
    (at<Vector>(out)->z = z);
}

static u32 stateKey(u32 p, u32 wanted) {
    const u32 state = at<dFile_select_c>(p)->mpPreviousState.get(),
              fn = at<Vtable>(at<State>(state)->mVtable.get())->mGetId.get(),
              saved = at<State>(wanted)->mVtable.get();
    const u32 a = call_ptr<u32>(fn, state);
    return a == call_ptr<u32>(at<Vtable>(saved)->mGetId.get(), wanted);
}

static void setLabel(u32 q, u32 name, u32 slot) {
    Local<SafeString> label;
    (at<SafeString>(label.a)->mVtable = kSafeStringVtable);
    (at<SafeString>(label.a)->mpText = name);
    fileSelectHD_paneSetTouchSound(q, label.a, SOUND_TOUCH);
    fileSelectHD_paneSetSelectionSound(q, ea(&l_soundArchive), SOUND_TOUCH);
    if (slot)
        fileSelectHD_paneSetConfirmSound(q, ea(&l_soundArchive), slot);
}

/* the original computes these with paired singles through its frame (psq_st of
 * the x/y pair at sp+pair, the sum also at sp+sum2 for the next psq_l); the
 * frame keeps them (gabi::NativeFrame, see fsCreate) */
static void vectorDifference(u32 dst, u32 a, u32 b, u32 pair) {
    const f32 x = fsubs_ppc(at<Pane>(a)->mX.get(), at<Pane>(b)->mX.get()),
              y = fsubs_ppc(at<Pane>(a)->mY.get(), at<Pane>(b)->mY.get());
    (at<Pair>(pair)->x = x);
    (at<Pair>(pair)->y = y);
    const f32 bz = at<Pane>(b)->mZ.get(), az = at<Pane>(a)->mZ.get();
    (at<Vector>(dst)->x = x);
    (at<Vector>(dst)->y = y);
    (at<Vector>(dst)->z = fsubs_ppc(az, bz));
}
static void vectorSumDifference(u32 dst, u32 a, u32 b, u32 c, u32 sum, u32 sum2, u32 res) {
    const f32 x = fadds_ppc(at<Pane>(a)->mX.get(), at<Pane>(b)->mX.get()),
              y = fadds_ppc(at<Pane>(a)->mY.get(), at<Pane>(b)->mY.get());
    (at<Pair>(sum)->x = x);
    (at<Pair>(sum)->y = y);
    (at<Pair>(sum2)->x = x);
    (at<Pair>(sum2)->y = y);
    const f32 bz = at<Pane>(b)->mZ.get(), az = at<Pane>(a)->mZ.get(), cx = at<Pane>(c)->mX.get(),
              cy = at<Pane>(c)->mY.get(), cz = at<Pane>(c)->mZ.get();
    const f32 rx = fsubs_ppc(x, cx), ry = fsubs_ppc(y, cy);
    (at<Pair>(res)->x = rx);
    (at<Pair>(res)->y = ry);
    (at<Vector>(dst)->x = rx);
    (at<Vector>(dst)->y = ry);
    (at<Vector>(dst)->z = fsubs_ppc(fadds_ppc(az, bz), cz));
}

static u32 partIndex(u32 p, u32 i) { return at<dFile_select_c>(p)->mpParts[i < 10 ? i : 0]; }
static void restoreIndexed(u32 p, u32 offset, u32 vectorIndex, bool reloadVector) {
    const u32 q = partIndex(p, at<be<u32>>(p + offset)->get() + (offset == 0x1D4 ? 0 : 6)),
              v = p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  (vectorIndex < 4 ? 48 * vectorIndex : 0);
    u8 flag = 0;
    if (vectorIndex < 4)
        flag = at<Pane>(q)->mFlags.get();
    const f32 y = fileSelectReadFloat(v + 4), x = fileSelectReadFloat(v),
              z = fileSelectReadFloat(v + 8);
    (at<Pane>(q)->mX = x);
    if (vectorIndex >= 4)
        flag = at<Pane>(q)->mFlags.get();
    (at<Pane>(q)->mY = y);
    (at<Pane>(q)->mZ = z);
    (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
    const u32 actual = at<be<u32>>(p + offset)->get(),
              q2 = partIndex(p, actual + (offset == 0x1D4 ? 0 : 6)),
              vi = reloadVector ? actual : vectorIndex;
    (at<Pane>(q2)->mAlpha = at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                                       u32(offsetof(Motion, mTransition)) +
                                       (vi < 4 ? u32(sizeof(Transition)) * vi : 0) +
                                       u32(offsetof(Transition, mAlpha)) + 1)
                                ->get());
}

static void touch(u32 p, bool targetR4 = false) {
    const u32 fn = at<StringVtable>(at<SafeString>(p)->mVtable.get())->mAssureTermination.get();
    if (targetR4)
        call_ptr<void>(fn, p, fn);
    else
        call_ptr<void>(fn, p);
}
static bool fileSelectSameString(u32 a, u32 b) {
    if (a == b)
        return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        const u8 x = at<be<u8>>(a)[i].get(), y = at<be<u8>>(b)[i].get();
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}

/* 026CBEE0 */
static void dFile_select_c__dt(u32 p, u32 flags) {
    if (p) {
        (at<dFile_select_c>(p)->mStateInterfaceVtable = kStateInterfaceVtable);
        (at<dFile_select_c>(p)->mDrawInterfaceVtable = kDrawInterfaceVtable);
        fileSelectHD_baseDestruct(p, 0u);
        if (flags & 1)
            fileSelectHD_operatorDelete(p);
    }
}

/* 026CBF4C */
static u32 fileSelectResetFocus() { return fileSelectHD_setFocus(mInputManager, 0u); }

/* 026CBF5C */
void dFile_select_c::initializeWait() {
    const u32 p = ea(this);
    const u32 screen = mpLayout.get();
    (mMenuPane = 3);
    (mOriginalPane = 6);
    fileSelectHD_startAnimation(at<Layout>(screen)->mpAnimation.get(), 0u, 0u);
    fileSelectVirtualCall(p, 0x8C);
    (mMessage = MESSAGE_SELECT);
    fileSelectHD_fileSelectResetFocus(p);
}

/* 026CBFCC */
void dFile_select_c::clearNavigation() {
    const u32 p = ea(this);
    constexpr u32 indices[] = {6, 7, 8, 0, 1, 2, 3, 5};
    for (u32 i : indices)
        fileSelectHD_paneSetNavigation(fileSelectGetPane(p, i), 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u);
}

/* 026CC1CC */
void Transition::initialize(u32 target, u32 start, u32 end, u32 a, u32 b, u32 c, f32 speed) {
    const f32 ey = fileSelectReadFloatWord(end + 4), sx = fileSelectReadFloatWord(start);
    (mTarget.y = ey);
    (mPosition.x = sx);
    const f32 ex = fileSelectReadFloatWord(end), sy = fileSelectReadFloatWord(start + 4),
              ez = fileSelectReadFloatWord(end + 8);
    (mPosition.y = sy);
    const f32 factor = l_initialSpeedRatio;
    (mAlphaStep = c);
    (mTarget.x = ex);
    (mStage = 0);
    (mAlpha = a);
    (mCurrentSpeed = fmuls_ppc(speed, factor));
    (mTargetAlpha = b);
    (mTarget.z = ez);
    (mSpeed = speed);
    const f32 sz = fileSelectReadFloatWord(start + 8);
    (mDelay = target);
    (mPosition.z = sz);
}

/* 026CC22C */
void Motion::setTransition(u32 index, u32 from, u32 to, u32 alphaStart, u32 alphaEnd, u32 delay,
                           u32 frames) {
    const u32 p = ea(this);
    Local<Vector> start, end;
    motionVector(p, index, from, start.a);
    motionVector(p, index, to, end.a);
    const f32 x = fsubs_ppc(at<Vector>(end.a)->x.get(), at<Vector>(start.a)->x.get()),
              y = fsubs_ppc(at<Vector>(end.a)->y.get(), at<Vector>(start.a)->y.get()),
              z = fsubs_ppc(at<Vector>(end.a)->z.get(), at<Vector>(start.a)->z.get());
    const f32 yy = fmuls_ppc(y, y);
    const f32 xy = fmadds(x, x, yy);
    const f32 norm = fmadds(z, z, xy);
    const f64 distance = fileSelectHD_sqrt(f64(norm));
    const f32 count = f32(s32(frames)), delta = f32(s32(alphaEnd - alphaStart));
    const f32 alphaRate = f32(std::fabs(f64(delta)) / f64(count));
    const u32 step = u32(s32(s16(ftoi(alphaRate))));
    const f32 speed = f32(distance / f64(count));
    const u32 receiver = p + u32(offsetof(Motion, mTransition)) + (index < 4 ? 48 * index : 0);
    fileSelectHD_Transition_initialize(receiver, delay, start.a, end.a, alphaStart, alphaEnd, step,
                                       speed);
}

/* 026CC974 */
void dFile_select_c::growInit() {
    const u32 p = ea(this);
    fileSelectHD_paneSelect(fileSelectGetPane(p, PANE_BACK));
    fileSelectHD_dFile_select_c_clearNavigation(p);
    fileSelectHD_fileSelectResetFocus(p);
    const u32 count = mPaneCount.get();
    (mInputDisabled = 0);
    fileSelectVirtualCall(at<be<u32>>(mpPanes.get() + (count > 6 ? 24 : 0))[0].get(), 0x23C);
    fileSelectVirtualCall(fileSelectGetPane(p, PANE_SLOT_FIRST + 1), 0x23C);
    fileSelectVirtualCall(fileSelectGetPane(p, PANE_SLOT_FIRST + 2), 0x23C);
    const u8 mode = mUseType.get();
    const u32 screen = mpLayout.get();
    (mMessage = mode ? MESSAGE_SAVE_SELECT : MESSAGE_SELECT);
    fileSelectHD_stopAnimation(at<Layout>(screen)->mpAnimation.get(), 0u, 0u);
    for (u32 i = 0; i < 3; ++i) {
        fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                          MOTION_RIGHT, MOTION_ORIGIN, 0u, 255u, 4 * i, 8u);
        const u32 q = at<dFile_select_c>(p)->mpParts[6 + i].get(),
                  v = p + u32(offsetof(dFile_select_c, mMotion)) +
                      u32(offsetof(Motion, mTransition)) + 48 * i;
        const f32 x = fileSelectReadFloat(v);
        const u8 flag = at<Pane>(q)->mFlags.get();
        const f32 y = fileSelectReadFloat(v + 4), z = fileSelectReadFloat(v + 8);
        (at<Pane>(q)->mX = x);
        (at<Pane>(q)->mY = y);
        (at<Pane>(q)->mZ = z);
        (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
        (at<Pane>(at<dFile_select_c>(p)->mpParts[6 + i].get())->mAlpha =
             at<be<u8>>(v + u32(offsetof(Transition, mAlpha)) + 1)[0].get());
    }
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_BUSY_ORIGIN, MOTION_BUSY_OUT, 0u, 0u, 0u, 8u);
    const u32 q = mpScreenParts[3].get();
    const f32 y =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, y)));
    const u8 flag = at<Pane>(q)->mFlags.get();
    const f32 x = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, x))),
              z = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, z)));
    (at<Pane>(q)->mX = x);
    (at<Pane>(q)->mY = y);
    (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
    (at<Pane>(q)->mZ = z);
    (at<Pane>(mpScreenParts[3].get())->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
    (mDelayTimer = 0);
    fileSelectDrawScreen(p);
}

/* 026CCB8C */
u32 Transition::move() {
    const u32 p = ea(this);
    u32 stage = mStage.get();
    if (stage == 0) {
        u32 delay = mDelay.get();
        if (delay) {
            --delay;
            (mDelay = delay);
            if (s32(delay) > 0)
                return 0;
            stage = mStage.get();
        }
        (mStage = stage + 1);
        return 0;
    }
    if (stage != 1)
        return 0;
    f32 speed = fmuls_ppc(mCurrentSpeed.get(), l_acceleration);
    const f32 v = mSpeed.get(), limit = fadds_ppc(v, v);
    if (speed > limit)
        speed = limit;
    const f32 steps = l_positionStep, ratio = l_positionRatio;
    (mCurrentSpeed = speed);
    const f32 remain =
        fileSelectHD_addCalcPos(p, p + u32(offsetof(Transition, mTarget)), ratio, speed, steps);
    const bool done = remain == l_zero;
    const u32 a = u32(s32(s16(mTargetAlpha.get()))), b = u32(s32(s16(mAlphaStep.get())));
    const u32 alpha = fileSelectHD_chaseAlpha(p + u32(offsetof(Transition, mAlpha)), a, b);
    return done && alpha != 0;
}

/* 026CCC94 */
u32 dFile_select_c::updateTransitions() {
    const u32 p = ea(this);
    u32 done = 1;
    for (u32 i = 0; i < 4; ++i) {
        if (!fileSelectHD_Transition_move(p + u32(offsetof(dFile_select_c, mMotion)) +
                                          u32(offsetof(Motion, mTransition)) + 48 * i))
            done = 0;
        if (i < 3) {
            const u32 q = at<dFile_select_c>(p)->mpParts[6 + i].get(),
                      v = p + u32(offsetof(dFile_select_c, mMotion)) +
                          u32(offsetof(Motion, mTransition)) + 48 * i;
            const u8 flag = at<Pane>(q)->mFlags.get();
            const f32 x = fileSelectReadFloat(v), z = fileSelectReadFloat(v + 8),
                      y = fileSelectReadFloat(v + 4);
            (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
            (at<Pane>(q)->mZ = z);
            (at<Pane>(q)->mY = y);
            (at<Pane>(q)->mX = x);
            (at<Pane>(at<dFile_select_c>(p)->mpParts[6 + i].get())->mAlpha =
                 at<be<u8>>(v + u32(offsetof(Transition, mAlpha)) + 1)[0].get());
        } else {
            const u32 q = mpScreenParts[3].get();
            const f32 z = fileSelectReadFloat(
                p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                u32(offsetof(Vector, z)));
            const u8 flag = at<Pane>(q)->mFlags.get();
            const f32 y = fileSelectReadFloat(
                          p + u32(offsetof(dFile_select_c, mMotion)) +
                          u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                          u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, y))),
                      x = fileSelectReadFloat(
                          p + u32(offsetof(dFile_select_c, mMotion)) +
                          u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                          u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, x)));
            (at<Pane>(q)->mY = y);
            (at<Pane>(q)->mX = x);
            (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
            (at<Pane>(q)->mZ = z);
            (at<Pane>(mpScreenParts[3].get())->mAlpha =
                 at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mAlpha)) + 1)
                     ->get());
        }
    }
    return done;
}

/* 026CCE60 */
void dFile_select_c::growMove() {
    const u32 p = ea(this);
    if (fileSelectHD_dFile_select_c_updateTransitions(p))
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_SelectSaveData));
    fileSelectDrawScreen(p);
}

/* 026CCECC */
u32 dFile_select_c::slotIdle(u32 slot) {
    fileSelectHD_serviceRefresh(mpService.get());
    const u32 q = fileSelectHD_serviceGetSlotData(mpService.get(), slot);
    return q && at<SlotData>(q)->mStatus.get() == 0;
}

/* 026CCF48 */
void dFile_select_c::refreshSlots() {
    const u32 p = ea(this);
    for (u32 i = 0; i < 3; i++)
        if (fileSelectHD_dFile_select_c_slotIdle(p, i))
            fileSelectHD_savePaneRefresh(fileSelectGetPane(p, i + 6));
}

/* 026CCFD8 */
void dFile_select_c::initializeSelectSaveData() {
    const u32 p = ea(this);
    fileSelectHD_fileSelectResetFocus(p);
    fileSelectHD_dFile_select_c_refreshSlots(p);
    fileSelectHD_paneSelect(fileSelectGetPane(p, PANE_BACK));
    fileSelectHD_dFile_select_c_clearNavigation(p);
    fileSelectHD_fileSelectResetFocus(p);
    if (mUseType.get()) {
        (mMessage = MESSAGE_SAVE_SELECT);
        fileSelectHD_stopAnimation(fileSelectGetAnimation(p), 5u, 3u);
        const u32 q = fileSelectGetPane(p, PANE_CHOICE);
        call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mSetButtonState.get(), q, 0u);
        (mStage = 0);
        (mMenuPane = 3);
        (mCancelSoundPlayed = 0);
        (mInputDisabled = 0);
    } else {
        const u32 count = mPaneCount.get();
        (mMessage = MESSAGE_SELECT);
        u32 array = mpPanes.get();
        if (count > 5)
            array += 20;
        const u32 q = at<be<u32>>(array)[0].get();
        call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mSetButtonState.get(), q, 1u);
        (mInputDisabled = 0);
        (mMenuPane = 3);
        (mStage = 0);
        (mCancelSoundPlayed = 0);
    }
}

/* 026CD0F4 */
u32 dFile_select_c::takeEvent() {
    const u32 p = ea(this);
    Local<EventRecord> incoming, outgoing;
    fileSelectHD_eventQueuePop(incoming.a, mInputManager + 0x30);
    if (!fileSelectHD_eventIsValid(at<EventRecord>(incoming.a)->mKind.get()))
        return 0;
    for (u32 i = 0; i < 8; i++) {
        const u32 index = l_eventPanes[i], q = fileSelectGetPane(p, index);
        if (!q)
            continue;
        const u32 id = fileSelectHD_paneGetId(q);
        if (at<EventRecord>(incoming.a)->mId.get() != id)
            continue;
        (mInputDisabled = 0);
        const u32 event = l_eventCodes[i], target = fileSelectHD_paneGetId(q);
        fileSelectHD_eventConstruct(outgoing.a, event, target, 0u);
        fileSelectHD_eventQueuePush(mInputManager + 0x14, outgoing.a);
        fileSelectHD_eventDestruct(outgoing.a);
        return index == 5 ? 0u : event;
    }
    return 0;
}

/* 026CD200 */
void dFile_select_c::linkSlots() {
    constexpr u32 a[] = {6, 7, 8}, left[] = {8, 6, 7}, right[] = {7, 8, 6};
    for (u32 i = 0; i < 3; ++i) {
        const u32 n = mPaneCount.get(), arr = mpPanes.get(),
                  q = at<be<u32>>(arr + (n > a[i] ? 4 * a[i] : 0))[0].get(),
                  l = at<be<u32>>(arr + (n > left[i] ? 4 * left[i] : 0))[0].get(),
                  r = at<be<u32>>(arr + (n > right[i] ? 4 * right[i] : 0))[0].get();
        fileSelectHD_paneSetNavigation(q, at<Pane>(l)->mId.get(), 0u, 0u, at<Pane>(r)->mId.get(),
                                       0u, 0u, 0u, 0u);
    }
}

/* 026CD384 */
u32 dFile_select_c::selectCancel() {
    const u32 p = ea(this);
    if (!fileSelectHD_eventQueueHasCancel(mInputManager + 0x30))
        return 0;
    if (!mCancelSoundPlayed.get())
        fileSelectHD_startSound(SOUND_CANCEL);
    fileSelectHD_fileSelectResetFocus(p);
    (mCancelSoundPlayed = 1);
    fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                             ea(&l_fileSelect.StateID_OutAllEnd));
    return 1;
}

/* 026CD420 */
void dFile_select_c::selectionTransition() {
    const u32 p = ea(this);
    const f32 ten = l_slotMaxFrames;
    const f64 factor = l_slotDelayFactor;
    for (u32 i = 0; i < 3; ++i) {
        if (i == selectNum.get())
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                              MOTION_ORIGIN, MOTION_BUSY_ORIGIN, 255u, 255u, 4 * i,
                                              4u);
        else {
            const s32 n = s32(ftoi(f64(s32(i)) * factor));
            const u32 frames = ftoi(fsubs_ppc(ten, f32(n)));
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                              MOTION_ORIGIN, MOTION_RIGHT, 255u, 0u, 4 * i, frames);
        }
        const u32 q = at<dFile_select_c>(p)->mpParts[6 + i].get(),
                  v = p + u32(offsetof(dFile_select_c, mMotion)) +
                      u32(offsetof(Motion, mTransition)) + 48 * i;
        const f32 x = fileSelectReadFloat(v);
        const u8 flag = at<Pane>(q)->mFlags.get();
        const f32 y = fileSelectReadFloat(v + 4), z = fileSelectReadFloat(v + 8);
        (at<Pane>(q)->mX = x);
        (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
        (at<Pane>(q)->mY = y);
        (at<Pane>(q)->mZ = z);
        (at<Pane>(at<dFile_select_c>(p)->mpParts[6 + i].get())->mAlpha =
             at<be<u8>>(v + u32(offsetof(Transition, mAlpha)) + 1)[0].get());
    }
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_BUSY_ORIGIN, MOTION_BUSY_ORIGIN, 255u, 255u, 0u, 8u);
    const u32 q = mpScreenParts[3].get();
    const f32 x =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, x)));
    const u8 flag = at<Pane>(q)->mFlags.get();
    const f32 y = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, y))),
              z = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, z)));
    (at<Pane>(q)->mY = y);
    (at<Pane>(q)->mZ = z);
    (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
    (at<Pane>(q)->mX = x);
    (at<Pane>(mpScreenParts[3].get())->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
    fileSelectHD_stopAnimation(fileSelectGetAnimation(p), 5u, 3u);
    fileSelectHD_startAnimation(fileSelectGetAnimation(p), 3u, 2u);
}

/* 026CD628 */
u32 dFile_select_c::selectDone() {
    const u32 p = ea(this);
    if (mUseType.get())
        return 0;
    if (!fileSelectHD_dFile_select_c_slotIdle(
            p, u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get())))
        return 0;
    (selectEnd = 1);
    return 1;
}

/* 026CD694 */
void dFile_select_c::enableSlots() {
    const u32 p = ea(this);
    if (mInputDisabled.get())
        return;
    u32 a = mpPanes.get() + (mPaneCount.get() > 6 ? 24 : 0), q = at<be<u32>>(a)[0].get();
    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mEnable.get(), q, cpu->r[4], a,
                   cpu->r[6]);
    a = mpPanes.get() + (mPaneCount.get() > 7 ? 28 : 0);
    q = at<be<u32>>(a)[0].get();
    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mEnable.get(), q, cpu->r[4], cpu->r[5],
                   a);
    q = fileSelectGetPane(p, PANE_SLOT_FIRST + 2);
    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mEnable.get(), q, cpu->r[4], cpu->r[5],
                   cpu->r[6]);
}

/* 026CD740 */
void dFile_select_c::executeSelectSaveData() {
    const u32 p = ea(this);
    fileSelectDrawScreen(p);
    if (!mInputManager)
        return;
    if (!mStage.get()) {
        if (!fileSelectHD_dFile_select_c_updateTransitions(p))
            return;
        (mStage = mStage.get() + 1);
        fileSelectHD_dFile_select_c_linkSlots(p);
        const u32 q = fileSelectGetPane(p, mOriginalPane.get());
        fileSelectHD_setFocus(mInputManager, at<Pane>(q)->mId.get());
    }
    if (fileSelectHD_dFile_select_c_selectCancel(p))
        return;
    const u32 event = fileSelectHD_dFile_select_c_takeEvent(p);
    if (!event) {
        fileSelectHD_dFile_select_c_enableSlots(p);
        return;
    }
    (selectNum = event - 0x56);
    (mOriginalPane = event - 0x50);
    fileSelectHD_dFile_select_c_selectionTransition(p);
    const u32 done = fileSelectHD_dFile_select_c_selectDone(p);
    fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                             done ? ea(&l_fileSelect.StateID_WaitNameInput)
                                  : ea(&l_fileSelect.StateID_ShowSaveData));
}

/* 026CD898 */
void dFile_select_c::executeWaitNameInput() {
    const u32 p = ea(this);
    fileSelectDrawScreen(p);
}

/* 026CD8E4 */
void dFile_select_c::loadSlotData(u32 slot) {
    const u32 p = ea(this);
    fileSelectHD_serviceRefresh(mpService.get());
    const u32 data = fileSelectHD_serviceGetSlotData(mpService.get(), slot & 255);
    if (!data)
        return;
    for (u32 i = 0; i < 8; ++i)
        fileSelectHD_offTriforce(mSave + u32(offsetof(SaveView, mCollect)), i);
    for (u32 i = 0; i < 3; ++i)
        fileSelectHD_offSymbol(mSave + u32(offsetof(SaveView, mCollect)), i);
    const u32 save = mSave;
    if (!at<SlotData>(data)->mStatus.get()) {
        (at<SaveView>(save)->mMaxLife = 12);
        (at<SaveView>(mSave)->mLife = 12);
    } else {
        (at<SaveView>(save)->mMaxLife = at<SlotData>(data)->mMaxLife.get());
        const u16 health = at<SlotData>(data)->mLife.get();
        (at<SaveView>(mSave)->mLife = health);
        for (u32 i = 0; i < 8; ++i)
            if (at<SlotData>(data)->mTriforce.get() & (1u << i))
                fileSelectHD_onTriforce(mSave + u32(offsetof(SaveView, mCollect)), i);
        for (u32 i = 0; i < 3; ++i)
            if (at<SlotData>(data)->mSymbol.get() & (1u << i))
                fileSelectHD_onSymbol(mSave + u32(offsetof(SaveView, mCollect)), i);
    }
    const u32 panel = fileSelectGetPane(p, PANE_INFO);
    fileSelectHD_collectionRefresh(panel);
    fileSelectHD_heartRefresh(fileSelectGetPane(panel, PANE_BACK));
}

/* 026CDA70 */
u32 dFile_select_c::slotFlag(u32 slot) {
    fileSelectHD_serviceRefresh(mpService.get());
    const u32 q = fileSelectHD_serviceGetSlotData(mpService.get(), slot);
    return q && at<SlotData>(q)->mFlag.get() != 0;
}

/* 026CDAEC */
void dFile_select_c::enableButtons() {
    const u32 p = ea(this);
    u32 enabled = 1;
    if (fileSelectHD_dFile_select_c_slotIdle(
            p, u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get())) &&
        !fileSelectHD_dFile_select_c_slotFlag(
            p, u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get())))
        enabled = 0;
    for (u32 i = 1; i <= 2; i++) {
        const u32 q = fileSelectGetPane(p, i);
        call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mSetDisabled.get(), q, enabled);
    }
}

/* 026CDBFC */
void dFile_select_c::initializeShowSaveData() {
    const u32 p = ea(this);
    const u32 save = fileSelectHD_saveGetSlot(
        mSave + u32(offsetof(SaveView, mSlots)),
        u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get()));
    const bool present = fileSelectHD_slotGetHeroMode(save) != 0;
    const u32 q = fileSelectGetPane(p, PANE_CHOICE);
    if (present)
        fileSelectHD_difficultySelectHero(q);
    else
        fileSelectHD_difficultySelectNormal(q);
    (mMessage = mUseType.get() ? MESSAGE_SAVE_CONFIRM : MESSAGE_SELECT);
    fileSelectHD_fileSelectResetFocus(p);
    fileSelectHD_dFile_select_c_clearNavigation(p);
    const u32 index = selectNum.get();
    (mDelayTimer = (index << 2) + 4);
    if (index == 0)
        (mDelayTimer = 8);
    (mStage = 0);
    fileSelectHD_dFile_select_c_loadSlotData(p, index);
    if (!mUseType.get())
        fileSelectHD_dFile_select_c_enableButtons(p);
}

/* 026CDCF4 */
void dFile_select_c::dataLabels() {
    Local<SafeString> first, others, last, three;
    u32 array = mpPanes.get(), count = mPaneCount.get();
    for (u32 i = 0; i < 8; ++i) {
        const u32 index = i ? l_eventPanes[i] : 5,
                  q = at<be<u32>>(array + (index < count ? 4 * index : 0))[0].get();
        if (!q)
            continue;
        const u32 str = i ? others.a : first.a;
        (at<SafeString>(str)->mVtable = kSafeStringVtable);
        (at<SafeString>(str)->mpText = guest_addr("SE_DRC_TOUCH_ON"));
        fileSelectHD_paneSetTouchSound(q, str, SOUND_TOUCH);
        fileSelectHD_paneSetSelectionSound(q, ea(&l_soundArchive), SOUND_TOUCH);
        if (i)
            fileSelectHD_paneSetConfirmSound(q, ea(&l_soundArchive), SOUND_SELECT);
        array = mpPanes.get();
        count = mPaneCount.get();
    }
    u32 q = at<be<u32>>(array)[0].get();
    if (q) {
        (at<SafeString>(last.a)->mVtable = kSafeStringVtable);
        (at<SafeString>(last.a)->mpText = guest_addr("SE_DRC_TOUCH_ON"));
        fileSelectHD_paneSetTouchSound(q, last.a, SOUND_TOUCH);
        fileSelectHD_paneSetSelectionSound(q, ea(&l_soundArchive), SOUND_TOUCH);
        fileSelectHD_paneSetConfirmSound(q, ea(&l_soundArchive), SOUND_CANCEL);
        count = mPaneCount.get();
        array = mpPanes.get();
    }
    q = at<be<u32>>(array + (count > 3 ? 12 : 0))[0].get();
    if (q) {
        (at<SafeString>(three.a)->mVtable = kSafeStringVtable);
        (at<SafeString>(three.a)->mpText = guest_addr("SE_DRC_TOUCH_ON"));
        fileSelectHD_paneSetTouchSound(q, three.a, SOUND_TOUCH);
        fileSelectHD_paneSetSelectionSound(q, ea(&l_soundArchive), SOUND_TOUCH);
        (at<SafeString>(three.a)->mVtable = kSafeStringVtable);
        (at<SafeString>(three.a)->mpText = guest_addr("SE_ENTER_GAME"));
        fileSelectHD_paneSetConfirmSound(q, three.a, 0u);
    }
}

/* 026CDEE4 */
void dFile_select_c::deleteButtons() {
    const u32 p = ea(this);
    Local<SafeString> name;
    u32 q = fileSelectGetPane(p, PANE_COPY);
    (at<SafeString>(name.a)->mVtable = kSafeStringVtable);
    (at<SafeString>(name.a)->mpText = guest_addr("T_Btn_00"));
    fileSelectHD_paneSetText(q, name.a, ea(&l_fileSelect.mLabelNames[2]));
    q = fileSelectGetPane(p, PANE_BACK);
    (at<SafeString>(name.a)->mVtable = kSafeStringVtable);
    (at<SafeString>(name.a)->mpText = guest_addr("T_Btn_00"));
    fileSelectHD_paneSetText(q, name.a, ea(&l_fileSelect.mLabelNames[3]));
    u32 part = mpParts[0].get();
    (at<Pane>(part)->mFlags = (at<Pane>(part)->mFlags.get() & 0xFEu) + 1);
    part = mpParts[1].get();
    const u32 flags = (at<Pane>(part)->mFlags.get() & 0xFEu) + 1;
    const f32 f = l_animationRate;
    (at<Pane>(part)->mFlags = flags);
    fileSelectHD_playAnimation(fileSelectGetAnimation(p), 1u, 1u, f);
    fileSelectHD_dFile_select_c_dataLabels(p);
}

/* 026CDFE0 */
void dFile_select_c::choiceButtons() {
    const u32 p = ea(this);
    Local<SafeString> str;
    (at<SafeString>(str.a)->mVtable = kSafeStringVtable);
    (at<SafeString>(str.a)->mpText = guest_addr("T_Btn_00"));
    fileSelectHD_paneSetText(fileSelectGetPane(p, PANE_COPY), str.a,
                             ea(&l_fileSelect.mLabelNames[5]));
    (at<SafeString>(str.a)->mVtable = kSafeStringVtable);
    (at<SafeString>(str.a)->mpText = guest_addr("T_Btn_00"));
    fileSelectHD_paneSetText(fileSelectGetPane(p, PANE_BACK), str.a,
                             ea(&l_fileSelect.mLabelNames[4]));
    u32 q = mpParts[0].get();
    const f32 rate = l_animationRate;
    (at<Pane>(q)->mFlags = at<Pane>(q)->mFlags.get() & 254);
    q = mpParts[1].get();
    (at<Pane>(q)->mFlags = at<Pane>(q)->mFlags.get() & 254);
    fileSelectHD_playAnimation(fileSelectGetAnimation(p), 1u, 1u, rate);
    fileSelectHD_paneSetConfirmSound(fileSelectGetPane(p, PANE_COPY), ea(&l_soundArchive),
                                     SOUND_CANCEL);
    fileSelectHD_paneSetConfirmSound(fileSelectGetPane(p, PANE_BACK), ea(&l_soundArchive),
                                     SOUND_CONFIRM);
}

/* 026CE118 */
void dFile_select_c::executeShowSaveData() {
    const u32 p = ea(this);
    const u32 stage = mStage.get();
    if (stage == 0) {
        const u32 ticks = mDelayTimer.get();
        if (ticks) {
            (mDelayTimer = ticks - 1);
            if (s32(ticks - 1) <= 0) {
                if (mUseType.get())
                    fileSelectHD_dFile_select_c_choiceButtons(p);
                else
                    fileSelectHD_dFile_select_c_deleteButtons(p);
                fileSelectHD_stopAnimation(fileSelectGetAnimation(p), 5u, 3u);
                fileSelectHD_playAnimation(fileSelectGetAnimation(p), 3u, 2u, l_animationRate);
                if (mUseType.get()) {
                    const u32 q = fileSelectGetPane(p, PANE_CHOICE);
                    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mSetButtonState.get(), q,
                                   0u);
                }
                (mStage = mStage.get() + 1);
            }
        }
        fileSelectHD_dFile_select_c_updateTransitions(p);
    } else if (stage == 1) {
        if (fileSelectHD_isAnimationFinished(fileSelectGetAnimation(p), 3u))
            (mStage = mStage.get() + 1);
        fileSelectHD_dFile_select_c_updateTransitions(p);
    } else if (stage == 2) {
        if (fileSelectHD_dFile_select_c_updateTransitions(p) &&
            fileSelectHD_isAnimationFinished(fileSelectGetAnimation(p), 1u))
            fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                     mUseType.get() ? ea(&l_fileSelect.StateID_SaveEXYesNo)
                                                    : ea(&l_fileSelect.StateID_SelectDownButton));
    }
    fileSelectDrawScreen(p);
}

/* 026CE368 */
u32 dFile_select_c::buttonOne() {
    const u32 p = ea(this);
    return at<Pane>(fileSelectGetPane(p, PANE_CHOICE))->mState.get() == 1;
}

/* 026CE394 */
void dFile_select_c::initializeCancelSaveData() {
    const u32 p = ea(this);
    fileSelectHD_fileSelectResetFocus(p);
    fileSelectHD_dFile_select_c_clearNavigation(p);
    fileSelectHD_paneSelect(fileSelectGetPane(p, mOriginalPane.get()));
    if (stateKey(p, ea(&l_fileSelect.StateID_EndDelete))) {
        fileSelectHD_stopAnimation(fileSelectGetAnimation(p), 4u, 2u);
        fileSelectHD_stopAnimation(fileSelectGetAnimation(p), 2u, 1u);
    } else {
        const bool deleting = stateKey(p, ea(&l_fileSelect.StateID_EndCopy));
        const u32 anim = fileSelectGetAnimation(p);
        const f32 rate = l_animationRate;
        if (deleting) {
            fileSelectHD_stopAnimation(anim, 2u, 1u);
            fileSelectHD_playAnimation(fileSelectGetAnimation(p), 4u, 2u, rate);
        } else {
            fileSelectHD_playAnimation(anim, 4u, 2u, rate);
            fileSelectHD_playAnimation(fileSelectGetAnimation(p), 2u, 1u, rate);
        }
    }
    const u32 vt = mVtable.get();
    (mStage = 0);
    call_ptr<void>(at<Vtable>(vt)->mUpdatePanes.get(), p);
    fileSelectVirtualCall(p, 0x8C);
    const u32 answer = fileSelectHD_dFile_select_c_buttonOne(p),
              save = fileSelectHD_saveGetSlot(
                  mSave + u32(offsetof(SaveView, mSlots)),
                  u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get()));
    fileSelectHD_slotSetHeroMode(save, answer);
}

/* 026CE620 */
void dFile_select_c::executeCancelSaveData() {
    const u32 p = ea(this);
    u32 done = fileSelectHD_isAnimationFinished(fileSelectGetAnimation(p), 2u);
    if (done) {
        const u32 state = mpPreviousState.get(),
                  fn = at<Vtable>(at<State>(state)->mVtable.get())->mGetId.get(),
                  savedTable = l_fileSelect.StateID_EndDelete.mVtable.get();
        const u32 current = call_ptr<u32>(fn, state);
        const u32 wanted = call_ptr<u32>(at<Vtable>(savedTable)->mGetId.get(),
                                         ea(&l_fileSelect.StateID_EndDelete));
        if (current != wanted)
            done = fileSelectHD_isAnimationFinished(fileSelectGetAnimation(p), 4u);
    }
    if (fileSelectHD_dFile_select_c_updateTransitions(p) && done)
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_SelectSaveData));
    fileSelectDrawScreen(p);
}

/* 026CE728 */
void dFile_select_c::linkButtonsIn() {
    constexpr u32 rows[][5] = {{0, 5, 5, 3, 1}, {1, 5, 5, 0, 2}, {2, 5, 5, 1, 3}, {3, 5, 5, 2, 0}};
    for (auto& r : rows) {
        const u32 n = mPaneCount.get(), arr = mpPanes.get();
        const auto pane = [&](u32 i) { return at<be<u32>>(arr + (i < n ? 4 * i : 0))[0].get(); };
        const u32 q = pane(r[0]), a = pane(r[1]), b = pane(r[2]), c = pane(r[3]), d = pane(r[4]);
        fileSelectHD_paneSetNavigation(q, at<Pane>(a)->mId.get(), 0u, 0u, at<Pane>(b)->mId.get(),
                                       0u, 0u, at<Pane>(c)->mId.get(), at<Pane>(d)->mId.get());
    }
    const u32 n = mPaneCount.get(), arr = mpPanes.get(),
              q = at<be<u32>>(arr + (n > 5 ? 20 : 0))[0].get(),
              a = at<be<u32>>(arr + (n > 3 ? 12 : 0))[0].get();
    fileSelectHD_paneSetNavigation(q, at<Pane>(a)->mId.get(), 0u, 0u, at<Pane>(a)->mId.get(), 0u,
                                   0u, 0u, 0u);
}

/* 026CEA60 */
void dFile_select_c::initializeSelectDownButton() {
    const u32 p = ea(this);
    fileSelectHD_dFile_select_c_linkButtonsIn(p);
    (mMessage = MESSAGE_SELECT);
    fileSelectHD_dFile_select_c_linkButtonsIn(p);
    const u32 q = fileSelectGetPane(p, mMenuPane.get());
    const u32 sound = mInputManager;
    fileSelectHD_setFocus(sound, at<Pane>(q)->mId.get());
    (mInputDisabled = 0);
}

/* 026CEAD4 */
void dFile_select_c::animateSlotsIn() {
    const u32 p = ea(this);
    u32 delay = 4;
    for (u32 i = 0; i < 3; ++i) {
        if (i == selectNum.get())
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                              MOTION_BUSY_ORIGIN, MOTION_ORIGIN, 255u, 255u, 0u,
                                              8u);
        else {
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                              MOTION_RIGHT, MOTION_ORIGIN, 0u, 255u, delay, 8u);
            delay += 4;
        }
        const u32 q = at<dFile_select_c>(p)->mpParts[6 + i].get(),
                  v = p + u32(offsetof(dFile_select_c, mMotion)) +
                      u32(offsetof(Motion, mTransition)) + 48 * i;
        const f32 x = fileSelectReadFloat(v);
        const u8 flag = at<Pane>(q)->mFlags.get();
        const f32 y = fileSelectReadFloat(v + 4), z = fileSelectReadFloat(v + 8);
        (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
        (at<Pane>(q)->mZ = z);
        (at<Pane>(q)->mY = y);
        (at<Pane>(q)->mX = x);
        (at<Pane>(at<dFile_select_c>(p)->mpParts[6 + i].get())->mAlpha =
             at<be<u8>>(v + u32(offsetof(Transition, mAlpha)) + 1)[0].get());
    }
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_BUSY_ORIGIN, MOTION_BUSY_ORIGIN, 255u, 255u, 0u, 8u);
    const u32 q = mpScreenParts[3].get();
    const f32 z =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, z)));
    const u8 flag = at<Pane>(q)->mFlags.get();
    const f32 y = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, y))),
              x = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, x)));
    (at<Pane>(q)->mY = y);
    (at<Pane>(q)->mX = x);
    (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
    (at<Pane>(q)->mZ = z);
    (at<Pane>(mpScreenParts[3].get())->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
}

/* 026CEC18 */
void dFile_select_c::enableSelection() {
    const u32 p = ea(this);
    if (!mInputDisabled.get()) {
        for (u32 i = 0; i < 4; ++i)
            fileSelectVirtualCall(fileSelectGetPane(p, i), 0xCC);
        fileSelectVirtualCall(fileSelectGetPane(p, PANE_CHOICE), 0xCC);
    }
}

/* 026CED0C */
void dFile_select_c::executeSelectDownButton() {
    const u32 p = ea(this);
    fileSelectDrawScreen(p);
    if (!mInputManager)
        return;
    const u32 state = fileSelectHD_getState(fileSelectGetPane(p, PANE_CHOICE) + 0x18),
              fn = at<Vtable>(at<State>(state)->mVtable.get())->mGetId.get(),
              saved = l_difficultySelectionState.mVtable.get();
    const u32 a = call_ptr<u32>(fn, state),
              b = call_ptr<u32>(at<Vtable>(saved)->mGetId.get(), ea(&l_difficultySelectionState));
    if (a != b && fileSelectHD_eventQueueHasCancel(mInputManager + 0x30)) {
        fileSelectHD_playAnimation(fileSelectGetAnimation(p), 4u, 2u, l_animationRate);
        fileSelectHD_dFile_select_c_animateSlotsIn(p);
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_CancelSaveData));
        fileSelectHD_startSound(SOUND_CANCEL);
        return;
    }
    const u32 event = fileSelectHD_dFile_select_c_takeEvent(p);
    if (!event) {
        fileSelectHD_dFile_select_c_enableSelection(p);
        return;
    }
    fileSelectHD_fileSelectResetFocus(p);
    fileSelectHD_dFile_select_c_clearNavigation(p);
    switch (event) {
    case EVENT_BACK:
        fileSelectHD_dFile_select_c_animateSlotsIn(p);
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_CancelSaveData));
        break;
    case EVENT_DELETE:
        (mMenuPane = 1);
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_DecideDeleteYesNo));
        break;
    case EVENT_COPY:
        (mMenuPane = 2);
        (mSelectedPane = 6);
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_SelectFileCopyTo));
        break;
    case EVENT_ACCEPT:
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_DecideSaveData));
        break;
    }
}

/* 026CEF90 */
void dFile_select_c::executeDecideSaveData() { (mClose = 1); }

/* 026CEF9C */
void dFile_select_c::outInit() {
    const u32 p = ea(this);
    const f32 rate = l_animationRate;
    fileSelectHD_playAnimation(fileSelectGetAnimation(p), 6u, 0u, rate);
    fileSelectHD_fileSelectResetFocus(p);
    fileSelectHD_dFile_select_c_clearNavigation(p);
}

/* 026CEFF0 */
void dFile_select_c::outMove() {
    const u32 p = ea(this);
    if (fileSelectHD_isAnimationFinished(fileSelectGetAnimation(p), 6u))
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_OutAll));
    fileSelectDrawScreen(p);
}

/* 026CF068 */
void dFile_select_c::linkButtons() {
    u32 count = mPaneCount.get(), array = mpPanes.get();
    u32 a = at<be<u32>>(array + (count > 2 ? 8 : 0))[0].get(),
        b = at<be<u32>>(array + (count > 3 ? 12 : 0))[0].get();
    fileSelectHD_paneSetNavigation(a, 0u, 0u, 0u, 0u, 0u, 0u, 0u, at<Pane>(b)->mId.get());
    count = mPaneCount.get();
    array = mpPanes.get();
    a = at<be<u32>>(array + (count > 3 ? 12 : 0))[0].get();
    b = at<be<u32>>(array + (count > 2 ? 8 : 0))[0].get();
    fileSelectHD_paneSetNavigation(a, 0u, 0u, 0u, 0u, 0u, 0u, at<Pane>(b)->mId.get(), 0u);
}

/* 026CF144 */
void dFile_select_c::initializeDecideDeleteYesNo() {
    const u32 p = ea(this);
    fileSelectHD_dFile_select_c_linkButtons(p);
    (mMessage = MESSAGE_DELETE_CONFIRM);
    fileSelectHD_dFile_select_c_choiceButtons(p);
    const u32 count = mPaneCount.get();
    (mStage = 0);
    u32 array = mpPanes.get();
    (mInputDisabled = 0);
    if (count > 5)
        array += 20;
    const u32 q = at<be<u32>>(array)[0].get();
    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mSetButtonState.get(), q, 0u);
}

/* 026CF1B8 */
u32 dFile_select_c::copySound() {
    const u32 p = ea(this);
    const u32 q = fileSelectGetPane(p, PANE_COPY), sound = mInputManager;
    return fileSelectHD_setFocus(sound, at<Pane>(q)->mId.get());
}

/* 026CF1E0 */
void dFile_select_c::enableDelete() {
    const u32 p = ea(this);
    if (mInputDisabled.get())
        return;
    for (u32 i = 2; i <= 3; i++) {
        const u32 q = fileSelectGetPane(p, i);
        call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mEnable.get(), q);
    }
}

/* 026CF264 */
void dFile_select_c::executeDecideDeleteYesNo() {
    const u32 p = ea(this);
    if (!mStage.get()) {
        (mStage = 1);
        fileSelectHD_dFile_select_c_copySound(p);
    }
    const u32 event = fileSelectHD_dFile_select_c_takeEvent(p);
    const bool cancel = event == EVENT_COPY, accept = event == EVENT_ACCEPT;
    if (!event)
        fileSelectHD_dFile_select_c_enableDelete(p);
    const bool escape = fileSelectHD_eventQueueHasCancel(mInputManager + 0x30) != 0;
    if (escape || cancel) {
        fileSelectHD_paneSelect(fileSelectGetPane(p, PANE_DELETE));
        fileSelectHD_dFile_select_c_animateSlotsIn(p);
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_CancelSaveData));
        if (!cancel)
            fileSelectHD_startSound(SOUND_CANCEL);
        return;
    }
    if (accept)
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_WaitDeleting));
    fileSelectDrawScreen(p);
}

/* 026CF3EC */
u32 dFile_select_c::finalizeDecideDeleteYesNo() {
    const u32 p = ea(this);
    const f32 f = l_animationRate;
    return fileSelectHD_playAnimation(fileSelectGetAnimation(p), 2u, 1u, f);
}

/* 026CF408 */
void dFile_select_c::initializeWaitDeleting() {
    const u32 p = ea(this);
    const u32 manager = fileSelectHD_getInputManager();
    fileSelectHD_setInputEnabled(manager, 0u);
    (at<NamePane>(fileSelectGetPane(p, PANE_NAME))->mBusy = 1);
    fileSelectHD_deleteSlot(
        mSaveManager, u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get()));
    fileSelectHD_dFile_select_c_clearNavigation(p);
    fileSelectHD_fileSelectResetFocus(p);
    (mMessage = MESSAGE_DELETING);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_BUSY_ORIGIN, MOTION_BUSY_OUT, 255u, 0u, 0u, 10u);
    busyPosition(p, false);
    (mDelayTimer = 30);
}

/* 026CF4E0 */
void dFile_select_c::executeWaitDeleting() {
    const u32 p = ea(this);
    waitService(p, ea(&l_fileSelect.StateID_EndDelete));
}

/* 026CF588 */
void dFile_select_c::initializeEndDelete() {
    const u32 p = ea(this);
    const u32 manager = fileSelectHD_getInputManager();
    fileSelectHD_setInputEnabled(manager, 1u);
    (at<NamePane>(fileSelectGetPane(p, PANE_NAME))->mBusy = 0);
    const u32 q = fileSelectGetPane(p, PANE_BACK);
    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mReset.get(), q);
    fileSelectHD_difficultySelectNormal(fileSelectGetPane(p, PANE_CHOICE));
    (mMessage = MESSAGE_DELETE_END);
    fileSelectHD_startSound(SOUND_COMPLETE);
    const u32 index = selectNum.get();
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), index,
                                      MOTION_BUSY_ORIGIN, MOTION_BUSY_OUT, 255u, 0u, 0u, 10u);
    (mStage = 0);
}

/* 026CF660 */
void dFile_select_c::executeEndDelete() {
    const u32 p = ea(this);
    const u32 stage = mStage.get();
    if (stage == 0) {
        if (fileSelectHD_dFile_select_c_updateTransitions(p)) {
            const u32 next = mStage.get() + 1, service = mpService.get();
            (mStage = next);
            fileSelectHD_serviceRefresh(service);
            fileSelectHD_savePaneReload(fileSelectGetPane(p, selectNum.get() + 6));
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)),
                                              selectNum.get(), MOTION_BUSY_OUT, MOTION_BUSY_ORIGIN,
                                              0u, 255u, 0u, 8u);
        }
    } else if (stage == 1) {
        if (fileSelectHD_dFile_select_c_updateTransitions(p)) {
            const u32 next = mStage.get() + 1;
            (mCompletionTimer = 30);
            (mStage = next);
        }
    } else if (stage == 2) {
        const u32 ticks = mCompletionTimer.get();
        if (ticks) {
            const u32 vt = mVtable.get();
            (mCompletionTimer = ticks - 1);
            call_ptr<void>(at<Vtable>(vt)->mUpdatePanes.get(), p);
            fileSelectVirtualCall(p, 0x8C);
            return;
        }
        fileSelectHD_dFile_select_c_animateSlotsIn(p);
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_CancelSaveData));
    }
    fileSelectDrawScreen(p);
}

/* 026CF838 */
void dFile_select_c::copyChoicesInit() {
    const u32 p = ea(this);
    u32 seen = 0, from = 5, to = 3;
    for (u32 i = 0; i < 3; ++i) {
        if (i == selectNum.get())
            continue;
        u32 delay;
        if (!seen) {
            delay = 4;
            (mFirstCopySlot = i);
            seen = 1;
        } else {
            from = 6;
            to = 4;
            delay = seen * 4;
            (mSecondCopySlot = i);
        }
        fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i, from, to,
                                          0u, 255u, delay, 8u);
        const u32 q = at<dFile_select_c>(p)->mpParts[6 + i].get(),
                  v = p + u32(offsetof(dFile_select_c, mMotion)) +
                      u32(offsetof(Motion, mTransition)) + 48 * i;
        const f32 z = fileSelectReadFloat(v + 8);
        const u8 flag = at<Pane>(q)->mFlags.get();
        const f32 y = fileSelectReadFloat(v + 4), x = fileSelectReadFloat(v);
        (at<Pane>(q)->mY = y);
        (at<Pane>(q)->mX = x);
        (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
        (at<Pane>(q)->mZ = z);
        const u32 again = at<dFile_select_c>(p)->mpParts[6 + i].get();
        const u8 alpha = at<be<u8>>(v + u32(offsetof(Transition, mAlpha)) + 1)[0].get();
        ++seen;
        (at<Pane>(again)->mAlpha = alpha);
    }
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_BUSY_ORIGIN, MOTION_BUSY_OUT, 255u, 0u, 0u, 10u);
    const u32 q = mpScreenParts[3].get();
    const f32 z =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, z)));
    const u8 flag = at<Pane>(q)->mFlags.get();
    const f32 y = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, y))),
              x = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, x)));
    (at<Pane>(q)->mY = y);
    (at<Pane>(q)->mX = x);
    (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
    (at<Pane>(q)->mZ = z);
    (at<Pane>(mpScreenParts[3].get())->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
}

/* 026CF9B4 */
void dFile_select_c::initializeSelectFileCopyTo() {
    const u32 p = ea(this);
    const f32 f = l_animationRate;
    fileSelectHD_playAnimation(fileSelectGetAnimation(p), 2u, 1u, f);
    fileSelectHD_dFile_select_c_copyChoicesInit(p);
    (mMessage = MESSAGE_COPY_SELECT);
    (mStage = 0);
    fileSelectHD_dFile_select_c_clearNavigation(p);
    fileSelectHD_fileSelectResetFocus(p);
    (mInputDisabled = 0);
}

/* 026CFA2C */
void dFile_select_c::linkCopyChoices() {
    const u32 second = mSecondCopySlot.get() + 6, first = mFirstCopySlot.get() + 6;
    const u32 n = mPaneCount.get(), array = mpPanes.get(),
              q = at<be<u32>>(array + (first < n ? 4 * first : 0))[0].get(),
              other = at<be<u32>>(array + (second < n ? 4 * second : 0))[0].get();
    fileSelectHD_paneSetNavigation(q, at<Pane>(other)->mId.get(), 0u, 0u,
                                   at<Pane>(other)->mId.get(), 0u, 0u, 0u, 0u);
    const u32 b = mSecondCopySlot.get() + 6, n2 = mPaneCount.get(), ar2 = mpPanes.get(),
              q2 = at<be<u32>>(ar2 + (b < n2 ? 4 * b : 0))[0].get(),
              a2 = at<be<u32>>(ar2 + (first < n2 ? 4 * first : 0))[0].get();
    fileSelectHD_paneSetNavigation(q2, at<Pane>(a2)->mId.get(), 0u, 0u, at<Pane>(a2)->mId.get(), 0u,
                                   0u, 0u, 0u);
}

/* 026CFB74 */
u32 dFile_select_c::cancelCopy() {
    const u32 p = ea(this);
    if (!fileSelectHD_eventQueueHasCancel(mInputManager + 0x30))
        return 0;
    fileSelectHD_dFile_select_c_clearNavigation(p);
    fileSelectHD_fileSelectResetFocus(p);
    fileSelectHD_paneSelect(fileSelectGetPane(p, mOriginalPane.get()));
    fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                             ea(&l_fileSelect.StateID_SelectSaveData));
    fileSelectHD_startSound(SOUND_CANCEL);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), selectNum.get(),
                                      MOTION_BUSY_ORIGIN, MOTION_ORIGIN, 255u, 255u, 0u, 8u);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)),
                                      mFirstCopySlot.get(), MOTION_FIRST, MOTION_ORIGIN, 255u, 255u,
                                      0u, 8u);
    restoreIndexed(p, 0x1C4, mFirstCopySlot.get(), true);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)),
                                      mSecondCopySlot.get(), MOTION_SECOND, MOTION_ORIGIN, 255u,
                                      255u, 0u, 8u);
    restoreIndexed(p, 0x1C8, mSecondCopySlot.get(), true);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_BUSY_OUT, MOTION_BUSY_OUT, 0u, 0u, 0u, 8u);
    const u32 q = mpScreenParts[3].get();
    const f32 z =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, z)));
    const u8 flag = at<Pane>(q)->mFlags.get();
    const f32 x = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, x))),
              y = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, y)));
    (at<Pane>(q)->mX = x);
    (at<Pane>(q)->mZ = z);
    (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
    (at<Pane>(q)->mY = y);
    (at<Pane>(mpScreenParts[3].get())->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
    return 1;
}

/* 026CFF10 */
void dFile_select_c::enableCopySlots() {
    const u32 p = ea(this);
    if (mInputDisabled.get())
        return;
    const u32 second = mSecondCopySlot.get() + 6, first = mFirstCopySlot.get() + 6;
    u32 q = fileSelectGetPane(p, first);
    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mEnable.get(), q);
    q = fileSelectGetPane(p, second);
    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mEnable.get(), q);
}

/* 026CFFB4 */
void dFile_select_c::executeSelectFileCopyTo() {
    const u32 p = ea(this);
    fileSelectDrawScreen(p);
    if (!mInputManager)
        return;
    const u32 stage = mStage.get();
    if (stage == 0) {
        if (!fileSelectHD_dFile_select_c_updateTransitions(p))
            return;
        fileSelectHD_dFile_select_c_linkCopyChoices(p);
        u32 q = fileSelectGetPane(p, mSelectedPane.get());
        fileSelectHD_setFocus(mInputManager, at<Pane>(q)->mId.get());
        const u32 next = mStage.get() + 1, idx = mFirstCopySlot.get() + 6, n = mPaneCount.get();
        (mStage = next);
        q = at<be<u32>>(mpPanes.get() + (idx < n ? 4 * idx : 0))[0].get();
        fileSelectHD_setFocus(mInputManager, at<Pane>(q)->mId.get());
    } else if (stage == 1) {
        if (fileSelectHD_dFile_select_c_cancelCopy(p))
            return;
        const u32 event = fileSelectHD_dFile_select_c_takeEvent(p);
        if (event) {
            (mSelectedPane = event - 0x50);
            fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                     ea(&l_fileSelect.StateID_DecideCopyYesNo));
        } else
            fileSelectHD_dFile_select_c_enableCopySlots(p);
    }
}

/* 026D0130 */
void dFile_select_c::copyConfirmInit() {
    const u32 p = ea(this);
    const u32 selected = mSelectedPane.get() - 6, side = selected == mSecondCopySlot.get() ? 4 : 3;
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), selected, side,
                                      MOTION_COPY_ORIGIN, 255u, 255u, 0u, 8u);
    restoreIndexed(p, 0x1D4, selected, false);
    u32 other = mFirstCopySlot.get(), from = 3, to = 5;
    if (selected == other) {
        other = mSecondCopySlot.get();
        from = 4;
        to = 6;
    }
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), other, from, to,
                                      255u, 0u, 0u, 10u);
    const u32 q = partIndex(p, other + 6), v = p + u32(offsetof(dFile_select_c, mMotion)) +
                                               u32(offsetof(Motion, mTransition)) +
                                               (other < 4 ? 48 * other : 0);
    const u8 flag = at<Pane>(q)->mFlags.get();
    const f32 x = fileSelectReadFloat(v), y = fileSelectReadFloat(v + 4),
              z = fileSelectReadFloat(v + 8);
    (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
    (at<Pane>(q)->mY = y);
    (at<Pane>(q)->mZ = z);
    (at<Pane>(q)->mX = x);
    (at<Pane>(partIndex(p, other + 6))->mAlpha =
         at<be<u8>>(v + u32(offsetof(Transition, mAlpha)) + 1)[0].get());
    fileSelectHD_dFile_select_c_loadSlotData(p, selected);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_COPY_OUT, MOTION_COPY_ORIGIN, 0u, 255u, 0u, 8u);
    const u32 busy = mpScreenParts[3].get();
    const f32 bz =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, z)));
    const u8 bf = at<Pane>(busy)->mFlags.get();
    const f32 by = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, y))),
              bx = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, x)));
    (at<Pane>(busy)->mY = by);
    (at<Pane>(busy)->mX = bx);
    (at<Pane>(busy)->mFlags = bf | PANE_POSITION_DIRTY);
    (at<Pane>(busy)->mZ = bz);
    (at<Pane>(mpScreenParts[3].get())->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
}

/* 026D0420 */
void dFile_select_c::initializeDecideCopyYesNo() {
    const u32 p = ea(this);
    fileSelectHD_startAnimation(fileSelectGetAnimation(p), 5u, 3u);
    fileSelectHD_fileSelectResetFocus(p);
    fileSelectHD_dFile_select_c_clearNavigation(p);
    fileSelectHD_dFile_select_c_choiceButtons(p);
    fileSelectHD_dFile_select_c_copyConfirmInit(p);
    (mMessage = MESSAGE_COPY_CONFIRM);
    (mStage = 0);
    (mInputDisabled = 0);
}

/* 026D0490 */
void dFile_select_c::copyCancelInit() {
    const u32 p = ea(this);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_COPY_ORIGIN, MOTION_COPY_OUT, 255u, 0u, 0u, 10u);
    const u32 busy = mpScreenParts[3].get();
    const f32 z =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, z)));
    const u8 flags = at<Pane>(busy)->mFlags.get();
    const f32 x = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, x))),
              y = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, y)));
    (at<Pane>(busy)->mX = x);
    (at<Pane>(busy)->mY = y);
    (at<Pane>(busy)->mFlags = flags | PANE_POSITION_DIRTY);
    (at<Pane>(busy)->mZ = z);
    (at<Pane>(mpScreenParts[3].get())->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
    const u32 selected = mSelectedPane.get() - 6;
    for (u32 i = 0; i < 3; ++i) {
        const u32 original = selectNum.get();
        if (i == original)
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), original,
                                              MOTION_BUSY_ORIGIN, MOTION_ORIGIN, 255u, 255u, 0u,
                                              8u);
        else if (i == selected)
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), selected,
                                              MOTION_COPY_ORIGIN, MOTION_ORIGIN, 255u, 255u, 0u,
                                              8u);
        else
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                              MOTION_RIGHT, MOTION_ORIGIN, 0u, 255u, 4u, 8u);
        const u32 q = at<dFile_select_c>(p)->mpParts[6 + i].get(),
                  v = p + u32(offsetof(dFile_select_c, mMotion)) +
                      u32(offsetof(Motion, mTransition)) + 48 * i;
        const f32 xx = fileSelectReadFloat(v);
        const u8 flag = at<Pane>(q)->mFlags.get();
        const f32 zz = fileSelectReadFloat(v + 8), yy = fileSelectReadFloat(v + 4);
        (at<Pane>(q)->mX = xx);
        (at<Pane>(q)->mZ = zz);
        (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
        (at<Pane>(q)->mY = yy);
        (at<Pane>(at<dFile_select_c>(p)->mpParts[6 + i].get())->mAlpha =
             at<be<u8>>(v + u32(offsetof(Transition, mAlpha)) + 1)[0].get());
    }
}

/* 026D0608 */
void dFile_select_c::executeDecideCopyYesNo() {
    const u32 p = ea(this);
    fileSelectDrawScreen(p);
    const u32 stage = mStage.get();
    if (stage == 0) {
        if (fileSelectHD_dFile_select_c_updateTransitions(p)) {
            (mStage = mStage.get() + 1);
            fileSelectHD_dFile_select_c_linkButtons(p);
        }
    } else if (stage == 1) {
        fileSelectHD_dFile_select_c_copySound(p);
        (mStage = mStage.get() + 1);
    }
    const u32 event = fileSelectHD_dFile_select_c_takeEvent(p);
    const bool cancel = event == EVENT_COPY, accept = event == EVENT_ACCEPT;
    if (!event)
        fileSelectHD_dFile_select_c_enableDelete(p);
    const bool escape = fileSelectHD_eventQueueHasCancel(mInputManager + 0x30) != 0;
    if (escape || cancel) {
        fileSelectHD_paneSelect(fileSelectGetPane(p, mSelectedPane.get()));
        fileSelectHD_paneSelect(fileSelectGetPane(p, mOriginalPane.get()));
        fileSelectHD_fileSelectResetFocus(p);
        fileSelectHD_dFile_select_c_copyCancelInit(p);
        const u32 answer = fileSelectHD_dFile_select_c_buttonOne(p),
                  save = fileSelectHD_saveGetSlot(
                      mSave + u32(offsetof(SaveView, mSlots)),
                      u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get()));
        fileSelectHD_slotSetHeroMode(save, answer);
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_SelectSaveData));
        if (!cancel)
            fileSelectHD_startSound(SOUND_CANCEL);
    } else if (accept)
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_WaitCopying));
}

/* 026D0848 */
u32 dFile_select_c::finalizeDecideCopyYesNo() {
    const u32 p = ea(this);
    const f32 f = l_animationRate;
    return fileSelectHD_playAnimation(fileSelectGetAnimation(p), 2u, 1u, f);
}

/* 026D0864 */
void dFile_select_c::initializeWaitCopying() {
    const u32 p = ea(this);
    const u32 manager = fileSelectHD_getInputManager();
    fileSelectHD_setInputEnabled(manager, 0u);
    (at<NamePane>(fileSelectGetPane(p, PANE_NAME))->mBusy = 1);
    const u32 answer = fileSelectHD_dFile_select_c_buttonOne(p);
    const u32 save = fileSelectHD_saveGetSlot(
        mSave + u32(offsetof(SaveView, mSlots)),
        u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get()));
    fileSelectHD_slotSetHeroMode(save, answer);
    const u32 index = mSelectedPane.get(),
              selected = at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get();
    fileSelectHD_copySlot(mSaveManager, (index - 6) & 255u, selected);
    fileSelectHD_fileSelectResetFocus(p);
    (mMessage = MESSAGE_COPYING);
    (mCompletionTimer = 0);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_COPY_ORIGIN, MOTION_COPY_OUT, 255u, 0u, 0u, 10u);
    busyPosition(p, false);
    (mDelayTimer = 30);
}

/* 026D0978 */
void dFile_select_c::executeWaitCopying() {
    const u32 p = ea(this);
    waitService(p, ea(&l_fileSelect.StateID_EndCopy));
}

/* 026D0A20 */
void dFile_select_c::initializeEndCopy() {
    const u32 p = ea(this);
    fileSelectHD_setInputEnabled(fileSelectHD_getInputManager(), 1u);
    (at<NamePane>(fileSelectGetPane(p, PANE_NAME))->mBusy = 0);
    fileSelectVirtualCall(fileSelectGetPane(p, PANE_BACK), 0x23C);
    (mMessage = MESSAGE_COPY_END);
    fileSelectHD_startSound(SOUND_COMPLETE);
    fileSelectHD_serviceRefresh(mpService.get());
    fileSelectHD_savePaneReload(fileSelectGetPane(p, mSelectedPane.get()));
    fileSelectHD_savePaneUpdate(fileSelectGetPane(p, mSelectedPane.get()));
    const u32 index = mSelectedPane.get() - 6;
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), index,
                                      MOTION_BUSY_ORIGIN, MOTION_COPY_ORIGIN, 0u, 255u, 0u, 8u);
    restoreIndexed(p, 0x1D4, index, false);
    fileSelectHD_dFile_select_c_loadSlotData(p, mSelectedPane.get() - 6);
    (mStage = 0);
}

/* 026D0C14 */
void dFile_select_c::deleteSlotsIn() {
    const u32 p = ea(this);
    u32 delay = 4;
    for (u32 i = 0; i < 3; ++i) {
        if (i == selectNum.get())
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                              MOTION_BUSY_ORIGIN, MOTION_ORIGIN, 255u, 255u, 0u,
                                              8u);
        else if (i == mSelectedPane.get() - 6)
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                              MOTION_COPY_ORIGIN, MOTION_ORIGIN, 255u, 255u, 0u,
                                              8u);
        else {
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                              MOTION_RIGHT, MOTION_ORIGIN, 0u, 255u, delay, 8u);
            delay += 4;
        }
        const u32 q = at<dFile_select_c>(p)->mpParts[6 + i].get(),
                  v = p + u32(offsetof(dFile_select_c, mMotion)) +
                      u32(offsetof(Motion, mTransition)) + 48 * i;
        const f32 x = fileSelectReadFloat(v);
        const u8 flag = at<Pane>(q)->mFlags.get();
        const f32 y = fileSelectReadFloat(v + 4), z = fileSelectReadFloat(v + 8);
        (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
        (at<Pane>(q)->mZ = z);
        (at<Pane>(q)->mY = y);
        (at<Pane>(q)->mX = x);
        (at<Pane>(at<dFile_select_c>(p)->mpParts[6 + i].get())->mAlpha =
             at<be<u8>>(v + u32(offsetof(Transition, mAlpha)) + 1)[0].get());
    }
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_COPY_ORIGIN, MOTION_COPY_ORIGIN, 255u, 255u, 0u, 8u);
    const u32 q = mpScreenParts[3].get();
    const f32 z =
        fileSelectReadFloat(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, z)));
    const u8 flag = at<Pane>(q)->mFlags.get();
    const f32 y = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, y))),
              x = fileSelectReadFloat(
                  p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                  3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                  u32(offsetof(Vector, x)));
    (at<Pane>(q)->mY = y);
    (at<Pane>(q)->mX = x);
    (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
    (at<Pane>(q)->mZ = z);
    (at<Pane>(mpScreenParts[3].get())->mAlpha =
         at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                    u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                    u32(offsetof(Transition, mAlpha)) + 1)
             ->get());
}

/* 026D0D84 */
void dFile_select_c::executeEndCopy() {
    const u32 p = ea(this);
    const u32 stage = mStage.get();
    if (stage == 0) {
        if (fileSelectHD_dFile_select_c_updateTransitions(p)) {
            (mStage = mStage.get() + 1);
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                              MOTION_COPY_ORIGIN, MOTION_COPY_ORIGIN, 255u, 255u,
                                              0u, 8u);
            const f32 x = fileSelectReadFloat(
                          p + u32(offsetof(dFile_select_c, mMotion)) +
                          u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                          u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, x))),
                      y = fileSelectReadFloat(
                          p + u32(offsetof(dFile_select_c, mMotion)) +
                          u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                          u32(offsetof(Transition, mPosition)) + u32(offsetof(Vector, y)));
            const u32 q = mpScreenParts[3].get();
            const f32 z = fileSelectReadFloat(
                p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mTransition)) +
                3 * u32(sizeof(Transition)) + u32(offsetof(Transition, mPosition)) +
                u32(offsetof(Vector, z)));
            const u8 flag = at<Pane>(q)->mFlags.get();
            (at<Pane>(q)->mX = x);
            (at<Pane>(q)->mZ = z);
            (at<Pane>(q)->mFlags = flag | PANE_POSITION_DIRTY);
            (at<Pane>(q)->mY = y);
            (at<Pane>(mpScreenParts[3].get())->mAlpha =
                 at<be<u8>>(p + u32(offsetof(dFile_select_c, mMotion)) +
                            u32(offsetof(Motion, mTransition)) + 3 * u32(sizeof(Transition)) +
                            u32(offsetof(Transition, mAlpha)) + 1)
                     ->get());
            fileSelectHD_stopAnimation(fileSelectGetAnimation(p), 5u, 3u);
            fileSelectHD_playAnimation(fileSelectGetAnimation(p), 3u, 2u, l_animationRate);
        }
    } else if (stage == 1) {
        if (fileSelectHD_isAnimationFinished(fileSelectGetAnimation(p), 3u)) {
            const u32 next = mStage.get() + 1;
            (mCompletionTimer = 30);
            (mStage = next);
        }
    } else if (stage == 2) {
        u32 ticks = mCompletionTimer.get();
        if (ticks) {
            --ticks;
            (mCompletionTimer = ticks);
        }
        if (s32(ticks) <= 0) {
            fileSelectHD_paneSelect(fileSelectGetPane(p, mSelectedPane.get()));
            fileSelectHD_dFile_select_c_deleteSlotsIn(p);
            fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                     ea(&l_fileSelect.StateID_CancelSaveData));
        }
    }
    fileSelectDrawScreen(p);
}

/* 026D0F94 */
u32 dFile_select_c::backSound() {
    const u32 p = ea(this);
    const u32 q = fileSelectGetPane(p, PANE_BACK), sound = mInputManager;
    return fileSelectHD_setFocus(sound, at<Pane>(q)->mId.get());
}

/* 026D0FBC */
void dFile_select_c::initializeSaveEXYesNo() {
    const u32 p = ea(this);
    (mMessage = MESSAGE_SAVE_CONFIRM);
    (mInputDisabled = 0);
    fileSelectHD_dFile_select_c_linkButtons(p);
    fileSelectHD_dFile_select_c_backSound(p);
    (mStage = 0);
}

/* 026D100C */
void dFile_select_c::executeSaveEXYesNo() {
    const u32 p = ea(this);
    fileSelectDrawScreen(p);
    if (!mInputManager)
        return;
    const u32 event = fileSelectHD_dFile_select_c_takeEvent(p);
    const bool cancel = event == EVENT_COPY, accept = event == EVENT_ACCEPT;
    if (!event)
        fileSelectHD_dFile_select_c_enableDelete(p);
    const bool escape = fileSelectHD_eventQueueHasCancel(mInputManager + 0x30) != 0;
    if (escape || cancel) {
        fileSelectHD_paneSelect(fileSelectGetPane(p, PANE_DELETE));
        fileSelectHD_dFile_select_c_animateSlotsIn(p);
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_CancelSaveData));
        if (!cancel)
            fileSelectHD_startSound(SOUND_CANCEL);
        return;
    }
    if (accept)
        fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                 ea(&l_fileSelect.StateID_SavingEX));
    fileSelectDrawScreen(p);
}

/* 026D11AC */
void dFile_select_c::initializeSavingEX() {
    const u32 p = ea(this);
    const u32 manager = fileSelectHD_getInputManager();
    fileSelectHD_setInputEnabled(manager, 0u);
    const u32 q = fileSelectGetPane(p, PANE_NAME);
    const f32 rate = l_animationRate;
    (at<NamePane>(q)->mBusy = 1);
    (l_saveComplete = u8(0));
    (mMessage = MESSAGE_SAVING);
    fileSelectHD_playAnimation(fileSelectGetAnimation(p), 2u, 1u, rate);
    fileSelectHD_saveSlot(
        mSaveManager, u32(at<be<u8>>(p + u32(offsetof(dFile_select_c, selectNum)) + 3)[0].get()));
    fileSelectHD_fileSelectResetFocus(p);
    fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                      MOTION_BUSY_ORIGIN, MOTION_NAME_OUT, 255u, 0u, 0u, 10u);
    busyPosition(p, true);
    (mStage = 0);
    (mDelayTimer = 30);
}

/* 026D12B0 */
void dFile_select_c::executeSavingEX() {
    const u32 p = ea(this);
    const u32 stage = mStage.get();
    if (stage == 0) {
        if (fileSelectHD_dFile_select_c_updateTransitions(p)) {
            const u32 next = mStage.get() + 1, vt = mVtable.get();
            (mStage = next);
            call_ptr<void>(at<Vtable>(vt)->mUpdatePanes.get(), p);
            fileSelectVirtualCall(p, 0x8C);
            return;
        }
    } else if (stage == 1) {
        const u32 ticks = mDelayTimer.get();
        if (ticks) {
            (mDelayTimer = ticks - 1);
            return;
        }
        const u32 status = at<Service>(mSaveManager)->mCommand.get();
        if (status != 2 && status != 3) {
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)),
                                              selectNum.get(), MOTION_BUSY_ORIGIN, MOTION_NAME_OUT,
                                              255u, 0u, 0u, 10u);
            (mStage = mStage.get() + 1);
            fileSelectDrawScreen(p);
            return;
        }
    } else if (stage == 2) {
        if (fileSelectHD_dFile_select_c_updateTransitions(p)) {
            fileSelectHD_serviceRefresh(mpService.get());
            fileSelectHD_savePaneReload(fileSelectGetPane(p, selectNum.get() + 6));
            fileSelectHD_difficultySelectNormal(fileSelectGetPane(p, PANE_CHOICE));
            const u32 q = fileSelectGetPane(p, PANE_CHOICE);
            call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mSetButtonState.get(), q, 0u);
            (at<NamePane>(fileSelectGetPane(p, PANE_NAME))->mBusy = 0);
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)),
                                              selectNum.get(), MOTION_BUSY_OUT, MOTION_BUSY_ORIGIN,
                                              0u, 255u, 0u, 10u);
            fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                              MOTION_BUSY_OUT, MOTION_BUSY_ORIGIN, 0u, 255u, 3u,
                                              10u);
            fileSelectHD_dFile_select_c_loadSlotData(p, selectNum.get());
            (mMessage = MESSAGE_SAVE_END);
            fileSelectHD_startSound(SOUND_COMPLETE);
            fileSelectHD_setInputEnabled(fileSelectHD_getInputManager(), 1u);
            const u32 next = mStage.get() + 1, vt = mVtable.get();
            (mStage = next);
            call_ptr<void>(at<Vtable>(vt)->mUpdatePanes.get(), p);
            fileSelectVirtualCall(p, 0x8C);
            return;
        }
    } else if (stage == 3) {
        if (fileSelectHD_dFile_select_c_updateTransitions(p))
            fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                                     ea(&l_fileSelect.StateID_EndSaveEX));
    }
    fileSelectDrawScreen(p);
}

/* 026D15A8 */
static void initializeEndSaveEX() { (l_saveComplete = u8(1)); }

/* 026D15B8 */
void dFile_select_c::executeEndSaveEX() {
    const u32 p = ea(this);
    const u32 pad = mPad;
    if ((at<Pad>(pad)->mTrigger.get() | at<Pad>(pad)->mRepeat.get()) & 1) {
        (mClose = 1);
        fileSelectHD_startSound(SOUND_SAVE_END);
    }
    fileSelectDrawScreen(p);
}

/* 026D1634 */
static u32 dFile_select_c__ct(u32 p, u32 service) {
    if (!p)
        p = fileSelectHD_operatorNew(0x2D8u);
    if (!p)
        return 0;
    fileSelectHD_baseConstruct(p);
    (at<dFile_select_c>(p)->mVtable = kFileSelectVtable);
    (at<dFile_select_c>(p)->mStateInterfaceVtable = kStateInterfaceVtable);
    (at<dFile_select_c>(p)->mDrawInterfaceVtable = kDrawInterfaceVtable);
    if (p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mOrigin)) == 0)
        fileSelectHD_operatorNew(0x30u);
    if (p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mRight)) == 0)
        fileSelectHD_operatorNew(0x30u);
    if (p + u32(offsetof(dFile_select_c, mMotion)) + u32(offsetof(Motion, mLeft)) == 0)
        fileSelectHD_operatorNew(0x30u);
    if (p + 0x104u == 0)
        fileSelectHD_operatorNew(0xC0u);
    (at<dFile_select_c>(p)->mClose = 0);
    (at<dFile_select_c>(p)->mCancelSoundPlayed = 0);
    (at<dFile_select_c>(p)->mOriginalPane = 0);
    (at<dFile_select_c>(p)->selectEnd = 0);
    (at<dFile_select_c>(p)->mSelectedPane = 0);
    if (p + u32(offsetof(dFile_select_c, mSlotPosition)) == 0)
        fileSelectHD_operatorNew(0x24u);
    if (p + u32(offsetof(dFile_select_c, mSlotOffset)) == 0)
        fileSelectHD_operatorNew(0x24u);
    if (p + u32(offsetof(dFile_select_c, mCopyPosition)) == 0)
        fileSelectHD_operatorNew(0x24u);
    if (p + u32(offsetof(dFile_select_c, mSlotFlags)) == 0)
        fileSelectHD_operatorNew(6u);
    (at<dFile_select_c>(p)->mStage = 0);
    (at<dFile_select_c>(p)->mCompletionTimer = 0);
    (at<dFile_select_c>(p)->mMessage = MESSAGE_SELECT);
    (at<dFile_select_c>(p)->mUseType = 0);
    (at<dFile_select_c>(p)->mInputDisabled = 0);
    (at<dFile_select_c>(p)->selectNum = 0);
    if (p + u32(offsetof(dFile_select_c, mpScreenParts)) == 0)
        fileSelectHD_operatorNew(0x18u);
    if (p + u32(offsetof(dFile_select_c, mpExtraPart)) == 0)
        fileSelectHD_operatorNew(4u);
    if (p + u32(offsetof(dFile_select_c, mpParts)) == 0)
        fileSelectHD_operatorNew(0x28u);
    const f32 zero = l_zero;
    (at<dFile_select_c>(p)->mpService = service);
    for (u32 i = 0; i < 3; i++) {
        const u32 first = p + u32(offsetof(dFile_select_c, mSlotPosition)) + 12 * i,
                  second = p + u32(offsetof(dFile_select_c, mCopyPosition)) + 12 * i;
        (at<Vector>(first)->z = zero);
        (at<Vector>(first)->x = zero);
        (at<Vector>(first)->y = zero);
        (at<Vector>(second)->x = zero);
        (at<Vector>(second)->y = zero);
        (at<Vector>(second)->z = zero);
    }
    return p;
}

/* 026D1804 */
void dFile_select_c::findScreenParts() {
    if (!l_fileSelect.mScreenNamesInitialized.get()) {
        (l_fileSelect.mScreenNamesInitialized = 1);
        const u32 names[] = {guest_addr("N_SaveData_00"),     guest_addr("N_All_00"),
                             guest_addr("N_BtnSeqence_00"),   guest_addr("N_StatusPos_00"),
                             guest_addr("N_SaveDataPosM_00"), guest_addr("N_SaveDataPosB_00")};
        for (u32 i = 0; i < 6; ++i) {
            (l_fileSelect.mScreenPartNames[i].mVtable = kSafeStringVtable);
            (l_fileSelect.mScreenPartNames[i].mpText = names[i]);
        }
        fileSelectHD_registerGlobalObject(0x101F69BCu);
    }
    const u32 screen = at<LayoutData>(at<Layout>(mpLayout.get())->mpData.get())->mpRootPane.get();
    for (u32 i = 0; i < 6; ++i) {
        const u32 str = ea(&l_fileSelect.mScreenPartNames[0]) + u32(sizeof(SafeString)) * i,
                  fn = at<StringVtable>(at<SafeString>(str)->mVtable.get())
                           ->mAssureTermination.get(),
                  vt = at<RootPane>(screen)->mVtable.get();
        call_ptr<void>(fn, str);
        const u32 q = call_ptr<u32>(at<Vtable>(vt)->mFindPane.get(), screen,
                                    at<SafeString>(str)->mpText.get(), 1u);
        (mpScreenParts[i] = q);
    }
}

/* 026D1A14 */
void dFile_select_c::findParts() {
    const u32 screen = at<LayoutData>(at<Layout>(mpLayout.get())->mpData.get())->mpRootPane.get();
    for (u32 i = 0; i < 10; i++) {
        const u32 str = ea(&l_fileSelect.mPaneNames[0]) + u32(sizeof(SafeString)) * i,
                  vt = at<SafeString>(str)->mVtable.get(),
                  fn = at<StringVtable>(vt)->mAssureTermination.get(),
                  screenVt = at<RootPane>(screen)->mVtable.get();
        call_ptr<void>(fn, str, fn);
        const u32 q = call_ptr<u32>(at<Vtable>(screenVt)->mFindPane.get(), screen,
                                    at<SafeString>(str)->mpText.get(), 1u);
        (mpParts[i] = q);
    }
}

/* 026D1AF0 */
void Transition::clear() {
    const f32 z = l_zero;
    (mTargetAlpha = 255);
    (mTarget.z = z);
    (mDelay = 0);
    (mTarget.x = z);
    (mTarget.y = z);
    (mPosition.y = z);
    (mStage = 0);
    (mAlpha = 255);
    (mPosition.z = z);
    (mAlphaStep = 255);
    (mSpeed = z);
    (mPosition.x = z);
}

/* 026D1B34 */
void Motion::initialize() {
    const u32 p = ea(this);
    const f32 zero = l_zero;
    (mSecondChoice.y = zero);
    (mFirstChoice.y = zero);
    (mFirstChoice.z = zero);
    (mFirstChoice.x = zero);
    (mSecondChoice.x = zero);
    (mSecondChoice.z = zero);
    for (u32 i = 0; i < 4; ++i) {
        const u32 a = p + u32(offsetof(Motion, mOrigin)) + 12 * i,
                  b = p + u32(offsetof(Motion, mRight)) + 12 * i,
                  d = p + u32(offsetof(Motion, mLeft)) + 12 * i;
        (at<Vector>(a)->y = zero);
        (at<Vector>(a)->z = zero);
        (at<Vector>(a)->x = zero);
        (at<Vector>(b)->z = zero);
        (at<Vector>(b)->y = zero);
        (at<Vector>(b)->x = zero);
        (at<Vector>(d)->z = zero);
        (at<Vector>(d)->y = zero);
        (at<Vector>(d)->x = zero);
        fileSelectHD_Transition_clear(p + u32(offsetof(Motion, mTransition)) + 48 * i);
    }
}

/* 026D1C7C */
void Motion::setPaneOrigins(u32 i, u32 pos) {
    const u32 p = ea(this);
    f32 y = fileSelectReadFloatWord(pos + 4), x = fileSelectReadFloatWord(pos),
        z = fileSelectReadFloatWord(pos + 8);
    const u32 off = i < 4 ? 12 * i : 0;
    u32 out = p + u32(offsetof(Motion, mOrigin)) + off;
    (at<Vector>(out)->z = z);
    (at<Vector>(out)->x = x);
    (at<Vector>(out)->y = y);
    z = fileSelectReadFloatWord(pos + 8);
    x = fileSelectReadFloatWord(pos);
    const f32 distance = l_slotOutOffset;
    y = fileSelectReadFloatWord(pos + 4);
    out = p + u32(offsetof(Motion, mRight)) + off;
    (at<Vector>(out)->z = z);
    (at<Vector>(out)->y = y);
    (at<Vector>(out)->x = fadds_ppc(x, distance));
    x = fileSelectReadFloatWord(pos);
    z = fileSelectReadFloatWord(pos + 8);
    y = fileSelectReadFloatWord(pos + 4);
    out = p + u32(offsetof(Motion, mLeft)) + off;
    (at<Vector>(out)->z = z);
    (at<Vector>(out)->y = y);
    (at<Vector>(out)->x = fsubs_ppc(x, distance));
}

/* 026D1D14 */
u32 dFile_select_c::_create(u32 resource, u32 archive, u32 options) {
    const u32 p =
        ea(this); // HD: keep the paired-single temporaries and saved registers in the native frame.
    NativeFrame<0x90> f;
    const u32 sp = f.sp();
    (at<FrameLinkage>(f.entry())->mSavedLR = cpu->lr);
    (at<FrameLinkage>(sp)->mBackChain = f.entry());
    for (u32 r = 25; r < 32; ++r)
        (at<CreateFrame>(sp)->mSavedGpr[r - 25] = cpu->r[r]);
    fileSelectHD_setModuleName(p, ea(&l_fileSelect.mModuleName), 9u);
    u32 fn = at<Vtable>(mVtable.get())->mCreateLayout.get();
    if (!call_ptr<u32>(fn, p, ea(&l_fileSelect.mLayoutName), mpHeap.get(), resource, archive,
                       options))
        return 0;
    if (!call_ptr<u32>(at<Vtable>(mVtable.get())->mCreatePanes.get(), p))
        return 0;
    fileSelectHD_dFile_select_c_findScreenParts(p);
    fileSelectHD_dFile_select_c_findParts(p);
    fileSelectHD_dFile_select_c_linkSlots(p);
    vectorDifference(p + u32(offsetof(dFile_select_c, mFirstChoiceOffset)), mpScreenParts[4].get(),
                     mpScreenParts[0].get(), sp + 0x28);
    vectorSumDifference(p + u32(offsetof(dFile_select_c, mSecondChoiceOffset)),
                        mpScreenParts[4].get(), mpScreenParts[5].get(), mpScreenParts[0].get(),
                        sp + 0x34, sp + 0x10, sp + 0x40);
    fileSelectHD_startAnimation(fileSelectGetAnimation(p), 0u, 0u);
    fileSelectVirtualCall(p, 0x8C);
    fileSelectHD_dFile_select_c_dataLabels(p);
    fileSelectHD_changeState(fileSelectGetPane(p, PANE_NAME) + 0x18, ea(&l_nameInputState));
    fileSelectHD_Motion_initialize(p + u32(offsetof(dFile_select_c, mMotion)));
    vectorDifference(p + EVENT_ACCEPT, mpScreenParts[4].get(), mpScreenParts[0].get(), sp + 0x4C);
    vectorSumDifference(p + 0x68, mpScreenParts[4].get(), mpScreenParts[5].get(),
                        mpScreenParts[0].get(), sp + 0x58, sp + 0x1C, sp + 0x64);
    for (u32 i = 0; i < 3; ++i)
        fileSelectHD_Motion_setPaneOrigins(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                           at<dFile_select_c>(p)->mpParts[6 + i].get() + 0x1C);
    fileSelectHD_Motion_setPaneOrigins(p + u32(offsetof(dFile_select_c, mMotion)), 3u,
                                       mpParts[7].get() + 0x1C);
    const u32 name = sp + 8;
    for (u32 i = 0; i < 4; ++i) {
        const u32 q = fileSelectGetPane(p, i);
        (at<SafeString>(name)->mVtable = kSafeStringVtable);
        (at<SafeString>(name)->mpText = guest_addr("T_Btn_00"));
        fileSelectHD_paneSetText(q, name,
                                 ea(&l_fileSelect.mLabelNames[0]) + u32(sizeof(SafeString)) * i);
    }
    for (u32 i = 0; i < 3; ++i)
        (at<Pane>(fileSelectGetPane(p, i))->mState = 2);
    const u32 state = mpStateProcessor.get(),
              created = call_ptr<u32>(at<Vtable>(at<be<u32>>(state)->get())->mGetId.get(), state,
                                      ea(&l_fileSelect.StateID_Wait));
    (mpState = created);
    call_ptr<void>(at<Vtable>(at<be<u32>>(created)->get())->mInitialize.get(), created);
    return 1;
}

/* 026D20D8 */
void dFile_select_c::_delete() {
    const u32 p = ea(this);
    fileSelectHD_setFocus(mInputManager, 0u);
    fileSelectVirtualCall(p, 0xAC);
    fileSelectVirtualCall(p, 0x84);
    fileSelectHD_baseCalc(p);
}

/* 026D2140 */
u32 dFile_select_c::_move() {
    const u32 p = ea(this);
    return fileSelectHD_updateState(p + u32(offsetof(dFile_select_c, mpStateProcessor)));
}

/* 026D2148 */
void dFile_select_c::_draw(u32 draw) {
    const u32 p = ea(this);
    const u32 state = fileSelectHD_getState(p + u32(offsetof(dFile_select_c, mpStateProcessor))),
              fn = at<Vtable>(at<State>(state)->mVtable.get())->mGetId.get(),
              saved = l_fileSelect.StateID_EndCopy.mVtable.get();
    const u32 a = call_ptr<u32>(fn, state),
              b = call_ptr<u32>(at<Vtable>(saved)->mGetId.get(), ea(&l_fileSelect.StateID_EndCopy));
    u32 count = mPaneCount.get();
    if (a == b) {
        u32 array = mpPanes.get(), selected = mSelectedPane.get();
        for (u32 i = 0; s32(i) < s32(count); ++i) {
            if (i != selected) {
                u32 q = at<be<u32>>(array + (i < count ? 4 * i : 0))[0].get();
                if (q) {
                    q = at<be<u32>>(array + (i < count ? 4 * i : 0))[0].get();
                    call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mDraw.get(), q, draw);
                    count = mPaneCount.get();
                    array = mpPanes.get();
                    selected = mSelectedPane.get();
                }
            }
        }
        const u32 q = at<be<u32>>(array + (selected < count ? 4 * selected : 0))[0].get();
        call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mDraw.get(), q, draw);
    } else {
        for (u32 i = 0; s32(i) < s32(count); ++i) {
            const u32 array = mpPanes.get();
            u32 q = at<be<u32>>(array + (i < count ? 4 * i : 0))[0].get();
            if (q) {
                q = at<be<u32>>(array + (i < count ? 4 * i : 0))[0].get();
                call_ptr<void>(at<Vtable>(at<Pane>(q)->mVtable.get())->mDraw.get(), q, draw);
                count = mPaneCount.get();
            }
        }
    }
}

/* 026D230C */
void dFile_select_c::showSlots() {
    const u32 p = ea(this);
    const u32 count = mPaneCount.get();
    (selectEnd = 0);
    u32 array = mpPanes.get();
    (mClose = 0);
    fileSelectHD_paneResetSelection(at<be<u32>>(array + (count > 3 ? 12 : 0))[0].get());
    fileSelectHD_paneSelect(fileSelectGetPane(p, selectNum.get() + 6));
    for (u32 i = 0; i < 3; i++)
        fileSelectHD_Motion_setTransition(p + u32(offsetof(dFile_select_c, mMotion)), i,
                                          MOTION_ORIGIN, MOTION_ORIGIN, 255u, 255u, 0u, 8u);
    fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                             ea(&l_fileSelect.StateID_SelectSaveData));
}

/* 026D23DC */
void dFile_select_c::reset() {
    const u32 p = ea(this);
    (mClose = 0);
    (selectEnd = 0);
    fileSelectHD_dFile_select_c_selectionTransition(p);
    fileSelectHD_changeState(p + u32(offsetof(dFile_select_c, mpStateProcessor)),
                             ea(&l_fileSelect.StateID_ShowSaveData));
}

/* 026D2424 */
u32 dFile_select_c::slotStatus() {
    const u32 p = ea(this);
    const u32 index = selectNum.get() + 6;
    return fileSelectHD_savePaneGetStatus(fileSelectGetPane(p, index));
}

/* 026D244C */
u32 dFile_select_c::deleteSound() {
    const u32 p = ea(this);
    const u32 q = fileSelectGetPane(p, PANE_CHOICE), sound = mInputManager;
    return fileSelectHD_setFocus(sound, at<Pane>(q)->mId.get());
}

/* 026D2474 */
u32 dFile_select_c::createPane(u32 info) {
    const u32 p = ea(this); // HD: callees observe this native frame and its saved registers.
    NativeFrame<0x38> f;
    const u32 sp = f.sp();
    (at<FrameLinkage>(f.entry())->mSavedLR = cpu->lr);
    (at<FrameLinkage>(sp)->mBackChain = f.entry());
    for (u32 r = 21; r < 32; ++r)
        (at<CreatePaneFrame>(sp)->mSavedGpr[r - 21] = cpu->r[r]);
    const u32 heap = at<AllocationManager>(mAllocationManager)->mpHeap.get();
    for (u32 i = 0; i < 10; ++i) {
        const u32 type = info + 0x14,
                  table = ea(&l_fileSelect.mPaneNames[0]) + u32(sizeof(SafeString)) * i;
        touch(type);
        touch(type);
        const u32 before = at<be<u32>>(type)[0].get();
        touch(table, true);
        if (before != at<be<u32>>(table)[0].get() &&
            !fileSelectSameString(at<be<u32>>(type)[0].get(), at<be<u32>>(table)[0].get()))
            continue;
        const u32 types[] = {ea(&l_savePaneType), ea(&l_collectionPaneType), ea(&l_namePaneType),
                             ea(&l_difficultyPaneType)};
        constexpr u32 sizes[] = {0xC4, 0x50, 0x104, 0x94};
        for (u32 kind = 0; kind < 4; ++kind) {
            touch(info + 8);
            touch(info + 8, kind == 1);
            const u32 old = at<be<u32>>(info + 8)[0].get();
            touch(types[kind]);
            if (old != at<SafeString>(types[kind])->mpText.get() &&
                !fileSelectSameString(at<be<u32>>(info + 8)[0].get(),
                                      at<SafeString>(types[kind])->mpText.get()))
                continue;
            const u32 count = mPaneCount.get(), slot = mpPanes.get() + (i < count ? 4 * i : 0);
            u32 q = fileSelectHD_heapNew(sizes[kind], heap, 4u);
            if (q) {
                if (kind == 0)
                    q = fileSelectHD_savePaneConstruct(q, mpService.get(), (i - 6) & 255);
                else if (kind == 3)
                    q = fileSelectHD_difficultyPaneConstruct(q, mpService.get());
                else
                    q = kind == 1 ? fileSelectHD_collectionPaneConstruct(q)
                                  : fileSelectHD_namePaneConstruct(q);
            }
            (at<be<u32>>(slot)[0] = q);
            const u32 n = mPaneCount.get(), array = mpPanes.get(),
                      made = at<be<u32>>(array + (i < n ? 4 * i : 0))[0].get();
            if (!made)
                return 0;
            const u32 pane = at<be<u32>>(array + (i < n ? 4 * i : 0))[0].get();
            switch (kind) {
            case 0:
                return fileSelectHD_savePaneCreate(pane, 0u, 0u, info);
            case 1:
                return fileSelectHD_collectionPaneCreate(pane, 0u, 0u, info);
            case 2:
                return fileSelectHD_namePaneCreate(pane, 0u, 0u, info);
            default:
                return fileSelectHD_difficultyPaneCreate(pane, 0u, 0u, info);
            }
        }
        return fileSelectHD_baseCreatePane(p, i, info);
    }
    return 1;
}

/* 026D2A58 */
u32 dFile_select_c::createLayout(u32 resource, u32 archive, u32 options, u32 a, u32 b) {
    const u32 p = ea(this);
    u32 q =
        fileSelectHD_heapNew(0x110u, at<AllocationManager>(mAllocationManager)->mpHeap.get(), 4u);
    if (q)
        q = fileSelectHD_layoutConstruct(q);
    (mpLayout = q);
    if (!q)
        return 0;
    call_ptr<void>(at<LayoutVtable>(at<Layout>(q)->mVtable.get())->mCreate.get(), q, resource,
                   archive, options, a, 7u, 4u, b);
    for (u32 i = 0; i < 7; ++i)
        fileSelectHD_bindAnimation(fileSelectGetAnimation(p), i,
                                   ea(&l_fileSelect.mAnimationNames[0]) +
                                       u32(sizeof(SafeString)) * i,
                                   ea(&l_fileSelect.mAnimationRoots[0]) +
                                       u32(sizeof(SafeString)) * l_animationRootIndices[i]);
    return 1;
}

// HD: compiler-generated and template code of this translation unit.
/* 026D2B68 */
static void __sinit_d_file_select_cpp() {
    (l_fileSelect.mHeaderWords[3] = 0x00000000u);
    (l_fileSelect.mHeaderWords[2] = 0x00000000u);
    (l_fileSelect.mHeaderWords[1] = 0x00000000u);
    (l_fileSelect.mHeaderWords[0] = 0x00000000u);
    fileSelectHD_registerGlobalObject(0x101F69C8u);
    (l_fileSelect.mNegativePi = l_negativePi);
    (l_fileSelect.mPositivePi = l_positivePi);
    fileSelectHD_constructHeaderByte(ea(&l_fileSelect.mHeaderByte));
    fileSelectHD_registerGlobalObject(0x101F69D4u);
    fileSelectHD_constructArchiveByte(ea(&l_fileSelect.mArchiveByte));
    fileSelectHD_registerGlobalObject(0x101F69E0u);
    (l_fileSelect.mAnimationRoots[3].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationNames[1].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationRoots[2].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationNames[6].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationNames[0].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationNames[5].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationRoots[0].mpText = guest_addr("G_All_00"));
    (l_fileSelect.mPaneNames[0].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationRoots[3].mpText = guest_addr("G_BtnDifficulty_00"));
    (l_fileSelect.mAnimationRoots[1].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationNames[2].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationNames[2].mpText = guest_addr("OutBtnSeqence"));
    (l_fileSelect.mAnimationNames[1].mpText = guest_addr("InBtnSeqence"));
    (l_fileSelect.mAnimationRoots[2].mpText = guest_addr("G_Status_00"));
    (l_fileSelect.mAnimationNames[3].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[0].mpText = guest_addr("L_BtnSeqence_00"));
    (l_fileSelect.mAnimationNames[4].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationNames[3].mpText = guest_addr("InStates"));
    (l_fileSelect.mAnimationRoots[1].mpText = guest_addr("G_Button_00"));
    (l_fileSelect.mPaneNames[1].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[1].mpText = guest_addr("L_BtnSeqence_01"));
    (l_fileSelect.mAnimationRoots[0].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[2].mVtable = kSafeStringVtable);
    (l_fileSelect.mAnimationNames[0].mpText = guest_addr("InAll"));
    (l_fileSelect.mPaneNames[2].mpText = guest_addr("L_BtnSeqence_02"));
    (l_fileSelect.mAnimationNames[4].mpText = guest_addr("OutStates"));
    (l_fileSelect.mAnimationNames[6].mpText = guest_addr("OutAll"));
    (l_fileSelect.mPaneNames[3].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[3].mpText = guest_addr("L_BtnSeqence_03"));
    (l_fileSelect.mAnimationNames[5].mpText = guest_addr("OnOffBtnDifficulty"));
    (l_fileSelect.mPaneNames[4].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[4].mpText = guest_addr("L_Collection_00"));
    (l_fileSelect.mPaneNames[5].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[5].mpText = guest_addr("L_BtnDifficulty_00"));
    (l_fileSelect.mModuleName.mVtable = kSafeStringVtable);
    (l_fileSelect.mModuleName.mpText = guest_addr("SaveData_00"));
    (l_fileSelect.mPaneNames[6].mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[0].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[6].mpText = guest_addr("L_SaveData_00"));
    (l_fileSelect.mPaneNames[7].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[7].mpText = guest_addr("L_SaveData_01"));
    (l_fileSelect.mPaneNames[8].mVtable = kSafeStringVtable);
    (l_fileSelect.mPaneNames[8].mpText = guest_addr("L_SaveData_02"));
    (l_fileSelect.mPaneNames[9].mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[0].mpText = guest_addr("BtnSequence_00"));
    (l_fileSelect.mTypeNames[1].mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[1].mpText = guest_addr("HeartAll_00"));
    (l_fileSelect.mTypeNames[2].mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[2].mpText = guest_addr("Collection_00"));
    (l_fileSelect.mTypeNames[3].mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[3].mpText = guest_addr("BtnSaveData_00"));
    (l_fileSelect.mTypeNames[4].mVtable = kSafeStringVtable);
    (l_fileSelect.mLabelNames[2].mVtable = kSafeStringVtable);
    (l_fileSelect.mLabelNames[1].mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[4].mpText = guest_addr("BtnDifficulty_00"));
    (l_fileSelect.mTypeNames[5].mVtable = kSafeStringVtable);
    (l_fileSelect.mLayoutName.mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[5].mpText = guest_addr("Heart_00"));
    (l_fileSelect.mTypeNames[6].mVtable = kSafeStringVtable);
    (l_fileSelect.mLabelNames[3].mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[6].mpText = guest_addr("BtnPearl_00"));
    (l_fileSelect.mLabelNames[5].mVtable = kSafeStringVtable);
    (l_fileSelect.mLabelNames[2].mpText = guest_addr("L_SaveData_00-T_Btn_copy"));
    (l_fileSelect.mTypeNames[7].mVtable = kSafeStringVtable);
    (l_fileSelect.mLabelNames[4].mVtable = kSafeStringVtable);
    (l_fileSelect.mTypeNames[7].mpText = guest_addr("BtnTriforce_00"));
    (l_fileSelect.mLabelNames[4].mpText = guest_addr("L_SaveData_00-T_Btn_yes"));
    (l_fileSelect.mPaneNames[9].mpText = guest_addr("L_HeartAll_00"));
    (l_fileSelect.mLayoutName.mpText = guest_addr("SaveData_00"));
    const u32 registration = l_stateIdInitialized;
    (l_fileSelect.mLabelNames[3].mpText = guest_addr("L_SaveData_00-T_Btn_start"));
    (l_fileSelect.mLabelNames[0].mVtable = kSafeStringVtable);
    (l_fileSelect.mLabelNames[5].mpText = guest_addr("L_SaveData_00-T_Btn_no"));
    (l_fileSelect.mLabelNames[0].mpText = guest_addr("L_SaveData_00-T_Btn_back"));
    (l_fileSelect.mLabelNames[1].mpText = guest_addr("L_SaveData_00-T_Btn_delete"));
    const u32 firstId = registration ? l_nextStateId : 0u;
    if (!registration)
        (l_stateIdInitialized = 1u);
    (l_fileSelect.StateID_Wait.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_Wait.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    const u32 value0 = (firstId + 0x00000001u);
    (l_fileSelect.StateID_Wait.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_Wait.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_Wait.mId = value0);
    const u32 value1 = (value0 + 0x00000001u);
    (l_fileSelect.StateID_Wait.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_InAll.mState.mId = value1);
    (l_fileSelect.StateID_InAll.mState.mCheck.mTarget = 0x00000000u);
    (l_fileSelect.StateID_InAll.mState.mInitialize.mVirtualSlot = u16(0x00000019u));
    (l_fileSelect.StateID_InAll.mState.mCheck.mVirtualSlot = u16(0x00000000u));
    (l_fileSelect.StateID_Wait.mpName = guest_addr("::StateID_Wait"));
    (l_fileSelect.StateID_InAll.mpNullState = kNullState);
    (l_fileSelect.StateID_InAll.mState.mpName = guest_addr("::StateID_InAll"));
    (l_fileSelect.StateID_InAll.mState.mFinalize.mVirtualSlot = u16(0x0000001Bu));
    (l_fileSelect.StateID_InAll.mState.mExecute.mVirtualSlot = u16(0x0000001Au));
    (l_fileSelect.StateID_Wait.mVtable = kStateVtable);
    (l_fileSelect.StateID_Wait.mInitialize.mTarget = kdFile_select_c_initializeWait);
    (l_fileSelect.StateID_InAll.mState.mExecute.mThisAdjust = u16(0x00000000u));
    const u32 value2 = (value1 + 0x00000001u);
    (l_fileSelect.StateID_Wait.mExecute.mTarget = kexecuteWait);
    (l_fileSelect.StateID_SelectSaveData.mId = value2);
    (l_fileSelect.StateID_SelectSaveData.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectSaveData.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectSaveData.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_InAll.mState.mFinalize.mTarget = 0x00000004u);
    (l_fileSelect.StateID_SelectSaveData.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectSaveData.mpName = guest_addr("::StateID_SelectSaveData"));
    (l_fileSelect.StateID_InAll.mState.mExecute.mTarget = 0x00000004u);
    (l_fileSelect.StateID_Wait.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_InAll.mState.mVtable = kVirtualStateVtable);
    (l_fileSelect.StateID_SelectSaveData.mVtable = kStateVtable);
    (l_fileSelect.StateID_SelectSaveData.mInitialize.mTarget =
         kdFile_select_c_initializeSelectSaveData);
    (l_fileSelect.StateID_SelectSaveData.mExecute.mTarget = kdFile_select_c_executeSelectSaveData);
    (l_fileSelect.StateID_SelectSaveData.mFinalize.mTarget = kfinalizeSelectSaveData);
    (l_fileSelect.StateID_Wait.mFinalize.mTarget = kfinalizeWait);
    const u32 value3 = (value2 + 0x00000001u);
    (l_fileSelect.StateID_SelectSaveData.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_Wait.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_Wait.mCheck.mTarget = kcheckWait);
    (l_fileSelect.StateID_Wait.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_InAll.mState.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_InAll.mState.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_InAll.mState.mInitialize.mTarget = 0x00000004u);
    (l_fileSelect.StateID_WaitNameInput.mId = value3);
    (l_fileSelect.StateID_WaitNameInput.mpName = guest_addr("::StateID_WaitNameInput"));
    (l_fileSelect.StateID_WaitNameInput.mVtable = kStateVtable);
    (l_fileSelect.StateID_SelectSaveData.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_InAll.mState.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitNameInput.mInitialize.mTarget = kinitializeWaitNameInput);
    (l_fileSelect.StateID_WaitNameInput.mExecute.mTarget = kdFile_select_c_executeWaitNameInput);
    (l_fileSelect.StateID_SelectSaveData.mCheck.mThisAdjust = u16(0x00000000u));
    const u32 value4 = (value3 + 0x00000001u);
    (l_fileSelect.StateID_SelectSaveData.mCheck.mTarget = kcheckSelectSaveData);
    (l_fileSelect.StateID_ShowSaveData.mId = value4);
    (l_fileSelect.StateID_ShowSaveData.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_ShowSaveData.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_ShowSaveData.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectSaveData.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitNameInput.mFinalize.mTarget = kfinalizeWaitNameInput);
    (l_fileSelect.StateID_WaitNameInput.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitNameInput.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_ShowSaveData.mpName = guest_addr("::StateID_ShowSaveData"));
    (l_fileSelect.StateID_WaitNameInput.mCheck.mTarget = kcheckWaitNameInput);
    (l_fileSelect.StateID_ShowSaveData.mVtable = kStateVtable);
    (l_fileSelect.StateID_ShowSaveData.mInitialize.mTarget =
         kdFile_select_c_initializeShowSaveData);
    (l_fileSelect.StateID_ShowSaveData.mExecute.mTarget = kdFile_select_c_executeShowSaveData);
    (l_fileSelect.StateID_ShowSaveData.mFinalize.mTarget = kfinalizeShowSaveData);
    (l_fileSelect.StateID_ShowSaveData.mCheck.mTarget = kcheckShowSaveData);
    const u32 value5 = (value4 + 0x00000001u);
    (l_fileSelect.StateID_ShowSaveData.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_ShowSaveData.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_CancelSaveData.mId = value5);
    (l_fileSelect.StateID_ShowSaveData.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_CancelSaveData.mpName = guest_addr("::StateID_CancelSaveData"));
    (l_fileSelect.StateID_ShowSaveData.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_CancelSaveData.mVtable = kStateVtable);
    (l_fileSelect.StateID_CancelSaveData.mInitialize.mTarget =
         kdFile_select_c_initializeCancelSaveData);
    const u32 value6 = (value5 + 0x00000001u);
    (l_fileSelect.StateID_WaitNameInput.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_CancelSaveData.mExecute.mTarget = kdFile_select_c_executeCancelSaveData);
    (l_fileSelect.StateID_CancelSaveData.mFinalize.mTarget = kfinalizeCancelSaveData);
    (l_fileSelect.StateID_ShowSaveData.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_CancelSaveData.mCheck.mTarget = kcheckCancelSaveData);
    (l_fileSelect.StateID_WaitNameInput.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectDownButton.mId = value6);
    (l_fileSelect.StateID_SelectDownButton.mpName = guest_addr("::StateID_SelectDownButton"));
    (l_fileSelect.StateID_WaitNameInput.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitNameInput.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectDownButton.mVtable = kStateVtable);
    (l_fileSelect.StateID_SelectDownButton.mInitialize.mTarget =
         kdFile_select_c_initializeSelectDownButton);
    (l_fileSelect.StateID_SelectDownButton.mExecute.mTarget =
         kdFile_select_c_executeSelectDownButton);
    (l_fileSelect.StateID_CancelSaveData.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_CancelSaveData.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_CancelSaveData.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_CancelSaveData.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_CancelSaveData.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_CancelSaveData.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectDownButton.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectDownButton.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectDownButton.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectDownButton.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectDownButton.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectDownButton.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectDownButton.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideSaveData.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideSaveData.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitNameInput.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitNameInput.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideSaveData.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectDownButton.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_CancelSaveData.mCheck.mThisAdjust = u16(0x00000000u));
    const u32 value7 = (value6 + 0x00000001u);
    (l_fileSelect.StateID_SelectDownButton.mFinalize.mTarget = kfinalizeSelectDownButton);
    (l_fileSelect.StateID_CancelSaveData.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectDownButton.mCheck.mTarget = kcheckSelectDownButton);
    (l_fileSelect.StateID_DecideSaveData.mId = value7);
    (l_fileSelect.StateID_DecideSaveData.mpName = guest_addr("::StateID_DecideSaveData"));
    (l_fileSelect.StateID_DecideSaveData.mVtable = kStateVtable);
    (l_fileSelect.StateID_DecideSaveData.mInitialize.mTarget = kinitializeDecideSaveData);
    (l_fileSelect.StateID_DecideSaveData.mExecute.mTarget = kdFile_select_c_executeDecideSaveData);
    (l_fileSelect.StateID_DecideSaveData.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideSaveData.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideSaveData.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideSaveData.mCheck.mThisAdjust = u16(0x00000000u));
    const u32 value8 = (value7 + 0x00000001u);
    (l_fileSelect.StateID_OutAllEnd.mState.mId = value8);
    (l_fileSelect.StateID_OutAllEnd.mState.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_OutAllEnd.mState.mInitialize.mVirtualSlot = u16(0x0000001Cu));
    (l_fileSelect.StateID_OutAllEnd.mState.mpName = guest_addr("::StateID_OutAll"));
    (l_fileSelect.StateID_OutAllEnd.mState.mInitialize.mTarget = 0x00000004u);
    (l_fileSelect.StateID_OutAllEnd.mState.mExecute.mTarget = 0x00000004u);
    const u32 value9 = (value8 + 0x00000001u);
    (l_fileSelect.StateID_OutAllEnd.mState.mFinalize.mTarget = 0x00000004u);
    (l_fileSelect.StateID_OutAllEnd.mState.mCheck.mTarget = 0x00000000u);
    (l_fileSelect.StateID_OutAll.mId = value9);
    (l_fileSelect.StateID_OutAll.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_OutAll.mpName = guest_addr("::StateID_OutAllEnd"));
    const u32 value10 = (value9 + 0x00000001u);
    (l_fileSelect.StateID_OutAll.mVtable = kStateVtable);
    (l_fileSelect.StateID_OutAllEnd.mState.mVtable = kVirtualStateVtable);
    (l_fileSelect.StateID_DecideSaveData.mFinalize.mTarget = kfinalizeDecideSaveData);
    (l_fileSelect.StateID_OutAllEnd.mState.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_OutAllEnd.mpNullState = kNullState);
    (l_fileSelect.StateID_DecideSaveData.mCheck.mTarget = kcheckDecideSaveData);
    (l_fileSelect.StateID_OutAll.mInitialize.mTarget = kinitializeOutAll);
    (l_fileSelect.StateID_OutAll.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_OutAll.mExecute.mTarget = kexecuteOutAll);
    (l_fileSelect.StateID_DecideDeleteYesNo.mId = value10);
    (l_fileSelect.StateID_DecideDeleteYesNo.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideDeleteYesNo.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_OutAll.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_OutAll.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideDeleteYesNo.mpName = guest_addr("::StateID_DecideDeleteYesNo"));
    (l_fileSelect.StateID_OutAllEnd.mState.mExecute.mVirtualSlot = u16(0x0000001Du));
    (l_fileSelect.StateID_OutAll.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_OutAll.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideSaveData.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_OutAllEnd.mState.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_OutAll.mFinalize.mTarget = kfinalizeOutAll);
    (l_fileSelect.StateID_OutAll.mCheck.mTarget = kcheckOutAll);
    (l_fileSelect.StateID_OutAll.mCheck.mThisAdjust = u16(0x00000000u));
    const u32 value11 = (value10 + 0x00000001u);
    (l_fileSelect.StateID_OutAll.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitDeleting.mId = value11);
    (l_fileSelect.StateID_DecideDeleteYesNo.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideDeleteYesNo.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitDeleting.mpName = guest_addr("::StateID_WaitDeleting"));
    (l_fileSelect.StateID_OutAllEnd.mState.mFinalize.mVirtualSlot = u16(0x0000001Eu));
    (l_fileSelect.StateID_OutAllEnd.mState.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideDeleteYesNo.mVtable = kStateVtable);
    (l_fileSelect.StateID_WaitDeleting.mVtable = kStateVtable);
    (l_fileSelect.StateID_DecideDeleteYesNo.mInitialize.mTarget =
         kdFile_select_c_initializeDecideDeleteYesNo);
    (l_fileSelect.StateID_OutAllEnd.mState.mCheck.mVirtualSlot = u16(0x00000000u));
    (l_fileSelect.StateID_DecideDeleteYesNo.mExecute.mTarget =
         kdFile_select_c_executeDecideDeleteYesNo);
    (l_fileSelect.StateID_DecideDeleteYesNo.mFinalize.mTarget =
         kdFile_select_c_finalizeDecideDeleteYesNo);
    (l_fileSelect.StateID_DecideDeleteYesNo.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideDeleteYesNo.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideDeleteYesNo.mCheck.mTarget = kcheckDecideDeleteYesNo);
    (l_fileSelect.StateID_DecideDeleteYesNo.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitDeleting.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitDeleting.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitDeleting.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitDeleting.mInitialize.mTarget =
         kdFile_select_c_initializeWaitDeleting);
    const u32 value12 = (value11 + 0x00000001u);
    (l_fileSelect.StateID_WaitDeleting.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideDeleteYesNo.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitDeleting.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitDeleting.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitDeleting.mExecute.mTarget = kdFile_select_c_executeWaitDeleting);
    (l_fileSelect.StateID_EndDelete.mId = value12);
    (l_fileSelect.StateID_EndDelete.mpName = guest_addr("::StateID_EndDelete"));
    (l_fileSelect.StateID_EndDelete.mVtable = kStateVtable);
    (l_fileSelect.StateID_WaitDeleting.mFinalize.mTarget = kfinalizeWaitDeleting);
    (l_fileSelect.StateID_EndDelete.mInitialize.mTarget = kdFile_select_c_initializeEndDelete);
    (l_fileSelect.StateID_WaitDeleting.mCheck.mThisAdjust = u16(0x00000000u));
    const u32 value13 = (value12 + 0x00000001u);
    (l_fileSelect.StateID_EndDelete.mExecute.mTarget = kdFile_select_c_executeEndDelete);
    (l_fileSelect.StateID_SelectFileCopyTo.mId = value13);
    const u32 value14 = (value13 + 0x00000001u);
    (l_fileSelect.StateID_EndDelete.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndDelete.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndDelete.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndDelete.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndDelete.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitDeleting.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndDelete.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndDelete.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitDeleting.mCheck.mTarget = kcheckWaitDeleting);
    (l_fileSelect.StateID_EndDelete.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndDelete.mFinalize.mTarget = kfinalizeEndDelete);
    (l_fileSelect.StateID_SelectFileCopyTo.mpName = guest_addr("::StateID_SelectFileCopyTo"));
    (l_fileSelect.StateID_SelectFileCopyTo.mVtable = kStateVtable);
    (l_fileSelect.StateID_EndDelete.mCheck.mTarget = kcheckEndDelete);
    (l_fileSelect.StateID_DecideCopyYesNo.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectFileCopyTo.mInitialize.mTarget =
         kdFile_select_c_initializeSelectFileCopyTo);
    (l_fileSelect.StateID_SelectFileCopyTo.mExecute.mTarget =
         kdFile_select_c_executeSelectFileCopyTo);
    (l_fileSelect.StateID_SelectFileCopyTo.mFinalize.mTarget = kfinalizeSelectFileCopyTo);
    (l_fileSelect.StateID_SelectFileCopyTo.mCheck.mTarget = kcheckSelectFileCopyTo);
    (l_fileSelect.StateID_DecideCopyYesNo.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideCopyYesNo.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideCopyYesNo.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideCopyYesNo.mId = value14);
    (l_fileSelect.StateID_DecideCopyYesNo.mFinalize.mThisAdjust = u16(0x00000000u));
    const u32 value15 = (value14 + 0x00000001u);
    (l_fileSelect.StateID_DecideCopyYesNo.mpName = guest_addr("::StateID_DecideCopyYesNo"));
    (l_fileSelect.StateID_DecideCopyYesNo.mVtable = kStateVtable);
    (l_fileSelect.StateID_WaitCopying.mId = value15);
    (l_fileSelect.StateID_DecideCopyYesNo.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectFileCopyTo.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideCopyYesNo.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectFileCopyTo.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectFileCopyTo.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideCopyYesNo.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_DecideCopyYesNo.mInitialize.mTarget =
         kdFile_select_c_initializeDecideCopyYesNo);
    (l_fileSelect.StateID_WaitCopying.mpName = guest_addr("::StateID_WaitCopying"));
    (l_fileSelect.StateID_WaitCopying.mVtable = kStateVtable);
    const u32 value16 = (value15 + 0x00000001u);
    (l_fileSelect.StateID_WaitCopying.mInitialize.mTarget = kdFile_select_c_initializeWaitCopying);
    (l_fileSelect.StateID_WaitCopying.mExecute.mTarget = kdFile_select_c_executeWaitCopying);
    (l_fileSelect.StateID_SelectFileCopyTo.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SelectFileCopyTo.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitCopying.mFinalize.mTarget = kfinalizeWaitCopying);
    (l_fileSelect.StateID_WaitCopying.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndCopy.mId = value16);
    (l_fileSelect.StateID_EndCopy.mpName = guest_addr("::StateID_EndCopy"));
    (l_fileSelect.StateID_SelectFileCopyTo.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndCopy.mVtable = kStateVtable);
    (l_fileSelect.StateID_EndCopy.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndCopy.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndCopy.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndCopy.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndCopy.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitCopying.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitCopying.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectFileCopyTo.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SelectFileCopyTo.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitCopying.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitCopying.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndCopy.mInitialize.mTarget = kdFile_select_c_initializeEndCopy);
    (l_fileSelect.StateID_WaitCopying.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_WaitCopying.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_WaitCopying.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndCopy.mExecute.mTarget = kdFile_select_c_executeEndCopy);
    (l_fileSelect.StateID_EndCopy.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndCopy.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideCopyYesNo.mExecute.mTarget =
         kdFile_select_c_executeDecideCopyYesNo);
    const u32 value17 = (value16 + 0x00000001u);
    (l_fileSelect.StateID_DecideCopyYesNo.mFinalize.mTarget =
         kdFile_select_c_finalizeDecideCopyYesNo);
    (l_fileSelect.StateID_SaveEXYesNo.mId = value17);
    (l_fileSelect.StateID_SaveEXYesNo.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SaveEXYesNo.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SaveEXYesNo.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndCopy.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SaveEXYesNo.mpName = guest_addr("::StateID_SaveEXYesNo"));
    (l_fileSelect.StateID_EndCopy.mFinalize.mTarget = kfinalizeEndCopy);
    (l_fileSelect.StateID_EndCopy.mCheck.mTarget = kcheckEndCopy);
    (l_fileSelect.StateID_SaveEXYesNo.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SaveEXYesNo.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_DecideCopyYesNo.mCheck.mTarget = kcheckDecideCopyYesNo);
    (l_fileSelect.StateID_SaveEXYesNo.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SaveEXYesNo.mVtable = kStateVtable);
    (l_fileSelect.StateID_SaveEXYesNo.mInitialize.mTarget = kdFile_select_c_initializeSaveEXYesNo);
    const u32 value18 = (value17 + 0x00000001u);
    (l_fileSelect.StateID_WaitCopying.mCheck.mTarget = kcheckWaitCopying);
    (l_fileSelect.StateID_SaveEXYesNo.mExecute.mTarget = kdFile_select_c_executeSaveEXYesNo);
    (l_fileSelect.StateID_SavingEX.mId = value18);
    (l_fileSelect.StateID_SavingEX.mpName = guest_addr("::StateID_SavingEX"));
    (l_fileSelect.StateID_SavingEX.mVtable = kStateVtable);
    (l_fileSelect.StateID_SaveEXYesNo.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SavingEX.mInitialize.mTarget = kdFile_select_c_initializeSavingEX);
    (l_fileSelect.StateID_SaveEXYesNo.mFinalize.mTarget = kfinalizeSaveEXYesNo);
    (l_fileSelect.StateID_SaveEXYesNo.mCheck.mTarget = kcheckSaveEXYesNo);
    (l_fileSelect.StateID_SavingEX.mExecute.mTarget = kdFile_select_c_executeSavingEX);
    (l_fileSelect.StateID_SavingEX.mFinalize.mTarget = kfinalizeSavingEX);
    (l_fileSelect.StateID_SavingEX.mCheck.mTarget = kcheckSavingEX);
    (l_fileSelect.StateID_SaveEXYesNo.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SavingEX.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SavingEX.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_SavingEX.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SavingEX.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    const u32 value19 = (value18 + 0x00000001u);
    (l_fileSelect.StateID_SavingEX.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SavingEX.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndSaveEX.mId = value19);
    (l_fileSelect.StateID_EndSaveEX.mInitialize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndSaveEX.mInitialize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndSaveEX.mExecute.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SavingEX.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndSaveEX.mExecute.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_nextStateId = value19);
    (l_fileSelect.StateID_EndSaveEX.mFinalize.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_SavingEX.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndSaveEX.mpName = guest_addr("::StateID_EndSaveEX"));
    (l_fileSelect.StateID_EndSaveEX.mFinalize.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndSaveEX.mVtable = kStateVtable);
    (l_fileSelect.StateID_EndSaveEX.mCheck.mThisAdjust = u16(0x00000000u));
    (l_fileSelect.StateID_EndSaveEX.mInitialize.mTarget = kinitializeEndSaveEX);
    (l_fileSelect.StateID_EndSaveEX.mExecute.mTarget = kdFile_select_c_executeEndSaveEX);
    (l_fileSelect.StateID_EndSaveEX.mFinalize.mTarget = kfinalizeEndSaveEX);
    (l_fileSelect.StateID_EndSaveEX.mCheck.mVirtualSlot = u16(0xFFFFFFFFu));
    (l_fileSelect.StateID_EndSaveEX.mCheck.mTarget = kcheckEndSaveEX);
}

/* 026D3740 */
static void fileSelectSafeString__dt(u32 p, u32 f) {
    if (p && (f & 1))
        fileSelectHD_operatorDelete(p);
}

/* 026D3754 */
static void fileSelectSafeString_assureTerminationImpl() {}

/* 026D3758 */
static void executeWait() {}

/* 026D375C */
static void finalizeWait() {}

/* 026D3760 */
static u32 checkWait() { return 0; }

/* 026D3768 */
static void fileSelectSelectSaveData_assureTerminationImpl() {}

/* 026D376C */
static void finalizeSelectSaveData() {}

/* 026D3770 */
static u32 checkSelectSaveData() { return 0; }

/* 026D3778 */
static void initializeWaitNameInput() {}

/* 026D377C */
static void finalizeWaitNameInput() {}

/* 026D3780 */
static u32 checkWaitNameInput() { return 0; }

/* 026D3788 */
static void finalizeShowSaveData() {}

/* 026D378C */
static u32 checkShowSaveData() { return 0; }

/* 026D3794 */
static void finalizeCancelSaveData() {}

/* 026D3798 */
static u32 checkCancelSaveData() { return 0; }

/* 026D37A0 */
static void finalizeSelectDownButton() {}

/* 026D37A4 */
static u32 checkSelectDownButton() { return 0; }

/* 026D37AC */
static void initializeDecideSaveData() {}

/* 026D37B0 */
static void finalizeDecideSaveData() {}

/* 026D37B4 */
static u32 checkDecideSaveData() { return 0; }

/* 026D37BC */
static void fileSelectOutAll_assureTerminationImpl() {}

/* 026D37C0 */
static void initializeOutAll() {}

/* 026D37C4 */
static void executeOutAll() {}

/* 026D37C8 */
static void finalizeOutAll() {}

/* 026D37CC */
static u32 checkOutAll() { return 0; }

/* 026D37D4 */
static u32 checkDecideDeleteYesNo() { return 0; }

/* 026D37DC */
static void finalizeWaitDeleting() {}

/* 026D37E0 */
static u32 checkWaitDeleting() { return 0; }

/* 026D37E8 */
static void finalizeEndDelete() {}

/* 026D37EC */
static u32 checkEndDelete() { return 0; }

/* 026D37F4 */
static void finalizeSelectFileCopyTo() {}

/* 026D37F8 */
static u32 checkSelectFileCopyTo() { return 0; }

/* 026D3800 */
static u32 checkDecideCopyYesNo() { return 0; }

/* 026D3808 */
static void finalizeWaitCopying() {}

/* 026D380C */
static u32 checkWaitCopying() { return 0; }

/* 026D3814 */
static void finalizeEndCopy() {}

/* 026D3818 */
static u32 checkEndCopy() { return 0; }

/* 026D3820 */
static void finalizeSaveEXYesNo() {}

/* 026D3824 */
static u32 checkSaveEXYesNo() { return 0; }

/* 026D382C */
static void finalizeSavingEX() {}

/* 026D3830 */
static u32 checkSavingEX() { return 0; }

/* 026D3838 */
static void finalizeEndSaveEX() {}

/* 026D383C */
static u32 checkEndSaveEX() { return 0; }

/* 026D3844 */
static u32 fileSelectState_getId(u32 p) { return at<State>(p)->mId.get(); }

/* 026D384C */
static void fileSelectState_prepare() {}

/* 026D3850 */
static void fileSelectState_enter() {}

/* 026D3854 */
static void fileSelectState_leave() {}

/* 026D3858 */
static u32 fileSelectState_initialize(u32 desc, u32 base, u32 r5, u32 r6, u32 r7, u32 r8, u32 r9,
                                      u32 r10) {
    u32 self;
    const u32 target = memberTarget(desc, base, 12, self, r9, r10);
    return tail_ptr<u32>(target, self, base, r5, r6, r7, r8, r9, r10);
}

/* 026D3898 */
static u32 fileSelectState_execute(u32 desc, u32 base, u32 r5, u32 r6, u32 r7, u32 r8, u32 r9,
                                   u32 r10) {
    u32 self;
    const u32 target = memberTarget(desc, base, 20, self, r9, r10);
    return tail_ptr<u32>(target, self, base, r5, r6, r7, r8, r9, r10);
}

/* 026D38D8 */
static u32 fileSelectState_finalize(u32 desc, u32 base, u32 r5, u32 r6, u32 r7, u32 r8, u32 r9,
                                    u32 r10) {
    u32 self;
    const u32 target = memberTarget(desc, base, 28, self, r9, r10);
    return tail_ptr<u32>(target, self, base, r5, r6, r7, r8, r9, r10);
}

/* 026D3918 */
static u32 fileSelectStateProcessor_getId(u32 p, u32 r4, u32 r5, u32 r6, u32 r7, u32 r8, u32 r9,
                                          u32 r10) {
    const u32 desc = at<StateProcessor>(p)->mpDescriptor.get();
    if (s32(at<State>(desc)->mId.get()) == -1)
        return at<StateProcessor>(p)->mId.get();
    return call_ptr<u32>(at<Vtable>(at<State>(desc)->mVtable.get())->mGetId.get(), desc, r4, r5, r6,
                         r7, r8, r9, r10);
}
/* WWHD glue (tools/verify/autoglue.py): private HD screen bindings
ext 02001338 void fileSelectHD_eventConstruct(u32, u32, u32, u32)
ext 020013C0 void fileSelectHD_eventQueuePop(u32, u32)
ext 02001490 void fileSelectHD_eventDestruct(u32)
ext 02001FF4 void fileSelectHD_eventQueuePush(u32, u32)
ext 02004E04 void fileSelectHD_bindAnimation(u32, u32, u32, u32)
ext 020053E4 u32 fileSelectHD_playAnimation(u32, u32, u32, f32)
ext 02005674 void fileSelectHD_startAnimation(u32, u32, u32)
ext 02005708 void fileSelectHD_stopAnimation(u32, u32, u32)
ext 02005840 u32 fileSelectHD_isAnimationFinished(u32, u32)
ext 02006364 u32 fileSelectHD_updateState(u32)
ext 020063C0 void fileSelectHD_changeState(u32, u32)
ext 02006478 u32 fileSelectHD_getState(u32)
ext 0200EE00 f32 fileSelectHD_addCalcPos(u32, u32, f32, f32, f32)
ext 0200F564 u32 fileSelectHD_chaseAlpha(u32, u32, u32)
ext 02035D78 u32 fileSelectHD_getInputManager()
ext 02035ED4 void fileSelectHD_setInputEnabled(u32, u32)
ext 025B7B80 void fileSelectHD_onTriforce(u32, u32)
ext 025B7BE8 void fileSelectHD_offTriforce(u32, u32)
ext 025B7CC0 void fileSelectHD_onSymbol(u32, u32)
ext 025B7D28 void fileSelectHD_offSymbol(u32, u32)
ext 025E1988 void fileSelectHD_startSound(u32)
ext 02615154 u32 fileSelectHD_setFocus(u32, u32)
ext 0262DEF4 void fileSelectHD_difficultySelectHero(u32)
ext 0262DF4C void fileSelectHD_difficultySelectNormal(u32)
ext 02642EEC void fileSelectHD_savePaneReload(u32)
ext 02643108 void fileSelectHD_savePaneUpdate(u32)
ext 02644008 u32 fileSelectHD_savePaneGetStatus(u32)
ext 02644074 void fileSelectHD_savePaneRefresh(u32)
ext 02644F0C void fileSelectHD_heartRefresh(u32)
ext 026476AC void fileSelectHD_collectionRefresh(u32)
ext 026CBF4C void fileSelectHD_fileSelectResetFocus(u32)
ext 026CBFCC void fileSelectHD_dFile_select_c_clearNavigation(u32)
ext 026CC1CC void fileSelectHD_Transition_initialize(u32, u32, u32, u32, u32, u32, u32, f32)
ext 026CC22C void fileSelectHD_Motion_setTransition(u32, u32, u32, u32, u32, u32, u32, u32)
ext 026CCB8C u32 fileSelectHD_Transition_move(u32)
ext 026CCC94 u32 fileSelectHD_dFile_select_c_updateTransitions(u32)
ext 026CCECC u32 fileSelectHD_dFile_select_c_slotIdle(u32, u32)
ext 026CCF48 void fileSelectHD_dFile_select_c_refreshSlots(u32)
ext 026CD0F4 u32 fileSelectHD_dFile_select_c_takeEvent(u32)
ext 026CD200 void fileSelectHD_dFile_select_c_linkSlots(u32)
ext 026CD384 u32 fileSelectHD_dFile_select_c_selectCancel(u32)
ext 026CD420 void fileSelectHD_dFile_select_c_selectionTransition(u32)
ext 026CD628 u32 fileSelectHD_dFile_select_c_selectDone(u32)
ext 026CD694 void fileSelectHD_dFile_select_c_enableSlots(u32)
ext 026CD8E4 void fileSelectHD_dFile_select_c_loadSlotData(u32, u32)
ext 026CDA70 u32 fileSelectHD_dFile_select_c_slotFlag(u32, u32)
ext 026CDAEC void fileSelectHD_dFile_select_c_enableButtons(u32)
ext 026CDCF4 void fileSelectHD_dFile_select_c_dataLabels(u32)
ext 026CDEE4 void fileSelectHD_dFile_select_c_deleteButtons(u32)
ext 026CDFE0 void fileSelectHD_dFile_select_c_choiceButtons(u32)
ext 026CE368 u32 fileSelectHD_dFile_select_c_buttonOne(u32)
ext 026CE728 void fileSelectHD_dFile_select_c_linkButtonsIn(u32)
ext 026CEAD4 void fileSelectHD_dFile_select_c_animateSlotsIn(u32)
ext 026CEC18 void fileSelectHD_dFile_select_c_enableSelection(u32)
ext 026CF068 void fileSelectHD_dFile_select_c_linkButtons(u32)
ext 026CF1B8 void fileSelectHD_dFile_select_c_copySound(u32)
ext 026CF1E0 void fileSelectHD_dFile_select_c_enableDelete(u32)
ext 026CF838 void fileSelectHD_dFile_select_c_copyChoicesInit(u32)
ext 026CFA2C void fileSelectHD_dFile_select_c_linkCopyChoices(u32)
ext 026CFB74 u32 fileSelectHD_dFile_select_c_cancelCopy(u32)
ext 026CFF10 void fileSelectHD_dFile_select_c_enableCopySlots(u32)
ext 026D0130 void fileSelectHD_dFile_select_c_copyConfirmInit(u32)
ext 026D0490 void fileSelectHD_dFile_select_c_copyCancelInit(u32)
ext 026D0C14 void fileSelectHD_dFile_select_c_deleteSlotsIn(u32)
ext 026D0F94 void fileSelectHD_dFile_select_c_backSound(u32)
ext 026D1804 void fileSelectHD_dFile_select_c_findScreenParts(u32)
ext 026D1A14 void fileSelectHD_dFile_select_c_findParts(u32)
ext 026D1AF0 void fileSelectHD_Transition_clear(u32)
ext 026D1B34 void fileSelectHD_Motion_initialize(u32)
ext 026D1C7C void fileSelectHD_Motion_setPaneOrigins(u32, u32, u32)
ext 026F17BC void fileSelectHD_paneSelect(u32)
ext 026F226C void fileSelectHD_paneResetSelection(u32)
ext 026F2540 void fileSelectHD_paneSetConfirmSound(u32, u32, u32)
ext 026F5B74 u32 fileSelectHD_paneGetId(u32)
ext 026F5C08 void fileSelectHD_paneSetNavigation(u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 026F5E80 void fileSelectHD_paneSetText(u32, u32, u32)
ext 026F620C u32 fileSelectHD_eventIsValid(u32)
ext 026F6340 u32 fileSelectHD_eventQueueHasCancel(u32)
ext 026F6524 void fileSelectHD_paneSetTouchSound(u32, u32, u32)
ext 026F6534 void fileSelectHD_paneSetSelectionSound(u32, u32, u32)
ext 026F88F0 void fileSelectHD_baseDestruct(u32, u32)
ext 026F89E8 void fileSelectHD_baseConstruct(u32)
ext 026F90D8 void fileSelectHD_setModuleName(u32, u32, u32)
ext 026F91F8 void fileSelectHD_baseCalc(u32)
ext 026F92C0 u32 fileSelectHD_baseCreatePane(u32, u32, u32)
ext 026FADB0 u32 fileSelectHD_layoutConstruct(u32)
ext 0271FC5C u32 fileSelectHD_slotGetHeroMode(u32)
ext 0271FC70 void fileSelectHD_slotSetHeroMode(u32, u32)
ext 027227A8 void fileSelectHD_copySlot(u32, u32, u32)
ext 02726FC4 void fileSelectHD_serviceRefresh(u32)
ext 027271F0 u32 fileSelectHD_serviceGetSlotData(u32, u32)
ext 0273AD10 u32 fileSelectHD_operatorNew(u32)
ext 0273AF40 void fileSelectHD_operatorDelete(u32)
ext 0273B050 u32 fileSelectHD_heapNew(u32, u32, u32)
ext 028EAB2C void fileSelectHD_constructArchiveByte(u32)
ext 028ED6F8 void fileSelectHD_constructHeaderByte(u32)
ext 028F026C void fileSelectHD_registerGlobalObject(u32)
ext 028F4384 f64 fileSelectHD_sqrt(f64)
data l_fileSelect 1049A3E0
data mInputManager 101F4FF0
data mPad 101F5088
data mAllocationManager 1018C404
data mSave 101F84DC
data mSaveManager 101F852C
data l_stateIdInitialized 101FD8E4
data l_nextStateId 101FDD50
data l_saveComplete 1047B07C
data l_animationRate 101021A8
data l_initialSpeedRatio 101021AC
data l_copyOutOffset 101021B0
data l_nameOutOffset 101021B4
data l_acceleration 101021C0
data l_positionRatio 101021C4
data l_positionStep 101021C8
data l_zero 101021CC
data l_slotDelayFactor 101021D0
data l_slotMaxFrames 101021D8
data l_slotOutOffset 10102278
data l_negativePi 10102288
data l_positivePi 1010228C
data l_eventPanes 10102074
data l_eventCodes 10102094
data l_animationRootIndices 101020B4
str 101021DC "SE_DRC_TOUCH_ON"
str 101021EC "SE_ENTER_GAME"
str 101021FC "T_Btn_00" in 026CDEE4
str 10102208 "T_Btn_00" in 026CDFE0
str 10102214 "N_BtnSeqence_00"
str 10102224 "N_SaveData_00"
str 10102234 "N_SaveDataPosM_00"
str 10102248 "N_SaveDataPosB_00"
str 1010225C "N_All_00"
str 10102268 "N_StatusPos_00"
str 1010227C "T_Btn_00" in 026D1D14
str 10102290 "InAll"
str 10102298 "OutAll"
str 101022A0 "L_BtnSeqence_00"
str 101022B0 "L_BtnSeqence_01"
str 101022C0 "L_BtnSeqence_02"
str 101022D0 "L_BtnSeqence_03"
str 101022E0 "L_Collection_00"
str 101022F0 "L_SaveData_00-T_Btn_yes"
str 10102308 "::StateID_InAll"
str 10102318 "::StateID_WaitNameInput"
str 10102330 "G_Button_00"
str 1010233C "G_Status_00"
str 10102348 "SaveData_00"
str 10102354 "HeartAll_00"
str 10102360 "BtnPearl_00"
str 1010236C "::StateID_OutAllEnd"
str 10102380 "::StateID_DecideDeleteYesNo"
str 1010239C "::StateID_EndDelete"
str 101023B0 "::StateID_EndSaveEX"
str 101023C4 "OutBtnSeqence"
str 101023D4 "OutStates"
str 101023E0 "L_SaveData_00"
str 101023F0 "L_SaveData_01"
str 10102400 "L_SaveData_02"
str 10102410 "L_HeartAll_00"
str 10102420 "Collection_00"
str 10102430 "L_SaveData_00-T_Btn_start"
str 1010244C "::StateID_DecideCopyYesNo"
str 10102468 "::StateID_WaitCopying"
str 10102480 "::StateID_EndCopy"
str 10102494 "::StateID_SaveEXYesNo"
str 101024AC "G_All_00"
str 101024B8 "G_BtnDifficulty_00"
str 101024CC "InBtnSeqence"
str 101024DC "InStates"
str 101024E8 "OnOffBtnDifficulty"
str 101024FC "L_BtnDifficulty_00"
str 10102510 "BtnSequence_00"
str 10102520 "BtnSaveData_00"
str 10102530 "BtnDifficulty_00"
str 10102544 "Heart_00"
str 10102550 "BtnTriforce_00"
str 10102560 "L_SaveData_00-T_Btn_back"
str 1010257C "L_SaveData_00-T_Btn_delete"
str 10102598 "L_SaveData_00-T_Btn_copy"
str 101025B4 "L_SaveData_00-T_Btn_no"
str 101025CC "::StateID_Wait"
str 101025DC "::StateID_SelectSaveData"
str 101025F8 "::StateID_ShowSaveData"
str 10102610 "::StateID_CancelSaveData"
str 1010262C "::StateID_SelectDownButton"
str 10102648 "::StateID_DecideSaveData"
str 10102664 "::StateID_OutAll"
str 10102678 "::StateID_WaitDeleting"
str 10102690 "::StateID_SelectFileCopyTo"
str 101026AC "::StateID_SavingEX"
ext 027225AC void fileSelectHD_deleteSlot(u32, u32)
ext 0272249C void fileSelectHD_saveSlot(u32, u32)
data l_savePaneType 10490D0C
data l_collectionPaneType 104910F0
data l_namePaneType 10494884
data l_difficultyPaneType 1048F95C
data l_difficultySelectionState 1048F964
data l_nameInputState 10494824
data l_soundArchive 104A0CD8
ext 02642870 u32 fileSelectHD_savePaneConstruct(u32,u32,u32)
ext 02647250 u32 fileSelectHD_collectionPaneConstruct(u32)
ext 0266D9F0 u32 fileSelectHD_namePaneConstruct(u32)
ext 0262DE3C u32 fileSelectHD_difficultyPaneConstruct(u32,u32)
ext 02643F7C u32 fileSelectHD_savePaneCreate(u32,u32,u32,u32)
ext 026474A8 u32 fileSelectHD_collectionPaneCreate(u32,u32,u32,u32)
ext 0266DB04 u32 fileSelectHD_namePaneCreate(u32,u32,u32,u32)
ext 0262DED8 u32 fileSelectHD_difficultyPaneCreate(u32,u32,u32,u32)
ext 027200A0 u32 fileSelectHD_saveGetSlot(u32,u32)
*/
