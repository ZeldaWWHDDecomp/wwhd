// agl multi-filter: construction of the seven filter groups and their parameters, drawing the active
// filters onto a target and handing their result to the result sampler, and teardown; plus this
// translation unit's compiler-emitted text, parameter-list and bool-parameter copies.
// Written from WWHD's instructions.
#include "agl/MultiFilter.h"
#include "sead/Reports.h"
using namespace agl;

static void* allocateObject(u32 size);
static void releaseObject(void* object);
static void updateFilterLists(MultiFilter* filter);
static void destroyGroupParameters(multi_filter::ParameterList* list, u32 flags);
static void constructArray(void* objects, u32 count, u32 stride, u32 constructor);
static void destroyArray(void* objects, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved);
static void setDisplayName(MultiFilter* filter, sead::SafeText* name);
static parameter_tree::Value* constructParameterObject(parameter_tree::Value* storage);
static s32 formatText(sead::SafeTextBuffer* text, const char* format, ReportArguments* arguments);
static sead::DisposerVtable sMultiFilterBaseMethods;
static sead::DisposerVtable sMultiFilterMethods;
static sead::DisposerVtable sParameterRootMethods;
static sead::DisposerVtable sParameterObjectMethods;
static sead::DisposerVtable sParameterListMethods;
static sead::SafeTextVtable sTextMethods;
static sead::SafeTextVtable sTextBufferMethods;
static sead::SafeTextVtable sFixedTextBase32Methods;
static sead::SafeTextVtable sFormatText32Methods;
static ParameterMethods sBoolParameterMethods;

/* the texture slot pool's first byte is its core guard */
static AllocatorCoreGuard* coreGuard(ContextSlots* slots) { return reinterpret_cast<AllocatorCoreGuard*>(slots); }

/* inlined into the constructor: placement array construction of a member array */
static void constructMemberArray(void* objects, u32 count, u32 stride, u32 constructor) {
    if (objects == nullptr) objects = allocateObject(count * stride);
    if (objects != nullptr) constructArray(objects, count, stride, constructor);
}

/* 0279D8D8 */
void multi_filter::ParameterRoot::afterApply() {
    // this-adjusting thunk: the parameter root is the multi-filter's second base, at +4
    MultiFilter* owner = gabi::at<MultiFilter>(gabi::ea(this) - offsetof(MultiFilter, mParameterRoot));
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)updateFilterLists), owner);
}

/* inlined: the sampler's native texture (GX2Texture) starts with its surface */
static sead::GraphicsSurface* nativeSurface(TextureSampler* sampler) {
    return gabi::at<sead::GraphicsSurface>(gabi::ea(sampler->nativeTexture));
}

/* 0279D8E0 */
void MultiFilter::releaseResultTexture() {
    gabi::AutoFrame frame{gabi::current_function()};
    TextureCacheEntry* texture = mResultTexture;
    if (texture != nullptr) {
        coreGuard(&mTextureSlots)->release(texture);
        mResultTexture = nullptr;
    }
    coreGuard(&mTextureSlots)->capture();
}

/* 0279D92C */
void MultiFilter::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mMethods = &sMultiFilterMethods;
    mParameterRoot.mTree.mTable = gabi::ea(&sParameterRootMethods);
    for (u32 group = 0; group < kGroupCount; ++group) {
        sead::LinkList* list = &mGroups[group];
        sead::Link* node = list->mNext;
        while (node != list) {
            sead::Link* next = node->mNext;
            reinterpret_cast<FilterListNode*>(node)->mFilter->release();
            node = next;
        }
    }
    releaseResultTexture();
    ResourceRecord* resource = mResource;
    if (resource != nullptr) {
        resource->detach();
        resource = mResource;
        if (resource != nullptr) gabi::call_ptr<void>(resource->mVtable->mDestroy.mFunction, resource, 3);
        mResource = nullptr;
    }
    mDefaultFilter.destroy(2);
    mRootParameters.mVtable = reinterpret_cast<parameter_tree::ValueVtable*>(&sParameterObjectMethods);
    mTextureSlots.destroy(2);
    mOutputBuffer.destroy(2);
    TextureSampler::destroy(&mResultSampler, 2);
    destroyArray(mGroupParameters, kGroupCount, sizeof(parameter_tree::Node), gabi::guest_fn((const void*)destroyGroupParameters), 0, 0);
    if (flags & 1) releaseObject(this);
}

