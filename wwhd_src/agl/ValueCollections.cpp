/* Value-collection accessors. Record bounds fallback preserves the original's first-record view. */
// References: WWHD's own instructions and strings only.
#include "agl/ValueCollections.h"
#include "sead/ElementAt.h"
static void initializeValueUniform(void* curve);
static void allocateValueUniform(void* curve, sead::RegistryHeap* heap, u32 flags);
static void copyValueUniformFormat(void* curve, const void* source);
static sead::RegistryHeap* currentCollectionHeap(sead::RegistryManagerState* manager);
static void* allocateCollectionArray(sead::RegistryHeap* heap, u32 bytes);
static void destroyValueUniform(void* curve, u32 flags);
static void flushValueUniform(void* curve, void* data, u32 invalidateGpu);
static sead::RegistryHeap* findCollectionHeap(sead::RegistryManagerState* manager, void* storage);
// Shared runtime-type pairs sCurveBase*/sAglCommonBase* are owned by sead/RuntimeStatics.cpp (sead/RuntimeType.h);
// the method tables this unit stores into them follow.
static u8 sCurveCollectionTypeMethods;
static u8 sCollectionBaseTypeMethods_l;

/* 027B432C */
u32 agl::CurveCollection::matchesType(const sead::RuntimeType* type) const {
    if (sCurveBaseGuard == 0) {
        sCurveBaseGuard = 1;
        sCurveBaseType.mTable = &sCurveCollectionTypeMethods;
    }
    if (type == &sCurveBaseType)
        return 1;
    if (sAglCommonBaseGuard == 0) {
        sAglCommonBaseGuard = 1;
        sAglCommonBaseType.mTable = &sCollectionBaseTypeMethods_l;
    }
    return type == &sAglCommonBaseType;
}

// Unit-local helpers are included in the canonical mutation ranges.
static void* allocateCollectionArray(sead::RegistryHeap* heap, u32 bytes) {
    if (!heap)
        heap = currentCollectionHeap(sRegistryManager.get());
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, bytes, 4);
}


/* 027B40B4 */
void agl::SpatialCollection::applyContext(u32 slot, void* context, void* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0)
        return;
    u32 slots = mValueCapacity;
    auto entries = mValueCounts.get();
    if (entries[slot < slots ? slot : 0] == 0)
        return;
    gabi::call_ptr<void>(mTable->mPrepareContext.mFunction, this, slot, context, output);
    u32 active = mCount;
    u32 capacity = mCapacity;
    auto begin = mRecords.get();
    u32 count = active <= capacity ? active : (u32)mCapacity;
    auto end = begin + count;
    for (auto record = begin; record != end; ++record) {
        u32 index = (gabi::ea(record) - gabi::ea(begin)) / sizeof(SpatialRecord);
        if (!gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index))
            continue;
        if (!gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, slot))
            continue;
        u32 currentCapacity = mCapacity;
        auto records = mRecords.get();
        auto selected = records + (index < currentCapacity ? index : 0);
        u32 values = selected->mValueCount;
        auto entries = selected->mValues.get();
        auto value = entries + (slot < values ? slot : 0);
        if ((value->mFlags & 1) == 0)
            gabi::call_ptr<void>(mTable->mApplyContext.mFunction, this, record, slot, context, output);
    }
}

