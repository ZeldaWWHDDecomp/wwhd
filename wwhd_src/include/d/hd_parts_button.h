/** HD button-part/event routing views; canonical parts base and narrow button extensions. */
#pragma once
#include "d/hd_parts_heart.h"
enum hdButtonEventType_e {
    HD_BUTTON_EVENT_DECIDE_e = 2,
    HD_BUTTON_EVENT_CANCEL_e = 4,
    HD_BUTTON_EVENT_MENU_e = 3,
    HD_BUTTON_EVENT_TOUCH_e = 13,
    HD_BUTTON_EVENT_TOUCH_END_e = 14,
    HD_BUTTON_EVENT_SPECIAL_e = 17
};
struct hdButtonEvent_l {
    be<u32> mType, mReceiverId;
    be<u8> mButton;
    u8 mPayload[0x1F];
};
struct hdButtonReceiverVtable_l {
    u8 mPrefix[0xC];
    gabi::gfn mDestroy;
};
struct hdButtonReceiver_l {
    u8 mPrefix[0x10];
    be<u32> mId;
    gptr<hdButtonReceiverVtable_l> mVtable;
    u8 mStorage[0x58 - 0x18];
    be<u8> mEnabled;   /* 0x58: the receiver takes input (DRC-item_screen) */
    u8 mTail[0x60 - 0x59];
};
WWHD_OFFSET(hdButtonReceiver_l, mEnabled, 0x58);
struct hdButtonEventLink_l {
    gptr<hdButtonEventLink_l> mpPrevious, mpNext;
    be<u32> mCount, mLinked;
};
struct hdButtonEventManager_l {
    hdButtonEventLink_l mEvents;
    u8 mPrefix[4];
    u8 mQueue[0x1C];
    hdButtonEvent_l mCurrentEvent;
    u8 mStorage[0xD0 - 0x58];
    be<u32> mCurrentReceiver;
};
struct hdButtonPartVtable_l {
    u8 mPrefix[0xCC];
    gabi::gfn mOnFocusChanged;
    u8 mGap[4];
    gabi::gfn mAcceptFocus;
    u8 mStorage[0x114 - 0xD8];
    gabi::gfn mOnTouchRelease;
};
struct hdButtonPart_c : hdParts_c {
    gptr<hdButtonReceiver_l> mpReceiver;
    be<u32> mAcceptedButtons;
    be<u8> mTouchEnabled, mReserved, mEnablePane, mEnableText;
    hdUiSafeString mTouchSound, mCursorSound;
    be<u32> mTouchSoundId, mCursorSoundId;
    static void destroy(hdButtonPart_c*, u32);
    u32 getReceiverId();
    void registerReceiver();
    u32 setReceiverButton(u32, u32);
    void setReceiverButtons(u32, u32, u32, u32, u32, u32, u32, u32);
    void setPaneEnabled(u32);
    void setTextEnabled(u32);
    void applyPaneFlags();
    void setText(hdUiSafeString*, void*);
    void setTextStyle(hdUiSafeString*, u32);
    BOOL createReceiver(u32, u32);
    void destroyReceiver();
    u32 dispatchButton(u32);
    BOOL requestFocus(u32);
    void clearFocus();
    void synchronizeFocus();
    BOOL hasAcceptedButton(u32);
    BOOL isTouchEvent(hdButtonEvent_l*);
    BOOL isTouchEndEvent(hdButtonEvent_l*);
    void handleTouchRelease();
    void setTouchSound(hdUiSafeString*, u32);
    void setCursorSound(hdUiSafeString*, u32);
    void playTouchSound();
    void playCursorSound();
    BOOL isDecideEvent(u32);
    BOOL isCancelEvent(u32);
};
struct hdButtonInputMode_l {
    u8 mPrefix[0x14];
    be<u8> mEnabled;
};
struct hdButtonMenu_l {
    u8 mPrefix[0x20];
    be<u32> mFlags;
};
WWHD_SIZE(hdButtonPart_c, 0x74);
WWHD_SIZE(hdButtonEvent_l, 0x28);
WWHD_OFFSET(hdButtonPart_c, mpReceiver, 0x50);
WWHD_OFFSET(hdButtonPart_c, mAcceptedButtons, 0x54);
WWHD_OFFSET(hdButtonPart_c, mTouchSound, 0x5C);
WWHD_OFFSET(hdButtonPart_c, mCursorSoundId, 0x70);
WWHD_SIZE(hdButtonReceiver_l, 0x60);

struct hdButtonConfigDeleteContext_l {
    gptr<void> mpOwner;
};
struct hdButtonConfigTreeNode_l {
    u8 mPrefix[0xC];
    hdUiSafeString mKey;
    be<u32> mValueVtable;
    gptr<void> mpConfig;
};
struct hdButtonPaneVtable_l {
    u8 mPrefix[0x14];
    gabi::gfn mGetType;
    u8 mStorage[0x5C - 0x18];
    gabi::gfn mFindPane;
};
// Button-specific pane methods and flags extend the canonical parts pane.
struct hdButtonPane_l : hdPartsPartsPane_c {};
// The layout screen's two button resource words are not named by the canonical base.
struct hdButtonLayoutResources_l {
    u8 mPrefix[0xD8];
    be<u32> mResourceCount;
    gptr<gptr<hdButtonPane_l>> mpResources;
};
struct hdButtonLayoutName_l {
    u8 mPrefix[0x1C];
    be<u32> mResourceName;
};
WWHD_OFFSET(hdButtonLayoutResources_l, mResourceCount, 0xD8);
WWHD_OFFSET(hdButtonLayoutResources_l, mpResources, 0xDC);
WWHD_OFFSET(hdButtonLayoutName_l, mResourceName, 0x1C);
inline hdButtonLayoutResources_l* hdButtonResources(hdPartsLayout_c* screen) {
    return hdPartsView<hdButtonLayoutResources_l>(gabi::ea(screen));
}
inline u32 hdButtonResourceName(hdPartsLyt_c* layout) {
    return hdPartsView<hdButtonLayoutName_l>(gabi::ea(layout))->mResourceName;
}
inline hdButtonPaneVtable_l* hdButtonPaneVtable(hdButtonPane_l* pane) {
    return reinterpret_cast<hdButtonPaneVtable_l*>(pane->mPane.mVtable.get());
}
inline hdButtonPartVtable_l* hdButtonVtable(hdButtonPart_c* part) {
    return reinterpret_cast<hdButtonPartVtable_l*>(part->mVtable.get());
}

// Message workspace forwarded by 026F6048 to hdTextPane_c::prepareMessage.
struct PaneWork_l;
inline void hdButtonPart_setTextStyle(hdButtonPart_c* button, hdUiSafeString* label,
                                     PaneWork_l* work) {
    button->setTextStyle(label, gabi::ea(work));
}
