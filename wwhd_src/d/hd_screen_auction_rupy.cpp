/* HD auction rupee meter/counter. */
#include "d/hd_screen_auction_rupy.h"
#include "d/hd_parts.h"
using namespace gabi;
static RupyState l_rupyIdle;
static u32 l_rupyDelegateVTableA;
static u32 l_rupyDelegateVTableB;
static u32 l_rupyScreenVTable;
static u32 l_rupyStringVTable;
static u32 l_rupyDigitFactory;
static u32 l_rupyStateVTable;
static u32 l_rupyStateOwner;
static RupyUIPointer l_rupyUI;
static RupyHeapPointer l_rupyHeap;
static f32 l_rupyOne;
static f32 l_rupyMeterMaximum;
static f32 l_rupyIdleFrame;
static f32 l_rupyZero;
static RupyWords l_rupyWords;
static RupyRange l_rupyRange;
static f32 l_rupyLow;
static f32 l_rupyHigh;
static f32 l_rupyBig;
static f32 l_rupySmall;
static RupyScales l_rupyScales;
static RupyRegistration l_rupyRegistrationA;
static RupyRegistration l_rupyRegistrationB;
static RupyRegistration l_rupyRegistrationC;
static u8 l_rupyObjectA;
static u8 l_rupyObjectB;
static RupySharedStrings l_rupyGroupNames;
static RupyAnimationNames l_rupyAnimationNames;
static RupySharedStrings l_rupyDigitNames;
static RupySharedStrings l_rupyUnusedNames;
static RupyLayoutNames l_rupyLayoutNames;
static RupyString l_rupyPartName;
static RupyString l_rupyDigitType;
static RupyState l_rupyStateWait;
static RupyState l_rupyStateExit;
static u32 l_rupyStateGuard;
static u32 l_rupyNextStateId;
static RupyAnimationMap l_rupyAnimationGroups;
extern void rupyBaseDestroy(RupyScreen *, u32);
extern void rupyDelete(void *);
extern void *rupyAllocate(u32);
extern void rupyBaseConstruct(RupyScreen *);
extern RupyPlay *rupyGetPlay();
extern void rupyAnimationStop(RupyAnimation *, u32, u32);
extern void rupyAnimationPlay(RupyAnimation *, u32, u32, f32);
extern void rupyAnimationFrame(RupyAnimation *, u32, u32, f32);
extern void rupyAnimationReset(RupyAnimation *, u32, u32);
extern void rupyAnimationLimit(RupyAnimation *, u32, u32, f32);
extern void rupyAuctionOpen(void *);
extern void rupyAuctionClose(void *);
extern void rupyInputOpen();
extern void rupyInputClose();
extern void rupyDigitSet(void *, u32);
extern void rupyDigitRoll(void *, u32, u32, u32);
extern void rupyPlaySound(u32);
extern void rupyChangeState(gptr<RupyStateMachine> *, RupyState *);
extern void rupySetLayoutNames(RupyScreen *, RupyLayoutNames *, u32);
extern u32 rupyFrameCount(void *);
extern void rupyBaseRemove(RupyScreen *);
extern RupyState *rupyGetCurrentState(gptr<RupyStateMachine> *);
extern void rupyExecuteState(gptr<RupyStateMachine> *);
extern u32 rupyBaseCreateChild(RupyScreen *, u32, RupyPart *);
extern void *rupyAllocateAligned(u32, void *, u32);
extern RupyLayout *rupyLayoutConstruct(RupyLayout *);
extern void rupyBindAnimation(RupyAnimation *, u32, RupyString *, RupyString *);
extern void rupyRegister(RupyRegistration *);
extern void rupyConstructFirst(be<u8> *);
extern void rupyConstructSecond(be<u8> *);
static RupyAnimation *animation(RupyScreen *screen) {
    return screen->mpLayout.get()->mpAnimation.get();
}
static void invoke(RupyScreen *screen, u32 offset) {
    gabi::call_ptr<void>(screen->mpVTable[offset / 4].get(), screen);
}
static void *child(RupyScreen *screen, u32 index) {
    u32 slot = gabi::ea(screen->mpChildren.get());
    if (index < screen->mChildCount)
        slot += 4 * index;
    return gabi::at<gptr<void>>(slot)->get();
}
/* 0261BC98: deleting destructor */
/* 0261BC98 */
void rupy_dtor(RupyScreen *p, u32 flags) {
    if (!p)
        return;
    p->mpDelegateVTableA = &l_rupyDelegateVTableA;
    p->mpDelegateVTableB = &l_rupyDelegateVTableB;
    rupyBaseDestroy(p, 0u);
    if (flags & 1)
        rupyDelete(p);
}

