/** HD collection-menu icon button. */
#pragma once
#include "d/hd_parts_button.h"
#include "d/hd_parts_lyt.h"
#include "d/hd_no_decide_button.h"
#include "d/d_save_readable.h"
#include "d/hd_input_eventmgr.h"
#include "hd/hd_ui_02001084.h"
#include "hd/hd_ui_020015C8.h"

// The merged NoDecideButton owns the phase word; button routing uses its canonical prefix.
inline hdButtonPart_c* collectButtonView(NoDecideButton_l* button) {
    return reinterpret_cast<hdButtonPart_c*>(button);
}

enum hdCollectKind_e {
    COLLECT_KIND_SWORD,
    COLLECT_KIND_SWORD_VARIANT,
    COLLECT_KIND_SHIELD,
    COLLECT_KIND_PIRATES_CHARM,
    COLLECT_KIND_SAIL,
    COLLECT_KIND_WIND_WAKER,
    COLLECT_KIND_TINGLE
};
enum hdCollectAnimation_e { COLLECT_ANIMATION_ICON_CHANGE = 5 };
enum hdCollectMaskAnimation_e {
    COLLECT_MASK_ANIMATION_EQUIP_OFF = 5,
    COLLECT_MASK_ANIMATION_EQUIP_ON = 6
};
enum hdCollectIcon_e {
    COLLECT_ICON_PIRATES_CHARM = 8,
    COLLECT_ICON_TINGLE = 9,
    COLLECT_ICON_SAIL = 6,
    COLLECT_ICON_WIND_WAKER = 7,
    COLLECT_ICON_SWIFT_SAIL = 16,
    COLLECT_ICON_NONE = 17
};
struct hdCollectIconRecord_c { be<s32> mItem, mDescription; };
WWHD_SIZE(hdCollectIconRecord_c, 8);
WWHD_OFFSET(hdCollectIconRecord_c, mItem, 0);
WWHD_OFFSET(hdCollectIconRecord_c, mDescription, 4);
struct hdCollectIconTable_c {
    hdCollectIconRecord_c mRecords[COLLECT_ICON_NONE];
    hdCollectIconRecord_c* record(u32 index) {
        return gabi::at<hdCollectIconRecord_c>(gabi::ea(this) + index * sizeof(hdCollectIconRecord_c));
    }
};
WWHD_SIZE(hdCollectIconTable_c, COLLECT_ICON_NONE * 8);
struct hdCollectPane_c;
// Only the collection-specific slots extend the merged parts vtable.
struct hdCollectButtonVtable_c : hdPartsVtbl_c {
    u8 mBeforeEvents[0xDC - 0xC8];
    gabi::gfn mAcceptDispatch;
    u8 mPadDispatch[4];
    gabi::gfn mAcceptDecide;
    u8 mPadDecide[4];
    gabi::gfn mAcceptSelect;
    u8 mPadSelect[4];
    gabi::gfn mAcceptPoint;
    u8 mBeforeAction[0x10C - 0xF8];
    gabi::gfn mOnDecide;
    u8 mMiddle[0x1F4 - 0x110];
    gabi::gfn mRefreshVisibility;
};
WWHD_SIZE(hdPartsVtbl_c, 0xC8);
WWHD_OFFSET(hdCollectButtonVtable_c, mCalcLayout, 0x8C);
WWHD_OFFSET(hdPartsPaneSRT_c, mFlags, 0x44);
WWHD_OFFSET(hdCollectButtonVtable_c, mRefreshVisibility, 0x1F4);
WWHD_OFFSET(hdCollectButtonVtable_c, mAcceptDispatch, 0xDC);
WWHD_OFFSET(hdCollectButtonVtable_c, mAcceptDecide, 0xE4);
WWHD_OFFSET(hdCollectButtonVtable_c, mAcceptSelect, 0xEC);
WWHD_OFFSET(hdCollectButtonVtable_c, mAcceptPoint, 0xF4);
WWHD_OFFSET(hdCollectButtonVtable_c, mOnDecide, 0x10C);
WWHD_SIZE(hdCollectButtonVtable_c, 0x1F8);
struct hdCollectPaneArray_c {
    be<s32> mCount;
    gptr<gptr<hdCollectPane_c>> mpPanes;
};
WWHD_SIZE(hdCollectPaneArray_c, 8);
WWHD_OFFSET(hdCollectPaneArray_c, mCount, 0);
WWHD_OFFSET(hdCollectPaneArray_c, mpPanes, 4);
struct hdBtnCollectIcon_c : NoDecideButton_l {
    /* 0x78 */ hdCollectPaneArray_c mIconPanes;
    /* 0x80 */ be<u32> mIcon;
    /* 0x84 */ be<u32> mCollectionKind;
    /* 0x88 */ be<u8> mIconVisible;
    u8 mPadding[3];
    u32 init(u32, u32, hdPartsDesc_c*);
    u32 loadLayout(void*, u32, u32, u32, void*);
    void setActive(u32);
    void setIcon(u32);
    void setSwordIcon(u32);
    void iconChangeEnter();
    void setIconVisible(u32);
    void release();
    void updateIcon();
    u32 swordIcon();
    u32 swordVariantIcon();
    u32 shieldIcon();
    u32 piratesCharmIcon();
    u32 sailIcon();
    u32 windWakerIcon();
    u32 tingleIcon();
    s32 itemNumber();
    s32 descriptionNumber();
};
WWHD_SIZE(hdBtnCollectIcon_c, 0x8C);
WWHD_OFFSET(hdBtnCollectIcon_c, mIconPanes, 0x78);
WWHD_OFFSET(hdBtnCollectIcon_c, mIcon, 0x80);
WWHD_OFFSET(hdBtnCollectIcon_c, mCollectionKind, 0x84);
WWHD_OFFSET(hdBtnCollectIcon_c, mIconVisible, 0x88);
struct hdCollectHeapManagerPointer_c { gptr<void> mpManager; };
WWHD_SIZE(hdCollectHeapManagerPointer_c, 4);
struct hdCollectEventManagerPointer_c { gptr<cking::ui::input::EventMgr> mpManager; };
WWHD_SIZE(hdCollectEventManagerPointer_c, 4);
inline hdUiEvent* collectCurrentEvent(cking::ui::input::EventMgr* manager) {
    return hdPartsView<hdUiEvent>(gabi::ea(manager) + offsetof(cking::ui::input::EventMgr, mEventQueue) + 0x1C);
}

