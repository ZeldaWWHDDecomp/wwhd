/** HD GamePad noOperate panel. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_input_ctrl.h"
struct hdNoOperateDRC_c : hdParts_c {
    /* 0x50 */ be<f32> mPointerOffsetY;
    /* 0x54 */ be<u8> mDisabled;
    u8 _55[3];
    void waitExecute();
    void displayExecute();
    void updatePosition();
    void inAllEnter();
    void inAllExecute();
    void outAllEnter();
    void outAllExecute();
    u32 setup(u32 archive, u32 resource, u32 flags);
    void release();
    void stateExecute();
    u32 loadScreen(void* name, u32 archive, u32 resource, u32 flags, void* descriptor);
};
WWHD_SIZE(hdNoOperateDRC_c, 0x58);
void noOperateDestroyBase(hdNoOperateDRC_c* screen, u32 flags);
hdNoOperateDRC_c* noOperateConstructBase(hdNoOperateDRC_c* screen);
void noOperateAttachArchives(hdNoOperateDRC_c* screen, hdUiSafeString* names, u32 count);
void noOperateDetachArchives(hdNoOperateDRC_c* screen);
void noOperateExecuteState(hdStateMachine* machine);
hdPartsLayout_c* noOperateAllocateLayout(u32 size, void* heap, u32 alignment);
void noOperateBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane);

WWHD_OFFSET(hdNoOperateDRC_c, mPointerOffsetY, 0x50);
WWHD_OFFSET(hdNoOperateDRC_c, mDisabled, 0x54);
void noOperateUpdatePosition(hdNoOperateDRC_c* screen);
hdController* noOperateControllerByOrder(void* manager, u32 controller, u32 index);
u32 noOperateHasPad(hdInputManager* manager);
u32 noOperateCanUpdate();
