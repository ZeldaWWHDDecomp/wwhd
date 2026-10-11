/** HD resource command queue and path loading. */
#include "m_Do/m_Do_dvd_thread.h"

using DvdThreadPtr = gptr<mDoDvdThd_thread_c>;
using DvdHeapPtr = gptr<void>;
static DvdThreadPtr l_dvdThread;
static DvdHeapPtr l_dvdHeap;
static mDoDvdThd_param_c l_param;
static u8 sDefaultDirection;
static mDoDvdThd_data_c l_commandVtable;
static mDoDvdThd_data_c l_callbackVtable;
static mDoDvdThd_data_c l_ramVtable;
static mDoDvdThd_data_c l_pathViewVtable;
static mDoDvdThd_data_c l_pathBaseVtable;
static mDoDvdThd_data_c l_pathBufferVtable;
static mDoDvdThd_data_c l_pathFixedVtable;
static mDoDvdThd_data_c l_threadVtable;
static mDoDvdThd_data_c l_threadBaseVtable;
static mDoDvdThd_data_c l_commandFailureText;
static mDoDvdThd_data_c l_threadName;
static mDoDvdThd_headerStatics_c l_dvdHeaderStatics;
static mDoDvdThd_staticRecords_c l_dvdRecords;
static f32 l_dvdInitialFirst;
static f32 l_dvdInitialSecond;

/* 025E21A8 */
void mDoDvdThd_convertPath(void* dest, void* input) {
    gabi::Local<mDoDvdThd_path_c> path;
    mDoDvd_normalizePath(path.get(), input);
    mDoDvd_copy(dest, path.get(), sizeof(mDoDvdThd_path_c));
}

/* 025E21E8 */
s32 mDoDvdThd_commandCallback(void* address) {
    auto* slot = static_cast<gptr<mDoDvdThd_command_c>*>(address);
    auto* command = slot->get();
    s32 result = mDoDvdThd_executeCommand(command);
    if (result != TRUE) mDoDvd_reportError(gabi::at<const char>(gabi::ea(&l_commandFailureText)));
    return FALSE;
}

/* 025E2234 */
void* mDoDvdThd_command_construct(void* self) {
    if (!self) self = mDoDvd_alloc(sizeof(mDoDvdThd_command_c));
    if (self) {
        auto* command = static_cast<mDoDvdThd_command_c*>(self);
        command->mIsDone = false;
        command->mpVtable = gabi::at<mDoDvdThd_commandVtable_c>(gabi::ea(&l_commandVtable));
        cNd_ForcedClear(command);
    }
    return self;
}

/* 025E2290 */
void* mDoDvdThd_callback_construct(void* self, void* callback, void* user) {
    if (!self) self = mDoDvd_alloc(sizeof(mDoDvdThd_callback_c));
    if (self) {
        mDoDvdThd_command_construct(self);
        auto* command = static_cast<mDoDvdThd_callback_c*>(self);
        command->mFunc = gabi::ea(callback);
        command->mUserData = user;
        command->mpVtable = gabi::at<mDoDvdThd_commandVtable_c>(gabi::ea(&l_callbackVtable));
        command->mResult = nullptr;
    }
    return self;
}

/* 025E230C */
void mDoDvdThd_param_c::kick() {
    auto* thread = l_dvdThread.get();
    u32 target = thread->mpVtable->mWake;
    // HD: The thread wake call replaces GameCube's OS message submission.
    gabi::call_ptr<void>(target, thread, 1, 1, gabi::cpu->r[6], gabi::cpu->r[7],
        gabi::cpu->r[8], gabi::cpu->r[9], gabi::cpu->r[10]);
}

/* 025E232C */
void mDoDvdThd_param_c::addition(void* command) {
    mDoDvd_lockMutex(&mMutex);
    cLs_Addition(&mCommandList, static_cast<node_class*>(command));
    mDoDvd_unlockMutex(&mMutex);
    kick();
}

/* 025E2384 */
void* mDoDvdThd_callback_create(void* callback, void* user) {
    void* heap = mDoDvd_getCommandHeap();
    void* command = mDoDvd_heapAlloc(sizeof(mDoDvdThd_callback_c), heap, -4);
    if (!command) return nullptr;
    command = mDoDvdThd_callback_construct(command, callback, user);
    if (command) l_param.addition(command);
    return command;
}

/* 025E2404 */
BOOL mDoDvdThd_callback_execute(void* self) {
    auto* command = static_cast<mDoDvdThd_callback_c*>(self);
    u32 target = command->mFunc;
    void* user = command->mUserData;
    u32 result = gabi::call_ptr<u32>(target, user, gabi::cpu->r[4], gabi::cpu->r[5], gabi::cpu->r[6],
        gabi::cpu->r[7], gabi::cpu->r[8], gabi::cpu->r[9], gabi::cpu->r[10]);
    command->mResult = gabi::at<void>(result);
    command->mIsDone = true;
    return result != 0;
}

