/* WWHD Saku (wooden barricade) actor layout. */
#pragma once
#include "f_op/f_op_actor.h"

// HD collision objects retain their full audited 0x130-byte stride.
struct SakuCylinder { u8 storage[0x130]; };
WWHD_SIZE(SakuCylinder, 0x130);
struct SakuSmokeCallback { u8 storage[0x20]; };
WWHD_SIZE(SakuSmokeCallback, 0x20);

struct daSaku_c : fopAc_ac_c {
    /* 0x3AC */ SakuSmokeCallback smoke[2];
    /* 0x3EC */ u8 collisionStatus[0x3C];
    /* 0x428 */ SakuCylinder targetCylinders[2][3];
    /* 0xB48 */ cXyz collisionPositions[2][3];
    /* 0xB90 */ SakuCylinder fireCylinders[3];
    /* 0xF20 */ request_of_phase_process_class modelPhase;
    /* 0xF28 */ request_of_phase_process_class collisionPhase;
    /* 0xF30 */ be<u32> heaps[2][2];
    /* 0xF40 */ be<u32> models[2][2];
    /* 0xF50 */ be<u32> backgrounds[2][2];
    /* 0xF60 */ be<u32> activeBackgrounds[2];
    /* 0xF68 */ be<f32> backgroundMatrices[2][12];
    /* 0xFC8 */ be<u32> emitters[2];
    /* 0xFD0 */ be<f32> smokeAlpha[2];
    /* 0xFD8 */ be<s32> particleTimers[2];
    /* 0xFE0 */ cXyz smokePositions[2];
    /* 0xFF8 */ be<u8> modelAlpha[2][2];
    /* 0xFFC */ be<s32> collisionTimers[3];
    /* 0x1008 */ be<s32> fireTimer;
    /* 0x100C */ be<u8> heapReleaseDelay[2];
    /* 0x100E */ be<u8> type;
    /* 0x100F */ be<u8> flags;
    /* 0x1010 */ be<u8> burning;
    /* 0x1011 */ u8 padding[3];
    /* 0x1014 */ be<s32> state[2];
    /* 0x101C */ be<u32> bottomSwitch;
    /* 0x1020 */ be<u32> topSwitch;
};
WWHD_SIZE(daSaku_c, 0x1024);
WWHD_OFFSET(daSaku_c, smoke, 0x3AC);
WWHD_OFFSET(daSaku_c, targetCylinders, 0x428);
WWHD_OFFSET(daSaku_c, collisionPositions, 0xB48);
WWHD_OFFSET(daSaku_c, fireCylinders, 0xB90);
WWHD_OFFSET(daSaku_c, heaps, 0xF30);
WWHD_OFFSET(daSaku_c, models, 0xF40);
WWHD_OFFSET(daSaku_c, backgroundMatrices, 0xF68);
WWHD_OFFSET(daSaku_c, state, 0x1014);
WWHD_OFFSET(daSaku_c, topSwitch, 0x1020);
