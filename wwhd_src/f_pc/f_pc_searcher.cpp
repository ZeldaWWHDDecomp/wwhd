/** Process search callbacks. */
#include "bindings.h"
#include "f_pc/f_pc_base.h"

/* 025E121C */
static void* fpcSch_JudgeForPName(void* i_proc, be<s16>* i_data) {
    if (((base_process_class*)i_proc)->mProcName == *i_data) {
        return i_proc;
    }
    return NULL;
}

/* 025E1234 */
static void* fpcSch_JudgeByID(void* i_proc, be<fpc_ProcID>* i_data) {
    if (((base_process_class*)i_proc)->mBsPcId == *i_data) {
        return i_proc;
    }
    return NULL;
}

/* 025E124C */
// HD: this translation unit's header statics copy their pi pair from rodata.
static void __sinit_f_pc_searcher_cpp() {
    sinit_header_statics_rodata(0x1048AB84, 0x101F4674, 0x1048AB90, 0x1005848C);
}

/* WWHD glue (tools/verify/autoglue.py): no data bindings. */
