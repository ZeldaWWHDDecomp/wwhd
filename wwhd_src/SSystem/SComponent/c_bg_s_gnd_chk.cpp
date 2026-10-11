/** Ground collision check construction and destruction. */
#include "SSystem/SComponent/c_bg_s_gnd_chk.h"

static const f32 initialGroundHeight = 0.0f;
static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
// HD: explicit primary and polygon-info vtable copies.
enum : u32 { checkVtable = 0x10000F20, polyInfoVtable = 0x10000F30 };
struct VectorWords {
    be<u32> x, y, z;
};

/* 02008DAC */
// HD: restore the secondary vtable, invoke the base destructor and honor the GHS deletion bit.
static void cBgS_GndChk__dt(cBgS_GndChk *object, s32 deleting) {
    if (object == NULL)
        return;
    object->cBgS_PolyInfo::__vtbl = polyInfoVtable;
    destroyBaseCheck(object, 0);
    if ((u32)deleting & 1)
        operator_delete(object);
}

/* 02008E0C */
// HD: allocating constructor initializes the precheck state and height.
static cBgS_GndChk *cBgS_GndChk__ct(cBgS_GndChk *object) {
    if (object == NULL)
        object = (cBgS_GndChk *)operator_new(sizeof(cBgS_GndChk));
    if (object == NULL)
        return NULL;
    constructBaseCheck(object);
    object->mBgIndex = 0x100;
    object->cBgS_PolyInfo::__vtbl = polyInfoVtable;
    object->mGndPrecheck = 0;
    object->mWallPrecheck = 0;
    object->mFlag = 0;
    object->mpBgW = NULL;
    object->mNowY = initialGroundHeight;
    object->cBgS_Chk::__vtbl = checkVtable;
    object->mPolyIndex = 0xFFFF;
    object->mActorId = fpcM_ERROR_PROCESS_ID_e;
    const VectorWords *zero = (const VectorWords *)&cXyz::Zero;
    VectorWords *pos = (VectorWords *)&object->m_pos;
    pos->x = zero->x;
    pos->y = zero->y;
    u32 z = zero->z;
    object->mFlag = 3;
    pos->z = z;
    object->mActorPid = fpcM_ERROR_PROCESS_ID_e;
    return object;
}

// HD: compiler-generated initialization of this translation unit's header statics.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject, padding[2];
};
struct HeaderZeroObject {
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x101FF440))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x101FF44C))
/* 02008ED0 */
static void __sinit_c_bg_s_gnd_chk_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x1018C5B8));
    f32 lo = headerPiMin, hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x1018C5C4));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x1018C5D0));
}

/* WWHD glue (tools/verify/autoglue.py)
data initialGroundHeight 10000F0C
data headerPiMin 10000F14
data headerPiMax 10000F18
ext 0273AD10 void* operator_new(u32)
ext 0273AF40 void operator_delete(void*)
ext 02008B60 void constructBaseCheck(cBgS_Chk*)
ext 02008B4C void destroyBaseCheck(cBgS_Chk*,s32)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
