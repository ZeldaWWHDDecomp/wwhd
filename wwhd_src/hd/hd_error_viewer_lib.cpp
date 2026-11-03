/* agl render-layer instance emitted in the hd_error_viewer translation unit (HDTU-LIB): the name of a
 * render step, a SafeString returned by value from a function-local table of 1 name(s) that the
 * first call copies out of the initialised data.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

static u32 sRenderStepNamesGuard;
static char* sRenderStepNames[1] = {};  // filled on first use
static const char* sRenderStepNameData[] = {"Layout"};
static sead::SafeTextVtable sSafeStringVtable;

/* 02734AD4 */
sead::SafeText* sead::instances::error_viewer::Layer::getRenderStepName(SafeText* name, s32 step) const {
    if (sRenderStepNamesGuard == 0) {
        sRenderStepNamesGuard = 1;
        memcpy_g(sRenderStepNames, sRenderStepNameData, sizeof(sRenderStepNames));
    }
    // HD: one step only; GHS reads the first entry whatever the step.
    const char* text = sRenderStepNames[0];
    if (name == NULL) {
        name = static_cast<SafeText*>(operator_new(sizeof(SafeText)));
    }
    if (name != NULL) {
        name->mText = reinterpret_cast<be<u8>*>(const_cast<char*>(text));
        name->mVtable = &sSafeStringVtable;
    }
    return name;
}

/* WWHD glue
data sRenderStepNamesGuard 101FDDE0 nocheck
data sRenderStepNames 101FDEB8 nocheck
data sRenderStepNameData 101F88A8
data sSafeStringVtable 10142274 nocheck
*/
