#pragma once
#include "d/hd_parts.h"
/* HD Valid UI part: shared parts/state/layout views from FW-parts_loop_display. */
struct hdPartsValid_c : hdParts_c {
    be<u8> mValid;
    u8 mPadding[3];
    u32 create(void* archive, void* name, void* parent, u32 flags);
    void update();
    u32 createLayout(void* archive, void* name, void* parent, u32 flags, void* heap);
    void initializeState_ChangeValid();
    void executeState_ChangeValid();
    void initializeState_Valid();
    void finalizeState_Valid();
    void initializeState_Invalid();
    void finalizeState_Invalid();
    void changeValid(u32 valid);
    void setValid(u32 valid);
    u8 isValid() const;

};
WWHD_OFFSET(hdPartsValid_c, mValid, 0x50);
WWHD_SIZE(hdPartsValid_c, 0x54);
enum ValidAnimationId { CHANGE_VALID = 0, LOOP_ALL = 1, LOOP_VALID = 2, LOOP_INVALID = 3 };
void ValidAnimation_stop(hdAnimSet*, u32);
void ValidAnimation_setEnd(hdAnimSet*, u32, u32);
void ValidAnimation_setStart(hdAnimSet*, u32, u32);
