/* Height-field background collision, GameCube names and HD guest layouts.
 * HD retains the 0xD4 derived size; local base views describe only verified fields. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
struct cBgD_Tri_t {
    be<u16> vtx0, vtx1, vtx2, id, grp;
};
struct cBgD_Blk_t {
    be<u16> startTri;
};
struct cBgD_Tree_t {
    be<u16> mFlag, mParent, mChild[8];
};
struct cBgD_Grp_t {
    u8 transforms[0x24];
    be<u16> m_parent, m_next_sibling, m_first_child;
    u8 metadata[4];
    be<u16> m_tree_idx;
    u8 tail[4];
};
struct HfPrevious {
    be<u32> index;
};
// HD: local cBgD_t view avoids redefining the shared opaque declaration.
struct HfData {
    be<s32> m_v_num;
    gptr<cXyz> m_v_tbl;
    be<s32> m_t_num;
    gptr<cBgD_Tri_t> m_t_tbl;
    be<s32> m_b_num;
    gptr<cBgD_Blk_t> m_b_tbl;
    be<s32> m_tree_num;
    gptr<cBgD_Tree_t> m_tree_tbl;
    be<s32> m_g_num;
    gptr<cBgD_Grp_t> m_g_tbl;
};
struct cBgW_TriElm {
    u8 plane[24];
};
struct cBgW_BlkElm {
    be<u16> roof, wall, ground;
};
struct cBgW_NodeTree {
    cXyz min, max;
    be<u32> vtable;
};
struct cBgW_GrpElm {
    cBgW_NodeTree aab;
    be<u32> flags;
};
struct HfVtable {
    u8 prefix[0x1C];
    be<u32> CalcPlane;
    be<u32> middle;
    be<u32> ClassifyPlane;
};
class dBgWHf {
  public:
    enum Flags { MOVE_BG_e = 1, NO_CALC_VTX_e = 2, NO_VTX_TBL_e = 0x10, GLOBAL_e = 0x20 };
    enum SvFlags { NO_CRR_POS = 1 };
    enum { INVALID_INDEX = 0xFFFF, LEAF = 1, MOVE = 1, LOCK = 0x80 };
    bool Set(HfData *, be<u16> *, f32, s32, s32, u32);
    void CalcPlane();
    void ClassifyPlane();
    void MoveHf();
    void MakeBlckMinMaxHf(s32, be<f32> *, be<f32> *);
    void MakeBlckBndHf(s32, be<f32> *, be<f32> *);
    void MakeNodeTreeRpHf(s32);
    void MakeNodeTreeGrpRpHf(s32);
    void MakeNodeTreeHf();
    void MatrixCrrPos();
    /* 0x00 */ be<u32> m_id;
    /* 0x04 */ gptr<HfVtable> __vtbl;
    u8 baseState[0x64];
    /* 0x6C */ be<u8> mFlags;
    u8 collisionState[0x1B];
    /* 0x88 */ gptr<cBgW_TriElm> pm_tri;
    u8 triangleState[4];
    /* 0x90 */ gptr<cXyz> pm_vtx_tbl;
    /* 0x94 */ gptr<HfData> pm_bgd;
    /* 0x98 */ gptr<cBgW_BlkElm> pm_blk;
    /* 0x9C */ gptr<cBgW_GrpElm> pm_grp;
    /* 0xA0 */ gptr<cBgW_NodeTree> m_nt_tbl;
    /* 0xA4 */ be<u32> m_rootGrpIdx;
    u8 worldState[0x12];
    /* 0xBA */ be<u8> mFlag;
    u8 padding;
    /* 0xBC */ be<u32> mSvFlag;
    /* 0xC0 */ gptr<cXyz> mBackVtx;
    /* 0xC4 */ be<f32> mC4;
    /* 0xC8 */ gptr<be<u16>> mC8;
    /* 0xCC */ be<s32> m_gridx;
    /* 0xD0 */ be<s32> m_gridz;
};
WWHD_SIZE(cBgD_Tri_t, 10);
WWHD_SIZE(cBgD_Tree_t, 20);
WWHD_SIZE(cBgD_Grp_t, 52);
WWHD_SIZE(cBgW_NodeTree, 28);
WWHD_SIZE(cBgW_GrpElm, 32);
WWHD_OFFSET(dBgWHf, pm_tri, 0x88);
WWHD_OFFSET(dBgWHf, pm_vtx_tbl, 0x90);
WWHD_OFFSET(dBgWHf, mC4, 0xC4);
WWHD_SIZE(dBgWHf, 0xD4);
void *operator_new(u32);
void *operator_new_array(u32);
void constructSavedVertexWorld(dBgWHf *);
s32 SetBaseWorld(dBgWHf *, HfData *, u32, void *);
void SetupPlane(cBgW_TriElm *, cXyz *, cXyz *);
void BlckConnect(dBgWHf *, be<u16> *, be<u32> *, s32);
void ClearForMinMaxY(cBgW_NodeTree *);
void SetMinMaxY(cBgW_NodeTree *, f32);
void SetMinY(cBgW_GrpElm *, f32);
void SetMaxY(cBgW_GrpElm *, f32);
void assertion(const char *, u32, const char *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);
