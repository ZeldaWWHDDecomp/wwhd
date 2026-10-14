#pragma once
#include "d/hd_message_parts.h"

struct NoControlPart_l : hdParts_c {
    static NoControlPart_l* construct(NoControlPart_l* part);
    void destroy(u32 flags);
    u32 create(void* archive,void* resource,u32 mode,u32 flags);
    u32 canControl();
    u32 canDecide();
    u32 canCancel();
    u32 canCursor();
    u32 canTouch();
    u32 canAdvance();
    u32 canClose();
    u32 canOpen();
    u32 canUpdate();
    u32 currentState();
};
WWHD_SIZE(NoControlPart_l, 0x50);
