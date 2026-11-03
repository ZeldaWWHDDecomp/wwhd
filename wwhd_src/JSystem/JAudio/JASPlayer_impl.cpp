// JAudio player pitch conversion and two-word random state, reconstructed from WWHD.
#include "JSystem/JAudio/JASPlayer_impl.h"
using namespace gabi;
struct KeyTable { be<f32> values[64]; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static KeyTable s_key_table;
static s32 sV0 = 0x1000;
static s32 sV1 = 0x5555;
static Initialization sInitialization;
static Registration sRegistration;
extern f32 playerKeyPitch(s32 key);
extern void registerPlayerGlobals(void*);
namespace JASystem { namespace Player {
/* 0284565C */
s32 extend8to16(u32 value) {
    // HD: the clear-sign-bit path preserves the complete incoming register.
    if (value & 0x80)
        return (s16)(u16)(value + 0xFF00);
    return (s32)value;
}
/* 02845674 */
f32 pitchToCent(f32 pitch, f32 range) {
    f32 scaled = fmuls_ppc(pitch, 4.0f);
    scaled = fmuls_ppc(scaled, range);
    s16 note = (s16)ftoi(scaled);
    f32 fraction = fsubs_ppc(scaled, (f32)note);
    if (scaled < 0.0f && fraction != 0.0f) {
        note = (s16)(note - 1);
        fraction = fadds_ppc(fraction, 1.0f);
    }
    // HD: the binary's not-less branch also adjusts an unordered fraction.
    if (!(fraction < 1.0f)) {
        note = (s16)(note + 1);
        fraction = fsubs_ppc(fraction, 1.0f);
    }
    u16 index = (u16)ftoi(fmuls_ppc(fraction, 64.0f));
    f32 base = playerKeyPitch((s32)note + 60);
    return fmuls_ppc(base, s_key_table.values[index]);
}
/* 02845758 */
s32 getRandomS32() {
    u32 product = (u32)sV0 * 0x13579BDE;
    s32 previous = sV1;
    u32 next = (u32)previous * 0x98765432 + (u32)((s32)product >> 4);
    sV0 = previous;
    ++next;
    sV1 = (s32)next;
    return (s32)next;
}
} }
/* 02845798 */
void __sinit_JASPlayer_impl_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerPlayerGlobals(&sRegistration);
}
/* WWHD glue
data s_key_table 1017A660
data sV0 101FC27C
data sV1 101FC280
data sInitialization 104B6644
data sRegistration 101FC284
ext 02815578 f32 playerKeyPitch(s32)
ext 028F026C void registerPlayerGlobals(void*)
*/
