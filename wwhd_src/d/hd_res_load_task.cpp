/**
 * HD asynchronous resource loading: request queues, archive reference counts and load polling.
 * Verified against the original with unit hd_res_load_task.
 */
// HD: This task and its sead string/heap plumbing have no GameCube counterpart.
#include "d/hd_res_load_task.h"
using namespace gabi;

namespace hd_res_load_task {
static MappedObject sSafeStringVtable;
static MappedObject sFixed64Vtable;
static MappedObject sTaskSecondVtable;
static MappedObject sTaskVtable;
static MappedObject sBufferedStringVtable;
static MappedObject sFixed64BaseVtable;
static MappedObject sFixed128Vtable;
static MappedObject sReferenceNodeVtable;
static MappedObject sTaskTypeName;
static MappedObject sBaseTypeName;
static MappedObject sTaskGlobalDestructor;
static MappedObject sHeaderAGlobalDestructor;
static MappedObject sHeaderBGlobalDestructor;
static MappedObject sHeaderObjectA;
static MappedObject sHeaderObjectB;
static SafeString sEmptyName;
static u32 sInstance;
static u32 sResourceManager;
static u32 sResourceControl;
static u32 sHeapManager;
static u32 sHeapEnvironment;
static u32 sFontManager;
static u32 sMessageManager;
static u32 sJpegManager;
static u32 sRandom;

static s16 sWaitRange;
static s16 sWaitMinimum;
static RequestRing sEmptyRing;
static f32 sDefaultLow;
static f32 sDefaultHigh;
static f32 sInitialLow;
static f32 sInitialHigh;
static u32 sTaskTypeInitialized;
static u32 sBaseTypeInitialized;
static u32 sTaskTypeInfo;
static u32 sBaseTypeInfo;

/* 0260B770 */
void singletonDisposer__dt(be<u8> *i_this, u32 flags) {
    const u32 p = gabi::ea(i_this);
    if (!p) {
        return;
    }
    if ((u8)*i_this) {
        sInstance = 0;
    }
    if (flags & 1) {
        resLoadTask_deleteObject(p);
    }
}

/* 0260B79C */
void taskSecondBase__dt(MappedObject *i_this, u32 flags) {
    const u32 p = gabi::ea(i_this);
    resLoadTask_destroyTask(p - 0x14, flags);
}

/* 0260B7A4 */
void ResLoadTask::setInstance() {
    const u32 self = gabi::ea(this);
    if (sInstance) {
        return;
    }
    sInstance = self;
    mIsInstance = 1;
}

/* zeroes a 16-byte ring header (placement-new null check as GHS emits it) */
/* inlined into ResLoadTask__ct */
static void ringCtor(IDRing *i_q) {
    u32 q = gabi::ea(i_q);
    if (!q) {
        q = resLoadTask_allocateObject(0x10u);
    }
    if (q) {
        gabi::at<IDRing>(q)->mCapacity = 0;
        gabi::at<IDRing>(q)->mHead = 0;
        gabi::at<IDRing>(q)->mEntries = 0;
        gabi::at<IDRing>(q)->mCount = 0;
    }
}

/* 0260B7C4 */
// HD: Free adapter preserves the GHS allocating-constructor return register.
ResLoadTask *ResLoadTask__ct(ResLoadTask *i_this, MappedObject *i_arg) {
    u32 p = gabi::ea(i_this);
    const u32 arg = gabi::ea(i_arg);
    if (!p) {
        p = resLoadTask_allocateObject(0x1120u);
        if (!p) {
            return 0;
        }
    }
    ResLoadTask *task = gabi::at<ResLoadTask>(p);
    resLoadTask_constructTaskBase(p, arg, gabi::guest_addr("ResLoadTask"));
    task->mIsInstance = 0;
    task->mTaskSecondVtable = gabi::ea(&sTaskSecondVtable);
    task->mTaskVtable = gabi::ea(&sTaskVtable);
    ringCtor(gabi::at<IDRing>(gabi::ea(&task->mRequests)));
    task->mState = STATE_IDLE;
    u32 t = gabi::ea(&task->mReferences);
    if (!t) {
        t = resLoadTask_allocateObject(0x1014u);
    }
    if (t) {
        const u32 nodes = gabi::ea(gabi::at<ReferenceTree>(t)->mStorage);
        ReferenceNode *nodesReferenceNode = gabi::at<ReferenceNode>(nodes);
        gabi::at<ReferenceTree>(t)->mRoot = 0;
        gabi::at<ReferenceTree>(t)->mCount = 0;
        gabi::at<ReferenceTree>(t)->mFree = nodes;
        gabi::at<ReferenceTree>(t)->mCapacity = 0x80;
        for (u32 i = 0; i < 0x7F; i++) {
            nodesReferenceNode[i].mLeft = nodes + sizeof(ReferenceNode) * (i + 1);
        }
        nodesReferenceNode[127].mLeft = 0;
        gabi::at<ReferenceTree>(t)->mNodes = nodes;
    }
    ringCtor(gabi::at<IDRing>(gabi::ea(&task->mLoadedIDs)));
    ringCtor(gabi::at<IDRing>(gabi::ea(&task->mUnloads)));
    task->mFlushUnloads = 0;
    task->mCurrentHeap = 0;
    task->mWaitTimer = 0;
    return task;
}

/* inlined into entriesCtor */
static void fixed64Ctor(FixedString64 *i_s) {
    const u32 s = gabi::ea(i_s);
    u32 q = s;
    if (!q) {
        q = resLoadTask_allocateObject(0xCu);
    }
    if (q) {
        gabi::at<FixedString64>(q)->mVtable = gabi::ea(&sBufferedStringVtable);
        gabi::at<FixedString64>(q)->mCapacity = 0x40;
        gabi::at<FixedString64>(q)->mStringTop = s + 0xC;
        i_s->mBuffer[63] = 0;
    }
    const u32 top = (u32)(i_s->mStringTop);
    i_s->mVtable = gabi::ea(&sFixed64BaseVtable);
    *gabi::at<be<u8>>(top) = 0;
    i_s->mVtable = gabi::ea(&sFixed64Vtable);
}

/* inlined into prepare */
static void entriesCtor(Request *i_base) {
    const u32 base = gabi::ea(i_base);
    for (u32 i = 0; i < 0x80; i++) {
        const u32 e = base + sizeof(Request) * i;
        Request *eRequest = gabi::at<Request>(e);
        if (!e) {
            continue;
        }
        u32 s = e;
        if (!s) {
            s = resLoadTask_allocateObject(0x4Cu);
        }
        if (s) {
            fixed64Ctor(gabi::at<FixedString64>(s));
        }
        s = e + 0x4C;
        if (!s) {
            s = resLoadTask_allocateObject(0x4Cu);
        }
        if (s) {
            fixed64Ctor(gabi::at<FixedString64>(s));
        }
        eRequest->mIsArchive = 0;
        eRequest->mID = 0;
        eRequest->mIsStage = 0;
    }
}

/* inlined into prepare and startLoad */
static u32 heapAlloc(Heap *i_heap, u32 size) {
    const u32 heap = gabi::ea(i_heap);
    const u32 h = heap ? heap : resLoadTask_getCurrentHeap(sHeapManager);
    return call_ptr<u32>((u32)gabi::at<HeapVtable>(gabi::at<Heap>(h)->mVtable)->mAlloc, h, size,
                         4u);
}

/* inlined into prepare */
static void setStr(SafeString *i_s, be<u8> *i_top) {
    const u32 top = gabi::ea(i_top);
    i_s->mVtable = gabi::ea(&sSafeStringVtable);
    i_s->mStringTop = top;
}

/* 0260B91C */
// Prepare permanent packs, archive heaps, font/message managers and the request queues.
void ResLoadTask::prepare() {
    const u32 self = gabi::ea(this);
    Local<u8[0x40]> F;
    const u32 f = F.a;
    const u32 a = f + 8, b = f + 0x10, e = f + 0x18, c = f + 0x20, d = f + 0x28, g = f + 0x30,
              h = f + 0x38;
    SafeString *aSafeString = gabi::at<SafeString>(a);
    resLoadTask_prepareTaskBase(self);
    setStr(aSafeString, gabi::at<be<u8>>(gabi::guest_addr("Pack/permanent_3d.pack")));
    resLoadTask_loadPermanentPack(sResourceManager, a,
                                  (u32)gabi::at<ResourceControl>(sResourceControl)->mPermanentHeap);
    aSafeString->mStringTop = gabi::guest_addr("Pack/first_szs_permanent.pack");
    aSafeString->mVtable = gabi::ea(&sSafeStringVtable);
    resLoadTask_loadPermanentArchive(
        sResourceManager, a, (u32)gabi::at<ResourceControl>(sResourceControl)->mPermanentHeap);
    u32 mgr = sResourceManager;
    const u32 pack = resLoadTask_getPermanentPackName(mgr);
    resLoadTask_loadPermanentPack(mgr, pack,
                                  (u32)gabi::at<HeapEnvironment>(sHeapEnvironment)->mParentHeap);
    const u32 parent = (u32)gabi::at<ResourceControl>(sResourceControl)->mParentHeap;
    setStr(aSafeString, gabi::at<be<u8>>(gabi::guest_addr("Particle")));
    u32 heap = resLoadTask_createExpHeap(0u, a, parent, 1u, 0u);
    mgr = sResourceManager;
    aSafeString->mVtable = gabi::ea(&sSafeStringVtable);
    gabi::at<SafeString>(b)->mVtable = gabi::ea(&sSafeStringVtable);
    aSafeString->mStringTop = gabi::guest_addr("Particle");
    gabi::at<SafeString>(b)->mStringTop = gabi::guest_addr("Particle/Particle.szs");
    resLoadTask_registerArchive(mgr, a, b, heap, 1u, 0x40000u, 0x2000u);
    call_ptr<void>((u32)gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable)->mAdjust, heap);
    setStr(gabi::at<SafeString>(b), gabi::at<be<u8>>(gabi::guest_addr("ProgramTexture")));
    heap = resLoadTask_createExpHeap(0u, b, parent, 1u, 0u);
    mgr = sResourceManager;
    gabi::at<SafeString>(b)->mVtable = gabi::ea(&sSafeStringVtable);
    aSafeString->mVtable = gabi::ea(&sSafeStringVtable);
    gabi::at<SafeString>(b)->mStringTop = gabi::guest_addr("ProgramTexture");
    aSafeString->mStringTop = gabi::guest_addr("ProgramTexture/ProgramTexture.szs");
    resLoadTask_registerArchive(mgr, b, a, heap, 1u, 0x40000u, 0x2000u);
    call_ptr<void>((u32)gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable)->mAdjust, heap);
    setStr(gabi::at<SafeString>(c), gabi::at<be<u8>>(gabi::guest_addr("FontResMgr")));
    u32 fh = resLoadTask_createFrameHeap(
        0x1000u, c, (u32)gabi::at<HeapEnvironment>(sHeapEnvironment)->mParentHeap, 1u, 0u);
    resLoadTask_createFontManager(fh);
    resLoadTask_attachFontHeap(sFontManager, fh);
    setStr(gabi::at<SafeString>(d), gabi::at<be<u8>>(gabi::guest_addr("MessageResMgr")));
    fh = resLoadTask_createFrameHeap(
        0x20000u, d, (u32)gabi::at<HeapEnvironment>(sHeapEnvironment)->mParentHeap, 1u, 0u);
    resLoadTask_createMessageManager(fh);
    resLoadTask_attachMessageHeap(sMessageManager, fh);
    resLoadTask_initializeJpegManager(sJpegManager);
    setStr(gabi::at<SafeString>(e), gabi::at<be<u8>>(gabi::guest_addr("LegendPicture")));
    u32 p2 = resLoadTask_getTextureParentHeap();
    heap = resLoadTask_createExpHeap(0u, e, p2, 1u, 0u);
    mgr = sResourceManager;
    gabi::at<SafeString>(e)->mVtable = gabi::ea(&sSafeStringVtable);
    gabi::at<SafeString>(e)->mStringTop = gabi::guest_addr("Jpeg");
    gabi::at<SafeString>(g)->mStringTop = gabi::guest_addr("Jpeg/Jpeg.sarc");
    gabi::at<SafeString>(g)->mVtable = gabi::ea(&sSafeStringVtable);
    resLoadTask_registerArchive(mgr, e, g, heap, 0u, 0x40000u, 0x2000u);
    call_ptr<void>((u32)gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable)->mAdjust, heap);
    setStr(gabi::at<SafeString>(h), gabi::at<be<u8>>(gabi::guest_addr("ResLoadTask")));
    p2 = resLoadTask_getTaskParentHeap();
    heap = resLoadTask_createExpHeap(0u, h, p2, 1u, 1u);
    u32 ring = heapAlloc(gabi::at<Heap>(heap), 0x5000);
    entriesCtor(gabi::at<Request>(ring));
    if (ring) {
        mRequests.mEntries = ring;
        mRequests.mHead = 0;
        mRequests.mCapacity = 0x80;
        mRequests.mCount = 0;
    }
    ring = heapAlloc(gabi::at<Heap>(heap), 0x200);
    if (ring) {
        mLoadedIDs.mEntries = ring;
        mLoadedIDs.mHead = 0;
        mLoadedIDs.mCapacity = 0x80;
        mLoadedIDs.mCount = 0;
    }
    ring = heapAlloc(gabi::at<Heap>(heap), 0x5000);
    entriesCtor(gabi::at<Request>(ring));
    if (ring) {
        mUnloads.mEntries = ring;
        mUnloads.mHead = 0;
        mUnloads.mCapacity = 0x80;
        mUnloads.mCount = 0;
    }
    call_ptr<void>((u32)gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable)->mAdjust, heap);
    mRequests.mHead = 0;
    mUnloads.mCount = 0;
    mUnloads.mHead = 0;
    mLoadedIDs.mCount = 0;
    mRequests.mCount = 0;
    mLoadedIDs.mHead = 0;
    resLoadTask_initializeLoadSignals();
    resLoadTask_initializeLoadNotification();
    setInstance();
}