/* 027B4244 */
s32 agl::SpatialCollection::findRecord() { return -1; }
/* 027B424C */
void agl::SpatialCollection::setCount(u32 count) { mCount = count; }
/* 027B4254 */
u32 agl::SpatialCollection::isEnabled(u32 index) {
    u32 capacity = mCapacity;
    auto records = mRecords.get();
    auto record = &sead::elementAt(records, index < capacity ? index : 0);
    return record->mFlags & 1;
}
/* 027B4278 */
void agl::SpatialCollection::setMask(u32 index, u32 enabled) {
    u32 capacity = mCapacity;
    auto records = mRecords.get();
    auto record = &sead::elementAt(records, index < capacity ? index : 0);
    if (enabled) {
        u32 mask = record->mMask;
        record->mMask = mask | ~0u;
    } else {
        record->mMask = 0;
    }
}
/* 027B42BC */
u32 agl::SpatialCollection::testMask(u32 index, u32 bit) {
    u32 capacity = mCapacity;
    auto records = mRecords.get();
    auto record = &sead::elementAt(records, index < capacity ? index : 0);
    u32 mask = record->mMask;
    u32 shift = bit & 63;
    u32 flag = shift < 32 ? 1u << shift : 0;
    return (mask & flag) != 0;
}
/* 027B4ED4 */
s32 agl::CurveCollection::findRecord() { return -1; }
/* 027B4EDC */
void agl::CurveCollection::setCount(u32 count) { mCount = count; }
/* 027B4EE4 */
u32 agl::CurveCollection::count() { return mCount; }
/* 027B4EEC */
u32 agl::CurveCollection::capacity() { return mCapacity; }
/* 027B4EF4 */
u32 agl::CurveCollection::isEnabled(u32 index) {
    u32 capacity = mCapacity;
    auto records = mRecords.get();
    auto record = &sead::elementAt(records, index < capacity ? index : 0);
    return record->mFlags & 1;
}
/* 027B4F18 */
void agl::CurveCollection::setMask(u32 index, u32 enabled) {
    u32 capacity = mCapacity;
    auto records = mRecords.get();
    auto record = &sead::elementAt(records, index < capacity ? index : 0);
    if (enabled) {
        u32 mask = record->mMask;
        record->mMask = mask | ~0u;
    } else {
        record->mMask = 0;
    }
}
/* 027B4F5C */
u32 agl::CurveCollection::testMask(u32 index, u32 bit) {
    u32 capacity = mCapacity;
    auto records = mRecords.get();
    auto record = &sead::elementAt(records, index < capacity ? index : 0);
    u32 mask = record->mMask;
    u32 shift = bit & 63;
    u32 flag = shift < 32 ? 1u << shift : 0;
    return (mask & flag) != 0;
}
/* 027B4D44 */
void agl::CurveCollection::applyContext(u32 slot, void* context, void* output) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0)
        return;
    u32 slots = mValueCapacity;
    auto entries = mValueCounts.get();
    if (entries[slot < slots ? slot : 0] == 0)
        return;
    gabi::call_ptr<void>(mTable->mPrepareContext.mFunction, this, slot, context, output);
    u32 active = mCount;
    u32 capacity = mCapacity;
    auto begin = mRecords.get();
    u32 count = active <= capacity ? active : (u32)mCapacity;
    auto end = begin + count;
    for (auto record = begin; record != end; ++record) {
        u32 offset = gabi::ea(record) - gabi::ea(begin);
        u32 index = (u32)(((u64)(offset >> 2) * 0x24924925u) >> 32);
        if (!gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index))
            continue;
        if (!gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, slot))
            continue;
        u32 currentCapacity = mCapacity;
        auto records = mRecords.get();
        auto selected = records + (index < currentCapacity ? index : 0);
        u32 values = selected->mValueCount;
        auto entries = selected->mValues.get();
        auto value = entries + (slot < values ? slot : 0);
        if ((value->mFlags & 1) == 0)
            gabi::call_ptr<void>(mTable->mApplyContext.mFunction, this, record, slot, context, output);
    }
}
/* 027B42F0 */
void agl::SpatialCollection::beforeAllocate() {}

/* 027B42F4 */
void agl::SpatialCollection::initializeRecord() {}

/* 027B42F8 */
void agl::SpatialCollection::initializeValue() {}

/* 027B42FC */
void agl::SpatialCollection::releaseRecord() {}

/* 027B4300 */
void agl::SpatialCollection::beforeUpdate() {}

/* 027B4304 */
void agl::SpatialCollection::updateRecord() {}

/* 027B4310 */
void agl::SpatialCollection::beforeRefresh() {}

