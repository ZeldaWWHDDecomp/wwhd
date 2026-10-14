/** HD collection screen: pearl and Triforce child routing. */
#pragma once
#include "d/hd_screen_btn_triforce.h"
#include "d/hd_parts_no_control.h"
struct hdCollection_c : NoControlPart_l {
    static void destroy(hdCollection_c*,s32);
    static hdCollection_c* construct(hdCollection_c*);
    void configureNeighbors();
    u32 load(void*,u32,u32);
    void synchronizeButtons();
    u32 findReceiver(u32);
    void updateObtained();
    u32 createCollectionChild(hdPartsDesc_c*);
};
WWHD_SIZE(hdCollection_c,0x50);
