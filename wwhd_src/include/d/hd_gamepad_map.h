/* HD gamepad map, pane and cell views. Private reconstruction. */
#pragma once
#include "wwhd.h"
struct HdPadPoint {
    be<f32> x;
    be<f32> y;
};
struct HdPadPoint3 {
    be<f32> x;
    be<f32> y;
    be<f32> z;
};
struct HdPadRect {
    be<f32> mLeft;
    be<f32> mTop;
    be<f32> mRight;
    be<f32> mBottom;
};
struct HdPadCoordinate {
    be<s8> x;
    be<s8> y;
};
struct HdPadState {
    u8 mOpaque;
};
struct HdPadStateController {
    u8 mOpaque[0x2C];
};
struct HdPadEvent {
    be<u32> mKind;
    be<u32> mId;
    u8 mPad[0x20];
};
struct HdPadEventSource {
    u8 mPad[0x30];
    HdPadEvent mEvent;
};
struct HdPadInputMode {
    u8 mPad[0x10];
    be<u32> mId;
};
struct HdPadFloatBits {
    union {
        u32 mBits;
        f32 mValue;
    };
};
struct HdPadLayout {
    u8 mPad0[20];
    be<f32> mWidth;
    be<f32> mHeight;
};
struct HdPadPane {
    u8 mPad0[44];
    be<f32> mScale;
    u8 mPad30[4];
    be<f32> mScaleX;
    be<f32> mScaleY;
    be<f32> mWidth;
    be<f32> mHeight;
};
struct HdPadAnimation {
    u8 mPad0[4];
    gptr<HdPadLayout> mLayout;
    u8 mPad8[204];
    gptr<void> mPlayer;
};
struct HdPadUi {
    u8 mPad0[44];
    gptr<void> mMapMode;
    u8 mPad30[12];
    HdPadCoordinate mCoordinate;
    u8 mPad3E[18];
    be<s32> mBossIndex;
};
struct HdPadUiOwner {
    u8 mPad0[536];
    gptr<HdPadUi> mUi;
};
struct HdPadOwnerPane {
    u8 mPad0[24];
    gptr<HdPadPane> mPane;
};
struct HdPadStick {
    u8 mPad0[312];
    HdPadPoint mPosition;
};
struct HdPadInputActive {
    u8 mPad0[20];
    be<u8> mActive;
};
struct HdPadMap {
    u8 mPad0[4];
    gptr<gfn> mVTable;
    u8 mPad8[16];
    HdPadStateController mState;
    gptr<HdPadAnimation> mAnimation;
    u8 mPad48[1124];
    be<f32> mAngle;
    u8 mPad4B0[8];
    be<u32> mBossVisible;
    be<u8> mZoomMode;
    u8 mPad4BD[35];
    HdPadPoint mOrigin;
    be<f32> mZoomStart;
    be<f32> mScale;
    HdPadPoint mCurrent;
    HdPadPoint mTarget;
    u8 mPad500[4];
    be<f32> mZoomX;
    be<f32> mZoomY;
};
struct HdPadDungeon {
    u8 mPad0[4];
    gptr<gfn> mVTable;
    u8 mPad8[16];
    HdPadStateController mState;
    gptr<HdPadAnimation> mAnimation;
    u8 mPad48[8];
    gptr<HdPadPane> mPane;
    u8 mPad54[72];
    gptr<HdPadInputMode> mInputMode;
    u8 mPadA0[852];
    HdPadPoint mDragPosition;
    u8 mPad3FC[1316];
    gptr<HdPadMap> mMap;
    be<u8> mMarkersActive;
};
struct HdPadBoss {
    u8 mPad0[24];
    HdPadStateController mState;
    u8 mPad44[44];
    HdPadPoint mPosition;
    u8 mPad78[24];
    gptr<HdPadMap> mMap;
    gptr<HdPadPane> mPane;
    HdPadPoint mOrigin;
};
struct HdPadMapCell {
    be<u32> mId;
    HdPadCoordinate mCoordinate;
    u8 mPad6[14];
    HdPadPoint mPosition;
    HdPadPoint mSize;
    HdPadRect mBounds;
    HdPadPoint mScaledPosition;
    HdPadPoint mScaledSize;
    HdPadRect mScaledBounds;
    be<u32> mBaseResource;
    be<u32> mResources[8];
    u8 mPad78[31];
    be<u8> mDiscovered;
};
struct HdPadMoveCell {
    u8 mPad0[24];
    HdPadPoint mCurrent;
    HdPadPoint mPosition;
};
struct HdPadCellArray {
    gptr<HdPadMapCell> mCells[49];
};
struct HdPadGrid {
    gptr<HdPadMapCell> mCells[49];
    gptr<HdPadOwnerPane> mOwner;
    u8 mPadC8[4];
    be<f32> mOriginX;
    u8 mPadD0[8];
    be<f32> mOriginY;
    HdPadPoint mCellSize;
    be<u32> mBaseResource;
    be<u32> mResources[8];
    u8 mPad108[40];
    be<u32> mDiscoveredCount;
    be<u32> mPreviousCount;
    u8 mPad138[4];
    be<u8> mChanged;
    u8 mPad13D[7];
    be<u32> mOffsetMode;
    HdPadPoint mOffset;
};
struct HdPadMenuMap {
    u8 mPad0[4];
    gptr<gfn> mVTable;
    u8 mPad8[16];
    HdPadStateController mState;
    gptr<HdPadAnimation> mAnimation;
    u8 mPad48[124];
    be<u32> mSoundFrame;
    u8 mPadC8[84];
    gptr<struct HdPadMapOwner> mMap;
    u8 mPad120[4];
    be<u32> mIconFrame;
};
struct HdPadMapOwner {
    u8 mPad0[332];
    gptr<HdPadCellArray> mCells;
    gptr<void> mIcons;
};
struct HdPadSave {
    u8 mPad0[228];
    u8 mMapData;
};
WWHD_OFFSET(HdPadLayout, mWidth, 0x14);
WWHD_OFFSET(HdPadLayout, mHeight, 0x18);
WWHD_OFFSET(HdPadPane, mScale, 0x2C);
WWHD_OFFSET(HdPadPane, mScaleX, 0x34);
WWHD_OFFSET(HdPadPane, mScaleY, 0x38);
WWHD_OFFSET(HdPadPane, mWidth, 0x3C);
WWHD_OFFSET(HdPadPane, mHeight, 0x40);
WWHD_OFFSET(HdPadAnimation, mLayout, 0x4);
WWHD_OFFSET(HdPadAnimation, mPlayer, 0xD4);
WWHD_OFFSET(HdPadUi, mMapMode, 0x2C);
WWHD_OFFSET(HdPadUi, mCoordinate, 0x3C);
WWHD_OFFSET(HdPadUi, mBossIndex, 0x50);
WWHD_OFFSET(HdPadUiOwner, mUi, 0x218);
WWHD_OFFSET(HdPadOwnerPane, mPane, 0x18);
WWHD_OFFSET(HdPadStick, mPosition, 0x138);
WWHD_OFFSET(HdPadInputActive, mActive, 0x14);
WWHD_OFFSET(HdPadMap, mVTable, 0x4);
WWHD_OFFSET(HdPadMap, mState, 0x18);
WWHD_OFFSET(HdPadMap, mAnimation, 0x44);
WWHD_OFFSET(HdPadMap, mAngle, 0x4AC);
WWHD_OFFSET(HdPadMap, mBossVisible, 0x4B8);
WWHD_OFFSET(HdPadMap, mZoomMode, 0x4BC);
WWHD_OFFSET(HdPadMap, mOrigin, 0x4E0);
WWHD_OFFSET(HdPadMap, mZoomStart, 0x4E8);
WWHD_OFFSET(HdPadMap, mScale, 0x4EC);
WWHD_OFFSET(HdPadMap, mCurrent, 0x4F0);
WWHD_OFFSET(HdPadMap, mTarget, 0x4F8);
WWHD_OFFSET(HdPadMap, mZoomX, 0x504);
WWHD_OFFSET(HdPadMap, mZoomY, 0x508);
WWHD_OFFSET(HdPadDungeon, mVTable, 0x4);
WWHD_OFFSET(HdPadDungeon, mState, 0x18);
WWHD_OFFSET(HdPadDungeon, mAnimation, 0x44);
WWHD_OFFSET(HdPadDungeon, mPane, 0x50);
WWHD_OFFSET(HdPadDungeon, mInputMode, 0x9C);
WWHD_OFFSET(HdPadDungeon, mDragPosition, 0x3F4);
WWHD_OFFSET(HdPadDungeon, mMap, 0x920);
WWHD_OFFSET(HdPadDungeon, mMarkersActive, 0x924);
WWHD_OFFSET(HdPadBoss, mState, 0x18);
WWHD_OFFSET(HdPadBoss, mPosition, 0x70);
WWHD_OFFSET(HdPadBoss, mMap, 0x90);
WWHD_OFFSET(HdPadBoss, mPane, 0x94);
WWHD_OFFSET(HdPadBoss, mOrigin, 0x98);
WWHD_OFFSET(HdPadMapCell, mId, 0x0);
WWHD_OFFSET(HdPadMapCell, mCoordinate, 0x4);
WWHD_OFFSET(HdPadMapCell, mPosition, 0x14);
WWHD_OFFSET(HdPadMapCell, mSize, 0x1C);
WWHD_OFFSET(HdPadMapCell, mBounds, 0x24);
WWHD_OFFSET(HdPadMapCell, mScaledPosition, 0x34);
WWHD_OFFSET(HdPadMapCell, mScaledSize, 0x3C);
WWHD_OFFSET(HdPadMapCell, mScaledBounds, 0x44);
WWHD_OFFSET(HdPadMapCell, mBaseResource, 0x54);
WWHD_OFFSET(HdPadMapCell, mResources, 0x58);
WWHD_OFFSET(HdPadMapCell, mDiscovered, 0x97);
WWHD_OFFSET(HdPadMoveCell, mCurrent, 0x18);
WWHD_OFFSET(HdPadMoveCell, mPosition, 0x20);
WWHD_OFFSET(HdPadCellArray, mCells, 0x0);
WWHD_OFFSET(HdPadGrid, mCells, 0x0);
WWHD_OFFSET(HdPadGrid, mOwner, 0xC4);
WWHD_OFFSET(HdPadGrid, mOriginX, 0xCC);
WWHD_OFFSET(HdPadGrid, mOriginY, 0xD8);
WWHD_OFFSET(HdPadGrid, mCellSize, 0xDC);
WWHD_OFFSET(HdPadGrid, mBaseResource, 0xE4);
WWHD_OFFSET(HdPadGrid, mResources, 0xE8);
WWHD_OFFSET(HdPadGrid, mDiscoveredCount, 0x130);
WWHD_OFFSET(HdPadGrid, mPreviousCount, 0x134);
WWHD_OFFSET(HdPadGrid, mChanged, 0x13C);
WWHD_OFFSET(HdPadGrid, mOffsetMode, 0x144);
WWHD_OFFSET(HdPadGrid, mOffset, 0x148);
WWHD_OFFSET(HdPadMenuMap, mVTable, 0x4);
WWHD_OFFSET(HdPadMenuMap, mState, 0x18);
WWHD_OFFSET(HdPadMenuMap, mAnimation, 0x44);
WWHD_OFFSET(HdPadMenuMap, mSoundFrame, 0xC4);
WWHD_OFFSET(HdPadMenuMap, mMap, 0x11C);
WWHD_OFFSET(HdPadMenuMap, mIconFrame, 0x124);
WWHD_OFFSET(HdPadMapOwner, mCells, 0x14C);
WWHD_OFFSET(HdPadMapOwner, mIcons, 0x150);
WWHD_OFFSET(HdPadSave, mMapData, 0xE4);
struct HdPadUiPointer {
    gptr<HdPadUiOwner> mOwner;
};
struct HdPadSavePointer {
    gptr<HdPadSave> mSave;
};
struct HdPadEventPointer {
    gptr<HdPadEventSource> mInput;
};
struct HdPadStickPointer {
    gptr<HdPadStick> mStick;
};
struct HdPadActivePointer {
    gptr<HdPadInputActive> mInput;
};
struct HdPadIdTable {
    be<u32> mIds[11];
};
struct HdPadBossPositions {
    HdPadPoint mPositions[1];
};
enum HdPadGridDimensions { HD_PAD_GRID_SIDE = 7, HD_PAD_GRID_CELLS = 49, HD_PAD_GRID_CENTER = 3 };
enum HdPadVirtualSlot {
    HD_PAD_UPDATE = 0x8C / 4,
    HD_PAD_DRAW = 0xB4 / 4,
    HD_PAD_ZOOM = 0x22C / 4,
    HD_PAD_CELL = 0x1C4 / 4
};
void hdPadPaneOrigin(HdPadPoint3 *, HdPadPane *);
u32 hdPadDisplayWidth();
u32 hdPadDisplayHeight();
void hdPadDungeonUpdateLayout(HdPadDungeon *);
void hdPadDungeonUpdateIcons(HdPadDungeon *);
void hdPadDungeonUpdateCursor(HdPadDungeon *);
void hdPadDungeonUpdatePane(HdPadDungeon *);
u32 hdPadUiBusy(HdPadUi *);
void hdPadDungeonUpdateBoss(HdPadDungeon *);
void hdPadDungeonUpdatePlayer(HdPadDungeon *);
void hdPadChangeState(HdPadStateController *, HdPadState *);
void hdPadCopyEvent(HdPadEvent *, HdPadEvent *);
Pair32 hdPadInputPosition(HdPadEventSource *);
void hdPadMapDrag(HdPadMap *, HdPadPoint *);
HdPadInputMode *hdPadGetInputMode(HdPadEventSource *);
void hdPadRotatePoint(HdPadPoint *, HdPadPoint *, HdPadPoint *, f32);
void hdPadStateExecute(HdPadStateController *);
f32 hdPadZoomRatio(HdPadMap *, u32, f32, f32);
void hdPadZoomPosition(HdPadMap *, HdPadPoint *, HdPadPoint *, u32);
void hdPadZoomScale(HdPadMap *, u32, f32, f32, f32);
u32 hdPadMapMode(void *);
void hdPadGridSetup(HdPadGrid *);
void hdPadGridSetupPanes(HdPadGrid *);
Pair32 hdPadPaneScale(HdPadPane *);
u32 hdPadMapDiscovered(void *, u32);
void hdPadIconsOpening(void *);
void hdPadIconsWaitInit(void *);
void hdPadIconsWaiting(void *);
void hdPadIconsClosing(void *);
void hdPadIconsFinish(void *);
void hdPadUiSound(void *);
void hdPadPlaySound(u32);
u32 hdPadAnimationEnded(void *, u32);
u32 hdPadIconsEnded(void *);
u32 hdPadCellEnded(HdPadMapCell *);
void hdPadMapBeginDraw(HdPadMapOwner *);
void hdPadMapEndDraw(HdPadMapOwner *);
HdPadMapCell *hdGridFind(HdPadCellArray *, HdPadCoordinate *);
u32 hdGridLookup(HdPadGrid *, u32);
f32 hdMapDisplayChase(HdPadPoint *, HdPadPoint *, f32, f32, f32);
enum HdPadEventKind { HD_PAD_EVENT_DRAG = 0x44 };
enum HdPadMenuFrame {
    HD_PAD_MENU_OPEN_BEGIN = 0x25,
    HD_PAD_MENU_WAIT_BEGIN = 0x2C,
    HD_PAD_MENU_CLOSE_BEGIN = 0x79,
    HD_PAD_MENU_FINISH_BEGIN = 0x80,
    HD_PAD_MENU_TRANSITION_FRAMES = 7
};
