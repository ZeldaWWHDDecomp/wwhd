#pragma once
#include "d/actor/d_a_npc_tc.h"

// Only the fields read by this cut unit; numeric names are those in tww's daNpc_Tc_HIO_c.
// HD: these six HIO fields retain their GameCube offsets.
struct daNpc_Tc_HIO_cut {
    u8 padding[0x38];
    be<f32> field_0x38;
    be<f32> field_0x3C;
    be<f32> field_0x40;
    u8 padding44[4];
    be<f32> field_0x48;
    be<f32> field_0x4C;
    be<f32> field_0x50;
};
WWHD_OFFSET(daNpc_Tc_HIO_cut, field_0x50, 0x50);

// HD: emitter global particle scale moved to +0x238; keep the z,x,y store order.
struct TcEmitter {
    u8 padding[0x238];
    cXyz mGlobalParticleScale;
};
WWHD_OFFSET(TcEmitter, mGlobalParticleScale, 0x238);

WWHD_SIZE(daNpc_Tc_HIO_cut, 0x54);
WWHD_SIZE(TcEmitter, 0x244);

struct TcActionTable {
    gptr<char> entries[9];
};
WWHD_SIZE(TcActionTable, 0x24);
