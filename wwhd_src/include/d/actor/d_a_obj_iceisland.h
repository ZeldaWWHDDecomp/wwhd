#pragma once
#include "f_op/f_op_actor.h"

// WWHD expands animation objects and lighting state; actor size is 0x6F4.
struct daObjIceisland_c : fopAc_ac_c {
    u8 phase[8];
    be<u32> model;
    u8 btk1[0x74], btk2[0x74], brk[0x78];
    be<u32> emitter1, emitter2;
    u8 lighting[0x1C0];
    u8 _6E0[10];
    be<s16> windVolume, meltEvent, freezeEvent;
    be<u32> state;
};
WWHD_OFFSET(daObjIceisland_c,model,0x3B4);
WWHD_OFFSET(daObjIceisland_c,emitter1,0x518);
WWHD_OFFSET(daObjIceisland_c,windVolume,0x6EA);
WWHD_OFFSET(daObjIceisland_c,state,0x6F0);
static_assert(sizeof(daObjIceisland_c)==0x6F4);
