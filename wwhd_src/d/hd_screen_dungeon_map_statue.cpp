/* HD dungeon-map statue marker: load one animation and select its first/second statue frame. */
#include "d/hd_screen_dungeon_map_statue.h"
static u32 sScreenDungeonMapStatueRegistration;
static u32 sScreenDungeonMapStatueSecondRegistration;
static u32 sScreenDungeonMapStatueThirdRegistration;

static hdPartsVtbl_c sStatueVtable;
static u8 sStatueHolderVtable;
static u8 sStatueSecondaryVtable;
static hdUiStringTable sStatueStringVtable;
static hdUiSafeString sStatueAnimationName;
static hdUiSafeString sStatueGroupName;
static hdUiSafeString sStatueArchiveName;
static hdUiSafeString sStatueLayoutName;
static hdScreenUiSystem* sStatueUiSystem;
static u32 sStatueGroupIndex;
static hdPartsStateId_c sStatueSetState;
static hdPartsStateIdVtbl_c sStatueStateVtable;
static hdStateId sStatueNullState;
static u32 sStatueStateCounterReady;
static u32 sStatueStateCounter;
struct hdDungeonStatueHeaderZero { be<u32> mWords[4]; };
static hdDungeonStatueHeaderZero sStatueHeaderZero;
static f32 sStatueMinimumAngle;
static f32 sStatueMaximumAngle;
static f32 sStatueFarDistance;
static f32 sStatueFarDistanceLimit;
static f32 sStatueNearDistance;
static f32 sStatueNearDistanceLimit;
static u8 sStatueHeaderObject;
static u8 sStatueHeaderOtherObject;

/* 02665CCC */
void hdDungeonMapStatue_c::destroy(hdDungeonMapStatue_c* part, s32 flags) {
    if (part == NULL) return;
    part->mStateHolder.mHolderVtable = &sStatueHolderVtable;
    part->mSecondaryVtable = &sStatueSecondaryVtable;
    statuePaneDestroy(part, 0);
    if (flags & 1) statueFree(part);
}

/* 02665D38 */
void hdDungeonMapStatue_c::setStatueEnter() {
    hdAnimSet_play(animSet(), 0, 0, 1.0f);
}

/* 02665D54 */
void hdDungeonMapStatue_c::calcLayoutVirtual() { vCalcLayout(); }

/* 02665D64 */
hdDungeonMapStatue_c* hdDungeonMapStatue_c::construct(hdDungeonMapStatue_c* part) {
    if (part == NULL) {
        part = statueAllocate(sizeof(hdDungeonMapStatue_c));
        if (part == NULL) return NULL;
    }
    statuePaneConstruct(part);
    part->mStateHolder.mHolderVtable = &sStatueHolderVtable;
    part->mVtable = &sStatueVtable;
    part->mSecondaryVtable = &sStatueSecondaryVtable;
    part->mSecondStatueSet = 0;
    part->mFirstStatueSet = 0;
    return part;
}

/* 02665DDC */
u32 hdDungeonMapStatue_c::create(u32 archiveArg, u32 layoutArg, void* description) {
    attachArchives(&sStatueArchiveName, 1);
    if (!vLoad(&sStatueLayoutName, gabi::ea(mpResAccessor.get()), archiveArg, layoutArg, description)) return FALSE;
    hdAnimSet_setFrame(animSet(), 0, 0, 2.0f);
    hdUiSafeString rotate;
    rotate.mText.v = gabi::guest_addr("N_Rotate_00");
    rotate.mVtable = &sStatueStringVtable;
    statueBindRootAndPane(this, &rotate);
    return TRUE;
}

/* 02665ED4 */
u32 hdDungeonMapStatue_c::load(void* name, u32 archiveArg, u32 layoutArg, u32 resourceArg, void* description) {
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(hdParts_new(sizeof(hdPartsLayout_c), sStatueUiSystem->mHeap, 4));
    if (layout != NULL) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (layout == NULL || !layout->vLoad(name, archiveArg, layoutArg, resourceArg, 1, 1, description)) return FALSE;
    hdUiSafeString* group = hdPartsView<hdUiSafeString>(gabi::ea(&sStatueGroupName) + u32(sStatueGroupIndex) * 8);
    bindDungeonStatueAnimation(animSet(), 0, &sStatueAnimationName, group);
    vCalcLayout();
    return TRUE;
}

/* 02665FD4 */
void hdDungeonMapStatue_c::setFirstStatue() {
    hdAnimSet_setFrame(animSet(), 0, 0, 0.0f);
    mFirstStatueSet = 1;
}

/* 02666020 */
void hdDungeonMapStatue_c::setSecondStatue() {
    hdAnimSet_setFrame(animSet(), 0, 0, 1.0f);
    mSecondStatueSet = 1;
}

