/** HD sea-chart menu factories and owning display; game layout reconstruction. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"
#include "d/hd_input_eventmgr.h"
class hdSeaChartFactory_c {
public:
    static hdParts_c* createRupy(u32 first, u32 second, u32 third);
    static hdParts_c* createHeart(u32 first, u32 second, u32 third);
    static hdParts_c* createBarDisplay(u32 first, u32 second, u32 third);
    static hdParts_c* createCompassClock(u32 first, u32 second, u32 third);
    static hdParts_c* createDungeonKey(u32 first, u32 second, u32 third);
    static hdParts_c* createSwimTime(u32 first, u32 second, u32 third);
    static hdParts_c* createShortcut(u32 first, u32 second, u32 third);
    static hdParts_c* createBossHp(u32 first, u32 second, u32 third);
    static hdParts_c* createEnemyHp(u32 first, u32 second, u32 third);
    static hdParts_c* createTime(u32 first, u32 second, u32 third);
    static hdParts_c* createArrow(u32 first, u32 second, u32 third);
    static hdParts_c* createBattery(u32 first, u32 second, u32 third);
    static hdParts_c* createRupySwordCounter(u32 first, u32 second, u32 third);
};

struct hdSeaMenuSelectedVtable_l { u8 mReserved[0xC]; gfn mDelete; };
struct hdSeaMenuSelected_l : cking::ui::input::ReceiverNode {
    be<u32> mId;
    gptr<hdSeaMenuSelectedVtable_l> mpVtable;
};
WWHD_OFFSET(hdSeaMenuSelected_l, mId, 0x10);
WWHD_OFFSET(hdSeaMenuSelected_l, mpVtable, 0x14);
/* the sea-chart menu derives from the "NoControl" parts class of FW-parts_msg_base */
struct hdSeaChartMenu_c : NoControlPart_l {
    gptr<hdPartsPaneSRT_c> mpMenuPanes[8];
    gptr<hdPartsPaneSRT_c> mpMainPane;
    gptr<hdPartsPaneSRT_c> mpInfoPanes[3];
    gptr<hdPartsPaneSRT_c> mpTextPane;
    gptr<hdPartsPaneSRT_c> mpMapMarkerPanes[49];
    gptr<hdSeaMenuSelected_l> mpSelectedPane;
    gptr<hdParts_c> mpMap;
    gptr<hdParts_c> mpActiveButton;
    be<u8> mMapInputActive;
    u8 mSelectionPadding[3];
    be<u32> mSelectionMode;
    be<u32> mPreviousSelection;
    be<u8> mOpening;
    be<u8> mClosing;
    u8 mTailPadding[2];
    static hdSeaChartMenu_c* construct(hdSeaChartMenu_c* part);
    void destroy(u32 flags);
    void allocateMap();
    void updateOpeningFlag();
    void beginOpening();
    static u32 getMapCoordinate(u32 index);
    void updateButton();
    void activateSelection();
    void pollSelectionActivation();
    void registerSelectedPane();
    void updateMapInput();
    void findTextPane();
    void findMapMarkerPanes();
    void initializeChildren();
    u32 create(u32 first, u32 second, u32 third);
    void updateOpenMap();
    void initializeMap();
    void findMenuPanes();
    void findMainPane();
    void findInfoPanes();
    void allocateSelectedPane();
    void deleteSelectedPane();
    void deleteMap();
    void releaseChildren();
    void hideMenuPanes();
    void updateSelectionPosition();
    void updateSelectionPanePosition();
    void updateCompassRoute();
    void forwardButtonEvent(u32 receiver);
    void pollButtonEvent();
    void bindActiveButton();
    void processMapAction();
    void executeMapMenu();
    void updateOpeningButton();
    void updateMarkerPanePosition();
    void updateChartIconFrames();
    void updateProgressIcons();
    u32 createMenuChild(hdPartsDesc_c* descriptor);
    u32 loadMenuLayout(void* name, u32 first, u32 second, u32 third, void* archive);
    void hideSelection();
    void stopAndResetIcons();
    void setSelectionMode(u32 mode);
    void openMap();
    void closeMap();
    void stopIconAnimations();
    static u32 getMapResource(u32 index);
    static s32 findMapResource(u32 resource);
};
WWHD_OFFSET(hdSeaChartMenu_c, mpMenuPanes, 0x50);
WWHD_OFFSET(hdSeaChartMenu_c, mpMainPane, 0x70);
WWHD_OFFSET(hdSeaChartMenu_c, mpInfoPanes, 0x74);
WWHD_OFFSET(hdSeaChartMenu_c, mpTextPane, 0x80);
WWHD_OFFSET(hdSeaChartMenu_c, mpMapMarkerPanes, 0x84);
WWHD_OFFSET(hdSeaChartMenu_c, mpSelectedPane, 0x148);
WWHD_OFFSET(hdSeaChartMenu_c, mpMap, 0x14C);
WWHD_OFFSET(hdSeaChartMenu_c, mpActiveButton, 0x150);
WWHD_OFFSET(hdSeaChartMenu_c, mMapInputActive, 0x154);
WWHD_OFFSET(hdSeaChartMenu_c, mSelectionMode, 0x158);
WWHD_OFFSET(hdSeaChartMenu_c, mPreviousSelection, 0x15C);
WWHD_OFFSET(hdSeaChartMenu_c, mOpening, 0x160);
WWHD_OFFSET(hdSeaChartMenu_c, mClosing, 0x161);
WWHD_SIZE(hdSeaChartMenu_c, 0x164);
