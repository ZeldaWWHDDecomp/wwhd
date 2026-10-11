/** Actor manager: layers, actor lookup, creation parameters and solid heaps.
 * WWHD source, verified against the native unit.
 */
// HD: entry signatures retain verified register widths; *Register entries are distinct
// from shared typed or overloaded GameCube wrappers.
#include "f_op/f_op_actor_mng.h"

// One definition of every TU-wide object; the other source parts use the header declarations.
static ActorManagerSinCosTable sincosTable;
static cXyz zeroPosition;
static csXyz zeroAngle;
static u32 HeapAlign;
static u8 HeapAdjustVerbose;
static u8 HeapAdjustQuiet;
static ActorManagerRoomStatuses roomStatuses;
static u32 nodeProcessType;
static u32 stageProcessID;
static ActorManagerMatrix viewMtx;
static ActorManagerClipper clipper;
static f32 systemFar;
static ActorManagerCullBoxes l_cullSizeBox;
static ActorManagerCullSpheres l_cullSizeSphere;
static u32 stageHeapFlags;
static u32 specialActorHeap;
static ActorManagerViewRoot* viewRoot;
static u32 saveSingleton;
static ActorManagerFairyOffsets fairy_offset_tbl;
static u32 fairy_offset_initialized;
static ActorManagerWaterCheck water_check;
static ActorManagerHeaderStatics headerStatics;
static u32 water_check_initialized;

const gc_tu_data gc_tu = {.safeString = 0x1005716C,
                          .gndChk = {0x100571A4, 0x100571B4, 0x100571D4, 0x100571C4},
                          .gndChkDt = {0x100571B4, 0x100571D4, 0x10057194},
                          .headerStatics = {0x1048745C, 0x101F30CC, 0}};
const ActorManagerTuData actorManagerTu = {
    0x100571E4, 0x10057204, 0x10057194, {0x101F32C0, 0x101F32CC, 0x101F32D8, 0x101F32E4, 0x101F32F0}};


static constexpr u32 fpcSch_JudgeByID = 0x025E1234;
static constexpr u32 fpcSch_JudgeForPName = 0x025E121C;

/* the __sinit of a unit without functions between f_op_actor_iter and f_op_actor_mng */
/* 025D52E8 */
static void __sinit_actorManagerHeader() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* fopScnM_LayerID (inline): the scene's layer id, -1 if it is not a node process */
static inline u32 scene_layer(u32 scn) {
    u32 layer = (u32)-1;
    if (actMngGuest_fpcBs_Is_JustOfType((u32)nodeProcessType,
                                        (u32)actorMngView<process_node_class>(scn)->mSubType))
        layer = (u32)actorMngView<process_node_class>(scn)->mLayer.mLayerID;
    return layer;
}

/* 025D537C */
static void fopAcM_setStageLayerRegister(u32 pProc) {
    u32 stageProc =
        actMngGuest_fopScnM_SearchByID((u32)stageProcessID); /* dStage_roomControl_c::getProcID() */
    if (stageProc == 0)
        actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0xF1,
                               gabi::guest_addr("stageProc != (0)"));
    u32 layer = scene_layer(stageProc);
    actMngGuest_fpcPi_Change(gabi::ea(&actorMngView<base_process_class>(pProc)->mPi), layer, 0xFFFD,
                             0xFFFD); /* fpcM_ChangeLayerID */
}

/* 025D5418 */
static void fopAcM_setRoomLayer(u32 pProc, s32 room_no) {
    if (room_no >= 0) {
        u32 roomProc = actMngGuest_fopScnM_SearchByID(
            (u32)roomStatuses.rooms[room_no].mProcID); /* getStatusProcID */
        if (roomProc == 0)
            actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x108,
                                   gabi::guest_addr("roomProc != (0)"));
        u32 layer = scene_layer(roomProc);
        actMngGuest_fpcPi_Change(gabi::ea(&actorMngView<base_process_class>(pProc)->mPi), layer, 0xFFFD,
                                 0xFFFD);
    }
}

