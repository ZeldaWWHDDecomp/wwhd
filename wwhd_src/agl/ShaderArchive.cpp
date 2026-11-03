/* WWHD shader archive updates. */
// References: WWHD's own instructions and strings only.
#include "agl/ShaderArchive.h"
#include "sead/Thread.h"
#include "agl/BitmapTransfer.h"
static void refreshArchiveProgram(agl::ShaderArchiveProgramView_l* program);
static u32 updateArchiveProgram(agl::ShaderArchiveProgramView_l* program);
static void markArchiveProgram(agl::ShaderArchiveProgramView_l* program, u32 flags);
struct ShaderCompileSelections_l { be<u8> mValues[1024]; };
static agl::ShaderArchiveOwnedObject_l* createCompiledArchive(const sead::SafeText* name,
    const gptr<be<u8>>* binaries, const gptr<be<u8>>* strings, u32 count,
    be<u8>* selections, sead::RegistryHeap* heap);
static sead::RegistryHeap* currentArchiveHeap(sead::RegistryManagerState* manager);
static void* allocateArchiveStorage(sead::RegistryHeap* heap, u32 bytes);
static agl::ShaderStageBindings_l* findArchiveStageBindings(const agl::ShaderProgramSourceHandle_l* source, u32 stage);
static void initializeArchiveStageLayout(agl::ShaderInputLayout_l* layout, u32 names, u32 values, sead::RegistryHeap* heap);
static agl::ShaderArchiveStageView_l* selectArchiveProgramStage(agl::ShaderArchiveProgramView_l* program, u32 stage);
static void initializeArchiveProgram(agl::ShaderArchiveProgramView_l* program, const sead::SafeText* name, sead::RegistryHeap* heap);
static void initializeArchiveVariants(agl::ShaderArchiveProgramView_l* program, u32 count, sead::RegistryHeap* heap);
static be<u8>* findArchiveVariantDefault(const agl::ShaderVariantSourceHandle_l* source);
static be<u8>* findArchiveVariantValue(const agl::ShaderVariantSourceHandle_l* source, u32 index);
static void initializeArchiveVariant(agl::ShaderArchiveProgramView_l* program, u32 index, const sead::SafeText* name, const sead::SafeText* initial, u32 count, sead::RegistryHeap* heap);
static void setArchiveVariantValue(agl::ShaderArchiveProgramView_l* program, u32 variant, u32 index, const sead::SafeText* value);
static void finishArchiveVariants(agl::ShaderArchiveProgramView_l* program, sead::RegistryHeap* heap);
static void* allocateArchiveObject(u32 bytes);
static void releaseArchiveObject(void* object);
static void releaseArchiveFilename(void* filename);
static void initializeArchiveDisposer(sead::RegisteredObject* object);
static void destroyArchiveDisposer(sead::RegisteredObject* object, u32 flags);
static sead::RegistryHeap* findArchiveHeap(sead::RegistryManagerState* manager, void* storage);
static agl::ShaderInputLayout_l* constructArchiveInputLayout(agl::ShaderInputLayout_l* object);
static void destroyArchiveInputLayout(agl::ShaderInputLayout_l* object, u32 flags);
static void constructArchiveLayouts(void* storage, u32 count, u32 stride, agl::ShaderLayoutConstructor_l constructor);
static void destroyArchiveLayouts(void* storage, u32 count, u32 stride, agl::ShaderLayoutDestructor_l destructor, u32 flags, u32 reserved);
static sead::DisposerVtable sShaderVariationMethods;
static u8 sShaderVariationNameMethods;
static agl::ShaderProgramSetMethods_l sShaderProgramSetMethods;
static sead::DisposerVtable sShaderArchiveMethods;
static u8 sShaderArchiveNameMethods;
static sead::SafeTextVtable sShaderArchiveTextMethods;
static void setArchiveName(void* receiver, const sead::SafeText* name);
/* 027B9070 */
u32 agl::ShaderArchive_l::update() {
    gabi::AutoFrame frame{gabi::current_function()};
    updateVariations();
    return updatePrograms((u32)mFlags & 1);
}