/* 0260BEF0 */
void ResLoadTask::setIdle() { mState = STATE_IDLE; }

/* 0260BEFC */
u32 ResLoadTask::release(Request *i_entry) {
    const u32 entry = gabi::ea(i_entry);
    const u32 st = (u32)(mState);
    if (st == STATE_LOADING || st == STATE_WAITING) {
        return 0;
    }
    Local<u32> key;
    *gabi::at<be<u32>>(key.a) = i_entry->mID;
    const u32 node =
        resLoadTask_findReference(gabi::ea(&mReferences), (u32)(mReferences.mRoot), key.a);
    if (!node) {
        return 0;
    }
    // HD: Retain the GHS wrapped-address guard before accessing the reference count.
    const u32 rc = node + offsetof(ReferenceNode, mReferences);
    if (!rc) {
        return 0;
    }
    u32 n = *gabi::at<be<u32>>(rc);
    if (!n) {
        return 1;
    }
    n--;
    *gabi::at<be<u32>>(rc) = n;
    if (n) {
        return 1;
    }
    const u32 ctrl = sResourceControl;
    if ((u8)(i_entry->mIsArchive)) {
        resLoadTask_deleteArchive(ctrl, entry);
    } else {
        resLoadTask_deleteResource(ctrl, gabi::ea(&i_entry->mResource), entry);
    }
    return 1;
}

