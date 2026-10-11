// HD: cking::ui::input::EventMgr dispatches touch and pointer events to UI receivers.
// This manager has no GameCube counterpart.
#include "d/hd_input_eventmgr.h"
using namespace gabi;

using namespace cking::ui::input;
const gc_tu_data gc_tu = {.safeString = 0x100E2610};
static cking::ui::input::Constants l_constants;
static u32 l_instance;
static u32 l_disposer;
static u32 l_inputManager;
static u32 l_options;
static cking::ui::input::Vec2 l_invalidTouch;
static cking::ui::input::Vec2 l_invalidDelta;
static cking::ui::input::StaticData l_staticData;
static cking::ui::input::DestructorRecords l_destructorRecords;

static f32 zero() { return (f32)l_constants.mZero; }

/* constructor */
/* 02614BE0 */
u32 hdUiEventMgr_construct(u32 p) {
    if (!p) {
        p = hdUiAllocate(sizeof(EventMgr));
        if (!p)
            return 0;
    }
    auto* manager = reinterpret_cast<EventMgr*>((uintptr_t)p);
    hdUiEventMgrBaseConstruct(manager);
    manager->mpFocus = nullptr;
    manager->mpHover = nullptr;
    manager->mMode = 0;
    manager->mpVtable = reinterpret_cast<EventMgrVtable*>((uintptr_t)ManagerVtable);
    manager->mSecondaryVtable = SecondaryVtable;
    manager->mNextMode = 0;
    const f32 z = zero();
    manager->mParamA = 0;
    manager->mpGrab = nullptr;
    Vec2* const vectors[] = {&manager->mTouchStart, &manager->mTouchCurrent,   &manager->mTouchPrevious,
                             &manager->mVelocity,   &manager->mSamplePrevious, &manager->mSampleCurrent,
                             &manager->mSwipe};
    for (u32 i = 0; i < 7; ++i) {
        if (i == 4)
            manager->mInertiaFrames = INERTIA_IDLE;
        Vec2* vector = reinterpret_cast<Vec2*>((uintptr_t)gabi::ea(vectors[i]));
        if (!vector)
            vector = reinterpret_cast<Vec2*>((uintptr_t)hdUiAllocate(sizeof(Vec2)));
        if (vector) {
            vector->value.y = z;
            vector->value.x = z;
        }
    }
    manager->mLastId = 0;
    manager->mModeChanged = 0;
    manager->mInertiaLength = 20;
    manager->mStopSpeed = l_constants.mStopSpeed;
    manager->mCurrentId = 0;
    manager->mFriction = l_constants.mFriction;
    manager->mParamB = 8;
    manager->mMargin = l_constants.mMargin;
    return p;
}

/* creates the singleton with its disposer */
/* 02614DAC */
void hdUiEventMgr_createInstance(u32 heap) {
    if (l_instance)
        return;
    const u32 p = hdUiAllocateAligned(sizeof(EventMgr), heap, 4u);
    const u32 d = gabi::ea(&reinterpret_cast<EventMgr*>((uintptr_t)p)->mDisposer);
    if (d) {
        hdUiDisposerConstruct(d, heap, 3u);
        reinterpret_cast<Disposer*>((uintptr_t)d)->mVtable = DisposerVtable;
    }
    l_disposer = d;
    l_instance = p ? hdUiEventMgr_construct(p) : 0u;
}

/* destructor */
/* 02614E48 */
void hdUiEventMgr_dtor(u32 p, u32 flags) {
    if (!p)
        return;
    reinterpret_cast<EventMgr*>((uintptr_t)p)->mSecondaryVtable = SecondaryVtable;
    hdUiEventMgrBaseDestroy(p, 0u);
    if (flags & 1)
        hdUiFree(p);
}

/* prepare: the event heap */
/* 02614EA8 */
void cking::ui::input::EventMgr::prepare() {
    Local<SafeString> name;
    const u32 size = hdUiHeapHeaderSize(4u) + 0x238;
    name->__vtbl = gc_tu.safeString;
    name->mStringTop = guest_addr("cking::ui::input::EventMgr::prepare");
    const u32 parent = hdUiGetParentHeap();
    const u32 heap = hdUiCreateHeap(size, name.a, parent, 1u, 0u);
    hdUiEventQueuePrepare(gabi::ea(this), heap, 0xAu);
    call_ptr<void>(reinterpret_cast<cking::ui::input::Heap*>((uintptr_t)heap)->mpVtable->mReset.get(), heap);
}

/* current id becomes the last id */
/* 02614F40 */
void cking::ui::input::EventMgr::commitId() {
    const u32 cur = (u32)mCurrentId;
    mCurrentId = 0;
    mLastId = cur;
}

/* sets the mode (flags a change) */
/* 02614F54 */
void cking::ui::input::EventMgr::setMode(u32 mode) {
    if ((u32)mMode == mode)
        return;
    mMode = mode;
    mNextMode = mode;
    mModeChanged = 1;
}

/* per-frame update */
/* 02614F74 */
void cking::ui::input::EventMgr::calc() {
    commitId();
    const u32 mode = (u32)mNextMode;
    mModeChanged = 0;
    setMode(mode);
    hdUiEventQueueBeginFrame(gabi::ea(this));
    hdUiEventQueueProcess(gabi::ea(this));
}

/* forwards to the event queue */
/* 02614FBC */
void cking::ui::input::EventMgr::forward() { hdUiEventQueueForward(gabi::ea(this)); }

