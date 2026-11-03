// Instrument modulation target, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASInstEffect.h"
struct Initialization { be<u32> words[4]; };
static Initialization sInitialization;
struct Registration { be<u32> words[3]; };
static Registration sRegistration;
extern void registerEffectGlobals(void*);
/* 02812410 */
void JASystem::TInstEffect::setTarget(s32 target) {
    // HD: range assertions are absent.
    mTarget = (u8)target;
}
/* 02812418 */
void __sinit_JASInstEffect_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerEffectGlobals(&sRegistration);
}
/* WWHD glue
data sInitialization 104B52F8
data sRegistration 101F9E00
ext 028F026C void registerEffectGlobals(void*)
*/