/* 0260BFC8 */
u32 ResLoadTask::unload(Request *i_entry) {
    const u32 entry = gabi::ea(i_entry);
    if (!(u8)(i_entry->mIsStage)) {
        return release(i_entry);
    }
    Local<FixedStringFrame> F;
    const u32 s = F.a;
    gabi::at<FixedString128>(s)->mCapacity = 0x80;
    gabi::at<FixedString128>(s)->mStringTop = s + 0xC;
    gabi::at<FixedString128>(s)->mBuffer[0] = 0;
    gabi::at<FixedString128>(s)->mVtable = gabi::ea(&sFixed128Vtable);
    gabi::at<FixedString128>(s)->mBuffer[127] = 0;
    call_ptr<void>((u32)gabi::at<SafeStringVtable>(i_entry->mArchive.mVtable)->mAssureTermination,
                   entry);
    const u32 fn = (u32)gabi::at<SafeStringVtable>(i_entry->mResource.mVtable)->mAssureTermination;
    const u32 arc = (u32)(i_entry->mArchive.mStringTop);
    call_ptr<void>(fn, gabi::ea(&i_entry->mResource));
    resLoadTask_formatString(s, gabi::guest_addr("%s_%s"), arc,
                             (u32)(i_entry->mResource.mStringTop));
    return resLoadTask_unloadStage(sResourceManager, s);
}

/* strlen capped at 0x40000 as sead does */
/* inlined into copyString and hashString */
static s32 cstrLength(be<u8> *i_p) {
    s32 n = 0;
    if (!(u8)*i_p) {
        return 0;
    }
    for (;;) {
        n++;
        if (n > 0x40000) {
            return 0;
        }
        i_p++;
        if (!(u8)*i_p) {
            return n;
        }
    }
}

/* copies the string object src into the buffered string dst (dst buffer loaded
 * first) */
/* inlined into peekRequest and request/release helpers */
static void copyString(BufferedString *i_dstObj, SafeString *i_src) {
    const u32 src = gabi::ea(i_src);
    const u32 dst = (u32)(i_dstObj->mStringTop);
    call_ptr<void>((u32)gabi::at<SafeStringVtable>(i_src->mVtable)->mAssureTermination, src);
    s32 n = cstrLength(gabi::at<be<u8>>((u32)(i_src->mStringTop)));
    const s32 size = s32((u32)(i_dstObj->mCapacity));
    const u32 fn = (u32)gabi::at<SafeStringVtable>(i_src->mVtable)->mAssureTermination;
    if (n >= size) {
        n = size - 1;
    }
    call_ptr<void>(fn, src);
    resLoadTask_moveBytes(dst, (u32)(i_src->mStringTop), u32(n), 0u);
    gabi::at<be<u8>>(dst)[n] = 0;
}

/* 0260C0A4 */
u32 ResLoadTask::peekRequest(Request *i_out) {
    const u32 out = gabi::ea(i_out);
    const s32 count = s32((u32)(mRequests.mCount));
    if (!count) {
        return 0;
    }
    u32 e;
    if (count > 0) {
        s32 i = s32((u32)(mRequests.mHead));
        const s32 cap = s32((u32)(mRequests.mCapacity));
        const u32 base = (u32)(mRequests.mEntries);
        if (i >= cap) {
            i -= cap;
        }
        e = base + u32(i) * sizeof(Request);
    } else {
        e = (u32)(mRequests.mEntries);
    }
    copyString(gabi::at<BufferedString>(out), gabi::at<SafeString>(e));
    copyString(gabi::at<BufferedString>(out + 0x4C), gabi::at<SafeString>(e + 0x4C));
    i_out->mID = (u32)(gabi::at<Request>(e)->mID);
    i_out->mIsArchive = (u8)(gabi::at<Request>(e)->mIsArchive);
    i_out->mIsStage = (u8)(gabi::at<Request>(e)->mIsStage);
    return 1;
}

/* 0260C290 */
u32 ResLoadTask::finish(Request *i_entry) {
    const u32 entry = gabi::ea(i_entry);
    const u32 ctrl = sResourceControl;
    const u32 heap = (u32)(mCurrentHeap);
    if ((u8)(i_entry->mIsArchive)) {
        resLoadTask_registerLoadedArchive(ctrl, entry, heap);
    } else {
        resLoadTask_registerLoadedResource(ctrl, gabi::ea(&i_entry->mResource), entry, heap);
    }
    return 1;
}

/* an empty request entry on the stack */
/* inlined into next, calc and request/release methods */
static void localEntry(Request *i_e) {
    const u32 e = gabi::ea(i_e);
    i_e->mID = 0;
    i_e->mArchive.mStringTop = e + 0xC;
    i_e->mResource.mCapacity = 0x40;
    i_e->mIsArchive = 0;
    i_e->mResource.mStringTop = e + 0x58;
    i_e->mArchive.mBuffer[0] = 0;
    i_e->mArchive.mBuffer[63] = 0;
    i_e->mResource.mBuffer[63] = 0;
    i_e->mArchive.mCapacity = 0x40;
    i_e->mResource.mBuffer[0] = 0;
    i_e->mArchive.mVtable = gabi::ea(&sFixed64Vtable);
    i_e->mResource.mVtable = gabi::ea(&sFixed64Vtable);
    i_e->mIsStage = 0;
}

/* 0260C2EC */
u32 ResLoadTask::next(Request *i_entry, u32 retry) {
    const u32 self = gabi::ea(this);
    if (!(u8)(i_entry->mIsStage)) {
        const s32 n = s32((u32)(mLoadedIDs.mCount));
        if (n < s32((u32)(mLoadedIDs.mCapacity))) {
            const s32 head = s32((u32)(mLoadedIDs.mHead));
            const s32 cap = s32((u32)(mLoadedIDs.mCapacity));
            s32 i = head + n;
            mLoadedIDs.mCount = u32(n + 1);
            const u32 id = (u32)(i_entry->mID);
            const u32 ids = (u32)(mLoadedIDs.mEntries);
            if (i >= cap) {
                i -= cap;
            }
            gabi::at<be<u32>>(ids)[u32(i)] = id;
        }
    }
    if (s32((u32)(mRequests.mCount)) > 0) {
        const s32 h = s32((u32)(mRequests.mHead)) + 1;
        mRequests.mHead = u32(h);
        const s32 cap = s32((u32)(mRequests.mCapacity));
        const u32 c = (u32)(mRequests.mCount);
        if (h >= cap) {
            mRequests.mHead = 0;
        }
        mRequests.mCount = c - 1;
    }
    Local<Request> E;
    localEntry(gabi::at<Request>(E.a));
    if (resLoadTask_peekTaskRequest(self, E.a)) {
        return resLoadTask_startTaskRequest(self, E.a, retry);
    }
    mState = STATE_IDLE;
    return 0;
}

/* 0260C43C */
void ResLoadTask::retryLoad(Request *i_entry, u32 retry) {
    const u32 entry = gabi::ea(i_entry);
    if (!resLoadTask_retryArchiveLoad(sResourceManager, entry, (u32)(mCurrentHeap))) {
        return;
    }
    const u32 heap = (u32)(mCurrentHeap);
    if (heap) {
        call_ptr<void>((u32)gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable)->mAdjust, heap);
    }
    finish(i_entry);
    next(i_entry, retry - 1);
}

