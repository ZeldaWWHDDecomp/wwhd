/* f_op_actor_mng: actor manager helpers (fopAcM), WWHD. 
 *
 * Translation unit 025D537C..025DA2F4 (f_op_actor_iter and a function-less unit's __sinit
 * 025D52E8 precede it; the unit's own __sinit 025DA1B4 sits after fpoAcM_absolutePos and is
 * followed by its per-TU inline destructors 025DA254/025DA268/025DA2E0 and an empty function
 * 025DA2F4; f_op_actor_tag starts at 025DA2F8).
 * Part 1 (this file): 025D52E8..025D63E4 — layers, search, create/append, heaps.
 * Ported from the GameCube f_op_actor_mng.cpp. fpcM_Create is inlined as
 * fpcSCtRq_Request(fpcLy_CurrentLayer(), name, createFunc, NULL, append); fopAcM_SearchByID(id)
 * is inlined as (id == -1 ? NULL : fopAcIt_Judge(fpcSch_JudgeByID, &id)).
 *
 * fopAcM_prm_class (0x24) as on GameCube: parameters +0, pos +4, angle +0x10, setID +0x16,
 * scale (u8 x3) +0x18, gbaName +0x1B, parent id +0x1C, argument +0x20, room +0x21. */
#include "bindings.h"

