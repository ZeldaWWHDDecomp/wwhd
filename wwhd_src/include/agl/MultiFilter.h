// agl multi-filter ("aglmf" in the binary): seven groups of four filter stages that render onto a target,
// the result sampler and its "result_sampler_linear" switch, and the parameter root ("mf_root_param") the
// stages register in. The translation unit starts in AGL#9 (its parameter-apply hook 0279D7D4) and also
// emits the text, parameter-list and bool-parameter copies declared below.
#pragma once
#include "agl/ParameterBase.h"
#include "agl/ParameterRoot.h"
#include "agl/ContextSlots.h"
#include "agl/TextureResource.h"
#include "agl/TextureSampler.h"
#include "agl/RenderBuffer.h"
#include "sead/RenderState.h"
namespace agl {

// The parameter tree (parameter_tree::Node / Value / MatchRecord), ParameterRoot and the texture slot pool
// (ContextSlots, whose first byte is the AllocatorCoreGuard) are AGL#9's canonical types.

// ---- a filter stage (AGL#12's class): only the members this unit touches ----
struct FilterMethods_l { sead::VtableSlot mHeader, mDestroy, mReservedA, mReservedB, mApply; };
struct FilterTarget_l { be<u32> mHead; };
struct Filter_l {
    u8 mBase[0x14];
    parameter_tree::Node mParameters;
    u8 mStateA[0x64];
    be<u32> mType;
    u8 mStateB[0x10];
    be<u8> mEnabled;
    u8 mStateC[0x23];
    gptr<FilterMethods_l> mMethods;
    u8 mStateD[0x28];
    static Filter_l* create(Filter_l* storage, s32 type);
    void destroy(u32 flags);
    static Filter_l* createForGroup(u32 group, u32 slot, sead::RegistryHeap* heap);
    void attach(struct MultiFilter* owner, sead::LinkList* list, sead::RegistryHeap* heap);
    void release();
    static void typeName(sead::SafeText* result, u32 type);
    u32 apply(gptr<FilterTarget_l>* target, u32 chain) {
        return gabi::call_ptr<u32>(mMethods->mApply.mFunction, this, target, chain);
    }
};

// list node of a filter group (sead list of filters)
struct FilterListNode { sead::Link mLink; gptr<Filter_l> mFilter; };

// GX2 sampler modes the result sampler is set to (wut facts)
enum : u32 { GX2TexXYFilter_Linear = 1, GX2TexMipFilter_Point = 1, GX2TexClamp_Clamp = 2 };

// bool parameter (agl::Parameter<bool>) as instantiated in this translation unit
struct BoolParameter {
    ParameterBase mBase;
    be<u8> mValue;
    u8 mPadding[3];
    u32 valueKind();
    const be<u8>* constValue();
    be<u8>* value();
    u32 valueBytes();
    void onResourceApplied(const be<u8>* data, u32 size);
};

struct MultiFilter {
    static constexpr u32 kGroupCount = 7;
    static constexpr u32 kFiltersPerGroup = 4;
    gptr<const void> mMethods;
    ParameterRoot mParameterRoot;
    sead::LinkList mActiveFilters;
    sead::LinkList mGroups[kGroupCount];
    parameter_tree::Node mGroupParameters[kGroupCount];
    gptr<Filter_l> mFilters[kGroupCount][kFiltersPerGroup];
    gptr<FilterTarget_l> mTarget;
    gptr<TextureCacheEntry> mResultTexture;
    be<u32> mTargetHead;
    TextureSampler mResultSampler;
    RenderBuffer mOutputBuffer;
    ColorSurface mOutputColor;
    be<u8> mOutputDirty;
    u8 mPaddingA[3];
    ContextSlots mTextureSlots;
    u8 mReserved[0x14];
    be<u32> mPassCount;
    be<u8> mEnabled;
    be<u8> mSuspended;
    u8 mPaddingB[2];
    gptr<ResourceRecord> mResource;
    parameter_tree::Value mRootParameters;
    BoolParameter mResultSamplerLinear;
    Filter_l mDefaultFilter;