/* 02614FC0 */
void cking::ui::input::EventMgr::setFocus(u32 v) {
    mpFocus = reinterpret_cast<cking::ui::input::Receiver*>((uintptr_t)(v));
}
/* 02614FC8 */
void cking::ui::input::EventMgr::setGrab(u32 v) {
    mpGrab = reinterpret_cast<cking::ui::input::Receiver*>((uintptr_t)(v));
}
/* 02614FD0 */
void cking::ui::input::EventMgr::setHover(u32 v) {
    mpHover = reinterpret_cast<cking::ui::input::Receiver*>((uintptr_t)(v));
}

/* reset: clears the state and unlinks every receiver */
/* 02614FD8 */
void cking::ui::input::EventMgr::reset() {
    mReceiverId = 0;
    setFocus(0);
    setGrab(0);
    setHover(0);
    mLastId = 0;
    const f32 z = zero();
    u32 node = gabi::ea((cking::ui::input::ReceiverNode*)mFirst);
    mSamplePrevious.value.y = z;
    mSwipe.value.x = z;
    mParamB = 8;
    mCurrentId = 0;
    mVelocity.value.x = z;
    mTouchStart.value.y = z;
    mTouchCurrent.value.x = z;
    mSamplePrevious.value.x = z;
    mTouchPrevious.value.x = z;
    mModeChanged = 0;
    mVelocity.value.y = z;
    mInertiaFrames = INERTIA_IDLE;
    mTouchStart.value.x = z;
    mTouchPrevious.value.y = z;
    mTouchCurrent.value.y = z;
    mSwipe.value.y = z;
    while (node != gabi::ea(this)) {
        const u32 n = node;
        const u32 owner = gabi::ea(
            (cking::ui::input::ReceiverList*)reinterpret_cast<cking::ui::input::ReceiverNode*>((uintptr_t)n)
                ->mpOwner);
        node = gabi::ea((cking::ui::input::ReceiverNode*)reinterpret_cast<cking::ui::input::ReceiverNode*>(
                            (uintptr_t)node)
                            ->mNext);
        if (owner) {
            reinterpret_cast<cking::ui::input::ReceiverNode*>((uintptr_t)n)->mpOwner =
                reinterpret_cast<cking::ui::input::ReceiverList*>((uintptr_t)(0));
            hdUiListErase(n);
            mCount = (u32)mCount - 1;
        }
    }
    hdUiEventQueueClear(gabi::ea(mEventQueue));
}

/* TList::pushBack (moves the node from its previous list) */
/* 026150C0 */
void hdUiReceiverList_pushBack(u32 list, u32 node) {
    auto* target = reinterpret_cast<ReceiverList*>((uintptr_t)list);
    auto* entry = reinterpret_cast<ReceiverNode*>((uintptr_t)node);
    ReceiverList* owner = entry->mpOwner;
    if (owner) {
        entry->mpOwner = nullptr;
        hdUiListErase(node);
        owner->mCount = (u32)owner->mCount - 1;
    }
    entry->mpOwner = target;
    hdUiListAppend(list, node);
    target->mCount = (u32)target->mCount + 1;
}

/* 02615140 */
u32 cking::ui::input::EventMgr::getMode() { return (u32)mMode; }
/* 02615148 */
void cking::ui::input::EventMgr::setParams(u32 a, u32 b) {
    mParamA = a;
    mParamB = b;
}
/* 02615154 */
void cking::ui::input::EventMgr::setModeDefault(u32 mode) {
    setMode(mode);
    setParams(0, 8);
}
/* 02615180 */
void cking::ui::input::EventMgr::setNextMode(u32 v) { mNextMode = v; }
/* 02615188 */
u8 cking::ui::input::EventMgr::isModeChanged() { return (u8)mModeChanged; }
/* 02615190 */
u32 cking::ui::input::EventMgr::getParamA() { return (u32)mParamA; }
/* 02615198 */
u32 cking::ui::input::EventMgr::getParamB() { return (u32)mParamB; }
/* 026151A0 */
u32 cking::ui::input::EventMgr::getHover() { return gabi::ea((cking::ui::input::Receiver*)mpHover); }

/* touch position delta to (bx, by) as two float words; the default vector 104A0908 when invalid */
/* inlined into EventMgr input methods */
static Pair32 touchDelta(u32 src, const Vec2* base) {
    const u32 wx = (u32) reinterpret_cast<Vec2*>((uintptr_t)src)->words[0],
              wy = (u32) reinterpret_cast<Vec2*>((uintptr_t)src)->words[1];
    const f32 lim = (f32)l_constants.mTouchLimit;
    Local<Vec2> v;
    v->words[0] = wx;
    v->words[1] = wy;
    const f32 x = (f32)v->value.x, y = (f32)v->value.y;
    if (x > lim && y > lim) {
        Local<Vec2> d;
        d->value.x = fsubs_ppc(x, (f32)base->value.x);
        d->value.y = fsubs_ppc(y, (f32)base->value.y);
        return {(u32)d->words[0], (u32)d->words[1]};
    }
    return {(u32)l_invalidDelta.words[0], (u32)l_invalidDelta.words[1]};
}

/* touch delta from the touch start (current controller's touch) */
/* 026151A8 */
Pair32 cking::ui::input::EventMgr::touchFromStart() {
    const u32 c = l_inputManager;
    const u32 src =
        ((u32) reinterpret_cast<InputManager*>((uintptr_t)c)->mPointer.mPointerFlags & POINTER_VALID)
            ? gabi::ea(&reinterpret_cast<InputManager*>((uintptr_t)c)->mPointer.mTouch)
            : gabi::ea(&l_invalidTouch);
    return touchDelta(src, &mTouchStart);
}

