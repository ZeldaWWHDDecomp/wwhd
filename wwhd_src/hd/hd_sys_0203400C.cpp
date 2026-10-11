/* HD exception and panic console. Private reconstruction. */
#include "hd/hd_exception_mgr.h"
using namespace gabi;
extern void *hdExcAllocate(u32);
extern void hdExcDelete(void *);
extern void hdExcHeapConstruct(HdExcHeap *);
extern void hdExcMutexConstruct(HdExcMutex *);
extern void hdExcMutexDestroy(HdExcMutex *, u32);
extern void hdExcLock(HdExcMutex *);
extern void hdExcUnlock(HdExcMutex *);
extern u32 hdExcTryLock(HdExcMutex *);
extern void hdExcHeapRelease(be<u32> *);
extern void hdExcHeapDestroy(HdExcHeap *, u32);
extern void hdExcVPrintf(const char *, HdExcVaList *);
extern gfn *hdExcSetCallback(u32, gfn *);
extern void hdExcRegisterCallback(void *);
extern void hdExcDumpFirstArgs(HdExcManager *, const char *, u32, const char *, HdExcVaList *);
extern void hdExcPanicFinish();
extern void hdExcRegister(void *);
extern void hdExcInitSigned(be<u8> *);
extern void hdExcInitUnsigned(be<u8> *);
static u32 l_classGuard;
static u32 l_baseGuard;
static u32 l_controller1Guard;
static u32 l_controller2Guard;
static HdExcType l_classType;
static HdExcType l_baseType;
static HdExcType l_controller1Type;
static HdExcType l_controller2Type;
static u32 l_classParent;
static u32 l_baseParent;
static u32 l_controllerParent;
static u32 l_heapVTable;
static u32 l_entryVTable;
static u32 l_delegateVTable;
static u32 l_printfEntry;
static u32 l_exceptionHandler;
static u32 l_dumpHandler;
static HdExcManager l_manager;
static u32 l_managerGuard;
static HdExcRegistration l_managerRegistration;
static HdExcStatics l_statics;
static HdExcRange l_range;
static f32 l_lower;
static f32 l_upper;
static HdExcRegistration l_registration1;
static HdExcRegistration l_registration2;
static HdExcRegistration l_registration3;

// HD: this wrapper preserves each call's variadic register arguments.
template <class... Args> static void exceptionPrintf(const char *format, Args... args) {
    gabi::call<void>(gabi::ea(&l_printfEntry), format, args...);
}

/* 0203400C */
u32 hdExcIsDerived(void *self, HdExcType *type) {
    if (!l_classGuard) {
        l_classGuard = 1;
        l_classType.mParent = &l_classParent;
    }
    if (type == &l_classType)
        return 1;
    if (!l_baseGuard) {
        l_baseGuard = 1;
        l_baseType.mParent = &l_baseParent;
    }
    return type == &l_baseType ? 1 : 0;
}

static u32 invokeDelegate(HdExcDelegate *delegate, u32 argument) {
    void *object = delegate->mObject;
    if (!object)
        return gabi::ea(delegate);
    s32 index = delegate->mIndex;
    if (!index)
        return gabi::ea(delegate);
    // HD: signed this adjustment and vtable offset use wrapping guest address arithmetic.
    void *adjusted = gabi::at<void>(gabi::ea(object) + (u32)(s32)delegate->mThisAdjustment);
    if (index < 0)
        return gabi::call_ptr<u32>(gabi::ea(delegate->mFunction.get()), adjusted, argument);
    HdExcVTablePointer *view =
        gabi::at<HdExcVTablePointer>(gabi::ea(adjusted) + (u32)(s32)delegate->mVTableOffset);
    return gabi::call_ptr<u32>(view->mVTable[(u32)index * 2 + 1].get(), adjusted, argument);
}

/* 02034084 */
u32 hdExcDelegateInvokeFirst(HdExcDelegate *self, u32 argument) {
    return invokeDelegate(self, argument);
}

/* 020340D8 */
u32 hdExcDelegateInvokeSecond(HdExcDelegate *self, u32 argument) {
    return invokeDelegate(self, argument);
}

/* 0203412C */
void hdExcEmptyCompanion() {}

static HdExcController *findDerived(HdExcControllerOwner *self, be<u32> *guard, HdExcType *type) {
    u32 offset = self->mHead.mOffset;
    HdExcOffsetNode *head = &self->mHead;
    u32 end = gabi::ea(head) - offset;
    HdExcController *object = gabi::at<HdExcController>(gabi::ea(self->mHead.mNext.get()) - offset);
    while (gabi::ea(object) != end) {
        if (!*guard) {
            *guard = 1;
            type->mParent = &l_controllerParent;
        }
        bool derived = false;
        if (object && gabi::call_ptr<u32>(object->mVTable[3].get(), object, type))
            derived = true;
        if (derived)
            return object;
        HdExcOffsetNode *node = gabi::at<HdExcOffsetNode>(gabi::ea(object) + offset);
        object = gabi::at<HdExcController>(gabi::ea(node->mNext.get()) - offset);
        end = gabi::ea(head) - (u32)head->mOffset;
    }
    return NULL;
}

