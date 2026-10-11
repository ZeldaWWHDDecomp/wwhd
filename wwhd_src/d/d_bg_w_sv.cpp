/** Saved-vertex world collision correction. */
#include "d/d_bg_w_sv.h"
#include <cmath>

static const f32 normalEpsilon = 0.000003814697265625f;
static const f32 weightMin = 0.0f;
static const f32 weightMax = 1.0f;
static const u32 headerPiMinBits = 0xC0490FDB;
static const u32 headerPiMaxBits = 0x40490FDB;
// HD: saved-vertex world vtable copy.
enum : u32 { savedVertexVtable = 0x100445D0 };

/* 024F5A18 */
// HD: GHS allocation and explicit vtable/flag initialization.
static dBgWSv *dBgWSv__ct(dBgWSv *object) {
    if (object == NULL)
        object = (dBgWSv *)operator_new(sizeof(dBgWSv));
    if (object != NULL) {
        constructBaseWorld(object);
        object->mBackVtx = NULL;
        object->__vtbl = savedVertexVtable;
        object->mSvFlag = 0;
    }
    return object;
}

/* 024F5A78 */
BOOL dBgWSv::Set(BgSvData *data, u32 flags) {
    if (SetBaseBackground(this, data, MOVE_BG_e | NO_CALC_VTX_e | GLOBAL_e | UNK40_e, NULL))
        return 1;
    mSvFlag = flags;
    if (flags & NO_CRR_POS)
        return 0;
    BgSvData *collision = pm_bgd.get();
    cXyz *vertices = (cXyz *)operator_new_array((u32)collision->m_v_num * sizeof(cXyz));
    mBackVtx = vertices;
    return vertices == NULL;
}

/* 024F5B08 */
void dBgWSv::CopyBackVtx() {
    cXyz *dest = mBackVtx.get();
    if (dest == NULL)
        return;
    cXyz *source = pm_vtx_tbl.get();
    if (source == NULL)
        return;
    BgSvData *collision = pm_bgd.get();
    if ((s32)collision->m_v_num <= 0)
        return;
    u32 index = 0;
    while (true) {
        dest[index].copy(source[index]);
        collision = pm_bgd.get();
        ++index;
        s32 count = collision->m_v_num;
        if ((s32)index >= count)
            return;
        source = pm_vtx_tbl.get();
        dest = mBackVtx.get();
    }
}

/* 024F5B78 */
// HD: preserve fused arithmetic and pointer reloads when pos aliases the vertex-table field.
BOOL dBgWSv::CrrPosWork(cXyz *pos, u32 v0, u32 v1, u32 v2) {
    cXyz *back = mBackVtx.get();
    f32 z0 = (f32)back[v0].z;
    f32 x0 = (f32)back[v0].x;
    f32 bz = (f32)back[v1].z - z0;
    f32 bx = (f32)back[v1].x - x0;
    f32 cx = (f32)back[v2].x - x0;
    f32 cz = (f32)back[v2].z - z0;
    f32 epsilon = normalEpsilon;
    if (std::fabs(bz) < epsilon || std::fabs(cz) < epsilon)
        return 1;
    f32 ratioC = cx / cz;
    f32 denB = gabi::fnmsubs(ratioC, bz, bx);
    f32 ratioB = bx / bz;
    f32 denC = gabi::fnmsubs(ratioB, cz, cx);
    if (std::fabs(denB) < epsilon || std::fabs(denC) < epsilon)
        return 1;
    f32 px = (f32)pos->x - x0;
    pos->x = px;
    back = mBackVtx.get();
    f32 pz = (f32)pos->z - (f32)back[v0].z;
    f32 numC = gabi::fnmsubs(ratioB, pz, px);
    pos->z = pz;
    f32 numB = gabi::fnmsubs(ratioC, pz, px);
    cXyz *current = pm_vtx_tbl.get();
    f32 wC = numC / denC;
    f32 ax = (f32)current[v0].x;
    f32 tx = (f32)current[v2].x;
    f32 wB = numB / denB;
    f32 sx = (f32)current[v1].x;
    f32 az = (f32)current[v0].z;
    f32 sz = (f32)current[v1].z;
    f32 tz = (f32)current[v2].z;
    f32 dxB = sx - ax, dzB = sz - az;
    f32 dxC = tx - ax, dzC = tz - az;
    f32 resultX = gabi::fmadds(wB, dxB, wC * dxC);
    f32 resultZ = gabi::fmadds(wB, dzB, wC * dzC);
    pos->x = resultX;
    pos->z = resultZ;
    current = pm_vtx_tbl.get();
    pos->x = resultX + (f32)current[v0].x;
    current = pm_vtx_tbl.get();
    pos->z = resultZ + (f32)current[v0].z;
    return 0;
}