/* 0260C4CC */
u32 ResLoadTask::startLoad(Request *i_entry, u32 retry) {
    const u32 entry = gabi::ea(i_entry);
    mState = STATE_LOADING;
    if (resLoadTask_isArchiveLoaded(sResourceManager, entry)) {
        mCurrentHeap = 0;
        return 1;
    }
    const u32 parent =
        resLoadTask_getResourceParentHeap(sResourceControl, gabi::ea(&i_entry->mResource), entry);
    call_ptr<void>((u32)gabi::at<SafeStringVtable>(i_entry->mArchive.mVtable)->mAssureTermination,
                   entry);
    Local<SafeString> name;
    gabi::at<SafeString>(name.a)->mVtable = gabi::ea(&sSafeStringVtable);
    gabi::at<SafeString>(name.a)->mStringTop = i_entry->mArchive.mStringTop;
    mCurrentHeap = resLoadTask_createExpHeap(0u, name.a, parent, 1u, 0u);
    const s32 range = s32(sWaitRange) + 1;
    const u32 r = resLoadTask_randomU32(sRandom);
    const u32 wait = u32((u64(r) * u64(u32(range))) >> 32);
    mWaitTimer = wait + u32(s32(sWaitMinimum));
    if ((u8)(i_entry->mIsStage)) {
        return resLoadTask_loadStage(sResourceManager, entry, gabi::ea(&i_entry->mResource),
                                     (u32)(mCurrentHeap));
    }
    if (!(u8)(i_entry->mIsArchive)) {
        return resLoadTask_loadResource(sResourceManager, entry, gabi::ea(&i_entry->mResource),
                                        (u32)(mCurrentHeap));
    }
    if (retry && resLoadTask_canRetryArchive(sResourceManager, entry)) {
        retryLoad(i_entry, retry);
        return 1;
    }
    return resLoadTask_loadArchive(sResourceManager, entry, (u32)(mCurrentHeap));
}

/* 0260C67C */
u32 ResLoadTask::waitLoad(Request *i_entry) {
    const s32 t = s32((u32)(mWaitTimer));
    if (t > 0) {
        mWaitTimer = u32(t - 1);
        return 0;
    }
    if ((u32)gabi::at<ResourceManager>(sResourceManager)->mLoadState == 1) {
        return 0;
    }
    const u32 heap = (u32)(mCurrentHeap);
    if (heap) {
        call_ptr<void>((u32)gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable)->mAdjust, heap);
    }
    if (!(u8)(i_entry->mIsStage)) {
        finish(i_entry);
    }
    return 1;
}

/* 0260C74C */
void ResLoadTask::calc() {
    if (!sResourceControl) {
        return;
    }
    if (((u8)(mFlushUnloads) || !(u32)(mRequests.mCount)) && !(u32)(mState)) {
        for (;;) {
            s32 n = s32((u32)(mUnloads.mCount));
            if (!n) {
                break;
            }
            if (n > 0) {
                s32 i = s32((u32)(mUnloads.mHead));
                const s32 cap = s32((u32)(mUnloads.mCapacity));
                const u32 base = (u32)(mUnloads.mEntries);
                if (i >= cap) {
                    i -= cap;
                }
                unload(gabi::at<Request>(base + u32(i) * sizeof(Request)));
                n = s32((u32)(mUnloads.mCount));
            } else {
                unload(gabi::at<Request>((u32)(mUnloads.mEntries)));
                n = s32((u32)(mUnloads.mCount));
            }
            while (n < 0) {
                unload(gabi::at<Request>((u32)(mUnloads.mEntries)));
                n = s32((u32)(mUnloads.mCount));
            }
            if (!n) {
                break;
            }
            const s32 h = s32((u32)(mUnloads.mHead)) + 1;
            mUnloads.mHead = u32(h);
            const s32 cap = s32((u32)(mUnloads.mCapacity));
            const u32 c = (u32)(mUnloads.mCount);
            if (h >= cap) {
                mUnloads.mHead = 0;
            }
            mUnloads.mCount = c - 1;
        }
        mFlushUnloads = 0;
        return;
    }
    Local<Request> E;
    localEntry(gabi::at<Request>(E.a));
    if (!peekRequest(gabi::at<Request>(E.a))) {
        return;
    }
    const u32 st = (u32)(mState);
    if (!st) {
        startLoad(gabi::at<Request>(E.a), 1);
        return;
    }
    if ((st == STATE_LOADING || st == STATE_WAITING) && waitLoad(gabi::at<Request>(E.a))) {
        next(gabi::at<Request>(E.a), 1);
    }
}

/* frees one ring's storage */
/* inlined into freeRings */
template <class T> static void freeRing(Ring<T> *i_ring) {
    if (!(u32)(i_ring->mEntries)) {
        return;
    }
    const u32 h = resLoadTask_findContainingHeap(sHeapManager, (u32)(i_ring->mEntries));
    call_ptr<void>((u32)gabi::at<HeapVtable>(gabi::at<Heap>(h)->mVtable)->mFree, h,
                   (u32)(i_ring->mEntries));
    i_ring->mCapacity = 0;
    i_ring->mCount = 0;
    i_ring->mEntries = 0;
    i_ring->mHead = 0;
}

/* 0260C940 */
void ResLoadTask::freeRings() {
    freeRing(&mRequests);
    freeRing(&mLoadedIDs);
    freeRing(&mUnloads);
}

/* 0260CA40 */
u32 ResLoadTask::addRef(Request *i_entry) {
    const u32 id = (u32)(i_entry->mID);
    const u32 tree = gabi::ea(&mReferences);
    ReferenceTree *treeReferenceTree = gabi::at<ReferenceTree>(tree);
    const u32 root = (u32)(treeReferenceTree->mRoot);
    Local<u32> key;
    gabi::at<be<u32>>(key.a)[0] = id;
    const u32 node = resLoadTask_findReference(tree, root, key.a);
    ReferenceNode *nodeReferenceNode = gabi::at<ReferenceNode>(node);
    if (node && node + offsetof(ReferenceNode, mReferences)) {
        nodeReferenceNode->mReferences = (u32)(nodeReferenceNode->mReferences) + 1;
        return 1;
    }
    if (s32((u32)(treeReferenceTree->mCount)) < s32((u32)(treeReferenceTree->mCapacity))) {
        const u32 n = (u32)(treeReferenceTree->mFree);
        ReferenceNode *nReferenceNode = gabi::at<ReferenceNode>(n);
        if (n) {
            treeReferenceTree->mFree = (u32)(nReferenceNode->mLeft);
        }
        if (n) {
            nReferenceNode->mLeft = 0;
            nReferenceNode->mID = id;
            nReferenceNode->mIsRed = 1;
            nReferenceNode->mVtable = gabi::ea(&sReferenceNodeVtable);
            nReferenceNode->mKey = id;
            nReferenceNode->mReferences = 1;
            nReferenceNode->mRight = 0;
            nReferenceNode->mTree = tree;
        }
        const u32 cnt = (u32)(treeReferenceTree->mCount) + 1;
        const u32 r = (u32)(treeReferenceTree->mRoot);
        treeReferenceTree->mCount = cnt;
        const u32 newRoot = resLoadTask_insertReference(tree, r, n);
        treeReferenceTree->mRoot = newRoot;
        gabi::at<ReferenceNode>(newRoot)->mIsRed = 0;
        return 0;
    }
    Local<u32> key2;
    gabi::at<be<u32>>(key2.a)[0] = id;
    const u32 n2 = resLoadTask_findReference(tree, root, key2.a);
    if (n2 && n2 + offsetof(ReferenceNode, mReferences)) {
        gabi::at<ReferenceNode>(n2)->mReferences = 1;
    }
    return 0;
}

