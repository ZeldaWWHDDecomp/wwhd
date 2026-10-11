/* Height-field collision planes and spatial bounds. Verified HD behavior. */
#include "d/d_bg_w_hf.h"
#include <cmath>
static const f32 positiveInfinity = 1000000000.0f;
static const f32 negativeInfinity = -1000000000.0f;
static const f32 boundsMargin = 1.0f;
static const f32 initialNormalY = 0.0f;
static const f32 piMin = -3.1415927410125732f;
static const f32 piMax = 3.1415927410125732f;
static const f32 normalEpsilon = 0.000003814697265625f;
enum : u32 { heightFieldVtable = 0x10044560 };

/* 024F5870 */
bool dBgWHf::Set(HfData *bgd, be<u16> *map, f32 normal, s32 nx, s32 nz, u32 flag) {
    mC8 = map;
    m_gridx = nx;
    m_gridz = nz;
    mC4 = normal;
    if (SetBaseWorld(this, bgd, MOVE_BG_e | NO_CALC_VTX_e | NO_VTX_TBL_e | GLOBAL_e, NULL))
        return true;
    mSvFlag = flag;
    if (flag & NO_CRR_POS)
        return false;
    u32 size = (u32)pm_bgd->m_v_num * sizeof(cXyz);
    cXyz *vertices = (cXyz *)operator_new_array(size);
    mBackVtx = vertices;
    return vertices == NULL;
}

/* 024F4B0C */
void dBgWHf::CalcPlane() {
    if (pm_vtx_tbl.get() == NULL)
        return;
    u32 nx = m_gridx, nz = m_gridz;
    HfData *bg = pm_bgd.get();
    cBgD_Tri_t *tri = bg->m_t_tbl.get();
    if (nx * nz * 2 != (u32)bg->m_t_num) {
        assertion("d_bg_w_hf.cpp", 0x61, "m_gridx * m_gridz * 2 == pm_bgd->m_t_num");
        nz = m_gridz;
    }
    s32 rows = nz * 2;
    u32 index = 0;
    cXyz normal;
    // HD: preserve PPC float subtraction and table/count reloads across plane callbacks.
    for (s32 z = 0; z < rows; ++z) {
        s32 count = m_gridx;
        for (s32 x = 0; x < count; ++x) {
            u32 slot = mC8.get()[index];
            cBgD_Tri_t *t = &tri[slot];
            u16 v0 = t->vtx0, v1 = t->vtx1;
            f32 ny = mC4;
            cXyz *vertices = pm_vtx_tbl.get();
            u16 v2 = t->vtx2;
            cXyz *a = &vertices[v0], *b = &vertices[v1], *c = &vertices[v2];
            f32 ay = a->y, by = b->y;
            f32 dx = (z & 1) ? gabi::fsubs_ppc(ay, by) : gabi::fsubs_ppc(by, ay);
            normal.y = ny;
            normal.x = dx;
            f32 cy = c->y;
            by = b->y;
            cBgW_TriElm *plane = &pm_tri.get()[slot];
            normal.z = (z & 1) ? gabi::fsubs_ppc(cy, by) : gabi::fsubs_ppc(by, cy);
            SetupPlane(plane, &normal, a);
            ++index;
            count = m_gridx;
            rows = (u32)m_gridz * 2;
        }
    }
}

/* 024F4D04 */
void dBgWHf::ClassifyPlane() {
    if (pm_vtx_tbl.get() == NULL)
        return;
    HfData *bg = pm_bgd.get();
    s32 count = bg->m_b_num;
    // HD: this address-taken scalar occupies a guest stack slot.
    HfPrevious previous;
    for (s32 b = 0; b < count; ++b) {
        cBgD_Blk_t *table = bg->m_b_tbl.get();
        cBgW_BlkElm *blocks = pm_blk.get();
        s32 t = table[b].startTri;
        s32 last = (u32)b == (u32)(count - 1) ? (s32)((u32)bg->m_t_num - 1)
                                              : (s32)table[b + 1].startTri - 1;
        blocks[b].roof = INVALID_INDEX;
        blocks = pm_blk.get();
        blocks[b].wall = INVALID_INDEX;
        blocks = pm_blk.get();
        blocks[b].ground = INVALID_INDEX;
        previous.index = INVALID_INDEX;
        for (; t <= last; ++t) {
            if (t < 0 || t >= (s32)pm_bgd->m_t_num)
                assertion("d_bg_w_hf.cpp", 0xAC, "0 <= t && t < pm_bgd->m_t_num");
            BlckConnect(this, &pm_blk.get()[b].ground, &previous.index, t);
        }
        bg = pm_bgd.get();
        count = bg->m_b_num;
    }
}