/* touch delta from the touch start (GamePad touch position) */
/* 02615260 */
Pair32 cking::ui::input::EventMgr::padTouchFromStart() {
    return touchDelta(gabi::ea(&reinterpret_cast<InputManager*>((uintptr_t)l_inputManager)->mPointer.mTouch),
                      &mTouchStart);
}

/* touch delta from the previous position */
/* 026152FC */
Pair32 cking::ui::input::EventMgr::padTouchFromPrev() {
    return touchDelta(gabi::ea(&reinterpret_cast<InputManager*>((uintptr_t)l_inputManager)->mPointer.mTouch),
                      &mTouchPrevious);
}

/* the current id, else the last one */
/* 02615398 */
u32 cking::ui::input::EventMgr::getId() {
    const u32 cur = (u32)mCurrentId;
    return cur ? cur : (u32)mLastId;
}

static u32 checkInput(u32 self, u32 off, u32 ev) {
    return call_ptr<u32>(
        reinterpret_cast<EventMgr*>((uintptr_t)self)->mpVtable->mSlots[off / sizeof(gfn)].get(), self, ev);
}
static void dispatchCommand(u32 self, u32 off) {
    call_ptr<void>(reinterpret_cast<EventMgr*>((uintptr_t)self)->mpVtable->mSlots[off / sizeof(gfn)].get(),
                   self);
}
static void dispatchPointer(u32 self, u32 off, u32 ev) {
    call_ptr<void>(reinterpret_cast<EventMgr*>((uintptr_t)self)->mpVtable->mSlots[off / sizeof(gfn)].get(),
                   self, ev);
}

/* shifts the touch history and stores the new position */
/* inlined into EventMgr input methods */
static void shiftTouch(u32 self, u32 pos) {
    const f32 a =
        (f32) reinterpret_cast<cking::ui::input::EventMgr*>((uintptr_t)self)->mSampleCurrent.value.x;
    const f32 x = (f32) reinterpret_cast<Vec2*>((uintptr_t)pos)->value.x;
    reinterpret_cast<cking::ui::input::EventMgr*>((uintptr_t)self)->mSamplePrevious.value.x = a;
    const f32 b =
        (f32) reinterpret_cast<cking::ui::input::EventMgr*>((uintptr_t)self)->mSampleCurrent.value.y;
    reinterpret_cast<cking::ui::input::EventMgr*>((uintptr_t)self)->mSampleCurrent.value.x = x;
    const f32 y = (f32) reinterpret_cast<Vec2*>((uintptr_t)pos)->value.y;
    reinterpret_cast<cking::ui::input::EventMgr*>((uintptr_t)self)->mSamplePrevious.value.y = b;
    reinterpret_cast<cking::ui::input::EventMgr*>((uintptr_t)self)->mSampleCurrent.value.y = y;
}

/* event dispatch: touch handling, then the button checks in priority order */
/* 026153B4 */
u32 cking::ui::input::EventMgr::dispatch(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    if (!ev)
        return 1;
    const u32 src =
        ((u32)event->mPointerFlags & POINTER_VALID) ? ev + SLOT_OnHoldBit12 : gabi::ea(&l_invalidTouch);
    Local<Vec2> pos;
    pos->words[0] = (u32) reinterpret_cast<Vec2*>((uintptr_t)src)->words[0];
    const f32 lim = (f32)l_constants.mTouchLimit;
    const f32 x = (f32)pos->value.x;
    pos->words[1] = (u32) reinterpret_cast<Vec2*>((uintptr_t)src)->words[1];
    if (x > lim && (f32)pos->value.y > lim) {
        if ((u32)event->mHold & BUTTON_TOUCH) {
            dispatchPointer(gabi::ea(this), SLOT_PointerDown, ev);
            shiftTouch(gabi::ea(this), pos.a);
            return 1;
        }
        if ((u32)event->mRepeat & BUTTON_TOUCH) {
            dispatchPointer(gabi::ea(this), SLOT_PointerMove, ev);
            shiftTouch(gabi::ea(this), pos.a);
            return 1;
        }
    } else {
        if (s32((u32)mInertiaFrames) > 0 || ((u32)event->mRelease & BUTTON_TOUCH)) {
            dispatchPointer(gabi::ea(this), SLOT_PointerUp, ev);
            return 1;
        }
        mReceiverId = 0;
        setFocus(0);
        setHover(0);
        const f32 z = zero();
        mSampleCurrent.value.x = z;
        mSamplePrevious.value.y = z;
        mSamplePrevious.value.x = z;
        mSampleCurrent.value.y = z;
    }
    if (checkInput(gabi::ea(this), SLOT_CheckHoldBit11, ev)) {
        dispatchCommand(gabi::ea(this), SLOT_OnHoldBit11);
        return 1;
    }
    if (checkInput(gabi::ea(this), SLOT_CheckHoldBit12, ev)) {
        dispatchCommand(gabi::ea(this), SLOT_OnHoldBit12);
        return 1;
    }
    if ((u8) reinterpret_cast<Options*>((uintptr_t)l_options)->mDirectionalInputEnabled) {
        if (checkInput(gabi::ea(this), SLOT_CheckLeft, ev)) {
            if (checkInput(gabi::ea(this), SLOT_CheckUp, ev))
                dispatchCommand(gabi::ea(this), SLOT_OnLeftUp);
            else if (checkInput(gabi::ea(this), SLOT_CheckDown, ev))
                dispatchCommand(gabi::ea(this), SLOT_OnLeftDown);
            else
                dispatchCommand(gabi::ea(this), SLOT_OnLeft);
            return 1;
        }
        if (checkInput(gabi::ea(this), SLOT_CheckRight, ev)) {
            if (checkInput(gabi::ea(this), SLOT_CheckUp, ev))
                dispatchCommand(gabi::ea(this), SLOT_OnRightUp);
            else if (checkInput(gabi::ea(this), SLOT_CheckDown, ev))
                dispatchCommand(gabi::ea(this), SLOT_OnRightDown);
            else
                dispatchCommand(gabi::ea(this), SLOT_OnRight);
            return 1;
        }
        constexpr u16 pairs[][2] = {{SLOT_CheckUp, SLOT_OnUp},
                                    {SLOT_CheckDown, SLOT_OnDown},
                                    {SLOT_CheckHoldBit3, SLOT_OnHoldBit3},
                                    {SLOT_CheckHoldBit4, SLOT_OnHoldBit4},
                                    {SLOT_CheckHoldBit14, SLOT_OnHoldBit14},
                                    {SLOT_CheckHoldBit0, SLOT_OnHoldBit0},
                                    {SLOT_CheckHoldBit2, SLOT_OnHoldBit2},
                                    {SLOT_CheckHoldBit5, SLOT_OnHoldBit5},
                                    {SLOT_CheckHoldBit1, SLOT_OnHoldBit1}};
        for (const auto& pr : pairs)
            if (checkInput(gabi::ea(this), pr[0], ev)) {
                dispatchCommand(gabi::ea(this), pr[1]);
                return 1;
            }
    }
    constexpr u16 tail[][2] = {{SLOT_CheckHoldBit13, SLOT_OnHoldBit13},
                               {SLOT_CheckFallbackFirst, SLOT_OnFallbackFirst},
                               {SLOT_CheckFallbackSecond, SLOT_OnFallbackSecond},
                               {SLOT_CheckFallbackThird, SLOT_OnFallbackThird},
                               {SLOT_CheckFallbackFourth, SLOT_OnFallbackFourth}};
    for (const auto& pr : tail)
        if (checkInput(gabi::ea(this), pr[0], ev)) {
            dispatchCommand(gabi::ea(this), pr[1]);
            return 1;
        }
    return 0;
}

