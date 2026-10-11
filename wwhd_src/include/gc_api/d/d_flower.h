/* GameCube flower packet API. */
#pragma once
#include "bindings.h"

struct dFlower_packet_c {
    /* 02548FEC newData [v d_a_grass]; guest pointer form of the position reference. */
    void newData(s8 type, cXyz* pos, s32 room, s8 item) { gabi::call(0x02548FEC, this, type, pos, room, item); }
};
