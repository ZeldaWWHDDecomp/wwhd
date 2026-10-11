/** Background mesh geometry. */
#include "SSystem/SComponent/c_bg_w.h"
#include <cmath>
using namespace gabi;

void *operator_new(u32);
void *operator_new_array(u32);
void *constructArray(void *, u32, u32, void *);
void *JKRHeap_getCurrentHeap();
u32 JKRHeap_getHeapType(void *);
void OSReport_Error(const char *);
void JUT_ASSERT_fail(const char *, s32, const char *);
void initializeMatrixStorage(BgMtx *, u32);
void PSMTXIdentity(BgMtx *);
void PSMTXCopy(const BgMtx *, BgMtx *);
void PSVECAdd(const cXyz *, const cXyz *, cXyz *);
void PSMTXMultVec(const BgMtx *, const cXyz *, cXyz *);
f32 PSVECDotProduct(const cXyz *, const cXyz *);
f32 cM_rndF(f32);
void clearBounds(cBgW_NodeTree *);
void setBoundsMin(cBgW_NodeTree *, const cXyz *);
void setBoundsMax(cBgW_NodeTree *, const cXyz *);
void expandBounds(cBgW_NodeTree *, const cXyz *);
s32 boundsCrossY(const cBgW_NodeTree *, const cXyz *);
s32 boundsUnderY(const cBgW_NodeTree *, f32);
s32 boundsTopY(const cBgW_NodeTree *, f32);
void cM3d_CalcPla(const cXyz *, const cXyz *, const cXyz *, cXyz *, be<f32> *);
void constructTriangle(cBgW_Triangle *);
void setTriangleBg(cBgW_Triangle *, const cXyz *, const cXyz *, const cXyz *, const cBgW_Plane *);
s32 cM3d_Cross_LinTri(const cBgW_Line *, const cBgW_Triangle *, cXyz *, u32, u32);
void setLineEnd(cBgW_Line *, const cXyz *);
s32 cM3d_Cross_MinMaxBoxLine(const cXyz *, const cXyz *, const cXyz *, const cXyz *);
s32 cM3d_CrossY_Tri_Front(const cXyz *, const cXyz *, const cXyz *, const cXyz *);
void registerGlobalObject(void *);
void initializeHeaderByte(void *);
void initializeHeaderFlag(void *);

// HD: these call sites consume the complete r3 word of boolean entries.
s32 cBgW_SetVtxEntry(cBgWMesh *);
s32 cBgW_SetTriEntry(cBgWMesh *);
s32 cBgW_RwgLineCheckEntry(cBgWMesh *, s32, cBgW_LinChk *);
s32 cBgW_LineCheckRpEntry(cBgWMesh *, cBgW_LinChk *, u32);
s32 cBgW_LineCheckGrpRpEntry(cBgWMesh *, cBgW_LinChk *, u32, u32);
s32 cBgW_RwgGroundCheckCommonEntry(cBgWMesh *, s32, cBgW_GndChk *, f32);
s32 cBgW_RwgGroundCheckGndEntry(cBgWMesh *, u32, cBgW_GndChk *);
s32 cBgW_RwgGroundCheckWallEntry(cBgWMesh *, u32, cBgW_GndChk *);
s32 cBgW_GroundCrossRpEntry(cBgWMesh *, cBgW_GndChk *, u32);
s32 cBgW_GroundCrossGrpRpEntry(cBgWMesh *, cBgW_GndChk *, u32, u32);

static const f32 zero = 0.0f;
static const f32 boundsMin = 1000000000.0f;
static const f32 boundsMax = -1000000000.0f;
static const f32 boundsMargin = 1.0f;
static const f32 moveCounterRange = 128.0f;
static const f32 groundNormalY = 0.5f;
static const f32 roofNormalY = -0.8f;
static const f32 normalEpsilon = 0.000003814697265625f;
static const f32 wallNormalY = 0.014f;
static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
// HD: GHS constructor/vtable entry addresses, passed to the array constructor.
enum : u32 {
    rwgVtable = 0x100011EC,
    nodeVtable = 0x1000120C,
    boxVtable = 0x100011CC,
    baseVtable = 0x1000121C,
    bgVtable = 0x10001308,
    triConstructor = 0x0200B56C,
    rwgConstructor = 0x020093C4,
    nodeConstructor = 0x0200B5C0,
    groupConstructor = 0x0200940C
};

/* 02009380 */
void ASSERT_SOLDHEAP() {
    void *heap = JKRHeap_getCurrentHeap();
    u32 type = JKRHeap_getHeapType(heap);
    if (type != 0x534C4944)
        OSReport_Error(at<char>(0x10001000));
}

/* 020093C4 */
// HD: allocating GHS constructor; the GameCube inline constructor only clears next.
static cBgW_RwgElm *cBgW_RwgElm__ct(cBgW_RwgElm *object) {
    if (object == NULL)
        object = (cBgW_RwgElm *)operator_new(sizeof(cBgW_RwgElm));
    if (object != NULL) {
        object->next = 0;
        object->__vtbl = rwgVtable;
    }
    return object;
}

/* 0200940C */
static cBgW_GrpElm *cBgW_GrpElm__ct(cBgW_GrpElm *object) {
    if (object == NULL)
        object = (cBgW_GrpElm *)operator_new(sizeof(cBgW_GrpElm));
    if (object != NULL) {
        object->__vtbl = nodeVtable;
        object->aab.__vtbl = boxVtable;
    }
    return object;
}

