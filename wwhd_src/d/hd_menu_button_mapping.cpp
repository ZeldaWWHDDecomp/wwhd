/* HD menu button mapping collection; game binary-derived implementation. */
#include "d/hd_menu_button_mapping.h"
#include "d/hd_screen_map_display_coordinator.h"



struct MenuMappingSystem_l { u8 _header[0x218]; gptr<hdMapDisplayCoordinator_c> mpSettings; };
struct MenuMappingPair_l { be<s8> mButton; be<u8> mSlot; };
struct MenuMappingDefinition_l { be<u8> mSlot; u8 _padding[3]; be<s32> mCount; MenuMappingPair_l mEntries[14]; };
WWHD_SIZE(MenuMappingDefinition_l, 0x24);
struct MenuMappingDungeonGroup_l {
    gptr<MenuMappingDungeonGroup_l> mpNext;
    be<u8> mSlot;
    be<s8> mButtons[14];
    u8 _padding;
};
WWHD_SIZE(MenuMappingDungeonGroup_l, 0x14);
struct MenuMappingDungeonList_l { be<s32> mCount; gptr<MenuMappingDungeonGroup_l> mpFirst; };
struct MenuMappingWorldVtable_l { u8 _header[0x1EC]; gfn mGetDungeonGroups; };
struct MenuMappingWorld_l { gptr<MenuMappingWorldVtable_l> mVtable; };
struct MenuMappingGameInfo_l { u8 _header[0x5150]; MenuMappingWorld_l mWorld; };
struct MenuMappingStaticValues_l {
    be<f32> mNegativePi;
    be<f32> mPositivePi;
    be<f32> mLargeBounds[2];
    be<f32> mSmallBounds[2];
    be<u8> mRandom;
    be<u8> mUtility;
    u8 _padding[2];
    be<u32> mStorage[4];
};
struct MenuMappingRegistration_l { u8 mStorage[12]; };
static MenuMappingSystem_l* sMappingSystem;
struct MenuMappingDefinitions_l { MenuMappingDefinition_l mGroups[2]; };
static MenuMappingDefinitions_l sMenuMappingDefinitions;
static MenuMappingStaticValues_l sMappingStaticValues;
static MenuMappingRegistration_l sMappingStorageRegistration;
static MenuMappingRegistration_l sMappingRandomRegistration;
static MenuMappingRegistration_l sMappingUtilityRegistration;
extern MenuMappingGameInfo_l* getMappingGameInfo();
extern void mappingGetFloors(MenuMappingWorld_l*, be<u8>*, be<u8>*);
extern void mappingRegisterStatic(MenuMappingRegistration_l*);
extern void mappingConstructRandom(be<u8>*);
extern void mappingConstructUtility(be<u8>*);

/* 0268C368 */
void hdMenuButtonMappings::refreshEnabled() {
    mPrimaryEnabled[0] = sMappingSystem->mpSettings->hasDungeonMap();
    mPrimaryEnabled[1] = sMappingSystem->mpSettings->hasDungeonBossKey();
    mPrimaryEnabled[2] = sMappingSystem->mpSettings->hasDungeonCompass();
}

/* 0268C660 */
void hdMenuButtonMappings::loadMenuMap() {
    mPrimaryGroupCount = 2;
    mTotalEntries = 10;
    for (u32 groupIndex = 0; s32(groupIndex) < s32(mPrimaryGroupCount); ++groupIndex) {
        MenuMappingDefinition_l* definition = &sMenuMappingDefinitions.mGroups[groupIndex];
        hdMenuButtonMapping* group = &mPrimaryGroups[groupIndex < GROUP_COUNT ? groupIndex : 0];
        group->mSlot = definition->mSlot;
        s32 count = definition->mCount;
        if (count > 0) {
            s32 entryCount = definition->mCount;
            for (u32 i = 0; s32(i) < entryCount; ++i) {
                hdMenuButtonEntry* entry = &group->mEntries[i < hdMenuButtonMapping::ENTRY_COUNT ? i : 0];
                entry->mSlot = definition->mEntries[i].mSlot;
                s32 button = definition->mEntries[i].mButton;
                entry->mButton = button;
                if (sMappingSystem->mpSettings->isRoomVisited(button) != 0) entry->mEnabled = 1;
            }
        }
        group->mEntryCount = count;
    }
    mTopFloor = 0x81;
    mBottomFloor = 0x80;
    refreshEnabled();
    mPrimarySelection = findPrimary(mPrimarySlot, s32(mPrimaryButton));
    snapshot();
}

