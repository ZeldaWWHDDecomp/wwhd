/** HD dungeon-item icon button; the missing NoDecideButton extension stays local until FW-msg-base merges. */
#pragma once
#include "d/hd_parts_button.h"
#include "d/hd_no_decide_button.h"
/* the dungeon-map buttons are "NoDecideButton" parts (NoDecideButton_l, FW-parts_msg_base), itself a button part */
struct hdDungeonButton_c : hdButtonPart_c {
    /* 0x74 */ be<u32> mNoDecideAnimationState; /* the NoDecideButton base's member */
    /* the "NoDecideButton" base (the merged header models it on hdParts_c) */
    NoDecideButton_l* noDecideButton() { return (NoDecideButton_l*)(hdParts_c*)this; }
};
struct hdDungeonItemAnimationNames_l {
    hdUiSafeString mNames[2];
};
struct hdDungeonItemAnimationRemap_l { be<u32> mIndex; };
struct hdDungeonItemIcon_c : hdDungeonButton_c {
    static hdDungeonItemIcon_c* construct(hdDungeonItemIcon_c*);
    static void destroy(hdDungeonItemIcon_c*, u32 flags);
    u32 receiveButton();
    void playIconChange(); void calcLayout();
    u32 load(void*, void*, void*);
    u32 loadLayout(void*, u32, u32, u32, void*);
    void setIcon(s32 icon); void enterIcon(); void leaveIcon();
};
WWHD_SIZE(hdDungeonButton_c, 0x78);
WWHD_OFFSET(hdDungeonButton_c, mNoDecideAnimationState, 0x74);
WWHD_SIZE(hdDungeonItemIcon_c, 0x78);
WWHD_SIZE(hdDungeonItemAnimationNames_l, 0x10);
WWHD_SIZE(hdDungeonItemAnimationRemap_l, 4);
void* dungeonButtonAllocate(u32);
void dungeonButtonBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

void dungeonIconAnimationOperationD4(hdAnimSet*, u32, u32);

// Per-TU static runtime configuration, shared only within this eight-TU unit.
#include "d/hd_screen_enemy_tower.h"
struct hdDungeonButtonStaticSettings_l {
    hdMapChartScalar_l mMinimumAngle, mMaximumAngle;
    be<u8> mSignedHeader, mUnsignedHeader;
    u8 mPadding[2];
    be<u32> mRegistrationWork[4];
};
struct hdDungeonButtonAngleConstants_l { hdMapChartScalar_l mMinimumAngle, mMaximumAngle; };
WWHD_OFFSET(hdDungeonButtonStaticSettings_l, mMinimumAngle, 0);
WWHD_OFFSET(hdDungeonButtonStaticSettings_l, mMaximumAngle, 4);
WWHD_OFFSET(hdDungeonButtonStaticSettings_l, mSignedHeader, 8);
WWHD_OFFSET(hdDungeonButtonStaticSettings_l, mUnsignedHeader, 9);
WWHD_OFFSET(hdDungeonButtonStaticSettings_l, mRegistrationWork, 0xC);
WWHD_SIZE(hdDungeonButtonStaticSettings_l, 0x1C);
WWHD_SIZE(hdDungeonButtonAngleConstants_l, 8);
