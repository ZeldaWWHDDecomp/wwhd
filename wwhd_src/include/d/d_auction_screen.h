#pragma once
#include "gc_api/gc_api.h"

// HD: Auction animation flags live in a controller rather than GameCube TU statics.
// These layouts describe only the verified prefix accessed by the flag adapters.
struct dAuction_controller_c {
    u8 padding[0x65];
    be<u8> talkmode;
    u8 padding66;
    be<u8> slotShow;
    u8 padding68[2];
    be<u8> gaugeShow;
};
WWHD_OFFSET(dAuction_controller_c, talkmode, 0x65);
WWHD_OFFSET(dAuction_controller_c, slotShow, 0x67);
WWHD_OFFSET(dAuction_controller_c, gaugeShow, 0x6A);
WWHD_SIZE(dAuction_controller_c, 0x6B);

struct dAuction_state_c {
    u8 padding[0x1F4];
    gptr<dAuction_controller_c> mpController;
};
WWHD_OFFSET(dAuction_state_c, mpController, 0x1F4);
WWHD_SIZE(dAuction_state_c, 0x1F8);