/* 024F5910 */
void dBgWHf::MoveHf() {
    u8 flag = mFlag, lock = mFlags;
    mFlag = flag | MOVE;
    if (lock & LOCK)
        return;
    // HD: retain both virtual calls and the vtable reload between them.
    HfVtable *vtable = __vtbl.get();
    gabi::call_ptr<void>(vtable->CalcPlane, this);
    vtable = __vtbl.get();
    gabi::call_ptr<void>(vtable->ClassifyPlane, this);
    MakeNodeTreeHf();
}

/* 024F4E78 */
void dBgWHf::MakeBlckMinMaxHf(s32 index, be<f32> *minimum, be<f32> *maximum) {
    if (index < 0 || index >= (s32)pm_bgd->m_v_num)
        assertion("d_bg_w_hf.cpp", 0xD3, "0 <= v_index && v_index < pm_bgd->m_v_num");
    cXyz *vertex = &pm_vtx_tbl.get()[index];
    f32 lo = *minimum, value = vertex->y;
    if (lo > value) {
        *minimum = value;
        value = vertex->y;
    }
    if ((f32)*maximum < value)
        *maximum = value;
}

/* 024F4F24 */
void dBgWHf::MakeBlckBndHf(s32 block, be<f32> *minimum, be<f32> *maximum) {
    HfData *bg = pm_bgd.get();
    u32 count = bg->m_b_num;
    if (block < 0 || block >= (s32)count) {
        assertion("d_bg_w_hf.cpp", 0xEA, "blck_id >= 0 && blck_id < pm_bgd->m_b_num");
        bg = pm_bgd.get();
        count = bg->m_b_num;
    }
    cBgD_Blk_t *table = bg->m_b_tbl.get();
    s32 first = table[block].startTri;
    s32 last =
        (u32)block == count - 1 ? (s32)((u32)bg->m_t_num - 1) : (s32)table[block + 1].startTri - 1;
    f32 lo = positiveInfinity, hi = negativeInfinity;
    *minimum = lo;
    *maximum = hi;
    for (s32 t = first; t <= last; ++t) {
        bg = pm_bgd.get();
        if (t < 0 || t >= (s32)bg->m_t_num) {
            assertion("d_bg_w_hf.cpp", 0xFB, "0 <= t && t < pm_bgd->m_t_num");
            bg = pm_bgd.get();
        }
        cBgD_Tri_t *tri = bg->m_t_tbl.get();
        u16 v = tri[t].vtx0;
        MakeBlckMinMaxHf(v, minimum, maximum);
        tri = pm_bgd->m_t_tbl.get();
        v = tri[t].vtx1;
        MakeBlckMinMaxHf(v, minimum, maximum);
        tri = pm_bgd->m_t_tbl.get();
        v = tri[t].vtx2;
        MakeBlckMinMaxHf(v, minimum, maximum);
    }
    f32 step = boundsMargin;
    *minimum = gabi::fsubs_ppc(*minimum, step);
    *maximum = gabi::fadds_ppc(*maximum, step);
}

