#pragma once
#include "gc_api/gc_api.h"
namespace daTagvolcano {
struct Act_c : fopAc_ac_c {
    Act_c();
    be<u32> field_0x290, field_0x294;
    be<u8> field_0x298, field_0x299;
    u8 padding[2];
    be<s32> mType, field_0x2a0;
    be<u8> field_0x2a4;
    u8 tail[3];
    s32 _create();
    BOOL check_timer_clear();
    BOOL _delete();
    BOOL _execute();
};
WWHD_OFFSET(Act_c, field_0x298, 0x3B4);
WWHD_OFFSET(Act_c, mType, 0x3B8);
WWHD_OFFSET(Act_c, field_0x2a0, 0x3BC);
WWHD_OFFSET(Act_c, field_0x2a4, 0x3C0);
WWHD_SIZE(Act_c, 0x3C4);
} // namespace daTagvolcano

// HD: these bindings retain the original timer calls and argument shapes.
void TagVolcano_timerDelete(void*);
void TagVolcano_timerStart(void*);
void TagVolcano_timerStop(void*, s32);
void TagVolcano_timerRestart(void*, s32);
void dTimer_createTimer(s32, u16, s32, s32, f32, f32, f32, f32);
BOOL TagVolcano_endCheck(dEvent_manager_c*, const char*);
void TagVolcano_seStart(u32);
void TagVolcano_orderOtherEvent(fopAc_ac_c*, const char*, u8, u16);
void TagVolcano_setNextStage(const char*, s32, s32, s32, s32, s32, s32, f32);
BOOL TagVolcano_isTbox(dSv_memory_c*, u32);