/* 02034130 */
HdExcController *hdExcFindFirstController(HdExcControllerOwner *self) {
    return findDerived(self, &l_controller1Guard, &l_controller1Type);
}

/* 02034200 */
HdExcController *hdExcFindSecondController(HdExcControllerOwner *self) {
    return findDerived(self, &l_controller2Guard, &l_controller2Type);
}

/* 020342D0 */
HdExcManager *hdExcManagerConstruct(HdExcManager *self) {
    if (!self) {
        self = static_cast<HdExcManager *>(hdExcAllocate(sizeof(HdExcManager)));
        if (!self)
            return NULL;
    }
    self->mProgramCallback = NULL;
    self->mIsiCallback = NULL;
    self->mDsiCallback = NULL;
    self->mHead = NULL;
    // HD: the embedded heap address can wrap to zero and use a separate allocation.
    HdExcHeap *heap = gabi::at<HdExcHeap>(gabi::ea(self) + offsetof(HdExcManager, mHeap));
    if (!heap)
        heap = static_cast<HdExcHeap *>(hdExcAllocate(sizeof(HdExcHeap)));
    if (heap) {
        hdExcHeapConstruct(heap);
        heap->mReserved14 = 0;
        heap->mReserved20 = 0;
        heap->mVTable = reinterpret_cast<gfn *>(&l_heapVTable);
        heap->mActive = 0;
        heap->mSelf = heap;
        heap->mHandle = 0;
        heap->mRefObject = NULL;
    }
    hdExcMutexConstruct(&self->mMutex);
    return self;
}

/* 02034380 */
HdExcManager *hdExcManagerInstance() {
    if (!l_managerGuard) {
        l_managerGuard = 1;
        hdExcManagerConstruct(&l_manager);
        hdExcRegister(&l_managerRegistration);
    }
    return &l_manager;
}

/* 020343D8 */
void hdExcManagerAdd(HdExcManager *self, HdExcEntry *entry) {
    hdExcLock(&self->mMutex);
    entry->mNext = self->mHead;
    self->mHead = entry;
    hdExcUnlock(&self->mMutex);
}

/* 02034434 */
HdExcEntry *hdExcEntryConstruct(HdExcEntry *self) {
    if (!self) {
        self = static_cast<HdExcEntry *>(hdExcAllocate(sizeof(HdExcEntry)));
        if (!self)
            return NULL;
    }
    self->mNext = NULL;
    self->mVTable = reinterpret_cast<gfn *>(&l_entryVTable);
    HdExcManager *manager = hdExcManagerInstance();
    hdExcManagerAdd(manager, self);
    return self;
}

/* 02034494 */
void hdExcManagerRemove(HdExcManager *self, HdExcEntry *entry) {
    hdExcLock(&self->mMutex);
    HdExcEntry *next = entry->mNext;
    entry->mNext = NULL;
    HdExcEntry *current = self->mHead;
    if (current == entry) {
        self->mHead = next;
        hdExcUnlock(&self->mMutex);
        return;
    }
    for (; current; current = current->mNext) {
        if (current->mNext == entry) {
            current->mNext = next;
            hdExcUnlock(&self->mMutex);
            return;
        }
    }
    hdExcUnlock(&self->mMutex);
}

/* 02034538 */
void hdExcEntryDestroy(HdExcEntry *self, s32 flags) {
    if (!self)
        return;
    self->mVTable = reinterpret_cast<gfn *>(&l_entryVTable);
    HdExcManager *manager = hdExcManagerInstance();
    hdExcManagerRemove(manager, self);
    if (flags & 1)
        hdExcDelete(self);
}

// HD: register-save bytes follow the GHS variadic ABI, including the caller's CR1 flag.
static void saveVarargs(HdExcVaFrame *frame, const u32 *registers) {
    for (int i = 0; i < 8; ++i)
        frame->mGprs[i] = registers[i];
    if (gabi::cpu->cr[6]) {
        for (int i = 0; i < 8; ++i)
            frame->mFprs[i] = gabi::cpu->f[1 + i].ps0;
    }
}

