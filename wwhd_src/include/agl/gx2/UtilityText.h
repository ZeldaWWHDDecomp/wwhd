#pragma once
#include "sead/FileDevice.h"
// Fixed-buffer sizes are unknown where their vtables are never stored.
// These neutral names identify this utility companion, not original class owners.
// These are receiver prefix views, not complete layouts of the original owning classes.
namespace agl::utility_text {
struct TextA : sead::SafeText { static void destroy(TextA*,u32); static void assureTerminationImpl(TextA*); };
struct TextB : sead::SafeText { static void destroy(TextB*,u32); static void assureTerminationImpl(TextB*); };
struct FixedTextA : sead::SafeTextBuffer { static void destroy(FixedTextA*,u32); static void assureTerminationImpl(FixedTextA*); };
struct FixedTextB : sead::SafeTextBuffer { static void destroy(FixedTextB*,u32); };
struct FixedTextC : sead::SafeTextBuffer { static void destroy(FixedTextC*,u32); };
struct FixedTextD : sead::SafeTextBuffer { static void destroy(FixedTextD*,u32); };
struct FixedTextE : sead::SafeTextBuffer { static void destroy(FixedTextE*,u32); };
struct FixedTextH : sead::SafeTextBuffer { static void destroy(FixedTextH*,u32); };
struct FixedTextF : sead::SafeTextBuffer { static void destroy(FixedTextF*,u32); };
struct FixedTextG : sead::SafeTextBuffer { static void destroy(FixedTextG*,u32); };
struct FixedTextI : sead::SafeTextBuffer { static void destroy(FixedTextI*,u32); };
struct TextC : sead::SafeText { static void destroy(TextC*,u32); static void assureTerminationImpl(TextC*); };
struct FixedTextJ : sead::SafeTextBuffer { static void destroy(FixedTextJ*,u32); static void assureTerminationImpl(FixedTextJ*); };
struct FixedTextM : sead::SafeTextBuffer { static void destroy(FixedTextM*,u32); };
struct FixedTextK : sead::SafeTextBuffer { static void destroy(FixedTextK*,u32); };
struct FixedTextL : sead::SafeTextBuffer { static void destroy(FixedTextL*,u32); };
struct FixedTextN : sead::SafeTextBuffer { static void destroy(FixedTextN*,u32); };
struct OwnedTextA : sead::SafeTextBuffer { static void destroy(OwnedTextA*,u32); };
struct OwnedTextB : sead::SafeTextBuffer { static void destroy(OwnedTextB*,u32); };
}
void utilityTextDeleteObject(void*);
void utilityTextDeleteArray(void*);