// HD: an id of -1 finds nothing (FALSE)
/* 025D54C4 */
static BOOL fopAcM_SearchByIDRegister(u32 actorID, u32 pDstActor) {
    gabi::Local<ActorManagerGuestID> id;
    id->value = actorID;
    if ((s32)actorID == -1) {
        *actorMngView<be<u32>>(pDstActor) = 0;
        return FALSE;
    }
    if (actMngGuest_fpcM_IsCreating(actorID)) {
        *actorMngView<be<u32>>(pDstActor) = 0;
        return TRUE;
    }
    u32 a = actMngGuest_fopAcIt_Judge(fpcSch_JudgeByID, gabi::ea(id.get()));
    *actorMngView<be<u32>>(pDstActor) = a;
    if (a == 0) return FALSE;
    return TRUE;
}

/* 025D5578 */
static BOOL fopAcM_SearchByName(s16 procName, u32 pDstActor) {
    gabi::Local<ActorManagerGuestName> name;
    name->value = procName;
    u32 a = actMngGuest_fopAcIt_Judge(fpcSch_JudgeForPName, gabi::ea(name.get()));
    *actorMngView<be<u32>>(pDstActor) = a;
    if (a == 0) return FALSE;
    u32 id = a != 0 ? (u32)actorMngActor(a)->mBsPcId : (u32)-1; /* fopAcM_GetID */
    if (actMngGuest_fpcM_IsCreating(id)) *actorMngView<be<u32>>(pDstActor) = 0;
    return TRUE;
}

/* 025D5600 */
static u32 fopAcM_CreateAppend() {
    u32 params = actMngGuest_cMl_memalignB(-4, 0x24);
    fopAcM_prm_class* append = actorMngView<fopAcM_prm_class>(params);
    if (params != 0) {
        actMngGuest_cLib_memSet(params, 0, 0x24);
        append->base.setID = 0xFFFF;
        append->room_no = 0xFF;
        append->scale.x = 10;
        append->scale.y = 10;
        append->scale.z = 10;
        append->parent_id = (u32)-1;
        append->argument = 0xFF;
    }
    return params;
}

/* 025D5678 */
static u32 createAppend(u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale, s32 i_argument,
                        u32 parentPcId) {
    u32 params = fopAcM_CreateAppend();
    fopAcM_prm_class* append = actorMngView<fopAcM_prm_class>(params);
    if (params == 0) return 0;
    u32 src = pPos != 0 ? pPos : gabi::ea(&zeroPosition);
    actorMngBits(append->base.position.x) = (u32)(actorMngBits(actorMngView<cXyz>(src)->x));
    actorMngBits(append->base.position.y) = (u32)(actorMngBits(actorMngView<cXyz>(src)->y));
    u32 z = (u32)(actorMngBits(actorMngView<cXyz>(src)->z));
    append->room_no = (u8)roomNo;
    actorMngBits(append->base.position.z) = z;
    u32 asrc = pAngle != 0 ? pAngle : gabi::ea(&zeroAngle);
    append->base.angle.x = (u16)(actorMngView<csXyz>(asrc)->x);
    append->base.angle.y = (u16)(actorMngView<csXyz>(asrc)->y);
    append->base.angle.z = (u16)(actorMngView<csXyz>(asrc)->z);
    if (pScale != 0) {
        append->scale.x = (u8)gabi::ftoi((f32)(actorMngView<cXyz>(pScale)->x) * 10.0f);
        append->scale.y = (u8)gabi::ftoi((f32)(actorMngView<cXyz>(pScale)->y) * 10.0f);
        u8 sz = (u8)gabi::ftoi((f32)(actorMngView<cXyz>(pScale)->z) * 10.0f);
        append->base.parameters = parameter;
        append->scale.z = sz;
    } else {
        append->base.parameters = parameter;
        append->scale.x = 10;
        append->scale.y = 10;
        append->scale.z = 10;
    }
    append->parent_id = parentPcId;
    append->argument = (u8)i_argument;
    return params;
}

/* 025D57E0 */
static s32 fopAcM_delete_actor(u32 pActor) {
    return actMngGuest_fpcM_Delete(pActor);
}

/* 025D57E4 */
static s32 fopAcM_delete_id(u32 actorID) {
    gabi::Local<ActorManagerGuestID> id;
    id->value = actorID;
    u32 a = 0;
    if ((s32)actorID != -1) a = actMngGuest_fopAcIt_Judge(fpcSch_JudgeByID, gabi::ea(id.get()));
    if (a != 0) return actMngGuest_fpcM_Delete(a);
    return TRUE;
}

