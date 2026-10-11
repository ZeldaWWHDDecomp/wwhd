/* daAgbsw0_c (Tingle bomb switch), WWHD layout.
 * GameCube -> WWHD: the Tingle Tuner fields before mTimer are removed; only the bomb trigger
 * remains. Actor base size is 0x3AC, and the timer and collision members total 0x170 bytes. */
#pragma once
#include "gc_api/gc_api.h"

class daAgbsw0_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x10006D9C, __aab_vt = 0x10006D8C };
    daAgbsw0_c();
    cPhs_State create();
    BOOL execute();
    u8 getSw0() { return fopAcM_GetParam(this) >> 0x10 & 0xFF; }

    // HD: view of the mutable collision source; its unchanged collision flags precede the cylinder.
    struct CylinderSource {
        /* 0x00 */ u8 mObjInf[0x30];
        struct {
            struct {
                /* 0x30 */ cXyz mCenter;
                /* 0x3C */ be<f32> mRadius;
                /* 0x40 */ be<f32> mHeight;
            } mCyl;
        } mCylAttr;
    };

    /* 0x3AC */ be<u32> mTimer;
    /* 0x3B0 */ dCcD_Stts mStts;
    /* 0x3EC */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(daAgbsw0_c, mTimer, 0x3AC);
WWHD_OFFSET(daAgbsw0_c, mStts, 0x3B0);
WWHD_OFFSET(daAgbsw0_c, mCyl, 0x3EC);
WWHD_SIZE(daAgbsw0_c, 0x51C);
WWHD_OFFSET(daAgbsw0_c::CylinderSource, mCylAttr, 0x30);
WWHD_SIZE(daAgbsw0_c::CylinderSource, 0x44);
