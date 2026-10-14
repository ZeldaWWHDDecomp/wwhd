/* HD menu map pane part and coordinate conversion. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_gamepad_map.h"
#include "d/hd_menu_pane_control.h"

struct hdMenuMapPanePart : hdParts_c {
    hdMenuPaneControl mControl;
    gptr<HdPadMap> mpChart;
    gptr<void> mpMap;
    u8 _mapTail[8];
    be<u32> mSuppressDungeonStageOffset;
    static hdMenuMapPanePart* construct(hdMenuMapPanePart* self);
    void destroy(u32 flags);
    void calc();
    void execute();
    void dispatch();
    void calculateOffset();
    void updateRotation();
    f32 getMapScale();
    void addDungeonStageOffset(const hdMenuPaneVector* input, hdMenuPaneVector* output);
    void transformMapPoint(const hdMenuPaneVector* input, hdMenuPaneVector* output);
    void getPanePosition(const hdMenuPaneVector* input, hdMenuPaneVector* output);
};
WWHD_SIZE(hdMenuMapPanePart, 0xA4);
WWHD_OFFSET(hdMenuMapPanePart, mControl, 0x50);
WWHD_OFFSET(hdMenuMapPanePart, mpChart, 0x90);
WWHD_OFFSET(hdMenuMapPanePart, mpMap, 0x94);
WWHD_OFFSET(hdMenuMapPanePart, mSuppressDungeonStageOffset, 0xA0);
