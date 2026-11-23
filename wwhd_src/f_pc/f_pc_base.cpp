/**
 * f_pc_base.cpp
 * Process framework - base process (create, execute, delete)
 *
 * WWHD (cking.rpx), written in the style of the GameCube decompilation (zeldaret/tww) and verified
 * function by function against the recompiled original (tools/verify, unit f_pc_base). Each
 * function is marked with its WWHD address; the glue block at the end lists this translation
 * unit's data (tools/verify/autoglue.py).
 */

#include "f_pc/f_pc_base.h"
#include "f_pc/f_pc_t60_exec_proc.h"
#include "gc_api/SSystem/c_malloc.h"
#include "gc_api/f_pc/f_pc.h"

enum : u32 { base_process_class__vt = 0x10057E58 }; /* HD: base_process_class's vtable */

/* 025DD258 */
BOOL fpcBs_Is_JustOfType(int i_typeA, int i_typeB) {
    if (i_typeB == i_typeA) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static int g_fpcBs_type;

/* 025DD268 */
int fpcBs_MakeOfType(be<int>* i_type) {
    static s32 t_type = 0x9130000;
    if (*i_type == 0) {
        *i_type = ++t_type;
    }
    return *i_type;
}

/* true60 test aid WWHD_TEST_PROC_ID_BASE (port savestate.cpp, default off): a portable state load sets the next process id,
 * so the new stage's processes get the same ids in both runs of a pair whatever the boot created before the load (the
 * title's short-lived processes, 01C1, count with the async loading time; id-derived values such as the ChuChu's chase
 * angle offset then differed). A weak definition (0: no change): the port's own definition (t60.cpp) replaces it; the
 * verify harness links this one. */
#ifdef GABI_GAME_BUILD
/* the game build reads the port's definition (t60.cpp): a weak definition here let the link keep this copy (0) and the
 * compiler fold the check away (build32: fpcBs_MakeOfId without the load, the pairs' ids 0x24x and unequal) */
extern "C" uint32_t wwhd_test_proc_id_base;
#else
extern "C" __attribute__((weak)) uint32_t wwhd_test_proc_id_base = 0;
#endif

/* 025DD290 */
fpc_ProcID fpcBs_MakeOfId() {
    static fpc_ProcID process_id = 1;
    if (wwhd_test_proc_id_base != 0) {
        process_id = wwhd_test_proc_id_base;
        wwhd_test_proc_id_base = 0;
    }
    fpc_ProcID id = process_id;
    // HD: the next id skips 0xFFFFFFFE and fpcM_ERROR_PROCESS_ID_e (GameCube: process_id++)
    fpc_ProcID next = id;
    do {
        next++;
    } while (next == 0xFFFFFFFE || next == fpcM_ERROR_PROCESS_ID_e);
    process_id = next;
    return id;
}

/* 025DD2B8 */
BOOL fpcBs_Execute(base_process_class* i_proc) {
    BOOL result;
    layer_class* savedLayer = fpcLy_CurrentLayer();
    fpcLy_SetCurrentLayer(i_proc->mLyTg.mpLayer);
    result = fpcMtd_Execute(i_proc->mpPcMtd, i_proc);
    fpcLy_SetCurrentLayer(savedLayer);
    return result;
}

/* 025DD314 */
void fpcBs_DeleteAppend(base_process_class* i_proc) {
    if (i_proc->mpUserData != NULL) {
        cMl::free(i_proc->mpUserData);
        i_proc->mpUserData = NULL;
    }
}

/* 025DD354 */
BOOL fpcBs_IsDelete(base_process_class* i_proc) {
    BOOL result;
    layer_class* savedLayer = fpcLy_CurrentLayer();
    fpcLy_SetCurrentLayer(i_proc->mLyTg.mpLayer);
    result = fpcMtd_IsDelete(i_proc->mpPcMtd, i_proc);
    fpcLy_SetCurrentLayer(savedLayer);
    return result;
}

/* 025DD3B0 */
BOOL fpcBs_Delete(base_process_class* i_proc) {
    BOOL result = fpcMtd_Delete(i_proc->mpPcMtd, i_proc);
    if (result == TRUE) {
        fpcBs_DeleteAppend(i_proc);
        i_proc->mBsType = 0;
        cMl::free(i_proc);
    }
    return result;
}

/* 025DD414 */
base_process_class* fpcBs_Create(s16 i_profName, fpc_ProcID i_procID, void* i_data) {
    process_profile_definition* procProfDef;
    base_process_class* procClass;
    u32 size;

    procProfDef = fpcPf_Get(i_profName);
    JUT_ASSERT_MSG(484, (procProfDef->mSize & 0x3) == 0 && (procProfDef->mSizeOther & 0x3) == 0, "(profile->size & 0x3) == 0 && (profile->heapSize & 0x3) == 0"); // HD
    size = procProfDef->mSize + procProfDef->mSizeOther;
    procClass = (base_process_class*)cMl::memalignB(-4, size);
    if (procClass == NULL) {
        return NULL;
    } else {
        sBs_ClearArea(procClass, size);
        fpcLyTg_Init(&procClass->mLyTg, procProfDef->mLayerID, procClass);
        fpcLnTg_Init(&procClass->mLnTg, procClass);
        fpcDtTg_Init(&procClass->mDtTg, procClass);
        fpcPi_Init(&procClass->mPi, procClass, procProfDef->mLayerID, procProfDef->mListID, procProfDef->mListPrio);
        procClass->mInitState = 0;
        procClass->mUnk0 = 0;
        procClass->mBsPcId = i_procID;
        procClass->mProfName = i_profName;
        procClass->mBsType = fpcBs_MakeOfType(&g_fpcBs_type);
        procClass->mProcName = procProfDef->mProcName;
        fpcPause_Init(procClass);
        procClass->mpPcMtd = procProfDef->sub_method;
        procClass->mpProf = procProfDef;
        procClass->mpUserData = i_data;
        procClass->mParameters = procProfDef->mParameters;
        return procClass;
    }
}

/* 025DD538 */
cPhs_State fpcBs_SubCreate(base_process_class* i_proc) {
    T60_EXEC_PROC(i_proc);  // (true60 test aid: draws in the create method keyed by the created process)
    switch (fpcMtd_Create(i_proc->mpPcMtd, i_proc)) {
    case cPhs_NEXT_e:
    case cPhs_COMPLEATE_e:
        fpcBs_DeleteAppend(i_proc);
        i_proc->mCreateResult = cPhs_NEXT_e;
        return cPhs_NEXT_e;
    case cPhs_INIT_e:
    case cPhs_LOADING_e:
        i_proc->mInitState = 1;
        i_proc->mCreateResult = cPhs_INIT_e;
        return cPhs_INIT_e;
    case cPhs_STOP_e:
        i_proc->mCreateResult = cPhs_STOP_e;
        return cPhs_STOP_e;
    case cPhs_ERROR_e:
    default:
        i_proc->mCreateResult = cPhs_ERROR_e;
        return cPhs_ERROR_e;
    }
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 025DD5F0 */
// base_process_class::base_process_class() (GHS: allocates when called with NULL)
static base_process_class* base_process_class__ct(base_process_class* i_this) {
    if (i_this == NULL) {
        i_this = (base_process_class*)operator_new(sizeof(base_process_class));
    }
    if (i_this != NULL) {
        i_this->__vtbl = base_process_class__vt;
    }
    return i_this;
}

/* 025DD630 */
// base_process_class::~base_process_class() (deleting)
static void base_process_class__dt(base_process_class* i_this, int flags) {
    if (i_this != NULL && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 025DD644 */
// __sinit_f_pc_base_cpp: the common header statics (the pi pair copied from .rodata)
static void __sinit_f_pc_base_cpp() {
    sinit_header_statics_rodata(0x1048A6A0, 0x101F3910, 0x1048A6AC, 0x10057E4C);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str  10057DF4                               # "f_pc_base.cpp"
str 10057E04 "(profile->size & 0x3) == 0 && (profile->heapSize & 0x3) == 0"  # HD: "(profile->size & 0x3) == 0 && (profile->heapSize & 0x3) == 0"
data g_fpcBs_type 101F3934
data t_type 101F3908
data process_id 101F390C
*/