/* 024F50B8 */
void dBgWHf::MakeNodeTreeRpHf(s32 index) {
    HfData *bg = pm_bgd.get();
    if (index < 0 || index >= (s32)bg->m_tree_num) {
        assertion("d_bg_w_hf.cpp", 0x10F, "0 <= node_index && node_index < pm_bgd->m_tree_num");
        bg = pm_bgd.get();
    }
    cBgD_Tree_t *node = &bg->m_tree_tbl.get()[index];
    if (node->mFlag & LEAF) {
        u16 child = node->mChild[0];
        if (child != INVALID_INDEX) {
            cBgW_NodeTree *box = &m_nt_tbl.get()[index];
            MakeBlckBndHf(child, &box->min.y, &box->max.y);
        }
    } else {
        ClearForMinMaxY(&m_nt_tbl.get()[index]);
        for (u32 n = 0; n < 8; ++n) {
            u16 child = node->mChild[n];
            if (child == INVALID_INDEX)
                continue;
            MakeNodeTreeRpHf(child);
            child = node->mChild[n];
            cBgW_NodeTree *boxes = m_nt_tbl.get();
            f32 value = boxes[child].min.y;
            SetMinMaxY(&boxes[index], value);
            child = node->mChild[n];
            boxes = m_nt_tbl.get();
            value = boxes[child].max.y;
            SetMinMaxY(&boxes[index], value);
        }
    }
    cBgW_NodeTree *box = &m_nt_tbl.get()[index];
    f32 inf = positiveInfinity;
    bool bad = box->min.x == inf || box->min.y == inf || box->min.z == inf;
    if (!bad) {
        f32 neg = negativeInfinity;
        bad = box->max.x == neg || box->max.y == neg || box->max.z == neg;
    }
    if (bad) {
        assertion("d_bg_w_hf.cpp", 0x169,
                  "m_nt_tbl[node_index].GetMinP()->x != (1000000000.0f) && "
                  "m_nt_tbl[node_index].GetMinP()->y != (1000000000.0f) && "
                  "m_nt_tbl[node_index].GetMinP()->z != (1000000000.0f) && "
                  "m_nt_tbl[node_index].GetMaxP()->x != -(1000000000.0f) && "
                  "m_nt_tbl[node_index].GetMaxP()->y != -(1000000000.0f) && "
                  "m_nt_tbl[node_index].GetMaxP()->z != -(1000000000.0f)");
        box = &m_nt_tbl.get()[index];
    }
    f32 delta = gabi::fsubs_ppc(box->max.x, box->min.x), epsilon = normalEpsilon;
    bad = !(std::fabs(delta) < epsilon) && box->min.x > box->max.x;
    if (!bad) {
        delta = gabi::fsubs_ppc(box->max.y, box->min.y);
        bad = !(std::fabs(delta) < epsilon) && box->min.y > box->max.y;
    }
    if (!bad) {
        delta = gabi::fsubs_ppc(box->max.z, box->min.z);
        bad = !(std::fabs(delta) < epsilon) && box->min.z > box->max.z;
    }
    if (bad)
        assertion(
            "d_bg_w_hf.cpp", 0x16F,
            "(cM3d_IsZero(m_nt_tbl[node_index].GetMaxP()->x - m_nt_tbl[node_index].GetMinP()->x) "
            "|| m_nt_tbl[node_index].GetMinP()->x <= m_nt_tbl[node_index].GetMaxP()->x) && "
            "(cM3d_IsZero(m_nt_tbl[node_index].GetMaxP()->y - m_nt_tbl[node_index].GetMinP()->y) "
            "|| m_nt_tbl[node_index].GetMinP()->y <= m_nt_tbl[node_index].GetMaxP()->y) && "
            "(cM3d_IsZero(m_nt_tbl[node_index].GetMaxP()->z - m_nt_tbl[node_index].GetMinP()->z) "
            "|| m_nt_tbl[node_index].GetMinP()->z <= m_nt_tbl[node_index].GetMaxP()->z)");
}

/* 024F5528 */
void dBgWHf::MakeNodeTreeGrpRpHf(s32 index) {
    HfData *bg = pm_bgd.get();
    if (index < 0 || index >= (s32)bg->m_g_num) {
        assertion("d_bg_w_hf.cpp", 0x186, "0 <= g && g < pm_bgd->m_g_num");
        bg = pm_bgd.get();
    }
    cBgD_Grp_t *group = &bg->m_g_tbl.get()[index];
    u16 tree = group->m_tree_idx;
    if (tree != INVALID_INDEX) {
        MakeNodeTreeRpHf(tree);
        group = &pm_bgd->m_g_tbl.get()[index];
        tree = group->m_tree_idx;
        f32 value = m_nt_tbl.get()[tree].min.y;
        SetMinY(&pm_grp.get()[index], value);
        group = &pm_bgd->m_g_tbl.get()[index];
        tree = group->m_tree_idx;
        value = m_nt_tbl.get()[tree].max.y;
        SetMaxY(&pm_grp.get()[index], value);
        group = &pm_bgd->m_g_tbl.get()[index];
    }
    u16 child = group->m_first_child;
    while (child != INVALID_INDEX) {
        MakeNodeTreeGrpRpHf(child);
        cBgW_GrpElm *boxes = pm_grp.get();
        f32 value = boxes[child].aab.min.y;
        SetMinY(&boxes[index], value);
        boxes = pm_grp.get();
        value = boxes[child].aab.max.y;
        SetMaxY(&boxes[index], value);
        child = pm_bgd->m_g_tbl.get()[child].m_next_sibling;
    }
}

