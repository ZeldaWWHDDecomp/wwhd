/** HD squid chart overlay. A sea-chart marker screen (hdSeaChartLinkPos_c) with its own layout and pane names. */
#pragma once
#include "d/hd_chart_marker.h"
struct hdSquidChart_c : hdSeaChartLinkPos_c {
    static hdSquidChart_c* construct(hdSquidChart_c* part);
    static void destroy(hdSquidChart_c* chart, u32 flags);
    void initialize();
    u32 create(u32 first,u32 second,u32 third);
    void destroyLayout();
    void executeState();
    u32 createChild(hdPartsDesc_c* descriptor);
    u32 createLayout(void* name,u32 resource,u32 first,u32 second,void* descriptor);
    static void initializeStatics();
    u32 visitorWord();
};
WWHD_SIZE(hdSquidChart_c,0x3BC);
