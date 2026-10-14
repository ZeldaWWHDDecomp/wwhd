/* HD dungeon lower-floor indicator and its caption. */
#pragma once
#include "d/hd_dungeon_map_markers.h"
#include "d/hd_sea_chart_state_handlers.h"
struct hdDungeonDownCaptionTable { u8 mReserved0[0xC4]; gfn mSetText; };
struct hdDungeonDownCaption { u8 mReserved0[8]; gptr<hdDungeonDownCaptionTable> mVtable; };
struct hdDungeonDownRootTable { u8 mReserved0[0x5C]; gfn mFindPane; };
struct hdDungeonDownRoot { u8 mReserved0[8]; gptr<hdDungeonDownRootTable> mVtable; };
struct hdDungeonDownAttachPane { u8 mReserved0[0x80]; be<u8> mStageName[1]; };
struct hdDungeonDownVisibilityPane { u8 mReserved0[0x45]; be<u8> mVisible; };
struct hdDungeonDownStringTable { hdUiSafeString mNames[5]; };
struct hdDungeonDownFloorRecord { gptr<hdUiSafeString> mpName; be<u32> mFloor; };
struct hdDungeonDownFloors { hdDungeonDownFloorRecord mRecords[5]; };
struct hdDungeonDownEmptyCaption { be<u16> mText; };
struct hdDungeonMapDown : hdSeaChartStateHandlers_c {
 gptr<hdDungeonDownCaption> mpCaption;
 gptr<hdDungeonDownAttachPane> mpAttachPane;
 gptr<hdDungeonDownVisibilityPane> mpVisibilityPane;
 be<u8> mUseStageProgress;
 u8 mPadding[3];
 static hdDungeonMapDown* construct(hdDungeonMapDown*);
 void destroy(u32);
 void findCaption();
 void resetCaption();
 u32 loadIndicator(u32,u32,void*);
 void finalizeIndicator();
 void executeIndicator();
 void drawIndicator(void*);
 u32 stageProgressVisible();
 void displayIndicator();
 void setCaptionLabel(hdDungeonDownCaption*,hdUiSafeString*);
 void updateCaption();
 void detectStageProgress();
};
WWHD_OFFSET(hdDungeonMapDown,mpCaption,0xA0);
WWHD_OFFSET(hdDungeonMapDown,mpAttachPane,0xA4);
WWHD_OFFSET(hdDungeonMapDown,mpVisibilityPane,0xA8);
WWHD_OFFSET(hdDungeonMapDown,mUseStageProgress,0xAC);
WWHD_SIZE(hdDungeonMapDown,0xB0);