/* 024F5684 */
void dBgWHf::MakeNodeTreeHf() {
    cXyz *vertices = pm_vtx_tbl.get();
    HfData *bg = pm_bgd.get();
    s32 count = bg->m_g_num;
    if (vertices == NULL) {
        if (count > 0) {
            cBgD_Grp_t *groups = bg->m_g_tbl.get();
            for (s32 i = 0; i < count; ++i)
                if (groups[i].m_parent == INVALID_INDEX) {
                    m_rootGrpIdx = i;
                    return;
                }
        }
        return;
    }
    for (s32 i = 0; i < count; ++i) {
        ClearForMinMaxY(&pm_grp.get()[i].aab);
        bg = pm_bgd.get();
        count = bg->m_g_num;
    }
    u32 root = m_rootGrpIdx;
    if (root != INVALID_INDEX) {
        MakeNodeTreeGrpRpHf(root);
        return;
    }
    if (count > 0) {
        cBgD_Grp_t *groups = bg->m_g_tbl.get();
        for (s32 i = 0; i < count; ++i)
            if (groups[i].m_parent == INVALID_INDEX) {
                m_rootGrpIdx = i;
                MakeNodeTreeGrpRpHf(i);
                return;
            }
    }
}

// HD: compiler-generated construction and per-translation-unit header statics.
/* 024F5800 */
static dBgWHf *dBgWHf__ct(dBgWHf *object) {
    if (object == NULL) {
        object = (dBgWHf *)operator_new(sizeof(dBgWHf));
        if (object == NULL)
            return object;
    }
    constructSavedVertexWorld(object);
    object->__vtbl = gabi::at<HfVtable>(heightFieldVtable);
    object->mC4 = initialNormalY;
    object->m_gridx = 0;
    object->m_gridz = 0;
    object->mC8 = NULL;
    return object;
}
/* 024F5A14 */
void dBgWHf::MatrixCrrPos() {}
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject, padding[2];
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x1046EE08))
/* 024F5980 */
static void __sinit_d_bg_w_hf_cpp() {
    headerStatics.words[2] = 0;
    headerStatics.words[0] = 0;
    headerStatics.words[3] = 0;
    headerStatics.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101D539C));
    f32 a = piMin, b = piMax;
    headerStatics.piMin = a;
    headerStatics.piMax = b;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D53A8));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D53B4));
}

/* WWHD glue (tools/verify/autoglue.py)
data positiveInfinity 10044140
data negativeInfinity 10044144
data boundsMargin 10044148
data initialNormalY 10044554
data piMin 10044558
data piMax 1004455C
data normalEpsilon 100030B8
str 10044078 in 024F4B0C
str 10044088 in 024F4B0C
str 100440B4 in 024F4D04
str 100440E4 in 024F4D04
str 10044104 in 024F4E78
str 10044114 in 024F4E78
str 1004414C in 024F4F24
str 1004415C in 024F4F24
str 10044188 in 024F4F24
str 100441AC in 024F50B8
str 100441BC in 024F50B8
str 100441F0 in 024F50B8
str 10044340 in 024F50B8
str 10044524 in 024F5528
str 10044534 in 024F5528
ext 0273AD10 void* operator_new(u32)
ext 0273ADAC void* operator_new_array(u32)
ext 024F5A18 void constructSavedVertexWorld(dBgWHf*)
ext 0200A030 s32 SetBaseWorld(dBgWHf*,HfData*,u32,void*)
ext 02018A7C void SetupPlane(cBgW_TriElm*,cXyz*,cXyz*)
ext 0200A564 void BlckConnect(dBgWHf*,be<u16>*,be<u32>*,s32)
ext 02017DD8 void ClearForMinMaxY(cBgW_NodeTree*)
ext 02017EE8 void SetMinMaxY(cBgW_NodeTree*,f32)
ext 02017EC0 void SetMinY(cBgW_GrpElm*,f32)
ext 02017ED4 void SetMaxY(cBgW_GrpElm*,f32)
ext 0273AA24 void assertion(const char*,u32,const char*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
