/** HD chart display coordinator and chart inventory. Private reconstruction. */
#pragma once
#include "d/hd_gamepad_map.h"
#include "d/hd_save_info_holder.h"
#include "hd/hd_sys_02006294.h"
struct hdChartEntry_c;
struct hdMapChartVector_l {
    be<s32> mCount;
    be<s32> mCapacity;
    gptr<gptr<hdChartEntry_c>> mpEntries;
};
WWHD_OFFSET(hdMapChartVector_l, mCount, 0);
WWHD_OFFSET(hdMapChartVector_l, mCapacity, 4);
WWHD_OFFSET(hdMapChartVector_l, mpEntries, 8);
WWHD_SIZE(hdMapChartVector_l, 0xC);
using hdChartCompareCallback = s32 (*)(hdChartEntry_c**, hdChartEntry_c**);
struct hdSelectedSeaCells_l {
    u8 mPad[0x118];
    gptr<HdPadMapCell> mpSelected;
};
WWHD_OFFSET(hdSelectedSeaCells_l, mpSelected, 0x118);
struct hdChartEntry_c {
    be<s8> mSeaNo;
    be<s8> mSaveNo;
    be<s8> mChartNo;
    be<u8> mType;
    be<u8> mUnknown4;
    be<u8> mUnknown5;
    be<s16> mPositions[16];
};
WWHD_OFFSET(hdChartEntry_c, mType, 3);
WWHD_OFFSET(hdChartEntry_c, mPositions, 6);
WWHD_SIZE(hdChartEntry_c, 0x26);
struct hdMapChartArchive_l {
    be<s32> mCount;
    hdChartEntry_c mEntries[1];
};
WWHD_OFFSET(hdMapChartArchive_l, mEntries, 4);
struct hdMapChartHeap_l {
    u8 mPad[0x10];
    gptr<void> mpHeap;
};
WWHD_OFFSET(hdMapChartHeap_l, mpHeap, 0x10);
WWHD_OFFSET(hdChartEntry_c, mSaveNo, 1);
WWHD_OFFSET(hdChartEntry_c, mChartNo, 2);
struct hdMapLocation_c {
    be<s8> mX;
    be<s8> mY;
    be<s16> mRoom;
    be<s16> mLayer;
    be<u8> mKind;
    u8 mPad;
};
struct hdMapLocationList_c {
    be<s32> mCount;
    hdMapLocation_c mEntries[1];
};
WWHD_SIZE(hdMapLocation_c, 8);
struct hdMapCoordinatorState_l {
    hdStateMachine mMachine;
    gptr<void> mpVTable;
};
WWHD_SIZE(hdMapCoordinatorState_l, 0x18);
struct hdMapStateMember_l {
    be<s16> mAdjust;
    be<s16> mVirtualIndex;
    gfn mFunction;
};
// The inherited field_0x4 holds the descriptor name; __vtbl is its guest vtable.
struct hdMapStateDescriptor_l : hdStateId {
    void setName(const char* name) { field_0x4 = gabi::guest_addr(name); }
    gfn* getVTable() const { return gabi::at<gfn>(__vtbl); }
    hdMapStateMember_l mEnter;
    hdMapStateMember_l mExecute;
    hdMapStateMember_l mLeave;
    hdMapStateMember_l mGetState;
};
WWHD_OFFSET(hdMapStateDescriptor_l, field_0x4, 4);
WWHD_OFFSET(hdMapStateDescriptor_l, __vtbl, 8);
WWHD_OFFSET(hdMapStateDescriptor_l, mEnter, 0xC);
WWHD_OFFSET(hdMapStateDescriptor_l, mExecute, 0x14);
WWHD_OFFSET(hdMapStateDescriptor_l, mLeave, 0x1C);
WWHD_OFFSET(hdMapStateDescriptor_l, mGetState, 0x24);
WWHD_SIZE(hdMapStateDescriptor_l, 0x2C);
struct hdMapChildView_l {
    u8 mPad[4];
    gptr<gfn> mpVTable;
    u8 mPadState[0x10];
    HdPadStateController mState;
    u8 mPadLink[0x3C];
    gptr<hdMapChildView_l> mpLink;
};
WWHD_OFFSET(hdMapChildView_l, mState, 0x18);
WWHD_OFFSET(hdMapChildView_l, mpLink, 0x80);
struct hdMapPlayer_l {
    u8 mPad[0xE];
    be<s16> mProcess;
    u8 mPadPosition[0x304];
    be<u32> mPositionX;
    u8 mPadY[4];
    be<u32> mPositionZ;
    u8 mPadRotation[0xA];
    be<s16> mAngle;
};
struct hdMapStageInfo_l {
    u8 mPad[9];
    be<u8> mStageType;
    be<u16> mMapMode;
    be<u32> mFlags;
};
struct hdMapStageContainer_l {
    gptr<gfn> mpVTable;
};
struct hdMapPlay_l {
    u8 mPad[0x5134];
    u8 mStageName[0x1C];
    hdMapStageContainer_l mStage;
    u8 mPadLink[0x9D8];
    gptr<hdMapPlayer_l> mpLink;
    u8 mPadBoat[0xC];
    gptr<hdMapPlayer_l> mpShip;
    u8 mPadPlayerStatus[0x198];
    be<u32> mPlayerStatus;
    u8 mPadSector[0x3A];
    be<u8> mLastSeaSector;
};
WWHD_OFFSET(hdMapPlayer_l, mPositionX, 0x314);
WWHD_OFFSET(hdMapPlayer_l, mAngle, 0x32A);
WWHD_OFFSET(hdMapPlay_l, mpShip, 0x5B3C);
struct hdMapSafeString_l {
    gptr<const char> mpText;
    gptr<gfn> mpVTable;
};
WWHD_SIZE(hdMapSafeString_l, 8);
struct hdMapCellCenter_l {
    u8 mPad[6];
    be<s16> mX;
    be<s16> mY;
};
WWHD_OFFSET(hdMapCellCenter_l, mX, 6);
WWHD_OFFSET(hdMapCellCenter_l, mY, 8);
struct hdMapHeapContext_l {
    u8 mPad[0x48];
    gptr<void> mpSecondary;
    gptr<void> mpPrimary;
    be<u32> mFlags;
    u8 mPadEmbedded[0x30];
    u8 mEmbedded;
    u8 mPadSpecial[0xDF];
    gptr<void> mpSpecial;
};
WWHD_OFFSET(hdMapHeapContext_l, mpSecondary, 0x48);
WWHD_OFFSET(hdMapHeapContext_l, mpPrimary, 0x4C);
WWHD_OFFSET(hdMapHeapContext_l, mFlags, 0x50);
WWHD_OFFSET(hdMapHeapContext_l, mEmbedded, 0x84);
WWHD_OFFSET(hdMapHeapContext_l, mpSpecial, 0x164);
struct hdMapHeapManager_l {
    u8 mPad[0x1020];
    be<u32> mCount;
    gptr<gptr<hdMapHeapContext_l>> mpContexts;
};
WWHD_OFFSET(hdMapHeapManager_l, mCount, 0x1020);
WWHD_OFFSET(hdMapHeapManager_l, mpContexts, 0x1024);
struct hdMapDisplay_c {
    u8 mPadStart[0x10];
    HdPadStateController mState;
};
struct hdMapDisplayCoordinator_c {
    gptr<void> mpOwner;
    gptr<gfn> mpVTable;
    gptr<void> mpParent;
    be<u32> mBaseFlags;
    hdMapCoordinatorState_l mState;
    gptr<HdPadMenuMap> mpSeaMap;
    gptr<HdPadDungeon> mpDungeonMap;
    HdPadPoint mSeaPosition;
    be<f32> mSeaScale;
    HdPadCoordinate mSeaCoordinate;
    be<s8> mSelectedEntry;
    u8 mPadSelected;
    HdPadPoint mDungeonPosition;
    be<f32> mDungeonScale;
    HdPadCoordinate mDungeonCoordinate;
    be<u8> mRestoreSettings;
    u8 mPadRestore;
    be<s32> mStageKind;
    be<s32> mMapKind;
    be<u8> mSpecialDungeon;
    be<u8> mInterior;
    be<u8> mBoss;
    u8 mPadFlags;
    be<u32> mDetectedMode;
    be<u32> mMode;
    be<u32> mPreviousMode;
    be<u8> mOpened;
    be<u8> mVisible;
    u8 mPadVisible[2];
    be<u32> mInputMode;
    be<u32> mMapFlags;
    be<u8> mShowLink;
    u8 mPadLink[3];
    gptr<hdMapLocationList_c> mpLocations;
    gptr<hdMapChartArchive_l> mpChartArchive;
    be<u32> mChartStorage[61];
    be<s32> mChartCount;
    be<s32> mChartCapacity;
    gptr<gptr<hdChartEntry_c>> mpCharts;
    void enterDisplayMenuMap();
    void enterDisplayDungeonMap();
    void enterHideMenuMap();
    void enterHideDungeonMap();
    void resetMenuMap();
    void resetDungeonMap();
    void leaveChangeMenuMap();
    void leaveChangeDungeonMap();
    void restoreInputMode();
    u32 isSeaMapForced();
    u32 isDungeonMapForced();
    u32 isMapForced();
    u32 isDungeonMode();
    u32 isDungeonStage();
    u32 detectMode();
    void updateMapKind();
    u32 canRestoreSettings();
    void enableRestoreSettings();
    hdChartEntry_c* getChart(s32 index);
    hdChartEntry_c* findChart(u32 chart);
    hdChartEntry_c* findSeaChart(u32 chart);
    void updateSeaMapSelection();
    void updateSeaMapCursor();
    void clearSeaMapSelection();
    u32 isSeaMapSelected();
    u32 isSeaMapCursorActive();
    u32 isSeaMapSelectionEmpty();
    void updateDungeonMapSelection();
    void clearDungeonMapSelection();
    u32 isTowerMap();
    u32 isDungeonOrTowerMap();
    u32 isForsakenFortressMap();
    u32 isDungeonMapRestricted();
    void clearChartProgress(s32 saveNo);
    u32 countObtainedCharts(s32 first, s32 count);
    u32 countCompletedCharts(s32 first, s32 count);
    u32 countObtainedSpecialCharts();
    u32 countObtainedSeaCharts();
    u32 countObtainedOtherCharts();
    u32 countCompletedSpecialCharts();
    u32 countCompletedSeaCharts();
    u32 countAllObtainedCharts();
    u32 countCollectedCharts();
    u32 countAllCompletedCharts();
    void obtainChart(u32 chart);
    u32 isChartObtained(u32 chart);
    u32 isChartOpened(u32 chart);
    u32 isChartComplete(u32 chart);
    u32 isTriforceChart(u32 chart);
    u32 isSeaChartComplete(u32 chart);
    void* getOwner();
    static void clearInstances();
    static void setInstance(hdMapDisplayCoordinator_c* instance, u32 index);
    static hdMapDisplayCoordinator_c* getInstance(u32 index);
    hdMapLocation_c* getLocation(s32 index);
    s32 findLocation(u32 x, u32 y);
    void updateLocationSelection();
    void updateLocations();
    void updateLocationList();
    void updateDungeonCoordinate();
    u32 isDungeonInterior();
    u32 isSeaExterior();
    void setSeaMapMarker(u32 unused, u32 marker);
    void setSeaMapMarkerForward(u32 unused, u32 marker);
    void setMarkersActive(u32 active);
    void updateSeaCells();
    s32 getSelectedSeaCell();
    u32 isSavedSeaCell(s32 cell);
    u32 isSelectedCellSaved();
    void saveLocationSelection(s32 location);
    void saveCurrentLocationSelection();
    void getLocationInfo(s32 index, be<u8>* x, be<u8>* y, be<s16>* room,
                         be<s16>* layer, be<u8>* kind);
    void executeMenuMap();
    void executeDungeonMap();
    void executeDisplayMenuMap();
    void executeHideMenuMap();
    void executeHideDungeonMap();
    void executeDisplayDungeonMap();
    void openSelectedMap();
    void changeSelectedMap();
    void resetMaps();
    void showMenuMap();
    void showDungeonMap();
    void showSelectedMap();
    void configureSeaMap(u32 origin, u32 mode, u32 scale);
    void configureDungeonMap(u32 origin, u32 mode, u32 scale);
    void saveMapSettings();
    void loadMapSettings();
    void saveSelectedMapSettings();
    void initializeMapMode();
    void updateMapMode();
    void updateStageKind();
    void updateStageFlags();
    u32 canUpdatePlayerPosition();
    u32 isLinkProcess();
    void updateLinkPosition();
    void updateShipPosition();
    void updatePlayerPositions();
    s32 getStageType();
    void updateInteriorFlags();
    u32 detectForcedMapMode();
    void enterChangeToMenuMap();
    void enterChangeToDungeonMap();
    void executeChangeToMenuMap();
    void executeChangeToDungeonMap();
    u32 canSaveSelectedCell();
    u32 isRoomVisited(s32 room);
    u32 isTreasureOpened(u32 chest);
    u32 hasDungeonMap();
    u32 hasDungeonBossKey();
    u32 hasDungeonCompass();
    static hdMapDisplayCoordinator_c*
    construct(hdMapDisplayCoordinator_c* instance);
    void destroy(u32 flags);
    void destroyMaps();