/* 025D5834 */
static u32 fopAcM_createRegister(u32 procName, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale,
                                 u32 i_argument, u32 createFunc) {
    u32 params = createAppend(parameter, pPos, roomNo, pAngle, pScale, i_argument, (u32)-1);
    if (params == 0) return (u32)-1;
    return actMngGuest_fpcSCtRq_Request(actMngGuest_fpcLy_CurrentLayer(), procName, createFunc, 0, params);
}

/* 025D58B4 */
static u32 fopAcM_create_str(u32 pProcNameString, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale,
                             u32 createFunc) {
    u32 nameInf = actMngGuest_dStage_searchName(pProcNameString);
    if (nameInf == 0) return (u32)-1;
    return fopAcM_createRegister((u32)(s32)(s16)(u16)(actorMngView<ActorManagerNameInfo>(nameInf)->mProcName),
                                 parameter, pPos, roomNo, pAngle, pScale,
                                 (u32)(s32)(s8)(u8)(actorMngView<ActorManagerNameInfo>(nameInf)->mArgument),
                                 createFunc);
}

/* 025D5928 */
static u32 fopAcM_fastCreate(u32 procName, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle, u32 pScale,
                             u32 i_argument, u32 createFunc) {
    u32 pUserData = *actorMngView<be<u32>>(gabi::cpu->r[1] + 8); /* 9th argument, on the stack */
    u32 params = createAppend(parameter, pPos, roomNo, pAngle, pScale, i_argument, (u32)-1);
    if (params == 0) return 0;
    return actMngGuest_fpcM_FastCreate(procName, createFunc, pUserData, params);
}

/* 025D59A8 */
static u32 fopAcM_fastCreate_str(u32 pProcNameString, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle,
                                 u32 pScale, u32 createFunc, u32 pUserData) {
    u32 nameInf = actMngGuest_dStage_searchName(pProcNameString);
    if (nameInf == 0) return 0;
    return actMngGuest_fopAcM_fastCreate(
        (u32)(s32)(s16)(u16)(actorMngView<ActorManagerNameInfo>(nameInf)->mProcName), parameter, pPos, roomNo,
        pAngle, pScale, (u32)(s32)(s8)(u8)(actorMngView<ActorManagerNameInfo>(nameInf)->mArgument),
        createFunc, pUserData); /* fopAcM_fastCreate */
}

/* 025D5A20 */
static u32 fopAcM_createChild(u32 procName, u32 parentPcId, u32 parameter, u32 pPos, s32 roomNo, u32 pAngle,
                              u32 pScale, u32 i_argument) {
    u32 createFunc = *actorMngView<be<u32>>(gabi::cpu->r[1] + 8); /* 9th argument, on the stack */
    u32 params = createAppend(parameter, pPos, roomNo, pAngle, pScale, i_argument, parentPcId);
    if (params == 0) return (u32)-1;
    return actMngGuest_fpcSCtRq_Request(actMngGuest_fpcLy_CurrentLayer(), procName, createFunc, 0, params);
}

/* 025D5AA4 */
static u32 fopAcM_createChild_str(u32 pProcNameString, u32 parentPcId, u32 parameter, u32 pPos, s32 roomNo,
                                  u32 pAngle, u32 pScale, u32 createFunc) {
    u32 nameInf = actMngGuest_dStage_searchName(pProcNameString);
    if (nameInf == 0) return (u32)-1;
    return actorMngCreateChild((u32)(s32)(s16)(u16)(actorMngView<ActorManagerNameInfo>(nameInf)->mProcName),
                               parentPcId, parameter, pPos, roomNo, pAngle, pScale,
                               (u32)(s32)(s8)(u8)(actorMngView<ActorManagerNameInfo>(nameInf)->mArgument),
                               createFunc); /* fopAcM_createChild */
}