/* 027B4314 */
void agl::SpatialCollection::refreshRecord() {}

/* 027B4318 */
void agl::SpatialCollection::prepareValues() {}

/* 027B431C */
void agl::SpatialCollection::beginApply() {}

/* 027B4320 */
void agl::SpatialCollection::applyValue() {}

/* 027B4324 */
void agl::SpatialCollection::finishValue() {}

/* 027B4328 */
void agl::SpatialCollection::applyWithContext() {}

/* 027B4308 */
u32 agl::SpatialCollection::needsRefresh() { return 0; }

/* 027B4F90 */
void agl::CurveCollection::beforeAllocate() {}

/* 027B4F94 */
void agl::CurveCollection::initializeRecord() {}

/* 027B4F98 */
void agl::CurveCollection::initializeValue() {}

/* 027B4F9C */
void agl::CurveCollection::releaseRecord() {}

/* 027B4FA0 */
void agl::CurveCollection::beforeUpdate() {}

/* 027B4FA4 */
void agl::CurveCollection::updateRecord() {}

/* 027B4FB0 */
void agl::CurveCollection::beforeRefresh() {}

/* 027B4FB4 */
void agl::CurveCollection::refreshRecord() {}

/* 027B4FB8 */
void agl::CurveCollection::prepareValues() {}

/* 027B4FBC */
void agl::CurveCollection::beginApply() {}

/* 027B4FC0 */
void agl::CurveCollection::applyValue() {}

/* 027B4FC4 */
void agl::CurveCollection::finishValue() {}

/* 027B4FC8 */
void agl::CurveCollection::applyWithContext() {}

/* 027B4FA8 */
u32 agl::CurveCollection::needsRefresh() { return 0; }