namespace f_op_actor_mng_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline void JUT_CONFIRM_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA38, file, line, msg); }
static inline void OSReport_Error_l(u32 fmt) { gabi::call(0x025F2710, fmt); }
static inline void OSReport_Error_l(u32 fmt, u32 a, u32 b) { gabi::call(0x025F2710, fmt, a, b); }
static inline void OSReport_Warning_l(u32 fmt) { gabi::call(0x025F27E8, fmt); }
static inline void OSReport_Warning_l(u32 fmt, u32 a, u32 b) { gabi::call(0x025F27E8, fmt, a, b); }
static inline u32 fopScnM_SearchByID_l(u32 id) { return gabi::call<u32>(0x025DC80C, id); }
static inline BOOL fpcBs_Is_JustOfType_l(s32 a, s32 b) { return gabi::call<BOOL>(0x025DD258, a, b); }
static inline void fpcPi_Change_l(u32 pi, u32 layer, u32 listId, u32 listPrio) { gabi::call(0x025E0D38, pi, layer, listId, listPrio); }
static inline BOOL fpcM_IsCreating_l(u32 id) { return gabi::call<BOOL>(0x025DD868, id); }
static inline u32 fopAcIt_Judge_l(u32 fn, u32 data) { return gabi::call<u32>(0x025D5218, fn, data); }
static inline u32 cMl_memalignB_l(s32 align, u32 size) { return gabi::call<u32>(0x02019430, align, size); }
static inline void cLib_memSet_l(u32 p, s32 v, u32 n) { gabi::call(0x0200ECCC, p, v, n); }
static inline s32 fpcM_Delete_l(u32 a) { return gabi::call<s32>(0x025DF944, a); }
static inline u32 fpcLy_CurrentLayer_l() { return gabi::call<u32>(0x025DED64); }
static inline u32 fpcSCtRq_Request_l(u32 layer, u32 name, u32 fn, u32 data, u32 append) {
    return gabi::call<u32>(0x025E14A8, layer, name, fn, data, append);
}
static inline u32 fpcM_FastCreate_l(u32 name, u32 fn, u32 data, u32 append) { return gabi::call<u32>(0x025DFAB8, name, fn, data, append); }
static inline u32 dStage_searchName_l(u32 name) { return gabi::call<u32>(0x025C109C, name); }
static inline u32 dStage_getName_l(s16 procName, s8 arg) { return gabi::call<u32>(0x025C1150, procName, arg); }
static inline u32 mDoExt_createSolidHeapFromGameToCurrent_l(u32 size, u32 align) { return gabi::call<u32>(0x025E3630, size, align); }
static inline u32 mDoExt_createSolidHeapToCurrent_l(u32 size, u32 parent, u32 align) { return gabi::call<u32>(0x025E35BC, size, parent, align); }
static inline void mDoExt_restoreCurrentHeap_l() { gabi::call(0x025E37D8); }
static inline void mDoExt_adjustSolidHeap_l(u32 h) { gabi::call(0x025E3678, h); }
static inline void mDoExt_destroySolidHeap_l(u32 h) { gabi::call(0x025E3868, h); }
static inline void JKRHeap_alloc_l(u32 size, s32 align) { gabi::call(0x027EC0AC, size, align); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 kZeroXyz = 0x101FFBA8;
static constexpr u32 kZeroSXyz = 0x101FFB14;
static constexpr u32 fpcSch_JudgeByID = 0x025E1234;
static constexpr u32 fpcSch_JudgeForPName = 0x025E121C;
static constexpr u32 g_fpcNd_type = 0x101F3D60;
static constexpr u32 sincosTable = 0x104A44F8; /* JMath sin/cos table: {sin, cos} per (u16)angle >> 3 */
static constexpr u32 HeapAlign = 0x101F32FC;   /* HD: heap alignment for the entry heaps */
static constexpr u32 HeapAdjustVerbose = 0x101F3300;
static constexpr u32 HeapAdjustQuiet = 0x101F3301;

struct u32_l { be<u32> v; };
struct s16_l { be<s16> v; };
struct cXyz_l { be<f32> x, y, z; };
struct csXyz_l { be<s16> x, y, z; };

/* 025D52E8: the __sinit of a unit without functions between f_op_actor_iter and f_op_actor_mng */
static void __sinit_025D52E8() {
    WWHD_FUNC(0x025D52E8, void, (u32)0);
    sinit_header_statics(0x1048745C, 0x101F30CC);
}
VERIFY(0x025D52E8, __sinit_025D52E8);

/* fopScnM_LayerID (inline): the scene's layer id, -1 if it is not a node process */
static inline u32 scene_layer(u32 scn) {
    u32 layer = (u32)-1;
    if (fpcBs_Is_JustOfType_l(ld(g_fpcNd_type), ld(scn + 0xB8))) layer = ld(scn + 0xCC);
    return layer;
}

/* 025D537C */
static void fopAcM_setStageLayer(u32 pProc) {
    WWHD_FUNC(0x025D537C, void, pProc);
    u32 stageProc = fopScnM_SearchByID_l(ld(0x1047E6C4)); /* dStage_roomControl_c::getProcID() */
    if (stageProc == 0) JUT_ASSERT_l(0x10057284, 0xF1, 0x10057298);
    u32 layer = scene_layer(stageProc);
    fpcPi_Change_l(pProc + 0x68, layer, 0xFFFD, 0xFFFD); /* fpcM_ChangeLayerID */
}
VERIFY(0x025D537C, fopAcM_setStageLayer);

/* 025D5418 */
static void fopAcM_setRoomLayer(u32 pProc, s32 room_no) {
    WWHD_FUNC(0x025D5418, void, pProc, room_no);
    if (room_no >= 0) {
        u32 roomProc = fopScnM_SearchByID_l(ld(0x1047E8F0 + room_no * 0x22C)); /* getStatusProcID */
        if (roomProc == 0) JUT_ASSERT_l(0x100572BC, 0x108, 0x100572AC);
        u32 layer = scene_layer(roomProc);
        fpcPi_Change_l(pProc + 0x68, layer, 0xFFFD, 0xFFFD);
    }
}
VERIFY(0x025D5418, fopAcM_setRoomLayer);

/* 025D54C4: HD: an id of -1 finds nothing (FALSE) */
static BOOL fopAcM_SearchByID(u32 actorID, u32 pDstActor) {
    WWHD_FUNC(0x025D54C4, BOOL, actorID, pDstActor);
    gabi::Local<u32_l> id;
    id->v = actorID;
    if ((s32)actorID == -1) {
        st(pDstActor, 0);
        return FALSE;
    }
    if (fpcM_IsCreating_l(actorID)) {
        st(pDstActor, 0);
        return TRUE;
    }
    u32 a = fopAcIt_Judge_l(fpcSch_JudgeByID, gabi::ea(id.get()));
    st(pDstActor, a);
    if (a == 0) return FALSE;
    return TRUE;
}
VERIFY(0x025D54C4, fopAcM_SearchByID);

/* 025D5578 */
static BOOL fopAcM_SearchByName(s16 procName, u32 pDstActor) {
    WWHD_FUNC(0x025D5578, BOOL, procName, pDstActor);
    gabi::Local<s16_l> name;
    name->v = procName;
    u32 a = fopAcIt_Judge_l(fpcSch_JudgeForPName, gabi::ea(name.get()));
    st(pDstActor, a);
    if (a == 0) return FALSE;
    u32 id = a != 0 ? ld(a + 4) : (u32)-1; /* fopAcM_GetID */
    if (fpcM_IsCreating_l(id)) st(pDstActor, 0);
    return TRUE;
}
VERIFY(0x025D5578, fopAcM_SearchByName);

/* 025D5600 */
static u32 fopAcM_CreateAppend() {
    WWHD_FUNC(0x025D5600, u32);
    u32 params = cMl_memalignB_l(-4, 0x24);
    if (params != 0) {
        cLib_memSet_l(params, 0, 0x24);
        st16(params + 0x16, 0xFFFF);
        st8(params + 0x21, 0xFF);
        st8(params + 0x18, 10);
        st8(params + 0x19, 10);
        st8(params + 0x1A, 10);
        st(params + 0x1C, (u32)-1);
        st8(params + 0x20, 0xFF);
    }
    return params;
}
VERIFY(0x025D5600, fopAcM_CreateAppend);

/* 025D5678 */
static u32 createAppend(u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale, s32 i_argument, u32 parentPcId) {
    WWHD_FUNC(0x025D5678, u32, parameter, pPos, roomNo, pAngle, pScale, i_argument, parentPcId);
    u32 params = fopAcM_CreateAppend();
    if (params == 0) return 0;
    u32 src = pPos != 0 ? pPos : kZeroXyz;
    st(params + 4, ld(src + 0));
    st(params + 8, ld(src + 4));
    u32 z = ld(src + 8);
    st8(params + 0x21, (u8)roomNo);
    st(params + 0xC, z);
    u32 asrc = pAngle != 0 ? pAngle : kZeroSXyz;
    st16(params + 0x10, ld16(asrc + 0));
    st16(params + 0x12, ld16(asrc + 2));
    st16(params + 0x14, ld16(asrc + 4));
    if (pScale != 0) {
        st8(params + 0x18, (u8)gabi::ftoi(ldf(pScale + 0) * 10.0f));
        st8(params + 0x19, (u8)gabi::ftoi(ldf(pScale + 4) * 10.0f));
        u8 sz = (u8)gabi::ftoi(ldf(pScale + 8) * 10.0f);
        st(params + 0, parameter);
        st8(params + 0x1A, sz);
    } else {
        st(params + 0, parameter);
        st8(params + 0x18, 10);
        st8(params + 0x19, 10);
        st8(params + 0x1A, 10);
    }
    st(params + 0x1C, parentPcId);
    st8(params + 0x20, (u8)i_argument);
    return params;
}
VERIFY(0x025D5678, createAppend);

/* 025D57E0 */
static s32 fopAcM_delete_actor(u32 pActor) {
    WWHD_FUNC(0x025D57E0, s32, pActor);
    return fpcM_Delete_l(pActor);
}
VERIFY(0x025D57E0, fopAcM_delete_actor);

/* 025D57E4 */
static s32 fopAcM_delete_id(u32 actorID) {
    WWHD_FUNC(0x025D57E4, s32, actorID);
    gabi::Local<u32_l> id;
    id->v = actorID;
    u32 a = 0;
    if ((s32)actorID != -1) a = fopAcIt_Judge_l(fpcSch_JudgeByID, gabi::ea(id.get()));
    if (a != 0) return fpcM_Delete_l(a);
    return TRUE;
}
VERIFY(0x025D57E4, fopAcM_delete_id);

/* 025D5834 */
static u32 fopAcM_create(u32 procName, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale, u32 i_argument, u32 createFunc) {
    WWHD_FUNC(0x025D5834, u32, procName, parameter, pPos, roomNo, pAngle, pScale, i_argument, createFunc);
    u32 params = createAppend(parameter, pPos, roomNo, pAngle, pScale, i_argument, (u32)-1);
    if (params == 0) return (u32)-1;
    return fpcSCtRq_Request_l(fpcLy_CurrentLayer_l(), procName, createFunc, 0, params);
}
VERIFY(0x025D5834, fopAcM_create);

/* 025D58B4 */
static u32 fopAcM_create_str(u32 pProcNameString, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale, u32 createFunc) {
    WWHD_FUNC(0x025D58B4, u32, pProcNameString, parameter, pPos, roomNo, pAngle, pScale, createFunc);
    u32 nameInf = dStage_searchName_l(pProcNameString);
    if (nameInf == 0) return (u32)-1;
    return fopAcM_create((u32)(s32)(s16)ld16(nameInf + 8), parameter, pPos, roomNo, pAngle, pScale,
                         (u32)(s32)(s8)ld8(nameInf + 0xA), createFunc);
}
VERIFY(0x025D58B4, fopAcM_create_str);

/* 025D5928 */
static u32 fopAcM_fastCreate(u32 procName, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale, u32 i_argument, u32 createFunc) {
    WWHD_FUNC(0x025D5928, u32, procName, parameter, pPos, roomNo, pAngle, pScale, i_argument, createFunc);
    u32 pUserData = ld(gabi::cpu->r[1] + 8); /* 9th argument, on the stack */
    u32 params = createAppend(parameter, pPos, roomNo, pAngle, pScale, i_argument, (u32)-1);
    if (params == 0) return 0;
    return fpcM_FastCreate_l(procName, createFunc, pUserData, params);
}
VERIFY(0x025D5928, fopAcM_fastCreate);

/* 025D59A8 */
static u32 fopAcM_fastCreate_str(u32 pProcNameString, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale, u32 createFunc, u32 pUserData) {
    WWHD_FUNC(0x025D59A8, u32, pProcNameString, parameter, pPos, roomNo, pAngle, pScale, createFunc, pUserData);
    u32 nameInf = dStage_searchName_l(pProcNameString);
    if (nameInf == 0) return 0;
    return gabi::call<u32>(0x025D5928, (u32)(s32)(s16)ld16(nameInf + 8), parameter, pPos, roomNo, pAngle, pScale,
                           (u32)(s32)(s8)ld8(nameInf + 0xA), createFunc, pUserData); /* fopAcM_fastCreate */
}
VERIFY(0x025D59A8, fopAcM_fastCreate_str);

/* 025D5A20 */
static u32 fopAcM_createChild(u32 procName, u32 parentPcId, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale, u32 i_argument) {
    WWHD_FUNC(0x025D5A20, u32, procName, parentPcId, parameter, pPos, roomNo, pAngle, pScale, i_argument);
    u32 createFunc = ld(gabi::cpu->r[1] + 8); /* 9th argument, on the stack */
    u32 params = createAppend(parameter, pPos, roomNo, pAngle, pScale, i_argument, parentPcId);
    if (params == 0) return (u32)-1;
    return fpcSCtRq_Request_l(fpcLy_CurrentLayer_l(), procName, createFunc, 0, params);
}
VERIFY(0x025D5A20, fopAcM_createChild);

/* 025D5AA4 */
static u32 fopAcM_createChild_str(u32 pProcNameString, u32 parentPcId, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale, u32 createFunc) {
    WWHD_FUNC(0x025D5AA4, u32, pProcNameString, parentPcId, parameter, pPos, roomNo, pAngle, pScale, createFunc);
    u32 nameInf = dStage_searchName_l(pProcNameString);
    if (nameInf == 0) return (u32)-1;
    return gabi::call<u32>(0x025D5A20, (u32)(s32)(s16)ld16(nameInf + 8), parentPcId, parameter, pPos, roomNo, pAngle, pScale,
                           (u32)(s32)(s8)ld8(nameInf + 0xA), createFunc); /* fopAcM_createChild */
}
VERIFY(0x025D5AA4, fopAcM_createChild_str);

/* the child's position and angle from the parent's (createChildFromOffset) */
static inline void child_offset(u32 parent, u32 pPosOffs, u32 pAngleOffs, u32 pos, u32 angle) {
    s16 parentAngleY = (s16)ld16(parent + 0x322);
    u32 po = pPosOffs != 0 ? pPosOffs : kZeroXyz;
    u32 ao = pAngleOffs != 0 ? pAngleOffs : kZeroSXyz;
    f32 ox = ldf(po + 0), oy = ldf(po + 4), oz = ldf(po + 8);
    f32 px = ldf(parent + 0x314), py = ldf(parent + 0x318), pz = ldf(parent + 0x31C);
    u32 t = sincosTable + ((u32)((u16)parentAngleY >> 3) << 3);
    f32 s = ldf(t), c = ldf(t + 4);
    stf(pos + 0, gabi::fadds_ppc(px, gabi::fmadds(oz, s, gabi::fmuls_ppc(ox, c))));
    stf(pos + 4, gabi::fadds_ppc(py, oy));
    stf(pos + 8, gabi::fadds_ppc(pz, gabi::fmsubs(oz, c, gabi::fmuls_ppc(ox, s))));
    st16(angle + 0, ld16(ao + 0));
    st16(angle + 2, (u16)(ld16(ao + 2) + parentAngleY));
    st16(angle + 4, ld16(ao + 4));
}

/* 025D5B20: HD: asserts and fails when the parent is not found */
static u32 fopAcM_createChildFromOffset(u32 procName, u32 parentPcId, u32 parameter, u32 pPosOffs, s32 roomNo, u32 pAngleOffs, u32 pScale, u32 i_argument) {
    WWHD_FUNC(0x025D5B20, u32, procName, parentPcId, parameter, pPosOffs, roomNo, pAngleOffs, pScale, i_argument);
    u32 createFunc = ld(gabi::cpu->r[1] + 8); /* 9th argument, on the stack */
    gabi::Local<u32_l> id;
    id->v = parentPcId;
    u32 pParent = 0;
    if ((s32)parentPcId != -1) pParent = fopAcIt_Judge_l(fpcSch_JudgeByID, gabi::ea(id.get()));
    if (pParent == 0) {
        JUT_ASSERT_l(0x100572E4, 0x2F0, 0x100572D4);
        return (u32)-1;
    }
    gabi::Local<cXyz_l> pos;
    gabi::Local<csXyz_l> angle;
    child_offset(pParent, pPosOffs, pAngleOffs, gabi::ea(pos.get()), gabi::ea(angle.get()));
    u32 params = createAppend(parameter, gabi::ea(pos.get()), roomNo, gabi::ea(angle.get()), pScale, i_argument, parentPcId);
    if (params == 0) return (u32)-1;
    return fpcSCtRq_Request_l(fpcLy_CurrentLayer_l(), procName, createFunc, 0, params);
}
VERIFY(0x025D5B20, fopAcM_createChildFromOffset);

/* 025D5D88 */
static u32 fopAcM_createChildFromOffset_str(u32 pProcNameString, u32 parentPcId, u32 parameter, u32 pPosOffs, s32 roomNo, u32 pAngleOffs, u32 pScale, u32 createFunc) {
    WWHD_FUNC(0x025D5D88, u32, pProcNameString, parentPcId, parameter, pPosOffs, roomNo, pAngleOffs, pScale, createFunc);
    gabi::Local<u32_l> id;
    id->v = parentPcId;
    u32 pParent = 0;
    if ((s32)parentPcId != -1) pParent = fopAcIt_Judge_l(fpcSch_JudgeByID, gabi::ea(id.get()));
    if (pParent == 0) {
        JUT_ASSERT_l(0x10057308, 0x320, 0x100572F8);
        return (u32)-1;
    }
    gabi::Local<cXyz_l> pos;
    gabi::Local<csXyz_l> angle;
    child_offset(pParent, pPosOffs, pAngleOffs, gabi::ea(pos.get()), gabi::ea(angle.get()));
    return fopAcM_createChild_str(pProcNameString, parentPcId, parameter, gabi::ea(pos.get()), roomNo, gabi::ea(angle.get()), pScale, createFunc);
}
VERIFY(0x025D5D88, fopAcM_createChildFromOffset_str);

/* 025D5FA4 */
static u32 fopAcM_getProcNameString(u32 i_this) {
    WWHD_FUNC(0x025D5FA4, u32, i_this);
    s8 arg = (s8)ld8(i_this + 0x2DD);
    s16 name = i_this != 0 ? (s16)ld16(i_this + 0xE) : 0x7FFF; /* fopAcM_GetProfName */
    u32 s = dStage_getName_l(name, arg);
    if (s == 0) s = 0x1005731C; /* "NONAME" */
    return s;
}
VERIFY(0x025D5FA4, fopAcM_getProcNameString);

/* 025D5FEC */
static BOOL fopAcM_createHeap(u32 i_this, u32 size, u32 align) {
    WWHD_FUNC(0x025D5FEC, BOOL, i_this, size, align);
    if (i_this == 0) JUT_ASSERT_l(0x10057370, 0x34C, 0x10057324);
    if (ld(i_this + 0xF4) != 0) JUT_ASSERT_l(0x10057370, 0x34D, 0x1005732C);
    if (align == 0) align = 0x20;
    u32 heap = mDoExt_createSolidHeapFromGameToCurrent_l(size, align);
    st(i_this + 0xF4, heap);
    if (heap == 0) {
        OSReport_Error_l(0x10057340);
        if (ld(i_this + 0xF4) == 0) JUT_CONFIRM_l(0x10057370, 0x35D, 0x1005735C);
        return FALSE;
    }
    u32 name = fopAcM_getProcNameString(i_this);
    st(ld(i_this + 0xF4) + 0x10, name); /* HD: the heap is named after the actor */
    return TRUE;
}
VERIFY(0x025D5FEC, fopAcM_createHeap);

/* 025D6100 */
static void fopAcM_adjustHeap(u32 i_this) {
    WWHD_FUNC(0x025D6100, void, i_this);
    mDoExt_restoreCurrentHeap_l();
    mDoExt_adjustSolidHeap_l(ld(i_this + 0xF4));
}
VERIFY(0x025D6100, fopAcM_adjustHeap);

/* 025D6134 */
static void fopAcM_DeleteHeap(u32 i_this) {
    WWHD_FUNC(0x025D6134, void, i_this);
    u32 heap = ld(i_this + 0xF4);
    if (heap != 0) {
        mDoExt_destroySolidHeap_l(heap);
        st(i_this + 0xF4, 0);
    }
}
VERIFY(0x025D6134, fopAcM_DeleteHeap);

/* 025D6174 (matcher: dRes_info_c::setRes): HD-only fopAcM_entrySolidHeap_(actor, cb, size,
 * parent heap): the solid heap is made in the given parent heap with the global alignment; an
 * estimate that is too small falls back to the largest free block (size 0). */
static BOOL fopAcM_entrySolidHeap_(u32 i_this, u32 createHeapCB, u32 estimatedHeapSize, u32 parentHeap) {
    WWHD_FUNC(0x025D6174, BOOL, i_this, createHeapCB, estimatedHeapSize, parentHeap);
    u32 name = fopAcM_getProcNameString(i_this);
    u32 heap;
    if (estimatedHeapSize != 0) {
        heap = mDoExt_createSolidHeapToCurrent_l(estimatedHeapSize, parentHeap, ld(HeapAlign));
        if (heap == 0) {
            if (ld8(HeapAdjustQuiet) == 0) OSReport_Warning_l(0x100573C0);
            return FALSE;
        }
        st(heap + 0x10, name);
        bool result = gabi::call_ptr<s32>(createHeapCB, i_this) != 0;
        u32 vt = ld(heap + 0xC);
        u32 freeSize = gabi::call_ptr<u32>(ld(vt + 0x74), heap); /* getFreeSize */
        if (freeSize >= 0x20) JKRHeap_alloc_l(0x20, 4);
        mDoExt_restoreCurrentHeap_l();
        if (result) {
            u32 vt2 = ld(heap + 0xC);
            u32 heapSize = gabi::call_ptr<u32>(ld(vt2 + 0x6C), heap); /* getHeapSize */
            u32 freeSize2 = gabi::call_ptr<u32>(ld(vt2 + 0x74), heap);
            u32 al = ld(HeapAlign) - 1;
            u32 allocSize = (heapSize - freeSize2 + al) & ~al;
            mDoExt_adjustSolidHeap_l(heap);
            if (estimatedHeapSize >= allocSize + 0x40 && ld8(HeapAdjustVerbose) != 0)
                OSReport_Warning_l(0x100573F8, allocSize, estimatedHeapSize);
            st(i_this + 0xF4, heap);
            return TRUE;
        }
        if (ld8(HeapAdjustQuiet) == 0) OSReport_Error_l(0x1005738C, estimatedHeapSize, name);
        mDoExt_destroySolidHeap_l(heap);
    }
    heap = mDoExt_createSolidHeapToCurrent_l(0, parentHeap, ld(HeapAlign));
    if (heap == 0) return FALSE;
    st(heap + 0x10, name);
    bool result = gabi::call_ptr<s32>(createHeapCB, i_this) != 0;
    mDoExt_restoreCurrentHeap_l();
    if (!result) {
        mDoExt_destroySolidHeap_l(heap);
        return FALSE;
    }
    if (ld8(HeapAdjustQuiet) == 0) {
        u32 vt = ld(heap + 0xC);
        gabi::call_ptr<u32>(ld(vt + 0x6C), heap);
        gabi::call_ptr<u32>(ld(vt + 0x74), heap);
    }
    mDoExt_adjustSolidHeap_l(heap);
    st(i_this + 0xF4, heap);
    return TRUE;
}
VERIFY(0x025D6174, fopAcM_entrySolidHeap_);

} // namespace f_op_actor_mng_cpp
