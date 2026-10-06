#pragma once
#include "bindings.h"
struct BalanceQuat { be<f32> x,y,z,w; };
struct daBalancelift_c : fopAc_ac_c {
    cXyz anchorTop;            // 3AC
    cXyz chainPosition;        // 3B8
    cXyz targetChainPosition;  // 3C4
    cXyz chainVelocity;        // 3D0
    cXyz chainTop;             // 3DC
    BalanceQuat supportRotation; //3E8
    BalanceQuat platformRotation;//3F8
    cXyz pivot;                //408
    cXyz pivotVelocity;        //414
    cXyz platformCenter;       //420
    be<f32> chainLength;       //42C
    be<s16> weight;
    u8 pad432[2];
    gptr<be<s16>> sharedWeight; //434
    be<s16> isSecond;
    be<s16> weightFlags;
    gptr<be<s16>> sharedFlags;  //43C
    request_of_phase_process_class phase;
    gptr<J3DModel> model;
    Mtx34 backgroundMatrix;
    gptr<u8> path;
    gptr<u8> background;
    be<f32> verticalSpeed;
    u8 pad488[0x178];
    gptr<u8> chainPacket;       //600
    u8 collisionStatus[0x3C];  //604
    u8 cylinder[0x130];        //640
    BOOL CreateHeap();
    void set_mtx();
    s32 CreateInit();
    void calc_weight();
    void calc_quat();
};
WWHD_OFFSET(daBalancelift_c, anchorTop, 0x3AC);
WWHD_OFFSET(daBalancelift_c, supportRotation, 0x3E8);
WWHD_OFFSET(daBalancelift_c, pivot, 0x408);
WWHD_OFFSET(daBalancelift_c, sharedWeight, 0x434);
WWHD_OFFSET(daBalancelift_c, phase, 0x440);
WWHD_OFFSET(daBalancelift_c, model, 0x448);
WWHD_OFFSET(daBalancelift_c, background, 0x480);
WWHD_OFFSET(daBalancelift_c, chainPacket, 0x600);
WWHD_OFFSET(daBalancelift_c, cylinder, 0x640);

WWHD_SIZE(daBalancelift_c, 0x770);