/* ---- request building and ring insertion (inlined in every request function)
 * ---- */

/* copies a request entry (both strings, id, kind, flag) into a ring slot */
/* inlined into pushBack and pushFront */
static void copyEntry(Request *i_slot, Request *i_src) {
    const u32 slot = gabi::ea(i_slot);
    const u32 src = gabi::ea(i_src);
    copyString(gabi::at<BufferedString>(slot), gabi::at<SafeString>(src));
    copyString(gabi::at<BufferedString>(slot + 0x4C), gabi::at<SafeString>(src + 0x4C));
    i_slot->mID = (u32)(i_src->mID);
    i_slot->mIsArchive = (u8)(i_src->mIsArchive);
    i_slot->mIsStage = (u8)(i_src->mIsStage);
}

/* inlined into requestArchive, requestResource, requestStage and releaseStage */
static void pushBack(RequestRing *i_ring, Request *i_src) {
    const s32 cnt = s32((u32)(i_ring->mCount));
    if (cnt >= s32((u32)(i_ring->mCapacity))) {
        return;
    }
    const s32 h = s32((u32)(i_ring->mHead));
    const s32 cap = s32((u32)(i_ring->mCapacity));
    s32 i = h + cnt;
    const u32 base = (u32)(i_ring->mEntries);
    i_ring->mCount = u32(cnt + 1);
    if (i >= cap) {
        i -= cap;
    }
    copyEntry(gabi::at<Request>(base + u32(i) * sizeof(Request)), i_src);
}

/* inserts at the front of the request ring */
/* inlined into enqueue */
static void pushFront(ResLoadTask *i_self, Request *i_src) {
    const s32 h = s32((u32)(i_self->mRequests.mHead)) - 1;
    const u32 c = (u32)(i_self->mRequests.mCount);
    i_self->mRequests.mHead = u32(h);
    if (h < 0) {
        i_self->mRequests.mHead = (u32)(i_self->mRequests.mCapacity) - 1;
    }
    s32 i = s32((u32)(i_self->mRequests.mHead));
    const s32 cap = s32((u32)(i_self->mRequests.mCapacity));
    i_self->mRequests.mCount = c + 1;
    if (i >= cap) {
        i -= cap;
    }
    copyEntry(gabi::at<Request>((u32)(i_self->mRequests.mEntries) + u32(i) * sizeof(Request)),
              i_src);
}

// HD: The matcher mislabeled this two-argument HashCRC32 entry as a resource lookup.
/* inlined into requestArchive, requestResource, requestStage and the release/search methods */
static u32 hashString(SafeString *i_s) {
    const u32 s = gabi::ea(i_s);
    call_ptr<void>((u32)gabi::at<SafeStringVtable>(i_s->mVtable)->mAssureTermination, s);
    const u32 fn = (u32)gabi::at<SafeStringVtable>(i_s->mVtable)->mAssureTermination;
    const u32 top = (u32)(i_s->mStringTop);
    call_ptr<void>(fn, s);
    const s32 len = cstrLength(gabi::at<be<u8>>((u32)(i_s->mStringTop)));
    return resLoadTask_hashCRC32(top, len);
}

/* hash of "<arc>_<name>" formatted into the FixedSafeString<128> at s */
/* inlined into requestResource, requestStage, releaseResource and releaseStage */
static u32 hashPair(FixedString128 *i_s, be<u8> *i_fmt, SafeString *i_arc, SafeString *i_name) {
    const u32 s = gabi::ea(i_s);
    const u32 fmt = gabi::ea(i_fmt);
    const u32 arc = gabi::ea(i_arc);
    const u32 name = gabi::ea(i_name);
    i_s->mBuffer[127] = 0;
    i_s->mBuffer[0] = 0;
    i_s->mCapacity = 0x80;
    i_s->mStringTop = s + 0xC;
    i_s->mVtable = gabi::ea(&sFixed128Vtable);
    call_ptr<void>((u32)gabi::at<SafeStringVtable>(i_arc->mVtable)->mAssureTermination, arc);
    const u32 fn = (u32)gabi::at<SafeStringVtable>(i_name->mVtable)->mAssureTermination;
    const u32 arcTop = (u32)(i_arc->mStringTop);
    call_ptr<void>(fn, name);
    resLoadTask_formatString(s, fmt, arcTop, (u32)(i_name->mStringTop));
    return hashString(gabi::at<SafeString>(s));
}

/* first string of a stack request entry, copied from src */
/* inlined into request/release methods */
static void entryFirst(Request *i_e, SafeString *i_src) {
    const u32 e = gabi::ea(i_e);
    i_e->mArchive.mBuffer[63] = 0;
    i_e->mArchive.mCapacity = 0x40;
    i_e->mArchive.mVtable = gabi::ea(&sFixed64BaseVtable);
    i_e->mArchive.mStringTop = e + 0xC;
    copyString(gabi::at<BufferedString>(e), i_src);
}

/* second string of a stack request entry, copied from src */
/* inlined into request/release methods */
static void entrySecond(Request *i_e, SafeString *i_src) {
    const u32 e = gabi::ea(i_e);
    i_e->mResource.mVtable = gabi::ea(&sFixed64BaseVtable);
    i_e->mResource.mBuffer[63] = 0;
    i_e->mArchive.mVtable = gabi::ea(&sFixed64Vtable);
    i_e->mResource.mStringTop = e + 0x58;
    i_e->mResource.mCapacity = 0x40;
    copyString(gabi::at<BufferedString>(e + 0x4C), i_src);
}

/* second string from the global empty-name SafeString 104A0CD8 (vtable read
 * once) */
/* inlined into requestArchive and releaseArchive */
static void entrySecondGlobal(Request *i_e) {
    const u32 e = gabi::ea(i_e);
    const u32 g = gabi::ea(&sEmptyName);
    SafeString *gSafeString = gabi::at<SafeString>(g);
    i_e->mResource.mBuffer[63] = 0;
    i_e->mArchive.mVtable = gabi::ea(&sFixed64Vtable);
    i_e->mResource.mStringTop = e + 0x58;
    i_e->mResource.mCapacity = 0x40;
    i_e->mResource.mVtable = gabi::ea(&sFixed64BaseVtable);
    const u32 vt = (u32)(gSafeString->mVtable);
    SafeStringVtable *vtSafeStringVtable = gabi::at<SafeStringVtable>(vt);
    call_ptr<void>((u32)vtSafeStringVtable->mAssureTermination, g);
    const u32 top = (u32)(gSafeString->mStringTop);
    s32 n = cstrLength(gabi::at<be<u8>>(top));
    const s32 size = s32((u32)(i_e->mResource.mCapacity));
    const u32 fn = (u32)vtSafeStringVtable->mAssureTermination;
    if (n >= size) {
        n = size - 1;
    }
    call_ptr<void>(fn, g);
    resLoadTask_moveBytes(e + 0x58, top, u32(n), 0u);
    i_e->mResource.mBuffer[n] = 0;
}

/* inlined into request/release methods */
static void entryTail(Request *i_e, u32 id, u32 kind, u32 flag) {
    i_e->mID = id;
    i_e->mIsArchive = u8(kind);
    i_e->mResource.mVtable = gabi::ea(&sFixed64Vtable);
    i_e->mIsStage = u8(flag);
}

/* reference-counted insertion: front requests go behind the request being
 * loaded */