/* 0279DAA0 */
u32 MultiFilter::applyFilter(Filter_l* filter, u32 index, u32 chain) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (filter->mEnabled != 0) chain = filter->apply(&mTarget, chain);
    if (mResultTexture != nullptr) {
        // A debug label "<index>_<type name>" is formatted and dropped (no reader in this build).
        gabi::FrameLocal<sead::SafeText> typeName(8);
        Filter_l::typeName(typeName, filter->mType);
        typeName->assureTermination();
        gabi::FrameLocal<multi_filter::FormatText32> label(0x10);
        gabi::cpu->cr[6] = 0;
        multi_filter::FormatText32::create(label, "%d_%s", index + 1, gabi::ea(typeName->mText.get()));
    }
    return chain;
}

/* inlined into draw: does the sampler already describe this texture's surface? */
static bool sameSurface(const TextureData* sampler, const TextureData* texture) {
    const sead::GraphicsSurface& a = sampler->surface;
    const sead::GraphicsSurface& b = texture->surface;
    return static_cast<u32>(a.mDimension) == static_cast<u32>(b.mDimension) &&
        static_cast<u32>(a.mWidth) == static_cast<u32>(b.mWidth) &&
        static_cast<u32>(a.mHeight) == static_cast<u32>(b.mHeight) &&
        static_cast<u32>(a.mDepth) == static_cast<u32>(b.mDepth) &&
        static_cast<u32>(a.mMipCount) == static_cast<u32>(b.mMipCount) &&
        static_cast<u32>(a.mFormat) == static_cast<u32>(b.mFormat) &&
        static_cast<u32>(a.mSwizzle) == static_cast<u32>(b.mSwizzle) &&
        static_cast<u32>(a.mTileMode) == static_cast<u32>(b.mTileMode) &&
        static_cast<u32>(a.mSamples) == static_cast<u32>(b.mSamples);
}

/* 0279DB64 */
u32 MultiFilter::draw(FilterTarget_l* target, u32 chain) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mEnabled == 0) return chain;
    sead::LinkList* active = &mActiveFilters;
    if ((static_cast<s32>(active->mCount) <= 0 || active->mNext == nullptr) && mDefaultFilter.mEnabled == 0) return chain;
    if (mTextureSlots.resetForCurrentCore() == 0) return chain;
    gabi::FrameLocal<sead::CompactRenderState> state(8);
    sead::CompactRenderState::create(state);
    state->mBlendEnable = 0;
    state->mDepthTest = 0;
    state->apply();
    mTarget = target;
    u32 head = target->mHead;
    TextureSamplerSettings& settings = mResultSampler.settings;
    mResultSampler.componentMapping[0] = 0;
    settings.modes[4] = GX2TexClamp_Clamp;
    mResultSampler.componentMapping[1] = 1;
    u32 dirty = mResultSampler.dirty;
    settings.modes[2] = GX2TexMipFilter_Point;
    settings.modes[3] = GX2TexClamp_Clamp;
    mTargetHead = head;
    mOutputDirty = 0;
    mResultSampler.componentMapping[3] = 5;
    u32 hasDefault = mDefaultFilter.mEnabled;
    settings.modes[1] = GX2TexXYFilter_Linear;
    mResultSampler.dirty = dirty | 7;
    mResultSampler.customMapping = 1;
    settings.modes[0] = GX2TexXYFilter_Linear;
    settings.modes[5] = GX2TexClamp_Clamp;
    mResultSampler.componentMapping[2] = 2;
    u32 index = 0;
    if (hasDefault != 0) chain = applyFilter(&mDefaultFilter, index++, chain);
    for (sead::Link* node = active->mNext; node != active; node = node->mNext) {
        chain = applyFilter(reinterpret_cast<FilterListNode*>(node)->mFilter, index, chain);
        ++index;
    }
    TextureCacheEntry* texture = mResultTexture;
    if (texture != nullptr) {
        u32 filter = mResultSamplerLinear.mValue;
        if (!sameSurface(&mResultSampler.texture, texture)) {
            mResultSampler.copyTexture(texture);
            u32 bits = mResultSampler.dirty;
            settings.modes[0] = filter;
            settings.modes[1] = filter;
            settings.modes[2] = GX2TexMipFilter_Point;
            mResultSampler.dirty = bits | 4;
        } else {
            // Same surface: only the image and mip pointers move (sampler copy and native texture).
            void* image = texture->surface.mImage;
            void* mipmaps = texture->surface.mMipImage;
            settings.modes[0] = filter;
            mResultSampler.texture.surface.mImage = image;
            nativeSurface(&mResultSampler)->mImage = image;
            u32 bits = mResultSampler.dirty;
            nativeSurface(&mResultSampler)->mMipImage = mipmaps;
            settings.modes[1] = filter;
            mResultSampler.dirty = bits | 4;
            settings.modes[2] = GX2TexMipFilter_Point;
            mResultSampler.texture.surface.mMipImage = mipmaps;
        }
    }
    return chain;
}

