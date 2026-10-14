/** HD pictograph save-slot post tags; no GameCube counterpart. Private source. */
#pragma once
#include "d/hd_screen_btn_pictograph.h"
#include "d/hd_picture_accessor.h"
struct hdPictographPost_c : hdBasicButton_c {
    gptr<hdPartsPaneSRT_c> mpButtonPanes[2];
    gptr<hdTextPane_c> mpTextPanes[5];
    gptr<hdPartsPaneSRT_c> mpMarkPanes[2];
    u8 _reservedB0[0xC];
    gptr<hdPictureSlotSummaries_c> mpSummaries;
    be<u8> mSelectedSlot;
    u8 _reservedC1[3];
    static void destroy(hdPictographPost_c*, s32);
    static hdPictographPost_c* construct(hdPictographPost_c*, hdPictureSlotSummaries_c*, u32);
    void initializeButtonPanes();
    void initializeTextPanes();
    void initializeMarkPanes();
    void refreshSummary();
    void refreshPlace();
    u32 initialize(u32, u32, hdPartsDesc_c*);
    void showPlayerName();
    void hidePlayerName();
    u32 load(void*, u32, u32, u32, void*);
};
WWHD_OFFSET(hdPictographPost_c, mpButtonPanes, 0x8C);
WWHD_OFFSET(hdPictographPost_c, mpTextPanes, 0x94);
WWHD_OFFSET(hdPictographPost_c, mpMarkPanes, 0xA8);
WWHD_OFFSET(hdPictographPost_c, mpSummaries, 0xBC);
WWHD_OFFSET(hdPictographPost_c, mSelectedSlot, 0xC0);
WWHD_SIZE(hdPictographPost_c, 0xC4);

struct hdPostCalendar_c { be<u32> mSecond, mMinute, mHour, mDay, mMonth, mYear; be<u32> mOther[5]; };
WWHD_SIZE(hdPostCalendar_c, 0x2C);
void hdPostTicksToCalendar(u32, u32, hdPostCalendar_c*);
void hdPostResetWork(PaneWork_l*, hdUiSafeString*, hdUiSafeString*);
void hdPostConstructWork(PaneWork_l*);
void hdPostLookupMessage(void*, PaneWork_l*, void*, PaneString_l*);
void* hdPostLookupEntry(void*, u32);
void hdPostAssureWideString(PaneString_l*);
void hdPostCopyBytes(void*, const void*, u32, u32);
void hdPostBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