/* the child's position and angle from the parent's (createChildFromOffset) */
static inline void child_offset(u32 parent, u32 pPosOffs, u32 pAngleOffs, u32 pos, u32 angle) {
    s16 parentAngleY = (s16)(u16)actorMngActor(parent)->current.angle.y;
    u32 po = pPosOffs != 0 ? pPosOffs : gabi::ea(&zeroPosition);
    u32 ao = pAngleOffs != 0 ? pAngleOffs : gabi::ea(&zeroAngle);
    f32 ox = (f32)(actorMngView<cXyz>(po)->x), oy = (f32)(actorMngView<cXyz>(po)->y),
        oz = (f32)(actorMngView<cXyz>(po)->z);
    f32 px = (f32)actorMngActor(parent)->current.pos.x, py = (f32)actorMngActor(parent)->current.pos.y,
        pz = (f32)actorMngActor(parent)->current.pos.z;
    u32 t = gabi::ea(&sincosTable) + ((u32)((u16)parentAngleY >> 3) << 3);
    f32 s = (f32)(actorMngView<ActorManagerSinCos>(t)->sin),
        c = (f32)(actorMngView<ActorManagerSinCos>(t)->cos);
    actorMngView<cXyz>(pos)->x = gabi::fadds_ppc(px, gabi::fmadds(oz, s, gabi::fmuls_ppc(ox, c)));
    actorMngView<cXyz>(pos)->y = gabi::fadds_ppc(py, oy);
    actorMngView<cXyz>(pos)->z = gabi::fadds_ppc(pz, gabi::fmsubs(oz, c, gabi::fmuls_ppc(ox, s)));
    actorMngView<csXyz>(angle)->x = (u16)(actorMngView<csXyz>(ao)->x);
    actorMngView<csXyz>(angle)->y = (u16)((u16)(actorMngView<csXyz>(ao)->y) + parentAngleY);
    actorMngView<csXyz>(angle)->z = (u16)(actorMngView<csXyz>(ao)->z);
}

// HD: asserts and fails when the parent is not found
/* 025D5B20 */
static u32 fopAcM_createChildFromOffset(u32 procName, u32 parentPcId, u32 parameter, u32 pPosOffs, s32 roomNo,
                                        u32 pAngleOffs, u32 pScale, u32 i_argument) {
    u32 createFunc = *actorMngView<be<u32>>(gabi::cpu->r[1] + 8); /* 9th argument, on the stack */
    gabi::Local<ActorManagerGuestID> id;
    id->value = parentPcId;
    u32 pParent = 0;
    if ((s32)parentPcId != -1) pParent = actMngGuest_fopAcIt_Judge(fpcSch_JudgeByID, gabi::ea(id.get()));
    if (pParent == 0) {
        actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x2F0,
                               gabi::guest_addr("mother != (0)"));
        return (u32)-1;
    }
    gabi::Local<cXyz> pos;
    gabi::Local<csXyz> angle;
    child_offset(pParent, pPosOffs, pAngleOffs, gabi::ea(pos.get()), gabi::ea(angle.get()));
    u32 params = createAppend(parameter, gabi::ea(pos.get()), roomNo, gabi::ea(angle.get()), pScale,
                              i_argument, parentPcId);
    if (params == 0) return (u32)-1;
    return actMngGuest_fpcSCtRq_Request(actMngGuest_fpcLy_CurrentLayer(), procName, createFunc, 0, params);
}

/* 025D5D88 */
static u32 fopAcM_createChildFromOffset_str(u32 pProcNameString, u32 parentPcId, u32 parameter, u32 pPosOffs,
                                            s32 roomNo, u32 pAngleOffs, u32 pScale, u32 createFunc) {
    gabi::Local<ActorManagerGuestID> id;
    id->value = parentPcId;
    u32 pParent = 0;
    if ((s32)parentPcId != -1) pParent = actMngGuest_fopAcIt_Judge(fpcSch_JudgeByID, gabi::ea(id.get()));
    if (pParent == 0) {
        actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x320,
                               gabi::guest_addr("mother != (0)"));
        return (u32)-1;
    }
    gabi::Local<cXyz> pos;
    gabi::Local<csXyz> angle;
    child_offset(pParent, pPosOffs, pAngleOffs, gabi::ea(pos.get()), gabi::ea(angle.get()));
    return fopAcM_createChild_str(pProcNameString, parentPcId, parameter, gabi::ea(pos.get()), roomNo,
                                  gabi::ea(angle.get()), pScale, createFunc);
}