/* 02009458 */
static cBgWMesh *cBgW_BgId__ct(cBgWMesh *object) {
    if (object == NULL)
        object = (cBgWMesh *)operator_new(8);
    if (object != NULL) {
        object->m_id = 0x100;
        object->__vtbl = at<cBgW_Vtable>(baseVtable);
    }
    return object;
}

/* 020094A0 */
// HD: explicit GHS allocation, matrix storage initialization and store ordering.
static cBgWMesh *cBgW__ct(cBgWMesh *object) {
    if (object == NULL)
        object = (cBgWMesh *)operator_new(sizeof(cBgWMesh));
    if (object == NULL)
        return NULL;
    cBgW_BgId__ct(object);
    object->pm_base = NULL;
    object->__vtbl = at<cBgW_Vtable>(bgVtable);
    initializeMatrixStorage(&object->mOldMtx, sizeof(BgMtx));
    initializeMatrixStorage(&object->mCurMtx, sizeof(BgMtx));
    object->pm_grp = NULL;
    object->pm_blk = NULL;
    object->m_rootGrpIdx = 0;
    f32 initial = zero;
    // HD: an otherwise unused word of the base padding is cleared.
    object->unusedHeaderWord = 0;
    object->mTransVel.x = initial;
    object->mTransVel.y = initial;
    object->pm_base = NULL;
    object->pm_rwg = NULL;
    object->mWallCorrectPriority = 0;
    object->pm_tri = NULL;
    object->pm_bgd = NULL;
    object->mIgnorePlaneType = 0;
    object->pm_vtx_tbl = NULL;
    object->mMoveCounter = 0;
    object->mbNeedsFullTransform = 1;
    object->m_nt_tbl = NULL;
    object->mFlags = cBgWMesh::GLOBAL_e;
    object->mTransVel.z = initial;
    PSMTXIdentity(&object->mOldMtx);
    PSMTXIdentity(&object->mCurMtx);
    object->mIgnorePlaneType = 0;
    object->mWallCorrectPriority = 2;
    object->m_rootGrpIdx = 0xFFFF;
    return object;
}

/* 0200959C */
void cBgWMesh::FreeArea() {
    pm_vtx_tbl = NULL;
    pm_rwg = NULL;
    pm_grp = NULL;
    pm_blk = NULL;
    m_nt_tbl = NULL;
    pm_tri = NULL;
}

/* 020095BC */
void cBgWMesh::GlobalVtx() {
    BgMtx *matrix = pm_base.get();
    if (matrix == NULL)
        return;
    bool full = mbNeedsFullTransform != 0;
    cBgD_Mesh *bg = pm_bgd.get();
    s32 count = bg->m_v_num;
    if (!full) {
        for (u32 i = 0; (s32)i < count; ++i) {
            cXyz *vertex = &pm_vtx_tbl[i];
            PSVECAdd(vertex, &mTransVel, vertex);
            bg = pm_bgd.get();
            count = bg->m_v_num;
        }
    } else {
        for (u32 i = 0; (s32)i < count; ++i) {
            cXyz *source = bg->m_v_tbl.get();
            cXyz *vertices = pm_vtx_tbl.get();
            PSMTXMultVec(matrix, &source[i], &vertices[i]);
            bg = pm_bgd.get();
            count = bg->m_v_num;
            if ((s32)(i + 1) < count)
                matrix = pm_base.get();
        }
    }
}

/* 020096B0 */
void cBgWMesh::CopyOldMtx() {
    if (pm_base) {
        PSMTXCopy(&mCurMtx, &mOldMtx);
        PSMTXCopy(pm_base.get(), &mCurMtx);
    }
}

/* 020096FC */
void cBgWMesh::MakeBlckTransMinMax(cXyz *min, cXyz *max) {
    PSVECAdd(min, &mTransVel, min);
    PSVECAdd(max, &mTransVel, max);
}

/* 02009750 */
void cBgWMesh::MakeBlckMinMax(s32 index, cXyz *min, cXyz *max) {
    cXyz *point = &pm_vtx_tbl[(u32)index];
    // HD: reload the point after each minimum store, preserving possible aliasing.
    f32 x = point->x;
    if ((f32)min->x > x) {
        min->x = x;
        x = point->x;
    }
    if ((f32)max->x < x)
        max->x = x;
    f32 y = point->y;
    if ((f32)min->y > y) {
        min->y = y;
        y = point->y;
    }
    if ((f32)max->y < y)
        max->y = y;
    f32 z = point->z;
    if ((f32)min->z > z) {
        min->z = z;
        z = point->z;
    }
    if ((f32)max->z < z)
        max->z = z;
}