    void execute();
    void processInput(u32 event, u32 context);
    u32 isSpecialChartUnlocked(u32 index);
    void initializeCharts();
    u32 setup();
    enum MapKind {
        MAP_SEA = 0,
        MAP_SHOP = 1,
        MAP_ABANDONED_SHIP = 2,
        MAP_PIRATE_SHIP = 3,
        MAP_FORTRESS_EXTERIOR = 4,
        MAP_FORTRESS_INTERIOR = 5,
        MAP_SAVAGE_LABYRINTH = 6,
        MAP_SAVAGE_LABYRINTH_LOWER = 7,
        MAP_ADMUMI = 8,
        MAP_OTHER = 9
    };
    enum {
        MODE_SEA = 0,
        MODE_DUNGEON = 1,
        CHART_SPECIAL_FIRST = 0,
        CHART_SPECIAL_COUNT = 3,
        CHART_SEA_FIRST = 3,
        CHART_SEA_COUNT = 46,
        CHART_OTHER_FIRST = 49,
        CHART_OTHER_COUNT = 12,
        CHART_TOTAL = 61
    };
};
WWHD_OFFSET(hdMapDisplayCoordinator_c, mpSeaMap, 0x28);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mpDungeonMap, 0x2c);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mSeaPosition, 0x30);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mSeaScale, 0x38);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mDungeonPosition, 0x40);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mDungeonScale, 0x48);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mRestoreSettings, 0x4e);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mStageKind, 0x50);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mMapKind, 0x54);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mSpecialDungeon, 0x58);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mInterior, 0x59);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mBoss, 0x5a);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mDetectedMode, 0x5c);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mMode, 0x60);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mPreviousMode, 0x64);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mOpened, 0x68);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mVisible, 0x69);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mInputMode, 0x6c);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mShowLink, 0x74);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mChartCount, 0x174);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mpCharts, 0x17c);
WWHD_SIZE(hdMapDisplayCoordinator_c, 0x180);
WWHD_OFFSET(hdChartEntry_c, mSeaNo, 0x0);
WWHD_OFFSET(hdChartEntry_c, mUnknown4, 0x4);
WWHD_OFFSET(hdChartEntry_c, mUnknown5, 0x5);
WWHD_OFFSET(hdMapChartArchive_l, mCount, 0x0);
WWHD_OFFSET(hdMapLocation_c, mX, 0x0);
WWHD_OFFSET(hdMapLocation_c, mY, 0x1);
WWHD_OFFSET(hdMapLocation_c, mRoom, 0x2);
WWHD_OFFSET(hdMapLocation_c, mLayer, 0x4);
WWHD_OFFSET(hdMapLocation_c, mKind, 0x6);
WWHD_OFFSET(hdMapLocationList_c, mCount, 0x0);
WWHD_OFFSET(hdMapLocationList_c, mEntries, 0x4);
WWHD_OFFSET(hdMapCoordinatorState_l, mMachine, 0);
WWHD_OFFSET(hdMapCoordinatorState_l, mpVTable, 0x14);
WWHD_OFFSET(hdMapStateMember_l, mAdjust, 0x0);
WWHD_OFFSET(hdMapStateMember_l, mVirtualIndex, 0x2);
WWHD_OFFSET(hdMapStateMember_l, mFunction, 0x4);
WWHD_OFFSET(hdMapStateDescriptor_l, mId, 0x0);
WWHD_OFFSET(hdMapChildView_l, mpVTable, 0x4);
WWHD_OFFSET(hdMapPlayer_l, mProcess, 0xe);
WWHD_OFFSET(hdMapPlayer_l, mPositionZ, 0x31c);
WWHD_OFFSET(hdMapStageInfo_l, mStageType, 0x9);
WWHD_OFFSET(hdMapStageInfo_l, mMapMode, 0xa);
WWHD_OFFSET(hdMapStageInfo_l, mFlags, 0xc);
WWHD_OFFSET(hdMapStageContainer_l, mpVTable, 0x0);
WWHD_OFFSET(hdMapPlay_l, mStageName, 0x5134);
WWHD_OFFSET(hdMapPlay_l, mStage, 0x5150);
WWHD_OFFSET(hdMapPlay_l, mpLink, 0x5b2c);
WWHD_OFFSET(hdMapPlay_l, mPlayerStatus, 0x5cd8);
WWHD_OFFSET(hdMapPlay_l, mLastSeaSector, 0x5d16);
WWHD_OFFSET(hdMapSafeString_l, mpText, 0x0);
WWHD_OFFSET(hdMapSafeString_l, mpVTable, 0x4);
WWHD_OFFSET(hdMapDisplay_c, mState, 0x10);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mpOwner, 0x0);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mpVTable, 0x4);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mpParent, 0x8);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mBaseFlags, 0xc);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mState, 0x10);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mSeaCoordinate, 0x3c);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mSelectedEntry, 0x3e);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mDungeonCoordinate, 0x4c);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mMapFlags, 0x70);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mpLocations, 0x78);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mpChartArchive, 0x7c);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mChartStorage, 0x80);
WWHD_OFFSET(hdMapDisplayCoordinator_c, mChartCapacity, 0x178);
WWHD_SIZE(hdMapStateMember_l, 8);

