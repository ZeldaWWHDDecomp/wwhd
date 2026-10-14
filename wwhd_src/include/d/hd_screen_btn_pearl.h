/** HD collection pearl buttons (derive from the no-decide button of the parts framework). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_no_decide_button.h"
struct hdCollectionPane_l : hdPartsPane_c {
    u8 mTransformStorage[0x28];
    be<u8> mFlags;
};
WWHD_OFFSET(hdCollectionPane_l, mFlags, 0x44);
struct hdPearlButton_c : NoDecideButton_l {
    hdScreenPaneArray mPearlPanes;
    be<u32> mPearlIndex;
    be<u8> mObtained;
    u8 mTailPadding[3];
    static void destroy(hdPearlButton_c*, s32);
    static hdPearlButton_c* construct(hdPearlButton_c*);
    u32 load(void*, u32, u32);
    void unload();
    void updateObtained();
    u32 itemId() const;
    u32 eventId() const;
};
WWHD_OFFSET(hdPearlButton_c, mPearlPanes, 0x78);
WWHD_OFFSET(hdPearlButton_c, mPearlIndex, 0x80);
WWHD_OFFSET(hdPearlButton_c, mObtained, 0x84);
WWHD_SIZE(hdPearlButton_c, 0x88);
