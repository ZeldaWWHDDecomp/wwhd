/* HD WarpMap_00: sea-chart links and warp-area children. No GameCube counterpart. */
#pragma once
#include "d/hd_screen_warp_area.h"
#include "d/hd_parts_button.h"
#include "d/hd_screen_sea_chart_menu.h"
#include "d/hd_sea_chart_components.h"
struct hdWarpMapChildDesc_l;
struct hdWarpMap_c : hdParts_c {
    gptr<hdPartsPaneSRT_c> mpAreaName;
    gptr<hdSeaChartMenu_c> mpSeaChart;
    be<s32> mSelectedArea;
    be<u8> mConfirmed;
    u8 mPadding5D[3];
    static hdWarpMap_c* construct(hdWarpMap_c* map);
    void updateAreaName(s32 area, u32 visible);
    void selectArea(s32 area, u32 visible);
    void bindSeaChart();
    u32 createChild(hdWarpMapChildDesc_l* description);
    void release();
    void bindAreaName();
    void initializeAreaName();
    void initializeLayout();
    void startIn();
    void restoreDisplayMode();
    void confirmWarp();
    void storeDestination();
    void focusArea(u32 firstArea);
    void focusFirstArea();
    void startDisplay();
    void executeConfirmation();
    void executeDisplay();
    u32 setup(u32 first, u32 second, void* description);
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* description);
    static void destroy(hdWarpMap_c* map, u32 flags);
    void refreshSeaChart();
    void initializeWarpAreas();
    void startOut();
    void calculateOut();
    void startAreaNameOut();
    void calculateAreaNameOut();
    void executeStateMachine();
    void executeIn();
    void startAreaNameIn();
    void executeAreaNameIn();
};
WWHD_OFFSET(hdWarpMap_c, mpSeaChart, 0x54);
WWHD_OFFSET(hdWarpMap_c, mSelectedArea, 0x58);
struct hdWarpMapManagerPointer_l { gptr<void> mpManager; };
WWHD_SIZE(hdWarpMapManagerPointer_l, 4);
extern void hdWarpMap_destroyBase(hdWarpMap_c* map, u32 flags);
extern void hdWarpMap_prepareDetails(void* details);
extern void hdWarpMap_refreshDetails(void* details);
extern void hdWarpMap_resetDetails(void* details);
extern void hdWarpMap_setInputMode(void* manager, u32 mode);
extern void hdWarpMap_startAnimation(hdAnimSet* animation, u32 track, u32 flags, f32 frame);
extern void hdWarpMap_executeMachine(hdStateMachine* machine);

extern void hdWarpMap_setText(void* panel, void* text);
extern u32 hdWarpMap_animationDone(hdAnimSet* animation, u32 track);
extern void hdWarpMap_changeState(hdStateMachine* machine, hdPartsStateId_c* state);

WWHD_SIZE(hdWarpMap_c, 0x60);
WWHD_OFFSET(hdWarpMap_c, mpAreaName, 0x50);
WWHD_OFFSET(hdWarpMap_c, mConfirmed, 0x5C);
struct hdWarpMapTextOwner_l { u8 mReserved[0x1BC]; gptr<void> mpTextSource; };
WWHD_OFFSET(hdWarpMapTextOwner_l, mpTextSource, 0x1BC);
struct hdWarpMapTextOwnerPointer_l { gptr<hdWarpMapTextOwner_l> mpOwner; };
extern void* hdWarpMap_getAreaText(void* source);
extern void hdWarpMap_constructBase(hdWarpMap_c* map);
extern void hdWarpMap_releaseBase(hdWarpMap_c* map);

struct hdWarpMapTextPaneVtbl_l : hdPartsPaneSRTVtbl_c {
    u8 mReserved60[0xC4 - 0x60];
    gfn mSetText;
};
WWHD_OFFSET(hdWarpMapTextPaneVtbl_l, mSetText, 0xC4);
extern void hdWarpMap_setTextCapacity(hdPartsPaneSRT_c* pane, u32 capacity);
extern void hdWarpMap_stopAnimation(hdAnimSet* animation, u32 track, u32 flags);