/* 020097D8 */
void cBgWMesh::MakeBlckBnd(u32 block, cXyz *min, cXyz *max) {
    cBgD_Mesh *bg = pm_bgd.get();
    u32 num = bg->m_b_num;
    cBgD_Blk_t *entry = &bg->m_b_tbl[block];
    u32 start = entry->startTri;
    u8 full = mbNeedsFullTransform;
    s32 last = block == num - 1 ? (s32)((u32)bg->m_t_num - 1) : (s32)((u32)entry[1].startTri - 1);
    if (!full) {
        MakeBlckTransMinMax(min, max);
        return;
    }
    f32 low = boundsMin, high = boundsMax;
    min->x = low;
    min->z = low;
    min->y = low;
    max->z = high;
    max->x = high;
    max->y = high;
    for (u32 i = start; (s32)i <= last; ++i) {
        for (u32 corner = 0; corner < 3; ++corner) {
            cBgD_Tri_t *tri = &pm_bgd->m_t_tbl[i];
            // The verified code reloads mesh storage between corner calls.
            be<u16> *indices = &tri->vtx0;
            u32 vertex = indices[corner];
            MakeBlckMinMax(vertex, min, max);
        }
    }
    f32 margin = boundsMargin, lx = min->x, ly = min->y;
    lx = fsubs_ppc(lx, margin);
    f32 lz = min->z;
    ly = fsubs_ppc(ly, margin);
    lz = fsubs_ppc(lz, margin);
    min->x = lx;
    min->y = ly;
    min->z = lz;
    f32 hx = max->x, hy = max->y;
    hx = fadds_ppc(hx, margin);
    f32 hz = max->z;
    hy = fadds_ppc(hy, margin);
    hz = fadds_ppc(hz, margin);
    max->x = hx;
    max->y = hy;
    max->z = hz;
}

/* 02009930 */
void cBgWMesh::MakeNodeTreeRp(u32 index) {
    cBgD_Tree_t *tree = &pm_bgd->m_tree_tbl[index];
    if ((u16)tree->mFlag & 1) {
        u32 block = tree->mChild[0];
        if (block != 0xFFFF) {
            cBgW_NodeTree *box = &m_nt_tbl[index];
            MakeBlckBnd(block, &box->mMin, &box->mMax);
        }
        return;
    }
    clearBounds(&m_nt_tbl[index]);
    for (u32 child = 0; child < 8; ++child) {
        u32 next = tree->mChild[child];
        if (next == 0xFFFF)
            continue;
        MakeNodeTreeRp(next);
        cBgW_NodeTree *boxes = m_nt_tbl.get();
        expandBounds(&boxes[index], &boxes[next].mMin);
        boxes = m_nt_tbl.get();
        expandBounds(&boxes[index], &boxes[next].mMax);
    }
}

/* 02009A10 */
void cBgWMesh::MakeNodeTreeGrpRp(u32 index) {
    cBgD_Grp_t *group = &pm_bgd->m_g_tbl[index];
    u32 node = group->m_tree_idx;
    if (node != 0xFFFF) {
        MakeNodeTreeRp(node);
        group = &pm_bgd->m_g_tbl[index];
        node = group->m_tree_idx;
        cBgW_GrpElm *groups = pm_grp.get();
        cBgW_NodeTree *nodes = m_nt_tbl.get();
        setBoundsMin(&groups[index].aab, &nodes[node].mMin);
        group = &pm_bgd->m_g_tbl[index];
        node = group->m_tree_idx;
        nodes = m_nt_tbl.get();
        groups = pm_grp.get();
        setBoundsMax(&groups[index].aab, &nodes[node].mMax);
        group = &pm_bgd->m_g_tbl[index];
    }
    for (u32 child = group->m_first_child; child != 0xFFFF;) {
        MakeNodeTreeGrpRp(child);
        cBgW_GrpElm *groups = pm_grp.get();
        setBoundsMin(&groups[index].aab, &groups[child].aab.mMin);
        groups = pm_grp.get();
        setBoundsMax(&groups[index].aab, &groups[child].aab.mMax);
        group = &pm_bgd->m_g_tbl[child];
        child = group->m_next_sibling;
    }
}

/* 02009B38 */
void cBgWMesh::MakeNodeTree() {
    bool hasVertices = bool(pm_vtx_tbl);
    cBgD_Mesh *bg = pm_bgd.get();
    s32 count = bg->m_g_num;
    if (hasVertices) {
        for (u32 i = 0; (s32)i < count; ++i) {
            clearBounds(&pm_grp[i].aab);
            bg = pm_bgd.get();
            count = bg->m_g_num;
        }
    }
    if (count <= 0)
        return;
    cBgD_Grp_t *groups = bg->m_g_tbl.get();
    for (u32 i = 0; (s32)i < count; ++i) {
        if ((u16)groups[i].m_parent == 0xFFFF) {
            m_rootGrpIdx = i;
            if (hasVertices)
                MakeNodeTreeGrpRp(i);
            return;
        }
    }
}

