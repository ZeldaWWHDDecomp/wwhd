/* HD DungeonMapUp_00: floor caption and upward-transition marker of the GamePad dungeon map. */
#pragma once
#include "d/hd_sea_chart_state_handlers.h"
#include "d/hd_screen_map_display_coordinator.h"
#include "d/hd_parts_text_pane.h"
#include "d/d_com_inf_game.h"

/* the text pane's string setter (virtual slot +0xC4: text, length) */
struct hdDungeonUpTextPaneVtbl_l {
    /* 0x00 */ u8 _00[0xC4];
    /* 0xC4 */ gfn mSetString;
};
WWHD_OFFSET(hdDungeonUpTextPaneVtbl_l, mSetString, 0xC4);

/* a sea-chart pane together with its resource name (+0x80) */
struct hdDungeonUpNamedPane_l : hdSeaChartPane_l {
    /* 0x45 */ u8 _45[0x80 - 0x45];
    /* 0x80 */ char mName[0x18];
};
WWHD_OFFSET(hdDungeonUpNamedPane_l, mName, 0x80);

/* The marker reuses the sea-chart transition: mTransition.mPaneIndex is the floor the marker sits on
 * (also the viewport index bindMap passes on), mTransition.mHidden the flag update() derives from the
 * entrance or the visibility pane (a nonzero value shows the marker, so the merged name is probably
 * inverted). */
struct hdDungeonMapUp_c : hdSeaChartStateHandlers_c {
    /* 0xA0 */ gptr<hdTextPane_c> mpFloorText;
    /* 0xA4 */ gptr<hdDungeonUpNamedPane_l> mpSourcePane;
    /* 0xA8 */ gptr<hdPartsPaneSRT_c> mpVisibilityPane;
    /* 0xAC */ be<u8> mDungeonEntrance;
    /* 0xAD */ u8 _AD[3];

    enum { FLOOR_COUNT = 5 };

    static hdDungeonMapUp_c* construct(hdDungeonMapUp_c* part);
    static void destroy(hdDungeonMapUp_c* part, s32 flags);
    void findFloorText();
    void resetFloorText();
    u32 create(u32 archiveArg, u32 layoutArg, void* description);
    void release();
    void stateExecute();
    void draw(void* info);
    u32 isEntranceUnlocked();
    void update();
    void setFloorText(hdTextPane_c* text, hdUiSafeString* label);
    void refreshFloorText();
    void findDungeonEntrance();
};
WWHD_OFFSET(hdDungeonMapUp_c, mpFloorText, 0xA0);
WWHD_OFFSET(hdDungeonMapUp_c, mpSourcePane, 0xA4);
WWHD_OFFSET(hdDungeonMapUp_c, mpVisibilityPane, 0xA8);
WWHD_OFFSET(hdDungeonMapUp_c, mDungeonEntrance, 0xAC);
WWHD_SIZE(hdDungeonMapUp_c, 0xB0);

/* the map display coordinator: the object at +0x218 of the UI owner (HdPadUi in d/hd_gamepad_map.h views
 * the same object; its +0x50 is the coordinator's stage kind, the dungeon number) */
inline hdMapDisplayCoordinator_c* hdDungeonMapUp_coordinator() {
    return reinterpret_cast<hdMapDisplayCoordinator_c*>(l_hdDungeonUi.mOwner->mUi.get());
}

/* merged sea-chart transition functions (free functions of hd_sea_chart_state_handlers.cpp or members
 * defined there), the allocator and helpers, bound by address (glue) */
void upTransitionDestroy(hdSeaChartStateHandlers_c* self, s32 flags);
hdSeaChartStateHandlers_c* upTransitionConstruct(hdSeaChartStateHandlers_c* self);
void upTransitionBindPane(hdSeaChartStateHandlers_c* self, hdUiSafeString* name);
void upTransitionUpdateBlend(hdSeaChartStateHandlers_c* self);
void upTransitionSetPosition(hdSeaChartStateHandlers_c* self, hdSeaChartPane_l* pane);
void upTransitionApply(hdSeaChartStateHandlers_c* self);
hdDungeonMapUp_c* upAllocate(u32 size);
void upFree(hdDungeonMapUp_c* part);
void assureDungeonUpName(hdUiSafeString* name);
void constructUpHeaderObject(void* object);
void constructUpHeaderOtherObject(void* object);