/* 025D5FA4 */
static u32 fopAcM_getProcNameString(u32 i_this) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    s8 arg = (s8)(u8)actorView->argument;
    s16 name = i_this != 0 ? (s16)(u16)actorView->mProfName : 0x7FFF; /* fopAcM_GetProfName */
    u32 s = actMngGuest_dStage_getName(name, arg);
    if (s == 0) s = gabi::guest_addr("UNKOWN");
    return s;
}

/* 025D5FEC */
static BOOL fopAcM_createHeap(u32 i_this, u32 size, u32 align) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    if (i_this == 0)
        actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x34C, gabi::guest_addr("i_this"));
    if ((u32)actorView->heap.v != 0)
        actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x34D,
                               gabi::guest_addr("i_this->heap == (0)"));
    if (align == 0) align = 0x20;
    u32 heap = actMngGuest_mDoExt_createSolidHeapFromGameToCurrent(size, align);
    actorView->heap.v = heap;
    if (heap == 0) {
        actorMngReportError(gabi::guest_addr("fopAcM_createHeap \212m\225\333\216\270\224s\012"));
        if ((u32)actorView->heap.v == 0)
            actMngGuest_JUT_CONFIRM(gabi::guest_addr("f_op_actor_mng.cpp"), 0x35D,
                                    gabi::guest_addr("i_this->heap != (0)"));
        return FALSE;
    }
    u32 name = fopAcM_getProcNameString(i_this);
    actorMngView<ActorManagerHeap>(actorView->heap.v)->mName = name; // HD: the heap is named after the actor
    return TRUE;
}

/* 025D6100 */
static void fopAcM_adjustHeap(u32 i_this) {
    actMngGuest_mDoExt_restoreCurrentHeap();
    actMngGuest_mDoExt_adjustSolidHeap((u32)actorMngActor(i_this)->heap.v);
}

/* 025D6134 */
static void fopAcM_DeleteHeap(u32 i_this) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    u32 heap = (u32)actorView->heap.v;
    if (heap != 0) {
        actMngGuest_mDoExt_destroySolidHeap(heap);
        actorView->heap.v = 0;
    }
}

