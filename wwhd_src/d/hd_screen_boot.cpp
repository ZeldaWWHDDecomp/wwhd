/* HD Boot_00 animation screen. */
#include "d/hd_screen_boot.h"

static u32 sScreenVtable;
static u32 sStateHolderVtable;
static u32 sSecondaryVtable;
static u32 sStringVtable;
static hdPartsStateIdVtbl_c sStateVtable;
extern void hdBootDestroyBase(hdParts_c* screen, u32 flags);
extern void hdBootConstructBase(hdParts_c* screen);
extern void hdBootFinalizeBase(hdParts_c* screen);
extern hdBoot_c* hdBootAllocate(u32 size);
extern void hdBootDelete(void* screen);
extern void hdBootBindAnimation(hdAnimSet* animation, u32 entry, hdUiSafeString* name, hdUiSafeString* group);
static f32 sBootAnimationRate = 1.0f;
static hdBootUiPointer sBootUiSystem;
static hdBootResourcePointer sBootResourceManager;
static hdUiSafeString sBootArchiveName;
static hdUiSafeString sBootLayoutName;
static hdBootSafeStringPair sBootAnimationNames;
static hdBootSafeStringPair sBootAnimationGroups;
static hdBootGroupIndex sBootGroupIndices[2] = {{1}, {0}};
static hdPartsStateId_c sBootLoopState;
static hdPartsStateId_c sBootOutAllState;
static hdBootRuntimeWords sBootRuntime;
static hdBootRegistration sBootRuntimeRegistration;
static hdBootRegistration sBootFirstRegistration;
static hdBootRegistration sBootSecondRegistration;
static hdBootLifetimeContext sBootFirstContext;
static hdBootLifetimeContext sBootSecondContext;
static f32 sBootMinimumAngle;
static f32 sBootMaximumAngle;
static hdPartsStateCounter_c sBootCounterReady;
static hdPartsStateCounter_c sBootNextState;
static hdStateId sBootNullState;
extern void hdBootRegisterStatics(hdBootRegistration* registration);
extern void hdBootConstructFirstContext(hdBootLifetimeContext* context);
extern void hdBootConstructSecondContext(hdBootLifetimeContext* context);

/* inlined into attachBootArchives */
static hdUiSafeString* bootArchiveNameAt(hdUiSafeString* names, u32 index) {
    return gabi::at<hdUiSafeString>(gabi::ea(names) + index * sizeof(hdUiSafeString));
}

/* 02628AD0 */
u32 hdBootStateKey(hdPartsStateId_c* state) {
    return state->mNumber;
}

/* 02628BD0 */
void hdBoot_c::destroy(hdBoot_c* screen, u32 flags) {
    if (!screen) return;
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    hdBootDestroyBase(screen, 0);
    if (flags & 1) hdBootDelete(screen);
}

/* 02628C3C */
void hdBoot_c::loopEnter() {
    const f32 rate = sBootAnimationRate;
    hdAnimSet_play(animSet(), 0, 1, rate);
}

/* 02628C58 */
void hdBoot_c::loopExecute() {
    vCalcLayout();
}

/* 02628C68 */
void hdBoot_c::outAllEnter() {
    const f32 rate = sBootAnimationRate;
    hdAnimSet_play(animSet(), 1, 0, rate);
}

/* 02628C84 */
void hdBoot_c::outAllExecute() {
    vCalcLayout();
}

/* 02628C94 */
hdBoot_c* hdBoot_c::construct(hdBoot_c* screen) {
    if (!screen) {
        screen = hdBootAllocate(sizeof(hdBoot_c));
        if (!screen) return nullptr;
    }
    hdBootConstructBase(screen);
    screen->mVtable.v = gabi::ea(&sScreenVtable);
    screen->mStateHolder.mHolderVtable.v = gabi::ea(&sStateHolderVtable);
    screen->mSecondaryVtable.v = gabi::ea(&sSecondaryVtable);
    return screen;
}

/* 02628D00 */
void hdBoot_c::attachBootArchives(hdUiSafeString* names, u32 count) {
    hdScreenHeap* heap = sBootUiSystem->mHeap;
    auto* accessor = static_cast<hdPartsResAccessor_c*>(hdParts_new(sizeof(hdPartsResAccessor_c), heap, 4));
    if (accessor) accessor = hdPartsResAccessor_c::construct(accessor);
    mpResAccessor = accessor;
    hdParts_allocArcLinks(&mArcLinks, count, heap, 4);
    for (u32 i = 0; i < count; ++i) {
        void* archive = hdParts_findArchive(sBootResourceManager, bootArchiveNameAt(names, i));
        if (!archive) continue;
        auto* link = static_cast<hdPartsArcLink_c*>(hdParts_new(sizeof(hdPartsArcLink_c), heap, 4));
        if (link) {
            link->mLink[0] = 0;
            link->mLink[1] = 0;
            hdParts_constructArcAccessor(link->mAccessor);
        }
        hdParts_setArcLink(link, archive, ".");
        hdParts_attachArcLink(mpResAccessor, link);
        if (s32(mArcLinks.mNum) < s32(mArcLinks.mMax)) {
            *mArcLinks.slot(mArcLinks.mNum) = link;
            mArcLinks.mNum = u32(mArcLinks.mNum) + 1;
        }
    }
}

/* 02628E10 */
u32 hdBoot_c::initializeBoot(u32 resource, u32 option, void* archive) {
    attachBootArchives(&sBootArchiveName, 1);
    auto load = mVtable->mLoad.get();
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!gabi::call_ptr<u32>(load, this, &sBootLayoutName, accessor, resource, option, archive)) return 0;
    hdAnimSet_operationCC(animSet(), 0, 1);
    vCalcLayout();
    startState(&sBootLoopState);
    return 1;
}