/* 02009C84 */
void cBgWMesh::Move() {
    u8 flags = mFlags;
    if ((flags & LOCK_e) || !(flags & MOVE_BG_e))
        return;
    if (!(flags & NO_CALC_VTX_e)) {
        bool full = mMoveCounter == 255;
        BgMtx *matrix = NULL;
        if (!full) {
            matrix = pm_base.get();
            for (u32 row = 0; row < 3 && !full; ++row) {
                for (u32 col = 0; col < 3; ++col) {
                    if (!((f32)mCurMtx.rows[row][col] == (f32)matrix->rows[row][col])) {
                        full = true;
                        break;
                    }
                }
            }
        }
        if (full)
            mbNeedsFullTransform = 1;
        else {
            f32 oldX = mCurMtx.rows[0][3], newX = matrix->rows[0][3];
            bool same = oldX == newX;
            if (same) {
                same = (f32)mCurMtx.rows[1][3] == (f32)matrix->rows[1][3];
                if (same)
                    same = (f32)mCurMtx.rows[2][3] == (f32)matrix->rows[2][3];
            }
            if (same) {
                PSMTXCopy(matrix, &mOldMtx);
                if (!((u32)mIgnorePlaneType & FLUSH))
                    return;
            } else {
                f32 dx = fsubs_ppc(newX, oldX);
                matrix = pm_base.get();
                f32 oldY = mCurMtx.rows[1][3];
                mTransVel.x = dx;
                f32 newY = matrix->rows[1][3], dy = fsubs_ppc(newY, oldY),
                    oldZ = mCurMtx.rows[2][3];
                mTransVel.y = dy;
                f32 newZ = matrix->rows[2][3], dz = fsubs_ppc(newZ, oldZ);
                mbNeedsFullTransform = 0;
                mTransVel.z = dz;
            }
        }
        u8 counter = mMoveCounter;
        mMoveCounter = counter == 255 ? 0 : counter + 1;
        GlobalVtx();
    }
    CopyOldMtx();
    DispatchCalcPlane();
    DispatchClassifyPlane();
    MakeNodeTree();
}

/* 02009E58 */
u8 cBgWMesh::ChkMemoryError() {
    return !pm_tri || !pm_rwg || !pm_blk || !m_nt_tbl || !pm_grp;
}

/* 02009EA4 */
u8 cBgWMesh::SetVtx() {
    ASSERT_SOLDHEAP();
    u8 flags = mFlags;
    if (flags & NO_VTX_TBL_e) {
        pm_vtx_tbl = NULL;
        return 0;
    }
    cBgD_Mesh *bg = pm_bgd.get();
    if (!(flags & MOVE_BG_e)) {
        pm_vtx_tbl = bg->m_v_tbl.get();
        return 0;
    }
    cXyz *vertices = (cXyz *)operator_new_array((u32)bg->m_v_num * sizeof(cXyz));
    pm_vtx_tbl = vertices;
    if (vertices == NULL)
        return 1;
    if (mFlags & UNK40_e) {
        s32 count = pm_bgd->m_v_num;
        f32 initial = zero;
        for (u32 i = 0; (s32)i < count; ++i) {
            vertices[i].x = initial;
            vertices = pm_vtx_tbl.get();
            vertices[i].y = initial;
            vertices = pm_vtx_tbl.get();
            vertices[i].z = initial;
            count = pm_bgd->m_v_num;
            if ((s32)(i + 1) < count)
                vertices = pm_vtx_tbl.get();
        }
    }
    GlobalVtx();
    return 0;
}

/* 02009FAC */
u8 cBgWMesh::SetTri() {
    ASSERT_SOLDHEAP();
    cBgD_Mesh *bg = pm_bgd.get();
    cBgW_TriElm *triangles = (cBgW_TriElm *)constructArray(
        NULL, (u32)bg->m_t_num, sizeof(cBgW_TriElm), at<void>(triConstructor));
    pm_tri = triangles;
    if (triangles == NULL)
        return 1;
    DispatchCalcPlane();
    return 0;
}

/* 0200A030 */
u8 cBgWMesh::Set(cBgD_Mesh *bg, u32 flags, BgMtx *matrix) {
    ASSERT_SOLDHEAP();
    f32 range = moveCounterRange;
    mFlags = GLOBAL_e;
    pm_vtx_tbl = NULL;
    pm_tri = NULL;
    pm_rwg = NULL;
    pm_blk = NULL;
    m_nt_tbl = NULL;
    pm_grp = NULL;
    f32 random = cM_rndF(range);
    mMoveCounter = (u32)ftoi(random);
    if (bg == NULL)
        return 1;
    mFlags = flags;
    if (flags & GLOBAL_e) {
        pm_base = NULL;
        PSMTXIdentity(&mOldMtx);
        PSMTXIdentity(&mCurMtx);
    } else {
        pm_base = matrix;
        PSMTXCopy(matrix, &mOldMtx);
        PSMTXCopy(pm_base.get(), &mCurMtx);
    }
    pm_bgd = bg;
    bool error = cBgW_SetVtxEntry(this) != 0;
    if (!error)
        error = cBgW_SetTriEntry(this) != 0;
    if (!error) {
        u32 count = pm_bgd->m_t_num;
        cBgW_RwgElm *storage = (cBgW_RwgElm *)constructArray(NULL, count, sizeof(cBgW_RwgElm),
                                                             at<void>(rwgConstructor));
        pm_rwg = storage;
        error = storage == NULL;
    }
    if (!error) {
        u32 count = pm_bgd->m_b_num;
        cBgW_BlkElm *storage = (cBgW_BlkElm *)operator_new_array(count * sizeof(cBgW_BlkElm));
        pm_blk = storage;
        error = storage == NULL;
    }
    if (!error) {
        u32 count = pm_bgd->m_tree_num;
        cBgW_NodeTree *storage = (cBgW_NodeTree *)constructArray(NULL, count, sizeof(cBgW_NodeTree),
                                                                 at<void>(nodeConstructor));
        m_nt_tbl = storage;
        error = storage == NULL;
    }
    if (!error) {
        u32 count = pm_bgd->m_g_num;
        cBgW_GrpElm *storage = (cBgW_GrpElm *)constructArray(NULL, count, sizeof(cBgW_GrpElm),
                                                             at<void>(groupConstructor));
        pm_grp = storage;
        error = storage == NULL;
    }
    if (error) {
        FreeArea();
        return 1;
    }
    DispatchClassifyPlane();
    mbNeedsFullTransform = 1;
    MakeNodeTree();
    return 0;
}