/* 024F5CBC */
void dBgWSv::CrrPos(BgSvPolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle,
                    csXyz *shape) {
    if (((u32)mSvFlag & NO_CRR_POS) || !accept)
        return;
    u32 index = poly->mPolyIndex;
    BgSvData *collision = pm_bgd.get();
    BgSvTri *triangle = &collision->m_t_tbl[index];
    u32 v0 = triangle->vtx0, v1 = triangle->vtx1, v2 = triangle->vtx2;
    if (!CrrPosWork(pos, v0, v1, v2))
        return;
    if (!CrrPosWork(pos, v1, v2, v0))
        return;
    CrrPosWork(pos, v2, v0, v1);
}

/* 024F5D84 */
// HD: preserve fused interpolation and each position store before reloading the table.
BOOL dBgWSv::TransPosWork(cXyz *pos, u32 v0, u32 v1, u32 v2) {
    cXyz *back = mBackVtx.get();
    f32 x0 = (f32)back[v0].x, z0 = (f32)back[v0].z;
    f32 bz = (f32)back[v1].z - z0;
    f32 bx = (f32)back[v1].x - x0;
    f32 cx = (f32)back[v2].x - x0;
    f32 cz = (f32)back[v2].z - z0;
    f32 epsilon = normalEpsilon;
    if (std::fabs(bz) < epsilon || std::fabs(cz) < epsilon)
        return 1;
    f32 ratioC = cx / cz;
    f32 denB = gabi::fnmsubs(ratioC, bz, bx);
    f32 ratioB = bx / bz;
    f32 denC = gabi::fnmsubs(ratioB, cz, cx);
    if (std::fabs(denB) < epsilon || std::fabs(denC) < epsilon)
        return 1;
    f32 px = (f32)pos->x - x0, pz = (f32)pos->z - z0;
    f32 wB = gabi::fnmsubs(ratioC, pz, px) / denB;
    f32 wC = gabi::fnmsubs(ratioB, pz, px) / denC;
    f32 zero = weightMin, one = weightMax;
    if (wB < zero || wB > one || wC < zero || wC > one)
        return 1;
    cXyz *current = pm_vtx_tbl.get();
    f32 ax = (f32)current[v0].x, bxNew = (f32)current[v1].x - ax;
    f32 ay = (f32)current[v0].y, byNew = (f32)current[v1].y - ay;
    f32 az = (f32)current[v0].z, bzNew = (f32)current[v1].z - az;
    f32 cxNew = (f32)current[v2].x - ax;
    f32 cyNew = (f32)current[v2].y - ay;
    f32 czNew = (f32)current[v2].z - az;
    f32 resultX = gabi::fmadds(wB, bxNew, wC * cxNew);
    f32 resultY = gabi::fmadds(wB, byNew, wC * cyNew);
    f32 resultZ = gabi::fmadds(wB, bzNew, wC * czNew);
    pos->x = resultX;
    pos->y = resultY;
    pos->z = resultZ;
    current = pm_vtx_tbl.get();
    pos->x = resultX + (f32)current[v0].x;
    current = pm_vtx_tbl.get();
    pos->y = resultY + (f32)current[v0].y;
    current = pm_vtx_tbl.get();
    pos->z = resultZ + (f32)current[v0].z;
    return 0;
}

/* 024F5F38 */
void dBgWSv::TransPos(BgSvPolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle,
                      csXyz *shape) {
    if (((u32)mSvFlag & NO_CRR_POS) || !accept)
        return;
    u32 index = poly->mPolyIndex;
    BgSvData *collision = pm_bgd.get();
    BgSvTri *triangle = &collision->m_t_tbl[index];
    u32 v0 = triangle->vtx0, v1 = triangle->vtx1, v2 = triangle->vtx2;
    if (!TransPosWork(pos, v0, v1, v2))
        return;
    if (!TransPosWork(pos, v1, v2, v0))
        return;
    TransPosWork(pos, v2, v0, v1);
}

// HD: compiler-generated initialization of this translation unit's header statics.
struct HeaderStatics {
    be<u32> piMinBits, piMaxBits;
    u8 byteObject, flagObject, padding[2];
};
struct HeaderZeroObject {
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x1046EE24))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x1046EE30))
/* 024F6000 */
static void __sinit_d_bg_w_sv_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101D53C0));
    u32 lower = headerPiMinBits, upper = headerPiMaxBits;
    headerStatics.piMinBits = lower;
    headerStatics.piMaxBits = upper;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D53CC));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D53D8));
}

/* 024F6094 */
// HD: the emitted base virtual method is empty.
void dBgWSv::MatrixCrrPos(BgSvPolyInfo *poly, void *user, u32 accept, cXyz *pos, csXyz *angle,
                          csXyz *shape) {
}

/* WWHD glue (tools/verify/autoglue.py)
data normalEpsilon 100030B8
data weightMin 100445BC
data weightMax 100445C0
data headerPiMinBits 100445C8
data headerPiMaxBits 100445CC
ext 0273AD10 void* operator_new(u32)
ext 0273ADAC void* operator_new_array(u32)
ext 024F23F4 void constructBaseWorld(dBgWSv*)
ext 0200A030 BOOL SetBaseBackground(dBgWSv*,BgSvData*,u32,void*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
