/* HD WarpArea_00: layout animations and Loop state. No GameCube counterpart.
 * */
#pragma once
#include "d/hd_parts_no_control.h"

struct hdWarpArea_c : NoControlPart_l {
    static hdWarpArea_c* construct(hdWarpArea_c* area);
    static void destroy(hdWarpArea_c* area, u32 flags);
    void initializeAnimations();
    void calculateLayout();
    void showChange();
    void hideChange();
    u32 setup(u32 first, u32 second, u32 third);
    u32 loadLayout(void* name, u32 first, u32 second, u32 third, void* description);
};
WWHD_SIZE(hdWarpArea_c, 0x50);
struct hdWarpAreaStateKey_l { be<u32> mKey; };
WWHD_SIZE(hdWarpAreaStateKey_l, 4);
extern void hdWarpArea_constructBase(hdWarpArea_c* area);
extern void hdWarpArea_destroyBase(hdWarpArea_c* area, u32 flags);
extern void hdWarpArea_startAnimation(hdAnimSet* animation, u32 track, u32 flags, f32 frame);
extern void hdWarpArea_setAnimationFrame(hdAnimSet* animation, u32 track, u32 mode, f32 frame);

extern u32 hdWarpArea_setupBase(hdWarpArea_c* area, hdUiSafeString* name, u32 first, u32 second, u32 third);
extern hdPartsLayout_c* hdWarpArea_allocateLayout(u32 size, hdScreenHeap* heap, u32 alignment);
extern hdPartsLayout_c* hdWarpArea_constructLayout(hdPartsLayout_c* layout);
extern void hdWarpArea_bindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane);

struct hdWarpAreaUiSystemPointer_l { gptr<hdScreenUiSystem> mpSystem; };
WWHD_SIZE(hdWarpAreaUiSystemPointer_l, 4);
