#pragma once
#include "bindings.h"
// HD profile allocates 0x880. GC's recovered header has no member layout.
struct daTag_Kf1_c : fopNpc_npc_c {
    be<s16> actionAdjustment, actionIndex; // 7DC,7DE: GHS member-function descriptor
    be<u32> actionTarget;                 // 7E0
    u8 eventCut[0x6C];                    // 7E4
    be<u8> attention; u8 pad851;
    be<s16> missingPartners; u8 pad854[2];
    be<u16> rupees;                       // 856
    be<u32> partnerIds[8];                // 858
    be<s16> partnerCount;                 // 878
    be<s8> cutIndex, order, state;
    be<u8> field87D;
    be<s8> actionPhase; u8 pad87F;
};
WWHD_OFFSET(daTag_Kf1_c, actionAdjustment, 0x7DC);
WWHD_OFFSET(daTag_Kf1_c, eventCut, 0x7E4);
WWHD_OFFSET(daTag_Kf1_c, attention, 0x850);
WWHD_OFFSET(daTag_Kf1_c, partnerIds, 0x858);
WWHD_OFFSET(daTag_Kf1_c, actionPhase, 0x87E);
WWHD_SIZE(daTag_Kf1_c, 0x880);