/* 025E2450 */
void* mDoDvdThd_toMainRam_construct(void* self, s32 direction) {
    if (!self) self = mDoDvd_alloc(sizeof(mDoDvdThd_toMainRam_c));
    if (self) {
        mDoDvdThd_command_construct(self);
        auto* command = static_cast<mDoDvdThd_toMainRam_c*>(self);
        command->mAllocDirection = (u8)direction;
        command->mpVtable = gabi::at<mDoDvdThd_commandVtable_c>(gabi::ea(&l_ramVtable));
        command->mPath[0] = 0;
        command->mHeap = nullptr;
        command->mData = nullptr;
        command->mDataSize = 0;
        // HD: Retain the null-constructor path even if the embedded guest address wraps to zero.
        u32 bufferAddress = gabi::ea(command) + offsetof(mDoDvdThd_toMainRam_c, mPathString);
        if (!bufferAddress) bufferAddress = gabi::ea(mDoDvd_alloc(sizeof(mDoDvdThd_pathBuffer_c)));
        if (bufferAddress) {
            auto* buffer = gabi::at<mDoDvdThd_pathBuffer_c>(bufferAddress);
            buffer->mData = buffer->mBuffer;
            buffer->mpVtable = gabi::at<mDoDvdThd_stringVtable_c>(gabi::ea(&l_pathBaseVtable));
            buffer->mCapacity = 256;
            buffer->mBuffer[255] = 0;
            auto* data = buffer->mData.get();
            buffer->mpVtable = gabi::at<mDoDvdThd_stringVtable_c>(gabi::ea(&l_pathBufferVtable));
            *data = 0;
            buffer->mpVtable = gabi::at<mDoDvdThd_stringVtable_c>(gabi::ea(&l_pathFixedVtable));
        }
        if (direction == 0) command->mAllocDirection = sDefaultDirection;
    }
    return self;
}

/* 025E26A8 */
BOOL mDoDvdThd_toMainRam_execute(void* self) {
    auto* command = static_cast<mDoDvdThd_toMainRam_c*>(self);
    void* heap = command->mHeap;
    if (!heap) heap = mDoDvd_getArchiveHeap();
    u32 direction = command->mAllocDirection == 0 ? 1 : 2;
    gabi::Local<mDoDvdThd_path_c> path;
    mDoDvd_copy(path.get(), command->mPath, sizeof(mDoDvdThd_path_c));
    void* data = mDoDvd_loadToMainRAM(path.get(), 0, 1, 0, heap, direction, 0, 0, 0);
    command->mData = data;
    if (data) {
        u32 size = mDoDvd_getSize(heap, data);
        command->mDataSize = size;
    }
    void* result = command->mData;
    command->mIsDone = true;
    return result != nullptr;
}

/* 025E276C */
void* mDoDvdThd_param_construct(void* self) {
    if (!self) self = mDoDvd_alloc(sizeof(mDoDvdThd_param_c));
    if (self) {
        auto* param = static_cast<mDoDvdThd_param_c*>(self);
        mDoDvd_clear(param->mMessageQueue, sizeof(param->mMessageQueue));
        mDoDvd_clear(param->mMessageBuf, sizeof(param->mMessageBuf));
        mDoDvd_clear(&param->mCommandList, sizeof(param->mCommandList));
        mDoDvd_clear(&param->mMutex, sizeof(param->mMutex));
        mDoDvd_initMutex(&param->mMutex);
        cLs_Create(&param->mCommandList);
    }
    return self;
}

/* 025E27EC */
void mDoDvdThd_param_c::cut(void* command) {
    mDoDvd_lockMutex(&mMutex);
    cLs_SingleCut(static_cast<node_class*>(command));
    mDoDvd_unlockMutex(&mMutex);
    kick();
}

/* 025E2840 */
void* mDoDvdThd_param_c::getFirstCommand() { return mCommandList.mpHead.get(); }

/* 025E2848 */
void mDoDvdThd_param_c::mainLoop() {
    // HD: One drain replaces the GameCube loop that waited for OS messages.
    using CommandSlot = gptr<mDoDvdThd_command_c>;
    gabi::Local<CommandSlot> command;
    void* next = getFirstCommand();
    *command.get() = static_cast<mDoDvdThd_command_c*>(next);
    while (next) {
        cut(next);
        mDoDvdThd_commandCallback(command.get());
        next = getFirstCommand();
        *command.get() = static_cast<mDoDvdThd_command_c*>(next);
    }
}