/* 0200A220 */
void cBgWMesh::GetTriPnt(u32 index, cXyz *p0, cXyz *p1, cXyz *p2) {
    cBgD_Mesh *bg = pm_bgd.get();
    if (bg == NULL) {
        JUT_ASSERT_fail(at<char>(0x10001258), 0xCE0, at<char>(0x10001248));
        bg = pm_bgd.get();
    }
    cBgD_Tri_t *tri = &bg->m_t_tbl[index];
    u32 i0 = tri->vtx0;
    cXyz *vertices = pm_vtx_tbl.get();
    p0->copy(vertices[i0]);
    u32 i1 = tri->vtx1;
    vertices = pm_vtx_tbl.get();
    p1->copy(vertices[i1]);
    u32 i2 = tri->vtx2;
    vertices = pm_vtx_tbl.get();
    p2->copy(vertices[i2]);
}

/* 0200A300 */
u32 cBgWMesh::GetGrpToRoomIndex(s32 index) {
    cBgD_Mesh *bg = NULL;
    if (index < 0) {
        JUT_ASSERT_fail(at<char>(0x10001294), 0xCA4, at<char>(0x10001264));
        bg = pm_bgd.get();
    } else {
        bg = pm_bgd.get();
        if (index >= (s32)bg->m_g_num) {
            JUT_ASSERT_fail(at<char>(0x10001294), 0xCA4, at<char>(0x10001264));
            bg = pm_bgd.get();
        }
    }
    cBgD_Grp_t *groups = bg->m_g_tbl.get();
    u32 parent = groups[(u32)index].m_parent;
    if (parent == 0xFFFF)
        return 0xFFFF;
    parent = groups[parent].m_parent;
    if (parent == 0xFFFF)
        return 0xFFFF;
    return groups[parent].m_room_id;
}

/* 0200A3BC */
void cBgWMesh::GetTrans(cXyz *dst) {
    BgMtx *matrix = pm_base.get();
    f32 old = mOldMtx.rows[0][3], now = matrix->rows[0][3];
    dst->x = fsubs_ppc(now, old);
    matrix = pm_base.get();
    now = matrix->rows[1][3];
    old = mOldMtx.rows[1][3];
    dst->y = fsubs_ppc(now, old);
    matrix = pm_base.get();
    now = matrix->rows[2][3];
    old = mOldMtx.rows[2][3];
    dst->z = fsubs_ppc(now, old);
}

/* 0200A3FC */
void cBgWMesh::GetTopUnder(be<f32> *top, be<f32> *under) {
    u32 index = m_rootGrpIdx;
    cBgW_GrpElm *groups = pm_grp.get();
    *under = groups[index].aab.mMin.y;
    index = m_rootGrpIdx;
    groups = pm_grp.get();
    *top = groups[index].aab.mMax.y;
}

/* 0200A430 */
void cBgWMesh::CalcPlane() {
    cXyz *vertices = pm_vtx_tbl.get();
    cBgD_Mesh *bg = pm_bgd.get();
    cBgD_Tri_t *tris = bg->m_t_tbl.get();
    if (vertices == NULL)
        return;
    bool full = mbNeedsFullTransform != 0;
    s32 count = bg->m_t_num;
    if (!full) {
        for (u32 i = 0; (s32)i < count; ++i) {
            cBgW_Plane *plane = &pm_tri[i].m_plane;
            f32 dot = PSVECDotProduct(&plane->mNormal, &mTransVel);
            plane->mD = fsubs_ppc(plane->mD, dot);
            count = pm_bgd->m_t_num;
        }
    } else {
        for (u32 i = 0; (s32)i < count; ++i) {
            cBgD_Tri_t *tri = &tris[i];
            u32 i0 = tri->vtx0;
            cBgW_TriElm *planes = pm_tri.get();
            u32 i1 = tri->vtx1, i2 = tri->vtx2;
            cBgW_Plane *plane = &planes[i].m_plane;
            cM3d_CalcPla(&vertices[i0], &vertices[i1], &vertices[i2], &plane->mNormal, &plane->mD);
            count = pm_bgd->m_t_num;
            if ((s32)(i + 1) < count)
                vertices = pm_vtx_tbl.get();
        }
    }
}

/* 0200A564 */
void cBgWMesh::BlckConnect(be<u16> *head, be<s32> *previous, u32 index) {
    if ((u16)*head == 0xFFFF)
        *head = index;
    u32 prev = *previous;
    if (prev != 0xFFFF)
        pm_rwg[prev].next = index;
    *previous = index;
    pm_rwg[index].next = 0xFFFF;
}

