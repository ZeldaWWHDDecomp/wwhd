/** Main startup and per-frame entry points; private decomp source. */
#pragma once
#include "m_Do/m_Do_machine.h"

// HD: startup record layout is verified; the four fields' roles remain unconfirmed.
struct mDoMain_StartupRecord {
    be<u32> field_0x0;
    be<u32> field_0x4;
    be<u32> field_0x8;
    be<u32> field_0xc;
};
WWHD_SIZE(mDoMain_StartupRecord, 0x10);
struct mDoMain_HeaderZeroRecord {
    be<u32> mWords[4];
};
void dComIfG_inf_c_ct(u8* gameInfo);
void* mDoExt_getGameHeap();
void mDoHIO_disabledEntry();
void mDoGph_Create();
void mReCPd_disabledCreate();
void mReCPd_disabledRead();
using mDoDvdThd_callback_func = s32 (*)(void*);
void mDoDvdThd_createCallback(mDoDvdThd_callback_func callback, void* argument);
void fapGm_Create();
void fapGm_Execute();
void mDoAud_Execute();
void mDoMch_HeapCheckAll();
void mDoMain_headerFirstByteConstruct(be<u8>* value);
void mDoMain_headerSecondByteConstruct(be<u8>* value);

struct mDoDvdThd_callback_c {
    static void create(mDoDvdThd_callback_func callback, void* argument) {
        mDoDvdThd_createCallback(callback, argument);
    }
};
