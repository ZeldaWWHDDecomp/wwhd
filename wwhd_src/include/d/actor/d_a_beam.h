#pragma once
#include "f_op/f_op_actor.h"
struct daBeam_c : fopAc_ac_c {
    u8 collision[0x658-0x3AC];
    be<u8> hit;
    u8 _659[3];
    cXyz hitPosition;
    be<u8> sideHit;
    u8 _669[3];
    be<s16> searchAdjustment, searchSlot;
    be<u32> searchTarget;
    be<s16> defaultAdjustment, defaultSlot;
    be<u32> defaultTarget;
    request_of_phase_process_class phase;
    be<u32> model, bck;
    u8 bckAnimation[0x718-0x68C];
    be<f32> beamFrame;
    be<u32> brk;
    u8 brkAnimation[0x798-0x720];
    be<f32> fadeFrame;
    be<u32> btk;
    u8 btkAnimation[0x814-0x7A0];
    u8 aimMatrix[0x30];
    be<s16> active, timer;
    be<u8> floorParticle, smokeParticle;
    u8 _84A[2];
    u8 lineCheck[0x8B8-0x84C];
    cXyz endpoint;
    be<f32> quaternion[4];
    be<u32> floorId;
    be<f32> floorScale;
    be<u32> floorActor, emitter;
    be<f32> emitterOffset;
    be<s32> switchId;
};
WWHD_SIZE(daBeam_c,0x8EC);
WWHD_OFFSET(daBeam_c,model,0x684);
WWHD_OFFSET(daBeam_c,beamFrame,0x718);
WWHD_OFFSET(daBeam_c,endpoint,0x8B8);