/* 025E29FC */
void mDoDvdThd_pathView_delete(void* self, s32 flags) {
    if (self && ((u32)flags & 1)) mDoDvd_free(self);
}
/* 025E2A28 */
void mDoDvdThd_pathBuffer_delete(void* self, s32 flags) {
    if (self && ((u32)flags & 1)) mDoDvd_free(self);
}
/* 025E2A3C */
void mDoDvdThd_toMainRam_delete(void* self, s32 flags) {
    if (self && ((u32)flags & 1)) mDoDvd_free(self);
}
/* 025E2A54 */
void mDoDvdThd_pathBufferBase_delete(void* self, s32 flags) {
    if (self && ((u32)flags & 1)) mDoDvd_free(self);
}
/* 025E2A78 */
void mDoDvdThd_pathFixed_delete(void* self, s32 flags) {
    if (self && ((u32)flags & 1)) mDoDvd_free(self);
}

/* 025E2A10 */
void mDoDvdThd_path_assureTermination(void* self) {
    auto* buffer = static_cast<mDoDvdThd_pathBuffer_c*>(self);
    u32 data = gabi::ea(buffer->mData.get());
    u32 size = buffer->mCapacity;
    // HD: Keep the PPC wrap for zero or negative capacities.
    *gabi::at<be<u8>>(data + size - 1) = 0;
}

/* 025E2A50 */
void mDoDvdThd_pathView_assureTermination(void* self) {}

/* 025E2A68 */
s32 mDoDvdThd_thread_run(void* self) {
    auto* thread = static_cast<mDoDvdThd_thread_c*>(self);
    u32 target = thread->mpVtable->mRun;
    return gabi::call_ptr<s32>(target, self, gabi::cpu->r[4], gabi::cpu->r[5], gabi::cpu->r[6],
        gabi::cpu->r[7], gabi::cpu->r[8], gabi::cpu->r[9], gabi::cpu->r[10]);
}

/* 025E2A8C */
void mDoDvdThd_mainLoop() { l_param.mainLoop(); }

/* 025E2A98 */
void mDoDvdThd_thread_delete(void* self, s32 flags) {
    if (self) {
        auto* thread = static_cast<mDoDvdThd_thread_c*>(self);
        thread->mDestructorVtable = gabi::ea(&l_threadBaseVtable);
        mDoDvd_threadDestruct(self, 0);
        if ((u32)flags & 1) mDoDvd_free(self);
    }
}

/* 025E255C */
void* mDoDvdThd_toMainRam_create(void* input, s32 direction, void* heap) {
    void* commandHeap = mDoDvd_getCommandHeap();
    void* allocation = mDoDvd_heapAlloc(sizeof(mDoDvdThd_toMainRam_c), commandHeap, -4);
    if (!allocation) return nullptr;
    auto* command = static_cast<mDoDvdThd_toMainRam_c*>(mDoDvdThd_toMainRam_construct(allocation, direction));
    if (!command) return nullptr;
    gabi::Local<mDoDvdThd_pathView_c> view;
    view->mData = static_cast<be<u8>*>(input);
    view->mpVtable = gabi::at<mDoDvdThd_stringVtable_c>(gabi::ea(&l_pathViewVtable));
    u32 buffer = gabi::ea(command->mPathString.mData.get());
    mDoDvdThd_pathView_assureTermination(view.get());
    u32 cursor = gabi::ea(view->mData.get());
    s32 length = 0;
    if (*gabi::at<be<u8>>(cursor)) {
        do {
            length++;
            cursor++;
            if (length > 0x40000) { length = 0; break; }
        } while (*gabi::at<be<u8>>(cursor));
    }
    s32 capacity = command->mPathString.mCapacity;
    auto* table = view->mpVtable.get();
    if (length >= capacity) length = (s32)((u32)capacity - 1);
    mDoDvdThd_assureTermination(view.get(), table);
    void* data = view->mData.get();
    mDoDvd_move(gabi::at<void>(buffer), data, length, 0);
    *gabi::at<be<u8>>(buffer + (u32)length) = 0;
    mDoDvdThd_convertPath(command->mPath, input);
    if (!command->mPath[0]) {
        auto* vtable = command->mpVtable.get();
        command->mIsDone = true;
        u32 target = vtable->mDestroy;
        // HD: The failed-path destructor explicitly leaves r8/r9/r10 as zero, one and its table.
        gabi::call_ptr<void>(target, command, 3, gabi::cpu->r[5], gabi::cpu->r[6], gabi::cpu->r[7],
                            0, 1, gabi::ea(vtable));
        return nullptr;
    }
    command->mHeap = heap;
    l_param.addition(command);
    return command;
}

