/** d_mesg_core.cpp — HD message manager core.
 * GameCube message work lives in d_mesg.cpp.
 */
#include "d/d_mesg_core.h"

using CorePtr = gptr<dMesg_core_c>;
static CorePtr messageManager;
static CorePtr messageStorage;
static u32 scaleInitialized;
static cXyz messageScale;
static f32 initialA;
static f32 initialB;
static f32 initialZero;
static f32 initialOne;
static f32 initialSpeed;
static u32 timerPeriods[3] = {8, 2, 4};

/* 025F7500 */
dMesg_core_c* dMesg_core_ct(dMesg_core_c* self) {
    if (!self) {
        self = static_cast<dMesg_core_c*>(operator_new(sizeof(dMesg_core_c)));
        if (!self)
            return nullptr;
    }
    self->mFrameCount = 0;
    dMesg_core_initProcessorWork(self->mProcessorWork);
    self->field_0x970 = 0;
    self->field_0x984 = 0;
    f32 a = initialA;
    f32 b = initialB;
    self->field_0x978 = a;
    self->field_0x974 = b;
    f32 zero = initialZero;
    f32 one = initialOne;
    self->field_0x988 = zero;
    self->mResetWork = nullptr;
    f32 speed = initialSpeed;
    self->field_0x98c = zero;
    self->field_0x980 = speed;
    // HD: retain GHS's 32-bit guest-address wrap check for the timer array.
    bool nullTimers = gabi::ea(&self->mTimers[0]) == 0;
    self->mSelectNum = 0xFFFFFFFF;
    self->mAuxScreenWork = nullptr;
    self->mMsgNo = 0xFFFFFFFF;
    self->field_0x990 = zero;
    self->field_0x97c = one;
    self->field_0x968 = 0;
    self->mRenderWork = nullptr;
    self->mResourceWork = nullptr;
    self->field_0x985 = 0;
    self->mSequenceWork = nullptr;
    self->field_0x96c = 0;
    self->mMode = 6;
    self->mScreenWork = nullptr;
    if (nullTimers)
        operator_new(sizeof(self->mTimers));
    self->field_0x9a0 = 0;
    self->field_0x9a1 = 0;
    dMesg_core_setMesgStatus(self, 0);
    if (!scaleInitialized) {
        scaleInitialized = 1;
        messageScale.x = one;
        messageScale.z = one;
        messageScale.y = one;
    }
    for (u32 i = 0; i < 3; ++i)
        self->mTimers[i] = 0;
    return self;
}

/* 025F7658 */
dMesg_core_c* dMesg_core_create(JKRHeap* heap) {
    dMesg_core_c* result = messageManager;
    if (result)
        return result;
    auto* storage = static_cast<dMesg_core_c*>(dMesg_core_allocate(sizeof(dMesg_core_c), heap, 4));
    if (storage) {
        dMesg_core_initDisposer(storage, heap, 3);
        storage->__vtbl = dMesg_core_c::__vt;
    }
    messageStorage = storage;
    result = storage ? dMesg_core_ct(storage) : nullptr;
    messageManager = result;
    return result;
}

/* 025F76EC */
void dMesg_core_dt(dMesg_core_c* self, u32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}

/* 025F7700 */
void dMesg_core_c::advanceTimers() {
    for (u32 i = 0; i < 3; ++i) {
        mTimers[i] = mTimers[i] + 1;
        u32 period = timerPeriods[i] * 20;
        u32 count = mTimers[i];
        // HD: signed comparison after unsigned increment/multiplication, including wrap.
        if (static_cast<s32>(count) >= static_cast<s32>(period))
            mTimers[i] = count - period;
    }
}

/* 025F77B0 */
void dMesg_core_c::update() {
    u32 count = mFrameCount + 1;
    mFrameCount = count % 10000;
    if (dMesg_core_getPlay()->mFrameFlag)
        dMesg_core_getPlay()->mFrameFlag = 0;
    if (dMesg_core_getPlay()->mSpecialMesgFlag) {
        dMesg_word_work* word = mWordWork;
        bool keep = false;
        if (!word || word->mType == 9) {
            constexpr u32 ids[] = {0x80000002, 0x80000025, 0x80000024,
                                   0x80000027, 0x8000004E, 0x8000004F};
            // HD: re-read the current message for each comparison, as in the original.
            for (u32 id : ids) {
                if (dMesg_core_getCurrentMsgNo() == id) {
                    keep = true;
                    break;
                }
            }
        }
        if (!keep)
            dMesg_core_getPlay()->mSpecialMesgFlag = 0;
    }
    advanceTimers();
}

/* 025F78C4 */
void dMesg_core_c::reset() {
    dMesg_reset_work* work = mResetWork;
    mFrameCount = 0;
    work->reset();
    mScreenWork->reset();
    mRenderWork->reset();
    mAuxScreenWork->reset();
    dMesg_core_resetSequence(mSequenceWork);
    dMesg_core_resetResource(mResourceWork);
    dMesg_core_setMesgStatus(this, 0);
}

/* 025F795C */
u32 dMesg_core_getMesgStatus() {
    return dMesg_core_getPlay()->mMesgStatus;
}

/* WWHD glue (tools/verify/autoglue.py)
data messageManager 101F4B5C
data messageStorage 101F4B60
data scaleInitialized 101FD2EC
data messageScale 101FDD44
data initialA 100E0C54
data initialB 100E0C50
data initialZero 100E0AD8
data initialOne 100E0C4C
data initialSpeed 100E0C58
data timerPeriods 100E0C5C
ext 025F65F4 void dMesg_core_initProcessorWork(void* work)
ext 0273B0D4 void* dMesg_core_allocate(u32 size, JKRHeap* heap, s32 alignment)
ext 02752B0C void dMesg_core_initDisposer(dMesg_core_c* self, JKRHeap* heap, u32 flags)
ext 025F74D0 void dMesg_core_setMesgStatus(dMesg_core_c* self, u32 status)
ext 025E1E60 u32 dMesg_core_getCurrentMsgNo()
ext 026BB330 void dMesg_reset_work::reset()
ext 026AD6D4 void dMesg_core_resetSequence(void* work)
ext 026516C0 void dMesg_core_resetResource(void* work)
*/
