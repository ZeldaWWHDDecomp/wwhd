/* HD directional arrows while crawling. */
#pragma once
#include "d/hd_parts.h"
struct hdCrawlingPartDesc_c { u8 _00[8]; hdUiSafeString mResource; u8 _10[4]; hdUiSafeString mClassName; };
struct hdCrawlingGameInfo_c { u8 _00[0x5bcb]; be<u8> mDirections; };
WWHD_OFFSET(hdCrawlingGameInfo_c,mDirections,0x5bcb);
struct hdCrawlingDirection_c : hdParts_c {
    be<u8> mShown; u8 _51[3];
    static hdCrawlingDirection_c* construct(hdCrawlingDirection_c* object);
    static void destroy(hdCrawlingDirection_c* object,u32 flags);
    void waitEnter(); void waitExecute(); void updateArrows(); void displayEnter(); void displayExecute();
    void hideArrows(); void endEnter(); u32 allArrowsClosed(); void endExecute();
    u32 setup(u32 first,u32 second,void* desc); void release(); void execute();
    u32 createArrow(hdCrawlingPartDesc_c* desc);
};
WWHD_SIZE(hdCrawlingDirection_c,0x54);