/* 0279DDA4 */
void MultiFilter::initialize(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<sead::SafeText> typeName(8), groupLabel(0x10), groupName(0x18), rootName(0x4C);
    gabi::FrameLocal<multi_filter::FormatText32> filterName(0x20);
    for (u32 group = 0; group < kGroupCount; ++group) {
        Filter_l::typeName(groupName, group);
        groupName->assureTermination();
        groupLabel->mVtable = &sTextMethods;
        groupLabel->mText = groupName->mText.get();
        mParameterRoot.mTree.appendChild(&mGroupParameters[group], groupLabel);
        for (u32 slot = 0; slot < kFiltersPerGroup; ++slot) {
            Filter_l* filter = Filter_l::createForGroup(group, slot, heap);
            filter->attach(this, &mGroups[group], heap);
            parameter_tree::Node* parameters = filter == nullptr ? nullptr : &filter->mParameters;
            Filter_l::typeName(typeName, group);
            typeName->assureTermination();
            gabi::cpu->cr[6] = 0;
            multi_filter::FormatText32* name = multi_filter::FormatText32::create(filterName, "%s_%d",
                gabi::ea(typeName->mText.get()), slot);
            mGroupParameters[group].appendChild(parameters, name);
            mFilters[group][slot] = filter;
        }
    }
    mTextureSlots.allocate(128, heap);
    rootName->mVtable = &sTextMethods;
    rootName->mText.v = gabi::guest_str("mf_root_param");
    mParameterRoot.mTree.appendValue(&mRootParameters, rootName);
}

