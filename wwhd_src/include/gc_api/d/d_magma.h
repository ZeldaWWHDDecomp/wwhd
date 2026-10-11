/* d_magma (GameCube names) -> WWHD. Local game-derived API. */
#pragma once
#include "bindings.h"

class dMagma_packet_c {
public:
    /* 0258CC64 [v d_a_magma] */
    void newFloor(cXyz& pos, cXyz& scale, s32 roomNo, s16 pathNo) {
        gabi::call(0x0258CC64, this, &pos, &scale, roomNo, pathNo);
    }
};
