/* HD crawling-direction arrow: a Loop part with its own layout resources. */
#pragma once
#include "d/hd_parts_in_out_loop.h"
struct hdCrawlingDirectionArrow_c : hdPartsInOutLoop_c {
    static hdCrawlingDirectionArrow_c* construct(hdCrawlingDirectionArrow_c* object);
    static void destroy(hdCrawlingDirectionArrow_c* object, u32 flags);
    u32 setup(u32 first, u32 second, u32 third);
};
WWHD_SIZE(hdCrawlingDirectionArrow_c, 0x5C);