struct hdMapDisplayInstances {
    gptr<hdMapDisplayCoordinator_c> mItems[3];
};

struct hdMapInitializerWords_l {
    be<u32> mValues[4];
};

struct hdDungeonStageIds_l {
    be<s32> mIds[6];
};

struct hdMapChartSwitches_l {
    be<s32> mIds[3];
};

struct hdMapStageKindPair_l {
    be<s32> mInput;
    be<s32> mOutput;
};

struct hdMapStageKinds_l {
    hdMapStageKindPair_l mPairs[8];
};

struct hdMapPointBits_l {
    be<u32> mX;
    be<u32> mY;
};

struct hdMapPaneView_l {
    u8 mPad[0x44];
    be<u8> mFlags;
    be<u8> mAlpha;
};

struct hdMapPaneMatrix_l {
    u8 mPad[0x48];
    be<u32> mMatrix[12];
};

struct hdMapTrigSample_l {
    be<f32> mSin;
    be<f32> mSinDelta;
    be<f32> mCos;
    be<f32> mCosDelta;
};

struct hdMapTrigTable_l {
    hdMapTrigSample_l mSamples[256];
};

WWHD_OFFSET(hdMapPaneView_l, mFlags, 0x44);
WWHD_OFFSET(hdMapPaneView_l, mAlpha, 0x45);
WWHD_OFFSET(hdMapPaneMatrix_l, mMatrix, 0x48);
WWHD_SIZE(hdMapTrigSample_l, 0x10);
WWHD_OFFSET(hdMapTrigSample_l, mSinDelta, 4);
WWHD_OFFSET(hdMapTrigSample_l, mCos, 8);
WWHD_OFFSET(hdMapTrigSample_l, mCosDelta, 0xC);
WWHD_SIZE(hdMapDisplayInstances, 0xC);
WWHD_OFFSET(hdMapDisplayInstances, mItems, 0);
WWHD_SIZE(hdMapInitializerWords_l, 0x10);
WWHD_OFFSET(hdMapInitializerWords_l, mValues, 0);
WWHD_SIZE(hdDungeonStageIds_l, 0x18);
WWHD_OFFSET(hdDungeonStageIds_l, mIds, 0);
WWHD_SIZE(hdMapChartSwitches_l, 0xC);
WWHD_OFFSET(hdMapChartSwitches_l, mIds, 0);
WWHD_SIZE(hdMapStageKindPair_l, 8);
WWHD_OFFSET(hdMapStageKindPair_l, mInput, 0);
WWHD_OFFSET(hdMapStageKindPair_l, mOutput, 4);
WWHD_SIZE(hdMapStageKinds_l, 0x40);
WWHD_OFFSET(hdMapStageKinds_l, mPairs, 0);
WWHD_SIZE(hdMapPointBits_l, 8);
WWHD_OFFSET(hdMapPointBits_l, mX, 0);
WWHD_OFFSET(hdMapPointBits_l, mY, 4);
WWHD_SIZE(hdMapPaneView_l, 0x46);
WWHD_SIZE(hdMapPaneMatrix_l, 0x78);
WWHD_OFFSET(hdMapTrigSample_l, mSin, 0);
WWHD_SIZE(hdMapTrigTable_l, 0x1000);
WWHD_OFFSET(hdMapTrigTable_l, mSamples, 0);