/* 0268C7F4 */
void hdMenuButtonMappings::loadDungeonMap() {
    MenuMappingWorld_l* world = &getMappingGameInfo()->mWorld;
    MenuMappingDungeonList_l* list = gabi::call_ptr<MenuMappingDungeonList_l*>(world->mVtable->mGetDungeonGroups.get(), world);
    if (list != NULL) {
        s32 count = list->mCount;
        mPrimaryGroupCount = count;
        mTotalEntries = 0;
        if ((u32(count) - 2) >= 4) return;
        MenuMappingDungeonGroup_l* definition = list->mpFirst;
        for (u32 groupIndex = 0; s32(groupIndex) < count; ++groupIndex) {
            if (definition != NULL) {
                hdMenuButtonMapping* group = &mPrimaryGroups[groupIndex < GROUP_COUNT ? groupIndex : 0];
                u8 slot = definition->mSlot;
                u32 entries = 0;
                group->mSlot = slot;
                for (u32 i = 0; i < 14; ++i) {
                    if (s32(definition->mButtons[i]) == -1) continue;
                    hdMenuButtonEntry* entry = &group->mEntries[entries < 14 ? entries : 0];
                    entry->mSlot = slot;
                    entry->mButton = definition->mButtons[i];
                    s32 button = definition->mButtons[i];
                    if (sMappingSystem->mpSettings->isRoomVisited(button) != 0) entry->mEnabled = 1;
                    ++entries;
                }
                group->mEntryCount = entries;
                mTotalEntries = s32(mTotalEntries) + entries;
                count = mPrimaryGroupCount;
            }
            definition = gabi::at<MenuMappingDungeonGroup_l>(gabi::ea(definition) + sizeof(MenuMappingDungeonGroup_l));
        }
    }
    world = &getMappingGameInfo()->mWorld;
    mappingGetFloors(world, &mTopFloor, &mBottomFloor);
    refreshEnabled();
    mPrimarySelection = findPrimary(mPrimarySlot, s32(mPrimaryButton));
    snapshot();
}

/* 0268C960 */
void hdMenuButtonMappings::rebuild() {
    reset();
    if (sMappingSystem->mpSettings->isDungeonMode() != 0) loadMenuMap();
    else loadDungeonMap();
}

/* 0268CAD4 */
void hdMenuButtonMappings::updateEnabledChanges() {
    refreshEnabled();
    for (u32 i = 0; i < 3; ++i) {
        mPendingEnabled[i] = 0;
        if (u8(mPrimaryEnabled[i]) != u8(mSecondaryEnabled[i])) mPendingEnabled[i] = 1;
    }
}

/* 0268CB98 */
void hdMenuButtonMappings::updateSelection() {
    mPrimarySelection = findPrimary(mPrimarySlot, s32(mPrimaryButton));
    for (u32 groupIndex = 0; s32(groupIndex) < s32(mPrimaryGroupCount); ++groupIndex) {
        hdMenuButtonMapping* group = &mPrimaryGroups[groupIndex < GROUP_COUNT ? groupIndex : 0];
        s32 count = group->mEntryCount;
        for (u32 entryIndex = 0; s32(entryIndex) < count; ++entryIndex) {
            hdMenuButtonEntry* entry = &group->mEntries[entryIndex < 14 ? entryIndex : 0];
            s32 button = entry->mButton;
            if (sMappingSystem->mpSettings->isRoomVisited(button) != 0) entry->mEnabled = 1;
        }
    }
    u8 oldSlot = mSecondarySlot;
    s32 button = mPrimaryButton;
    u8 slot = mPrimarySlot;
    s32 oldButton = mSecondaryButton;
    mPrimaryChanged = 0;
    if (slot != oldSlot) mPrimaryChanged = 1;
    u8 slotChanged = mPrimaryChanged;
    u8 buttonChanged = 0;
    mSecondaryChanged = 0;
    if (button != oldButton) {
        buttonChanged = 1;
        mSecondaryChanged = 1;
    }
    if (slotChanged != 0 || buttonChanged != 0) {
        hdMenuButtonEntry* old = findSecondary(mPrimarySlot, s32(mPrimaryButton));
        if (old != NULL) {
            hdMenuButtonEntry* current = mPrimarySelection;
            if (current != NULL && old->mEnabled == 0) current->mFlags = 1;
        }
    }
    updateEnabledChanges();
}

