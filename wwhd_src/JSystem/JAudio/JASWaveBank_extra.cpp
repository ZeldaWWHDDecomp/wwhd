// Cafe wave-bank heap selection and static registration.
#include "JSystem/JAudio/JASWaveBank_extra.h"
using namespace JASystem;
using WaveHeapPointer = gptr<void>;
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static WaveHeapPointer sCurrentHeap;
static Initialization sWaveInitialization;
static Registration sWaveRegistration;
extern void registerWaveBankGlobals(void*);
/* 028452A4 */
void* TWaveBank::getCurrentHeap() {
    void* heap = sCurrentHeap;
    return heap ? heap : JASDram.get();
}
/* 028452C0 */
void __sinit_JASWaveBank_cpp() {
    sWaveInitialization.words[2] = 0;
    sWaveInitialization.words[0] = 0;
    sWaveInitialization.words[3] = 0;
    sWaveInitialization.words[1] = 0;
    registerWaveBankGlobals(&sWaveRegistration);
}
/* WWHD glue
data sCurrentHeap 101FC26C
data sWaveInitialization 104B6624
data sWaveRegistration 101FC260
ext 028F026C void registerWaveBankGlobals(void*)
*/