/* touch down: focuses the first receiver hit by the touch */
/* 02615C80 */
u32 cking::ui::input::EventMgr::touchDown(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    setGrab(0);
    setHover(0);
    const f32 z = zero();
    mInertiaFrames = INERTIA_IDLE;
    mVelocity.value.y = z;
    mSampleCurrent.value.x = z;
    mTouchStart.value.y = z;
    mTouchStart.value.x = z;
    mSampleCurrent.value.y = z;
    mTouchPrevious.value.y = z;
    mSamplePrevious.value.y = z;
    mVelocity.value.x = z;
    mTouchCurrent.value.x = z;
    mSamplePrevious.value.x = z;
    mTouchCurrent.value.y = z;
    mTouchPrevious.value.x = z;
    if (!ev) {
        mReceiverId = 0;
        setFocus(0);
        return 0;
    }
    const u32 src =
        ((u32)event->mPointerFlags & POINTER_VALID) ? gabi::ea(&event->mTouch) : gabi::ea(&l_invalidTouch);
    Local<Vec2> pos;
    pos->words[0] = (u32) reinterpret_cast<Vec2*>((uintptr_t)src)->words[0];
    u32 node = gabi::ea((cking::ui::input::ReceiverNode*)mFirst);
    pos->words[1] = (u32) reinterpret_cast<Vec2*>((uintptr_t)src)->words[1];
    if (node == gabi::ea(this))
        return 0;
    const f32 x = (f32)pos->value.x, y = (f32)pos->value.y;
    do {
        const u32 item = gabi::ea(
            (cking::ui::input::Receiver*)reinterpret_cast<cking::ui::input::ReceiverNode*>((uintptr_t)node)
                ->mpReceiver);
        if (call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mContains.get(), item, x,
                          y)) {
            hdUiQueueEvent(gabi::ea(this), EVENT_TOUCH_DOWN,
                           (u32) reinterpret_cast<cking::ui::input::Receiver*>((uintptr_t)item)->mId, 0u);
            setFocus(item);
            mReceiverId = (u32) reinterpret_cast<cking::ui::input::Receiver*>((uintptr_t)item)->mId;
            if (call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mCanDrag.get(), item)) {
                const f32 px = (f32)pos->value.x;
                mVelocity.value.x = z;
                mTouchCurrent.value.x = px;
                const f32 py = (f32)pos->value.y;
                mTouchPrevious.value.x = px;
                mTouchPrevious.value.y = py;
                mTouchStart.value.y = y;
                mVelocity.value.y = z;
                mInertiaFrames = INERTIA_IDLE;
                mTouchCurrent.value.y = py;
                mTouchStart.value.x = x;
            }
            return 1;
        }
        node = gabi::ea((cking::ui::input::ReceiverNode*)reinterpret_cast<cking::ui::input::ReceiverNode*>(
                            (uintptr_t)node)
                            ->mNext);
    } while (node != gabi::ea(this));
    return 0;
}

