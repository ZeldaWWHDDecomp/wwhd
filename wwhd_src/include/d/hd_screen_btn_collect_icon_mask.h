/** HD collection equipment-mask button; no GameCube counterpart. */
#pragma once
#include "d/hd_screen_btn_collect_icon.h"
struct hdBtnCollectIconMask_c : NoDecideButton_l {
    /* 0x78 */ hdCollectPaneArray_c mIconPanes;
    /* 0x80 */ be<u8> mIconVisible;
    /* 0x81 */ be<u8> mEquipped;
    u8 mPadding[2];
    /* 0x84 */ be<u32> mAnimation;
    u32 init(u32, u32, hdPartsDesc_c*);
    u32 loadLayout(void*, u32, u32, u32, void*);
    void setActive(u32);
    u32 isEquipped();
    void setEquipped(u32);
    void finalize();
    void touchEnter();
    void cursorEnter();
    void equipmentAnimationEnter();
    void equipOffEnter();
    void equipOffMove();
    void equipOnEnter();
    void equipOnMove();
    void toggleEquipment();
    void setIconVisible(u32);
    void release();
    void updateEquipment();
    void decideMove();
    void offMove();
    void selectMove();
    void touchMove();
    void cursorMove();
    void equipmentAnimationMove();
    hdCollectButtonVtable_c* eventTable() { return reinterpret_cast<hdCollectButtonVtable_c*>(mVtable.get()); }
    u32 acceptsSelect(hdUiEvent* event) { return gabi::call_ptr<u32>(eventTable()->mAcceptSelect.get(), this, u32(event->mKind)); }
    u32 acceptsDispatch(hdUiEvent* event) { return gabi::call_ptr<u32>(eventTable()->mAcceptDispatch.get(), this, u32(event->mKind)); }
    u32 acceptsDecide(hdUiEvent* event) { return gabi::call_ptr<u32>(eventTable()->mAcceptDecide.get(), this, u32(event->mKind)); }
    u32 acceptsPoint(hdUiEvent* event) { return gabi::call_ptr<u32>(eventTable()->mAcceptPoint.get(), this, event); }
    void onDecide() { gabi::call_ptr<void>(eventTable()->mOnDecide.get(), this); }
};
WWHD_SIZE(hdBtnCollectIconMask_c, 0x88);
WWHD_OFFSET(hdBtnCollectIconMask_c, mIconPanes, 0x78);
WWHD_OFFSET(hdBtnCollectIconMask_c, mIconVisible, 0x80);
WWHD_OFFSET(hdBtnCollectIconMask_c, mEquipped, 0x81);
WWHD_OFFSET(hdBtnCollectIconMask_c, mAnimation, 0x84);
