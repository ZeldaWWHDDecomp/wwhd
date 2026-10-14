#pragma once
#include "d/hd_message_parts.h"

struct PaneOwnerPart_l {
 gptr<gfn> mpVTable; u8 mReserved[0x24]; gptr<void> mpPane;
 static PaneOwnerPart_l* construct(PaneOwnerPart_l*);
 void destroy(u32);
};
WWHD_OFFSET(PaneOwnerPart_l, mpPane, 0x28);
WWHD_SIZE(PaneOwnerPart_l, 0x2C);
