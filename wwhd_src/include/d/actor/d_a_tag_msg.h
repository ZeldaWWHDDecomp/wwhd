#pragma once
#include "bindings.h"
struct daTag_Msg_c : fopAc_ac_c {
 be<u8> mAction;
 u8 padding[3];
 u16 getMessage(); u32 getType2(); u32 getSwbit(); u32 getSwbit2();
 u16 getEventFlag(); u32 getEventNo();
 s32 rangeCheck(); s32 otherCheck(); s32 arrivalTerms(); const char* myDemoName();
};
WWHD_OFFSET(daTag_Msg_c,mAction,0x3AC);
WWHD_SIZE(daTag_Msg_c,0x3B0);
