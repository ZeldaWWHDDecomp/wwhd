/** Background line query setup. */
#include "d/d_bg_s_lin_chk.h"

static const f32 headerPiMin = -3.1415927;
static const f32 headerPiMax = 3.1415927;

/* 024F1AFC */
void dBgS_LinChk::Set(cXyz *pi_start, cXyz *pi_end, fopAc_ac_c *pi_actor) {
    u32 id = pi_actor != NULL ? (u32)pi_actor->mBsPcId : fpcM_ERROR_PROCESS_ID_e;
    SetLineCheck(this, pi_start, pi_end, id);
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
#define headerStatics (*gabi::at<HeaderStatics>(0x1046ED44))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x1046ED50))
/* 024F1B1C */
static void __sinit_d_bg_s_lin_chk_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101D5294));
    f32 lo = headerPiMin, hi = headerPiMax;
    headerStatics.piMin = lo;
    headerStatics.piMax = hi;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D52A0));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D52AC));
}

/* WWHD glue (tools/verify/autoglue.py)
data headerPiMin 10043B98
data headerPiMax 10043B9C
ext 0200909C void SetLineCheck(dBgS_LinChk*,cXyz*,cXyz*,u32)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