struct hdCollectUiSystemPointer_c { gptr<hdScreenUiSystem> mpSystem; };
WWHD_SIZE(hdCollectUiSystemPointer_c, 4);

struct hdCollectStaticAngles_c { be<f32> mLow, mHigh; u8 mFirst, mSecond; };
WWHD_SIZE(hdCollectStaticAngles_c, 10);
WWHD_OFFSET(hdCollectStaticAngles_c, mFirst, 8);
WWHD_OFFSET(hdCollectStaticAngles_c, mSecond, 9);
struct hdCollectZeroVector_c { be<u32> mWords[4]; };
WWHD_SIZE(hdCollectZeroVector_c, 16);
struct hdCollectGlobalRegistration_c { u8 mRecord[12]; };
WWHD_SIZE(hdCollectGlobalRegistration_c, 12);

template<class T, u32 N> struct hdCollectRuntimeArray_c {
    T mEntries[N];
    T& operator[](u32 index) { return *hdPartsView<T>(gabi::ea(this) + index * sizeof(T)); }
};
using hdCollectCollectAnimationIndicesArray_c = hdCollectRuntimeArray_c<be<u32>, 1>;
using hdCollectCollectGroupsArray_c = hdCollectRuntimeArray_c<hdUiSafeString, 1>;
using hdCollectCollectRegistrationsArray_c = hdCollectRuntimeArray_c<hdCollectGlobalRegistration_c, 3>;
using hdCollectCollectAngleConstantsArray_c = hdCollectRuntimeArray_c<be<f32>, 2>;
using hdCollectCollectMaskAnimationIndicesArray_c = hdCollectRuntimeArray_c<be<u32>, 2>;
using hdCollectCollectMaskGroupsArray_c = hdCollectRuntimeArray_c<hdUiSafeString, 2>;
using hdCollectCollectMaskRegistrationsArray_c = hdCollectRuntimeArray_c<hdCollectGlobalRegistration_c, 3>;
using hdCollectCollectMaskAngleConstantsArray_c = hdCollectRuntimeArray_c<be<f32>, 2>;
using hdCollectCollectMaskParentsArray_c = hdCollectRuntimeArray_c<hdPartsStateId_c, 6>;

using hdCollectWord = be<u32>;
using hdCollectSignedWord = be<s32>;
using hdCollectByte = be<u8>;

inline dSv_player_c& collectSavePlayer() {
    // Preserve guest NULL plus the canonical player offset, as the original lwz/addi sequence does.
    d_save_cpp::SaveGlobal* save = hdPartsView<d_save_cpp::SaveGlobal>(gabi::ea(d_save_cpp::saveBase()));
    return save->mInfo.mSavedata.mPlayer;
}

/* 0262A088 (hd_screen_btn_collect_icon.cpp): the collection button's constructor */
hdBtnCollectIcon_c* collectIconConstruct(hdBtnCollectIcon_c* button);
