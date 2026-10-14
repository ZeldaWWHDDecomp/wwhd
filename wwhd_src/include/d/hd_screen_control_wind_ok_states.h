/** HD wind confirmation states; companion of the merged wait implementation. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_button.h"
#include "d/hd_input_ctrl.h"
struct hdControlWindOK_c : hdParts_c {
    be<u8> mWaitTimer, mTouchEnabled, mTouchTriggered, mCancelTriggered, mReady, mForced, mHighlight;
    u8 _57;
    u32 createChild(hdPartsDesc_c* descriptor);
    void resetInput();
    void resetEnter();
    u32 testTouch();
    u32 testCancel();
    void waitExecute();
    void displayEnter();
    void updateButton();
    void displayExecute();
    void outWaitEnter();
    void outAllEnter();
    void hide();
    u32 setup(u32 archive, u32 resource, u32 flags);
    void release();
    void stateExecute();
    void enableTouch();
};
WWHD_SIZE(hdControlWindOK_c, 0x58);
WWHD_OFFSET(hdControlWindOK_c, mWaitTimer, 0x50);
WWHD_OFFSET(hdControlWindOK_c, mTouchEnabled, 0x51);
WWHD_OFFSET(hdControlWindOK_c, mTouchTriggered, 0x52);
WWHD_OFFSET(hdControlWindOK_c, mCancelTriggered, 0x53);
WWHD_OFFSET(hdControlWindOK_c, mReady, 0x54);
WWHD_OFFSET(hdControlWindOK_c, mForced, 0x55);
WWHD_OFFSET(hdControlWindOK_c, mHighlight, 0x56);
void windOKDestroyBase(hdControlWindOK_c* panel, u32 flags);
hdControlWindOK_c* windOKConstructBase(hdControlWindOK_c* panel);
void windOKResetButton(hdParts_c* button);
void windOKClearButton(hdParts_c* button);
void windOKHighlightButton(hdParts_c* button);
void windOKFinishButton(hdParts_c* button);
void windOKSelectReceiver(hdButtonEventManager_l* manager, void* receiver);
void windOKCopyEvent(hdButtonEvent_l* destination, hdButtonEvent_l* source);
u32 windOKTouchEvent(hdButtonEvent_l* event);
u32 windOKCancelEvent(hdButtonEvent_l* event);
void windOKPlaySound(const char* name);
u32 windOKInputBusy(hdInputManager* manager);
u32 windOKTestTouch(hdControlWindOK_c* panel);
u32 windOKTestCancel(hdControlWindOK_c* panel);
void windOKUpdateButton(hdControlWindOK_c* panel);
void windOKResetInput(hdControlWindOK_c* panel);
void windOKAttachArchives(hdControlWindOK_c* panel, hdUiSafeString* names, u32 count);
void windOKDetachArchives(hdControlWindOK_c* panel);
void windOKExecuteState(hdStateMachine* machine);
struct hdWindOKGameState_l { u8 _00[0x5BD0]; be<u8> mCancel; };
WWHD_OFFSET(hdWindOKGameState_l, mCancel, 0x5BD0);
hdWindOKGameState_l* windOKGameState();

struct hdBtnControlWind_c;
void* windOKAllocate(u32 size, void* heap, u32 alignment);
hdBtnControlWind_c* windOKConstructButton(hdBtnControlWind_c* button);
u32 windOKSetupButton(hdBtnControlWind_c* button, void* archive, void* resource, void* descriptor);
u32 windOKCreateBase(hdControlWindOK_c* panel, u32 index, hdPartsDesc_c* descriptor);
