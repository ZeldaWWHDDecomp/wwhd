/* Background-world registration and collision-query dispatch. Verified HD behavior. */
#include "SSystem/SComponent/c_bg_s.h"
static s32 l_SetCounter = 0;
static const f32 groundHeightMin = -1000000000.0f;
static const f32 piMin = -3.1415927410125732f;
static const f32 piMax = 3.1415927410125732f;
enum : u32 { registryVtable = 0x10000E3C, elementVtable = 0x10000E24 };
static cBgS_ChkElm *cBgS_ChkElm__ct(cBgS_ChkElm *);

/* 02007F60 */
void cBgS_ChkElm::Init() {
    m_actor_ptr = NULL;
    m_actor_id = fpcM_ERROR_PROCESS_ID_e;
    m_flags = 0;
    m_bgw_base_ptr = NULL;
}
/* 02007F7C */
void cBgS_ChkElm::Regist2(cBgWMesh *bgw, u32 pid, void *actor) {
    u32 flags = m_flags;
    m_bgw_base_ptr = bgw;
    m_actor_id = pid;
    m_actor_ptr = actor;
    m_flags = flags | USED;
}
/* 02007F98 */
void cBgS_ChkElm::Release() {
    u32 flags = m_flags;
    m_bgw_base_ptr = NULL;
    m_actor_ptr = NULL;
    m_actor_id = fpcM_ERROR_PROCESS_ID_e;
    m_flags = flags & ~USED;
}

/* 020086D4 */
bool cBgS::Regist(cBgWMesh *bgw, u32 pid, void *actor) {
    if (bgw == NULL || (u32)bgw->m_id < MAX_BG)
        return true;
    if (cBgW_ChkMemoryError(bgw))
        return true;
    u32 i = l_SetCounter;
    if (i >= MAX_BG)
        assertion("c_bg_s.cpp", 0xA3, "0 <= i && i < 256");
    while (true) {
        cBgS_ChkElm *elm = &m_chk_element[i];
        u32 next = i + 1;
        if (!(elm->m_flags & cBgS_ChkElm::USED)) {
            elm->Regist2(bgw, pid, actor);
            bgw->m_id = i;
            l_SetCounter = (s32)next < MAX_BG ? next : 0;
            return false;
        }
        i = next;
        u32 start = l_SetCounter;
        if ((s32)i >= MAX_BG)
            i = 0;
        if (start == i)
            break;
    }
    bgw->m_id = MAX_BG;
    return true;
}
/* 020087EC */
bool cBgS::Release(cBgWMesh *bgw) {
    if (bgw == NULL)
        return true;
    u32 id = bgw->m_id;
    if (id >= MAX_BG)
        return true;
    cBgS_ChkElm *elm = &m_chk_element[id];
    if (!(elm->m_flags & cBgS_ChkElm::USED))
        return true;
    elm->Release();
    bgw->m_id = MAX_BG;
    return false;
}
/* 02008020 */
void cBgS::Ct() {
    l_SetCounter = 0;
    for (u32 i = 0; i < MAX_BG; ++i)
        m_chk_element[i].Init();
}
/* 0200805C */
void cBgS::Dt() {
    for (u32 i = 0; i < MAX_BG; ++i)
        if (m_chk_element[i].m_flags & cBgS_ChkElm::USED)
            m_chk_element[i].Release();
    for (u32 i = 0; i < MAX_BG; ++i)
        m_chk_element[i].Init();
}

