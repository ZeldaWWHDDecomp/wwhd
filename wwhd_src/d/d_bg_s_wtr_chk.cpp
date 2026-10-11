/** Water collision query. */
#include "d/d_bg_s_wtr_chk.h"

static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;
// HD: per-translation-unit GHS vtable copies.
enum : u32 {
    waterVtable = 0x10043CEC,
    checkVtable = 0x10043CFC,
    groupPassVtable = 0x10043D0C,
    polyPassVtable = 0x10043D1C
};

/* 024F22DC */
// HD: GHS allocation and base construction precede the water-group flag.
static dBgS_WtrChk *dBgS_WtrChk__ct(dBgS_WtrChk *i_this) {
    if (i_this == NULL)
        i_this = (dBgS_WtrChk *)operator_new(sizeof(dBgS_WtrChk));
    if (i_this != NULL) {
        constructSpecialGroupCheck(i_this);
        u32 flags = i_this->mGrpPass.mGrp;
        i_this->mPolyInfo.__vtbl = waterVtable;
        i_this->mPolyPass.__vtbl = polyPassVtable;
        i_this->mCheck.__vtbl = checkVtable;
        i_this->mGrpPass.mGrp = flags | SplGroupPassCheck::WATER_GRP;
        i_this->mGrpPass.__vtbl = groupPassVtable;
    }
    return i_this;
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
#define headerStatics (*gabi::at<HeaderStatics>(0x1046EDB4))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x1046EDC0))
/* 024F2360 */
static void __sinit_d_bg_s_wtr_chk_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101D5330));
    f32 lo = headerPiMin, hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D533C));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D5348));
}

/* WWHD glue (tools/verify/autoglue.py)
data headerPiMin 10043D34
data headerPiMax 10043D38
ext 0273AD10 void* operator_new(u32)
ext 024F2104 dBgS_SplGrpChk* constructSpecialGroupCheck(dBgS_SplGrpChk*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
