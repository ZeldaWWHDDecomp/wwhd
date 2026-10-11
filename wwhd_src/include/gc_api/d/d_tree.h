/* GameCube tree packet API. */
#pragma once
#include "bindings.h"

struct dTree_packet_c {
    /* 025CAAF4 newData [v d_a_grass]; guest pointer form of the position reference. */
    void newData(cXyz* pos, u8 status, s32 room) { gabi::call(0x025CAAF4, this, pos, status, room); }
};
