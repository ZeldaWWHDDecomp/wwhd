/* Saved-vertex collision world: GameCube names and HD size 0xC4.
 * Local base views retain the verified fields without importing caller-only collision types. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
struct BgSvTri {
    be<u16> vtx0, vtx1, vtx2, id, grp;
};
struct BgSvData {
    be<s32> m_v_num;
    gptr<cXyz> m_v_tbl;
    be<s32> m_t_num;
    gptr<BgSvTri> m_t_tbl;
};
struct BgSvPolyInfo {
    be<u16> mPolyIndex;
    u8 otherInfo[14];
};
class dBgWSv {
  public:
    enum Flags_e { MOVE_BG_e = 1, NO_CALC_VTX_e = 2, GLOBAL_e = 0x20, UNK40_e = 0x40 };
    enum SvFlags_e { NO_CRR_POS = 1 };
    BOOL Set(BgSvData *, u32);
    void CopyBackVtx();
    BOOL CrrPosWork(cXyz *, u32, u32, u32);
    BOOL TransPosWork(cXyz *, u32, u32, u32);
    void CrrPos(BgSvPolyInfo *, void *, u32, cXyz *, csXyz *, csXyz *);
    void TransPos(BgSvPolyInfo *, void *, u32, cXyz *, csXyz *, csXyz *);
    void MatrixCrrPos(BgSvPolyInfo *, void *, u32, cXyz *, csXyz *, csXyz *);
    /* 0x00 */ be<u32> m_id;
    /* 0x04 */ be<u32> __vtbl;
    u8 collisionState[0x88];
    /* 0x90 */ gptr<cXyz> pm_vtx_tbl;
    /* 0x94 */ gptr<BgSvData> pm_bgd;
    u8 derivedState[0x24];
    /* 0xBC */ be<u32> mSvFlag;
    /* 0xC0 */ gptr<cXyz> mBackVtx;
};
WWHD_SIZE(BgSvTri, 0xA);
WWHD_OFFSET(dBgWSv, pm_vtx_tbl, 0x90);
WWHD_OFFSET(dBgWSv, pm_bgd, 0x94);
WWHD_OFFSET(dBgWSv, mSvFlag, 0xBC);
WWHD_OFFSET(dBgWSv, mBackVtx, 0xC0);
WWHD_SIZE(dBgWSv, 0xC4);
void *operator_new(u32);
void *operator_new_array(u32);
void constructBaseWorld(dBgWSv *);
BOOL SetBaseBackground(dBgWSv *, BgSvData *, u32, void *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
