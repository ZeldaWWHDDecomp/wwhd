/* dBgWDeform: deformable background collision, WWHD layout.
 * GameCube -> WWHD: the verified base members retain their offsets. HD Set omits the
 * J3DModel argument and skin-deform setup. Only the accessed layout prefix is described. */
#pragma once
#include "bindings.h"
#include "gc_api/MSL_C++/new.h"

/* cBgD_t prefix: the unsigned vertex count used by HD allocation. */
struct dBgWDeformBgData {
    /* 0x00 */ be<u32> m_v_num;
};
WWHD_SIZE(dBgWDeformBgData, 4);

class dBgWDeform {
public:
    enum Flags_e { MOVE_BG_e = 1, NO_CALC_VTX_e = 2, NO_VTX_TBL_e = 0x10, GLOBAL_e = 0x20 };
    BOOL Set(cBgD_t* bgd, u32 flag);
    void SetFlag(u32 flag) { mSvFlag = flag; }
    bool ChkNoCrrPos() { return mSvFlag & 1; }
    void SetBackVtx(cXyz* vtx) { mBackVtx = vtx; }
    cXyz* GetBackVtx() { return mBackVtx; }

    /* 0x00 */ u8 _00[0x94];
    /* 0x94 */ gptr<dBgWDeformBgData> pm_bgd;
    /* 0x98 */ u8 _98[0xBC - 0x98];
    /* 0xBC */ be<u32> mSvFlag;
    /* 0xC0 */ gptr<cXyz> mBackVtx;
};
WWHD_OFFSET(dBgWDeform, pm_bgd, 0x94);
WWHD_OFFSET(dBgWDeform, mSvFlag, 0xBC);
WWHD_OFFSET(dBgWDeform, mBackVtx, 0xC0);
WWHD_SIZE(dBgWDeform, 0xC4);