/* 0200A5A8 */
void cBgWMesh::ClassifyPlane() {
    if (!pm_vtx_tbl)
        return;
    cBgD_Mesh *bg = pm_bgd.get();
    s32 count = bg->m_b_num;
    if (count <= 0)
        return;
    f32 roof = roofNormalY, ground = groundNormalY, epsilon = normalEpsilon;
    for (u32 block = 0; (s32)block < count; ++block) {
        cBgD_Blk_t *entry = &bg->m_b_tbl[block];
        u32 start = entry->startTri;
        s32 last = block == (u32)count - 1 ? (s32)((u32)bg->m_t_num - 1)
                                           : (s32)((u32)entry[1].startTri - 1);
        pm_blk[block].roof = 0xFFFF;
        pm_blk[block].wall = 0xFFFF;
        pm_blk[block].ground = 0xFFFF;
        Stack<be<s32>> prevRoof, prevWall, prevGround;
        *prevGround = 0xFFFF;
        *prevRoof = 0xFFFF;
        *prevWall = 0xFFFF;
        for (u32 i = start; (s32)i <= last; ++i) {
            cBgW_Plane *plane = &pm_tri[i].m_plane;
            f32 nx = plane->mNormal.x, ny = plane->mNormal.y;
            bool tiny = std::fabs(nx) < epsilon;
            if (tiny) {
                tiny = std::fabs((f32)plane->mNormal.y) < epsilon;
                if (tiny)
                    tiny = std::fabs((f32)plane->mNormal.z) < epsilon;
            }
            if (tiny)
                continue;
            u32 ignore = mIgnorePlaneType;
            be<u16> *head;
            be<s32> *previous;
            if (!(ny < ground)) {
                if (ignore & IGNORE_GROUND)
                    continue;
                previous = prevGround.get();
                head = &pm_blk[block].ground;
            } else if (ny < roof) {
                if (ignore & IGNORE_ROOF)
                    continue;
                previous = prevRoof.get();
                head = &pm_blk[block].roof;
            } else {
                if (ignore & IGNORE_WALL)
                    continue;
                previous = prevWall.get();
                head = &pm_blk[block].wall;
            }
            BlckConnect(head, previous, i);
        }
        bg = pm_bgd.get();
        count = bg->m_b_num;
    }
}

/* 0200A78C */
u8 cBgWMesh::RwgLineCheck(s32 index, cBgW_LinChk *chk) {
    Stack<cBgW_Triangle> triangle;
    Stack<cXyz> point;
    constructTriangle(triangle.get());
    u32 found = 0;
    do {
        cBgD_Tri_t *tri = &pm_bgd->m_t_tbl[(u32)index];
        u32 i1 = tri->vtx1, i0 = tri->vtx0, i2 = tri->vtx2;
        cBgW_TriElm *planes = pm_tri.get();
        cXyz *vertices = pm_vtx_tbl.get();
        setTriangleBg(triangle.get(), &vertices[i0], &vertices[i1], &vertices[i2],
                      &planes[(u32)index].m_plane);
        u32 front = chk->mFrontFlag, back = chk->mBackFlag;
        s32 hit = cM3d_Cross_LinTri(&chk->mLin, triangle.get(), point.get(), front, back);
        if (hit) {
            void *pass = chk->mPolyPassChk.get();
            s32 through = DispatchPolyThrough(index, pass);
            if (!through) {
                setLineEnd(&chk->mLin, point.get());
                if (index < 0)
                    JUT_ASSERT_fail(at<char>(0x100012B8), 0x7B, at<char>(0x100012A8));
                chk->mPolyIndex = (u32)index;
                found = 1;
            }
        }
        index = pm_rwg[(u32)index].next;
    } while (index != 0xFFFF);
    return (u8)found;
}

/* 0200A8BC */
u8 cBgWMesh::LineCheckRp(cBgW_LinChk *chk, u32 index) {
    cBgW_NodeTree *box = &m_nt_tbl[index];
    if (!cM3d_Cross_MinMaxBoxLine(&box->mMin, &box->mMax, &chk->mLin.mStart, &chk->mLin.mEnd))
        return 0;
    cBgD_Tree_t *tree = &pm_bgd->m_tree_tbl[index];
    u32 found = 0;
    if ((u16)tree->mFlag & 1) {
        for (u32 kind = 0; kind < 3; ++kind) {
            u8 enabled = kind == 0   ? chk->mPreWallChk
                         : kind == 1 ? chk->mPreGroundChk
                                     : chk->mPreRoofChk;
            if (!enabled)
                continue;
            u32 block = tree->mChild[0];
            be<u16> *head = kind == 0   ? &pm_blk[block].wall
                            : kind == 1 ? &pm_blk[block].ground
                                        : &pm_blk[block].roof;
            u32 next = *head;
            if (next != 0xFFFF && cBgW_RwgLineCheckEntry(this, next, chk))
                found = 1;
        }
    } else {
        for (u32 child = 0; child < 8; ++child) {
            u32 next = tree->mChild[child];
            if (next != 0xFFFF && cBgW_LineCheckRpEntry(this, chk, next))
                found = 1;
        }
    }
    return (u8)found;
}