/* inlined into requestArchive, requestResource and requestStage */
static void enqueue(ResLoadTask *i_self, Request *i_L, u32 front) {
    if (!front) {
        pushBack(&i_self->mRequests, i_L);
        return;
    }
    s32 cnt = s32((u32)(i_self->mRequests.mCount));
    Local<Request> TT;
    const u32 T = TT.a;
    localEntry(gabi::at<Request>(T));
    if (cnt > 0) {
        s32 i = s32((u32)(i_self->mRequests.mHead));
        const s32 cap = s32((u32)(i_self->mRequests.mCapacity));
        const u32 base = (u32)(i_self->mRequests.mEntries);
        if (i >= cap) {
            i -= cap;
        }
        const u32 e = base + u32(i) * sizeof(Request);
        copyString(gabi::at<BufferedString>(T), gabi::at<SafeString>(e));
        copyString(gabi::at<BufferedString>(T + 0x4C), gabi::at<SafeString>(e + 0x4C));
        gabi::at<Request>(T)->mID = (u32)(gabi::at<Request>(e)->mID);
        const s32 h = s32((u32)(i_self->mRequests.mHead)) + 1;
        const s32 c2 = s32((u32)(i_self->mRequests.mCapacity));
        gabi::at<Request>(T)->mIsArchive = (u8)(gabi::at<Request>(e)->mIsArchive);
        i_self->mRequests.mHead = u32(h);
        const s32 c = s32((u32)(i_self->mRequests.mCount));
        gabi::at<Request>(T)->mIsStage = (u8)(gabi::at<Request>(e)->mIsStage);
        if (h >= c2) {
            i_self->mRequests.mHead = 0;
        }
        cnt = c - 1;
        i_self->mRequests.mCount = u32(cnt);
    } else if (cnt == 0) {
        if (s32((u32)(i_self->mRequests.mCapacity)) > 0) {
            pushFront(i_self, i_L);
        }
        return;
    }
    if (cnt >= s32((u32)(i_self->mRequests.mCapacity))) {
        return;
    }
    pushFront(i_self, i_L);
    if (s32((u32)(i_self->mRequests.mCount)) >= s32((u32)(i_self->mRequests.mCapacity))) {
        return;
    }
    pushFront(i_self, gabi::at<Request>(T));
}

/* 0260CBB4 */
u32 ResLoadTask::requestArchive(SafeString *i_name, u32 front) {
    const u32 self = gabi::ea(this);
    if (s32((u32)(mRequests.mCount)) >= s32((u32)(mRequests.mCapacity))) {
        return 0;
    }
    const u32 h = hashString(i_name);
    Local<Request> LL;
    const u32 L = LL.a;
    entryFirst(gabi::at<Request>(L), i_name);
    entrySecondGlobal(gabi::at<Request>(L));
    entryTail(gabi::at<Request>(L), h, 1, 0);
    if (addRef(gabi::at<Request>(L))) {
        return 1;
    }
    enqueue(gabi::at<ResLoadTask>(self), gabi::at<Request>(L), front);
    return 1;
}

/* 0260D618 */
u32 ResLoadTask::requestResource(SafeString *i_arc, SafeString *i_name, u32 front) {
    const u32 self = gabi::ea(this);
    if (s32((u32)(mRequests.mCount)) >= s32((u32)(mRequests.mCapacity))) {
        return 0;
    }
    Local<FixedStringFrame> S;
    const u32 h = hashPair(gabi::at<FixedString128>(S.a),
                           gabi::at<be<u8>>(gabi::guest_addr("%s_%s")), i_arc, i_name);
    Local<Request> LL;
    const u32 L = LL.a;
    entryFirst(gabi::at<Request>(L), i_arc);
    entrySecond(gabi::at<Request>(L), i_name);
    entryTail(gabi::at<Request>(L), h, 0, 0);
    if (addRef(gabi::at<Request>(L))) {
        return 1;
    }
    enqueue(gabi::at<ResLoadTask>(self), gabi::at<Request>(L), front);
    return 1;
}

/* 0260E0DC */
u32 ResLoadTask::requestStage(SafeString *i_arc, SafeString *i_name) {
    const u32 self = gabi::ea(this);
    if (s32((u32)(mRequests.mCount)) >= s32((u32)(mRequests.mCapacity))) {
        return 0;
    }
    Local<FixedStringFrame> S;
    const u32 h = hashPair(gabi::at<FixedString128>(S.a),
                           gabi::at<be<u8>>(gabi::guest_addr("%s_%s")), i_arc, i_name);
    Local<Request> LL;
    const u32 L = LL.a;
    entryFirst(gabi::at<Request>(L), i_arc);
    entrySecond(gabi::at<Request>(L), i_name);
    entryTail(gabi::at<Request>(L), h, 0, 1);
    pushBack(&gabi::at<ResLoadTask>(self)->mRequests, gabi::at<Request>(L));
    return 1;
}

/* 0260E500 */
void ResLoadTask::releaseArchive(SafeString *i_name) {
    const u32 h = hashString(i_name);
    Local<Request> LL;
    const u32 L = LL.a;
    Request *LRequest = gabi::at<Request>(L);
    entryFirst(LRequest, i_name);
    entrySecondGlobal(LRequest);
    entryTail(LRequest, h, 1, 0);
    if (release(LRequest)) {
        return;
    }
    pushBack(&mUnloads, LRequest);
}

/* 0260E8A8 */
void ResLoadTask::releaseResource(SafeString *i_arc, SafeString *i_name) {
    const u32 name = gabi::ea(i_name);
    Local<FixedStringFrame> S;
    const u32 h = hashPair(gabi::at<FixedString128>(S.a),
                           gabi::at<be<u8>>(gabi::guest_addr("%s_%s")), i_arc, i_name);
    Local<ReleaseResourceFrame> F;
    const u32 T = F.a, L = F.a + 8;
    SafeString *TSafeString = gabi::at<SafeString>(T);
    entryFirst(gabi::at<Request>(L), i_arc);
    entrySecond(gabi::at<Request>(L), i_name);
    gabi::at<Request>(L)->mResource.mVtable = gabi::ea(&sFixed64Vtable);
    TSafeString->mStringTop = gabi::guest_addr("Stage");
    gabi::at<Request>(L)->mID = h;
    gabi::at<Request>(L)->mIsArchive = 0;
    gabi::at<Request>(L)->mIsStage = 0;
    TSafeString->mVtable = gabi::ea(&sSafeStringVtable);
    call_ptr<void>((u32)gabi::at<SafeStringVtable>(i_name->mVtable)->mAssureTermination, name);
    call_ptr<void>((u32)gabi::at<SafeStringVtable>(i_name->mVtable)->mAssureTermination, name);
    const u32 fn = (u32)gabi::at<SafeStringVtable>(TSafeString->mVtable)->mAssureTermination;
    const u32 top = (u32)(i_name->mStringTop);
    call_ptr<void>(fn, T);
    bool stage = top == (u32)(TSafeString->mStringTop);
    if (!stage) {
        u32 a = (u32)(i_name->mStringTop), b = (u32)(TSafeString->mStringTop);
        bool decided = false;
        for (u32 i = 0; i < 0x40001; i++) {
            const u32 x = (u8)gabi::at<be<u8>>(a)[i], y = (u8)gabi::at<be<u8>>(b)[i];
            if (x != y) {
                decided = true;
                break;
            }
            if (!x) {
                stage = true;
                decided = true;
                break;
            }
        }
        (void)decided;
    }
    if (stage) {
        const u32 cnt = (u32)(mUnloads.mCount);
        mFlushUnloads = 1;
        if (cnt) {
            pushBack(&mUnloads, gabi::at<Request>(L));
        }
    }
    if (release(gabi::at<Request>(L))) {
        return;
    }
    pushBack(&mUnloads, gabi::at<Request>(L));
}

