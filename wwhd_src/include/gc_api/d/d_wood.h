/* dWood packet API, GameCube names with WWHD call addresses. */
#pragma once
#include "bindings.h"
namespace dWood {
struct Packet_c {
    /* 025D077C [v d_a_obj_wood] */
    s32 put_unit(const cXyz& pos, s32 roomNo) { return gabi::call<s32>(0x025D077C, this, &pos, roomNo); }
};
}