/* 02008860 */
// HD: keep query initialization and registry reloads in the original callback order.
bool cBgS::LineCross(cBgS_LinChk *chk) {
    chk->mpBgW = NULL;
    chk->mActorId = fpcM_ERROR_PROCESS_ID_e;
    chk->mBgIndex = MAX_BG;
    chk->mPolyIndex = INVALID_POLY;
    chk->mFlag = (u32)chk->mFlag & ~cBgS_LinChk::HIT_e;
    bool ret = false;
    for (u32 i = 0; i < MAX_BG; ++i) {
        cBgS_ChkElm *elm = &m_chk_element[i];
        if (!(elm->m_flags & cBgS_ChkElm::USED))
            continue;
        cBgWMesh *world = elm->m_bgw_base_ptr.get();
        if (world->pm_vtx_tbl.get() == NULL)
            continue;
        if (cBgS_ChkSameActorPid(chk, elm->m_actor_id))
            continue;
        u32 flags = chk->mFlag;
        chk->mPreWallChk = ((flags >> 30) & 1) ^ 1;
        chk->mPreGroundChk = ((flags >> 31) & 1) ^ 1;
        chk->mPreRoofChk = ((flags >> 29) & 1) ^ 1;
        world = elm->m_bgw_base_ptr.get();
        if (!cBgW_LineCheckGrpRp(world, chk, world->m_rootGrpIdx, 1))
            continue;
        u32 pid = elm->m_actor_id;
        cBgWMesh *ptr = elm->m_bgw_base_ptr.get();
        chk->mActorId = pid;
        chk->mpBgW = (cBgW *)ptr;
        chk->mBgIndex = i;
        ret = true;
    }
    if (ret)
        chk->mFlag = (u32)chk->mFlag | cBgS_LinChk::HIT_e;
    return ret;
}
/* 02008974 */
f32 cBgS::GroundCross(cBgS_GndChk *chk) {
    f32 height = groundHeightMin;
    u32 flags = chk->mFlag;
    chk->mPolyIndex = INVALID_POLY;
    chk->mpBgW = NULL;
    chk->mActorId = fpcM_ERROR_PROCESS_ID_e;
    chk->mBgIndex = MAX_BG;
    chk->mGndPrecheck = flags & 1;
    chk->mNowY = height;
    chk->mWallPrecheck = flags & 2;
    for (u32 i = 0; i < MAX_BG; ++i) {
        cBgS_ChkElm *elm = &m_chk_element[i];
        if (!(elm->m_flags & cBgS_ChkElm::USED))
            continue;
        cBgWMesh *world = elm->m_bgw_base_ptr.get();
        if (world->pm_vtx_tbl.get() == NULL)
            continue;
        if (cBgS_ChkSameActorPid(chk, elm->m_actor_id))
            continue;
        world = elm->m_bgw_base_ptr.get();
        if (!cBgW_GroundCrossGrpRp(world, chk, world->m_rootGrpIdx, 1))
            continue;
        u32 pid = elm->m_actor_id;
        cBgWMesh *ptr = elm->m_bgw_base_ptr.get();
        chk->mActorId = pid;
        chk->mpBgW = (cBgW *)ptr;
        chk->mBgIndex = i;
    }
    return chk->mNowY;
}

/* 020080C0 */
// HD: free entry for GameCube's static cBgS::ConvDzb; preserve relocation/assertion ordering.
static cBgD_Reloc *ConvDzb(cBgD_Reloc *pbgd) {
    u32 base = gabi::ea(pbgd), flag = pbgd->flag;
    if (flag & cBgD_Reloc::RELOCATED)
        return pbgd;
    u32 vertex = pbgd->m_v_tbl.v;
    pbgd->flag = flag | cBgD_Reloc::RELOCATED;
    if (vertex & 3)
        assertion("c_bg_s.cpp", 0x244, "((int)pbgd->m_v_tbl % 4) == 0");
    if (pbgd->m_t_tbl.v & 1)
        assertion("c_bg_s.cpp", 0x245, "((int)pbgd->m_t_tbl % 2) == 0");
    if (pbgd->m_b_tbl.v & 1)
        assertion("c_bg_s.cpp", 0x246, "((int)pbgd->m_b_tbl % 2) == 0");
    if (pbgd->m_tree_tbl.v & 1)
        assertion("c_bg_s.cpp", 0x247, "((int)pbgd->m_tree_tbl % 2) == 0");
    if (pbgd->m_g_tbl.v & 3)
        assertion("c_bg_s.cpp", 0x248, "((int)pbgd->m_g_tbl % 4) == 0");
    if (pbgd->m_ti_tbl.v & 3)
        assertion("c_bg_s.cpp", 0x249, "((int)pbgd->m_ti_tbl % 4) == 0");
    vertex = pbgd->m_v_tbl.v;
    u32 groups = pbgd->m_g_tbl.v;
    if (vertex)
        pbgd->m_v_tbl.v = vertex + base;
    u32 trees = pbgd->m_tree_tbl.v, info = pbgd->m_ti_tbl.v;
    pbgd->m_g_tbl.v = groups + base;
    u32 blocks = pbgd->m_b_tbl.v, tris = pbgd->m_t_tbl.v;
    pbgd->m_ti_tbl.v = info + base;
    s32 count = pbgd->m_g_num;
    pbgd->m_tree_tbl.v = trees + base;
    pbgd->m_t_tbl.v = tris + base;
    pbgd->m_b_tbl.v = blocks + base;
    for (s32 i = 0; i < count; ++i) {
        cBgD_Grp_t *group = &pbgd->m_g_tbl.get()[i];
        group->m_name.v = (u32)group->m_name.v + base;
        count = pbgd->m_g_num;
    }
    return pbgd;
}

