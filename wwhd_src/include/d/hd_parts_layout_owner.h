#pragma once
#include "d/hd_message_parts.h"

struct hdMessageLayoutOwner_c : hdPartsPartsPane_c {
 static hdMessageLayoutOwner_c* construct(hdMessageLayoutOwner_c*,void*,u32,u32);
 void destroy(u32);
};
WWHD_OFFSET(hdMessageLayoutOwner_c, mPane, 0);
WWHD_OFFSET(hdMessageLayoutOwner_c, mAnimated, 0xB0);
WWHD_SIZE(hdMessageLayoutOwner_c, 0xB4);
