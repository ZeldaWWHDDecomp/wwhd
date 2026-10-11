/** HD UI event layouts; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdUiEvent {
    be<s32> mKind;
    be<s32> mId;
    u8 mPayload[0x20];
};
WWHD_OFFSET(hdUiEvent, mPayload, 8);
WWHD_SIZE(hdUiEvent, 0x28);
struct hdUiEventBox {
    be<u32> field_0x0; // HD: outer bookkeeping roles are unconfirmed.
    be<u32> field_0x4;
    hdUiEvent mEvent;
    be<u32> field_0x30;
    be<u32> mVtable;
};
WWHD_OFFSET(hdUiEventBox, mEvent, 8);
WWHD_OFFSET(hdUiEventBox, mVtable, 0x34);
WWHD_SIZE(hdUiEventBox, 0x38);
struct hdUiInputStateHolder {
    be<u32> field_0x0; // HD: initialized to zero; role unconfirmed.
    be<u32> mVtable;
};
WWHD_SIZE(hdUiInputStateHolder, 8);
u32 hdUiBlockSet(void* destination, u32 value, u32 size);
void hdUiBlockMove(void* destination, const void* source, u32 size, u32 flush);
