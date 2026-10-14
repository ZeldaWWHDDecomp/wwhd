/* HD MenuBtns screen and its menu-specific BasicButton extension. */
#pragma once
#include "d/hd_parts_basic_button.h"
#include "d/hd_menu_pane_control.h"
// Menu-specific extension of the canonical BasicButton layout.
struct hdMenuButton_c : hdBasicButton_c {
    be<u32> mOrder;
    u8 _buttonTail[0xC];
};
WWHD_OFFSET(hdMenuButton_c, mOrder, 0x8C);
WWHD_SIZE(hdMenuButton_c, 0x9C);
struct hdMenuButtonPaneArray {
    be<s32> mCount;
    gptr<gptr<hdMenuPane_l>> mpData;
    hdMenuPane_l* get(u32 i) { return mpData.get()[i < u32(mCount) ? i : 0]; }
};
WWHD_SIZE(hdMenuButtonPaneArray, 8);
struct hdMenuButtons : hdParts_c {
    hdMenuButtonPaneArray mPanes;
    be<u32> mAvailability;
    be<u8> mEnabled;
    u8 _padding[3];
    be<u32> mSelected;
    enum { NO_SELECTION = 3, MENU_MAP_AVAILABLE = 1, DUNGEON_MAP_AVAILABLE = 2 };
    static hdMenuButtons* construct(hdMenuButtons* self);
    void destroy(u32 flags);
    hdMenuButton_c* button(u32 i) { return reinterpret_cast<hdMenuButton_c*>(mChildren.safeSlot(i)->get()); }
    u32 hasFocus();
    u32 canAcceptInput();
    void updateFocus();
    u32 hasMenuMap();
    u32 hasDungeonMap();
    u32 availableMaps();
    void revealAvailableMaps();
    void updateButtons();
    void displayExecute();
    void refreshAvailability();
    void inAllEnter();
    void inAllExecute();
    void outAllEnter();
    void outAllExecute();
    void menuOpenEnter();
    void menuOpenExecute();
    void menuCloseEnter();
    void menuCloseExecute();
    u32 createPanes();
    void initializeButtons();
    u32 buttonKey(u32 index);
    void connectButtons();
    u32 create(void* first, void* second, void* third);
    void releasePanes();
    void finalize();
    void executeState();
    void show();
    void openMenu();
    void closeMenu();
    u32 createChild(hdPartsDesc_c* descriptor);
    u32 loadLayout(void* name, void* accessor, u32 first, u32 second, void* descriptor);
    void executeChildren();
};
WWHD_SIZE(hdMenuButtons, 0x64);
WWHD_OFFSET(hdMenuButtons, mPanes, 0x50);
WWHD_OFFSET(hdMenuButtons, mAvailability, 0x58);
WWHD_OFFSET(hdMenuButtons, mEnabled, 0x5C);
WWHD_OFFSET(hdMenuButtons, mSelected, 0x60);
