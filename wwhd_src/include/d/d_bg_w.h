// Background collision mesh: GameCube names, verified HD layout.
// HD: this private complete dBgW view coexists with the shared caller-facing dBgW.
// The mesh and collision-element sizes match GC; virtual dispatch remains explicit GHS calls.
// Check/index arguments retain full registers rather than narrowing GC's u16/bool parameters.
#pragma once
#include "SSystem/SComponent/c_bg_w.h"
#include "bindings.h"
namespace background_world {
struct PolyInfoData {
    be<u32> m_info0, m_info1, m_info2, m_info3;
};
struct MeshData {
    /* 0x00 */ be<s32> m_v_num;
    /* 0x04 */ gptr<cXyz> m_v_tbl;
    /* 0x08 */ be<s32> m_t_num;
    /* 0x0C */ gptr<cBgD_Tri_t> m_t_tbl; /* cBgD_Tri_t, stride 0xA: vtx0, vtx1, vtx2, id, grp */
    /* 0x10 */ be<s32> m_b_num;
    /* 0x14 */ gptr<cBgD_Blk_t> m_b_tbl;
    /* 0x18 */ be<s32> m_tree_num;
    /* 0x1C */ gptr<cBgD_Tree_t> m_tree_tbl; /* cBgD_Tree_t, stride 0x14: flag, parent, child[8] / block */
    /* 0x20 */ be<s32> m_g_num;
    /* 0x24 */ gptr<cBgD_Grp_t>
        m_g_tbl; /* cBgD_Grp_t, stride 0x34: next sibling +0x26, first child +0x28, tree +0x2E, info +0x30 */
    /* 0x28 */ be<s32> m_ti_num;
    /* 0x2C */ gptr<PolyInfoData> m_ti_tbl; /* cBgD_Ti_t, stride 0x10: info0..info3 */
};
WWHD_OFFSET(MeshData, m_ti_tbl, 0x2C);
WWHD_SIZE(MeshData, 0x30);
WWHD_SIZE(PolyInfoData, 0x10);

struct HeightCheck;
struct SphereCheck;
class dBgW {
  public:
    enum { INVALID_INDEX = 0xFFFF, LEAF = 1, MOVE = 1 };
    enum PolyThrough {
        OBJ_THROUGH = 2,
        CAM_THROUGH = 1,
        LINK_THROUGH = 4,
        ARROW_THROUGH = 8,
        BOMB_THROUGH = 0x20,
        BOOMERANG_THROUGH = 0x40,
        ROPE_THROUGH = 0x80
    };
    enum GroupInfo {
        WATER_GROUP = 0x100,
        YOGAN_GROUP = 0x200,
        DOKU_GROUP = 0x400,
        LIGHT_GROUP = 0x80000,
        SPECIAL_GROUPS = WATER_GROUP | YOGAN_GROUP | DOKU_GROUP | LIGHT_GROUP
    };
    enum GroupPass { NORMAL_GRP = 1, WATER_GRP = 2, YOGAN_GRP = 4, DOKU_GRP = 8, LIGHT_GRP = 0x10 };