/* the nearest receiver that accepts the focused one (distance 02617174) */
/* 02615ED0 */
u32 cking::ui::input::EventMgr::findTarget() {
    u32 focus = gabi::ea((cking::ui::input::Receiver*)mpFocus);
    if (!focus)
        return 0;
    const f32 z = zero();
    f32 bestD = z;
    u32 best = 0;
    u32 node = gabi::ea((cking::ui::input::ReceiverNode*)mFirst);
    if (node == gabi::ea(this))
        return 0;
    for (;;) {
        const u32 item = gabi::ea(
            (cking::ui::input::Receiver*)reinterpret_cast<cking::ui::input::ReceiverNode*>((uintptr_t)node)
                ->mpReceiver);
        if (item != focus &&
            call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mCanDrop.get(), item) &&
            call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mAccepts.get(), item,
                          gabi::ea((cking::ui::input::Receiver*)mpFocus))) {
            const f32 d = hdUiReceiverDistance(gabi::ea((cking::ui::input::Receiver*)mpFocus), item);
            if (bestD == z || bestD > d) {
                bestD = d;
                best = item;
            }
        }
        node = gabi::ea((cking::ui::input::ReceiverNode*)reinterpret_cast<cking::ui::input::ReceiverNode*>(
                            (uintptr_t)node)
                            ->mNext);
        if (node == gabi::ea(this))
            return best;
        focus = gabi::ea((cking::ui::input::Receiver*)mpFocus);
    }
}

/* inlined into EventMgr input methods */
static void zeroTouch(be<f32>* const* fields, u32 n) {
    const f32 z = zero();
    for (u32 i = 0; i < n; i++)
        *fields[i] = z;
}

/* touch move: drag over targets (events 0x43..0x48, 3) */
/* 02616020 */
u32 cking::ui::input::EventMgr::touchMove(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    if (!ev) {
        mReceiverId = 0;
        setFocus(0);
        setGrab(0);
        setHover(0);
        be<f32>* const o[] = {&mTouchCurrent.value.x,  &mSampleCurrent.value.y, &mSampleCurrent.value.x,
                              &mTouchStart.value.x,    &mTouchCurrent.value.y,  &mSamplePrevious.value.x,
                              &mVelocity.value.y,      &mTouchStart.value.y,    &mVelocity.value.x,
                              &mSamplePrevious.value.y};
        zeroTouch(o, 10);
        return 0;
    }
    const u32 src =
        ((u32)event->mPointerFlags & POINTER_VALID) ? gabi::ea(&event->mTouch) : gabi::ea(&l_invalidTouch);
    Local<Vec2> pos;
    const u32 wx = (u32) reinterpret_cast<Vec2*>((uintptr_t)src)->words[0];
    const u32 px = (u32)mTouchCurrent.words[0];
    pos->words[0] = wx;
    const u32 wy = (u32) reinterpret_cast<Vec2*>((uintptr_t)src)->words[1];
    mTouchPrevious.words[0] = px;
    const u32 grab = gabi::ea((cking::ui::input::Receiver*)mpGrab);
    const u32 py = (u32)mTouchCurrent.words[1];
    pos->words[1] = wy;
    mTouchCurrent.words[1] = wy;
    mTouchCurrent.words[0] = wx;
    mTouchPrevious.words[1] = py;
    if (grab) {
        const u32 best = findTarget();
        const u32 hover = gabi::ea((cking::ui::input::Receiver*)mpHover);
        if (best) {
            if (hover == best) {
                hdUiQueueEvent(gabi::ea(this), EVENT_DRAG_OVER, (u32)mReceiverId, 0u);
                return 1;
            }
            if (!call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)best)->mpVtable->mCanDrop.get(), best))
                return 0;
            setHover(best);
            hdUiQueueEvent(gabi::ea(this), EVENT_DRAG_ENTER, (u32)mReceiverId, 0u);
            return 1;
        }
        if (hover) {
            setHover(0);
            hdUiQueueEvent(gabi::ea(this), EVENT_DRAG_LEAVE, (u32)mReceiverId, 0u);
            return 0;
        }
        hdUiQueueEvent(gabi::ea(this), EVENT_DRAG_MOVE, (u32)mpGrab->mId, 0u);
        return 0;
    }
    u32 item = gabi::ea((cking::ui::input::Receiver*)mpFocus);
    if (!item)
        return 0;
    if (call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mCanDrag.get(), item)) {
        const f32 dy = fsubs_ppc((f32)mTouchStart.value.y, (f32)pos->value.y);
        const f32 dy2 = fmuls_ppc(dy, dy);
        const f32 dx = fsubs_ppc((f32)mTouchStart.value.x, (f32)pos->value.x);
        const f32 d2 = fmadds(dx, dx, dy2);
        if (d2 > (f32)l_constants.mDragDistanceSquared) {
            const u32 id = (u32)mpFocus->mId;
            mReceiverId = id;
            hdUiQueueEvent(gabi::ea(this), EVENT_DRAG_START, id, 0u);
            setGrab(gabi::ea((cking::ui::input::Receiver*)mpFocus));
            return 1;
        }
    }
    item = gabi::ea((cking::ui::input::Receiver*)mpFocus);
    bool keep = call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mCanDrag.get(), item);
    if (!keep) {
        item = gabi::ea((cking::ui::input::Receiver*)mpFocus);
        keep = call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mContains.get(), item,
                             (f32)pos->value.x, (f32)pos->value.y);
    }
    if (!keep) {
        mReceiverId = 0;
        setGrab(0);
        setHover(0);
        return 0;
    }
    hdUiQueueEvent(gabi::ea(this), EVENT_TOUCH_MOVE, (u32)mReceiverId, 0u);
    setHover(0);
    return 1;
}