/* 0268CD2C */
static void initializeMenuMappingStatics() {
    for (s32 i = 3; i >= 0; --i) sMappingStaticValues.mStorage[i] = 0;
    mappingRegisterStatic(&sMappingStorageRegistration);
    sMappingStaticValues.mNegativePi = -3.1415927410125732f;
    sMappingStaticValues.mPositivePi = 3.1415927410125732f;
    mappingConstructRandom(&sMappingStaticValues.mRandom);
    mappingRegisterStatic(&sMappingRandomRegistration);
    mappingConstructUtility(&sMappingStaticValues.mUtility);
    mappingRegisterStatic(&sMappingUtilityRegistration);
    sMappingStaticValues.mLargeBounds[0] = 50000.0f;
    sMappingStaticValues.mSmallBounds[0] = 10000.0f;
    sMappingStaticValues.mLargeBounds[1] = 50000.0f;
    sMappingStaticValues.mSmallBounds[1] = 10000.0f;
}

typedef hdMenuButtonMapping* (*MenuMappingConstructor)(hdMenuButtonMapping*);
typedef void (*MenuMappingDestructor)(hdMenuButtonMapping*, u32);
extern void* mappingAllocate(u32);
extern void mappingDelete(void*);
extern void mappingConstructArray(void*, u32, u32, MenuMappingConstructor);
extern void mappingDestroyArray(void*, u32, u32, MenuMappingDestructor, u32, u32);
extern hdMenuButtonMapping* mappingConstructGroup(hdMenuButtonMapping*);
extern void mappingDestroyGroup(hdMenuButtonMapping*, u32);

/* 0268BF64 */
hdMenuButtonMappings* hdMenuButtonMappings::construct(hdMenuButtonMappings* self) {
    if (self == NULL) {
        self = static_cast<hdMenuButtonMappings*>(mappingAllocate(sizeof(hdMenuButtonMappings)));
        if (self == NULL) return NULL;
    }
    be<u8>* secondary = self->mSecondaryEnabled;
    if (gabi::ea(secondary) == 0) mappingAllocate(3);
    be<u8>* pending = self->mPendingEnabled;
    if (gabi::ea(pending) == 0) mappingAllocate(3);
    self->mPrimaryGroupCount = 0;
    self->mTotalEntries = 0;
    hdMenuButtonMapping* primaryGroups = self->mPrimaryGroups;
    if (gabi::ea(primaryGroups) == 0) primaryGroups = static_cast<hdMenuButtonMapping*>(mappingAllocate(0x280));
    if (primaryGroups != NULL) mappingConstructArray(primaryGroups, GROUP_COUNT, sizeof(hdMenuButtonMapping), mappingConstructGroup);
    hdMenuButtonMapping* secondaryGroups = self->mSecondaryGroups;
    if (gabi::ea(secondaryGroups) == 0) secondaryGroups = static_cast<hdMenuButtonMapping*>(mappingAllocate(0x280));
    if (secondaryGroups != NULL) mappingConstructArray(secondaryGroups, GROUP_COUNT, sizeof(hdMenuButtonMapping), mappingConstructGroup);
    self->mPrimarySelection = 0;
    self->mSecondarySelection = 0;
    self->mPrimarySlot = hdMenuButtonMapping::NO_SLOT;
    self->mSecondaryChanged = 0;
    self->mPrimaryButton = -1;
    self->mTopFloor = hdMenuButtonMapping::NO_SLOT;
    self->mSecondaryButton = -1;
    self->mBottomFloor = hdMenuButtonMapping::NO_SLOT;
    self->mSecondarySlot = hdMenuButtonMapping::NO_SLOT;
    self->mPrimaryChanged = 0;
    for (u32 i = 0; i < 3; ++i) self->mPrimaryEnabled[i] = 0;
    for (u32 i = 0; i < 3; ++i) secondary[i] = 0;
    for (u32 i = 0; i < 3; ++i) pending[i] = 0;
    return self;
}

