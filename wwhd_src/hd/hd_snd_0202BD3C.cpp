/** HD sound-manager singleton and sound-handle holder. No GameCube counterpart.
 * Private decomp source; subsystem and two handle-forwarder names remain descriptive. */
#include "bindings.h"
#include "hd/hd_snd_0202BD3C.h"
static hdSoundManager* sSoundManager;
static hdSoundManager* sSoundManagerDisposer;

/* 0202BD3C */
static void connectSoundManager(hdSoundManager* i_this) {
    void* subsystem = hdSoundManagerGetFirstSubsystem(i_this->mContainer);
    hdSoundManagerSetFirstSubsystem(i_this->mPlayer, subsystem);
    subsystem = hdSoundManagerGetSecondSubsystem(i_this->mContainer);
    hdSoundManagerSetSecondSubsystem(i_this->mPlayer, subsystem);
    subsystem = hdSoundManagerGetThirdSubsystem(i_this->mContainer);
    hdSoundManagerSetThirdSubsystem(i_this->mPlayer, subsystem);
    subsystem = hdSoundManagerGetFifthSubsystem(i_this->mContainer);
    hdSoundManagerSetLastSubsystem(i_this->mPlayer, subsystem);
    // HD: both final container entries use the same player setter.
    subsystem = hdSoundManagerGetSixthSubsystem(i_this->mContainer);
    hdSoundManagerSetLastSubsystem(i_this->mPlayer, subsystem);
}

/* 0202BDC8 */
static hdSoundManager* constructSoundManager(hdSoundManager* i_this) {
    hdSoundManager* manager = i_this;
    if (manager == NULL) {
        manager = (hdSoundManager*)operator_new(sizeof(hdSoundManager));
        if (manager == NULL) {
            return NULL;
        }
    }
    manager->mContainer = NULL;
    manager->mPlayer = NULL;
    hdSoundManagerContainer* container = (hdSoundManagerContainer*)operator_new(sizeof(hdSoundManagerContainer));
    if (container != NULL) {
        container = hdSoundManagerConstructContainer(container, NULL);
    }
    manager->mContainer = container;
    hdSoundManagerPlayer* player = (hdSoundManagerPlayer*)operator_new(sizeof(hdSoundManagerPlayer));
    if (player != NULL) {
        player = hdSoundManagerConstructPlayer(player);
    }
    manager->mPlayer = player;
    // HD: connect has a void entry, but this call forwards its resulting r3 to path initialization.
    u32 result = hdSoundManagerConnectResult(manager);
    hdSoundManagerBuildPaths(result);
    return manager;
}

/* 0202BE64 */
static hdSoundManager* createSoundManager(void* heap) {
    hdSoundManager* existing = sSoundManager;
    if (existing != NULL) {
        return existing;
    }
    hdSoundManager* disposer = (hdSoundManager*)hdSoundManagerAllocate(sizeof(hdSoundManager), heap, 4);
    if (disposer != NULL) {
        hdSoundManagerConstructDisposer(disposer, heap, 3);
        disposer->mDisposerVtable = 0x100040B8;
    }
    sSoundManagerDisposer = disposer;
    hdSoundManager* manager = disposer;
    if (disposer != NULL) {
        manager = constructSoundManager(disposer);
    }
    sSoundManager = manager;
    return manager;
}

