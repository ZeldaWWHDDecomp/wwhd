// Timing query rotation and a rolling, six-component load history.
#include "agl/TimingHistory_l.h"
#include "sead/HeapRegistry.h"
#include "agl/ShaderHolder.h"
using namespace agl_history_l;
void* allocateTimingObject(u32);
void releaseTimingObject(void*);
void* allocateHistoryOnHeap(u32,void*,s32);
void* allocateHistoryArray(u32,void*,s32);
void initializeHistoryAllocation(void*,u32);
void registerHistoryDisposer(sead::Disposer*,void*,u32);
void destroyHistoryDisposer(sead::Disposer*,u32);
void* selectHistoryHeap(sead::RegistryManagerState*);
void setHistoryMemory(void*,u32,u32);
void flushHistoryMemory(void*,u32);
void sampleHistoryTopTimestamp(void*);
void sampleHistoryBottomTimestamp(void*);
Pair32 readHistoryQuery(void*);
f64 convertHistoryTicks(u32,u32);
static sead::DisposerVtable sShaderHolderDisposerTable;
static sead::DisposerVtable sTimingDisposerTable;
static OwnerDispatchPrefix sTimingTable;

/* 0278B910 */
static void destroyShaderHolderDisposer(sead::Disposer* disposer, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (disposer == NULL) return;
    disposer->mVtable = &sShaderHolderDisposerTable;
    if (disposer == sShaderHolderDisposer) {
        agl::detail::ShaderHolder* holder = sShaderHolder;
        sShaderHolderDisposer = NULL;
        gabi::call_ptr<void>(holder->mVtable->mDestroy.mFunction,holder,2);
        sShaderHolder = NULL;
    }
    destroyHistoryDisposer(disposer,0);
    if (flags & 1) releaseTimingObject(disposer);
}
/* 0278BBA8 */
static TimingHistory* createTimingHistory(TimingHistory* timing) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (timing == NULL) timing = (TimingHistory*)allocateTimingObject(sizeof(*timing));
    if (timing != NULL) {
        timing->mVtable = &sTimingTable;
        be<u32>* flags = memberAt<be<u32>>(timing,0x14);
        if (flags == NULL) flags = (be<u32>*)allocateTimingObject(4);
        if (flags != NULL) *flags = 0;
        if (memberAt<void>(timing,0x18) == NULL) allocateTimingObject(12);
        timing->mPreset = 95.0f;
        HistoryStorage* history = memberAt<HistoryStorage>(timing,0x30);
        if (history == NULL) history = (HistoryStorage*)allocateTimingObject(sizeof(*history));
        if (history != NULL) {
            history->mCapacity = 0;
            history->mHead = 0;
            history->mSamples = NULL;
            history->mCount = 0;
        }
        timing->mPresetScalar = 2.0f;
        be<f32>* adjustment = memberAt<be<f32>>(timing,0x44);
        if (adjustment == NULL) adjustment = (be<f32>*)allocateTimingObject(8);
        if (adjustment != NULL) {
            adjustment[0] = 0.03f;
            adjustment[1] = 0.05f;
        }
    }
    return timing;
}
/* 0278BC9C */
static void createTimingSingleton(void* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (sTimingOwner != NULL) return;
    TimingHistory* timing = (TimingHistory*)allocateHistoryOnHeap(sizeof(TimingHistory),heap,4);
    sead::Disposer* disposer = memberAt<sead::Disposer>(timing,4);
    if (disposer != NULL) {
        registerHistoryDisposer(disposer,heap,3);
        disposer->mVtable = &sTimingDisposerTable;
    }
    sTimingDisposer = disposer;
    if (timing != NULL) timing = createTimingHistory(timing);
    sTimingOwner = timing;
}
/* inlined into agl_history_l::QueryRing::initialize and agl_history_l::TimingHistory::initialize */
static void* heapAllocate(void* heap,u32 size,s32 alignment) {
    sead::RegistryHeap* typedHeap = (sead::RegistryHeap*)heap;
    return gabi::call_ptr<void*>(typedHeap->dispatch()->mAllocate.mFunction,heap,size,alignment);
}
/* 0278BD3C */
void agl_history_l::QueryRing::initialize(void* heap) {
    QueryRing* query = this;
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (heap == NULL) heap = selectHistoryHeap(sRegistryManager);
    void* data = heapAllocate(heap,24,32);
    if (data == NULL) {
        u32 size = u32(query->mCount) << 3;
        void* existing = query->mData;
        setHistoryMemory(existing,0,size);
    } else {
        query->mCount = 3;
        query->mData = (be<u64>*)data;
        setHistoryMemory(data,0,24);
    }
    u32 size = u32(query->mCount) << 3;
    void* current = query->mData;
    flushHistoryMemory(current,size);
    query->mFlags = 0;
}
/* 0278BDF8 */
void agl_history_l::TimingHistory::initialize(void* heap) {
    TimingHistory* timing = this;
    auto frame = gabi::AutoFrame(gabi::current_function());
    timing->mFlags = 1;
    for (u32 i = 0; i < 3; ++i) {
        QueryRing* query = (QueryRing*)allocateHistoryArray(sizeof(QueryRing),heap,4);
        if (query != NULL) {
            initializeHistoryAllocation(query,sizeof(QueryRing));
            query->mData = NULL;
            query->mCount = 0;
            be<u32>* flags = memberAt<be<u32>>(query,8);
            if (flags == NULL) flags = (be<u32>*)allocateTimingObject(4);
            if (flags != NULL) *flags = 0;
        }
        timing->mQueries[i] = query;
        query->initialize(heap);
    }
    timing->mElapsed = 0.0f;
    timing->mLoad = 0.0f;
    if (heap == NULL) heap = selectHistoryHeap(sRegistryManager);
    Sample* samples = (Sample*)heapAllocate(heap,20*sizeof(Sample),4);
    if (samples != NULL) {
        timing->mHistory.mSamples = samples;
        timing->mHistory.mCapacity = 20;
    }
    timing->mHistory.mCount = 0;
    timing->mHistory.mHead = 0;
}
// A ring insertion subtracts its capacity once; it is not a general modulo operation.
/* inlined into agl_history_l::TimingHistory::update */
static void prependSample(HistoryStorage* history,Sample* sample) {
    s32 head = u32(history->mHead) - 1;
    s32 count = history->mCount;
    history->mHead = head;
    if (head < 0) history->mHead = u32(history->mCapacity)-1;
    s32 capacity = history->mCapacity;
    head = history->mHead;
    history->mCount = u32(count)+1;
    if (head >= capacity) head = u32(head)-u32(capacity);
    // A word copy, so stored NaN payloads move unchanged.
    Sample* samples = history->mSamples;
    be<u32>* destination = (be<u32>*)&samples[u32(head)];
    be<u32>* source = (be<u32>*)sample;
    for (u32 i=0; i<6; ++i) destination[i] = u32(source[i]);
}
/* inlined into agl_history_l::TimingHistory::update */
static Sample* previousSample(HistoryStorage* history,u32 offset,s32 capacity) {
    s32 index = u32(history->mHead)+offset;
    if (index >= capacity) index = u32(index)-u32(capacity);
    Sample* samples = history->mSamples;
    return &samples[u32(index)];
}
/* 0278BF5C */
void agl_history_l::TimingHistory::update() {
    TimingHistory* timing = this;
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!(u32(timing->mFlags)&1)) {
        timing->mLoad = 0.0f;
        timing->mElapsed = 0.0f;
        return;
    }
    QueryRing* oldQuery = timing->mQueries[0];
    if ((u32(oldQuery->mFlags)&3)==3) {
        u32 queryCount = oldQuery->mCount;
        be<u64>* start = oldQuery->mData;
        be<u64>* end = start;
        if (queryCount > 2) end = memberAt<be<u64>>(end,16);
        if (queryCount > 1) start = memberAt<be<u64>>(start,8);
        auto endTicks = readHistoryQuery(end);
        auto startTicks = readHistoryQuery(start);
        u64 elapsed = ((u64(endTicks.r3)<<32)|endTicks.r4)-((u64(startTicks.r3)<<32)|startTicks.r4);
        f64 ticks = convertHistoryTicks(u32(elapsed>>32),u32(elapsed));
        const f64 tickScale = f64(0.03703703731298447f);
        f32 elapsedTime = f32(gabi::ppc_nan2(ticks,tickScale,ticks*tickScale,gabi::ppc_dnan64()));
        f32 scaled = gabi::fmuls_ppc(100.0f,elapsedTime);
        f32 load = gabi::fdivs_ppc(scaled,16666.666015625f);
        timing->mElapsed = elapsedTime;
        timing->mLoad = load;
        oldQuery->mFlags = 0;
        timing->mFlags = u32(timing->mFlags)|2;
    } else {
        oldQuery->mFlags = 0;
        timing->mFlags = u32(timing->mFlags)&~2u;
    }
    QueryRing* middle = timing->mQueries[1];
    QueryRing* newest = timing->mQueries[2];
    timing->mQueries[0] = middle;
    timing->mQueries[1] = newest;
    timing->mQueries[2] = oldQuery;
    HistoryStorage* history = &timing->mHistory;
    s32 count = history->mCount;
    f32 value = timing->mLoad;
    f32 meanThree = value;
    if (count < 3) {
        gabi::FrameLocal<Sample> sample(8);
        s32 capacity = history->mCapacity;
        sample->mForecast = value;
        sample->mDelta = 0.0f;
        sample->mAcceleration = 0.0f;
        sample->mMeanThree = value;
        sample->mValue = meanThree;
        sample->mMeanFive = value;
        if (count < capacity) prependSample(history,sample);
        else if (count > 0) {
            capacity = history->mCapacity;
            count = u32(count)-1;
            history->mCount = count;
            if (count < capacity) prependSample(history,sample);
        }
        return;
    }
    s32 capacity = history->mCapacity;
    f32 meanFive = value;
    if (count > 4) {
        for (u32 i=0; i<2; ++i) meanThree = gabi::fadds_ppc(meanThree,f32(previousSample(history,i,capacity)->mValue));
        meanThree = gabi::fdivs_ppc(meanThree,3.0f);
        for (u32 i=0; i<4; ++i) meanFive = gabi::fadds_ppc(meanFive,f32(previousSample(history,i,capacity)->mValue));
        meanFive = gabi::fdivs_ppc(meanFive,5.0f);
    }
    Sample* previous = previousSample(history,0,capacity);
    gabi::FrameLocal<Sample> sample(0x20);
    sample->mValue = value;
    sample->mMeanThree = meanThree;
    sample->mMeanFive = meanFive;
    f32 delta = gabi::fsubs_ppc(meanThree,f32(previous->mMeanThree));
    sample->mDelta = delta;
    f32 acceleration = gabi::fsubs_ppc(delta,f32(previous->mDelta));
    f32 forecast = gabi::fadds_ppc(value,delta);
    f32 nextDelta = gabi::fadds_ppc(delta,acceleration);
    forecast = gabi::fadds_ppc(forecast,nextDelta);
    sample->mAcceleration = acceleration;
    sample->mForecast = forecast;
    if (count < capacity) prependSample(history,sample);
    else {
        capacity = history->mCapacity;
        count = u32(count)-1;
        history->mCount = count;
        if (count < capacity) prependSample(history,sample);
    }
}
/* 0278C4DC */
void agl_history_l::TimingHistory::beginCapture() {
    TimingHistory* timing = this;
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!(u32(timing->mFlags)&1)) return;
    QueryRing* query = timing->mQueries[1];
    sampleHistoryTopTimestamp(query->mData);
    be<u64>* data = query->mData;
    if (u32(query->mCount)>1) data = memberAt<be<u64>>(data,8);
    sampleHistoryBottomTimestamp(data);
    query->mFlags = u32(query->mFlags)|1;
}
/* 0278C544 */
void agl_history_l::TimingHistory::endCapture() {
    TimingHistory* timing = this;
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!(u32(timing->mFlags)&1)) return;
    QueryRing* query = timing->mQueries[1];
    be<u64>* data = query->mData;
    if (u32(query->mCount)>2) data = memberAt<be<u64>>(data,16);
    sampleHistoryBottomTimestamp(data);
    query->mFlags = u32(query->mFlags)|2;
}
/* 0278C5A4 */
static void destroyTimingDisposer(sead::Disposer* disposer,u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (disposer == NULL) return;
    disposer->mVtable = &sTimingDisposerTable;
    if (disposer == sTimingDisposer) {
        TimingHistory* owner = sTimingOwner;
        sTimingDisposer = NULL;
        gabi::call_ptr<void>(owner->mVtable->mDestroy.mFunction,owner,2);
        sTimingOwner = NULL;
    }
    destroyHistoryDisposer(disposer,0);
    if (flags&1) releaseTimingObject(disposer);
}
/* WWHD glue
data sShaderHolderDisposerTable 10165630
data sTimingDisposerTable 1016573C
data sTimingTable 101656DC
ext 0273AD10 void* allocateTimingObject(u32)
ext 0273AF40 void releaseTimingObject(void*)
ext 0273B0D4 void* allocateHistoryOnHeap(u32,void*,s32)
ext 0273B050 void* allocateHistoryArray(u32,void*,s32)
ext 028F521C void initializeHistoryAllocation(void*,u32)
ext 02752B0C void registerHistoryDisposer(sead::Disposer*,void*,u32)
ext 02752BEC void destroyHistoryDisposer(sead::Disposer*,u32)
ext 02756140 void* selectHistoryHeap(sead::RegistryManagerState*)
ext C0009990 void setHistoryMemory(void*,u32,u32)
ext C0008898 void flushHistoryMemory(void*,u32)
ext 027AE6D8 void sampleHistoryTopTimestamp(void*)
ext 027AE6DC void sampleHistoryBottomTimestamp(void*)
ext 027AE6E0 Pair32 readHistoryQuery(void*)
ext 028F543C f64 convertHistoryTicks(u32,u32)
*/