/* 0268C2D4 */
void hdMenuButtonMappings::destroy(u32 flags) {
    if (this == NULL) return;
    reset();
    mappingDestroyArray(mSecondaryGroups, GROUP_COUNT, sizeof(hdMenuButtonMapping), mappingDestroyGroup, 0, 0);
    mappingDestroyArray(mPrimaryGroups, GROUP_COUNT, sizeof(hdMenuButtonMapping), mappingDestroyGroup, 0, 0);
    if ((flags & 1) != 0) mappingDelete(this);
}

/* 0268C4C4 */
void hdMenuButtonMappings::snapshot() {
    for (u32 groupIndex = 0; groupIndex < GROUP_COUNT; ++groupIndex) {
        hdMenuButtonMapping* source = &mPrimaryGroups[groupIndex];
        hdMenuButtonMapping* destination = &mSecondaryGroups[groupIndex];
        destination->mSlot = source->mSlot;
        destination->mEntryCount = source->mEntryCount;
        for (u32 entryIndex = 0; entryIndex < hdMenuButtonMapping::ENTRY_COUNT; ++entryIndex) {
            hdMenuButtonEntry* from = &source->mEntries[entryIndex];
            hdMenuButtonEntry* to = &destination->mEntries[entryIndex];
            s8 button = from->mButton;
            from->mFlags = 0;
            to->mButton = button;
            to->mSlot = from->mSlot;
            to->mEnabled = from->mEnabled;
            to->mFlags = from->mFlags;
        }
    }
    u8 button = mPrimaryButton;
    hdMenuButtonEntry* selection = mPrimarySelection;
    mSecondaryButton = button;
    u8 slot = mPrimarySlot;
    mSecondarySelection = selection;
    mSecondarySlot = slot;
    for (u32 i = 0; i < 3; ++i) mSecondaryEnabled[i] = mPrimaryEnabled[i];
}

/* 0268C9C4 */
hdMenuButtonEntry* hdMenuButtonMappings::findSecondary(u32 slot, u32 button) {
    hdMenuButtonEntry* found = NULL;
    s32 remainingGroups = mPrimaryGroupCount;
    for (u32 groupIndex = 0; remainingGroups > 0; ++groupIndex, --remainingGroups) {
        hdMenuButtonMapping* group = &mSecondaryGroups[groupIndex < GROUP_COUNT ? groupIndex : 0];
        s32 remainingEntries = group->mEntryCount;
        bool differentSlot = u32(group->mSlot) != slot;
        for (u32 entryIndex = 0; remainingEntries > 0; ++entryIndex, --remainingEntries) {
            hdMenuButtonEntry* entry = &group->mEntries[entryIndex < hdMenuButtonMapping::ENTRY_COUNT ? entryIndex : 0];
            if (!differentSlot && u32(s32(entry->mButton)) == button) {
                found = entry;
                break;
            }
        }
    }
    return found;
}

static void resetMapping(hdMenuButtonMapping* mapping) {
    mapping->mSlot = hdMenuButtonMapping::NO_SLOT;
    mapping->mEntryCount = 0;
    for (u32 i = 0; i < hdMenuButtonMapping::ENTRY_COUNT; ++i) {
        hdMenuButtonEntry* entry = &mapping->mEntries[i];
        entry->mButton = -1;
        entry->mEnabled = 0;
        entry->mFlags = 0;
        entry->mSlot = hdMenuButtonMapping::NO_SLOT;
    }
}

