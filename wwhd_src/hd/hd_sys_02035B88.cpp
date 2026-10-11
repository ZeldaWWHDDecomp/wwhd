/** HD HOME-menu controller, ProcUI callbacks, and root-scene task companions.
 * No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_sys_02035B88.h"

static u32 sRootSceneTypeInitialized;
static u32 sRootSceneParentTypeInitialized;
static hdTaskRuntimeTypeInfo sRootSceneType;
static hdTaskRuntimeTypeInfo sRootSceneParentType;
static hdHomeMenu_c sHomeStorage;
static u32 sHomeInitialized;
static hdHomeMenu_c* sHomeInstance;
static u8 sSavedDimEnabled;
static void* sErrorViewer;
enum : u32 {
    GAME_TASK_VTABLE_ADDRESS = 0x100050D8,
    ROOT_SCENE_TYPE_NAME_ADDRESS = 0x10004FB4,
    ROOT_SCENE_PARENT_TYPE_NAME_ADDRESS = 0x10004FA4,
    ROOT_SCENE_TASK_SIZE = 0x1120,
};

/* 02035B88 */
// HD: release current-task registration, destroy the sead task base, then optionally free storage.
static void destroyHomeGameTask(hdHomeGameTask* i_this, s32 flags) {
    if (i_this != NULL) {
        i_this->__vtbl = GAME_TASK_VTABLE_ADDRESS;
        hdCurrentTaskRegistrationDestroy(&i_this->mCurrentTaskRegistration, 2);
        hdHomeGameTaskBaseDestroy(i_this, 0);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 02035BF4 */
// HD: empty companion after the GameTask destructor; original symbol unknown.
static void homeGameTaskEmptyCompanion() {
}

/* 02035BF8 */
// HD: accept the root-scene task type or its parent, initializing type records lazily.
static u32 isRootSceneTaskDerivedFrom(void* i_this, hdTaskRuntimeTypeInfo* typeInfo) {
    if (sRootSceneTypeInitialized == 0) {
        sRootSceneTypeInitialized = 1;
        sRootSceneType.mTypeNameAddress = ROOT_SCENE_TYPE_NAME_ADDRESS;
    }
    if (typeInfo == &sRootSceneType) {
        return TRUE;
    }
    if (sRootSceneParentTypeInitialized == 0) {
        sRootSceneParentTypeInitialized = 1;
        sRootSceneParentType.mTypeNameAddress = ROOT_SCENE_PARENT_TYPE_NAME_ADDRESS;
    }
    return typeInfo == &sRootSceneParentType ? TRUE : FALSE;
}

/* 02035C70 */
// HD: empty companion between the type check and factory; original symbol unknown.
static void rootSceneTaskEmptyCompanion() {
}

/* 02035C74 */
// HD: allocate the root-scene task from the construction argument's current heap.
static void* createRootSceneTask(hdTaskConstructArg* argument) {
    hdTaskHeapArray* heapArray = argument->mHeapArray;
    void* heap = heapArray->mHeaps[heapArray->mCurrentHeapIndex];
    void* task = hdHomeRootTaskAllocate(ROOT_SCENE_TASK_SIZE, heap, 4);
    if (task == NULL) {
        return NULL;
    }
    return hdRootSceneTaskConstruct(task, argument);
}

/* 02035CC8 */
// HD: allocate when necessary, enable both flags, and construct the mutex.
static hdHomeMenu_c* createHomeMenu(hdHomeMenu_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdHomeMenu_c*)operator_new(sizeof(hdHomeMenu_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mEnabled = TRUE;
    i_this->mAllowed = TRUE;
    hdHomeMenuMutexConstruct(&i_this->mMutex);
    return i_this;
}

/* 02035D1C */
// HD: deleting destructor; the contained mutex receives its non-deleting flag.
static void destroyHomeMenu(hdHomeMenu_c* i_this, s32 flags) {
    if (i_this != NULL) {
        hdHomeMenuMutexDestroy(&i_this->mMutex, 2);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 02035D70 */
static u32 isHomeMenuAllowed(hdHomeMenu_c* i_this) {
    return i_this->mAllowed;
}

/* 02035D78 */
static hdHomeMenu_c* getHomeMenuInstance() {
    return sHomeInstance;
}

/* 02035D84 */
// HD: show the error viewer's notice for a denied HOME request when the menu is allowed.
static u32 onHomeMenuDenied() {
    hdHomeMenu_c* menu = getHomeMenuInstance();
    if (isHomeMenuAllowed(menu) != 0) {
        hdShowHomeDeniedNotice(sErrorViewer);
    }
    return 0;
}

/* 02035DC0 */
// HD: save dimming state on foreground release, then enable it; retain both singleton lookups.
static u32 onReleaseForeground() {
    u8 enabled = (u8)isScreenDimEnabled(getScreenDimInstance());
    sSavedDimEnabled = enabled;
    setScreenDimEnabled(getScreenDimInstance(), TRUE);
    return 0;
}

/* 02035DFC */
// HD: restore the saved dimming state on foreground acquisition.
static u32 onAcquireForeground() {
    hdScreenDim_c* dim = getScreenDimInstance();
    setScreenDimEnabled(dim, sSavedDimEnabled);
    return 0;
}

/* 02035E2C */
// HD: construct/register once, publish the singleton, then register all three ProcUI callbacks.
static void initializeHomeMenu() {
    if (sHomeInitialized == 0) {
        sHomeInitialized = 1;
        createHomeMenu(&sHomeStorage);
        __register_global_object(0x1018F374);
    }
    sHomeInstance = &sHomeStorage;
    ProcUIRegisterCallback(HD_PROC_UI_HOME_DENIED_e, onHomeMenuDenied, NULL, 1);
    ProcUIRegisterCallback(HD_PROC_UI_RELEASE_FOREGROUND_e, onReleaseForeground, NULL, 1);
    ProcUIRegisterCallback(HD_PROC_UI_ACQUIRE_FOREGROUND_e, onAcquireForeground, NULL, 1);
}

/* 02035ED4 */
// HD: retain the original low-byte flag store and full-width nonzero test for the system call.
static void setHomeMenuEnabled(hdHomeMenu_c* i_this, u32 enabled) {
    hdHomeMenuMutexLock(&i_this->mMutex);
    i_this->mEnabled = (u8)enabled;
    OSEnableHomeButtonMenu(enabled != 0 ? TRUE : FALSE);
    hdHomeMenuMutexUnlock(&i_this->mMutex);
}

/* 02035F3C */
static void setHomeMenuAllowed(hdHomeMenu_c* i_this, u32 allowed) {
    i_this->mAllowed = (u8)allowed;
}

/* 02035F44 */
// HD: pi pair and byte objects precede the HOME controller; its zeroed header object follows it.
static void __sinit_hd_home_menu_cpp() {
    sinit_header_statics_rodata(0x1020092C, 0x1018F380, 0x10200978, 0x100050E8);
}

/* WWHD glue (tools/verify/autoglue.py): data and local HD bindings
 data sRootSceneTypeInitialized 101FD598
 data sRootSceneParentTypeInitialized 101FD594
 data sRootSceneType 101FD97C
 data sRootSceneParentType 101FD980
 data sHomeStorage 10200938
 data sHomeInitialized 10200988
 data sHomeInstance 1018F3A4
 data sSavedDimEnabled 1018F3A8
 data sErrorViewer 1018F2BC
 ext 020355A4 void hdCurrentTaskRegistrationDestroy(be<u8>*, s32)
 ext 027452C8 void hdHomeGameTaskBaseDestroy(hdHomeGameTask*, s32)
 ext 0273B050 void* hdHomeRootTaskAllocate(u32, void*, s32)
 ext 0260B7C4 void* hdRootSceneTaskConstruct(void*, hdTaskConstructArg*)
 ext 02760084 void hdHomeMenuMutexConstruct(hdHomeMenuMutex*)
 ext 02760168 void hdHomeMenuMutexDestroy(hdHomeMenuMutex*, s32)
 ext 027601BC void hdHomeMenuMutexLock(hdHomeMenuMutex*)
 ext 027601F0 void hdHomeMenuMutexUnlock(hdHomeMenuMutex*)
 ext 020330BC void hdShowHomeDeniedNotice(void*)
 ext 02032C6C hdScreenDim_c* getScreenDimInstance()
 ext 02032BD0 u32 isScreenDimEnabled(hdScreenDim_c*)
 ext 02032AD8 void setScreenDimEnabled(hdScreenDim_c*, u32)
 ext C0007890 void ProcUIRegisterCallback(u32, hdHomeProcUICallback, void*, u32)
 ext C0009B18 void OSEnableHomeButtonMenu(u32)
*/
