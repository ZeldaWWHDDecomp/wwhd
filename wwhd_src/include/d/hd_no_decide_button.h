#pragma once
#include "d/hd_message_parts.h"
#include "d/hd_parts_button.h"
struct NoDecideButton_l : hdParts_c {
 be<u32> mControlMode; u8 mReserved54[4]; be<u8> mEnabled; u8 mReserved59[0x1B]; be<u32> mAnimationState;
    void destroy(u32 flags);
    static NoDecideButton_l* construct(NoDecideButton_l* part);
    void enterOff();
    void enterOnTouch();
    void enterDisabled();
    void enterHide();
    void updateOnTouch();
    void updateDisabled();
    void updateHide();
    void reset();
    void update();
    void setOnTouch();
    void setOnCursor(u32 immediate);
    void hide();
    u32 createLayout(void* archive,void* resource,u32 mode,u32 flags,u32 user);
    void updateOffInput();
    void updateOnTouchInput();
    void updateOnCursorInput();
    void enterCursorAnimation();
    void updateReadyInput();
    void updateIdle();
    void enableWhileActive();
    void updateCursorIndicator();
    void updateOffAnimation();
    void updateCursorAnimation();
    u32 create(void*,void*,u32,u32);
};
WWHD_OFFSET(NoDecideButton_l, mpLayout, 0x44);
WWHD_OFFSET(NoDecideButton_l, mControlMode, 0x50);
WWHD_OFFSET(NoDecideButton_l, mEnabled, 0x58);
WWHD_OFFSET(NoDecideButton_l, mAnimationState, 0x74);
WWHD_SIZE(NoDecideButton_l, 0x78);

