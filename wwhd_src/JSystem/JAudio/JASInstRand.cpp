// Instrument random modulation, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASInstRand.h"
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
using RandomPointer = gptr<void>;
static RandomPointer sRandom;
extern u32 nextInstrumentRandom(void*);
extern void registerRandomGlobals(void*);
/* 02812440 */
f32 JASystem::TInstRand::getY(s32, s32) const {
    // HD: a shared generator supplies an unsigned unit value; no symmetric remap.
    u32 random = nextInstrumentRandom(sRandom);
    f32 unit = fmuls_ppc((f32)random, 2.3283064365386963e-10f);
    f32 width = mWidth;
    f32 base = mBase;
    return fmadds(unit, width, base);
}
/* 028124AC */
void __sinit_JASInstRand_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerRandomGlobals(&sRegistration);
}
/* WWHD glue
data sInitialization 104B5308
data sRegistration 101F9E0C
data sRandom 101F8B60
ext 0275CEE4 u32 nextInstrumentRandom(void*)
ext 028F026C void registerRandomGlobals(void*)
*/