/* 0260EEE8 */
void ResLoadTask::releaseStage(SafeString *i_arc, SafeString *i_name) {
    Local<FixedStringFrame> S;
    const u32 h = hashPair(gabi::at<FixedString128>(S.a),
                           gabi::at<be<u8>>(gabi::guest_addr("%s_%s")), i_arc, i_name);
    Local<Request> LL;
    const u32 L = LL.a;
    Request *LRequest = gabi::at<Request>(L);
    entryFirst(LRequest, i_arc);
    entrySecond(LRequest, i_name);
    const u32 st = (u32)(mState);
    entryTail(LRequest, h, 0, 1);
    if (!st) {
        unload(LRequest);
        return;
    }
    pushBack(&mUnloads, LRequest);
}

/* 0260F30C */
u32 ResLoadTask::isLoadedId(u32 id) {
    const u32 cnt = (u32)(mLoadedIDs.mCount);
    for (u32 i = 0; i != cnt; i++) {
        const s32 h = s32((u32)(mLoadedIDs.mHead));
        const s32 cap = s32((u32)(mLoadedIDs.mCapacity));
        s32 j = h + s32(i);
        const u32 base = (u32)(mLoadedIDs.mEntries);
        if (j >= cap) {
            j -= cap;
        }
        if ((u32)gabi::at<be<u32>>(base)[u32(j)] == id) {
            return 1;
        }
    }
    return 0;
}

/* 0260F374 */
u32 ResLoadTask::isPendingId(u32 id) {
    if (isLoadedId(id)) {
        return 0;
    }
    const u32 cnt = (u32)(mRequests.mCount);
    for (u32 i = 0; i != cnt; i++) {
        s32 j = s32((u32)(mRequests.mHead)) + s32(i);
        const s32 cap = s32((u32)(mRequests.mCapacity));
        if (j >= cap) {
            j -= cap;
        }
        if ((u32)gabi::at<Request>(mRequests.mEntries)[u32(j)].mID == id) {
            return 1;
        }
    }
    return 0;
}

/* 0260F414 */
u32 ResLoadTask::isPendingArchive(SafeString *i_name) { return isPendingId(hashString(i_name)); }

/* 0260F4CC */
u32 ResLoadTask::isPendingResource(SafeString *i_arc, SafeString *i_name) {
    const u32 arc = gabi::ea(i_arc);
    const u32 name = gabi::ea(i_name);
    Local<FixedStringFrame> S;
    const u32 s = S.a;
    FixedString128 *sFixedString128 = gabi::at<FixedString128>(s);
    sFixedString128->mStringTop = s + 0xC;
    sFixedString128->mCapacity = 0x80;
    sFixedString128->mVtable = gabi::ea(&sFixed128Vtable);
    sFixedString128->mBuffer[127] = 0;
    sFixedString128->mBuffer[0] = 0;
    call_ptr<void>((u32)gabi::at<SafeStringVtable>(i_arc->mVtable)->mAssureTermination, arc);
    const u32 fn = (u32)gabi::at<SafeStringVtable>(i_name->mVtable)->mAssureTermination;
    const u32 arcTop = (u32)(i_arc->mStringTop);
    call_ptr<void>(fn, name);
    resLoadTask_formatString(s, gabi::guest_addr("%s_%s"), arcTop, (u32)(i_name->mStringTop));
    return isPendingId(hashString(gabi::at<SafeString>(s)));
}

/* 0260F61C */
void ResLoadTask::forgetId(u32 id) {
    const s32 cnt = s32((u32)(mLoadedIDs.mCount));
    for (u32 i = 0; i != u32(cnt); i++) {
        const s32 h = s32((u32)(mLoadedIDs.mHead));
        const s32 cap = s32((u32)(mLoadedIDs.mCapacity));
        s32 j = h + s32(i);
        const u32 base = (u32)(mLoadedIDs.mEntries);
        if (j >= cap) {
            j -= cap;
        }
        if ((u32)gabi::at<be<u32>>(base)[u32(j)] != id) {
            continue;
        }
        if (cnt <= 0 || s32(i) < 0 || s32(i) >= cnt) {
            break;
        }
        s32 last = cnt - 1;
        if (s32(i) < last) {
            s32 k = s32(i), m = s32(i) + 1;
            do {
                const s32 hh = s32((u32)(mLoadedIDs.mHead));
                const s32 cc = s32((u32)(mLoadedIDs.mCapacity));
                s32 a = hh + k;
                if (a >= cc) {
                    a -= cc;
                }
                s32 b = hh + m;
                const u32 bb = (u32)(mLoadedIDs.mEntries);
                if (b >= cc) {
                    b -= cc;
                }
                gabi::at<be<u32>>(bb)[u32(a)] = gabi::at<be<u32>>(bb)[u32(b)];
                const s32 c = s32((u32)(mLoadedIDs.mCount));
                k++;
                last = c - 1;
                m++;
            } while (k < last);
        }
        mLoadedIDs.mCount = u32(last);
        break;
    }
    const u32 root = (u32)(mReferences.mRoot);
    Local<u32[2]> keys;
    gabi::at<be<u32>>(keys.a)[0] = id;
    gabi::at<be<u32>>(keys.a)[1] = id;
    const u32 n = resLoadTask_findReference(gabi::ea(&mReferences), root, keys.a + 4);
    if (!n || !(n + 0x18)) {
        return;
    }
    const u32 r = resLoadTask_eraseReference(gabi::ea(&mReferences), root, keys.a);
    mReferences.mRoot = r;
    if (r) {
        gabi::at<ReferenceNode>(r)->mIsRed = 0;
    }
}

// HD: Compiler-generated and template code of this translation unit.
/* 0260F7A8 */
void __sinit_hd_res_load_task_cpp() {
    sEmptyRing.mHead = 0;
    sEmptyRing.mEntries = 0;
    sEmptyRing.mCount = 0;
    sEmptyRing.mCapacity = 0;
    resLoadTask_registerGlobalObject(gabi::ea(&sTaskGlobalDestructor));
    const f32 lo = sInitialLow, hi = sInitialHigh;
    sDefaultLow = lo;
    sDefaultHigh = hi;
    resLoadTask_constructHeaderObjectA(gabi::ea(&sHeaderObjectA));
    resLoadTask_registerGlobalObject(gabi::ea(&sHeaderAGlobalDestructor));
    resLoadTask_constructHeaderObjectB(gabi::ea(&sHeaderObjectB));
    resLoadTask_registerGlobalObject(gabi::ea(&sHeaderBGlobalDestructor));
}

/* 0260F850 */
// HD: GHS deleting destructor keeps its two-bit deletion mode.
void ResLoadTask__dt(ResLoadTask *i_this, u32 flags) {
    const u32 p = gabi::ea(i_this);
    ResLoadTask *pResLoadTask = gabi::at<ResLoadTask>(p);
    if (!p) {
        return;
    }
    pResLoadTask->mTaskVtable = gabi::ea(&sTaskVtable);
    resLoadTask_destroySingletonDisposer(gabi::ea(&pResLoadTask->mIsInstance), 2u);
    resLoadTask_destroyTaskBase(p, 0u);
    if (flags & 1) {
        resLoadTask_deleteObject(p);
    }
}

