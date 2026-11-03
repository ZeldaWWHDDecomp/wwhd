// Cascade-shadow configuration construction reconstructed from assigned WWHD instructions.
#include "bindings.h"
#include "agl/CascadeShadow.h"
#include "sead/HeapRegistry.h"
void* allocateCascadeObject(u32);
void constructShadowParameterObject(agl::parameter_tree::Value*);
void initializeCascadeParameterBase(agl::CascadeScalarPrefix*);
void describeCascadeParameter(agl::CascadeScalarPrefix*, sead::SafeText*, sead::SafeText*, sead::SafeText*, agl::parameter_tree::Value*);
void initializeCascadeMap(agl::CascadeMapStorage_l*);
void initializeCascadeSupport(agl::CascadeSupportStorage_l*);
u32 matchesCascadeScope(agl::ShadowUnit_l*, void*);
u32 matchesAlternateCascadeScope(agl::ShadowUnit_l*, void*);
u32 gatherCascadeBounds(agl::CascadeShadow*, void*, u32);
u32 gatherAlternateCascadeBounds(agl::CascadeShadow*, void*, u32);
static u32 sCascadeTextTable;
static u32 sCascadeRootTable;
static u32 sCascadeBoundsTable;
static u32 sCascadeIntegerTable;
static u32 sCascadePairBaseTable;
static u32 sCascadePairTable;
static u32 sCascadeBooleanTable;
static u32 sCascadeFloatTable;
static u32 sCascadeParameterObjectTable;
void releaseCascadeMapResources(agl::CascadeMapStorage_l*);
void destroyCascadeMap(agl::CascadeMapStorage_l*, u32);
sead::RegistryHeap* findCascadeArrayHeap(sead::RegistryManagerState*, void*);
static u8 sDefaultCascadeHint = 0;
static u8 sPairCascadeHint = 0;
static u8 sBooleanCascadeHint = 0;
static void setCascadeText(sead::SafeText* text, const char* value) {
    text->mText = gabi::at<be<u8>>(gabi::guest_str(value));
    text->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sCascadeTextTable);
}
static void setCascadeHint(sead::SafeText* text, be<u8>* value) {
    text->mText = value;
    text->mVtable = reinterpret_cast<sead::SafeTextVtable*>(&sCascadeTextTable);
}
// Each embedded parameter keeps the GHS null-preserving allocation path.
static agl::CascadeScalarPrefix* initializeCascadeParameter(agl::CascadeScalarPrefix* parameter,
        u32 size, u32 table, sead::SafeText* key, sead::SafeText* label, sead::SafeText* hint,
        agl::parameter_tree::Value* owner, be<u8>* deferredHint) {
    if (parameter == NULL) parameter = (agl::CascadeScalarPrefix*)allocateCascadeObject(size);
    if (parameter != NULL) {
        initializeCascadeParameterBase(parameter);
        parameter->mTable = table;
        if (deferredHint != NULL) setCascadeHint(hint, deferredHint);
        describeCascadeParameter(parameter, key, label, hint, owner);
    }
    return parameter;
}
/* 02797EEC */
agl::CascadeShadow* agl::CascadeShadow::create(CascadeShadow* shadow) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (shadow == NULL) shadow = (CascadeShadow*)allocateCascadeObject(sizeof(CascadeShadow));
    if (shadow == NULL) return shadow;
    gabi::FrameLocal<sead::SafeText> rootName(8);
    setCascadeText(rootName.get(), "aglsdw");
    ParameterRoot::create(&shadow->mRoot, rootName.get(), 0);
    shadow->mFirstCount = 0; shadow->mSecondCount = 0; shadow->mFirstIndex = -1;
    shadow->mBoundsTable = gabi::ea(&sCascadeBoundsTable);
    shadow->mRoot.mTree.mTable = gabi::ea(&sCascadeRootTable);
    shadow->mSecondBoundsCallback = gabi::guest_fn((const void*)gatherAlternateCascadeBounds);
    shadow->mSecondIndex = -1;
    shadow->mFirstBoundsCallback = gabi::guest_fn((const void*)gatherCascadeBounds);
    constructShadowParameterObject(&shadow->mParameters);

    gabi::FrameLocal<sead::SafeText> boundingKey(0x10), boundingLabel(0x18), boundingHint(0xB0);
    setCascadeText(boundingKey.get(), "bounding_calc_type");
    setCascadeText(boundingLabel.get(), "Bounding Calc Type");
    CascadeScalarPrefix* parameter = initializeCascadeParameter(
        gabi::at<CascadeScalarPrefix>(gabi::ea(shadow) + offsetof(CascadeShadow, mBoundingCalculation)),
        sizeof(CascadeWordParameter), gabi::ea(&sCascadeIntegerTable), boundingKey.get(), boundingLabel.get(),
        boundingHint.get(), &shadow->mParameters, gabi::at<be<u8>>(gabi::ea(&sDefaultCascadeHint)));
    if (parameter != NULL) reinterpret_cast<CascadeWordParameter*>(parameter)->mValue = 1;

    gabi::FrameLocal<sead::SafeText> countKey(0x20), countLabel(0x28), countHint(0xB8);
    setCascadeText(countKey.get(), "cascade_num"); setCascadeText(countLabel.get(), "Cascade Num");
    parameter = initializeCascadeParameter(
        gabi::at<CascadeScalarPrefix>(gabi::ea(shadow) + offsetof(CascadeShadow, mCascadeCount)),
        sizeof(CascadeWordParameter), gabi::ea(&sCascadeIntegerTable), countKey.get(), countLabel.get(),
        countHint.get(), &shadow->mParameters, gabi::at<be<u8>>(gabi::ea(&sDefaultCascadeHint)));
    if (parameter != NULL) reinterpret_cast<CascadeWordParameter*>(parameter)->mValue = 1;

    gabi::FrameLocal<sead::SafeText> nearKey(0x30), nearLabel(0x38), nearHint(0xC0);
    setCascadeText(nearKey.get(), "cascade_near"); setCascadeText(nearLabel.get(), "cascade_near");
    parameter = initializeCascadeParameter(
        gabi::at<CascadeScalarPrefix>(gabi::ea(shadow) + offsetof(CascadeShadow, mCascadeNear)),
        sizeof(CascadeArrayParameter), gabi::ea(&sCascadePairBaseTable), nearKey.get(), nearLabel.get(),
        nearHint.get(), &shadow->mParameters, gabi::at<be<u8>>(gabi::ea(&sPairCascadeHint)));
    if (parameter != NULL) {
        auto* pair = reinterpret_cast<CascadeArrayParameter*>(parameter);
        pair->mData = NULL; pair->mCount = 0; pair->mTable = gabi::ea(&sCascadePairTable);
    }

    gabi::FrameLocal<sead::SafeText> clampKey(0x40), clampLabel(0x48), clampHint(0xC8);
    setCascadeText(clampKey.get(), "depth_clamp"); setCascadeText(clampLabel.get(), "Depth Clamp");
    parameter = initializeCascadeParameter(
        gabi::at<CascadeScalarPrefix>(gabi::ea(shadow) + offsetof(CascadeShadow, mDepthClamp)),
        sizeof(CascadeBoolParameter), gabi::ea(&sCascadeBooleanTable), clampKey.get(), clampLabel.get(),
        clampHint.get(), &shadow->mParameters, gabi::at<be<u8>>(gabi::ea(&sBooleanCascadeHint)));
    if (parameter != NULL) reinterpret_cast<CascadeBoolParameter*>(parameter)->mValue = 1;

    gabi::FrameLocal<sead::SafeText> farKey(0x50), farLabel(0x58), farHint(0x60);
    setCascadeText(farKey.get(), "optimize_offset_far"); setCascadeText(farLabel.get(), "Optimize Offset Far");
    setCascadeText(farHint.get(), "Min=0");
    parameter = initializeCascadeParameter(
        gabi::at<CascadeScalarPrefix>(gabi::ea(shadow) + offsetof(CascadeShadow, mFarOffset)),
        sizeof(CascadeFloatParameter), gabi::ea(&sCascadeFloatTable), farKey.get(), farLabel.get(), farHint.get(),
        &shadow->mParameters, NULL);
    if (parameter != NULL) reinterpret_cast<CascadeFloatParameter*>(parameter)->mValue = 0.0f;

    gabi::FrameLocal<sead::SafeText> offsetKey(0x68), offsetLabel(0x70), offsetHint(0x78);
    setCascadeText(offsetKey.get(), "optimize_offset_near"); setCascadeText(offsetLabel.get(), "Optimize Offset Near");
    setCascadeText(offsetHint.get(), "Min=0");
    parameter = initializeCascadeParameter(
        gabi::at<CascadeScalarPrefix>(gabi::ea(shadow) + offsetof(CascadeShadow, mNearOffset)),
        sizeof(CascadeFloatParameter), gabi::ea(&sCascadeFloatTable), offsetKey.get(), offsetLabel.get(), offsetHint.get(),
        &shadow->mParameters, NULL);
    if (parameter != NULL) reinterpret_cast<CascadeFloatParameter*>(parameter)->mValue = 0.0f;

    gabi::FrameLocal<sead::SafeText> widthKey(0x80), widthLabel(0x88), widthHint(0x90);
    setCascadeText(widthKey.get(), "stable_texel_width"); setCascadeText(widthLabel.get(), "Stable Texel Width");
    setCascadeText(widthHint.get(), "Min=1, Max=32");
    parameter = initializeCascadeParameter(
        gabi::at<CascadeScalarPrefix>(gabi::ea(shadow) + offsetof(CascadeShadow, mStableTexelWidth)),
        sizeof(CascadeWordParameter), gabi::ea(&sCascadeIntegerTable), widthKey.get(), widthLabel.get(), widthHint.get(),
        &shadow->mParameters, NULL);
    if (parameter != NULL) reinterpret_cast<CascadeWordParameter*>(parameter)->mValue = 1;

    gabi::FrameLocal<sead::SafeText> epsilonKey(0x98), epsilonLabel(0xA0), epsilonHint(0xA8);
    setCascadeText(epsilonKey.get(), "same_point_epsilon"); setCascadeText(epsilonLabel.get(), "Same Point Epsilon");
    setCascadeText(epsilonHint.get(), "Min=0, Max=1");
    parameter = initializeCascadeParameter(
        gabi::at<CascadeScalarPrefix>(gabi::ea(shadow) + offsetof(CascadeShadow, mPointEpsilon)),
        sizeof(CascadeFloatParameter), gabi::ea(&sCascadeFloatTable), epsilonKey.get(), epsilonLabel.get(), epsilonHint.get(),
        &shadow->mParameters, NULL);
    if (parameter != NULL) reinterpret_cast<CascadeFloatParameter*>(parameter)->mValue = 0.05f;

    ShadowUnitBuffer* units = gabi::at<ShadowUnitBuffer>(gabi::ea(shadow) + offsetof(CascadeShadow, mUnits));
    if (units == NULL) units = (ShadowUnitBuffer*)allocateCascadeObject(sizeof(ShadowUnitBuffer));
    if (units != NULL) { units->mData = NULL; units->mCount = 0; }
    initializeCascadeMap(&shadow->mMap);
    shadow->mMapFlag = 1;
    initializeCascadeSupport(&shadow->mSupport);
    shadow->mSupport.mIndexCallback = NULL;
    be<u32>* reset = gabi::at<be<u32>>(gabi::ea(shadow) + offsetof(CascadeShadow, mResetWord));
    if (reset == NULL) reset = (be<u32>*)allocateCascadeObject(sizeof(be<u32>));
    if (reset != NULL) *reset = 0;
    return shadow;
}
/* 027983D4 */
void agl::CascadeShadow::releaseMap() {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseCascadeMapResources(&mMap);
}
/* 027983DC */
void agl::CascadeShadow::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mRoot.mTree.mTable = gabi::ea(&sCascadeRootTable);
    mBoundsTable = gabi::ea(&sCascadeBoundsTable);
    releaseMap();
    ShadowUnit_l* data = mUnits.mData;
    if (data != NULL) {
        s32 index = 0;
        while (index < (s32)(u32)mUnits.mCount) {
            ShadowUnit_l* unit = gabi::at<ShadowUnit_l>(gabi::ea(data) + (u32)index * sizeof(ShadowUnit_l));
            gabi::call_ptr<void>(unit->mVtable->mRelease, unit, 2);
            ++index;
            u32 count = mUnits.mCount;
            data = mUnits.mData;
            if (index >= (s32)count) break;
        }
        sead::RegistryHeap* heap = findCascadeArrayHeap(sRegistryManager, data);
        void* storage = mUnits.mData;
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage);
        mUnits.mCount = 0; mUnits.mData = NULL;
    }
    destroyCascadeMap(&mMap, 2);
    CascadeArrayParameter* near = gabi::at<CascadeArrayParameter>(gabi::ea(this) + offsetof(CascadeShadow, mCascadeNear));
    if (near != NULL) {
        void* storage = near->mData;
        near->mTable = gabi::ea(&sCascadePairTable);
        if (storage != NULL) {
            operator_delete(storage);
            near->mCount = 0; near->mData = NULL;
        }
    }
    mParameters.mVtable = reinterpret_cast<parameter_tree::ValueVtable*>(&sCascadeParameterObjectTable);
    if (flags & 1) operator_delete(this);
}
static agl::ShadowUnit_l* cascadeUnitAt(agl::CascadeShadow* shadow, u32 index) {
    u32 count = shadow->mUnits.mCount;
    agl::ShadowUnit_l* data = shadow->mUnits.mData;
    return gabi::at<agl::ShadowUnit_l>(gabi::ea(data) + (index < count ? index * sizeof(agl::ShadowUnit_l) : 0));
}
static void collectCascadeMemberBounds(agl::ShadowUnit_l* unit, bool alternate,
                                      agl::CascadeBounds* bounds, void* context) {
    const sead::MemberSlot& method = alternate ? unit->mFirstBounds : unit->mSecondBounds;
    s16 index = method.mVirtualIndex;
    s16 adjustment = method.mObjectAdjustment;
    be<u8>* object = gabi::at<be<u8>>(gabi::ea(unit) + (s32)adjustment);
    u32 target;
    if (index < 0) target = method.mTarget;
    else {
        s16 tableOffset = (u32)method.mTarget;
        sead::VirtualEntry* table = *gabi::at<gptr<sead::VirtualEntry>>(gabi::ea(object) + (s32)tableOffset);
        target = table[index].mTarget;
    }
    gabi::call_ptr<void>(target, object, bounds, context);
}
static u32 gatherCascadeBoundsImpl(agl::CascadeShadow* shadow, void* context, u32 lane,
                                  bool alternate, agl::CascadeBounds* bounds) {
    for (u32 axis = 0; axis < 3; ++axis) {
        bounds->mMinimum[axis] = 0x1.fffffep127f;
        bounds->mMaximum[axis] = -0x1.fffffep127f;
    }
    u32 mask = 0;
    bool collected = false;
    s32 index = 0;
    if (index >= (s32)(u32)shadow->mCascadeCount.mValue) return mask;
    do {
        agl::ShadowUnit_l* unit = cascadeUnitAt(shadow, (u32)index);
        u32 included = alternate ? matchesAlternateCascadeScope(unit, context) : matchesCascadeScope(unit, context);
        if (included != 0) {
            if (!collected) {
                unit = cascadeUnitAt(shadow, (u32)index);
                collectCascadeMemberBounds(unit, alternate, bounds, context);
                collected = true;
            }
            unit = cascadeUnitAt(shadow, (u32)index);
            agl::CascadeBounds* destination = &unit->mBounds[lane < 3 ? lane : 0];
            for (u32 axis = 0; axis < 3; ++axis) {
                f32 value = bounds->mMinimum[axis];
                if ((f32)destination->mMinimum[axis] > value) destination->mMinimum[axis] = value;
            }
            for (u32 axis = 0; axis < 3; ++axis) {
                f32 value = bounds->mMaximum[axis];
                if ((f32)destination->mMaximum[axis] < value) destination->mMaximum[axis] = value;
            }
            u32 shift = (u32)index & 63;
            if (shift < 32) mask |= 1u << shift;
        }
        ++index;
    } while (index < (s32)(u32)shadow->mCascadeCount.mValue);
    return mask;
}
/* 027991D0 */
u32 gatherCascadeBounds(agl::CascadeShadow* shadow, void* context, u32 lane) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<agl::CascadeBounds> bounds(8);
    return gatherCascadeBoundsImpl(shadow, context, lane, false, bounds.get());
}
/* 027993A4 */
u32 gatherAlternateCascadeBounds(agl::CascadeShadow* shadow, void* context, u32 lane) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<agl::CascadeBounds> bounds(8);
    return gatherCascadeBoundsImpl(shadow, context, lane, true, bounds.get());
}
// Separate emitted entry identities; these names claim no distinct bounds policy.
/* 02799578 */
u32 gatherSecondaryCascadeBounds(agl::CascadeShadow* shadow, void* context, u32 lane) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<agl::CascadeBounds> bounds(8);
    return gatherCascadeBoundsImpl(shadow, context, lane, false, bounds.get());
}
/* 0279974C */
u32 gatherSecondaryAlternateCascadeBounds(agl::CascadeShadow* shadow, void* context, u32 lane) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<agl::CascadeBounds> bounds(8);
    return gatherCascadeBoundsImpl(shadow, context, lane, true, bounds.get());
}
/* 02799920 */
u32 agl::CascadeShadow::matchingCascadeMask(void* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 mask = 0;
    s32 index = 0;
    while (index < (s32)(u32)mCascadeCount.mValue) {
        ShadowUnit_l* unit = cascadeUnitAt(this, (u32)index);
        if (matchesCascadeScope(unit, context) != 0 && (index & 32) == 0) mask |= 1u << (index & 31);
        ++index;
    }
    return mask;
}
/* 027999AC */
u32 agl::CascadeShadow::matchingAlternateCascadeMask(void* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 mask = 0;
    s32 index = 0;
    while (index < (s32)(u32)mCascadeCount.mValue) {
        ShadowUnit_l* unit = cascadeUnitAt(this, (u32)index);
        if (matchesAlternateCascadeScope(unit, context) != 0 && (index & 32) == 0) mask |= 1u << (index & 31);
        ++index;
    }
    return mask;
}
/* 02799A38 */
u32 agl::CascadeShadow::cascadeMask() {
    s32 count = (s32)(u32)mCascadeCount.mValue;
    u32 mask = 0;
    for (s32 index = 0; index < count; ++index) {
        if ((index & 32) == 0) mask |= 1u << (index & 31);
    }
    return mask;
}
/* 02799A6C */
u32 agl::CascadeShadow::alternateCascadeMask() {
    s32 count = (s32)(u32)mCascadeCount.mValue;
    u32 mask = 0;
    for (s32 index = 0; index < count; ++index) {
        if ((index & 32) == 0) mask |= 1u << (index & 31);
    }
    return mask;
}
/* WWHD glue
 ext 027C988C u32 matchesCascadeScope(agl::ShadowUnit_l*, void*)
 ext 027C99BC u32 matchesAlternateCascadeScope(agl::ShadowUnit_l*, void*)
 data sCascadeParameterObjectTable 10166E18
 ext 027965BC void releaseCascadeMapResources(agl::CascadeMapStorage_l*)
 ext 02796624 void destroyCascadeMap(agl::CascadeMapStorage_l*, u32)
 ext 02755FEC sead::RegistryHeap* findCascadeArrayHeap(sead::RegistryManagerState*, void*)
 data sCascadeTextTable 10166D98
 data sCascadeRootTable 10167258
 data sCascadeBoundsTable 101672C0
 data sCascadeIntegerTable 10166EE8
 data sCascadePairBaseTable 10166FD8
 data sCascadePairTable 10167050
 data sCascadeBooleanTable 10166E70
 data sCascadeFloatTable 10166F60
 data sDefaultCascadeHint 10166D94
 data sPairCascadeHint 10166D95
 data sBooleanCascadeHint 10166D96
 str 101670E0 "aglsdw"
 str 10167150 "bounding_calc_type"
 str 10167164 "Bounding Calc Type"
 str 101670E8 "cascade_num"
 str 101670F4 "Cascade Num"
 str 10167178 "cascade_near"
 str 10167100 "depth_clamp"
 str 1016710C "Depth Clamp"
 str 10167118 "optimize_offset_far"
 str 1016712C "Optimize Offset Far"
 str 101670D8 "Min=0"
 str 10167188 "optimize_offset_near"
 str 101671A0 "Optimize Offset Near"
 str 101671B8 "stable_texel_width"
 str 101671CC "Stable Texel Width"
 str 10167140 "Min=1, Max=32"
 str 101671E0 "same_point_epsilon"
 str 101671F4 "Same Point Epsilon"
 str 10167208 "Min=0, Max=1"
 ext 0273AD10 void* allocateCascadeObject(u32)
 ext 027A84B8 void constructShadowParameterObject(agl::parameter_tree::Value*)
 ext 0279E674 void initializeCascadeParameterBase(agl::CascadeScalarPrefix*)
 ext 0279E618 void describeCascadeParameter(agl::CascadeScalarPrefix*, sead::SafeText*, sead::SafeText*, sead::SafeText*, agl::parameter_tree::Value*)
 ext 02796034 void initializeCascadeMap(agl::CascadeMapStorage_l*)
 ext 0278D81C void initializeCascadeSupport(agl::CascadeSupportStorage_l*)
*/
