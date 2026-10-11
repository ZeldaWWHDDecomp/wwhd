/** Resource command queue. HD replaces DVD entries with paths and expands OS objects. */
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_list.h"

struct mDoDvdThd_commandVtable_c {
    u8 padding[0xC];
    be<u32> mDestroy;
    u8 padding10[4];
    be<u32> mExecute;
};
struct mDoDvdThd_command_c : node_class {
    /* 0x0C */ be<u8> mIsDone;
    u8 paddingD[3];
    /* 0x10 */ gptr<mDoDvdThd_commandVtable_c> mpVtable;
};
struct mDoDvdThd_callback_c : mDoDvdThd_command_c {
    /* 0x14 */ be<u32> mFunc;
    /* 0x18 */ gptr<void> mUserData;
    /* 0x1C */ gptr<void> mResult;
};
// HD: sead string views hold a pointer and vtable; path buffers add capacity and inline storage.
struct mDoDvdThd_stringVtable_c {
    u8 padding[0x14];
    be<u32> mAssureTermination;
};
struct mDoDvdThd_pathView_c {
    gptr<be<u8>> mData;
    gptr<mDoDvdThd_stringVtable_c> mpVtable;
};
struct mDoDvdThd_pathBuffer_c : mDoDvdThd_pathView_c {
    be<s32> mCapacity;
    be<u8> mBuffer[256];
};
struct mDoDvdThd_toMainRam_c : mDoDvdThd_command_c {
    /* 0x014 */ be<u8> mAllocDirection;
    /* 0x015 */ be<u8> mPath[257];
    u8 padding116[2];
    /* 0x118 */ gptr<void> mData;
    /* 0x11C */ be<u32> mDataSize;
    /* 0x120 */ gptr<void> mHeap;
    /* 0x124 */ mDoDvdThd_pathBuffer_c mPathString;
};
struct mDoDvdThd_mutex_c { u8 opaque[0x2C]; };
struct mDoDvdThd_param_c {
    /* 0x00 */ u8 mMessageQueue[0x3C];
    /* 0x3C */ u8 mMessageBuf[0x10];
    /* 0x4C */ node_list_class mCommandList;
    /* 0x58 */ mDoDvdThd_mutex_c mMutex;
    void kick();
    void addition(void* command);
    void cut(void* command);
    void* getFirstCommand();
    void mainLoop();
};
struct mDoDvdThd_threadVtable_c {
    u8 padding[0x1C];
    be<u32> mWake;
    u8 padding20[0xC];
    be<u32> mStart;
    u8 padding30[0x1C];
    be<u32> mRun;
};
struct mDoDvdThd_thread_c {
    u8 padding0[0xC];
    /* 0x0C */ gptr<mDoDvdThd_threadVtable_c> mpVtable;
    u8 padding10[8];
    /* 0x18 */ be<u32> mDestructorVtable; // Base-vtable role is inferred from the destructor store.
    u8 padding1C[0x94 - 0x1C];
};
struct mDoDvdThd_path_c { be<u8> bytes[257]; };
struct mDoDvdThd_data_c { u8 unused; };
struct mDoDvdThd_headerStatics_c {
    be<f32> mFloatPair[2];
    be<u8> mFlag, mByte;
    u8 padding[2];
    be<u32> mZeroWords[4];
};
struct mDoDvdThd_staticRecord_c { u8 opaque[12]; };
struct mDoDvdThd_staticRecords_c { mDoDvdThd_staticRecord_c mFirst, mSecond, mThird; };
WWHD_SIZE(mDoDvdThd_command_c, 0x14);
WWHD_SIZE(mDoDvdThd_callback_c, 0x20);
WWHD_SIZE(mDoDvdThd_pathView_c, 8);
WWHD_SIZE(mDoDvdThd_pathBuffer_c, 0x10C);
WWHD_OFFSET(mDoDvdThd_toMainRam_c, mPath, 0x15);
WWHD_OFFSET(mDoDvdThd_toMainRam_c, mData, 0x118);
WWHD_OFFSET(mDoDvdThd_toMainRam_c, mPathString, 0x124);
WWHD_SIZE(mDoDvdThd_toMainRam_c, 0x230);
WWHD_OFFSET(mDoDvdThd_param_c, mCommandList, 0x4C);
WWHD_OFFSET(mDoDvdThd_param_c, mMutex, 0x58);
WWHD_SIZE(mDoDvdThd_param_c, 0x84);
WWHD_SIZE(mDoDvdThd_thread_c, 0x94);
WWHD_SIZE(mDoDvdThd_path_c, 257);
WWHD_SIZE(mDoDvdThd_headerStatics_c, 0x1C);
WWHD_SIZE(mDoDvdThd_staticRecords_c, 36);

// HD: Dynamic call sites pass the live caller registers beyond their nominal parameters.
inline s32 mDoDvdThd_executeCommand(mDoDvdThd_command_c* command) {
    u32 target = command->mpVtable->mExecute;
    return gabi::call_ptr<s32>(target, command, gabi::cpu->r[4], gabi::cpu->r[5], gabi::cpu->r[6],
        gabi::cpu->r[7], gabi::cpu->r[8], gabi::cpu->r[9], gabi::cpu->r[10]);
}
inline void mDoDvdThd_assureTermination(mDoDvdThd_pathView_c* view, mDoDvdThd_stringVtable_c* table) {
    u32 target = table->mAssureTermination;
    gabi::call_ptr<void>(target, view, gabi::cpu->r[4], gabi::cpu->r[5], gabi::cpu->r[6],
        gabi::cpu->r[7], gabi::cpu->r[8], gabi::cpu->r[9], gabi::cpu->r[10]);
}
void cNd_ForcedClear(node_class*);
void* mDoDvd_alloc(u32);
void* mDoDvd_heapAlloc(u32, void*, s32);
void mDoDvd_free(void*);
void mDoDvd_normalizePath(void*, void*);
void mDoDvd_copy(void*, void*, u32);
void mDoDvd_move(void*, void*, s32, s32);
void mDoDvd_clear(void*, u32);
void mDoDvd_initMutex(mDoDvdThd_mutex_c*);
void mDoDvd_lockMutex(mDoDvdThd_mutex_c*);
void mDoDvd_unlockMutex(mDoDvdThd_mutex_c*);
void mDoDvd_reportError(const char*);
void* mDoDvd_getCommandHeap();
void* mDoDvd_getArchiveHeap();
void* mDoDvd_loadToMainRAM(void*, u32, u32, u32, void*, u32, u32, u32, u32);
u32 mDoDvd_getSize(void*, void*);
void mDoDvd_threadConstruct(mDoDvdThd_thread_c*, mDoDvdThd_pathView_c*, s32, void*, s32, s32, u32, u32);
void mDoDvd_threadDestruct(void*, s32);
void mDoDvd_registerStatic(void*);
void mDoDvd_initFlag(void*);
void mDoDvd_initByte(void*);