/* 0261BD04: resets the meter to the current value */
/* 0261BD04 */
void rupy_resetMeter(RupyScreen *self) {
    rupyAnimationStop(animation(self), RUPY_METER_APPEAR, 2u);
    rupyAnimationStop(animation(self), RUPY_METER_FILL, 1u);
    RupyPlay *g = rupyGetPlay();
    const s16 v = s16(g->mMeter);
    self->mMeterEnabled = 0;
    self->mMeterShown = 0;
    self->mShownRupees = f32(f64(s32(v)));
}

/* 0261BD90: resets the counter */
/* 0261BD90 */
void rupy_resetCounter(RupyScreen *self) {
    rupyAnimationStop(animation(self), RUPY_COUNTER_APPEAR, 0u);
    self->mCounterEnabled = 0;
    self->mCounterIdle = 1;
    self->mCounting = 0;
    self->mCounterDelay = 0;
    self->mCounterShown = 0;
    self->mCounterStarted = 0;
    self->mCounterValue = 0;
}

/* 0261BDF0: opens the screen */
/* 0261BDF0 */
void rupy_open(RupyScreen *self) {
    rupyAuctionOpen(l_rupyUI.get()->mpAuction.get());
    rupy_resetMeter(self);
    rupy_resetCounter(self);
    invoke(self, 0x8C);
    rupyInputOpen();
    self->mOpen = 1;
}

/* 0261BE58: meter: shows/hides it and follows the value (at most 100) */
/* 0261BE58 */
void rupy_meterMove(RupyScreen *self) {
    const f32 one = l_rupyOne;
    const u32 on = self->mMeterEnabled, shown = self->mMeterShown;
    if (!on) {
        if (!shown)
            return;
        self->mMeterShown = 0;
        rupyAnimationPlay(animation(self), RUPY_METER_HIDE, 2u, one);
        return;
    }
    if (!shown) {
        self->mMeterShown = 1;
        rupyAnimationPlay(animation(self), RUPY_METER_APPEAR, 2u, one);
    }
    RupyPlay *g = rupyGetPlay();
    f32 v = f32(f64(s32(s16(g->mMeter))));
    const f32 mx = l_rupyMeterMaximum;
    const f32 cur = self->mShownRupees;
    if (!(v <= mx))
        v = mx;
    if (cur == v)
        return;
    const f32 r = v / mx;
    self->mShownRupees = v;
    rupyAnimationFrame(animation(self), RUPY_METER_FILL, 1u, fmuls_ppc(r, self->mMeterFrames));
}

/* 0261BF64: sets the three digits from the counter value */
/* 0261BF64 */
void rupy_setDigits(RupyScreen *self) {
    s16 div = 10, mul = 1;
    for (u32 i = 0; i < 3; i++) {
        const s32 v = s16(self->mCounterValue);
        const s16 d = s16((v - (v / div) * div) / mul);
        div = s16(div * 10);
        mul = s16(mul * 10);
        rupyDigitSet(child(self, i), u32(s32(d)));
        self->mDigits[i] = u16(d);
    }
}

/* 0261BFFC: counts one step towards the bid (1, 10 or 100 by distance) and rolls the digits */
/* 0261BFFC */
void rupy_countStep(RupyScreen *self, u32 up) {
    RupyPlay *g = rupyGetPlay();
    const s32 cur = s16(self->mCounterValue);
    const s32 tgt = s16(g->mBid);
    const s32 dist = up ? s16(tgt - cur) : s16(cur - tgt);
    s32 step;
    if (dist < 200)
        step = dist < 50 ? 1 : 10;
    else
        step = 100;
    s32 v = up ? s16(cur + step) : s16(cur - step);
    self->mCounterValue = u16(v);
    s16 div = 10, mul = 1;
    for (u32 i = 0; i < 3; i++) {
        if (i)
            v = s16(self->mCounterValue);
        const s16 d = s16((v - (v / div) * div) / mul);
        div = s16(div * 10);
        mul = s16(mul * 10);
        void *c = child(self, i);
        const s32 old = s16(self->mDigits[i]);
        rupyDigitRoll(c, u32(old), u32(s32(d)), up);
        self->mDigits[i] = u16(d);
    }
}