    static MultiFilter* create(MultiFilter* storage);
    void destroy(u32 flags);
    void releaseResultTexture();
    u32 applyFilter(Filter_l* filter, u32 index, u32 chain);
    u32 draw(FilterTarget_l* target, u32 chain);
    void initialize(sead::RegistryHeap* heap);
    // the multi-filter's own parameter-apply hook (AGL#9 range)
    void updateFilterLists();
};

// ---- this translation unit's copies of inline classes: only their out-of-line virtuals ----
namespace multi_filter {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBuffer : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct FixedTextBase32 : sead::FixedSafeText<32> { void destroy(u32 flags); };
struct FixedTextA : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextC : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextD : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextE : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextF : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextG : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextH : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextI : sead::SafeTextBuffer { void destroy(u32 flags); };
struct FixedTextJ : sead::SafeTextBuffer { void destroy(u32 flags); };
// sead::FormatFixedSafeString<32>: printf-style construction
struct FormatText32 : sead::FixedSafeText<32> {
    static FormatText32* create(FormatText32* storage, const char* format, u32 first, u32 second);
    void destroy(u32 flags);
};
// the filter groups' list heads (constructed through __construct_array)
struct FilterList : sead::LinkList { static FilterList* create(FilterList* storage); };
// parameter list (the group parameters and the root's list) and parameter object hooks of this unit
struct ParameterList : parameter_tree::Node {
    static ParameterList* create(ParameterList* storage);
    void destroy(u32 flags);
    u32 constantOneHook();
    void emptyHook();
    u32 matchesRecord(const gptr<parameter_tree::MatchRecord>* record);
    void trailingEmptyHook();
    void alternateTrailingEmptyHook();
};
struct ParameterObject : parameter_tree::Value {
    u32 constantOneHook();
    void emptyHook();
    u32 alternateConstantOneHook();
    void alternateEmptyHook();
    u32 matchesRecord(const gptr<parameter_tree::MatchRecord>* record);
};
struct ParameterRoot : ::agl::ParameterRoot {
    void afterApply();
    void onContextMismatch(const void* input);
};
}
}
WWHD_OFFSET(agl::FilterMethods_l, mApply, 0x20);
WWHD_OFFSET(agl::Filter_l, mParameters, 0x14);
WWHD_OFFSET(agl::Filter_l, mType, 0xDC);
WWHD_OFFSET(agl::Filter_l, mEnabled, 0xF0);
WWHD_OFFSET(agl::Filter_l, mMethods, 0x114);
WWHD_SIZE(agl::Filter_l, 0x140);
WWHD_OFFSET(agl::FilterListNode, mFilter, 8);
WWHD_OFFSET(agl::BoolParameter, mValue, 12);
WWHD_SIZE(agl::BoolParameter, 0x10);
WWHD_OFFSET(agl::MultiFilter, mParameterRoot, 4);
WWHD_OFFSET(agl::MultiFilter, mActiveFilters, 0x1CC);
WWHD_OFFSET(agl::MultiFilter, mGroups, 0x1D8);
WWHD_OFFSET(agl::MultiFilter, mGroupParameters, 0x22C);
WWHD_OFFSET(agl::MultiFilter, mFilters, 0x4E8);
WWHD_OFFSET(agl::MultiFilter, mTarget, 0x558);
WWHD_OFFSET(agl::MultiFilter, mResultTexture, 0x55C);
WWHD_OFFSET(agl::MultiFilter, mTargetHead, 0x560);
WWHD_OFFSET(agl::MultiFilter, mResultSampler, 0x564);
WWHD_OFFSET(agl::MultiFilter, mOutputBuffer, 0x6FC);
WWHD_OFFSET(agl::MultiFilter, mOutputColor, 0x73C);
WWHD_OFFSET(agl::MultiFilter, mOutputDirty, 0x884);
WWHD_OFFSET(agl::MultiFilter, mTextureSlots, 0x888);
WWHD_OFFSET(agl::MultiFilter, mPassCount, 0x8B8);
WWHD_OFFSET(agl::MultiFilter, mEnabled, 0x8BC);
WWHD_OFFSET(agl::MultiFilter, mResource, 0x8C0);
WWHD_OFFSET(agl::MultiFilter, mRootParameters, 0x8C4);
WWHD_OFFSET(agl::MultiFilter, mResultSamplerLinear, 0x920);
WWHD_OFFSET(agl::MultiFilter, mDefaultFilter, 0x930);
WWHD_SIZE(agl::MultiFilter, 0xA70);
