/* HD dungeon floor variants. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_gamepad_map.h"
#include "d/hd_screen_sea_chart_map.h"
struct hdDungeonFloorFlags_l {
 be<u8> mEntered; u8 mReserved1[5]; be<u8> mDestinationRequested;
 u8 mReserved7[5]; be<u32> mCurrentFloorIndex;
 u8 mReserved10[0x514-0x10]; be<u32> mNeedsUpdate; u8 mReserved518[0xB]; be<u8> mFirstRoom;
};
using hdDungeonFloorCell_l = SeaMapTile_l;
struct hdDungeonFloorPointWords { be<u32> x,y; };
struct hdDungeonFloorPoint3Words { be<u32> x,y,z; };
struct hdDungeonMapFloor_l : hdParts_c {
 hdDungeonFloorCell_l mFloorCells[10];
 hdDungeonFloorCell_l mReferenceCell;
 u8 mReserved49C[0x4B0-0x49C];
 gptr<void> mpCellContext; u8 mReserved4B4[4];
 gptr<hdDungeonFloorFlags_l> mpFloorFlags;
 u8 mReserved4BC[3]; be<u8> mSelectedRoom; u8 mReserved4C0[0x20];
 be<f32> mOriginX;
 u8 mReserved4E4[0x50C-0x4E4];
 hdDungeonFloorPointWords mUpperPosition,mLowerPosition;
 be<u32> mFloorCount;
 be<s32> mChildCount;
 u8 mReserved524[4];
 gptr<gptr<hdParts_c>> mpFloorChildren;
 gptr<hdUiSafeString> mpFloorCaptions;
 u8 mReserved530[0x5D8 - 0x530];
 be<u32> mEnterStart,mEnterEnd,mLeaveStart,mLeaveEnd;
};
WWHD_OFFSET(hdDungeonMapFloor_l, mFloorCount, 0x51C);
WWHD_OFFSET(hdDungeonMapFloor_l, mpFloorCaptions, 0x52C);
WWHD_SIZE(hdDungeonMapFloor_l, 0x5E8);
struct hdDungeonMapFloorFirst : hdDungeonMapFloor_l {
 static hdDungeonMapFloorFirst* construct(hdDungeonMapFloorFirst*);
 void destroy(u32);
 u32 loadFloor(u32, u32, void*);
 u32 loadLayout(void*,u32,u32,u32,void*);
 void finalizeFloor();
 void drawFloor(void*);
};
WWHD_SIZE(hdDungeonMapFloorFirst, 0x5E8);
struct hdDungeonMapFloorSecond : hdDungeonMapFloor_l {
 static hdDungeonMapFloorSecond* construct(hdDungeonMapFloorSecond*);
 void destroy(u32);
 u32 loadFloor(u32, u32, void*);
 u32 loadLayout(void*,u32,u32,u32,void*);
 void finalizeFloor();
 void drawFloor(void*);
};
WWHD_SIZE(hdDungeonMapFloorSecond, 0x5E8);
struct hdDungeonMapFloorThird : hdDungeonMapFloor_l {
 static hdDungeonMapFloorThird* construct(hdDungeonMapFloorThird*);
 void destroy(u32);
 u32 loadFloor(u32, u32, void*);
 u32 loadLayout(void*,u32,u32,u32,void*);
 void finalizeFloor();
 void drawFloor(void*);
};
WWHD_SIZE(hdDungeonMapFloorThird, 0x5E8);
struct hdDungeonMapFloorFourth : hdDungeonMapFloor_l {
 static hdDungeonMapFloorFourth* construct(hdDungeonMapFloorFourth*);
 void destroy(u32);
 u32 loadFloor(u32, u32, void*);
 u32 loadLayout(void*,u32,u32,u32,void*);
 void finalizeFloor();
 void drawFloor(void*);
};
WWHD_SIZE(hdDungeonMapFloorFourth, 0x5E8);
struct hdDungeonMapFloorFifth : hdDungeonMapFloor_l {
 static hdDungeonMapFloorFifth* construct(hdDungeonMapFloorFifth*);
 void destroy(u32);
 u32 loadFloor(u32, u32, void*);
 u32 loadLayout(void*,u32,u32,u32,void*);
 void finalizeFloor();
 void drawFloor(void*);
};
WWHD_SIZE(hdDungeonMapFloorFifth, 0x5E8);

struct hdDungeonFloorNameTable { hdUiSafeString mNames[56]; };
WWHD_SIZE(hdDungeonFloorNameTable,0x1C0);
struct hdDungeonFloorDesc_l { u8 mReserved0[0x14]; hdUiSafeString mName; u8 mReserved1C[12]; };
WWHD_OFFSET(hdDungeonFloorDesc_l,mName,0x14);
struct hdDungeonMapFloorMain : hdDungeonMapFloor_l {
 be<u8> mDestinationActive;
 u8 mPadding[3];
 static hdDungeonMapFloorMain* construct(hdDungeonMapFloorMain*);
 void destroy(u32);
 u32 loadFloor(u32,u32,void*);
 void finalizeFloor();
 void drawFloor(void*);
 void showDestination();
 void hideDestination();
 void resetDestination();
 u32 destinationFinished();
 void updateDestination();
 void updateButtons();
 void clearUnavailableButtonFocus();
 hdDungeonFloorCell_l* findFloorCell(u32);
 void executeFloor();
 void resetFloor();
 s32 currentRoom();
 void resetAnimations();
 void prepareUpperCell();
 void arrangeCurrentCell();
 hdParts_c* findFloorChild(u32);
 void arrangeFloorOffsets();
 void clearUpperTarget();
 void resetUpperCell();
 u32 createFloorChild(hdDungeonFloorDesc_l*);
 u32 loadLayout(void*,u32,u32,u32,void*);
};
WWHD_OFFSET(hdDungeonMapFloorMain,mDestinationActive,0x5E8);
WWHD_SIZE(hdDungeonMapFloorMain,0x5EC);
struct hdDungeonFloorArchiveNames { hdUiSafeString mNames[2]; };
struct hdDungeonFloorStatics {
 be<f32> mMinimum, mMaximum;
 HdPadPoint mOrigin, mOffset;
 be<u8> mFirstFlag, mSecondFlag;
 u8 mPadding[2];
 be<u32> mStorage[4];
};
struct hdDungeonFloorRegistration { be<u32> mWords[3]; };
WWHD_OFFSET(hdDungeonFloorStatics,mStorage,0x1C);
WWHD_SIZE(hdDungeonFloorStatics,0x2C);

struct hdDungeonFloorRangeTable { be<u32> mValue; };
WWHD_SIZE(hdDungeonFloorRangeTable,4);
WWHD_OFFSET(hdDungeonMapFloor_l,mpFloorFlags,0x4B8);
WWHD_OFFSET(hdDungeonMapFloor_l,mUpperPosition,0x50C);
WWHD_OFFSET(hdDungeonMapFloor_l,mLowerPosition,0x514);
WWHD_OFFSET(hdDungeonMapFloor_l,mChildCount,0x520);
WWHD_OFFSET(hdDungeonMapFloor_l,mpFloorChildren,0x528);
struct hdDungeonFloorVtable : hdPartsVtbl_c { u8 mReservedC8[0x1C4-0xC8]; gfn mFindCell; u8 mReserved1C8[4]; gfn mCurrentRoom; u8 mReserved1D0[0x224-0x1D0]; gfn mFindRoom; };
WWHD_OFFSET(hdDungeonFloorVtable,mFindCell,0x1C4);
struct hdDungeonFloorAnimationRemap { be<u32> mIds[5]; };