    void positionWallCorrect(dBgS_Acch *acch, f32 dist, cBgW_Plane *plane, cXyz *pupper_pos, f32 speed);
    bool RwgWallCorrect(dBgS_Acch *pwi, u32 i_poly_idx);
    bool WallCorrectRp(dBgS_Acch *acch, s32 i);
    bool WallCorrectGrpRp(dBgS_Acch *acch, s32 grp_id, s32 depth);
    bool RwgRoofChk(u32 poly_index, HeightCheck *check);
    bool RoofChkRp(HeightCheck *check, s32 i);
    bool RoofChkGrpRp(HeightCheck *check, s32 grp_id, s32 depth);
    bool RwgSplGrpChk(u32 poly_index, HeightCheck *check);
    bool SplGrpChkRp(HeightCheck *check, s32 i);
    bool SplGrpChkGrpRp(HeightCheck *check, s32 grp_id, s32 depth);
    bool RwgSphChk(u32 i_poly_idx, SphereCheck *check, u32 data);
    bool SphChkRp(SphereCheck *check, u32 user, s32 i);
    bool SphChkGrpRp(SphereCheck *check, u32 user, s32 grp_id, s32 depth);
    void Move();
    void ChangeAttributeCodeByPathPntNo(u32 pnt_no, u32 attr);
    void CrrPos(cBgS_PolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle, csXyz *shape_angle);
    void TransPos(cBgS_PolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle, csXyz *shape_angle);
    bool ChkPolyThrough(s32 poly_index, dBgS_PolyPassChk *chk);
    bool ChkGrpThrough(s32 grp_id, dBgS_GrpPassChk *chk, s32 depth);
    void MatrixCrrPos(cBgS_PolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle,
                      csXyz *shape_angle);
    /* 0x00 */ be<u32> m_id;
    /* 0x04 */ be<u32> __vtbl; /* ChkPolyThrough +0x2C, ChkGrpThrough +0x3C */
    /* 0x08 */ u8 baseCollisionState[0x80];
    /* 0x88 */ gptr<cBgW_TriElm> pm_tri;     /* cBgW_TriElm: plane at +0, stride 0x18 */
    /* 0x8C */ gptr<cBgW_RwgElm> pm_rwg;     /* cBgW_RwgElm: next at +0, stride 8 */
    /* 0x90 */ gptr<cXyz> pm_vtx_tbl;        /* stride 0xC */
    /* 0x94 */ gptr<MeshData> pm_bgd;        /* cBgD_t */
    /* 0x98 */ gptr<cBgW_BlkElm> pm_blk;     /* cBgW_BlkElm: roof +0, wall +2, ground +4, stride 6 */
    /* 0x9C */ gptr<cBgW_GrpElm> pm_grp;     /* cBgW_GrpElm: aab at +0, stride 0x20 */
    /* 0xA0 */ gptr<cBgW_NodeTree> m_nt_tbl; /* cBgW_NodeTree: aab at +0, stride 0x1C */
    /* 0xA4 */ be<s32> m_rootGrpIdx;
    /* 0xA8 */ be<u32> m_crr_func;
    /* 0xAC */ be<s16> mOldRotY;
    /* 0xAE */ be<s16> mRotYDelta;
    /* 0xB0 */ be<u32> mpRideCb;
    /* 0xB4 */ be<u32> mpPushPullCb;
    /* 0xB8 */ be<u16> mRoomId;
    /* 0xBA */ be<u8> mFlag;
    /* 0xBB */ be<u8> mGrpRoomInf;
};
WWHD_OFFSET(dBgW, pm_tri, 0x88);
WWHD_OFFSET(dBgW, m_crr_func, 0xA8);
WWHD_SIZE(dBgW, 0xBC);

static inline void cBgW_ct(dBgW *w) { gabi::call(0x020094A0, w); }
static inline bool cBgW_Set(dBgW *w, u32 bgd, u32 flag, u32 mtx) {
    return gabi::call<bool>(0x0200A030, w, bgd, flag, mtx);
}
static inline void cBgW_Move(dBgW *w) { gabi::call(0x02009C84, w); }
static inline u32 dBgS_GetRoomPathPntNo(u32 info2) { return gabi::call<u32>(0x024EE650, info2); }
static inline void dBgS_ChangeAttributeCode(u32 code, u32 dst) { gabi::call(0x024EE638, code, dst); }
static inline bool cM3d_Cross_AabCyl(u32 aab, u32 cyl) { return gabi::call<bool>(0x02010E14, aab, cyl); }
static inline bool cM3d_Cross_AabSph(u32 aab, u32 sph) { return gabi::call<bool>(0x020119B0, aab, sph); }
static inline bool cM3dGAab_CrossY(u32 aab, u32 pos) { return gabi::call<bool>(0x02017D3C, aab, pos); }
static inline bool cM3dGAab_UnderPlaneYUnder(u32 aab, f32 y) { return gabi::call<bool>(0x02017D84, aab, y); }
static inline bool cM3dGAab_TopPlaneYUnder(u32 aab, f32 y) { return gabi::call<bool>(0x02017D98, aab, y); }
static inline bool cM3dGPla_getCrossY(u32 pla, u32 pos, u32 y) {
    return gabi::call<bool>(0x02018AE4, pla, pos, y);
}
static inline bool cM3d_CrossY_Tri(u32 a, u32 b, u32 c, u32 pla, u32 pnt) {
    return gabi::call<bool>(0x020122A8, a, b, c, pla, pnt);
}
static inline void cM3dGTri_ct(u32 tri) { gabi::call(0x02019040, tri); }
static inline void cM3dGTri_setBg(u32 tri, u32 a, u32 b, u32 c, u32 pla) {
    gabi::call(0x020192AC, tri, a, b, c, pla);
}
static inline bool cM3d_Cross_SphTri(u32 sph, u32 tri, u32 out) {
    return gabi::call<bool>(0x02014200, sph, tri, out);
}
static inline f32 dBgS_Acch_GetWallAddY(dBgS_Acch *a, u32 vec, s32 i) {
    return gabi::call<f32>(0x024F1204, a, vec, i);
}
static inline void dBgS_Acch_SetWallCir(dBgS_Acch *a) { gabi::call(0x024F10A8, a); }
static inline void dBgS_Acch_CalcWallBmdCyl(dBgS_Acch *a) { gabi::call(0x024F1134, a); }
static inline void cM3dGLin_SetStartEnd(u32 lin, u32 start, u32 end) {
    gabi::call(0x02018808, lin, start, end);
}
static inline bool cM3d_Len2dSqPntAndSegLine(f32 px, f32 pz, f32 x0, f32 z0, f32 x1, f32 z1, u32 ox, u32 oz,
                                             u32 od) {
    return gabi::call<bool>(0x020109FC, px, pz, x0, z0, x1, z1, ox, oz, od);
}
static inline f32 cM3d_Len2dSq(f32 x0, f32 z0, f32 x1, f32 z1) {
    return gabi::call<f32>(0x020109E8, x0, z0, x1, z1);
}
static inline void cM2d_CrossCirLin(u32 cir, f32 x, f32 z, f32 dx, f32 dz, u32 ox, u32 oz) {
    gabi::call(0x02010284, cir, x, z, dx, dz, ox, oz);
}

struct HeightCheck {
    cBgS_PolyInfo mPolyInfo;
    cBgS_Chk mCheck;
    dBgS_Chk mPass;
    cXyz m_pos;
    be<f32> m_roof, m_height;
};
struct SphereCheck {
    u8 sphere[0x14];
    cBgS_PolyInfo mPolyInfo;
    cBgS_Chk mCheck;
    u8 passChecks[0x14];
    be<u32> mpCallback;
};
WWHD_SIZE(HeightCheck, 0x4C);
WWHD_OFFSET(SphereCheck, mpCallback, 0x4C);
struct WorldVtable {
    u8 prefix[0x2C];
    be<u32> ChkPolyThrough;
    u8 reserved30[12];
    be<u32> ChkGrpThrough;
    be<u32> reserved40, CrrPos;
};
template <class T> inline u32 tableAddress(const gptr<T> &table, u32 index) {
    return table.v + index * u32(sizeof(T));
}
// HD: every table index uses 32-bit guest arithmetic, including deliberately negative test indices.
inline cBgD_Tri_t *triangle(u32 table, u32 index) {
    return gabi::at<cBgD_Tri_t>(table + index * u32(sizeof(cBgD_Tri_t)));
}
inline cBgD_Grp_t *group(u32 table, u32 index) {
    return gabi::at<cBgD_Grp_t>(table + index * u32(sizeof(cBgD_Grp_t)));
}
inline PolyInfoData *polyInfo(u32 table, u32 index) {
    return gabi::at<PolyInfoData>(table + index * u32(sizeof(PolyInfoData)));
}
inline dBgS_AcchCir *circle(dBgS_Acch *acch, u32 offset) {
    return gabi::at<dBgS_AcchCir>(acch->pm_acch_cir.v + offset);
}
inline cBgW_BlkElm *block(u32 table, u32 index) {
    return gabi::at<cBgW_BlkElm>(table + index * u32(sizeof(cBgW_BlkElm)));
}
inline bool dispatchPoly(dBgW *w, u32 poly, u32 chk) {
    return gabi::call_ptr<bool>(gabi::at<WorldVtable>(w->__vtbl)->ChkPolyThrough, w, poly, chk);
}
inline bool dispatchGroup(dBgW *w, u32 grp, u32 chk, s32 depth) {
    return gabi::call_ptr<bool>(gabi::at<WorldVtable>(w->__vtbl)->ChkGrpThrough, w, grp, chk, depth);
}
inline void assertion(u32 file, s32 line, u32 message) { JUT_ASSERT_fail(STR(file), line, STR(message)); }
inline void dispatchCrrPos(dBgW *w, cBgS_PolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle,
                           csXyz *shapeAngle) {
    gabi::call_ptr(gabi::at<WorldVtable>(w->__vtbl)->CrrPos, w, poly, user, accept, pos, angle, shapeAngle);
}
inline bool cM3d_IsZero(f32 value) { return std::fabs(value) < 3.814697265625e-06f; }
inline u8 polyPassFlag(const dBgS_PolyPassChk *chk, u32 index) {
    switch (index) {
    case 0:
        return chk->mbObjThrough;
    case 1:
        return chk->mbCamThrough;
    case 2:
        return chk->mbLinkThrough;
    case 3:
        return chk->mbArrowThrough;
    case 4:
        return chk->mbBombThrough;
    case 5:
        return chk->mbBoomerangThrough;
    default:
        return chk->mbRopeThrough;
    }
}
inline void dispatchCorrection(dBgW *w, void *user, cBgS_PolyInfo *poly, u32 accept, cXyz *pos, csXyz *angle,
                               csXyz *shapeAngle) {
    gabi::call_ptr((u32)w->m_crr_func, w, user, poly, accept, pos, angle, shapeAngle);
}
// HD: per-TU GHS vtable, header-static and assertion string identities.
enum : u32 {
    wallCorrectionFile = 0x10043D44,
    wallCorrectionXCheck = 0x10043D50,
    wallCorrectionZCheck = 0x10043D9C,
    wallPolyIndexCheck = 0x10043DF4,
    wallNormalCheck = 0x10043E04,
    circleIndexCheck = 0x10043E2C,
    acchHeaderFile = 0x10043E40,
    backgroundIndexCheck = 0x10043E50,
    polyInfoHeaderFile = 0x10043E60,
    wallCollisionFile = 0x10043E74,
    wallPositionXCheck = 0x10043E80,
    wallPositionZCheck = 0x10043ED4,
    roofPolyIndexCheck = 0x10043F28,
    roofCollisionFile = 0x10043F38,
    specialPolyIndexCheck = 0x10043F4C,
    specialCollisionFile = 0x10043F5C,
    spherePolyIndexCheck = 0x10043F70,
    sphereCollisionFile = 0x10043F80,
    meshPolyIndexCheck = 0x10043FAC,
    meshHeaderFile = 0x10043FDC,
    meshInfoIndexCheck = 0x10043FE8,
    worldVtable = 0x10044018,
    headerDestructor = 0x101D5354,
    headerObject = 0x1046EDD0
};
} // namespace background_world