/* bounces the inertia velocity at the screen edges (640x360 less the margin) */
/* 0261634C */
void cking::ui::input::EventMgr::bounce() {
    const u32 grab = gabi::ea((cking::ui::input::Receiver*)mpGrab);
    if (!grab)
        return;
    const f32 m = (f32)mMargin;
    const u32 mtx = gabi::ea(&reinterpret_cast<Receiver*>((uintptr_t)grab)->mpLayout->mTransform);
    const f32 h = fsubs_ppc((f32)l_constants.mHalfHeight, m);
    const f32 w = fsubs_ppc((f32)l_constants.mHalfWidth, m);
    Local<WordMatrix> copy;
    for (u32 i = 0; i < 12; i++)
        copy->mWords[i] = reinterpret_cast<be<u32>*>((uintptr_t)mtx)[i];
    const f32 nw = -w;
    const f32 tx = (f32) reinterpret_cast<Mtx34*>(copy.get())->m[0][3];
    const f32 z = zero();
    const f32 ty = (f32) reinterpret_cast<Mtx34*>(copy.get())->m[1][3];
    if (tx < nw || w < tx) { /* blt / bge */
        const f32 v = -(f32)mVelocity.value.x;
        mVelocity.value.x = v;
        mTouchCurrent.value.x = v > z ? nw : w;
    }
    const f32 nh = -h;
    if (ty < nh || h < ty) {
        const f32 v = -(f32)mVelocity.value.y;
        mVelocity.value.y = v;
        mTouchCurrent.value.y = fsubs_ppc(v > z ? nh : h, (f32)l_constants.mBounceOffset);
    }
}

/* touch release: swipe events, inertia scrolling of the grabbed receiver, drop */
/* 02616444 */
u32 cking::ui::input::EventMgr::touchUp(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    const f32 z = zero();
    if (!ev) {
        mReceiverId = 0;
        setFocus(0);
        setGrab(0);
        setHover(0);
        mSampleCurrent.value.y = z;
        mInertiaFrames = INERTIA_IDLE;
        be<f32>* const o[] = {&mTouchStart.value.y,     &mVelocity.value.y,     &mTouchCurrent.value.x,
                              &mSamplePrevious.value.x, &mTouchCurrent.value.y, &mSamplePrevious.value.y,
                              &mTouchPrevious.value.x,  &mTouchStart.value.x,   &mTouchPrevious.value.y,
                              &mSampleCurrent.value.x,  &mVelocity.value.x};
        zeroTouch(o, 11);
        return 0;
    }
    Local<TouchUpScratch> F; /* +0 position words, +8 delta from the start */
    F->mPosition.words[0] = (u32)event->mTouch.words[0];
    const u32 wy = (u32)event->mTouch.words[1];
    const f32 sx = (f32)F->mPosition.value.x;
    const f32 ax = (f32)mSamplePrevious.value.x;
    F->mPosition.words[1] = wy;
    const f32 sy = (f32)F->mPosition.value.y;
    const f32 dx = fsubs_ppc(sx, ax);
    const f32 ay = (f32)mSamplePrevious.value.y;
    F->mCalculatedDelta.value.x = dx;
    const f32 dy = fsubs_ppc(sy, ay);
    F->mDelta.words[0] = (u32)F->mCalculatedDelta.words[0];
    F->mCalculatedDelta.value.y = dy;
    F->mDelta.words[1] = (u32)F->mCalculatedDelta.words[1];
    if (gabi::ea((cking::ui::input::Receiver*)mpGrab)) {
        if (gabi::ea((cking::ui::input::Receiver*)mpHover)) {
            const u32 best = findTarget();
            if (best) {
                setHover(best);
                hdUiQueueEvent(gabi::ea(this), EVENT_DROP, (u32)mpGrab->mId, 0u);
                mReceiverId = 0;
                setFocus(0);
                mTouchPrevious.value.x = z;
                mInertiaFrames = INERTIA_IDLE;
                be<f32>* const o[] = {&mTouchCurrent.value.y, &mVelocity.value.y,      &mTouchCurrent.value.x,
                                      &mTouchStart.value.y,   &mTouchPrevious.value.y, &mTouchStart.value.x,
                                      &mVelocity.value.x};
                zeroTouch(o, 7);
                return 1;
            }
        }
        const u32 item = gabi::ea((cking::ui::input::Receiver*)mpFocus);
        if (item &&
            call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mCanSwipe.get(), item)) {
            const f32 ddx = (f32)F->mDelta.value.x;
            if ((f32)l_constants.mSwipeXThreshold < fabsf(ddx)) { /* bge: branch when not less */
                hdUiQueueEvent(gabi::ea(this), EVENT_SWIPE_X, (u32)mReceiverId, 0u);
                const f32 ddy = (f32)F->mDelta.value.y;
                mSwipe.value.x = ddx;
                mSwipe.value.y = ddy;
            }
            const f32 ddy = (f32)F->mDelta.value.y;
            if ((f32)l_constants.mSwipeYThreshold < fabsf(ddy)) {
                hdUiQueueEvent(gabi::ea(this), EVENT_SWIPE_Y, (u32)mReceiverId, 0u);
                const f32 ddx2 = (f32)F->mDelta.value.x;
                mSwipe.value.y = ddy;
                mSwipe.value.x = ddx2;
            }
            mReceiverId = 0;
            setGrab(0);
            setHover(0);
            return 0;
        }
        const s32 frames = s32((u32)mInertiaFrames);
        if (frames == -1) {
            mInertiaFrames = (u32)mInertiaLength;
            const auto v = padTouchFromPrev();
            mVelocity.words[0] = v.r3;
            mVelocity.words[1] = v.r4;
            hdUiQueueEvent(gabi::ea(this), EVENT_DRAG_MOVE, (u32)mpGrab->mId, 0u);
            return 1;
        }
        bool go = false;
        if (frames > 0) {
            const f32 vy = (f32)mVelocity.value.y, vx = (f32)mVelocity.value.x;
            const f32 l2 = fmadds(vx, vx, fmuls_ppc(vy, vy));
            if (l2 > z) {
                const f64 e = frsqrte(l2);          /* the estimate stays double in the FPR */
                const f32 e2 = f32(e * round25(e)); /* fmuls rounds frC to 25 bits */
                const f32 eh = f32(e * f64((f32)l_constants.mHalf));
                const f32 t = fnmsubs(e2, l2, (f32)l_constants.mThree);
                const f32 len = fmuls_ppc(fmuls_ppc(t, eh), l2);
                go = !(len < (f32)mStopSpeed);
            } else {
                go = !(fmuls_ppc(z, l2) < (f32)mStopSpeed);
            }
        }
        if (!go) {
            mInertiaFrames = INERTIA_IDLE;
            mReceiverId = 0;
            setGrab(0);
            setHover(0);
            return 0;
        }
        mInertiaFrames = (u32)mInertiaFrames - 1;
        bounce();
        const f32 fr = (f32)mFriction;
        const f32 vx = fmuls_ppc((f32)mVelocity.value.x, fr);
        const f32 px = (f32)mTouchCurrent.value.x;
        const f32 vy = fmuls_ppc((f32)mVelocity.value.y, fr);
        mVelocity.value.x = vx;
        const f32 nx = fadds_ppc(px, vx);
        const f32 py = (f32)mTouchCurrent.value.y;
        mVelocity.value.y = vy;
        const f32 ny = fadds_ppc(py, vy);
        mTouchCurrent.value.x = nx;
        mTouchCurrent.value.y = ny;
        const u32 best = findTarget();
        if (best) {
            mTouchCurrent.value.x = z;
            mVelocity.value.y = z;
            mInertiaFrames = INERTIA_IDLE;
            mVelocity.value.x = z;
            mTouchCurrent.value.y = z;
            mTouchPrevious.value.y = z;
            mTouchPrevious.value.x = z;
            setHover(best);
            hdUiQueueEvent(gabi::ea(this), EVENT_DROP, (u32)mpGrab->mId, 0u);
            return 1;
        }
        hdUiQueueEvent(gabi::ea(this), EVENT_DRAG_MOVE, (u32)mpGrab->mId, 0u);
        return 0;
    }
    const u32 item = gabi::ea((cking::ui::input::Receiver*)mpFocus);
    if (item && call_ptr<u32>(reinterpret_cast<Receiver*>((uintptr_t)item)->mpVtable->mContains.get(), item,
                              (f32)F->mPosition.value.x, (f32)F->mPosition.value.y)) {
        hdUiQueueEvent(gabi::ea(this), EVENT_TOUCH_UP, (u32)mReceiverId, 0u);
        mReceiverId = 0;
        setFocus(0);
        setGrab(0);
        setHover(0);
        be<f32>* const o[] = {&mTouchStart.value.y, &mVelocity.value.x,      &mTouchPrevious.value.y,
                              &mTouchStart.value.x, &mTouchPrevious.value.x, &mVelocity.value.y};
        zeroTouch(o, 6);
        return 1;
    }
    mReceiverId = 0;
    setGrab(0);
    setHover(0);
    return 0;
}

