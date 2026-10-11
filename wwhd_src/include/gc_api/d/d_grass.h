/* GameCube grass packet API. */
#pragma once
#include "bindings.h"

struct dGrass_packet_c {
    /* 0254D2D4 newData [v d_a_grass]; guest pointer form of the position reference. */
    void newData(cXyz* pos, s32 room, s8 item) { gabi::call(0x0254D2D4, this, pos, room, item); }
};
