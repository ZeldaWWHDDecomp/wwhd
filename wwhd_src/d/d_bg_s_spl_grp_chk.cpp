/** Special-group collision query. */
#include "d/d_bg_s_spl_grp_chk.h"

static const f32 initialZero = 0.0f;
static const f32 initialRoof = 1000000000.0f;
static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
// HD: per-translation-unit GHS vtable copies.
enum : u32 {
    polyInfoVtable = 0x10043C2C,
    specialVtable = 0x10043C4C,
    checkVtable = 0x10043C5C,
    groupPassVtable = 0x10043C6C,
    polyPassVtable = 0x10043C7C
};

/* 024F2100 */
// HD: emitted empty base destructor has no live arguments.
static void dBgS_SplGrpChk_empty() {
}

/* 024F2104 */
// HD: GHS allocation and explicit base vtables, with verified initialization order.
static dBgS_SplGrpChk *dBgS_SplGrpChk__ct(dBgS_SplGrpChk *i_this) {
    if (i_this == NULL)
        i_this = (dBgS_SplGrpChk *)operator_new(sizeof(dBgS_SplGrpChk));
    if (i_this != NULL) {
        i_this->mPolyInfo.mBgIndex = 0x100;
        i_this->mPolyInfo.mActorId = fpcM_ERROR_PROCESS_ID_e;
        i_this->mPolyInfo.mpBgW = NULL;
        i_this->mPolyInfo.__vtbl = polyInfoVtable;
        i_this->mPolyInfo.mPolyIndex = 0xFFFF;
        constructBgCheck(&i_this->mCheck);
        i_this->mPolyPass.mbArrowThrough = 0;
        f32 zero = initialZero;
        i_this->mPolyPass.mbBoomerangThrough = 0;
        i_this->m_height = zero;
        i_this->mPolyPass.mbCamThrough = 0;
        f32 roof = initialRoof;
        i_this->mPolyPass.mbObjThrough = 0;
        i_this->mPolyPass.mbRopeThrough = 0;
        i_this->mCheck.__vtbl = checkVtable;
        i_this->mPolyPass.mbBombThrough = 0;
        i_this->m_roof = roof;
        i_this->mPolyPass.mbLinkThrough = 0;
        i_this->m_flags = 0;
        i_this->m_ground.x = zero;
        i_this->mCheck.mGrpPassChk = &i_this->mGrpPass;
        i_this->mCheck.mPolyPassChk = &i_this->mPolyPass;
        i_this->m_ground.z = zero;
        i_this->m_ground.y = zero;
        i_this->mPolyInfo.__vtbl = specialVtable;
        i_this->mGrpPass.__vtbl = groupPassVtable;
        i_this->mGrpPass.mGrp = 0;
        i_this->mPolyPass.__vtbl = polyPassVtable;
    }
    return i_this;
}

/* 024F220C */
void dBgS_SplGrpChk::Init() {
    u32 flags = (u32)m_flags & ~(FIND | IN);
    f32 height = m_ground.y;
    mPolyInfo.mpBgW = NULL;
    mPolyInfo.mActorId = fpcM_ERROR_PROCESS_ID_e;
    m_height = height;
    m_flags = flags;
    mPolyInfo.mBgIndex = 0x100;
    mPolyInfo.mPolyIndex = 0xFFFF;
}

// HD: compiler-generated initialization of this translation unit's header statics.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject;
    u8 padding[2];
};
struct HeaderZeroObject {
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x1046ED98))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x1046EDA4))
/* 024F2248 */
static void __sinit_d_bg_s_spl_grp_chk_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101D530C));
    f32 lo = headerPiMin, hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D5318));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D5324));
}

/* WWHD glue (tools/verify/autoglue.py)
data initialZero 10043C8C
data initialRoof 10043C90
data headerPiMin 10043C9C
data headerPiMax 10043CA0
ext 0273AD10 void* operator_new(u32)
ext 02008B60 void constructBgCheck(SplBgCheck*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