/* 0279DFE0 */
MultiFilter* MultiFilter::create(MultiFilter* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    MultiFilter* filter = storage;
    if (filter == nullptr) filter = static_cast<MultiFilter*>(allocateObject(sizeof(MultiFilter)));
    if (filter == nullptr) return filter;
    filter->mMethods = &sMultiFilterBaseMethods;
    gabi::FrameLocal<sead::SafeText> rootName(8), linearName(0x10), linearLabel(0x18), linearMeta(0x28), displayName(0x20);
    rootName->mVtable = &sTextMethods;
    rootName->mText.v = gabi::guest_str("aglmf");
    ParameterRoot::create(&filter->mParameterRoot, rootName, 0);
    sead::LinkList* active = &filter->mActiveFilters;
    if (active == nullptr) active = static_cast<sead::LinkList*>(allocateObject(sizeof(sead::LinkList)));
    if (active != nullptr) { active->mNext = active; active->mCount = 0; active->mPrevious = active; }
    filter->mMethods = &sMultiFilterMethods;
    filter->mParameterRoot.mTree.mTable = gabi::ea(&sParameterRootMethods);
    constructMemberArray(filter->mGroups, kGroupCount, sizeof(sead::LinkList),
        gabi::guest_fn(reinterpret_cast<const void*>(&multi_filter::FilterList::create)));
    constructMemberArray(filter->mGroupParameters, kGroupCount, sizeof(parameter_tree::Node),
        gabi::guest_fn(reinterpret_cast<const void*>(&multi_filter::ParameterList::create)));
    if (gabi::ea(filter->mFilters) == 0) allocateObject(sizeof(filter->mFilters));
    TextureSampler::create(&filter->mResultSampler);
    filter->mOutputBuffer.construct();
    ColorSurface* color = &filter->mOutputColor;
    color->construct();
    filter->mTextureSlots.create(&filter->mTextureSlots);
    filter->mResource = nullptr;
    filter->mPassCount = 0;
    filter->mSuspended = 0;
    filter->mEnabled = 1;
    constructParameterObject(&filter->mRootParameters);
    linearName->mVtable = &sTextMethods;
    linearLabel->mVtable = &sTextMethods;
    // label: "set the result's texture filter to Linear"
    linearLabel->mText.v = gabi::guest_str("\214\213\211\312\202\314\203e\203N\203X\203`\203\203\203t\203B\203\213\203^\202\360Linear\202\311\202\267\202\351");
    linearName->mText.v = gabi::guest_str("result_sampler_linear");
    BoolParameter* linear = &filter->mResultSamplerLinear;
    if (linear == nullptr) linear = static_cast<BoolParameter*>(allocateObject(sizeof(BoolParameter)));
    if (linear != nullptr) {
        ParameterBase::create(&linear->mBase);
        linear->mBase.mMethods = reinterpret_cast<sead::VtableSlot*>(&sBoolParameterMethods);
        linearMeta->mVtable = &sTextMethods;
        linearMeta->mText.v = gabi::guest_str("");
        linear->mBase.initialize(linearName, linearLabel, linearMeta, &filter->mRootParameters);
        linear->mValue = 1;
    }
    Filter_l::create(&filter->mDefaultFilter, -1);
    displayName->mVtable = &sTextMethods;
    displayName->mText.v = gabi::guest_str("Icon=LIGHT");
    setDisplayName(filter, displayName);
    filter->mResultTexture = nullptr;
    filter->mResultSampler.overrideEnabled = 1;
    filter->mOutputBuffer.mColors[0] = color;
    return filter;
}

/* HD: compiler-generated and template code of this translation unit */

/* 0279E1F0 */
void multi_filter::Text::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E204 */
void multi_filter::FixedTextBase32::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E218 */
void multi_filter::FixedTextA::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E22C */
void multi_filter::ParameterList::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    mTable = gabi::ea(&sParameterListMethods);
    if (flags & 1) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E24C */
void multi_filter::Text::assureTerminationImpl() {}

/* 0279E250 */
void multi_filter::TextBuffer::assureTerminationImpl() {
    mText[static_cast<s32>(mCapacity) - 1] = 0;
}

/* 0279E268 */
void multi_filter::TextBuffer::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E27C */
void multi_filter::FixedTextB::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E290 */
void multi_filter::FixedTextF::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E2A4 */
void multi_filter::FixedTextG::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E2B8 */
void multi_filter::FixedTextC::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E2CC */
void multi_filter::FixedTextD::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E2E0 */
void multi_filter::FixedTextE::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E2F4 */
multi_filter::FilterList* multi_filter::FilterList::create(FilterList* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    FilterList* list = storage;
    if (list == nullptr) list = static_cast<FilterList*>(allocateObject(sizeof(sead::LinkList)));
    if (list != nullptr) {
        list->mPrevious = list;
        list->mNext = list;
        list->mCount = 0;
    }
    return list;
}