/* 027B47C8 */
void agl::CurveCollection::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0)
        return;
    gabi::call_ptr<void>(mTable->mBeforeUpdate.mFunction, this);
    auto begin = mRecords.get();
    u32 active = mCount;
    u32 capacity = mCapacity;
    auto end = begin + (active <= capacity ? active : capacity);
    for (auto record = begin; record != end; ++record) {
        u32 offset = gabi::ea(record) - gabi::ea(begin);
        u32 index = (u32)(((u64)(offset >> 2) * 0x24924925u) >> 32);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index))
            gabi::call_ptr<void>(mTable->mUpdateRecord.mFunction, this, record);
    }
}
/* 027B4A7C */
void agl::CurveCollection::refresh() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0)
        return;
    gabi::call_ptr<void>(mTable->mBeforeRefresh.mFunction, this);
    auto begin = mRecords.get();
    u32 active = mCount;
    u32 capacity = mCapacity;
    u32 count = active <= capacity ? active : (u32)mCapacity;
    auto end = begin + count;
    for (auto record = begin; record != end; ++record) {
        u32 offset = gabi::ea(record) - gabi::ea(begin);
        u32 index = (u32)(((u64)(offset >> 2) * 0x24924925u) >> 32);
        if (gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index))
            gabi::call_ptr<void>(mTable->mRefreshRecord.mFunction, this, record);
    }
}
/* 027B48C0 */
u32 agl::CurveCollection::prepare(u32 slot, void* context, u32 outputIndex) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 slots = mValueCapacity;
    auto counts = mValueCounts.get();
    auto countWord = counts + (slot < slots ? slot : 0);
    *countWord = 0;
    u32 active = mCount;
    if (active == 0)
        return 0;
    u32 capacity = mCapacity;
    auto begin = mRecords.get();
    auto end = begin + (active <= capacity ? active : capacity);
    if (begin == end)
        return 0;
    for (auto record = begin; record != end; ++record) {
        u32 offset = gabi::ea(record) - gabi::ea(begin);
        u32 index = (u32)(((u64)(offset >> 2) * 0x24924925u) >> 32);
        if (!gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index))
            continue;
        u32 ready = gabi::call_ptr<u32>(mTable->mPrepareValue.mFunction, this, record, slot, context);
        u32 currentCapacity = mCapacity;
        auto records = mRecords.get();
        auto selected = records + (index < currentCapacity ? index : 0);
        u32 values = selected->mValueCount;
        auto entries = selected->mValues.get();
        auto value = entries + (slot < values ? slot : 0);
        u32 flags = value->mFlags;
        value->mFlags = (u8)(ready ^ 1) ? flags | 1 : flags & ~1u;
        if (ready) {
            u32 currentValues = record->mValueCount;
            auto currentEntries = record->mValues.get();
            auto current = currentEntries + (slot < currentValues ? slot : 0);
            u32 stride = current->mUniform.stride;
            u32 displacement = stride * outputIndex;
            u32 base = gabi::ea(current->mUniform.gpuData.get());
            current->mUniform.cpuData = gabi::at<void>(base + displacement);
            *countWord = (u32)*countWord + 1;
        }
    }
    return *countWord;
}
/* 027B46A8 */
void agl::CurveCollection::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 capacity = mCapacity;
    auto record = mRecords.get();
    auto end = record + capacity;
    for (; record != end; ++record) {
        gabi::call_ptr<void>(mTable->mReleaseRecord.mFunction, this, record);
        auto entries = record->mValues.get();
        if (gabi::ea(entries)) {
            u32 values = record->mValueCount;
            for (u32 index = 0; (s32)index < (s32)values; ++index) {
                u32 address = gabi::ea(entries) + (index << 5);
                if (address) {
                    destroyValueUniform(gabi::at<UniformBuffer>(address + 4), 2);
                    values = record->mValueCount;
                    entries = record->mValues.get();
                }
            }
            auto heap = findCollectionHeap(sRegistryManager.get(), entries);
            auto dispatch = heap->dispatch();
            gabi::call_ptr<void>(dispatch->mRelease.mFunction, heap, record->mValues.get());
            record->mValueCount = 0;
            record->mValues = nullptr;
        }
    }
    auto records = mRecords.get();
    if (gabi::ea(records)) {
        auto heap = findCollectionHeap(sRegistryManager.get(), records);
        auto dispatch = heap->dispatch();
        gabi::call_ptr<void>(dispatch->mRelease.mFunction, heap, mRecords.get());
        mCapacity = 0;
        mRecords = nullptr;
    }
}
/* 027B4B78 */
void agl::CurveCollection::apply(u32 slot, void* context, u32 invalidateGpu) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCount == 0)
        return;
    u32 slots = mValueCapacity;
    auto counts = mValueCounts.get();
    if (counts[slot < slots ? slot : 0] == 0)
        return;
    gabi::call_ptr<void>(mTable->mPrepareApply.mFunction, this, slot, context);
    u32 active = mCount;
    u32 capacity = mCapacity;
    auto begin = mRecords.get();
    u32 count = active <= capacity ? active : (u32)mCapacity;
    auto end = begin + count;
    for (auto record = begin; record != end; ++record) {
        u32 offset = gabi::ea(record) - gabi::ea(begin);
        u32 index = (u32)(((u64)(offset >> 2) * 0x24924925u) >> 32);
        if (!gabi::call_ptr<u32>(mTable->mIsEnabled.mFunction, this, index))
            continue;
        if (!gabi::call_ptr<u32>(mTable->mCanApply.mFunction, this, index, slot))
            continue;
        u32 currentCapacity = mCapacity;
        auto records = mRecords.get();
        auto selected = records + (index < currentCapacity ? index : 0);
        u32 values = selected->mValueCount;
        auto entries = selected->mValues.get();
        auto value = entries + (slot < values ? slot : 0);
        if (value->mFlags & 1)
            continue;
        gabi::call_ptr<void>(mTable->mBeginApply.mFunction, this, record, slot, context);
        gabi::call_ptr<void>(mTable->mApplyValue.mFunction, this, record, slot, context);
        u32 currentValues = record->mValueCount;
        auto currentEntries = record->mValues.get();
        auto current = currentEntries + (slot < currentValues ? slot : 0);
        void* data = current->mUniform.cpuData;
        flushValueUniform(&current->mUniform, data, invalidateGpu);
    }
}
/* 027B43A4 */
void agl::CurveCollection::initialize(u32 owner, s32 recordCount, s32 valueCount, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mOwner.v = owner;
    mCount = 0;
    gabi::call_ptr<void>(mTable->mBeforeAllocate.mFunction, this, heap);
    if (recordCount > 0) {
        void* storage = allocateCollectionArray(heap, (u32)recordCount * sizeof(CurveRecord));
        u32 base = gabi::ea(storage);
        for (u32 index = 0; index < (u32)recordCount; ++index) {
            u32 address = base + index * sizeof(CurveRecord);
            if (address) {
                auto record = gabi::at<CurveRecord>(address);
                record->mFlags = 0;
                record->mMask = 0;
                record->mValues = nullptr;
                record->mValueCount = 0;
            }
        }
        if (storage) {
            mRecords = (CurveRecord*)storage;
            mCapacity = recordCount;
        }
    }
    if (valueCount > 0) {
        void* storage = allocateCollectionArray(heap, (u32)valueCount * 4);
        if (storage) {
            mValueCounts = (be<u32>*)storage;
            mValueCapacity = valueCount;
        }
    }
    u32 capacity = mCapacity;
    auto begin = mRecords.get();
    auto end = begin + capacity;
    UniformBuffer* prototype = nullptr;
    for (auto record = begin; record != end; ++record) {
        if (valueCount > 0) {
            void* storage = allocateCollectionArray(heap, (u32)valueCount * sizeof(CurveValue));
            u32 base = gabi::ea(storage);
            for (u32 index = 0; index < (u32)valueCount; ++index) {
                u32 address = base + index * sizeof(CurveValue);
                if (address) {
                    auto value = gabi::at<CurveValue>(address);
                    value->mFlags = 0;
                    initializeValueUniform(&value->mUniform);
                }
            }
            if (storage) {
                record->mValues = (CurveValue*)storage;
                record->mValueCount = valueCount;
            }
        }
        u32 offset = gabi::ea(record) - gabi::ea(begin);
        u32 index = (u32)(((u64)(offset >> 2) * 0x24924925u) >> 32);
        gabi::call_ptr<void>(mTable->mSetRecordEnabled.mFunction, this, index, 1);
        gabi::call_ptr<void>(mTable->mInitializeRecord.mFunction, this, record, heap);
        u32 count = record->mValueCount;
        auto values = record->mValues.get();
        auto valueEnd = values + count;
        for (auto value = values; value != valueEnd; ++value) {
            auto curve = &value->mUniform;
            if (!prototype) {
                gabi::call_ptr<void>(mTable->mInitializeValue.mFunction, this, curve, heap);
                prototype = curve;
                allocateValueUniform(curve, heap, 2);
            } else {
                copyValueUniformFormat(curve, prototype);
                allocateValueUniform(curve, heap, 2);
            }
        }
    }
}
/* WWHD glue
ext 027BEB5C void initializeValueUniform(void* curve)
ext 027BEEE4 void allocateValueUniform(void* curve, sead::RegistryHeap* heap, u32 flags)
ext 027BEE9C void copyValueUniformFormat(void* curve, const void* source)
ext 02756140 sead::RegistryHeap* currentCollectionHeap(sead::RegistryManagerState* manager)
ext 027BEC90 void destroyValueUniform(void* curve, u32 flags)
ext 027BEF80 void flushValueUniform(void* curve, void* data, u32 invalidateGpu)
ext 02755FEC sead::RegistryHeap* findCollectionHeap(sead::RegistryManagerState* manager, void* storage)
data sCurveCollectionTypeMethods 10168FCC
data sCollectionBaseTypeMethods_l 10168F14
*/
