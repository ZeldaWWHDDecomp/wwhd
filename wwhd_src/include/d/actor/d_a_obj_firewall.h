#pragma once
#include "f_op/f_op_actor.h"
/* HD layout reconstructed from constructor and member accesses.
 * Guest actor allocations are four-byte aligned; byte-backed be<> wrappers
 * describe the guest layout without imposing host struct alignment. */
struct daObjFirewall_c : fopAc_ac_c {
    /* 0x3AC */ be<u32> phase[2];
    /* 0x3B4 */ u8 collisionStatus[0x3c];
    /* 0x3F0 */ u8 collisionCylinder[0x130];
    /* 0x520 */ gptr<J3DModel> model;
    /* 0x524 */ be<u32> background;
    /* 0x528 */ u8 textureAnimation[0x74];
    /* 0x59C */ u8 colorAnimation[0x78];
    /* 0x614 */ be<u32> emitters[12];
    /* 0x644 */ be<u32> saveSwitch;
    /* 0x648 */ u8 light[0x24];
    /* 0x66C */ cXyz lightPosition;
    /* 0x678 */ be<u32> unknown678;
    /* 0x67C */ be<f32> lightStrength;
    /* 0x680 */ be<s16> actionAdjustment;
    /* 0x682 */ be<s16> actionVirtualIndex;
    /* 0x684 */ be<u32> action;
    /* 0x688 */ be<s16> eventIndex;
    /* 0x68A */ be<u8> soundEnabled;
    /* 0x68B */ be<u8> unknown68b;
    /* 0x68C */ cXyz soundPositions[8];
    /* 0x6EC */ be<u8> soundInitialized;
    /* 0x6ED */ be<u8> burning;
    /* 0x6EE */ u8 pad6ee[2];
    /* 0x6F0 */ be<u32> eventStarted;
    /* 0x6F4 */ be<u32> playerSoundIndex;
};
WWHD_OFFSET(daObjFirewall_c,model,0x520);
WWHD_OFFSET(daObjFirewall_c,emitters,0x614);
WWHD_OFFSET(daObjFirewall_c,action,0x684);
WWHD_OFFSET(daObjFirewall_c,soundPositions,0x68C);
WWHD_SIZE(daObjFirewall_c,0x6F8);