/* 02628F68 */
u32 hdBoot_c::loadBoot(hdUiSafeString* name, hdPartsResAccessor_c* accessor, u32 resource, u32 option, void* archive) {
    hdScreenHeap* heap = sBootUiSystem->mHeap;
    auto* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, gabi::ea(accessor), resource, option, 2, 2, archive)) return 0;
    for (u32 i = 0; i < 2; ++i) {
        hdAnimSet* animation = animSet();
        const u32 group = sBootGroupIndices[i].mIndex;
        hdBootBindAnimation(animation, i, &sBootAnimationNames.mNames[i], &sBootAnimationGroups.mNames[group]);
    }
    vCalcLayout();
    return 1;
}

/* 02628F20 */
void hdBoot_c::finalizeBoot() {
    vDestroyLayout();
    hdBootFinalizeBase(this);
}

/* 02628F60 */
void hdBoot_c::executeBootState() {
    hdParts_executeStateMachine(stateMachine());
}

/* HD: compiler-generated static setup for this translation unit. */
/* 02629094 */
static void initializeBootStatics() {
    sBootRuntime.mWords[3] = 0;
    sBootRuntime.mWords[2] = 0;
    sBootRuntime.mWords[1] = 0;
    sBootRuntime.mWords[0] = 0;
    hdBootRegisterStatics(&sBootRuntimeRegistration);
    sBootMinimumAngle = -3.1415927410125732f;
    sBootMaximumAngle = 3.1415927410125732f;
    hdBootConstructFirstContext(&sBootFirstContext);
    hdBootRegisterStatics(&sBootFirstRegistration);
    hdBootConstructSecondContext(&sBootSecondContext);
    hdBootRegisterStatics(&sBootSecondRegistration);
    sBootAnimationNames.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sBootAnimationNames.mNames[0].mText.v = gabi::guest_addr("Loop");
    sBootAnimationNames.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sBootLayoutName.mVtable.v = gabi::ea(&sStringVtable);
    sBootAnimationGroups.mNames[0].mVtable.v = gabi::ea(&sStringVtable);
    sBootAnimationGroups.mNames[0].mText.v = gabi::guest_addr("G_All_00");
    sBootArchiveName.mVtable.v = gabi::ea(&sStringVtable);
    sBootArchiveName.mText.v = gabi::guest_addr("Boot_00");
    sBootLayoutName.mText.v = gabi::guest_addr("Boot_00");
    sBootAnimationGroups.mNames[1].mVtable.v = gabi::ea(&sStringVtable);
    sBootAnimationNames.mNames[1].mText.v = gabi::guest_addr("OutAll");
    sBootAnimationGroups.mNames[1].mText.v = gabi::guest_addr("G_Loop_00");
    u32 first;
    if (sBootCounterReady.mValue) first = u32(sBootNextState.mValue) + 1;
    else {
        sBootCounterReady.mValue = 1;
        first = 1;
    }
    auto* stateVtable = &sStateVtable;
    sBootLoopState.setup(first, "::StateID_Loop", stateVtable, 0x19, &sBootNullState);
    sBootNextState.mValue = first + 1;
    sBootOutAllState.setup(first + 1, "::StateID_OutAll", stateVtable, 0x1C, &sBootNullState);
}

/* WWHD glue
data sScreenVtable 100E5E90 nocheck
data sStateHolderVtable 100E5F88 nocheck
data sSecondaryVtable 100E5F98 nocheck
data sStringVtable 100E5D7C nocheck
data sStateVtable 100E5DDC nocheck
data sBootAnimationRate 100E5E28
data sBootUiSystem 1018C404
data sBootResourceManager 101F7274
data sBootArchiveName 1048F2B4
data sBootLayoutName 1048F358
data sBootAnimationNames 1048F2E8
data sBootAnimationGroups 1048F2D8
data sBootGroupIndices 100E5D70
data sBootLoopState 1048F2F8
data sBootOutAllState 1048F328
data sBootRuntime 1048F2C8
data sBootRuntimeRegistration 101F53C8
data sBootFirstRegistration 101F53D4
data sBootSecondRegistration 101F53E0
data sBootFirstContext 1048F2C4
data sBootSecondContext 1048F2C5
data sBootMinimumAngle 1048F2BC
data sBootMaximumAngle 1048F2C0
data sBootCounterReady 101FD8E4
data sBootNextState 101FDD50
data sBootNullState 101FF32C
str 100E5E38 "Boot_00" in 02629094
str 100E5E40 "Loop" in 02629094
str 100E5E48 "OutAll" in 02629094
str 100E5E50 "G_Loop_00" in 02629094
str 100E5E5C "G_All_00" in 02629094
str 100E5E68 "::StateID_Loop" in 02629094
str 100E5E78 "::StateID_OutAll" in 02629094
ext 028F026C void hdBootRegisterStatics(hdBootRegistration* registration)
ext 028ED6F8 void hdBootConstructFirstContext(hdBootLifetimeContext* context)
ext 028EAB2C void hdBootConstructSecondContext(hdBootLifetimeContext* context)
str 100E5E2C "." in 02628D00
ext 02004E04 void hdBootBindAnimation(hdAnimSet* animation, u32 entry, hdUiSafeString* name, hdUiSafeString* group)
ext 026F88F0 void hdBootDestroyBase(hdParts_c* screen, u32 flags)
ext 026F89E8 void hdBootConstructBase(hdParts_c* screen)
ext 026F91F8 void hdBootFinalizeBase(hdParts_c* screen)
ext 0273AD10 hdBoot_c* hdBootAllocate(u32 size)
ext 0273AF40 void hdBootDelete(void* screen)
*/