/* 02008438 */
void *cBgS::GetActorPointer(u32 actor_index) {
    if (actor_index >= MAX_BG)
        assertion("c_bg_s.cpp", 0x2B4, "0 <= actor_index && actor_index < 256");
    return m_chk_element[actor_index].m_actor_ptr.get();
}
/* 02008498 */
cBgWMesh *cBgS::GetBgWPointer(cBgS_PolyInfo *poly) {
    u32 id = poly->mBgIndex;
    if (id >= MAX_BG)
        return NULL;
    cBgS_ChkElm *elm = &m_chk_element[id];
    if (!(elm->m_flags & cBgS_ChkElm::USED))
        return NULL;
    return elm->m_bgw_base_ptr.get();
}
/* 02008254 */
bool cBgS::ChkPolySafe(cBgS_PolyInfo *poly) {
    if (poly->mPolyIndex == INVALID_POLY)
        return false;
    u32 id = poly->mBgIndex;
    if (id >= MAX_BG)
        return false;
    cBgS_ChkElm *elm = &m_chk_element[id];
    if (!(elm->m_flags & cBgS_ChkElm::USED))
        return false;
    cBgW *ptr = poly->mpBgW.get();
    cBgWMesh *world = elm->m_bgw_base_ptr.get();
    u32 pid = elm->m_actor_id;
    if ((void *)ptr != (void *)world)
        return false;
    return poly->mActorId == pid;
}
/* 020082C0 */
s32 cBgS::GetTriGrp(u32 bg_index, s32 poly_index) {
    if (bg_index >= MAX_BG)
        assertion("c_bg_s.cpp", 0x2F5, "0 <= bg_index && bg_index < 256");
    cBgS_ChkElm *elm = &m_chk_element[bg_index];
    if (!(elm->m_flags & cBgS_ChkElm::USED))
        return -1;
    cBgWMesh *world = elm->m_bgw_base_ptr.get();
    cBgD_Mesh *data = NULL;
    if (poly_index >= 0)
        data = world->pm_bgd.get();
    if (poly_index < 0 || poly_index >= (s32)data->m_t_num) {
        assertion("c_bg_w.h", 0x2A2, "0 <= poly_index && poly_index < pm_bgd->m_t_num");
        data = world->pm_bgd.get();
    }
    return data->m_t_tbl.get()[poly_index].grp;
}
/* 0200839C */
s32 cBgS::GetGrpToRoomId(u32 bg_index, u32 grp_id) {
    if (bg_index >= MAX_BG)
        assertion("c_bg_s.cpp", 0x306, "0 <= bg_index && bg_index < 256");
    cBgS_ChkElm *elm = &m_chk_element[bg_index];
    if (!(elm->m_flags & cBgS_ChkElm::USED) || grp_id == INVALID_POLY)
        return INVALID_POLY;
    cBgWMesh *world = elm->m_bgw_base_ptr.get();
    auto *vtable = (BgSRoomVtable *)world->__vtbl.get();
    return gabi::call_ptr<s32>(vtable->GetGrpToRoomIndex, world, grp_id);
}
/* 020084C8 */
cBgW_Plane *cBgS::GetTriPla(u32 bg_index, s32 poly_index) {
    if (bg_index >= MAX_BG)
        return NULL;
    cBgS_ChkElm *elm = &m_chk_element[bg_index];
    if (!(elm->m_flags & cBgS_ChkElm::USED))
        return NULL;
    cBgWMesh *world = elm->m_bgw_base_ptr.get();
    if (poly_index < 0 || poly_index >= (s32)world->pm_bgd->m_t_num)
        assertion("c_bg_w.h", 0x2AF, "0 <= poly_index && poly_index < pm_bgd->m_t_num");
    return &world->pm_tri.get()[poly_index].m_plane;
}
/* 02008570 */
void cBgS::GetTriPnt(cBgS_PolyInfo *poly, cXyz *p0, cXyz *p1, cXyz *p2) {
    u32 id = poly->mBgIndex;
    if (id >= MAX_BG)
        assertion("c_bg_s.cpp", 0x373, "0 <= id && id < 256");
    cBgS_ChkElm *elm = &m_chk_element[id];
    if (elm->m_flags & cBgS_ChkElm::USED)
        cBgW_GetTriPnt(elm->m_bgw_base_ptr.get(), poly->mPolyIndex, p0, p1, p2);
}
/* 02008B48 */
void cBgS::Move() {}
/* 020085F8 */
s32 cBgS::GetGrpInf(cBgS_PolyInfo *poly, s32 grp_id) {
    u32 id = poly->mBgIndex;
    if (id >= MAX_BG)
        assertion("c_bg_s.cpp", 0x3FA, "0 <= bg_index && bg_index < 256");
    cBgS_ChkElm *elm = &m_chk_element[id];
    if (!(elm->m_flags & cBgS_ChkElm::USED))
        return -1;
    cBgWMesh *world = elm->m_bgw_base_ptr.get();
    cBgD_Mesh *data = NULL;
    if (grp_id >= 0)
        data = world->pm_bgd.get();
    if (grp_id < 0 || grp_id >= (s32)data->m_g_num) {
        // HD: the assertion text calls this index grp_id.
        assertion("c_bg_w.h", 0x2E1, "0 <= grp_index && grp_index < pm_bgd->m_g_num");
        data = world->pm_bgd.get();
    }
    return data->m_g_tbl.get()[grp_id].m_info;
}

