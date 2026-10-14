#pragma once
#include "d/hd_message_parts.h"

struct OneTimePart_l : hdParts_c {
    be<u8> mTriggered; u8 mReserved51[3];
    static OneTimePart_l* construct(OneTimePart_l* part);
    void destroy(u32 flags);
    u32 create(void* archive,void* resource,u32 mode,u32 flags);
    u32 currentState();
    void resetAnimation();
    void update();
    void dispatchUpdate();
    u32 createLayout(void* archive,void* resource,u32 mode,u32 flags,u32 user);
};
WWHD_SIZE(OneTimePart_l, 0x54);