/* 025E28B0 */
void mDoDvdThd_create() {
    void* heap = l_dvdHeap;
    auto* thread = static_cast<mDoDvdThd_thread_c*>(mDoDvd_alloc(sizeof(mDoDvdThd_thread_c)));
    if (thread) {
        gabi::Local<mDoDvdThd_pathView_c> view;
        view->mData = gabi::at<be<u8>>(gabi::ea(&l_threadName));
        view->mpVtable = gabi::at<mDoDvdThd_stringVtable_c>(gabi::ea(&l_pathViewVtable));
        mDoDvd_threadConstruct(thread, view.get(), 0, heap, 0, 0x7FFFFFFF, 0x5000, 0x20);
        thread->mpVtable = gabi::at<mDoDvdThd_threadVtable_c>(gabi::ea(&l_threadVtable));
    }
    l_dvdThread = thread;
    auto* table = thread->mpVtable.get();
    u32 target = table->mStart;
    gabi::call_ptr<void>(target, thread, gabi::cpu->r[4], gabi::cpu->r[5], gabi::cpu->r[6],
        gabi::cpu->r[7], gabi::cpu->r[8], gabi::cpu->r[9], gabi::ea(table));
}

// HD: Adjacent compiler startup initializer; attribution follows its parameter constructor call.
/* 025E295C */
void mDoDvdThd_staticInit() {
    l_dvdHeaderStatics.mZeroWords[2] = 0;
    l_dvdHeaderStatics.mZeroWords[0] = 0;
    l_dvdHeaderStatics.mZeroWords[3] = 0;
    l_dvdHeaderStatics.mZeroWords[1] = 0;
    mDoDvd_registerStatic(&l_dvdRecords.mFirst);
    f32 first = l_dvdInitialFirst, second = l_dvdInitialSecond;
    l_dvdHeaderStatics.mFloatPair[0] = first;
    l_dvdHeaderStatics.mFloatPair[1] = second;
    mDoDvd_initFlag(&l_dvdHeaderStatics.mFlag);
    mDoDvd_registerStatic(&l_dvdRecords.mSecond);
    mDoDvd_initByte(&l_dvdHeaderStatics.mByte);
    mDoDvd_registerStatic(&l_dvdRecords.mThird);
    mDoDvdThd_param_construct(&l_param);
}

/* WWHD glue
ext 0273AD10 void* mDoDvd_alloc(u32)
ext 0273B050 void* mDoDvd_heapAlloc(u32, void*, s32)
ext 0273AF40 void mDoDvd_free(void*)
ext 0284B7BC void mDoDvd_normalizePath(void*, void*)
ext C000A848 void mDoDvd_copy(void*, void*, u32)
ext C0009988 void mDoDvd_move(void*, void*, s32, s32)
ext 028F521C void mDoDvd_clear(void*, u32)
ext C0009D68 void mDoDvd_initMutex(mDoDvdThd_mutex_c*)
ext C0009E40 void mDoDvd_lockMutex(mDoDvdThd_mutex_c*)
ext C000A1C8 void mDoDvd_unlockMutex(mDoDvdThd_mutex_c*)
ext 025F2710 void mDoDvd_reportError(const char*)
ext 025EE060 void* mDoDvd_getCommandHeap()
ext 025E3268 void* mDoDvd_getArchiveHeap()
ext 027EBB2C void* mDoDvd_loadToMainRAM(void*, u32, u32, u32, void*, u32, u32, u32, u32)
ext 027EC134 u32 mDoDvd_getSize(void*, void*)
ext 027607C0 void mDoDvd_threadConstruct(mDoDvdThd_thread_c*, mDoDvdThd_pathView_c*, s32, void*, s32, s32, u32, u32)
ext 027609AC void mDoDvd_threadDestruct(void*, s32)
ext 02019C94 void cNd_ForcedClear(node_class*)
ext 0200FE78 s32 cLs_Addition(node_list_class*, node_class*)
ext 0200FDF4 s32 cLs_SingleCut(node_class*)
ext 02010008 void cLs_Create(node_list_class*)
ext 028F026C void mDoDvd_registerStatic(void*)
ext 028ED6F8 void mDoDvd_initFlag(void*)
ext 028EAB2C void mDoDvd_initByte(void*)
data l_dvdThread 1048CE3C
data l_dvdHeap 101F8B9C
data l_param 1048CE40
data sDefaultDirection 1048CEC4
data l_commandVtable 10058688
data l_callbackVtable 100586A0
data l_ramVtable 100586B8
data l_pathViewVtable 1005855C
data l_pathBaseVtable 10058574
data l_pathBufferVtable 1005859C
data l_pathFixedVtable 100585B4
data l_threadVtable 100585CC
data l_threadBaseVtable 1005858C
data l_commandFailureText 10058518
data l_threadName 10058674
data l_dvdHeaderStatics 1048CE20
data l_dvdRecords 101F472C
data l_dvdInitialFirst 10058680
data l_dvdInitialSecond 10058684
*/