/* 0279E338 */
multi_filter::FormatText32* multi_filter::FormatText32::create(FormatText32* storage, const char* format, u32 first, u32 second) {
    // Variadic: the prologue saves r3-r10 (and f1-f8 when cr1.eq says floats were passed).
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    const u32 saved[8] = {gabi::ea(storage), gabi::ea(format), first, second,
                          gabi::cpu->r[7], gabi::cpu->r[8], gabi::cpu->r[9], gabi::cpu->r[10]};
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<ReportSave> save(8);
    for (u32 index = 0; index < 8; ++index) save->mIntegers[index] = saved[index];
    if (floats) for (u32 index = 0; index < 8; ++index) {
        u64 bits;
        memcpy(&bits, &gabi::cpu->f[index + 1].ps0, sizeof(bits));
        save->mFloatWords[index * 2] = bits >> 32;
        save->mFloatWords[index * 2 + 1] = bits;
    }
    FormatText32* text = storage;
    if (text == nullptr) text = static_cast<FormatText32*>(allocateObject(sizeof(FormatText32)));
    if (text != nullptr) {
        text->mText = text->mStorage;
        text->mVtable = &sTextBufferMethods;
        text->mCapacity = 32;
        text->mStorage[31] = 0;
        text->mVtable = &sFixedTextBase32Methods;
        text->mText[0] = 0;
        text->mVtable = &sFormatText32Methods;
        save->mArguments.mIntegerIndex = 2;
        save->mArguments.mOverflow = entry + 8;
        save->mArguments.mSavedRegisters = gabi::ea(save->mIntegers);
        save->mArguments.mFloatIndex = 0;
        formatText(text, format, &save->mArguments);
    }
    return text;
}

/* 0279E458 */
void multi_filter::FormatText32::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E46C */
void multi_filter::FixedTextH::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E480 */
void multi_filter::FixedTextI::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E494 */
void multi_filter::FixedTextJ::destroy(u32 flags) {
    if (gabi::ea(this) != 0 && (flags & 1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseObject), this);
}

/* 0279E4A8 */
void BoolParameter::onResourceApplied(const be<u8>* data, u32 size) {}

/* 0279E4AC */
u32 BoolParameter::valueKind() { return ParameterType_Bool; }

/* 0279E4B4 */
const be<u8>* BoolParameter::constValue() { return &mValue; }

/* 0279E4BC */
be<u8>* BoolParameter::value() { return &mValue; }

/* 0279E4C4 */
u32 BoolParameter::valueBytes() { return sizeof(bool); }

/* 0279E4CC */
u32 multi_filter::ParameterObject::constantOneHook() { return 1; }

/* 0279E4D4 */
void multi_filter::ParameterObject::emptyHook() {}

/* 0279E4D8 */
u32 multi_filter::ParameterObject::alternateConstantOneHook() { return 1; }

/* 0279E4E0 */
void multi_filter::ParameterObject::alternateEmptyHook() {}

/* 0279E4E4 */
u32 multi_filter::ParameterObject::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) {
    parameter_tree::MatchRecord* input = *record;
    u32 nameHash = mNameHash;
    return static_cast<u32>(input->mComparisonValue) == nameHash;
}

/* 0279E500 */
u32 multi_filter::ParameterList::constantOneHook() { return 1; }

/* 0279E508 */
void multi_filter::ParameterList::emptyHook() {}

/* 0279E50C */
u32 multi_filter::ParameterList::matchesRecord(const gptr<parameter_tree::MatchRecord>* record) {
    parameter_tree::MatchRecord* input = *record;
    u32 nameHash = mNameHash;
    return static_cast<u32>(input->mSecondaryValue) == nameHash;
}

/* 0279E528 */
void multi_filter::ParameterList::trailingEmptyHook() {}

/* 0279E52C */
void multi_filter::ParameterList::alternateTrailingEmptyHook() {}

/* 0279E530 */
multi_filter::ParameterList* multi_filter::ParameterList::create(ParameterList* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    ParameterList* list = storage;
    if (list == nullptr) list = static_cast<ParameterList*>(allocateObject(sizeof(parameter_tree::Node)));
    if (list != nullptr) parameter_tree::Node::create(list);
    return list;
}

/* 0279E578 */
void multi_filter::ParameterRoot::onContextMismatch(const void* input) {}