struct hdWarpMapDestinationActor_l { u8 mReserved[0x6AC]; be<u32> mWarpDestination; };
WWHD_OFFSET(hdWarpMapDestinationActor_l, mWarpDestination, 0x6AC);
struct hdWarpMapPlay_l {
    u8 mReserved[0x5B3C];
    gptr<hdWarpMapDestinationActor_l> mpDestinationActor;
    u8 mReserved5B40[0xE8];
    be<u8> mDisplayMode, mDisplaySubmode, mDisplayOption;
    be<u8> mSavedDisplayMode, mSavedDisplaySubmode, mSavedDisplayOption;
};
WWHD_OFFSET(hdWarpMapPlay_l, mpDestinationActor, 0x5B3C);
WWHD_OFFSET(hdWarpMapPlay_l, mDisplayMode, 0x5C28);
WWHD_OFFSET(hdWarpMapPlay_l, mSavedDisplayOption, 0x5C2D);
struct hdWarpMapDisplayControl_l {
    u8 mReserved[0x40]; be<u8> mMode;
    u8 mReserved41[0x19]; be<u8> mActive;
};
WWHD_OFFSET(hdWarpMapDisplayControl_l, mActive, 0x5A);
struct hdWarpMapStatus_l { u8 mReserved[0x18]; be<u32> mActive; };
struct hdWarpMapStatusPointer_l { gptr<hdWarpMapStatus_l> mpStatus; };
extern hdWarpMapPlay_l* hdWarpMap_getPlay();
extern void hdWarpMap_playSound(u32 sound);

extern u32 hdWarpMap_findReceiver(hdSeaChartGrid_c* details, HdPadCoordinate* key);
extern hdSeaChartCell_c* hdWarpMap_receiverItem(hdSeaChartGrid_c* details, u32 receiver);
extern void hdWarpMap_prepareAreaItem(hdSeaChartCell_c* item);
extern void hdWarpMap_activateAreaItem(hdSeaChartCell_c* item);

extern void hdWarpMap_attachArchives(hdWarpMap_c* map, hdUiSafeString* names, u32 count);
extern hdPartsLayout_c* hdWarpMap_allocateLayout(u32 size, hdScreenHeap* heap, u32 alignment);
extern hdPartsLayout_c* hdWarpMap_constructLayout(hdPartsLayout_c* layout);
extern void hdWarpMap_bindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane);
struct hdWarpMapUiSystemPointer_l { gptr<hdScreenUiSystem> mpSystem; };

struct hdWarpMapTextSource_l {
    u8 mReserved[0x18]; hdStateMachine mMachine;
    u8 mReserved2C[0xA5]; be<u8> mReady;
    u8 mReservedD2[3]; be<u8> mAccepted;
};
WWHD_OFFSET(hdWarpMapTextSource_l, mReady, 0xD1);
WWHD_OFFSET(hdWarpMapTextSource_l, mAccepted, 0xD5);
extern hdPartsStateId_c* hdWarpMap_currentState(hdStateMachine* machine);

extern void hdWarpMap_copyEvent(hdButtonEvent_l* output, hdButtonEvent_l* input);
extern u32 hdWarpMap_isCancel(hdButtonEvent_l* event);

/* Resource and pane names consumed by the layout's child factory. */
struct hdWarpMapChildDesc_l {
    u8 mReserved00[8]; hdUiSafeString mResourceName;
    u8 mReserved10[4]; hdUiSafeString mPaneName;
};
WWHD_OFFSET(hdWarpMapChildDesc_l, mPaneName, 0x14);
extern hdParts_c* hdWarpMap_allocateChild(u32 size, hdScreenHeap* heap, u32 alignment);
extern u32 hdWarpMap_createBaseChild(hdWarpMap_c* map, u32 index, hdWarpMapChildDesc_l* description);
