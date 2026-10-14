/** HD GamePad information panel. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_input_ctrl.h"
struct hdInformationDRC_c : hdParts_c {
    void inAllEnter();
    void inAllExecute();
    void outAllEnter();
    void outAllExecute();
    u32 setup(u32 archive, u32 resource, u32 flags);
    void release();
    void stateExecute();
    u32 loadScreen(void* name, u32 archive, u32 resource, u32 flags, void* descriptor);
};
WWHD_SIZE(hdInformationDRC_c, 0x50);
void informationDestroyBase(hdInformationDRC_c* screen, u32 flags);
hdInformationDRC_c* informationConstructBase(hdInformationDRC_c* screen);
void informationAttachArchives(hdInformationDRC_c* screen, hdUiSafeString* names, u32 count);
void informationDetachArchives(hdInformationDRC_c* screen);
void informationExecuteState(hdStateMachine* machine);
hdPartsLayout_c* informationAllocateLayout(u32 size, void* heap, u32 alignment);
void informationBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane);