/* 0261C134: counter: shows it, counts towards the bid with a delay and plays the sounds */
/* 0261C134 */
void rupy_counterMove(RupyScreen *self) {
    const f32 one = l_rupyOne;
    const u32 on = self->mCounterEnabled, shown = self->mCounterShown;
    if (!on) {
        if (!shown)
            return;
        self->mCounterShown = 0;
        rupyAnimationPlay(animation(self), RUPY_COUNTER_HIDE, 0u, one);
        return;
    }
    if (!shown) {
        self->mCounterShown = 1;
        rupyAnimationPlay(animation(self), RUPY_COUNTER_APPEAR, 0u, one);
        if (!self->mCounterStarted) {
            self->mCounterIdle = 1;
            self->mCounterStarted = 1;
            self->mCounting = 1;
            rupyAnimationStop(animation(self), RUPY_COUNTER_COUNT, 0u);
        }
    }
    RupyPlay *g = rupyGetPlay();
    const s32 cur = s16(self->mCounterValue);
    if (cur < s32(s16(g->mBid))) {
        if (cur == 0) {
            g = rupyGetPlay();
            self->mCounterValue = g->mBid;
            rupy_setDigits(self);
        } else if (const u32 w = self->mCounterDelay) {
            self->mCounterDelay = w - 1;
        } else {
            rupy_countStep(self, 1u);
            g = rupyGetPlay();
            if (s16(self->mCounterValue) == s16(g->mBid))
                rupyPlaySound(RUPY_SOUND_FINISHED);
            else {
                rupyPlaySound(RUPY_SOUND_COUNT);
                self->mCounterDelay = 3;
            }
        }
    } else {
        g = rupyGetPlay();
        if (s16(self->mCounterValue) > s16(g->mBid)) {
            if (const u32 w = self->mCounterDelay)
                self->mCounterDelay = w - 1;
            else {
                rupy_countStep(self, 0u);
                g = rupyGetPlay();
                if (s16(self->mCounterValue) != s16(g->mBid))
                    self->mCounterDelay = 3;
            }
        }
    }
    const u32 counting = self->mCounting, idle = self->mCounterIdle;
    if (counting) {
        if (idle)
            return;
        self->mCounterIdle = 1;
        rupyAnimationReset(animation(self), RUPY_COUNTER_COUNT, 0u);
        rupyAnimationLimit(animation(self), RUPY_COUNTER_COUNT, 0u, l_rupyIdleFrame);
    } else {
        if (!idle)
            return;
        self->mCounterIdle = 0;
        rupyAnimationPlay(animation(self), RUPY_COUNTER_COUNT, 0u, one);
    }
}

/* 0261C3DC: state Move: meter and counter, then the screen update */
/* 0261C3DC */
void rupy_move(RupyScreen *self) {
    rupy_meterMove(self);
    rupy_counterMove(self);
    invoke(self, 0xB4);
    invoke(self, 0x8C);
}

/* 0261C438: closes the screen */
/* 0261C438 */
void rupy_close(RupyScreen *self) {
    rupyAuctionClose(l_rupyUI.get()->mpAuction.get());
    rupyInputClose();
    self->mOpen = 0;
}

/* 0261C47C */
void rupy_toMove(RupyScreen *self) { rupyChangeState(&self->mpStateMachine, &l_rupyIdle); }

/* 0261C48C: constructor */
/* 0261C48C */
u32 rupy_ctor(RupyScreen *p) {
    if (!p) {
        p = static_cast<RupyScreen *>(rupyAllocate(sizeof(RupyScreen)));
        if (!p)
            return 0;
    }
    rupyBaseConstruct(p);
    const f32 z = l_rupyZero;
    p->mCounterStarted = 0;
    p->mMeterEnabled = 0;
    p->mCounting = 0;
    p->mMeterShown = 0;
    p->mMeterFrames = z;
    p->mShownRupees = z;
    p->mpVTable = reinterpret_cast<gfn *>(&l_rupyScreenVTable);
    p->mCounterValue = 0;
    p->mCounterShown = 0;
    p->mOpen = 0;
    p->mpDelegateVTableB = &l_rupyDelegateVTableB;
    p->mpDelegateVTableA = &l_rupyDelegateVTableA;
    p->mCounterEnabled = 0;
    p->mCounterDelay = 0;
    p->mCounterIdle = 0;
    for (u32 i = 0; i < 3; i++)
        p->mDigits[i] = 0;
    return gabi::ea(p);
}