/* 0266606C */
static void initializeDungeonMapStatueStatics() {
    sStatueHeaderZero.mWords[3] = 0;
    sStatueHeaderZero.mWords[2] = 0;
    sStatueHeaderZero.mWords[1] = 0;
    sStatueHeaderZero.mWords[0] = 0;
    __register_global_object(gabi::ea(&sScreenDungeonMapStatueRegistration));
    sStatueMinimumAngle = -3.1415927410125732f;
    sStatueMaximumAngle = 3.1415927410125732f;
    constructStatueHeaderObject(&sStatueHeaderObject);
    __register_global_object(gabi::ea(&sScreenDungeonMapStatueSecondRegistration));
    constructStatueHeaderOtherObject(&sStatueHeaderOtherObject);
    __register_global_object(gabi::ea(&sScreenDungeonMapStatueThirdRegistration));
    sStatueFarDistance = 50000.0f;
    sStatueNearDistance = 10000.0f;
    hdParts_setSafeString(&sStatueAnimationName, &sStatueStringVtable, "SetStatue");
    sStatueNearDistanceLimit = 10000.0f;
    hdParts_setSafeString(&sStatueArchiveName, &sStatueStringVtable, "DungeonMapStatue_00");
    hdParts_setSafeString(&sStatueGroupName, &sStatueStringVtable, "G_All_00");
    hdParts_setSafeString(&sStatueLayoutName, &sStatueStringVtable, "DungeonMapStatue_00");
    sStatueFarDistanceLimit = 50000.0f;
    u32 number;
    if (sStatueStateCounterReady) number = sStatueStateCounter + 1;
    else {
        number = 1;
        sStatueStateCounterReady = 1;
    }
    sStatueSetState.setup(number, "::StateID_SetStatue", &sStatueStateVtable, 0x28, &sStatueNullState);
    sStatueStateCounter = number;
}

/* WWHD glue
 data sScreenDungeonMapStatueRegistration 101F5DB8 nocheck
 data sScreenDungeonMapStatueSecondRegistration 101F5DC4 nocheck
 data sScreenDungeonMapStatueThirdRegistration 101F5DD0 nocheck
ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*, u32)
ext 026F91F8 void hdParts_c::detachArchives()
ext 026F8CD0 void hdParts_c::displayExecute()
data sStatueVtable 100F3780 nocheck
data sStatueHolderVtable 100F38D8 nocheck
data sStatueSecondaryVtable 100F38E8 nocheck
data sStatueStringVtable 100F3670 nocheck
data sStatueAnimationName 10493BB4 nocheck
data sStatueGroupName 10493BAC nocheck
data sStatueArchiveName 10493BBC nocheck
data sStatueLayoutName 10493C20 nocheck
data sStatueUiSystem 1018C404 nocheck
data sStatueGroupIndex 100F366C nocheck
data sStatueSetState 10493BF0 nocheck
data sStatueStateVtable 100F36D0 nocheck
data sStatueNullState 101FF32C nocheck
data sStatueStateCounterReady 101FD8E4 nocheck
data sStatueStateCounter 101FDD50 nocheck
data sStatueHeaderZero 10493BE0 nocheck
data sStatueMinimumAngle 10493BC4 nocheck
data sStatueMaximumAngle 10493BC8 nocheck
data sStatueFarDistance 10493BCC nocheck
data sStatueFarDistanceLimit 10493BD0 nocheck
data sStatueNearDistance 10493BD4 nocheck
data sStatueNearDistanceLimit 10493BD8 nocheck
data sStatueHeaderObject 10493BDC nocheck
data sStatueHeaderOtherObject 10493BDD nocheck
str 100F3720 "N_Rotate_00" in 02665DDC
str 100F3768 "SetStatue" in 0266606C
str 100F3740 "DungeonMapStatue_00" in 0266606C
str 100F3774 "G_All_00" in 0266606C
str 100F3754 "::StateID_SetStatue" in 0266606C
ext 0268B728 void statuePaneDestroy(hdMenuMapPanePart*, u32)
ext 0273AF40 void statueFree(hdDungeonMapStatue_c*)
ext 0273AD10 hdDungeonMapStatue_c* statueAllocate(u32)
ext 0268B7B0 hdMenuMapPanePart* statuePaneConstruct(hdMenuMapPanePart*)
ext 02685784 void statueBindRootAndPane(hdMenuMapPanePart*, hdUiSafeString*)
ext 02004E04 void bindDungeonStatueAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)
ext 028ED6F8 void constructStatueHeaderObject(void*)
ext 028EAB2C void constructStatueHeaderOtherObject(void*)
*/
