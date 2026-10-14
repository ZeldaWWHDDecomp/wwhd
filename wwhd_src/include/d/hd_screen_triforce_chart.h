/** HD triforce chart overlay. A sea-chart marker screen (hdSeaChartLinkPos_c) with its own layout and pane names. */
#pragma once
#include "d/hd_chart_marker.h"
struct hdTriforceChart_c : hdSeaChartLinkPos_c {
    be<u32> mPieceStatus[8];
    static hdTriforceChart_c* construct(hdTriforceChart_c* part);
    static void destroy(hdTriforceChart_c* chart, u32 flags);
    void initialize();
    u32 create(u32 first,u32 second,u32 third);
    void destroyLayout();
    void executeState();
    u32 createChild(hdPartsDesc_c* descriptor);
    u32 createLayout(void* name,u32 resource,u32 first,u32 second,void* descriptor);
    static void initializeStatics();
    void refreshPieces();
    u32 visitorWord();
};
WWHD_SIZE(hdTriforceChart_c,0x3DC);
WWHD_OFFSET(hdTriforceChart_c,mPieceStatus,0x3BC);