/* 0261C54C: screen setup (layout, child screens, meter scale, first state) */
/* 0261C54C */
u32 rupy_setup(RupyScreen *self, u32 a, u32 b, u32 c) {
    rupySetLayoutNames(self, &l_rupyLayoutNames, 2u);
    if (!call_ptr<u32>(self->mpVTable[31].get(), self, &l_rupyPartName, self->mpArchive.get(), a, b,
                       c))
        return 0;
    if (!call_ptr<u32>(self->mpVTable[41].get(), self))
        return 0;
    rupyAnimationStop(animation(self), RUPY_COUNTER_APPEAR, 0u);
    rupyAnimationStop(animation(self), RUPY_METER_APPEAR, 2u);
    RupyAnimation *an = animation(self);
    u32 frameSlot = gabi::ea(an->mpFrames.get());
    if (an->mCount > 4)
        frameSlot += 0x10;
    RupyFrame *frame = gabi::at<RupyFrame>(gabi::ea(gabi::at<gptr<void>>(frameSlot)->get()));
    const u32 frameCount = rupyFrameCount(frame->mpResource.get());
    self->mMeterFrames = f32(f64(frameCount));
    RupyStateMachine *machine = self->mpStateMachine.get();
    RupyStateAction *action =
        gabi::call_ptr<RupyStateAction *>(machine->mpVTable[5].get(), machine, &l_rupyIdle);
    self->mpCurrentAction = action;
    gabi::call_ptr<void>(action->mpVTable[7].get(), action);
    invoke(self, 0xB4);
    invoke(self, 0x8C);
    return 1;
}

/* 0261C6EC: screen removal */
/* 0261C6EC */
void rupy_remove(RupyScreen *self) {
    invoke(self, 0xAC);
    invoke(self, 0x84);
    rupyBaseRemove(self);
}

/* 0261C740: runs the state machine unless the Move state is current */
/* 0261C740 */
void rupy_execute(RupyScreen *self) {
    RupyState *state = rupyGetCurrentState(&self->mpStateMachine);
    const u32 getId = state->mpVTable[5].get();
    gfn *idleVTable = l_rupyIdle.mpVTable.get();
    const u32 current = gabi::call_ptr<u32>(getId, state);
    if (current == gabi::call_ptr<u32>(idleVTable[5].get(), &l_rupyIdle))
        return;
    rupyExecuteState(&self->mpStateMachine);
}

/* 0261C7C0: draw when open */
/* 0261C7C0 */
void rupy_draw(RupyScreen *self, u32 a) {
    if (!self->mOpen)
        return;
    call_ptr<void>(self->mpVTable[39].get(), self, a);
    call_ptr<void>(self->mpVTable[49].get(), self, a);
}