/* 02034598 */
void hdExcPrint(const char *format, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    u32 entry = gabi::cpu->r[1];
    HdExcVaFrame *frame = gabi::at<HdExcVaFrame>(entry - sizeof(HdExcVaFrame));
    gabi::cpu->r[1] = gabi::ea(frame);
    frame->mBackchain = entry;
    const u32 registers[8] = {gabi::guest_addr(format), a4, a5, a6, a7, a8, a9, a10};
    saveVarargs(frame, registers);
    frame->mList.mOverflow = gabi::at<void>(entry + 8);
    frame->mList.mFprIndex = 0;
    frame->mList.mRegisterSave = &frame->mGprs;
    frame->mList.mGprIndex = 1;
    hdExcVPrintf(format, &frame->mList);
    gabi::cpu->r[1] = entry;
}

/* 02034620 */
void hdExcManagerDestroy(HdExcManager *self, s32 flags) {
    if (!self)
        return;
    hdExcMutexDestroy(&self->mMutex, 2);
    HdExcHeap *heap = gabi::at<HdExcHeap>(gabi::ea(self) + offsetof(HdExcManager, mHeap));
    if (heap) {
        u8 active = heap->mActive;
        heap->mVTable = reinterpret_cast<gfn *>(&l_heapVTable);
        if (active) {
            HdExcRefObject *ref = heap->mRefObject;
            if (ref) {
                heap->mRefObject = NULL;
                hdExcHeapRelease(&heap->mHandle);
                ref->mReferenceCount = ref->mReferenceCount - 1;
            }
            heap->mActive = 0;
        }
        hdExcHeapDestroy(heap, 0);
    }
    if (flags & 1)
        hdExcDelete(self);
}

/* 020346DC */
void hdExcManagerDump(HdExcManager *self) {
    if (!hdExcTryLock(&self->mMutex)) {
        exceptionPrintf("Dump lock unavailable");
        return;
    }
    // HD: the successful tryLock intentionally remains held while invoking all entries.
    for (HdExcEntry *entry = self->mHead; entry; entry = entry->mNext) {
        exceptionPrintf("Dump entry", entry);
        gabi::call_ptr<void>(entry->mVTable[5].get(), entry);
    }
}

/* 02034784 */
void hdExcRestoreCallbacks(HdExcCallbacks *self) {
    hdExcSetCallback(HD_EXC_DSI, self->mDsiCallback.get());
    hdExcSetCallback(HD_EXC_ISI, self->mIsiCallback.get());
    hdExcSetCallback(HD_EXC_PROGRAM, self->mProgramCallback.get());
}

/* 020347D0 */
u32 hdExcHandler() {
    exceptionPrintf("Exception dump begin");
    hdExcManagerDump(hdExcManagerInstance());
    hdExcRestoreCallbacks(reinterpret_cast<HdExcCallbacks *>(hdExcManagerInstance()));
    exceptionPrintf("Exception dump end");
    return 1;
}

/* 02034820 */
void hdExcInstallCallbacks(HdExcCallbacks *self) {
    self->mDsiCallback = hdExcSetCallback(HD_EXC_DSI, reinterpret_cast<gfn *>(&l_exceptionHandler));
    self->mIsiCallback = hdExcSetCallback(HD_EXC_ISI, reinterpret_cast<gfn *>(&l_exceptionHandler));
    self->mProgramCallback =
        hdExcSetCallback(HD_EXC_PROGRAM, reinterpret_cast<gfn *>(&l_exceptionHandler));
}

/* 02034884 */
void hdExcInitialize(HdExcCallbacks *self) {
    hdExcInstallCallbacks(self);
    HdExcDelegate *delegate =
        gabi::at<HdExcDelegate>(gabi::ea(self) + offsetof(HdExcCallbacks, mDumpDelegate));
    if (delegate) {
        delegate->mObject = self;
        delegate->mThisAdjustment = 0;
        delegate->mVTable = reinterpret_cast<gfn *>(&l_delegateVTable);
        delegate->mFunction = reinterpret_cast<gfn *>(&l_dumpHandler);
        delegate->mIndex = -1;
    }
    self->mDelegate = delegate;
    hdExcRegisterCallback(&self->mRegistration);
}

/* 020348F4 */
void hdExcDumpFirst(HdExcManager *self) {
    exceptionPrintf("Exception dump first");
    hdExcManagerDump(self);
}

/* 02034934 */
void hdExcDumpSecond(HdExcManager *self) {
    exceptionPrintf("Exception dump second");
    hdExcManagerDump(self);
}

