/** HD HOME-menu controller views. */
#pragma once
#include "wwhd.h"
#include "hd/hd_sys_02035444.h"

struct hdHomeGameTask {
    u8 _padding00[0x20];
    be<u32> __vtbl;
    u8 _padding24[0xA4];
    be<u8> mCurrentTaskRegistration;
    u8 _paddingC9[3];
};
WWHD_OFFSET(hdHomeGameTask, __vtbl, 0x20);
WWHD_OFFSET(hdHomeGameTask, mCurrentTaskRegistration, 0xC8);
WWHD_SIZE(hdHomeGameTask, 0xCC);
struct hdHomeMenuMutex {
    u8 mStorage[0x3C]; // Opaque sead mutex; only passed to its out-of-unit methods.
};
struct hdHomeMenu_c {
    be<u8> mEnabled;
    be<u8> mAllowed;
    u8 _padding02[2];
    hdHomeMenuMutex mMutex;
};
WWHD_OFFSET(hdHomeMenu_c, mAllowed, 1);
WWHD_OFFSET(hdHomeMenu_c, mMutex, 4);
WWHD_SIZE(hdHomeMenu_c, 0x40);
struct hdScreenDim_c;
using hdHomeProcUICallback = u32 (*)();
enum : u32 {
    HD_PROC_UI_ACQUIRE_FOREGROUND_e = 0,
    HD_PROC_UI_RELEASE_FOREGROUND_e = 1,
    HD_PROC_UI_HOME_DENIED_e = 5,
};
void hdCurrentTaskRegistrationDestroy(be<u8>* registration, s32 flags);
void hdHomeGameTaskBaseDestroy(hdHomeGameTask* task, s32 flags);
void* hdHomeRootTaskAllocate(u32 size, void* heap, s32 alignment);
void* hdRootSceneTaskConstruct(void* task, hdTaskConstructArg* argument);
void hdHomeMenuMutexConstruct(hdHomeMenuMutex* mutex);
void hdHomeMenuMutexDestroy(hdHomeMenuMutex* mutex, s32 flags);
void hdHomeMenuMutexLock(hdHomeMenuMutex* mutex);
void hdHomeMenuMutexUnlock(hdHomeMenuMutex* mutex);
void hdShowHomeDeniedNotice(void* viewer);
hdScreenDim_c* getScreenDimInstance();
u32 isScreenDimEnabled(hdScreenDim_c* dim);
void setScreenDimEnabled(hdScreenDim_c* dim, u32 enabled);
void ProcUIRegisterCallback(u32 event, hdHomeProcUICallback callback, void* data, u32 priority);
void OSEnableHomeButtonMenu(u32 enabled);
