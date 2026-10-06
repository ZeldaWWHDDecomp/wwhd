#pragma once
/* d_a_mozo.h (WWHD) - daMozo_c layout. */
#include "f_op/f_op_actor.h"
#include "d/d_cc_d.h"
#include "m_Do/m_Do_ext.h"

struct MozoProcedure {
    be<s16> adjustment, virtualIndex;
    be<u32> target;
};
struct MozoColorAnimation {
    J3DFrameCtrl frame;
    u8 storage[0x78 - 0x10];
};
struct daMozo_c : fopAc_ac_c {
    MozoProcedure procedure;                 // 0x3AC
    u8 phase[8];                             // 0x3B4
    gptr<mDoExt_McaMorf> morf;                // 0x3BC
    MozoColorAnimation colorAnimation;        // 0x3C0
    be<u32> colorResource;                    // 0x438
    mDoExt_btkAnm textureAnimation;            // 0x43C
    be<u32> textureResource;                  // 0x4B0
    cXyz beamOrigin, beamTarget;              // 0x4B4, 0x4C0
    cXyz leftOrigin, rightOrigin;             // 0x4CC, 0x4D8
    cXyz unrotatedOrigin, unrotatedTarget;     // 0x4E4, 0x4F0
    u8 unused4FC[12];
    cXyz fireOrigin, fireTarget;              // 0x508, 0x514
    cXyz soundPosition;                       // 0x520
    be<u32> leftChild, rightChild;            // 0x52C, 0x530
    be<f32> quaternion[4];                    // 0x534
    be<s32> animation;                        // 0x544
    be<s16> fireTimer;                        // 0x548
    u8 unused54A[4];
    be<u8> type, requestEvent;                // 0x54E
    be<u32> fireEmitter, smokeEmitter;         // 0x550
    dCcD_Stts collisionStatus;                // 0x558
    u8 capsule[0x138];                        // 0x594
};
WWHD_SIZE(MozoProcedure, 8);
WWHD_SIZE(MozoColorAnimation, 0x78);
WWHD_OFFSET(daMozo_c, procedure, 0x3AC);
WWHD_OFFSET(daMozo_c, phase, 0x3B4);
WWHD_OFFSET(daMozo_c, morf, 0x3BC);
WWHD_OFFSET(daMozo_c, colorAnimation, 0x3C0);
WWHD_OFFSET(daMozo_c, textureAnimation, 0x43C);
WWHD_OFFSET(daMozo_c, beamOrigin, 0x4B4);
WWHD_OFFSET(daMozo_c, fireOrigin, 0x508);
WWHD_OFFSET(daMozo_c, quaternion, 0x534);
WWHD_OFFSET(daMozo_c, type, 0x54E);
WWHD_OFFSET(daMozo_c, collisionStatus, 0x558);
WWHD_OFFSET(daMozo_c, capsule, 0x594);
WWHD_SIZE(daMozo_c, 0x6CC);
