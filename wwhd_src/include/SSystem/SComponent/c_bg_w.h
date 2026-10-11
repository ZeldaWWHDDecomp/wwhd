/* Background mesh: GameCube names and verified HD layouts.
 * Minimal local geometry/check views avoid importing caller-facing dBgW bindings.
 * HD: cBgWMesh/cBgD_Mesh are typed views of the existing opaque cBgW/cBgD_t names.
 * Guest references are pointers; explicit vtables preserve GHS dispatch. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"

struct BgMtx {
    be<f32> rows[3][4];
};
struct cBgD_Blk_t {
    be<u16> startTri;
};
struct cBgD_Tri_t {
    be<u16> vtx0, vtx1, vtx2, id, grp;
};
struct cBgD_Tree_t {
    be<u16> mFlag, mParent;
    be<u16> mChild[8];
};
struct cBgD_Grp_t {
    gptr<char> m_name;
    cXyz m_scale;
    csXyz m_rotation;
    u8 padding[2];
    cXyz m_translation;
    be<u16> m_parent, m_next_sibling, m_first_child, m_room_id, m_first_vtx_idx, m_tree_idx;
    be<u32> m_info;
};
struct cBgD_Mesh {
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
struct cBgW_Plane {
    cXyz mNormal;
    be<f32> mD;
    be<u32> __vtbl;
};
struct cBgW_TriElm {
    cBgW_Plane m_plane;
    be<u32> __vtbl;
};
struct cBgW_RwgElm {
    be<u16> next;
    u8 padding[2];
    be<u32> __vtbl;
};
struct cBgW_BlkElm {
    be<u16> roof, wall, ground;
};
struct cBgW_NodeTree {
    cXyz mMin, mMax;
    be<u32> __vtbl;
};
struct cBgW_GrpElm {
    cBgW_NodeTree aab;
    be<u32> __vtbl;
};
struct cBgW_Line {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
};
struct cBgW_Triangle {
    cBgW_Plane m_plane;
    cXyz mA, mB, mC;
};
struct cBgW_PolyInfo {
    gptr<void> mPolyPassChk, mGrpPassChk;
    u8 actorInfo[12];
    be<u16> mPolyIndex;
    u8 otherPolyInfo[14];
};
struct cBgW_LinChk : cBgW_PolyInfo {
    cBgW_Line mLin;
    cXyz mWork;
    be<u32> mFlag;
    be<u8> mPreWallChk, mPreGroundChk, mPreRoofChk, mFrontFlag, mBackFlag;
};
struct cBgW_GndChk : cBgW_PolyInfo {
    cXyz m_pos;
    be<u32> mFlag;
    be<f32> mNowY;
    be<u32> mWallPrecheck, mGndPrecheck;
};
class cBgWMesh;
struct cBgW_Vtable {
    u8 unused[0x1C];
    be<u32> CalcPlane, reserved20, ClassifyPlane, reserved28, ChkPolyThrough;
    u8 reserved30[12];
    be<u32> ChkGrpThrough;
};
class cBgWMesh {
  public:
    enum Flags_e {
        MOVE_BG_e = 1,
        NO_CALC_VTX_e = 2,
        NO_VTX_TBL_e = 0x10,
        GLOBAL_e = 0x20,
        UNK40_e = 0x40,
        LOCK_e = 0x80
    };
    enum IgnorePlane_e { IGNORE_GROUND = 1, IGNORE_WALL = 2, IGNORE_ROOF = 4, FLUSH = 8 };
    void FreeArea();
    void GlobalVtx();
    void CopyOldMtx();
    void MakeBlckTransMinMax(cXyz *, cXyz *);
    void MakeBlckMinMax(s32, cXyz *, cXyz *);
    void MakeBlckBnd(u32, cXyz *, cXyz *);
    void MakeNodeTreeRp(u32);
    void MakeNodeTreeGrpRp(u32);
    void MakeNodeTree();
    void Move();
    u8 ChkMemoryError();
    u8 SetVtx();
    u8 SetTri();
    u8 Set(cBgD_Mesh *, u32, BgMtx *);
    void GetTriPnt(u32, cXyz *, cXyz *, cXyz *);
    u32 GetGrpToRoomIndex(s32);
    void GetTrans(cXyz *);
    void GetTopUnder(be<f32> *, be<f32> *);
    void CalcPlane();
    void ClassifyPlane();
    void BlckConnect(be<u16> *, be<s32> *, u32);
    u8 RwgLineCheck(s32, cBgW_LinChk *);
    u8 LineCheckRp(cBgW_LinChk *, u32);
    u8 LineCheckGrpRp(cBgW_LinChk *, u32, u32);
    u8 RwgGroundCheckCommon(s32, cBgW_GndChk *, f32);
    u8 RwgGroundCheckGnd(u32, cBgW_GndChk *);
    u8 RwgGroundCheckWall(u32, cBgW_GndChk *);
    u8 GroundCrossRp(cBgW_GndChk *, u32);
    u8 GroundCrossGrpRp(cBgW_GndChk *, u32, u32);
    // HD: virtual entries must retain dynamic dispatch, including during verification.
    void DispatchCalcPlane() {
        gabi::call_ptr<void>(__vtbl->CalcPlane, this);
    }
    void DispatchClassifyPlane() {
        gabi::call_ptr<void>(__vtbl->ClassifyPlane, this);
    }
    s32 DispatchPolyThrough(s32 index, void *pass) {
        return gabi::call_ptr<s32>(__vtbl->ChkPolyThrough, this, index, pass);
    }
    s32 DispatchGrpThrough(u32 index, void *pass, u32 depth) {
        return gabi::call_ptr<s32>(__vtbl->ChkGrpThrough, this, index, pass, depth);
    }
    be<s32> m_id;
    gptr<cBgW_Vtable> __vtbl;
    gptr<BgMtx> pm_base;
    BgMtx mOldMtx, mCurMtx;
    be<u8> mFlags, mbNeedsFullTransform;
    be<u16> padding6E;
    be<u32> unusedHeaderWord; // HD: constructor clears this padding word.
    be<u8> mMoveCounter, mWallCorrectPriority;
    u8 padding76[2];
    be<u32> mIgnorePlaneType;
    cXyz mTransVel;
    gptr<cBgW_TriElm> pm_tri;
    gptr<cBgW_RwgElm> pm_rwg;
    gptr<cXyz> pm_vtx_tbl;
    gptr<cBgD_Mesh> pm_bgd;
    gptr<cBgW_BlkElm> pm_blk;
    gptr<cBgW_GrpElm> pm_grp;
    gptr<cBgW_NodeTree> m_nt_tbl;
    be<u32> m_rootGrpIdx;
};
WWHD_SIZE(cBgD_Tri_t, 0xA);
WWHD_SIZE(cBgD_Tree_t, 0x14);
WWHD_SIZE(cBgD_Grp_t, 0x34);
WWHD_SIZE(cBgW_TriElm, 0x18);
WWHD_SIZE(cBgW_RwgElm, 8);
WWHD_SIZE(cBgW_BlkElm, 6);
WWHD_SIZE(cBgW_NodeTree, 0x1C);
WWHD_SIZE(cBgW_GrpElm, 0x20);
WWHD_SIZE(cBgW_Triangle, 0x38);
WWHD_SIZE(cBgW_PolyInfo, 0x24);
WWHD_OFFSET(cBgW_LinChk, mLin, 0x24);
WWHD_OFFSET(cBgW_LinChk, mPreWallChk, 0x50);
WWHD_OFFSET(cBgW_GndChk, mNowY, 0x34);
WWHD_OFFSET(cBgWMesh, pm_base, 8);
WWHD_OFFSET(cBgWMesh, mOldMtx, 0xC);
WWHD_OFFSET(cBgWMesh, mCurMtx, 0x3C);
WWHD_OFFSET(cBgWMesh, mFlags, 0x6C);
WWHD_OFFSET(cBgWMesh, mTransVel, 0x7C);
WWHD_OFFSET(cBgWMesh, pm_bgd, 0x94);
WWHD_OFFSET(cBgWMesh, m_rootGrpIdx, 0xA4);
WWHD_SIZE(cBgWMesh, 0xA8);