// HD: compiler-generated array construction and header statics.
/* 02007FBC */
static cBgS *cBgS__ct(cBgS *object) {
    if (object == NULL)
        object = (cBgS *)operator_new(sizeof(cBgS));
    if (object != NULL) {
        object->__vtbl = registryVtable;
        constructElements(object, cBgS::MAX_BG, sizeof(cBgS_ChkElm), cBgS_ChkElm__ct);
    }
    return object;
}
/* 02008B04 */
static cBgS_ChkElm *cBgS_ChkElm__ct(cBgS_ChkElm *object) {
    if (object == NULL)
        object = (cBgS_ChkElm *)operator_new(sizeof(cBgS_ChkElm));
    if (object != NULL) {
        object->__vtbl = elementVtable;
        object->Init();
    }
    return object;
}
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject, padding[2];
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x101FF3D0))
/* 02008A70 */
static void __sinit_c_bg_s_cpp() {
    headerStatics.words[2] = 0;
    headerStatics.words[0] = 0;
    headerStatics.words[3] = 0;
    headerStatics.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C528));
    f32 first = piMin, second = piMax;
    headerStatics.piMin = first;
    headerStatics.piMax = second;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C534));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C540));
}

/* WWHD glue (tools/verify/autoglue.py)
data l_SetCounter 1018C520
data groundHeightMin 10000E0C
data piMin 10000E14
data piMax 10000E18
str 10000C08 "c_bg_s.cpp" in 020080C0
str 10000B88 "((int)pbgd->m_v_tbl % 4) == 0" in 020080C0
str 10000BA8 "((int)pbgd->m_t_tbl % 2) == 0" in 020080C0
str 10000BC8 "((int)pbgd->m_b_tbl % 2) == 0" in 020080C0
str 10000C14 "((int)pbgd->m_tree_tbl % 2) == 0" in 020080C0
str 10000BE8 "((int)pbgd->m_g_tbl % 4) == 0" in 020080C0
str 10000C38 "((int)pbgd->m_ti_tbl % 4) == 0" in 020080C0
str 10000CA8 "c_bg_s.cpp" in 020082C0
str 10000C58 "0 <= bg_index && bg_index < 256" in 020082C0
str 10000CB4 "c_bg_w.h" in 020082C0
str 10000C78 "0 <= poly_index && poly_index < pm_bgd->m_t_num" in 020082C0
str 10000CE0 "c_bg_s.cpp" in 0200839C
str 10000CC0 "0 <= bg_index && bg_index < 256" in 0200839C
str 10000D24 "c_bg_s.cpp" in 02008438
str 10000CFC "0 <= actor_index && actor_index < 256" in 02008438
str 10000D60 "c_bg_w.h" in 020084C8
str 10000D30 "0 <= poly_index && poly_index < pm_bgd->m_t_num" in 020084C8
str 10000D80 "c_bg_s.cpp" in 02008570
str 10000D6C "0 <= id && id < 256" in 02008570
str 10000DD4 "c_bg_s.cpp" in 020085F8
str 10000D8C "0 <= bg_index && bg_index < 256" in 020085F8
str 10000DE0 "c_bg_w.h" in 020085F8
str 10000DAC "0 <= grp_index && grp_index < pm_bgd->m_g_num" renamed in 020085F8
str 10000E00 "c_bg_s.cpp" in 020086D4
str 10000DEC "0 <= i && i < 256" in 020086D4
ext 0273AD10 void* operator_new(u32)
ext 028EFFD0 void constructElements(cBgS*,u32,u32,BgSElementCtor)
ext 02009E58 s32 cBgW_ChkMemoryError(cBgWMesh*)
ext 02008BB8 s32 cBgS_ChkSameActorPid(cBgS_Chk*,u32)
ext 0200AB4C s32 cBgW_LineCheckGrpRp(cBgWMesh*,cBgS_LinChk*,u32,u32)
ext 0200B380 s32 cBgW_GroundCrossGrpRp(cBgWMesh*,cBgS_GndChk*,u32,u32)
ext 0200A220 void cBgW_GetTriPnt(cBgWMesh*,u32,cXyz*,cXyz*,cXyz*)
ext 0273AA24 void assertion(const char*,u32,const char*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