/* button event senders (event 0xD with a button code, or 0x62/0x63) */
/* inlined into EventMgr input methods */
static u32 sendButton(u32 self, u32 code) {
    Local<ButtonParameter> b;
    b->mCode = u8(code);
    hdUiQueueEvent(self, EVENT_BUTTON, INVALID_RECEIVER_ID, b.a);
    return 0;
}
/* 02616950 */
u32 cking::ui::input::EventMgr::sendButton0() { return sendButton(gabi::ea(this), 0); }
/* 02616990 */
u32 cking::ui::input::EventMgr::sendButton1() { return sendButton(gabi::ea(this), 1); }
/* 026169C8 */
u32 cking::ui::input::EventMgr::sendButton3() { return sendButton(gabi::ea(this), 3); }
/* 02616A00 */
u32 cking::ui::input::EventMgr::sendButton4() { return sendButton(gabi::ea(this), 4); }
/* 02616A38 */
u32 cking::ui::input::EventMgr::sendButtonD() { return sendButton(gabi::ea(this), 0xD); }
/* 02616A6C */
u32 cking::ui::input::EventMgr::sendButtonE() { return sendButton(gabi::ea(this), 0xE); }
/* 02616AA4 */
u32 cking::ui::input::EventMgr::sendButton2() { return sendButton(gabi::ea(this), 2); }
/* 02616ADC */
u32 cking::ui::input::EventMgr::sendButton5() { return sendButton(gabi::ea(this), 5); }
/* 02616B14 */
u32 cking::ui::input::EventMgr::sendEvent62() {
    hdUiQueueEvent(gabi::ea(this), EVENT_BUTTON11, INVALID_RECEIVER_ID, 0u);
    return 0;
}
/* 02616B44 */
u32 cking::ui::input::EventMgr::sendEvent63() {
    hdUiQueueEvent(gabi::ea(this), EVENT_BUTTON12, INVALID_RECEIVER_ID, 0u);
    return 0;
}

/* stick-direction button: the stick bit when the stick is tilted, else the D-pad bit */
/* inlined into EventMgr input methods */
static u8 stickOrPad(u32 ev, u32 stickBit, u32 padBit) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    const f32 z = zero();
    const bool tilted = !((f32)event->mStick.value.x == z) || !((f32)event->mStick.value.y == z);
    const u32 b = tilted ? stickBit : padBit;
    return u8((((u32)event->mHold >> b) | ((u32)event->mTrigger >> b)) & 1);
}
/* 02616B74 */
u8 cking::ui::input::EventMgr::isLeft(u32 ev) { return stickOrPad(ev, 20, 16); }
/* 02616BCC */
u8 cking::ui::input::EventMgr::isRight(u32 ev) { return stickOrPad(ev, 21, 17); }
/* 02616C24 */
u8 cking::ui::input::EventMgr::isUp(u32 ev) { return stickOrPad(ev, 22, 18); }
/* 02616C7C */
u8 cking::ui::input::EventMgr::isDown(u32 ev) { return stickOrPad(ev, 23, 19); }

