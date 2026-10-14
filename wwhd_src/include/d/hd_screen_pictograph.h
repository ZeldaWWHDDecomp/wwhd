/* HD Picto Box layout parts. No GameCube screen counterpart. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_lyt.h"

struct hdPictoStateKey {
    be<u32> mNumber;
    u32 number();
};
struct hdPictographBackdrop : hdParts_c {
    static hdPictographBackdrop* construct(hdPictographBackdrop*);
    static void destroy(hdPictographBackdrop*, u32);
    u32 loadBackdrop(u32, u32, hdPartsDesc_c*);
    void setTexture(void*);
    void setAlpha(f32);
    u32 loadLayout(hdUiSafeString*, u32, u32, u32, void*);
};
WWHD_SIZE(hdPictographBackdrop, 0x50);

struct hdPictoStaticStorage {
    be<f32> mMinimum, mMaximum;
    be<u8> mFirstFlag, mSecondFlag;
    u8 mReservedA[2];
    be<u32> mWords[4];
};
struct hdPictoRegistration { be<u32> mWords[3]; };
WWHD_OFFSET(hdPictoStaticStorage, mWords, 0xC);
WWHD_SIZE(hdPictoStaticStorage, 0x1C);