/* 0200AB4C */
u8 cBgWMesh::LineCheckGrpRp(cBgW_LinChk *chk, u32 index, u32 depth) {
    void *pass = chk->mGrpPassChk.get();
    if (DispatchGrpThrough(index, pass, depth))
        return 0;
    cBgW_NodeTree *box = &pm_grp[index].aab;
    if (!cM3d_Cross_MinMaxBoxLine(&box->mMin, &box->mMax, &chk->mLin.mStart, &chk->mLin.mEnd))
        return 0;
    cBgD_Grp_t *group = &pm_bgd->m_g_tbl[index];
    u32 node = group->m_tree_idx, found = 0;
    if (node != 0xFFFF) {
        if (cBgW_LineCheckRpEntry(this, chk, node))
            found = 1;
        group = &pm_bgd->m_g_tbl[index];
    }
    ++depth;
    for (u32 child = group->m_first_child; child != 0xFFFF;) {
        if (cBgW_LineCheckGrpRpEntry(this, chk, child, depth))
            found = 1;
        group = &pm_bgd->m_g_tbl[child];
        child = group->m_next_sibling;
    }
    return (u8)found;
}

/* 0200AC70 */
// HD: parameter order is index, check, height; comparisons retain unordered behavior.
u8 cBgWMesh::RwgGroundCheckCommon(s32 index, cBgW_GndChk *chk, f32 height) {
    if (!(height < (f32)chk->m_pos.y) || !(height > (f32)chk->mNowY))
        return 0;
    cBgD_Tri_t *tri = &pm_bgd->m_t_tbl[(u32)index];
    cXyz *vertices = pm_vtx_tbl.get();
    if (!cM3d_CrossY_Tri_Front(&vertices[(u16)tri->vtx0], &vertices[(u16)tri->vtx1],
                               &vertices[(u16)tri->vtx2], &chk->m_pos))
        return 0;
    if (DispatchPolyThrough(index, chk->mPolyPassChk.get()))
        return 0;
    chk->mNowY = height;
    if (index < 0)
        JUT_ASSERT_fail(at<char>(0x100012DC), 0x7B, at<char>(0x100012CC));
    chk->mPolyIndex = index;
    return 1;
}

static u8 groundList(cBgWMesh *mesh, u32 index, cBgW_GndChk *chk, bool wall) {
    u32 found = 0;
    f32 threshold = wallNormalY;
    do {
        cBgW_Plane *plane = &mesh->pm_tri[index].m_plane;
        cBgW_RwgElm *link = &mesh->pm_rwg[index];
        f32 ny = plane->mNormal.y;
        if (!wall || !(ny < threshold)) {
            // HD: preserve the PPC multiply/subtract evaluation and double intermediate.
            f32 term = -fmuls_ppc(plane->mNormal.x, chk->m_pos.x);
            f32 sum = (f32)(-((f64)(f32)plane->mNormal.z * (f64)(f32)chk->m_pos.z - (f64)term));
            f32 height = fsubs_ppc(sum, plane->mD) / ny;
            if (cBgW_RwgGroundCheckCommonEntry(mesh, index, chk, height))
                found = 1;
        }
        index = link->next;
    } while (index != 0xFFFF);
    return found;
}

/* 0200ADA4 */
u8 cBgWMesh::RwgGroundCheckGnd(u32 index, cBgW_GndChk *chk) {
    return groundList(this, index, chk, false);
}
/* 0200AE54 */
u8 cBgWMesh::RwgGroundCheckWall(u32 index, cBgW_GndChk *chk) {
    return groundList(this, index, chk, true);
}

static bool groundBox(cBgW_NodeTree *box, cBgW_GndChk *chk) {
    if (!boundsCrossY(box, &chk->m_pos))
        return false;
    if (!boundsUnderY(box, chk->m_pos.y))
        return false;
    return !boundsTopY(box, chk->mNowY);
}

/* 0200AF30 */
u8 cBgWMesh::GroundCrossRp(cBgW_GndChk *chk, u32 index) {
    cBgD_Tree_t *tree = &pm_bgd->m_tree_tbl[index];
    u32 found = 0;
    if ((u16)tree->mFlag & 1) {
        if ((u32)chk->mGndPrecheck) {
            u32 head = pm_blk[(u16)tree->mChild[0]].ground;
            if (head != 0xFFFF && cBgW_RwgGroundCheckGndEntry(this, head, chk))
                found = 1;
        }
        if ((u32)chk->mWallPrecheck) {
            u32 head = pm_blk[(u16)tree->mChild[0]].wall;
            if (head != 0xFFFF && cBgW_RwgGroundCheckWallEntry(this, head, chk))
                found = 1;
        }
    } else {
        // GameCube's deliberately ordered octree traversal: 2,3,6,7,0,1,4,5.
        const u32 childOrder[8] = {2, 3, 6, 7, 0, 1, 4, 5};
        for (u32 child : childOrder) {
            u32 next = tree->mChild[child];
            if (next == 0xFFFF)
                continue;
            cBgW_NodeTree *box = &m_nt_tbl[next];
            if (groundBox(box, chk) && cBgW_GroundCrossRpEntry(this, chk, tree->mChild[child]))
                found = 1;
        }
    }
    return found;
}

/* 0200B380 */
u8 cBgWMesh::GroundCrossGrpRp(cBgW_GndChk *chk, u32 index, u32 depth) {
    if (DispatchGrpThrough(index, chk->mGrpPassChk.get(), depth))
        return 0;
    if (!groundBox(&pm_grp[index].aab, chk))
        return 0;
    cBgD_Grp_t *group = &pm_bgd->m_g_tbl[index];
    u32 node = group->m_tree_idx, found = 0;
    if (node != 0xFFFF) {
        if (cBgW_GroundCrossRpEntry(this, chk, node))
            found = 1;
        group = &pm_bgd->m_g_tbl[index];
    }
    ++depth;
    for (u32 child = group->m_first_child; child != 0xFFFF;) {
        if (cBgW_GroundCrossGrpRpEntry(this, chk, child, depth))
            found = 1;
        child = pm_bgd->m_g_tbl[child].m_next_sibling;
    }
    return found;
}