/* 0202BEF8 */
static void destroySoundManager(hdSoundManager* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    hdSoundManagerContainer* container = i_this->mContainer;
    if (container != NULL) {
        hdSoundManagerDestroyContainer(container, 3);
        i_this->mContainer = NULL;
    }
    hdSoundManagerPlayer* player = i_this->mPlayer;
    if (player != NULL) {
        operator_delete(player);
        i_this->mPlayer = NULL;
    }
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0202BF78 */
static void* getSoundManagerSecondSubsystem(hdSoundManager* i_this) {
    return hdSoundManagerGetSecondSubsystem(i_this->mContainer);
}

/* 0202BF80 */
static void* getSoundManagerThirdSubsystem(hdSoundManager* i_this) {
    return hdSoundManagerGetThirdSubsystem(i_this->mContainer);
}

/* 0202BF88 */
static void* getSoundManagerFourthSubsystem(hdSoundManager* i_this) {
    return hdSoundManagerGetFourthSubsystem(i_this->mContainer);
}

/* 0202BF90 */
static void* getSoundManagerSixthSubsystem(hdSoundManager* i_this) {
    return hdSoundManagerGetSixthSubsystem(i_this->mContainer);
}

/* 0202BF98 */
static void* getSoundManagerFifthSubsystem(hdSoundManager* i_this) {
    return hdSoundManagerGetFifthSubsystem(i_this->mContainer);
}

/* 0202BFA0 */
static void destroySoundManagerDisposer(hdSoundManager* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mDisposerVtable = 0x100040B8;
    if (i_this == sSoundManagerDisposer) {
        hdSoundManager* manager = sSoundManager;
        sSoundManagerDisposer = NULL;
        destroySoundManager(manager, 2);
        sSoundManager = NULL;
    }
    hdSoundManagerDestroyDisposer(i_this, 0);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0202C03C */
static void __sinit_hd_sound_manager_cpp() {
    sinit_header_statics_rodata(0x101FFD80, 0x1018EC40, 0x101FFD8C, 0x100040B0);
}

/* 0202C0D0 */
static hdSoundHandleHolder* constructSoundHandle(hdSoundHandleHolder* i_this) {
    hdSoundHandleHolder* holder = i_this;
    if (holder == NULL) {
        holder = (hdSoundHandleHolder*)operator_new(sizeof(hdSoundHandleHolder));
        if (holder == NULL) {
            return NULL;
        }
    }
    holder->field_0x8 = 0;
    holder->mHandle = NULL;
    holder->field_0x10 = 0;
    holder->mVtable = 0x100040D0;
    holder->mId = -1;
    return holder;
}

/* 0202C128 */
static u32 stopSoundHandle(hdSoundHandleHolder* i_this, u32 argument, u32 option) {
    void* handle = i_this->mHandle;
    if (handle == NULL) {
        return 0;
    }
    return hdSoundHandleStop(handle, argument, option);
}

/* 0202C1A0 */
static u32 pauseSoundHandle(hdSoundHandleHolder* i_this, u32 argument, u32 option) {
    void* handle = i_this->mHandle;
    if (handle == NULL) {
        return 0;
    }
    return hdSoundHandlePause(handle, argument, option);
}

/* 0202C1C0 */
static u32 firstSoundHandleForward(hdSoundHandleHolder* i_this, u32 argument, u32 option) {
    void* handle = i_this->mHandle;
    if (handle == NULL) {
        return 0;
    }
    return hdSoundHandleFirstForward(handle, argument, option);
}

/* 0202C1D0 */
static u32 secondSoundHandleForward(hdSoundHandleHolder* i_this, u32 argument, u32 option) {
    void* handle = i_this->mHandle;
    if (handle == NULL) {
        return 0;
    }
    return hdSoundHandleSecondForward(handle, argument, option);
}

/* 0202C138 */
static void destroySoundHandle(hdSoundHandleHolder* i_this, s32 flags) {
    if (i_this == NULL) {
        return;
    }
    i_this->mVtable = 0x100040D0;
    // HD: only r3/r4 are supplied here; the thunk forwards the existing r5 register.
    hdSoundHandleStopForDestroy(i_this, 0);
    hdSoundHandleDestroy(&i_this->mHandle);
    if (flags & 1) {
        operator_delete(i_this);
    }
}

/* 0202C1B0 */
static u32 isSoundHandleValid(hdSoundHandleHolder* i_this) {
    return i_this->mHandle != NULL ? 1 : 0;
}

/* 0202C1E0 */
static void __sinit_hd_sound_handle_cpp() {
    sinit_header_statics_rodata(0x101FFD9C, 0x1018EC6C, 0x101FFDA8, 0x100040C8);
}

/* 0202C274 */
static u32 getSoundHandleId(hdSoundHandleHolder* i_this) {
    return i_this->mId;
}

/* WWHD glue (tools/verify/autoglue.py): private sound APIs and singleton state
 data sSoundManager 1018EC64
 data sSoundManagerDisposer 1018EC68
 ext 0202BA00 hdSoundManagerContainer* hdSoundManagerConstructContainer(hdSoundManagerContainer*, void*)
 ext 0202BB58 void hdSoundManagerDestroyContainer(hdSoundManagerContainer*, s32)
 ext 02761BD8 hdSoundManagerPlayer* hdSoundManagerConstructPlayer(hdSoundManagerPlayer*)
 ext 0202BC78 void* hdSoundManagerGetFirstSubsystem(hdSoundManagerContainer*)
 ext 0202BC80 void* hdSoundManagerGetSecondSubsystem(hdSoundManagerContainer*)
 ext 0202BC88 void* hdSoundManagerGetThirdSubsystem(hdSoundManagerContainer*)
 ext 0202BC90 void* hdSoundManagerGetFourthSubsystem(hdSoundManagerContainer*)
 ext 0202BC98 void* hdSoundManagerGetFifthSubsystem(hdSoundManagerContainer*)
 ext 0202BCA0 void* hdSoundManagerGetSixthSubsystem(hdSoundManagerContainer*)
 ext 02761C74 void hdSoundManagerSetFirstSubsystem(hdSoundManagerPlayer*, void*)
 ext 02761C7C void hdSoundManagerSetSecondSubsystem(hdSoundManagerPlayer*, void*)
 ext 02761C84 void hdSoundManagerSetThirdSubsystem(hdSoundManagerPlayer*, void*)
 ext 02761C8C void hdSoundManagerSetLastSubsystem(hdSoundManagerPlayer*, void*)
 ext 0273B0D4 void* hdSoundManagerAllocate(u32, void*, u32)
 ext 02752B0C void hdSoundManagerConstructDisposer(hdSoundManager*, void*, u32)
 ext 02752BEC void hdSoundManagerDestroyDisposer(hdSoundManager*, u32)
 ext 0202BD3C u32 hdSoundManagerConnectResult(hdSoundManager*)
 ext 0202DD2C void hdSoundManagerBuildPaths(u32)
 ext 02883FA8 u32 hdSoundHandleStop(void*, u32, u32)
 ext 028850D4 u32 hdSoundHandlePause(void*, u32, u32)
 ext 02885914 u32 hdSoundHandleFirstForward(void*, u32, u32)
 ext 028859A0 u32 hdSoundHandleSecondForward(void*, u32, u32)
 ext 0202C128 void hdSoundHandleStopForDestroy(hdSoundHandleHolder*, u32)
 ext 02896CD8 void hdSoundHandleDestroy(gptr<void>*)
*/