// Unit-local helpers are included in the canonical mutation ranges.
static void* allocateArchiveStorage(sead::RegistryHeap* heap, u32 bytes) {
    if (!heap)
        heap = currentArchiveHeap(sRegistryManager.get());
    return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, bytes, 4);
}

template <class T> static void releaseArchiveArray(be<u32>& count, gptr<T>& storage, bool releaseNull = false) {
    auto pointer = storage.get();
    if (!pointer && !releaseNull)
        return;
    auto heap = findArchiveHeap(sRegistryManager.get(), pointer);
    gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage.get());
    count = 0;
    storage = nullptr;
}

/* 027B7A48 */
agl::ShaderVariation_l* agl::ShaderVariation_l::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    ShaderVariation_l* result = this;
    if (gabi::ea(result) == 0) {
        result = (ShaderVariation_l*)allocateArchiveObject(sizeof(ShaderVariation_l));
        if (!result)
            return result;
    }
    initializeArchiveDisposer(&result->mDisposer);
    result->mDisposer.mVtable = &sShaderVariationMethods;
    result->mNameMethods = gabi::ea(&sShaderVariationNameMethods);
    result->mFlags = 1;
    result->mArchive = nullptr;
    result->mSource = nullptr;
    result->mSourceObject = nullptr;
    result->mCompiledObject = nullptr;
    result->mSelectionCount = 0;
    result->mSelections = nullptr;
    return result;
}
/* 027B7B1C */
agl::ShaderArchiveProgramSet_l* agl::ShaderArchiveProgramSet_l::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    ShaderArchiveProgramSet_l* result = this;
    if (gabi::ea(result) == 0) {
        result = (ShaderArchiveProgramSet_l*)allocateArchiveObject(sizeof(ShaderArchiveProgramSet_l));
        if (!result)
            return result;
    }
    result->mArchive = nullptr;
    result->mVariantIndex = 0;
    result->mMethods = &sShaderProgramSetMethods;
    result->mProgramIndex = 0;
    constructArchiveLayouts(result->mLayouts, 3, sizeof(ShaderInputLayout_l), constructArchiveInputLayout);
    result->mValueCount = 0;
    result->mValues = nullptr;
    result->mFlags = 0;
    return result;
}
/* 027B91B4 */
void agl::ShaderArchiveProgramSet_l::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    auto values = mValues.get();
    mMethods = &sShaderProgramSetMethods;
    if (values) {
        auto heap = findArchiveHeap(sRegistryManager.get(), values);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, mValues.get());
        mValueCount = 0;
        mValues = nullptr;
    }
    destroyArchiveLayouts(mLayouts, 3, sizeof(ShaderInputLayout_l), destroyArchiveInputLayout, 0, 0);
    if (flags & 1)
        releaseArchiveObject(this);
}
/* 027B9268 */
void agl::ShaderVariation_l::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    mDisposer.mVtable = &sShaderVariationMethods;
    auto compiled = mCompiledObject.get();
    mNameMethods = gabi::ea(&sShaderVariationNameMethods);
    if (compiled) {
        gabi::call_ptr<void>(compiled->mMethods->mDestroy.mFunction, compiled, 3);
        mCompiledObject = nullptr;
    }
    auto source = mSourceObject.get();
    if (source) {
        gabi::call_ptr<void>(source->mMethods->mDestroy.mFunction, source, 3);
        mSourceObject = nullptr;
    }
    auto selections = mSelections.get();
    if (selections) {
        auto heap = findArchiveHeap(sRegistryManager.get(), selections);
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, mSelections.get());
        mSelectionCount = 0;
        mSelections = nullptr;
    }
    destroyArchiveDisposer(&mDisposer, 0);
    if (flags & 1)
        releaseArchiveObject(this);
}
/* 027B744C */
agl::ShaderArchive_l* agl::ShaderArchive_l::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    ShaderArchive_l* result = this;
    if (gabi::ea(result) == 0) {
        result = (ShaderArchive_l*)allocateArchiveObject(sizeof(ShaderArchive_l));
        if (!result)
            return result;
    }
    initializeArchiveDisposer(&result->mDisposer);
    result->mDisposer.mVtable = &sShaderArchiveMethods;
    result->mNameMethods = gabi::ea(&sShaderArchiveNameMethods);
    result->mBinaryImageAddress = 0;
    result->mResource = nullptr;
    result->mPrograms = nullptr;
    result->mProgramCount = 0;
    result->mUpdateCount = 0;
    result->mCallback = nullptr;
    result->mFilename = nullptr;
    result->mContext = 0;
    result->mFlags = 0;
    result->mProgramSets = nullptr;
    result->mProgramSetCount = 0;
    result->mVariations = nullptr;
    result->mVariationCount = 0;
    result->mStrings = nullptr;
    result->mStringCount = 0;
    result->mBinaries = nullptr;
    result->mBinaryCount = 0;
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mVtable = &sShaderArchiveTextMethods;
    name->mText.v = gabi::guest_str("Icon = LAYOUT, Security = agl_shader");
    setArchiveName(&result->mNameMethods, name.get());
    return result;
}
/* 027B75E0 */
void agl::ShaderArchive_l::releaseTransient() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mResource) {
        auto sets = mProgramSets.get();
        if (sets) {
            for (u32 index = 0; (s32)index < (s32)(u32)mProgramSetCount; ++index) {
                auto set = sets + index;
                gabi::call_ptr<void>(set->mMethods->mDestroy, set, 2);
                sets = mProgramSets.get();
            }
            releaseArchiveArray(mProgramSetCount, mProgramSets, true);
        }
        releaseArchiveArray(mStringCount, mStrings);
        releaseArchiveArray(mBinaryCount, mBinaries);
        auto variations = mVariations.get();
        if (variations) {
            for (u32 index = 0; (s32)index < (s32)(u32)mVariationCount; ++index) {
                auto variation = variations + index;
                gabi::call_ptr<void>(variation->mDisposer.mVtable->mDestroy.mFunction, variation, 2);
                variations = mVariations.get();
            }
            releaseArchiveArray(mVariationCount, mVariations, true);
        }
    }
    mResource = nullptr;
}
/* 027B7780 */
void agl::ShaderArchive_l::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseTransient();
    auto programs = mPrograms.get();
    if (programs) {
        for (u32 index = 0; (s32)index < (s32)(u32)mProgramCount; ++index) {
            auto program = programs + index;
            gabi::call_ptr<void>(program->mMethods->mDestroy.mFunction, program, 2);
            programs = mPrograms.get();
        }
        releaseArchiveArray(mProgramCount, mPrograms, true);
    }
    mBinaryImageAddress = 0;
    void* filename = mFilename;
    if (filename) {
        releaseArchiveFilename(filename);
        mFilename = nullptr;
    }
    mUpdateCount = 0;
}
/* 027B7860 */
void agl::ShaderArchive_l::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    mDisposer.mVtable = &sShaderArchiveMethods;
    mNameMethods = gabi::ea(&sShaderArchiveNameMethods);
    release();
    destroyArchiveDisposer(&mDisposer, 0);
    if (flags & 1)
        releaseArchiveObject(this);
}
/* 027B814C */
void agl::ShaderArchiveProgramSet_l::updateLayouts() {
    gabi::AutoFrame frame{gabi::current_function()};
    for (auto layout = mLayouts; layout != mLayouts + 3; ++layout) {
        auto variation = layout->mVariation.get();
        if (!variation || (variation->mFlags & 1) == 0)
            continue;
        layout->mProgramData = variation->mCompiledObject.get();
        auto archive = mArchive.get();
        u32 index = mProgramIndex;
        u32 count = archive->mProgramCount;
        auto programs = archive->mPrograms.get();
        refreshArchiveProgram(programs + (index < count ? index : 0));
    }
}
/* 027B81E4 */
void agl::ShaderArchive_l::updateVariations() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::TickTime> clock(8);
    sead::TickTime::construct(clock.get());
    u32 count = mVariationCount;
    auto begin = mVariations.get();
    auto end = begin + count;
    for (auto variation = begin; variation != end; ++variation) {
        if (variation->mFlags & 1)
            variation->compile();
    }
    u32 setCount = mProgramSetCount;
    auto sets = mProgramSets.get();
    auto setEnd = sets + setCount;
    for (auto set = sets; set != setEnd; ++set)
        set->updateLayouts();
    count = mVariationCount;
    begin = mVariations.get();
    end = begin + count;
    for (auto variation = begin; variation != end; ++variation)
        variation->mFlags = (u32)variation->mFlags & ~1u;
}
/* 027B8EB0 */
void agl::ShaderArchiveProgramSet_l::markProgramDirty() {
    if ((mFlags & 1) == 0)
        return;
    auto archive = mArchive.get();
    u32 index = mProgramIndex;
    u32 count = archive->mProgramCount;
    auto programs = archive->mPrograms.get();
    u32 variant = mVariantIndex;
    auto binary = programs[index < count ? index : 0].mBinary.get();
    ShaderArchiveProgramView_l* selected;
    if (variant == 0)
        selected = binary->mPrimary.get();
    else {
        u32 variants = binary->mVariantCount;
        auto entries = binary->mVariants.get();
        selected = entries + (variant - 1 < variants ? variant - 1 : 0);
    }
    u32 flags = (u8)((u32)selected->mFlags | 8);
    flags |= 2;
    if ((flags >> 1) & 1) {
        selected->mFlags = flags & ~2u;
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)markArchiveProgram), selected, 0);
    } else {
        selected->mFlags = flags;
    }
}
/* 027B8F48 */
u32 agl::ShaderArchive_l::updatePrograms(u32 once) {
    gabi::AutoFrame frame{gabi::current_function()};
    mUpdateCount = (u32)mUpdateCount + 1;
    gabi::FrameLocal<sead::TickTime> clock(8);
    sead::TickTime::construct(clock.get());
    u32 setCount = mProgramSetCount;
    auto sets = mProgramSets.get();
    auto setEnd = sets + setCount;
    for (auto set = sets; set != setEnd; ++set)
        set->markProgramDirty();
    u32 count = mProgramCount;
    auto programs = mPrograms.get();
    auto end = programs + count;
    for (auto program = programs; program != end; ++program) {
        u32 context = mContext;
        auto binary = program->mBinary.get();
        binary->mContext = context;
        if ((!once || (u32)mUpdateCount <= 1) && updateArchiveProgram(program))
            return 0;
    }
    clock->elapsed();
    auto callback = mCallback.get();
    if (callback)
        gabi::call_ptr<void>(callback->mMethods->mUpdate, callback, this);
    return 1;
}
/* 027B90AC */
s32 agl::ShaderArchive_l::findProgram(const sead::SafeText* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 count = mProgramCount;
    auto begin = mPrograms.get();
    auto end = begin + count;
    for (auto program = begin; program != end; ++program) {
        auto binary = program->mBinary.get();
        binary->mName.assureTermination();
        binary->mName.assureTermination();
        auto identity = binary->mName.mText.get();
        gabi::call_ptr<void>(name->mVtable->mAssureTermination.mFunction, name);
        auto right = name->mText.get();
        bool same = identity == right;
        if (!same) {
            auto left = binary->mName.mText.get();
            for (u32 index = 0; index < 0x40001; ++index) {
                u32 first = left[index];
                u32 second = right[index];
                if (first != second)
                    break;
                if (first == 0) {
                    same = true;
                    break;
                }
            }
        }
        if (same) {
            u32 offset = gabi::ea(program) - gabi::ea(begin);
            return offset / sizeof(ShaderArchiveProgramView_l);
        }
    }
    return -1;
}
/* 027B7BE8 */
void agl::ShaderVariation_l::initialize(ShaderArchive_l* archive, u32 index, const ShaderVariationSourceHandle_l* handle,
                                      u32 compiled, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    auto source = handle->mSource.get();
    mArchive = archive;
    mSource = source;
    u32 flags = mFlags;
    mIndex = (u16)index;
    mFlags = compiled ? flags | 2 : flags & ~2u;
    u32 selected = mIndex;
    u32 binaryCount = archive->mBinaryCount;
    auto binaries = archive->mBinaries.get();
    binaries[selected < binaryCount ? selected : 0] = gabi::at<be<u8>>(gabi::ea(mSource.get()) + 0x10);
    archive = mArchive.get();
    u32 stringCount = archive->mStringCount;
    selected = mIndex;
    u32 offset = source->mTextOffset;
    auto strings = archive->mStrings.get();
    strings[selected < stringCount ? selected : 0] = gabi::at<be<u8>>(gabi::ea(source) + offset + 0x10);
    archive = mArchive.get();
    s32 count = archive->mVariationCount;
    if (count > 0) {
        void* storage = allocateArchiveStorage(heap, (u32)count);
        if (storage) {
            mSelectionCount = count;
            mSelections = (be<u8>*)storage;
        }
    }
    auto begin = mSelections.get();
    auto end = begin + (u32)mSelectionCount;
    for (auto selection = begin; selection != end; ++selection)
        *selection = 0;
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mVtable = &sShaderArchiveTextMethods;
    name->mText.v = gabi::guest_str("Icon = NOTE");
    setArchiveName(&mNameMethods, name.get());
}
/* 027B8024 */
void agl::ShaderVariation_l::compile() {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((mFlags & 2) == 0)
        return;
    auto compiled = mCompiledObject.get();
    if (compiled) {
        gabi::call_ptr<void>(compiled->mMethods->mDestroy.mFunction, compiled, 3);
        mCompiledObject = nullptr;
    }
    auto archive = mArchive.get();
    u32 index = mIndex;
    u32 count = archive->mStringCount;
    auto strings = archive->mStrings.get();
    gabi::FrameLocal<sead::SafeText> name(8);
    name->mText = strings[index < count ? index : 0].get();
    name->mVtable = &sShaderArchiveTextMethods;
    auto root = sBitmapHeapSource_l.get();
    auto binaries = archive->mBinaries.get();
    auto sourceNames = archive->mStrings.get();
    u32 variationCount = archive->mVariationCount;
    auto heap = root->mCompileHeap.get();
    gabi::FrameLocal<ShaderCompileSelections_l> selections(0x10);
    auto result = createCompiledArchive(name.get(), binaries, sourceNames, variationCount, selections->mValues, heap);
    archive = mArchive.get();
    mCompiledObject = result;
    count = archive->mVariationCount;
    auto begin = archive->mVariations.get();
    auto end = begin + count;
    for (auto variation = begin; variation != end; ++variation) {
        u32 capacity = variation->mSelectionCount;
        index = mIndex;
        auto values = variation->mSelections.get();
        auto selected = values + (index < capacity ? index : 0);
        u32 offset = gabi::ea(variation) - gabi::ea(begin);
        u32 ordinal = offset / sizeof(ShaderVariation_l);
        u32 value = selections->mValues[ordinal];
        *selected = value;
    }
}
/* 027B7D78 */
void agl::ShaderArchiveProgramSet_l::initialize(ShaderArchive_l* archive, u32 index,
                                              const ShaderProgramSourceHandle_l* handle, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ShaderProgramSourceHandle_l> source(8);
    source->mSource = handle->mSource.get();
    mProgramIndex = index;
    mArchive = archive;
    u32 selected = (u16)index;
    u32 count = archive->mProgramCount;
    auto programs = archive->mPrograms.get();
    auto program = programs + (selected < count ? selected : 0);
    for (u32 stage = 0; stage < 3; ++stage) {
        auto layout = mLayouts + stage;
        auto resource = source->mSource.get();
        s32 variationIndex = resource->mStages[stage].mVariationIndex;
        if (variationIndex == -1) {
            layout->mVariation = nullptr;
            continue;
        }
        auto owner = mArchive.get();
        u32 variations = owner->mVariationCount;
        auto entries = owner->mVariations.get();
        layout->mVariation = entries + ((u32)variationIndex < variations ? (u32)variationIndex : 0);
        auto bindings = findArchiveStageBindings(source.get(), stage);
        auto binary = program->mBinary.get();
        initializeArchiveStageLayout(layout, bindings->mCount, binary->mVariantDimensionCount, heap);
        auto variation = layout->mVariation.get();
        layout->mSourceData = gabi::at<be<u8>>(gabi::ea(variation->mSource.get()) + 0x10);
        u32 bindingCount = bindings->mCount;
        auto record = gabi::at<ShaderStageBinding_l>(gabi::ea(bindings) + 8);
        for (u32 binding = 0; binding < bindingCount; ++binding) {
            s32 nameCount = layout->mNameCount;
            s32 nameCapacity = layout->mNameCapacity;
            if (nameCount < nameCapacity) {
                auto names = layout->mNames.get();
                names[nameCount] = gabi::at<be<u8>>(gabi::ea(record) + 0xC);
                layout->mNameCount = (u32)layout->mNameCount + 1;
            }
            u32 valueOffset = record->mValueOffset;
            s32 valueCapacity = layout->mValueCapacity;
            s32 valueCount = layout->mValueCount;
            if (valueCount < valueCapacity) {
                auto values = layout->mValues.get();
                values[valueCount] = gabi::at<be<u8>>(gabi::ea(record) + valueOffset + 0xC);
                layout->mValueCount = (u32)layout->mValueCount + 1;
            }
            record = gabi::at<ShaderStageBinding_l>(gabi::ea(record) + (u32)record->mSize);
        }
        auto target = selectArchiveProgramStage(program, stage);
        target->mSource = layout;
    }
    auto binary = program->mBinary.get();
    s32 dimensions = binary->mVariantDimensionCount;
    if (dimensions > 0) {
        void* storage = allocateArchiveStorage(heap, (u32)dimensions * 4);
        if (storage) {
            mValueCount = dimensions;
            mValues = (be<u32>*)storage;
        }
        auto begin = mValues.get();
        auto end = begin + (u32)mValueCount;
        for (auto value = begin; value != end; ++value)
            *value = 0;
    }
    gabi::FrameLocal<sead::SafeText> name(0xC);
    name->mVtable = &sShaderArchiveTextMethods;
    name->mText.v = gabi::guest_str("Icon = CIRCLE_ORENGE");
    setArchiveName(this, name.get());
}
/* 027B78D4 */
void agl::ShaderArchiveProgramView_l::initializeSources(const sead::SafeText* name,
    const ShaderVariantListHandle_l* variants, const ShaderResourceWord_l* first,
    u32 unused, const ShaderResourceWord_l* second, const ShaderResourceWord_l* third,
    const ShaderResourceWord_l* fourth, const ShaderResourceWord_l* fifth, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    initializeArchiveProgram(this, name, heap);
    auto binary = mBinary.get();
    u32 word = first->mValue;
    binary->mResourceWords[0] = word;
    word = second->mValue;
    binary = mBinary.get();
    binary->mResourceWords[5] = word;
    binary = mBinary.get();
    word = third->mValue;
    binary->mResourceWords[2] = word;
    binary = mBinary.get();
    word = fourth->mValue;
    binary->mResourceWords[3] = word;
    binary = mBinary.get();
    word = fifth->mValue;
    binary->mResourceWords[4] = word;
    auto list = variants->mList.get();
    u32 count = list->mCount;
    if (count == 0)
        return;
    initializeArchiveVariants(this, count, heap);
    list = variants->mList.get();
    count = list->mCount;
    auto record = gabi::at<ShaderVariantSource_l>(gabi::ea(list) + 8);
    gabi::FrameLocal<ShaderVariantSourceHandle_l> source(8);
    gabi::FrameLocal<sead::SafeText> value(0xC), variantName(0x14), defaultValue(0x1C);
    for (u32 variant = 0; variant < count; ++variant) {
        source->mSource = record;
        variantName->mVtable = &sShaderArchiveTextMethods;
        variantName->mText = gabi::at<be<u8>>(gabi::ea(record) + 0x10);
        auto initial = findArchiveVariantDefault(source.get());
        defaultValue->mVtable = &sShaderArchiveTextMethods;
        defaultValue->mText = initial;
        auto current = source->mSource.get();
        initializeArchiveVariant(this, variant, variantName.get(), defaultValue.get(), current->mValueCount, heap);
        for (u32 index = 0; (s32)index < (s32)(u32)source->mSource->mValueCount; ++index) {
            auto text = findArchiveVariantValue(source.get(), index);
            value->mText = text;
            value->mVtable = &sShaderArchiveTextMethods;
            setArchiveVariantValue(this, variant, index, value.get());
        }
        record = gabi::at<ShaderVariantSource_l>(gabi::ea(record) + (u32)record->mSize);
    }
    finishArchiveVariants(this, heap);
}
/* WWHD glue
ext 027BA710 void initializeArchiveProgram(agl::ShaderArchiveProgramView_l* program, const sead::SafeText* name, sead::RegistryHeap* heap)
ext 027BA95C void initializeArchiveVariants(agl::ShaderArchiveProgramView_l* program, u32 count, sead::RegistryHeap* heap)
ext 027BC4AC be<u8>* findArchiveVariantDefault(const agl::ShaderVariantSourceHandle_l* source)
ext 027BC4F8 be<u8>* findArchiveVariantValue(const agl::ShaderVariantSourceHandle_l* source, u32 index)
ext 027BAA54 void initializeArchiveVariant(agl::ShaderArchiveProgramView_l* program, u32 index, const sead::SafeText* name, const sead::SafeText* initial, u32 count, sead::RegistryHeap* heap)
ext 027BAA9C void setArchiveVariantValue(agl::ShaderArchiveProgramView_l* program, u32 variant, u32 index, const sead::SafeText* value)
ext 027BAC4C void finishArchiveVariants(agl::ShaderArchiveProgramView_l* program, sead::RegistryHeap* heap)
str 10169AF4
ext 027BC654 agl::ShaderStageBindings_l* findArchiveStageBindings(const agl::ShaderProgramSourceHandle_l* source, u32 stage)
ext 027D3F1C void initializeArchiveStageLayout(agl::ShaderInputLayout_l* layout, u32 names, u32 values, sead::RegistryHeap* heap)
ext 027BAC58 agl::ShaderArchiveStageView_l* selectArchiveProgramStage(agl::ShaderArchiveProgramView_l* program, u32 stage)
str 10169AE8
ext 02756140 sead::RegistryHeap* currentArchiveHeap(sead::RegistryManagerState* manager)
ext 027C6060 agl::ShaderArchiveOwnedObject_l* createCompiledArchive(const sead::SafeText* name, const gptr<be<u8>>* binaries, const gptr<be<u8>>* strings, u32 count, be<u8>* selections, sead::RegistryHeap* heap)
ext 02760DE8 sead::TickTime* sead::TickTime::construct(sead::TickTime* object)
ext 02760E58 Pair32 sead::TickTime::elapsed()
ext 027BBCA0 void refreshArchiveProgram(agl::ShaderArchiveProgramView_l* program)
ext 027BBB9C u32 updateArchiveProgram(agl::ShaderArchiveProgramView_l* program)
ext 027BB9E0 void markArchiveProgram(agl::ShaderArchiveProgramView_l* program, u32 flags) callback
data sShaderArchiveMethods 10169B0C
data sShaderArchiveNameMethods 10169B1C
data sShaderArchiveTextMethods 101699E0
str 10169AC0
ext 02785CB4 void setArchiveName(void* receiver, const sead::SafeText* name)
ext 0273AFC8 void releaseArchiveFilename(void* filename)
data sShaderVariationMethods 10169B2C
data sShaderVariationNameMethods 10169B3C
data sShaderProgramSetMethods 10169B4C
ext 0273AD10 void* allocateArchiveObject(u32 bytes)
ext 0273AF40 void releaseArchiveObject(void* object)
ext 02752A84 void initializeArchiveDisposer(sead::RegisteredObject* object)
ext 02752BEC void destroyArchiveDisposer(sead::RegisteredObject* object, u32 flags)
ext 02755FEC sead::RegistryHeap* findArchiveHeap(sead::RegistryManagerState* manager, void* storage)
ext 027B9470 agl::ShaderInputLayout_l* constructArchiveInputLayout(agl::ShaderInputLayout_l* object) callback
ext 027B94B8 void destroyArchiveInputLayout(agl::ShaderInputLayout_l* object, u32 flags) callback
ext 028EFFD0 void constructArchiveLayouts(void* storage, u32 count, u32 stride, agl::ShaderLayoutConstructor_l constructor)
ext 028F0164 void destroyArchiveLayouts(void* storage, u32 count, u32 stride, agl::ShaderLayoutDestructor_l destructor, u32 flags, u32 reserved)
*/