/* 0268C0D4 */
void hdMenuButtonMappings::reset() {
    for (u32 i = 0; i < 3; ++i) {
        mPrimaryEnabled[i] = 0;
    }
    for (u32 i = 0; i < 3; ++i) {
        mSecondaryEnabled[i] = 0;
    }
    for (u32 i = 0; i < 3; ++i) {
        mPendingEnabled[i] = 0;
    }
    mPrimaryGroupCount = 0;
    mTotalEntries = 0;
    for (u32 i = 0; i < GROUP_COUNT; ++i) {
        resetMapping(&mPrimaryGroups[i]);
        resetMapping(&mSecondaryGroups[i]);
    }
    mPrimarySelection = 0;
    mSecondaryChanged = 0;
    mPrimarySlot = hdMenuButtonMapping::NO_SLOT;
    mPrimaryButton = -1;
    mSecondarySlot = hdMenuButtonMapping::NO_SLOT;
    mSecondaryButton = -1;
    mTopFloor = hdMenuButtonMapping::NO_SLOT;
    mSecondarySelection = 0;
    mPrimaryChanged = 0;
    mBottomFloor = hdMenuButtonMapping::NO_SLOT;
}

/* 0268C3CC */
hdMenuButtonEntry* hdMenuButtonMappings::findPrimary(u32 slot, u32 button) {
    s32 remainingGroups = mPrimaryGroupCount;
    for (u32 groupIndex = 0; remainingGroups > 0; ++groupIndex, --remainingGroups) {
        hdMenuButtonMapping* group = &mPrimaryGroups[groupIndex < GROUP_COUNT ? groupIndex : 0];
        s32 remainingEntries = group->mEntryCount;
        bool differentSlot = u32(group->mSlot) != slot;
        for (u32 entryIndex = 0; remainingEntries > 0; ++entryIndex, --remainingEntries) {
            hdMenuButtonEntry* entry = &group->mEntries[entryIndex < hdMenuButtonMapping::ENTRY_COUNT ? entryIndex : 0];
            if (!differentSlot && u32(s32(entry->mButton)) == button) {
                return entry;
            }
        }
    }
    return NULL;
}

/* WWHD glue (tools/verify/autoglue.py): menu local methods
 data sMappingSystem 101F8344
 data sMenuMappingDefinitions 100F9D88
 data sMappingStaticValues 104965FC
 data sMappingStorageRegistration 101F6350
 data sMappingRandomRegistration 101F635C
 data sMappingUtilityRegistration 101F6368
 ext 02689E7C u32 hdMapDisplayCoordinator_c::hasDungeonMap()
 ext 02689F30 u32 hdMapDisplayCoordinator_c::hasDungeonBossKey()
 ext 02689FE4 u32 hdMapDisplayCoordinator_c::hasDungeonCompass()
 ext 02689D28 u32 hdMapDisplayCoordinator_c::isRoomVisited(s32)
 ext 0268714C u32 hdMapDisplayCoordinator_c::isDungeonMode()
 ext 025200D4 MenuMappingGameInfo_l* getMappingGameInfo()
 ext 0258DA64 void mappingGetFloors(MenuMappingWorld_l*,be<u8>*,be<u8>*)
 ext 028F026C void mappingRegisterStatic(MenuMappingRegistration_l*)
 ext 028ED6F8 void mappingConstructRandom(be<u8>*)
 ext 028EAB2C void mappingConstructUtility(be<u8>*)
 ext 0273AD10 void* mappingAllocate(u32)
 ext 0273AF40 void mappingDelete(void*)
 ext 028EFFD0 void mappingConstructArray(void*,u32,u32,MenuMappingConstructor)
 ext 028F0164 void mappingDestroyArray(void*,u32,u32,MenuMappingDestructor,u32,u32)
 ext 0268CE70 hdMenuButtonMapping* mappingConstructGroup(hdMenuButtonMapping*) callback
 ext 0268CEEC void mappingDestroyGroup(hdMenuButtonMapping*,u32) callback
*/
