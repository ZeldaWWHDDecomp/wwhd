#pragma once
#include "d/actor/d_a_npc_tc.h"

// Only the fields read by this cut unit; a view of daNpc_Tc_HIO_c (d_a_npc_tc.h), whose names it takes.
// HD: these six HIO fields retain their GameCube offsets.
struct daNpc_Tc_HIO_cut {
    u8 padding[0x38];
    be<f32> mJumpSpeedF;  /* was field_0x38: view of daNpc_Tc_HIO_c mJumpSpeedF (l_cutHIO is l_HIO, 0x104687F4): cutSitToJumpProc 022E93D4 sets speedF from it */
    be<f32> mJumpGravity;  /* was field_0x3C: view of daNpc_Tc_HIO_c mJumpGravity (l_cutHIO is l_HIO, 0x104687F4): cutSitToJumpProc 022E93D4 sets gravity from it */
    be<f32> mJumpSpeedY;  /* was field_0x40: view of daNpc_Tc_HIO_c mJumpSpeedY (l_cutHIO is l_HIO, 0x104687F4): cutSitToJumpProc 022E93D4 sets speed.y from it */
    u8 padding44[4];
    be<f32> mBackJumpSpeedF;  /* was field_0x48: view of daNpc_Tc_HIO_c mBackJumpSpeedF (l_cutHIO is l_HIO, 0x104687F4): cutBackJumpStart 022E919C sets speedF from it */
    be<f32> mBackJumpGravity;  /* was field_0x4C: view of daNpc_Tc_HIO_c mBackJumpGravity (l_cutHIO is l_HIO, 0x104687F4): cutBackJumpStart 022E919C sets gravity from it */
    be<f32> mBackJumpSpeedY;  /* was field_0x50: view of daNpc_Tc_HIO_c mBackJumpSpeedY (l_cutHIO is l_HIO, 0x104687F4): cutBackJumpStart 022E919C sets speed.y from it */
};
WWHD_OFFSET(daNpc_Tc_HIO_cut, mBackJumpSpeedY, 0x50);

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