/* 0260F8EC */
u32 ResLoadTask::isDerived(const be<u32> *i_info) {
    const u32 info = gabi::ea(i_info);
    if (!sTaskTypeInitialized) {
        sTaskTypeInitialized = 1;
        sTaskTypeInfo = gabi::ea(&sTaskTypeName);
    }
    if (info == gabi::ea(&sTaskTypeInfo)) {
        return 1;
    }
    if (!sBaseTypeInitialized) {
        sBaseTypeInitialized = 1;
        sBaseTypeInfo = gabi::ea(&sBaseTypeName);
    }
    return info == gabi::ea(&sBaseTypeInfo);
}

// HD: GHS deleting-destructor thunks adjust the second base before dispatch.
/* 0260F9D4 */
void archiveRequestSecondBase__dt(MappedObject *i_this, u32 flags) {
    const u32 p = gabi::ea(i_this);
    resLoadTask_destroyArchiveRequestThunkTarget(p - 0x10, flags);
}
/* 0260F9DC */
void resourceRequestSecondBase__dt(MappedObject *i_this, u32 flags) {
    const u32 p = gabi::ea(i_this);
    resLoadTask_destroyResourceRequestThunkTarget(p - 0x10, flags);
}

} // namespace hd_res_load_task
using namespace hd_res_load_task;

// clang-format off
/* WWHD glue
ext 0261390C void resLoadTask_destroyResourceRequestThunkTarget(u32, u32)
ext 026138AC void resLoadTask_destroyArchiveRequestThunkTarget(u32, u32)
ext 0273AF40 void resLoadTask_deleteObject(u32)
ext 027452C8 void resLoadTask_destroyTaskBase(u32, u32)
ext 0260B770 void resLoadTask_destroySingletonDisposer(u32, u32)
ext 028F026C void resLoadTask_registerGlobalObject(u32)
ext 028EAB2C void resLoadTask_constructHeaderObjectB(u32)
ext 028ED6F8 void resLoadTask_constructHeaderObjectA(u32)
ext 0260AEFC u32 resLoadTask_eraseReference(u32, u32, u32)
ext 0260ACC8 u32 resLoadTask_findReference(u32, u32, u32)
ext 02759C28 void resLoadTask_formatString(u32, u32, u32, u32)
ext C0009988 void resLoadTask_moveBytes(u32, u32, u32, u32)
ext 0273B264 u32 resLoadTask_hashCRC32(u32, u32)
ext 0260AD08 u32 resLoadTask_insertReference(u32, u32, u32)
ext 02755FEC u32 resLoadTask_findContainingHeap(u32, u32)
ext 026127D4 u32 resLoadTask_loadArchive(u32, u32, u32)
ext 02612540 u32 resLoadTask_canRetryArchive(u32, u32)
ext 02612908 u32 resLoadTask_loadResource(u32, u32, u32, u32)
ext 02612AE8 u32 resLoadTask_loadStage(u32, u32, u32, u32)
ext 0275CEE4 u32 resLoadTask_randomU32(u32)
ext 02754B38 u32 resLoadTask_createExpHeap(u32, u32, u32, u32, u32)
ext 02607F80 u32 resLoadTask_getResourceParentHeap(u32, u32, u32)
ext 02612504 u32 resLoadTask_isArchiveLoaded(u32, u32)
ext 026105C4 u32 resLoadTask_retryArchiveLoad(u32, u32, u32)
ext 0260C4CC u32 resLoadTask_startTaskRequest(u32, u32, u32)
ext 0260C0A4 u32 resLoadTask_peekTaskRequest(u32, u32)
ext 0260623C void resLoadTask_registerLoadedResource(u32, u32, u32, u32)
ext 02604EB4 void resLoadTask_registerLoadedArchive(u32, u32, u32)
ext 026126C0 u32 resLoadTask_unloadStage(u32, u32)
ext 0260757C void resLoadTask_deleteResource(u32, u32, u32)
ext 02607138 void resLoadTask_deleteArchive(u32, u32)
ext 0203106C void resLoadTask_initializeLoadNotification()
ext 02031048 void resLoadTask_initializeLoadSignals()
ext 0203E8F4 u32 resLoadTask_getTaskParentHeap()
ext 02612E64 void resLoadTask_registerArchive(u32, u32, u32, u32, u32, u32, u32)
ext 0203E8D0 u32 resLoadTask_getTextureParentHeap()
ext 027199CC void resLoadTask_initializeJpegManager(u32)
ext 025F4A60 void resLoadTask_attachMessageHeap(u32, u32)
ext 025F3A14 void resLoadTask_createMessageManager(u32)
ext 02753004 u32 resLoadTask_createFrameHeap(u32, u32, u32, u32, u32)
ext 025F33A4 void resLoadTask_attachFontHeap(u32, u32)
ext 025F2FE0 void resLoadTask_createFontManager(u32)
ext 02610CF8 void resLoadTask_loadPermanentPack(u32, u32, u32)
ext 02612BE0 u32 resLoadTask_getPermanentPackName(u32)
ext 02611864 void resLoadTask_loadPermanentArchive(u32, u32, u32)
ext 027488E8 void resLoadTask_prepareTaskBase(u32)
ext 02756140 u32 resLoadTask_getCurrentHeap(u32)
ext 0273AD10 u32 resLoadTask_allocateObject(u32)
ext 027454E0 void resLoadTask_constructTaskBase(u32, u32, u32)
ext 0260F850 void resLoadTask_destroyTask(u32, u32)
data sInstance 101F4F54
data sResourceManager 101F4F7C
data sResourceControl 101F4F28
data sHeapManager 101F8B4C
data sHeapEnvironment 101F7274
data sFontManager 101F4A50
data sMessageManager 101F4AE8
data sJpegManager 101F8378
data sRandom 101F8B60
data sWaitRange 1047C768
data sWaitMinimum 1047C76A
data sEmptyRing 1048DD3C
data sDefaultLow 1048DD30
data sDefaultHigh 1048DD34
data sInitialLow 100E1FC8
data sInitialHigh 100E1FCC
data sTaskTypeInitialized 101FD598
data sBaseTypeInitialized 101FD594
data sTaskTypeInfo 101FD97C
data sBaseTypeInfo 101FD980
str 100E1EA4 in 0260B7C4
str 100E1F40 in 0260B91C
str 100E1EC4 in 0260B91C
str 100E1F58 in 0260B91C
str 100E1EE4 in 0260B91C
str 100E1F64 in 0260B91C
str 100E1EFC in 0260B91C
str 100E1F74 in 0260B91C
str 100E1F20 in 0260B91C
str 100E1F30 in 0260B91C
str 100E1EB0 in 0260B91C
str 100E1F80 in 0260B91C
str 100E1EB8 in 0260B91C
str 100E1F90 in 0260BFC8
str 100E1F98 in 0260D618
str 100E1FA0 in 0260E0DC
str 100E1FA8 in 0260E8A8
str 100E1FB0 in 0260E8A8
str 100E1FB8 in 0260EEE8
str 100E1FC0 in 0260F4CC
data sSafeStringVtable 100E1DD4
data sFixed64Vtable 100E1E3C
data sTaskSecondVtable 100E1FD0
data sTaskVtable 100E2090
data sBufferedStringVtable 100E1DEC
data sFixed64BaseVtable 100E1E24
data sFixed128Vtable 100E1E6C
data sReferenceNodeVtable 100E1E84
data sTaskTypeName 100E1E14
data sBaseTypeName 100E1E04
data sTaskGlobalDestructor 101F4F30
data sHeaderAGlobalDestructor 101F4F3C
data sHeaderBGlobalDestructor 101F4F48
data sHeaderObjectA 1048DD38
data sHeaderObjectB 1048DD39
data sEmptyName 104A0CD8
*/
