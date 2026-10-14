/** HD GamePad battery display; game-derived */
#pragma once
#include "d/hd_parts_no_control.h"
struct hdBattery_c : NoControlPart_l {
    /* 0x50 */ be<u8> mDisplayRequested;
    u8 _51[3];
    void hide();
    void waitExecute();
    void displayEnter();
    void displayExecute();
    u32 setup(u32 archive, u32 resource, u32 flags);
    void drawIfRequested();
    void toWait();
    u32 loadScreen(void* archive, u32 resource, u32 flags, u32 mode, void* heap);
};
WWHD_OFFSET(hdBattery_c, mDisplayRequested, 0x50);
WWHD_SIZE(hdBattery_c, 0x54);

void batteryDestroyBase(hdBattery_c* screen, u32 flags);
u32 batterySetupBase(hdBattery_c* screen, hdUiSafeString* name, u32 archive, u32 resource, u32 flags);
void batteryDrawBase(hdBattery_c* screen);
void batteryStartAnimation(hdAnimSet* animation, u32 track, u32 flags, f32 frame);
hdPartsLayout_c* batteryAllocateLayout(u32 size, void* heap, u32 alignment);
void batteryBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane);
