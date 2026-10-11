/** Base background collision checks. */
#include "SSystem/SComponent/c_bg_s_chk.h"
// HD: GHS base vtable copy.
enum : u32 { checkVtable = 0x10000E80 };

/* 02008B4C */
// HD: empty base deleting destructor tests the low GHS flag bit.
static void cBgS_Chk__dt(cBgS_Chk *object, s32 flags) {
    if (object != NULL && ((u32)flags & 1))
        operator_delete(object);
}

/* 02008B60 */
// HD: allocating constructor also initializes mActorPid to zero.
static cBgS_Chk *cBgS_Chk__ct(cBgS_Chk *object) {
    if (object == NULL)
        object = (cBgS_Chk *)operator_new(sizeof(cBgS_Chk));
    if (object != NULL) {
        object->mActorPid = 0;
        object->mPolyPassChk = NULL;
        object->mGrpPassChk = NULL;
        object->__vtbl = checkVtable;
        object->mSameActorChk = 1;
    }
    return object;
}

/* 02008BB8 */
u8 cBgS_Chk::ChkSameActorPid(u32 pid) {
    u32 actor = mActorPid;
    if (actor == fpcM_ERROR_PROCESS_ID_e || pid == fpcM_ERROR_PROCESS_ID_e || !mSameActorChk)
        return 0;
    return actor == pid;
}

// HD: compiler-generated statics; adjacent header-only translation units are hosted here.
struct HeaderGroup {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject, padding[2];
    be<u32> words[4];
};
WWHD_SIZE(HeaderGroup, 0x1C);
#define headerGroups (gabi::at<HeaderGroup>(0x101FF3EC))
static const f32 headerPiMin0 = -3.1415927;
static const f32 headerPiMax0 = 3.1415927;
/* 02008BF0 */
static void __sinit_c_bg_s_chk_cpp() {
    HeaderGroup *group = &headerGroups[0];
    group->words[2] = 0;
    group->words[0] = 0;
    group->words[3] = 0;
    group->words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C54C));
    f32 lo = headerPiMin0, hi = headerPiMax0;
    group->piMin = lo;
    group->piMax = hi;
    initializeHeaderByte(&group->byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C558));
    initializeHeaderFlag(&group->flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C564));
}
static const f32 headerPiMin1 = -3.1415927;
static const f32 headerPiMax1 = 3.1415927;
/* 02008C84 */
static void __sinit_collisionHeaderFirst() {
    HeaderGroup *group = &headerGroups[1];
    group->words[2] = 0;
    group->words[0] = 0;
    group->words[3] = 0;
    group->words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C570));
    f32 lo = headerPiMin1, hi = headerPiMax1;
    group->piMin = lo;
    group->piMax = hi;
    initializeHeaderByte(&group->byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C57C));
    initializeHeaderFlag(&group->flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C588));
}
static const f32 headerPiMin2 = -3.1415927;
static const f32 headerPiMax2 = 3.1415927;
/* 02008D18 */
static void __sinit_collisionHeaderSecond() {
    HeaderGroup *group = &headerGroups[2];
    group->words[2] = 0;
    group->words[0] = 0;
    group->words[3] = 0;
    group->words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C594));
    f32 lo = headerPiMin2, hi = headerPiMax2;
    group->piMin = lo;
    group->piMax = hi;
    initializeHeaderByte(&group->byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C5A0));
    initializeHeaderFlag(&group->flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C5AC));
}

/* WWHD glue (tools/verify/autoglue.py)
data headerPiMin0 10000E74
data headerPiMax0 10000E78
data headerPiMin1 10000EA0
data headerPiMax1 10000EA4
data headerPiMin2 10000EEC
data headerPiMax2 10000EF0
ext 0273AD10 void* operator_new(u32)
ext 0273AF40 void operator_delete(void*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
