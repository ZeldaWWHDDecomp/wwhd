#pragma once
#include "d/hd_message_parts.h"

struct ProgressBarPart_l : hdParts_c {
    be<u32> mTarget; be<u32> mShown;
    static ProgressBarPart_l* construct(ProgressBarPart_l* part);
    void destroy(u32 flags);
    u32 create(void* archive,void* resource,u32 mode,u32 flags);
    void refresh();
    void loadProgress();
    void reset();
    void dispatchUpdate();
    u32 createLayout(void* archive,void* resource,u32 mode,u32 flags,u32 user);
};
WWHD_SIZE(ProgressBarPart_l, 0x58);

struct ProgressPlay_l { u8 mReserved[0x5B62]; be<s16> mShown; };
WWHD_OFFSET(ProgressPlay_l,mShown,0x5B62);