// HD: the supplied parent heap and global alignment are used; an undersized estimate retries the largest block.
/* 025D6174 */
static BOOL fopAcM_entrySolidHeap_(u32 i_this, u32 createHeapCB, u32 estimatedHeapSize, u32 parentHeap) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    u32 name = fopAcM_getProcNameString(i_this);
    u32 heap;
    if (estimatedHeapSize != 0) {
        heap = actMngGuest_mDoExt_createSolidHeapToCurrent(estimatedHeapSize, parentHeap, (u32)(HeapAlign));
        if (heap == 0) {
            if ((u8)(HeapAdjustQuiet) == 0)
                actorMngReportWarning(gabi::guest_addr(
                    "\214\251\220\317\202\340\202\350\203q\201[\203v\203T\203C\203Y\202\305\203q\201["
                    "\203v\202\252\212m\225\333\202\305\202\253\202\334\202\271\202\361\202\305\202\265\202"
                    "\275\201B\012"));
            return FALSE;
        }
        actorMngView<ActorManagerHeap>(heap)->mName = name;
        bool result = gabi::call_ptr<s32>(createHeapCB, i_this) != 0;
        u32 vt = (u32)(actorMngView<ActorManagerHeap>(heap)->mVtable);
        u32 freeSize = gabi::call_ptr<u32>((u32)(actorMngView<ActorManagerHeapVtable>(vt)->getFreeSize),
                                           heap); /* getFreeSize */
        if (freeSize >= 0x20) actMngGuest_JKRHeap_alloc(0x20, 4);
        actMngGuest_mDoExt_restoreCurrentHeap();
        if (result) {
            u32 vt2 = (u32)(actorMngView<ActorManagerHeap>(heap)->mVtable);
            u32 heapSize = gabi::call_ptr<u32>((u32)(actorMngView<ActorManagerHeapVtable>(vt2)->getHeapSize),
                                               heap); /* getHeapSize */
            u32 freeSize2 =
                gabi::call_ptr<u32>((u32)(actorMngView<ActorManagerHeapVtable>(vt2)->getFreeSize), heap);
            u32 al = (u32)(HeapAlign)-1;
            u32 allocSize = (heapSize - freeSize2 + al) & ~al;
            actMngGuest_mDoExt_adjustSolidHeap(heap);
            if (estimatedHeapSize >= allocSize + 0x40 && (u8)(HeapAdjustVerbose) != 0)
                actorMngReportWarningHeaps(
                    gabi::guest_addr("\214\251\220\317\202\340\202\350\203q\201["
                                     "\203v\203T\203C\203Y\202\305\202\315\213\363\202\253\202\252\221\275"
                                     "\202\267\202\254\202\334\202\267\201B %08x %08x\012\033[m"),
                    allocSize, estimatedHeapSize);
            actorView->heap.v = heap;
            return TRUE;
        }
        if ((u8)(HeapAdjustQuiet) == 0)
            actorMngReportErrorHeap(
                gabi::guest_addr(
                    "\214\251\220\317\202\340\202\350\203q\201[\203v\203T\203C\203Y(%08x)\202\305\223o\230^"
                    "\216\270\224s\202\265\202\334\202\265\202\275\201B[%s]\012"),
                estimatedHeapSize, name);
        actMngGuest_mDoExt_destroySolidHeap(heap);
    }
    heap = actMngGuest_mDoExt_createSolidHeapToCurrent(0, parentHeap, (u32)(HeapAlign));
    if (heap == 0) return FALSE;
    actorMngView<ActorManagerHeap>(heap)->mName = name;
    bool result = gabi::call_ptr<s32>(createHeapCB, i_this) != 0;
    actMngGuest_mDoExt_restoreCurrentHeap();
    if (!result) {
        actMngGuest_mDoExt_destroySolidHeap(heap);
        return FALSE;
    }
    if ((u8)(HeapAdjustQuiet) == 0) {
        u32 vt = (u32)(actorMngView<ActorManagerHeap>(heap)->mVtable);
        gabi::call_ptr<u32>((u32)(actorMngView<ActorManagerHeapVtable>(vt)->getHeapSize), heap);
        gabi::call_ptr<u32>((u32)(actorMngView<ActorManagerHeapVtable>(vt)->getFreeSize), heap);
    }
    actMngGuest_mDoExt_adjustSolidHeap(heap);
    actorView->heap.v = heap;
    return TRUE;
}

