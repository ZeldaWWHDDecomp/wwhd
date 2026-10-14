/* HD menu button mappings. Names describe binary-observed roles; no GameCube counterpart. */
#pragma once
#include "bindings.h"

struct hdMenuButtonEntry {
    be<s8> mButton;
    be<u8> mSlot;
    be<u8> mEnabled;
    be<u8> mFlags;
};
WWHD_SIZE(hdMenuButtonEntry, 4);
WWHD_OFFSET(hdMenuButtonEntry, mButton, 0);
WWHD_OFFSET(hdMenuButtonEntry, mSlot, 1);
WWHD_OFFSET(hdMenuButtonEntry, mEnabled, 2);
WWHD_OFFSET(hdMenuButtonEntry, mFlags, 3);

struct hdMenuButtonMapping {
    enum { NO_SLOT = 10, ENTRY_COUNT = 14 };
    be<u8> mSlot;
    u8 _padding[3];
    be<u32> mEntryCount;
    hdMenuButtonEntry mEntries[ENTRY_COUNT];
};
WWHD_SIZE(hdMenuButtonMapping, 0x40);
WWHD_OFFSET(hdMenuButtonMapping, mSlot, 0);
WWHD_OFFSET(hdMenuButtonMapping, mEntryCount, 4);
WWHD_OFFSET(hdMenuButtonMapping, mEntries, 8);

struct hdMenuButtonMappings {
    enum { GROUP_COUNT = 10 };
    be<u8> mPrimaryEnabled[3];
    be<u8> mSecondaryEnabled[3];
    be<u8> mPendingEnabled[3];
    u8 _padding[3];
    be<s32> mPrimaryGroupCount;
    be<s32> mTotalEntries;
    hdMenuButtonMapping mPrimaryGroups[GROUP_COUNT];
    hdMenuButtonMapping mSecondaryGroups[GROUP_COUNT];
    gptr<hdMenuButtonEntry> mPrimarySelection;
    gptr<hdMenuButtonEntry> mSecondarySelection;
    be<u8> mPrimarySlot;
    be<s8> mPrimaryButton;
    be<u8> mSecondarySlot;
    be<s8> mSecondaryButton;
    be<u8> mPrimaryChanged;
    be<u8> mSecondaryChanged;
    be<u8> mTopFloor;
    be<u8> mBottomFloor;

    static hdMenuButtonMappings* construct(hdMenuButtonMappings* self);
    void destroy(u32 flags);
    void reset();
    void snapshot();
    void refreshEnabled();
    void loadMenuMap();
    void loadDungeonMap();
    void rebuild();
    void updateEnabledChanges();
    void updateSelection();
    hdMenuButtonEntry* findSecondary(u32 slot, u32 button);
    hdMenuButtonEntry* findPrimary(u32 slot, u32 button);
};
WWHD_SIZE(hdMenuButtonMappings, 0x524);
WWHD_OFFSET(hdMenuButtonMappings, mPrimaryGroupCount, 0xC);
WWHD_OFFSET(hdMenuButtonMappings, mTotalEntries, 0x10);
WWHD_OFFSET(hdMenuButtonMappings, mPrimaryGroups, 0x14);
WWHD_OFFSET(hdMenuButtonMappings, mSecondaryGroups, 0x294);
WWHD_OFFSET(hdMenuButtonMappings, mPrimarySelection, 0x514);
WWHD_OFFSET(hdMenuButtonMappings, mSecondarySelection, 0x518);
WWHD_OFFSET(hdMenuButtonMappings, mPrimarySlot, 0x51C);
WWHD_OFFSET(hdMenuButtonMappings, mBottomFloor, 0x523);