/* 02034974 */
void hdExcPanic(const char *file, u32 line, const char *format, u32 a6, u32 a7, u32 a8, u32 a9,
                u32 a10) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    u32 entry = gabi::cpu->r[1];
    HdExcPanicFrame *frame = gabi::at<HdExcPanicFrame>(entry - sizeof(HdExcPanicFrame));
    gabi::cpu->r[1] = gabi::ea(frame);
    frame->mSavedR31 = gabi::cpu->r[31];
    frame->mSavedR30 = gabi::cpu->r[30];
    frame->mSavedR29 = gabi::cpu->r[29];
    const u32 registers[8] = {
        gabi::guest_addr(file), line, gabi::guest_addr(format), a6, a7, a8, a9, a10};
    saveVarargs(&frame->mArgs, registers);
    frame->mArgs.mList.mFprIndex = 0;
    frame->mArgs.mList.mGprIndex = 3;
    u32 indices = frame->mArgs.mList.mIndices;
    frame->mArgs.mList.mRegisterSave = &frame->mArgs.mGprs;
    frame->mArgs.mList.mOverflow = gabi::at<void>(entry + 8);
    frame->mCopy.mIndices = indices;
    frame->mCopy.mOverflow = gabi::at<void>(entry + 8);
    frame->mCopy.mRegisterSave = &frame->mArgs.mGprs;
    exceptionPrintf("Panic file and line", file, line);
    hdExcVPrintf(format, &frame->mArgs.mList);
    exceptionPrintf("Panic newline");
    HdExcManager *manager = hdExcManagerInstance();
    hdExcDumpFirstArgs(manager, file, line, format, &frame->mCopy);
    hdExcPanicFinish();
    gabi::cpu->r[1] = entry;
}

/* 02034A80 */
void hdExcStaticInit() {
    l_statics.mWords[2] = 0;
    l_statics.mWords[0] = 0;
    l_statics.mWords[3] = 0;
    l_statics.mWords[1] = 0;
    hdExcRegister(&l_registration1);
    const f32 lower = l_lower;
    const f32 upper = l_upper;
    l_range.mLower = lower;
    l_range.mUpper = upper;
    hdExcInitSigned(&l_range.mSigned);
    hdExcRegister(&l_registration2);
    hdExcInitUnsigned(&l_range.mUnsigned);
    hdExcRegister(&l_registration3);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_classGuard 101FD598
data l_baseGuard 101FD594
data l_controller1Guard 101FD698
data l_controller2Guard 101FD694
data l_classType 101FD97C
data l_baseType 101FD980
data l_controller1Type 101FD978
data l_controller2Type 101FD968
data l_classParent 10004AA4
data l_baseParent 10004A94
data l_controllerParent 10004AB4
data l_heapVTable 10004CB0
data l_entryVTable 10004C98
data l_delegateVTable 10004CC0
data l_printfEntry 0286DBB0
data l_exceptionHandler 020347D0
data l_dumpHandler 02034934
data l_manager 1020084C
data l_managerGuard 102008D4
data l_managerRegistration 1018F2C0
data l_statics 1020083C
data l_range 10200830
data l_lower 10004DEC
data l_upper 10004DF0
data l_registration1 1018F2CC
data l_registration2 1018F2D8
data l_registration3 1018F2E4
str 10004CD0 "Dump lock unavailable" renamed
str 10004D00 "Dump entry" renamed
str 10004D2C "Exception dump begin" renamed
str 10004D5C "Exception dump end" renamed
str 10004D6C "Exception dump first" renamed
str 10004DA8 "Exception dump second" renamed
str 10004DDC "Panic file and line" renamed
str 10004DD8 "Panic newline" renamed
ext 0273AD10 void* hdExcAllocate(u32)
ext 0273AF40 void hdExcDelete(void*)
ext 02752A84 void hdExcHeapConstruct(HdExcHeap*)
ext 02760084 void hdExcMutexConstruct(HdExcMutex*)
ext 02760168 void hdExcMutexDestroy(HdExcMutex*,u32)
ext 027601BC void hdExcLock(HdExcMutex*)
ext 027601F0 void hdExcUnlock(HdExcMutex*)
ext 027601C4 u32 hdExcTryLock(HdExcMutex*)
ext 0273B310 void hdExcHeapRelease(be<u32>*)
ext 02752BEC void hdExcHeapDestroy(HdExcHeap*,u32)
ext 0286DB50 void hdExcVPrintf(const char*,HdExcVaList*)
ext C000A018 gfn* hdExcSetCallback(u32,gfn*)
ext 0273ED58 void hdExcRegisterCallback(void*)
ext 020348F4 void hdExcDumpFirstArgs(HdExcManager*,const char*,u32,const char*,HdExcVaList*)
ext 0286DB54 void hdExcPanicFinish()
ext 028F026C void hdExcRegister(void*)
ext 028ED6F8 void hdExcInitSigned(be<u8>*)
ext 028EAB2C void hdExcInitUnsigned(be<u8>*)
*/
