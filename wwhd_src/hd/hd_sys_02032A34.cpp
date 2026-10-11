/** HD screen-dimming switch and companion header initializer. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_sys_02032A34.h"

static hdScreenDim_c sDimStorage;
static u32 sDimInitialized;
static hdScreenDim_c* sDimInstance;

/* 02032A34 */
// HD: allocate when called without storage, then enable the flag and construct the mutex.
static hdScreenDim_c* createScreenDim(hdScreenDim_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdScreenDim_c*)operator_new(sizeof(hdScreenDim_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mEnabled = TRUE;
    hdScreenDimMutexConstruct(&i_this->mMutex);
    return i_this;
}

/* 02032A84 */
// HD: deleting destructor; the mutex receives its non-deleting destructor flag.
static void destroyScreenDim(hdScreenDim_c* i_this, s32 flags) {
    if (i_this != NULL) {
        hdScreenDimMutexDestroy(&i_this->mMutex, 2);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 02032AD8 */
// HD: keep the mutex locked around the flag update and coreinit system call.
static void setScreenDimEnabled(hdScreenDim_c* i_this, u32 enabled) {
    hdScreenDimMutexLock(&i_this->mMutex);
    i_this->mEnabled = (u8)enabled;
    if (enabled != 0) {
        IMEnableDim();
    } else {
        IMDisableDim();
    }
    hdScreenDimMutexUnlock(&i_this->mMutex);
}

/* 02032B48 */
// HD: construct/register once, publish the instance, then synchronize with the system state.
static void syncScreenDimEnabled() {
    if (sDimInitialized == 0) {
        sDimInitialized = 1;
        createScreenDim(&sDimStorage);
        __register_global_object(0x1018F240);
    }
    sDimInstance = &sDimStorage;
    hdScreenDimSystemState state;
    IMIsDimEnabled(&state.mEnabled);
    setScreenDimEnabled(sDimInstance, state.mEnabled == 1 ? TRUE : FALSE);
}

/* 02032BD0 */
static u32 isScreenDimEnabled(hdScreenDim_c* i_this) {
    return i_this->mEnabled;
}

/* 02032BD8 */
// HD: the pi pair and byte objects precede the dim object; the zeroed header object follows it.
static void __sinit_hd_screen_dim_cpp() {
    sinit_header_statics_rodata(0x10200798, 0x1018F24C, 0x102007E4, 0x10004A50);
}

/* 02032C6C */
static hdScreenDim_c* getScreenDimInstance() {
    return sDimInstance;
}

/* 02032C78 */
// HD: the adjacent initializer-only translation unit's header statics.
static void __sinit_hd_screen_dim_companion_cpp() {
    sinit_header_statics_rodata(0x102007F8, 0x1018F274, 0x10200804, 0x10004A58);
}

/* WWHD glue (tools/verify/autoglue.py): data and local HD system calls
 data sDimStorage 102007A4
 data sDimInitialized 102007F4
 data sDimInstance 1018F270
 ext 02760084 void hdScreenDimMutexConstruct(hdScreenDimMutex*)
 ext 02760168 void hdScreenDimMutexDestroy(hdScreenDimMutex*, s32)
 ext 027601BC void hdScreenDimMutexLock(hdScreenDimMutex*)
 ext 027601F0 void hdScreenDimMutexUnlock(hdScreenDimMutex*)
 ext C0009020 void IMEnableDim()
 ext C0009010 void IMDisableDim()
 ext C0009060 void IMIsDimEnabled(be<u32>*)
*/