// HD: preserve the earlier pointer comparison and repeated source field reads.
static bool equalStrings(be<u8> *earlierA, be<u8> *earlierB, RupyString *left, RupyString *right) {
    if (earlierA == earlierB)
        return true;
    const u32 a = gabi::ea(left->mpBuffer.get()), b = gabi::ea(right->mpBuffer.get());
    for (u32 index = 0; index < 0x40001; ++index) {
        const u8 x = *gabi::at<be<u8>>(a + index), y = *gabi::at<be<u8>>(b + index);
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}
static void terminate(RupyString *string) {
    gabi::call_ptr<void>(string->mpVTable[5].get(), string);
}
static be<u8> *stringBuffer(RupyString *string) {
    terminate(string);
    return string->mpBuffer.get();
}
/* 0261C830 */
u32 rupy_createChild(RupyScreen *self, RupyPart *info) {
    RupyString *type = gabi::at<RupyString>(gabi::ea(info) + 8);
    RupyString *name = gabi::at<RupyString>(gabi::ea(info) + 0x14);
    be<u8> *requiredType = stringBuffer(&l_rupyDigitType);
    for (u32 index = 0; index < 3; ++index) {
        terminate(name);
        terminate(name);
        be<u8> *actualName = name->mpBuffer.get();
        RupyString *reference = &l_rupyDigitNames.mNames[index];
        be<u8> *expectedName = stringBuffer(reference);
        if (!equalStrings(actualName, expectedName, name, reference))
            continue;
        // HD: the temporary safe-string is an observed two-word guest stack allocation.
        gabi::Local<RupyString> storage;
        RupyString *temporary = gabi::at<RupyString>(storage.a);
        temporary->mpBuffer = requiredType;
        temporary->mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
        terminate(type);
        terminate(type);
        be<u8> *actualType = type->mpBuffer.get();
        terminate(temporary);
        if (!equalStrings(actualType, temporary->mpBuffer.get(), type, temporary))
            return rupyBaseCreateChild(self, index, info);
        u32 slot = gabi::ea(self->mpChildren.get());
        if (index < self->mChildCount)
            slot += 4 * index;
        *gabi::at<gptr<void>>(slot) =
            gabi::call_ptr<void *>(gabi::ea(&l_rupyDigitFactory), 0u, 0u, info);
        return child(self, index) ? 1 : 0;
    }
    return 1;
}

/* 0261CA68: loads the layout and binds its six animations */
/* 0261CA68 */
u32 rupy_loadScreen(RupyScreen *self, u32 a, u32 b, u32 c, u32 d, u32 e) {
    RupyLayout *layout =
        static_cast<RupyLayout *>(rupyAllocateAligned(0x110, l_rupyHeap.get()->mpHeap.get(), 4));
    if (layout)
        layout = rupyLayoutConstruct(layout);
    self->mpLayout = layout;
    if (!layout)
        return 0;
    if (!gabi::call_ptr<u32>(layout->mpVTable[5].get(), layout, a, b, c, d, 6u, 3u, e))
        return 0;
    for (u32 index = 0; index < 6; ++index)
        rupyBindAnimation(animation(self), index, &l_rupyAnimationNames.mNames[index],
                          gabi::at<RupyString>(gabi::ea(&l_rupyGroupNames) +
                                               8 * u32(l_rupyAnimationGroups.mGroups[index])));
    invoke(self, 0x8C);
    return 1;
}

/* 0261CB94: static initialiser: shared objects, layout/animation/pane names and two states */
/* 0261CB94 */
void rupy_staticInit() {
    l_rupyWords.mValues[3] = 0;
    l_rupyWords.mValues[2] = 0;
    l_rupyWords.mValues[1] = 0;
    l_rupyWords.mValues[0] = 0;
    rupyRegister(&l_rupyRegistrationA);
    const f32 lo = l_rupyLow, hi = l_rupyHigh;
    l_rupyRange.mLow = lo;
    l_rupyRange.mHigh = hi;
    rupyConstructFirst(&l_rupyObjectA);
    rupyRegister(&l_rupyRegistrationB);
    rupyConstructSecond(&l_rupyObjectB);
    rupyRegister(&l_rupyRegistrationC);
    const f32 big = l_rupyBig, small = l_rupySmall;
    l_rupyScales.mValues[0] = big;
    l_rupyScales.mValues[2] = small;
    l_rupyGroupNames.mNames[0].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyGroupNames.mNames[0].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2FE0"));
    l_rupyGroupNames.mNames[1].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyGroupNames.mNames[1].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2FF4"));
    l_rupyAnimationNames.mNames[0].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyAnimationNames.mNames[0].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2FC0"));
    l_rupyAnimationNames.mNames[1].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyAnimationNames.mNames[1].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2F88"));
    l_rupyAnimationNames.mNames[2].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyScales.mValues[1] = big;
    l_rupyAnimationNames.mNames[2].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E3000"));
    l_rupyAnimationNames.mNames[3].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyAnimationNames.mNames[3].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2F68"));
    l_rupyAnimationNames.mNames[4].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyAnimationNames.mNames[4].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2F70"));
    l_rupyAnimationNames.mNames[5].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyAnimationNames.mNames[5].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E3010"));
    l_rupyUnusedNames.mNames[2].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyDigitNames.mNames[0].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyDigitNames.mNames[0].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2F9C"));
    l_rupyDigitNames.mNames[1].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyDigitNames.mNames[1].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2FA8"));
    l_rupyDigitNames.mNames[2].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyDigitNames.mNames[2].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2FB4"));
    l_rupyScales.mValues[3] = small;
    l_rupyGroupNames.mNames[2].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyUnusedNames.mNames[1].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyGroupNames.mNames[2].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2F78"));
    l_rupyLayoutNames.mNames[0].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyPartName.mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    const u32 guard = l_rupyStateGuard;
    l_rupyUnusedNames.mNames[0].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyUnusedNames.mNames[0].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E301C"));
    l_rupyUnusedNames.mNames[2].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E301C"));
    l_rupyUnusedNames.mNames[1].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E301C"));
    l_rupyLayoutNames.mNames[0].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E3030"));
    l_rupyLayoutNames.mNames[1].mpVTable = reinterpret_cast<gfn *>(&l_rupyStringVTable);
    l_rupyPartName.mpBuffer = gabi::at<be<u8>>(gabi::guest_addr("auction text 100E3030"));
    l_rupyLayoutNames.mNames[1].mpBuffer =
        gabi::at<be<u8>>(gabi::guest_addr("auction text 100E301C"));
    u32 id;
    if (guard)
        id = l_rupyNextStateId;
    else {
        id = 0;
        l_rupyStateGuard = 1;
    }
    l_rupyStateWait.mThisFinal = 0;
    l_rupyStateWait.mFinalKind = 0;
    l_rupyStateWait.mExitIndex = 0x1B;
    l_rupyStateWait.mInitIndex = 0x19;
    l_rupyStateWait.mThisInit = 0;
    l_rupyStateWait.mpVTable = reinterpret_cast<gfn *>(&l_rupyStateVTable);
    l_rupyStateWait.mId = id + 1;
    l_rupyStateWait.mInitKind = 4;
    l_rupyStateWait.mThisExit = 0;
    l_rupyStateWait.mpName = gabi::at<be<u8>>(gabi::guest_addr("auction text 100E303C"));
    l_rupyStateExit.mThisInit = 0;
    l_rupyStateWait.mpOwner = &l_rupyStateOwner;
    l_rupyStateExit.mInitIndex = 0x1C;
    l_rupyStateExit.mpName = gabi::at<be<u8>>(gabi::guest_addr("auction text 100E2FD0"));
    l_rupyStateWait.mMoveIndex = 0x1A;
    l_rupyStateWait.mFinalIndex = 0;
    l_rupyStateExit.mpOwner = &l_rupyStateOwner;
    l_rupyStateExit.mFinalKind = 0;
    l_rupyStateExit.mpVTable = reinterpret_cast<gfn *>(&l_rupyStateVTable);
    l_rupyStateExit.mExitKind = 4;
    l_rupyStateExit.mMoveKind = 4;
    l_rupyStateExit.mInitKind = 4;
    l_rupyStateWait.mExitKind = 4;
    l_rupyStateWait.mThisMove = 0;
    l_rupyStateWait.mMoveKind = 4;
    l_rupyStateExit.mId = id + 2;
    l_rupyStateExit.mMoveIndex = 0x1D;
    l_rupyStateExit.mThisMove = 0;
    l_rupyStateExit.mFinalIndex = 0;
    l_rupyStateExit.mExitIndex = 0x1E;
    l_rupyStateExit.mThisFinal = 0;
    l_rupyNextStateId = id + 2;
    l_rupyStateExit.mThisExit = 0;
}

/* empty virtual functions / state enter-exit functions */
/* 0261CF60 */
void rupy_nop1() {}

/* 0261CF64 */
void rupy_nop2() {}

/* 0261CF80 */
void rupy_nop3() {}

/* 0261CF84 */
void rupy_nop4() {}

/* 0261CF90 */
void rupy_nop5() {}

/* 0261CF94 */
void rupy_nop6() {}

/* 0261CF98 */
void rupy_nop7() {}
/* 0261BB08 */
// GHS places this state number template copy after the TU's code
static s32 hdScreenAuctionRupy_getStateNumber(hdPartsStateId_c* stateId) {
    return stateId->mNumber;
}

/* WWHD glue (tools/verify/autoglue.py)
data l_rupyIdle 1049DD60
data l_rupyDelegateVTableA 100E3148
data l_rupyDelegateVTableB 100E3158
data l_rupyScreenVTable 100E3050
data l_rupyStringVTable 100E2E6C
data l_rupyDigitFactory 0261D088
data l_rupyStateVTable 100E2EE4
data l_rupyStateOwner 101FF32C
data l_rupyUI 101F8344
data l_rupyHeap 1018C404
data l_rupyOne 100E2F30
data l_rupyMeterMaximum 100E2F40
data l_rupyIdleFrame 100E2F44
data l_rupyZero 100E2F48
data l_rupyWords 1048E014
data l_rupyRange 1048DFF8
data l_rupyLow 100E2F58
data l_rupyHigh 100E2F5C
data l_rupyBig 100E2F60
data l_rupySmall 100E2F64
data l_rupyScales 1048E000
data l_rupyRegistrationA 101F514C
data l_rupyRegistrationB 101F5158
data l_rupyRegistrationC 101F5164
data l_rupyObjectA 1048E010
data l_rupyObjectB 1048E011
data l_rupyGroupNames 1048E024
data l_rupyAnimationNames 1048E03C
data l_rupyDigitNames 1048E06C
data l_rupyUnusedNames 1048E084
data l_rupyLayoutNames 1048E09C
data l_rupyPartName 1048E10C
data l_rupyDigitType 1048E170
data l_rupyStateWait 1048E0AC
data l_rupyStateExit 1048E0DC
data l_rupyStateGuard 101FD8E4
data l_rupyNextStateId 101FDD50
data l_rupyAnimationGroups 100E2E84
str 100E2FE0 "auction text 100E2FE0" renamed
str 100E2FC0 "auction text 100E2FC0" renamed
str 100E3000 "auction text 100E3000" renamed
str 100E2F68 "auction text 100E2F68" renamed
str 100E2F70 "auction text 100E2F70" renamed
str 100E3010 "auction text 100E3010" renamed
str 100E2F9C "auction text 100E2F9C" renamed
str 100E2FB4 "auction text 100E2FB4" renamed
str 100E2F78 "auction text 100E2F78" renamed
str 100E301C "auction text 100E301C" renamed
str 100E3030 "auction text 100E3030" renamed
str 100E2FF4 "auction text 100E2FF4" renamed
str 100E2F88 "auction text 100E2F88" renamed
str 100E2FA8 "auction text 100E2FA8" renamed
str 100E303C "auction text 100E303C" renamed
str 100E2FD0 "auction text 100E2FD0" renamed
ext 026F88F0 void rupyBaseDestroy(RupyScreen*,u32)
ext 0273AF40 void rupyDelete(void*)
ext 0273AD10 void* rupyAllocate(u32)
ext 026F89E8 void rupyBaseConstruct(RupyScreen*)
ext 025200D4 RupyPlay* rupyGetPlay()
ext 02005674 void rupyAnimationStop(RupyAnimation*,u32,u32)
ext 020053E4 void rupyAnimationPlay(RupyAnimation*,u32,u32,f32)
ext 0200552C void rupyAnimationFrame(RupyAnimation*,u32,u32,f32)
ext 02005708 void rupyAnimationReset(RupyAnimation*,u32,u32)
ext 02005488 void rupyAnimationLimit(RupyAnimation*,u32,u32,f32)
ext 02676900 void rupyAuctionOpen(void*)
ext 026768E4 void rupyAuctionClose(void*)
ext 025916EC void rupyInputOpen()
ext 025916FC void rupyInputClose()
ext 0261D454 void rupyDigitSet(void*,u32)
ext 0261D4A8 void rupyDigitRoll(void*,u32,u32,u32)
ext 025E1988 void rupyPlaySound(u32)
ext 020063C0 void rupyChangeState(gptr<RupyStateMachine>*,RupyState*)
ext 026F90D8 void rupySetLayoutNames(RupyScreen*,RupyLayoutNames*,u32)
ext 028713B8 u32 rupyFrameCount(void*)
ext 026F91F8 void rupyBaseRemove(RupyScreen*)
ext 02006478 RupyState* rupyGetCurrentState(gptr<RupyStateMachine>*)
ext 02006364 void rupyExecuteState(gptr<RupyStateMachine>*)
ext 026F92C0 u32 rupyBaseCreateChild(RupyScreen*,u32,RupyPart*)
ext 0273B050 void* rupyAllocateAligned(u32,void*,u32)
ext 026FADB0 RupyLayout* rupyLayoutConstruct(RupyLayout*)
ext 02004E04 void rupyBindAnimation(RupyAnimation*,u32,RupyString*,RupyString*)
ext 028F026C void rupyRegister(RupyRegistration*)
ext 028ED6F8 void rupyConstructFirst(be<u8>*)
ext 028EAB2C void rupyConstructSecond(be<u8>*)
*/