/* 0200B4C8 */
// HD: the base virtual implementation has no live arguments.
static u8 cBgW_ChkPolyThrough() {
    return 0;
}
/* 0200B4D0 */
static u8 cBgW_ChkGrpThrough() {
    return 0;
}

struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject;
    u8 padding[2];
};
struct HeaderZeroObject {
    be<u32> words[4];
};
#define headerStatics (*at<HeaderStatics>(0x101FF4CC))
#define headerZeroObject (*at<HeaderZeroObject>(0x101FF4D8))
/* 0200B4D8 */
static void __sinit_c_bg_w_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(at<void>(0x1018C66C));
    headerStatics.piMin = headerPiMin;
    headerStatics.piMax = headerPiMax;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(at<void>(0x1018C678));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(at<void>(0x1018C684));
}

/* WWHD glue (tools/verify/autoglue.py)
data zero 1000122C
data boundsMin 10001230
data boundsMax 10001234
data boundsMargin 10001238
data moveCounterRange 1000123C
data groundNormalY 100012A0
data roofNormalY 100012A4
data normalEpsilon 100030B8
data wallNormalY 100012F0
data headerPiMin 10001300
data headerPiMax 10001304
ext 02009EA4 s32 cBgW_SetVtxEntry(cBgWMesh*)
ext 02009FAC s32 cBgW_SetTriEntry(cBgWMesh*)
ext 0200A78C s32 cBgW_RwgLineCheckEntry(cBgWMesh*,s32,cBgW_LinChk*)
ext 0200A8BC s32 cBgW_LineCheckRpEntry(cBgWMesh*,cBgW_LinChk*,u32)
ext 0200AB4C s32 cBgW_LineCheckGrpRpEntry(cBgWMesh*,cBgW_LinChk*,u32,u32)
ext 0200AC70 s32 cBgW_RwgGroundCheckCommonEntry(cBgWMesh*,s32,cBgW_GndChk*,f32)
ext 0200ADA4 s32 cBgW_RwgGroundCheckGndEntry(cBgWMesh*,u32,cBgW_GndChk*)
ext 0200AE54 s32 cBgW_RwgGroundCheckWallEntry(cBgWMesh*,u32,cBgW_GndChk*)
ext 0200AF30 s32 cBgW_GroundCrossRpEntry(cBgWMesh*,cBgW_GndChk*,u32)
ext 0200B380 s32 cBgW_GroundCrossGrpRpEntry(cBgWMesh*,cBgW_GndChk*,u32,u32)
ext 0273AD10 void* operator_new(u32)
ext 0273ADAC void* operator_new_array(u32)
ext 028EFFD0 void* constructArray(void*,u32,u32,void*)
ext 027EC230 void* JKRHeap_getCurrentHeap()
ext 027EC23C u32 JKRHeap_getHeapType(void*)
ext 025F2710 void OSReport_Error(const char*)
ext 0273AA24 void JUT_ASSERT_fail(const char*,s32,const char*)
ext 028F521C void initializeMatrixStorage(BgMtx*,u32)
ext 028E9098 void PSMTXIdentity(BgMtx*)
ext 028E90D4 void PSMTXCopy(const BgMtx*,BgMtx*)
ext 028E8D88 void PSVECAdd(const cXyz*,const cXyz*,cXyz*)
ext 028E8F64 void PSMTXMultVec(const BgMtx*,const cXyz*,cXyz*)
ext 028E8F44 f32 PSVECDotProduct(const cXyz*,const cXyz*)
ext 020198D8 f32 cM_rndF(f32)
ext 02017DAC void clearBounds(cBgW_NodeTree*)
ext 02017DF4 void setBoundsMin(cBgW_NodeTree*,const cXyz*)
ext 02017E34 void setBoundsMax(cBgW_NodeTree*,const cXyz*)
ext 02017E74 void expandBounds(cBgW_NodeTree*,const cXyz*)
ext 02017D3C s32 boundsCrossY(const cBgW_NodeTree*,const cXyz*)
ext 02017D84 s32 boundsUnderY(const cBgW_NodeTree*,f32)
ext 02017D98 s32 boundsTopY(const cBgW_NodeTree*,f32)
ext 02010D18 void cM3d_CalcPla(const cXyz*,const cXyz*,const cXyz*,cXyz*,be<f32>*)
ext 02019040 void constructTriangle(cBgW_Triangle*)
ext 020192AC void setTriangleBg(cBgW_Triangle*,const cXyz*,const cXyz*,const cXyz*,const cBgW_Plane*)
ext 02012AE8 s32 cM3d_Cross_LinTri(const cBgW_Line*,const cBgW_Triangle*,cXyz*,u32,u32)
ext 02018870 void setLineEnd(cBgW_Line*,const cXyz*)
ext 02010FFC s32 cM3d_Cross_MinMaxBoxLine(const cXyz*,const cXyz*,const cXyz*,const cXyz*)
ext 02012504 s32 cM3d_CrossY_Tri_Front(const cXyz*,const cXyz*,const cXyz*,const cXyz*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