/* hold bits of the event */
/* 02616CD4 */
u32 cking::ui::input::EventMgr::holdBit0(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 0) & 1;
}
/* 02616CE0 */
u32 cking::ui::input::EventMgr::holdBit1(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 1) & 1;
}
/* 02616CEC */
u32 cking::ui::input::EventMgr::holdBit3(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 3) & 1;
}
/* 02616CF8 */
u32 cking::ui::input::EventMgr::holdBit4(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 4) & 1;
}
/* 02616D04 */
u32 cking::ui::input::EventMgr::holdBit13(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 13) & 1;
}
/* 02616D10 */
u32 cking::ui::input::EventMgr::holdBit14(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 14) & 1;
}
/* 02616D1C */
u32 cking::ui::input::EventMgr::holdBit2(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 2) & 1;
}
/* 02616D28 */
u32 cking::ui::input::EventMgr::holdBit5(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 5) & 1;
}
/* 02616D34 */
u32 cking::ui::input::EventMgr::holdBit11(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 11) & 1;
}
/* 02616D40 */
u32 cking::ui::input::EventMgr::holdBit12(u32 ev) {
    auto* event = reinterpret_cast<Event*>((uintptr_t)ev);
    return ((u32)event->mHold >> 12) & 1;
}

/* SingletonDisposer deleting destructor (deletes the instance) */
/* 02616D4C */
void hdUiEventMgr_disposerDtor(u32 p, u32 flags) {
    if (!p)
        return;
    reinterpret_cast<Disposer*>((uintptr_t)p)->mVtable = DisposerVtable;
    if (p == l_disposer) {
        const u32 inst = l_instance;
        l_disposer = 0;
        call_ptr<void>(reinterpret_cast<EventMgr*>((uintptr_t)inst)->mpVtable->mSlots[3].get(), inst, 2u);
        l_instance = 0;
    }
    hdUiDisposerDestroy(p, 0u);
    if (flags & 1)
        hdUiFree(p);
}

/* static initialiser (shared objects, the default touch limits) */
/* 02616DF4 */
void __sinit_hd_input_eventmgr_cpp() {
    l_staticData.mZeros[3] = 0;
    l_staticData.mZeros[2] = 0;
    l_staticData.mZeros[1] = 0;
    l_staticData.mZeros[0] = 0;
    hdUiRegisterObject(gabi::ea(l_destructorRecords.mRecords[0]));
    const f32 lo = l_constants.mMinAngle, hi = l_constants.mMaxAngle;
    l_staticData.mMinAngle = lo;
    l_staticData.mMaxAngle = hi;
    hdUiConstructFirstFlag(gabi::ea(&l_staticData.mFirstFlag));
    hdUiRegisterObject(gabi::ea(l_destructorRecords.mRecords[1]));
    hdUiConstructSecondFlag(gabi::ea(&l_staticData.mSecondFlag));
    hdUiRegisterObject(gabi::ea(l_destructorRecords.mRecords[2]));
    const f32 a = l_constants.mTouchDefault, c = l_constants.mDeltaDefault;
    l_staticData.mDefaultTouch.value.x = a;
    l_staticData.mDefaultDelta.value.x = c;
    l_staticData.mDefaultTouch.value.y = a;
    l_staticData.mDefaultDelta.value.y = c;
}

/* empty function */
/* 02616ED0 */
void hdUiEventMgr_emptyVirtual() {}

/* WWHD glue
ext 02000020 void hdUiEventMgrBaseConstruct(EventMgr*)
ext 020000B4 void hdUiEventMgrBaseDestroy(u32, u32)
ext 02000108 void hdUiEventQueuePrepare(u32, u32, u32)
ext 02000110 void hdUiEventQueueForward(u32)
ext 02000118 void hdUiEventQueueBeginFrame(u32)
ext 020002E0 void hdUiEventQueueProcess(u32)
ext 02000324 void hdUiQueueEvent(u32, u32, u32, u32)
ext 0200209C void hdUiEventQueueClear(u32)
ext 0203E8E8 u32 hdUiGetParentHeap()
ext 02617174 f32 hdUiReceiverDistance(u32, u32)
ext 0273AD10 u32 hdUiAllocate(u32)
ext 0273AF40 void hdUiFree(u32)
ext 0273B0D4 u32 hdUiAllocateAligned(u32, u32, u32)
ext 0273B2F0 void hdUiListAppend(u32, u32)
ext 0273B310 void hdUiListErase(u32)
ext 02752B0C void hdUiDisposerConstruct(u32, u32, u32)
ext 02752BEC void hdUiDisposerDestroy(u32, u32)
ext 02753004 u32 hdUiCreateHeap(u32, u32, u32, u32, u32)
ext 0275320C u32 hdUiHeapHeaderSize(u32)
ext 028EAB2C void hdUiConstructSecondFlag(u32)
ext 028ED6F8 void hdUiConstructFirstFlag(u32)
ext 028F026C void hdUiRegisterObject(u32)
str 100E2658 in 02614EA8
data l_constants 100E2648
data l_instance 101F4FF0
data l_disposer 101F4FF4
data l_inputManager 101F5088
data l_options 101F8378
data l_invalidTouch 1049FFA0
data l_invalidDelta 104A0908
data l_staticData 1048DDE8
data l_destructorRecords 101F4FCC
*/