/* WWHD glue
data sincosTable 104A44F8
data zeroPosition 101FFBA8
data zeroAngle 101FFB14
data HeapAlign 101F32FC
data HeapAdjustVerbose 101F3300
data HeapAdjustQuiet 101F3301
data roomStatuses 1047E8F0
data nodeProcessType 101F3D60
data stageProcessID 1047E6C4
data viewMtx 104B45F8
data clipper 1048CFF0
data systemFar 1048D04C
data l_cullSizeBox 101F30F0
data l_cullSizeSphere 101F3240
data stageHeapFlags 1047E6C0
data specialActorHeap 1047C8B4
data viewRoot 101F95D0
data saveSingleton 101F84DC
data fairy_offset_tbl 10487494
data fairy_offset_initialized 10487508
data water_check 104874B8
data headerStatics 10487478
data water_check_initialized 1048750C
str 10057284 in 025D537C
str 10057298 in 025D537C
str 100572AC in 025D5418
str 100572BC in 025D5418
str 100572D4 in 025D5B20
str 100572E4 in 025D5B20
str 100572F8 in 025D5D88
str 10057308 in 025D5D88
str 1005731C in 025D5FA4
str 10057324 in 025D5FEC
str 1005732C in 025D5FEC
str 10057340 in 025D5FEC
str 1005735C in 025D5FEC
str 10057370 in 025D5FEC
str 1005738C in 025D6174
str 100573C0 in 025D6174
str 100573F8 in 025D6174
ext 025F2710 void actorMngReportError(u32 format)
ext 025F2710 void actorMngReportErrorHeap(u32 format, u32 size, u32 name)
ext 025F27E8 void actorMngReportWarning(u32 format)
ext 025F27E8 void actorMngReportWarningHeaps(u32 format, u32 first, u32 second)
ext 025D5A20 u32 actorMngCreateChild(u32 name, u32 parent, u32 parameters, u32 position, s32 room, u32 angle, u32 scale, u32 argument, u32 callback)
ext 025DA2F4 void actorMngAssureTermination(u32 string)
ext 028ED6F8 void actorMngRandConstructor(u32 object)
ext 028EAB2C void actorMngListConstructor(u32 object)
ext 02008B4C void actorMngSplGroupDestructor(u32 object, s32 flags)
ext 0273AA24 void actMngGuest_JUT_ASSERT(u32 file, s32 line, u32 msg)
ext 0273AA38 void actMngGuest_JUT_CONFIRM(u32 file, s32 line, u32 msg)
ext 025DC80C u32 actMngGuest_fopScnM_SearchByID(u32 id)
ext 025DD258 BOOL actMngGuest_fpcBs_Is_JustOfType(s32 a, s32 b)
ext 025E0D38 void actMngGuest_fpcPi_Change(u32 pi, u32 layer, u32 listId, u32 listPrio)
ext 025DD868 BOOL actMngGuest_fpcM_IsCreating(u32 id)
ext 025D5218 u32 actMngGuest_fopAcIt_Judge(u32 fn, u32 data)
ext 02019430 u32 actMngGuest_cMl_memalignB(s32 align, u32 size)
ext 0200ECCC void actMngGuest_cLib_memSet(u32 p, s32 v, u32 n)
ext 025DF944 s32 actMngGuest_fpcM_Delete(u32 a)
ext 025DED64 u32 actMngGuest_fpcLy_CurrentLayer()
ext 025E14A8 u32 actMngGuest_fpcSCtRq_Request(u32 layer, u32 name, u32 fn, u32 data, u32 append)
ext 025DFAB8 u32 actMngGuest_fpcM_FastCreate(u32 name, u32 fn, u32 data, u32 append)
ext 025C109C u32 actMngGuest_dStage_searchName(u32 name)
ext 025C1150 u32 actMngGuest_dStage_getName(s16 procName, s8 arg)
ext 025E3630 u32 actMngGuest_mDoExt_createSolidHeapFromGameToCurrent(u32 size, u32 align)
ext 025E35BC u32 actMngGuest_mDoExt_createSolidHeapToCurrent(u32 size, u32 parent, u32 align)
ext 025E37D8 void actMngGuest_mDoExt_restoreCurrentHeap()
ext 025E3678 void actMngGuest_mDoExt_adjustSolidHeap(u32 h)
ext 025E3868 void actMngGuest_mDoExt_destroySolidHeap(u32 h)
ext 027EC0AC void actMngGuest_JKRHeap_alloc(u32 size, s32 align)
ext 025E2F64 u32 actMngGuest_mDoExt_getHeap(s32 i)
ext 025D6174 BOOL actMngGuest_fopAcM_entrySolidHeap_(u32 a, u32 cb, u32 size, u32 heap)
ext 025D5FA4 u32 actMngGuest_fopAcM_getProcNameString(u32 a)
ext 0200F8D0 BOOL actMngGuest_cLib_chaseAngleS(u32 p, u32 target, u32 step)
ext 0200F93C s16 actMngGuest_cLib_targetAngleY(u32 a, u32 b)
ext 0201ADE0 void actMngGuest_cXyz_mi(u32 self, u32 out, u32 other)
ext 028E8DD0 f32 actMngGuest_PSVECSquareMag(u32 v)
ext 028E9108 void actMngGuest_PSMTXConcat(u32 a, u32 b, u32 out)
ext 028E8F64 void actMngGuest_PSMTXMultVec(u32 m, u32 in, u32 out)
ext 02838274 BOOL actMngGuest_J3DUClipper_clipBox(u32 self, u32 mtx, u32 max, u32 min)
ext 02838148 BOOL actMngGuest_J3DUClipper_clipSphere(u32 self, u32 mtx, u32 c, f32 r)
ext 0283801C void actMngGuest_J3DUClipper_calcViewFrustum(u32 self)
ext 0286DE88 void actMngGuest_box_init(u32 box)
ext 0286DC38 void actMngGuest_box_transform(u32 out, u32 in, u32 mtx)
ext 0253EC0C s32 actMngGuest_dEvt_order(u32 evt, u32 type, u32 prio, u32 flag, u32 hind, u32 a1, u32 a2, u32 evId, u32 info)
ext 0253ED80 s32 actMngGuest_dEvt_orderOld(u32 evt, u32 type, u32 prio, u32 flag, u32 hind, u32 a1, u32 a2, u32 name)
ext 0253EE04 u32 actMngGuest_dEvt_convPId(u32 evt, u32 id)
ext 02543F10 u32 actMngGuest_dEvmng_getEventIdx(u32 mng, u32 name, u32 room)
ext 025445F4 u32 actMngGuest_dEvmng_getEventPrio(u32 mng, u32 idx)
ext 025D5834 u32 actMngGuest_fopAcM_create(u32 name, u32 prm, u32 pos, u32 room, u32 angle, u32 scale, u32 arg, u32 fn)
ext 025D5928 u32 actMngGuest_fopAcM_fastCreate(u32 name, u32 prm, u32 pos, u32 room, u32 angle, u32 scale, u32 arg, u32 fn, u32 data)
ext 02551150 u32 actMngGuest_check_itemno(u32 itemNo)
ext 02551108 u32 actMngGuest_getItemNoByLife(u32 itemNo)
ext 025510E8 BOOL actMngGuest_isHeart(u32 itemNo)
ext 0201A478 void actMngGuest_csXyz_ct(u32 p, s16 x, s16 y, s16 z)
ext 028E8D88 void actMngGuest_PSVECAdd(u32 a, u32 b, u32 out)
ext 027200D0 u32 actMngGuest_save_sub(u32 p)
ext 0271FC5C BOOL actMngGuest_heroMode(u32 p)
ext 025D8AB0 u32 actMngGuest_fopAcM_fastCreateItem(u32 pos, u32 itemNo, u32 room, u32 angle, u32 scale, f32 sF, f32 sY, f32 g, u32 bit, u32 fn)
ext 025D8120 u32 actMngGuest_fopAcM_createItemFromTable(u32 pos, u32 tbl, u32 bit, u32 room, u32 type, u32 angle, u32 action, u32 scale)
ext 025512A4 u32 actMngGuest_getEmonoItemFromLifeBallTable(u32 t)
ext 02551684 u32 actMngGuest_getItemFromLifeBallTableWithoutEmono(u32 t)
ext 0255106C BOOL actMngGuest_isLimitedItem(u32 i)
ext 02551080 BOOL actMngGuest_isNonSavedEmono(u32 i)
ext 025B8DC8 BOOL actMngGuest_dSv_memBit_isSwitch(u32 p, u32 bit, s32 room)
ext 025BA494 BOOL actMngGuest_dSv_info_isItem(u32 p, u32 bit, s32 room)
ext 0200EA74 s32 actMngGuest_cDT_GetInf(u32 tbl, u32 field, u32 idx)
ext 025261C0 void actMngGuest_daIball_remove_old()
ext 025D4604 BOOL actMngGuest_fopAc_IsActor(u32 p)
ext 025DFB28 u32 actMngGuest_fpcM_JudgeInLayer(u32 layer, u32 fn, u32 data)
ext 02008E0C void actMngGuest_cBgS_GndChk_ct(u32 p)
ext 02008DAC void actMngGuest_cBgS_Chk_dt(u32 p, s32 f)
ext 02008974 f32 actMngGuest_cBgS_GroundCross(u32 bgs, u32 chk)
ext 020195B0 s16 actMngGuest_cM_atan2s(f32 y, f32 x)
ext 0200F378 void actMngGuest_cLib_addCalcAngleS(u32 p, u32 target, s32 scale, s32 maxStep, s32 minStep)
ext 025D537C void actMngGuest_fopAcM_setStageLayer(u32 a)
ext 025D5418 void actMngGuest_fopAcM_setRoomLayer(u32 a, s32 room)
ext 0255F530 u32 actMngGuest_dKy_Sound_get()
ext 024F22DC void actMngGuest_dBgS_WtrChk_ct(u32 p)
ext 028F026C void actMngGuest___register_global_object(u32 d)
ext 024EF7C0 BOOL actMngGuest_dBgS_SplGrpChk(u32 bgs, u32 chk)
ext 02520C0C BOOL actMngGuest_dComIfGs_checkGetItem(u32 i)
ext 0273AF40 void actMngGuest_operator_delete(u32 p)
*/