/* WWHD glue
ext 0273AD10 void* allocateObject(u32 size)
ext 0273AF40 void releaseObject(void* object) callback
ext 0279D7D4 void updateFilterLists(MultiFilter* filter) callback
ext 0279E22C void destroyGroupParameters(multi_filter::ParameterList* list, u32 flags) callback
ext 028EFFD0 void constructArray(void* objects, u32 count, u32 stride, u32 constructor)
ext 028F0164 void destroyArray(void* objects, u32 count, u32 stride, u32 destructor, u32 flags, u32 reserved)
ext 02785CB4 void setDisplayName(MultiFilter* filter, sead::SafeText* name)
ext 02759C10 s32 formatText(sead::SafeTextBuffer* text, const char* format, ReportArguments* arguments)
ext 0279A07C agl::ParameterRoot* agl::ParameterRoot::create(ParameterRoot* root, sead::SafeText* name, u32 context)
ext 0279CA34 agl::parameter_tree::Node* agl::parameter_tree::Node::create(Node* node)
ext 0279CB24 void agl::parameter_tree::Node::appendChild(Node* child, sead::SafeText* name)
ext 0279CB9C void agl::parameter_tree::Node::appendValue(Value* value, sead::SafeText* name)
ext 027A84B8 parameter_tree::Value* constructParameterObject(parameter_tree::Value* storage)
ext 0279A95C agl::ContextSlots* agl::ContextSlots::create(ContextSlots* slots)
ext 0279AA14 void agl::ContextSlots::destroy(u32 flags)
ext 0279AAFC void agl::ContextSlots::allocate(s32 count, sead::RegistryHeap* heap)
ext 0279ABB4 u32 agl::ContextSlots::resetForCurrentCore()
ext 0279AE68 void agl::AllocatorCoreGuard::release(void* storage)
ext 0279AE88 void agl::AllocatorCoreGuard::capture()
ext 027ABC60 agl::Filter_l* agl::Filter_l::create(Filter_l* storage, s32 type)
ext 027ADE00 void agl::Filter_l::destroy(u32 flags)
ext 027ABDD0 agl::Filter_l* agl::Filter_l::createForGroup(u32 group, u32 slot, sead::RegistryHeap* heap)
ext 027AC144 void agl::Filter_l::attach(MultiFilter* owner, sead::LinkList* list, sead::RegistryHeap* heap)
ext 027AC294 void agl::Filter_l::release()
ext 027ABF8C void agl::Filter_l::typeName(sead::SafeText* result, u32 type)
ext 027BDF7C agl::TextureSampler* agl::TextureSampler::create(TextureSampler*)
ext 027BE2B0 void agl::TextureSampler::destroy(TextureSampler*, u32 flags)
ext 027BDEB4 void agl::TextureSampler::copyTexture(const TextureData* source)
ext 027B95B4 agl::RenderBuffer* agl::RenderBuffer::construct()
ext 027B9694 void agl::RenderBuffer::destroy(u32 flags)
ext 027B6CB8 agl::ColorSurface* agl::ColorSurface::construct()
ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
ext 0274FEB0 void sead::CompactRenderState::apply()
data sMultiFilterBaseMethods 1016799C
data sMultiFilterMethods 10167C38
data sParameterRootMethods 10167C60
data sParameterObjectMethods 10167A6C
data sParameterListMethods 10167AAC
data sTextMethods 1016795C
data sTextBufferMethods 10167974
data sFixedTextBase32Methods 101679DC
data sFormatText32Methods 10167AEC
data sBoolParameterMethods 10167B4C
str 10167BC4 "%d_%s" in 0279DAA0
str 10167BCC "%s_%d" in 0279DDA4
str 10167BD4 "mf_root_param" in 0279DDA4
str 10167BE4 "aglmf" in 0279DFE0
str 10167BEC "result_sampler_linear" in 0279DFE0
str 10167C04 in 0279DFE0
str 10167958 "" in 0279DFE0
str 10167C2C "Icon=LIGHT" in 0279DFE0
*/
